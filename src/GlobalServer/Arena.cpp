#include "Arena.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "GlobalServer.h"
#include "PacketProcessor.h"
#include "PlayerManager.h"
#include "RoleInfoManager.h"
#include "ArenaConfig.h"
#include "SortList.pb.h"
#include "RankMgr.h"
#include "KVPair.h"
#include "Mail.h"
#include "TblDataMgr.h"
#include "GlobalVar.h"
#include "MiscCfgMgr.h"
#include "SysLanguages.h"
#include "OperateActivity.h"
#include "OperateActivityCfg.h"

extern GlobalServer  * g_globalServer;

CTblField g_arenaSortInfoTblDef[] = { 
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1}, 
	{ "JOB",          	CTblField::DB_TYPE_UINT8,  sizeof(UINT8)},
	{ "SEX",          	CTblField::DB_TYPE_UINT8,  sizeof(UINT8)},
	{ "RANK",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)},
	//{ "BATTLEPOWER",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)},
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)},
	{ NULL, 0, 0 },
};

CTblField g_arenaHistorySortList[] = {
	{ "ID",         					CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "REWARD_TIMESTAMP",         		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BINARYDATA", 					CTblField::DB_TYPE_BLOB,   ARENA_REWARD_INFO_LEN },
	{ NULL, 0, 0 },	
};

CTblField g_hasGotRewardCharIDTblDef[] = {
	//{ "ID",         					CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "REWARD_TIMESTAMP",         		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BINARYDATA", 					CTblField::DB_TYPE_BLOB,   ARENA_REWARD_INFO_LEN },
	{ NULL, 0, 0 },	
};

CTblField g_userArenaInfoblDef[] = {
	{ "CHARID",         		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CHALLENGENUM",         	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "LASTPKTIME",         	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "RESETNUM",         		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{  NULL, 					0, 							0				 },	
};


void stArenaSortInfo::fillDBData(stArenaSortInfoDB & dbData)
{
	dbData.m_charID = m_charID;
	strncpy(dbData.m_name,m_name,sizeof(dbData.m_name));
	dbData.m_job = m_job;
	dbData.m_sex = m_sex;
	dbData.m_rank = m_rank;
	dbData.m_photoID = m_photoID;
}

void stArenaSortInfo::initFromDB(stArenaSortInfoDB & dbData)
{
	m_charID  = dbData.m_charID;
	strncpy(m_name,dbData.m_name,sizeof(m_name));
	m_job = dbData.m_job;
	m_sex = dbData.m_sex;
	m_rank = dbData.m_rank;
	m_photoID = dbData.m_photoID;
}

void stUserArenaInfo::initFromDB(const stUserArenaInfoDB & data)
{
	m_charID = data.m_charID;
	m_challengeNum = data.m_challengeNum;
	m_lastPKTime = data.m_lastPKTime;
	m_resetNum = data.m_resetNum;
}

void stUserArenaInfo::fillDBData(stUserArenaInfoDB & data)
{
	data.m_charID = m_charID;
	data.m_challengeNum = m_challengeNum;
	data.m_lastPKTime = m_lastPKTime;
	data.m_resetNum = m_resetNum;
}

ArenaMgr::ArenaMgr()
{
	m_theLowestRank = 0;
}

ArenaMgr::~ArenaMgr()
{
	
}

void ArenaMgr::loadAll()
{
	CTblQueryResult* result = g_databaseEngine.select(g_arenaSortInfoTblDef,"ARENASORTLIST",NULL,NULL,0,NULL); 
	if (result){
		if (result->getData()){
			stArenaSortInfoDB *  arenaSortInfoDB = (stArenaSortInfoDB *) result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stArenaSortInfo sortInfo;
				sortInfo.initFromDB(arenaSortInfoDB[i]);
				m_charID2SortInfo[sortInfo.m_charID] = sortInfo;
				m_rank2CharID[sortInfo.m_rank] = sortInfo.m_charID;
				if (sortInfo.m_rank > m_theLowestRank){
					m_theLowestRank = sortInfo.m_rank;
				}
#ifdef _HDZ_DEBUG
				Athena::logger->trace("竞技场排名信息charID=%d,name=%s,rank=%d,photoid=%d",sortInfo.m_charID,sortInfo.m_name, sortInfo.m_rank, sortInfo.m_photoID);
#endif
			}
		}
	}
	SAFE_DELETE(result);
	
	result = g_databaseEngine.select(g_hasGotRewardCharIDTblDef,"ARENA_GOT_REWARD_CHARID",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stHasGotRewardCharIDs * pDBData = (stHasGotRewardCharIDs *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			UINT16 num = *(UINT16 *)(pDBData[i].m_data);
			UINT32 *pCharID = (UINT32 *)(pDBData[i].m_data + sizeof(UINT16));
#ifdef _HDZ_DEBUG
			Athena::logger->trace("竞技场中时间:%d,已经领取奖励的玩家数量是:%d",pDBData[i].m_rewardTimestamp,num);
#endif
			for (UINT16 j = 0; j < num; ++j){
				m_hasRewardCharID[pDBData[i].m_rewardTimestamp].insert(pCharID[j]);
			}
		}
	}

	SAFE_DELETE(result);

	result = g_databaseEngine.select(g_arenaHistorySortList,"HISTORY_ARENA_SORTLIST",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stYestodayArenaSortListDB * pDBData = (stYestodayArenaSortListDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			std::set<UINT32> hasRewardCharID;
			if (m_hasRewardCharID.find(pDBData[i].m_rewardTimestamp) != m_hasRewardCharID.end()){
				hasRewardCharID	= m_hasRewardCharID[pDBData[i].m_rewardTimestamp];	
			}
			std::map<UINT32,UINT32> & charID2Rank = m_historyCharID2Rank[pDBData[i].m_rewardTimestamp];
			UINT16 num = *(UINT16 *)(pDBData[i].m_data);
			UINT32 *charID = (UINT32 *)(pDBData[i].m_data + sizeof(UINT16));
			for (UINT16 j = 0; j < num; ++j){
				if (charID[j] > MAX_ROBOT_CHARID){//大于这个才是机器人
					if (hasRewardCharID.find(charID[j]) != hasRewardCharID.end()){//已经领取了
						continue;
					}
					charID2Rank[charID[j]] = j + 1;
				}
			}
		}
	}

	SAFE_DELETE(result);
	
	result = g_databaseEngine.select(g_userArenaInfoblDef,"ARENA_USER_INFO",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stUserArenaInfoDB * pDBData = (stUserArenaInfoDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stUserArenaInfo &userInfo = m_userArenaInfos[pDBData[i].m_charID];
			userInfo.initFromDB(pDBData[i]);
		}
	}

	SAFE_DELETE(result);

	//加载完后进行处理
	UINT32 serverID = (UINT32)atoi(Athena::global["serverID"].c_str());

	if (m_theLowestRank < s_INIT_PK_RANK){
		m_theLowestRank = s_INIT_PK_RANK;
		Athena::logger->trace("区:%d的最低排名是:%d",m_theLowestRank,m_theLowestRank);
	}

	UINT32 genRobotNum = 0;
	for (UINT16 i = 1;i < m_theLowestRank; ++i) {
		if (m_rank2CharID.find(i) == m_rank2CharID.end()){//还没有人加机器人
			stArenaSortInfo  sortInfo;
			sortInfo.m_rank = i;
			sortInfo.m_charID = sortInfo.m_robotID = genRobotNum + 1;
			stRobotBase * pRobotBase =  g_robotBaseMgr.get(sortInfo.m_robotID);
			if (pRobotBase) {
				sortInfo.m_job = pRobotBase->m_job;
				sortInfo.m_sex = pRobotBase->m_sex;
				sortInfo.m_photoID = 1;
				strncpy(sortInfo.m_name,pRobotBase->m_robotName,sizeof(sortInfo.m_name));
			}
			m_charID2SortInfo[sortInfo.m_charID] = sortInfo;
			m_rank2CharID[sortInfo.m_rank] = sortInfo.m_charID;	
			genRobotNum += 1;
		}
	}

	Athena::logger->trace("区:%d创建了机器人:%d个,排行帮的人数:%d",serverID,genRobotNum,m_charID2SortInfo.size());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("目前的历史排名奖励有:%d",m_historyCharID2Rank.size());
	{
		std::map<UINT32,std::map<UINT32,UINT32> >::iterator it = m_historyCharID2Rank.begin();
		for (;it != m_historyCharID2Rank.end();++it){
			Athena::logger->trace("timestamp:%d,size:%d",it->first,it->second.size());
			std::map<UINT32,UINT32>::iterator subIt = it->second.begin();
			for (;subIt != it->second.end();++subIt){
				Athena::logger->trace("charID:%d,rank:%d",subIt->first,subIt->second);	
			}
		}
	}

	{
		std::map<UINT32,UINT32>::iterator it =   m_rank2CharID.begin();
		for (;it != m_rank2CharID.end();++it){
			Athena::logger->trace("竞技场的排名奖励信息:rank:%d,charID:%d",it->first,it->second);
		}
	}
#endif
}

void ArenaMgr::sendPKList(Player * pUser)
{
	if (!pUser){
		return ;
	}	

	std::map<UINT32,stArenaSortInfo>::iterator it =   m_charID2SortInfo.find(pUser->getID());
	UINT32 myRank = m_theLowestRank + 1;

	if (it == m_charID2SortInfo.end()){//还没有进入排名
		stArenaSortInfo  sortInfo;
		sortInfo.m_charID = pUser->getID();
		sortInfo.m_rank = myRank;
		sortInfo.m_job = pUser->m_job;
		sortInfo.m_sex = pUser->m_sex;
		sortInfo.m_photoID = pUser->m_photoID;
		myRank = sortInfo.m_rank;
		strncpy(sortInfo.m_name,pUser->getName(),sizeof(sortInfo.m_name));
		m_charID2SortInfo[sortInfo.m_charID] = sortInfo;
		m_rank2CharID[myRank] = sortInfo.m_charID;	
		
		m_theLowestRank = myRank;
		stArenaSortInfoDB sortInfoDB;
		sortInfo.fillDBData(sortInfoDB);
		insertRecord(sortInfoDB);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("玩家(name=%s,charID=%d)进入竞技场后排名:%d",pUser->getName(),pUser->getID(),myRank);
#endif
	}
	else {
		myRank = it->second.m_rank;
	}

	SortProto::stRetArenaPKList cmd;
	cmd.set_totalusernum(m_charID2SortInfo.size());
	cmd.set_myrank(myRank);
	//获得排名间隔
	INT32 rankInterval = std::max((INT32)(myRank * 0.2f / 10.0f),1);//myRank / 100 + myRank % 100 ? 1 : 0;
	UINT32 beginRank = 0;
	if (myRank <= s_PKLIST_BEFORE_NUM){//大于s_PKLIST_BEFORE_NUM时候的处理
		beginRank = s_PKLIST_BEFORE_NUM + 1;
	}
	else {
		beginRank = myRank;
	}
	
	for (UINT32 i = (beginRank - s_PKLIST_BEFORE_NUM * rankInterval); i < beginRank; i += rankInterval){//前 s_PKLIST_BEFORE_NUM 名
		if (m_rank2CharID.find(i) == m_rank2CharID.end()){
			continue;
		}
		UINT32 charID = m_rank2CharID[i];
		if (charID == pUser->getID()){
			continue;
		}
		std::map<UINT32,stArenaSortInfo>::iterator it =   m_charID2SortInfo.find(charID);
		if (it != m_charID2SortInfo.end()){
			SortProto::stSynArenaSortInfo * pSynSortInfo = cmd.add_data();
			pSynSortInfo->set_charid(it->second.m_charID);
			pSynSortInfo->set_name(it->second.m_name);
			pSynSortInfo->set_rank(it->second.m_rank);
			pSynSortInfo->set_job(it->second.m_job);
			pSynSortInfo->set_sex(it->second.m_sex);
			pSynSortInfo->set_photoid(it->second.m_photoID);
			if (it->second.m_robotID){
				stRobotBase * pRobotBase =  g_robotBaseMgr.get(it->second.m_robotID);
				if (pRobotBase){
					pSynSortInfo->set_level(pRobotBase->m_lv);
				}
			}
			else {
				pSynSortInfo->set_level(CRankMgr::getSingleton().getLevelByCharID(it->second.m_charID));
			}
		}
	}
	
	for (UINT32 i = beginRank + rankInterval ; i <= (beginRank + rankInterval * s_PKLIST_AFTER_NUM) ; i += rankInterval){//后 s_PKLIST_AFTER_NUM 名
		if (m_rank2CharID.find(i) == m_rank2CharID.end()){
			continue;
		}
		UINT32 charID = m_rank2CharID[i];
		if (charID == pUser->getID()){
			continue;
		}
		std::map<UINT32,stArenaSortInfo>::iterator it =   m_charID2SortInfo.find(charID);
		if (it != m_charID2SortInfo.end()){
			SortProto::stSynArenaSortInfo * pSynSortInfo = cmd.add_data();
			pSynSortInfo->set_charid(it->second.m_charID);
			pSynSortInfo->set_name(it->second.m_name);
			pSynSortInfo->set_rank(it->second.m_rank);
			pSynSortInfo->set_job(it->second.m_job);
			pSynSortInfo->set_sex(it->second.m_sex);
			pSynSortInfo->set_photoid(it->second.m_photoID);
			if (it->second.m_robotID){
				stRobotBase * pRobotBase =  g_robotBaseMgr.get(it->second.m_robotID);
				if (pRobotBase){
					pSynSortInfo->set_level(pRobotBase->m_lv);
				}
			}
			else {
				pSynSortInfo->set_level(CRankMgr::getSingleton().getLevelByCharID(it->second.m_charID));
			}
		}
	}
	
	for (INT32 i = 1; i <= 10  ;  ++i){//发送前10
		if (m_rank2CharID.find(i) == m_rank2CharID.end()){
			break;
		}

		UINT32 charID = m_rank2CharID[i];
		std::map<UINT32,stArenaSortInfo>::iterator it =   m_charID2SortInfo.find(charID);
		if (it != m_charID2SortInfo.end()){
			stArenaSortInfo & sortInfo = it->second;
			SortProto::stSynArenaSortInfo * pSyn = cmd.add_topten();
			pSyn->set_charid(sortInfo.m_charID);
			pSyn->set_name(sortInfo.m_name);
			pSyn->set_rank(sortInfo.m_rank);
			if (it->second.m_robotID){
				stRobotBase * pRobotBase =  g_robotBaseMgr.get(it->second.m_robotID);
				if (pRobotBase){
					pSyn->set_level(pRobotBase->m_lv);
				}
			}
			else {
				pSyn->set_level(CRankMgr::getSingleton().getLevelByCharID(it->second.m_charID));
			}

			pSyn->set_job(sortInfo.m_job);
			pSyn->set_sex(sortInfo.m_sex);
			pSyn->set_photoid(sortInfo.m_photoID);
		}
	}
	
	stUserArenaInfo *pUserArenaInfo = getUserArenaInfo(pUser->getID());
	if (pUserArenaInfo){
		UINT32 cd = CMiscCfgMgr::getSingleton().getArenaCD();
		UINT32 remainTime = pUserArenaInfo->m_lastPKTime + cd > g_globalServer->getSec() ? pUserArenaInfo->m_lastPKTime + cd - g_globalServer->getSec() : 0;
		cmd.set_cd(remainTime);
		cmd.set_challengenum(pUserArenaInfo->m_challengeNum);
		cmd.set_resetnum(pUserArenaInfo->m_resetNum);
	}
	pUser->sendProtoToMe(cmd);
}

void ArenaMgr::sendSortList(Player * pUser,UINT16 beginRank)
{
	if (!pUser){
		return ;
	}
	
	SortProto::stRetArenaSortList retCmd;

	std::map<UINT32,stArenaSortInfo>::iterator it = m_charID2SortInfo.find(pUser->getID());

	if (it != m_charID2SortInfo.end()){
		retCmd.set_myrank(it->second.m_rank);
	}
	else {
		retCmd.set_myrank(s_INIT_PK_RANK + 1); 
	}
	
	INT32 endRank = beginRank + 10;

	for (INT32 i = beginRank; i <= endRank  ;  ++i){
		if (m_rank2CharID.find(i) == m_rank2CharID.end()){
			break;
		}

		UINT32 charID = m_rank2CharID[i];
		std::map<UINT32,stArenaSortInfo>::iterator it =   m_charID2SortInfo.find(charID);
		if (it != m_charID2SortInfo.end()){
			stArenaSortInfo & sortInfo = it->second;
			SortProto::stSynArenaSortInfo * pSyn = retCmd.add_sortinfo();
			pSyn->set_charid(sortInfo.m_charID);
			pSyn->set_name(sortInfo.m_name);
			pSyn->set_rank(sortInfo.m_rank);
			//pSyn->set_level(CRankMgr::getSingleton().getLevelByCharID(sortInfo.m_charID));
			if (it->second.m_robotID){
				stRobotBase * pRobotBase =  g_robotBaseMgr.get(it->second.m_robotID);
				if (pRobotBase){
					pSyn->set_level(pRobotBase->m_lv);
				}
			}
			else {
				pSyn->set_level(CRankMgr::getSingleton().getLevelByCharID(it->second.m_charID));
			}

			pSyn->set_job(sortInfo.m_job);
			pSyn->set_sex(sortInfo.m_sex);
			pSyn->set_photoid(sortInfo.m_photoID);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void g_sendArenaMailCB(UINT64 autoID)
{

}

void ArenaMgr::replaceRank(Player * pUser,UINT32 rank,UINT32 historyRank)
{
	if (!pUser){//超过这个名次不替换
		return ;
	}

	UINT32 myRank = (UINT32)-1;
	
	std::map<UINT32,stArenaSortInfo>::iterator it = m_charID2SortInfo.find(pUser->getID());

	if  (it != m_charID2SortInfo.end()){
		myRank = it->second.m_rank;
	}
	
	if (myRank < rank){//小于挑战名次,不改变
		return ;
	}

	{//在排行榜
		UINT32 charID = m_rank2CharID[rank];
		stArenaSortInfo & otherSortInfo = m_charID2SortInfo[charID];
		stArenaSortInfo & mySortInfo = m_charID2SortInfo[pUser->getID()];
		otherSortInfo.m_rank = myRank;
		mySortInfo.m_rank = rank;
		m_rank2CharID[rank] = pUser->getID();
		m_rank2CharID[myRank] = charID;
		
		Player * pOther = PlayerManager::getSingleton().getUserByID(charID); 
		if (pOther){
			notifyMyRank2Game(pOther);	
		}

		//更新数据库
		stArenaSortInfoDB dbData;
		otherSortInfo.fillDBData(dbData);
		updateRecord(dbData);
		mySortInfo.fillDBData(dbData);
		updateRecord(dbData);
		
		//发邮件
		stMailDBInfo mailInfo;
		mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
		mailInfo.m_receiverID = otherSortInfo.m_charID;
		strncpy(mailInfo.m_receiverName,otherSortInfo.m_name,sizeof(mailInfo.m_receiverName));
		strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
		strncpy(mailInfo.m_title,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_HIGHLADDER_SORTLIST),sizeof(mailInfo.m_title));
#ifdef _THAILAND_VER
		char mailCnt[400] = { 0 };
#else
		char mailCnt[250] = { 0 };
#endif
		snprintf(mailCnt,sizeof(mailCnt),"MailWnd_6;%s;%d",mySortInfo.m_name,myRank);
		strncpy(mailInfo.m_content,mailCnt,sizeof(mailInfo.m_content));
		mailInfo.m_sendTime = g_globalServer->getSec();
		MailManager::getSingleton().sendMail(mailInfo,g_sendArenaMailCB);
		
		if ((rank <= s_HISTORY_RANK_LEVELUP_REWARD) && (rank < historyRank)){//排名小于历史名次,给奖励
			SortProto::stArenaReachNewRank newRankCmd;
			newRankCmd.set_newrank(rank);
			newRankCmd.set_oldrank(historyRank);
			UINT32 resNum = ArenaConfig::getSingleton().getHistoryRankReward(historyRank,rank);
			if (resNum <= 0){
				resNum = 1;
			}
			UINT8  resType = 0;			
			if (g_globalServer->isAppVersion()){
				resType = eResource_Reputation;
			}
			else {
				resType = eResource_Diamond;
			}
			newRankCmd.set_rewardnum(resNum);
			pUser->sendProtoToMe(newRankCmd);

			Global::stNotifyArenaReward notifyCmd;
			notifyCmd.set_charid(pUser->getID());
			notifyCmd.set_restype(resType);
			notifyCmd.set_resnum(resNum);
			pUser->sendProto2Game(notifyCmd);
		}
	}

	notifyMyRank2Game(pUser);
}

void ArenaMgr::timer(UINT32 cur)
{
	struct tm curTm;
	RealTime::getLocalTime(curTm,cur);
	UINT32 refreshTime = CGlobalVarMgr::getSingleton().getValue(eGlobalVar_Refresh_YestoryArenaRank);
	if ((!g_isTheSameDay(refreshTime,cur)) && (curTm.tm_hour >= 22)){//不在同一天开始刷)
		updateYestodaySortList();
		CGlobalVarMgr::getSingleton().setValue(eGlobalVar_Refresh_YestoryArenaRank,cur);
	}
}

void ArenaMgr::insertRecord(const stArenaSortInfoDB & data)
{
	Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::insertRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_arenaSortInfoTblDef,(const char *)&data,"ARENASORTLIST",autoID,pDelaySql);
}


void ArenaMgr::insertRecordCB(bool opResult,UINT64 autoID)
{}

void ArenaMgr::updateRecord(const stArenaSortInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",data.m_charID);
	
	Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_arenaSortInfoTblDef,(const char *)&data,"ARENASORTLIST",where,pDelaySql);
}

void ArenaMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void ArenaMgr::deleteRecord(UINT32 charID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%d",charID);
	Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::deleteRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("ARENASORTLIST",where,pDelaySql);
}

void ArenaMgr::deleteRecordCB(bool opResult,UINT64 autoID)
{}

void ArenaMgr::updateYestodaySortList()
{
	UINT32 cur = g_globalServer->getSec();
	stYestodayArenaSortListDB dbData;
	dbData.m_rewardTimestamp = cur;
	std::map<UINT32,UINT32> & charID2Rank = m_historyCharID2Rank[cur];
	*(UINT16 *)(dbData.m_data) = s_MAX_REWARD_NUM;
	UINT32 * charID = (UINT32 *)(dbData.m_data + sizeof(UINT16));
	
	std::map<UINT32,UINT32>::iterator rankIt =  m_rank2CharID.begin();
	for (UINT16 i = 0; (i < s_MAX_REWARD_NUM) && (rankIt != m_rank2CharID.end()) ; ++i,++rankIt){
		charID[i] = rankIt->second;
		if (charID[i] > MAX_ROBOT_CHARID){//非机器人才给放在内存
			Player * pUser = PlayerManager::getSingleton().getUserByID(charID[i]);
			if (pUser){//在线情况下直接发
				stBatchSendMailTask task;
				char szMailContent[50] = { 0 };
				task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_HIGHLADDER_SORTLIST);
				snprintf(szMailContent,sizeof(szMailContent),"MailWnd_8;%d",i + 1);
				task.m_content = szMailContent;
				ArenaConfig::stArenaEverydayReward * pCfg = ArenaConfig::getSingleton().getArenaEverydayReward(i + 1);
				if (pCfg){
					if (pCfg->m_itemID1) {
						task.m_attaches.push_back(std::make_pair(pCfg->m_itemID1,pCfg->m_num1));   
					}

					if (pCfg->m_itemID2){
						task.m_attaches.push_back(std::make_pair(pCfg->m_itemID2,pCfg->m_num2));
					}
				}
				task.m_receiveIDs.insert(charID[i]);
				CBatchSendMailMgr::getSingleton().addTask(task);
			}
			else {
				charID2Rank[charID[i]] = i + 1;
			}
		}
	}
	
	Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::insertYestorySortListCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_arenaHistorySortList,(const char *)&dbData,"HISTORY_ARENA_SORTLIST",autoID,pDelaySql);

	while (m_historyCharID2Rank.size() > s_MAX_REWARD_DAY){
		std::map<UINT32,std::map<UINT32,UINT32> >::iterator it = m_historyCharID2Rank.begin();
		if (it != m_historyCharID2Rank.end()){
			char where[65] = { 0 };
			bzero(where, 65);
			snprintf(where, 64, "REWARD_TIMESTAMP=%d",it->first);
			Athena::CUDSqlCallBack<ArenaMgr> * pDelDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::deleteYestorySortListCB,false,0);
			CCUDSqlOperation * pDelDelaySql = ATHENA_NEW CCUDSqlOperation(pDelDelayCB);
			g_databaseEngine.deleteRecord("HISTORY_ARENA_SORTLIST",where,pDelDelaySql);
			
			//删除对应的已经领取奖励表
			Athena::CUDSqlCallBack<ArenaMgr> * pDelDelayCB1 = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::deleteGotRewardCharIDCB,false,0);
			CCUDSqlOperation * pDelDelaySql1 = ATHENA_NEW CCUDSqlOperation(pDelDelayCB1);
			g_databaseEngine.deleteRecord("ARENA_GOT_REWARD_CHARID",where,pDelDelaySql1);
			m_hasRewardCharID.erase(it->first);
			m_historyCharID2Rank.erase(it);
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("目前的历史排名奖励有:%d",m_historyCharID2Rank.size());
	{
		std::map<UINT32,std::map<UINT32,UINT32> >::iterator it = m_historyCharID2Rank.begin();
		for (;it != m_historyCharID2Rank.end();++it){
			Athena::logger->trace("timestamp:%d,size:%d",it->first,it->second.size());
		}
	}
