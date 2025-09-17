#include "One2OnePVP.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "SortList.pb.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "GlobalVar.h"
#include "Mail.h"
#include "RankMgr.h"
#include "ArenaConfig.h"
#include "PlayerManager.h"
#include <tinyxml/tinyxml.h>
#include "SysLanguages.h"

CTblField g_One2OnePVPTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "SCORE",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "JOB",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,   sizeof(UINT16)  },
	{ NULL, 0, 0 },
};

CTblField g_One2OnePVPLastWeekTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "SCORE",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "RANK",           CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "JOB",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",          	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,   sizeof(UINT16)  },
	{ NULL, 0, 0 },
};

CTblField g_one2OnePVPHistorySortList[] = {
	{ "ID",                             CTblField::DB_TYPE_UINT32,  		sizeof(UINT32)   	 },
	{ "RANK_TYPE",     					CTblField::DB_TYPE_UINT8,			sizeof(UINT8)    	 },
	{ "REWARD_TIMESTAMP",               CTblField::DB_TYPE_UINT32,  		sizeof(UINT32)   	 },
	{ "BINARYDATA",                     CTblField::DB_TYPE_BLOB,   ONE2ONE_PVP_REWARD_INFO_LEN 	 },
	{ NULL, 0, 0 },
};  

CTblField g_One2OnePvPHasGotRewardCharIDTblDef[] = {
	//{ "ID",                             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "REWARD_TIMESTAMP",               CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BINARYDATA",                     CTblField::DB_TYPE_BLOB,   ONE2ONE_PVP_REWARD_INFO_LEN },
	{ NULL, 0, 0 },
};

void stOne2OnePVPHistorySortList::initFromDB(const stOne2OnePVPHistorySortListDB & data)
{
	m_id = data.m_id;
	m_rankType = data.m_rankType;
	m_rewardTimestamp = data.m_rewardTimestamp;

	UINT16 num = *(UINT16 *)(data.m_data);
	UINT32 * pCharID = (UINT32 *)(data.m_data + sizeof(UINT16));

	for (UINT16 i = 0;i < num;++i){
		UINT32 charID = pCharID[i];
		m_charID2Rank[charID] = i + 1;
	}
}

void stOne2OnePVPHistorySortList::fillDBData(stOne2OnePVPHistorySortListDB & data)
{
	
}


void COne2OnePVPMgr::load()
{
	CTblQueryResult* result = g_databaseEngine.select(g_One2OnePVPTblDef,"ONE2ONEPVP_RANK",NULL,NULL,0,NULL);	
	if (result){
		if (result->getData()){
			stOne2OnePVPDB *  pInfoDB = (stOne2OnePVPDB *) result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stOne2OnePVPInfo info;
				info.m_charID = pInfoDB[i].m_charID;
				info.m_score = pInfoDB[i].m_score;
				strncpy(info.m_name,pInfoDB[i].m_name,sizeof(info.m_name));
				info.m_job = pInfoDB[i].m_job;
				info.m_sex = pInfoDB[i].m_sex;
				info.m_photoID = pInfoDB[i].m_photoID;
				m_scoreSortList.insertSortInfo(info);
			}
		}
	}
	SAFE_DELETE(result);

	result = g_databaseEngine.select(g_One2OnePVPLastWeekTblDef,"ONE2ONEPVP_RANK_LASTWEEK",NULL,NULL,0,NULL);
	
	if (result){
		if (result->getData()){
			stOne2OnePVPLastWeekDB * pInfoDB = (stOne2OnePVPLastWeekDB *)result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stOne2OnePVPLastWeekInfo info ;
				info.initFromDB(pInfoDB[i]);
				m_lastweekTopN.push_back(info);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("巅峰排行上周第%d名为charID=%d,name=%s",info.m_rank,info.m_charID,info.m_name);
#endif
			}
		}
	}

	SAFE_DELETE(result);
	
	result = g_databaseEngine.select(g_One2OnePvPHasGotRewardCharIDTblDef,"ONE2ONE_PVP_GOT_REWARD_CHARID",NULL,NULL,0,NULL);
	
	if (result){
		if (result->getData()){
			stOne2OnePVPHasGotRewardCharIDs * pInfoDB = (stOne2OnePVPHasGotRewardCharIDs *)result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				UINT16 num = *(UINT16 *)(pInfoDB[i].m_data);
				UINT32 *charID = (UINT32 *)(pInfoDB[i].m_data + sizeof(UINT16));
				for (UINT16 j = 0;j < num ; ++j){
					m_gotRewardCharIDs[pInfoDB[i].m_rewardTimestamp].insert(charID[j]);	
#ifdef _HDZ_DEBUG
				Athena::logger->trace("巅峰排行上周第timestamp:%d,已经领取的CHARID:%d",pInfoDB[i].m_rewardTimestamp,charID[j]);
#endif
				}
			}
		}
	}

	SAFE_DELETE(result);

	result = g_databaseEngine.select(g_one2OnePVPHistorySortList,"HISTORY_ONE2ONEPVP_SORTLIST",NULL,NULL,0,NULL);
	if (result){
		if (result->getData()){
			stOne2OnePVPHistorySortListDB * pInfoDB = (stOne2OnePVPHistorySortListDB *)result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stOne2OnePVPHistorySortList info;
				info.initFromDB(pInfoDB[i]);
				std::set<UINT32>  gotRewardCharIDs;
				if (m_gotRewardCharIDs.find(info.m_rewardTimestamp) != m_gotRewardCharIDs.end()){
					gotRewardCharIDs = m_gotRewardCharIDs[info.m_rewardTimestamp];
				}
				std::set<UINT32>::iterator it = gotRewardCharIDs.begin();
				for (;it != gotRewardCharIDs.end();++it){
					info.m_charID2Rank.erase(*it);	
				}
				m_historyRankRewards[info.m_rewardTimestamp] = info;
#ifdef _HDZ_DEBUG
				Athena::logger->trace("rankType:%d,timestamp:%d,size:%d",info.m_rankType,info.m_rewardTimestamp,info.m_charID2Rank.size());
#endif
			}
		}
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("区:%d 巅峰排行榜记录数为%d",m_scoreSortList.getListNum());
	}
