#include "GloryfashionMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "SortList.pb.h"
#include "RankMgr.h"
#include "LimitTimePetCfg.h"
#include "Misc.pb.h"
#include "Mail.h"
#include "GloryfashionCfg.h"

CTblField g_gloryFashionTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BUYTIME",	    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
  	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
    { "JOB",            CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",            CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ NULL, 0, 0 },
};

CTblField g_gloryFashionActivityTblDef[] = {
	{ "ID",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACTIVITY_ID",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "IS_START",       CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ NULL, 0, 0 },
};

void stGloryfashionActivityInfo::initFromDB(stGloryfashionActivityDB & data)
{
	m_id = data.m_id;
	m_activityID = data.m_activityID;
	m_isStart = data.m_isStart;
}

void stGloryfashionActivityInfo::fillDBData(stGloryfashionActivityDB & data)
{
	data.m_id = m_id;
	data.m_activityID = m_activityID;
	data.m_isStart = m_isStart;
}

void CGloryfashionRankMgr::load()
{
	CTblQueryResult * result = NULL;
	result = g_databaseEngine.select(g_gloryFashionTblDef,"GLORYFASHION_RANK",NULL,NULL,0,NULL);
	if (result){
		stGloryfashionRankDB * pGloryfashionInfo = (stGloryfashionRankDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stGloryfashionRankInfo info ;
			info.m_charID = pGloryfashionInfo[i].m_charID;
			info.m_time = pGloryfashionInfo[i].m_time;
			strncpy(info.m_name,pGloryfashionInfo[i].m_name,sizeof(info.m_name));
			info.m_job = pGloryfashionInfo[i].m_job;
			info.m_sex = pGloryfashionInfo[i].m_sex;
			m_gloryfashionSortList.insertSortInfo(info);
		}
	}
	SAFE_DELETE(result);
	Athena::logger->trace("荣耀时装购买排行榜记录数为%d",m_gloryfashionSortList.getListNum());

	result = g_databaseEngine.select(g_gloryFashionActivityTblDef,"GLORYFASHION_ACTIVITY",NULL,NULL,0,NULL);
	if (!result || !result->getData()){
		UINT64 autoID = 0;
		stGloryfashionActivityDB dbData;
		g_databaseEngine.insertRecord(g_gloryFashionActivityTblDef,(const char *)&dbData,"GLORYFASHION_ACTIVITY",autoID,NULL);
		m_activityInfo.m_id = autoID;
	}
	else {
		stGloryfashionActivityDB * pDBData = (stGloryfashionActivityDB * )result->getData();
		m_activityInfo.initFromDB(pDBData[0]);
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("id:%d,activityID:%d,isStart:%d",m_activityInfo.m_id,m_activityInfo.m_activityID,m_activityInfo.m_isStart);
#endif
}

void CGloryfashionRankMgr::addRecord(const char * pData)
{
	Athena::CUDSqlCallBack<CGloryfashionRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGloryfashionRankMgr>(this,&CGloryfashionRankMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_gloryFashionTblDef, pData, "GLORYFASHION_RANK", autoID, pDelaySql);  
}

void CGloryfashionRankMgr::addRecordCB(bool opResult,UINT64 autoID)
{}

void CGloryfashionRankMgr::updateRecord(UINT32 key,const char * pData)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",key);

	Athena::CUDSqlCallBack<CGloryfashionRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGloryfashionRankMgr>(this,&CGloryfashionRankMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_gloryFashionTblDef,pData,"GLORYFASHION_RANK",where,pDelaySql); 
}

void CGloryfashionRankMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void CGloryfashionRankMgr::removeRecord(UINT32 key)                                  
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%d",key);
	Athena::CUDSqlCallBack<CGloryfashionRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGloryfashionRankMgr>(this,&CGloryfashionRankMgr::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GLORYFASHION_RANK",where,pDelaySql);
}

void CGloryfashionRankMgr::removeRecordCB(bool opResult,UINT64 autoID)               
{
}

void CGloryfashionRankMgr::removeAllGloryfashionRecord()
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID > 0 ");
	Athena::CUDSqlCallBack<CGloryfashionRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGloryfashionRankMgr>(this,&CGloryfashionRankMgr::removeAllGloryfashionRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GLORYFASHION_RANK",where,pDelaySql);
}

void CGloryfashionRankMgr::updateActivityInfo()
{

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%u",m_activityInfo.m_id);

	stGloryfashionActivityDB dbData;
	m_activityInfo.fillDBData(dbData);
	Athena::CUDSqlCallBack<CGloryfashionRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGloryfashionRankMgr>(this,&CGloryfashionRankMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_gloryFashionActivityTblDef,(const char *)&dbData,"GLORYFASHION_ACTIVITY",where,pDelaySql); 
}

void CGloryfashionRankMgr::updateActivityInfoCB(bool opResult,UINT64 autoID)
{}

void CGloryfashionRankMgr::removeAllGloryfashionRecordCB(bool opResult,UINT64 autoID)
{}

void CGloryfashionRankMgr::timer(UINT32 cur)
{
	if (!m_activityInfo.m_isStart){//还没有开启
		CGloryfashionCfg::stActivityInfo *pOpenInfo = CGloryfashionCfg::getSingleton().getActicityInfo(cur);
		if (pOpenInfo){//活动开启了
			m_activityInfo.m_isStart = 1;
			m_activityInfo.m_activityID = pOpenInfo->m_id; 
			updateActivityInfo();
			activityStart();
		}
	}
	else if (m_activityInfo.m_isStart > 0){//已经开始了
		CGloryfashionCfg::stActivityInfo *pOpenInfo = CGloryfashionCfg::getSingleton().getActicityInfo(cur);
		if (!pOpenInfo){
			m_activityInfo.m_isStart = 0;
			updateActivityInfo();
			activityEnd();
		}
		else if (pOpenInfo && (pOpenInfo->m_id != m_activityInfo.m_activityID)){
			m_activityInfo.m_activityID = pOpenInfo->m_id;
			updateActivityInfo();
		}
	}
}

void CGloryfashionRankMgr::activityStart()
{
	removeAllGloryfashionRecord();
	m_gloryfashionSortList.clear();
}

void CGloryfashionRankMgr::activityEnd()
{
}

void CGloryfashionRankMgr::updateGloryfashionTime(Player * pUser,UINT32 time)
{
	if (!pUser){
		return ;
	}
	
	if (m_gloryfashionSortList.isInSortList(pUser->getID())){
		return;
	}
	else {
		stGloryfashionRankInfo info;
		info.m_charID = pUser->getID();
		info.m_time = time;
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_job = pUser->m_job;
		info.m_sex = pUser->m_sex;
		m_gloryfashionSortList.insertSortInfo(info);
		
		stGloryfashionRankDB dbData;
		info.fillDBData(dbData);
		addRecord((const char *)&dbData);
	}
}

void CGloryfashionRankMgr::sendGloryfashionSortList(Player *pUser,UINT32 beginRank)
{
	if (!pUser){
		return ;
	}
	
	SortProto::stGloryfashionSortList retCmd;
	retCmd.set_beginrank(beginRank);

	std::vector<stGloryfashionRankInfo> vec;
	m_gloryfashionSortList.getFromN12N2(vec,beginRank,beginRank + s_SEND_SORTITEM_MAX);
	
	for (UINT8 i = 0;i < vec.size();++i){
		retCmd.add_name(vec[i].m_name);
	}

	pUser->sendProtoToMe(retCmd);
}

void CGloryfashionRankMgr::synRecord(Player * pUser)
{
	if(!pUser){
		return;
	}
	
	if(m_gloryfashionSortList.isInSortList(pUser->getID())){
		stGloryfashionRankInfo info = m_gloryfashionSortList.getSortInfo(pUser->getID());
		strncpy(info.m_name, pUser->getName(),sizeof(info.m_name));
		m_gloryfashionSortList.updateSortInfo(info);

		stGloryfashionRankDB dbData;     
		info.fillDBData(dbData);         
		updateRecord(info.getKey(), (const char *)&dbData);
	}
}
