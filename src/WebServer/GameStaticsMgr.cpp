#include "GameStaticsMgr.h"
#include "Log4cxxHelp.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "AthenaNew.h"
#include "WebServer.h"
#include "UserLogMgr.h"
#include "OpIDMgr.h"
#include <ostream>

extern WebServer * g_webServer;

CTblField g_onlineInfoDef[] = {
	{ "YEAR",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "MONTH",   	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "DAY",     	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "TIMESTAMP",	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "ONLINENUM",  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 		},
};

CTblField g_rechargeStaticsDef[] = {
	{ "USERS",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "TIMES",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "ACCOUNT",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "CASH",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "DAU",      	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "PCU",      	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "TIME",		CTblField::DB_TYPE_STRING,	25				},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDef[] = {
	{ "NEWUSERS",   CTblField::DB_TYPE_STRING,  250  			},
	{ "DAY_2",      CTblField::DB_TYPE_STRING,  250  			},
	{ "DAY_3",      CTblField::DB_TYPE_STRING,  250  			},
	{ "DAY_4",      CTblField::DB_TYPE_STRING,  250  			},
	{ "DAY_5",      CTblField::DB_TYPE_STRING,  250  			},
	{ "DAY_6",      CTblField::DB_TYPE_STRING,  250  			},
	{ "DAY_7",      CTblField::DB_TYPE_STRING,  250  			},
	{ "DAY_14",     CTblField::DB_TYPE_STRING,  250  			},
	{ "DAY_30",     CTblField::DB_TYPE_STRING,  250  			},
	{ "TIME",		CTblField::DB_TYPE_STRING,	25				},
	{  NULL, 		0, 							0				},
};

CTblField g_rowCountTblDef[] = {
	{" COUNT(*) " , CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_opIDRowCountTblDef[] = {
	{" OP_ID " , CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{" COUNT(*) " , CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_serverInfoStaticsTblDef[] = {
	{"REG_DEVICE" ,   	 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"LOGIN_DEVICE" , 	 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"REG_ACCOUNT" ,   	 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"LOGIN_ACCOUNT" , 	 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"OLD_LOGIN_ACCOUNT" ,  CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"NEW_LOGIN_ACCOUNT" , 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"NEW_REG_ACCOUNT" ,   	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"ROLE_NUM" , 		 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"CREATE_ROLE_RATE" ,  	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"ACU" , 				CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"PCU" , 				CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"DT" , 				CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"RECHARGE_NUM" , 	 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"ACC_RECHARGE_NUM" ,  	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"NEW_RECHARGE_NUM" ,  	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"RECHARGE_TIMES" ,    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"RECHARGE_AMOUNT" ,   	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"RECHARGE_RATE" ,     	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"ARPU" , 			 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"ARPPU" , 			 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"HAS_DIAMOND" , 		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"AVE_HAS_DIAMOND" ,   	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"RECHARGE_USERS" ,    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"OP_ID" ,    			CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "TIME",				CTblField::DB_TYPE_STRING,	25				},
	{ NULL, 0, 0 },
};

CTblField g_totalRechargeNumTblDef[] = {
	{"ID" ,                CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"NUM" ,               CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};


CTblField g_activityLogQueryTblDef[] = {
	{"STEP_ID" ,           		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"COUNT(*)" ,          		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"COUNT(DISTINCT CHARID)" , CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"ACTIVITY_NAME",           CTblField::DB_TYPE_STRING,  50             },
	{"STEP_DESC",               CTblField::DB_TYPE_STRING,  50             },
	{ NULL, 0, 0 },	
};

void CGameStaticsMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_totalRechargeNumTblDef," TOTAL_RECHARGE_NUM ",NULL,NULL,0,NULL);
	if (result && result->getData() && result->getRowCount()){
		stTotalRechargeNumDB * pDBData = (stTotalRechargeNumDB * )result->getData();
		m_totalRechargeNum = pDBData->m_num;
		m_totalRechargeID = pDBData->m_id;
	}  
	else {
		UINT64 autoID = 0;
		m_totalRechargeNum = 0;
		stTotalRechargeNumDB dbData;
		dbData.m_num = 0;
		g_databaseEngine.insertRecord(g_totalRechargeNumTblDef,(const char *)&dbData,"TOTAL_RECHARGE_NUM",autoID,NULL);
		m_totalRechargeID = autoID;
	}
	SAFE_DELETE(result);
}

std::string CGameStaticsMgr::getOnlineInfo(Json::Value & searchCond)
{
	CTblQueryResult* result = NULL;
	
	struct tm _tm;
	if (searchCond["time"].isString()){
		std::string inputTime = searchCond["time"].asString();
		inputTime += " 00:00:00";
#ifdef _HDZ_DEBUG
		Athena::logger->trace("查询的时间:%s",inputTime.c_str());
#endif
		if (!strptime(inputTime.c_str(), "%Y-%m-%d %H:%M:%S", &_tm)){
			Athena::logger->trace("查询在线人数转换时间失败!");
		}
	}
	else {
#ifdef _HDZ_DEBUG
		Athena::logger->trace("查询在线人数,没有time参数!");
#endif
		RealTime::getLocalTime(_tm,time(NULL));
	}
	
	char szWhere[65] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," YEAR=%d AND MONTH=%d AND DAY=%d ",_tm.tm_year + 1900,_tm.tm_mon + 1,_tm.tm_mday);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("查询条件:%s",szWhere);
#endif

	result = g_databaseEngine.select(g_onlineInfoDef,"ONLINE_STATICS",szWhere,NULL,0,NULL);

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value fields;
	Json::Value data;
	Json::Value dataList;
	Json::Value numInfo;

	fields["time"] = "时间";
	fields["userNum"] = "人数";

	if (result && result->getData()){
		stOnlineInfo * pInfo = (stOnlineInfo *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount(); ++i){
			numInfo["time"] = g_utc2StringFormat(pInfo[i].m_timestamp);
			numInfo["userNum"] = pInfo[i].m_num;
			dataList.append(numInfo);
		}
	}

	data["reg_count"]  = 0;
	data["old_login_count"] = 0;
	data["fields"] = fields;
	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

CTblField g_mallItemIDTblDef[] = {
	{ " DISTINCT ITEMID  ",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0        },	
};

CTblField g_mallItemIDStaticDef[] = {
	{ " count(*)  ",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0        },
}; 

std::string CGameStaticsMgr::getMallBuyInfo(Json::Value & searchCond)
{
	UINT32 fromPage = 1;
	UINT32 pageSize = 5;
	if (searchCond["page"].isInt()){
		fromPage = searchCond["page"].asInt();
	}
	
	if (searchCond["size"].isInt()){
		pageSize = searchCond["size"].asInt();
	}

	CTblQueryResult* result = NULL;
	
	char szSql[250] = { 0 };
	bzero(szSql,sizeof(szSql));
	std::vector<UINT32> itemVec;
	snprintf(szSql,sizeof(szSql)," select DISTINCT ITEMID from MALLITEM_STATICS order by ITEMID asc ");
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_mallItemIDTblDef);
		result->fillData(4);
		UINT32 *pItemID = (UINT32 *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			itemVec.push_back(pItemID[i]);	
		}
	}

	SAFE_DELETE(result);
	
	UINT32 beginIndex = 0;
	if (fromPage > 0){
		beginIndex = (fromPage - 1) * pageSize;
	}


	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	Json::Value fields;
	Json::Value dataList;

	fields["item_id"] = "道具ID";
	fields["user"] = "购买人数";
	fields["times"] = "购买次数";
	fields["cash"] = "钻石消耗";
	//fields["rate"] = "占比";

	Json::Value info;
	
	data["all_total"] = (UINT32)itemVec.size();
	
	for (UINT32 i = beginIndex;(i < beginIndex + pageSize) && (i < itemVec.size());++i){
		info["item_id"] = itemVec[i];
		snprintf(szSql,sizeof(szSql),"select count(DISTINCT CHARID) from MALLITEM_STATICS where ITEMID=%d ",itemVec[i]);
		result = g_databaseEngine.query(szSql,strlen(szSql),szSql); 
		if (result){
			result->setFieldDef(g_mallItemIDStaticDef);
			result->fillData(4);
			info["user"] = *(UINT32 *)result->getData();
		}   
		SAFE_DELETE(result);

		bzero(szSql,sizeof(szSql));
		snprintf(szSql,sizeof(szSql),"select count(*) from MALLITEM_STATICS  where ITEMID=%d ",itemVec[i]);
		result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
		if (result){
			result->setFieldDef(g_mallItemIDStaticDef);
			result->fillData(4);
			info["times"] =  *(UINT32 *)result->getData();
		}
		SAFE_DELETE(result);
		
		bzero(szSql,sizeof(szSql));
		snprintf(szSql,sizeof(szSql),"select count(CONSUME_DIAMOND) from MALLITEM_STATICS  where ITEMID=%d ",itemVec[i]);
		result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
		if (result){
			result->setFieldDef(g_mallItemIDStaticDef);
			result->fillData(4);
			info["cash"] =  *(UINT32 *)result->getData();
		}
		SAFE_DELETE(result);
		dataList.append(info);
	}
	data["fields"] = fields;
	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	return json_file;
}

std::string CGameStaticsMgr::getRechargeInfo(Json::Value & searchCond)
{
	std::string start;
	std::string end;

	if (searchCond["stime"].isString()){
		start = searchCond["stime"].asString();
	}
	
	if (searchCond["etime"].isString()){
		end = searchCond["etime"].asString();
	}
	
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	Json::Value opID;
	Json::Value x;
	Json::Value y;
	Json::Value statics;
	
	Json::Value yAccount;
	Json::Value yTimes;
	Json::Value yCash;

	char szWhere[128] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," TIME >= '%s' AND TIME <= '%s' ",start.c_str(),end.c_str());

	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_rechargeStaticsDef," RECHARGE_STATICS ",szWhere,NULL,0,NULL);

	if (result && result->getData()){
		stRechargeStatics * pData = (stRechargeStatics *)result->getData();
		Json::Value info;
		for (UINT16 i = 0;i < result->getRowCount();++i){
			info["user"]  = pData[i].m_users;
			info["times"] = pData[i].m_times;
			info["account"] = pData[i].m_accounts;
			info["cash"] = pData[i].m_cash;
			info["dau"] = pData[i].m_dau;
			info["pcu"] = pData[i].m_pcu;
			info["time"] = pData[i].m_date;
			Json::Value xSub(pData[i].m_date);
			x.append(xSub);
			statics.append(info);
			Json::Value subAccount(pData[i].m_accounts);
			Json::Value subTimes(pData[i].m_times);
			Json::Value subCash(pData[i].m_cash);

			yAccount.append(subAccount);
			yTimes.append(subTimes);
			yCash.append(subCash);
		}
	}
	
	y["付费人数"] = yAccount;
	y["充值次数"] = yTimes;
	y["充值现金"] = yCash;

	opID["测试区"] = statics;
	data["2106"] = opID;
	data["x"] = x;
	data["y"] = y;
	root["date"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

CTblField g_haveInfoStaticsNewUsersDef[] = {
	{ "NEWUSERS",   CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDay2Def[] = {
	{ "DAY_2",      CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDay3Def[] = {
	{ "DAY_3",      CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDay4Def[] = {
	{ "DAY_4",      CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDay5Def[] = {
	{ "DAY_5",      CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDay6Def[] = {
	{ "DAY_6",      CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDay7Def[] = {
	{ "DAY_7",      CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDay14Def[] = {
	{ "DAY_14",      CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

CTblField g_haveInfoStaticsDay30Def[] = {
	{ "DAY_30",      CTblField::DB_TYPE_STRING,  150  			},
	{ NULL, 0, 0        },
};

void CGameStaticsMgr::refreshHavaInfo()
{
	std::string today = g_utc2StringDate(g_webServer->getSec());
	
	std::map<UINT32,UINT32> opIDCountNum;
	std::ostringstream os;

	//计算新增人数
	char szWhere[65] = { 0 };
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(FIRST_CREATE_ROLE_TIME,\"%%Y-%%m-%%d\") = '%s'",today.c_str());
	getRecordCountByOPID(" OP_ID,count(*)","ENTER_SELECT_PAGE",szWhere," OP_ID ",opIDCountNum);
	std::map<UINT32,UINT32>::iterator it = opIDCountNum.begin();
	bool bFirst = true;
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	
	static char g_tmpVal[150] = { 0 };
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));

	std::string tblLoginLog = UserLogMgr::getSingleton().getRealTblName("USER_LOGIN_LOG");
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today.c_str());
	if (getRecordCount("HAVEINFO_STATICS",szWhere) > 0){
		g_databaseEngine.updateRecord(g_haveInfoStaticsNewUsersDef,(const char *)g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	
	}
	else {
		UINT64 autoID = 0;
		stHaveInfoStatics dbData;
		strncpy(dbData.m_newUsers,os.str().c_str(),sizeof(dbData.m_newUsers));
		strncpy(dbData.m_date,today.c_str(),sizeof(dbData.m_date));
		g_databaseEngine.insertRecord(g_haveInfoStaticsDef,(const char *)&dbData,"HAVEINFO_STATICS",autoID);
	}
	
	std::string todayLoginTblName = "USER_LOGIN_LOG_"+ g_utc2StringDate(g_webServer->getSec(),"%Y%m%d");
	//today - 1 的次日留存
	bzero(szWhere,sizeof(szWhere));
	std::string today_2 = g_utc2StringDate(g_webServer->getSec() - 86400);
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(CREATETIME,\"%%Y-%%m-%%d\") = '%s'",today_2.c_str());	
	//UINT32 day2 = getRecordCount(todayLoginTblName.c_str(),szWhere," COUNT(DISTINCT ACCOUNT) ");
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,COUNT(DISTINCT ACCOUNT) ",todayLoginTblName.c_str(),szWhere," OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	bFirst = true;
	os.str("");
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));

	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today_2.c_str());
	g_databaseEngine.updateRecord(g_haveInfoStaticsDay2Def,(const char *)&g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	

	//today - 2 的次日留存
	bzero(szWhere,sizeof(szWhere));
	std::string today_3 = g_utc2StringDate(g_webServer->getSec() - 86400 * 2);
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(CREATETIME,\"%%Y-%%m-%%d\") = '%s'",today_3.c_str());	
	//UINT32 day3 = getRecordCount(todayLoginTblName.c_str(),szWhere,"COUNT(DISTINCT ACCOUNT)");	
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,COUNT(DISTINCT ACCOUNT) ",todayLoginTblName.c_str(),szWhere," OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	bFirst = true;
	os.str("");
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));

	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today_3.c_str());
	g_databaseEngine.updateRecord(g_haveInfoStaticsDay3Def,(const char *)g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	
	
	//today - 3 的次日留存
	bzero(szWhere,sizeof(szWhere));
	std::string today_4 = g_utc2StringDate(g_webServer->getSec() - 86400 * 3);
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(CREATETIME,\"%%Y-%%m-%%d\") = '%s'",today_4.c_str());	
	//UINT32 day4 = getRecordCount(todayLoginTblName.c_str(),szWhere,"COUNT(DISTINCT ACCOUNT)");	
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,COUNT(DISTINCT ACCOUNT) ",todayLoginTblName.c_str(),szWhere," OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	bFirst = true;
	os.str("");
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));
	
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today_4.c_str());
	g_databaseEngine.updateRecord(g_haveInfoStaticsDay4Def,(const char *)g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	
	
	//today - 4 的次日留存
	bzero(szWhere,sizeof(szWhere));
	std::string today_5 = g_utc2StringDate(g_webServer->getSec() - 86400 * 4);
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(CREATETIME,\"%%Y-%%m-%%d\") = '%s'",today_5.c_str());	
	//UINT32 day5 = getRecordCount(todayLoginTblName.c_str(),szWhere,"COUNT(DISTINCT ACCOUNT)");	
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,COUNT(DISTINCT ACCOUNT) ",todayLoginTblName.c_str(),szWhere," OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	bFirst = true;
	os.str("");
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));
	
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today_5.c_str());
	g_databaseEngine.updateRecord(g_haveInfoStaticsDay5Def,(const char *)g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	
	
	//today - 5 的次日留存
	bzero(szWhere,sizeof(szWhere));
	std::string today_6 = g_utc2StringDate(g_webServer->getSec() - 86400 * 5);
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(CREATETIME,\"%%Y-%%m-%%d\") = '%s'",today_6.c_str());	
	//UINT32 day6 = getRecordCount(todayLoginTblName.c_str(),szWhere,"COUNT(DISTINCT ACCOUNT)");	
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,COUNT(DISTINCT ACCOUNT) ",todayLoginTblName.c_str(),szWhere," OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	bFirst = true;
	os.str("");
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));
	
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today_6.c_str());
	g_databaseEngine.updateRecord(g_haveInfoStaticsDay6Def,(const char *)g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	
	
	//today - 6 的次日留存
	bzero(szWhere,sizeof(szWhere));
	std::string today_7 = g_utc2StringDate(g_webServer->getSec() - 86400 * 6);
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(CREATETIME,\"%%Y-%%m-%%d\") = '%s'",today_7.c_str());	
	//UINT32 day7 = getRecordCount(todayLoginTblName.c_str(),szWhere,"COUNT(DISTINCT ACCOUNT)");	
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,COUNT(DISTINCT ACCOUNT) ",todayLoginTblName.c_str(),szWhere," OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	bFirst = true;
	os.str("");
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));
	
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today_7.c_str());
	g_databaseEngine.updateRecord(g_haveInfoStaticsDay7Def,(const char *)g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	

	//today - 13 的留存
	bzero(szWhere,sizeof(szWhere));
	std::string today_14 = g_utc2StringDate(g_webServer->getSec() - 86400 * 13);
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(CREATETIME,\"%%Y-%%m-%%d\") = '%s'",today_14.c_str());	
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,COUNT(DISTINCT ACCOUNT) ",todayLoginTblName.c_str(),szWhere," OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	bFirst = true;
	os.str("");
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));
	
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today_14.c_str());
	g_databaseEngine.updateRecord(g_haveInfoStaticsDay14Def,(const char *)g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	
	
	//today - 29 的留存
	bzero(szWhere,sizeof(szWhere));
	std::string today_30 = g_utc2StringDate(g_webServer->getSec() - 86400 * 29);
	snprintf(szWhere,sizeof(szWhere),"DATE_FORMAT(CREATETIME,\"%%Y-%%m-%%d\") = '%s'",today_30.c_str());	
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,COUNT(DISTINCT ACCOUNT) ",todayLoginTblName.c_str(),szWhere," OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	bFirst = true;
	os.str("");
	for (;it != opIDCountNum.end();++it){
		if (bFirst){
			os << it->first << "-" << it->second;
			bFirst = false;
		}
		else {
			os <<";" << it->first << "-" << it->second;
		}
	}
	bzero(g_tmpVal,sizeof(g_tmpVal));
	strncpy(g_tmpVal,os.str().c_str(),sizeof(g_tmpVal));
	
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME ='%s'",today_30.c_str());
	g_databaseEngine.updateRecord(g_haveInfoStaticsDay30Def,(const char *)g_tmpVal,"HAVEINFO_STATICS",szWhere,NULL);	
}

void CGameStaticsMgr::timer(UINT32 cur)
{
	if (cur < g_webServer->getOpenServerTime()){//还没有开服,不计算
		return ;
	}

	if (m_lastRefreshTime + s_REFRESH_HAVA_INTERVAL > cur){
		return ;
	}	
#ifdef _HDZ_DEBUG
	Athena::logger->trace("CGameStaticsMgr::timer");
#endif 
	m_lastRefreshTime = cur;
	refreshHavaInfo();
	refreshServerInfoStatics1();
}

void CGameStaticsMgr::initHavaData(std::map<UINT32,UINT32> & channel2Num,std::string inStr)
{
	std::vector<std::string> strVec;
	g_stringtok(strVec,inStr,";");

	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string> strSubVec;
		g_stringtok(strSubVec,strVec[i],"-");

		if (strSubVec.size() >= 2){
			UINT32 opID = (UINT32)atoi(strSubVec[0].c_str());
			UINT32 num = (UINT32)atoi(strSubVec[1].c_str());
			channel2Num[opID] = num;
			channel2Num[0] += num;
		}
	}
}

std::string CGameStaticsMgr::getHavaInfo(Json::Value & searchCond)
{
	std::string start;
	std::string end;

	if (searchCond["stime"].isString()){
		start = searchCond["stime"].asString();
	}
	
	if (searchCond["etime"].isString()){
		end = searchCond["etime"].asString();
	}

	UINT32 page = 0; 
	UINT32 size = 0; 

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}    

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}
	
	UINT32 opID = 0;

	if (searchCond["op_id"].isString()){
		opID = (UINT32)atoi(searchCond["op_id"].asString().c_str());
	}

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	Json::Value fields;
	Json::Value dataList;
	
	Json::Value field;
	fields["1"] = "日期" ;
	fields["2"] = "渠道";
	fields["3"] = "注册人数";
	fields["4"] = "2日留存";
	fields["5"] = "3日留存";
	fields["6"] = "4日留存";
	fields["7"] = "5日留存";
	fields["8"] = "6日留存";
	fields["9"] = "7日留存";
	fields["14"] = "14日留存";
	fields["30"] = "30日留存";

	//fields["9"] = "14日留存";
	//fields["10"] = "21日留存";
	//fields["11"] = "30日留存";
	
	char szWhere[128] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," TIME >= '%s' AND TIME <= '%s' ",start.c_str(),end.c_str());

	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_haveInfoStaticsDef," HAVEINFO_STATICS ",szWhere,NULL,size,NULL,(page - 1) * size);
	std::map<UINT32,UINT32> opID2Num[10];

	if (result && result->getData()){
		stHaveInfoStatics * pData = (stHaveInfoStatics *)result->getData();
		Json::Value info;
		for (UINT16 i = 0;i < result->getRowCount();++i){
			opID2Num[0].clear();
			opID2Num[1].clear();
			opID2Num[2].clear();
			opID2Num[3].clear();
			opID2Num[4].clear();
			opID2Num[5].clear();
			opID2Num[6].clear();
			opID2Num[7].clear();
			opID2Num[8].clear();
			opID2Num[9].clear();

			info["1"]  = pData[i].m_date;
			initHavaData(opID2Num[1],pData[i].m_newUsers);
			info["2"] = COpIDMgr::getSingleton().getOpIDName(opID);
			//info["2"] = pData[i].m_newUsers;
			UINT32 newUsers = opID2Num[1][opID];
			info["3"] = newUsers;
			if (newUsers > 0){
				/*info["3"] =	g_float2string(((pData[i].m_day_2 * 1.0f) / pData[i].m_newUsers * 100)) + "%";
				info["4"] = g_float2string(((pData[i].m_day_3 * 1.0f) / pData[i].m_newUsers * 100)) + "%";
				info["5"] = g_float2string(((pData[i].m_day_4 * 1.0f) / pData[i].m_newUsers * 100)) + "%";
				info["6"] = g_float2string(((pData[i].m_day_5 * 1.0f) / pData[i].m_newUsers * 100)) + "%";
				info["7"] = g_float2string(((pData[i].m_day_6 * 1.0f) / pData[i].m_newUsers * 100)) + "%";
				info["8"] = g_float2string(((pData[i].m_day_7 * 1.0f) / pData[i].m_newUsers * 100)) + "%";*/
				initHavaData(opID2Num[2],pData[i].m_day_2);
				initHavaData(opID2Num[3],pData[i].m_day_3);
				initHavaData(opID2Num[4],pData[i].m_day_4);
				initHavaData(opID2Num[5],pData[i].m_day_5);
				initHavaData(opID2Num[6],pData[i].m_day_6);
				initHavaData(opID2Num[7],pData[i].m_day_7);
				initHavaData(opID2Num[8],pData[i].m_day_14);
				initHavaData(opID2Num[9],pData[i].m_day_30);

				info["4"] =	g_float2string(((opID2Num[2][opID] * 1.0f) / newUsers * 100)) + "%";
				info["5"] = g_float2string(((opID2Num[3][opID] * 1.0f) / newUsers * 100)) + "%";
				info["6"] = g_float2string(((opID2Num[4][opID] * 1.0f) / newUsers * 100)) + "%";
				info["7"] = g_float2string(((opID2Num[5][opID] * 1.0f) / newUsers * 100)) + "%";
				info["8"] = g_float2string(((opID2Num[6][opID] * 1.0f) / newUsers * 100)) + "%";
				info["9"] = g_float2string(((opID2Num[7][opID] * 1.0f) / newUsers * 100)) + "%";
				info["14"] = g_float2string(((opID2Num[8][opID] * 1.0f) / newUsers * 100)) + "%";
				info["30"] = g_float2string(((opID2Num[9][opID] * 1.0f) / newUsers * 100)) + "%";
			}
			else {
				info["4"] = "0.00%";
				info["5"] = "0.00%";
				info["6"] = "0.00%";
				info["7"] = "0.00%";
				info["8"] = "0.00%";
				info["9"] = "0.00%";
				info["14"] = "0.00%";
				info["30"] = "0.00%";
			}
			//info["9"] = pData[i].m_day_14;
			//info["10"] = pData[i].m_day_21;
			//info["11"] = pData[i].m_day_30;
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

CTblField g_queryExchangeTblDef[] ={
	{ "MODULE_NAME",    CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "COUNT(*)",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "COUNT(NUM)",    CTblField::DB_TYPE_UINT64,  sizeof(UINT64)   },
	{ "COUNT(DISTINCT CHARID)",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ NULL, 0, 0 },
};

#pragma pack(1) 
struct stQueryExchangeInfoDB
{
	char m_name[33];
	UINT32 m_times;
	UINT64 m_flagCount;
	UINT32 m_playerCount;

	stQueryExchangeInfoDB()
	{
		bzero(m_name,sizeof(m_name));
		m_times = 0;
		m_flagCount = 0;
		m_playerCount = 0;
	}
};
#pragma pack() 

std::string CGameStaticsMgr::getExchangeList(Json::Value & searchCond)
{
	 UINT32 page = 1;
	 UINT32 size = 0;
	 std::string start;
	 std::string end;
	 UINT32 flag = 0;
	
	 std::ostringstream os;
	 bool addCond  = false;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

	if (searchCond["stime"].isString()){
		start = searchCond["stime"].asString();
		if (start.size() > 0){
			start += " 00:00:00";
			if (!addCond){
				addCond = true;
				os << " DOTIME >= '" << start << "' ";
			}
			else {
				os << " AND DOTIME <= '" << start << "' ";
			}
		}
	}
	
	if (searchCond["etime"].isString()){
		end = searchCond["etime"].asString();
		if (end.size() > 0){
			end += " 23:59:59";
			if (!addCond){
				addCond = true;
				os << " DOTIME <= '" << end << "' ";
			}
			else {
				os << " AND DOTIME <= '" << end << "' ";
			}
		}
	}

	if (searchCond["flag"].isInt()){
		flag = searchCond["flag"].asInt();

		if (1 == flag){
			flag = eResource_Diamond;	
		} 
		else if (2 == flag){
			flag = eResource_GoldCoin;
		}

		if (!addCond){
			addCond = true;
			os << " FLAG = " << flag;
		}
		else {
			os << " AND FLAG = " << flag;
		}
	}


	Json::FastWriter writer;
	Json::Value root;
	Json::Value data;
	Json::Value objList;
	Json::Value obj;

	root["status"] = 1;
	data["login_count"] = 0;
	data["total"] = 0;

	CTblQueryResult* result = NULL;
	std::ostringstream osSql;
	osSql << "SELECT MODULE_NAME,COUNT(*),SUM(NUM) ,COUNT(DISTINCT CHARID) FROM " << UserLogMgr::getSingleton().getExchangeListTable(start,end)
		<< " WHERE " << os.str() << " GROUP BY  MODULE_NAME ";
	//char szSql[1024] = { 0 };
	//snprintf(szSql,sizeof(szSql),"SELECT MODULE_NAME, COUNT(DISTINCT CHARID),COUNT(*),SUM(NUM) FROM EXCHANGE_LIST GROUP BY  MODULE_NAME limit %d,%d",(page - 1) * size,size);
	//snprintf(szSql,sizeof(szSql),"SELECT MODULE_NAME,COUNT(*),SUM(NUM) ,COUNT(DISTINCT CHARID) FROM EXCHANGE_LIST WHERE %s GROUP BY  MODULE_NAME",os.str().c_str());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("查询消耗的sql语句:%s",osSql.str().c_str());
#endif
	result =  g_databaseEngine.query(osSql.str().c_str(),osSql.str().size(),osSql.str().c_str());
	if (result){
		result->setFieldDef(g_queryExchangeTblDef);
		UINT32 len = result->cacRecordLen();
		result->fillData(len);
		stQueryExchangeInfoDB * pDBData = (stQueryExchangeInfoDB * )result->getData();
#ifdef _HDZ_DEBUG
		Athena::logger->trace("查询到的记录数:%d",result->getRowCount());
#endif
		for (UINT16 i = 0;i < result->getRowCount();++i){
			obj["modules"] =  pDBData[i].m_name;
			obj["flag_count"] = (double)pDBData[i].m_flagCount;
			obj["times"] = pDBData[i].m_times;
			obj["player_count"] = pDBData[i].m_playerCount;
			obj["name"] = pDBData[i].m_name; 
			objList.append(obj);
		}
	}

	SAFE_DELETE(result);
	data["list"] = objList;
	root["data"] = data;
	std::string json_file = writer.write(root);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("返回查询结果");
#endif
	return json_file;
}

UINT32 CGameStaticsMgr::getRecordCount(const char * szTableName,const char * szWhere,const char * szCountField)
{
	UINT32 ret = 0;
	if (!szTableName || !szWhere){
		return ret;
	}   

	CTblQueryResult* result = NULL;
	char szSql[250] = { 0 };
	bzero(szSql,sizeof(szSql));
	if (szCountField){
		snprintf(szSql,sizeof(szSql),"select %s from %s where %s ",szCountField,szTableName,szWhere);
	}
	else {
		snprintf(szSql,sizeof(szSql),"select count(*) from %s where %s ",szTableName,szWhere);
	}
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_rowCountTblDef);
		result->fillData(4);
		ret = *(UINT32 *)result->getData();
	}   
	SAFE_DELETE(result);
	return ret;
}

