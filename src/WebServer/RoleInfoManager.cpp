#include "RoleInfoManager.h"
#include "Log4cxxHelp.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "AthenaNew.h"
#include "WebServer.h"
#include "Role.pb.h"
#include "MiscCfgMgr.h"
#include "OpIDMgr.h"
#include <ostream>

extern WebServer * g_webServer;

CTblField g_getRoleListTblDef[] = {
	{ "CHARID",			CTblField::DB_TYPE_UINT32,	sizeof(UINT32) 	 },
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING,	MAX_ACCNAMESIZE +1	},
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1 },
	{ "LEVEL",			CTblField::DB_TYPE_UINT16,	sizeof(UINT16)	},
	{ "GOLDCOIN",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "GIVEDIAMOND",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "DIAMOND",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "RECHARGENUM",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "GIVE_RECHARGENUM", CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "LASTLOGINTIME",  CTblField::DB_TYPE_STRING,  20  			},
	{ "CREATETIME",     CTblField::DB_TYPE_STRING,  20              },
	{ "GM",             CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ NULL, 0, 0 },
};



CTblField g_roleDetailInfoDef[] = {
	{ "CHARID",			CTblField::DB_TYPE_UINT32,	sizeof(UINT32) 	 },
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING,	MAX_ACCNAMESIZE +1	},
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1 },
	{ "LEVEL",			CTblField::DB_TYPE_UINT16,	sizeof(UINT16)	},
	{ "RECHARGENUM",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "GIVE_RECHARGENUM", CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "SP",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
 	{ "HONOUR",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
  	{ "REPUTATION",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
   	{ "LILIAN",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
    { "BRAVEVAL",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "PEACKCERTIFICATE",CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "NEWBIEGUILDEID",	CTblField::DB_TYPE_UINT16,	sizeof(UINT16)	},
	{ "LASTLOGINTIME",  CTblField::DB_TYPE_STRING,  20  			},
	{ "CREATETIME",     CTblField::DB_TYPE_STRING,  20              },
	{ "OP_ID",          CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "GOLDCOIN",       CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "DIAMOND",        CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ "GIVEDIAMOND",    CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },
	{ NULL, 0, 0 },
};

CTblField g_roleRechargeInfoDef[] = {
	{ "ACCOUNT",		CTblField::DB_TYPE_STRING,	MAX_ACCNAMESIZE +1	},
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1 },
	{ "RECHARGENUM",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 },
};

CTblField g_jobStaticsTblDef[] = {
	{" COUNT(*) " , CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_getRoleLevelInfoTblDef[] = {
	{ "LEVEL",			CTblField::DB_TYPE_UINT16,	sizeof(UINT16)	},
	{" COUNT(*) " , 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_getVipInfoTblDef[] = {
	{ "LEVEL",		CTblField::DB_TYPE_UINT16,	sizeof(UINT16)  },
	{ "VIPLV" , 	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ NULL, 0, 0 },
};


CTblField g_getAdvRoleInfoTblDef[] = {
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ NULL, 0, 0 },
};

CTblField g_roleFullInfoDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",        CTblField::DB_TYPE_STRING,  MAX_ACCNAMESIZE +1  },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "LEVEL",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "JOB",            CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "SEX",            CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "EXP",            CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "GOLDCOIN",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "DIAMOND",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "GIVEDIAMOND",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "HEISHIJIEJING",  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "REPUTATION",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "HONOUR",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "BRAVEVAL",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "PEACKCERTIFICATE",CTblField::DB_TYPE_UINT32, sizeof(UINT32)  },  
	{ "HUFU",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "YUNTIE",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "JUEXINGEXP",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "INTEGRAL",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "SP",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "LILIAN",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  }, 
	{ "VIPLV",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "SERVERID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "MAPID",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },  
	{ "POSX",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "POSY",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "POSZ",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "LEAVEGUILDTIME", CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "BITMASK",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "RECHARGENUM",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "GIVE_RECHARGENUM", CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "TOTALCONSUME",   CTblField::DB_TYPE_UINT64,  sizeof(UINT64)  },  
	{ "BATTLEPOWER",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "STARNUM",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },  
	{ "LASTLOGINTIME",  CTblField::DB_TYPE_STRING,  20              },  
	{ "CREATETIME",     CTblField::DB_TYPE_STRING,  20              },  
	{ "ADVUSER",        CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },  
	{ "GM",             CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },  
    { "OP_ID",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "CONTRIBUTE",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "HEADID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "FACEID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "NEWBIEGUILDEID", CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "EQUIP_AVE_RAISELV",CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "EQUIP_AVE_REFINELV",CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "WING_REFINELV",  CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "FUWEN_AVE_RAISELV",CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "FUWEN_AVE_QUALITY",  CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  },
	{ "ONE2ONE_PVP_SCORE",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "SUBNEWBIE_GUIDEID",CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
    { "SEVENDAYCONSUMEDIAMOND",CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 },
};

RoleInfoManager::RoleInfoManager()
{
	m_pAllRoleInfo = NULL;
	m_roleNum = 0;
	m_writeRoleIndex = 0;
    m_writeRoleVecIndex = 0;
	m_refresh = false;
	
	char szCreateRole[4096] = { 0 };
	snprintf(szCreateRole,sizeof(szCreateRole),"CREATE TABLE `%%s`(\
		`CHARID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`LEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`JOB`  tinyint unsigned NOT NULL DEFAULT '0',\
		`SEX`  tinyint unsigned NOT NULL DEFAULT '0',\
		`EXP`  int(10) unsigned NOT NULL DEFAULT '0',\
		`GOLDCOIN`  int(10) unsigned NOT NULL DEFAULT '0',\
		`DIAMOND`  int(10) unsigned NOT NULL DEFAULT '0',\
		`GIVEDIAMOND` int(10) unsigned NOT NULL DEFAULT '0',\
		`HEISHIJIEJING`  int(10) unsigned NOT NULL DEFAULT '0',\
		`REPUTATION`  int(10) unsigned NOT NULL DEFAULT '0',\
		`HONOUR`  int(10) unsigned NOT NULL DEFAULT '0',\
		`BRAVEVAL`  int(10) unsigned NOT NULL DEFAULT '0',\
		`PEACKCERTIFICATE`  int(10) unsigned NOT NULL DEFAULT '0',\
		`HUFU`  int(10) unsigned NOT NULL DEFAULT '0',\
		`YUNTIE`  int(10) unsigned NOT NULL DEFAULT '0',\
		`JUEXINGEXP`  int(10) unsigned NOT NULL DEFAULT '0',\
		`INTEGRAL`  int(10) unsigned NOT NULL DEFAULT '0',\
		`SP`  int(10) unsigned NOT NULL DEFAULT '0',\
		`LILIAN`  int(10) unsigned NOT NULL DEFAULT '0',\
		`VIPLV`  smallint unsigned NOT NULL DEFAULT '0',\
		`SERVERID`  int(10) unsigned NOT NULL DEFAULT '0',\
		`MAPID`  smallint unsigned NOT NULL DEFAULT '0',\
		`POSX`  int(10) unsigned NOT NULL DEFAULT '0',\
		`POSY`  int(10) unsigned NOT NULL DEFAULT '0',\
		`POSZ`  int(10) unsigned NOT NULL DEFAULT '0',\
		`LEAVEGUILDTIME`  int(10) unsigned NOT NULL DEFAULT '0',\
		`BITMASK`  int(10) unsigned NOT NULL DEFAULT '0',\
		`RECHARGENUM`  int(10) unsigned NOT NULL DEFAULT '0',\
		 `GIVE_RECHARGENUM` int(10) unsigned NOT NULL DEFAULT '0',\
		`TOTALCONSUME` bigint(20) unsigned NOT NULL DEFAULT '0',\
		`BATTLEPOWER`  int(10) unsigned NOT NULL DEFAULT '0',\
		`STARNUM`      smallint unsigned NOT NULL DEFAULT '0',\
		`LASTLOGINTIME` datetime NOT NULL,\
		`CREATETIME`    datetime NOT NULL,\
		`ADVUSER`  tinyint unsigned NOT NULL DEFAULT '0',\
		`GM`  smallint unsigned NOT NULL DEFAULT '0',\
		`OP_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`CONTRIBUTE` int(10) unsigned NOT NULL DEFAULT '0',\
		`HEADID` int(10) unsigned NOT NULL DEFAULT '0',\
		`FACEID` int(10) unsigned NOT NULL DEFAULT '0',\
		`NEWBIEGUILDEID` smallint unsigned NOT NULL DEFAULT '0',\
		`EQUIP_AVE_RAISELV` smallint unsigned NOT NULL DEFAULT '0',\
		`EQUIP_AVE_REFINELV` tinyint unsigned NOT NULL DEFAULT '0',\
		`WING_REFINELV` tinyint unsigned NOT NULL DEFAULT '0',\
		`FUWEN_AVE_RAISELV` smallint unsigned NOT NULL DEFAULT '0',\
		`FUWEN_AVE_QUALITY` tinyint unsigned NOT NULL DEFAULT '0',\
		`ONE2ONE_PVP_SCORE` int(10) unsigned NOT NULL DEFAULT '0',\
		`SUBNEWBIE_GUIDEID` smallint unsigned NOT NULL DEFAULT '0',\
		`SEVENDAYCONSUMEDIAMOND` int unsigned NOT NULL DEFAULT '0',\
		PRIMARY KEY (`CHARID`),\
		UNIQUE KEY `NAME` (`NAME`),\
		KEY `ACCOUNTID` (`ACCOUNT`)\
		) ENGINE=InnoDB AUTO_INCREMENT=15001 DEFAULT CHARSET=utf8;");
	m_createRoleStr.assign(szCreateRole,strlen(szCreateRole));
	#ifdef _HDZ_DEBUG
	Athena::logger->trace("创建角色cache的语句为:%s",m_createRoleStr.c_str());
	#endif
}

std::string RoleInfoManager::getOnePageUser(Json::Value & searchCond)
{
	UINT32 page = 0;
	UINT32 size = 0;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("执行函数%s",__PRETTY_FUNCTION__);
#endif
	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}
	
	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::ostringstream os;
	bool addCond  = false;

	if (searchCond["player_id"].isString()){
		std::string playerID = searchCond["player_id"].asString();
		if (playerID.size() > 0) {
			addCond = true;
			os << " CHARID = " << playerID << " ";
		}
	}
	
	if (searchCond["user_account"].isString()){
		std::string userAccount = searchCond["user_account"].asString();
		if (userAccount.size() > 0) {
			if (!addCond){
				addCond = true;
				os << " ACCOUNT like '%" << userAccount << "%' ";
			}
			else {
				os << " AND ACCOUNT like '%" << userAccount << "%' ";
			}
		}
	}

	if (searchCond["name"].isString()){
		std::string name = searchCond["name"].asString();
		if (name.size() > 0) {
			if (!addCond){
				addCond = true;
				os << "  NAME like '%" << name << "%'";
			}
			else {
				os << " AND NAME like '%" << name << "%'";
			}
		}
	}
	
	if (searchCond["cash"].isArray() && (searchCond["cash"].size() >= 2)){
		Json::Value cashVal = searchCond["cash"];
		if (cashVal[0u].isString() && cashVal[1u].isString()){
			if (!addCond){
				addCond = true;
				os <<" (RECHARGENUM >= " << cashVal[0u].asString()  << " AND RECHARGENUM <= " <<cashVal[1u].asString() << " ) ";
			}
			else {
				os <<" AND (RECHARGENUM >= " << cashVal[0u].asString()	<< " AND RECHARGENUM <= " <<cashVal[1u].asString() << " ) ";
			}
		}
	}
	
	if (searchCond["level"].isArray() && (searchCond["level"].size() >= 2)){
		if (searchCond["level"][0u].isString() && searchCond["level"][1u].isString() ){
			if (!addCond){
				addCond = true;
				os <<" (LEVEL >= " << searchCond["level"][0u].asString()    << " AND LEVEL <= " <<searchCond["level"][1u].asString() << " ) ";
			}
			else {
				os <<" AND (LEVEL >= " << searchCond["level"][0u].asString()	<< " AND LEVEL <= " <<searchCond["level"][1u].asString() << " ) ";
			}
		}
	}

	if (searchCond["gm"].isString()){
		std::string gm = searchCond["gm"].asString();
		if (gm != "请选择"){
			if (gm.size() > 0) {
				if (!addCond){
					addCond = true;
					os << "  GM = " << (UINT32)atoi(gm.c_str());
				}
				else {
					os << " AND  GM = " << (UINT32)atoi(gm.c_str());
				}
			}
		}
	}		
	
	if (searchCond["op_id"].isString()){
		std::string opID = searchCond["op_id"].asString();
		UINT32 iOpID = (UINT32)atoi(opID.c_str());
		if (iOpID > 0){
			if (!addCond){
				addCond = true;
				os << " OP_ID = " << opID;
			}
			else {
				os << " AND  OP_ID =  " << opID;
			}
		}
	}

	if (searchCond["created_at"].isArray()){
		Json::Value createTimeArray = searchCond["created_at"];
		std::string beginTime;
		std::string endTime;
		if (createTimeArray.size() >= 2){
			Json::Value arr1 = createTimeArray[(Json::UInt)0];
			Json::Value arr2 = createTimeArray[(Json::UInt)1];

			if (arr1.isString()){
				beginTime = arr1.asString();
			}

			if (arr2.isString()){
				endTime = arr2.asString();
			}
		}
		
		if (beginTime.size() > 0){
			if (!addCond){
				addCond = true;
				os << " CREATETIME >= '" << beginTime <<" 00:00:00'";
			}
			else {
				os << " AND CREATETIME >= '" << beginTime <<" 00:00:00'";
			}
		}

		if (endTime.size() > 0){
			if (!addCond){
				addCond = true;
				os << " CREATETIME <= '" << endTime <<" 23:59:59'";
			}
			else {
				os << " AND CREATETIME <= '" << endTime <<" 23:59:59'";
			}
		}
	}
		
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["player_id"] = "玩家ID";
	field["name"] = "玩家名字";
	field["user_account"] = "玩家账号";
	field["level"] = "角色等级";
	field["vipLevel"] = "vip等级";
	field["createTime"] = "创建时间";
	field["lastLoginTime"] = "最后登录";
	field["rechargeNum"] = "充值金额";
	field["goldCoin"] = "金币";
	field["diamond"] = "钻石";
	//field["status"] = "状态";
	field["mark"] = "标记";
	
	data["all_total"] = getRecordCount("ROLEINFO",os.str().c_str());  
	data["fields"] = field;
	CTblQueryResult* result = NULL;
	if (os.str().size() > 0){
		result = g_databaseEngine.select(g_getRoleListTblDef,"ROLEINFO",os.str().c_str(),NULL,size,NULL,(page - 1) * size);
	}
	else {
		result = g_databaseEngine.select(g_getRoleListTblDef,"ROLEINFO",NULL,NULL,size,NULL,(page - 1) * size);
	}

	if (result && result->getData()){
		Json::Value dataList;
		Json::Value userInfo;
		stRoleListInfo * pInfo = (stRoleListInfo *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount(); ++i){
			userInfo["player_id"] = pInfo[i].m_charID;
			userInfo["name"] =  pInfo[i].m_name;
			userInfo["user_account"] = pInfo[i].m_account;
			userInfo["level"] = pInfo[i].m_level;
			userInfo["vipLevel"] = CMiscCfgMgr::getSingleton().getVipLv(pInfo[i].m_rechargeNum + pInfo[i].m_giveRechargeNum);
			userInfo["createTime"] = pInfo[i].m_createTime;
			userInfo["lastLoginTime"] = pInfo[i].m_lastLoginTime;
			userInfo["rechargeNum"] = pInfo[i].m_rechargeNum;
			userInfo["goldCoin"] = pInfo[i].m_goldCoin;
			userInfo["status"] = "正常";
			if (pInfo[i].m_gm == 0){
				userInfo["mark"] = "普通";
			}
			else if (pInfo[i].m_gm == 1){
				userInfo["mark"] = "GM";	
			}
			else if (pInfo[i].m_gm == 2){
				userInfo["mark"] = "指导员";
			}
			else if (pInfo[i].m_gm == 3){
				userInfo["mark"] = "福利号";
			}
			else {
				userInfo["mark"] = "未知类型";
			}
			userInfo["diamond"] = pInfo[i].m_diamond + pInfo[i].m_giveDiamonod;
			dataList.append(userInfo);
		}
		data["list"] = dataList;
		root["data"] = data;
	}
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

std::string RoleInfoManager::getUserDetailInfo(Json::Value & searchCond)
{
	CTblQueryResult* result = NULL;
	std::ostringstream os;
	bool addCond = false;
	
	if (searchCond["player_id"].isString()){
		std::string playerID = searchCond["player_id"].asString();
		if (playerID.size() > 0) {
			addCond = true;
			os << " CHARID = " << playerID << " ";
		}
	}
	
	if (searchCond["user_account"].isString()){
		std::string userAccount = searchCond["user_account"].asString();
		if (userAccount.size() > 0) {
			if (!addCond){
				addCond = true;
				os << " ACCOUNT = '" << userAccount << "' ";
			}
			else {
				os << " AND ACCOUNT = '" << userAccount << "' ";
			}
		}
	}

	if (searchCond["name"].isString()){
		std::string name = searchCond["name"].asString();
		if (name.size() > 0) {
			if (!addCond){
				addCond = true;
				os << "  NAME = '" << name << "'";
			}
			else {
				os << " AND NAME = '" << name << "'";
			}
		}
	}

	result = g_databaseEngine.select(g_roleDetailInfoDef,"ROLEINFO",os.str().c_str(),NULL,0,NULL);
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["player_id"] = "玩家ID";
	field["name"] = "玩家名字";
	field["user_account"] = "玩家账号";
	//field["platform"] = "平台";
	field["level"] = "角色等级";
	//field["status"] = "状态";
	field["rechargeNum"] = "充值金额";
	field["vipLevel"] = "vip等级";
	//field["vipExp"] = "vip经验";
	field["sp"] = "活力值";
	field["honour"] = "荣誉值";
	field["reputation"] = "声望";
	field["lilian"] = "历练值";
	field["braveVal"] = "勇敢凭证";
	field["peakVal"] = "巅峰值";
	field["createTime"] = "创建时间";
	field["lastLoginTime"] = "最后登录";
	field["opID"] = "渠道";

	data["fields"] = field;

	if (!result || !result->getData()){
		root["status"] = 2;
		root["data"] = data;
		std::string json_file = writer.write(root);
		return json_file;
	}



	Json::Value userInfo;
	stRoleDetailInfo * pInfo = (stRoleDetailInfo *)result->getData();
	userInfo["player_id"] = pInfo->m_charID;
	userInfo["name"] = pInfo->m_name;
	userInfo["user_account"] = pInfo->m_account;
	//field["platform"] = "平台";
	userInfo["level"] = pInfo->m_level;
	//field["status"] = "状态";
	userInfo["rechargeNum"] = pInfo->m_rechargeNum;
	userInfo["vipLevel"] = CMiscCfgMgr::getSingleton().getVipLv(pInfo->m_rechargeNum + pInfo->m_giveRechargeNum);;
	//field["vipExp"] = "vip经验";
	userInfo["sp"] = pInfo->m_sp;
	userInfo["honour"] = pInfo->m_honour;
	userInfo["reputation"] = pInfo->m_reputaion;
	userInfo["lilian"] = pInfo->m_lilian;
	userInfo["braveVal"] = pInfo->m_braveVal;
	userInfo["peakVal"] = pInfo->m_peakCertificate;
	userInfo["createTime"] = pInfo->m_createTime;
	userInfo["lastLoginTime"] = pInfo->m_lastLoginTime;
	userInfo["opID"] = COpIDMgr::getSingleton().getOpIDName(pInfo->m_opID);

	data["content"] = userInfo;
	root["data"] = data;

	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

std::string RoleInfoManager::getEditUserInfo(Json::Value & searchCond)
{
	CTblQueryResult* result = NULL;
	std::ostringstream os;
	bool addCond = false;
	
	if (searchCond["player_id"].isString()){
		std::string playerID = searchCond["player_id"].asString();
		if (playerID.size() > 0) {
			addCond = true;
			os << " CHARID = " << playerID << " ";
		}
	}
	
	if (searchCond["user_account"].isString()){
		std::string userAccount = searchCond["user_account"].asString();
		if (userAccount.size() > 0) {
			if (!addCond){
				addCond = true;
				os << " ACCOUNT = '" << userAccount << "' ";
			}
			else {
				os << " AND ACCOUNT = '" << userAccount << "' ";
			}
		}
	}

	if (searchCond["name"].isString()){
		std::string name = searchCond["name"].asString();
		if (name.size() > 0) {
			if (!addCond){
				addCond = true;
				os << "  NAME = '" << name << "'";
			}
			else {
				os << " AND NAME = '" << name << "'";
			}
		}
	}

	result = g_databaseEngine.select(g_roleDetailInfoDef,"ROLEINFO",os.str().c_str(),NULL,0,NULL);

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["player_id"] = "玩家ID";
	field["name"] = "玩家名字";
	field["user_account"] = "玩家账号";
	field["user_guideID"] = "新手ID";
	field["coin"] = "金币";
	field["diamond"] = "钻石";
	field["vip"] = "vip点数";

	data["fields"] = field;
	
	if (!result || !result->getData()){
		root["status"] = 2;
		root["data"] = data;
		std::string json_file = writer.write(root);
		return json_file;
	}
	
	stRoleDetailInfo * pInfo = (stRoleDetailInfo *)result->getData();
	Json::Value userInfo;
	userInfo["player_id"] = pInfo->m_charID;
	userInfo["name"] = pInfo->m_name;
	userInfo["user_account"] = pInfo->m_account;
	//userInfo["level"] = pInfo->m_level;
	userInfo["user_guideID"] = pInfo->m_newbieGuideID; 
	userInfo["coin"] = pInfo->m_GoldCoin;
	userInfo["diamond"] = pInfo->m_Diamond + pInfo->m_giveDiamond;
	userInfo["vip"] = pInfo->m_rechargeNum;

	data["content"] = userInfo;

	root["data"] = data;
	std::string content = writer.write(root);
	SAFE_DELETE(result);
	return content;
}

std::string RoleInfoManager::rechargeRank(Json::Value & searchCond)
{
	CTblQueryResult* result = NULL;
	std::ostringstream os;
	UINT32 page = 0;
	UINT32 size = 0;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("执行函数%s",__PRETTY_FUNCTION__);
#endif
	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}
	
	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif


	os << " RECHARGENUM > 0 ";
	
	char szOderBy[65] = { 0 };
	snprintf(szOderBy,64, " RECHARGENUM DESC ");

	result = g_databaseEngine.select(g_roleRechargeInfoDef,"ROLEINFO",os.str().c_str(),szOderBy,size,NULL,(page - 1) * size);

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	Json::Value fields;
	Json::Value dataList;

	fields["name"] = "玩家名字";
	fields["account"] = "玩家账号";
	fields["num"] = "充值数量";
	data["all_total"] = getRecordCount("ROLEINFO",os.str().c_str());

	if (result && result->getData()){
		//data["all_total"] = (UINT32)result->getRowCount();
		stRoleRechargeInfo * pInfo = (stRoleRechargeInfo *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			Json::Value info;
			info["name"] = pInfo[i].m_name;
			info["account"] = pInfo[i].m_account;
			info["num"] = pInfo[i].m_rechargeNum;
			dataList.append(info);
		}
	}
	data["fields"] = fields;
	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

std::string RoleInfoManager::getJobStatics(Json::Value & searchCond)
{
	CTblQueryResult* result = NULL;
	
	char szSql[250] = { 0 };
	snprintf(szSql,sizeof(szSql),"select count(*) from ROLEINFO where JOB = 1");
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	data["total"] = 3;
	Json::Value jobList;
	Json::Value jobInfo;
	
	if (result){
		result->setFieldDef(g_jobStaticsTblDef);
		result->fillData(4);
		jobInfo["num"] = *(UINT32 *)result->getData();
		jobInfo["name"] = "战士";
		jobList.append(jobInfo);
	}
	
	SAFE_DELETE(result);
	
	bzero(szSql,sizeof(szSql));
	snprintf(szSql,sizeof(szSql),"select count(*) from ROLEINFO where JOB = 3");
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	
	if (result){
		result->setFieldDef(g_jobStaticsTblDef);
		result->fillData(4);
		jobInfo["num"] = *(UINT32 *)result->getData();
		jobInfo["name"] = "法师";
		jobList.append(jobInfo);
	}
	
	SAFE_DELETE(result);
	
	bzero(szSql,sizeof(szSql));
	snprintf(szSql,sizeof(szSql),"select count(*) from ROLEINFO where JOB = 2");
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_jobStaticsTblDef);
		result->fillData(4);
		jobInfo["num"] = *(UINT32 *)result->getData();
		jobInfo["name"] = "弓箭手";
		jobList.append(jobInfo);
	}

	data["list"] = jobList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

std::string RoleInfoManager::getLevelInfo(Json::Value & searchCond)
{
	CTblQueryResult* result = NULL;
	
	std::map<UINT16,UINT32> lv2Num;

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	Json::Value lvList;
	Json::Value lvInfo;
	
	char szSql[250] = { 0 };
	snprintf(szSql,sizeof(szSql)," SELECT LEVEL,COUNT(LEVEL)  FROM ROLEINFO GROUP BY LEVEL ");
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_getRoleLevelInfoTblDef);
		result->fillData(4 + 2);
		stLevelStatics * pData = (stLevelStatics *)result->getData();	
		for (UINT32 i = 0;i < result->getRowCount();++i){
			lvInfo["level"] = pData[i].m_level;
			lvInfo["num"] = pData[i].m_num;
			lvList.append(lvInfo);	
		}
	}
	
	data["total"] = (UINT32)lv2Num.size();
	data["list"] = lvList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

struct stLevelInfoStatic
{
	UINT16 m_lv;				//玩家等级
	UINT16 m_num;				//玩家数量
	UINT16 m_vip0Num;			//vip0玩家数量
	UINT16 m_vip1_8Num;			//vip1到8人数
	UINT16 m_vip9_12Num;		//vip9到12人数
	UINT16 m_vip13_14Num;		//vip13到14人数
	UINT16 m_vip15;				//vip15人数

	stLevelInfoStatic()
	{
		m_lv = 0;
		m_num = 0;
		m_vip0Num = 0;
		m_vip1_8Num = 0;
		m_vip9_12Num = 0;
		m_vip13_14Num = 0;
		m_vip15 = 0;
	}
};

std::string RoleInfoManager::getLevelInfo2(Json::Value & searchCond)
{
	CTblQueryResult* result = NULL;
	
	std::ostringstream os;
	bool addCond  = false;

	if (searchCond["start_time"].isString()){
		std::string start = searchCond["start_time"].asString();
		if (start.size() > 0){
			addCond = true;
			os << " CREATETIME >= '" << start << "' ";
		}
		else {
			os << " AND CREATETIME >= '" << start << "' ";
		}
	}
	
	if (searchCond["end_time"].isString()){
		std::string end = searchCond["end_time"].asString();
		if (end.size() > 0){
			if (!addCond){
				addCond = true;
				os << " CREATETIME <= '" << end << "' ";
			}
			else {
				os << " AND CREATETIME <= '" << end << "' ";
			}
		}
	}

	std::map<UINT16,stLevelInfoStatic> lvStatics;
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value fields;
	Json::Value data;
	Json::Value lvList;
	Json::Value lvInfo;
	UINT32 totalNum = 0;

	fields["lv"] = "等级";
	fields["num"] = "人数";
	fields["rate"] = "比例";
	fields["vip0"] = "vip0人数";
	fields["vip1_8"] = "vip1到8人数";
	fields["vip9_12"] = "vip9到12人数";
	fields["vip13_14"] = "vip13到14人数";
	fields["vip15"] = "vip15人数";

	char szSql[250] = { 0 };
	if (os.str().size() > 0){
		 snprintf(szSql,sizeof(szSql)," SELECT LEVEL,COUNT(LEVEL)  FROM ROLEINFO  WHERE %s GROUP BY LEVEL",os.str().c_str());
	}
	else {
		snprintf(szSql,sizeof(szSql)," SELECT LEVEL,COUNT(LEVEL)  FROM ROLEINFO GROUP BY LEVEL");
	}

	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_getRoleLevelInfoTblDef);
		result->fillData(4 + 2);
		stLevelStatics * pData = (stLevelStatics *)result->getData();	
		for (UINT32 i = 0;i < result->getRowCount();++i){
			totalNum += pData[i].m_num;
			stLevelInfoStatic & info = lvStatics[pData[i].m_level];
			info.m_lv = pData[i].m_level;
			info.m_num = pData[i].m_num;
		}
	}
	SAFE_DELETE(result);
	
	bzero(szSql,sizeof(szSql));
	if (os.str().size() > 0){
		snprintf(szSql,sizeof(szSql)," SELECT LEVEL,COUNT(LEVEL)  FROM ROLEINFO WHERE %s AND VIPLV=0 GROUP BY LEVEL ",os.str().c_str());
	}
	else {
		snprintf(szSql,sizeof(szSql)," SELECT LEVEL,COUNT(LEVEL)  FROM ROLEINFO WHERE VIPLV=0 GROUP BY LEVEL ");
	}
	
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_getRoleLevelInfoTblDef);
		result->fillData(4 + 2);
		stLevelStatics * pData = (stLevelStatics *)result->getData();	
		for (UINT32 i = 0;i < result->getRowCount();++i){
			stLevelInfoStatic & info = lvStatics[pData[i].m_level];
			info.m_vip0Num = pData[i].m_level;
		}
	}
	SAFE_DELETE(result);

	bzero(szSql,sizeof(szSql));
	if (os.str().size() > 0){
		snprintf(szSql,sizeof(szSql)," SELECT LEVEL,VIPLV FROM ROLEINFO  WHERE %s  AND VIPLV > 0 ",os.str().c_str());
	}
	else {
		snprintf(szSql,sizeof(szSql)," SELECT LEVEL,VIPLV FROM ROLEINFO  WHERE VIPLV > 0");
	}

	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_getVipInfoTblDef);
		result->fillData(2 + 2);

		stVipLvStatics * pData = (stVipLvStatics *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			stLevelInfoStatic & info = lvStatics[pData[i].m_level];
			if ((pData[i].m_vipLv >= 1) && (pData[i].m_vipLv <= 8)){
				info.m_vip1_8Num += 1;
			}
			else if ((pData[i].m_vipLv >= 9) && (pData[i].m_vipLv <= 12)){
				info.m_vip9_12Num += 1;
			}
			else if ((pData[i].m_vipLv >= 13) && (pData[i].m_vipLv <= 14)){
				info.m_vip13_14Num += 1;
			}
			else {
				info.m_vip15 += 1;
			}
		}
	}
	SAFE_DELETE(result);

	std::map<UINT16,stLevelInfoStatic>::iterator it = lvStatics.begin();
	for (;it != lvStatics.end();++it){
		stLevelInfoStatic & lvInfoStatic = it->second;
		lvInfo["lv"] = lvInfoStatic.m_lv;
		lvInfo["num"] = lvInfoStatic.m_num;
		if (totalNum > 0){
			lvInfo["rate"] =  g_float2string((lvInfoStatic.m_num * 1.0f) / totalNum);
		}
		else {
			lvInfo["rate"] = "0.00%";
		}

		lvInfo["vip0"] = lvInfoStatic.m_vip0Num;
		lvInfo["vip1_8"] = lvInfoStatic.m_vip1_8Num;
		lvInfo["vip9_12"] = lvInfoStatic.m_vip9_12Num;
		lvInfo["vip13_14"] = lvInfoStatic.m_vip13_14Num;
		lvInfo["vip15"] = lvInfoStatic.m_vip15;
		lvList.append(lvInfo);
	}

	data["list"] = lvList;
	data["fields"] = fields;
	root["data"] = data;
	std::string json_file = writer.write(root);
	return json_file;
}