#endif
}

void COne2OnePVPMgr::addRecord(const char * pData)
{
	Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_One2OnePVPTblDef,pData,"ONE2ONEPVP_RANK",autoID,pDelaySql);
}

void COne2OnePVPMgr::addRecordCB(bool opResult,UINT64 autoID)
{}
	
void COne2OnePVPMgr::addRewardRecord(stOne2OnePVPHistorySortListDB & data)
{
	Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::addRewardRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_one2OnePVPHistorySortList,(const char *)&data,"HISTORY_ONE2ONEPVP_SORTLIST",autoID,pDelaySql);
}

void COne2OnePVPMgr::addRewardRecordCB(bool opResult,UINT64 autoID)
{
	
}

void COne2OnePVPMgr::updateRecord(UINT32 key,const char * pData)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",key);
	
	Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_One2OnePVPTblDef,pData,"ONE2ONEPVP_RANK",where,pDelaySql);
}

void COne2OnePVPMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void COne2OnePVPMgr::removeRecord(UINT32 key)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%d",key);
	Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("ONE2ONEPVP_RANK",where,pDelaySql);
}

void COne2OnePVPMgr::removeRewardRecord(UINT32 timestamp)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "REWARD_TIMESTAMP=%d",timestamp);
	Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::removeRewardRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("HISTORY_ONE2ONEPVP_SORTLIST",where,pDelaySql);
}

void COne2OnePVPMgr::removeRewardRecordCB(bool opResult,UINT64 autoID)
{}

void COne2OnePVPMgr::removeRecordCB(bool opResult,UINT64 autoID)
{

}

void COne2OnePVPMgr::removeAllRecord()
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID > 0 ");
	Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::removeAllRecordeCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("ONE2ONEPVP_RANK",where,pDelaySql);
}

void COne2OnePVPMgr::removeAllRecordeCB(bool opResult,UINT64 autoID)
{
	
}

void COne2OnePVPMgr::sendScoreSortList(Player * pUser,UINT16 beginRank,SortProto::eOne2OneSortListType type)
{
	if (!pUser){
		return ;
	}

	SortProto::stRetOne2OnePVPSortList retCmd;
	retCmd.set_etype(type);
	retCmd.set_score(getScore(pUser->getID()));	
	std::vector<stOne2OnePVPInfo> vec;

	m_scoreSortList.getFromN12N2(vec,beginRank,beginRank + s_SEND_SORTITEM_MAX);	
	retCmd.set_myrank(m_scoreSortList.getMySortRank(pUser->getID()));
	for (UINT8 i = 0;i < vec.size();++i){
		SortProto::stSynOne2OnePVPInfo * pSortItem = retCmd.add_sortlist();
		if (pSortItem){
			pSortItem->set_charid(vec[i].m_charID);
			pSortItem->set_name(vec[i].m_name);
			pSortItem->set_score(vec[i].m_score);
			pSortItem->set_job(vec[i].m_job);
			pSortItem->set_sex(vec[i].m_sex);
			pSortItem->set_rank(beginRank + i);
			pSortItem->set_photoid(vec[i].m_photoID);
			pSortItem->set_level(CRankMgr::getSingleton().getLevelByCharID(vec[i].m_charID));
		}
	}
	
	for (UINT8 i = 0;i < m_lastweekTopN.size();++i){
		stOne2OnePVPLastWeekInfo & info = m_lastweekTopN[i];
		SortProto::stSynOne2OnePVPInfo * pSortItem = retCmd.add_lastweeksortlist();
		if (pSortItem){
			pSortItem->set_charid(info.m_charID);
			pSortItem->set_name(info.m_name);
			pSortItem->set_score(info.m_score);
			pSortItem->set_job(info.m_job);
			pSortItem->set_sex(info.m_sex);
			pSortItem->set_rank(info.m_rank);
			pSortItem->set_photoid(info.m_photoID);
			pSortItem->set_level(CRankMgr::getSingleton().getLevelByCharID(info.m_charID));
		}
	}

	pUser->sendProtoToMe(retCmd);
}

void COne2OnePVPMgr::timer(UINT32 cur)
{
	UINT32 refreshTime = CGlobalVarMgr::getSingleton().getValue(eGlobalVar_Refresh_One2OneRank);
	UINT32 weekRefreshTime = CGlobalVarMgr::getSingleton().getValue(eGlobalVar_Refresh_Week_One2OneRank);
	
	struct tm curTm;
    RealTime::getLocalTime(curTm,cur);
	if (curTm.tm_hour == 1){
		if ((!g_isTheSameWeek(weekRefreshTime,cur)) && (curTm.tm_wday == 0)){//不同周,先检查不周
			CGlobalVarMgr::getSingleton().setValue(eGlobalVar_Refresh_Week_One2OneRank,cur);	
			refreshLastWeekData(cur);
		}
		else if (!g_isTheSameDay(refreshTime,cur)){//不同天
			CGlobalVarMgr::getSingleton().setValue(eGlobalVar_Refresh_One2OneRank,cur);
			refreshYestodayData(cur);
		}	

		while (m_historyRankRewards.size() > s_MAX_REWARD_DAY){
			std::map<UINT32,stOne2OnePVPHistorySortList>::iterator it = m_historyRankRewards.begin();
			if (it != m_historyRankRewards.end()){
				removeRewardRecord(it->first);
				m_gotRewardCharIDs.erase(it->first);

				char where[65] = { 0 };
				bzero(where, 65);
				snprintf(where, 64, "REWARD_TIMESTAMP=%d",it->first);
				Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::removeGotRewardCharIDRecordCB,false,0);
				CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
				g_databaseEngine.deleteRecord("ONE2ONE_PVP_GOT_REWARD_CHARID",where,pDelaySql);

#ifdef _HDZ_DEBUG
				Athena::logger->trace("删除了巅峰历史排名奖励:key=%d",it->first);
#endif
				m_historyRankRewards.erase(it);
			}
		}
	}

	if (m_lastCheckTime + s_CHECK_MATCH_INTERVAL < g_globalServer->getSec()){
		m_lastCheckTime = g_globalServer->getSec();
		//checkMatch();
		checkMatchNew();
	}
}