void  CGameStaticsMgr::getRecordCountByOPID(const char * szCountField,const char * szTableName,const char * szWhere,const char * szGroupBy,std::map<UINT32,UINT32> & kvPair)
{
	if (!szCountField || !szTableName){
		return ;
	}
	
	CTblQueryResult* result = NULL;
	std::ostringstream os;
	os << " select " << os.str().c_str() << szCountField << " from " << szTableName;
	if (szWhere){
		os << " where " << szWhere ;
	}

	if (szGroupBy){
		os << " group by " << szGroupBy;
	}

	result = g_databaseEngine.query(os.str().c_str(),os.str().size(),os.str().c_str());
	if (result){
		result->setFieldDef(g_opIDRowCountTblDef);
		result->fillData(4 + 4);
		for (UINT16 i = 0;i < result->getRowCount();++i){
			UINT32 opID = *(UINT32*)(result->getData() + i * 8);
			UINT32 num = *(UINT32 *)(result->getData() + 4 + i * 8);
			kvPair[opID] = num;
		}
	}   
	SAFE_DELETE(result);
}

CTblField g_openServerDayNumDef[] = {
	{ "OPEN_DAY",  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 		},
};

std::string CGameStaticsMgr::getServerInfo(Json::Value & searchCond)
{
	std::string start;
	std::string end;
	
	std::ostringstream os;
	bool addCond  = false;

	if (searchCond["starttime"].isString()){
		start = searchCond["starttime"].asString();
		if (start.size() > 0){
			addCond = true;
			os << " TIME >= '" << start << "' ";
		}
		else {
			os << " AND TIME <= '" << start << "' ";
		}
	}
	
	if (searchCond["endtime"].isString()){
		end = searchCond["endtime"].asString();
		if (end.size() > 0){
			if (!addCond){
				addCond = true;
				os << " TIME <= '" << end << "' ";
			}
			else {
				os << " AND TIME <= '" << end << "' ";
			}
		}
	}
	
	std::string opID;
	if (searchCond["op_id"].isString()){
		opID = searchCond["op_id"].asString();
		if (opID.size() > 0){
			
		}
		else {
			opID = "0";
		}

		if (!addCond){
			addCond = true;
			os << " OP_ID = " << opID;
		}
		else {
			os << " AND  OP_ID =  " << opID;
		}
	}


	UINT32 page = 0; 
	UINT32 size = 0; 

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}    

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	Json::Value fields;
	Json::Value dataList;
	Json::Value countFields;
	Json::Value extra;

	countFields.append("02_regDevice");
	countFields.append("12_pcu");
	countFields.append("03_loginDevice");
	//countFields.append("");

	Json::Value field;
	fields["00_opID"] = "渠道";
	fields["01_timer"] = "日期" ;
	fields["02_regDevice"] = "进入设备数量";
	fields["03_loginDevice"] = "登陆设备数";
	fields["04_regAccount"] = "进入账号数";
	fields["05_loginAccount"] = "登录账号数";
	fields["06_oldLoginAccount"] = "老账号登录数";
	fields["07_newLoginAccount"] = "新账号登录数";
	fields["08_newRegAccount"] = "新进入账号数量";
	fields["09_roleNum"] = "创建角色数量";
	fields["10_createRoleRate"] = "创角率";
	if (opID == "0"){
		fields["11_acu"] = "平均在线";
		fields["12_pcu"] = "最高在线";
		fields["13_dt"] = "dt时间";
	}
	fields["14_rechargeNum"] = "充值账号数";
	fields["15_accRechargeNum"] = "累计充值账号";
	fields["16_newRechargeNum"] = "新增充值账号";
	fields["17_rechargeTimes"] = "充值次数";
	fields["18_rechargeAmount"] = "充值总额";
	fields["19_rechargeRate"] = "付费率";
	fields["20_arpu"] = "arpu值";
	fields["21_arppu"] = "arppu值";
	fields["22_hasDiamond"] = "付费玩家用户钻石";
	fields["23_avHasDiamond"] = "付费玩家平均钻石";
	fields["24_rechargeUsers"] = "充值保有人数";

	//char szWhere[128] = { 0 };
	//snprintf(szWhere,sizeof(szWhere)," TIME >= '%s' AND TIME <= '%s' ",start.c_str(),end.c_str());

	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_serverInfoStaticsTblDef," SERVERINFO_STATICS ",os.str().c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		Json::Value info;
		stServerInfoStatics * pData = (stServerInfoStatics *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			info["00_opID"] = COpIDMgr::getSingleton().getOpIDName(pData[i].m_opID);
			info["01_timer"] =  pData[i].m_date ;
			info["02_regDevice"] = pData[i].m_regDevice;
			info["03_loginDevice"] =  pData[i].m_loginDevice;
			info["04_regAccount"] = pData[i].m_regAccount;
			info["05_loginAccount"] = pData[i].m_loginAccount;
			info["06_oldLoginAccount"] = pData[i].m_oldLoginAccount;
			info["07_newLoginAccount"] = pData[i].m_newLoginAccount;
			info["08_newRegAccount"] = pData[i].m_newRegAccount;
			info["09_roleNum"] = pData[i].m_roleNum;
			std::string createRoleRateStr = g_float2string(pData[i].m_createRoleRate / 100.0f);
			createRoleRateStr += "%";
			info["10_createRoleRate"] = createRoleRateStr;
			if (opID == "0"){
				info["11_acu"] = pData[i].m_acu;
				info["12_pcu"] = pData[i].m_pcu;
				info["13_dt"] = pData[i].m_dt;
			}
			info["14_rechargeNum"] = pData[i].m_rechargeNum;
			info["15_accRechargeNum"] = pData[i].m_accRechargeNum;
			info["16_newRechargeNum"] = pData[i].m_newRechargeNum;
			info["17_rechargeTimes"] = pData[i].m_rechargeTimes;
			info["18_rechargeAmount"] = pData[i].m_rechargeAmount;
			std::string rechargeRate = g_float2string(pData[i].m_rechargeRate / 100.0f);
			rechargeRate += "%";
			info["19_rechargeRate"] = rechargeRate;
			info["20_arpu"] = g_float2string(pData[i].m_arpu / 100.0f);
			info["21_arppu"] = g_float2string(pData[i].m_arppu / 100.0f);
			info["22_hasDiamond"] = pData[i].m_hasDiamond;
			info["23_avHasDiamond"] = pData[i].m_avHasDiamond;
			info["24_rechargeUsers"] = pData[i].m_rechargeUsers;
			dataList.append(info);
		}
	}

	SAFE_DELETE(result);
	
	Json::Value subExtra1;
	Json::Value subExtra2;
	Json::Value subExtra3;
	subExtra1.append("开服天数");
	result = g_databaseEngine.select(g_openServerDayNumDef," OPEN_SERVER_INFO ",NULL,NULL);
	if (result && result->getData() && result->getRowCount()){
		UINT32 dayNum = *(UINT32 *)(result->getData());
		subExtra1.append(dayNum);
	}
	else {
		subExtra1.append(0);
	}
	SAFE_DELETE(result);
	extra.append(subExtra1);

	subExtra2.append("累计充值");
	subExtra2.append(0);
	extra.append(subExtra2);

	subExtra3.append("充值帐号总数");
	subExtra3.append(0);
	extra.append(subExtra3);

	data["fields"] = fields;
	data["list"] = dataList;
	data["countfields"] = countFields;
	data["extra"] = extra;
	root["data"] = data;
	std::string json_file = writer.write(root);
	return json_file;

}

