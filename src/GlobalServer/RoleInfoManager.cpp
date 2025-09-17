#include "RoleInfoManager.h"
#include "Log4cxxHelp.h"
#include "TblQueryResult.h"
#include "GlobalServer.h"
#include "MysqlDatabase.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "PlayerManager.h"
#include "AthenaNew.h"
#include "PacketProcessor.h"
#include "RechargeMgr.h"
#include "Role.pb.h"
#include "RankMgr.h"
#include "datacenter_log.h"
#include "log_singleton.h"
#include "Arena.h"
#include "KoreaLog.h"
#include "AdvEventReport.h"
#include "Guild.h"
#include "HttpClient.h"
#include "OpIDMgr.h"
#include "Game.pb.h"

extern GlobalServer * g_globalServer;

CTblField createRoleInfoDef[] = {
	{ "CHARID",    	    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING,	MAX_ACCNAMESIZE +1},
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1},
	{ "SERVERID",		CTblField::DB_TYPE_UINT32,	sizeof(UINT32)  },
	{ "LEVEL",			CTblField::DB_TYPE_UINT16,	sizeof(UINT16)  },
	{ "JOB",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "SEX",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "LASTLOGINTIME",  CTblField::DB_TYPE_STRING,  20  			},
	{ "CREATETIME",     CTblField::DB_TYPE_STRING,  20  			},
	{ "HEADID",     	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "FACEID",     	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "OP_ID",     	    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 },
};

CTblField g_maxCharIDTblDef [] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 },
};

CTblField g_roleBaseInfoTblDef [] = {
	{ "CHARID",    	    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1},
	{ "JOB",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "SEX",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING, 	MAX_ACCNAMESIZE + 1},
	{ "CREATETIME",		CTblField::DB_TYPE_STRING, 	20 },
	{ NULL, 0, 0 },
};

void RoleInfoManager::loadMaxCharID()
{
	CTblQueryResult * result = g_databaseEngine.select(g_maxCharIDTblDef," ROLEINFO ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		UINT32 * pCharID = (UINT32 *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			UINT32 realCharID = pCharID[i] & CHARID_ID_INFO_MASK;
			if (realCharID > m_maxCharID){
				m_maxCharID = realCharID;
			}	
		}   
	}
	if (m_maxCharID <= MAX_ROBOT_CHARID){
		m_maxCharID = MAX_ROBOT_CHARID;
	}
	Athena::logger->trace("当前最大角色ID为:%d",m_maxCharID);
	SAFE_DELETE(result);
	
	result = g_databaseEngine.select(g_roleBaseInfoTblDef," ROLEINFO ",NULL,NULL,0,NULL);

	if (result && result->getData()){
		stRoleBaseInfoDB * pData = (stRoleBaseInfoDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stRoleBaseInfo & roleBaseInfo = m_allRoleBaseInfos[pData[i].m_charID];
			roleBaseInfo.m_charID = pData[i].m_charID;
			roleBaseInfo.m_job = pData[i].m_job;
			roleBaseInfo.m_sex = pData[i].m_sex;
			roleBaseInfo.m_name = pData[i].m_name;
			
			UINT32 createTime = g_stringDate2Utc(pData[i].m_createTime);
			FirstCreateRoleCharID_IT  it = m_firstCreateRoleCharIDs.find(pData[i].m_account);

			if (it == m_firstCreateRoleCharIDs.end()){
				m_firstCreateRoleCharIDs[pData[i].m_account] = roleBaseInfo.m_charID;
			}
			else if (it->second > createTime){
				m_firstCreateRoleCharIDs[pData[i].m_account] = roleBaseInfo.m_charID;
			}
		}
	}
	SAFE_DELETE(result);
}

void RoleInfoManager::createRole(stCreateRoleInfo & createRoleInfo,stReqCreateInfo reqCreateInfo)
{
	UINT32 zoneID = g_globalServer->getZoneID();
	Athena::logger->trace("@@@Charles $$$$ zoneID is %d", zoneID);


	std::string serverIDs = Athena::global["serverID"];
	Athena::logger->trace("@@@Charles $$$$ serverIDs sss is %s", serverIDs.c_str());



	createRoleInfo.m_charID = (zoneID << 22) |(++m_maxCharID);

	Athena::CUDSqlCallBack<RoleInfoManager,stCreateRoleInfo,stReqCreateInfo> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<RoleInfoManager,stCreateRoleInfo,stReqCreateInfo>(&RoleInfoManager::getSingleton(),&RoleInfoManager::createRoleCB,false,0,createRoleInfo,reqCreateInfo);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	std::string timestamp = g_utc2StringFormat(g_globalServer->getSec());
	// std::string lasttamp = '0000-00-00 00:00:00';
	strncpy(createRoleInfo.m_lastLoginTime,timestamp.c_str(),sizeof(createRoleInfo.m_lastLoginTime));

	strncpy(createRoleInfo.m_createTime,timestamp.c_str(),sizeof(createRoleInfo.m_createTime));
	Athena::logger->trace("@@@Charles $$$$ m_lastLoginTime sss is %s", createRoleInfo.m_lastLoginTime);

	g_databaseEngine.insertRecord(createRoleInfoDef,(const char *)&createRoleInfo,"ROLEINFO",autoID,pDelaySql);
}


static void g_reportNftCreateRoleCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){ 
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("上报Nft创建角色获得的http响应信息%s",ret.c_str());
		}  
	}
	else {
		Athena::logger->trace("上报Nft创建角色失败");
	}
}

