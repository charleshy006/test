#include "MapModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "MapMgr.h"
#include "CopyMap.h"
#include "Hero.h"
#include "Npc.h"
#include "Skill.h"
#include "Obj.h"
#include "DropMgr.h"
#include "HeroMgr.h"
#include "MarryMgr.h"

extern GameServer * g_gameServer;

MapModHandle::MapModHandle()
{
	registerHandle(&MapModHandle::reqEnterLevelMap);
	registerHandle(&MapModHandle::reqLeaveLevelMap);
	registerHandle(&MapModHandle::reqMove);
	registerHandle(&MapModHandle::reqAttack);
	registerHandle(&MapModHandle::clientFinishLoadRes);
	registerHandle(&MapModHandle::interruptPlaySkill);
	registerHandle(&MapModHandle::changeDir);
	registerHandle(&MapModHandle::attackHit);
	registerHandle(&MapModHandle::changeHeroGroup);
	registerHandle(&MapModHandle::sendMainCityOnlineUser);
	registerHandle(&MapModHandle::changeMainCity);
	registerHandle(&MapModHandle::calcThunderDamage);
}

MapModHandle::~MapModHandle()
{

}

bool MapModHandle::handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<MapModHandle,GameUser>::handle(pUser,pData,cmdLen,funID); 
}

void MapModHandle::reqEnterLevelMap(GameUser * pUser,Map::ReqEnterLevelMap * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_levelMgr.enterLv(recvCmd->levelid());
}

void MapModHandle::clientFinishLoadRes(GameUser * pUser,Map::stClientLoadResFinish * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	//如果在打跨服,转发消息
    if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
    {
        pUser->sendProto2PubGame(*recvCmd);
        return;
    }


	if (!pUser->getMap() || !pUser->getMap()->isCopyMap()){
		return ;
	}

	((CCopyMap*)pUser->getMap())->resLoadFinish(pUser);
}

void MapModHandle::reqMove(GameUser * pUser,Map::ReqMove * recvCmd)
{
    //Athena::logger->trace("(name:%s, id:%d)****************移动****************", pUser->getName(), pUser->getID());
	if (!pUser || !recvCmd) {
		return ;
	}
	//如果在打跨服,转发消息
	if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}

	if (!pUser->getMap()){
		return ;
	}
	
	CBeing * pBeing = CBeing::getBeing(recvCmd->type(),recvCmd->moid(),pUser->getMap());

	if (!pBeing){
		// Athena::logger->trace("请求移动时候找不到对象 user[%d]",pUser->getID());
		return ;
	}

	CPos tarPos;
	tarPos.m_x = recvCmd->path().x();
	tarPos.m_y = recvCmd->path().y();
	tarPos.m_z = recvCmd->path().z();
	
	Map::stInstantMove failCmd;
	failCmd.set_type(recvCmd->type());
	failCmd.set_moid(recvCmd->moid());
	failCmd.mutable_destpos()->set_x(recvCmd->path().x());
	failCmd.mutable_destpos()->set_y(recvCmd->path().y());
	failCmd.mutable_destpos()->set_z(recvCmd->path().z());

	if (pBeing->isSetState(Map::BEING_STATE_ICE) || pBeing->isSetState(Map::BEING_STATE_DIZZY)){
		// Athena::logger->error("玩家(name:%s,id:%d)遭受了技能的不可移动状态,不能移动",pBeing->getName(),pBeing->getUniqueID());
		pUser->sendProto2Me(failCmd);
		return ;
	}

	if (pBeing->move(tarPos,recvCmd->isinstant())){
		if (pBeing == pUser){
			if (pUser->getMap() && pUser->getMap()->isCopyMap()){
				((CCopyMap *)pUser->getMap())->onUserMove(pUser);
			} else if (pUser->getMap()->getTempID() == 5){
                MarryMgr::getSingleton().synMove(pUser);
            }
		}
		//Athena::logger->trace("玩家移动到了x=%d,y=%d,z=%d",recvCmd->path().x(),recvCmd->path().y(),recvCmd->path().z());	
	}
	else {
		pUser->sendProto2Me(failCmd);
	}
}

