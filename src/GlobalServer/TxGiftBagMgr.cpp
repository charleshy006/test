#include "TxGiftBagMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"

CTblField g_txGiftBagTblDef[] = {
	{ "CHARID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "TASKID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "BILLNO",      CTblField::DB_TYPE_STRING,  50 			  },  
	{ NULL, 0, 0 },
};

CTxGiftBagMgr::~CTxGiftBagMgr()
{

}

bool CTxGiftBagMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_txGiftBagTblDef," TX_GIFT_BAG_AWARD ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stTxGiftBagAwardDB * pDBData = (stTxGiftBagAwardDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			m_awardInfos[pDBData[i].m_charID].insert(pDBData[i].m_taskID);
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了%d个玩家领取礼包信息",m_awardInfos.size());
#endif
	return true;
}

void CTxGiftBagMgr::addRecord(stTxGiftBagAwardDB & data)
{
	Athena::CUDSqlCallBack<CTxGiftBagMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CTxGiftBagMgr>(this,&CTxGiftBagMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_txGiftBagTblDef,(const char *)&data,"TX_GIFT_BAG_AWARD",autoID,pDelaySql);
}

void CTxGiftBagMgr::addRecordCB(bool opResult,UINT64 autoID)
{
}

void CTxGiftBagMgr::addGotGiftBagInfo(UINT32 charID,UINT32 taskID,const char * billNo)
{
	if (!billNo){
		return ;
	}
	stTxGiftBagAwardDB data;
	data.m_charID  = charID;
	data.m_taskID  = taskID;
	strncpy(data.m_billNo,billNo,sizeof(data.m_billNo));
	addRecord(data);
	m_awardInfos[charID].insert(taskID);
}

bool CTxGiftBagMgr::hasGotGiftBag(UINT32 charID,UINT32 taskID)
{
	std::map<UINT32,std::set<UINT32> >::iterator it = m_awardInfos.find(charID);
	if (it != m_awardInfos.end()){
		if (it->second.find(taskID) != it->second.end()){
			return true;	
		}	
		else {
			return false;
		}
	}
	else {
		return false;
	}
}
