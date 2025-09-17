#include "HistoryRankMgr.h"
#include "GameUser.h"
#include "GameUserManager.h"


CHistoryRankMgr::CHistoryRankMgr()
{

}

CHistoryRankMgr::~CHistoryRankMgr()
{
	
}

void CHistoryRankMgr::cancelOldUserTitle()
{
	std::map<UINT16,stHistoryRankInfo>::iterator it = m_moduleRanks.begin();
	for (;it != m_moduleRanks.end();++it){
		std::map<UINT32,UINT16>::iterator subIt = it->second.m_charID2Rank.begin();
		for (;subIt !=it->second.m_charID2Rank.end();++subIt){
			GameUser * pUser = GameUserManager::getSingleton().getUser(subIt->first);
			if (pUser){
				UINT32 titleID = pUser->m_titleMgr.getTitleIDByGotCond(getGotTitleTypeByModuleID(it->first),subIt->second);
				pUser->m_titleMgr.delTitle(titleID);
			}
		}
	}
}

UINT16 CHistoryRankMgr::getGotTitleTypeByModuleID(UINT16 moduleID)
{
	switch (moduleID){
		case Global::eRankModule_Level_Rank :         //等级排名
			{
				return eTitleGotType_LevelRank;
			}
			break;
		case Global::eRankModule_BattlePower_Rank:    //战力排名
			{
				return eTitleGotType_BattlePowerRank;
			}
			break;
		case Global::eRankModule_Copymap_Rank :       //副本排行
			{
				return eTitleGotType_CopymapRank;
			}
			break;
		case Global::eRankModule_One2One_Rank :       //巅峰排名 
			{
				return eTitleGotType_One2OnePVPRank;
			}
			break;
		case Global::eRankModule_Leitai_Rank  :       //擂台排行 
			{
				return eTitleGotType_LeitaiRank;
			}
			break;
		case Global::eRankModule_Arena_Rank :   //竞技场
			{
				return eTitleGotType_ArenaRank; 
			}
			break;
	}

	return 0;
}

void CHistoryRankMgr::initHistoryRankInfo(Global::stSynRankModuleInfo * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	cancelOldUserTitle();
	m_moduleRanks.clear();

	for (UINT16 i = 0;i < recvCmd->data_size();++i){
		const Global::stRankModuleInfo & info = recvCmd->data(i);	
		stHistoryRankInfo & rankInfo = m_moduleRanks[info.moduleid()];
		rankInfo.m_moduleID = info.moduleid();
		for (UINT16 j = 0;j < info.charid2rank_size();++j){
			const Global::stCharID2Rank & charID2Rank = info.charid2rank(j);
			rankInfo.m_charID2Rank.insert(std::make_pair(charID2Rank.charid(),charID2Rank.rank()));
			GameUser * pUser = GameUserManager::getSingleton().getUser(charID2Rank.charid());
			if (pUser){
				pUser->m_titleMgr.checkObtainTitle(getGotTitleTypeByModuleID(rankInfo.m_moduleID),charID2Rank.rank(),false);			
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("模块:id=%d,charID:%d,rank:%d",rankInfo.m_moduleID,charID2Rank.charid(),charID2Rank.rank());
#endif
		}
	}
}

void CHistoryRankMgr::checkGotTitle(GameUser * pUser)
{
	if (!pUser){
		return ;	
	}	
	
	std::map<UINT16,stHistoryRankInfo>::iterator it = m_moduleRanks.begin();

	for (;it != m_moduleRanks.end();++it){
		std::map<UINT32,UINT16>::iterator subIt = it->second.m_charID2Rank.begin();
		for (;subIt !=it->second.m_charID2Rank.end();++subIt){
			if (pUser->getID() == subIt->first){
				pUser->m_titleMgr.checkObtainTitle(getGotTitleTypeByModuleID(it->first),subIt->second,false,false);
			}
		}
	}
}