void  RoleInfoManager::createRoleCB(bool opResult,UINT64 autoID,stCreateRoleInfo createData,stReqCreateInfo reqCreateInfo)
{
	autoID = createData.m_charID;
	Global::stLoginGlobalFail failCmd;
	failCmd.set_account(createData.m_account);
	if (!opResult) {
		failCmd.set_errocode(Global::stLoginGlobalFail::CREATE_FAIL);
		g_globalServer->getConnMgr().broadcastByID(reqCreateInfo.m_gateID,failCmd);
		Athena::logger->debug("[创建角色数据失败] account=%s, charid=%u, name=%s",createData.m_account, autoID,createData.m_name);
		return ;
	}


	CRankMgr::getSingleton().afterRoleCreate(autoID,1);
	PlayerManager::getSingleton().afterCreateRole(autoID,createData.m_serverID);
	//ArenaMgr::getSingleton().afterCreateRole(createData,autoID,createData.m_zoneID);
	Global::stCreateRoleResult ret;
	ret.set_retcode(Role::CREATE_ROLE_SUCCESS);
	ret.set_account(createData.m_account);
	ret.set_charid(autoID);
	ret.set_level(1);
	ret.set_name(createData.m_name);
	ret.set_job(createData.m_job);
	ret.set_sex(createData.m_sex);
	ret.set_serverid(createData.m_serverID);
	ret.set_createtime(g_stringDate2Utc(createData.m_createTime));
	g_globalServer->getConnMgr().broadcastByID(reqCreateInfo.m_gateID,ret);
	
	stRoleBaseInfo &roleBaseInfo = m_allRoleBaseInfos[ret.charid()];
	roleBaseInfo.m_charID = ret.charid();
	roleBaseInfo.m_job = ret.job();
	roleBaseInfo.m_sex = ret.sex();
	roleBaseInfo.m_name = ret.name();
	
	FirstCreateRoleCharID_IT  it = m_firstCreateRoleCharIDs.find(ret.account());

	if (it == m_firstCreateRoleCharIDs.end()){
		m_firstCreateRoleCharIDs[ret.account()] = ret.charid();
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("account=%s发送选择界面数据成功,deviceID=%s,ip=%s",createData.m_account,reqCreateInfo.m_deviceID.c_str(),reqCreateInfo.m_ip.c_str());
#endif
	/*--------------- 打生态日志 ----------------*/
	uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
	uuzu::logsdk::UserInfo userInfo;
	userInfo.level = 1;
	userInfo.passport = createData.m_account; //必须填写
	userInfo.exp = 0;
	userInfo.viplevel = 0;
	userInfo.blackcoin = 0;
	userInfo.gamecoin = 0;
	userInfo.moneycoin = 0;
	userInfo.device = reqCreateInfo.m_deviceID;
	userInfo.clientip = reqCreateInfo.m_ip;
	userInfo.firstintime = g_globalServer->getSec();
	userInfo.version = Athena::global["gameVersion"];

	char szCharID[20] = { 0 };
	snprintf(szCharID,sizeof(szCharID),"%d",(UINT32)autoID);
	userInfo.uid = szCharID;   //除了est没创角前，其他时候必须填写
	std::string opID = g_int2string(createData.m_opID); 
	logger.setPID(opID.c_str()); 
	logger.sendDataLog(userInfo,reqCreateInfo.m_channelID.c_str(), "create", "","","","","","","est");
	/*-------------------------------------------*/
	
	/*------------------ 韩国项目日志 ------------------*/
	CKoreaLogMgr::getSingleton().createRole(createData.m_account,(UINT32)autoID,reqCreateInfo.m_ip,createData.m_opID,createData.m_name,g_globalServer->getSec(),"android");
	/*------------------ 韩国项目日志 ------------------*/

	Global::stNotifyWebCreateRole webCmd;
	webCmd.set_account(createData.m_account);
	webCmd.set_opid(createData.m_opID);
	webCmd.set_deviceid(reqCreateInfo.m_deviceID);
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

	/***** 广告上报 ****/
	// AdvEventReport::getSingleton().reportCreateRole(g_globalServer->getSec(),createData.m_opID,reqCreateInfo.m_deviceID,reqCreateInfo.m_ip,129,(UINT32)atoi(Athena::global["serverID"].c_str()),Athena::global["bundleID"],Athena::global["gameVersion"],createData.m_account,createData.m_name);	
	/***** 广告上报 ****/

	// //NFT项目上报
	// if (Athena::global["nftServer"] == "true" && Athena::global["NftUrl"] != "")
	// {
	// 	CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
	// 	char buf[1024] = { 0 };
	// 	snprintf(buf, sizeof(buf) - 1, "%s/createuser?account=%s&serverid=%s", Athena::global["NftUrl"].c_str(), createData.m_account, Athena::global["serverID"].c_str());
	// 	pRequest->setUrl(buf);
  
	// 	pRequest->setRequestType(CHttpRequest::kHttpGet);
  
	// 	pRequest->setResponseCallback(g_reportNftCreateRoleCB);
	// 	CHttpClient::getSingleton().asynSend(pRequest);
	// }
	
	return ;
}

CTblField g_selectRoleInfoDef[] = {
	{ "CHARID",			CTblField::DB_TYPE_UINT32,	sizeof(UINT32) 	 	},
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING,	MAX_ACCNAMESIZE +1	},
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1 	},
	{ "SERVERID",		CTblField::DB_TYPE_UINT32,	sizeof(UINT32)  	},
	{ "LEVEL",			CTblField::DB_TYPE_UINT16,	sizeof(UINT16)  	},
	{ "JOB",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)  		},
	{ "SEX",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)  		},
	{ "VIPLV",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)      },
	{  NULL, 			0, 							0					},
};

void RoleInfoManager::loadSelectRoleInfo(std::string account,UINT16 gateID,std::string deviceID,UINT32 opID, UINT32 serverID)
{
	SvrInfo * pSvrInfo = g_globalServer->getSvrInfoByID(gateID);
	if (!pSvrInfo){
		Athena::logger->trace("请求加载快照数据失败,网关ID不存在!");
		return ;
	}
	stReqLoadSnapInfo reqLoadInfo;
	reqLoadInfo.m_account = account;
	reqLoadInfo.m_deviceID = deviceID;
	reqLoadInfo.m_gateID = gateID;
	reqLoadInfo.m_opID = opID;

	UINT32 svrID = 0;
	if (Athena::global["isHeFu"] == "true"){
		if(0 == serverID)
		{
			Athena::logger->error("请求加载快照数据失败,serverID:%d", serverID);
			return;
		}
		svrID = serverID;
	}else{
		svrID = (UINT32)atoi(Athena::global["serverID"].c_str());
	}
                                                 
	Athena::logger->trace("请求加载快照数据,serverID:%d account=%s",svrID, account.c_str());
                                                               
	char szWhere[250] = { 0 };
	bzero(szWhere, sizeof(szWhere));
	snprintf(szWhere, sizeof(szWhere), " ACCOUNT='%s' AND SERVERID=%d ", account.c_str(),svrID);
	Athena::QueryCallback<RoleInfoManager,stReqLoadSnapInfo> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,stReqLoadSnapInfo>(this,&RoleInfoManager::loadSelectRoleInfoCB,NULL,reqLoadInfo);
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_selectRoleInfoDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);

}

//按VipLv 降序排序
bool VipLvSort(const stGateLoadRoleInfo& v1, const stGateLoadRoleInfo& v2)
{
	return v1.m_vipLv > v2.m_vipLv;
}

//VipLv相同时按Level降序排序
bool LevelSort(const stGateLoadRoleInfo& v1, const stGateLoadRoleInfo& v2)
{
	if(v1.m_vipLv == v2.m_vipLv){
		return v1.m_level > v2.m_level;
	}
	return false;
}

