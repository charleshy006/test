#include "CopyMap.h"
#include "GameUser.h"
#include "Npc.h"
#include "GameUserManager.h"
#include "NpcMgr.h"
#include "MapMgr.h"
#include "TblDataMgr.h"
#include <math.h>
#include "GameServer.h"
#include "GlobalSvrClient.h"
#include "GameEventNotify.h"
#include "ArenaConfig.h"
#include "HeroMgr.h"
#include "RobotMgr.h"
#include "GuildProto.pb.h"
#include "OperateActivityCfg.h"
#include <iostream>
#include <sstream>
#include "PubGame.pb.h"
#include "HeroLeagueCfg.h"
#include "HeroLeague.h"


extern GameServer * g_gameServer;

CCopyMap::CCopyMap():
	m_oneSec(1),m_twoSec(2),m_tenSec(10)
{
	reset();
	m_bFinish = false;
	m_bFail = 0;
	m_hasLoadResNum = 0;
	m_pkWithFriend = false;
}

CCopyMap::~CCopyMap()
{

}

//副本是否已经到时
bool CCopyMap::isTimeOut(UINT64 uTime)
{
	if(uTime >= m_openTime + m_lastTime){//副本时间持续时间到了
		return true;
	}

	/*if ((!m_loadResFinish) && (uTime >= m_openTime + 120 * 1000)){//两分钟之内还没有加载完
	  return true;	
	  }*/
	return false;
}


void CCopyMap::timerUpdate(UINT64 uTime,UINT16 group)
{
	//秒定时器
	if(m_oneSec(g_gameServer->getRealTime()))
	{
		//实现副本具体逻辑功能
		//如果到达时间，则结束
		if(isTimeOut(uTime) || checkFailLv())
		{
			doFail();
			endGame();//立即结束
		}


		if(m_isEnd) //副本已经结束了
		{
			doEndAct(uTime);
		}
		else
		{
			doAct(uTime);
		}
	}

	CMap::timerUpdate(uTime,group);
}

void CCopyMap::doAct(UINT64 uTime)
{
	if (m_openTime + s_WAIT_TIME < uTime){
		notifyBegin();
	}

	if (!m_bBegin){//还没有开始
		return ;
	}

	if (m_twoSec(g_gameServer->getRealTime())){
		summonBoss();
		summonRes();
		summonBuff1();
		summonBuff2();
	}

	if (checkFinishLv()){
		doFinish();	
	}
}

void CCopyMap::doEndAct(UINT64 uTime)
{
	//sendAllUser2City();
	//然后卸载地图
	//unLoadMap();
}

void CCopyMap::reset()
{
	m_isEnd = false;
	m_endTime = 0;
	m_openTime = g_gameServer->getMSec();
	m_realStartTime = g_gameServer->getMSec();
	m_lastTime = 86400 * 30 * 1000l;
	m_pCopyMapBase = NULL;
	m_ownerID = 0;
	m_loadResFinish = false;
	m_bBegin = false;
}


void CCopyMap::endGame()
{
	if (m_isEnd){
		return;
	}

	m_isEnd = true;
	m_endTime = g_gameServer->getMSec();//记录当前时间 
#ifdef _HDZ_DEBUG
	Athena::logger->trace("副本结束了,mapID=%d,mapTempID=%d",getID(),getTempID());
#endif
}

bool CCopyMap::checkIsAllDie(CObj::ObjType eType)
{
	TObjSet & objSet = m_allObjs[eType];
	TObjSet::iterator it = objSet.begin();

	for (;it != objSet.end();++it){
		CBeing * pBeing =(CBeing *) *it;
		if (!pBeing->isDie()){
			return false;
		}		
	}
	return true;
}

void CCopyMap::sendAllUser2City()
{
	if (!m_pCopyMapBase){
		return ;
	}
	CMap * pNewMap = CMapMgr::getSingleton().getMapByID(NEW_PERSON_MAPID);
	if (!pNewMap){
		Athena::logger->error("副本结束时候,传送玩家到主城时候找不到地图对象!");
		return ;
	}
	TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
	TObjSet::iterator it = objSet.begin();

	std::vector<GameUser *> userVec;
	for (;it != objSet.end();++it){
		GameUser * pUser =(GameUser *) * it;
		if (pUser){
			userVec.push_back(pUser);
		}
	}

	for (UINT16 i = 0;i < userVec.size();++i){
		if (userVec[i]->m_bRobot){
			CRobotMgr::getSingleton().returnRobot(m_pCopyMapBase->m_id,userVec[i]->getID());
			continue;
		}
		CMapMgr::getSingleton().changeMap(userVec[i],pNewMap);
	}
}

void CCopyMap::onNpcDie(CNpc* pNpc,CBeing * pAtter)//NPC死亡
{
	if (!pNpc){
		return ;	
	}

	if (m_isEnd){
		return ;
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("有npc死亡");
#endif

	bool bIsSummonBoss = false;

	if (checkIsAllDie(CObj::OBJ_TYPE_MONSTER)){//所有怪都死亡了
		if (m_summonWave.size() > 0){ 
			std::map<UINT16,stSummonWave>::iterator it = m_summonWave.begin();  
			stSummonWave & wave = it->second;
			m_summonTime = g_gameServer->getSec() + wave.m_delay;

			for (UINT16 i = 0;i < wave.m_npcDefines.size();++i){
				NpcDefine & define = wave.m_npcDefines[i];
				if (define.m_isBoss){
					bIsSummonBoss = true;
				}
			}

			if ((wave.m_dirPos.m_x > 0) && (wave.m_dirPos.m_y > 0) && (wave.m_dirPos.m_z > 0)){
				Map::stNotifyUserPointToPos retCmd;
				retCmd.set_x(wave.m_dirPos.m_x);
				retCmd.set_y(wave.m_dirPos.m_y);
				retCmd.set_z(wave.m_dirPos.m_z);
				retCmd.set_lasttime(wave.m_lastTime);
				sendProto2Map(retCmd);
			}
		} 
#ifdef _HDZ_DEBUG
		Athena::logger->trace("所有npc都死亡了,要召唤下一波了");
#endif
	}

	if (bIsSummonBoss){//是否召唤boss
		Map::stAlertBossAppear cmd;
		cmd.set_delay(m_summonTime - g_gameServer->getSec());
		sendProto2Map(cmd);
	}
}

void CCopyMap::onHeroDie(CHero * pHero)	//武将死亡
{
	if (!pHero){
		return ;
	}
}

void CCopyMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)	//玩家死亡
{
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	CHeroMgr::getSingleton().clearHeroByOwerID(pUser->getID());
}

void CCopyMap::doFail()
{
	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}
}

void CCopyMap::doFinish()
{
	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	//让所有npc死亡
	TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_MONSTER];
	TObjSet::iterator it = objSet.begin();

	for (;it != objSet.end();++it){
		CBeing * pBeing =(CBeing *) *it;
		if (!pBeing->isDie()){
			UINT32 curHP = pBeing->getHP();
			pBeing->decHP(curHP,NULL);
		}		
	}

}


void CCopyMap::setCopyMapBase(stCopyMapBase * pCopyMapBase)
{
	if (!pCopyMapBase){
		return ;
	}

	m_pCopyMapBase = pCopyMapBase ;
}

void CCopyMap::summonBoss()
{
	if (m_isEnd) {//已经结束了不需要再判断
		return ;
	}

	if (m_summonTime && (g_gameServer->getSec() >  m_summonTime)){//时间到了,可以召唤boss出来了
		m_summonTime = 0;
		std::map<UINT16,stSummonWave>::iterator it = m_summonWave.begin();  
		if (it != m_summonWave.end()){
			NpcDefineVec &defineVec =  it->second.m_npcDefines;
			for (UINT16 i = 0; i < defineVec.size(); ++i){
				summonNpc(defineVec[i]);
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("召唤了第:%d波怪",it->first);
#endif
			/*stSummonWave & wave = it->second;
			  if ((wave.m_dirPos.m_x > 0) && (wave.m_dirPos.m_y > 0) && (wave.m_dirPos.m_z > 0)){
			  Map::stNotifyUserPointToPos retCmd;
			  retCmd.set_x(wave.m_dirPos.m_x);
			  retCmd.set_y(wave.m_dirPos.m_y);
			  retCmd.set_z(wave.m_dirPos.m_z);
			  sendProto2Map(retCmd);
			  }*/
			m_summonWave.erase(it);
		}
	}
}

bool CCopyMap::checkFinishLv()
{
	if (!m_pCopyMapBase){
		return false;	
	}

	if (m_bFinish){
		return true;
	}
	return false;
}

bool CCopyMap::checkFailLv()
{
	if (!m_pCopyMapBase){
		return false;	
	}

	if (m_bFail){
		return true;
	}
	return false;
}

UINT32 CCopyMap::getOrgHP(UINT32 hp)
{
	return hp;
}

void CCopyMap::onUserEnter(GameUser * pUser)
{
	if (!pUser || !m_pCopyMapBase){
		return ;
	}

	m_allEnterUsers.insert(pUser->getID());
}

bool CCopyMap::isExist(UINT32 charID)
{
	if (m_allEnterUsers.find(charID) != m_allEnterUsers.end()){
		return true;
	}
	else {
		return false;
	}
}

void CCopyMap::onHeroEnter(CHero *pHero)
{
}

void CCopyMap::notifyExector()
{
	if (getPlayerCount() > 0){//换人执行NPC AI
		GameUser * pUser1 = getFirstGameUser();	
		if (pUser1){
			Map::stNotifyExecAI cmd;
			if(g_gameServer->isPubGameServer()){
				cmd.set_charid(pUser1->getNormalCharID());
			}else{
				cmd.set_charid(pUser1->getID());
			}
			pUser1->sendProto2Me(cmd);
		}
	}
}

void CCopyMap::onUserLeave(GameUser * pUser)
{
	if (!pUser){
		return ;
	}
	CHeroMgr::getSingleton().clearHeroByOwerID(pUser->getID());
	notifyExector();
}

void CCopyMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	if (!pAtter){
		return ;
	}

	UINT32 memberID = 0;
	if (pAtter->theGameUser()){
		memberID = pAtter->getID();
	}
	else if (pAtter->theHero()){
		memberID = pAtter->getOwnerID();
	}

	if (memberID <= 0){
		return ;
	}

	stPKCamp & camp = m_campInfos[pAtter->getCampID()];
	std::map<UINT32,stPKCampMem>::iterator it = camp.m_memberIDs.find(memberID);
	if ( it != camp.m_memberIDs.end()){
		it->second.m_damage += decHP;
	}	
}

void CCopyMap::createCamp(UINT8 campID)
{
	stPKCamp & camp = m_campInfos[campID];
	camp.reset();
	camp.m_campID = campID;
}

