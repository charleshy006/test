#include "ActivityConfigMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "WebServer.h"

CTblField g_activityConfigTblDef[] = {
	{ "ACTIVITY_ID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CONFIG",    		  CTblField::DB_TYPE_STRING,  4096  		   },  
	{ NULL, 0, 0 },
};

CActivityConfigMgr::~CActivityConfigMgr()
{

}

void CActivityConfigMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_activityConfigTblDef," ACTIVITY_CONFIG ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stActivityConfigInfoDB * pDBData = (stActivityConfigInfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			m_activities.insert(pDBData[i].m_activityID);
		}   
	}   

#ifdef _HDZ_DEBUG
	std::set<UINT32>::iterator it = m_activities.begin();
	for (;it != m_activities.end();++it){
		Athena::logger->trace("加载的活动ID:%d",*it);
	}
#endif
}

void CActivityConfigMgr::save(UINT32 id,std::string configJson)
{
	std::set<UINT32>::iterator it = m_activities.find(id);	

	if (it != m_activities.end()){//存在则更新
		char szWhere[65] = { 0 };
		snprintf(szWhere,sizeof(szWhere),"ACTIVITY_ID=%d",id);
		stActivityConfigInfoDB data;
		data.m_activityID = id;
		strncpy(data.m_config,configJson.c_str(),sizeof(data.m_config));
		g_databaseEngine.updateRecord(g_activityConfigTblDef,(const char*)&data,"ACTIVITY_CONFIG",szWhere);
	}
	else {
		m_activities.insert(id);
		UINT64 autoID = 0;
		stActivityConfigInfoDB data;
		data.m_activityID = id;
		strncpy(data.m_config,configJson.c_str(),sizeof(data.m_config));
		g_databaseEngine.insertRecord(g_activityConfigTblDef,(const char*)&data,"ACTIVITY_CONFIG",autoID);
	}
}

void CActivityConfigMgr::del(UINT32 id)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ACTIVITY_ID=%u", id);
    m_activities.erase(id);
	g_databaseEngine.deleteRecord("ACTIVITY_CONFIG",where,NULL);
}