void COne2OnePVPMgr::removeGotRewardCharIDRecordCB(bool opResult,UINT64 autoID)
{}

void COne2OnePVPMgr::refreshYestodayData(UINT32 cur)
{
	Athena::logger->trace("刷新了昨日数据!");
	std::vector<stOne2OnePVPInfo> vec;
	m_scoreSortList.getTopN(vec,s_MAX_REWARD_NUM);
	
	stOne2OnePVPHistorySortListDB dbData;
	dbData.m_rankType = eOne2OnePVP_Day_Rank;
	dbData.m_rewardTimestamp = cur;

	*(UINT16 *)(dbData.m_data) = vec.size();
	UINT32 * pCharID = (UINT32 *)(dbData.m_data + sizeof(UINT16));

	for (UINT16 i = 0;(i < vec.size()) && (i < s_MAX_REWARD_NUM); ++i){
		Player * pUser = PlayerManager::getSingleton().getUserByID(vec[i].m_charID);
		if (pUser){
			stBatchSendMailTask task;
			char szMailContent[50] = { 0 };
			task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_ONETOONE);
			snprintf(szMailContent,sizeof(szMailContent),"MailWnd_9;%d",i + 1); 
			task.m_content = szMailContent;
			task.m_receiveIDs.insert(vec[i].m_charID);
			stEverydayReward *pCfg =  getEverydayRewardByRank(i + 1);
			if (pCfg){
				if (pCfg->m_itemID){
					task.m_attaches.push_back(std::make_pair(pCfg->m_itemID,pCfg->m_num));	
				}	
			}
			CBatchSendMailMgr::getSingleton().addTask(task);
		}
		else {
			pCharID[i] = vec[i].m_charID;
		}
	}

	addRewardRecord(dbData);
	m_historyRankRewards[cur].initFromDB(dbData);	
}

void COne2OnePVPMgr::refreshLastWeekData(UINT32 cur)
{
	Athena::logger->trace("刷新了周数据!");
	std::vector<stOne2OnePVPInfo> vec;
	m_scoreSortList.getTopN(vec,3);
	m_lastweekTopN.clear();

	for (UINT16 i = 0;i < vec.size();++i){
		stOne2OnePVPInfo & one1 = vec[i];
		stOne2OnePVPLastWeekInfo one2;
		one2.m_charID = one1.m_charID;
		one2.m_job = one1.m_job;
		strncpy(one2.m_name,one1.m_name,sizeof(one2.m_name));
		one2.m_rank = i + 1;
		one2.m_score = one1.m_score;
		one2.m_sex = one1.m_sex;
		one2.m_photoID = one1.m_photoID;
		m_lastweekTopN.push_back(one2);		
	}

	vec.clear();
	m_scoreSortList.getTopN(vec,s_MAX_REWARD_NUM);
	
	stOne2OnePVPHistorySortListDB dbData;
	dbData.m_rankType = eOne2OnePVP_Week_Rank;
	dbData.m_rewardTimestamp = cur;

	*(UINT16 *)(dbData.m_data) = vec.size();
	UINT32 * pCharID = (UINT32 *)(dbData.m_data + sizeof(UINT16));

	for (UINT16 i = 0;(i < vec.size()) && (i < s_MAX_REWARD_NUM); ++i){
		Player * pUser = PlayerManager::getSingleton().getUserByID(vec[i].m_charID);
		if (pUser){
			stBatchSendMailTask task;
			char szMailContent[50] = { 0 };
			task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM);
			snprintf(szMailContent,sizeof(szMailContent),"MailWnd_10;%d",i + 1); 
			task.m_content = szMailContent;
			task.m_receiveIDs.insert(vec[i].m_charID);
			stWeekReward *pCfg = getWeekRewardByRank(i + 1);
			if (pCfg->m_itemID1){
				task.m_attaches.push_back(std::make_pair(pCfg->m_itemID1,pCfg->m_num1));
			}

			if (pCfg->m_itemID2){
				task.m_attaches.push_back(std::make_pair(pCfg->m_itemID2,pCfg->m_num2));
			}
			
			if (pCfg->m_itemID3){
				task.m_attaches.push_back(std::make_pair(pCfg->m_itemID3,pCfg->m_num3));
			}
			CBatchSendMailMgr::getSingleton().addTask(task);
		}
		else {
			pCharID[i] = vec[i].m_charID;
		}
	}

	m_scoreSortList.clear();
	removeAllRecord();
	addRewardRecord(dbData);
	m_historyRankRewards[cur].initFromDB(dbData);

	Global::stNotifyClearOne2OneScore cmd;
	cmd.set_flag(1);
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