void CCopyMap::joinCamp(UINT8 campID,UINT32 charID)
{
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.find(campID);
	if (it == m_campInfos.end()){
		return  ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUserOrRobot(charID);

	if (!pUser){
		Athena::logger->error("[加入阵营]玩家加入阵营失败,玩家已经不在线了!");
		return ;
	}

	stPKCamp & camp = it->second;
	stPKCampMem mem;
	mem.m_battlePower = pUser->m_roleInfo.m_battlePower;
	mem.m_charID = charID;
	mem.m_name = pUser->getName();
	mem.m_level = pUser->getLevel();
	camp.m_memberIDs.insert(std::make_pair(charID,mem));

	camp.m_aliveIDs.insert(charID);
	camp.m_dieIDs.erase(charID);
}

void CCopyMap::joinCamp(UINT8 campID,GameUser* pUser)
{
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.find(campID);
	if (it == m_campInfos.end()){
		return  ;
	}

	if (!pUser){
		return;
	}

	stPKCamp & camp = it->second;
	stPKCampMem mem;
	mem.m_battlePower = pUser->m_roleInfo.m_battlePower;
	mem.m_charID = pUser->getID();
	mem.m_name = pUser->getName();
	mem.m_level = pUser->getLevel();
	camp.m_memberIDs.insert(std::make_pair(pUser->getID(),mem));

	camp.m_aliveIDs.insert(pUser->getID());
	camp.m_dieIDs.erase(pUser->getID());
}

void CCopyMap::synAdd(CObj *pObj,const CAreaIdx & areaIdx)
{
	if (!pObj){
		return ;
	}

	{//告诉别人自己进来了
		TVecUserID  allNotifyUser;
		allNotifyUser.resize(100);
		TObjSet & userObjs =  m_allObjs[CObj::OBJ_TYPE_PLAYER];
		TObjSet::iterator it = userObjs.begin();

#ifdef _NEW_SYN_METHOD
		for (;it != userObjs.end();++it){
			CObj * pObjTmp = *it;
			if (pObjTmp && pObjTmp->enterViewSight(pObj->getType(),pObj->getUniqueID())){
				allNotifyUser.push_back(pObjTmp->getUniqueID());	
			}
		}
#else 
		for (;it != userObjs.end();++it){
			CObj * pObjTmp = *it;
			if (pObjTmp){
				allNotifyUser.push_back(pObjTmp->getID());
			}
		}
#endif
		if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
			Map::NotifyPlayerAppear playerCmd;
			Map::stPlayerData  * pPlayerData = playerCmd.add_data();
			if (!pPlayerData){
				return ;
			}
			pObj->getAppear(*pPlayerData);
            //时装翅膀
            GameUser * pSelf = pObj->theGameUser();
            if (pSelf) {
                CItem * pItem = pSelf->m_bagMgr.m_equipBag.getWingItem();
                if (pItem) {
                    pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
                }
            }
			GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,playerCmd);
#ifdef PLAYER_APPEAR_DEBUG
			Athena::logger->trace("[CopyMap]给下列玩家通知了charID=%d,发送了NotifyPlayerAppear协议",pObj->getID());
			for (UINT32 i = 0;i < allNotifyUser.size();++i){
				if (allNotifyUser[i] > 0)
				{
					Athena::logger->trace("charID=%d",allNotifyUser[i]);
				}
			}
#endif
		}
		else if (CObj::OBJ_TYPE_MONSTER == pObj->getType()){
			Map::NotifyNpcAppear npcCmd;
			Map::NotifyNpcAppear_stNpcData *pNpcData = npcCmd.add_data();
			if (!pNpcData){
				return ;
			}
			pObj->getAppear(*pNpcData);
			GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,npcCmd);
		}
		else if (CObj::OBJ_TYPE_HERO == pObj->getType()){
			Map::NotifyHeroAppear heroCmd;
			Map::NotifyHeroAppear_stHeroData * pHeroData = heroCmd.add_data();
			if (!pHeroData){
				return ;
			}
			pObj->getAppear(*pHeroData);
			GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,heroCmd);
		}

	}
	//如果是玩家发送其它人数据给他
	if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
		GameUser * pUser = (GameUser *)pObj;
		Map::NotifyPlayerAppear playerCmd;
		Map::NotifyNpcAppear npcCmd;
		Map::NotifyHeroAppear heroCmd;

		for (UINT8 objType = CObj::OBJ_TYPE_PLAYER; objType < CObj::OBJ_TYPE_MAX; ++objType){
			if (objType == CObj::OBJ_TYPE_PLAYER){
				TObjSet & objSet = m_allObjs[objType];
				TObjSet::iterator it = objSet.begin();

				for (;it != objSet.end();++it){
					CObj * pSynObj = *it;
					if (pObj == pSynObj) {//是自己不用发
						continue;
					}
#ifdef _NEW_SYN_METHOD
					if (!pObj->enterViewSight(pSynObj->getType(),pSynObj->getUniqueID())){
						continue;	
					}
#endif
					Map::stPlayerData  * pPlayerData = ATHENA_NEW Map::stPlayerData();
					if (!pPlayerData){
						return ;
					}
					pSynObj->getAppear(*pPlayerData);
                    //时装翅膀
                    GameUser * pOther = pSynObj->theGameUser();
                    if (pOther) {
                        CItem * pItem = pOther->m_bagMgr.m_equipBag.getWingItem();
                        if (pItem) {
                            pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
                        }
                    }

					if (pPlayerData->ByteSize() + playerCmd.ByteSize() >= (INT32)(MAX_DATASIZE - 5)){//人数满了
						pUser->sendProto2Me(playerCmd);
						playerCmd.mutable_data()->Clear();
#ifdef PLAYER_APPEAR_DEBUG
						Athena::logger->trace("[CopyMap]给玩家charID=%d发送了NotifyPlayerAppear协议 targetID[%d]",pUser->getID(), pSynObj->getID());
#endif
					}
					playerCmd.mutable_data()->AddAllocated(pPlayerData);
				}
			}
			else if (objType == CObj::OBJ_TYPE_MONSTER){
				TObjSet & objSet = m_allObjs[objType];
				TObjSet::iterator it = objSet.begin();

				for (;it != objSet.end();++it){
					CObj * pSynObj = *it;
#ifdef _NEW_SYN_METHOD
					if (!pObj->enterViewSight(pSynObj->getType(),pSynObj->getUniqueID())){
						continue;	
					}
#endif
					Map::NotifyNpcAppear_stNpcData *pNpcData = ATHENA_NEW Map::NotifyNpcAppear_stNpcData();
					if (!pNpcData){
						return ;
					}
					pSynObj->getAppear(*pNpcData);
					if (npcCmd.ByteSize() +  pNpcData->ByteSize()>= (INT32)(MAX_DATASIZE - 5)){//满了,先发送
						pUser->sendProto2Me(npcCmd);
						npcCmd.mutable_data()->Clear();
					}
					npcCmd.mutable_data()->AddAllocated(pNpcData);
				}
			}
			else if (objType == CObj::OBJ_TYPE_HERO){
				TObjSet & objSet = m_allObjs[objType];
				TObjSet::iterator it = objSet.begin();

				for (;it != objSet.end();++it){
					CObj * pSynObj = *it;
#ifdef _NEW_SYN_METHOD
					if (!pObj->enterViewSight(pSynObj->getType(),pSynObj->getUniqueID())){
						continue;	
					}
#endif
					Map::NotifyHeroAppear_stHeroData * pHeroData = ATHENA_NEW Map::NotifyHeroAppear_stHeroData();
					pSynObj->getAppear(*pHeroData);
					if (heroCmd.ByteSize() +  pHeroData->ByteSize()>= (INT32)(MAX_DATASIZE - 5)){
						pUser->sendProto2Me(heroCmd);
						heroCmd.mutable_data()->Clear();
					}
					heroCmd.mutable_data()->AddAllocated(pHeroData);
				}
			}
		}

		if (playerCmd.data_size()){
			pUser->sendProto2Me(playerCmd);
#ifdef PLAYER_APPEAR_DEBUG
		for (UINT32 i = 0; i < (UINT32)playerCmd.data_size();++i){
			Athena::logger->trace("[CopyMap]给charID=%d发送了charID=%d",pUser->getID(),playerCmd.data(i).moid());
		}
#endif
		}

		if (npcCmd.data_size()){
			pUser->sendProto2Me(npcCmd);
		}

		if (heroCmd.data_size()){
			pUser->sendProto2Me(heroCmd);
		}
	}
}

void CCopyMap::synRemove(CObj *pObj,const CAreaIdx & areaIdx)
{
	if (!pObj){
		return;
	}

	TVecUserID  allNotifyUser;
	allNotifyUser.resize(100);
	TObjSet & userObjs =  m_allObjs[CObj::OBJ_TYPE_PLAYER];
	TObjSet::iterator it = userObjs.begin();
#ifdef _NEW_SYN_METHOD
	for (;it != userObjs.end();++it){
		CObj * pObjTmp = *it;
		if (pObjTmp && pObjTmp->leaveViewSight(pObj->getType(),pObj->getUniqueID())){
			allNotifyUser.push_back(pObjTmp->getUniqueID());
		}
	}
#else
	for (;it != userObjs.end();++it){
		CObj * pObjTmp = *it;
		if (pObjTmp){
			allNotifyUser.push_back(pObjTmp->getID());
		}
	}
#endif

	Map::NotifyObjDisappear cmd;
	Map::NotifyObjDisappear_stObjDisappearInfo * disappearInfo = cmd.add_data();
	if (!disappearInfo){
		return ; 
	}   

	disappearInfo->set_motype(pObj->getType());
	disappearInfo->set_moid(pObj->getUniqueID());
	GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,cmd);
}

void CCopyMap::synMove(CObj *pObj,const CAreaIdx & areaIdxS,const CAreaIdx & areaIdxT)
{
	if (!pObj){
		return ;
	}

	TVecUserID  allNotifyUser;
	allNotifyUser.resize(100);
	TObjSet & userObjs =  m_allObjs[CObj::OBJ_TYPE_PLAYER];
	TObjSet::iterator it = userObjs.begin();

#ifdef _NEW_SYN_METHOD
	for (;it != userObjs.end();++it){
		CObj * pObjTmp = *it;
		if (pObjTmp && pObjTmp->isInViewSight(pObj->getType(),pObj->getUniqueID())){
			allNotifyUser.push_back(pObjTmp->getUniqueID());
		}
	}
#else
	for (;it != userObjs.end();++it){
		CObj * pObjTmp = *it;
		if (pObjTmp){
			allNotifyUser.push_back(pObjTmp->getID());
		}
	}
#endif
	Map::stNotifyMODMove cmd;
	Map::stNotifyMODMove_stModMoveInfo * pMoveInfo = cmd.add_data();
	if (!pMoveInfo){
		return ;
	}
	pMoveInfo->set_type(pObj->getType());
	pMoveInfo->set_x(pObj->getPos().m_x);
	pMoveInfo->set_y(pObj->getPos().m_y);
	pMoveInfo->set_z(pObj->getPos().m_z);
	pMoveInfo->set_dir(pObj->getPos().m_dir);
	pMoveInfo->set_modid(pObj->getUniqueID());
	GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,cmd);		
}

void CCopyMap::synTeleport(CObj *pObj,const CAreaIdx & areaIdxS,const CAreaIdx & areaIdxT)
{
	CMap::synTeleport(pObj,areaIdxS,areaIdxT);
}

void CCopyMap::resLoadFinish(GameUser * pUser)
{
	if (!pUser){
		return ;
	}

	m_hasLoadResNum += 1;

	bool allReady = false;

	if (eLevelType_Peakedness == m_pCopyMapBase->m_type){//巅峰对决
		if (m_hasLoadResNum >= 2){
			allReady = true;
		}
	}
	else if ((eLevelType_Team == m_pCopyMapBase->m_type) 
			|| (eLevelType_Guild_Copymap == m_pCopyMapBase->m_type)
			|| (eLevelType_World_Boss == m_pCopyMapBase->m_type)){//组队副本
		if (m_hasLoadResNum >= 3){
			allReady = true;
		}
	}

	if (allReady){
		notifyBegin();
	}
}

void CCopyMap::notifyBegin()
{
	if (m_bBegin){
		return ;
	}

	m_bBegin = true;
	m_realStartTime = g_gameServer->getMSec();
	Map::stBeginBattle cmd;
	cmd.set_delay(1);
	cmd.set_remaintime(getRemainTime());
	sendProto2Map(cmd);

	if (m_summonWave.size() > 0){
		std::map<UINT16,stSummonWave>::iterator it = m_summonWave.begin();  
		stSummonWave & wave = it->second;
		m_summonTime = g_gameServer->getSec() + wave.m_delay;
	}
}

void CCopyMap::summonRes()
{
	for (UINT16 i = 0; i < m_mobaReses.size();++i){
		stMobaResInstance & res = m_mobaReses[i];
		if (res.m_bExist){
			continue;
		}

		if (res.m_summonTime < g_gameServer->getSec()){
			res.m_bExist = true;	

			Map::stNotifyResAppear retCmd;
			retCmd.set_id(res.m_id);
			std::map<UINT16,ZoneDefine>::iterator it = m_pMapTemplate->m_resZoneDefines.find(res.m_id);
			if (it != m_pMapTemplate->m_resZoneDefines.end()){
				retCmd.set_x(it->second.m_rect.m_pos.m_x);
				retCmd.set_y(it->second.m_rect.m_pos.m_y);
				retCmd.set_z(it->second.m_rect.m_pos.m_z);
			}
			sendProto2Map(retCmd);

			TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
			TObjSet::iterator subIt = objSet.begin();

			for (;subIt != objSet.end();++subIt){
				GameUser * pUser =(GameUser *) * subIt;
				if (pUser && (!pUser->isDie())){
					onUserMove(pUser);
				}
			}
		}
	}
}

void CCopyMap::summonBuff1()
{
	for (UINT16 i = 0; i < m_mobaBuffs.size();++i){
		stMobaBuffInstance & buff = m_mobaBuffs[i];
		if (buff.m_bExist){//存在了
			continue;
		}

		if (buff.m_summonTime < g_gameServer->getSec()){
			buff.m_buffID = ArenaConfig::getSingleton().getMobaBuffID();
			buff.m_bExist = true;
			if (m_pMapTemplate){
				Map::stNotifyBuffAppear retCmd;
				retCmd.set_id(buff.m_id);
				retCmd.set_buffid(buff.m_buffID);
				std::map<UINT16,ZoneDefine>::iterator it = m_pMapTemplate->m_buffZoneDefines.find(buff.m_id);
				if (it != m_pMapTemplate->m_buffZoneDefines.end()){
					retCmd.set_x(it->second.m_rect.m_pos.m_x);
					retCmd.set_y(it->second.m_rect.m_pos.m_y);
					retCmd.set_z(it->second.m_rect.m_pos.m_z);
				}
				sendProto2Map(retCmd);

				TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
				TObjSet::iterator subIt = objSet.begin();

				for (;subIt != objSet.end();++subIt){
					GameUser * pUser =(GameUser *) * subIt;
					if (pUser && (!pUser->isDie())){
						onUserMove(pUser);
					}
				}
			}
		}
	}
	return;
}

void CCopyMap::summonBuff2()
{
	return ;
}

void CCopyMap::afterInit()
{
	if (!m_pMapTemplate){
		return;
	}	

	std::map<UINT16,ZoneDefine>::iterator it = m_pMapTemplate->m_buffZoneDefines.begin();
	for (;it != m_pMapTemplate->m_buffZoneDefines.end();++it){
		stMobaBuffInstance buff;
		buff.m_id = it->first;
		m_mobaBuffs.push_back(buff);
	}

	it = m_pMapTemplate->m_resZoneDefines.begin();
	for (;it != m_pMapTemplate->m_resZoneDefines.end();++it){
		stMobaResInstance res;
		res.m_id = it->first;
		m_mobaReses.push_back(res);
	}
}

void CCopyMap::addCampScore(UINT8 campID,UINT32 score)
{
	stPKCamp & camp =  m_campInfos[campID];
	camp.m_score += score;

	Map::stSynCampScore retCmd;
	retCmd.set_campid(camp.m_campID);
	retCmd.set_score(camp.m_score);
	sendProto2Map(retCmd);

	if (camp.m_score >= s_WIN_NEED_SCORE){//第一到底
		m_bFinish = true;
		camp.m_win = true;
	}
}	

UINT16 CCopyMap::checkCollideResZone(CPos & pos)
{
	if (!m_pMapTemplate){
		return 0;
	}

	return checkCollideZone(pos,m_pMapTemplate->m_resZoneDefines);
}

UINT16 CCopyMap::checkCollideZone(CPos & pos,std::map<UINT16,ZoneDefine> & zones)
{
	std::map<UINT16,ZoneDefine>::iterator it = zones.begin();
	for (;it != zones.end();++it){
		ZoneDefine & zone = it->second;
		INT32 minX = zone.m_rect.m_pos.m_x - zone.m_rect.m_width;
		INT32 maxX = zone.m_rect.m_pos.m_x + zone.m_rect.m_width;
		INT32 minZ = zone.m_rect.m_pos.m_z - zone.m_rect.m_height;
		INT32 maxZ = zone.m_rect.m_pos.m_z + zone.m_rect.m_height;

		if ((pos.m_x >= minX) && (pos.m_x <= maxX) && (pos.m_z >= minZ) && (pos.m_z <= maxZ)){
			return it->first;	
		}	
	}
	return 0;
}

