#include "EmeraldDreamMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "SortList.pb.h"
#include "Mail.h"
#include "SysLanguages.h"
#include "PlayerManager.h"
#include "EmeraldDreamCfg.h"

extern GlobalServer * g_globalServer;

CTblField g_emeraldDreamRankTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "DAMAGE",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "JOB",            CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",            CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "LEVEL",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

CTblField g_emeraldDreamOpenTimeTblDef[] = {
	{ "ID",				CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ACTIVITY_ID",	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)	 },
	{ "BEGIN_TIME",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "END_TIME",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) 	 },
	{ NULL, 0, 0 },
};

void stEmeraldDreamOpenTimeInfo::initFromDB(stEmeraldDreamOpenTimeDB & data)
{
	m_id			=	data.m_id;
	m_activityID	=	data.m_activityID;                //活动ID
	m_beginTime		=	data.m_beginTime;                 //开始时间
	m_endTime		=	data.m_endTime;                   //结束时间}
}

void stEmeraldDreamOpenTimeInfo::fillDBData(stEmeraldDreamOpenTimeDB & data)
{
	data.m_id			=	m_id;
	data.m_activityID	=	m_activityID;                //活动ID
	data.m_beginTime	=	m_beginTime;                 //开始时间
	data.m_endTime		=	m_endTime;                   //结束时间}
}