void COne2OnePVPMgr::addMatchUser(Player * pUser)
{
	if (!pUser){
		return ;
	}	
	
	SortProto::stOne2OnePVPMatchResult retCmd;
	struct tm _tm; 
	RealTime::getLocalTime(_tm,g_globalServer->getSec());
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

	if (!ArenaConfig::getSingleton().isOne2OneOpen(secNum)){//时间未到
		retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eNot_In_Time);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	if (pUser->m_level < 18){
		retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eFail);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	UINT32 myScore = getScore(pUser->getID());
	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin();
	
	bool bMatch = false;

	for (;it != m_waitMatchUsers.end();++it){
		stMatchCfg & matchInfo = it->second;
		if ((myScore >= matchInfo.m_startPoint) && (myScore <= matchInfo.m_endPoint)){
			matchInfo.m_waitCharIDs.insert(std::make_pair(pUser->getID(),g_globalServer->getSec()));
			bMatch = true;
			break;
		}
	}
	if (!bMatch){
		Athena::logger->error("one2One匹配错误!");
		return ;
	}
	retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eWait);
	pUser->sendProtoToMe(retCmd);

}

void COne2OnePVPMgr::cancelMatch(Player * pUser)
{
	if (!pUser){
		return ;
	}

	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin();
	for (;it != m_waitMatchUsers.end();++it){
		it->second.m_waitCharIDs.erase(pUser->getID());	
	}
	SortProto::stCancelOne2OnePVPResult retCmd;
	retCmd.set_result(SortProto::stCancelOne2OnePVPResult::eSuccess);
	pUser->sendProtoToMe(retCmd);
}

UINT32 COne2OnePVPMgr::getScore(UINT32 charID)
{
	stOne2OnePVPInfo info = m_scoreSortList.getSortInfo(charID);
	return info.m_score;
}

void COne2OnePVPMgr::updateScore(Player * pUser,UINT32 score)
{
	if (!pUser){
		return ;
	}

	if (m_scoreSortList.isInSortList(pUser->getID())){
		stOne2OnePVPInfo info = m_scoreSortList.getSortInfo(pUser->getID()); 
		info.m_score = score;
		m_scoreSortList.updateSortInfo(info);
		stOne2OnePVPDB dbData;
		info.fillDBData(dbData);
		updateRecord(info.m_charID,(const char *)&dbData);
	}  
	else {
		stOne2OnePVPInfo info;
		info.m_charID = pUser->getID();
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_sex = pUser->m_sex;
		info.m_score = score;
		info.m_job = pUser->m_job;
		info.m_photoID = pUser->m_photoID;
		m_scoreSortList.insertSortInfo(info);
		stOne2OnePVPDB dbData;
		info.fillDBData(dbData);
		addRecord((const char *)&dbData);
	}   
}

void COne2OnePVPMgr::getTop3(stHistoryRankInfo & info)
{
	for (UINT16 i = 0;i < m_lastweekTopN.size();++i){
		stOne2OnePVPLastWeekInfo & data = m_lastweekTopN[i];
		info.m_charID2Rank.insert(std::make_pair(data.m_charID,i + 1));
	}
}
	
COne2OnePVPMgr::stEverydayReward * COne2OnePVPMgr::getEverydayRewardByRank(UINT32 rank)
{
	for (UINT32 i = 0;i < m_everydayRewards.size();++i){
		stEverydayReward & rewardCfg = m_everydayRewards[i];
		if ((rank >= rewardCfg.m_beginRank) && (rank <= rewardCfg.m_endRank)){
			return &m_everydayRewards[i];	
		}
	}	
	return NULL;
}

COne2OnePVPMgr::stWeekReward * COne2OnePVPMgr::getWeekRewardByRank(UINT32 rank)
{
	for (UINT16 i = 0;i < m_weekRewards.size();++i){
		stWeekReward & rewardCfg = m_weekRewards[i];
		if ((rank >= rewardCfg.m_beginRank) && (rank <= rewardCfg.m_endRank)){
			return &m_weekRewards[i];
		}
	}
	return NULL;
}


void g_sendOne2OnePVPMailCB(UINT64 autoID)
{

}

void COne2OnePVPMgr::userOnline(Player * pUser)
{
	std::map<UINT32,stOne2OnePVPHistorySortList>::iterator it = m_historyRankRewards.begin();	

	for (;it != m_historyRankRewards.end();++it){
		stOne2OnePVPHistorySortList & info = it->second;
		
		std::map<UINT32,UINT32>::iterator subIt = info.m_charID2Rank.find(pUser->getID());
		
		if (subIt != info.m_charID2Rank.end()) {
			stMailDBInfo mailInfo;
			mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
			mailInfo.m_receiverID = pUser->getID();
			strncpy(mailInfo.m_receiverName,pUser->getName(),sizeof(mailInfo.m_receiverName));
			strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
			strncpy(mailInfo.m_title,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_ONETOONE),sizeof(mailInfo.m_title));
			char mailCnt[250] = { 0 };
			if (eOne2OnePVP_Day_Rank == info.m_rankType){
				snprintf(mailCnt,sizeof(mailCnt),"MailWnd_9;%d",subIt->second);
			}
			else {
				snprintf(mailCnt,sizeof(mailCnt),"MailWnd_10;%d",subIt->second);
			}
			strncpy(mailInfo.m_content,mailCnt,sizeof(mailInfo.m_content));
			mailInfo.m_sendTime = g_globalServer->getSec();

			if (eOne2OnePVP_Day_Rank == info.m_rankType){//日排名
				stEverydayReward *pCfg =  getEverydayRewardByRank(subIt->second);
				if (pCfg){
					if (pCfg->m_itemID){
						mailInfo.m_attachID1 = pCfg->m_itemID;
						mailInfo.m_attachNum1 = pCfg->m_num;
					}	
				}
			}
			else if (eOne2OnePVP_Week_Rank == info.m_rankType){
				stWeekReward *pCfg = getWeekRewardByRank(subIt->second);
				if (pCfg->m_itemID1){
					mailInfo.m_attachID1 = pCfg->m_itemID1;
					mailInfo.m_attachNum1 = pCfg->m_num1;
				}

				if (pCfg->m_itemID2){
					mailInfo.m_attachID2 = pCfg->m_itemID2;
					mailInfo.m_attachNum2 = pCfg->m_num2;
				}

				if (pCfg->m_itemID3){
					mailInfo.m_attachID3 = pCfg->m_itemID3;
					mailInfo.m_attachNum3 = pCfg->m_num3;
				}
			}
			MailManager::getSingleton().sendMail(mailInfo,g_sendOne2OnePVPMailCB);
			info.m_charID2Rank.erase(subIt);
			saveGotRewardCharID(it->first,pUser->getID());
		}
	}
}