void RoleInfoManager::loadSelectRoleInfoCB(CTblQueryResult* result,stReqLoadSnapInfo loadSnapInfo)
{
	Athena::logger->trace("RoleInfoManager::loadSelectRoleInfoCB m_account=%s", loadSnapInfo.m_account.c_str());
	Global::stEnterSelectPage webCmd;
	webCmd.set_account(loadSnapInfo.m_account);
	webCmd.set_deviceid(loadSnapInfo.m_deviceID);
	webCmd.set_opid(loadSnapInfo.m_opID);
	Global::stRetSnapData  cmd;
	cmd.set_account(loadSnapInfo.m_account);
	if (!result){
		g_globalServer->getConnMgr().broadcastByID(loadSnapInfo.m_gateID,cmd);
		g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
		return ;	
	}	
	UINT32 recordLen = 0;
	result->setFieldDef(g_selectRoleInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);

	std::vector<stGateLoadRoleInfo> vec;
	if (result->getData()){
		//stGateRoleInfo * pSnapData = (stGateRoleInfo * )result->getData();
		stGateLoadRoleInfo * pSnapData = (stGateLoadRoleInfo * )result->getData();
		for (UINT16 i = 0; i < result->getRowCount();++i){
			/*
			Global::stSnapData * pSyn = cmd.add_data();
			if (pSyn) {
				UINT32 time = g_stringDate2Utc(pSnapData[i].m_lastLoginTime);
				time = 0;
				pSyn->set_account(loadSnapInfo.m_account);
				pSyn->set_charid(pSnapData[i].m_charID);
				pSyn->set_level(pSnapData[i].m_level);
				pSyn->set_name(pSnapData[i].m_name);
				pSyn->set_job(pSnapData[i].m_job);
				pSyn->set_sex(pSnapData[i].m_sex);
				pSyn->set_serverid(pSnapData[i].m_serverID);
			}
			*/
			vec.push_back(pSnapData[i]);
		}
	}

	std::sort(vec.begin(),vec.end(),VipLvSort);
	std::sort(vec.begin(),vec.end(),LevelSort);

	std::vector<stGateLoadRoleInfo>::iterator it = vec.begin();
	for(;it != vec.end(); ++it)
	{
		Global::stSnapData * pSyn = cmd.add_data();           
		stGateLoadRoleInfo & pSnapData = *it;
		if (pSyn) {                                                      
			pSyn->set_account(loadSnapInfo.m_account);                   
			pSyn->set_charid(pSnapData.m_charID);                     
			pSyn->set_level(pSnapData.m_level);                       
			pSyn->set_name(pSnapData.m_name);                         
			pSyn->set_job(pSnapData.m_job);                           
			pSyn->set_sex(pSnapData.m_sex);                           
			pSyn->set_serverid(pSnapData.m_serverID);                 
		}                                                                
	}

	g_globalServer->getConnMgr().broadcastByID(loadSnapInfo.m_gateID,cmd);
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

bool RoleInfoManager::deleteRole(const UINT32 charid)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u", charid);
	bool ret = g_databaseEngine.deleteRecord("ROLEINFO",where,NULL);

	if (!ret) {
		Athena::logger->debug("[删除角色失败] charid=%u", charid);
		return false;
	}
	Athena::logger->debug("[删除角色成功] charid=%u", charid);
	return true;
}


CTblField g_roleInfoDef[] = {
	{ "CHARID",			CTblField::DB_TYPE_UINT32,	sizeof(UINT32) 	 },
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING,	MAX_ACCNAMESIZE +1	},
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1 },
	{ "LEVEL",			CTblField::DB_TYPE_UINT16,	sizeof(UINT16)	},
	{ "JOB",          	CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "SEX",          	CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },

	{ "EXP",			CTblField::DB_TYPE_UINT32,  sizeof(UINT32)	},
	{ "GOLDCOIN",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "DIAMOND",      	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "GIVEDIAMOND",   	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "HEISHIJIEJING",  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "REPUTATION",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "HONOUR",      	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "BRAVEVAL",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "PEACKCERTIFICATE",CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "HUFU",      		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "YUNTIE",    		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "JUEXINGEXP",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "INTEGRAL",	    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "SP",         	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "LILIAN",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "VIPLV",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "SERVERID",		CTblField::DB_TYPE_UINT32,	sizeof(UINT32)  },
	{ "MAPID",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "POSX",        	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "POSY",        	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "POSZ",        	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "LEAVEGUILDTIME", CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "BITMASK",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "RECHARGENUM",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "GIVE_RECHARGENUM", CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "TOTALCONSUME",   CTblField::DB_TYPE_UINT64,  sizeof(UINT64)  },
	{ "BATTLEPOWER",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "STARNUM",    	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "LASTLOGINTIME",  CTblField::DB_TYPE_STRING,  20   			},
	{ "CREATETIME",     CTblField::DB_TYPE_STRING,  20              },
	{ "ADVUSER",     	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "GM",     		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "OP_ID",     		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "CONTRIBUTE",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "HEADID",     	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "FACEID",     	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "NEWBIEGUILDEID",	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "EQUIP_AVE_RAISELV",CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "EQUIP_AVE_REFINELV",CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "WING_REFINELV",    CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "FUWEN_AVE_RAISELV",CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "FUWEN_AVE_QUALITY",CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "ONE2ONE_PVP_SCORE",CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "SUBNEWBIE_GUIDEID",CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
    { "SEVENDAYCONSUMEDIAMOND",CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "LUCKDRAWDATA",    CTblField::DB_TYPE_BLOB,       60 },
	{ "ENCHANTDATA",    CTblField::DB_TYPE_BLOB,      ENCHANT_DATA_LEN },
	{ "DAILY_INVITE_INFO",    CTblField::DB_TYPE_STRING,       60 },
	{ "SOUL_FIRE",     	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "DRAGON_NEST_INFO",    CTblField::DB_TYPE_STRING,       20 },
	{ "COUPLE_ID",     	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "COUPLE_NAME",    CTblField::DB_TYPE_STRING,       33 },
	{ "GUILDBLACKMARKET",	CTblField::DB_TYPE_BLOB,  200 },
	{ "LIFEESSENCE",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "GUILDCRYSTAL",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32) }, 
	{ "STONECOPYMAPINFO",    CTblField::DB_TYPE_STRING,       20 },
	{ "LATE_WALFARE",    CTblField::DB_TYPE_STRING,       20 },
	{ "RES_BACK_INFO",    CTblField::DB_TYPE_STRING,       1000 },
	{ "MONSTER",        CTblField::DB_TYPE_BLOB,                200 },
	{ "MOUNT",       	CTblField::DB_TYPE_BLOB,                200 },
	{ "PTOTOID",        CTblField::DB_TYPE_BLOB,   				100 },
	{ "USERPHOTOID",	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "LEVEL_WFLFARE_OPEN_TIME",  CTblField::DB_TYPE_STRING,              100 },
	{ "LEVEL_WFLFARE",  CTblField::DB_TYPE_STRING,       		200 },
	{ "TRAIN_SP",  CTblField::DB_TYPE_UINT32,       		sizeof(UINT32)  },
	{ "GUAJI_START",  CTblField::DB_TYPE_UINT32,       		sizeof(UINT32)  },
	{ "GUAJI_END",  CTblField::DB_TYPE_UINT32,       		sizeof(UINT32)  },
	{ "NFTVIP",	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "NFTVIP_GIFTTIME",  CTblField::DB_TYPE_UINT32,       		sizeof(UINT32)  },
	{ "GUAJI_TIME",  CTblField::DB_TYPE_UINT32,       		sizeof(UINT32)  },
	{ "BINARYDATA",     CTblField::DB_TYPE_BLOB,      ROLE_BIN_DATA_LEN + sizeof(BinaryStamp)}, 
	{ NULL, 0, 0 },
};