void CCopyMap::onUserMove(GameUser * pUser)
{
	if (!pUser|| !m_pMapTemplate){
		return ;
	}

	UINT16 id = checkCollideZone(pUser->getPos(),m_pMapTemplate->m_buffZoneDefines);

	for (UINT16 i = 0;i < m_mobaBuffs.size();++i){
		stMobaBuffInstance & buff = m_mobaBuffs[i];

		if ((buff.m_bExist) && (buff.m_id == id) && (buff.m_buffID > 0)){
			CSkill::throwBuff(pUser,pUser,buff.m_buffID);
			buff.m_bExist = false;
			buff.m_buffID = 0;
			buff.m_summonTime = g_gameServer->getSec() + s_SUMMON_BUFF_INTERVAL;

			Map::stNotifyBuffDisappear retCmd;
			retCmd.set_id(buff.m_id);
			sendProto2Map(retCmd);
		}
	}

	id = checkCollideZone(pUser->getPos(),m_pMapTemplate->m_resZoneDefines);
	for (UINT16 i = 0;i < m_mobaReses.size();++i){
		stMobaResInstance & res = m_mobaReses[i];

		if (res.m_bExist && (id == res.m_id) && (pUser->m_beginCollectTime <= 0)){//还没有采集
			//开始采集
			Map::stOpCopyMapRes retCmd;
			retCmd.set_collect(true);
			pUser->sendProto2Me(retCmd);
			pUser->m_beginCollectTime = g_gameServer->getSec();
		}
		else if((id <= 0) && (pUser->m_beginCollectTime > 0)){//正在采集,则取消
			pUser->m_beginCollectTime = 0;
			Map::stOpCopyMapRes retCmd;
			retCmd.set_collect(false);
			pUser->sendProto2Me(retCmd);
		}
	}
}

void CCopyMap::onUserRelive(GameUser * pUser)
{

}

void CCopyMap::onResCollect(GameUser * pUser,UINT16 id)
{
	if (!pUser){
		return ;
	}

	for (UINT16 i = 0;i < m_mobaReses.size();++i){
		stMobaResInstance & res = m_mobaReses[i];
		if (id == res.m_id){
			res.m_bExist = false;
			res.m_summonTime = g_gameServer->getSec() + s_SUMMON_RES_INTERVAL;
		}
	}
}

CGuildTeamMap::CGuildTeamMap()
{
	m_firstKillCharID = 0;	
}

CGuildTeamMap::~CGuildTeamMap()
{

}

UINT32 CGuildTeamMap::getRemainTime()
{
	if (m_openTime + s_GUILD_TEAM_MAP_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_GUILD_TEAM_MAP_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

bool CGuildTeamMap::isTimeOut(UINT64 uTime)
{
	if (CCopyMap::isTimeOut(uTime)){
		return true;
	}

	if (m_openTime + s_GUILD_TEAM_MAP_LASTTIME< uTime){
		return true;
	}
	return false;
}


UINT32 CGuildTeamMap::getOrgHP(UINT32 hp)
{
	hp = hp / TEAM_HERO_HP_COEF;
	return hp;
}

void CGuildTeamMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Role::stFinishLvResult retCmd;
	retCmd.set_result(Role::stFinishLvResult::SUCCESS);
	this->sendProto2Map(retCmd);

	/*Global::stTeamCopyMapFinish cmd;
	  cmd.set_copymapid(m_pCopyMapBase->m_id);
	  TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
	  TObjSet::iterator it = objSet.begin();
	  for (;it != objSet.end();++it){
	  GameUser * pUser =(GameUser *) * it;
	  if (pUser){
	  Global::stFirstKillBaseInfo * pBaseInfo = cmd.add_userinfo();
	  if (pBaseInfo){
	  pBaseInfo->set_charid(pUser->getID());
	  pBaseInfo->set_name(pUser->getName());
	  pBaseInfo->set_job(pUser->getJob());
	  pBaseInfo->set_sex(pUser->getSex());
	  pBaseInfo->set_level(pUser->getLevel());
	  pBaseInfo->set_battlepower(pUser->m_roleInfo.m_battlePower);
	  }
	  pUser->m_levelMgr.giveTeamCopymapReward(m_pCopyMapBase->m_id);
	  }
	  }
	  GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);*/
	stPKCamp & camp = m_campInfos[Map::eCamp_Red];
	Global::stGuildCopymapKillBoss cmd;
	std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
	for (;subIt != camp.m_memberIDs.end();++subIt){
		stPKCampMem & mem = subIt->second;
		Global::stUserDamagePair * pPair = cmd.add_damageinfo();
		if (pPair){
			pPair->set_charid(subIt->first);
			pPair->set_damage(mem.m_damage);
		}
	}
	cmd.set_copymapid(m_pCopyMapBase->m_id);
	cmd.set_killcharid(m_firstKillCharID);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("组队副本结束,已经进行了结算");
#endif
	endGame();
}

void CGuildTeamMap::onNpcDie(CNpc*pNpc,CBeing * pAtter)
{
	if (!pNpc){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	CCopyMap::onNpcDie(pNpc,pAtter);
	if (pNpc->isBoss()){//boss死亡了,要结束
		m_bFinish = true;	
		if (pAtter){
			if (pAtter->theGameUser()){
				m_firstKillCharID = pAtter->getID();
			}
			else {
				m_firstKillCharID = pAtter->getOwnerID();
			}
		}
	}
}

void CGuildTeamMap::onHeroDie(CHero *pHero)
{

}

void CGuildTeamMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}

	pHero->hpMultiCoef(TEAM_HERO_HP_COEF);
}

void CGuildTeamMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}	
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif

}

void CGuildTeamMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}

	pUser->batiMultiCoef(0);
	pUser->hpMultiCoef(1);
}

void CGuildTeamMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}
	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	/*if (getPlayerCount() > 0){//换人执行NPC AI
	  GameUser * pUser1 = getFirstGameUser();	
	  if (pUser1){
	  Map::stNotifyExecAI cmd;
	  cmd.set_charid(pUser1->getID());
	  pUser1->sendProto2Me(cmd);
	  }
	  }*/	
}

void CGuildTeamMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);
}

void CGuildTeamMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	
}

void CGuildTeamMap::doFail()
{
	CCopyMap::doFail();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return ;
	}

	stPKCamp & camp = m_campInfos[Map::eCamp_Red];
	Role::stCopymapFail failCmd;
	failCmd.set_copymapid(getID());
	sendProto2Map(failCmd);

	Global::stGuildCopymapFail cmd;
	cmd.set_copymapid(m_pCopyMapBase->m_id);
	TObjSet::iterator it = m_allObjs[CObj::OBJ_TYPE_MONSTER].begin();
	if (it != m_allObjs[CObj::OBJ_TYPE_MONSTER].end()){
		CNpc * pBoss = (CNpc *)(*it);
		cmd.set_bosshp(pBoss->getHP());
	}

	std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
	for (;subIt != camp.m_memberIDs.end();++subIt){
		stPKCampMem & mem = subIt->second;
		Global::stUserDamagePair * pPair = cmd.add_damageinfo();
		if (pPair){
			pPair->set_charid(subIt->first);
			pPair->set_damage(mem.m_damage);
		}
	}
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

CWorldBossMap::CWorldBossMap()
{

}

CWorldBossMap::~CWorldBossMap()
{

}

UINT32 CWorldBossMap::getRemainTime()
{
	if (m_openTime + s_WORLD_BOSS_MAP_LASTIME > g_gameServer->getMSec()){
		return (m_openTime + s_WORLD_BOSS_MAP_LASTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

bool CWorldBossMap::isTimeOut(UINT64 uTime)
{
	if (CCopyMap::isTimeOut(uTime)){
		return true;
	}

	if (m_openTime + s_WORLD_BOSS_MAP_LASTIME< uTime){
		return true;
	}
	return false;
}


UINT32 CWorldBossMap::getOrgHP(UINT32 hp)
{
	hp = hp / TEAM_HERO_HP_COEF;
	return hp;

}

void CWorldBossMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Role::stFinishLvResult retCmd;
	retCmd.set_result(Role::stFinishLvResult::SUCCESS);
	this->sendProto2Map(retCmd);

	/*Global::stTeamCopyMapFinish cmd;
	  cmd.set_copymapid(m_pCopyMapBase->m_id);*/
	TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
	TObjSet::iterator it = objSet.begin();
	for (;it != objSet.end();++it){
		GameUser * pUser =(GameUser *) * it;
		if (pUser){
			/*Global::stFirstKillBaseInfo * pBaseInfo = cmd.add_userinfo();
			  if (pBaseInfo){
			  pBaseInfo->set_charid(pUser->getID());
			  pBaseInfo->set_name(pUser->getName());
			  pBaseInfo->set_job(pUser->getJob());
			  pBaseInfo->set_sex(pUser->getSex());
			  pBaseInfo->set_level(pUser->getLevel());
			  pBaseInfo->set_battlepower(pUser->m_roleInfo.m_battlePower);
			  }*/
			pUser->m_levelMgr.giveTeamCopymapReward(m_pCopyMapBase->m_id);
		}
	}
	//GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("组队副本结束,已经进行了结算");
#endif
	endGame();
}

void CWorldBossMap::onNpcDie(CNpc * pNpc,CBeing * pAtter)
{
	if (!pNpc){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	CCopyMap::onNpcDie(pNpc,pAtter);
	if (pNpc->isBoss()){//boss死亡了,要结束
		m_bFinish = true;	
		stPKCamp & camp = m_campInfos[Map::eCamp_Red];
		Global::stKillWorldBoss cmd;
		std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
		for (;subIt != camp.m_memberIDs.end();++subIt){
			stPKCampMem & mem = subIt->second;
			Global::stUserDamagePair * pPair = cmd.add_damageinfo();
			if (pPair){
				pPair->set_charid(subIt->first);
				pPair->set_damage(mem.m_damage);
			}
		}
		if (pAtter){
			if (pAtter->theGameUser()){
				cmd.set_killcharid(pAtter->getID());
			}
			else {
				cmd.set_killcharid(pAtter->getOwnerID());
			}
		}
		cmd.set_bossid(pNpc->getID());
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}
}

void CWorldBossMap::onHeroDie(CHero *)
{

}

void CWorldBossMap::onHeroEnter(CHero * pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}

	pHero->hpMultiCoef(TEAM_HERO_HP_COEF);

}

void CWorldBossMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif

}

void CWorldBossMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}

	pUser->batiMultiCoef(0);
	pUser->hpMultiCoef(1);
}

void CWorldBossMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}
	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	/*if (getPlayerCount() > 0){//换人执行NPC AI
	  GameUser * pUser1 = getFirstGameUser();	
	  if (pUser1){
	  Map::stNotifyExecAI cmd;
	  cmd.set_charid(pUser1->getID());
	  pUser1->sendProto2Me(cmd);
	  }
	  }*/

	Global::stUserLeaveWorldBoss cmd;
	cmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void CWorldBossMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);
}

void CWorldBossMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	
}

void CWorldBossMap::doFail()
{
	CCopyMap::doFail();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return ;
	}
	stPKCamp & camp = m_campInfos[Map::eCamp_Red];
	Role::stCopymapFail failCmd;
	failCmd.set_copymapid(getID());
	sendProto2Map(failCmd);

	Global::stWorldBossCopymapFail cmd;
	TObjSet::iterator it = m_allObjs[CObj::OBJ_TYPE_MONSTER].begin();
	if (it != m_allObjs[CObj::OBJ_TYPE_MONSTER].end()){
		CNpc * pBoss = (CNpc *)(*it);
		cmd.set_bosshp(pBoss->getHP());
	}

	std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
	for (;subIt != camp.m_memberIDs.end();++subIt){
		stPKCampMem & mem = subIt->second;
		Global::stUserDamagePair * pPair = cmd.add_damageinfo();
		if (pPair){
			pPair->set_charid(subIt->first);
			pPair->set_damage(mem.m_damage);
		}
	}
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

CRealTimeMap::CRealTimeMap()
{

}

CRealTimeMap::~CRealTimeMap()
{

}