#endif
}

void ArenaMgr::insertYestorySortListCB(bool opResult,UINT64 autoID)
{
	
}

void ArenaMgr::deleteYestorySortListCB(bool opResult,UINT64 autoID)
{
	
}

void ArenaMgr::deleteGotRewardCharIDCB(bool opResult,UINT64 autoID)
{}


UINT32 ArenaMgr::getMyRank(UINT32 charID)
{
	std::map<UINT32,stArenaSortInfo>::iterator it =  m_charID2SortInfo.find(charID);
	if (it != m_charID2SortInfo.end()){
		return  it->second.m_rank;	
	}
	return m_charID2SortInfo.size() + 1; 
}

void ArenaMgr::challege(Player * pUser,UINT32 charID)
{
	if (!pUser){
		return ;
	}
	
	std::map<UINT32,stArenaSortInfo>::iterator subIt =   m_charID2SortInfo.find(charID);
	if (subIt == m_charID2SortInfo.end()){
		return ;
	}

	UINT32 myRank = getMyRank(pUser->getID()); 
	INT32 rankInterval = std::max((INT32)(myRank * 0.2f / 10.0f),1);
	UINT32 beginRank = 0;
	if (myRank <= s_PKLIST_BEFORE_NUM){//大于s_PKLIST_BEFORE_NUM时候的处理
		beginRank = s_PKLIST_BEFORE_NUM + 1;
	}
	else {
		beginRank = myRank;
	}
	UINT32 validRankMin = beginRank - s_PKLIST_BEFORE_NUM * rankInterval;
	
	if (subIt->second.m_rank < validRankMin){//不能挑战超过这个名次的玩家
		Athena::logger->error("竞技场试图挑战一个比自己排名高许多的玩家,这小子使用了挂");
		return ;	
	}
	
	SortProto::stChallengeArenaUserResult retCmd;	
	stUserArenaInfo * pUserArenaInfo = getUserArenaInfo(pUser->getID());			
	if (pUserArenaInfo){
		if (pUserArenaInfo->m_challengeNum >= CMiscCfgMgr::getSingleton().getArenaChallengeNum()){
			retCmd.set_result(SortProto::stChallengeArenaUserResult::eTimeUseOut);
			pUser->sendProtoToMe(retCmd);
			return ;
		}

		if (pUserArenaInfo->m_lastPKTime + CMiscCfgMgr::getSingleton().getArenaCD() > g_globalServer->getSec() ){
			retCmd.set_result(SortProto::stChallengeArenaUserResult::eCoolDown);
			pUser->sendProtoToMe(retCmd);
			return ;
		}
	}

	OperateActivityMgr::getSingleton().checkOperateActivity(COperateActivityCfg::getSingleton().findIdByType(
            COperateActivityCfg::Athletics_Challenge), pUser->getID(), 1);
	if (subIt->second.m_robotID){//是机器人 
		Global::stBeginPKWithOther cmd;
		cmd.set_othercharid(charID);
		cmd.set_mycharid(pUser->getID());
		cmd.set_datasize(0);
		cmd.set_type(Global::stBeginPKWithOther::eArena_PK);
		cmd.set_challengerank(subIt->second.m_rank);
		cmd.set_robotid(subIt->second.m_robotID);
		pUser->sendProto2Game(cmd);
		return; 	
	}

	Player * pOther = PlayerManager::getSingleton().getUserByID(charID);
	if (pOther){
		Global::stBeginPKWithOther cmd;
		cmd.set_othercharid(charID);
		cmd.set_mycharid(pUser->getID());
		cmd.set_datasize(0);
		cmd.set_type(Global::stBeginPKWithOther::eArena_PK);
		cmd.set_challengerank(subIt->second.m_rank);
		pUser->sendProto2Game(cmd);
	}
	else {
		RoleInfoManager::getSingleton().loadOfflineUserChallege(pUser->getID(),charID,Global::stBeginPKWithOther::eArena_PK,subIt->second.m_rank);
	}
}