void CEmeraldDreamMgr::load()
{
	CTblQueryResult * result = NULL;
	result = g_databaseEngine.select(g_emeraldDreamRankTblDef,"EMERALD_DREAM_RANK",NULL,NULL,0,NULL);
	if (result){
		stEmeraldDreamRankDB * pEmeraldDreamInfo = (stEmeraldDreamRankDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stEmeraldDreamRankInfo info ;
			info.m_charID = pEmeraldDreamInfo[i].m_charID;
			strncpy(info.m_name,pEmeraldDreamInfo[i].m_name,sizeof(info.m_name));
			info.m_damage = pEmeraldDreamInfo[i].m_damage;
			info.m_job = pEmeraldDreamInfo[i].m_job;
			info.m_sex = pEmeraldDreamInfo[i].m_sex;
			info.m_level = pEmeraldDreamInfo[i].m_level;
			info.m_photoID = pEmeraldDreamInfo[i].m_photoID;
			m_emeraldDreamSortList.insertSortInfo(info);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("翡翠梦境排行榜记录:charID:%d, name: %s, damage: %d, job: %d, sex: %d, level: %d",pEmeraldDreamInfo[i].m_charID,pEmeraldDreamInfo[i].m_name,pEmeraldDreamInfo[i].m_damage, pEmeraldDreamInfo[i].m_job, pEmeraldDreamInfo[i].m_sex, pEmeraldDreamInfo[i].m_level);
#endif
		}
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("翡翠梦境排行榜记录数为:%d",m_emeraldDreamSortList.getListNum());
#endif
	result = g_databaseEngine.select(g_emeraldDreamOpenTimeTblDef,"EMERALD_DREAM_OPEN_TIME",NULL,NULL,0,NULL);
	if(result && result->getData()){
		stEmeraldDreamOpenTimeDB * pDBData = (stEmeraldDreamOpenTimeDB *) result->getData();
		if(pDBData){
			m_emeraldDreamOpenTimeInfo.initFromDB(pDBData[0]);
		}
	}else{
		UINT64 autoID = 0;
		m_emeraldDreamOpenTimeInfo.m_activityID = 0;
		m_emeraldDreamOpenTimeInfo.m_beginTime = 0;
		m_emeraldDreamOpenTimeInfo.m_endTime = 0;
		stEmeraldDreamOpenTimeDB dbData;
		m_emeraldDreamOpenTimeInfo.fillDBData(dbData);
		g_databaseEngine.insertRecord(g_emeraldDreamOpenTimeTblDef,(const char *)&dbData,"EMERALD_DREAM_OPEN_TIME",autoID,NULL);
		m_emeraldDreamOpenTimeInfo.m_id = autoID;
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("翡翠梦境开启情况:id:%d,activityid:%d, beginTime:%d, endTime:%d",m_emeraldDreamOpenTimeInfo.m_id, m_emeraldDreamOpenTimeInfo.m_activityID, m_emeraldDreamOpenTimeInfo.m_beginTime, m_emeraldDreamOpenTimeInfo.m_endTime);
#endif
}

//增加一个记录
void CEmeraldDreamMgr::addRecord(const char * pData)
{
	Athena::CUDSqlCallBack<CEmeraldDreamMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CEmeraldDreamMgr>(this,&CEmeraldDreamMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_emeraldDreamRankTblDef, pData, "EMERALD_DREAM_RANK", autoID, pDelaySql);
}

//增加记录回调                                 
void CEmeraldDreamMgr::addRecordCB(bool opResult,UINT64 autoID)
{}

//更新一条记录                                    
void CEmeraldDreamMgr::updateRecord(UINT32 key,const char * pData)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",key);

	Athena::CUDSqlCallBack<CEmeraldDreamMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CEmeraldDreamMgr>(this,&CEmeraldDreamMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_emeraldDreamRankTblDef,pData,"EMERALD_DREAM_RANK",where,pDelaySql);
}

//更新一条记录回调                                
void CEmeraldDreamMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

//更新翡翠梦境记录                                         
void CEmeraldDreamMgr::updateEmeraldDreamInfo(Player * pUser,UINT32 damage)
{
	if (!pUser){
		return ;
	}

	Global::strRemoveNum cmd;
	cmd.set_charid(pUser->getID());
	pUser->sendProto2Game(cmd);

	bool isShow = false;
	UINT32 max_damage = 0;
	SortProto::stSendRankInfo retCmd;

	if(m_emeraldDreamSortList.isInSortList(pUser->getID())){
		stEmeraldDreamRankInfo info = m_emeraldDreamSortList.getSortInfo(pUser->getID());
		if(info.m_damage < damage){
			info.m_damage = damage;
			info.m_level = pUser->m_level;
			m_emeraldDreamSortList.updateSortInfo(info);
			retCmd.set_promoted(true);
			isShow = true;
			stEmeraldDreamRankDB dbData;
			info.fillDBData(dbData);
			updateRecord(info.getKey(),(const char *)&dbData);
		}
		else{
			retCmd.set_promoted(false);
			isShow = false;
		}
		max_damage = info.m_damage;
	}
	else{
		stEmeraldDreamRankInfo info;
		info.m_charID = pUser->getID();
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_damage = damage;
		info.m_job = pUser->m_job;
		info.m_sex = pUser->m_sex;
		info.m_level = pUser->m_level;
		info.m_photoID = pUser->m_photoID;
		max_damage = info.m_damage;
		m_emeraldDreamSortList.insertSortInfo(info);

		stEmeraldDreamRankDB dbData;
		info.fillDBData(dbData);
		addRecord((const char *)&dbData);

		retCmd.set_promoted(true);
		isShow = true;
	}

	SortProto::SortListInfo * myRankInfo = retCmd.add_myinfo();
	if(myRankInfo){
		myRankInfo->set_name(pUser->getName());
		myRankInfo->set_damage(damage);
		myRankInfo->set_rank(m_emeraldDreamSortList.getMySortRank(pUser->getID()));
		myRankInfo->set_highest_damage(max_damage);
	}

	if(isShow){
		SortProto::SortListInfo * rivalRankInfo = retCmd.add_rivalinfo();

		UINT16 rank = switchRank(m_emeraldDreamSortList.getMySortRank(pUser->getID()));
		if(0 != rank){
			stEmeraldDreamRankInfo rankInfo;
			bzero(&rankInfo,sizeof(rankInfo));
			bool res = getPlayerInfo(rankInfo,rank);
			
			if(rivalRankInfo && res){
				rivalRankInfo->set_name(rankInfo.m_name);
				rivalRankInfo->set_damage(rankInfo.m_damage);
				rivalRankInfo->set_rank(m_emeraldDreamSortList.getMySortRank(rankInfo.m_charID));
			}
		}else{
			rivalRankInfo->set_name(pUser->getName());
			rivalRankInfo->set_damage(max_damage);
			rivalRankInfo->set_rank(m_emeraldDreamSortList.getMySortRank(pUser->getID()));
		}

	}
	else{
		stEmeraldDreamRankInfo info = m_emeraldDreamSortList.getSortInfo(pUser->getID());
		myRankInfo->set_highest_damage(info.m_damage);
	}

	pUser->sendProtoToMe(retCmd);
}

void CEmeraldDreamMgr::timer(UINT32 cur)
{
	if(cur <  g_globalServer->getOpenServerTime()){//还没有开服
		return ;
	}

	//零点清零
	struct tm curTm;
	RealTime::getLocalTime(curTm,g_globalServer->getSec());
	if ((curTm.tm_hour == 0) && (curTm.tm_min == 0) && (curTm.tm_sec < 10) && (!m_zeroRefresh)){
		m_zeroRefresh  = true;
		giveReward();
		m_emeraldDreamSortList.clear();
		delRankInfo();
	}
	else if (curTm.tm_sec > 10){
		m_zeroRefresh  = false;
	}


	if(0 == m_emeraldDreamOpenTimeInfo.m_beginTime){
		UINT32 delayDay = COpenFunction::getOpenDay(OpenFunID_Small_Monster);;
		struct tm curTm;
		RealTime::getLocalTime(curTm, g_globalServer->getOpenServerTime());
		UINT32 restTime = 86400 - (curTm.tm_hour * 3600 + curTm.tm_min * 60 + curTm.tm_sec);//获得开服时间到当天晚上0点还剩多少秒
		//开服当天也算一天 加上剩下天数秒数
		if(delayDay > 0){
#ifdef _HDZ_DEBUG
			Athena::logger->trace("翡翠梦境开服信息：开服时间：%d, 当天剩余时间：%d, 剩余天数时间：%d,  延时天数：%d", g_globalServer->getOpenServerTime(), restTime, (delayDay - 1) * 86400, delayDay);
#endif
			m_emeraldDreamOpenTimeInfo.m_activityID = 1;
			m_emeraldDreamOpenTimeInfo.m_beginTime	=	g_globalServer->getOpenServerTime() + restTime + (delayDay - 1) * 86400;
			m_emeraldDreamOpenTimeInfo.m_endTime	=	m_emeraldDreamOpenTimeInfo.m_beginTime + CEmeraldDreamCfg::getSingleton().getChangeId(1)->m_openTime * 86400;
		}else{
#ifdef _HDZ_DEBUG
			Athena::logger->trace("翡翠梦境开服信息：开服时间：%d", g_globalServer->getOpenServerTime());
#endif
			m_emeraldDreamOpenTimeInfo.m_activityID = 1;
			m_emeraldDreamOpenTimeInfo.m_beginTime  =   g_globalServer->getOpenServerTime() - (curTm.tm_hour * 3600 + curTm.tm_min * 60 + curTm.tm_sec) ;//开服当天0点算起
			m_emeraldDreamOpenTimeInfo.m_endTime    =   m_emeraldDreamOpenTimeInfo.m_beginTime + CEmeraldDreamCfg::getSingleton().getChangeId(1)->m_openTime * 86400;
		}
		stEmeraldDreamOpenTimeDB dbData;
		m_emeraldDreamOpenTimeInfo.fillDBData(dbData);
		updateTimeRecord(dbData);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("翡翠梦境开启动态1:id:%d,activityid:%d, beginTime:%d, endTime:%d",m_emeraldDreamOpenTimeInfo.m_id, m_emeraldDreamOpenTimeInfo.m_activityID, m_emeraldDreamOpenTimeInfo.m_beginTime, m_emeraldDreamOpenTimeInfo.m_endTime);
#endif
	}else{
		if(g_globalServer->getSec() >= m_emeraldDreamOpenTimeInfo.m_endTime){
			stChangeId * changeId = CEmeraldDreamCfg::getSingleton().getChangeId(m_emeraldDreamOpenTimeInfo.m_activityID  + 1);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("翡翠梦境开启动态m_emeraldDreamOpenTimeInfo.m_activityID：%d", m_emeraldDreamOpenTimeInfo.m_activityID);
#endif
			if(changeId){
				m_emeraldDreamOpenTimeInfo.m_activityID = changeId->m_id;
				//m_emeraldDreamOpenTimeInfo.m_beginTime = g_globalServer->getSec();
				m_emeraldDreamOpenTimeInfo.m_beginTime = m_emeraldDreamOpenTimeInfo.m_endTime;
				//m_emeraldDreamOpenTimeInfo.m_endTime = g_globalServer->getSec() + changeId->m_openTime * 86400;
				m_emeraldDreamOpenTimeInfo.m_endTime = m_emeraldDreamOpenTimeInfo.m_beginTime + changeId->m_openTime * 86400;
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[存在]翡翠梦境开启动态:id:%d,activityid:%d, beginTime:%d, endTime:%d", m_emeraldDreamOpenTimeInfo.m_id, m_emeraldDreamOpenTimeInfo.m_activityID, m_emeraldDreamOpenTimeInfo.m_beginTime,
						m_emeraldDreamOpenTimeInfo.m_endTime);
#endif
			}
			else{
				m_emeraldDreamOpenTimeInfo.m_activityID = 1;
				m_emeraldDreamOpenTimeInfo.m_beginTime = m_emeraldDreamOpenTimeInfo.m_endTime;
				m_emeraldDreamOpenTimeInfo.m_endTime = m_emeraldDreamOpenTimeInfo.m_beginTime + CEmeraldDreamCfg::getSingleton().getChangeId(1)->m_openTime * 86400;
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[不存在]翡翠梦境开启动态:id:%d,activityid:%d, beginTime:%d, endTime:%d", m_emeraldDreamOpenTimeInfo.m_id, m_emeraldDreamOpenTimeInfo.m_activityID, m_emeraldDreamOpenTimeInfo.m_beginTime,	m_emeraldDreamOpenTimeInfo.m_endTime);
#endif
			}
			stEmeraldDreamOpenTimeDB dbData;
			m_emeraldDreamOpenTimeInfo.fillDBData(dbData);
			updateTimeRecord(dbData);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("翡翠梦境开启动态2:id:%d,activityid:%d, beginTime:%d, endTime:%d", m_emeraldDreamOpenTimeInfo.m_id, m_emeraldDreamOpenTimeInfo.m_activityID, m_emeraldDreamOpenTimeInfo.m_beginTime, m_emeraldDreamOpenTimeInfo.m_endTime);
#endif
		}			
	}
}