void MapModHandle::reqAttack(GameUser *pUser,Map::stThrowSkill * recvCmd)
{
	if (!pUser || !recvCmd) {
		return ;
	}
	//如果在打跨服,转发消息
	if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}
	
	CBeing * pBeing = CBeing::getBeing(recvCmd->atttype(),recvCmd->attmoid(),pUser->getMap());
	
	Map::stThrowSkillFail failCmd;

	if (!pBeing){
		failCmd.set_reason(Map::stThrowSkillFail::ATTER_NOT_EXIST);
		pUser->sendProto2Me(failCmd);
		// Athena::logger->error("(name:%s,id:%d)请求攻击时候找不到对象(moType:%d,moID:%d)!",pUser->getName(),pUser->getID(),recvCmd->atttype(),recvCmd->attmoid());
		return ;
	}
	
	if (pBeing->isSetState(Map::BEING_STATE_SILENCE)){//遭手了沉默
		failCmd.set_reason(Map::stThrowSkillFail::ATTER_HAS_SILENCE);
		pUser->sendProto2Me(failCmd);
		return ;
	}

	CSkill * pSkill = pBeing->getSkillByID(recvCmd->skill());
	if (!pSkill){
		failCmd.set_reason(Map::stThrowSkillFail::ATTER_SKILL_NOT_EXIST);
		pUser->sendProto2Me(failCmd);
		// Athena::logger->error("(name:%s,id:%d)请求攻击时候(pAtter:%s,%d)找不到技能:%d!",pUser->getName(),pUser->getID(),pBeing->getName(),pBeing->getID(),recvCmd->skill());
		return ;
	}
	
	if (!pSkill->checkCost(pBeing)){//检查消耗
		failCmd.set_reason(Map::stThrowSkillFail::ATTER_MP_NOT_ENOUGH);
		pUser->sendProto2Me(failCmd);
		// Athena::logger->error("(name:%s,id:%d)请求攻击时候扣除魔法失败!(skillid=%d)",pUser->getName(),pUser->getID(),recvCmd->skill());
		return ;
	}
	
	if (pSkill->isCoolDown(pBeing->getRaiseSkillValue(pSkill->m_pSkillBase->m_skillID, eRaiseSkill_ReduceCD))){
        pBeing->setCoolDown(true);
		// Athena::logger->error("(name:%s,id:%d)还处于冷却时间,却释放了技能(skillid=%d)!",pUser->getName(),pUser->getID(),recvCmd->skill());
		return ;
	} else {
        pBeing->setCoolDown(false);
    }
	
	pSkill->setLastUseTime();
	pBeing->m_skillCDMgr.throwSkillSuccess(recvCmd->skill(),pSkill->getLastTime(),pSkill->getMaxHitNum());

	Map::stThrowSkill retCmd;
	retCmd.set_attmoid(recvCmd->attmoid());
	retCmd.set_atttype(recvCmd->atttype());
	retCmd.set_skill(recvCmd->skill());
	retCmd.set_step(recvCmd->step());
	//todo 检查技能释放的合法性
	if (pUser->getMap()){//广播播放魔法
		pUser->getMap()->sendProto2Map(retCmd,pUser->getID());
	}	
}

void MapModHandle::reqLeaveLevelMap(GameUser * pUser,Map::stReqLeaveLevel * recvCmd)
{
	if (!pUser || !recvCmd) {
		return ;
	}
	//如果在打跨服,转发消息
	if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}

	Map::stLeaveLevelSuccess retCmd;
	pUser->sendProto2Me(retCmd);
	pUser->m_levelMgr.setInLvFlag(false);
	Map::stNotifyEnterMapInfo loadSceneCmd;
	
	if (pUser->getMap() && pUser->getMap()->isCopyMap()){//如果在副本
		UINT32 toMapID = pUser->m_lastIntoMapID;
		CMap *pNewMap = CMapMgr::getSingleton().getMapByID(toMapID);
		if (!pNewMap){
			toMapID = NEW_PERSON_MAPID;
		}
		else if (pNewMap && pNewMap->isCopyMap() &&(!((CCopyMap *)pNewMap)->isGuildHall())){//请求离开的不是公会
			toMapID = NEW_PERSON_MAPID;
		}

		pNewMap = CMapMgr::getSingleton().getMapByID(toMapID);
		if (pNewMap){
			CMapMgr::getSingleton().changeMap(pUser,pNewMap);
		}
		else {
			Athena::logger->error("(name:%s,id:%d,从副本里出来时候,找不到要去的关卡:%d)",pUser->getName(),pUser->getID(),toMapID);
		}
	}
	else if(pUser->getMap()){
		loadSceneCmd.set_mapid(pUser->getMap()->getID());
		pUser->sendProto2Me(loadSceneCmd);
	}
	else {
		Athena::logger->error("(name:%s,id:%d)请求离开战役失败,找不到要去哪里!",pUser->getName(),pUser->getID());
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("account=%s,name=%s请求离开关卡成功",pUser->getAccount(),pUser->getName());
#endif
}