UINT32 CRealTimeMap::getRemainTime()
{
	if (m_openTime + s_REALTIME_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_REALTIME_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

UINT32 CRealTimeMap::getOrgHP(UINT32 hp)
{
	hp = hp / ONE_2_ONE_HERO_HP_COEF;
	return hp;
}

void CRealTimeMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	stRealTimeCopymapNum notify;

	Role::stRealTimePKResult retCmd;
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	ArenaConfig::stRealTimeReward *pRewardWin =  ArenaConfig::getSingleton().getRealTimeReward(true);
	ArenaConfig::stRealTimeReward *pRewardLose =  ArenaConfig::getSingleton().getRealTimeReward(false);

	for (;it != m_campInfos.end();++it){
		stPKCamp & camp =it->second;
		if (camp.m_win){//胜利的一方
			std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
			for (;subIt != camp.m_memberIDs.end();++subIt){
				stPKCampMem & campMem = subIt->second;
				GameUser * pUser = GameUserManager::getSingleton().getUser(campMem.m_charID);
				if (pUser){
					if (g_gameServer->isPubGameServer())
					{
						PubGame::stNotifyRealTimeResult cmd;
						cmd.set_charid(pUser->getNormalCharID());
						cmd.set_win(true);
						cmd.set_type(PubGame::eRealTimePKType_KillAll);
						cmd.set_costlilian(m_pCopyMapBase->m_costLiLian);
						pUser->sendProto2NormalGame(cmd);
					}
					else
					{
						if (pRewardWin){
							pUser->addResource((eResource)pRewardWin->m_itemID,pRewardWin->m_itemNum,"实时战场");
						}
						pUser->addReamTimePKNum(1);
						notify.notify(pUser,1);
						pUser->removeResource(eResource_Lilian,m_pCopyMapBase->m_costLiLian,"实时战场");
						Global::stOperateActivityCb globalCmd;
						globalCmd.set_charid(pUser->getID());
						globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Three_Vs_Three));
						globalCmd.set_num(1);
						GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
					}
				}
				Role::stSynRealTimeItem * pSyn = retCmd.add_sidea();
				if (pSyn){
					pSyn->set_rolename(campMem.m_name);
					pSyn->set_level(campMem.m_level);
					pSyn->set_battlepower(campMem.m_battlePower);
					pSyn->set_killnum(campMem.m_killNum);
					pSyn->set_charid(campMem.m_charID);
					if (pRewardWin){
						pSyn->set_addhonour(pRewardWin->m_itemNum);
					}

					if (camp.m_dieIDs.find(campMem.m_charID) != camp.m_dieIDs.end()){
						pSyn->set_isdie(true);
					}
					else {
						pSyn->set_isdie(false);
					}
				}
			}
		}
		else {//失败的一方
			std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
			for (;subIt != camp.m_memberIDs.end();++subIt){
				stPKCampMem & campMem = subIt->second;
				GameUser * pUser = GameUserManager::getSingleton().getUser(campMem.m_charID);
				if (pUser){
					if (g_gameServer->isPubGameServer())
					{
						PubGame::stNotifyRealTimeResult cmd;
						cmd.set_charid(pUser->getNormalCharID());
						cmd.set_win(false);
						cmd.set_type(PubGame::eRealTimePKType_KillAll);
						cmd.set_costlilian(m_pCopyMapBase->m_costLiLian);
						pUser->sendProto2NormalGame(cmd);
					}
					else
					{
						if (pRewardLose){
							pUser->addResource((eResource)pRewardLose->m_itemID,pRewardLose->m_itemNum,"实时战场");
						}
						pUser->addReamTimePKNum(1);
						notify.notify(pUser,1);
						pUser->removeResource(eResource_Lilian,m_pCopyMapBase->m_costLiLian,"实时战场");
						Global::stOperateActivityCb globalCmd;
						globalCmd.set_charid(pUser->getID());
						globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Three_Vs_Three));
						globalCmd.set_num(1);
						GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
					}
				}

				Role::stSynRealTimeItem * pSyn = retCmd.add_sideb();
				if (pSyn){
					pSyn->set_rolename(campMem.m_name);
					pSyn->set_level(campMem.m_level);
					pSyn->set_battlepower(campMem.m_battlePower);
					pSyn->set_killnum(campMem.m_killNum);
					pSyn->set_charid(campMem.m_charID);
					if (pRewardLose){
						pSyn->set_addhonour(pRewardLose->m_itemNum);
					}

					if (camp.m_dieIDs.find(campMem.m_charID) != camp.m_dieIDs.end()){
						pSyn->set_isdie(true);
					}
					else {
						pSyn->set_isdie(false);
					}
				}
			}
		}
	}
	this->sendProto2Map(retCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("实时副本结束,已经进行了结算");
#endif
	endGame();
}

void CRealTimeMap::onNpcDie(CNpc*,CBeing * pAtter)
{

}

void CRealTimeMap::onHeroDie(CHero *)
{

}

void CRealTimeMap::onHeroEnter(CHero *pHero)
{
	CCopyMap::onHeroEnter(pHero);
	if (!pHero || !m_pCopyMapBase){
		return ;
	}

	pHero->hpMultiCoef(ONE_2_ONE_HERO_HP_COEF);	
}

void CRealTimeMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (camp.m_aliveIDs.size() <= 0){
		m_bFinish = true;
		camp.m_win = false;
		std::map<UINT8,stPKCamp>::iterator subIt = m_campInfos.begin();
		for (;subIt != m_campInfos.end();++subIt){
			if (subIt->second.m_campID != pUser->getCampID()){
				subIt->second.m_win = true;
			}
		}
	}

	if (pAtter){
		UINT32 ownerID = 0;
		if (pAtter->theGameUser()){
			ownerID = pAtter->getUniqueID();	
		}
		else if(pAtter->theHero()){
			ownerID = pAtter->getOwnerID();	
		}

		if (ownerID > 0){
			stPKCamp & atterCamp = m_campInfos[pAtter->getCampID()];
			atterCamp.m_memberIDs[ownerID].m_killNum += 1;
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif
}

void CRealTimeMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}
	pUser->hpMultiCoef(ONE_2_ONE_PLAYER_HP_COEF);	
	pUser->batiMultiCoef(1);
}

void CRealTimeMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}
	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (camp.m_aliveIDs.size() <= 0){
		m_bFinish = true;
		camp.m_win = false;

		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
		for (;it != m_campInfos.end();++it){
			if (pUser->getCampID() != it->second.m_campID){
				it->second.m_win = true;
			}
		}
	}

	if (g_gameServer->isPubGameServer())
	{
		pUser->NotifyNormalOnPub(false);
	}
}

void CRealTimeMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);

}

void CRealTimeMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	
}

void CRealTimeMap::doFail()
{
	CCopyMap::doFail();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Role::stRealTimeFail retCmd;
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	UINT16 i = 0;
	for (;it != m_campInfos.end();++it){
		stPKCamp & camp =it->second;
		std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
		for (;subIt != camp.m_memberIDs.end();++subIt){
			stPKCampMem & campMem = subIt->second;
			Role::stSynRealTimeItem * pSyn = NULL;
			if (0 == i){
				pSyn = retCmd.add_sidea();
			}
			else {
				pSyn = retCmd.add_sideb();
			}
			if (pSyn){
				pSyn->set_charid(campMem.m_charID);
				pSyn->set_rolename(campMem.m_name);
				pSyn->set_level(campMem.m_level);
				pSyn->set_battlepower(campMem.m_battlePower);
				pSyn->set_killnum(campMem.m_killNum);
			}
		}
		++i;
	}
	this->sendProto2Map(retCmd);
}

void CRealTimeMap::doAct(UINT64 uTime)
{
	CCopyMap::doAct(uTime);	
	if (m_openTime + s_REALTIME_LASTTIME < uTime){//超过10分钟就结束,两方都失败
		m_bFail = true;
	}
}

CTeamMap::CTeamMap()
{

}

CTeamMap::~CTeamMap()
{

}

UINT32 CTeamMap::getRemainTime()
{
	if (m_openTime + s_TEAM_MAP_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_TEAM_MAP_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

bool CTeamMap::isTimeOut(UINT64 uTime)
{
	if (CCopyMap::isTimeOut(uTime)){
		return true;
	}

	if (m_openTime + s_TEAM_MAP_LASTTIME< uTime){
		return true;
	}
	return false;
}

UINT32 CTeamMap::getOrgHP(UINT32 hp)
{
	hp = hp / TEAM_HERO_HP_COEF;
	return hp;
}

void CTeamMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Role::stFinishLvResult retCmd;
	retCmd.set_result(Role::stFinishLvResult::SUCCESS);
	this->sendProto2Map(retCmd);

	Global::stTeamCopyMapFinish cmd;
	cmd.set_copymapid(m_pCopyMapBase->m_id);
	TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
	TObjSet::iterator it = objSet.begin();
	for (;it != objSet.end();++it){
		GameUser * pUser =(GameUser *) * it;
		if (pUser){
			stTeamFinishCopymapNum notify;
			notify.notify(pUser,1);

			stFinishCopyMapNotify notify1;
			notify1.notify(pUser,1,m_pCopyMapBase->m_id);

			Global::stFirstKillBaseInfo * pBaseInfo = cmd.add_userinfo();
			if (pBaseInfo){
				pBaseInfo->set_charid(pUser->getID());
				pBaseInfo->set_name(pUser->getName());
				pBaseInfo->set_job(pUser->getJob());
				pBaseInfo->set_sex(pUser->getSex());
				pBaseInfo->set_level(pUser->getLevel());
				pBaseInfo->set_battlepower(pUser->m_roleInfo.m_battlePower);
				pBaseInfo->set_photoid(pUser->m_photoMgr.getUsePhotoID() ? pUser->m_photoMgr.getUsePhotoID() : 1);
			}
			pUser->m_levelMgr.giveTeamCopymapReward(m_pCopyMapBase->m_id);
			pUser->addTeamCopyMapNum(1);
			Global::stOperateActivityCb globalCmd;
			globalCmd.set_charid(pUser->getID());
			globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Group_Map));
			globalCmd.set_num(1);
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);

			Global::stReqCheckTask cmd;                                   
			cmd.set_charid(pUser->getID());  
			cmd.set_squaretype(5);
			cmd.set_battletype(3);                                        
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		}
	}
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("组队副本结束,已经进行了结算");
#endif
	endGame();
}

void CTeamMap::onNpcDie(CNpc* pNpc,CBeing * pAtter)
{
	if (!pNpc){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	CCopyMap::onNpcDie(pNpc,pAtter);
	if (pNpc->isBoss()){//boss死亡了,要结束
		m_bFinish = true;
	}
}

void CTeamMap::onHeroDie(CHero *)
{

}

void CTeamMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(TEAM_HERO_HP_COEF);
}

void CTeamMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}	
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif

}

void CTeamMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}

	pUser->batiMultiCoef(0);
	pUser->hpMultiCoef(1);
}

void CTeamMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	/*if (getPlayerCount() > 0){//换人执行NPC AI
	  GameUser * pUser1 = getFirstGameUser();	
	  if (pUser1){
	  Map::stNotifyExecAI cmd;
	  cmd.set_charid(pUser1->getID());
	  pUser1->sendProto2Me(cmd);
	  }
	  }*/	
}

void CTeamMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);

}

void CTeamMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	

}

void CTeamMap::doFail()
{
	CCopyMap::doFail();
	if (m_isEnd){//已经结束
		return;
	}

	Role::stCopymapFail failCmd;
	failCmd.set_copymapid(getID());
	sendProto2Map(failCmd);
}

void COne2OneMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	ArenaConfig::stOne2OnePVPReward *pWinReward =  ArenaConfig::getSingleton().getOne2OnePVPReward(true);
	ArenaConfig::stOne2OnePVPReward *pLoseReward =  ArenaConfig::getSingleton().getOne2OnePVPReward(false);

	UINT32 addScore = 0;
	UINT32 decScore = 0;

	if (pWinReward && pLoseReward && (!m_pkWithFriend)){
		struct tm _tm;
		RealTime::getLocalTime(_tm,g_gameServer->getSec());
		UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

		if (ArenaConfig::getSingleton().isOne2OneDoubleOpen(secNum)){
			addScore = pWinReward->m_doubleRewardNum;
			decScore = pLoseReward->m_doubleRewardNum;
		}
		else {
			addScore = pWinReward->m_rewardNum;
			decScore = pLoseReward->m_rewardNum;
		}
	}

	Role::stOne2OnePVPResult retCmd;
	retCmd.set_addscore(addScore);
	retCmd.set_delscore(decScore);

	Global::stOne2OnePVPFinish cmd;

	GameUser * pWinUser = NULL;
	GameUser * pLoserUser = NULL;
	PubGame::stNotifyOne2OneResult notifyCmd;
	notifyCmd.set_costlilian(m_pCopyMapBase->m_costLiLian);

	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	for (;it != m_campInfos.end();++it){
		stPKCamp &camp = it->second;

		if (camp.m_win){//胜利的一方
			std::map<UINT32,stPKCampMem>::iterator subIt =   camp.m_memberIDs.begin();
			if (subIt != camp.m_memberIDs.end()) {
				stPKCampMem & campMem = subIt->second;
				retCmd.mutable_winer()->set_name(campMem.m_name);
				retCmd.mutable_winer()->set_level(campMem.m_level);
				retCmd.mutable_winer()->set_battlepower(campMem.m_battlePower);
				//retCmd.mutable_winer()->set_score(campMem.m_score);
				cmd.set_charid1(campMem.m_charID);
				cmd.set_win1(true);
				pWinUser = GameUserManager::getSingleton().getUserOrRobot(campMem.m_charID);
				if (pWinUser && (!m_pkWithFriend)){
					pWinUser->addOne2OnePVPScore(addScore);
					retCmd.mutable_winer()->set_score(pWinUser->getOne2OnePVPScore());
					if (g_gameServer->isPubGameServer())
					{
						//如果跨服，通知原本gs处理
						notifyCmd.set_charid1(pWinUser->getNormalCharID());
						notifyCmd.set_win1(true);
						notifyCmd.set_score1(addScore);
						notifyCmd.set_serveridx1(pWinUser->getNormalServerID());
						// notifyCmd.set_withfriend(m_pkWithFriend);
						// pWinUser->sendProto2NormalGame(notifyCmd);
					}else{	
						cmd.set_score1(pWinUser->getOne2OnePVPScore());
						pWinUser->m_countMgr.addDayRefreshValue(ECOUNT_ONE2ONE_WIN_NUM,1);
						pWinUser->m_sevenDayActivityMgr.addOne2OneWinNum(1);
						Global::stOperateActivityCb globalCmd;
						globalCmd.set_charid(pWinUser->getID());
						globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::One_Vs_One));					
						globalCmd.set_num(1);
						GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
					}
				}
			}
		}
		else {//失败的一方
			std::map<UINT32,stPKCampMem>::iterator subIt =   camp.m_memberIDs.begin();
			if (subIt != camp.m_memberIDs.end()) {
				stPKCampMem & campMem = subIt->second;
				retCmd.mutable_loser()->set_name(campMem.m_name);
				retCmd.mutable_loser()->set_level(campMem.m_level);
				retCmd.mutable_loser()->set_battlepower(campMem.m_battlePower);
				//retCmd.mutable_loser()->set_score(campMem.m_score);
				cmd.set_charid2(campMem.m_charID);
				cmd.set_win2(false);
				pLoserUser = GameUserManager::getSingleton().getUserOrRobot(campMem.m_charID);
				if (pLoserUser && (!m_pkWithFriend)){
					pLoserUser->removeOne2OnePVPScore(decScore);
					retCmd.mutable_loser()->set_score(pLoserUser->getOne2OnePVPScore());
					if (g_gameServer->isPubGameServer())
					{
						//如果跨服，通知原本gs处理
						notifyCmd.set_charid2(pLoserUser->getNormalCharID());
						notifyCmd.set_win2(false);
						notifyCmd.set_score2(decScore);
						notifyCmd.set_serveridx2(pLoserUser->getNormalServerID());
						// notifyCmd.set_withfriend(m_pkWithFriend);
						// pLoserUser->sendProto2NormalGame(notifyCmd);
					}else{	
						cmd.set_score2(pLoserUser->getOne2OnePVPScore());
						Global::stOperateActivityCb globalCmd;
						globalCmd.set_charid(pLoserUser->getID());
						globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::One_Vs_One));					
						globalCmd.set_num(1);
						GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
					}
				}
			}
		}
	}
	cmd.set_pkwithfriend(m_pkWithFriend);
	this->sendProto2Map(retCmd);

	if (g_gameServer->isPubGameServer())
	{	
		notifyCmd.set_withfriend(m_pkWithFriend);
		if (pWinUser)
		{
			pWinUser->sendProto2NormalGame(notifyCmd);
		}
		if (pLoserUser && (pWinUser == NULL || pLoserUser->getNormalServerID() != pWinUser->getNormalServerID()))
		{
			pLoserUser->sendProto2NormalGame(notifyCmd);
		}
	}
	else
	{
		if (!m_pkWithFriend) {
			TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
			TObjSet::iterator subIt = objSet.begin();
			for (;subIt != objSet.end();++subIt){
				GameUser * pUser =(GameUser *) * subIt;
				if (pUser){
					stDianfengPkNum notify;
					notify.notify(pUser,1);
					pUser->removeResource(eResource_Lilian,m_pCopyMapBase->m_costLiLian,"巅峰");
				}
			}
		}
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("巅峰对决结束,出现了胜负,已经进行了结算");
#endif
	endGame();
}

