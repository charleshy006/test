#include "KoreaHttpInf.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "WebServer.h"
#include "md5.h"
#include "json/reader.h"
#include "json/writer.h"
#include "GameSvrClientMgr.h"
#include "Global.pb.h"
#include "WebUserConnMgr.h"
#include "GameSvrClientMgr.h"
#include "GlobalSvrClient.h"

extern WebServer * g_webServer;

CTblField g_todayAddCharTblDef[] = { 
	{" COUNT(*) " , CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_accountAndCharTblDef[] = {
	{" COUNT(DISTINCT ACCOUNT) " , CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{" COUNT(*) " , 			   CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{" SUM(RECHARGENUM) " ,		   CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_todayRechargeTblDef[] = {
	{" SUM(RECHARGENUM) " ,        CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_searchCharIDTblDef[] = {
	{" CHARID " ,        CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_getRoleDataTblDef[] = {
	{" CHARID " ,       CTblField::DB_TYPE_UINT32,  sizeof(UINT32) 		},
	{ "ACCOUNT",        CTblField::DB_TYPE_STRING,  MAX_ACCNAMESIZE +1  },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 	},
	{ "LEVEL",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  	},
	{ "GOLDCOIN",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  	},
	{ "DIAMOND",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  	},
	{ "GIVEDIAMOND",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  	},
	{ NULL, 0, 0 },
};

void CKoreaHttpInfMgr::load()
{
	
}

bool CKoreaHttpInfMgr::verifySign(std::vector<std::string> &vec,std::string &sign)
{
	char tokenBuf[1024] = { 0 };
	int  bufLen = 0;

	std::ostringstream os ;
	for (UINT16 i = 0;i < vec.size();++i){
		os << vec[i];
	}
	
	snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s%s",os.str().c_str(),KOREA_HTTP_INF_SERVER_KEY);
	bufLen = strlen(tokenBuf);
	char digest[64] = {0};
	utils::Md5(tokenBuf,bufLen,digest);
	utils::HexString::bin2asc(digest,16,digest,64,bufLen);
	digest[bufLen] = 0;
	
	std::string subDigest(&digest[3],24);

	if (sign == subDigest){
		return true;
	}
	else {
		return false;
	}
}

std::string CKoreaHttpInfMgr::getOnlineInfo()
{
	Json::FastWriter writer;
	Json::Value root;
	root["return_code"]	 = 1;
	root["return_msg"] = "";
	Json::Value onlineData;
	
	onlineData["online_user"] = m_onlineNum;

	CTblQueryResult* result = NULL;
	char szWhere[128] = { 0 };
	std::string dateStr = g_utc2StringDate(g_webServer->getSec());
	snprintf(szWhere,sizeof(szWhere)," CREATETIME >= '%s 00:00:00' AND CREATETIME <= '%s 23:59:59' ",dateStr.c_str(),dateStr.c_str());
	result = g_databaseEngine.select(g_todayAddCharTblDef," ROLEINFO ",szWhere,NULL,0,NULL);
	if (result){
		result->setFieldDef(g_todayAddCharTblDef);
		UINT32 recLen = result->cacRecordLen();
		result->fillData(recLen);
		onlineData["daily_character"] = *(UINT32 *)result->getData();
	}

	SAFE_DELETE(result);
	result = g_databaseEngine.select(g_accountAndCharTblDef," ROLEINFO ",NULL,NULL,0,NULL);
	if (result){
		result->setFieldDef(g_accountAndCharTblDef);
		UINT32 recLen = result->cacRecordLen();
		result->fillData(recLen);
		onlineData["total_user"] = *(UINT32 *)result->getData();
		onlineData["total_character"] = *((UINT32 *)result->getData() + 1);
		onlineData["total_pay"] =  *((UINT32 *)result->getData() + 2) / 10;
	}
	SAFE_DELETE(result);
	
	dateStr = g_utc2StringDate(g_webServer->getSec(),"%Y%m%d");
	std::string rechargeTblName = "USER_RECHARGE_LOG_";
	rechargeTblName += dateStr;
	
	result = g_databaseEngine.select(g_todayRechargeTblDef,rechargeTblName.c_str(),NULL,NULL,0,NULL);
	if (result){
		result->setFieldDef(g_todayRechargeTblDef);
		UINT32 recLen = result->cacRecordLen();
		result->fillData(recLen);
		onlineData["daily_pay"] = *(UINT32 *)result->getData();
	}
	SAFE_DELETE(result);
	root["online_data"] = onlineData;
	std::string json_file = writer.write(root);
	return json_file;
}

std::string CKoreaHttpInfMgr::searchRoleID(std::string roleName)
{
	Json::FastWriter writer;
	Json::Value root;
	root["return_code"]	 = 1;
	root["return_msg"] = "";

	CTblQueryResult* result = NULL;
#ifdef _THAILAND_VER
	char szWhere[300] = { 0 };
#else
	char szWhere[128] = { 0 };
#endif
	snprintf(szWhere,sizeof(szWhere)," NAME = '%s' ",roleName.c_str());
	result = g_databaseEngine.select(g_searchCharIDTblDef," ROLEINFO ",szWhere,NULL,0,NULL);
	if (result){
		result->setFieldDef(g_searchCharIDTblDef);
		UINT32 recLen = result->cacRecordLen();
		result->fillData(recLen);
		root["character_id"] = *(UINT32 *)(result->getData());
	}
	SAFE_DELETE(result);
	std::string json_file = writer.write(root);
	return json_file;

}

std::string CKoreaHttpInfMgr::getRoleData(UINT32 charID)
{
	Json::FastWriter writer;
	Json::Value root;
	Json::Value baseData;
	root["return_code"]	 = 1;
	root["return_msg"] = "";

	CTblQueryResult* result = NULL;
	char szWhere[128] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," CHARID = '%d' ",charID);
	result = g_databaseEngine.select(g_getRoleDataTblDef," ROLEINFO ",szWhere,NULL,0,NULL);
	if (result){
		result->setFieldDef(g_getRoleDataTblDef);
		UINT32 recLen = result->cacRecordLen();
		result->fillData(recLen);
		
		stGetRoleDataDB * pData = (stGetRoleDataDB * )result->getData();
		baseData["user_account"] = pData->m_account;
		baseData["character_id"] = pData->m_charID;
		baseData["character_name"] = pData->m_name;
		baseData["user_level"] = pData->m_level;
		baseData["money"] = pData->m_diamond + pData->m_giveDiamond;
		baseData["coin"] = pData->m_goinCoin;
		Json::Value other;
		baseData["other"] = other;
	}
	SAFE_DELETE(result);
	root["base_data"] = baseData;
	std::string json_file = writer.write(root);
	return json_file;
}

bool CKoreaHttpInfMgr::lockCharacter(UINT32 charID,bool bBan,UINT16 connID)
{
	Global::stReqBanUser cmd; 
	cmd.set_charid(charID);
	cmd.set_webconnid(connID);
	cmd.set_ban(bBan);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	return true;
}

void CKoreaHttpInfMgr::sendNotice(UINT32 noticeID,std::string content,UINT32 interval,UINT32 num)
{
	Global::stAddOrUpdateNotice cmd;
	cmd.set_op(Global::eInsert_Notice);
	cmd.set_num(num);
	cmd.set_interval(interval);
	cmd.set_content(content);
	cmd.set_noticeid(noticeID);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void CKoreaHttpInfMgr::delNotice(UINT32 noticeID)
{
	Global::stDelNotice cmd;
	cmd.set_noticeid(noticeID);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void CKoreaHttpInfMgr::sendMail(UINT32 charID,std::string title,std::string content,UINT32 itemID1,UINT32 itemNum1,UINT32 itemID2,UINT32 itemNum2,UINT32 itemID3,UINT32 itemNum3,UINT16 connID)
{
	Global::stSendMail2User cmd;
	cmd.set_content(content);
	cmd.set_title(title);
	cmd.set_webconnid(connID);
	
	if (itemID1 > 0){
		Global::stSendMailAttach * pSyn = cmd.add_attaches();
		if (pSyn){
			pSyn->set_itemid(itemID1);
			pSyn->set_num(itemNum1);
		}
	}
	
	if (itemID2 > 0){
		Global::stSendMailAttach * pSyn = cmd.add_attaches();
		if (pSyn){
			pSyn->set_itemid(itemID2);
			pSyn->set_num(itemNum2);
		}
	}
	
	if (itemID3 > 0){
		Global::stSendMailAttach * pSyn = cmd.add_attaches();
		if (pSyn){
			pSyn->set_itemid(itemID3);
			pSyn->set_num(itemNum3);
		}
	}
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}
