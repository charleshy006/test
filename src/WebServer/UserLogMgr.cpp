#include "UserLogMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "WebServer.h"
#include "json/writer.h"
#include "json/reader.h"
#include "TblDataMgr.h"
#include "datacenter_log.h"
#include "log_singleton.h"
#include <ostream>

extern WebServer * g_webServer;

CTblField g_userLevelupLogTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      	CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",      		CTblField::DB_TYPE_STRING,    33             },
	{ "OLDLV",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "NEWLV",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "OLDEXP",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NEWEXP",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DOTIME",    		CTblField::DB_TYPE_STRING,    20             },
	{ NULL, 0, 0 },
};

 CTblField g_userLoginLogTblDef[] = {
 	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "OP_ID",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ADVUSER",      CTblField::DB_TYPE_UINT8,  sizeof(UINT8)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",      	  CTblField::DB_TYPE_STRING,    33             },
	{ "LOGINIP",	  CTblField::DB_TYPE_STRING,  	40		   	   },
	{ "DOTIME",    	  CTblField::DB_TYPE_STRING,    20             },
	{ "CREATETIME",   CTblField::DB_TYPE_STRING,    20             },
	{ "DEVICE_ID",    CTblField::DB_TYPE_STRING,   125             },
 	{ NULL, 0, 0 },
 };

