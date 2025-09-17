#include "HistoryRankMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "GlobalVar.h"
#include "RankMgr.h"
#include "One2OnePVP.h"
#include "Arena.h"

CTblField g_historyRankTblDef[] = {
	{ "MODULE_ID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "INFO",    		CTblField::DB_TYPE_STRING,  100  			 },  
	{ NULL, 0, 0 },
};

void stHistoryRankInfo::initFromDB(const stHistoryRankInfoDB & data)
{
	m_moduleID = data.m_moduleID;
	std::vector<std::string> strVec;
	g_stringtok(strVec,data.m_info,";");

	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string> subStrVec;
		g_stringtok(subStrVec,strVec[i],"-");
		if (subStrVec.size() >= 2){
			UINT32 charID = (UINT32)atoi(subStrVec[0].c_str());
			UINT16 rank = (UINT16)atoi(subStrVec[1].c_str());
			m_charID2Rank[charID] = rank;
		}
	}
}

void stHistoryRankInfo::fillDBData(stHistoryRankInfoDB & data)
{
	data.m_moduleID = m_moduleID;
	std::ostringstream os;
	
	std::map<UINT32,UINT16>::iterator it = m_charID2Rank.begin();
	
	bool first = true;
	for (;it != m_charID2Rank.end();++it){
		if (first){
			first = false;
			os << it->first << "-" << it->second;
		}
		else {
			os << ";" << it->first << "-" << it->second;
		}
	}
	strncpy(data.m_info,os.str().c_str(),sizeof(data.m_info));
}

CHistoryRankMgr::~CHistoryRankMgr()
{

}

bool CHistoryRankMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_historyRankTblDef," MODULE_HISTORY_RANK ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stHistoryRankInfoDB * pDataDB = (stHistoryRankInfoDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stHistoryRankInfo info;
			info.initFromDB(pDataDB[i]);
			m_moduleRanks[info.m_moduleID] = info;
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("打印模块的历史排行:");
		std::map<UINT32,stHistoryRankInfo >::iterator it = m_moduleRanks.begin();

		for (;it != m_moduleRanks.end();++it){
			stHistoryRankInfo & info = it->second;
			Athena::logger->trace("moduleID:%d ",info.m_moduleID);

			std::map<UINT32,UINT16>::iterator subIt = info.m_charID2Rank.begin();
			for (;subIt != info.m_charID2Rank.end();++subIt){
				Athena::logger->trace("charID:%d,rank:%d",subIt->first,subIt->second);
			}
		}
	}
#endif
	return true;
}

void CHistoryRankMgr::addRecord(const stHistoryRankInfoDB & data)
{
	Athena::CUDSqlCallBack<CHistoryRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHistoryRankMgr>(this,&CHistoryRankMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_historyRankTblDef,(const char *)&data,"MODULE_HISTORY_RANK",autoID,pDelaySql);
}

void CHistoryRankMgr::addRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CHistoryRankMgr::updateRecord(const stHistoryRankInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " MODULE_ID=%u", data.m_moduleID);

	Athena::CUDSqlCallBack<CHistoryRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHistoryRankMgr>(this,&CHistoryRankMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_historyRankTblDef,(const char *)&data,"MODULE_HISTORY_RANK",where,pDelaySql);
}

void CHistoryRankMgr::updateRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CHistoryRankMgr::save(stHistoryRankInfo & info)
{
	std::map<UINT32,stHistoryRankInfo >::iterator it = m_moduleRanks.find(info.m_moduleID);	
	if (it == m_moduleRanks.end()){
		stHistoryRankInfoDB data;
		info.fillDBData(data);
		addRecord(data);
		m_moduleRanks.insert(std::make_pair(info.m_moduleID,info));
	}
	else {
		stHistoryRankInfoDB data;
		info.fillDBData(data);
		updateRecord(data);
		it->second = info;
	}
}

void CHistoryRankMgr::timer(UINT32 cur)
{
	UINT32 refreshTime = CGlobalVarMgr::getSingleton().getValue(eGlobalVar_Refresh_HistoryRank);

	if (!g_isTheSameDay(refreshTime,cur)){//不在同一天开始刷
		CGlobalVarMgr::getSingleton().setValue(eGlobalVar_Refresh_HistoryRank,cur);
		stHistoryRankInfo info;
		info.m_moduleID = Global::eRankModule_Level_Rank;
		CRankMgr::getSingleton().getLevelSortListTop3(info);
		save(info);
		info.reset();
		
		info.m_moduleID = Global::eRankModule_BattlePower_Rank;
		CRankMgr::getSingleton().getBattlePowerSortListTop3(info);	
		save(info);
		info.reset();

		info.m_moduleID = Global::eRankModule_Copymap_Rank;
		CRankMgr::getSingleton().getCopymapSortListTop3(info);
		save(info);
		info.reset();
		
		info.m_moduleID = Global::eRankModule_Leitai_Rank ;
		CRankMgr::getSingleton().getLeitaiSortListTop3(info);
		save(info);
		info.reset();

		info.m_moduleID = Global::eRankModule_One2One_Rank;
		COne2OnePVPMgr::getSingleton().getTop3(info);
		save(info);

		info.m_moduleID = Global::eRankModule_Arena_Rank;
		ArenaMgr::getSingleton().getTop3(info);
		save(info);
		Athena::logger->trace("开始刷新各种排名!");
		broadcastRankInfo();
	}
}

void CHistoryRankMgr::broadcastRankInfo()
{
	Global::stSynRankModuleInfo retCmd;
	std::map<UINT32,stHistoryRankInfo >::iterator it = m_moduleRanks.begin();
	for (;it != m_moduleRanks.end();++it){
		stHistoryRankInfo & rankInfo = it->second;
		Global::stRankModuleInfo * pSyn = retCmd.add_data();
		if (pSyn){
			pSyn->set_moduleid(it->first);
			std::map<UINT32,UINT16>::iterator subIt = rankInfo.m_charID2Rank.begin();
			for (;subIt != rankInfo.m_charID2Rank.end();++subIt){
				Global::stCharID2Rank * pSubSyn = pSyn->add_charid2rank();
				if (pSubSyn) {
					pSubSyn->set_charid(subIt->first);
					pSubSyn->set_rank(subIt->second);
				}
			}
		}
	}
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,retCmd);
}