std::string CGameStaticsMgr::getActivityLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	Json::Value data;
	Json::Value dataList;	
	
	UINT32 page = 1;
	UINT32 size = 0;
	std::string start;
	std::string end;

	std::ostringstream os;
	bool addCond  = false;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  
	
	UINT32 activityID = 0;
	if (searchCond["ativity_id"].isString()){
		activityID = (UINT32)atoi(searchCond["ativity_id"].asString().c_str());		
	}

	if (searchCond["starttime"].isString()){
		start = searchCond["starttime"].asString();
		start += " 00:00:00";
		if (start.size() > 0){
			addCond = true;
			os << " DOTIME >= '" << start << "' ";
		}
		else {
			os << " AND DOTIME <= '" << start << "' ";
		}
	}

	if (searchCond["endtime"].isString()){
		end = searchCond["endtime"].asString();
		end += " 23:59:59";
		if (end.size() > 0){
			if (!addCond){
				addCond = true;
				os << " DOTIME <= '" << end << "' ";
			}
			else {
				os << " AND DOTIME <= '" << end << "' ";
			}
		}
	}
	
	if (!addCond){
		addCond = true;
		os <<" ACTIVITY_ID=" <<activityID;
	}
	else {
		os <<" AND ACTIVITY_ID=" <<activityID;
	}

	CTblQueryResult* result = NULL;
	std::ostringstream osSql;
	osSql << "SELECT STEP_ID,COUNT(*),COUNT(DISTINCT CHARID),ACTIVITY_NAME,STEP_DESC FROM " << UserLogMgr::getSingleton().getUnionTblName("ACTIVITY_LOG")
		<< " WHERE " << os.str() << " GROUP BY  STEP_ID ";