CTblField g_userRechargeLogTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",      	  CTblField::DB_TYPE_STRING,    33             },
	{ "TID",      	  CTblField::DB_TYPE_STRING,    50             },
	{ "RECHARGENUM",  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NOWDIAMOND",   CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DOTIME", 	  CTblField::DB_TYPE_STRING,    20             },
	{ "NEW_RECHARGE", CTblField::DB_TYPE_UINT8,    sizeof(UINT8)   },
	{ "OP_ID", 		  CTblField::DB_TYPE_UINT32,   sizeof(UINT32)  },
	{ "ADDDIAMOND",	  CTblField::DB_TYPE_UINT32,   sizeof(UINT32)  },
	{ "VIPLV",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "ITEMID",       CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

CTblField g_userVipLevelupLogTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",      	  CTblField::DB_TYPE_STRING,    33             },
	{ "OLDEXP", 	  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NEWEXP", 	  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ADDDAIMOND",   CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DOTIME",  	 CTblField::DB_TYPE_STRING,    20             },
	{ NULL, 0, 0 },
};

CTblField g_userHonourChgLogTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",         CTblField::DB_TYPE_STRING,    33             },
	{ "OLDHONOUR",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NEWHONOUR",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "REASON",       CTblField::DB_TYPE_STRING,   50              },
	{ "DOTIME",      CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_itemChgLogTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",         CTblField::DB_TYPE_STRING,    33             },
	{ "ITEMNAME",     CTblField::DB_TYPE_STRING,  33               },
	{ "NUM",      	  CTblField::DB_TYPE_UINT32, sizeof(UINT32)    },
	{ "REASON",       CTblField::DB_TYPE_STRING,  50               },
	{ "LEVEL",        CTblField::DB_TYPE_UINT16, sizeof(UINT16)    },
	{ "VIPLEVEL",     CTblField::DB_TYPE_UINT16, sizeof(UINT16)    },
	{ "ISADD",        CTblField::DB_TYPE_UINT8, sizeof(UINT8)      },
	{ "ITEM_TYPE",    CTblField::DB_TYPE_UINT16, sizeof(UINT16)    },
	{ "ITEM_QUALITY", CTblField::DB_TYPE_UINT16, sizeof(UINT16)    },
	{ "ITEM_RAISELV", CTblField::DB_TYPE_UINT16, sizeof(UINT16)    },
	{ "ITEM_REFINELV",CTblField::DB_TYPE_UINT16, sizeof(UINT16)    },
	{ "DOTIME",      CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_resChgLogTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",         CTblField::DB_TYPE_STRING,    33             },
	{ "RESNAME",      CTblField::DB_TYPE_STRING,  33               },
	{ "ADDNUM",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "OLDVALUE",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NEWVALUE",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "REASON",       CTblField::DB_TYPE_STRING,  50               },
	{ "DOTIME",      CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_dailyTaskLogTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",         CTblField::DB_TYPE_STRING,    33             },
	{ "TASKID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "TASKNAME",     CTblField::DB_TYPE_STRING,  33               },
	{ "DOTIME",      CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_rowNumTblDef[] = {
	{" COUNT(*) " , CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_exchangeListTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",         CTblField::DB_TYPE_STRING,    33             },
	{ "FLAG",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NUM",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "MODULE_NAME",  CTblField::DB_TYPE_STRING,  	33             },
	{ "DOTIME",       CTblField::DB_TYPE_STRING,    20             },
	{ NULL, 0, 0 },
};

CTblField g_mallBuyLogTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",         CTblField::DB_TYPE_STRING,    33             },
	{ "LEVEL",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "VIPLEVEL",     CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "SHOPID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "RESID",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "RESNUM",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ITEMID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ITEMNUM",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DOTIME",       CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_heroPropChgLogTblDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACCOUNT",      CTblField::DB_TYPE_STRING,    49             },
	{ "NAME",         CTblField::DB_TYPE_STRING,    33             },
	{ "LEVEL",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "VIPLEVEL",     CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "HEROID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACTION",       CTblField::DB_TYPE_STRING,    10             },
	{ "OLDLV",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "NEWLV",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "OLDEVOLV",     CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "NEWEVOLV",     CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "DOTIME",       CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_enterSelectPageTblDef[] = {
	{ "ACCOUNT",      			CTblField::DB_TYPE_STRING,    49             },
	{ "FIRST_ENTER_TIME",       CTblField::DB_TYPE_STRING,   20              },
	{ "FIRST_CREATE_ROLE_TIME", CTblField::DB_TYPE_STRING,   20              },
	{ "LAST_ENTER_TIME",        CTblField::DB_TYPE_STRING,   20              },
	{ "OP_ID",                  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DEVICE_ID",              CTblField::DB_TYPE_STRING,   125             },
	{ NULL, 0, 0 },
};

CTblField g_enterSelectPageLogTblDef[] = {
	{ "ACCOUNT",      			CTblField::DB_TYPE_STRING,   49              },
	{ "FIRST_ENTER_TIME",       CTblField::DB_TYPE_STRING,   20              },
	{ "OP_ID",                  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DEVICE_ID",              CTblField::DB_TYPE_STRING,   125             },
	{ "DOTIME",        			CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_guildLogTblDef[] = {
	{ "ACCOUNT",                CTblField::DB_TYPE_STRING,   49              },
	{ "OP_ID",                  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CHARID",                 CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",                 	CTblField::DB_TYPE_STRING,   33              },
	{ "LEVEL",                  CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "VIPLV",                  CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "GUILDID",                CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACTION",              	CTblField::DB_TYPE_STRING,   20              },
	{ "OP_OBJ",              	CTblField::DB_TYPE_STRING,   20              },
	{ "DOTIME",              	CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};


CTblField g_guildResLogTblDef[] = {
	{ "GUILDID",                CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "RES",                 	CTblField::DB_TYPE_STRING,      20           },
	{ "OLDVAL",                 CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NEWVAL",                 CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "REASON",                 CTblField::DB_TYPE_STRING,   20              },
	{ "DOTIME",                 CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_moduleLogTblDef[] = {
	{ "ACCOUNT",                CTblField::DB_TYPE_STRING,   49              },
	{ "OP_ID",                  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CHARID",                 CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",      	  			CTblField::DB_TYPE_STRING,    33             },
	{ "LEVEL",                  CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "VIPLV",                  CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "MODULE",                 CTblField::DB_TYPE_STRING,   20              },
	{ "COPYMAPID",              CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "OTHERCHARID",            CTblField::DB_TYPE_STRING,   50              },
	{ "RESULT",                 CTblField::DB_TYPE_UINT8,  sizeof(UINT8)     },	
	{ "OLDRANK",                CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NEWRANK",                CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DOTIME",                 CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 }, 
};

CTblField g_createRoleTblDef[] = {
	{ "ACCOUNT",      			CTblField::DB_TYPE_STRING,   49              },
	{ "OP_ID",                  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DEVICE_ID",              CTblField::DB_TYPE_STRING,   125             },
	{ "DOTIME",        			CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_skillChgTblDef[] = {
	{ "SKILLID",                CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "POS",                    CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "OLDVAL",                 CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "NEWVAL",                 CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "ACTION",        			CTblField::DB_TYPE_STRING,   20              },
	{ "DOTIME",        			CTblField::DB_TYPE_STRING,   20              },
	{ "DECGOILD",               CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ NULL, 0, 0 },
};

CTblField g_spriteTblDef[] = {
	{ "SPRITEID",               CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "OLDVAL",                 CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "NEWVAL",                 CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "ACTION",        			CTblField::DB_TYPE_STRING,   20              },
	{ "DOTIME",        			CTblField::DB_TYPE_STRING,   20              },
	{ "CONSUME",                CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ NULL, 0, 0 },
};

CTblField g_copymapTblDef[] = {
	{ "COPYMAPID",               CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "WIN",                     CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "ACTION",        			 CTblField::DB_TYPE_STRING,   20              },
	{ "DOTIME",        			 CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

CTblField g_equipChgTblDef[] = {
	{ "ITEMID",              CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "OLDRAISELV",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "NEWRAISELV",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "OLDREFINELV",         CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "NEWREFINELV",         CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "ACTION",        		 CTblField::DB_TYPE_STRING,   20              },
	{ "DOTIME",        		 CTblField::DB_TYPE_STRING,   20              },
	{ "DECGOLDCOIN",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DECYUNTIE",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DECITEM",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ADDGOLDCOIN",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ADDYUNTIE",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ADDITEM",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ NULL, 0, 0 },
};

CTblField g_activityLogTblDef[] = {
	{"CHARID" ,           CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"ACTIVITY_ID" ,      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"ACTIVITY_NAME",     CTblField::DB_TYPE_STRING,  50             },
	{"STEP_ID" ,          CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{"STEP_DESC",         CTblField::DB_TYPE_STRING,  50             },
	{ "DOTIME",        	  CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

void UserLogMgr::addUserLevelupLog(stUserLevelupLogDB & data)
{
	std::string realTblName = getRealTblName("USER_LEVELUP_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_userLevelupLogTblDef,(const char*)&data,realTblName.c_str(),autoID);	
}

void UserLogMgr::addLoginLog(stUserLoginLogDB & data)
{
	EnterSelectPageCache_IT it = m_enterSelectPageCache.find(data.m_account);
	if (it == m_enterSelectPageCache.end()){
		Athena::logger->error("进入选择界面表还没有该玩家数据,打印登录日志不成功!");
		return ;
	}

	if (strncmp(it->second.m_createRoleTime,"0000-00-00 00:00:00",sizeof(it->second.m_createRoleTime)) == 0){
		Athena::logger->error("打印登录日志时候没有玩家创建角色时间!");
	}
	else {
		bzero(data.m_createTime,sizeof(data.m_createTime));
		strncpy(data.m_createTime,it->second.m_createRoleTime,sizeof(data.m_createTime));	
	}
	std::string realTblName = getRealTblName("USER_LOGIN_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_userLoginLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addRechargeLog(stUserRechargeLogDB & data)
{
	std::string realTblName = getRealTblName("USER_RECHARGE_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_userRechargeLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addVipLevelupLog(stUserVipLevelupLogDB & data)
{
	std::string realTblName = getRealTblName("USER_VIPLEVELUP_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_userVipLevelupLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addHonourChgLog(stHonourChgLogDB & data)
{
	std::string realTblName = getRealTblName("HONOUR_CHG_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_userHonourChgLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addItemChgLog(stItemChgLogDB & data)
{
	std::string realTblName = getRealTblName("ITEM_CHG_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_itemChgLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addResChgLog(stResChgLogDB & data)
{
	std::string realTblName = getRealTblName("RES_CHG_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_resChgLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addDailyTaskLog(stDailyTaskLogDB & data)
{
	std::string realTblName = getRealTblName("DAILYTASK_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_dailyTaskLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addExchangeLog(stExchangeListDB & data)
{
	std::string realTblName = getRealTblName("EXCHANGE_LIST");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_exchangeListTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addMallBuyLog(stMallBuyLogDB & data)
{
	std::string realTblName = getRealTblName("MALL_BUY_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_mallBuyLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addHeroPropChgLog(stHeroPropChgLogDB & data)
{
	std::string realTblName = getRealTblName("HERO_PROP_CHG_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_heroPropChgLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

CTblField g_espLastEnterTblDef[] = {
	{ "LAST_ENTER_TIME",        CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};

void UserLogMgr::addEnterSelectPageLog(stEnterSelectPageLogDB & data)
{
	std::string realTblName = getRealTblName("ENTER_SELECT_PAGE_LOG");
	
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ACCOUNT='%s'",data.m_account);
	
	EnterSelectPageCache_IT it = m_enterSelectPageCache.find(data.m_account); 
	//CTblQueryResult* result = NULL;
	//result = g_databaseEngine.select(g_enterSelectPageTblDef,"ENTER_SELECT_PAGE",where,NULL);

	if (it == m_enterSelectPageCache.end()){//第一次进入
		stEnterSelectPageDB dbData;
		dbData.m_opID = data.m_opID;
		strncpy(dbData.m_account,data.m_account,sizeof(dbData.m_account));
		strncpy(dbData.m_deviceID,data.m_deviceID,sizeof(dbData.m_deviceID));
		std::string strFirstEnterTime = g_utc2StringFormat(g_webServer->getSec());
		strncpy(dbData.m_firstEnterTime,strFirstEnterTime.c_str(),sizeof(dbData.m_firstEnterTime));
		strncpy(dbData.m_lastEnterTime,strFirstEnterTime.c_str(),sizeof(dbData.m_lastEnterTime));
		strncpy(data.m_firstEnterTime,strFirstEnterTime.c_str(),sizeof(data.m_firstEnterTime));
		UINT64 autoID = 0;
		g_databaseEngine.insertRecord(g_enterSelectPageTblDef,(const char *)&dbData,"ENTER_SELECT_PAGE",autoID);

		stEnterSelectPage  cache;
		strncpy(cache.m_firstEnterTime,strFirstEnterTime.c_str(),sizeof(cache.m_firstEnterTime));
		strncpy(cache.m_createRoleTime,"0000-00-00 00:00:00",sizeof(cache.m_createRoleTime));
		m_enterSelectPageCache[data.m_account] = cache;
		
		/*--------------- 打生态日志 ----------------*/
		uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
		uuzu::logsdk::UserInfo userInfo;
		userInfo.level = 1;
		userInfo.passport = data.m_account; //必须填写
		userInfo.exp = 0;
		userInfo.viplevel = 0;
		userInfo.blackcoin = 0;
		userInfo.gamecoin = 0;
		userInfo.moneycoin = 0;
		userInfo.device = data.m_deviceID;
		//userInfo.clientip = data.m_ip;
		userInfo.firstintime = g_webServer->getSec();
		userInfo.version = Athena::global["gameVersion"];
		std::string opID = g_int2string(data.m_opID); 
		logger.setPID(opID.c_str()); 
		logger.sendDataLog(userInfo,"", "login", "","","","","","","est");
		/*-------------------------------------------*/
	}
	else {
		std::string lastEnterTime = g_utc2StringFormat(g_webServer->getSec());
		char szLastEnterTime[20] = { 0 };
		strncpy(szLastEnterTime,lastEnterTime.c_str(),sizeof(szLastEnterTime));
		//stEnterSelectPageDB * pData = (stEnterSelectPageDB *)result->getData();
		strncpy(data.m_firstEnterTime,it->second.m_firstEnterTime,sizeof(data.m_firstEnterTime));
		g_databaseEngine.updateRecord(g_espLastEnterTblDef,(const char *)szLastEnterTime,"ENTER_SELECT_PAGE",where,NULL);		
	}
	//SAFE_DELETE(result);

	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_enterSelectPageLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addGuildLog(stGuildLogDB & data)
{
	std::string realTblName = getRealTblName("GUILD_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guildLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addGuildResLog(stGuildResLogDB  & data)
{
	std::string realTblName = getRealTblName("GUILD_RES_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guildResLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addModuleLog(stModuleLogDB & data)
{
	std::string realTblName = getRealTblName("MODULE_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_moduleLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addCreateRoleLog(stCreateRoleLogDB & data)
{
	std::string realTblName = getRealTblName("CREATE_ROLE_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_createRoleTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addSkillChgLog(stSkillChgLogDB & data)
{
	std::string realTblName = getRealTblName("SKILL_CHG_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_skillChgTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addSpriteLog(stSpriteLogDB & data)
{
	std::string realTblName = getRealTblName("SPRITE_CHG_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_spriteTblDef,(const char *)&data,realTblName.c_str(),autoID);
}
	
void UserLogMgr::addCopymapLog(stCopymapLogDB & data)
{
	std::string realTblName = getRealTblName("COPYMAP_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_copymapTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addEquipChgLog(stEquipChgLogDB & data)
{
	std::string realTblName = getRealTblName("EQUIP_CHG_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_equipChgTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::addActivityLog(stActivityLogDB & data)
{
	std::string realTblName = getRealTblName("ACTIVITY_LOG");
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_activityLogTblDef,(const char *)&data,realTblName.c_str(),autoID);
}

void UserLogMgr::test()
{
	stUserLevelupLogDB data1;
	data1.m_charID = 1;
	strncpy(data1.m_account,"test1",sizeof(data1.m_account));
	strncpy(data1.m_name,"zhangshan",sizeof(data1.m_name));
	data1.m_newExp = 2;
	data1.m_newLv = 2;
	data1.m_oldExp = 1;
	data1.m_oldLv = 1;
	strncpy(data1.m_levelupTime,g_utc2StringFormat(time(NULL)).c_str(),sizeof(data1.m_levelupTime));
	addUserLevelupLog(data1);
	
	stUserLoginLogDB data2;
	data2.m_charID = 2;
	strncpy(data2.m_account,"test1",sizeof(data2.m_account));
	strncpy(data2.m_name,"zhangshan",sizeof(data2.m_name));
	snprintf(data2.m_loginIP,sizeof(data2.m_loginIP),"120.130.10.1");
	strncpy(data2.m_loginTime,g_utc2StringFormat(time(NULL)).c_str(),sizeof(data2.m_loginTime));
	addLoginLog(data2);
	
	stUserRechargeLogDB data3;
	data3.m_charID = 3;
	strncpy(data3.m_account,"test1",sizeof(data3.m_account));
	strncpy(data3.m_name,"zhangshan",sizeof(data3.m_name));
	data3.m_nowDiamond = 10;
	data3.m_rechargeNum = 10;
	snprintf(data3.m_tid,sizeof(data3.m_tid),"asdfsafd21232q4");
	strncpy(data3.m_rechargeTime,g_utc2StringFormat(time(NULL)).c_str(),sizeof(data3.m_rechargeTime));
	addRechargeLog(data3);
	
	stUserVipLevelupLogDB data4;
	data4.m_charID = 4;
	strncpy(data4.m_account,"test1",sizeof(data4.m_account));
	strncpy(data4.m_name,"zhangshan",sizeof(data4.m_name));
	data4.m_addDiamond = 10;
	data4.m_newExp = 10;
	data4.m_oldExp = 9;
	strncpy(data4.m_levelupTime,g_utc2StringFormat(time(NULL)).c_str(),sizeof(data4.m_levelupTime));
	addVipLevelupLog(data4);
	
	stHonourChgLogDB data5;
	data5.m_charID = 5;
	strncpy(data5.m_account,"test1",sizeof(data5.m_account));
	strncpy(data5.m_name,"zhangshan",sizeof(data5.m_name));
	data5.m_newHonour =6;
	data5.m_oldHonour = 5;
	snprintf(data5.m_reason,sizeof(data5.m_reason),"test");
	strncpy(data5.m_chgTime,g_utc2StringFormat(time(NULL)).c_str(),sizeof(data5.m_chgTime));
	addHonourChgLog(data5);
	
	stItemChgLogDB data6;
	data6.m_charID = 6;
	strncpy(data6.m_account,"test1",sizeof(data6.m_account));
	strncpy(data6.m_name,"zhangshan",sizeof(data6.m_name));
	snprintf(data6.m_name,sizeof(data6.m_name),"道具名字");
	snprintf(data6.m_reason,sizeof(data6.m_reason),"强化");
	strncpy(data6.m_chgTime,g_utc2StringFormat(time(NULL)).c_str(),sizeof(data5.m_chgTime));
	addItemChgLog(data6);
	
	stResChgLogDB data7;
	data7.m_addNum = 10;
	data7.m_charID = 6;
	strncpy(data7.m_account,"test1",sizeof(data7.m_account));
	strncpy(data7.m_name,"zhangshan",sizeof(data7.m_name));
	data7.m_newValue = 8;
	data7.m_oldValue = 7;
	snprintf(data7.m_resName,sizeof(data7.m_resName),"资源名字");
	snprintf(data7.m_reason,sizeof(data7.m_reason),"升级");
	strncpy(data7.m_chgTime,g_utc2StringFormat(time(NULL)).c_str(),sizeof(data7.m_chgTime));
	addResChgLog(data7);
	
	stDailyTaskLogDB data8;
	data8.m_charID = 8;
	strncpy(data8.m_account,"test1",sizeof(data8.m_account));
	strncpy(data8.m_name,"zhangshan",sizeof(data8.m_name));
	data8.m_taskID = 8;
	snprintf(data8.m_taskName,sizeof(data8.m_taskName),"任务名字");
	strncpy(data8.m_doTime,g_utc2StringFormat(time(NULL)).c_str(),sizeof(data8.m_doTime));
	addDailyTaskLog(data8);
	
}

UserLogMgr::UserLogMgr() 
{
	stLogDesc & log1 = m_maxLogDesc[0];
	log1.m_id = eLog_Type_Levelup;
	strncpy(log1.m_desc,"玩家升级日志",sizeof(log1.m_desc));

	stLogDesc & log2 = m_maxLogDesc[1];
	log2.m_id = eLog_Type_Login;
	strncpy(log2.m_desc,"玩家登录日志",sizeof(log2.m_desc));
	
	stLogDesc & log3 = m_maxLogDesc[2];
	log3.m_id = eLog_Type_Recharge;
	strncpy(log3.m_desc,"充值日志",sizeof(log3.m_desc));
	
	stLogDesc & log4 = m_maxLogDesc[3];
	log4.m_id = eLog_Type_VipLevelup;
	strncpy(log4.m_desc,"vip充值日志",sizeof(log4.m_desc));

	stLogDesc & log5 = m_maxLogDesc[4];
	log5.m_id = eLog_Type_HonourChg;
	strncpy(log5.m_desc,"荣誉变化日志",sizeof(log5.m_desc));

	stLogDesc & log6 = m_maxLogDesc[5];
	log6.m_id = eLog_Type_ItemChg;
	strncpy(log6.m_desc,"道具变化日志",sizeof(log6.m_desc));
	
	stLogDesc & log7 = m_maxLogDesc[6];
	log7.m_id = eLog_Type_ResChg;
	strncpy(log7.m_desc,"资源变化日志",sizeof(log7.m_desc));
	
	stLogDesc & log8 = m_maxLogDesc[7];
	log8.m_id = eLog_Type_DailyTask;
	strncpy(log8.m_desc,"日常任务日志",sizeof(log8.m_desc));

	stLogDesc & log9 = m_maxLogDesc[8];
	log9.m_id = eLog_Type_MallBuy;
	strncpy(log9.m_desc,"商店购买日志",sizeof(log9.m_desc));
	
	stLogDesc & log10 = m_maxLogDesc[9];
	log10.m_id = eLog_Type_HeroPropChg;
	strncpy(log10.m_desc,"宠物属性变化",sizeof(log10.m_desc));
	

	stLogDesc & log11 = m_maxLogDesc[10];
	log11.m_id = eLog_Type_Guild;
	strncpy(log11.m_desc,"公会日志",sizeof(log11.m_desc));
	
	
	stLogDesc & log12 = m_maxLogDesc[11];
	log12.m_id = eLog_Type_Module;
	strncpy(log12.m_desc,"模块日志",sizeof(log12.m_desc));
	
	stLogDesc & log13 = m_maxLogDesc[12];
	log13.m_id = eLog_Type_SkillChg;
	strncpy(log13.m_desc,"技能日志",sizeof(log13.m_desc));
	
	stLogDesc & log14 = m_maxLogDesc[13];
	log14.m_id = eLog_Type_Sprite;
	strncpy(log14.m_desc,"技能日志",sizeof(log14.m_desc));
	
	stLogDesc & log15 = m_maxLogDesc[14];
	log15.m_id = eLog_Type_Copymap;
	strncpy(log15.m_desc,"副本日志",sizeof(log15.m_desc));

	stLogDesc & log16 = m_maxLogDesc[15];
	log16.m_id = eLog_Type_Equip;
	strncpy(log16.m_desc,"装备日志",sizeof(log16.m_desc));

	stLogDesc & log17 = m_maxLogDesc[16];
	log17.m_id = eLog_Type_Activity;
	strncpy(log17.m_desc,"活动日志",sizeof(log17.m_desc));

	m_logTbls.insert(std::make_pair("USER_LEVELUP_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("USER_LOGIN_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("USER_RECHARGE_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("USER_VIPLEVELUP_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("HONOUR_CHG_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("ITEM_CHG_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("RES_CHG_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("DAILYTASK_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("EXCHANGE_LIST",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("MALL_BUY_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("HERO_PROP_CHG_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("ENTER_SELECT_PAGE_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("GUILD_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("GUILD_RES_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("MODULE_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("SKILL_CHG_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("SPRITE_CHG_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("COPYMAP_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("EQUIP_CHG_LOG",std::set<std::string>()));
	m_logTbls.insert(std::make_pair("ACTIVITY_LOG",std::set<std::string>()));

	char createSql[1024] = { 0 };
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `USER_LEVELUP_LOG` (\
			`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
			`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
			`ACCOUNT` varchar(49) NOT NULL DEFAULT '', \
			`NAME` varchar(33) NOT NULL DEFAULT '',\
			`OLDLV`  smallint unsigned NOT NULL DEFAULT '0',\
			`NEWLV`  smallint unsigned NOT NULL DEFAULT '0',\
			`OLDEXP` int(10) unsigned NOT NULL DEFAULT '0',\
			`NEWEXP` int(10) unsigned NOT NULL DEFAULT '0',\
			`DOTIME` datetime NOT NULL,\
			PRIMARY KEY (`ID`)\
			) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");	
	m_createSqls.insert(std::make_pair("USER_LEVELUP_LOG",createSql));	
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `USER_LOGIN_LOG` ( \
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`OP_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ADVUSER`  tinyint unsigned NOT NULL DEFAULT '0',\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`NAME` varchar(33) NOT NULL DEFAULT '', \
		`LOGINIP` varchar(40) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		`CREATETIME`    datetime NOT NULL,\
		`DEVICE_ID` varchar(125) NOT NULL DEFAULT '',\
		 PRIMARY KEY (`ID`)\
		) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("USER_LOGIN_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `USER_RECHARGE_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`TID` varchar(50) NOT NULL DEFAULT '',\
		`RECHARGENUM` int(10) unsigned NOT NULL DEFAULT '0',\
		`NOWDIAMOND`  int(10) unsigned NOT NULL DEFAULT '0',\
		`DOTIME` datetime NOT NULL, \
		`NEW_RECHARGE` tinyint unsigned NOT NULL DEFAULT '0',\
		`OP_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ADDDIAMOND`  int(10) unsigned NOT NULL DEFAULT '0',\
		`VIPLV`  smallint unsigned NOT NULL DEFAULT '0',\
		`ITEMID` smallint unsigned NOT NULL DEFAULT '0',\
		PRIMARY KEY (`ID`))\
		ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("USER_RECHARGE_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `USER_VIPLEVELUP_LOG`\
			(`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
			 `CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
			 `ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
			 `NAME` varchar(33) NOT NULL DEFAULT '',\
			 `OLDEXP` int(10) unsigned NOT NULL DEFAULT '0',\
			 `NEWEXP` int(10) unsigned NOT NULL DEFAULT '0',\
			 `ADDDAIMOND` int(10) unsigned NOT NULL DEFAULT '0',\
			 `DOTIME` datetime NOT NULL,PRIMARY KEY (`ID`))\
			ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("USER_VIPLEVELUP_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `HONOUR_CHG_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`OLDHONOUR` int(10) unsigned NOT NULL DEFAULT '0',\
		`NEWHONOUR` int(10) unsigned NOT NULL DEFAULT '0', \
		`REASON` varchar(50) NOT NULL DEFAULT '',  \
		`DOTIME` datetime NOT NULL, \
		PRIMARY KEY (`ID`))ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("HONOUR_CHG_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `ITEM_CHG_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '', \
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`ITEMNAME` varchar(33) NOT NULL DEFAULT '',\
		`NUM` int(10) unsigned NOT NULL DEFAULT '0',\
		`REASON` varchar(50) NOT NULL DEFAULT '',\
		`LEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`VIPLEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`ISADD`  tinyint unsigned NOT NULL DEFAULT '0',\
		`ITEM_TYPE`  smallint unsigned NOT NULL DEFAULT '0',\
		`ITEM_QUALITY`  smallint unsigned NOT NULL DEFAULT '0',\
		`ITEM_RAISELV`  smallint unsigned NOT NULL DEFAULT '0',\
		`ITEM_REFINELV` smallint unsigned NOT NULL DEFAULT '0',\
		 `DOTIME` datetime NOT NULL, \
		 PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("ITEM_CHG_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `RES_CHG_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '', \
		`NAME` varchar(33) NOT NULL DEFAULT '', \
		`RESNAME` varchar(33) NOT NULL DEFAULT '',\
		`ADDNUM`  int(10) unsigned NOT NULL DEFAULT '0',\
		`OLDVALUE` int(10) unsigned NOT NULL DEFAULT '0',\
		`NEWVALUE` int(10) unsigned NOT NULL DEFAULT '0',\
		`REASON` varchar(50) NOT NULL DEFAULT '', \
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("RES_CHG_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `DAILYTASK_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`TASKID` int(10) unsigned NOT NULL DEFAULT '0', \
		`TASKNAME` varchar(33) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL, \
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("DAILYTASK_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `EXCHANGE_LIST`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		 `CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		 `ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		 `NAME` varchar(33) NOT NULL DEFAULT '',\
		 `FLAG` int(10) unsigned NOT NULL DEFAULT '0',\
		  `NUM`  int(10) unsigned NOT NULL DEFAULT '0',\
		  `MODULE_NAME` varchar(33) NOT NULL DEFAULT '',\
		  `DOTIME` datetime NOT NULL,\
		  PRIMARY KEY (`ID`) \
		  )ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("EXCHANGE_LIST",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `MALL_BUY_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`LEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`VIPLEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`SHOPID`  int(10) unsigned NOT NULL DEFAULT '0',\
		`RESID`  int(10) unsigned NOT NULL DEFAULT '0',\
		`RESNUM` int(10) unsigned NOT NULL DEFAULT '0',\
		`ITEMID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ITEMNUM` int(10) unsigned NOT NULL DEFAULT '0',\
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("MALL_BUY_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `HERO_PROP_CHG_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`LEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`VIPLEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`HEROID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACTION` varchar(10) NOT NULL DEFAULT '',\
		`OLDLV` smallint unsigned NOT NULL DEFAULT '0',\
		`NEWLV` smallint unsigned NOT NULL DEFAULT '0',\
		`OLDEVOLV` smallint unsigned NOT NULL DEFAULT '0',\
		`NEWEVOLV` smallint unsigned NOT NULL DEFAULT '0',\
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("HERO_PROP_CHG_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `ENTER_SELECT_PAGE_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`FIRST_ENTER_TIME` datetime NOT NULL, \
		`OP_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`DEVICE_ID` varchar(125) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,  \
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("ENTER_SELECT_PAGE_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `GUILD_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`OP_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`LEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`VIPLV`  smallint unsigned NOT NULL DEFAULT '0',\
		`GUILDID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACTION`  varchar(20) NOT NULL DEFAULT '',\
		`OP_OBJ`  varchar(20) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("GUILD_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `GUILD_RES_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`GUILDID` int(10) unsigned NOT NULL DEFAULT '0',\
		`RES`  varchar(20) NOT NULL DEFAULT '',\
		`OLDVAL` int(10) unsigned NOT NULL DEFAULT '0',\
		`NEWVAL` int(10) unsigned NOT NULL DEFAULT '0',\
		`REASON` varchar(20) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("GUILD_RES_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `MODULE_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`OP_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`NAME` varchar(33) NOT NULL DEFAULT '',\
		`LEVEL`  smallint unsigned NOT NULL DEFAULT '0',\
		`VIPLV`  smallint unsigned NOT NULL DEFAULT '0',\
		`MODULE`  varchar(20) NOT NULL DEFAULT '',\
		`COPYMAPID` int(10) unsigned NOT NULL DEFAULT '0',\
		`OTHERCHARID` varchar(50) NOT NULL DEFAULT '',\
		`RESULT` tinyint unsigned NOT NULL DEFAULT '0',\
		`OLDRANK`  int(10) unsigned NOT NULL DEFAULT '0',\
		`NEWRANK` int(10) unsigned NOT NULL DEFAULT '0',\
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("MODULE_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `CREATE_ROLE_LOG`(\
		 `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`ACCOUNT` varchar(49) NOT NULL DEFAULT '',\
		`OP_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`DEVICE_ID` varchar(125) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;");
	m_createSqls.insert(std::make_pair("CREATE_ROLE_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `SKILL_CHG_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`SKILLID` int(10) unsigned NOT NULL DEFAULT '0',\
		`POS` tinyint unsigned NOT NULL DEFAULT '0',\
		`OLDVAL` smallint unsigned NOT NULL DEFAULT '0',\
		`NEWVAL` smallint unsigned NOT NULL DEFAULT '0',\
		`ACTION` varchar(20) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		`DECGOILD` int(10) unsigned NOT NULL DEFAULT '0',\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;\
		");
	m_createSqls.insert(std::make_pair("SKILL_CHG_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `SPRITE_CHG_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`SPRITEID` int(10) unsigned NOT NULL DEFAULT '0',\
		`OLDVAL` smallint unsigned NOT NULL DEFAULT '0',\
		`NEWVAL` smallint unsigned NOT NULL DEFAULT '0',\
		`ACTION` varchar(20) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		`CONSUME` int(10) unsigned NOT NULL DEFAULT '0',\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;\
		");
	m_createSqls.insert(std::make_pair("SPRITE_CHG_LOG",createSql));

	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `COPYMAP_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`COPYMAPID` int(10) unsigned NOT NULL DEFAULT '0',\
		`WIN` tinyint unsigned NOT NULL DEFAULT '0',\
		`ACTION` varchar(20) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;\
		");
	m_createSqls.insert(std::make_pair("COPYMAP_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `EQUIP_CHG_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`ITEMID` int(10) unsigned NOT NULL DEFAULT '0',\
		`OLDRAISELV` smallint unsigned NOT NULL DEFAULT '0' ,\
		`NEWRAISELV` smallint unsigned NOT NULL DEFAULT '0' ,\
		`OLDREFINELV` smallint unsigned NOT NULL DEFAULT '0' ,\
		`NEWREFINELV` smallint unsigned NOT NULL DEFAULT '0' ,\
		`ACTION` varchar(20) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		`DECGOLDCOIN` int(10) unsigned NOT NULL DEFAULT '0',\
		`DECYUNTIE` int(10) unsigned NOT NULL DEFAULT '0',\
		`DECITEM`int(10) unsigned NOT NULL DEFAULT '0',\
		`ADDGOLDCOIN` int(10) unsigned NOT NULL DEFAULT '0',\
		`ADDYUNTIE` int(10) unsigned NOT NULL DEFAULT '0',\
		`ADDITEM`int(10) unsigned NOT NULL DEFAULT '0',\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;\
		");
	m_createSqls.insert(std::make_pair("EQUIP_CHG_LOG",createSql));
	
	bzero(createSql,sizeof(createSql));
	snprintf(createSql,sizeof(createSql),"CREATE TABLE `ACTIVITY_LOG`(\
		`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
		`CHARID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACTIVITY_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`ACTIVITY_NAME` varchar(50) NOT NULL DEFAULT '',\
		`STEP_ID` int(10) unsigned NOT NULL DEFAULT '0',\
		`STEP_DESC` varchar(50) NOT NULL DEFAULT '',\
		`DOTIME` datetime NOT NULL,\
		PRIMARY KEY (`ID`)\
		)ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;\
	");
	m_createSqls.insert(std::make_pair("ACTIVITY_LOG",createSql));
}

std::string UserLogMgr::getUserLogLists()
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value dataList;
	Json::Value logInfo;
	
	for (UINT8 i = 0;i < eMax_Log_Type; ++i){
		logInfo["id"] = m_maxLogDesc[i].m_id;
		logInfo["title"] = m_maxLogDesc[i].m_desc;
		dataList.append(logInfo);
	}

	data["list"] =  dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	return json_file;
}

std::string UserLogMgr::getUserLogByID(Json::Value & searchCond)
{
	UINT16 logID = 0;
	
	if (searchCond["log_id"].isString()){
		logID = (UINT32)atoi(searchCond["log_id"].asString().c_str());
	}	
	
	std::string ret;

	switch (logID) {
		case eLog_Type_Levelup :		//升级日志
			{
				return getUserLevelupLog(searchCond);
			}
			break;
		case eLog_Type_Login :			//玩家登录
			{
				return getUserLoginLog(searchCond);	
			}
			break;
		case eLog_Type_Recharge :		//充值日志
			{
				return getRechargeLog(searchCond);
			}
			break;
		case eLog_Type_VipLevelup :		//vip升级日志
			{
				return getVipLevelupLog(searchCond);
			}
			break;
		case eLog_Type_HonourChg :		//荣誉变化
			 {
			 	return getHonourChgLog(searchCond);
			 }
			break;
		case eLog_Type_ItemChg :		//道具变化日志
			 {
			 	return getItemChgLog(searchCond);
			 }
			break;
		case eLog_Type_ResChg :			//资源变化日志
			{
				return getResChgLog(searchCond);
			}
			break;
		case eLog_Type_DailyTask :		//日常任务
			{
				return getDailyTaskLog(searchCond);
			}
			break;
		case eLog_Type_HeroPropChg : //宠物属性变化
			{
				return getHeroPropChgLog(searchCond);
			}
			break;
		/*case eLog_Type_EnterSelect : //进入日志
			{
				return enterSelectLog(searchCond);	
			}
			break;*/
		case eLog_Type_Guild : //公会日志
			{
				return guildLog(searchCond);
			}
			break;
		/*case eLog_Type_GuildRes : //公会资源日志
			{
				return guildResLog(searchCond);
			}
			break;*/
		case eLog_Type_Module : //模块日志
			{
				return moduleLog(searchCond);
			}
			break;
		case eLog_Type_MallBuy : //商店购买
			{
				return mallBuyLog(searchCond); 
			}
			break;
		case eLog_Type_SkillChg :
			{
				return skillChgLog(searchCond);	
			}
			break;
		case eLog_Type_Sprite :
			{
				return spriteLog(searchCond); 
			}
			break;
		case eLog_Type_Copymap :
			{
				return copymapLog(searchCond); 
			}
			break;
		case eLog_Type_Equip :
			{
				return equipChgLog(searchCond); 
			}
			break;
	}
	return ret;
}

UINT32 UserLogMgr::getRecordCount(const char * szTableName,const char * szWhere)
{
	UINT32 ret = 0;
	if (!szTableName || !szWhere){
		return ret;
	}

	CTblQueryResult* result = NULL;
	std::ostringstream os;
	os << "select count(*) from " << szTableName << " where " << szWhere;
	//char szSql[250] = { 0 };
	//bzero(szSql,sizeof(szSql));
	//snprintf(szSql,sizeof(szSql),"select count(*) from %s where %s ",szTableName,szWhere);
	result = g_databaseEngine.query(os.str().c_str(),os.str().size(),os.str().c_str());
	if (result){
		result->setFieldDef(g_rowNumTblDef);
		result->fillData(4);
		ret = *(UINT32 *)result->getData();
	}
	SAFE_DELETE(result);
	return ret;
}

std::string UserLogMgr::getSearchCond(Json::Value & searchCond)
{
	UINT32 page = 0;
	UINT32 size = 0;

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
	
	if (searchCond["start_time"].isString()){
		std::string startTime = searchCond["start_time"].asString();
		if (startTime.size() > 0){
			if (!addCond){
				addCond = true;
				os << " DOTIME >= '" << startTime << "'";
			}	
			else {
				os << " AND DOTIME >= '" << startTime << "'";
			}
		}
	}
	
	if (searchCond["end_time"].isString()){
		std::string startTime = searchCond["end_time"].asString();
		if (startTime.size() > 0){
			if (!addCond){
				addCond = true;
				os << " DOTIME <= '" << startTime << "'";
			}	
			else {
				os << " AND DOTIME <= '" << startTime << "'";
			}
		}
	}
	
	return os.str();
}

std::string UserLogMgr::getUnionTblName(std::string tblName)
{
	std::ostringstream os;
	std::map<std::string,std::set<std::string> >::iterator it = m_logTbls.find(tblName);
	std::set<std::string>::iterator subIt = it->second.begin();
	
	bool bFirst = true;
	
	if (it->second.size() <= 0){
		return tblName;
	}
	os << "(";
	for (;subIt != it->second.end();++subIt){
		if (bFirst){
			os << "( select * from " << *subIt << " )";	
			bFirst = false;
		}
		else {
			os << " UNION ( select * from " << *subIt << " )";	
		}
	}
	
	os << ") AS " << tblName;
	return os.str();
}

std::string UserLogMgr::getTblNameByDate(std::string tblName,Json::Value & searchCond)
{
	std::map<std::string,std::set<std::string> >::iterator it = m_logTbls.find(tblName);
	std::set<std::string>::iterator subIt = it->second.begin();
	
	if (it->second.size() <= 0){
		return tblName;
	}
	
	if (searchCond["start_time"].isString()){
		std::string startTime = searchCond["start_time"].asString();
		if (startTime.size() > 0){
			struct tm _tm;
			char szTodayTblName[100] = { 0 };
			if (strptime(startTime.c_str(),"%Y-%m-%d %H:%M:%S",&_tm)){
				snprintf(szTodayTblName,sizeof(szTodayTblName),"%s_%d%02d%02d",tblName.c_str(),_tm.tm_year + 1900,_tm.tm_mon + 1,_tm.tm_mday);
			}
			std::string upperStr = szTodayTblName;
			std::string lowerStr = szTodayTblName;
			std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
			std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

			std::set<std::string>::iterator subIt = it->second.find(upperStr);
			if (subIt != it->second.end()){
				return *subIt;
			}
			
			subIt = it->second.find(lowerStr);
			if (subIt != it->second.end()){
				return *subIt;
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("根据日期获得表格名字时候在内存中找不到表名字");
#endif
		}
	}
	return tblName;
}

std::string UserLogMgr::getUserLevelupLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "经验变化前";
	field["field3"] = "经验变化后";
	field["field4"] = "等级变化前";
	field["field5"] = "等级变化后";

	data["fields"] = field;
	
	
	Json::Value dataList;
	Json::Value userInfo;
	
	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("USER_LEVELUP_LOG");
	std::string unionTblName = getTblNameByDate("USER_LEVELUP_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_userLevelupLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stUserLevelupLogDB * pData = (stUserLevelupLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_levelupTime;
			userInfo["field2"] = pData[i].m_oldExp;
			userInfo["field3"] = pData[i].m_newExp;
			userInfo["field4"] = pData[i].m_oldLv;
			userInfo["field5"] = pData[i].m_newLv;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);

	SAFE_DELETE(result);
	return json_file;
}
	
std::string UserLogMgr::getUserLoginLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "登录IP";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  
	
	std::string strWhere = getSearchCond(searchCond);
	
	//std::string unionTblName = getUnionTblName("USER_LOGIN_LOG");
	std::string unionTblName = getTblNameByDate("USER_LOGIN_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_userLoginLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stUserLoginLogDB * pData = (stUserLoginLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_loginTime;
			userInfo["field2"] = pData[i].m_loginIP;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);

	SAFE_DELETE(result);
	return json_file;
}

std::string UserLogMgr::getRechargeLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "充值时间";
	field["field2"] = "订单号";
	field["field3"] = "充值金额";
	field["field4"] = "现在钻石数量";
	field["field5"] = "订单状态";
	field["field6"] = "获得钻石";
	field["field7"] = "vip等级";
	field["field8"] = "商品id";
	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("USER_RECHARGE_LOG");
	std::string unionTblName = getTblNameByDate("USER_RECHARGE_LOG",searchCond);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("充值日志查询:tblName:%s,szWhere:%s",unionTblName.c_str(),strWhere.c_str());
#endif
	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_userRechargeLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stUserRechargeLogDB * pData = (stUserRechargeLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_rechargeTime;
			userInfo["field2"] = pData[i].m_tid;
			userInfo["field3"] = pData[i].m_rechargeNum;
			userInfo["field4"] = pData[i].m_nowDiamond;
			userInfo["field5"] = "成功";
			userInfo["field6"] = pData[i].m_addDiamond;
			userInfo["field7"] = pData[i].m_viLv;
			userInfo["field8"] = pData[i].m_itemID;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

std::string UserLogMgr::getVipLevelupLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "经验变化前";
	field["field3"] = "经验变化后";
	field["field4"] = "获得黄金";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif


	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("USER_VIPLEVELUP_LOG");
	std::string unionTblName = getTblNameByDate("USER_VIPLEVELUP_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_userVipLevelupLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stUserVipLevelupLogDB * pData = (stUserVipLevelupLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_levelupTime;
			userInfo["field2"] = pData[i].m_oldExp;
			userInfo["field3"] = pData[i].m_newExp;
			userInfo["field4"] = pData[i].m_addDiamond;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}
	
std::string  UserLogMgr::getHonourChgLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "变化前";
	field["field3"] = "变化后";
	field["field4"] = "原因";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("HONOUR_CHG_LOG");
	std::string unionTblName = getTblNameByDate("HONOUR_CHG_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_userHonourChgLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stHonourChgLogDB * pData = (stHonourChgLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_chgTime;
			userInfo["field2"] = pData[i].m_oldHonour;
			userInfo["field3"] = pData[i].m_newHonour;
			userInfo["field4"] = pData[i].m_reason;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::getItemChgLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "资源名字";
	field["field3"] = "变化数量";
	field["field4"] = "变化模块";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;

	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("ITEM_CHG_LOG");
	std::string unionTblName = getTblNameByDate("ITEM_CHG_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_itemChgLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);
	
	if (result && result->getData()){
		stItemChgLogDB * pData = (stItemChgLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_chgTime;
			userInfo["field2"] = pData[i].m_itemName;
			userInfo["field3"] = pData[i].m_num;
			userInfo["field4"] = pData[i].m_reason;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

std::string UserLogMgr::getResChgLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "资源名字";
	field["field3"] = "增加数量";
	field["field4"] = "获得或消耗";
	field["field5"] = "变化前";
	field["field6"] = "变化后";
	field["field7"] = "原因";


	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	
	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("RES_CHG_LOG");
	std::string unionTblName = getTblNameByDate("RES_CHG_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_resChgLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stResChgLogDB * pData = (stResChgLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_chgTime;
			userInfo["field2"] = pData[i].m_resName;
			userInfo["field3"] = (int) pData[i].m_newValue - (int) pData[i].m_oldValue;
			userInfo["field4"] = pData[i].m_newValue > pData[i].m_oldValue ? "获得" : "消耗";
			userInfo["field5"] = pData[i].m_oldValue;
			userInfo["field6"] = pData[i].m_newValue;
			userInfo["field7"] = pData[i].m_reason;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::getDailyTaskLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "完成时间";
	field["field2"] = "任务ID";
	field["field3"] = "任务名字";


	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("DAILYTASK_LOG");
	std::string unionTblName = getTblNameByDate("DAILYTASK_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_dailyTaskLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stDailyTaskLogDB * pData = (stDailyTaskLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_doTime;
			userInfo["field2"] = pData[i].m_taskID;
			userInfo["field3"] = pData[i].m_taskName;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}
	
std::string UserLogMgr::getHeroPropChgLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "武将ID";
	field["field2"] = "武将名字";
	field["field3"] = "行为";
	field["field4"] = "原来等级";
	field["field5"] = "现在等级";
	field["field6"] = "原来品质";
	field["field7"] = "现在品质";
	field["field8"] = "时间";
	

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("HERO_PROP_CHG_LOG");
	std::string unionTblName = getTblNameByDate("HERO_PROP_CHG_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_heroPropChgLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stHeroPropChgLogDB * pData = (stHeroPropChgLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_heroID;
			stRoleBase * pRoleBase =   g_roleBaseMgr.get(pData[i].m_heroID);
			if (pRoleBase){
				userInfo["field2"] = pRoleBase->m_name;
			}
			userInfo["field3"] = pData[i].m_action;
			userInfo["field4"] = pData[i].m_oldLv;
			userInfo["field5"] = pData[i].m_newLv;
			userInfo["field6"] = pData[i].m_evoOldLv;
			userInfo["field7"] = pData[i].m_evoNewLv;
			userInfo["field8"] = pData[i].m_doTime;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::enterSelectLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "账号";
	field["field2"] = "首次进入时间";
	field["field3"] = "运营商ID";
	field["field4"] = "设备ID";
	field["field5"] = "进入时间";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("ENTER_SELECT_PAGE_LOG");
	std::string unionTblName = getTblNameByDate("ENTER_SELECT_PAGE_LOG",searchCond);
	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_enterSelectPageLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stEnterSelectPageLogDB * pData = (stEnterSelectPageLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_account;
			userInfo["field2"] = pData[i].m_firstEnterTime;
			userInfo["field3"] = pData[i].m_opID;
			userInfo["field4"] = pData[i].m_deviceID; 
			userInfo["field5"] = pData[i].m_doTime;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::guildLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "公会ID";
	field["field2"] = "操作类型";
	field["field3"] = "操作对象";
	field["field4"] = "时间";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("GUILD_LOG");
	std::string unionTblName = getTblNameByDate("GUILD_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_guildLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stGuildLogDB * pData = (stGuildLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_guildID;
			userInfo["field2"] = pData[i].m_action;
			userInfo["field3"] = pData[i].m_opObjID;
			userInfo["field4"] = pData[i].m_doTime;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::guildResLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "资源名字";
	field["field2"] = "变化前";
	field["field3"] = "变化后";
	field["field4"] = "原因";
	field["field5"] = "时间";
	
	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("GUILD_RES_LOG");
	std::string unionTblName = getTblNameByDate("GUILD_RES_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_guildResLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stGuildResLogDB * pData = (stGuildResLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_res;
			userInfo["field2"] = pData[i].m_oldVal;
			userInfo["field3"] = pData[i].m_oldVal;
			userInfo["field4"] = pData[i].m_reason;
			userInfo["field5"] = pData[i].m_doTime;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::moduleLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "模块玩法";
	field["field2"] = "副本ID";
	field["field3"] = "其它玩家ID";
	field["field4"] = "结果";
	field["field5"] = "原来排名";
	field["field6"] = "新的排名";
	field["field7"] = "时间";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("MODULE_LOG");
	std::string unionTblName = getTblNameByDate("MODULE_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_moduleLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stModuleLogDB * pData = (stModuleLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_module;
			userInfo["field2"] = pData[i].m_copymapID;
			userInfo["field3"] = pData[i].m_otherCharID;
			userInfo["field4"] = pData[i].m_result;
			userInfo["field5"] = pData[i].m_oldRank;
			userInfo["field6"] = pData[i].m_newRank;
			userInfo["field7"] = pData[i].m_doTime;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

std::string UserLogMgr::mallBuyLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "商店ID";
	field["field2"] = "资源ID";
	field["field3"] = "资源数量";
	field["field4"] = "道具ID";
	field["field5"] = "道具数量";
	field["field6"] = "时间";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("MALL_BUY_LOG");
	std::string unionTblName = getTblNameByDate("MALL_BUY_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_mallBuyLogTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stMallBuyLogDB * pData = (stMallBuyLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_shopID;
			userInfo["field2"] = pData[i].m_resID;
			userInfo["field3"] = pData[i].m_resNum;
			userInfo["field4"] = pData[i].m_itemID;
			userInfo["field5"] = pData[i].m_itemNum;
			userInfo["field6"] = pData[i].m_doTime;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::skillChgLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "技能ID";
	field["field3"] = "技能位置";
	field["field4"] = "操作";
	field["field5"] = "原来等级";
	field["field6"] = "现在等级";
	field["field7"] = "消耗金币";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("SKILL_CHG_LOG");
	std::string unionTblName = getTblNameByDate("SKILL_CHG_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_skillChgTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stSkillChgLogDB * pData = (stSkillChgLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_doTime;
			userInfo["field2"] = pData[i].m_skillID;
			userInfo["field3"] =  pData[i].m_pos;
			userInfo["field4"] =  pData[i].m_action;
			userInfo["field5"] =  pData[i].m_oldLv;
			userInfo["field6"] =  pData[i].m_newLv;
			userInfo["field7"] =  pData[i].m_decGoldcoin;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::spriteLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "精灵ID";
	field["field3"] = "原来等级";
	field["field4"] = "现在等级";
	field["field5"] = "消耗";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("SPRITE_CHG_LOG");
	std::string unionTblName = getTblNameByDate("SPRITE_CHG_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_spriteTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stSpriteLogDB * pData = (stSpriteLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_doTime;
			userInfo["field2"] = pData[i].m_spriteID;
			userInfo["field3"] =  pData[i].m_oldLv;
			userInfo["field4"] =  pData[i].m_newLv;
			userInfo["field5"] =  pData[i].m_consume;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}
	
std::string UserLogMgr::copymapLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "副本类型";
	field["field3"] = "副本名字";
	field["field4"] = "成功/失败";
	field["field5"] = "操作";
	field["field6"] = "消耗";

	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("COPYMAP_LOG");
	std::string unionTblName = getTblNameByDate("COPYMAP_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_copymapTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stCopymapLogDB * pData = (stCopymapLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] =  pData[i].m_doTime;
			stCopyMapBase * pCopyMapBase =  g_copyMapBaseMgr.get(pData[i].m_copymapID);
			if (pCopyMapBase) {
				if (eLevelType_Normal == pCopyMapBase->m_type){
					userInfo["field2"] = "普通副本";
				}
				else if (eLevelType_Leader == pCopyMapBase->m_type){
					userInfo["field2"] = "首领副本";	
				}
				else if (eLevelType_Elite == pCopyMapBase->m_type){
					userInfo["field2"] = "精英副本";
				}
				else if (eLevelType_GoldCoin == pCopyMapBase->m_type){
					userInfo["field2"] = "金币副本";  
				}
				else if (eLevelType_Survival == pCopyMapBase->m_type){
					userInfo["field2"] = "生存副本";
				}
				else {
					userInfo["field2"] = "其它";
				}
				userInfo["field3"] = pCopyMapBase->m_name;

				if (pCopyMapBase->m_costLiLian){
					userInfo["field6"] = "消耗历练";
				}
				else if (pCopyMapBase->m_costSP){
					userInfo["field6"] = "消耗体力";
				}
				else {
					userInfo["field6"] = "无消耗";
				}
			}

			if (pData[i].m_win) {
				userInfo["field4"] = "成功";
			}
			else {
				userInfo["field4"] = "失败";
			}
			userInfo["field5"] =  pData[i].m_action;
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;
}

std::string UserLogMgr::equipChgLog(Json::Value & searchCond)
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	Json::Value field;
	field["field1"] = "时间";
	field["field2"] = "道具ID";
	field["field3"] = "装备名字";
	field["field4"] = "强化前等级";
	field["field5"] = "强化后等级";
	field["field6"] = "精炼前等级";
	field["field7"] = "精炼后等级";
	field["field8"] = "操作";
	field["field9"] = "消耗金币";
	field["field10"] = "消耗龙晶石";
	field["field11"] = "消耗道具";
	field["field12"] = "获得金币";
	field["field13"] = "获得龙晶石";
	field["field14"] = "获得道具";
	data["fields"] = field;

	Json::Value dataList;
	Json::Value userInfo;
	
	UINT32 page = 0;
	UINT32 size = 0;

	if (searchCond["page"].isString()){
		page = (UINT32)atoi(searchCond["page"].asString().c_str());
	}   

	if (searchCond["size"].isString()){
		size = (UINT32)atoi(searchCond["size"].asString().c_str());
	}  

#ifdef _HDZ_DEBUG
	Athena::logger->trace("page =%d,size=%d",page,size);
#endif

	std::string strWhere = getSearchCond(searchCond);
	//std::string unionTblName = getUnionTblName("EQUIP_CHG_LOG");
	std::string unionTblName = getTblNameByDate("EQUIP_CHG_LOG",searchCond);

	data["total"] = getRecordCount(unionTblName.c_str(),strWhere.c_str()); 
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_equipChgTblDef,unionTblName.c_str(),strWhere.c_str(),NULL,size,NULL,(page - 1) * size);

	if (result && result->getData()){
		stEquipChgLogDB * pData = (stEquipChgLogDB *)result->getData();
		for (UINT32 i = 0;i < result->getRowCount();++i){
			userInfo["field1"] = pData[i].m_doTime;
			userInfo["field2"] = pData[i].m_itemID;
			
			stItemBase * pItemBase = g_itemBaseMgr.get(pData[i].m_itemID);
			if (pItemBase) {
				userInfo["field3"] = pItemBase->m_name;
			}
			userInfo["field4"] = pData[i].m_oldRaiseLv;
			userInfo["field5"] = pData[i].m_newRaiseLv;
			userInfo["field6"] = pData[i].m_oldRefineLv;
			userInfo["field7"] = pData[i].m_newRefineLv;
			userInfo["field8"] = pData[i].m_action;
			userInfo["field9"] = pData[i].m_decGoldcoin;
			userInfo["field10"] = pData[i].m_decYuntie;
			pItemBase = g_itemBaseMgr.get(pData[i].m_decItemID);
			if (pItemBase){
				userInfo["field11"] = pItemBase->m_name;	
			}
			userInfo["field12"] = pData[i].m_addGoldcoin;
			userInfo["field13"] = pData[i].m_addYuntie;

			pItemBase = g_itemBaseMgr.get(pData[i].m_addItemID);
			if (pItemBase){
				userInfo["field14"] =  pItemBase->m_name;
			}
			dataList.append(userInfo);
		}
	}

	data["list"] = dataList;
	root["data"] = data;
	std::string json_file = writer.write(root);
	SAFE_DELETE(result);
	return json_file;

}

std::string UserLogMgr::getRealTblName(const char * szTblName)
{
	if (!szTblName){
		return std::string();
	}
	std::string todayName = getTodayTblName(szTblName);

	std::map<std::string,std::set<std::string> >::iterator it = m_logTbls.find(szTblName);
	if (it != m_logTbls.end()){
		if (it->second.find(todayName) == it->second.end()){//今日表不存在
			std::map<std::string,std::string>::iterator subIt =  m_createSqls.find(szTblName);		
			if (subIt != m_createSqls.end()){
				size_t pos = subIt->second.find(szTblName);
				if (pos != std::string::npos){
					std::string createSql = subIt->second;
					createSql.replace(pos,strlen(szTblName),todayName);
					g_databaseEngine.directExecute(createSql.c_str(),createSql.size(),createSql.c_str());
					it->second.insert(todayName);
				}
			}
			/*std::ostringstream os;
			os << " ALTER TABLE " << szTblName << "  UNION=("; 
			std::set<std::string>::iterator thdIt = it->second.begin();
			bool first = true;
			for (;thdIt != it->second.end();++thdIt){
				if ((*thdIt) == szTblName){
					continue;
				}

				if (first){
					os << *thdIt; 
					first = false;
				}
				else {
					os << "," << *thdIt;
				}
			}
			os << ");";
			g_databaseEngine.directExecute(os.str().c_str(),os.str().size(),os.str().c_str());	*/
		}
	}
	return todayName;
}

std::string UserLogMgr::getTodayTblName(const char * szTblName)
{
	if (!szTblName){
		return std::string();
	}	

	struct tm _tm;
	RealTime::getLocalTime(_tm,(time_t)g_webServer->getSec());
	char szTodayTblName[100] = { 0 };
	snprintf(szTodayTblName,sizeof(szTodayTblName),"%s_%d%02d%02d",szTblName,_tm.tm_year + 1900,_tm.tm_mon + 1,_tm.tm_mday);
	return szTodayTblName;
}

CTblField g_logTblDef[] = { 
	{ "TABLE_NAME",      CTblField::DB_TYPE_STRING,    100 },  
	{ NULL, 0, 0 },  
};  

#pragma pack(1)
struct stLogTblInfo
{
	char m_tblName[100];

	stLogTblInfo()
	{
		bzero(m_tblName,sizeof(m_tblName));
	}
};
#pragma pack()

#define GET_ALL_TABLE(tblName,tblNameWild) do{\
	pResult = g_databaseEngine.listTable(g_logTblDef,tblNameWild);\
	if (pResult && pResult->getData()){\
		for (UINT16 i = 0;i < pResult->getRowCount();++i){\
			stLogTblInfo * pTblNameInfo = (stLogTblInfo *)pResult->getData();\
			m_logTbls[tblName].insert(pTblNameInfo[i].m_tblName);\
		}\
	}\
	SAFE_DELETE(pResult);\
}while(false);

void UserLogMgr::init()
{
	CTblQueryResult * pResult = NULL;
	GET_ALL_TABLE("USER_LEVELUP_LOG","USER_LEVELUP_LOG%");
	GET_ALL_TABLE("USER_LOGIN_LOG","USER_LOGIN_LOG%");
	GET_ALL_TABLE("USER_RECHARGE_LOG","USER_RECHARGE_LOG%");
	GET_ALL_TABLE("USER_VIPLEVELUP_LOG","USER_VIPLEVELUP_LOG%");
	GET_ALL_TABLE("HONOUR_CHG_LOG","HONOUR_CHG_LOG%");
	GET_ALL_TABLE("ITEM_CHG_LOG","ITEM_CHG_LOG%");
	GET_ALL_TABLE("RES_CHG_LOG","RES_CHG_LOG%");
	GET_ALL_TABLE("DAILYTASK_LOG","DAILYTASK_LOG%");
	GET_ALL_TABLE("EXCHANGE_LIST","EXCHANGE_LIST%");
	GET_ALL_TABLE("MALL_BUY_LOG","MALL_BUY_LOG%");
	GET_ALL_TABLE("HERO_PROP_CHG_LOG","HERO_PROP_CHG_LOG%");
	GET_ALL_TABLE("ENTER_SELECT_PAGE_LOG","ENTER_SELECT_PAGE_LOG%");
	GET_ALL_TABLE("GUILD_LOG","GUILD_LOG%");
	GET_ALL_TABLE("GUILD_RES_LOG","GUILD_RES_LOG%");
	GET_ALL_TABLE("MODULE_LOG","MODULE_LOG%");
	GET_ALL_TABLE("CREATE_ROLE_LOG","CREATE_ROLE_LOG%");
	GET_ALL_TABLE("SKILL_CHG_LOG","SKILL_CHG_LOG%");
	GET_ALL_TABLE("SPRITE_CHG_LOG","SPRITE_CHG_LOG%");
	GET_ALL_TABLE("COPYMAP_LOG","COPYMAP_LOG%");
	GET_ALL_TABLE("EQUIP_CHG_LOG","EQUIP_CHG_LOG%");
	GET_ALL_TABLE("ACTIVITY_LOG","ACTIVITY_LOG%");
#ifdef _HDZ_DEBUG
	{
		std::map<std::string,std::set<std::string> >::iterator it = m_logTbls.begin();
		for (;it != m_logTbls.end();++it){
			Athena::logger->trace("表%s所具有的分表有:",it->first.c_str());

			std::set<std::string>::iterator subIt = it->second.begin();
			for (;subIt != it->second.end();++subIt){
				Athena::logger->trace("分表:%s",(*subIt).c_str());	
			}
		}
	}
#endif
}

void UserLogMgr::dropAllLogTbl()
{
	if (!g_databaseEngine.initialize(Athena::global["mysqlurl"].c_str())){
		Athena::logger->error("初始化数据库失败!");
		return ;
	}
	init();
	char dropSql[250] = { 0 };
	std::map<std::string,std::set<std::string> >::iterator it = m_logTbls.begin();
	for (;it != m_logTbls.end();++it){
		std::set<std::string>::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			Athena::logger->trace("删除%s的分表:%s",it->first.c_str(),(*subIt).c_str());
			bzero(dropSql,sizeof(dropSql));
			snprintf(dropSql,sizeof(dropSql),"drop table %s;",(*subIt).c_str());
			g_databaseEngine.directExecute(dropSql,strlen(dropSql),dropSql);
		}
	}
}

CTblField g_espCreateRoleTimeTblDef[] = {
	{ "FIRST_CREATE_ROLE_TIME", CTblField::DB_TYPE_STRING,   20              },
	{ NULL, 0, 0 },
};


void UserLogMgr::updateCreateRoleTime(const std::string & account)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ACCOUNT='%s'",account.c_str());
	char firstCreateRoleTime[20] = { 0 };
	
	EnterSelectPageCache_IT it = m_enterSelectPageCache.find(account); 
	if (it != m_enterSelectPageCache.end()){
		strncpy(firstCreateRoleTime,it->second.m_createRoleTime,sizeof(firstCreateRoleTime));
	}

	/*CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_espCreateRoleTimeTblDef,"ENTER_SELECT_PAGE",where,NULL);

	if (result && result->getData()){
		strncpy(firstCreateRoleTime,(const char *)result->getData(),sizeof(firstCreateRoleTime));
	}
	SAFE_DELETE(result);*/

	if (strncmp(firstCreateRoleTime,"0000-00-00 00:00:00",sizeof(firstCreateRoleTime)) == 0){
		std::string curStr = g_utc2StringFormat(g_webServer->getSec());
		bzero(firstCreateRoleTime,sizeof(firstCreateRoleTime));
		strncpy(firstCreateRoleTime,curStr.c_str(),sizeof(firstCreateRoleTime));
		g_databaseEngine.updateRecord(g_espCreateRoleTimeTblDef,(const char *)firstCreateRoleTime,"ENTER_SELECT_PAGE",where,NULL);
		if (it != m_enterSelectPageCache.end()){
			strncpy(it->second.m_createRoleTime,curStr.c_str(),sizeof(it->second.m_createRoleTime));	
		}
	}
}

void UserLogMgr::loadEnterSelectPage()
{
	CTblQueryResult* pResult = g_databaseEngine.select(g_enterSelectPageTblDef,"ENTER_SELECT_PAGE",NULL,NULL,0,NULL); 
	if (pResult){
		stEnterSelectPageDB * pSelectPage = (stEnterSelectPageDB *)pResult->getData();
		for (UINT16 i = 0;i < pResult->getRowCount();++i){
			stEnterSelectPage & enterSelectPage = m_enterSelectPageCache[pSelectPage[i].m_account]; 
			strncpy(enterSelectPage.m_createRoleTime,pSelectPage[i].m_createRoleTime,sizeof(enterSelectPage.m_createRoleTime));
			strncpy(enterSelectPage.m_firstEnterTime,pSelectPage[i].m_firstEnterTime,sizeof(enterSelectPage.m_firstEnterTime));
		}
	}
	SAFE_DELETE(pResult);
}

std::string UserLogMgr::getExchangeListTable(std::string & start,std::string & end)
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("开始进入:getExchangeListTable");
#endif
	std::map<std::string,std::set<std::string> >::iterator it = m_logTbls.find("EXCHANGE_LIST");	
	if (it == m_logTbls.end()){
		return "EXCHANGE_LIST";
	}

	if ((start.size() <= 0) || (end.size() <= 0)){
		Athena::logger->error("开始时间跟结束时间必须都有");
		return "EXCHANGE_LIST";
	}

	std::ostringstream os;
	//std::set<std::string>::iterator subIt = it->second.begin();
	bool bFirst = true;
	
	if (it->second.size() <= 0){
		return "EXCHANGE_LIST";
	}
	
	struct tm _tm1,_tm2;
	char szTodayTblName[100] = { 0 };
	if (!strptime(start.c_str(),"%Y-%m-%d %H:%M:%S",&_tm1)){
		Athena::logger->error("EXCHANGE_LIST,查询时候 start 转换时间时候出现错误!");
		return "EXCHANGE_LIST";
	}
	
	if (!strptime(end.c_str(),"%Y-%m-%d %H:%M:%S",&_tm2)){
		Athena::logger->error("EXCHANGE_LIST,查询时候 end 转换时间时候出现错误!");
		return "EXCHANGE_LIST";
	}
	
	UINT32 sYear = _tm1.tm_year + 1900,eYear = _tm2.tm_year + 1900;
	UINT16 sMon  = _tm1.tm_mon + 1, eMon =  _tm2.tm_mon + 1;
	UINT16 sDay  = _tm1.tm_mday, eDay = _tm2.tm_mday;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("开始计算间隔天数start:%s,end:%s",start.c_str(),end.c_str());
#endif
	UINT32 diffDay = g_dayDiff(eYear,eMon,eDay,sYear,sMon,sDay);
	if (diffDay > 7){
		diffDay = 7;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("计算出来的间隔天数:%d",diffDay);
#endif
	UINT32 s_months[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
	
	UINT16 index = 0;
	for (UINT16 i = 0;i < diffDay;++i){
		UINT32 dayOfMon = 0;
		if (sMon < 13){
			dayOfMon = s_months[sMon];
			if ((sMon == 2) && (g_isLeapYear(sYear))){//如果是2月份
				dayOfMon += 1;
			}
		}
		UINT16 nextDay = sDay + index++;
		if (nextDay > dayOfMon){
			sMon += 1;
			sDay = 1;
			index = 0;
			if (sMon > 12){
				sMon = 1;
				sYear += 1;
			}
		}
		bzero(szTodayTblName,sizeof(szTodayTblName));
		snprintf(szTodayTblName,sizeof(szTodayTblName),"EXCHANGE_LIST_%d%02d%02d",sYear,sMon,nextDay);
		if (!isTblExist("EXCHANGE_LIST",szTodayTblName)){
			continue;
		}
		if (bFirst) {
			os << "(";
			os << "( select * from " << szTodayTblName << " )";
			bFirst = false;
		}
		else {
			os << " UNION ( select * from " << szTodayTblName << " )";
		}
	}
	
	if (os.str().size() > 0){
		os << ") AS  EXCHANGE_LIST" ;
	}
	else {
		os << "EXCHANGE_LIST";
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("开始离开:getExchangeListTable");
#endif
	return os.str();
}

bool UserLogMgr::isTblExist(std::string tblName,std::string dateTblName)
{
	std::map<std::string,std::set<std::string> >::iterator it = m_logTbls.find(tblName);
	if (it == m_logTbls.end()){
		return false;
	}

	if (it->second.size() <= 0){
		return false;
	}   
	
	std::string upperStr = dateTblName;
	std::string lowerStr = dateTblName;
	std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

	std::set<std::string>::iterator subIt = it->second.find(upperStr);
	if (subIt != it->second.end()){
		return true;
	}

	subIt = it->second.find(lowerStr);
	if (subIt != it->second.end()){
		return true;
	}
	return false;
}
