#include "DrawHeroScoreMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "SortList.pb.h"
#include "RankMgr.h"
#include "LimitTimePetCfg.h"
#include "Misc.pb.h"
#include "Mail.h"
#include "SysLanguages.h"

CTblField g_drawHeroScoreTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "SCORE",         	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "REWARDS",        CTblField::DB_TYPE_STRING,  50 				},
	{ NULL, 0, 0 },
};

CTblField g_drawHeroTblDef [] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "SCORE",        	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)	 },
	{ "JOB",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ NULL, 0, 0 },
};

CTblField g_drawHeroActivityTblDef[] = {
	{ "ID",         	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACTIVITY_ID",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)	 },
	{ "IS_START",       CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ NULL, 0, 0 },
};

void stDrawHeroScoreInfo::fillDBData(stDrawHeroScoreDB & data)
{
	data.m_charID = m_charID;
	data.m_score = m_score;
	
	std::ostringstream os;
	std::set<UINT16>::iterator it = m_rewardIDs.begin();
	for (;it != m_rewardIDs.end();++it){
		os << *it << ";";
	}
	strncpy(data.m_rewards,os.str().c_str(),sizeof(data.m_rewards));
}

void stDrawHeroScoreInfo::initFromData(stDrawHeroScoreDB & data)
{
	m_charID = data.m_charID;
	m_score = data.m_score;

	std::vector<std::string> strVec;
	g_stringtok(strVec,data.m_rewards,";");
	for (UINT16 i = 0;i < strVec.size();++i){
		m_rewardIDs.insert((UINT16)atoi(strVec[i].c_str()));		
	}
}

void stDrawHeroActivityInfo::initFromDB(stDrawHeroActivityDB & data)
{
	m_id = data.m_id;
	m_activityID = data.m_activityID;
	m_isStart = data.m_isStart;
}

void stDrawHeroActivityInfo::fillDBData(stDrawHeroActivityDB & data)
{
	data.m_id = m_id;
	data.m_activityID = m_activityID;
	data.m_isStart = m_isStart;
}

void CDrawHeroRankMgr::load()
{
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_drawHeroTblDef,"DRAWHERO_RANK",NULL,NULL,0,NULL);
	if (result){
		stDrawHeroRankDB * pDrawHeroInfo = (stDrawHeroRankDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stDrawHeroRankInfo info ;
			info.m_charID = pDrawHeroInfo[i].m_charID;
			info.m_job = pDrawHeroInfo[i].m_job;
			strncpy(info.m_name,pDrawHeroInfo[i].m_name,sizeof(info.m_name));
			info.m_score = pDrawHeroInfo[i].m_score;
			info.m_sex = pDrawHeroInfo[i].m_sex;
			m_drawHeroSortList.insertSortInfo(info);
		}
	}
	SAFE_DELETE(result);
	Athena::logger->trace("抽武将排行榜记录数为%d",m_drawHeroSortList.getListNum());	
	
	result = g_databaseEngine.select(g_drawHeroActivityTblDef,"DRAWHERO_ACTIVITY",NULL,NULL,0,NULL);
	if (!result || !result->getData()){
		UINT64 autoID = 0;		
		stDrawHeroActivityDB dbData;
		g_databaseEngine.insertRecord(g_drawHeroActivityTblDef,(const char *)&dbData,"DRAWHERO_ACTIVITY",autoID,NULL);
		m_activityInfo.m_id = autoID;
	}
	else {
		stDrawHeroActivityDB * pDBData = (stDrawHeroActivityDB * )result->getData();
		m_activityInfo.initFromDB(pDBData[0]);
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("id:%d,activityID:%d,isStart:%d",m_activityInfo.m_id,m_activityInfo.m_activityID,m_activityInfo.m_isStart);
#endif
}

void CDrawHeroRankMgr::addRecord(const char * pData)
{
	Athena::CUDSqlCallBack<CDrawHeroRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroRankMgr>(this,&CDrawHeroRankMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_drawHeroTblDef,pData,"DRAWHERO_RANK",autoID,pDelaySql);	
}

void CDrawHeroRankMgr::addRecordCB(bool opResult,UINT64 autoID)
{}

void CDrawHeroRankMgr::updateRecord(UINT32 key,const char * pData)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",key);
	
	Athena::CUDSqlCallBack<CDrawHeroRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroRankMgr>(this,&CDrawHeroRankMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_drawHeroTblDef,pData,"DRAWHERO_RANK",where,pDelaySql);
}

void CDrawHeroRankMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void CDrawHeroRankMgr::removeRecord(UINT32 key)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%d",key);
	Athena::CUDSqlCallBack<CDrawHeroRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroRankMgr>(this,&CDrawHeroRankMgr::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("DRAWHERO_RANK",where,pDelaySql);
}