#ifdef _HDZ_DEBUG
	Athena::logger->trace("查询活动参与的sql语句:%s",osSql.str().c_str());
#endif
	result =  g_databaseEngine.query(osSql.str().c_str(),osSql.str().size(),osSql.str().c_str());
	if (result){
		result->setFieldDef(g_activityLogQueryTblDef);
		UINT32 len = result->cacRecordLen();
		result->fillData(len);
		stActivityQueryInfo * pDBData = (stActivityQueryInfo * )result->getData();
		data["total"] = 1;
		Json::Value data;
		Json::Value details;

		for (UINT16 i = 0;i < result->getRowCount();++i){
			if (0 == i){
				data["activity_id"] = activityID;
				data["name"] =	 pDBData[i].m_name;
			}
			Json::Value detail;
			detail["name"] = pDBData[i].m_desc;
			detail["count"] = pDBData[i].m_playerCount;
			detail["times"] = pDBData[i].m_playerTime;
			details.append(detail);
		}
		data["detail"] = details;
		dataList.append(data);
	}

	SAFE_DELETE(result);
	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	return json_file;
}

CTblField g_acuPcuTblDef[] = {
	{ " count(*) ",   		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ " sum(ONLINENUM) ",   CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ " max(ONLINENUM) ",   CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ NULL, 0, 0 		},
};