void CEmeraldDreamMgr::updateTimeRecord(stEmeraldDreamOpenTimeDB &data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%u",data.m_id);

	Athena::CUDSqlCallBack<CEmeraldDreamMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CEmeraldDreamMgr>(this,&CEmeraldDreamMgr::updateTimeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_emeraldDreamOpenTimeTblDef,(const char *)&data,"EMERALD_DREAM_OPEN_TIME",where,pDelaySql);
}

void CEmeraldDreamMgr::updateTimeRecordCB(bool opResult,UINT64 autoID)
{}

void CEmeraldDreamMgr::giveReward()
{
	std::vector<stEmeraldDreamRankInfo> vec;
	m_emeraldDreamSortList.getFromN12N2(vec, 1, m_emeraldDreamSortList.getListNum());

	std::vector<stEmeraldDreamRankInfo>::iterator it = vec.begin();
	for(UINT16 i = 0; i < vec.size(); ++i){

		stBatchSendMailTask task;
		char szMailContent[50] = { 0 };
		task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_EMERALD_DREAM);
		task.m_receiveIDs.insert(vec[i].m_charID);

		UINT16 rank = m_emeraldDreamSortList.getMySortRank(vec[i].m_charID);
		stRankAward * award = CEmeraldDreamCfg::getSingleton().getRankAward(rank);
		snprintf(szMailContent,sizeof(szMailContent),"MailWnd_26;%d",rank);
		task.m_content = szMailContent;
		if(award){
			task.m_attaches.push_back(std::make_pair(award->m_itemID,1));
			CBatchSendMailMgr::getSingleton().addTask(task);
		}
	}
}