std::string RoleInfoManager::getAdvUsers(Json::Value & searchCond)
{
	CTblQueryResult* result = NULL;

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	Json::Value fields;
	Json::Value dataList;

	fields["name"] = "玩家名字";
	
	char szSql[250] = { 0 };
	bzero(szSql,sizeof(szSql));
	snprintf(szSql,sizeof(szSql),"select count(*) from ROLEINFO where ADVUSER = 1 ");
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_jobStaticsTblDef);
		result->fillData(4);
		data["all_total"] = *(UINT32 *)result->getData();
	}
	SAFE_DELETE(result);

	Json::Value info;
	
	result = g_databaseEngine.select(g_getAdvRoleInfoTblDef,"ROLEINFO","  ADVUSER = 1 ",NULL,5 ,NULL);

	if (result && result->getData()){
		stAdvRoleInfo * pAdvRoleInfo = (stAdvRoleInfo *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			info["name"] = pAdvRoleInfo[i].m_name;
			dataList.append(info);
		}
	}
	SAFE_DELETE(result);

	data["fields"] = fields;
	data["list"] = dataList;

	root["data"] = data;
	std::string json_file = writer.write(root);
	return json_file;
}

UINT32 RoleInfoManager::getRecordCount(const char * szTableName,const char * szWhere)
{
	UINT32 ret = 0;
	if (!szTableName || !szWhere){
		return ret;
	}

	CTblQueryResult* result = NULL;
	char szSql[250] = { 0 };
	bzero(szSql,sizeof(szSql));
	if (strlen(szWhere) > 0){
		snprintf(szSql,sizeof(szSql),"select count(*) from %s where %s ",szTableName,szWhere);
	}
	else {
		snprintf(szSql,sizeof(szSql),"select count(*) from %s  ",szTableName);
	}

	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_jobStaticsTblDef);
		result->fillData(4);
		ret = *(UINT32 *)result->getData();
	}
	SAFE_DELETE(result);
	return ret;
}