void ArenaMgr::userOnline(Player * pUser)
{
	if (!pUser){
		return;
	}
	
	notifyMyRank2Game(pUser);
	std::map<UINT32,std::map<UINT32,UINT32> >::iterator it = m_historyCharID2Rank.begin();

	for (;it != m_historyCharID2Rank.end();++it){
		std::map<UINT32,UINT32>::iterator subIt = it->second.find(pUser->getID());
		if (subIt != it->second.end()){			
			stMailDBInfo mailInfo;
			mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
			mailInfo.m_receiverID = pUser->getID();
			strncpy(mailInfo.m_receiverName,pUser->getName(),sizeof(mailInfo.m_receiverName));
			strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
			strncpy(mailInfo.m_title,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_HIGHLADDER_SORTLIST),sizeof(mailInfo.m_title));
			char mailCnt[250] = { 0 };
			snprintf(mailCnt,sizeof(mailCnt),"MailWnd_8;%d",subIt->second);
			strncpy(mailInfo.m_content,mailCnt,sizeof(mailInfo.m_content));
			mailInfo.m_sendTime = it->first;
			ArenaConfig::stArenaEverydayReward * pCfg = ArenaConfig::getSingleton().getArenaEverydayReward(subIt->second);
			if (pCfg){
				if (pCfg->m_itemID1){
					mailInfo.m_attachID1 = pCfg->m_itemID1;
					mailInfo.m_attachNum1 = pCfg->m_num1;
				}	

				if (pCfg->m_itemID2){
					mailInfo.m_attachID2 = pCfg->m_itemID2;
					mailInfo.m_attachNum2 = pCfg->m_num2;
				}
			}
			MailManager::getSingleton().sendMail(mailInfo,g_sendArenaMailCB);
			it->second.erase(subIt);
			//m_hasRewardCharID[it->first].insert(pUser->getID());
			saveGotRewardCharID(it->first,pUser->getID());
		}
	}
}