void CEmeraldDreamMgr::delRankInfo()
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere,sizeof(szWhere),"CHARID>0");
	Athena::CUDSqlCallBack<CEmeraldDreamMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CEmeraldDreamMgr>(this,&CEmeraldDreamMgr::delRankInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("EMERALD_DREAM_RANK",szWhere,pDelaySql);
}

void CEmeraldDreamMgr::delRankInfoCB(bool opResult,UINT64 autoID)
{}

UINT16 CEmeraldDreamMgr::switchRank(UINT16 myRank)
{
	UINT16 showRank = 0;
	if(myRank < 2){
		showRank = 0;
	}else if(myRank <= 4){
		showRank = 1;
	}else if(myRank <= 9){
		showRank = 4;
	}else if(myRank <= 19){
		showRank = 9;
	}else if(myRank <= 49){
		showRank = 19;
	}else if(myRank <= 99){
		showRank = 49;
	}else if(myRank<= 149){
		showRank = 99;
	}else if(myRank <= 199){
		showRank = 149;
	}else if(myRank <= 499){
		showRank = 199;
	}else if(myRank <= 999){
		showRank = 499;
	}else if(myRank <= 1499){
		showRank = 999;
	}else if(myRank <= 2000){
		showRank = 1499;
	}else{
		showRank = 2000;
	}
	return showRank;
}