void  RoleInfoManager::getAllRoleFullInfo()
{
	m_roleNum = 0;
	m_writeRoleIndex = 0;
    m_writeRoleVecIndex = 0;
	std::string today = g_utc2StringDate(g_webServer->getSec(),"%Y%m%d");
	m_curRoleInfoTblName = "ROLEINFO_CACHE";
	m_curRoleInfoTblName += "_";
	m_curRoleInfoTblName += today;
	char szCreateRoleInfoTbl[4096] = { 0 };
	snprintf(szCreateRoleInfoTbl,sizeof(szCreateRoleInfoTbl),m_createRoleStr.c_str(),m_curRoleInfoTblName.c_str());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("创建角色sql语句:%s",szCreateRoleInfoTbl);
#endif
	g_databaseEngine.directExecute(szCreateRoleInfoTbl,strlen(szCreateRoleInfoTbl),szCreateRoleInfoTbl);
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_roleFullInfoDef,"ROLEINFO",NULL,NULL,0,NULL);
	if (result && result->getData()){
		m_roleNum = result->getRowCount();
		if (m_roleNum > 0){
            /*
			m_pAllRoleInfo = ATHENA_NEW RoleInfo[m_roleNum];
            if (m_pAllRoleInfo) {
                bzero(m_pAllRoleInfo,sizeof(m_pAllRoleInfo));
                bcopy(result->getData(),m_pAllRoleInfo,sizeof(RoleInfo) * m_roleNum);
            } else {
                Athena::logger->error("ATHENA_NEW error, m_roleNum is %d, sizeof RoleInfo is %d", m_roleNum, sizeof(RoleInfo));
            }
            */
            m_allRoleVec.clear();
            UINT32 count = m_roleNum / MAX_WRITE_COUNT;
            UINT32 lastPartSize = m_roleNum % MAX_WRITE_COUNT;
            UINT32 blockSize = sizeof(RoleInfoBase) * MAX_WRITE_COUNT;
            RoleInfoBase * pRole = NULL;
            RoleInfoBase * pBase = (RoleInfoBase *)result->getData();

            for (UINT32 i = 0; i < count; i++) {
                pRole = ATHENA_NEW RoleInfoBase[MAX_WRITE_COUNT];
                if (pRole) {
                    memset(pRole, 0, blockSize);
                    memcpy(pRole, pBase, blockSize);
                    m_allRoleVec.push_back(pRole);
                    pBase += MAX_WRITE_COUNT;
                } else {
                    Athena::logger->error("ATHENA_NEW error, cur count is %d", i+1);
                }
            }
            if (lastPartSize) {
                pRole = ATHENA_NEW RoleInfoBase[lastPartSize];
                if (pRole) {
                    memset(pRole, 0, lastPartSize * sizeof(RoleInfoBase));
                    memcpy(pRole, pBase, lastPartSize * sizeof(RoleInfoBase));
                    m_allRoleVec.push_back(pRole);
                } else {
                    Athena::logger->error("ATHENA_NEW error, lastPartSize is %d", lastPartSize);
                }
            }
            Athena::logger->trace("m_allRoleVec size is %d", m_allRoleVec.size());
		}
	}
	SAFE_DELETE(result);
	Athena::logger->trace("开始往角色表里写:%d调记录!",m_roleNum);
}