void MapModHandle::interruptPlaySkill(GameUser * pUser,Map::stInterruptPlaySkill * recvCmd)
{
	if (!pUser || !recvCmd) {
		return ;
	}
	//如果在打跨服,转发消息
	if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}

	CBeing * pBeing = CBeing::getBeing(recvCmd->atttype(),recvCmd->attid(),pUser->getMap());

	if (!pBeing){
		// Athena::logger->trace("请求打断技能播放时候找不到对象!");
		return ;
	}
	
	pBeing->interruptSkillPlay(recvCmd->nextaction());
}

void MapModHandle::changeDir(GameUser * pUser,Map::stMapObjectChgDir * recvCmd)
{
	if (!pUser || !recvCmd) {
		return ;
	}
	//如果在打跨服,转发消息
	if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}
	CBeing * pBeing = CBeing::getBeing(recvCmd->motype(),recvCmd->moid(),pUser->getMap());
	if (!pBeing){
		// Athena::logger->error("请求改变朝向时候找不到对象! user[%d]",pUser->getID());
		return ;
	}
	
	pBeing->changeDir(recvCmd->dir());
}

bool g_specialCheckSkillID(UINT32 skillID)
{
	switch (skillID){
		case 8116115 :
		case 8116075 :
		case 8116215 :
		case 8116185 :
		case 8116155 :
		case 8116165 :
		case 8116125 :
		case 8116095 :
		case 8116065 :
		case 8116045 :
			{
				return true;	
			}
		break;
	}
	return false;
}

void MapModHandle::attackHit(GameUser * pUser,Map::stNotifyHit * recvCmd)
{
	if (!pUser || !recvCmd) {
		return ;
	}
	//如果在打跨服,转发消息
	if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}
	
	CBeing * pAtter = CBeing::getBeing(recvCmd->atttype(),recvCmd->attmoid(),pUser->getMap());
	
	if (!pAtter){
		Athena::logger->error("技能通知碰撞时候找不到攻击对象,(modType:%d,moID:%d) user[%d]!",recvCmd->atttype(),recvCmd->attmoid(),pUser->getID());
		return ;
	}
	
	if (pAtter->isDie()){//已经死亡了还发送计算伤害协议
#ifdef _HDZ_DEBUG
		Athena::logger->error("攻击者已经死亡了还发送计算伤害协议过来");
#endif
		return ;
	}

    if (pAtter->isCoolDown()) {
		// Athena::logger->error("Map::stNotifyHit cd 中, 拒绝回应");
		return ;
    }
	
	pAtter->m_skillCDMgr.incHitNum();

	Map::AttackResult retCmd;
	retCmd.set_atttype(pAtter->getType());
	retCmd.set_attmoid(pAtter->getUniqueID());
	retCmd.set_step(recvCmd->step());
	UINT32 recoverHP = pAtter->getRecoverHP();
	bool   bAbsorbHP = false;

	for (UINT8 i = 0;i < recvCmd->hitinfos_size(); ++i){
		const Map::stHitInfo & hitInfo = recvCmd->hitinfos(i);
		
		if (!pAtter->isRobot()){//非机器人才检测
			if (!pAtter->m_skillCDMgr.hitValid(hitInfo.skillid())){
#ifdef _HDZ_DEBUG
				Athena::logger->error("(name:%s,id:%d)技能已经超过最大碰撞次数了,还发送攻击请求过来",pAtter->getName(),pAtter->getID());
#endif
				continue;
			}
		}

		CBeing * pDefer = NULL;
		CSkill * pSkill = pAtter->getSkillByID(hitInfo.skillid());
		if (pSkill){
			/*if (g_specialCheckSkillID(hitInfo.skillid())){
				if (pSkill->isCoolDown()){
					Athena::logger->error("atter(%s,%d)已经冷却是了还试图发送协议过来,玩家(name:%s,id:%d)可能使用了挂",pAtter->getName(),pAtter->getUniqueID(),pUser->getName(),pUser->getID());
					return;
				}
			}*/
			//pSkill->setLastUseTime();
			pDefer = CBeing::getBeing(hitInfo.motype(),hitInfo.moid(),pUser->getMap());
			if (!pDefer){
#ifdef _HDZ_DEBUG
				Athena::logger->error("通知技能碰撞时候找不到防御对象(defType:%d,defModID:%d)",hitInfo.motype(),hitInfo.moid());
#endif
				continue;
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到技能碰撞的通知:atter(%s,%d),defer(%s,%d),技能:%d",pAtter->getName(),pAtter->getUniqueID(),pDefer->getName(),pDefer->getUniqueID(),hitInfo.skillid());
#endif
			std::pair<UINT32,UINT32> result = pSkill->doAction(pAtter,pDefer,recvCmd->step());
			retCmd.set_damagetime(hitInfo.damagetime());
			if (!pSkill->isCalcDam()){//不需要计算伤害
				continue;
			}
			
			if (pSkill->isNormal() && (!bAbsorbHP)){//普通攻击要回血
				bAbsorbHP = true;
				recoverHP += (UINT32)(result.first * (pAtter->getAbsorbHPPer() / 100.0f));
			}

			Map::AttRetInfo * pRetInfo = retCmd.add_attlist();
			if (pRetInfo){
				pRetInfo->set_dechp(result.first);
				pRetInfo->set_type(hitInfo.motype());
				pRetInfo->set_moid(hitInfo.moid());
				pRetInfo->set_skill(hitInfo.skillid());
				pRetInfo->set_result(result.second);
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("地图对象moType=%d,moID=%d受到的伤害为%d",hitInfo.motype(),hitInfo.moid(),result.first);
#endif
		}
		else {
			Athena::logger->error("通知技能碰撞时候找不到攻击者(modType:%d,moID:%d)的技能ID:%d",recvCmd->atttype(),recvCmd->attmoid(),hitInfo.skillid());
		}
	}

	if (pAtter->getMap()){
		pAtter->getMap()->sendProto2Map(retCmd);
	}

	if (pAtter->isPlayer() &&bAbsorbHP &&(recoverHP > 0)){
		pAtter->addHP(recoverHP);	
	}
}

void MapModHandle::sendMainCityOnlineUser(GameUser * pUser,Map::stReqOnlineUserOnMainCity * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUserManager::getSingleton().sendMainCityUser(pUser);
}

void MapModHandle::changeHeroGroup(GameUser * pUser,Map::stChangeHeroGroup * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	//如果在打跨服,转发消息
	if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}
	
	Map::stChangeHeroGroupError errorCmd;
	if (pUser->m_heroMgr.isColdDown()){
		errorCmd.set_result(Map::stChangeHeroGroupError::eCHG_HERO_COLD_DOWN);
		pUser->sendProto2Me(errorCmd);
		return ;
	}
	
	if (recvCmd->groupid() == pUser->m_heroMgr.getCurGroupID()){
		errorCmd.set_result(Map::stChangeHeroGroupError::eCUR_GROUP_HAS_GOOUT);
		pUser->sendProto2Me(errorCmd);
		return ;
	}
	
	if (pUser->isDie()){
		Athena::logger->error("已经死亡了不能再召唤!");
		return ;
	}

	CHeroMgr::getSingleton().clearHeroByOwerID(pUser->getID());
	pUser->m_heroMgr.changeHero(recvCmd->groupid());
	pUser->m_heroMgr.setColdDown();
}