bool CEmeraldDreamMgr::getPlayerInfo(stEmeraldDreamRankInfo &info, UINT16 rank)
{
	std::vector<stEmeraldDreamRankInfo> vec;
	m_emeraldDreamSortList.getFromN12N2(vec,rank,rank);
    if (vec.size()) {
        info = vec[0];
        return true;
    } else {
        Athena::logger->trace("getPlayerInfo : vec size is %d", vec.size());
    }
    return false;
}

//发送排行
void CEmeraldDreamMgr::sendEmeraldDreamSortList(Player *pUser,UINT32 beginRank, UINT32 num)
{
	if (!pUser){
		return ;
	}

	SortProto::stEmeraldDreamSortList retCmd;
	retCmd.set_beginrank(beginRank);

	std::vector<stEmeraldDreamRankInfo> vec;
	m_emeraldDreamSortList.getFromN12N2(vec,beginRank,beginRank + num);

	for (UINT8 i = 0;i < vec.size();++i){
		SortProto::SortListInfo * info = retCmd.add_info();
		if(info){
			info->set_charid(vec[i].m_charID);
			info->set_name(vec[i].m_name);
			info->set_damage(vec[i].m_damage);
			info->set_sex(vec[i].m_sex);
			info->set_job(vec[i].m_job);
			info->set_level(vec[i].m_level);
			info->set_photoid(vec[i].m_photoID);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CEmeraldDreamMgr::getRankInfo(UINT32 charID)
{
	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if(!pUser){
		return ;
	}

	Global::stUserRankInfoResult retCmd;

	retCmd.set_charid(charID);

	if(m_emeraldDreamSortList.isInSortList(charID)){
		stEmeraldDreamRankInfo info = m_emeraldDreamSortList.getSortInfo(pUser->getID());
		retCmd.set_damage(info.m_damage);
		retCmd.set_rank(m_emeraldDreamSortList.getMySortRank(pUser->getID()));
	}
	else
	{
		retCmd.set_damage(0);
		retCmd.set_rank(0);
	}

	retCmd.set_endtime(m_emeraldDreamOpenTimeInfo.m_endTime);

	/**************排名前三名*************/
	std::vector<stEmeraldDreamRankInfo> vec;
	m_emeraldDreamSortList.getFromN12N2(vec,1,3);
	for (UINT8 i = 0;i < vec.size();++i){
		Global::userSortInfo * info = retCmd.add_info();
		if(info){
			info->set_name(vec[i].m_name);
		}
	}

	stEmeraldDreamOpenTimeInfo info = CEmeraldDreamMgr::getSingleton().getCopyMapInfo();
	stChangeId * changeId = CEmeraldDreamCfg::getSingleton().getChangeId(info.m_activityID);
	if(changeId){
		 retCmd.set_levelid(changeId->m_levelId);
	}

	pUser->sendProto2Game(retCmd);
}

void CEmeraldDreamMgr::synRecord(Player * pUser)
{
	if(!pUser){
		return;
	}

	if(m_emeraldDreamSortList.isInSortList(pUser->getID())){
		stEmeraldDreamRankInfo info = m_emeraldDreamSortList.getSortInfo(pUser->getID());

		strncpy(info.m_name, pUser->getName(), sizeof(info.m_name));
		info.m_photoID = pUser->m_photoID;
		stEmeraldDreamRankDB dbData;
		info.fillDBData(dbData);
		updateRecord(info.getKey(),(const char *)&dbData);

	}
}
