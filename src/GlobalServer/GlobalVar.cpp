#include "GlobalVar.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"

CTblField g_globalVarInfoTblDef[] = {
	{ "KEY",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "VALUE",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ NULL, 0, 0 },
};

CGlobalVarMgr::~CGlobalVarMgr()
{

}

bool CGlobalVarMgr::verifyDataBase()
{
	CTblQueryResult * result = g_databaseEngine.select(g_globalVarInfoTblDef," GLOBAL_VAR ",NULL,NULL,0,NULL);
	if (!result || !result->getData() || !result->getRowCount()){
		Athena::logger->error("访问global_var不成功,数据库没有启动起来!");
		SAFE_DELETE(result);
		return false;
	}
	SAFE_DELETE(result);
	return true;
}

bool CGlobalVarMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_globalVarInfoTblDef," GLOBAL_VAR ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stGlobalVarInfoDB * pDBData = (stGlobalVarInfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			m_key2Values[pDBData[i].m_key] = pDBData[i].m_value;
		}
	}
	
	stGlobalVarInfoDB dbData;
	UINT64 autoID = 0;
	if (getValue(eGlobalVar_Refresh_HistoryRank) <= 0){
		dbData.m_key = eGlobalVar_Refresh_HistoryRank;
		dbData.m_value = g_globalServer->getSec();
		m_key2Values[dbData.m_key] = dbData.m_value;
		g_databaseEngine.insertRecord(g_globalVarInfoTblDef,(const char *)&dbData,"GLOBAL_VAR",autoID,NULL);
	}
	
	if (getValue(eGlobalVar_Refresh_YestoryArenaRank) <= 0){
		dbData.m_key = eGlobalVar_Refresh_YestoryArenaRank;
		dbData.m_value = 0;
		m_key2Values[dbData.m_key] = dbData.m_value;
		g_databaseEngine.insertRecord(g_globalVarInfoTblDef,(const char *)&dbData,"GLOBAL_VAR",autoID,NULL);
	}
	
	if (getValue(eGlobalVar_Refresh_One2OneRank) <= 0){
		dbData.m_key = eGlobalVar_Refresh_One2OneRank;
		dbData.m_value = g_globalServer->getSec();
		m_key2Values[dbData.m_key] = dbData.m_value;
		g_databaseEngine.insertRecord(g_globalVarInfoTblDef,(const char *)&dbData,"GLOBAL_VAR",autoID,NULL);
	}

	if (getValue(eGlobalVar_Refresh_Week_One2OneRank) <= 0){
		dbData.m_key = eGlobalVar_Refresh_Week_One2OneRank;
		dbData.m_value = g_globalServer->getSec();
		m_key2Values[dbData.m_key] = dbData.m_value;
		g_databaseEngine.insertRecord(g_globalVarInfoTblDef,(const char *)&dbData,"GLOBAL_VAR",autoID,NULL);
	}
	
	if (getValue(eGlobalVar_Refresh_Guild_Zero) <= 0){
		dbData.m_key = eGlobalVar_Refresh_Guild_Zero;
		dbData.m_value = g_globalServer->getSec();
		m_key2Values[dbData.m_key] = dbData.m_value;
		g_databaseEngine.insertRecord(g_globalVarInfoTblDef,(const char *)&dbData,"GLOBAL_VAR",autoID,NULL);
	}
	
	if (getValue(eGlobalVar_Refresh_Guild_Score) <= 0){
		m_key2Values[dbData.m_key] = dbData.m_value;
		dbData.m_key = eGlobalVar_Refresh_Guild_Score;
		dbData.m_value = g_globalServer->getSec();
		g_databaseEngine.insertRecord(g_globalVarInfoTblDef,(const char *)&dbData,"GLOBAL_VAR",autoID,NULL);
	}

	if (getValue(eGlobalVar_Refresh_Week_LeiTaiRank) <= 0){
		m_key2Values[dbData.m_key] = dbData.m_value;
		dbData.m_key = eGlobalVar_Refresh_Week_LeiTaiRank;
		dbData.m_value = g_globalServer->getSec();
		g_databaseEngine.insertRecord(g_globalVarInfoTblDef,(const char *)&dbData,"GLOBAL_VAR",autoID,NULL);
	}

#ifdef _HDZ_DEBUG
	std::map<UINT32,UINT32>::iterator it = m_key2Values.begin();
	for (;it != m_key2Values.end();++it){
		Athena::logger->trace("已经设置的全局变量是:key=%d,valuer=%d",it->first,it->second);
	}
#endif
	return true;
}

UINT32 CGlobalVarMgr::getValue(UINT32 key)
{
	std::map<UINT32,UINT32>::iterator it = m_key2Values.find(key);
	if (it != m_key2Values.end()){
		return it->second;
	}
	else {
		return 0;
	}
}

UINT32 CGlobalVarMgr::operator [](UINT32 key)
{
	return getValue(key);
}

void CGlobalVarMgr::setValue(UINT32 key,UINT32 value)
{
	std::map<UINT32,UINT32>::iterator it = m_key2Values.find(key);	
	if (it == m_key2Values.end()){
		m_key2Values[key] = value;
		addRecord(key,value);
	}
	else {
		m_key2Values[key] = value;
		updateRecord(key,value);
	}
}

void CGlobalVarMgr::addRecord(UINT32 key,UINT32 value)
{
	stGlobalVarInfoDB data;
	data.m_key = key;
	data.m_value = value;
	Athena::CUDSqlCallBack<CGlobalVarMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGlobalVarMgr>(this,&CGlobalVarMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_globalVarInfoTblDef,(const char *)&data,"GLOBAL_VAR",autoID,pDelaySql);
}

void CGlobalVarMgr::addRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CGlobalVarMgr::updateRecord(UINT32 key,UINT32 value)
{
	stGlobalVarInfoDB data;
	data.m_key = key;
	data.m_value = value;

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `KEY`=%u", key);

	Athena::CUDSqlCallBack<CGlobalVarMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGlobalVarMgr>(this,&CGlobalVarMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_globalVarInfoTblDef,(const char *)&data,"GLOBAL_VAR",where,pDelaySql);
}

void CGlobalVarMgr::updateRecordCB(bool opResult,UINT64 autoID)
{
	
}
