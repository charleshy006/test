#include "RechargeMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "Global.pb.h"

CTblField g_rechargeInfoDef[] = {
	{ "CHARID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  }, 
	{ "MONEY",    	  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  }, 
	{ "ITEMID",    	  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  }, 
	{ "TID",    	  CTblField::DB_TYPE_STRING,  50  			  }, 
	{ "OBTAIN",    	  CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  }, 
#ifdef _YUNDING_TONGJI
	{ "ISEXTEND",     CTblField::DB_TYPE_UINT8,  sizeof(UINT8)	},
	{ "DATA",         CTblField::DB_TYPE_STRING,  1024             },
#endif
	{ NULL, 0, 0 },
};

CTblField g_updateObtainFlagDef[] = {
	{ "OBTAIN",    	  CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  }, 
	{ NULL, 0, 0 },
};

void stRechargeInfo::initFromDB(const stRechargeDBInfo & dbData)
{
	m_charID = dbData.m_charID;
	m_money = dbData.m_money;
	m_itemID = dbData.m_itemID;
	m_bObtain = dbData.m_bObtain;
	strncpy(m_tid,dbData.m_tid,sizeof(m_tid));
#ifdef _YUNDING_TONGJI
	m_isExtend = dbData.m_isExtend;
	strncpy(m_data, dbData.m_data, sizeof(m_data));
#endif
}

void stRechargeInfo::fillDBData(stRechargeDBInfo & dbData)
{
	dbData.m_charID = m_charID;
	dbData.m_money = m_money;
	dbData.m_itemID = m_itemID;
	dbData.m_bObtain = m_bObtain;
	strncpy(dbData.m_tid,m_tid,sizeof(dbData.m_tid));
#ifdef _YUNDING_TONGJI
	dbData.m_isExtend = m_isExtend;
	strncpy(dbData.m_data, m_data, sizeof(dbData.m_data));
#endif
}

void CRechargeMgr::load()
{
	CTblQueryResult* result = g_databaseEngine.select(g_rechargeInfoDef,"RECHARGE_INFO",NULL,NULL,0,NULL);
	if (result && result->getData()){
		for (UINT32 i = 0; i < result->getRowCount();++i) {
			stRechargeDBInfo * pInfo = (stRechargeDBInfo *)result->getData() + i;
			if (!pInfo->m_bObtain){//只有未领取的才加载
				stRechargeInfo one;
				one.fillDBData(*pInfo);
				m_userRechargeMap[pInfo->m_charID].push_back(one);
			}
			m_tidCaches.insert(pInfo->m_tid);
		}
	}

#ifdef _HDZ_DEBUG
	std::map<UINT32,std::vector<stRechargeInfo> >::iterator it =  m_userRechargeMap.begin();
	for (;it != m_userRechargeMap.end(); ++it){
		for (UINT16 i = 0;i < it->second.size();++i){
			stRechargeInfo & info = it->second[i];
			Athena::logger->trace("加载了充值相关信息charID=%d,money=%d,itemID=%d,isObtain:%d",it->first,info.m_money,info.m_itemID,info.m_bObtain);	
		}
	}
#endif
}

void CRechargeMgr::addRecord(stRechargeDBInfo & data)
{
	Athena::CUDSqlCallBack<CRechargeMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRechargeMgr>(this,&CRechargeMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_rechargeInfoDef,(const char *)&data,"RECHARGE_INFO",autoID,pDelaySql);
}

void CRechargeMgr::addRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CRechargeMgr::updateRecord(stRechargeDBInfo & data)
{
	char where[65] = { 0 };
	bzero(where, 65);	
	snprintf(where, 64, "CHARID=%d",data.m_charID);
	Athena::CUDSqlCallBack<CRechargeMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRechargeMgr>(this,&CRechargeMgr::updateRecordCB,false,0);	
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);	
	g_databaseEngine.updateRecord(g_rechargeInfoDef,(const char *)&data,"RECHARGE_INFO",where,pDelaySql);	
}

void CRechargeMgr::updateRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CRechargeMgr::updateObtainFlag(UINT32 charID)
{
	UINT8 data = 1;
	char where[65] = { 0 };
	bzero(where, 65);	
	snprintf(where, 64, "CHARID=%d",charID);
	Athena::CUDSqlCallBack<CRechargeMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRechargeMgr>(this,&CRechargeMgr::updateObtainFlagCB,false,0);	
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);	
	g_databaseEngine.updateRecord(g_updateObtainFlagDef,(const char *)&data,"RECHARGE_INFO",where,pDelaySql);	
}