void COne2OnePVPMgr::saveGotRewardCharID(UINT32 timestamp,UINT32 charID)
{
	std::map<UINT32,std::set<UINT32> >::iterator it =  m_gotRewardCharIDs.find(timestamp);
	if (it == m_gotRewardCharIDs.end()){//还没有
		m_gotRewardCharIDs[timestamp].insert(charID);
		
		stOne2OnePVPHasGotRewardCharIDs data;
		data.m_rewardTimestamp = timestamp;
		*(UINT16 *)(data.m_data) = 1;
		UINT32 * pCharID = (UINT32 *)(data.m_data + sizeof(UINT16));
		pCharID[0] = charID;

		Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::saveGotRewardCharIDRecordCB,false,0);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		UINT64 autoID = 0;
		g_databaseEngine.insertRecord(g_One2OnePvPHasGotRewardCharIDTblDef,(const char *)&data,"ONE2ONE_PVP_GOT_REWARD_CHARID",autoID,pDelaySql);
	}
	else {
		it->second.insert(charID);	
		char where[65] = { 0 };
		bzero(where, 65);
		snprintf(where, 64, "REWARD_TIMESTAMP=%u",timestamp);
		
		std::set<UINT32>::iterator subIt = it->second.begin();
		stOne2OnePVPHasGotRewardCharIDs data;
		data.m_rewardTimestamp = timestamp;
		*(UINT16 *)(data.m_data) = it->second.size();
		UINT32 * pCharID = (UINT32 *)(data.m_data + sizeof(UINT16));

		for (UINT16 i = 0;(i < s_MAX_REWARD_NUM) && (subIt != it->second.end());++i,++subIt){
			pCharID[i] = *subIt;	
		}

		Athena::CUDSqlCallBack<COne2OnePVPMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<COne2OnePVPMgr>(this,&COne2OnePVPMgr::updateGotRewardCharIDecordCB,false,0);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		g_databaseEngine.updateRecord(g_One2OnePvPHasGotRewardCharIDTblDef,(const char *)&data,"ONE2ONE_PVP_GOT_REWARD_CHARID",where,pDelaySql);
	}
}
	
void COne2OnePVPMgr::saveGotRewardCharIDRecordCB(bool opResult,UINT64 autoID)
{
	
}

void COne2OnePVPMgr::updateGotRewardCharIDecordCB(bool opResult,UINT64 autoID)
{

}

void COne2OnePVPMgr::checkMatchReset()
{
	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >::iterator it =  m_waitState1Users.begin();
	for (;it != m_waitState1Users.end();++it){
		it->second.clear();	
	}

	it = m_waitState2Users.begin();
	for (;it != m_waitState2Users.end();++it){
		it->second.clear();
	}
	
	it = m_waitState3Users.begin();
	for (;it != m_waitState3Users.end();++it){
		it->second.clear();
	}

}

