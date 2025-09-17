#include "LevelMgr.h"
#include "Role.pb.h"
#include "GameUser.h"
#include "GameEventNotify.h"
#include "MiscCfgMgr.h"
#include "GlobalSvrClient.h"
#include "Map.h"
#include "SortList.pb.h"
#include "ArenaConfig.h"
#include "EmeraldDreamCfg.h"
#include "OperateActivityCfg.h"
#include "CopyMap.h"
#include "MapMgr.h"
#include "ResBackCfg.h"

std::map<UINT32,CLevelMgr::stCharpterInfo> CLevelMgr::s_charptersCnt;		//章节的配置信息
std::map<UINT32,UINT32>  CLevelMgr::s_levelID2CharpterID;					//关卡ID与章节ID对应关系


CLevelMgr::CLevelMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_isInLv = false;
	m_openBoxItemID = 0;
	m_openBoxItemNum = 0;
	m_canOpenBox = false;
	m_lastRefreshTime = 0;
	m_teamCopymapNum = 0;
	m_emeraldDreamNum = 0;
}
	
CLevelMgr::~CLevelMgr()
{
	
}

struct stLevelInfoParseCB : public TblDatabaseCB<stCopyMapBase>
{
	stLevelInfoParseCB() {}
	~stLevelInfoParseCB() {}

	virtual bool doit(stCopyMapBase * pBase)
	{
		if (!pBase){
			return true;
		}
		CLevelMgr::addLevel2Charpter(pBase->m_chapter,pBase->m_id);
		return true;	
	}
};

struct stForeceFinishPVELv : public TblDatabaseCB<stCopyMapBase>
{
	GameUser * m_pOwner;

	stForeceFinishPVELv(GameUser * pOwner) : m_pOwner(pOwner)
	{
		
	}

	~stForeceFinishPVELv()
	{
		
	}

	virtual bool doit(stCopyMapBase * pBase)
	{
		if (!pBase || !m_pOwner){
			return true;
		}
		if (m_pOwner->m_levelMgr.isPVELevel(pBase->m_type)){
			m_pOwner->m_levelMgr.forceFinishPVELv(pBase);
		}
		return true;	
	}
};

bool CLevelMgr::loadCfg()
{
	stLevelInfoParseCB cb;
	g_copyMapBaseMgr.execEveryOne(cb);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印章节信息:");
	std::map<UINT32,stCharpterInfo>::iterator it = s_charptersCnt.begin();
	for (;it != s_charptersCnt.end(); ++it){
		stCharpterInfo & one = it->second;
		Athena::logger->trace("charpterID :%d拥有的关卡为:",it->first);
		std::set<UINT32>::iterator subIt = one.m_lvIDs.begin();
		for (;subIt != one.m_lvIDs.end(); ++subIt){
			Athena::logger->trace("levelID = %d",*subIt);
		}
	}
#endif
	return true;
}

void CLevelMgr::finishAllPVELevel()
{
	stForeceFinishPVELv cb(m_pOwner);
	g_copyMapBaseMgr.execEveryOne(cb);
	userOnline();
}

void CLevelMgr::addLevel2Charpter(UINT32 charpterID,UINT32 levelID)
{
	std::map<UINT32,stCharpterInfo>::iterator it = s_charptersCnt.find(charpterID);

	if (it == s_charptersCnt.end()){
		stCharpterInfo one;
		one.m_charpterID = charpterID;
		one.m_lvIDs.insert(levelID);
		s_charptersCnt.insert(std::make_pair(charpterID,one));
	}
	else {
		it->second.m_lvIDs.insert(levelID);	
	}

	s_levelID2CharpterID[levelID] = charpterID;
}
	
UINT32 CLevelMgr::save(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	
	//保存组队副本次数
	*(UINT16 *)(buf + ret) = m_teamCopymapNum;
	ret += sizeof(UINT16);
	//保存刷新时间
	*(UINT32 *)(buf + ret) = m_lastRefreshTime;
	ret += sizeof(UINT32);
	//保存章节数量
	*(UINT16 *)(buf + ret) = m_charpterInfos.size();
	ret += sizeof(UINT16);

	std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.begin();
	for (;it != m_charpterInfos.end();++it){
		//章节数据
		stCharpterInfoDB & one = it->second;
		*(UINT32 *)(buf + ret) = one.m_charpterID;
		ret += sizeof(UINT32);

		*(UINT8 *)(buf + ret) = one.m_charpterRewardStep;
		ret += sizeof(UINT8);
		
		//关卡数量
		*(UINT16 *)(buf + ret) = one.m_levelInfos.size();
		ret += sizeof(UINT16);
		//具体的关卡
		std::map<UINT32,stLevelInfoDB>::iterator subIt = one.m_levelInfos.begin();	
		for (;subIt != one.m_levelInfos.end(); ++subIt){
			stLevelInfoDB & subOne = 	subIt->second;
			
			*(UINT32 *)(buf + ret) = subOne.m_levelID;
			ret += sizeof(UINT32);

			*(UINT8 *)(buf + ret) = subOne.m_starNum;
			ret += sizeof(UINT8);

			*(UINT16 *)(buf + ret) = subOne.m_rushNum;
			ret += sizeof(UINT16);

			*(UINT16 *)(buf + ret) = subOne.m_resetNum;
			ret += sizeof(UINT16);
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name=%s,保存了关卡数据%d",m_pOwner->getName(),ret);
#endif
	return ret;
}

UINT32 CLevelMgr::load(byte * buf)
{
	UINT32 ret = 0;
	//组队副本次数
	m_teamCopymapNum  = *(UINT16*)(buf + ret);
	ret += sizeof(UINT16);
	//最近一次刷新时间
	m_lastRefreshTime = *(UINT32*)(buf + ret);
	ret += sizeof(UINT32);
	//章节数量
	UINT16 charpterNum = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);
	
	m_pOwner->m_roleInfo.m_starNum = 0;

	for (UINT16 i = 0;i < charpterNum; ++i){
		stCharpterInfoDB one;
		one.m_charpterID = *(UINT32*)(buf + ret);
		ret += sizeof(one.m_charpterID);

		one.m_charpterRewardStep = *(UINT8*)(buf + ret);
		ret += sizeof(one.m_charpterRewardStep);

		UINT16 levelNum = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);

		for (UINT16 j = 0;j < levelNum; ++j){
			stLevelInfoDB lvInfo;
			lvInfo.m_levelID = *(UINT32*)(buf + ret);
			ret += sizeof(UINT32);

			lvInfo.m_starNum  = *(UINT8*)(buf + ret);
			ret += sizeof(UINT8);
			
			m_pOwner->m_roleInfo.m_starNum += lvInfo.m_starNum;

			lvInfo.m_rushNum = *(UINT16 *)(buf + ret);
			ret += sizeof(UINT16);

			lvInfo.m_resetNum = *(UINT16 *)(buf + ret);
			ret += sizeof(UINT16);
			one.m_levelInfos.insert(std::make_pair(lvInfo.m_levelID,lvInfo));
		}

		m_charpterInfos.insert(std::make_pair(one.m_charpterID,one));
	}

	if(0 == m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM)){
		    m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM, CEmeraldDreamCfg::getSingleton().getDailyNum());
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("name=%s,加载了关卡数据%d",m_pOwner->getName(),ret);
#endif
	return ret;
}

