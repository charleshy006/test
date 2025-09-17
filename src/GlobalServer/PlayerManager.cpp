#include "PlayerManager.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include <list>

PlayerManager::~PlayerManager()
{
	for (TIDMap::iterator it = m_idMap.begin();it != m_idMap.end();++it){
		SAFE_DELETE(it->second);
	}
}

CTblField g_charZoneIDTblDef [] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ZONEID",         CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ NULL, 0, 0 },
};

CTblField g_onlineStaticsTblDef [] = {
	{ "YEAR",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "MONTH",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DAY",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "TIMESTAMP",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ONLINENUM",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ NULL, 0, 0 },
};

#pragma pack(1)
struct stCharZoneIDInfo
{
	UINT32 m_charID;
	UINT16 m_zoneID;

	stCharZoneIDInfo()
	{
		m_charID = 0;
		m_zoneID = 0;
	}
};

struct stOnlineStaticsInfo
{
	UINT32 m_year;
	UINT32 m_month;
	UINT32 m_day;
	UINT32 m_timestamp;
	UINT32 m_onlineNum;


	stOnlineStaticsInfo()
	{
		m_year = 0;
		m_month = 0;
		m_day = 0;
		m_timestamp = 0;
		m_onlineNum = 0;
	}
};
#pragma pack()

void PlayerManager::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_charZoneIDTblDef," ROLEINFO ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stCharZoneIDInfo * pDBData = (stCharZoneIDInfo * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			m_charID2ZoneIDMap[pDBData[i].m_charID] = pDBData[i].m_zoneID;	
		}
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了%d个玩家的区信息",m_charID2ZoneIDMap.size());
#endif
}

void PlayerManager::afterCreateRole(UINT32 charID,UINT16 zoneID)
{
	m_charID2ZoneIDMap[charID] = zoneID;	
}

void PlayerManager::removeAllUserByConn(UINT16 svrID)
{
	struct RemoveUserCallback : execElement<Element>
	{
		std::list<Player *> m_delList;
		UINT16 m_svrID;
		RemoveUserCallback(UINT16 svrID) : m_svrID(svrID)
		{
			
		}

		virtual bool exec(Element* e)
		{
			Player * pUser = (Player *)e;
			if ((pUser->getGateID() == m_svrID)
					|| (pUser->getGameID() == m_svrID)){
				m_delList.push_back(pUser);
			}
			return true;
		}

	};

	RemoveUserCallback cb(svrID);
	execEveryElement(cb);

	for (std::list<Player *>::iterator it = cb.m_delList.begin();it != cb.m_delList.end();++it){
		Player * pUser = *it;
		pUser->unreg();
		this->removeUser(pUser);
		SAFE_DELETE(pUser);
	}
}

bool PlayerManager::addUser(Player* pUser)
{ 
	if (!pUser){
		return false;
	}
	
	if (addElement(pUser)) {
		UINT32 zoneID = m_charID2ZoneIDMap[pUser->getID()];
		if (zoneID){
			m_zonePlayers[zoneID].insert(pUser);
		}

		if (!m_account2PlayerMap.insert(std::make_pair(pUser->getAccount(),pUser)).second){
			removeElement(pUser);
			m_zonePlayers[zoneID].erase(pUser);
			return false;
		}
		return true;
	}
	return false;
}

void PlayerManager::removeUser(Player* pUser)
{ 
	if (!pUser){
		return ;
	}
	removeElement(pUser);
	UINT32 zoneID = m_charID2ZoneIDMap[pUser->getID()];
	if (zoneID){
		m_zonePlayers[zoneID].erase(pUser);
	}
	
	Account2PlayerMap_IT it = m_account2PlayerMap.find(pUser->getAccount());

	if (it != m_account2PlayerMap.end()){
		m_account2PlayerMap.erase(it);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("账号%s,name=%s下线了",pUser->getAccount().c_str(),pUser->getName());
#endif
	}
}

void PlayerManager::timerUpdate(UINT32 cur)
{
	/*for (TIDMap::iterator it = m_idMap.begin();it != m_idMap.end();++it){
		Player * pUser = (Player *)it->second;
		pUser->timerUpdate(cur);	
	}*/
}

void PlayerManager::getRecommondList(std::vector<Player *> & vec,UINT32 zoneID)
{
	TIDMap_IT it =  m_idMap.begin();
	for (;it != m_idMap.end();++it){
		vec.push_back((Player *)it->second);
		if (vec.size() >= 10){
			break;
		}
	}
}

Player* PlayerManager::getUserByAccount(std::string account)
{
	Account2PlayerMap_IT it = m_account2PlayerMap.find(account);

	if (it == m_account2PlayerMap.end()){
		return NULL;
	}
	else {
		return it->second;
	}
}
	
void PlayerManager::updateOnlineNum(UINT32 sec)
{
	stOnlineStaticsInfo data;
	struct tm tm;
	RealTime::getLocalTime(tm,sec);
	data.m_day = tm.tm_mday;
	data.m_month = tm.tm_mon + 1;
	data.m_year = tm.tm_year + 1900;
	data.m_timestamp = sec;
	data.m_onlineNum = size();

	Athena::CUDSqlCallBack<PlayerManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<PlayerManager>(this,&PlayerManager::updateOnlineNumCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_onlineStaticsTblDef,(const char *)&data,"ONLINE_STATICS",autoID,pDelaySql);
}

void PlayerManager::updateOnlineNumCB(bool opResult,UINT64 autoID)
{

}