void RoleInfoManager::loadRoleInfo(CQuerySqlOperation * pDelaySql,const char * szWhere)
{
	 g_databaseEngine.select(g_roleInfoDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);
}

void RoleInfoManager::loadRoleInfoCB(CTblQueryResult* result,stReqLoadInfo reqData,UINT16 gateID)
{
	if (!result){//没有数据
		return ;
	}
	UINT32 recordLen = 0;

	result->setFieldDef(g_roleInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	RoleInfoForSave roleInfoSave;
	Global::stLoginGlobalFail failCmd;
	failCmd.set_account(reqData.m_account);

	if (result->getData()) {
		RoleInfoForSave * p = (RoleInfoForSave *) result->getData();
		memcpy(&roleInfoSave,p,sizeof(RoleInfoForSave));
	}
	else {
		Athena::logger->error("%s登录加载信息失败",reqData.m_account.c_str());
		failCmd.set_errocode(Global::stLoginGlobalFail::LOAD_DATA_FAIL);
		g_globalServer->getConnMgr().broadcastByID(gateID,failCmd);
		return ;
	}

	Player *pUser = PlayerManager::getSingleton().getUserByAccount(reqData.m_account);
	
	if (pUser){
		Athena::logger->error("该帐号已经登录%s",reqData.m_account.c_str());
		failCmd.set_errocode(Global::stLoginGlobalFail::HAS_LOGIN_IN);
		g_globalServer->getConnMgr().broadcastByID(gateID,failCmd);
		return ;
	}

	pUser = ATHENA_NEW Player(reqData.m_gateSvrID,reqData.m_account,reqData.m_ip.c_str());
		
	if (roleInfoSave.m_roleInfo.m_bitmask & BITMASK_FORBID_LOGIN){
		Athena::logger->error("玩家name=%s,account=%s被禁止登录",roleInfoSave.m_roleInfo.m_name,reqData.m_account.c_str());
		failCmd.set_errocode(Global::stLoginGlobalFail::FORBID_LOGIN);
		g_globalServer->getConnMgr().broadcastByID(gateID,failCmd);
		return ;
	}
	
	pUser->m_channelID = reqData.m_channelID;	
	pUser->m_deviceID = reqData.m_deviceID;
	pUser->setID(roleInfoSave.m_roleInfo.m_charID);
	pUser->setName(roleInfoSave.m_roleInfo.m_name);
	pUser->setTempID(0);
	pUser->setGameID(reqData.m_gameSvrID);
	Global::stRetRoleInfo  cmd;
	cmd.set_ip(reqData.m_ip);
	cmd.set_eloaddatatype(Global::eLoadData_Login);
	cmd.set_account(reqData.m_account);
	cmd.set_charid(pUser->getID());
	cmd.set_ip(pUser->getIP());
	cmd.set_channelid(pUser->m_channelID);
	cmd.set_netip(reqData.m_netIP);

	byte* sour = roleInfoSave.m_binData;
	BinaryStamp* stamp = (BinaryStamp*)sour;
	cmd.set_datasize(stamp->m_totalSize + sizeof(RoleInfo));
	std::string binData;
	binData.append((char *)&roleInfoSave.m_roleInfo,sizeof(RoleInfo));
	binData.append((char *)roleInfoSave.m_binData + sizeof(BinaryStamp),stamp->m_totalSize);
	cmd.set_data(binData);
	cmd.set_gateid(gateID);
	cmd.set_deviceid(pUser->m_deviceID);
	
	pUser->m_opID = roleInfoSave.m_roleInfo.m_opID;

	if (!PlayerManager::getSingleton().addUser(pUser)){
		Athena::logger->error("[角色登录]添加角色到容器时候失败");
		SAFE_DELETE(pUser);
		failCmd.set_errocode(Global::stLoginGlobalFail::OTHER);
		g_globalServer->getConnMgr().broadcastByID(gateID,failCmd);
		return;
	}
	Athena::logger->trace("[角色登录]角色(account=%s,name=%s,charID=%d)登录中心服务器成功,deviceID=%s!",pUser->getAccount().c_str(),pUser->getName(),pUser->getID(),pUser->m_deviceID.c_str());
	g_globalServer->getConnMgr().broadcastByID(reqData.m_gameSvrID,cmd);

	return ;
}

void  RoleInfoManager::saveRoleInfo(RoleInfoForSave& roleInfoSave)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u", roleInfoSave.m_roleInfo.m_charID);
	
	Athena::CUDSqlCallBack<RoleInfoManager,RoleInfo> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<RoleInfoManager,RoleInfo>(&RoleInfoManager::getSingleton(),&RoleInfoManager::saveRoleInfoCB,false,0,roleInfoSave.m_roleInfo);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_roleInfoDef,(const char *)&roleInfoSave,"ROLEINFO",where,pDelaySql);
}

void RoleInfoManager::saveRoleInfoCB(bool opResult,UINT64 autoID,RoleInfo roleInfo)
{
	if (opResult) {
		Athena::logger->trace("保存玩家%d,%s数据成功",roleInfo.m_charID,roleInfo.m_name);
	}
	else {
		Athena::logger->trace("保存玩家%d,%s数据失败",roleInfo.m_charID,roleInfo.m_name);
	}
}