void ArenaMgr::saveGotRewardCharID(UINT32 timestamp,UINT32 charID)
{
	std::map<UINT32,std::set<UINT32> >::iterator it =  m_hasRewardCharID.find(timestamp);
	if (it == m_hasRewardCharID.end()){//还没有
		m_hasRewardCharID[timestamp].insert(charID);
		
		stHasGotRewardCharIDs data;
		data.m_rewardTimestamp = timestamp;
		*(UINT16 *)(data.m_data) = 1;
		UINT32 * pCharID = (UINT32 *)(data.m_data + sizeof(UINT16));
		pCharID[0] = charID;

		Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::addGotRewardCharIDRecordCB,false,0);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		UINT64 autoID = 0;
		g_databaseEngine.insertRecord(g_hasGotRewardCharIDTblDef,(const char *)&data,"ARENA_GOT_REWARD_CHARID",autoID,pDelaySql);
	}
	else {
		it->second.insert(charID);	
		char where[65] = { 0 };
		bzero(where, 65);
		snprintf(where, 64, "REWARD_TIMESTAMP=%u",timestamp);
		
		std::set<UINT32>::iterator subIt = it->second.begin();
		stHasGotRewardCharIDs data;
		data.m_rewardTimestamp = timestamp;
		*(UINT16 *)(data.m_data) = it->second.size();
		UINT32 * pCharID = (UINT32 *)(data.m_data + sizeof(UINT16));

		for (UINT16 i = 0;(i < s_MAX_REWARD_NUM) && (subIt != it->second.end());++i,++subIt){
			pCharID[i] = *subIt;	
		}

		Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::updateGotRewardCharIDRecordCB,false,0);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		g_databaseEngine.updateRecord(g_hasGotRewardCharIDTblDef,(const char *)&data,"ARENA_GOT_REWARD_CHARID",where,pDelaySql);
	}
}
	
