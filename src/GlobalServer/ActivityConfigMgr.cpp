#include "ActivityConfigMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "json/reader.h"

CTblField g_activityConfigTblDef[] = {
	{ "ACTIVITY_ID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CONFIG",    		  CTblField::DB_TYPE_STRING,  4096  		   },  
	{ NULL, 0, 0 },
};

CActivityConfigMgr::~CActivityConfigMgr()
{

}

bool CActivityConfigMgr::load()
{
	UINT32 cur = time(NULL);
	CTblQueryResult * result = g_databaseEngine.select(g_activityConfigTblDef," ACTIVITY_CONFIG ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stActivityConfigInfoDB * pDBData = (stActivityConfigInfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			Json::Value configJson;
			Json::Reader reader;

			if (!reader.parse(pDBData[i].m_config,configJson)){
				continue;
			}
			std::string endTime;
			if (configJson["activityTime_end"].isString()){
				endTime = configJson["activityTime_end"].asString();
			}
			char szTime[120] = { 0 };
			snprintf(szTime,sizeof(szTime),"%s:00",endTime.c_str());

			struct tm _tm;
			if (strptime(szTime,"%Y-%m-%d %H:%M:%S",&_tm)){
				UINT32 iEndTime = mktime(&_tm);

				if (iEndTime < cur){
					Athena::logger->trace("ACTIVITY_CONFIG id:%d,已经过了时间不再加载!",pDBData[i].m_activityID);
					continue;
				}
			}
			m_activitiesConfig[pDBData[i].m_activityID] = pDBData[i].m_config;
		}
	}

#ifdef _HDZ_DEBUG
	std::map<UINT32,std::string>::iterator it = m_activitiesConfig.begin();
	for (;it != m_activitiesConfig.end();++it){
		Athena::logger->trace("加载了活动配置:activityID=%d,config:%s",it->first,it->second.c_str());
	}
#endif
	return true;
}

void CActivityConfigMgr::sendActivityConfig(UINT16 svrID)
{
	Global::stSynActivityConfig cmd;
	std::map<UINT32,std::string>::iterator it = m_activitiesConfig.begin();
	for (;it != m_activitiesConfig.end();++it){
		Global::stActivityConfig * pSyn = cmd.add_data();
		if (pSyn){
			pSyn->set_activityid(it->first);
			pSyn->set_cofing(it->second);
		}
	}
	g_globalServer->getConnMgr().broadcastByID(svrID,cmd);	
}