COne2OneMap::COne2OneMap()
{
}

COne2OneMap::~COne2OneMap()
{
}

UINT32 COne2OneMap::getOrgHP(UINT32 hp)
{
	hp = hp / ONE_2_ONE_HERO_HP_COEF;
	return hp;

}

void COne2OneMap::onNpcDie(CNpc*pNpc,CBeing * pAtter)
{

}

void COne2OneMap::onHeroDie(CHero *pHero)
{

}

UINT32 COne2OneMap::getRemainTime()
{
	if (m_openTime +  s_ONE2ONE_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_ONE2ONE_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

void COne2OneMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(ONE_2_ONE_HERO_HP_COEF);		
}

void COne2OneMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pAtter || !pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	m_bFinish = true;
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	for (;it != m_campInfos.end();++it){
		stPKCamp & camp = it->second;
		if (camp.m_campID != pUser->getCampID()){
			camp.m_win = true;
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif

}

void COne2OneMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}
	pUser->hpMultiCoef(ONE_2_ONE_PLAYER_HP_COEF);	
	pUser->batiMultiCoef(1);
}

void COne2OneMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}
	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	m_bFinish = true;	
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();

	for (;it != m_campInfos.end();++it){
		stPKCamp & camp = it->second;
		if (camp.m_campID != pUser->getCampID()){
			camp.m_win = true;
		}
	}

	if (g_gameServer->isPubGameServer())
	{
		pUser->NotifyNormalOnPub(false);
	}
}

void COne2OneMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);

}

void COne2OneMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	

}

void COne2OneMap::doFail()
{
	CCopyMap::doFail();
	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Role::stOne2OnePVPFail retCmd;
	retCmd.set_pkwithfriend(this->m_pkWithFriend);
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	for (;it != m_campInfos.end();++it){
		stPKCamp &camp = it->second;
		std::map<UINT32,stPKCampMem>::iterator subIt =   camp.m_memberIDs.begin();
		if (subIt != camp.m_memberIDs.end()) {
			stPKCampMem & campMem = subIt->second;
			Role::stSynOne2OnePVPItem * pSyn = retCmd.add_loser();
			if (pSyn) {
				pSyn->set_name(campMem.m_name);
				pSyn->set_level(campMem.m_level);
				pSyn->set_battlepower(campMem.m_battlePower);
				pSyn->set_score(campMem.m_score);
				pSyn->set_charid(campMem.m_charID);
			}
		}
	}
	this->sendProto2Map(retCmd);
}

void COne2OneMap::doAct(UINT64 uTime)
{
	CCopyMap::doAct(uTime);	
	if (m_openTime + s_ONE2ONE_LASTTIME < uTime){//超过10分钟就结束,两方都失败
		m_bFail = true;
	}
}

CMobaRealTime::CMobaRealTime()
{
}

CMobaRealTime::~CMobaRealTime()
{
}

UINT32 CMobaRealTime::getOrgHP(UINT32 hp)
{
	hp = hp / ONE_2_ONE_HERO_HP_COEF;
	return hp;

}

void CMobaRealTime::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	stRealTimeCopymapNum notify;
	Role::stMobaPKResult retCmd;
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	ArenaConfig::stRealTimeReward *pRewardWin =  ArenaConfig::getSingleton().getRealTimeReward(true);
	ArenaConfig::stRealTimeReward *pRewardLose =  ArenaConfig::getSingleton().getRealTimeReward(false);

	for (;it != m_campInfos.end();++it){
		stPKCamp & camp =it->second;
		if (camp.m_win){//胜利的一方
			std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
			for (;subIt != camp.m_memberIDs.end();++subIt){
				stPKCampMem & campMem = subIt->second;
				GameUser * pUser = GameUserManager::getSingleton().getUser(campMem.m_charID);
				if (pUser){
					if (g_gameServer->isPubGameServer())
					{
						PubGame::stNotifyRealTimeResult cmd;
						cmd.set_charid(pUser->getNormalCharID());
						cmd.set_win(true);
						cmd.set_type(PubGame::eRealTimePKType_Finish);
						cmd.set_costlilian(m_pCopyMapBase->m_costLiLian);
						pUser->sendProto2NormalGame(cmd);
					}
					else
					{
						if (pRewardWin){
							pUser->addResource((eResource)pRewardWin->m_itemID,pRewardWin->m_itemNum,"积分战场");
						}
						pUser->addReamTimePKNum(1);
						notify.notify(pUser,1);
						pUser->removeResource(eResource_Lilian,m_pCopyMapBase->m_costLiLian,"积分战场");
						Global::stOperateActivityCb globalCmd;
						globalCmd.set_charid(pUser->getID());
						globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Three_Vs_Three));
						globalCmd.set_num(1);
						GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
					}
				}
				Role::stSynMobaItem * pSyn = retCmd.add_sidea();
				if (pSyn){
					pSyn->set_rolename(campMem.m_name);
					pSyn->set_level(campMem.m_level);
					pSyn->set_battlepower(campMem.m_battlePower);
					pSyn->set_killnum(campMem.m_killNum);
					pSyn->set_charid(campMem.m_charID);
					if (pRewardWin){
						pSyn->set_addhonour(pRewardWin->m_itemNum);
					}
					pSyn->set_bekillnum(campMem.m_beKillNum);
				}
			}
		}
		else {//失败的一方
			std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
			for (;subIt != camp.m_memberIDs.end();++subIt){
				stPKCampMem & campMem = subIt->second;
				GameUser * pUser = GameUserManager::getSingleton().getUser(campMem.m_charID);
				if (pUser){
					if (g_gameServer->isPubGameServer())
					{
						PubGame::stNotifyRealTimeResult cmd;
						cmd.set_charid(pUser->getNormalCharID());
						cmd.set_win(false);
						cmd.set_type(PubGame::eRealTimePKType_Finish);
						cmd.set_costlilian(m_pCopyMapBase->m_costLiLian);
						pUser->sendProto2NormalGame(cmd);
					}
					else
					{
						if (pRewardLose){
							pUser->addResource((eResource)pRewardLose->m_itemID,pRewardLose->m_itemNum,"积分战场");
						}
						pUser->addReamTimePKNum(1);
						notify.notify(pUser,1);
						pUser->removeResource(eResource_Lilian,m_pCopyMapBase->m_costLiLian,"积分战场");
						Global::stOperateActivityCb globalCmd;
						globalCmd.set_charid(pUser->getID());
						globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Three_Vs_Three));
						globalCmd.set_num(1);
						GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
					}
				}

				Role::stSynMobaItem * pSyn = retCmd.add_sideb();
				if (pSyn){
					pSyn->set_rolename(campMem.m_name);
					pSyn->set_level(campMem.m_level);
					pSyn->set_battlepower(campMem.m_battlePower);
					pSyn->set_killnum(campMem.m_killNum);
					pSyn->set_charid(campMem.m_charID);
					if (pRewardLose){
						pSyn->set_addhonour(pRewardLose->m_itemNum);
					}
					pSyn->set_bekillnum(campMem.m_beKillNum);
				}
			}
		}
	}
	this->sendProto2Map(retCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("实时副本结束,已经进行了结算");
#endif
	endGame();
}

void CMobaRealTime::onNpcDie(CNpc* pNpc,CBeing * pAtter)
{

}

void CMobaRealTime::onHeroDie(CHero *pHero)
{}

void CMobaRealTime::onHeroEnter(CHero *pHero)
{
	CCopyMap::onHeroEnter(pHero);
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(ONE_2_ONE_HERO_HP_COEF);	
}

void CMobaRealTime::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());
	camp.m_memberIDs[pUser->getID()].m_beKillNum += 1;

	if (pAtter){
		UINT32 ownerID = 0;
		if (pAtter->theGameUser()){
			ownerID = pAtter->getUniqueID();	
		}
		else if(pAtter->theHero()){
			ownerID = pAtter->getOwnerID();	
		}

		if (ownerID > 0){
			stPKCamp & atterCamp = m_campInfos[pAtter->getCampID()];
			atterCamp.m_memberIDs[ownerID].m_killNum += 1;
			if (atterCamp.m_memberIDs[ownerID].m_killNum <= 1){//首次击杀
				addCampScore(pAtter->getCampID(),13);	
			}
			else {
				addCampScore(pAtter->getCampID(),3);
			}
		}
	}

	if (pUser->m_beginCollectTime > 0){
		pUser->m_beginCollectTime = 0;
		Map::stOpCopyMapRes retCmd;
		retCmd.set_collect(false);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("玩家(name:%s,id:%d)死亡了，采集被取消",pUser->getName(),pUser->getID());
#endif
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif
}

void CMobaRealTime::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}
	pUser->hpMultiCoef(ONE_2_ONE_PLAYER_HP_COEF);	
	pUser->batiMultiCoef(1);

}

void CMobaRealTime::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}
	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (camp.m_aliveIDs.size() <= 0){
		m_bFinish = true;
		camp.m_win = false;

		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
		for (;it != m_campInfos.end();++it){
			if (pUser->getCampID() != it->second.m_campID){
				it->second.m_win = true;
			}
		}
	}	

	if (g_gameServer->isPubGameServer())
	{
		pUser->NotifyNormalOnPub(false);
	}
}

void CMobaRealTime::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);

}

void CMobaRealTime::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	

}

