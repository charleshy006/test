#include "GiftBag.h"
#include "HttpClient.h"
#include "json/reader.h"
#include "KVPair.h"
#include "md5.h"
#include "Log4cxxHelp.h"
#include "PlayerManager.h"
#include "Player.h"
#include "GlobalServer.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "Role.pb.h"

extern GlobalServer  * g_globalServer;

CTblField g_giftBagInfoTblDef[] = { 
	{ "GIFT_BAG_ID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "GIFT_CONTENT",        CTblField::DB_TYPE_STRING,  250			  }, 
	{ NULL, 0, 0 },
};

void stGiftBagInfo::initFromDB(const stGiftBagCfgDB & data)
{
	m_id = data.m_id;
	
	std::vector<std::string> strVec;
	g_stringtok(strVec,data.m_config,";");
	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string> subStrVec;
		g_stringtok(subStrVec,strVec[i],":");
		if (subStrVec.size() >= 2){
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			m_items[itemID] = num;	
		}
	}
}

void CGiftBagMgr::load()
{
	CTblQueryResult* result = g_databaseEngine.select(g_giftBagInfoTblDef,"GIFT_BAG",NULL,NULL,0,NULL);
	if (result){
		if (result->getData()){
			stGiftBagCfgDB * pDataDB = (stGiftBagCfgDB *)result->getData();		
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stGiftBagInfo info;
				info.initFromDB(pDataDB[i]);
				m_giftBags[info.m_id] = info;
			}
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了礼包配置");
	{
		std::map<UINT32,stGiftBagInfo>::iterator it = m_giftBags.begin();
		for (;it != m_giftBags.end();++it){
			stGiftBagInfo & info = it->second;
			Athena::logger->trace("id:%d,size:%d",info.m_id,info.m_items.size());
		}
	}
#endif
}

void CGiftBagMgr::addGiftBagCfg(UINT32 id,const std::string & content)
{
	stGiftBagCfgDB data;
	data.m_id = id;
	strncpy(data.m_config,content.c_str(),sizeof(data.m_config));

	if (getGiftBagInfo(id)){
		char where[65] = { 0 };
		bzero(where, 65);
		snprintf(where, 64, "GIFT_BAG_ID=%u",id);

		Athena::CUDSqlCallBack<CGiftBagMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGiftBagMgr>(this,&CGiftBagMgr::updateGiftBagCfgCB,false,0);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		g_databaseEngine.updateRecord(g_giftBagInfoTblDef,(const char *)&data,"GIFT_BAG",where,pDelaySql);
	}
	else {
		Athena::CUDSqlCallBack<CGiftBagMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGiftBagMgr>(this,&CGiftBagMgr::addGiftBagCfgCB,false,0);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		UINT64 autoID = 0;
		g_databaseEngine.insertRecord(g_giftBagInfoTblDef,(const char *)&data,"GIFT_BAG",autoID,pDelaySql);
	}

	stGiftBagInfo info;
	info.initFromDB(data);
	m_giftBags[id] = info;
}

void CGiftBagMgr::addGiftBagCfgCB(bool opResult,UINT64 autoID)
{

}

void CGiftBagMgr::updateGiftBagCfgCB(bool opResult,UINT64 autoID)
{
	
}

void CGiftBagMgr::removeGiftBagCfg(UINT32 id)
{
	m_giftBags.erase(id);
	char where[65] = { 0 };
	bzero(where, 65);   
	snprintf(where, 64, " GIFT_BAG_ID=%d ", id);
	Athena::CUDSqlCallBack<CGiftBagMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGiftBagMgr>(this,&CGiftBagMgr::removeGiftBagCfgCB,false,0);    
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GIFT_BAG",where,pDelaySql);
}

void CGiftBagMgr::removeGiftBagCfgCB(bool opResult,UINT64 autoID)
{
	
}

struct stReqGiftBagInfo
{
	UINT32 m_charID;
	std::string m_code;

	stReqGiftBagInfo()
	{   
		m_charID = 0;
	}   
};

static void g_reqGiftBagCB(CHttpClient* client, CHttpResponse* response)
{
	stReqGiftBagInfo * pData = NULL; 
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (response->getHttpRequest()&& response->getHttpRequest()->getUserData()){
			pData = (stReqGiftBagInfo *)response->getHttpRequest()->getUserData();
			response->getHttpRequest()->setUserData(NULL);
		}
		
		Player * pUser = PlayerManager::getSingleton().getUserByID(pData->m_charID);
		
		if (!pUser){
			SAFE_DELETE(pData);
			return ;
		}

		if (retVec.size() > 0){
			std::string ret;
			ret.assign(&retVec[0],retVec.size());

			Json::Reader reader;
			Json::Value  root;
			if (!reader.parse(ret,root)){
				SAFE_DELETE(pData);
				Athena::logger->trace("领取礼包返回解析json错误!");
				return ;
			}

			UINT16 status = (UINT16)-1;
			if (root["status"].isInt()){
				status = root["status"].asInt();
			}
			
			Role::stGotGiftBagResult retCmd;
			if (0 == status){//成功
				if (root["data"].isObject()){
					Json::Value data = root["data"];
					std::string  rewardItemStr;
					if (data["prop_id"].isString()){
						rewardItemStr = data["prop_id"].asString();
						std::vector<std::string> strVec;
						g_stringtok(strVec,rewardItemStr,";");
						std::vector<std::string> subStrVec;
						
						Global::stGiveGiftBagReward cmd;
						cmd.set_charid(pUser->getID());
						for (UINT16 i = 0;i < strVec.size();++i){
							subStrVec.clear();
							g_stringtok(subStrVec,strVec[i],",");
							
							if (subStrVec.size() >= 2){
								Global::stGiftBagReward * pSyn = cmd.add_rewards();
								if (pSyn){
									pSyn->set_itemid((UINT32)atoi(subStrVec[0].c_str()));
									pSyn->set_num((UINT32)atoi(subStrVec[1].c_str()));
								}	
							}
						}
						pUser->sendProto2Game(cmd);
						retCmd.set_result(Role::stGotGiftBagResult::eSuccess);
						pUser->sendProtoToMe(retCmd);
						Athena::logger->trace("name:%s,id:%d,code:%s 领取礼包成功!",pUser->getName(),pUser->getID(),pData->m_code.c_str());
					}
					else {
						Athena::logger->error("name:%s,id:%d,code:%s,领取礼包成功,但是没有找到礼包配置",pUser->getName(),pUser->getID(),pData->m_code.c_str());
					}
				}
			}
			else if (1 == status){//校验码错误
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Error_Other);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:校验码错误",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (2 == status){//时间过期
				retCmd.set_result(Role::stGotGiftBagResult::eTime_Expire);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:时间过期",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (3 == status){//缺少参数
				retCmd.set_result(Role::stGotGiftBagResult::eInnerError);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:缺少参数",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (100 == status){//o_id,game_id,server_id 参数传递错误
				retCmd.set_result(Role::stGotGiftBagResult::eInnerError);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:o_id,game_id,server_id 参数传递错误",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (104 == status){//活动批次的码失效
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Invalid);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:活动批次的码失效",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (105 == status){
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Invalid);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:码已经失效",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (106 == status){
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Not_Exist);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:码不存在",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (107 == status){
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Invalid);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:活动过期",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (108 == status){
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Invalid);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:码超过使用次数",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (109 == status){
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Error_Other);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:活动游戏编码错误",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (112 == status){
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Error_Other);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:用户名非该码绑定用户",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
			else if (113 == status){
				retCmd.set_result(Role::stGotGiftBagResult::eCode_Used_Not_Other);
				pUser->sendProtoToMe(retCmd);
				Athena::logger->error("name:%s,id:%d,code:%s 领取礼包不成功:该用户用过礼包码,不可使用其他礼包码",pUser->getName(),pUser->getID(),pData->m_code.c_str());
			}
		}
	}
	if (response&&response->getHttpRequest()&& response->getHttpRequest()->getUserData()){
		pData = (stReqGiftBagInfo *)(response->getHttpRequest()->getUserData());
		response->getHttpRequest()->setUserData(NULL);
		SAFE_DELETE(pData);
	}
}