void ArenaMgr::addGotRewardCharIDRecordCB(bool opResult,UINT64 autoID)
{
	
}

void ArenaMgr::updateGotRewardCharIDRecordCB(bool opResult,UINT64 autoID)
{

}

void ArenaMgr::getTop3(stHistoryRankInfo & info)
{
	std::map<UINT32,UINT32>::iterator it =  m_rank2CharID.find(1);	
	
	if (it != m_rank2CharID.end()){
		info.m_charID2Rank[it->second] = 1;	
	}

	it = m_rank2CharID.find(2);
	if (it != m_rank2CharID.end()){
		info.m_charID2Rank[it->second] = 2;	
	}
	
	it = m_rank2CharID.find(3);
	if (it != m_rank2CharID.end()){
		info.m_charID2Rank[it->second] = 3;	
	}
}

void ArenaMgr::notifyMyRank2Game(Player * pUser)
{
	if (!pUser){
		return ;
	}
	Global::stSynMyArenaRank retCmd;
	retCmd.set_charid(pUser->getID());
	retCmd.set_rank(getMyRank(pUser->getID()));
	pUser->sendProto2Game(retCmd);	
}
	
void ArenaMgr::addArenaUserInfo(const stUserArenaInfoDB & data)
{
	Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::addArenaUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_userArenaInfoblDef,(const char *)&data,"ARENA_USER_INFO",autoID,pDelaySql);
}