UINT32 CMobaRealTime::getRemainTime()
{
	if (m_openTime + s_BATTLE_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_BATTLE_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}


void CMobaRealTime::doAct(UINT64 uTime)
{
	CCopyMap::doAct(uTime);	
	if (m_openTime + s_BATTLE_LASTTIME < uTime){//超过10分钟就结束,谁的分数多谁赢
		m_bFinish = true;
		if (m_campInfos.size() >= 2){
			std::map<UINT8,stPKCamp>::iterator it1 =  m_campInfos.begin();	
			std::map<UINT8,stPKCamp>::iterator it2 =  ++it1;

			if ((it1 != m_campInfos.end()) && (it2 != m_campInfos.end())){
				if (it1->second.m_score >= it2->second.m_score){
					it1->second.m_win = true;					
				}		
				else {
					it2->second.m_win = true;
				}
			}		
		}
	}
}

void CMobaRealTime::onUserRelive(GameUser * pUser)
{
	if (!pUser){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.erase(pUser->getID());
	camp.m_aliveIDs.insert(pUser->getID());

	onUserMove(pUser);
}

void CMobaRealTime::doFail()
{
	CCopyMap::doFail();
}

CManorFightMap::CManorFightMap()
{
	m_manorID = 0;
	m_formation = 0;
	m_teamAID = 0;
	m_teamBID = 0;
}

CManorFightMap::~CManorFightMap()
{}

UINT32 CManorFightMap::getRemainTime()
{
	if (m_openTime + s_MANOR_FIGHT_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_MANOR_FIGHT_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

UINT32 CManorFightMap::getOrgHP(UINT32 hp)
{
	hp = hp / ONE_2_ONE_HERO_HP_COEF;
	return hp;
}

void CManorFightMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Global::stFinishManorFight cmd;
	cmd.set_manorid(m_manorID);
	cmd.set_fomation(m_formation);
	cmd.set_bfinish(true);
	cmd.set_bfail(false);
	cmd.set_teamaid(m_teamAID);	
	cmd.set_teambid(m_teamBID);

	GuildProto::stManorFightPKResult retCmd;
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();

	for (;it != m_campInfos.end();++it){
		stPKCamp & camp =it->second;
		if (camp.m_win){//胜利的一方
			std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
			for (;subIt != camp.m_memberIDs.end();++subIt){
				stPKCampMem & campMem = subIt->second;
				GuildProto::stSynManorFightItem * pSyn = retCmd.add_sidea();
				if (pSyn){
					pSyn->set_rolename(campMem.m_name);
					pSyn->set_level(campMem.m_level);
					pSyn->set_battlepower(campMem.m_battlePower);
					pSyn->set_killnum(campMem.m_killNum);

					if (camp.m_dieIDs.find(campMem.m_charID) != camp.m_dieIDs.end()){
						pSyn->set_isdie(true);
					}
					else {
						pSyn->set_isdie(false);
					}
				}

				Global::stManorFightBattleProp * pCmdSyn = cmd.add_userinfo();
				if (pCmdSyn){
					GameUser * pUser = GameUserManager::getSingleton().getUser(campMem.m_charID);
					if (pUser){
						pCmdSyn->set_charid(pUser->getID());
						pCmdSyn->set_curhp(pUser->getHP() / ONE_2_ONE_PLAYER_HP_COEF);
					}
					else {
						pCmdSyn->set_charid(pUser->getID());
						pCmdSyn->set_curhp(0);
					}
#ifdef _HDZ_DEBUG
					Athena::logger->trace("领地战决出胜负后同步胜利玩家:%d,血量:%d",pCmdSyn->charid(),pCmdSyn->curhp());
#endif
				}
			}
		}
		else {//失败的一方
			std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
			for (;subIt != camp.m_memberIDs.end();++subIt){
				stPKCampMem & campMem = subIt->second;

				GuildProto::stSynManorFightItem * pSyn = retCmd.add_sideb();
				if (pSyn){
					pSyn->set_rolename(campMem.m_name);
					pSyn->set_level(campMem.m_level);
					pSyn->set_battlepower(campMem.m_battlePower);
					pSyn->set_killnum(campMem.m_killNum);

					if (camp.m_dieIDs.find(campMem.m_charID) != camp.m_dieIDs.end()){
						pSyn->set_isdie(true);
					}
					else {
						pSyn->set_isdie(false);
					}
				}

				Global::stManorFightBattleProp * pCmdSyn = cmd.add_userinfo();
				if (pCmdSyn){
					pCmdSyn->set_charid(campMem.m_charID);
					pCmdSyn->set_curhp(0);
#ifdef _HDZ_DEBUG
					Athena::logger->trace("领地战决出胜负后同步失败玩家:%d,血量:%d",pCmdSyn->charid(),pCmdSyn->curhp());
#endif
				}
			}
		}
	}
	this->sendProto2Map(retCmd);

	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("领地战结束了分出胜负:manorID:%d,formation:%d,teamA:%d,teamB:%d",m_manorID,m_formation,m_teamAID,m_teamBID);
#endif
	endGame();
}

void CManorFightMap::onNpcDie(CNpc*,CBeing * pAtter)
{

}

void CManorFightMap::onHeroDie(CHero *)
{

}

void CManorFightMap::onHeroEnter(CHero *pHero)
{
	CCopyMap::onHeroEnter(pHero);
	if (!pHero || !m_pCopyMapBase){
		return ;
	}

	pHero->hpMultiCoef(ONE_2_ONE_HERO_HP_COEF);	
}

void  CManorFightMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (camp.m_aliveIDs.size() <= 0){
		m_bFinish = true;
		camp.m_win = false;
		std::map<UINT8,stPKCamp>::iterator subIt = m_campInfos.begin();
		for (;subIt != m_campInfos.end();++subIt){
			if (subIt->second.m_campID != pUser->getCampID()){
				subIt->second.m_win = true;
			}
		}
	}

	if (pAtter){
		UINT32 ownerID = 0;
		if (pAtter->theGameUser()){
			ownerID = pAtter->getUniqueID();	
		}
		else if(pAtter->theHero()){
			ownerID = pAtter->getOwnerID();	
		}

		if (ownerID > 0){
			stPKCamp & atterCamp = m_campInfos[pAtter->getCampID()];
			atterCamp.m_memberIDs[ownerID].m_killNum += 1;
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif
}

void CManorFightMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}
	pUser->hpMutilCoefInManor(ONE_2_ONE_PLAYER_HP_COEF);	
	pUser->batiMultiCoef(1);
}

void CManorFightMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}
	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (camp.m_aliveIDs.size() <= 0){
		m_bFinish = true;
		camp.m_win = false;

		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
		for (;it != m_campInfos.end();++it){
			if (pUser->getCampID() != it->second.m_campID){
				it->second.m_win = true;
			}
		}
	}

	std::map<UINT32,stPKCampMem>::iterator it = camp.m_memberIDs.find(pUser->getID());
	if (it != camp.m_memberIDs.end()){
		it->second.m_bLeave = true;	
	}
}

void CManorFightMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);

}

void CManorFightMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	
}

void CManorFightMap::doFail()
{
	CCopyMap::doFail();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Global::stFinishManorFight cmd;
	GuildProto::stManorFightPKFail retCmd;
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	UINT16 i = 0;
	for (;it != m_campInfos.end();++it){
		stPKCamp & camp =it->second;
		std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
		for (;subIt != camp.m_memberIDs.end();++subIt){
			stPKCampMem & campMem = subIt->second;
			GuildProto::stSynManorFightItem * pSyn = NULL;
			if (0 == i){
				pSyn = retCmd.add_sidea();
			}
			else {
				pSyn = retCmd.add_sideb();
			}
			if (pSyn){
				pSyn->set_rolename(campMem.m_name);
				pSyn->set_level(campMem.m_level);
				pSyn->set_battlepower(campMem.m_battlePower);
				pSyn->set_killnum(campMem.m_killNum);
			}

			Global::stManorFightBattleProp * pCmdSyn = cmd.add_userinfo();
			if (pCmdSyn){
				GameUser * pUser = GameUserManager::getSingleton().getUser(campMem.m_charID);
				if (pUser){
					pCmdSyn->set_charid(pUser->getID());
					pCmdSyn->set_curhp(pUser->getHP() / ONE_2_ONE_PLAYER_HP_COEF);
				}
				else {
					pCmdSyn->set_charid(pUser->getID());
					pCmdSyn->set_curhp(0);
				}

				if (campMem.m_bLeave){//提前离开的
					pCmdSyn->set_curhp(0);
				}
#ifdef _HDZ_DEBUG
				Athena::logger->trace("领地战平手后同步玩家:%d,血量:%d",pCmdSyn->charid(),pCmdSyn->curhp());
#endif
			}
		}
		++i;
	}
	this->sendProto2Map(retCmd);

	cmd.set_manorid(m_manorID);
	cmd.set_fomation(m_formation);
	cmd.set_teamaid(m_teamAID);
	cmd.set_teambid(m_teamBID);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("领地战结束了未分出胜负:manorID:%d,formation:%d,teamA:%d,teamB:%d",m_manorID,m_formation,m_teamAID,m_teamBID);
#endif
}

void CManorFightMap::doAct(UINT64 uTime)
{
	CCopyMap::doAct(uTime);	
	if (m_openTime + s_MANOR_FIGHT_LASTTIME < uTime){//超过5分钟就结束,两方都失败
		m_bFail = true;
	}
}

CEmeraldDreamMap::CEmeraldDreamMap()
{

}

CEmeraldDreamMap::~CEmeraldDreamMap()
{

}

UINT32 CEmeraldDreamMap::getRemainTime()
{
	if (m_openTime + s_EMERALD_DREAM_MAP_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_EMERALD_DREAM_MAP_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

bool CEmeraldDreamMap::isTimeOut(UINT64 uTime)
{
	if (CCopyMap::isTimeOut(uTime)){
		return true;
	}

	if(m_openTime + s_EMERALD_DREAM_MAP_LASTTIME < uTime){
		return true;
	}
	return false;
}

UINT32 CEmeraldDreamMap::getOrgHP(UINT32 hp)
{
	hp = hp / EMERALD_DREAM_HERO_HP_COEF;
	return hp;
}

void CEmeraldDreamMap::doFinish()
{
	CCopyMap::doFinish();

	if(m_isEnd){//已经结束
		return ;
	}

	if(!m_pCopyMapBase){
		return ;
	}

	//	Role::stFinishLvResult retCmd;
	//	retCmd.set_result(Role::stFinishLvResult::SUCCESS);
	//	this->sendProto2Map(retCmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("翡翠梦境结束,已经进行了结算");
#endif
	endGame();

}

void CEmeraldDreamMap::onNpcDie(CNpc * pNpc,CBeing * pAtter)
{
	if(!pNpc){
		return ;
	}

	if(m_isEnd){
		return ;
	}

	CCopyMap::onNpcDie(pNpc,pAtter);
	if (pNpc->isBoss()){//boss死亡了,要结束
		m_bFinish = true;
		stPKCamp & camp = m_campInfos[Map::eCamp_Red];         
		Global::stKillEmeraldDreamBoss cmd; 
		std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
		for (;subIt != camp.m_memberIDs.end();++subIt){
			stPKCampMem & mem = subIt->second;
			Global::stUserDamagePair * pPair = cmd.add_damageinfo();
			if (pPair){
				pPair->set_charid(subIt->first);
				pPair->set_damage(mem.m_damage);
			}
		}

		cmd.set_bossid(pNpc->getID());
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}
}

void CEmeraldDreamMap::onHeroDie(CHero *)
{

}

void CEmeraldDreamMap::onHeroEnter(CHero * pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}

	pHero->hpMultiCoef(EMERALD_DREAM_HERO_HP_COEF);
}

void CEmeraldDreamMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second;
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif
}

void CEmeraldDreamMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}

	pUser->batiMultiCoef(0);
	pUser->hpMultiCoef(1);
}

void CEmeraldDreamMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);

	if (!pUser){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];

	camp.m_dieIDs.insert(pUser->getID());

	camp.m_aliveIDs.erase(pUser->getID());
}

void CEmeraldDreamMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);
}

void CEmeraldDreamMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);
}

void CEmeraldDreamMap::doFail()
{
	CCopyMap::doFail();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return ;
	}

	stPKCamp & camp = m_campInfos[Map::eCamp_Red];
	Global::stKillEmeraldDreamBoss cmd;
	std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
	for (;subIt != camp.m_memberIDs.end();++subIt){
		stPKCampMem & mem = subIt->second;
		Global::stUserDamagePair * pPair = cmd.add_damageinfo();
		if (pPair){
			pPair->set_charid(subIt->first);
			pPair->set_damage(mem.m_damage);
		}
	}

	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	//	stPKCamp & camp = m_campInfos[Map::eCamp_Red];
	//	Role::stCopymapFail failCmd;
	//	failCmd.set_copymapid(getID());
	//	sendProto2Map(failCmd);
	/*
	   Global::stEmeraldDreamCopymapFail cmd;
	   TObjSet::iterator it = m_allObjs[CObj::OBJ_TYPE_MONSTER].begin();
	   if (it != m_allObjs[CObj::OBJ_TYPE_MONSTER].end()){
	   CNpc * pBoss = (CNpc *)(*it);
	   cmd.set_bosshp(pBoss->getHP());
	   }

	   std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
	   for (;subIt != camp.m_memberIDs.end();++subIt){
	   stPKCampMem & mem = subIt->second;
	   Global::stUserDamagePair * pPair = cmd.add_damageinfo();
	   if (pPair){
	   pPair->set_charid(subIt->first);
	   pPair->set_damage(mem.m_damage);
	   }
	   }
	   GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	   */
}


void CHeroLeagueMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	struct tm _tm;                                                      
	RealTime::getLocalTime(_tm,g_gameServer->getSec());                 
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;  
	                                                                    
	//星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */ 
	UINT16 wday = _tm.tm_wday;                                          
	secNum = secNum + wday * 86400 ;


	Role::stOne2OnePVPResult retCmd;
	retCmd.set_addscore(0);
	retCmd.set_delscore(0);

	GameUser * pWinUser = NULL;
	GameUser * pLoserUser = NULL;
	PubGame::stNotifyHeroLeagueResult notifyCmd;

	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	for (;it != m_campInfos.end();++it){
		stPKCamp &camp = it->second;
		
		CHeroLeagueMgr mgr = CHeroLeagueMgr::getSingleton();

		if (camp.m_win){//胜利的一方
			std::map<UINT32,stPKCampMem>::iterator subIt =   camp.m_memberIDs.begin();
			if (subIt != camp.m_memberIDs.end()) {
				stPKCampMem & campMem = subIt->second;
				retCmd.mutable_winer()->set_name(campMem.m_name);
				retCmd.mutable_winer()->set_level(campMem.m_level);
				retCmd.mutable_winer()->set_battlepower(campMem.m_battlePower);
				pWinUser = GameUserManager::getSingleton().getUserOrRobot(campMem.m_charID);
				if (pWinUser && (!m_pkWithFriend)){
					
					if (g_gameServer->isPubGameServer())
					{
						//time  334800-338399   初赛时间段
						CHeroLeagueCfg cfg = CHeroLeagueCfg::getSingleton();
						if(cfg.isPreOpen(secNum)|| cfg.isRemOpen(secNum)){
							//获得初赛配置
							stPreliminary preliminary = cfg.getPreConfiguration();
							UINT8 victoryPoint = 0;

							PubGame::stPubRoleInfo *info = notifyCmd.mutable_user1();  
							if(info){
								info->set_charid(pWinUser->getNormalCharID());
								info->set_pubcharid(pWinUser->getID());
								info->set_areacode(pWinUser->getAreaCode());
								info->set_name(pWinUser->getName());
								info->set_level(pWinUser->getLevel());
								info->set_viplv(pWinUser->getVipLv());
								info->set_job(pWinUser->getJob());
								info->set_sex(pWinUser->getSex());
								info->set_battlepower(pWinUser->getBattlePower());
								info->set_pubserverindex(pWinUser->getNormalServerID());
								info->set_photoid(pWinUser->m_photoMgr.getUsePhotoID());
								if(cfg.isRemOpen(secNum)){
									stIntermediary inter = cfg.getIntermediary(); 
									info->set_score(inter.m_victoryPoints);
									victoryPoint = inter.m_victoryPoints;
								}else{
									info->set_score(preliminary.m_victoryPoints);
									victoryPoint = preliminary.m_victoryPoints; 
								}
							}
#ifdef _HDZ_DEBUG
							Athena::logger->trace("[英雄联赛](name:%s, charID:%d, pubCharID:%d)战斗胜利，增加%d点积分!",pWinUser->getName(), pWinUser->getNormalCharID(), pWinUser->getID(),victoryPoint);
#endif
						}else if(/*决赛时间*/CHeroLeagueCfg::getSingleton().isFinalFightOpen(secNum)){
							PubGame::stPubRoleInfo *info = notifyCmd.mutable_user1();
							if(info){                                             
								info->set_charid(pWinUser->getNormalCharID());                
								info->set_pubcharid(pWinUser->getID());          
								info->set_areacode(pWinUser->getAreaCode());            
								info->set_name(pWinUser->getName());                    
								info->set_level(pWinUser->getLevel());                  
								info->set_viplv(pWinUser->getVipLv());                  
								info->set_job(pWinUser->getJob());                      
								info->set_sex(pWinUser->getSex());                      
								info->set_battlepower(pWinUser->getBattlePower());      
								info->set_pubserverindex(pWinUser->getNormalServerID());
								info->set_winorloss(true);
								info->set_photoid(pWinUser->m_photoMgr.getUsePhotoID());
#ifdef HDZ_DEBUG
								Athena::logger->trace("[英雄联赛 copyMap info win] charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d,winorloss:%d", pWinUser->getNormalCharID(), pWinUser->getID(), pWinUser->getAreaCode(), pWinUser->getName(), pWinUser->getLevel(), pWinUser->getVipLv(), pWinUser->getJob(), pWinUser->getSex(), pWinUser->getBattlePower(), pWinUser->getNormalServerID(), 1);
#endif
							}

							if(mgr.getUserNum() == 2){
								PubGame::stReqSynChampionInfo synCmd;
								synCmd.mutable_info()->set_charid(pWinUser->getID());
								synCmd.mutable_info()->set_name(pWinUser->getName());
								synCmd.mutable_info()->set_areacode(pWinUser->getAreaCode());
								synCmd.mutable_info()->set_level(pWinUser->getLevel());
								synCmd.mutable_info()->set_job(pWinUser->getJob());
								synCmd.mutable_info()->set_sex(pWinUser->getSex());
								synCmd.mutable_info()->set_leader(true);
								synCmd.mutable_info()->set_battlepower(pWinUser->getBattlePower());
								synCmd.mutable_info()->set_pos(0);
								synCmd.mutable_info()->set_effectid(pWinUser->getEffectID());
								synCmd.mutable_info()->set_wingbreaklv(pWinUser->m_wingBreakLv);
								synCmd.mutable_info()->set_photoid(pWinUser->m_photoMgr.getUsePhotoID());

								CItem *pHead = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(HEAD_SLOT_INDEX);
								if (pHead){
									synCmd.mutable_info()->set_headid(pHead->getID()); 
								}

								CItem *pBody = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(BODY_SLOT_INDEX);
								if (pBody){
									synCmd.mutable_info()->set_bodyid(pBody->getID());
								}
							
								CItem * pWeaponid = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(WEAPON_SLOT_INDEX);
								if (pWeaponid){
							    	synCmd.mutable_info()->set_weaponid(pWeaponid->getID());
								}

								CItem * pCloackid = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(CLOAK_SLOT_INDEX);
								if (pCloackid){
									synCmd.mutable_info()->set_cloackid(pCloackid->getID());
								}

								CItem * pWing = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(WING_SLOT_INDEX);
								if (pWing){
									synCmd.mutable_info()->set_wingid(pWing->getID());
								}

								CItem * pShoe = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(SHOE_SLOT_INDEX);
								if (pShoe){
									synCmd.mutable_info()->set_shoeid(pShoe->getID());
								}

								CItem * pFashionHead = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_HEAD_SLOT_INDEX);
								if (pFashionHead){
									synCmd.mutable_info()->set_fashionhair(pFashionHead->getID());
								}

								CItem * pFashionFace = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_FACE_SLOT_INDEX);
								if (pFashionFace){
									synCmd.mutable_info()->set_fashionface(pFashionFace->getID());
								}

								CItem * pFashionBody = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_BODY_SLOT_INDEX);
								if (pFashionBody){
									synCmd.mutable_info()->set_fashionbody(pFashionBody->getID());
								}

								CItem * pFashionWeapon = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_WEAPON_SLOT_INDEX);
								if (pFashionWeapon){
									synCmd.mutable_info()->set_fashionweapon(pFashionWeapon->getID());
								}

								CItem * pFashionShoe = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_SHOE_SLOT_INDEX);
								if (pFashionShoe){
									synCmd.mutable_info()->set_fashionshoe(pFashionShoe->getID());
								}

								CItem * pFashionDecoration = pWinUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_RING_SLOT_INDEX);
								if (pFashionDecoration){
									synCmd.mutable_info()->set_fashiondecoration(pFashionDecoration->getID());
								}
								g_gameServer->getConnMgr().broadcastByType(GAMESERVER,synCmd);

							}

						}else{}
					}else{	
						pWinUser->m_countMgr.addDayRefreshValue(ECOUNT_ONE2ONE_WIN_NUM,1);
						pWinUser->m_sevenDayActivityMgr.addOne2OneWinNum(1);
						Global::stOperateActivityCb globalCmd;
						globalCmd.set_charid(pWinUser->getID());
						globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::One_Vs_One));					
						globalCmd.set_num(1);
						GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
					}
				}
			}
		}
		else {//失败的一方
			std::map<UINT32,stPKCampMem>::iterator subIt =   camp.m_memberIDs.begin();
			if (subIt != camp.m_memberIDs.end()) {
				stPKCampMem & campMem = subIt->second;
				retCmd.mutable_loser()->set_name(campMem.m_name);
				retCmd.mutable_loser()->set_level(campMem.m_level);
				retCmd.mutable_loser()->set_battlepower(campMem.m_battlePower);
				pLoserUser = GameUserManager::getSingleton().getUserOrRobot(campMem.m_charID);
				if (pLoserUser && (!m_pkWithFriend)){
					if (g_gameServer->isPubGameServer())
					{
						//获得初赛配置
						CHeroLeagueCfg cfg = CHeroLeagueCfg::getSingleton();       
						if(cfg.isPreOpen(secNum)|| cfg.isRemOpen(secNum)){         

							PubGame::stPubRoleInfo *info = notifyCmd.mutable_user2();             
							if(info){                                                        
								info->set_charid(pLoserUser->getNormalCharID());               
								info->set_pubcharid(pLoserUser->getID());                      
								info->set_areacode(pLoserUser->getAreaCode());                 
								info->set_name(pLoserUser->getName());                         
								info->set_level(pLoserUser->getLevel());                       
								info->set_viplv(pLoserUser->getVipLv());                       
								info->set_job(pLoserUser->getJob());                           
								info->set_sex(pLoserUser->getSex());                           
								info->set_battlepower(pLoserUser->getBattlePower());           
								info->set_pubserverindex(pLoserUser->getNormalServerID());     
								info->set_score(0);      
								info->set_photoid(pLoserUser->m_photoMgr.getUsePhotoID());
							}                                                                
						}//决赛时间
						else if(CHeroLeagueCfg::getSingleton().isFinalFightOpen(secNum)){
							PubGame::stPubRoleInfo *info = notifyCmd.mutable_user2();              
							if(info){                                                         
								info->set_charid(pLoserUser->getNormalCharID());                
								info->set_pubcharid(pLoserUser->getID());                       
								info->set_areacode(pLoserUser->getAreaCode());                  
								info->set_name(pLoserUser->getName());                          
								info->set_level(pLoserUser->getLevel());                        
								info->set_viplv(pLoserUser->getVipLv());                        
								info->set_job(pLoserUser->getJob());                            
								info->set_sex(pLoserUser->getSex());                            
								info->set_battlepower(pLoserUser->getBattlePower());            
								info->set_pubserverindex(pLoserUser->getNormalServerID());      
								info->set_winorloss(false);                             
								info->set_photoid(pLoserUser->m_photoMgr.getUsePhotoID());
#ifdef HDZ_DEBUG
								Athena::logger->trace("[英雄联赛 copyMap info loss] charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d,winorloss:%d", pLoserUser->getNormalCharID(), pLoserUser->getID(), pLoserUser->getAreaCode(), pLoserUser->getName(), pLoserUser->getLevel(), pLoserUser->getVipLv(), pLoserUser->getJob(), pLoserUser->getSex(), pLoserUser->getBattlePower(), pLoserUser->getNormalServerID(), 0);
#endif
							}                                                                 
						}else{

						}
					}else{	
						Global::stOperateActivityCb globalCmd;
						globalCmd.set_charid(pLoserUser->getID());
						globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::One_Vs_One));					
						globalCmd.set_num(1);
						GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
					}
				}
			}
		}
	}
	this->sendProto2Map(retCmd);

	if (g_gameServer->isPubGameServer())
	{	
		g_gameServer->getConnMgr().broadcastByType(GAMESERVER,notifyCmd);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("英雄联赛结束,出现了胜负,已经进行了结算");
#endif
	endGame();
}

CHeroLeagueMap::CHeroLeagueMap()
{
}

CHeroLeagueMap::~CHeroLeagueMap()
{
}

UINT32 CHeroLeagueMap::getOrgHP(UINT32 hp)
{
	hp = hp / HERO_LEAGUE_HERO_HP_COEF;
	return hp;

}

void CHeroLeagueMap::onNpcDie(CNpc*pNpc,CBeing * pAtter)
{

}

void CHeroLeagueMap::onHeroDie(CHero *pHero)
{

}

UINT32 CHeroLeagueMap::getRemainTime()
{
	if (m_openTime +  s_HERO_LEAGUE_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_HERO_LEAGUE_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

void CHeroLeagueMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(HERO_LEAGUE_HERO_HP_COEF);		
}

void CHeroLeagueMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pAtter || !pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	m_bFinish = true;
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	for (;it != m_campInfos.end();++it){
		stPKCamp & camp = it->second;
		if (camp.m_campID != pUser->getCampID()){
			camp.m_win = true;
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif

}

void CHeroLeagueMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}
	pUser->hpMultiCoef(HERO_LEAGUE_PLAYER_HP_COEF);	
	pUser->batiMultiCoef(1);
}

void CHeroLeagueMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}
	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	m_bFinish = true;	
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();

	for (;it != m_campInfos.end();++it){
		stPKCamp & camp = it->second;
		if (camp.m_campID != pUser->getCampID()){
			camp.m_win = true;
		}
	}

	if (g_gameServer->isPubGameServer())
	{
		pUser->NotifyNormalOnPub(false);
	}
}

void CHeroLeagueMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);

}

void CHeroLeagueMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	

}

void CHeroLeagueMap::doFail()
{
	CCopyMap::doFail();
	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Role::stOne2OnePVPFail retCmd;
	retCmd.set_pkwithfriend(this->m_pkWithFriend);
	std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();
	for (;it != m_campInfos.end();++it){
		stPKCamp &camp = it->second;
		std::map<UINT32,stPKCampMem>::iterator subIt =   camp.m_memberIDs.begin();
		if (subIt != camp.m_memberIDs.end()) {
			stPKCampMem & campMem = subIt->second;
			Role::stSynOne2OnePVPItem * pSyn = retCmd.add_loser();
			if (pSyn) {
				pSyn->set_name(campMem.m_name);
				pSyn->set_level(campMem.m_level);
				pSyn->set_battlepower(campMem.m_battlePower);
				pSyn->set_score(campMem.m_score);
				pSyn->set_charid(campMem.m_charID);
			}
		}
	}
	this->sendProto2Map(retCmd);
}

void CHeroLeagueMap::doAct(UINT64 uTime)
{
	CCopyMap::doAct(uTime);	
	if (m_openTime + s_HERO_LEAGUE_LASTTIME < uTime){//超过10分钟就结束,两方都失败
		m_bFail = true;
	}
}


CArenaMap::CArenaMap()
{
	m_pRobot = NULL;
}

CArenaMap::~CArenaMap()
{
	SAFE_DELETE(m_pRobot);
}

UINT32 CArenaMap::getOrgHP(UINT32 hp)
{
	hp = hp / ARENA_HP_HERO_COEF;
	return hp;

}

void CArenaMap::onNpcDie(CNpc*pNpc,CBeing * pAtter)
{

}

void CArenaMap::onHeroDie(CHero *pHero)
{

}

UINT32 CArenaMap::getRemainTime()
{
	if (m_openTime +  s_ARENA_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_ARENA_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

void CArenaMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(ARENA_HP_HERO_COEF);		
}

void CArenaMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pAtter || !pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	m_bFinish = true;	
	bool bWin = true;
	if (!pUser->m_bRobot)	//玩家死亡，失败结束
	{
		bWin = false;
	}

    GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
    if (pOwner)
    {
        pOwner->m_levelMgr.finishArenaNew(m_pCopyMapBase->m_id, bWin, false);
    }

}

void CArenaMap::onUserEnter(GameUser * pUser)
{
    CCopyMap::onUserEnter(pUser);
    if (!pUser || !m_pCopyMapBase){
        return ;
    }
    pUser->hpMultiCoef(ARENA_HP_COEF);	
    pUser->batiMultiCoef(1);

    if (pUser->m_bRobot)
    {
        m_pRobot = pUser;
    }
    else
    {
        if (m_ownerID == 0)
		{
			m_ownerID = pUser->getID();
		}
	}
}

void CArenaMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}

	if (!pUser->m_bRobot)
	{
		pUser->m_levelMgr.setInLvFlag(false);
	}
	pUser->hpMultiCoef(1);	
}

void CArenaMap::doFail()
{
	CCopyMap::doFail();
	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
	if (pOwner)
	{
		pOwner->m_levelMgr.finishArenaNew(m_pCopyMapBase->m_id,false, true);
	}

}