void RoleInfoManager::timer(UINT32 cur)
{
	struct tm curTm;
	RealTime::getLocalTime(curTm,cur);
	if ((curTm.tm_hour == 0)   
			&&(curTm.tm_min == 0)
			&&(curTm.tm_sec < 5) && (!m_refresh)){
		getAllRoleFullInfo();
		m_refresh = true;
	}
	else if (curTm.tm_sec > 5) {
		m_refresh = false;
	}
	/*
	UINT16 writeCount = 0;
	while ((m_writeRoleIndex < m_roleNum) && (writeCount < 30) && m_pAllRoleInfo){
		++writeCount;
		UINT64 autoID = 0;
		g_databaseEngine.insertRecord(g_roleFullInfoDef,(const char*)&m_pAllRoleInfo[m_writeRoleIndex],m_curRoleInfoTblName.c_str(),autoID);
		++m_writeRoleIndex;
	}

	if (m_writeRoleIndex >= m_roleNum){
		SAFE_DELETE(m_pAllRoleInfo);
	}
    */
    if (m_writeRoleVecIndex < m_allRoleVec.size()) {
        UINT32 writeCount = 0, maxCount = MAX_WRITE_COUNT;
        if (m_writeRoleVecIndex + 1 == m_allRoleVec.size()) {
            maxCount = m_roleNum % MAX_WRITE_COUNT;
            if (!maxCount) {
                maxCount = MAX_WRITE_COUNT;
            }
        }
        Athena::logger->trace("maxCount is %d, m_writeRoleVecIndex is %d", maxCount, m_writeRoleVecIndex);
        while (writeCount < maxCount) {
            UINT64 autoID = 0;
            g_databaseEngine.insertRecord(g_roleFullInfoDef,
                (const char*)&m_allRoleVec[m_writeRoleVecIndex][writeCount], m_curRoleInfoTblName.c_str(), autoID);
            ++writeCount;
        }
        m_writeRoleVecIndex++;
        Athena::logger->trace("now m_writeRoleVecIndex is %d", m_writeRoleVecIndex);
    }
    if (m_writeRoleVecIndex >= m_allRoleVec.size()) {
        for (size_t i = 0; i < m_allRoleVec.size(); i++) {
            SAFE_DELETE(m_allRoleVec[i]);
        }
        if (m_allRoleVec.size()) {
            m_allRoleVec.clear();
            Athena::logger->trace("clear vec");
        }
    }
}

