#include "OpenServerInfo.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Global.pb.h"
#include "RankMgr.h"

extern GlobalServer * g_globalServer;

CTblField g_openServerInfoTblDef[] = {
	{ "ID",      	 CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "OPEN_TIME",   CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "OPEN_DAY",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "LASTREFRESHTIME",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ NULL, 0, 0 },
};

void stOpenServerInfo::initFromDB(const stOpenServerInfoDB & data)
{
	m_id = data.m_id;
	m_openDay = data.m_openDay;
	m_openTime = data.m_openTime;
	m_lastRefreshTime = data.m_lastRefreshTime;
}

void stOpenServerInfo::fillDBData(stOpenServerInfoDB & data)
{
	data.m_id = m_id;
	data.m_openDay = m_openDay;
	data.m_openTime = m_openTime;
	data.m_lastRefreshTime = m_lastRefreshTime;
}

COpenServerMgr::~COpenServerMgr()
{

}

bool COpenServerMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_openServerInfoTblDef," OPEN_SERVER_INFO ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stOpenServerInfoDB * pDBData = (stOpenServerInfoDB * )result->getData();
		m_openServerInfo.initFromDB(pDBData[0]);
	}
	else {
		UINT64 autoID = 0;
		m_openServerInfo.m_openDay = 0;
		m_openServerInfo.m_openTime = 0;
		m_openServerInfo.m_lastRefreshTime = 0;
		stOpenServerInfoDB dbData;
		m_openServerInfo.fillDBData(dbData);
		g_databaseEngine.insertRecord(g_openServerInfoTblDef,(const char *)&dbData,"OPEN_SERVER_INFO",autoID,NULL);
		m_openServerInfo.m_id = autoID;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("开服信息:id:%d,openTime:%d,openDay:%d",m_openServerInfo.m_id,m_openServerInfo.m_openTime,m_openServerInfo.m_openDay);
#endif
	return true;
}

void COpenServerMgr::timer(UINT32 cur)
{
	if (cur < g_globalServer->getOpenServerTime()){//还没有开服
		return ;
	}

	if (!g_isTheSameDay(cur,m_openServerInfo.m_lastRefreshTime)){
		m_openServerInfo.m_openDay += 1;
		m_openServerInfo.m_lastRefreshTime = cur;
		if (m_openServerInfo.m_openTime == 0){
			m_openServerInfo.m_openTime = cur;
		}
		stOpenServerInfoDB dbData;
		m_openServerInfo.fillDBData(dbData);
		updateRecord(dbData);
		sendOpenServerInfo();

		if (m_openServerInfo.m_openDay >= 8){
			CRankMgr::getSingleton().checkOpenSvrPowerRank();	
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("增加开服天数,当前开服天数是:%d",m_openServerInfo.m_openDay);
#endif
	}	
}

void COpenServerMgr::updateRecord(stOpenServerInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `ID`=%u", data.m_id);

	Athena::CUDSqlCallBack<COpenServerMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COpenServerMgr>(this,&COpenServerMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_openServerInfoTblDef,(const char *)&data,"OPEN_SERVER_INFO",where,pDelaySql);
}

void COpenServerMgr::updateRecordCB(bool opResult,UINT64 autoID)
{

}

void COpenServerMgr::sendOpenServerInfo()
{
	Global::stSetOpenServerDay cmd;
	cmd.set_day(m_openServerInfo.m_openDay);
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}
