#include "BlackWhiteMgr.h"
#include "GlobalServer.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"

extern GlobalServer * g_globalServer;

CTblField g_loginIPTypeTblDef[] = {
	{"IP_TYPE",  CTblField::DB_TYPE_UINT16,   sizeof(UINT16)    },
	{ "IP",      CTblField::DB_TYPE_STRING,   50                },
	{ NULL, 0, 0 },
};

CTblField g_maintennaceTblDef[] = {
	{"ID",  					CTblField::DB_TYPE_UINT32,   sizeof(UINT32)    },
	{"MAINTENNACE_BEGIN",  		CTblField::DB_TYPE_UINT32,   sizeof(UINT32)    },
	{"MAINTENNACE_LASTTIME",  	CTblField::DB_TYPE_UINT32,   sizeof(UINT32)    },
	{ NULL, 0, 0 },
};

CBlackWhiteMgr::~CBlackWhiteMgr()
{
}
	
CBlackWhiteMgr::CBlackWhiteMgr()
{
	m_keyID = 0;
	m_maintennaceTime = 0;
	m_maintenaceLastTime = 0;
}

bool CBlackWhiteMgr::isBlackName(std::string & ip)
{
	if (m_blackLists.find(ip) != m_blackLists.end()){
		return true;
	}	
	else {
		return false;
	}
}
	
bool CBlackWhiteMgr::isWhiteName(std::string & ip)
{
	if (m_whiteLists.find(ip) != m_whiteLists.end()){
		return true;
	}	
	else {
		return false;
	}
}

bool CBlackWhiteMgr::isMaintenane()
{
	if (!m_maintennaceTime){
		return false;
	}	

	if (m_maintennaceTime + m_maintenaceLastTime > g_globalServer->getSec()){
		return true;
	}
	return false;
}

void CBlackWhiteMgr::load()
{
	CTblQueryResult* result = g_databaseEngine.select(g_loginIPTypeTblDef,"LOGIN_IP_TYPE",NULL,NULL,0,NULL);
	if (result){
		if (result->getData()){
			stLoginIPTypeDB *  pInfoDB = (stLoginIPTypeDB *) result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				if (pInfoDB[i].m_type == eWhite_Login_IP){
					m_whiteLists.insert(pInfoDB[i].m_ip);
				}
				else if (pInfoDB[i].m_type == eBlack_Login_IP){
					m_blackLists.insert(pInfoDB[i].m_ip);	
				}
			}
		}
	}
	SAFE_DELETE(result);

	result = g_databaseEngine.select(g_maintennaceTblDef,"MAINTENNACEINFO",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stMaintennaceDB * pDBData = (stMaintennaceDB *)result->getData();
		m_keyID = pDBData->m_id;
		m_maintennaceTime = pDBData->m_beginTime;
		m_maintenaceLastTime = pDBData->m_lastTime;
	}
	else {
		stMaintennaceDB dbData;
		dbData.m_beginTime = 0;
		dbData.m_lastTime = 0;
		UINT64 autoID = 0;
		g_databaseEngine.insertRecord(g_maintennaceTblDef,(const char *)&dbData,"MAINTENNACEINFO",autoID,NULL);
		m_maintennaceTime = 0;
		m_maintenaceLastTime = 0;
		m_keyID = autoID;
	}
	SAFE_DELETE(result);
	Athena::logger->trace("服务器启动加载了维护时间:%d,持续时间:%d",m_maintennaceTime,m_maintenaceLastTime);
}
	
void CBlackWhiteMgr::addBlackName(const std::string & ip)
{
	m_blackLists.insert(ip);	
}

void CBlackWhiteMgr::addWhiteName(const std::string & ip)
{
	m_whiteLists.insert(ip);
}

void CBlackWhiteMgr::updateMaintennaceInfo()
{
	stMaintennaceDB dbData;
	dbData.m_id = m_keyID;
	dbData.m_beginTime = m_maintennaceTime;
	dbData.m_lastTime = m_maintenaceLastTime;

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `ID`=%u", m_keyID);

	Athena::CUDSqlCallBack<CBlackWhiteMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CBlackWhiteMgr>(this,&CBlackWhiteMgr::updateMaintennaceInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_maintennaceTblDef,(const char *)&dbData,"MAINTENNACEINFO",where,pDelaySql);
}

void CBlackWhiteMgr::updateMaintennaceInfoCB(bool opResult,UINT64 autoID)
{
}