CTblField existNameTblDef[] = {
	{ "CHARID",			CTblField::DB_TYPE_UINT32,	sizeof(UINT32) 	 },
	{ NULL, 0, 0 },
};

void RoleInfoManager::existInTbl(const stCreateRoleInfo &createInfo,stReqCreateInfo reqCreateInfo)
{
#ifdef _THAILAND_VER
	char szWhere[250] = { 0 };
#else
	char szWhere[65] = { 0 };
#endif
	bzero(szWhere, sizeof(szWhere));
	snprintf(szWhere, sizeof(szWhere) - 1, " NAME='%s' ", createInfo.m_name);
	Athena::QueryCallback<RoleInfoManager,stCreateRoleInfo,stReqCreateInfo> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,stCreateRoleInfo,stReqCreateInfo>(this,&RoleInfoManager::existInTblCB,NULL,createInfo,reqCreateInfo);
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(existNameTblDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);
}

void RoleInfoManager::existInTblCB(CTblQueryResult* result,stCreateRoleInfo createInfo,stReqCreateInfo reqCreateInfo)
{
	if (result){
		Global::stCreateRoleResult ret;
		ret.set_retcode(Role::CREATE_ROLE_NAME_REPEAT);
		ret.set_account(createInfo.m_account);
		g_globalServer->getConnMgr().broadcastByID(reqCreateInfo.m_gateID,ret);
		Athena::logger->error("创建角色失败,名字已经存在");
		return ;
	}
	//createRole(createInfo,gateID);
	existAccountInTbl(createInfo,reqCreateInfo);
}

void RoleInfoManager::existAccountInTbl(const stCreateRoleInfo &createInfo,stReqCreateInfo reqCreateInfo)
{
	char szWhere[200] = { 0 };
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere, sizeof(szWhere), " ACCOUNT='%s' AND SERVERID=%d ", createInfo.m_account,createInfo.m_serverID);
	Athena::QueryCallback<RoleInfoManager,stCreateRoleInfo,stReqCreateInfo> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,stCreateRoleInfo,stReqCreateInfo>(this,&RoleInfoManager::existAccountInTblCB,NULL,createInfo,reqCreateInfo);
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(existNameTblDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);
}

void RoleInfoManager::existAccountInTblCB(CTblQueryResult* result,stCreateRoleInfo createInfo,stReqCreateInfo reqCreateInfo)
{
	if (result && result->getRowCount() >= 3){
		Global::stCreateRoleResult ret;
		ret.set_retcode(Role::CREATE_ACCOUNT_HAS_ROLE);
		ret.set_account(createInfo.m_account);
		g_globalServer->getConnMgr().broadcastByID(reqCreateInfo.m_gateID,ret);
		Athena::logger->error("账号已经存在角色!");
		return ;
	}
	createRole(createInfo,reqCreateInfo);
}

void RoleInfoManager::existJobAndSex(const stCreateRoleInfo &createInfo,UINT16 gateID)
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere, 64, " JOB=%d AND SEX=%d AND SERVERID=%d ", createInfo.m_job,createInfo.m_sex,createInfo.m_serverID);
	Athena::QueryCallback<RoleInfoManager,stCreateRoleInfo,UINT16> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,stCreateRoleInfo,UINT16>(this,&RoleInfoManager::existJobAndSexCB,NULL,createInfo,gateID);
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(existNameTblDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);
}

void RoleInfoManager::existJobAndSexCB(CTblQueryResult* result,stCreateRoleInfo createInfo,UINT16 gateID)
{
	if (result){
		Global::stCreateRoleResult ret;
		ret.set_retcode(Role::CREATE_ACCOUNT_HAS_ROLE);
		ret.set_account(createInfo.m_account);
		g_globalServer->getConnMgr().broadcastByID(gateID,ret);
		Athena::logger->error("账号已经存在角色!");
		return ;
	}
	//createRole(createInfo,gateID);
}

CTblField g_rechargeUserInfoDef[] = {
	{ "CHARID",			CTblField::DB_TYPE_UINT32,	sizeof(UINT32) 	 },
	{ NULL, 0, 0 },
};

void RoleInfoManager::findRechargeUserInfo(UINT32 accountID,UINT32 pointCard)
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere, 64, " ACCOUNTID=%d ", accountID);
	Athena::QueryCallback<RoleInfoManager,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,UINT32>(this,&RoleInfoManager::findRechargeUserInfoCB,NULL,pointCard);
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_rechargeUserInfoDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);
}