void CArenaMap::doAct(UINT64 uTime)
{
	CCopyMap::doAct(uTime);	
	if (m_openTime + s_ARENA_LASTTIME < uTime){//超过10分钟就结束,两方都失败
		m_bFail = true;
	}
}


void CArenaMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}


#ifdef _HDZ_DEBUG
	Athena::logger->trace("天梯对决结束,出现了胜负,已经进行了结算");
#endif
	endGame();
}

CBeing* CArenaMap::getBeing(UINT8 type,UINT32 moID)
{
	if (CObj::OBJ_TYPE_PLAYER == type)
	{
		if (m_pRobot != NULL && moID == m_pRobot->getID())
		{
			return m_pRobot;
		}
	}
	return NULL;
}


CInterceptMap::CInterceptMap()
{
	m_pRobot = NULL;
	m_buffCnt = 0;
}

CInterceptMap::~CInterceptMap()
{
	SAFE_DELETE(m_pRobot);
}

UINT32 CInterceptMap::getOrgHP(UINT32 hp)
{
	hp = hp / INTERCEPT_HP_HERO_COEF;
	return hp;

}

void CInterceptMap::onNpcDie(CNpc*pNpc,CBeing * pAtter)
{

}

void CInterceptMap::onHeroDie(CHero *pHero)
{

}

UINT32 CInterceptMap::getRemainTime()
{
	if (m_openTime +  s_INTERCEPT_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_INTERCEPT_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

void CInterceptMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(INTERCEPT_HP_HERO_COEF);		
}

void CInterceptMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pAtter || !pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	m_bFinish = true;	
	bool bWin = true;
	if (!pUser->m_bRobot)	//玩家死亡，失败结束
	{
		bWin = false;
	}

	GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
	if (pOwner)
	{
		pOwner->OnInterceptEnd(bWin, false);
	}

}

void CInterceptMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}

	pUser->batiMultiCoef(1);

	if (pUser->m_bRobot)
	{
		m_pRobot = pUser;
		pUser->hpMultiCoef(INTERCEPT_HP_COEF * (1+0.2*m_buffCnt));	
		pUser->setDamagePer(20*m_buffCnt);
	}
	else
	{
		if (m_ownerID == 0)
		{
			m_ownerID = pUser->getID();
		}
		pUser->hpMultiCoef(INTERCEPT_HP_COEF);	
	}
}

void CInterceptMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}

	if (!pUser->m_bRobot)
	{
		pUser->m_levelMgr.setInLvFlag(false);
	}
	if (!m_bFinish)	
	{
		GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
		if (pOwner)
		{
			pOwner->OnInterceptEnd(false, false);
		}
	}
	pUser->hpMultiCoef(1);	
	pUser->setDamagePer(0);
}

void CInterceptMap::doFail()
{
	CCopyMap::doFail();
	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
	if (pOwner)
	{
		pOwner->OnInterceptEnd(false, true);
	}

}

void CInterceptMap::doAct(UINT64 uTime)
{
	CCopyMap::doAct(uTime);	
	if (m_openTime + s_INTERCEPT_LASTTIME < uTime){//超过10分钟就结束,两方都失败
		m_bFail = true;
	}
}


void CInterceptMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}


#ifdef _HDZ_DEBUG
	Athena::logger->trace("拦截结束,出现了胜负,已经进行了结算");
#endif
	endGame();
}

CBeing* CInterceptMap::getBeing(UINT8 type,UINT32 moID)
{
	if (CObj::OBJ_TYPE_PLAYER == type)
	{
		if (m_pRobot != NULL && moID == m_pRobot->getID())
		{
			return m_pRobot;
		}
	}
	return NULL;
}



CLeitaiMap::CLeitaiMap()
{
	m_pRobot = NULL;
}

CLeitaiMap::~CLeitaiMap()
{
	SAFE_DELETE(m_pRobot);
}

UINT32 CLeitaiMap::getOrgHP(UINT32 hp)
{
	hp = hp / LEITAI_HP_HERO_COEF;
	return hp;

}

void CLeitaiMap::onNpcDie(CNpc*pNpc,CBeing * pAtter)
{

}

void CLeitaiMap::onHeroDie(CHero *pHero)
{

}

UINT32 CLeitaiMap::getRemainTime()
{
	if (m_openTime +  s_LEITAI_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_LEITAI_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

void CLeitaiMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(LEITAI_HP_HERO_COEF);
	if (pHero->getOwnerID() == m_ownerID)
	{
		GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
		if (pOwner)
		{
			INT32 hp = pOwner->m_leitaiMgr.getHeroHP(pHero->getID());
			if (hp >= 0)
			{
				pHero->setHP(hp * LEITAI_HP_HERO_COEF);
			}
		}
	}
}

UINT32 CLeitaiMap::getUseTime()
{
	return (g_gameServer->getMSec() - m_realStartTime) / 1000;
}

void CLeitaiMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pAtter || !pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	m_bFinish = true;	
	bool bWin = true;
	if (!pUser->m_bRobot)	//玩家死亡，失败结束
	{
		bWin = false;
	}

	GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
	if (pOwner)
	{
		pOwner->OnleitaiResult(false, getUseTime(), LEITAI_HP_COEF);
	}

}

void CLeitaiMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}
	pUser->hpMultiCoef(LEITAI_HP_COEF);	
	pUser->batiMultiCoef(1);

	if (pUser->m_bRobot)
	{
		m_pRobot = pUser;
	}
	else
	{
		if (m_ownerID == 0)
		{
			m_ownerID = pUser->getID();
		}

		pUser->m_heroMgr.useLeitaiBattleHero();

		UINT32 hp = pUser->m_leitaiMgr.getUserHP() * LEITAI_HP_COEF;
		pUser->forceSetHP(hp);
		CSkill::throwBuff(pUser,pUser,pUser->m_leitaiMgr.getBuffID());
	}
}

void CLeitaiMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}

	if (!pUser->m_bRobot)
	{
		pUser->m_levelMgr.setInLvFlag(false);
		pUser->m_heroMgr.restoreLeitaiBattleHero();
	}
	pUser->hpMultiCoef(1);	
}

void CLeitaiMap::doFail()
{
	CCopyMap::doFail();
	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
	if (pOwner)
	{
		pOwner->OnleitaiResult(true, getUseTime(), LEITAI_HP_COEF);
	}

}

void CLeitaiMap::doAct(UINT64 uTime)
{
	CCopyMap::doAct(uTime);	
	if (m_openTime + s_LEITAI_LASTTIME < uTime){//超过10分钟就结束,两方都失败
		m_bFail = true;
	}
}


void CLeitaiMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}


#ifdef _HDZ_DEBUG
	Athena::logger->trace("擂台对决结束,出现了胜负,已经进行了结算");
#endif
	endGame();
}

CBeing* CLeitaiMap::getBeing(UINT8 type,UINT32 moID)
{
	if (CObj::OBJ_TYPE_PLAYER == type)
	{
		if (m_pRobot != NULL && moID == m_pRobot->getID())
		{
			return m_pRobot;
		}
	}
	return NULL;
}

CDragonNestMap::CDragonNestMap()
{

}

CDragonNestMap::~CDragonNestMap()
{

}

UINT32 CDragonNestMap::getRemainTime()
{
	if (m_openTime + s_DRAGON_NEST_MAP_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_DRAGON_NEST_MAP_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

bool CDragonNestMap::isTimeOut(UINT64 uTime)
{
	if (CCopyMap::isTimeOut(uTime)){
		return true;
	}

	if (m_openTime + s_DRAGON_NEST_MAP_LASTTIME< uTime){
		return true;
	}
	return false;
}

UINT32 CDragonNestMap::getOrgHP(UINT32 hp)
{
	hp = hp / DRAGON_NEST_HERO_HP_COEF;
	return hp;
}

void CDragonNestMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Role::stFinishLvResult retCmd;
	retCmd.set_result(Role::stFinishLvResult::SUCCESS);
	this->sendProto2Map(retCmd);

    stPKCamp & camp = m_campInfos[Map::eCamp_Red];         
    std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
    for (;subIt != camp.m_memberIDs.end();++subIt){
        GameUser * pUser = GameUserManager::getSingleton().getUser(subIt->first);
        if (pUser){
            pUser->m_dragonNest.addUsedTimes();
			pUser->m_levelMgr.giveTeamCopymapReward(m_pCopyMapBase->m_id);
        }
    }

	endGame();
}

void CDragonNestMap::onNpcDie(CNpc* pNpc,CBeing * pAtter)
{
	if (!pNpc){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	CCopyMap::onNpcDie(pNpc,pAtter);
	if (pNpc->isBoss()){//boss死亡了,要结束
		m_bFinish = true;
	}
}

void CDragonNestMap::onHeroDie(CHero *)
{

}

void CDragonNestMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(DRAGON_NEST_HERO_HP_COEF);
}

void CDragonNestMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}	
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif

}

void CDragonNestMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}

	pUser->batiMultiCoef(0);
	pUser->hpMultiCoef(1);
    if (!pUser->m_bRobot && !m_ownerID) {
        m_ownerID = pUser->getID();
	}
}

void CDragonNestMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());
}

void CDragonNestMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);

}

void CDragonNestMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	

}

void CDragonNestMap::doFail()
{
	CCopyMap::doFail();
	if (m_isEnd){//已经结束
		return;
	}

	Role::stCopymapFail failCmd;
	failCmd.set_copymapid(getID());
	sendProto2Map(failCmd);
}

CStoneCopyMap::CStoneCopyMap()
{

}

CStoneCopyMap::~CStoneCopyMap()
{

}

UINT32 CStoneCopyMap::getRemainTime()
{
	if (m_openTime + s_STONE_COPYMAP_MAP_LASTTIME > g_gameServer->getMSec()){
		return (m_openTime + s_STONE_COPYMAP_MAP_LASTTIME - g_gameServer->getMSec()) / 1000;
	}
	else {
		return 0;
	}
}

bool CStoneCopyMap::isTimeOut(UINT64 uTime)
{
	if (CCopyMap::isTimeOut(uTime)){
		return true;
	}

	if (m_openTime + s_STONE_COPYMAP_MAP_LASTTIME < uTime){
		return true;
	}
	return false;
}

UINT32 CStoneCopyMap::getOrgHP(UINT32 hp)
{
	hp = hp / STONE_COPYMAP_HERO_HP_COEF;
	return hp;
}

void CStoneCopyMap::doFinish()
{
	CCopyMap::doFinish();

	if (m_isEnd){//已经结束
		return;
	}

	if (!m_pCopyMapBase){
		return;
	}

	Role::stFinishLvResult retCmd;
	retCmd.set_result(Role::stFinishLvResult::SUCCESS);
	this->sendProto2Map(retCmd);

    stPKCamp & camp = m_campInfos[Map::eCamp_Red];         
    std::map<UINT32,stPKCampMem>::iterator subIt = camp.m_memberIDs.begin();
    for (;subIt != camp.m_memberIDs.end();++subIt){
        GameUser * pUser = GameUserManager::getSingleton().getUser(subIt->first);
        if (pUser){
            pUser->m_stoneCopyMap.addUsedTimes();
			pUser->m_levelMgr.giveTeamCopymapReward(m_pCopyMapBase->m_id);
        }
    }

	endGame();
}

void CStoneCopyMap::onNpcDie(CNpc* pNpc,CBeing * pAtter)
{
	if (!pNpc){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	CCopyMap::onNpcDie(pNpc,pAtter);
	if (pNpc->isBoss()){//boss死亡了,要结束
		m_bFinish = true;
	}
}

void CStoneCopyMap::onHeroDie(CHero *)
{

}

void CStoneCopyMap::onHeroEnter(CHero *pHero)
{
	if (!pHero || !m_pCopyMapBase){
		return ;
	}
	pHero->hpMultiCoef(STONE_COPYMAP_HERO_HP_COEF);
}

void CStoneCopyMap::onPlayerDie(CBeing * pAtter,GameUser * pUser)
{
	CCopyMap::onPlayerDie(pAtter,pUser);
	if (!pUser){
		return ;
	}

	if (m_isEnd){
		return ;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}	
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家:(%s,%d)死亡了,各个阵营目前信息如下:",pUser->getName(),pUser->getID());
		std::map<UINT8,stPKCamp>::iterator it = m_campInfos.begin();   
		for (;it != m_campInfos.end();++it){
			stPKCamp & camp = it->second; 
			Athena::logger->trace("campID:%d,liveNum:%d,dieNum:%d",camp.m_campID,camp.m_aliveIDs.size(),camp.m_dieIDs.size());
		}
	}
#endif

}

void CStoneCopyMap::onUserEnter(GameUser * pUser)
{
	CCopyMap::onUserEnter(pUser);
	if (!pUser || !m_pCopyMapBase){
		return ;
	}

	pUser->batiMultiCoef(0);
	pUser->hpMultiCoef(1);
    if (!pUser->m_bRobot && !m_ownerID) {
        m_ownerID = pUser->getID();
	}
}

void CStoneCopyMap::onUserLeave(GameUser * pUser)
{
	CCopyMap::onUserLeave(pUser);
	if (!pUser){
		return ;
	}

	if (checkIsAllDie(CObj::OBJ_TYPE_PLAYER)){//所有人都死亡
		m_bFail = true;
	}

	stPKCamp & camp = m_campInfos[pUser->getCampID()];
	camp.m_dieIDs.insert(pUser->getID());
	camp.m_aliveIDs.erase(pUser->getID());
}

void CStoneCopyMap::onNpcDecHP(CBeing * pAtter,UINT32 decHP)
{
	CCopyMap::onNpcDecHP(pAtter,decHP);

}

void CStoneCopyMap::onUserMove(GameUser * pUser)
{
	CCopyMap::onUserMove(pUser);	

}

void CStoneCopyMap::doFail()
{
	CCopyMap::doFail();
	if (m_isEnd){//已经结束
		return;
	}

	Role::stCopymapFail failCmd;
	failCmd.set_copymapid(getID());
	sendProto2Map(failCmd);
}