stGiftBagInfo * CGiftBagMgr::getGiftBagInfo(UINT32 id)
{
	std::map<UINT32,stGiftBagInfo>::iterator it = m_giftBags.find(id);

	if (it != m_giftBags.end()){
		return &it->second;
	}
	else {
		return NULL;
	}
}


void CGiftBagMgr::reqGiftBagReward(Player * pUser,std::string code)
{
	std::map<std::string,std::string> kvSortMap;
	Athena::logger->trace("@@@@@@@@@@@@@@@@【游戏服务器】处理 兑换码 m_opID:%s", code.c_str());

	kvSortMap.insert(std::make_pair("user_account",pUser->getAccount()));
	kvSortMap.insert(std::make_pair("code",code));
	kvSortMap.insert(std::make_pair("o_id",g_int2string(pUser->m_opID)));
	kvSortMap.insert(std::make_pair("game_id","129"));
	kvSortMap.insert(std::make_pair("server_id",Athena::global["serverID"]));
	kvSortMap.insert(std::make_pair("type","n"));
	kvSortMap.insert(std::make_pair("time",g_int2string(g_globalServer->getSec())));

	std::map<std::string,std::string>::iterator it = kvSortMap.begin();
	std::ostringstream os ;
	bool first = true;
	for (;it != kvSortMap.end();++it){
		if (first){
			os << it->first <<"="<< it->second;
			first = false;
		}    
		else {
			os <<"&"<<it->first<<"="<<it->second;
		}   
	}  

	//os << Athena::global["serverKey"];
// 	std::string calcStr = os.str();
// 	calcStr += Athena::global["serverKey"];
// #ifdef _HDZ_DEBUG
// 	Athena::logger->trace("计算md5前的数据:%s",calcStr.c_str());
// #endif

// 	char tokenBuf[1024] = {0};
// 	int  bufLen = 0;
// 	snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",calcStr.c_str());
// 	bufLen = strlen(tokenBuf);
// 	char digest[64] = {0};
// 	utils::Md5(tokenBuf,bufLen,digest);
// 	utils::HexString::bin2asc(digest,16,digest,64,bufLen);
// 	digest[bufLen] = 0;
	os <<"&verify" <<"=" <<"";

	CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
     if (Athena::global["isNotUuzu"] == std::string("true")) {
         std::string url = Athena::global["ApiURL"];
         url += "useGiftCode";

         pRequest->setUrl(url.c_str());
         pRequest->setRequestType(CHttpRequest::kHttpPost);
		 Athena::logger->trace("URL :%s",url.c_str());
     } else {
         pRequest->setUrl("http://api.youzu.com/game/code/use");
         pRequest->setRequestType(CHttpRequest::kHttpPost);
         pRequest->setTag("reqGiftBag");
     }
	// std::string url = "http://oss.hailang.m-dz.com:8077/api/useGiftCode";
	// pRequest->setUrl(url.c_str());
	// pRequest->setRequestType(CHttpRequest::kHttpPost);
		

	stReqGiftBagInfo * pData = ATHENA_NEW stReqGiftBagInfo();
	pData->m_charID = pUser->getID();
	pData->m_code = code;
	pRequest->setUserData(pData);

	pRequest->setRequestData(os.str().c_str(),os.str().size());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送过去的数据是:%s",os.str().c_str());
#endif
		Athena::logger->trace("@@@@@@@@@@@@@[游戏服务器添加异步回调函数 等待OSS Json串返回！！！]]");

	pRequest->setResponseCallback(g_reqGiftBagCB);
		Athena::logger->trace("@@@@@@@@[游戏服务器处理完毕 ！向OSS发送数据：]]:%s",os.str().c_str());
		Athena::logger->trace("==================================================]]");

	CHttpClient::getSingleton().asynSend(pRequest);
}