void RoleInfoManager::findRechargeUserInfoCB(CTblQueryResult* result,UINT32 pointCard)
{
	if (!result){
		return ;
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_rechargeUserInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	
	if (!result->getData()){
		return ;
	}

	//UINT32 charID = *(UINT32 *)result->getData();
	//CRechargeMgr::getSingleton().offlineRecharge(charID,pointCard);
}

void RoleInfoManager::loadOneUserDetail(UINT32 charID,UINT32 myCharID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID=%d ", charID);

	Athena::QueryCallback<RoleInfoManager,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,UINT32>(&RoleInfoManager::getSingleton(),&RoleInfoManager::loadOneUserDetailCB,NULL,myCharID);
	CQuerySqlOperation * delayCallback = ATHENA_NEW CQuerySqlOperation(pCallBack);
	RoleInfoManager::getSingleton().loadRoleInfo(delayCallback,where);
}

void RoleInfoManager::loadOneUserDetailCB(CTblQueryResult* result,UINT32 charID)
{
	if (!result){//没有数据
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (!pUser){
		return ;
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_roleInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	RoleInfoForSave roleInfoSave;

	if (result->getData()) {
		RoleInfoForSave * p = (RoleInfoForSave *) result->getData();
		memcpy(&roleInfoSave,p,sizeof(RoleInfoForSave));
	}
	else {
		Athena::logger->error("从数据库加载charID=%d的信息失败!",charID);
		return ;
	}

	Global::stRetOtherUserData2Me cmd;
	cmd.set_mycharid(charID);
	cmd.set_othercharid(roleInfoSave.m_roleInfo.m_charID);
	byte* sour = roleInfoSave.m_binData;
	BinaryStamp* stamp = (BinaryStamp*)sour;
	cmd.set_datasize(stamp->m_totalSize + sizeof(RoleInfo));
	std::string binData;
	binData.append((char *)&roleInfoSave.m_roleInfo,sizeof(RoleInfo));
	binData.append((char *)roleInfoSave.m_binData + sizeof(BinaryStamp),stamp->m_totalSize);
	cmd.set_data(binData);
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(roleInfoSave.m_roleInfo.m_charID);
	if (pGuild){
		cmd.set_guildname(pGuild->getName());    
	}   
	pUser->sendProto2Game(cmd);
}

void RoleInfoManager::loadOfflineUserChallege(UINT32 myCharID,UINT32 otherID,UINT8 pkType,UINT32 rank)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID=%d ", otherID);

	Athena::QueryCallback<RoleInfoManager,UINT32,UINT8,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,UINT32,UINT8,UINT32>(this,&RoleInfoManager::loadOfflineUserChallegeCB,NULL,myCharID,pkType,rank);
	CQuerySqlOperation * pDelay = ATHENA_NEW CQuerySqlOperation(pCallBack);
	RoleInfoManager::getSingleton().loadRoleInfo(pDelay,where);

}

void RoleInfoManager::loadOfflineUserChallegeCB(CTblQueryResult* result,UINT32 myCharID,UINT8 pkType,UINT32 rank)
{
	if (!result){//没有数据
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(myCharID);
	if (!pUser){
		return ;
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_roleInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	RoleInfoForSave roleInfoSave;

	if (result->getData()) {
		RoleInfoForSave * p = (RoleInfoForSave *) result->getData();
		memcpy(&roleInfoSave,p,sizeof(RoleInfoForSave));
	}
	else {
		Athena::logger->error("从数据库加载charID=%d的信息失败!",roleInfoSave.m_roleInfo.m_charID);
		return ;
	}

	Global::stBeginPKWithOther cmd;
	cmd.set_mycharid(myCharID);
	cmd.set_othercharid(roleInfoSave.m_roleInfo.m_charID);
	cmd.set_type((Global::stBeginPKWithOther_ePKType)pkType);
	byte* sour = roleInfoSave.m_binData;
	BinaryStamp* stamp = (BinaryStamp*)sour;
	cmd.set_datasize(stamp->m_totalSize + sizeof(RoleInfo));
	std::string binData;
	binData.append((char *)&roleInfoSave.m_roleInfo,sizeof(RoleInfo));
	binData.append((char *)roleInfoSave.m_binData + sizeof(BinaryStamp),stamp->m_totalSize);
	cmd.set_data(binData);
	cmd.set_challengerank(rank);
	pUser->sendProto2Game(cmd);
}

CTblField  g_setGMTblDef[] = {
	{ "GM",     		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ NULL, 0, 0 },
}; 

void RoleInfoManager::setUserGMValue(UINT32 charID,UINT16 gmValue)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u", charID);
	
	Athena::CUDSqlCallBack<RoleInfoManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<RoleInfoManager>(this,&RoleInfoManager::setUserGMValueCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_setGMTblDef,(const char *)&gmValue,"ROLEINFO",where,pDelaySql);
}

void RoleInfoManager::setUserGMValueCB(bool opResult,UINT64 autoID)
{
	
}

CTblField  g_updateLeaveGuildTimeTblDef[] = {
	{ "LEAVEGUILDTIME",     	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ NULL, 0, 0 },
}; 

void RoleInfoManager::updateLeaveGuildTime(UINT32 charID,UINT32 leaveTime)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u", charID);
	
	Athena::CUDSqlCallBack<RoleInfoManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<RoleInfoManager>(this,&RoleInfoManager::updateLeaveGuildTimeCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_updateLeaveGuildTimeTblDef,(const char *)&leaveTime,"ROLEINFO",where,pDelaySql);
}

void RoleInfoManager::updateLeaveGuildTimeCB(bool opResult,UINT64 autoID)
{
	
}

CTblField  g_roleBitMaskTblDef[] = {
	{ "BITMASK",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 },
}; 

void RoleInfoManager::selectRoleBitMaskForModify(UINT32 charID,bool ban)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID=%d ", charID);

	Athena::QueryCallback<RoleInfoManager,UINT32,bool> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,UINT32,bool>(this,&RoleInfoManager::selectRoleBitMaskForModifyCB,NULL,charID,ban);
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_roleBitMaskTblDef,"ROLEINFO",where,NULL,0,pDelaySql);
}

void RoleInfoManager::selectRoleBitMaskForModifyCB(CTblQueryResult* result,UINT32 charID,bool ban)
{
	if (!result){
		return ;	
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (pUser){
		return ;
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_roleBitMaskTblDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	if (result->getData()){
		UINT32 _bitMask =*(UINT32 *)result->getData();
		if (ban){
			_bitMask |= BITMASK_FORBID_LOGIN;
		}
		else {
			_bitMask &= (~BITMASK_FORBID_LOGIN);
		}
		updateBitMask(charID,_bitMask);
	}
}

void RoleInfoManager::updateBitMask(UINT32 charID,UINT32 bitMask)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u", charID);
	
	Athena::CUDSqlCallBack<RoleInfoManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<RoleInfoManager>(this,&RoleInfoManager::updateBitMaskCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_roleBitMaskTblDef,(const char *)&bitMask,"ROLEINFO",where,pDelaySql);

}

void RoleInfoManager::updateBitMaskCB(bool opResult,UINT64 autoID)
{
	
}

CTblField g_gmModifyUserInfoTblDef[] = {
	{ "NEWBIEGUILDEID",	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "GOLDCOIN",       CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "DIAMOND",        CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "GIVEDIAMOND",    CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "RECHARGENUM",    CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ NULL, 0, 0 },
};

CTblField g_gmGetUserInfoTblDef[] = {
	{ "CHARID",    	    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING,	MAX_ACCNAMESIZE +1},
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1},
	{ "GOLDCOIN",       CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "DIAMOND",        CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "GIVEDIAMOND",    CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "RECHARGENUM",    CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ NULL, 0, 0 },
};