void MapModHandle::changeMainCity(GameUser * pUser,Map::stReqChangeMainCity * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CMap * pNewMap = CMapMgr::getSingleton().getMapByID(recvCmd->levelid());

	if (!pNewMap){
		Athena::logger->error("[切换主城]找不到主城id:%d",recvCmd->levelid());
		return ;
	}
	CMapMgr::getSingleton().changeMap(pUser,pNewMap);
}

void MapModHandle::calcThunderDamage(GameUser * pUser,Map::stCalcThunderDamage * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	//如果在打跨服,转发消息
	if (!g_gameServer->isPubGameServer() && pUser->isOnPub())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}
	
	Map::stDirectDamge cmd;
	
	CBeing * pAtter = NULL;
	CBeing * pDefer = NULL;
	
	pAtter = CBeing::getBeing(recvCmd->attmodtype(),recvCmd->attmodid(),pUser->getMap());
	pDefer = CBeing::getBeing(recvCmd->deftype(),recvCmd->defmodid(),pUser->getMap());

	if (!pAtter || !pDefer){
		return ;	
	}
	
	INT32 dam = 0;
	pAtter->calcPKData();
	pDefer->calcPKData();
	
	dam = (INT32)((pAtter->getPKCalcData().m_thunderDamage * 3 * g_randBetween(3,6) - pDefer->getPKCalcData().m_refThunderDamage) * (g_randBetween(95,105) / 100.0f));
	
	dam = std::max(dam,1);

	pDefer->decHP(dam,pAtter);
	cmd.set_atttype(pAtter->getType());
	cmd.set_attmodid(pAtter->getUniqueID());
	cmd.set_modtype(pDefer->getType());
	cmd.set_modid(pDefer->getUniqueID());
	cmd.set_damage(dam);
	cmd.set_result(Map::eAttResult_Thunder_Damage);
	pAtter->getMap()->sendProto2Map(cmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name:%s,id:%d收到了雷电伤害:%d",pDefer->getName(),pDefer->getUniqueID(),dam);
#endif
}