void CDrawHeroRankMgr::removeRecordCB(bool opResult,UINT64 autoID)
{}

void CDrawHeroRankMgr::removeAllDrawHeroRecord()
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID > 0 ");
	Athena::CUDSqlCallBack<CDrawHeroRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroRankMgr>(this,&CDrawHeroRankMgr::removeAllDrawHeroRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("DRAWHERO_RANK",where,pDelaySql);
}

void CDrawHeroRankMgr::updateActivityInfo()
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%u",m_activityInfo.m_id);
	
	stDrawHeroActivityDB dbData;
	m_activityInfo.fillDBData(dbData);
	Athena::CUDSqlCallBack<CDrawHeroRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroRankMgr>(this,&CDrawHeroRankMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_drawHeroActivityTblDef,(const char *)&dbData,"DRAWHERO_ACTIVITY",where,pDelaySql);

}

void CDrawHeroRankMgr::updateActivityInfoCB(bool opResult,UINT64 autoID)
{}

void CDrawHeroRankMgr::removeAllDrawHeroRecordCB(bool opResult,UINT64 autoID)
{

}

void CDrawHeroRankMgr::timer(UINT32 cur)
{
	if (!m_activityInfo.m_isStart){//还没有开启
		LimitTimePetCfg::stActivityOpenInfo *pOpenInfo =  LimitTimePetCfg::getSingleton().getOpenInfo(cur);
		if (pOpenInfo){//活动开启了
			m_activityInfo.m_isStart = 1;
			m_activityInfo.m_activityID = pOpenInfo->m_activityID;
			updateActivityInfo();
			activityStart();
		}
	}
	else if (m_activityInfo.m_isStart > 0){//已经开始了
		LimitTimePetCfg::stActivityOpenInfo *pOpenInfo =  LimitTimePetCfg::getSingleton().getOpenInfo(cur);
		if (!pOpenInfo){
            Athena::logger->error("限时武神抽奖 结算时间 %d", cur);
			m_activityInfo.m_isStart = 0;
			updateActivityInfo();
			activityEnd();
		}
		else if (pOpenInfo && (pOpenInfo->m_activityID != m_activityInfo.m_activityID)){
			m_activityInfo.m_activityID = pOpenInfo->m_activityID;
			updateActivityInfo();
		}
	}
}

void CDrawHeroRankMgr::activityStart()
{
	removeAllDrawHeroRecord();
	m_drawHeroSortList.clear();
	CDrawHeroScoreMgr::getSingleton().clearAllDrawHeroScore();
}

void CDrawHeroRankMgr::activityEnd()
{
    Athena::logger->error("限时武神抽奖结束时候发奖励 找配置:%d",m_activityInfo.m_activityID);
	LimitTimePetCfg::stActivity *pActivity =  LimitTimePetCfg::getSingleton().getActivity(m_activityInfo.m_activityID);
	if (!pActivity){
		Athena::logger->error("限时武神抽奖结束时候发奖励没有找到配置:%d",m_activityInfo.m_activityID);
		return ;
	}

	for (UINT16 i = 0;i < pActivity->m_rankRewards.size();++i){
		LimitTimePetCfg::stRankReward & rankReward = pActivity->m_rankRewards[i];
		stBatchSendMailTask task;
		task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_TIME_WUJIANG_DRAW);
		char szMailContent[50] = { 0 };
		snprintf(szMailContent,sizeof(szMailContent),"MailWnd_17;%d",i + 1);
		task.m_content = szMailContent;
		
		std::vector<stDrawHeroRankInfo> vec;
		m_drawHeroSortList.getFromN12N2(vec,rankReward.m_beginRank,rankReward.m_endRank);
		for (UINT16 j = 0;j < vec.size();++j){
			if(vec[j].m_score >= LimitTimePetCfg::getSingleton().getMinScore()){
				task.m_receiveIDs.insert(vec[j].m_charID);	
			}
		}
		task.m_attaches.push_back(std::make_pair(rankReward.m_boxID,1));
		CBatchSendMailMgr::getSingleton().addTask(task);
	}
}