void CGameStaticsMgr::refreshServerInfoStatics1()
{
	m_refreshServerStaticNum += 1;
	Athena::logger->trace("开始刷服务器数据:");
	CTblQueryResult* result = NULL;
	std::string tblEnterPageLog = UserLogMgr::getSingleton().getRealTblName("ENTER_SELECT_PAGE_LOG");
	std::string tblLoginLog = UserLogMgr::getSingleton().getRealTblName("USER_LOGIN_LOG");
	std::string tblRechargeLog = UserLogMgr::getSingleton().getRealTblName("USER_RECHARGE_LOG");
	std::string tblCreateRoleLog = UserLogMgr::getSingleton().getRealTblName("CREATE_ROLE_LOG");
	char szWhere[225] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," ID > 0 ");
	std::string today = g_utc2StringDate(g_webServer->getSec());
	
	//计算在线人数和最高人数
	UINT32 acu = 0;
	UINT32 pcu = 0;

	struct tm _tm;
	RealTime::getLocalTime(_tm,g_webServer->getSec());
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," YEAR=%d AND MONTH=%d AND DAY=%d ",_tm.tm_year + 1900,_tm.tm_mon + 1,_tm.tm_mday);

	char szSql[250] = { 0 };
	bzero(szSql,sizeof(szSql));
	snprintf(szSql,sizeof(szSql),"select count(*),sum(ONLINENUM),max(ONLINENUM) from ONLINE_STATICS where %s ",szWhere);
	result = g_databaseEngine.query(szSql,strlen(szSql),szSql);
	if (result){
		result->setFieldDef(g_acuPcuTblDef);
		result->fillData(12);
		UINT32  recordNum= *(UINT32 *)result->getData();
		UINT32 totalUser = *((UINT32 *)result->getData() + 1);
		UINT32 maxOnline = *((UINT32 *)result->getData() + 2);
		acu = recordNum > 0 ? (totalUser / recordNum) : 0;
		pcu = maxOnline;
	}
	SAFE_DELETE(result);
	
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," (DOTIME >='%s 00:00:00') AND (DOTIME <= '%s 23:59:59')",today.c_str(),today.c_str());
	//UINT32 roleNum = getRecordCount(tblCreateRoleLog.c_str(),szWhere);

	std::map<UINT32,stServerInfoStatics> serverInfos;
	std::vector<UINT32> opIDVec = COpIDMgr::getSingleton().getAllOpID(g_webServer->getPlatformID());