bool RoleInfoManager::addOrderId(UINT32 charId, const stOrderInfo & orderInfo) {
    std::map<UINT32, std::map<std::string, stOrderInfo> >::iterator it =
            m_id2OrderItemMap.find(charId);
    if (it != m_id2OrderItemMap.end()) {
        return it->second.insert(std::make_pair(orderInfo.order, orderInfo)).second;
    }
    std::map<std::string, stOrderInfo> itemMap;
    itemMap.insert(std::make_pair(orderInfo.order, orderInfo));
    m_id2OrderItemMap.insert(std::make_pair(charId, itemMap));
    return true;
}

void RoleInfoManager::getOrderInfo(UINT32 charId, const std::string & orderId, stOrderInfo & orderInfo) {
    std::map<UINT32, std::map<std::string, stOrderInfo> >::iterator it =
            m_id2OrderItemMap.find(charId);
    if (it != m_id2OrderItemMap.end()) {
        std::map<std::string, stOrderInfo>::iterator subIt = it->second.find(orderId);
        if (subIt != it->second.end()) {
            orderInfo = subIt->second;
            return ;
        }
    }
}

CTblField g_getRoleTblDef[] = {
	{ "CHARID",			CTblField::DB_TYPE_UINT32,	sizeof(UINT32) 	 },
	{ "NAME",			CTblField::DB_TYPE_STRING, 	MAX_NAMESIZE + 1 },
	{ NULL, 0, 0 },
};

void RoleInfoManager::getRole(std::string & account, std::string & name, UINT32 & charId) {
    std::stringstream ss;
    ss << " ACCOUNT = '" << account << "'";
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_getRoleTblDef,"ROLEINFO",ss.str().c_str(),NULL,0,NULL);
	if (result && result->getData()){
        stRoleAccInfo * pInfo = (stRoleAccInfo *)result->getData();
        charId = pInfo->m_charID;
        name = pInfo->m_name;
    }
	SAFE_DELETE(result);
}