void COne2OnePVPMgr::checkMatchNew()
{
	checkMatchReset();
	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin();
	for (;it != m_waitMatchUsers.end();++it){
		stMatchCfg & matchInfo = it->second;
		
		if (eMatch_Type1 ==matchInfo.m_matchType){//类型一的通通匹配机器人
			std::map<UINT32,UINT32>::iterator subIt = matchInfo.m_waitCharIDs.begin();
			UINT16 matchNum = 0;
			for (;subIt != matchInfo.m_waitCharIDs.end();){
				std::map<UINT32,UINT32>::iterator subItTmp = subIt;
				++subIt;
				Global::stOne2OnePkWithRobot cmd;
				cmd.set_charid(subItTmp->first);
				UINT16 userLv = CRankMgr::getSingleton().getLevelByCharID(subItTmp->first);
				cmd.set_robotgroup(getRobotGroupID(eMatch_Type1,userLv));
				g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
				matchInfo.m_waitCharIDs.erase(subItTmp);
				++matchNum;
				if (matchNum >= 10){
					break;
				}
			}		
			continue;
		}
		
		//同一阶段的都匹配玩家先
		std::map<UINT32,UINT32>::iterator subIt1,subIt2;
		while (matchInfo.m_waitCharIDs.size() >= 2){
			UINT32 charID1 = 0, charID2 = 0;
			subIt1 = matchInfo.m_waitCharIDs.begin();
			if (subIt1 != matchInfo.m_waitCharIDs.end()){
				charID1 = subIt1->first;
			}
			++subIt1;
			if (subIt1 != matchInfo.m_waitCharIDs.end()){
				charID2 = subIt1->first;
			}

			if ((charID1 > 0) && (charID2 > 0 )){
				Global::stOne2OnePVPEnterCopyMap cmd;
				cmd.add_memberids(charID1);
				cmd.add_memberids(charID2);
				matchInfo.m_waitCharIDs.erase(charID1);
				matchInfo.m_waitCharIDs.erase(charID2);
				g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("one2One 匹配到玩家了1(%d,%d)!",charID1,charID2);
#endif
			}
		}
		
		//放到各个阶段
		std::map<UINT32,UINT32>::iterator subIt = matchInfo.m_waitCharIDs.begin();
		for (;subIt != matchInfo.m_waitCharIDs.end();++subIt){
			if (subIt->second + matchInfo.m_matchTimeState1 > g_globalServer->getSec()){//第一阶段
				m_waitState1Users[matchInfo.m_matchType].push_back(std::make_pair(subIt->first,matchInfo.m_id));
			}
			else if (subIt->second + matchInfo.m_matchTimeState2 > g_globalServer->getSec()){//第二阶段
				m_waitState2Users[matchInfo.m_matchType].push_back(std::make_pair(subIt->first,matchInfo.m_id));
			}
			else {
				m_waitState3Users[matchInfo.m_matchType].push_back(std::make_pair(subIt->first,matchInfo.m_id));
			}
		}
	}
	
	//处于第一阶段的怎么处理
	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >::iterator thdIt =  m_waitState1Users.begin();
	/*for (;thdIt != m_waitState1Users.end();++thdIt){
		for (UINT16 i = 0;i < thdIt->second.size();++i){
			
		}
	}*/
	
	
	//处于第二阶段的玩家
	thdIt =  m_waitState2Users.begin();
	for (;thdIt != m_waitState2Users.end();++thdIt){
		for (UINT16 i = 0;i < thdIt->second.size();++i){
			if (eMatch_Type1 == thdIt->first){//类型一不进行任何处理
			
			}	
			else if (eMatch_Type2 == thdIt->first){//类型一不进行任何处理,开始匹配机器人了
				Global::stOne2OnePkWithRobot cmd;
				cmd.set_charid(thdIt->second[i].first);
				UINT16 userLv = CRankMgr::getSingleton().getLevelByCharID(thdIt->second[i].first);
				cmd.set_robotgroup(getRobotGroupID(eMatch_Type2,userLv));
				g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
				it = m_waitMatchUsers.find(thdIt->second[i].second);
				if (it != m_waitMatchUsers.end()){
					it->second.m_waitCharIDs.erase(thdIt->second[i].first);
				}
			}
			else {//其它两种,到上下两级匹配人
				UINT16 id = thdIt->second[i].second;
				UINT16 from = id > 1 ? id - 1 : id;
				UINT16 to = id + 1;

				if (from < to) {
					for (;from <= to; ++from){
						it = m_waitMatchUsers.find(from);
						if (it != m_waitMatchUsers.end()){
							if (it->second.m_waitCharIDs.size() > 0){
								std::map<UINT32,UINT32>::iterator subIt = it->second.m_waitCharIDs.begin();
								if ((subIt != it->second.m_waitCharIDs.end()) && (thdIt->second[i].first != subIt->first)){
									Global::stOne2OnePVPEnterCopyMap cmd; 
									cmd.add_memberids(thdIt->second[i].first);
									cmd.add_memberids(subIt->first);
									it->second.m_waitCharIDs.erase(subIt);

									it = m_waitMatchUsers.find(id);
									if (it != m_waitMatchUsers.end()){
										it->second.m_waitCharIDs.erase(thdIt->second[i].first);
									}
									g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
#ifdef _HDZ_DEBUG
									Athena::logger->trace("one2One 匹配到玩家了2!");
#endif
								}
							}	
						}
					}
				}
			}
		}
	}

	//第三阶段一律放机器人
	thdIt =  m_waitState3Users.begin();
	for (;thdIt != m_waitState3Users.end();++thdIt){
		for (UINT16 i = 0;i < thdIt->second.size();++i){
			Global::stOne2OnePkWithRobot cmd;
			cmd.set_charid(thdIt->second[i].first);
			UINT16 userLv = CRankMgr::getSingleton().getLevelByCharID(thdIt->second[i].first);
			cmd.set_robotgroup(getRobotGroupID(thdIt->first,userLv));
			g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
			it = m_waitMatchUsers.find(thdIt->second[i].second);
			if (it != m_waitMatchUsers.end()){
				it->second.m_waitCharIDs.erase(thdIt->second[i].first);
			}
		}
	}
}