#ifdef  _HDZ_DEBUG
    Athena::logger->trace("[GameStaticsMgr] platform id is %d, opIdVec size is %d",
            g_webServer->getPlatformID(), opIDVec.size());
#endif
	for (UINT16 i = 0;i < opIDVec.size();++i){
		stServerInfoStatics & info = serverInfos[opIDVec[i]];	
		info.m_acu = acu;
		info.m_pcu = pcu;
		//info.m_roleNum = roleNum;
		info.m_opID = opIDVec[i];
	}
	
	//注册设备
	std::map<UINT32,UINT32> opIDCountNum;
	getRecordCountByOPID(" OP_ID,count(DISTINCT DEVICE_ID,OP_ID)",tblEnterPageLog.c_str(),NULL," OP_ID ",opIDCountNum);
	std::map<UINT32,UINT32>::iterator it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		serverInfos[it->first].m_regDevice = it->second;
	}
	
	//登录设备
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(DISTINCT DEVICE_ID,OP_ID) ",tblLoginLog.c_str(),NULL, " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		serverInfos[it->first].m_loginDevice = it->second;
	}
	
	//注册账号
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(DISTINCT ACCOUNT,OP_ID) ",tblEnterPageLog.c_str(),NULL, " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		serverInfos[it->first].m_regAccount = it->second;
	}
	
	//登录账号
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(DISTINCT ACCOUNT,OP_ID) ",tblLoginLog.c_str(),NULL, " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		stServerInfoStatics & dbData = serverInfos[it->first];
		dbData.m_loginAccount = it->second;
		dbData.m_dt = dbData.m_loginAccount > 0 ? ((dbData.m_acu * 24 * 60) / dbData.m_loginAccount) : 0;
	}
	//bzero(szWhere,sizeof(szWhere));
	//snprintf(szWhere,sizeof(szWhere)," (FIRST_CREATE_ROLE_TIME >='%s 00:00:00') AND (FIRST_CREATE_ROLE_TIME <= '%s 23:59:59')",today.c_str(),today.c_str());
	//新登录账号,老账号登录数
	snprintf(szWhere,sizeof(szWhere)," (CREATETIME >='%s 00:00:00') AND (CREATETIME <= '%s 23:59:59')",today.c_str(),today.c_str());
	//dbData.m_newLoginAccount =  getRecordCount(tblLoginLog.c_str(),szWhere," count(DISTINCT ACCOUNT,OP_ID) ");

	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(DISTINCT ACCOUNT,OP_ID) ",tblLoginLog.c_str(),szWhere, " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		stServerInfoStatics & dbData = serverInfos[it->first];
		dbData.m_newLoginAccount = it->second;
		dbData.m_oldLoginAccount =  dbData.m_loginAccount > dbData.m_newLoginAccount ?dbData.m_loginAccount - dbData.m_newLoginAccount : 0;
	}
	
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," (FIRST_ENTER_TIME >='%s 00:00:00') AND (FIRST_ENTER_TIME <= '%s 23:59:59')",today.c_str(),today.c_str());
	//新注册账号
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(DISTINCT ACCOUNT,OP_ID) "," ENTER_SELECT_PAGE ",szWhere, " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		stServerInfoStatics & dbData = serverInfos[it->first];
		dbData.m_newRegAccount = it->second;
		//创建角率
		dbData.m_createRoleRate = (UINT32)((dbData.m_newRegAccount > 0 ? (((dbData.m_newLoginAccount * 1.0f) / dbData.m_newRegAccount)) : 0) * 10000);
	}
	
	//角色数量
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(*) "," ROLEINFO ",NULL, " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		serverInfos[it->first].m_roleNum = it->second;
	}


	//充值账号数
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(DISTINCT ACCOUNT,OP_ID) ",tblRechargeLog.c_str(), NULL , " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		serverInfos[it->first].m_rechargeNum = it->second;
	}
	//累积充值账号数
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(DISTINCT ACCOUNT,OP_ID) "," ROLEINFO ", " RECHARGENUM > 0 " , " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		serverInfos[it->first].m_accRechargeNum = it->second;
	}
	
	//付费玩家剩余钻石
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,sum(DIAMOND) + sum(GIVEDIAMOND) as zuan "," ROLEINFO ", " RECHARGENUM > 0 " , " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		stServerInfoStatics & dbData = serverInfos[it->first];
		dbData.m_hasDiamond = it->second;
		if (dbData.m_accRechargeNum > 0){
			dbData.m_avHasDiamond = dbData.m_hasDiamond / dbData.m_accRechargeNum;
		}
	}
	
	//新充值账号数
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(DISTINCT ACCOUNT,OP_ID) ",tblRechargeLog.c_str(), " NEW_RECHARGE > 0 " , " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		serverInfos[it->first].m_newRechargeNum = it->second;
	}

	//充值保有人数
	opIDCountNum.clear();
	std::string tblLoginLogAsT1 = tblLoginLog + " t1 ";
	getRecordCountByOPID(" OP_ID,count(DISTINCT ACCOUNT,OP_ID) ",tblLoginLogAsT1.c_str(), "  (t1.CHARID IN (SELECT DISTINCT t2.CHARID FROM ROLEINFO t2 where RECHARGENUM>0))  " , " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		stServerInfoStatics & dbData = serverInfos[it->first];
		dbData.m_rechargeUsers = it->second > dbData.m_newRechargeNum ? it->second - dbData.m_newRechargeNum : 0;
		//serverInfos[it->first].m_rechargeUsers = it->second > dbData.m_newRechargeNum ? it->second - dbData.m_newRechargeNum : 0;
	}

	
	//充值次数
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,count(*) ",tblRechargeLog.c_str(),NULL , " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		serverInfos[it->first].m_rechargeTimes = it->second;
	}
	
	//充值数量
	opIDCountNum.clear();
	getRecordCountByOPID(" OP_ID,sum(RECHARGENUM) ",tblRechargeLog.c_str(),NULL , " OP_ID ",opIDCountNum);
	it = opIDCountNum.begin();
	for (;it != opIDCountNum.end();++it){
		stServerInfoStatics & dbData = serverInfos[it->first];
		dbData.m_rechargeAmount = it->second;
		dbData.m_rechargeRate = dbData.m_loginAccount > 0 ? (UINT32)(((dbData.m_rechargeNum * 1.0f)/ (dbData.m_loginAccount)) * 10000) : 0;
		dbData.m_arpu = (UINT32)((dbData.m_loginAccount > 0 ? ((dbData.m_rechargeAmount * 1.0f) / dbData.m_loginAccount) : 0) * 100) ;
		dbData.m_arppu = (UINT32)((dbData.m_rechargeNum > 0 ? ((dbData.m_rechargeAmount * 1.0f) / dbData.m_rechargeNum) : 0) * 100);

	}

	//开始更新
	bool bIsExist = false;
	bzero(szWhere,sizeof(szWhere));
	snprintf(szWhere,sizeof(szWhere)," TIME='%s' ",today.c_str()); 
	result = g_databaseEngine.select(g_serverInfoStaticsTblDef," SERVERINFO_STATICS ",szWhere,NULL);
	if (result && result->getData()){//已经存在了
		bIsExist = true;
	}
	else {
		bIsExist = false;
	}
	SAFE_DELETE(result);
	
	stServerInfoStatics & allOpStatics  = serverInfos[0];//固定写一个0的op id,然后汇总所有渠道数据,用于getServerInfo查询用

	std::map<UINT32,stServerInfoStatics>::iterator subIt = serverInfos.begin();
	for (;subIt != serverInfos.end();++subIt){
		stServerInfoStatics & dbData = subIt->second;
		if (0 != dbData.m_opID){
			allOpStatics.m_accRechargeNum += dbData.m_accRechargeNum;
			allOpStatics.m_acu = dbData.m_acu;
			allOpStatics.m_loginAccount += dbData.m_loginAccount;
			allOpStatics.m_loginDevice += dbData.m_loginDevice;
			allOpStatics.m_newLoginAccount += dbData.m_newLoginAccount;
			allOpStatics.m_newRechargeNum += dbData.m_newRechargeNum;
			allOpStatics.m_newRegAccount += dbData.m_newRegAccount;
			allOpStatics.m_oldLoginAccount += dbData.m_oldLoginAccount;
			allOpStatics.m_pcu = dbData.m_pcu;
			allOpStatics.m_rechargeAmount += dbData.m_rechargeAmount;
			allOpStatics.m_rechargeNum += dbData.m_rechargeNum;
			allOpStatics.m_rechargeTimes += dbData.m_rechargeTimes;
			allOpStatics.m_rechargeUsers += dbData.m_rechargeUsers;
			allOpStatics.m_regAccount += dbData.m_regAccount;
			allOpStatics.m_regDevice  += dbData.m_regDevice;
			allOpStatics.m_roleNum += dbData.m_roleNum;
			allOpStatics.m_hasDiamond += dbData.m_hasDiamond;
			allOpStatics.m_rechargeUsers += dbData.m_rechargeUsers;
		}
	}
	
	allOpStatics.m_arppu = (UINT32)((allOpStatics.m_rechargeNum > 0 ? (allOpStatics.m_rechargeAmount * 1.0f) / allOpStatics.m_rechargeNum : 0) * 100); 
	allOpStatics.m_arpu = (UINT32)((allOpStatics.m_loginAccount > 0 ? (allOpStatics.m_rechargeAmount * 1.0f) / allOpStatics.m_loginAccount : 0) * 100); 
	allOpStatics.m_createRoleRate = (UINT32)((allOpStatics.m_newRegAccount > 0 ? ((allOpStatics.m_newLoginAccount * 1.0f) / allOpStatics.m_newRegAccount) : 0) * 10000);
	allOpStatics.m_dt = allOpStatics.m_loginAccount > 0 ? ((allOpStatics.m_acu * 24 * 60) / allOpStatics.m_loginAccount) : 0;
	allOpStatics.m_rechargeRate = (UINT32)((allOpStatics.m_loginAccount > 0 ? (allOpStatics.m_rechargeNum * 1.0f) / allOpStatics.m_loginAccount : 0) * 10000) ;
	if (allOpStatics.m_accRechargeNum > 0){
		allOpStatics.m_avHasDiamond = allOpStatics.m_hasDiamond / allOpStatics.m_accRechargeNum;
	}

	bool bNeedWriteCache = false;
	if (m_refreshServerStaticNum >= s_MAX_WRITE_SERVER_STATIC_CACHE){
		bNeedWriteCache = true;	
		m_refreshServerStaticNum = 0;
	}
	subIt = serverInfos.begin();
	for (;subIt != serverInfos.end();++subIt){
		stServerInfoStatics & dbData = subIt->second;
		dbData.m_opID = subIt->first;
		bzero(szWhere,sizeof(szWhere));
		snprintf(szWhere,sizeof(szWhere)," TIME='%s' AND OP_ID = %d ",today.c_str(),subIt->first);
        Athena::logger->trace("[GameStaticsMgr] in for : szWhere is %s", szWhere);
		if (bIsExist){
			strncpy(dbData.m_date,today.c_str(),sizeof(dbData.m_date));
			g_databaseEngine.updateRecord(g_serverInfoStaticsTblDef,(const char *)&dbData,"SERVERINFO_STATICS",szWhere,NULL);		
            Athena::logger->trace("[GameStaticsMgr] update SERVERINFO_STATICS record, date is %s, szWhere is %s", today.c_str(), szWhere);
		}
		else {
			UINT64 autoID = 0;
			strncpy(dbData.m_date,today.c_str(),sizeof(dbData.m_date));
			g_databaseEngine.insertRecord(g_serverInfoStaticsTblDef,(const char *)&dbData,"SERVERINFO_STATICS",autoID);
            Athena::logger->trace("[GameStaticsMgr] insert SERVERINFO_STATICS record, autoid is %d, date is %s", autoID, today.c_str());
		}
		
		if (bNeedWriteCache){
			UINT64 autoID = 0;
			std::string curDateTime = g_utc2StringDate(g_webServer->getSec(),"%Y-%m-%d %H:%M:%S");
			strncpy(dbData.m_date,curDateTime.c_str(),sizeof(dbData.m_date));
			g_databaseEngine.insertRecord(g_serverInfoStaticsTblDef,(const char *)&dbData,"SERVERINFO_STATICS_CACHE",autoID);
            Athena::logger->trace("[GameStaticsMgr] insert SERVERINFO_STATICS_CACHE record, autoid is %d, cur date is %s", autoID, curDateTime.c_str());
		}
	}
	SAFE_DELETE(result);
}