void RoleInfoManager::gmModifyOfflineUserInfo(UINT32 charID,const stGMModifyUserInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u", charID);
    stGMModifyUserInfoDB dbData;
    dbData.m_newbieGuideID = data.m_newbieGuideID;
    CTblQueryResult * result = g_databaseEngine.select(g_gmGetUserInfoTblDef, " ROLEINFO ", where, NULL, 0, NULL);
    if (result && result->getData()) {
        stGMGetUserInfoDB * pData = (stGMGetUserInfoDB *)result->getData();
        if (!pData) {
            Athena::logger->trace("stGMGetUserInfoDB指针为空");
            return ;
        }
        std::string account = pData->m_account;
        std::string name = pData->m_name;
        dbData.m_goldCoin = pData->m_goldCoin;
        dbData.m_diamond = pData->m_diamond;
        dbData.m_giveDiamond = pData->m_giveDiamond;
        dbData.m_vipPoint = pData->m_vipPoint;
#ifdef _HDZ_DEBUG
        Athena::logger->trace("[gm离线修改前]account is %s, name is %s, gold is %d, diamond is %d, give diamond is %d, vip point is %d", account.c_str(), name.c_str(), dbData.m_goldCoin, dbData.m_diamond, dbData.m_giveDiamond, dbData.m_vipPoint);
#endif
        if (data.m_goldCoin) {
            UINT32 oldVal = dbData.m_goldCoin;
            if (data.m_goldCoin >= dbData.m_goldCoin) {
                dbData.m_goldCoin = 0;
            } else {
                dbData.m_goldCoin -= data.m_goldCoin;
            }
            UINT32 newVal = dbData.m_goldCoin;

            if (oldVal != newVal) {
                Global::stResourceChgLog webCmd;
                webCmd.set_account(account);
                webCmd.set_charid(charID);
                webCmd.set_name(name);
                webCmd.set_oldvalue(oldVal);
                webCmd.set_newvalue(newVal);
                webCmd.set_resid(eResource_GoldCoin);
                webCmd.set_reason("gm离线扣除金币");
                g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
            }
        }
        if (data.m_diamond) {
            UINT32 decGiveDiamond = 0, decDiamond = 0;
            UINT32 oldVal = dbData.m_diamond + dbData.m_giveDiamond;
            if (data.m_diamond >= oldVal) {
                decDiamond = dbData.m_diamond;
                decGiveDiamond = dbData.m_giveDiamond;
                dbData.m_diamond = 0;
                dbData.m_giveDiamond = 0;
            } else if (data.m_diamond >= dbData.m_giveDiamond) {
                decDiamond = data.m_diamond - dbData.m_giveDiamond;
                decGiveDiamond = dbData.m_giveDiamond;
                dbData.m_diamond -= decDiamond;
                dbData.m_giveDiamond = 0;
            } else {
                decGiveDiamond = data.m_diamond;
                dbData.m_giveDiamond -= decGiveDiamond;
            }
            UINT32 newVal = dbData.m_diamond + dbData.m_giveDiamond;

            if (oldVal != newVal) {
                Global::stResourceChgLog webCmd;
                webCmd.set_account(account);
                webCmd.set_charid(charID);
                webCmd.set_name(name);
                webCmd.set_oldvalue(oldVal);
                webCmd.set_newvalue(newVal);
                webCmd.set_resid(eResource_Diamond);
                webCmd.set_reason("gm离线扣除钻石");
                g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
            }
        }
        if (data.m_vipPoint >= dbData.m_vipPoint) {
            dbData.m_vipPoint = 0;
        } else {
            dbData.m_vipPoint -= data.m_vipPoint;
        }
#ifdef _HDZ_DEBUG
        Athena::logger->trace("[gm离线修改后]gold is %d, diamond is %d, give diamond is %d, vip point is %d",
                    dbData.m_goldCoin, dbData.m_diamond, dbData.m_giveDiamond, dbData.m_vipPoint);
#endif
    } else {
        Athena::logger->trace("[gm离线扣除]没有找到对应的玩家");
        return ;
    }
	
	Athena::CUDSqlCallBack<RoleInfoManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<RoleInfoManager>(this,&RoleInfoManager::gmModifyOfflineUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_gmModifyUserInfoTblDef,(const char *)&dbData,"ROLEINFO",where,pDelaySql);

}

void RoleInfoManager::gmModifyOfflineUserInfoCB(bool opResult,UINT64 autoID)
{

}
	
CTblField g_clearOne2OneScoreTblDef [] = {
	{ "ONE2ONE_PVP_SCORE",CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 },
};

void RoleInfoManager::clearOne2OneScore()
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID > 0");
	
	Athena::CUDSqlCallBack<RoleInfoManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<RoleInfoManager>(this,&RoleInfoManager::clearOne2OneScoreCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT32 score = 0;
	g_databaseEngine.updateRecord(g_clearOne2OneScoreTblDef,(const char *)&score,"ROLEINFO",where,pDelaySql);
}

void RoleInfoManager::clearOne2OneScoreCB(bool opResult,UINT64 autoID)
{
	
}

stRoleBaseInfo * RoleInfoManager::getRoleBaseInfo(UINT32 charID)
{
	AllRoleBaseInfo_IT it = m_allRoleBaseInfos.find(charID);
	if (it != m_allRoleBaseInfos.end()){
		return &it->second;
	}
	return NULL;
}

static void g_notifyLogin2GiftSvrCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("上报玩家信息成功:ret=%s!",ret.c_str());
		}
	}
	else {
		Athena::logger->error("获得的http响应失败!");
	}
}

void  RoleInfoManager::notifyLogin2GiftSvr(std::string account,UINT32 charID)
{
	// if (!COpIDMgr::getSingleton().isYYB()){
	// 	return ;
	// } 

	// CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
	// pRequest->setUrl(Athena::global["billURL"].c_str());
	// pRequest->setRequestType(CHttpRequest::kHttpPost);
	// pRequest->setTag("reportLoginSvr");
	// static char s_reqParam[200] = { 0 };
	// bzero(s_reqParam,sizeof(s_reqParam));
	// sprintf(s_reqParam,"account=%s&charID=%d&serverID=%s",account.c_str(),charID,Athena::global["serverID"].c_str());
	// pRequest->setRequestData(s_reqParam,strlen(s_reqParam)); 
	// pRequest->setResponseCallback(g_notifyLogin2GiftSvrCB);
	// CHttpClient::getSingleton().asynSend(pRequest);
}

UINT32 RoleInfoManager::getFirstCreateCharID(std::string account)
{
	FirstCreateRoleCharID_IT it = m_firstCreateRoleCharIDs.find(account);
	if (it != m_firstCreateRoleCharIDs.end()){
		return it->second;
	}
	return 0;
}

void RoleInfoManager::existNameInTbl(const std::string& newName, UINT32 charID)
{
#ifdef _THAILAND_VER
	char szWhere[250] = { 0 };
#else
	char szWhere[65] = { 0 };
#endif
	bzero(szWhere, sizeof(szWhere));
	snprintf(szWhere, sizeof(szWhere) - 1, " NAME='%s' ", newName.c_str());
	Athena::QueryCallback<RoleInfoManager, std::string, UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager, std::string, UINT32>(this,&RoleInfoManager::existNameInTblCB,NULL,newName,charID);	
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(existNameTblDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);
}