void COne2OnePVPMgr::checkMatch()
{
	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin();
	//小于8秒
	std::map<UINT32,UINT16> lessThan8;
	//大于8秒小于16
	std::map<UINT32,UINT16> from8to16;
	//小于16秒小于24
	std::map<UINT32,UINT16> from16to24;
	//大于24秒
	std::map<UINT32,UINT16> greatThan24;

	for (;it != m_waitMatchUsers.end();++it){
		stMatchCfg & matchInfo = it->second;
		std::map<UINT32,UINT32>::iterator subIt1,subIt2;
		
		while (matchInfo.m_waitCharIDs.size() >= 2){
			UINT32 charID1 = 0, charID2 = 0;
			subIt1 = matchInfo.m_waitCharIDs.begin();
			if (subIt1 != matchInfo.m_waitCharIDs.end()){
				charID1 = subIt1->first;
			}
			++subIt1;
			if (subIt1 != matchInfo.m_waitCharIDs.end()){
				charID2 = subIt1->first;
			}

			if ((charID1 > 0) && (charID2 > 0 )){
				Global::stOne2OnePVPEnterCopyMap cmd;
				cmd.add_memberids(charID1);
				cmd.add_memberids(charID2);
				matchInfo.m_waitCharIDs.erase(charID1);
				matchInfo.m_waitCharIDs.erase(charID2);
				g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("one2One 匹配到玩家了1(%d,%d)!",charID1,charID2);
#endif
			}
		}
		
		std::map<UINT32,UINT32>::iterator subIt = matchInfo.m_waitCharIDs.begin();
		for (;subIt != matchInfo.m_waitCharIDs.end();++subIt){
			if (subIt->second + 8 > g_globalServer->getSec()){//小于8秒
				lessThan8.insert(std::make_pair(subIt->first,it->first));	
			}
			else if (subIt->second + 16 > g_globalServer->getSec()){
				from8to16.insert(std::make_pair(subIt->first,it->first)); 	
			}
			else if (subIt->second + 24 > g_globalServer->getSec()){
				from16to24.insert(std::make_pair(subIt->first,it->first));
			}
			else {
				greatThan24.insert(std::make_pair(subIt->first,it->first));
			}
		}
	}
	
	std::map<UINT32,UINT16>::iterator thdIt  = from8to16.begin();
	for (;thdIt != from8to16.end();++thdIt){
		UINT16 id = thdIt->second;
		UINT16 from = id > 1 ? id - 1 : id;
		UINT16 to = id + 1;
		
		if (from <= to) {
			for (;from <= to; ++from){
				it = m_waitMatchUsers.find(from);
				if (it != m_waitMatchUsers.end()){
					if (it->second.m_waitCharIDs.size() > 0){
						std::map<UINT32,UINT32>::iterator subIt = it->second.m_waitCharIDs.begin();
						if ((subIt != it->second.m_waitCharIDs.end()) && (thdIt->first != subIt->first)){
							Global::stOne2OnePVPEnterCopyMap cmd; 
							cmd.add_memberids(thdIt->first);
							cmd.add_memberids(subIt->first);
							it->second.m_waitCharIDs.erase(subIt);

							it = m_waitMatchUsers.find(id);
							if (it != m_waitMatchUsers.end()){
								it->second.m_waitCharIDs.erase(thdIt->first);
							}
							g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("one2One 匹配到玩家了2!");
#endif
						}
					}	
				}
			}
		}
	}

	thdIt  = from16to24.begin();
	for (;thdIt != from16to24.end();++thdIt){
		UINT16 id = thdIt->second;
		UINT16 from = id > 2 ? id - 2 : 1;
		UINT16 to = id + 2;
		
		if (from <= to) {
			for (;from <= to; ++from){
				it = m_waitMatchUsers.find(from);
				if (it != m_waitMatchUsers.end()){
					if (it->second.m_waitCharIDs.size() > 0){
						std::map<UINT32,UINT32>::iterator subIt = it->second.m_waitCharIDs.begin();
						if ((subIt != it->second.m_waitCharIDs.end()) && (subIt->first != thdIt->first)){
							Global::stOne2OnePVPEnterCopyMap cmd; 
							cmd.add_memberids(thdIt->first);
							cmd.add_memberids(subIt->first);
							it->second.m_waitCharIDs.erase(subIt);
							
							it = m_waitMatchUsers.find(id);
							if (it != m_waitMatchUsers.end()){
								it->second.m_waitCharIDs.erase(thdIt->first);
							}

							g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("one2One 匹配到玩家了3!");
#endif
						}
					}	
				}
			}
		}
	}

	thdIt = greatThan24.begin();
	for (;thdIt != greatThan24.end();++thdIt){
		it = m_waitMatchUsers.begin();
		for (;it != m_waitMatchUsers.end();++it){
			std::map<UINT32,UINT32>::iterator subIt = it->second.m_waitCharIDs.begin();
			if ((subIt != it->second.m_waitCharIDs.end()) && (subIt->first != thdIt->first)){
				Global::stOne2OnePVPEnterCopyMap cmd; 
				cmd.add_memberids(thdIt->first);
				cmd.add_memberids(subIt->first);
				it->second.m_waitCharIDs.erase(subIt);
				
				it = m_waitMatchUsers.find(thdIt->second);
				if (it != m_waitMatchUsers.end()){
					it->second.m_waitCharIDs.erase(thdIt->first);
				}
				g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("one2One 匹配到玩家了4!");
#endif
				break;
			}	
		}
	}
}

UINT16 COne2OnePVPMgr::getRobotGroupID(UINT16 matchType,UINT16 lv)
{
	std::map<UINT16,std::vector<stMatchRobotGroup> >::iterator it = m_matchRobotInfos.find(matchType);
	if (it == m_matchRobotInfos.end()){
		return 0;
	}

	std::vector<stMatchRobotGroup> & vec = it->second;

	for (UINT16 i = 0;i < vec.size();++i){
		stMatchRobotGroup & groupInfo = vec[i];
		if ((lv >= groupInfo.m_minLv) && (lv <= groupInfo.m_maxLv)){
			return groupInfo.m_groupID;
		}
	}
	return 0;
}