std::string CGameStaticsMgr::getCommonQuery(Json::Value & searchCond)
{
	std::string type;
	if (searchCond["search"].isObject()){
		Json::Value  search = searchCond["search"];

		if (search["type"].isString()){
			type = search["type"].asString();	
		}
		if (type == "realTimeData"){
			return getIncAccountPerHour(searchCond,search);	
		}

	}
	return "";	
}

std::string CGameStaticsMgr::getIncAccountPerHour(Json::Value & searchCond2,Json::Value & searchCond1)
{
	std::string start;
	std::string end;
	
	std::ostringstream os;
	bool addCond  = false;

	if (searchCond1["starttime"].isString()){
		start = searchCond1["starttime"].asString();
		if (start.size() > 0){
			addCond = true;
			os << " DATE_FORMAT(TIME,\"%Y-%m-%d\") = '" << start <<"'";
		}
		else {
			os << " AND DATE_FORMAT(TIME,\"%Y-%m-%d\") = '" << start <<"'";
		}
	}
	
	std::string opID;	
	if (searchCond2["op_id"].isString()){
		opID = searchCond2["op_id"].asString();
		if (opID.size() > 0){
			
		}
		else {
			opID = "0";
		}

		if (!addCond){
			addCond = true;
			os << " OP_ID = " << opID;
		}
		else {
			os << " AND  OP_ID =  " << opID;
		}
	}


	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	Json::Value fields;
	Json::Value dataList;
	Json::Value countFields;
	Json::Value extra;

	countFields.append("02_regDevice");
	countFields.append("12_pcu");
	countFields.append("03_loginDevice");

	Json::Value field;
	fields["00_opID"] = "渠道";
	fields["01_timer"] = "日期" ;
	fields["02_regDevice"] = "进入设备数量";
	fields["03_loginDevice"] = "登陆设备数";
	fields["04_regAccount"] = "进入账号数";
	fields["05_loginAccount"] = "登录账号数";
	fields["06_oldLoginAccount"] = "老账号登录数";
	fields["07_newLoginAccount"] = "新账号登录数";
	fields["08_newRegAccount"] = "新进入账号数量";
	fields["09_roleNum"] = "创建角色数量";
	fields["10_createRoleRate"] = "创角率";
	if (opID == "0"){
		fields["11_acu"] = "平均在线";
		fields["12_pcu"] = "最高在线";
		fields["13_dt"] = "dt时间";
	}
	fields["14_rechargeNum"] = "充值账号数";
	fields["15_accRechargeNum"] = "累计充值账号";
	fields["16_newRechargeNum"] = "新增充值账号";
	fields["17_rechargeTimes"] = "充值次数";
	fields["18_rechargeAmount"] = "充值总额";
	fields["19_rechargeRate"] = "付费率";
	fields["20_arpu"] = "arpu值";
	fields["21_arppu"] = "arppu值";
	fields["22_hasDiamond"] = "付费玩家用户钻石";
	fields["23_avHasDiamond"] = "付费玩家平均钻石";
	fields["24_rechargeUsers"] = "充值保有人数";

	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_serverInfoStaticsTblDef," SERVERINFO_STATICS_CACHE ",os.str().c_str(),NULL);

	if (result && result->getData()){
		Json::Value info;
		stServerInfoStatics * pData = (stServerInfoStatics *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			info["00_opID"] = COpIDMgr::getSingleton().getOpIDName(pData[i].m_opID);
			info["01_timer"] =  pData[i].m_date ;
			info["02_regDevice"] = pData[i].m_regDevice;
			info["03_loginDevice"] =  pData[i].m_loginDevice;
			info["04_regAccount"] = pData[i].m_regAccount;
			info["05_loginAccount"] = pData[i].m_loginAccount;
			info["06_oldLoginAccount"] = pData[i].m_oldLoginAccount;
			info["07_newLoginAccount"] = pData[i].m_newLoginAccount;
			info["08_newRegAccount"] = pData[i].m_newRegAccount;
			info["09_roleNum"] = pData[i].m_roleNum;
			std::string createRoleRateStr = g_float2string(pData[i].m_createRoleRate / 100.0f);
			createRoleRateStr += "%";
			info["10_createRoleRate"] = createRoleRateStr;
			if (opID == "0"){
				info["11_acu"] = pData[i].m_acu;
				info["12_pcu"] = pData[i].m_pcu;
				info["13_dt"] = pData[i].m_dt;
			}
			info["14_rechargeNum"] = pData[i].m_rechargeNum;
			info["15_accRechargeNum"] = pData[i].m_accRechargeNum;
			info["16_newRechargeNum"] = pData[i].m_newRechargeNum;
			info["17_rechargeTimes"] = pData[i].m_rechargeTimes;
			info["18_rechargeAmount"] = pData[i].m_rechargeAmount;
			std::string rechargeRate = g_float2string(pData[i].m_rechargeRate / 100.0f);
			rechargeRate += "%";
			info["19_rechargeRate"] = rechargeRate;
			info["20_arpu"] = g_float2string(pData[i].m_arpu / 100.0f);
			info["21_arppu"] = g_float2string(pData[i].m_arppu / 100.0f);
			info["22_hasDiamond"] = pData[i].m_hasDiamond;
			info["23_avHasDiamond"] = pData[i].m_avHasDiamond;
			info["24_rechargeUsers"] = pData[i].m_rechargeUsers;
			dataList.append(info);
		}
	}

	SAFE_DELETE(result);
	
	Json::Value subExtra1;
	Json::Value subExtra2;
	Json::Value subExtra3;
	subExtra1.append("开服天数");
	result = g_databaseEngine.select(g_openServerDayNumDef," OPEN_SERVER_INFO ",NULL,NULL);
	if (result && result->getData() && result->getRowCount()){
		UINT32 dayNum = *(UINT32 *)(result->getData());
		subExtra1.append(dayNum);
	}
	else {
		subExtra1.append(0);
	}
	SAFE_DELETE(result);
	extra.append(subExtra1);

	data["fields"] = fields;
	data["list"] = dataList;
	data["countfields"] = countFields;
	data["extra"] = extra;
	root["data"] = data;
	std::string json_file = writer.write(root);
	return json_file;
}