void RoleInfoManager::existNameInTblCB(CTblQueryResult * result, std::string newName, UINT32 charID)
{
	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	Global::stExistName retCmd;
	retCmd.set_charid(charID);
	retCmd.set_name(newName.c_str());
    if (!pUser) {
        Athena::logger->error("[existNameInTblCB]invalid char id %d", charID);
		return ;
    }
	if (result){
		retCmd.set_result(Global::stExistName::eName_Repetition);
		pUser->sendProto2Game(retCmd);
		Athena::logger->error("改名失败,名字已经存在");
		return ;
	}
	
	retCmd.set_result(Global::stExistName::eSuccess);
	pUser->sendProto2Game(retCmd);
}

CTblField g_selectAccInfoDef[] = {
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING,	MAX_ACCNAMESIZE +1	},
	{  NULL, 			0, 							0					},
};

bool RoleInfoManager::isNewAccount(const std::string & account) {
	char szWhere[80] = { 0 };
	bzero(szWhere, sizeof(szWhere));
	snprintf(szWhere, sizeof(szWhere), " ACCOUNT='%s' ", account.c_str());
	CTblQueryResult * result = g_databaseEngine.select(g_selectAccInfoDef,"ROLEINFO",szWhere,NULL,0,NULL);
    bool bNew = true;
    if (result && result->getData()) {
        bNew = false;
    }
	Athena::logger->trace("account is %s, bNew is %d", account.c_str(), bNew);
    return bNew;
}

CTblField changeRoleInfoDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)	 },
	{ NULL,				0, 							0 				 },
};

void RoleInfoManager::updateNameRecord(const stChangeNameDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",data.m_charID);

	Athena::CUDSqlCallBack<RoleInfoManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<RoleInfoManager>(this,&RoleInfoManager::updateNameRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(changeRoleInfoDef,(const char *)&data,"ROLEINFO",where,pDelaySql);
}

void RoleInfoManager::updateNameRecordCB(bool opResult,UINT64 autoID)
{}

void RoleInfoManager::synRecord(Player * pUser)
{
	if(!pUser){
		return;
	}	

	stChangeNameDB dataDB;
	dataDB.m_charID = pUser->getID();
	strncpy(dataDB.m_name, pUser->getName(), sizeof(dataDB.m_name));
	dataDB.m_photoID = pUser->m_photoID;
	updateNameRecord(dataDB);
}

void RoleInfoManager::loadNftOfflineUser(UINT32 charID, UINT32 connID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID=%d ", charID);

	Athena::QueryCallback<RoleInfoManager, UINT32, UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager, UINT32, UINT32>(&RoleInfoManager::getSingleton(),&RoleInfoManager::loadNftOfflineUserCB,NULL,charID, connID);
	CQuerySqlOperation * delayCallback = ATHENA_NEW CQuerySqlOperation(pCallBack);
	RoleInfoManager::getSingleton().loadRoleInfo(delayCallback,where);
}

void RoleInfoManager::loadNftOfflineUserCB(CTblQueryResult* result, UINT32 charID, UINT32 connID)
{
	if (!result){//没有数据
		Global::stGlobal2WebQueryUserNftResult  webCmd;
		webCmd.set_charid(charID);
		webCmd.set_webconnid(connID);
		webCmd.set_result(1);
		g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
		return ;
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_roleInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	RoleInfoForSave roleInfoSave;

	if (result->getData()) {
		RoleInfoForSave * p = (RoleInfoForSave *) result->getData();
		memcpy(&roleInfoSave,p,sizeof(RoleInfoForSave));
	}
	else {
		Athena::logger->error("从数据库加载charID=%d的信息失败!",charID);
		Global::stGlobal2WebQueryUserNftResult  webCmd;
		webCmd.set_charid(charID);
		webCmd.set_webconnid(connID);
		webCmd.set_result(1);
		g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
		return ;
	}

	Global::stRetNftOfflineUserData cmd;
	cmd.set_charid(charID);
	cmd.set_webconnid(connID);
	byte* sour = roleInfoSave.m_binData;
	BinaryStamp* stamp = (BinaryStamp*)sour;
	cmd.set_datasize(stamp->m_totalSize + sizeof(RoleInfo));
	std::string binData;
	binData.append((char *)&roleInfoSave.m_roleInfo,sizeof(RoleInfo));
	binData.append((char *)roleInfoSave.m_binData + sizeof(BinaryStamp),stamp->m_totalSize);
	cmd.set_data(binData);
	  
	g_globalServer->getConnMgr().broadcastByID(GAMESERVER,cmd);
}


void RoleInfoManager::loadNftOfflineUserForChange(UINT32 charID, UINT32 connID, UINT32 itemID, INT32 count)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID=%d ", charID);

	Athena::QueryCallback<RoleInfoManager, UINT32, UINT32 ,INT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager, UINT32, UINT32 ,INT32>(&RoleInfoManager::getSingleton(),&RoleInfoManager::loadNftOfflineUserForChangeCB,NULL, connID, itemID, count);
	CQuerySqlOperation * delayCallback = ATHENA_NEW CQuerySqlOperation(pCallBack);
	RoleInfoManager::getSingleton().loadRoleInfo(delayCallback,where);
}

void RoleInfoManager::loadNftOfflineUserForChangeCB(CTblQueryResult* result,  UINT32 connID, UINT32 itemID, INT32 count)
{
	if (!result){//没有数据
		Global::stGlobal2WebChangeNftItemResult  webCmd;
		webCmd.set_webconnid(connID);
		webCmd.set_result(1);
		g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
		return ;
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_roleInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	RoleInfoForSave roleInfoSave;

	if (result->getData()) {
		RoleInfoForSave * p = (RoleInfoForSave *) result->getData();
		memcpy(&roleInfoSave,p,sizeof(RoleInfoForSave));
	}
	else {
		Athena::logger->error("从数据库加载的信息失败!");
		Global::stGlobal2WebChangeNftItemResult  webCmd;
		webCmd.set_webconnid(connID);
		webCmd.set_result(1);
		g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
		return ;
	}

	Global::stRetChangeNftOfflineUserData cmd;
	cmd.set_webconnid(connID);
	cmd.set_itemid(itemID);
	cmd.set_count(count);
	byte* sour = roleInfoSave.m_binData;
	BinaryStamp* stamp = (BinaryStamp*)sour;
	cmd.set_datasize(stamp->m_totalSize + sizeof(RoleInfo));
	std::string binData;
	binData.append((char *)&roleInfoSave.m_roleInfo,sizeof(RoleInfo));
	binData.append((char *)roleInfoSave.m_binData + sizeof(BinaryStamp),stamp->m_totalSize);
	cmd.set_data(binData);
	cmd.set_charid(roleInfoSave.m_roleInfo.m_charID);
	  
	g_globalServer->getConnMgr().broadcastByID(GAMESERVER,cmd);
}