bool COne2OnePVPMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/arenamatch.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[巅峰对决]加载文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[巅峰对决]巅峰配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	TiXmlElement * pArenamatchNode = pRoot->FirstChildElement("Arenamatch");
	if (pArenamatchNode){
		TiXmlElement * pMatch = pArenamatchNode->FirstChildElement("match");
		while (pMatch){
			stMatchCfg match;
			if (pMatch->Attribute("id",&iVal)){
				match.m_id = iVal;	
			}		
			if (pMatch->Attribute("startPoint",&iVal)){
				match.m_startPoint = iVal;
			}
			if (pMatch->Attribute("endPoint",&iVal)){
				match.m_endPoint = iVal; 
			}
			if (pMatch->Attribute("matchType",&iVal)){
				match.m_matchType = iVal; 
			}
			if (pMatch->Attribute("matchTimeStage1",&iVal)){
				match.m_matchTimeState1 = iVal; 
			}
			if (pMatch->Attribute("matchTimeStage2",&iVal)){
				match.m_matchTimeState2 = iVal; 
			}
			if (pMatch->Attribute("matchRobot",&iVal)){
				match.m_matchRangeID = iVal; 
			}

			m_waitMatchUsers[match.m_id] = match;
			pMatch = pMatch->NextSiblingElement("match");
		}
	}
	
	TiXmlElement *pArenarobotNode = pRoot->FirstChildElement("Arenarobot");
	if (pArenarobotNode){
		TiXmlElement * pRobotRangeNode = pArenarobotNode->FirstChildElement("robotRange");
		while (pRobotRangeNode){
			UINT16 matchType = 0;
			if (pRobotRangeNode->Attribute("id",&iVal)){
				matchType = iVal;
			}
			TiXmlElement *  pRobotNode = pRobotRangeNode->FirstChildElement("robot");
			while (pRobotNode){
				stMatchRobotGroup groupInfo;
				if (pRobotNode->Attribute("startLevel",&iVal)){
					groupInfo.m_minLv = iVal;	
				}
				if (pRobotNode->Attribute("endLevel",&iVal)){
					groupInfo.m_maxLv = iVal;
				}
				if (pRobotNode->Attribute("group",&iVal)){
					groupInfo.m_groupID = iVal;
				}
				m_matchRobotInfos[matchType].push_back(groupInfo);
				pRobotNode = pRobotNode->NextSiblingElement("robot");
			}
			pRobotRangeNode = pRobotRangeNode->NextSiblingElement("robotRange"); 
		}
	}

	TiXmlElement * pArenaRewardNode = pRoot->FirstChildElement("ArenaReward");
	if (pArenaRewardNode){
		TiXmlElement * pRewardNode = pArenaRewardNode->FirstChildElement("reward");
		while (pRewardNode){
			stEverydayReward reward;
			if (pRewardNode->Attribute("Startrank",&iVal)){
				reward.m_beginRank = iVal;	
			}
			if (pRewardNode->Attribute("Endrank",&iVal)){
				reward.m_endRank = iVal;
			}
			if (pRewardNode->Attribute("Fristrewardid",&iVal)){
				reward.m_itemID = iVal;
			}
			if (pRewardNode->Attribute("Fristrewardnum",&iVal)){
				reward.m_num = iVal;	
			}
			m_everydayRewards.push_back(reward);
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
	}

	TiXmlElement * pTimeRewardNode = pRoot->FirstChildElement("TimeReward");
	if (pTimeRewardNode){
		TiXmlElement * pRewardNode = pTimeRewardNode->FirstChildElement("reward");
		while (pRewardNode){
			stWeekReward reward;
			if (pRewardNode->Attribute("Startrank",&iVal)){
				reward.m_beginRank = iVal;
			}

			if (pRewardNode->Attribute("Endrank",&iVal)){
				reward.m_endRank = iVal;	
			}

			if (pRewardNode->Attribute("Fristrewardid",&iVal)){
				reward.m_itemID1 = iVal;
			}

			if (pRewardNode->Attribute("Fristrewardnum",&iVal)){
				reward.m_num1 = iVal;
			}

			if (pRewardNode->Attribute("Secondrewardid",&iVal)){
				reward.m_itemID2 = iVal;
			}

			if (pRewardNode->Attribute("Secondrewardnum",&iVal)){
				reward.m_num2 = iVal;
			}

			if (pRewardNode->Attribute("Thirdrewardid",&iVal)){
				reward.m_itemID3 = iVal;
			}

			if (pRewardNode->Attribute("Thirdrewardnum",&iVal)){
				reward.m_num3 = iVal;
			}

			m_weekRewards.push_back(reward);
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
		pTimeRewardNode = pTimeRewardNode->NextSiblingElement("TimeReward");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("巅峰匹配配置:");
	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin(); 
	for (;it != m_waitMatchUsers.end();++it){
		stMatchCfg & match = it->second;
		Athena::logger->trace("id=%d,startPoint=%d,endPoint=%d,matchType=%d,matchTimeState1=%d,matchTimeState2=%d,matchRangeID:%d",match.m_id,match.m_startPoint,match.m_endPoint,match.m_matchType,match.m_matchTimeState1,match.m_matchTimeState2,match.m_matchRangeID);
	}
	
	Athena::logger->trace("每天奖励配置:");
	for (UINT16 i = 0;i < m_everydayRewards.size();++i){
		stEverydayReward & reward = m_everydayRewards[i];
		Athena::logger->trace("beginRank=%d,endRank=%d,itemID1=%d,num1=%d",reward.m_beginRank,reward.m_endRank,reward.m_itemID,reward.m_num);
	}

	Athena::logger->trace("周奖励配置:");
	for (UINT16 i = 0;i < m_weekRewards.size();++i){
		stWeekReward & reward = m_weekRewards[i];
		Athena::logger->trace("beginRank=%d,endRank=%d,itemID1=%d,num1=%d,itemID2=%d,num2=%d",reward.m_beginRank,reward.m_endRank,reward.m_itemID1,reward.m_num1,reward.m_itemID2,reward.m_num2);	
	}

	Athena::logger->trace("打印机器人匹配信息:");
	std::map<UINT16,std::vector<stMatchRobotGroup> >::iterator subIt = m_matchRobotInfos.begin();
	for (;subIt != m_matchRobotInfos.end();++subIt){
		Athena::logger->trace("匹配的类型为:%d",subIt->first);
	
		std::vector<stMatchRobotGroup> & vec = subIt->second;
		for (UINT16 i = 0;i < vec.size();++i){
			stMatchRobotGroup & groupInfo = vec[i];
			Athena::logger->trace("startLv:%d,endLv:%d,group:%d",groupInfo.m_minLv,groupInfo.m_maxLv,groupInfo.m_groupID);
		}
	}
#endif
	return true;
}

void COne2OnePVPMgr::synAllsRecord(Player *pUser)
{
	if(!pUser){
		return ;
	}

	if (m_scoreSortList.isInSortList(pUser->getID())){
		stOne2OnePVPInfo info = m_scoreSortList.getSortInfo(pUser->getID());
		strncpy(info.m_name, pUser->getName(), sizeof(info.m_name));
		m_scoreSortList.updateSortInfo(info);

		stOne2OnePVPDB dbData;
		info.fillDBData(dbData);
		updateRecord(info.m_charID,(const char *)&dbData);
	}

	std::vector<stOne2OnePVPLastWeekInfo>::iterator it = m_lastweekTopN.begin();
	for(;it != m_lastweekTopN.end(); ++it){
		stOne2OnePVPLastWeekInfo & info = *it;
		if(info.m_charID == pUser->getID()){
			strncpy(info.m_name, pUser->getName(), sizeof(info.m_name));
		}
	}
}