void ArenaMgr::addArenaUserInfoCB(bool opResult,UINT64 autoID)
{

}

void ArenaMgr::updateArenaUserInfo(const stUserArenaInfoDB & data,const char * szWhere)
{
	if (!szWhere){
		return ;
	}

	Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::updateArenaUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_userArenaInfoblDef,(const char *)&data,"ARENA_USER_INFO",szWhere,pDelaySql);
}

void ArenaMgr::updateArenaUserInfoCB(bool opResult,UINT64 autoID)
{
	
}

void ArenaMgr::resetUserArenaInfo()
{
	m_userArenaInfos.clear();
	char szWhere[65] = { 0 };
	snprintf(szWhere,sizeof(szWhere),"CHARID>0");
	
	Athena::CUDSqlCallBack<ArenaMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ArenaMgr>(this,&ArenaMgr::deleteArenaUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("ARENA_USER_INFO",szWhere,pDelaySql);
}

void ArenaMgr::deleteArenaUserInfoCB(bool opResult,UINT64 autoID)
{
	
}
	
stUserArenaInfo * ArenaMgr::getUserArenaInfo(UINT32 charID)
{
	UserArenaInfoMap_IT it = m_userArenaInfos.find(charID);

	if (it == m_userArenaInfos.end()){
		return NULL;
	}
	return &it->second;
}

void ArenaMgr::saveUserArenaInfo(stUserArenaInfo & data)
{
	UserArenaInfoMap_IT it = m_userArenaInfos.find(data.m_charID);
	stUserArenaInfoDB dbData;
	data.fillDBData(dbData);	

	if (it == m_userArenaInfos.end()){
		m_userArenaInfos[data.m_charID] = data;
		addArenaUserInfo(dbData);
	}
	else {
		char szWhere[65] = {  0 };
		snprintf(szWhere,sizeof(szWhere)," CHARID=%d ",data.m_charID);
		updateArenaUserInfo(dbData,szWhere);	
	}
}
	
void ArenaMgr::resetChallengeNum(Player * pUser)
{
	if (!pUser){
		return ;
	}

	SortProto::stResetArenaChallengeNunResult retCmd;

	UserArenaInfoMap_IT it = m_userArenaInfos.find(pUser->getID());

	if (it == m_userArenaInfos.end()){
		stUserArenaInfo userInfo;
		userInfo.m_charID = pUser->getID();
		m_userArenaInfos[pUser->getID()] = userInfo;
		saveUserArenaInfo(userInfo);
	}
	else {
		it->second.m_resetNum += 1;
		it->second.m_challengeNum = 0;
		stUserArenaInfo userInfo = it->second;
		saveUserArenaInfo(userInfo);
	}
	
	retCmd.set_result(SortProto::stResetArenaChallengeNunResult::eSuccess);
	pUser->sendProtoToMe(retCmd);
}

void ArenaMgr::clearCD(Player * pUser)
{
	if (!pUser){
		return ;
	}
	UserArenaInfoMap_IT it = m_userArenaInfos.find(pUser->getID());

	if (it == m_userArenaInfos.end()){
		stUserArenaInfo userInfo;
		userInfo.m_charID = pUser->getID();
		userInfo.m_lastPKTime = 0;
		m_userArenaInfos[pUser->getID()] = userInfo;
		saveUserArenaInfo(userInfo);
	}
	else {
		it->second.m_lastPKTime = 0;
		stUserArenaInfo userInfo = it->second;
		saveUserArenaInfo(userInfo); 
	}

	SortProto::stClearArenaCDResult retCmd;
	retCmd.set_result(SortProto::stClearArenaCDResult::eSuccess);
	pUser->sendProtoToMe(retCmd);
}

void ArenaMgr::changeUserArenaInfo(Player * pUser)
{
	if (!pUser){
		return;
	}	
	
	UserArenaInfoMap_IT it = m_userArenaInfos.find(pUser->getID());

	if (it == m_userArenaInfos.end()){
		stUserArenaInfo userInfo;
		userInfo.m_charID = pUser->getID();
		userInfo.m_lastPKTime = g_globalServer->getSec();
		userInfo.m_challengeNum = 1;
		saveUserArenaInfo(userInfo);
		m_userArenaInfos[pUser->getID()] = userInfo;
	}
	else {
		it->second.m_lastPKTime = g_globalServer->getSec();
		it->second.m_challengeNum += 1;
		stUserArenaInfo userInfo = it->second;
		saveUserArenaInfo(userInfo); 
	}
}

void ArenaMgr::synRecord(Player * pUser)
{
	if(!pUser){
		return ;
	}

	std::map<UINT32,stArenaSortInfo>::iterator it = m_charID2SortInfo.find(pUser->getID());
	if(it != m_charID2SortInfo.end()){
		stArenaSortInfo & mySortInfo = it->second;
		strncpy(mySortInfo.m_name, pUser->getName(), sizeof(mySortInfo.m_name));

		stArenaSortInfoDB dbData;
		mySortInfo.fillDBData(dbData);
		updateRecord(dbData);
	}
}
