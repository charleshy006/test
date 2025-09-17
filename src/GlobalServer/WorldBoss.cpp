#include "WorldBoss.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "WorldBossCfg.h"
#include "Misc.pb.h"
#include "Team.h"
#include "Role.pb.h"
#include "Mail.h"
#include "TblDataMgr.h"
#include "MiscCfgMgr.h"
#include "SysLanguages.h"

CTblField g_worldBossActivityTblDef[] = {
	{ "ID",    	   CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "LEVELID",   CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BOSSID",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BEGINTIME", CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ENDTIME",   CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "IS_START",  CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ NULL, 0, 0 },
};

CTblField g_worldBossDamageTblDef[] = {
	{ "CHARID",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",      CTblField::DB_TYPE_STRING,  33   			},  
	{ "DAMAGE",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "JOB", 	   CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",  	   CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ NULL, 0, 0 },
};

CTblField g_worldBossBuyInfTblDef[] = {
	{ "CHARID",    		  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "LASTLEAVETIME",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BUY_NUM",   		  CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

CTblField g_worldWorldTblDef[] = {
	{ "BOSSID",    		 CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BOSSHP",   		 CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "STEPID",   		 CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

void stWorldBossActivityInfo::initFromDB(const stWorldBossActivityDB & data)
{
	m_id = data.m_id;
	m_levelID = data.m_levelID;
	m_bossID = data.m_bossID;
	m_beginTime = data.m_beginTime;
	m_endTime = data.m_endTime;
	m_isStart = data.m_isStart;
}

void stWorldBossActivityInfo::fillDBData(stWorldBossActivityDB & data)
{
	data.m_id = m_id;
	data.m_levelID = m_levelID;
	data.m_beginTime = m_beginTime;
	data.m_endTime = m_endTime;
	data.m_bossID = m_bossID;
	data.m_isStart = m_isStart;
}

void stWorldBossDamageRankInfo::initFromDB(const stWorldBossDamageRankDB & data)
{
	m_charID = data.m_charID;
	strncpy(m_name,data.m_name,sizeof(m_name));
	m_damage = data.m_damage;
	m_job = data.m_job;
	m_sex = data.m_sex;
	m_photoID = data.m_photoID;
}

void stWorldBossDamageRankInfo::fillDBData(stWorldBossDamageRankDB & data)
{
	data.m_charID = m_charID;
	strncpy(m_name,data.m_name,sizeof(m_name));
	data.m_damage = m_damage;
	data.m_job = m_job;
	data.m_sex = m_sex;
	data.m_photoID = m_photoID;
}

void stWorldBossBuyInfo::initFromDB(const stWorldBossBuyDB & data)
{
	m_charID = data.m_charID;
	m_lastLeaveTime = data.m_lastLeaveTime;
	m_buyNum = data.m_buyNum;
}

void stWorldBossBuyInfo::fillDBData(stWorldBossBuyDB & data)
{
	data.m_charID = m_charID;
	data.m_lastLeaveTime = m_lastLeaveTime;
	data.m_buyNum = m_buyNum;
}

void stWorldBossInfo::initFromDB(const stWorldBossDB & data)
{
	m_bossID = data.m_bossID;
	m_hp = data.m_hp;
	m_stepID = data.m_stepID;
}

void stWorldBossInfo::fillDBData(stWorldBossDB & data)
{
	data.m_bossID = m_bossID;
	data.m_hp = m_hp;
	data.m_stepID = m_stepID;
}

CWorldBossMgr::~CWorldBossMgr()
{

}

bool CWorldBossMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_worldBossActivityTblDef," WORLDBOSS_ACTIVITY ",NULL,NULL,0,NULL);

	if (!result || !result->getData()){
		UINT64 autoID = 0;
		stWorldBossActivityDB dbData;
		g_databaseEngine.insertRecord(g_worldBossActivityTblDef,(const char *)&dbData,"WORLDBOSS_ACTIVITY",autoID,NULL);
		m_bossActivityInfo.m_id = autoID;
	}
	else if (result && result->getData()){
		stWorldBossActivityDB * pDBData = (stWorldBossActivityDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			m_bossActivityInfo.initFromDB(*pDBData);
		}
	}
	
	SAFE_DELETE(result);
	result = g_databaseEngine.select(g_worldBossBuyInfTblDef," WORLD_BOSS_BUY_INFO ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stWorldBossBuyDB * pDBData = (stWorldBossBuyDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stWorldBossBuyInfo &info = m_buyInfos[pDBData[i].m_charID];
			info.initFromDB(pDBData[i]);
		}	
	}
	SAFE_DELETE(result);

	result = g_databaseEngine.select(g_worldBossDamageTblDef," WORLD_BOSS_RANK ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stWorldBossDamageRankDB  * pDBData = (stWorldBossDamageRankDB  * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stWorldBossDamageRankInfo info;
			info.initFromDB(pDBData[i]);
			m_damageSortList.insertSortInfo(info);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("世界副本伤害排名:charID:%d,name:%s,job:%d,sex:%d,damage:%d",info.m_charID,info.m_name,info.m_job,info.m_sex,info.m_damage);
#endif
		}
	}

	result = g_databaseEngine.select(g_worldWorldTblDef," WORLDBOSS ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stWorldBossDB  * pDBData = (stWorldBossDB  * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stWorldBossInfo info;
			info.initFromDB(pDBData[i]);
			m_bossInfos.insert(std::make_pair(info.m_bossID,info));
#ifdef _HDZ_DEBUG
			Athena::logger->trace("世界boss信息:bossID:%d,hp:%d,stepID:%d",info.m_bossID,info.m_hp,info.m_stepID);
#endif
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("bossID:%d,beginTime:%d",m_bossActivityInfo.m_bossID,m_bossActivityInfo.m_beginTime);
	std::map<UINT32,stWorldBossBuyInfo>::iterator it = m_buyInfos.begin();
	for (;it != m_buyInfos.end();++it){
		stWorldBossBuyInfo & info = it->second;
		Athena::logger->trace("charID:%d,lastleaveTime:%d,buyNum:%d",info.m_charID,info.m_lastLeaveTime,info.m_buyNum);	
	}
#endif
	return true;
}

void CWorldBossMgr::addRecord(stWorldBossActivityDB & data)
{
	Athena::CUDSqlCallBack<CWorldBossMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CWorldBossMgr>(this,&CWorldBossMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_worldBossActivityTblDef,(const char *)&data,"WORLDBOSS_ACTIVITY",autoID,pDelaySql);
}

void CWorldBossMgr::addRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CWorldBossMgr::updateRecord(stWorldBossActivityDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `ID`=%u", data.m_id);

	Athena::CUDSqlCallBack<CWorldBossMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CWorldBossMgr>(this,&CWorldBossMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_worldBossActivityTblDef,(const char *)&data,"WORLDBOSS_ACTIVITY",where,pDelaySql);
}

void CWorldBossMgr::updateRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CWorldBossMgr::addWorldBossRecord(stWorldBossDB & data)
{
	Athena::CUDSqlCallBack<CWorldBossMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CWorldBossMgr>(this,&CWorldBossMgr::addWorldBossRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_worldWorldTblDef,(const char *)&data,"WORLDBOSS",autoID,pDelaySql);
}

void CWorldBossMgr::addWorldBossRecordCB(bool opResult,UINT64 autoID)
{}

void CWorldBossMgr::updateWorldBossRecord(stWorldBossDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `BOSSID`=%u", data.m_bossID);

	Athena::CUDSqlCallBack<CWorldBossMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CWorldBossMgr>(this,&CWorldBossMgr::updateWorldBossRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_worldWorldTblDef,(const char *)&data,"WORLDBOSS",where,pDelaySql);

}

void CWorldBossMgr::updateWorldBossRecordCB(bool opResult,UINT64 autoID)
{}

void CWorldBossMgr::timer(UINT32 cur)
{
	struct tm _tm;
	RealTime::getLocalTime(_tm,cur);
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

	if (m_bossActivityInfo.m_beginTime <= 0){//还没有开启
		if (CWorldBossCfg::getSingleton().isInOpenTime(secNum)){//副本活动开启
			CWorldBossCfg::stActivityOpenInfo *pActivityInfo =  CWorldBossCfg::getSingleton().getCurActivityInfo(secNum);
			if (pActivityInfo) {
				activityStart();
				m_bossActivityInfo.m_beginTime = pActivityInfo->m_beginTime;
				m_bossActivityInfo.m_endTime = pActivityInfo->m_endTime;
				m_bossActivityInfo.m_bossID = pActivityInfo->m_bossID;
				m_bossActivityInfo.m_levelID = pActivityInfo->m_copymapID;

				std::map<UINT32,stWorldBossInfo>::iterator it =  m_bossInfos.find(m_bossActivityInfo.m_bossID);
				if (it == m_bossInfos.end()){
					stWorldBossInfo & info = m_bossInfos[m_bossActivityInfo.m_bossID];	
					info.m_bossID = m_bossActivityInfo.m_bossID;
					stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(info.m_bossID,1));
					if (pMonsterBase){
						info.m_hp = pMonsterBase->m_hp;
					}
					info.m_stepID = 1;

					stWorldBossDB worldBossDB;
					info.fillDBData(worldBossDB);
					addWorldBossRecord(worldBossDB);
				}
				else {
					stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(it->second.m_bossID,it->second.m_stepID));
					if (pMonsterBase){
						it->second.m_hp = pMonsterBase->m_hp;
					}
					stWorldBossDB worldBossDB;
					it->second.fillDBData(worldBossDB);
					updateWorldBossRecord(worldBossDB);
				}

				m_bossActivityInfo.m_isStart = 1;
				stWorldBossActivityDB dbData;
				m_bossActivityInfo.fillDBData(dbData);
				updateRecord(dbData);
			}
		}		
	}
	else if (m_bossActivityInfo.m_beginTime > 0){
		if (m_bossActivityInfo.m_isStart && (!CWorldBossCfg::getSingleton().isInOpenTime(secNum))){//结束
			activityEnd();
			m_bossActivityInfo.m_isStart = 0;
			m_bossActivityInfo.m_beginTime = 0;
			stWorldBossActivityDB dbData;
			m_bossActivityInfo.fillDBData(dbData);
			updateRecord(dbData);
		}
	}

	if (m_bossActivityInfo.m_isStart){//检测匹配
		if(m_checkMatchTick > 0){
			m_checkMatchTick--;
			if(m_checkMatchTick<=0){
				checkMatch();
			}
		}
	}
}

void CWorldBossMgr::activityStart()
{
	Athena::logger->trace("世界boss活动开启了!");	
	m_damageSortList.clear();
	dropDamageSortListTable();
}

void CWorldBossMgr::activityEnd()
{
	Athena::logger->trace("世界boss活动结束了!");
	MiscProto::stRetBossWorldMatch retCmd;
	retCmd.set_result(MiscProto::stRetBossWorldMatch::eActivity_End);

	std::set<UINT32>::iterator it = m_waitMatches.begin();
	for (;it != m_waitMatches.end();++it){
		Player * pUser = PlayerManager::getSingleton().getUserByID(*it);
		if (pUser){
			pUser->sendProtoToMe(retCmd);
		}
	}
	m_waitMatches.clear();
	
	/*Role::stChatCmd notifyCmd;
	notifyCmd.set_type(Role::CHAT_TYPE_NOTICE);
	notifyCmd.set_content("世界boss结束了");
	notifyCmd.set_srccharid(0);
	notifyCmd.set_srccharname("系统");
	PlayerManager::getSingleton().sendProto2AllZone(notifyCmd);*/

	//发送奖励
	std::vector<CWorldBossCfg::stRewardInfo> &rewardVec = CWorldBossCfg::getSingleton().getReward();
	for (UINT16 i = 0;i < rewardVec.size();++i){
		CWorldBossCfg::stRewardInfo & reward = rewardVec[i];
		std::vector<stWorldBossDamageRankInfo> vec;
		m_damageSortList.getFromN12N2(vec,reward.m_beginRank,reward.m_endRank);	

		stBatchSendMailTask task;
		char szMailContent[50] = { 0 };
		task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_WORLD_BOSS_REWARD);
		snprintf(szMailContent,sizeof(szMailContent),"MailWnd_15;%d",reward.m_beginRank);
		task.m_content = szMailContent;
		task.m_attaches.push_back(std::make_pair(reward.m_itemID,1));	
		for (UINT16 j = 0;j < vec.size();++j){
			task.m_receiveIDs.insert(vec[j].m_charID);
		}

		CBatchSendMailMgr::getSingleton().addTask(task);
	}
	//m_damageSortList.clear();
	//dropDamageSortListTable();
	m_buyInfos.clear();
	dropBuyInfo();
}

void CWorldBossMgr::updateMyDamage(Player * pUser,UINT32 damage)
{
	if (!pUser){
		return ;
	}	

	if (m_damageSortList.isInSortList(pUser->getID())){
		stWorldBossDamageRankInfo info = m_damageSortList.getSortInfo(pUser->getID()); 
		info.m_damage += damage;
		m_damageSortList.updateSortInfo(info);
	}   
	else {
		stWorldBossDamageRankInfo info;
		info.m_charID = pUser->getID();
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_job = pUser->m_job;
		info.m_sex = pUser->m_sex;
		info.m_damage = damage;
		info.m_photoID = pUser->m_photoID;
		m_damageSortList.insertSortInfo(info);
	} 
}

void CWorldBossMgr::sendDamageSortList(Player * pUser,UINT32 fromRank)
{
	if (!pUser){
		return ;
	}

	MiscProto::stRetWorldBossDamageRank retCmd;
	std::vector<stWorldBossDamageRankInfo> vec;
	m_damageSortList.getFromN12N2(vec,fromRank,fromRank + s_SEND_SORTITEM_MAX);
	retCmd.set_fromrank(fromRank);
	retCmd.set_myrank(m_damageSortList.getMySortRank(pUser->getID()));

	for (UINT16 i = 0;i < vec.size();++i){
		MiscProto::stWorldBossDamageSortItem * pSyn = retCmd.add_sortitem();
		stWorldBossDamageRankInfo & info = vec[i];
		if (pSyn){
			pSyn->set_charid(info.m_charID);
			pSyn->set_name(info.m_name);
			pSyn->set_job(info.m_job);
			pSyn->set_sex(info.m_sex);
			pSyn->set_damage(info.m_damage);
			pSyn->set_photoid(info.m_photoID);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CWorldBossMgr::userOffline(Player * pUser)
{
	if (!pUser){
		return ;
	}	
	m_waitMatches.erase(pUser->getID());
}

void CWorldBossMgr::cancelMatch(Player * pUser)
{
	if (!pUser){
		return ;
	}
	
	m_waitMatches.erase(pUser->getID());
	
	MiscProto::stCancelWorldBossMatchResult retCmd;
	retCmd.set_result(MiscProto::stCancelWorldBossMatchResult::eSuccess);
	pUser->sendProtoToMe(retCmd);
}

void CWorldBossMgr::userEnterMatch(Player * pUser)
{
	if (!pUser){
		return ;
	}
	struct tm _tm;
	RealTime::getLocalTime(_tm,g_globalServer->getSec());
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

	MiscProto::stRetBossWorldMatch retCmd;

	if (!CWorldBossCfg::getSingleton().isInOpenTime(secNum)){
		retCmd.set_result(MiscProto::stRetBossWorldMatch::eNot_Open);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	if (pUser->m_level < CMiscCfgMgr::getSingleton().getWorldBossLvLimit()){
		retCmd.set_result(MiscProto::stRetBossWorldMatch::eLevel_Too_Low);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	if (getLastLeaveTime(pUser->getID()) + s_CHALLEGE_CD > g_globalServer->getSec()){
		retCmd.set_result(MiscProto::stRetBossWorldMatch::eCool_Down);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	UINT32 worldBossHP = 0;
	UINT16 worldBossLv = 1;
	std::map<UINT32,stWorldBossInfo>::iterator bossIt =  m_bossInfos.find(m_bossActivityInfo.m_bossID);
	if (bossIt != m_bossInfos.end()){
		worldBossHP = bossIt->second.m_hp;
		worldBossLv = bossIt->second.m_stepID;
	}
	else {
		Athena::logger->trace("请求世界boss匹配失败,不存在的boss");
		return ;
	}
	
	if (worldBossHP <= 0){
		retCmd.set_result(MiscProto::stRetBossWorldMatch::eBoss_Die);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	m_waitMatches.insert(pUser->getID());

	Athena::logger->trace("CWorldBossMgr::userEnterMatch  userid=%d", pUser->getID());

	if (m_waitMatches.size() >= s_MAX_MATCH_NUM){
		retCmd.set_result(MiscProto::stRetBossWorldMatch::eSuccess);
		pUser->sendProtoToMe(retCmd);

		checkMatch();
	}
	else {
		retCmd.set_result(MiscProto::stRetBossWorldMatch::eWait);
		pUser->sendProtoToMe(retCmd);

		if(m_checkMatchTick <=0){
			m_checkMatchTick = s_MATCH_ROBOT;
		}
	}
}
//检测匹配
void CWorldBossMgr::checkMatch(bool force)
{
	if(m_waitMatches.empty())
	{
		return;
	}

	std::map<UINT32,stWorldBossInfo>::iterator bossIt =  m_bossInfos.find(m_bossActivityInfo.m_bossID);
	if (bossIt == m_bossInfos.end()){
		return;
	}

	Global::stTeamEnterCopyMap cmd;
	cmd.set_copymapid(m_bossActivityInfo.m_levelID);
	cmd.set_teamid(0);
	cmd.set_bosshp(bossIt->second.m_hp);
	cmd.set_bosslv(bossIt->second.m_stepID);

	Player * pUser = NULL;

	std::set<UINT32>::iterator it = m_waitMatches.begin();
	for (;it != m_waitMatches.end();){
		std::set<UINT32>::iterator tmpSubIt = it;
		++it;
		Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
		if (pMemInfo){
			pMemInfo->set_charid(*tmpSubIt);
			pMemInfo->set_adddamageper(getAddDamagePer(*tmpSubIt));
		}
		if(!pUser){
			pUser = PlayerManager::getSingleton().getUserByID(*tmpSubIt);
		}
		Athena::logger->trace("CWorldBossMgr::checkMatch  userid=%d", *tmpSubIt);
		m_waitMatches.erase(tmpSubIt);
		
		if (cmd.memerinfo_size() >= CTeam::s_MAX_MEMBER){   
			break;
		}   
	}
	if(pUser){
		g_globalServer->getConnMgr().broadcastByID(pUser->getGameID(),cmd);
	}
	if(!m_waitMatches.empty()){
		m_checkMatchTick = s_MATCH_ROBOT;
	}
}

UINT32 CWorldBossMgr::getWorldBossHP()
{
	std::map<UINT32,stWorldBossInfo>::iterator bossIt =  m_bossInfos.find(m_bossActivityInfo.m_bossID);
	if (bossIt != m_bossInfos.end()){
		return bossIt->second.m_hp;
	}
	return 0;
}

void CWorldBossMgr::openWin(Player * pUser)
{
	if (!pUser){
		return ;
	}
	
	MiscProto::stRetOpenWorldBossWin retCmd;
	std::map<UINT32,stWorldBossBuyInfo>::iterator it = m_buyInfos.find(pUser->getID());
	if (it != m_buyInfos.end()){
		stWorldBossBuyInfo & info = it->second;
		retCmd.set_buynum(info.m_buyNum);
		UINT32 remainTime = (info.m_lastLeaveTime  + s_CHALLEGE_CD > g_globalServer->getSec() ? info.m_lastLeaveTime + s_CHALLEGE_CD - g_globalServer->getSec() : 0);
		retCmd.set_reliveremaintime(remainTime);
	}

	if (m_bossActivityInfo.m_isStart){
		retCmd.set_begintime(m_bossActivityInfo.m_beginTime);
		retCmd.set_endtime(m_bossActivityInfo.m_endTime);
	}
	else {
		struct tm _tm;
		RealTime::getLocalTime(_tm,g_globalServer->getSec());
		UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

		CWorldBossCfg::stActivityOpenInfo * pActivityInfo = CWorldBossCfg::getSingleton().getNextActivityInfo(secNum);
		if (pActivityInfo){
			retCmd.set_begintime(pActivityInfo->m_beginTime);
			retCmd.set_endtime(pActivityInfo->m_endTime);
			if (pActivityInfo->m_beginTime < secNum){
				retCmd.set_tomorrow(true);	
			}
		}
	}
	retCmd.set_killername(m_killerName);
	std::map<UINT32,stWorldBossInfo>::iterator bossIt =  m_bossInfos.find(m_bossActivityInfo.m_bossID);
	if (bossIt != m_bossInfos.end()){
		retCmd.set_stepid(bossIt->second.m_stepID);
		retCmd.set_hp(bossIt->second.m_hp);
		retCmd.set_bossid(bossIt->second.m_bossID);
	}
	pUser->sendProtoToMe(retCmd);
}
	
void CWorldBossMgr::clearCD(UINT32 charID)
{
	std::map<UINT32,stWorldBossBuyInfo>::iterator it = m_buyInfos.find(charID);
	if (it == m_buyInfos.end()){
		stWorldBossBuyInfo info;
		info.m_charID = charID;
		info.m_buyNum = 0;
		info.m_lastLeaveTime = 0;
		m_buyInfos.insert(std::make_pair(charID,info));

		stWorldBossBuyDB dbData;
		info.fillDBData(dbData);
		addBuyInfoRecord(dbData);
	}
	else {
		it->second.m_lastLeaveTime = 0;

		stWorldBossBuyDB dbData;
		it->second.fillDBData(dbData);
		updateBuyInfoRecord(dbData);
	}
}

UINT16 CWorldBossMgr::getAddDamagePer(UINT32 charID)
{
	std::map<UINT32,stWorldBossBuyInfo>::iterator it = m_buyInfos.find(charID);
	if (it != m_buyInfos.end()){
		return it->second.m_buyNum * s_ADD_DAMAGE_PER_TIME;
	}
	return 0;
}

void CWorldBossMgr::reqBuy(Player * pUser)
{
	if (!pUser){
		return ;
	}
	
	MiscProto::stRaiseDamageInWorldBossResult retCmd;

	UINT16 buyNum = 0;
	std::map<UINT32,stWorldBossBuyInfo>::iterator it = m_buyInfos.find(pUser->getID());
	if (it != m_buyInfos.end()){
		buyNum = it->second.m_buyNum;
	}
	
	if (buyNum >= CMiscCfgMgr::getSingleton().getWorldBossMaxBuyNum()){
		retCmd.set_result(MiscProto::stRaiseDamageInWorldBossResult::eTime_Out);
		pUser->sendProtoToMe(retCmd);
		return ;	
	}

	Global::stWorldBossBuyDecDiamond cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_hasbuynum(buyNum);
	pUser->sendProto2Game(cmd);
}

void CWorldBossMgr::setBuyNum(UINT32 charID,UINT16 buyNum)
{
	std::map<UINT32,stWorldBossBuyInfo>::iterator it = m_buyInfos.find(charID);
	if (it == m_buyInfos.end()){
		stWorldBossBuyInfo info;
		info.m_charID = charID;
		info.m_buyNum = buyNum;
		info.m_lastLeaveTime = 0;
		m_buyInfos.insert(std::make_pair(charID,info));

		stWorldBossBuyDB dbData;
		info.fillDBData(dbData);
		addBuyInfoRecord(dbData);
	}
	else {
		it->second.m_buyNum = buyNum;

		stWorldBossBuyDB dbData;
		it->second.fillDBData(dbData);
		updateBuyInfoRecord(dbData);
	}
}

UINT32 CWorldBossMgr::getLastLeaveTime(UINT32 charID)
{
	std::map<UINT32,stWorldBossBuyInfo>::iterator it = m_buyInfos.find(charID);
	if (it != m_buyInfos.end()){
		return it->second.m_lastLeaveTime;
	}
	return 0;
}

void CWorldBossMgr::setLastLeaveTime(UINT32 charID,UINT32 cur)
{
	std::map<UINT32,stWorldBossBuyInfo>::iterator it = m_buyInfos.find(charID);
	if (it == m_buyInfos.end()){
		stWorldBossBuyInfo info;
		info.m_charID = charID;
		info.m_buyNum = 0;
		info.m_lastLeaveTime = cur;
		m_buyInfos.insert(std::make_pair(charID,info));

		stWorldBossBuyDB dbData;
		info.fillDBData(dbData);
		addBuyInfoRecord(dbData);
	}
	else {
		it->second.m_lastLeaveTime = cur;

		stWorldBossBuyDB dbData;
		it->second.fillDBData(dbData);
		updateBuyInfoRecord(dbData);
	}
}

void CWorldBossMgr::addBuyInfoRecord(stWorldBossBuyDB & data)
{
	Athena::CUDSqlCallBack<CWorldBossMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CWorldBossMgr>(this,&CWorldBossMgr::addBuyInfoRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_worldBossBuyInfTblDef,(const char *)&data,"WORLD_BOSS_BUY_INFO",autoID,pDelaySql);
}
	
void CWorldBossMgr::addBuyInfoRecordCB(bool opResult,UINT64 autoID)
{}

void CWorldBossMgr::updateBuyInfoRecord(stWorldBossBuyDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `CHARID`=%u", data.m_charID);

	Athena::CUDSqlCallBack<CWorldBossMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CWorldBossMgr>(this,&CWorldBossMgr::updateBuyInfoRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_worldBossBuyInfTblDef,(const char *)&data,"WORLD_BOSS_BUY_INFO",where,pDelaySql);
}

void CWorldBossMgr::updateBuyInfoRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CWorldBossMgr::updateBossHP(UINT32 hp)
{
	std::map<UINT32,stWorldBossInfo>::iterator it = m_bossInfos.find(m_bossActivityInfo.m_bossID);
	if (it != m_bossInfos.end()){
		
		if (0 == it->second.m_hp){//boss已经死亡
			return ;
		}

		if (it->second.m_hp > hp){
			it->second.m_hp -= hp;
		}
		else {
			it->second.m_hp = 0;
		}
		
		if (it->second.m_hp <= 0){
			stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(it->second.m_bossID,it->second.m_stepID));
			if (pMonsterBase){
				it->second.m_hp = (UINT32)(pMonsterBase->m_hp * 0.1);
			}
		}

		stWorldBossDB dbData;
		it->second.fillDBData(dbData);
		updateWorldBossRecord(dbData);
	}
}

void CWorldBossMgr::incBossStep()
{
	std::map<UINT32,stWorldBossInfo>::iterator it = m_bossInfos.find(m_bossActivityInfo.m_bossID);
	if (it != m_bossInfos.end()){
		it->second.m_stepID += 1;
		it->second.m_hp = 0;
		stWorldBossDB dbData;
		it->second.fillDBData(dbData);
		updateWorldBossRecord(dbData);
	}
}

void CWorldBossMgr::dropDamageSortListTable()
{
	char where[65] = { 0 };
	bzero(where, 65);   
	snprintf(where, 64, " ID > 0 ");
	Athena::CUDSqlCallBack<CWorldBossMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CWorldBossMgr>(this,&CWorldBossMgr::dropDamageSortListTableCB,false,0);    
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("WORLD_BOSS_RANK",where,pDelaySql);
}

void CWorldBossMgr::dropDamageSortListTableCB(bool opResult,UINT64 autoID)
{

}

void CWorldBossMgr::dropBuyInfo()
{
	char where[65] = { 0 };
	bzero(where, 65);   
	snprintf(where, 64, " ID > 0 ");
	Athena::CUDSqlCallBack<CWorldBossMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CWorldBossMgr>(this,&CWorldBossMgr::dropBuyInfoCB,false,0);    
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("WORLD_BOSS_BUY_INFO",where,pDelaySql);

}

void CWorldBossMgr::dropBuyInfoCB(bool opResult,UINT64 autoID)
{

}

void CWorldBossMgr::synRecord(Player * pUser)
{
	if(!pUser){
		return;
	}

	if (m_damageSortList.isInSortList(pUser->getID())){
		stWorldBossDamageRankInfo info = m_damageSortList.getSortInfo(pUser->getID());
		if(0 == strcmp(m_killerName.c_str(), info.m_name)){
			m_killerName = pUser->getName();
		}
		strncpy(info.m_name, pUser->getName(), sizeof(info.m_name));
		info.m_photoID = pUser->m_photoID;
		m_damageSortList.updateSortInfo(info);
	}
}