void CLevelMgr::userOnline()
{
	checkDataRefresh();

	Role::stSynAllLevelInfo retCmd;
	std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.begin();

	for (;it != m_charpterInfos.end(); ++it){
		stCharpterInfoDB & charpter = it->second;
		Role::stSynCharpterInfo * pSynInfo = retCmd.add_charpterinfo();	
		if (pSynInfo){
			pSynInfo->set_charpterid(charpter.m_charpterID);
			pSynInfo->set_rewardstep(charpter.m_charpterRewardStep);

			std::map<UINT32,stLevelInfoDB>::iterator subIt = charpter.m_levelInfos.begin();
			for (;subIt != charpter.m_levelInfos.end();++subIt){
				stLevelInfoDB & lvInfo = subIt->second;
				Role::stSynLevelInfo * pSynLvInfo = pSynInfo->add_levelinfo();
				if (pSynLvInfo){
					pSynLvInfo->set_levelid(lvInfo.m_levelID);
					pSynLvInfo->set_rushnum(lvInfo.m_rushNum);
					pSynLvInfo->set_star((Role::eLevelStar)lvInfo.m_starNum);
					pSynLvInfo->set_resetnum(lvInfo.m_resetNum);
				}
			}
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}

bool CLevelMgr::checkFinish()
{
	return true;
}

void CLevelMgr::addDropItem(UINT32 itemID,UINT32 num)
{
	m_randomDropItems[itemID] += num;
}

void CLevelMgr::finishArena(UINT32 copymapID,bool bWin)
{
	if (Athena::global["checkPvP"] == "true")
	{
		return;
	}
	
	SortProto::stRetCommintArenaResult retCmd;
	if (!checkFinish()){
		retCmd.set_result(SortProto::stRetCommintArenaResult::COND_NOT_MEET);
		m_pOwner->sendProto2Me(retCmd);
		Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,条件不符合,这小子可能用挂了",m_pOwner->getName(),m_pOwner->getID());
		return ;
	}
	
	if (!isInLv()){
		Athena::logger->error("name=%s,id=%d,请求完成关卡失败,还没有进入关卡",m_pOwner->getName(),m_pOwner->getID());
		return ;
	}

	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(copymapID);	
	if (!pCopyMapBase){
		retCmd.set_result(SortProto::stRetCommintArenaResult::INNER_ERROR);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,找不到关卡配置",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
	if (m_pOwner->getLevel() < pCopyMapBase->m_limitLv){
		retCmd.set_result(SortProto::stRetCommintArenaResult::ROLE_LV_TOO_LOW);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,小于限制等级",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	

	if (pCopyMapBase->m_costLiLian > 0) {
		if (!m_pOwner->removeResource(eResource_Lilian,pCopyMapBase->m_costLiLian,"完成关卡")){
			retCmd.set_result(SortProto::stRetCommintArenaResult::LILIAN_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,历练值不足",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}
	
	retCmd.set_result(SortProto::stRetCommintArenaResult::SUCCESS);
	m_pOwner->sendProto2Me(retCmd);

	{
		Global::stReqCheckTask cmd;                                 
		cmd.set_charid(m_pOwner->getID());        
		cmd.set_squaretype(5);
		cmd.set_battletype(4);                                      
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd); 
	}

	//给奖励
	m_pOwner->addExp(pCopyMapBase->m_expCoef * m_pOwner->getLevel());
	m_pOwner->addResource(eResource_GoldCoin,pCopyMapBase->m_goldCoin,"竞技场");
	std::map<UINT32,UINT32>::iterator it = m_randomDropItems.begin();
	for (;it != m_randomDropItems.end();++it){
		m_pOwner->giveAssert(it->first,it->second,0,"竞技场");	
	}
	m_randomDropItems.clear();

	Global::stArenaPKResult cmd;
	cmd.set_charid(m_pOwner->getID());
	cmd.set_historyrank(m_pOwner->m_arenaHistoryRank);
	if (bWin&& (m_arenaPKInfo.m_challengeTime + s_ARENA_PK_MAX_LASTIME > g_gameServer->getSec())){
		cmd.set_win(true);
		if (m_arenaPKInfo.m_rank < m_pOwner->m_arenaHistoryRank){
			m_pOwner->m_arenaHistoryRank = m_arenaPKInfo.m_rank;
		}
	}
	else {
		cmd.set_win(false);
	}
	cmd.set_rank(m_arenaPKInfo.m_rank);
	m_arenaPKInfo.reset();
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	stArenaNum arenaNotify;
	arenaNotify.notify(m_pOwner,1);
	m_canOpenBox = true;
	m_pOwner->m_titleMgr.checkObtainTitle(eTitleGotType_Finish_Lv,pCopyMapBase->m_id);
	m_pOwner->m_taskMgr.checkDailyTask();
}

void CLevelMgr::finishArenaNew(UINT32 copymapID,bool bWin, bool bTimeOut)
{
	{
		SortProto::stArenaResultNotify cmd;
		if (bWin)
		{
			cmd.set_result(SortProto::stArenaResultNotify::WIN);
		}
		else if (bTimeOut)
		{
			cmd.set_result(SortProto::stArenaResultNotify::TIMEOUT);
		}
		else
		{
			cmd.set_result(SortProto::stArenaResultNotify::LOSE);
		}
		m_pOwner->sendProto2Me(cmd);
	}

	{
		Global::stReqCheckTask cmd;                                 
		cmd.set_charid(m_pOwner->getID());        
		cmd.set_squaretype(5);
		cmd.set_battletype(4);                                      
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd); 
	}

	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(copymapID);	
	if (!pCopyMapBase){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,找不到关卡配置",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	//给奖励
	m_pOwner->addExp(pCopyMapBase->m_expCoef * m_pOwner->getLevel());
	m_pOwner->addResource(eResource_GoldCoin,pCopyMapBase->m_goldCoin,"竞技场");
	std::map<UINT32,UINT32>::iterator it = m_randomDropItems.begin();
	for (;it != m_randomDropItems.end();++it){
		m_pOwner->giveAssert(it->first,it->second,0,"竞技场");	
	}
	m_randomDropItems.clear();

	Global::stArenaPKResult cmd;
	cmd.set_charid(m_pOwner->getID());
	cmd.set_historyrank(m_pOwner->m_arenaHistoryRank);
	if (bWin&& (m_arenaPKInfo.m_challengeTime + s_ARENA_PK_MAX_LASTIME > g_gameServer->getSec())){
		cmd.set_win(true);
		if (m_arenaPKInfo.m_rank < m_pOwner->m_arenaHistoryRank){
			m_pOwner->m_arenaHistoryRank = m_arenaPKInfo.m_rank;
		}
	}
	else {
		cmd.set_win(false);
	}
	cmd.set_rank(m_arenaPKInfo.m_rank);
	m_arenaPKInfo.reset();
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	stArenaNum arenaNotify;
	arenaNotify.notify(m_pOwner,1);
	m_canOpenBox = true;
	m_pOwner->m_titleMgr.checkObtainTitle(eTitleGotType_Finish_Lv,pCopyMapBase->m_id);
	m_pOwner->m_taskMgr.checkDailyTask();
}

bool CLevelMgr::finishLeitai(UINT32 copymapID)
{
	copymapID = m_leitaiPKInfo.m_levelID;
	m_leitaiPKInfo.m_levelID = 0;
	m_leitaiPKInfo.m_charID = 0;
	SortProto::stRetSendLeitaiResult retCmd;
	if (!checkFinish()){
		retCmd.set_result(SortProto::stRetSendLeitaiResult::COND_NOT_MEET);
		m_pOwner->sendProto2Me(retCmd);
		Athena::logger->trace("name=%s,id=%d,请求完成擂台,条件不符合,这小子可能用挂了",m_pOwner->getName(),m_pOwner->getID());
		return false;
	}

	if (!isInLv()){
		Athena::logger->error("name=%s,id=%d,请求完成擂台失败,还没有进入关卡",m_pOwner->getName(),m_pOwner->getID());
		return false;
	}

	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(copymapID);	
	if (!pCopyMapBase){
		retCmd.set_result(SortProto::stRetSendLeitaiResult::INNER_ERROR);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求完成擂台失败,找不到关卡配置",m_pOwner->getName(),m_pOwner->getID());
#endif
		return false;
	}

	if (m_pOwner->getLevel() < pCopyMapBase->m_limitLv){
		retCmd.set_result(SortProto::stRetSendLeitaiResult::ROLE_LV_TOO_LOW);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求完成擂台失败,小于限制等级",m_pOwner->getName(),m_pOwner->getID());
#endif
		return false;
	}


	if (pCopyMapBase->m_costLiLian > 0) {
		if (!m_pOwner->removeResource(eResource_Lilian,pCopyMapBase->m_costLiLian,"完成关卡")){
			retCmd.set_result(SortProto::stRetSendLeitaiResult::LILIAN_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求完成擂台失败,历练值不足",m_pOwner->getName(),m_pOwner->getID());
#endif
			return false;
		}
	}

	retCmd.set_result(SortProto::stRetSendLeitaiResult::SUCCESS);
	m_pOwner->sendProto2Me(retCmd);

	//给奖励
	m_pOwner->addExp(pCopyMapBase->m_expCoef * m_pOwner->getLevel());
	m_pOwner->addResource(eResource_GoldCoin,pCopyMapBase->m_goldCoin,"荣耀宝座");
	std::map<UINT32,UINT32>::iterator it = m_randomDropItems.begin();
	for (;it != m_randomDropItems.end();++it){
		m_pOwner->giveAssert(it->first,it->second,0,"荣耀宝座");	
	}
	m_randomDropItems.clear();

	stLeitaiCopymapNum notify;
	notify.notify(m_pOwner,1);
	m_canOpenBox = true;
	m_pOwner->m_titleMgr.checkObtainTitle(eTitleGotType_Finish_Lv,pCopyMapBase->m_id);
	m_pOwner->m_taskMgr.checkDailyTask();
    m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Letai_King, 1, m_pOwner->getVipLv());
	return true;
}

void CLevelMgr::forceFinishPVELv(stCopyMapBase * pCopyMapBase)
{
	if (!pCopyMapBase){
		return ;
	}
	
	stCharpterInfoDB & charpter = m_charpterInfos[pCopyMapBase->m_chapter];
	charpter.m_charpterID = pCopyMapBase->m_chapter;
	charpter.m_charpterRewardStep = 0;
	stLevelInfoDB & levelInfo = charpter.m_levelInfos[pCopyMapBase->m_id];
	levelInfo.m_levelID = pCopyMapBase->m_id;
	levelInfo.m_resetNum = 0;
	levelInfo.m_rushNum = 1;
	levelInfo.m_starNum = 3; 
	if (isCalcStar(pCopyMapBase->m_type)){
		m_pOwner->m_roleInfo.m_starNum += levelInfo.m_starNum;
	}

	if (eLevelType_Elite == pCopyMapBase->m_type){//精英副本
		stEliteCopymapNum notify;
		notify.notify(m_pOwner,1);
	}
	else if (eLevelType_Survival == pCopyMapBase->m_type){//生存副本
		stShengCunCopymapNum notify;
		notify.notify(m_pOwner,1);
	}
	else if (eLevelType_GoldCoin == pCopyMapBase->m_type){//金币副本
		stGoldcoinCopymapNum notify;
		notify.notify(m_pOwner,1);
	}
	
	stFinishCopyMapNotify notify;
	notify.notify(m_pOwner,1,pCopyMapBase->m_id);
	notify.notify(m_pOwner,1,0);
	
	m_pOwner->m_titleMgr.checkObtainTitle(eTitleGotType_Finish_Lv,pCopyMapBase->m_id);
	m_pOwner->m_taskMgr.checkDailyTask();
}

void CLevelMgr::finishLv(Role::stReqFinishLv * recvCmd)
{
	
	checkDataRefresh();
	Role::stFinishLvResult retCmd;
	
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(recvCmd->levelid());	
	if (!pCopyMapBase){
		retCmd.set_result(Role::stFinishLvResult::INNER_ERROR);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,找不到关卡配置",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
	if (!isPVELevel(pCopyMapBase->m_type)){
		Athena::logger->error("非pve关卡试图进入从CLevelMgr::finishLv入口进入,失败!");
		return;
	}

	if (!checkFinish()){
		retCmd.set_result(Role::stFinishLvResult::COND_NOT_MEET);
		m_pOwner->sendProto2Me(retCmd);
		Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,条件不符合,这小子可能用挂了",m_pOwner->getName(),m_pOwner->getID());
		return ;
	}
	
	if (!isInLv()){
		Athena::logger->error("name=%s,id=%d,请求完成关卡失败,还没有进入关卡",m_pOwner->getName(),m_pOwner->getID());
		return ;
	}

	if(!isTaskLevel(pCopyMapBase->m_type)){	
		if (m_pOwner->getLevel() < pCopyMapBase->m_limitLv){
			retCmd.set_result(Role::stFinishLvResult::ROLE_LV_TOO_LOW);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,小于限制等级",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	
		if (!m_pOwner->m_levelMgr.isLevelFinish(pCopyMapBase->m_preLvID)){
			retCmd.set_result(Role::stFinishLvResult::PRE_LEVEL_NOT_FINISH);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("account=%s,name=%s,id=%d,等级限制,完成关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}

	if (pCopyMapBase->m_costSP > 0){
		if (!m_pOwner->removeResource(eResource_Sp,pCopyMapBase->m_costSP,"完成关卡")){
			retCmd.set_result(Role::stFinishLvResult::SP_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,体力不足",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}

	if (pCopyMapBase->m_costLiLian > 0) {
		if (!m_pOwner->removeResource(eResource_Lilian,pCopyMapBase->m_costLiLian,"完成关卡")){
			retCmd.set_result(Role::stFinishLvResult::LILIAN_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,历练值不足",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}

	if(!isTaskLevel(pCopyMapBase->m_type)){	
		std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.find(pCopyMapBase->m_chapter);
		if (it == m_charpterInfos.end()){
			stCharpterInfoDB one;
			one.m_charpterID = pCopyMapBase->m_chapter;
			one.m_charpterRewardStep = 0;
			stLevelInfoDB subOne;
			subOne.m_levelID = recvCmd->levelid();
			subOne.m_rushNum = 1;
			subOne.m_starNum = recvCmd->star();
			if (isCalcStar(pCopyMapBase->m_type)){
				m_pOwner->m_roleInfo.m_starNum += recvCmd->star();
			}
			one.m_levelInfos.insert(std::make_pair(subOne.m_levelID,subOne));
			m_charpterInfos.insert(std::make_pair(one.m_charpterID,one));
			m_pOwner->synMyCopymapStarNum();
		}
		else {
			std::map<UINT32,stLevelInfoDB>::iterator subIt = it->second.m_levelInfos.find(recvCmd->levelid());			
			if (subIt == it->second.m_levelInfos.end()){
				stLevelInfoDB subOne;
				subOne.m_levelID = recvCmd->levelid();
				subOne.m_rushNum = 1;
				subOne.m_starNum = recvCmd->star();
				if (isCalcStar(pCopyMapBase->m_type)){
					m_pOwner->m_roleInfo.m_starNum += recvCmd->star();
				}
				it->second.m_levelInfos.insert(std::make_pair(subOne.m_levelID,subOne));
				m_pOwner->synMyCopymapStarNum();
			}
			else {
				if ((1 == pCopyMapBase->m_chapter) && (subIt->second.m_rushNum > 0)){
					/*Athena::logger->trace("已经超过了关卡最大次数");
					retCmd.set_result(Role::stFinishLvResult::TIME_USE_OUT);
					m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
					Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,次数已经用完了",m_pOwner->getName(),m_pOwner->getID());
#endif
					return ;*/
				}
				else if (subIt->second.m_rushNum >= m_pOwner->getCountByVip(eVipFun_Battle_Num)){
					Athena::logger->trace("已经超过了关卡最大次数");
					retCmd.set_result(Role::stFinishLvResult::TIME_USE_OUT);
					m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
					Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,次数已经用完了",m_pOwner->getName(),m_pOwner->getID());
#endif
					return ;
				}
				subIt->second.m_rushNum += 1;
				if (recvCmd->star() > subIt->second.m_starNum){
				 //subIt->second.m_starNum = recvCmd->star();
					if (isCalcStar(pCopyMapBase->m_type)){
						m_pOwner->m_roleInfo.m_starNum += (recvCmd->star() - subIt->second.m_starNum);
					 }
					 subIt->second.m_starNum = recvCmd->star();
					 m_pOwner->synMyCopymapStarNum();
				}
			}
		}
	}


	retCmd.set_result(Role::stFinishLvResult::SUCCESS);
	m_pOwner->sendProto2Me(retCmd);
	
	char szRewardDesc[30] = { 0 };
	if (eLevelType_Elite == pCopyMapBase->m_type){//精英副本
		stEliteCopymapNum notify;
		notify.notify(m_pOwner,1);
		snprintf(szRewardDesc,sizeof(szRewardDesc),"精英关卡:%d",pCopyMapBase->m_id);

		Global::stReqCheckTask cmd;
		cmd.set_charid(m_pOwner->getID());
		cmd.set_squaretype(5);
		cmd.set_battletype(2);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	}
	else if (eLevelType_Survival == pCopyMapBase->m_type){//生存副本
		stShengCunCopymapNum notify;
		notify.notify(m_pOwner,1);
		snprintf(szRewardDesc,sizeof(szRewardDesc),"生存关卡:%d",pCopyMapBase->m_id);
        m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Stone_Map, 1, m_pOwner->getVipLv());
	}
	else if (eLevelType_GoldCoin == pCopyMapBase->m_type){//金币副本
		stGoldcoinCopymapNum notify;
		notify.notify(m_pOwner,1);
		snprintf(szRewardDesc,sizeof(szRewardDesc),"金币关卡:%d",pCopyMapBase->m_id);
        m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Gold_Map, 1, m_pOwner->getVipLv());
	}
	else if(isTaskLevel(pCopyMapBase->m_type)){
		Global::stReqCheckMission cmd;
		cmd.set_charid(m_pOwner->getID());
		cmd.set_levelid(recvCmd->levelid());
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}else{
		snprintf(szRewardDesc,sizeof(szRewardDesc),"普通关卡:%d",pCopyMapBase->m_id);
		Global::stReqCheckTask cmd;
		cmd.set_charid(m_pOwner->getID());
		cmd.set_squaretype(5);
		cmd.set_battletype(1);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
/*
		Global::stReqCheckMission gcmd;
		gcmd.set_charid(m_pOwner->getID());
		gcmd.set_levelid(recvCmd->levelid());
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(gcmd);
        */
	}

	//给奖励
	m_pOwner->addExp(pCopyMapBase->m_expCoef * m_pOwner->getLevel());
	if (recvCmd->goldcoin() <= pCopyMapBase->m_goldCoin)	{
		m_pOwner->addResource(eResource_GoldCoin,recvCmd->goldcoin(),szRewardDesc);
	}

	for (UINT16 i = 0;i < recvCmd->getitem_size();++i){
		const Role::stObtainItem & obtainItem = recvCmd->getitem(i);
		if (obtainItem.num() <= m_randomDropItems[obtainItem.itemid()]){
			m_randomDropItems[obtainItem.itemid()] = 0;
			if (obtainItem.num() > 0){
				m_pOwner->giveAssert(obtainItem.itemid(),obtainItem.num(),0,szRewardDesc);
			}
		}
		else {
			Athena::logger->error("(name=%s,id=%d)完成关卡奖励道具出错,发送的道具数量过多跟服务器不一致(itemID:%d,num:%d)",m_pOwner->getName(),m_pOwner->getID(),obtainItem.itemid(),obtainItem.num());
		}
	}	
	
	m_randomDropItems.clear();	
	m_canOpenBox = true;

	stFinishCopyMapNotify notify;
	notify.notify(m_pOwner,1,pCopyMapBase->m_id);
	notify.notify(m_pOwner,1,0);
	m_randomDropItems.clear();
	

	m_pOwner->m_titleMgr.checkObtainTitle(eTitleGotType_Finish_Lv,pCopyMapBase->m_id);
	m_pOwner->m_taskMgr.checkDailyTask();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("完成了关卡:%d",pCopyMapBase->m_id);
#endif

	Game::stCopymapLog webCmd;
	webCmd.set_copymapid(pCopyMapBase->m_id);
	webCmd.set_win(true);
	webCmd.set_action("完成关卡");
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

    UINT8 activityid = 0;
    if (eLevelType_Elite == pCopyMapBase->m_type){ //精英副本
        activityid = COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Elite_Map);
    } else if (eLevelType_Normal == pCopyMapBase->m_type) { //普通副本
        activityid = COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Common_Map);
    }
    if (activityid) {
        Global::stOperateActivityCb globalCmd;
        globalCmd.set_charid(m_pOwner->getID());
        globalCmd.set_activityid(activityid);
        globalCmd.set_num(1);
        GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
    }
}

void CLevelMgr::giveTeamCopymapReward(UINT32 copyMapID)
{
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(copyMapID);	
	if (!pCopyMapBase){
		Athena::logger->trace("name=%s,id=%d,请求完成关卡:%d,失败,找不到关卡配置",m_pOwner->getName(),m_pOwner->getID(),copyMapID);
		return ;
	}
	
	if (m_pOwner->getLevel() < pCopyMapBase->m_limitLv){
		Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,小于限制(%d)等级",m_pOwner->getName(),m_pOwner->getID(),copyMapID);
		return ;
	}
	
	if (!m_pOwner->m_levelMgr.isLevelFinish(pCopyMapBase->m_preLvID)){
		Athena::logger->error("account=%s,name=%s,id=%d,前置关卡限制,完成关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		return ;
	}

	if (pCopyMapBase->m_costLiLian > 0) {
		if (!m_pOwner->removeResource(eResource_Lilian,pCopyMapBase->m_costLiLian,"完成关卡")){
			Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,历练值不足",m_pOwner->getName(),m_pOwner->getID());
			return ;
		}
	}

	if (pCopyMapBase->m_costSP > 0) {
		if (!m_pOwner->removeResource(eResource_Sp,pCopyMapBase->m_costSP,"完成关卡")){
			Athena::logger->trace("name=%s,id=%d,请求完成关卡失败,体力值不足",m_pOwner->getName(),m_pOwner->getID());
			return ;
		}
	}
	
	std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.find(pCopyMapBase->m_chapter);
	if (it == m_charpterInfos.end()){
		stCharpterInfoDB one;
		one.m_charpterID = pCopyMapBase->m_chapter;
		one.m_charpterRewardStep = 0;
		stLevelInfoDB subOne;
		subOne.m_levelID = copyMapID;
		subOne.m_rushNum = 1;
		one.m_levelInfos.insert(std::make_pair(subOne.m_levelID,subOne));
		m_charpterInfos.insert(std::make_pair(one.m_charpterID,one));
	}
	else {
		std::map<UINT32,stLevelInfoDB>::iterator subIt = it->second.m_levelInfos.find(copyMapID);			
		if (subIt == it->second.m_levelInfos.end()){
			stLevelInfoDB subOne;
			subOne.m_levelID = copyMapID;
			subOne.m_rushNum = 1;
			it->second.m_levelInfos.insert(std::make_pair(subOne.m_levelID,subOne));
		}
		else {
			subIt->second.m_rushNum += 1;
		}
	}

	//给奖励
	m_pOwner->addExp(pCopyMapBase->m_expCoef * m_pOwner->getLevel());
	m_pOwner->addResource(eResource_GoldCoin,pCopyMapBase->m_goldCoin,"完成组队关卡");
	//m_pOwner->addTeamCopyMapNum(1);	
	std::map<UINT32,UINT32>::iterator subIt =  m_randomDropItems.begin();
	for (;subIt != m_randomDropItems.end();++subIt){
		m_pOwner->giveAssert(subIt->first,subIt->second,0,"完成组队关卡");
	}
	m_randomDropItems.clear();
	m_pOwner->giveAssert(m_openBoxItemID,m_openBoxItemNum,0,"完成组队关卡");
	m_openBoxItemID = 0;
	m_openBoxItemNum = 0;
	m_pOwner->m_taskMgr.checkDailyTask();

	if (eLevelType_Team == pCopyMapBase->m_type){
        m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Team_Map, 1, m_pOwner->getVipLv());
    } else if (eLevelType_Dragon_Nest == pCopyMapBase->m_type) {
        m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Dragon_Map, 1, m_pOwner->getVipLv());
    } else if (eLevelType_Stone_CopyMap == pCopyMapBase->m_type) {
        m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Balck_Map, 1, m_pOwner->getVipLv());
    }
}

void CLevelMgr::rushLv(UINT32 levelID,UINT8 num,Role::eRushType eType)
{
	checkDataRefresh();
	Role::stLevelRushResult retCmd;
	retCmd.set_type(eType);
	retCmd.set_levelid(levelID);
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelID);	
	if (!pCopyMapBase){
		retCmd.set_result(Role::stLevelRushResult::INNER_ERROR);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求扫荡失败,找不到相关配置!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
    
	if (num >= 5){//5连抢,要判断vip
		if ((m_pOwner->getCountByVip(eVipFun_5Times_Rush) <= 0) && (m_pOwner->getLevel() < CMiscCfgMgr::getSingleton().getOneKeyRushLv())){
			retCmd.set_result(Role::stLevelRushResult::VIP_LV_TOO_LOW);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求扫荡失败,找不到相关配置!",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}

	if (!pCopyMapBase->m_canRush){//不可以扫荡
		retCmd.set_result(Role::stLevelRushResult::LV_CANNT_RUSH);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求扫荡失败,改关卡不可以扫荡!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.find(pCopyMapBase->m_chapter);
	if (it == m_charpterInfos.end()){
		retCmd.set_result(Role::stLevelRushResult::NOT_3_STAR);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求扫荡失败,没有到达3星!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	std::map<UINT32,stLevelInfoDB>::iterator subIt = it->second.m_levelInfos.find(levelID);			
	if (subIt == it->second.m_levelInfos.end()){
		retCmd.set_result(Role::stLevelRushResult::NOT_3_STAR);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求扫荡失败,没有到达3星!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;

	}

	if (Athena::global["nftServer"] == "true")
	{
		stChapEquipLimit* plimit = CMiscCfgMgr::getSingleton().getChapEqupLimit(pCopyMapBase->m_chapter);
		if(plimit && (plimit->m_sr>0||plimit->m_ssr>0))
		{
			// 裝備等值規則 ：1件ssr=2件sr（ur也屬於ssr條件）
			UINT32 srValue = 0;
			for (int i = HEAD_SLOT_INDEX; i <= WING_SLOT_INDEX; i++)
			{
				CItem *pItem = m_pOwner->m_bagMgr.m_equipBag.getEquipByIndex(i);
				if (pItem != NULL)
				{
					UINT8 quality = pItem->m_pItemBase->m_quality;
					if (pItem->isSR(quality))
					{
						srValue++;
					}
					else if (pItem->isSSR(quality) || pItem->isUR(quality))
					{
						srValue +=2;
					} 
				}
			}
			UINT32 srNeed = plimit->m_sr;
			if (plimit->m_sr == 0)
			{
				srNeed = plimit->m_ssr*2;
			}
			
			if (srValue < srNeed)
			{
				retCmd.set_result(Role::stLevelRushResult::BATTLEPOWER);
				m_pOwner->sendProto2Me(retCmd);	
				return;
			}  
		}
	}

	if (subIt->second.m_starNum < Role::eLEVEL_STAR_3){
		retCmd.set_result(Role::stLevelRushResult::NOT_3_STAR);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求扫荡失败,没有到达3星!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
    bool rushLvlIsOpen = CMiscCfgMgr::getSingleton().rushLvlIsOpen();
    if (rushLvlIsOpen) {
        UINT32 ownRushTicket = m_pOwner->m_bagMgr.getItemNumByID(122216);
        UINT32 spNum = m_pOwner->getSP();
        if (!ownRushTicket) {
			retCmd.set_result(Role::stLevelRushResult::RUSH_TICKET_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求扫荡失败,扫荡券不足!",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
        }
        if (!spNum) {
			retCmd.set_result(Role::stLevelRushResult::SP_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求扫荡失败,体力不足!",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
        }
        UINT32 realNum = 0;
        spNum < ownRushTicket ? (realNum = spNum) : (realNum = ownRushTicket);
        if (num > realNum) {
            num = realNum;
        }
        Athena::logger->trace("real num is %d, ownRushTicket is %d, spNum is %d", num, ownRushTicket, spNum);
    }
	if (num + subIt->second.m_rushNum > m_pOwner->getCountByVip(eVipFun_Battle_Num)){
		retCmd.set_result(Role::stLevelRushResult::RUSH_NUM_OUT);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求扫荡失败,上当次数不够!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	UINT16 needSlot = 8;
	if (num > 1){
		needSlot = 21;
	}

	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < needSlot){
		retCmd.set_result(Role::stLevelRushResult::BAG_FULL);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求扫荡失败,背包满了!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	//todo 检查体力
	if (pCopyMapBase->m_costSP > 0){	
		if (!m_pOwner->removeResource(eResource_Sp,num * pCopyMapBase->m_costSP,"扫荡")){
			retCmd.set_result(Role::stLevelRushResult::SP_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求扫荡失败,体力不足!",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}
	
	//检查历练
	if (pCopyMapBase->m_costLiLian > 0){
		if (!m_pOwner->removeResource(eResource_Lilian,num * pCopyMapBase->m_costLiLian,"扫荡")){
			retCmd.set_result(Role::stLevelRushResult::LILIAN_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d,请求扫荡失败,历练不足!",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}
    
    if (rushLvlIsOpen) {
        m_pOwner->m_bagMgr.removeItemByItemID(122216, num, "扫荡");
    }
	subIt->second.m_rushNum += num;
	
	UINT32 exp = pCopyMapBase->m_expCoef * m_pOwner->getLevel() * num;
	//m_pOwner->addExp(exp);
	retCmd.set_exp(m_pOwner->addExp(exp));
	m_pOwner->addResource(eResource_GoldCoin,pCopyMapBase->m_goldCoin * num,"完成关卡");
	retCmd.set_goldcoin(pCopyMapBase->m_goldCoin * num);
	
	std::vector<CDropMgr::stDropEntry> dropVec;
	for (UINT16 i = 0;i < num; ++i){
		genDropInfo(dropVec,pCopyMapBase);
	}

	for (UINT16 i = 0;i < dropVec.size();++i){
		CDropMgr::stDropEntry & dropInfo = dropVec[i];
		m_pOwner->giveAssert(dropInfo.m_itemID,dropInfo.m_num,0,"完成关卡");
		Role::stSynRushReward * pSyn = retCmd.add_items();
		if (pSyn){
			pSyn->set_itemid(dropInfo.m_itemID);
			pSyn->set_num(dropInfo.m_num);
		}
	}	
	retCmd.set_result(Role::stLevelRushResult::SUCCESS);
	m_pOwner->sendProto2Me(retCmd);
	
    UINT8 activityid = 0;
	if (eLevelType_Elite == pCopyMapBase->m_type){//精英副本
		stEliteCopymapNum notify;
		notify.notify(m_pOwner,num);
	}
	else if (eLevelType_Survival == pCopyMapBase->m_type){//生存副本
		stShengCunCopymapNum notify;
		notify.notify(m_pOwner,num);
	}
	else if (eLevelType_GoldCoin == pCopyMapBase->m_type){//金币副本
		stGoldcoinCopymapNum notify;
		notify.notify(m_pOwner,num);
	}
    if (eLevelType_Elite == pCopyMapBase->m_type){ //精英副本
        activityid = COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Elite_Map);
    } else if (eLevelType_Normal == pCopyMapBase->m_type) { //普通副本
        activityid = COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Common_Map);
    }
    if (activityid) {
        Global::stOperateActivityCb globalCmd;
        globalCmd.set_charid(m_pOwner->getID());
        globalCmd.set_activityid(activityid);
        globalCmd.set_num(num);
        GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
    }

	stFinishCopyMapNotify notify;
	notify.notify(m_pOwner,num,pCopyMapBase->m_id);
	notify.notify(m_pOwner,num,0);
	
	Game::stCopymapLog webCmd;
	webCmd.set_copymapid(pCopyMapBase->m_id);
	webCmd.set_win(true);
	if (num > 1){
		webCmd.set_action("5连扫荡");
	}
	else {
		webCmd.set_action("扫荡");
	}
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);	
}

void CLevelMgr::reqCharpterReward(UINT32 charpterID,UINT8 step)
{
	checkDataRefresh();
	Role::stCharpterRewardResult retCmd;
	retCmd.set_step(step);
	stCharpterBase * pCharpterBase = g_charpterBaseMgr.get(charpterID);
	if (!pCharpterBase){
		retCmd.set_result(Role::stCharpterRewardResult::INNER_ERROR);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,领取章节奖励失败,找不到章节配置!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;	
	}
	
	std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.find(charpterID);
	if (it == m_charpterInfos.end()){
		retCmd.set_result(Role::stCharpterRewardResult::CHARPTER_NOT_EXSIT);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,领取章节奖励失败,章节不存在!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
	//总的星星数
	UINT16 starNum = 0;
	std::map<UINT32,stLevelInfoDB>::iterator subIt = it->second.m_levelInfos.begin();			
	for (;subIt != it->second.m_levelInfos.end();++subIt){
		stLevelInfoDB & lvInfo = subIt->second;
		starNum += lvInfo.m_starNum;
	}

	UINT16 needStarNum = (UINT16)-1;
	UINT32 rewardBoxID = 0;
	UINT8  rewardFlag = 0;

	if (1 == step){
		needStarNum = 9;
		rewardBoxID = pCharpterBase->m_9StarReward;
		rewardFlag = Role::eCharpterReward_Step_1;
	}
	else if (2 == step){
		needStarNum = 18;
		rewardBoxID = pCharpterBase->m_18StarReward;
		rewardFlag = Role::eCharpterReward_Step_2;
	}
	else if (3 == step){
		needStarNum = 27;
		rewardBoxID = pCharpterBase->m_27StarReward;
		rewardFlag = Role::eCharpterReward_Step_3;
	}
	else {//不存在的阶段
		return ;
	}

	if (starNum < needStarNum){
		retCmd.set_result(Role::stCharpterRewardResult::STARNUM_NOT_ENOUGH);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,领取章节奖励失败,星星数不足!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (it->second.m_charpterRewardStep & rewardFlag){
		retCmd.set_result(Role::stCharpterRewardResult::HAS_GET_REWARD);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,领取章节奖励失败,已经领过奖励了!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return;
	}
	

	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < m_pOwner->m_boxMgr.needSlotNum(rewardBoxID)){
		retCmd.set_result(Role::stCharpterRewardResult::BAG_FULL);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,领取章节奖励失败,背包满了!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return;
	}
	
	it->second.m_charpterRewardStep |= rewardFlag;
	m_pOwner->m_boxMgr.openBox(rewardBoxID);
	retCmd.set_step(step);
	retCmd.set_result(Role::stCharpterRewardResult::SUCCESS);
	m_pOwner->sendProto2Me(retCmd);
}

void CLevelMgr::resetRushNum(UINT32 levelID)
{
	checkDataRefresh();
	Role::stResetRushNumResult retCmd;
	retCmd.set_levelid(levelID);
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelID);	
	if (!pCopyMapBase){
		retCmd.set_result(Role::stResetRushNumResult::INNER_ERROR);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求重置扫荡次数失败,找不到相关配置!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.find(pCopyMapBase->m_chapter);
	if (it == m_charpterInfos.end()){
		retCmd.set_result(Role::stResetRushNumResult::CHARPTER_NOT_FINISH);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求重置扫荡次数失败,章节没有完成!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	std::map<UINT32,stLevelInfoDB>::iterator subIt = it->second.m_levelInfos.find(levelID);			
	if (subIt == it->second.m_levelInfos.end()){
		retCmd.set_result(Role::stResetRushNumResult::LEVEL_NOT_FINISH);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求重置扫荡次数失败,关卡没有完成!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
	UINT32 needDiamond = CMiscCfgMgr::getSingleton().getDiamondByRushReset(subIt->second.m_resetNum + 1);
	
	if (((UINT32)-1 == needDiamond) || !m_pOwner->removeResource(eResource_Diamond,needDiamond,"扫荡重置")){
		retCmd.set_result(Role::stResetRushNumResult::DIAMOND_NOT_ENOUGH);
		m_pOwner->sendProto2Me(retCmd);	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("name=%s,id=%d,请求重置扫荡次数失败,没有金钱!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}	
	
	subIt->second.m_resetNum += 1;
	subIt->second.m_rushNum = 0;
	retCmd.set_result(Role::stResetRushNumResult::SUCCESS);	
	m_pOwner->sendProto2Me(retCmd);
    stResetRushNum resetRushNumNotify;
    resetRushNumNotify.notify(m_pOwner, 1);
}

UINT16 CLevelMgr::getStarNum() { return m_pOwner->m_roleInfo.m_starNum ; } 

bool CLevelMgr::isLevelFinish(UINT32 levelID)
{
	if (!levelID){
		return true;
	}

	std::map<UINT32,UINT32>::iterator it =  s_levelID2CharpterID.find(levelID);
	if (it == s_levelID2CharpterID.end()){
		return false;	
	}	

	std::map<UINT32,stCharpterInfoDB>::iterator subIt =  m_charpterInfos.find(it->second);	
	if (subIt  == m_charpterInfos.end()){
		return false;
	}

	if (subIt->second.m_levelInfos.find(levelID) != subIt->second.m_levelInfos.end()){
		return true;
	}

	return false;
}

UINT16 CLevelMgr::getRushNum(UINT32 levelID)
{
	std::map<UINT32,UINT32>::iterator it =  s_levelID2CharpterID.find(levelID);
	if (it == s_levelID2CharpterID.end()){
		return 0;	
	}	

	std::map<UINT32,stCharpterInfoDB>::iterator subIt =  m_charpterInfos.find(it->second);	
	if (subIt  == m_charpterInfos.end()){
		return 0;
	}
	
	std::map<UINT32,stLevelInfoDB>::iterator thdIt = subIt->second.m_levelInfos.find(levelID);
	if (thdIt != subIt->second.m_levelInfos.end()){
		return thdIt->second.m_rushNum;
	}
	return 0;
}

void CLevelMgr::genDropInfo(std::vector<CDropMgr::stDropEntry> & dropVec,stCopyMapBase * pCopyMapBase)
{
	if (!pCopyMapBase){
		return ;
	}

	INT16 levelDropNum = 0;
	INT16 charpterDropNum = 0;
	INT16 equipDropNum = 0;

	UINT32 levelDropNumTotalRate = 0;
	UINT32 charpterDropNumTotalRate = 0;
	UINT32 equipDropNumTotalRate = 0;

	std::map<UINT32,UINT32> lvDropNum2Rate;
	std::map<UINT32,UINT32> charpterDropNum2Rate;
	std::map<UINT32,UINT32> equipDropNum2Rate;

	for (UINT16 i = 0;i < pCopyMapBase->m_levelDropNumRate.size();++i){
		stDropNumInfo & dropNumInfo = pCopyMapBase->m_levelDropNumRate[i];
		lvDropNum2Rate[dropNumInfo.m_num] = dropNumInfo.m_rate;
		levelDropNumTotalRate += dropNumInfo.m_rate;
	}
	
	for (UINT16 i = 0;i < pCopyMapBase->m_charpterDropNumRate.size(); ++i){
		stDropNumInfo & dropNumInfo = pCopyMapBase->m_charpterDropNumRate[i];
		charpterDropNum2Rate[dropNumInfo.m_num] = dropNumInfo.m_rate;
		charpterDropNumTotalRate += dropNumInfo.m_rate;
	}

	for (UINT16 i = 0;i < pCopyMapBase->m_equipDropNumRate.size(); ++i){
		stDropNumInfo & dropNumInfo = pCopyMapBase->m_equipDropNumRate[i];
		equipDropNum2Rate[dropNumInfo.m_num] = dropNumInfo.m_rate;
		equipDropNumTotalRate += dropNumInfo.m_rate;
	}

	levelDropNum = g_selectByMap(lvDropNum2Rate,levelDropNumTotalRate);
	charpterDropNum = g_selectByMap(charpterDropNum2Rate,charpterDropNumTotalRate);
	equipDropNum = g_selectByMap(equipDropNum2Rate,equipDropNumTotalRate);

	if (levelDropNum == -1){
		levelDropNum = 0;
	}

	if (charpterDropNum == -1){
		charpterDropNum = 0;
	}
	
	if (equipDropNum == -1){
		equipDropNum = 0;	
	}
	CDropMgr::getSingleton().processDropGroup(pCopyMapBase->m_dropID,dropVec,levelDropNum);
	CDropMgr::getSingleton().processDropGroup(pCopyMapBase->m_charpterDropID,dropVec,charpterDropNum);
	CDropMgr::getSingleton().processDropGroup(pCopyMapBase->m_equipDrawID,dropVec,equipDropNum);
}

void CLevelMgr::notifyEnterLvInfo(stCopyMapBase * pCopyMapBase)
{
	if (!pCopyMapBase){
		return ;
	}
	m_randomDropItems.clear();

	Map::NotifyEnterLevelMap enterCmd;
	enterCmd.set_levelid(pCopyMapBase->m_id);
	for (UINT16 i = 0;i < MAX_LEVEL_MONSTER_ID; ++i){
		if (pCopyMapBase->m_monsterInfo[i].m_npcID){
			Map::stSynMonsterInfo *pSyn = enterCmd.add_monsterinfo();
			pSyn->set_monsterid(pCopyMapBase->m_monsterInfo[i].m_npcID);
			pSyn->set_monsterlv(pCopyMapBase->m_monsterInfo[i].m_npcLv);
		}
	}

	if (pCopyMapBase->m_bossInfo.m_npcID){
		enterCmd.mutable_bossinfo()->set_monsterid(pCopyMapBase->m_bossInfo.m_npcID);
		enterCmd.mutable_bossinfo()->set_monsterlv(pCopyMapBase->m_bossInfo.m_npcLv);
	}

	std::vector<CDropMgr::stDropEntry> dropVec;
	genDropInfo(dropVec,pCopyMapBase);

	for (UINT16 i = 0;i < dropVec.size(); ++i){
		CDropMgr::stDropEntry & dropInfo = dropVec[i];
		m_pOwner->m_levelMgr.addDropItem(dropInfo.m_itemID,dropInfo.m_num);
		Map::stSynDropInfo *pSyn = enterCmd.add_dropinfo();
		if (pSyn){
			pSyn->set_itemid(dropInfo.m_itemID);
			pSyn->set_num(dropInfo.m_num);
		}
	}
	
	dropVec.clear();
	CDropMgr::getSingleton().processDropGroup(pCopyMapBase->m_drawCardID,dropVec,1);
	if (dropVec.size()){
		 enterCmd.mutable_drawinfo()->set_itemid(dropVec[0].m_itemID);
		 enterCmd.mutable_drawinfo()->set_num(dropVec[0].m_num);
		 m_pOwner->m_levelMgr.setOpenBoxInfo(dropVec[0].m_itemID,dropVec[0].m_num);
	}

	m_pOwner->sendProto2Me(enterCmd);

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("(name:%s,id:%d)随机出来了所有道具有:",m_pOwner->getName(),m_pOwner->getID());
		std::map<UINT32,UINT32>::iterator it = m_randomDropItems.begin();
		for (;it != m_randomDropItems.end();++it){
			Athena::logger->trace("itemID:%d,num:%d",it->first,it->second);	
		}

		Athena::logger->trace("抽奖得到的道具:itemID:%d,num:%d",m_openBoxItemID,m_openBoxItemNum);
	}
#endif
}

bool CLevelMgr::isPVELevel(UINT16 type)
{
	switch (type){
		case eLevelType_Normal :
		case eLevelType_Leader :
		case eLevelType_Elite :
		case eLevelType_GoldCoin :
		case eLevelType_Survival :
		case eLevelType_OpenShow :
		case eLevelType_COC :
		case eLevelType_Gold :
		case eLevelType_Long :
		case eLevelType_Snow :
			{
				return true;
			}
			break;
	}
	return false;
}

void CLevelMgr::enterArenaLv(UINT32 levelID, GameUser* pRobot)
{
	SortProto::stChallengeArenaUserResult  retCmd;
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelID);
	if (!pCopyMapBase){
		retCmd.set_result(SortProto::stChallengeArenaUserResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,找不到关卡配置,进入竞技场失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (m_pOwner->getLevel() < pCopyMapBase->m_limitLv){
		retCmd.set_result(SortProto::stChallengeArenaUserResult::eLevel_Limit);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,等级限制,进入竞技场失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
	UINT16 needSlot = 2;
	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < needSlot){
		retCmd.set_result(SortProto::stChallengeArenaUserResult::eBagFull);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,背包满了,竞技场失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (!m_pOwner->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
		retCmd.set_result(SortProto::stChallengeArenaUserResult::eLilian_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,历练不足,进入竞技场失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (m_pOwner->m_levelMgr.isInLv()){
		Athena::logger->error("[竞技场]account=%s,name=%s,id=%d 已经pve场景",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		retCmd.set_result(SortProto::stChallengeArenaUserResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	if (m_pOwner->getMap() && m_pOwner->getMap()->isCopyMap() && (!((CCopyMap *)m_pOwner->getMap())->isGuildHall())){
		Athena::logger->error("[竞技场]account=%s,name=%s,id=%d 已经在副本了",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		retCmd.set_result(SortProto::stChallengeArenaUserResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	if (Athena::global["nftServer"] == "true")
	{ 
			// 裝備等值規則 ：1件ssr=2件sr（ur也屬於ssr條件）
			UINT32 srValue = 0;
			UINT32 srNeed = 1;
			for (int i = HEAD_SLOT_INDEX; i <= WING_SLOT_INDEX; i++)
			{
				CItem *pItem = m_pOwner->m_bagMgr.m_equipBag.getEquipByIndex(i);
				if (pItem != NULL)
				{
					UINT8 quality = pItem->m_pItemBase->m_quality;
					if (pItem->isSR(quality))
					{
						srValue++;
						break;
					}
					else if (pItem->isSSR(quality) || pItem->isUR(quality))
					{
						srValue +=2;
						break;
					} 
				}
			} 
			if (srValue < srNeed)
			{
				retCmd.set_result(SortProto::stChallengeArenaUserResult::eEquip);
				m_pOwner->sendProto2Me(retCmd);	
				return;
			}   
	}

	if (Athena::global["checkPvP"] == "true")
	{
		if (pRobot == NULL)
		{
			Athena::logger->error("[竞技场]account=%s,name=%s,id=%d 机器人数据为空",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
			retCmd.set_result(SortProto::stChallengeArenaUserResult::eInner_Error);
			m_pOwner->sendProto2Me(retCmd);
			return;
		}
		pRobot->m_bRobot = true;
		CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(m_pOwner->getName(),pCopyMapBase->m_id);
		if (!pCopyMap){
			Athena::logger->error("[竞技场]account=%s,name=%s,id=%d 创建副本不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
			retCmd.set_result(SortProto::stChallengeArenaUserResult::eInner_Error);
			m_pOwner->sendProto2Me(retCmd);
			return;
		}
		
		pCopyMap->createCamp(Map::eCamp_Red);
		pCopyMap->createCamp(Map::eCamp_Black);
		
		m_pOwner->setCampID(Map::eCamp_Black);
		pCopyMap->joinCamp(Map::eCamp_Black,m_pOwner);

		notifyEnterLvInfo(pCopyMapBase);
		m_pOwner->m_levelMgr.setInLvFlag(true);
		m_pOwner->resetBattleProp();
		CMapMgr::getSingleton().changeMap(m_pOwner,pCopyMap,1);

		pRobot->setCampID(Map::eCamp_Red);
		pRobot->resetBattleProp();
		pCopyMap->joinCamp(Map::eCamp_Red,pRobot);
		CMapMgr::getSingleton().robotChangeMap(pRobot,pCopyMap,2);
	

	}else{
		notifyEnterLvInfo(pCopyMapBase);
		m_pOwner->m_levelMgr.setInLvFlag(true);
		Map::stNotifyEnterMapInfo loadSceneCmd;
		loadSceneCmd.set_mapid(pCopyMapBase->m_id);
		m_pOwner->sendProto2Me(loadSceneCmd);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("进入竞技场成功:%d",pCopyMapBase->m_id);
#endif
	return ;

}

void CLevelMgr::enterInterceptLv(UINT32 levelID)
{
	if (Athena::global["checkPvP"] == "true")
	{
		return;
	}

	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelID);
	if (!pCopyMapBase){
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,找不到关卡配置,进入竞拦截场景失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	notifyEnterLvInfo(pCopyMapBase);
	m_pOwner->m_levelMgr.setInLvFlag(true);
	Map::stNotifyEnterMapInfo loadSceneCmd;
	loadSceneCmd.set_mapid(pCopyMapBase->m_id);
	m_pOwner->sendProto2Me(loadSceneCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("进入拦截场景成功:%d",pCopyMapBase->m_id);
#endif
	return ;
}


void CLevelMgr::enterInterceptLv_ServerCheck(UINT32 levelID, GameUser* pRobot, UINT32 buffCnt)
{
	if (Athena::global["checkPvP"] != "true")
	{
		return;
	}
	if (m_pOwner->m_levelMgr.isInLv() || (m_pOwner->getMap() && m_pOwner->getMap()->isCopyMap())) {
        Athena::logger->error("[进入拦截场景]已经在关卡中");
        return ;
    }
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelID);
	if (!pCopyMapBase){
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,找不到关卡配置,进入竞拦截场景失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (pRobot == NULL)
	{
		Athena::logger->error("[竞技场]account=%s,name=%s,id=%d 机器人数据为空",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		return;
	}
	pRobot->m_bRobot = true;
	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(m_pOwner->getName(),pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[竞技场]account=%s,name=%s,id=%d 创建副本不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		return;
	}

	pCopyMap->setSpecailParam(0, buffCnt);
	
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);
	
	m_pOwner->setCampID(Map::eCamp_Black);
	pCopyMap->joinCamp(Map::eCamp_Black,m_pOwner);

	notifyEnterLvInfo(pCopyMapBase);
	m_pOwner->m_levelMgr.setInLvFlag(true);
	m_pOwner->resetBattleProp();
	CMapMgr::getSingleton().changeMap(m_pOwner,pCopyMap,1);

	pRobot->setCampID(Map::eCamp_Red);
	pRobot->resetBattleProp();
	pCopyMap->joinCamp(Map::eCamp_Red,pRobot);
	CMapMgr::getSingleton().robotChangeMap(pRobot,pCopyMap,2);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("进入拦截场景成功:%d",pCopyMapBase->m_id);
#endif
	return ;
}

void CLevelMgr::enterLeitaiLv(UINT8 mode)
{
	if (Athena::global["checkPvP"] == "true")
	{
		GameUser* pRobot = m_leitaiPKInfo.m_pRobot;
		if (pRobot == NULL)
		{
			Athena::logger->error("[擂台]account=%s,name=%s,id=%d 机器人数据为空",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
			return;
		}
	}
	UINT32 levelID = 0;
	ArenaConfig::stLeitaiSwitchMap * pSwitchMap = NULL;
	if (SortProto::eLeitaiMode_Normal == mode){//简单模式
		pSwitchMap = ArenaConfig::getSingleton().getEasySwitchMap();	
	}
	else {//苦难模式
		pSwitchMap = ArenaConfig::getSingleton().getDiffSwitchMap();
	}
	
	if (!pSwitchMap || !pSwitchMap->m_level2Rate.size()){
		Athena::logger->error("进入擂台失败,配置文件错误!");
		return ;
	}
	
	UINT32 totalLvRate = 0;
	std::map<UINT32,UINT32> levelRates;
	std::map<UINT32,UINT32 >::iterator lvIt = pSwitchMap->m_level2Rate.begin();
	for (;lvIt != pSwitchMap->m_level2Rate.end();++lvIt){
		totalLvRate += lvIt->second;	
		levelRates[lvIt->first] = lvIt->second;
	}

	INT32 index = g_selectByMap(levelRates,totalLvRate); 
	if (-1 == index){
		Athena::logger->error("擂台进入关卡时候错误,随机不到关卡");
		return ;
	}
	
	levelID = index;
	m_leitaiPKInfo.m_levelID = levelID;

	SortProto::stChallegeLeitaiUserResult  retCmd;
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelID);
	if (!pCopyMapBase){
		retCmd.set_result(SortProto::stChallegeLeitaiUserResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,找不到关卡配置,进入擂台失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (m_pOwner->getLevel() < pCopyMapBase->m_limitLv){
		retCmd.set_result(SortProto::stChallegeLeitaiUserResult::eLevel_Limit);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,等级限制,进入擂台失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
	UINT16 needSlot = 2;
	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < needSlot){
		retCmd.set_result(SortProto::stChallegeLeitaiUserResult::eBagFull);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,背包满了,进入擂台失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (!m_pOwner->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
		retCmd.set_result(SortProto::stChallegeLeitaiUserResult::eLilian_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,历练不足,进入擂台失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	Map::NotifyEnterLevelMap enterCmd;
	enterCmd.set_levelid(pCopyMapBase->m_id);
	m_randomDropItems.clear();

	ArenaConfig::stLeitaiReward *pLeitaiReward = NULL;
	
	if (SortProto::eLeitaiMode_Normal == mode){
		pLeitaiReward = ArenaConfig::getSingleton().getEasyLeitaiReward();
	}
	else {
		pLeitaiReward = ArenaConfig::getSingleton().getDifficultLeitaiReward();
	}
	
	if (pLeitaiReward) {
		UINT16 pkRound = m_pOwner->m_leitaiMgr.getPKRound() + 1;
		std::map<UINT16,ArenaConfig::stLeitaiRoundReward>::iterator roundIt = pLeitaiReward->m_roundReward.find(pkRound);
		
		if (roundIt != pLeitaiReward->m_roundReward.end()){
			std::vector<CDropMgr::stDropEntry> dropVec;
			ArenaConfig::stLeitaiRoundReward & roundReward = roundIt->second;
			
			if (g_selectByTenThound(roundReward.m_rate1)){
				m_pOwner->m_levelMgr.addDropItem(roundReward.m_itemID1,roundReward.m_num1);
				Map::stSynDropInfo *pSyn = enterCmd.add_dropinfo();
				if (pSyn){
					pSyn->set_itemid(roundReward.m_itemID1);
					pSyn->set_num(roundReward.m_num1);
				}
			}
			
			if (g_selectByTenThound(roundReward.m_rate2)){
				m_pOwner->m_levelMgr.addDropItem(roundReward.m_itemID2,roundReward.m_num2);
				Map::stSynDropInfo *pSyn = enterCmd.add_dropinfo();
				if (pSyn){
					pSyn->set_itemid(roundReward.m_itemID2);
					pSyn->set_num(roundReward.m_num2);
				}
			}
			
			if (g_selectByTenThound(roundReward.m_rate3)){
				m_pOwner->m_levelMgr.addDropItem(roundReward.m_itemID3,roundReward.m_num3);
				Map::stSynDropInfo *pSyn = enterCmd.add_dropinfo();
				if (pSyn){
					pSyn->set_itemid(roundReward.m_itemID3);
					pSyn->set_num(roundReward.m_num3);
				}
			}
			
			if (g_selectByTenThound(roundReward.m_rate4)){
				m_pOwner->m_levelMgr.addDropItem(roundReward.m_itemID4,roundReward.m_num4);
				Map::stSynDropInfo *pSyn = enterCmd.add_dropinfo();
				if (pSyn){
					pSyn->set_itemid(roundReward.m_itemID4);
					pSyn->set_num(roundReward.m_num4);
				}
			}

			dropVec.clear();
			CDropMgr::getSingleton().processDropGroup(pLeitaiReward->m_dropID,dropVec,1);
			if (dropVec.size()){
				enterCmd.mutable_drawinfo()->set_itemid(dropVec[0].m_itemID);
				enterCmd.mutable_drawinfo()->set_num(dropVec[0].m_num);
				m_pOwner->m_levelMgr.setOpenBoxInfo(dropVec[0].m_itemID,dropVec[0].m_num);
			}
		}
	}
	
	m_pOwner->sendProto2Me(enterCmd);

	if (Athena::global["checkPvP"] == "true")
	{
		GameUser* pRobot = m_leitaiPKInfo.m_pRobot;
		pRobot->m_bRobot = true;
		CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(m_pOwner->getName(),pCopyMapBase->m_id);
		if (!pCopyMap){
			Athena::logger->error("[擂台]account=%s,name=%s,id=%d 创建副本不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
			retCmd.set_result(SortProto::stChallegeLeitaiUserResult::eInner_Error);
			m_pOwner->sendProto2Me(retCmd);
			return;
		}
		
		pCopyMap->createCamp(Map::eCamp_Red);
		pCopyMap->createCamp(Map::eCamp_Black);
		
		m_pOwner->setCampID(Map::eCamp_Black);
		pCopyMap->joinCamp(Map::eCamp_Black,m_pOwner);

		//notifyEnterLvInfo(pCopyMapBase);
		m_pOwner->m_levelMgr.setInLvFlag(true);
		m_pOwner->resetBattleProp();
		CMapMgr::getSingleton().changeMap(m_pOwner,pCopyMap,1);

		pRobot->setCampID(Map::eCamp_Red);
		pRobot->resetBattleProp();
		pCopyMap->joinCamp(Map::eCamp_Red,pRobot);
		CMapMgr::getSingleton().robotChangeMap(pRobot,pCopyMap,2);
	}
	else
	{
		m_pOwner->m_levelMgr.setInLvFlag(true);
		Map::stNotifyEnterMapInfo loadSceneCmd;
		loadSceneCmd.set_mapid(pCopyMapBase->m_id);
		m_pOwner->sendProto2Me(loadSceneCmd);
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("进入擂台成功:%d",pCopyMapBase->m_id);
#endif
	return ;
}

void CLevelMgr::enterLv(UINT32 levelID)
{
	checkDataRefresh();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了%s请求进入关卡id=%d",m_pOwner->getName(),levelID);
#endif
	
	Map::stEnterLevelError  errorCmd;
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelID);
	if (!pCopyMapBase){
		errorCmd.set_errorcode(Map::stEnterLevelError::SVR_CFG_ERROR);
		m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,找不到关卡配置,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (!isPVELevel(pCopyMapBase->m_type)){
		Athena::logger->error("非pve关卡试图进入从CLevelMgr::enterLv入口进入,失败!");
		return;
	}
	
	/*if (m_pOwner->m_levelMgr.isInLv()){//是否在关卡中
		errorCmd.set_errorcode(Map::stEnterLevelError::HAS_IN_LEVEL);
		m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,已经在关卡中,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}*/

	if(!isTaskLevel(pCopyMapBase->m_type)){
		if (m_pOwner->getLevel() < pCopyMapBase->m_limitLv){
			errorCmd.set_errorcode(Map::stEnterLevelError::LEVEL_LIMIT);
			m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("account=%s,name=%s,id=%d,等级限制,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}

	UINT16 needSlot = 0;
	if (eLevelType_GoldCoin == pCopyMapBase->m_type){
		needSlot = 2;
	}
	else if (eLevelType_Survival == pCopyMapBase->m_type){
		needSlot = 2;
	}
	else if (eLevelType_AreaPK == pCopyMapBase->m_type){
		needSlot = 2;
	}
	else {
		needSlot = 8;
	}

	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < needSlot){
		errorCmd.set_errorcode(Map::stEnterLevelError::BAG_FULL);
		m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,背包满了,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
	if (!m_pOwner->checkResource(eResource_Sp,pCopyMapBase->m_costSP)){
		errorCmd.set_errorcode(Map::stEnterLevelError::SP_NOT_ENGOUH);
		m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,体力不足,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}
	
	if (!m_pOwner->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
		errorCmd.set_errorcode(Map::stEnterLevelError::LILIAN_NOT_ENOUGH);
		m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,历练不足,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (Athena::global["nftServer"] == "true")
	{ 
		UINT32 srValue = 0;
		UINT32 srNeed = 1;
		// 裝備等值規則 ：1件ssr=2件sr（ur也屬於ssr條件）
		for (int i = HEAD_SLOT_INDEX; i <= WING_SLOT_INDEX; i++)
		{
			CItem *pItem = m_pOwner->m_bagMgr.m_equipBag.getEquipByIndex(i);
			if (pItem != NULL)
			{
				UINT8 quality = pItem->m_pItemBase->m_quality;
				if (pItem->isSR(quality))
				{
					srValue++;
				}
				else if (pItem->isSSR(quality) || pItem->isUR(quality))
				{
					srValue +=2;
				} 
			}
		}
		stChapEquipLimit* plimit = CMiscCfgMgr::getSingleton().getChapEqupLimit(pCopyMapBase->m_chapter);
		if(plimit && (plimit->m_sr>0||plimit->m_ssr>0))
		{
			srNeed = plimit->m_sr;
			if (plimit->m_sr == 0)
			{
				srNeed = plimit->m_ssr*2;
			} 
		}
		if (srValue < srNeed)
		{
			errorCmd.set_errorcode(Map::stEnterLevelError::EQUIP_LIMIT);
			m_pOwner->sendProto2Me(errorCmd);
			return;
		}  
	}

	if(!isTaskLevel(pCopyMapBase->m_type)){
		if (!m_pOwner->m_levelMgr.isLevelFinish(pCopyMapBase->m_preLvID)){
			errorCmd.set_errorcode(Map::stEnterLevelError::PRE_LEVEL_NOT_FINISH);
			m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("account=%s,name=%s,id=%d,等级限制,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}

		UINT16 rushNum = m_pOwner->m_levelMgr.getRushNum(pCopyMapBase->m_id);
		if ((1 == pCopyMapBase->m_chapter) && (rushNum > 0)){//章节一的只能打一次
		/*	errorCmd.set_errorcode(Map::stEnterLevelError::RUSH_NUM_USE_OUT);
			m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("account=%s,name=%s,id=%d,次数用完了,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif*/
		}
		else if (rushNum >= m_pOwner->getCountByVip(eVipFun_Battle_Num)){
			errorCmd.set_errorcode(Map::stEnterLevelError::RUSH_NUM_USE_OUT);
			m_pOwner->sendProto2Me(errorCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("account=%s,name=%s,id=%d,次数用完了,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;
		}
	}
	notifyEnterLvInfo(pCopyMapBase);

	m_pOwner->m_levelMgr.setInLvFlag(true);

	Map::stNotifyEnterMapInfo loadSceneCmd;
	loadSceneCmd.set_mapid(pCopyMapBase->m_id);
	m_pOwner->sendProto2Me(loadSceneCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("进入关卡成功:%d",pCopyMapBase->m_id);
#endif
	return ;
}

bool CLevelMgr::isTaskLevel(UINT32 copyMapType)
{
	if(copyMapType == eLevelType_Gold 
			|| copyMapType == eLevelType_Long
			|| copyMapType == eLevelType_Snow){
		return true;
	}
	return false;
}

void CLevelMgr::arenaChallengeRank(UINT32 rank)
{
	m_arenaPKInfo.m_rank = rank;
	m_arenaPKInfo.m_challengeTime = g_gameServer->getSec();
}

void CLevelMgr::openBox()
{
	if (!m_openBoxItemID){
		return ;
	}

	if (!m_canOpenBox){
		Athena::logger->trace("(name=%s,id=%d)还没有结束关卡,却试图开盘牌",m_pOwner->getName(),m_pOwner->getID());
		return ;
	}
	m_canOpenBox = false;
	m_pOwner->giveAssert(m_openBoxItemID,m_openBoxItemNum,0,"关卡翻牌");
	m_openBoxItemID = 0;
	m_openBoxItemNum = 0;
}

void CLevelMgr::calcRefreshTime()
{
	struct tm _tm;
	RealTime::getLocalTime(_tm,g_gameServer->getSec());
	UINT32 curSecOfDay =  _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;
	UINT32 refreshSecOfDay = 5 * 3600;
	if (curSecOfDay > refreshSecOfDay){
		m_lastRefreshTime = g_gameServer->getSec() - (curSecOfDay - refreshSecOfDay);		
	}
	else {
		m_lastRefreshTime = g_gameServer->getSec() - 86400 + (refreshSecOfDay - curSecOfDay);
	}
}

void CLevelMgr::checkDataRefresh()
{
	UINT32 	interval = 0;
	if (0 == m_lastRefreshTime){//刷新时间为0,说明第一次登录
		calcRefreshTime();
	}

	interval = g_gameServer->getSec() > m_lastRefreshTime ? g_gameServer->getSec() - m_lastRefreshTime : 0;

	if (interval >= 86400){
		m_teamCopymapNum = 0;
		calcRefreshTime();
		
		std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.begin();
		
		for (;it != m_charpterInfos.end();++it){
			stCharpterInfoDB & charpter = it->second;
			if (Athena::global["nftServer"] != "true" && 1 == charpter.m_charpterID){//章节1的都不刷新, 新增机甲项目判断(机甲没有这个限制)
				continue;
			}
			std::map<UINT32,stLevelInfoDB>::iterator subIt = charpter.m_levelInfos.begin();
			for (;subIt != charpter.m_levelInfos.end();++subIt){
				stLevelInfoDB & lvInfo = subIt->second;
				lvInfo.m_rushNum = 0;
				lvInfo.m_resetNum = 0;
			}
		}
	}	
}

bool CLevelMgr::isCalcStar(UINT8 lvType)
{
	switch (lvType){
		case eLevelType_Normal :
		case eLevelType_Leader :
		case eLevelType_Elite :
		case eLevelType_GoldCoin :
			{
				return true;
			}
			break;
	}
	return false;
}

void CLevelMgr::resetAllTeamCopymap()
{
	std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.find(0);
	if (it == m_charpterInfos.end()){
		return ;
	}

	stCharpterInfoDB & charpter = it->second;
	std::map<UINT32,stLevelInfoDB>::iterator subIt = charpter.m_levelInfos.begin();

	for (;subIt != charpter.m_levelInfos.end();++subIt){
		stLevelInfoDB & levelInfo = subIt->second;
		
		stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelInfo.m_levelID);
		if (pCopyMapBase && (eLevelType_Team == pCopyMapBase->m_type)){
			levelInfo.m_rushNum = 0;	
		}
	}
}

void CLevelMgr::enterEmeraldDream(UINT32 levelID)
{
	SortProto::stEnterEmeraldDreamResult retCmd;
	
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了%s请求进入翡翠梦境id=%d",m_pOwner->getName(),levelID);
#endif

	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(levelID);
	if(!pCopyMapBase){
		retCmd.set_result(SortProto::stEnterEmeraldDreamResult::SVR_CFG_ERROR);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[进入翡翠梦境副本失败]account=%s,name=%s,id=%d,找不到关卡配置,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	struct tm curTm;
	RealTime::getLocalTime(curTm, g_gameServer->getOpenServerTime());
	UINT32 restTime = 86400 -  (curTm.tm_hour * 3600 + curTm.tm_min * 60 + curTm.tm_sec);//获得开服时间到当天晚上0点还剩多少秒
	//开服当天也算一天 加上剩下天数秒数
	UINT32 openTime = 0;
	UINT32 delayDay = m_pOwner->m_openFuncMgr.getOpenDay(OpenFunID_Small_Monster);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[进入翡翠梦境副本]延时天数：%d",delayDay);
#endif
	if(delayDay > 0){
		openTime = g_gameServer->getOpenServerTime() + restTime + (delayDay - 1) * 86400;
	}else{
		openTime = g_gameServer->getOpenServerTime() - (curTm.tm_hour * 3600 + curTm.tm_min * 60 + curTm.tm_sec);
	}
	if(g_gameServer->getSec() < openTime){
		retCmd.set_result(SortProto::stEnterEmeraldDreamResult::NOT_OPEN_TIME);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[进入翡翠梦境副本失败]account=%s,name=%s,id=%d,不在开放时间，进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (!m_pOwner->m_openFuncMgr.checkFunOpen(OpenFunID_Small_Monster)){
		retCmd.set_result(SortProto::stEnterEmeraldDreamResult::eLvl_Limit);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[进入翡翠梦境副本失败]account=%s,name=%s,id=%d,等级限制,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 8){
		retCmd.set_result(SortProto::stEnterEmeraldDreamResult::BAG_FULL);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[进入翡翠梦境副本失败]account=%s,name=%s,id=%d,背包满了,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif   
		return ;
	 }

	if((m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CURRENT_NUM) + 1) > m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM)){
		retCmd.set_result(SortProto::stEnterEmeraldDreamResult::Fight_Num_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[进入翡翠梦境副本失败]account=%s,name=%s,id=%d,次数用完了,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(pCopyMapBase->m_bossInfo.m_npcID,1));
	if(!pMonsterBase){
#ifdef _HDZ_DEBUG
        Athena::logger->error("[进入翡翠梦境副本失败]account=%s,name=%s,id=%d,创建BOSS失败,进入关卡失败",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (m_pOwner->m_levelMgr.isInLv() || (m_pOwner->getMap() && m_pOwner->getMap()->isCopyMap())) {
        Athena::logger->error("[进入翡翠梦境副本失败]已经在关卡中");
        return ;
    }

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(m_pOwner->getName(),levelID);
	if (!pCopyMap){
		Athena::logger->error("[进入翡翠梦境副本失败]进入副本失败,创建副本不成功!");
		return;
	}

	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->setBossHP(pMonsterBase->m_hp);
	pCopyMap->setBossLv(pMonsterBase->m_level);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("设置血量为:%d,等级为：%d",pMonsterBase->m_hp, pMonsterBase->m_level);
#endif

 	m_pOwner->setCampID(Map::eCamp_Red);
  	pCopyMap->joinCamp(Map::eCamp_Red,m_pOwner);
   	m_pOwner->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
    m_pOwner->resetBattleProp();

	CMapMgr::getSingleton().changeMap(m_pOwner,pCopyMap, 1);

	m_pOwner->m_levelMgr.setInLvFlag(true);
	retCmd.set_result(SortProto::stEnterEmeraldDreamResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);

	pCopyMap->notifyExector();

#ifdef _HDZ_DEBUG
	Athena::logger->trace("请求进入翡翠梦境id=%d成功！",m_pOwner->getName(),levelID);
#endif

    m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Emerald_Dream, 1, m_pOwner->getVipLv());
}

UINT32 CLevelMgr::getGuajiLv()
{
	//查找关卡进度
	INT32 chapid = 2;
	UINT32 lvID = 10100; //默认按关卡1-1
	std::map<UINT32,stCharpterInfoDB>::iterator it =  m_charpterInfos.begin();
	for (;it != m_charpterInfos.end(); ++it){
		stCharpterInfoDB & charpter = it->second;
		if (INT32(charpter.m_charpterID) > chapid)
		{
			chapid = INT32(charpter.m_charpterID);
		}
	}
	it = m_charpterInfos.find(UINT32(chapid));
	if (it != m_charpterInfos.end()){
		std::map<UINT32,stLevelInfoDB>::iterator subIt = it->second.m_levelInfos.begin();	
		lvID = subIt->first;
	}
	return lvID;
}