void CDrawHeroRankMgr::updateDrawHeroScore(Player * pUser,UINT32 score)
{
	if (!pUser){
		return ;
	}	

	if (m_drawHeroSortList.isInSortList(pUser->getID())){
		stDrawHeroRankInfo info = m_drawHeroSortList.getSortInfo(pUser->getID());

		if (info.m_score < score){
			info.m_score = score;
			m_drawHeroSortList.updateSortInfo(info);
			stDrawHeroRankDB dbData;
			info.fillDBData(dbData);
			updateRecord(info.getKey(),(const char *)&dbData);
		}
	}
	else {
		UINT32 lastOneKey = m_drawHeroSortList.getLastOneKey();
		stDrawHeroRankInfo info;
		info.m_charID = pUser->getID();
		info.m_score = score;
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_job = pUser->m_job;
		info.m_sex = pUser->m_sex;
		m_drawHeroSortList.insertSortInfo(info);

		stDrawHeroRankDB dbData;
		info.fillDBData(dbData);
		addRecord((const char *)&dbData);
		
		if (!m_drawHeroSortList.isInSortList(lastOneKey)){//已经不在排行帮了
			removeRecord(lastOneKey);		
		}
	}
}

void CDrawHeroRankMgr::sendDrawHeroSortList(Player *pUser,UINT32 beginRank)
{
	if (!pUser){
		return ;
	}

	SortProto::stDrawHeroSortList retCmd;
	retCmd.set_beginrank(beginRank);
	retCmd.set_score(CDrawHeroScoreMgr::getSingleton().getScoreByID(pUser->getID()));

	std::vector<stDrawHeroRankInfo> vec;
	m_drawHeroSortList.getFromN12N2(vec,beginRank,beginRank + s_SEND_SORTITEM_MAX);
	retCmd.set_myrank(m_drawHeroSortList.getMySortRank(pUser->getID()));

	for (UINT8 i = 0;i < vec.size();++i){
		if(vec[i].m_score >= LimitTimePetCfg::getSingleton().getMinScore()){
			SortProto::stDrawHeroSortItem * pSortItem = retCmd.add_sortitem();
			if (pSortItem){
				pSortItem->set_charid(vec[i].m_charID);
				pSortItem->set_level(CRankMgr::getSingleton().getLevelByCharID(vec[i].m_charID));
				pSortItem->set_name(vec[i].m_name);
				pSortItem->set_score(vec[i].m_score);
				pSortItem->set_job(vec[i].m_job);
				pSortItem->set_sex(vec[i].m_sex);
			}
		}
	}

	pUser->sendProtoToMe(retCmd);
}

void CDrawHeroRankMgr::synRecord(Player * pUser)
{
	if(!pUser){
		return;	
	}

	if (m_drawHeroSortList.isInSortList(pUser->getID())){
		stDrawHeroRankInfo info = m_drawHeroSortList.getSortInfo(pUser->getID());
		strncpy(info.m_name, pUser->getName(), sizeof(info.m_name));
		m_drawHeroSortList.updateSortInfo(info);
		stDrawHeroRankDB dbData;
		info.fillDBData(dbData);
		updateRecord(info.getKey(),(const char *)&dbData);
	}
}

void CDrawHeroScoreMgr::load()
{
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(g_drawHeroScoreTblDef,"DRAWHERO_SCORE",NULL,NULL,0,NULL);	
	if (result){
		if (result->getData()){
			stDrawHeroScoreDB *  pDBInfo = (stDrawHeroScoreDB *) result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stDrawHeroScoreInfo & info =  m_charID2ScoreMap[pDBInfo[i].m_charID];
				info.initFromData(pDBInfo[i]);
			}
		}
	}
	SAFE_DELETE(result);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("保存的角色抽奖积分记录条数:%d",m_charID2ScoreMap.size());
#endif
}

void CDrawHeroScoreMgr::addRecord(const stDrawHeroScoreDB & data)
{
	Athena::CUDSqlCallBack<CDrawHeroScoreMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroScoreMgr>(this,&CDrawHeroScoreMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_drawHeroScoreTblDef,(const char *)&data,"DRAWHERO_SCORE",autoID,pDelaySql);
}

void CDrawHeroScoreMgr::addRecordCB(bool opResult,UINT64 autoID)
{}

void CDrawHeroScoreMgr::updateRecord(const stDrawHeroScoreDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",data.m_charID);
	
	Athena::CUDSqlCallBack<CDrawHeroScoreMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroScoreMgr>(this,&CDrawHeroScoreMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_drawHeroScoreTblDef,(const char *)&data,"DRAWHERO_SCORE",where,pDelaySql);
}

void CDrawHeroScoreMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void CDrawHeroScoreMgr::removeRecord(UINT32 charID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%d",charID);
	Athena::CUDSqlCallBack<CDrawHeroScoreMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroScoreMgr>(this,&CDrawHeroScoreMgr::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("DRAWHERO_SCORE",where,pDelaySql);
}

void CDrawHeroScoreMgr::removeRecordCB(bool opResult,UINT64 autoID)
{}

UINT32 CDrawHeroScoreMgr::getScoreByID(UINT32 charID)
{
	CharID2ScoreMap_IT it = m_charID2ScoreMap.find(charID);
	if (it == m_charID2ScoreMap.end()){
		return 0;
	}
	return it->second.m_score;
}

void CDrawHeroScoreMgr::userIncScore(UINT32 charID,UINT32 score)
{
	CharID2ScoreMap_IT it = m_charID2ScoreMap.find(charID);
	if (it == m_charID2ScoreMap.end()){
		stDrawHeroScoreInfo info;
		info.m_charID = charID;
		info.m_score = score;
		m_charID2ScoreMap[charID] = info;

		stDrawHeroScoreDB dbInfo;
		info.fillDBData(dbInfo);
		addRecord(dbInfo);
	}
	else {
		stDrawHeroScoreInfo & info = it->second;
		info.m_score += score;

		stDrawHeroScoreDB dbInfo;
		info.fillDBData(dbInfo);
		updateRecord(dbInfo);
	}
}
	
void CDrawHeroScoreMgr::scoreReward(Player * pUser,UINT16 scoreID)
{
	MiscProto::stDrawHeroScoreRewardRet retCmd;

	LimitTimePetCfg::stScoreReward *pScoreReward =  LimitTimePetCfg::getSingleton().getScoreRewardInfo(CDrawHeroRankMgr::getSingleton().getActivityID(),scoreID);
	if (!pScoreReward){
		retCmd.set_result(MiscProto::stDrawHeroScoreRewardRet::eInner_Error);
		pUser->sendProtoToMe(retCmd);
		return ;
	}  

	CharID2ScoreMap_IT it = m_charID2ScoreMap.find(pUser->getID());
	if (it == m_charID2ScoreMap.end()){
		retCmd.set_result(MiscProto::stDrawHeroScoreRewardRet::eScore_Not_Enough);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (it->second.m_score < pScoreReward->m_score){
		retCmd.set_result(MiscProto::stDrawHeroScoreRewardRet::eScore_Not_Enough);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	if (it->second.m_rewardIDs.find(scoreID) != it->second.m_rewardIDs.end()){
		retCmd.set_result(MiscProto::stDrawHeroScoreRewardRet::eHas_Got_Reward);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	it->second.m_rewardIDs.insert(scoreID);
	retCmd.set_result(MiscProto::stDrawHeroScoreRewardRet::eSuccess);
	pUser->sendProtoToMe(retCmd);


	stDrawHeroScoreDB dbInfo;
	stDrawHeroScoreInfo & info = it->second;
	info.fillDBData(dbInfo);
	updateRecord(dbInfo);

	Global::stDrawHeroScoreReward cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_boxid(pScoreReward->m_boxID);
	pUser->sendProto2Game(cmd);
}

void CDrawHeroScoreMgr::rankReward(Player * pUser)
{
	if (!pUser){
		return ;
	}
	
	return ;

	MiscProto::stDrawHeroRankReward retCmd;
	UINT16 myRank = CDrawHeroRankMgr::getSingleton().getMyRank(pUser->getID());
	UINT32 boxID = LimitTimePetCfg::getSingleton().getBoxIDByRank(CDrawHeroRankMgr::getSingleton().getActivityID(),myRank);

	if (0 == boxID){
		retCmd.set_result(MiscProto::stDrawHeroRankReward::eHas_No_Rewards);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	CharID2ScoreMap_IT it = m_charID2ScoreMap.find(pUser->getID());
	if (it == m_charID2ScoreMap.end()){
		retCmd.set_result(MiscProto::stDrawHeroRankReward::eHas_No_Rewards);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	if (it->second.m_gotRankReward){
		retCmd.set_result(MiscProto::stDrawHeroRankReward::eHas_Got_Reward);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	it->second.m_gotRankReward = true;
	retCmd.set_result(MiscProto::stDrawHeroRankReward::eSuccess);
	pUser->sendProtoToMe(retCmd);

	Global::stDrawHeroRankReward cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_boxid(boxID);
	pUser->sendProtoToMe(cmd);
}

stDrawHeroScoreInfo * CDrawHeroScoreMgr::getDrawHeroScoreInfo(UINT32 charID)
{
	CharID2ScoreMap_IT it = m_charID2ScoreMap.find(charID); 
	if (it != m_charID2ScoreMap.end()){
		return &it->second;
	}
	return NULL;
}

void CDrawHeroScoreMgr::clearAllDrawHeroScore()
{
	m_charID2ScoreMap.clear();
	
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID > 0");
	Athena::CUDSqlCallBack<CDrawHeroScoreMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDrawHeroScoreMgr>(this,&CDrawHeroScoreMgr::clearAllDrawHeroScoreCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("DRAWHERO_SCORE",where,pDelaySql);

}

void CDrawHeroScoreMgr::clearAllDrawHeroScoreCB(bool opResult,UINT64 autoID)
{

}