void CRechargeMgr::updateObtainFlagCB(bool opResult,UINT64 autoID)
{
	
}

void CRechargeMgr::delRecord(UINT32 charID)
{
	char where[65] = { 0 };
	bzero(where, 65);	
	snprintf(where, 64, " CHARID=%d ", charID);
	Athena::CUDSqlCallBack<CRechargeMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRechargeMgr>(this,&CRechargeMgr::delRecordCB,false,0);			
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("RECHARGE_INFO",where,pDelaySql);
}

void CRechargeMgr::delRecordCB(bool opResult,UINT64 autoID)
{}

CRechargeMgr::CRechargeMgr()
{
	
}

CRechargeMgr::~CRechargeMgr()
{

}

void CRechargeMgr::userOnline(Player * pUser)
{
	if (!pUser){
		return ;
	}

	std::map<UINT32,std::vector<stRechargeInfo> >::iterator it =  m_userRechargeMap.find(pUser->getID());
	if (it != m_userRechargeMap.end()) {
		Global::stUserRecharge retCmd;
		retCmd.set_charid(pUser->getID());
		for (UINT16 i = 0;i< it->second.size();++i){
			stRechargeInfo & info = it->second[i];
			Global::stRechargeInfo * pSyn = retCmd.add_rechargeinfo();
			if (pSyn){
				pSyn->set_itemid(info.m_itemID);
				pSyn->set_money(info.m_money);
				pSyn->set_tid(info.m_tid);
#ifdef _YUNDING_TONGJI
				if(info.m_isExtend){
					pSyn->set_data(info.m_data);
					retCmd.set_isextend(info.m_isExtend);
				}
#endif
			}
		}	
		pUser->sendProto2Game(retCmd);
		m_userRechargeMap.erase(it);
		updateObtainFlag(pUser->getID());
	}
}

void CRechargeMgr::onlineRecharge(Player * pUser,UINT32 money,UINT32 itemID,const std::string & tid, bool isExtend, const std::string & data)
{
	Global::stUserRecharge retCmd;
	retCmd.set_charid(pUser->getID());
	Global::stRechargeInfo * pSyn = retCmd.add_rechargeinfo();
	if (pSyn){
		pSyn->set_itemid(itemID);
		pSyn->set_money(money);
		pSyn->set_tid(tid);
#ifdef _YUNDING_TONGJI
		if(isExtend){
			pSyn->set_data(data);
			retCmd.set_isextend(isExtend);
		}
#endif
	}

	pUser->sendProto2Game(retCmd);
	stRechargeInfo info ;
	info.m_bObtain = 0;
	info.m_charID = pUser->getID();
	info.m_itemID = itemID;
	info.m_money = money;
#ifdef _YUNDING_TONGJI
	info.m_isExtend = isExtend;
	strncpy(info.m_data, data.c_str(), sizeof(info.m_data));
#endif
	strncpy(info.m_tid,tid.c_str(),sizeof(info.m_tid));
	stRechargeDBInfo dbData;
	info.fillDBData(dbData);
	addRecord(dbData);
}

void CRechargeMgr::offlineRecharge(UINT32 charID,UINT32 money,UINT32 itemID,const std::string & tid, bool isExtend, const std::string & data)
{
	std::vector<stRechargeInfo> & myRechargeInfos  =  m_userRechargeMap[charID];
	stRechargeInfo info ;
	info.m_bObtain = 0;
	info.m_charID = charID;
	info.m_itemID = itemID;
	info.m_money = money;
#ifdef _YUNDING_TONGJI
	info.m_isExtend = isExtend;
	strncpy(info.m_data,data.c_str(),sizeof(info.m_data));
#endif
	strncpy(info.m_tid,tid.c_str(),sizeof(info.m_tid));
	myRechargeInfos.push_back(info);
	stRechargeDBInfo dbData;
	info.fillDBData(dbData);
	addRecord(dbData);
}

void CRechargeMgr::addTid(std::string tid)
{
	m_tidCaches.insert(tid);	
}

bool CRechargeMgr::checkTid(std::string tid)
{
	if (m_tidCaches.find(tid) != m_tidCaches.end()){
		return true;
	}
	else {
		return false;
	}
}
