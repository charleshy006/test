#include "Skill.h"
#include "Being.h"
#include "SkillInfo.h"
#include "TblDataMgr.h"
#include "GameUser.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "GameServer.h"
#include "Map.h"
#include "GameUserManager.h"
#include "CopyMap.h"
#include "Role.pb.h"
#include <tinyxml/tinyxml.h>

extern GameServer * g_gameServer;

void stFuncAddHP::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pBuff || !pAtter || !pDefer){
		return ;
	}
	if (pBuff->m_paramsVec.size() < 4){
		return ;
	}
	
	UINT32 addHP = 0;
	if (0 == pBuff->m_paramsVec[2]){
		addHP = pBuff->m_paramsVec[3];
	}
	else {
		addHP = (UINT32)(pDefer->getMaxHP() * (pBuff->m_paramsVec[3] / 100.0f));
	}
	addHP = addHP == 0 ? 1 : addHP;
	pDefer->addHP(addHP);
}

void stFuncAddHP::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncAddHP::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	
}

void stFuncSilence::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pBuff || (pBuff->m_paramsVec.size() <= 0) || !pAtter || !pDefer){
		return ;
	}
	
	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_SILENCE,pBuff->m_paramsVec[0],(UINT32)-1,pBuff);
}

void stFuncSilence::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncSilence::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
		
}

void stFuncSummonHero::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter ||!pDefer||!pBuff){
		return ;
	}
	if (pBuff->m_paramsVec.size() < 4){
		return ;
	} 

	UINT32 lastTime = pBuff->m_paramsVec[0] / 1000;
	UINT32 heroID = pBuff->m_paramsVec[1];
	UINT32 hpPer = pBuff->m_paramsVec[2];
	UINT32 damagePer = pBuff->m_paramsVec[3];
	
	if (heroID == pAtter->getID()){
		pAtter->summonSelf(lastTime,hpPer,damagePer);
	}
	else {
		if (pAtter->getMap()){
			CPos bornPos = pAtter->getPos();
			pAtter->getMap()->summonHero(heroID,pAtter->getOwnerID(),1,1,bornPos,0,true);
		}	
	}
}

void stFuncSummonHero::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncSummonHero::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	
}

void stFuncImmunce::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() <= 0)){
		return ;
	}
	
	//清空所有debuff
	pDefer->m_stateMgr.clearAllDebuff();
	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_IMMUNE_DEBUFF,pBuff->m_paramsVec[0],(UINT32)-1,pBuff);
}

void stFuncImmunce::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncImmunce::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	
}

void stFuncRage::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() <= 0)){
		return ;
	}

	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_RAGE,pBuff->m_paramsVec[0],(UINT32)-1,pBuff);	
}

void stFuncRage::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncRage::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	
}
	
void stFuncLimitDefense::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 2)){
		return ;
	}
	INT32 extralNum = -1 * pBuff->m_paramsVec[1];
	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_LIMIT_DEFENSE,pBuff->m_paramsVec[0],(UINT32)-1,pBuff,extralNum);
}

void stFuncLimitDefense::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncLimitDefense::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	if (!pDefer || !pBuff || (pBuff->m_paramsVec.size() < 2)){
		return ;
	}
}

void stFuncChgVelocity::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}

	UINT32 lastTime = pBuff->m_paramsVec[0];
	UINT8  add = pBuff->m_paramsVec[1];
	UINT32 num = pBuff->m_paramsVec[2];
	INT32 extraNum = num;
	if (!add){
		extraNum = -1 * extraNum;
	}

	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_DEC_VELOCITY,lastTime,(UINT32)-1,pBuff,extraNum);
}

void stFuncChgVelocity::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncChgVelocity::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}

void stFuncDizzy::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 1)){
		return ;
	}
	UINT32 lastTime = pBuff->m_paramsVec[0];
	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_DIZZY,lastTime,(UINT32)-1,pBuff,0);	
}

void stFuncDizzy::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncDizzy::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}

void stFuncProvocative::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
	
	UINT32 lastTime = pBuff->m_paramsVec[0];
	UINT8  add = pBuff->m_paramsVec[1];
	UINT32 num = pBuff->m_paramsVec[2];

	INT32 extraNum = num;
	if (!add){
		extraNum = -1 * extraNum;
	}

	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_PROVATION,lastTime,(UINT32)-1,pBuff,extraNum); 
}

void stFuncProvocative::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncProvocative::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}

void stFuncChgDamageProp::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
	
	UINT32 lastTime = pBuff->m_paramsVec[0];
	UINT8  add = pBuff->m_paramsVec[1];
	UINT32 num = pBuff->m_paramsVec[2];

	INT32 extraNum = num;
	if (!add){
		extraNum = -1 * extraNum;
	}
	//pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_CHG_DAMAGE_PROP,lastTime,(UINT32)-1,pBuff,0,extraNum); 
	
	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_CHG_DAMAGE_PROP,lastTime,(UINT32)-1,pBuff,extraNum); 
}

void stFuncChgDamageProp::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncChgDamageProp::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	if (!pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
}

void stFuncChgRefBangProp::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
	
	UINT32 lastTime = pBuff->m_paramsVec[0];
	UINT8  add = pBuff->m_paramsVec[1];
	UINT32 num = pBuff->m_paramsVec[2];

	INT32 extraNum = num;
	if (!add){
		extraNum = -1 * extraNum;
	}
	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_CHG_REF_BANG_PROP,lastTime,(UINT32)-1,pBuff,extraNum); 
}

void stFuncChgRefBangProp::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncChgRefBangProp::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	if (!pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
}

void stFuncChgHitRateProp::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
	
	UINT32 lastTime = pBuff->m_paramsVec[0];
	UINT8  add = pBuff->m_paramsVec[1];
	UINT32 num = pBuff->m_paramsVec[2];

	INT32 extraNum = num;
	if (!add){
		extraNum = -1 * extraNum;
	}

	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_CHG_HIT_RATE,lastTime,(UINT32)-1,pBuff,extraNum); 
}

void stFuncChgHitRateProp::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncChgHitRateProp::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	if (!pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
}
	
void stFuncTimerDecHP::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff){
		return ;
	}
	
	if (pBuff->m_paramsVec.size() < 6){
		return ;
	}

	UINT32  interval = pBuff->m_paramsVec[0];
	UINT32  lastTime = pBuff->m_paramsVec[1];
	if (interval > 0){//持续性
		pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_TIMER_DEC_HP,lastTime,interval,pBuff);		
	}
}

void stFuncTimerDecHP::doFunc(CBeing * pDefer,stStateInfo * pState)
{
	if (!pState || !pDefer){
		return;
	}

	if (pState->m_pBuff->m_paramsVec.size() < 6){
		return ;
	}

	UINT32  num = pState->m_pBuff->m_paramsVec[2];
	//UINT32  perB = pBuff->m_paramsVec[3];
	UINT32  perC = pState->m_pBuff->m_paramsVec[4];
	UINT32  perD = pState->m_pBuff->m_paramsVec[5];
	UINT32 dam = num + (UINT32)(pDefer->getHP() * perD / 100.0f) + (UINT32)(pDefer->getMaxHP() * perC / 100.0f);
	CBeing * pAtter = pDefer->getBeing(pState->m_attType,pState->m_moID, pDefer->getMap());
	std::max(dam,1u);
	pDefer->decHP(dam,pAtter);
	Map::stDirectDamge retCmd;
	retCmd.set_modtype(pDefer->getType());
	retCmd.set_modid(pDefer->getUniqueID());
	retCmd.set_damage(dam);
	retCmd.set_atttype(pState->m_attType);
	retCmd.set_attmodid(pState->m_moID);
	pDefer->getMap()->sendProto2Map(retCmd);
}

void stFuncTimerDecHP::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}

void stFuncChgBati::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff){
		return ;
	}

	if (pBuff->m_paramsVec.size() < 4){
		return ;
	}

	UINT32  interval = pBuff->m_paramsVec[0];
	UINT32  lastTime = pBuff->m_paramsVec[1];
	bool	bAdd = pBuff->m_paramsVec[2] ? true : false;
	UINT32  num = pBuff->m_paramsVec[3];
	
	if (bAdd){
		pDefer->addBati(num);
	}
	else {
		pDefer->decBati(num);	
	}

	if (interval > 0){//持续性
		pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_CHG_BATI,lastTime,interval,pBuff);		
	}
}

void stFuncChgBati::doFunc(CBeing * pDefer,stStateInfo * pState)
{
	if (!pDefer || !pState){
		return ;	
	}

	if (pState->m_pBuff->m_paramsVec.size() < 4){
		return ;
	}
	
	bool	bAdd = pState->m_pBuff->m_paramsVec[2] ? true : false;
	UINT32  num = pState->m_pBuff->m_paramsVec[3];
	
	if (bAdd){
		pDefer->addBati(num);
	}
	else {
		pDefer->decBati(num);	
	}
}

void stFuncChgBati::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}

void stFuncTimerAddHP::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff){
		return ;
	}
	
	if (pBuff->m_paramsVec.size() < 6){
		return ;
	}

	UINT32  interval = pBuff->m_paramsVec[0];
	UINT32  lastTime = pBuff->m_paramsVec[1];
	UINT32  num = pBuff->m_paramsVec[2];
	UINT32  perC = pBuff->m_paramsVec[4];
	UINT32  perD = pBuff->m_paramsVec[5];
	UINT32 dam = num + (UINT32)(pDefer->getHP() * perD / 100.0f) + (UINT32)(pDefer->getMaxHP() * perC / 100.0f);

	if (interval > 0){//持续性
		pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_TIMER_ADD_HP,lastTime,interval,pBuff);		
	}
	else {
		pDefer->addHP(dam);
	}
}

void stFuncTimerAddHP::doFunc(CBeing * pDefer,stStateInfo * pState)
{
	if (!pState || !pDefer){
		return;
	}

	if (pState->m_pBuff->m_paramsVec.size() < 6){
		return ;
	}

	UINT32  num = pState->m_pBuff->m_paramsVec[2];
	UINT32  perC = pState->m_pBuff->m_paramsVec[4];
	UINT32  perD = pState->m_pBuff->m_paramsVec[5];
	UINT32 dam = num + (UINT32)(pDefer->getHP() * perD / 100.0f) + (UINT32)(pDefer->getMaxHP() * perC / 100.0f);
	pDefer->addHP(dam);
}

void stFuncTimerAddHP::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}

void stFuncChgDodge::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
	
	UINT32 lasttime = pBuff->m_paramsVec[0];
	UINT8  addOp = pBuff->m_paramsVec[1];
	UINT32 extraNum = pBuff->m_paramsVec[2];
	
	if (!addOp){
		extraNum = -1 * extraNum;
	}
	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_CHG_DODGE,lasttime,(UINT32)-1,pBuff,extraNum);
}
	
void stFuncChgDodge::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncChgDodge::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}

void stFuncIce::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() <= 0)){
		return ;
	}

	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_ICE,pBuff->m_paramsVec[0],(UINT32)-1,pBuff);
}

void stFuncIce::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncIce::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	
}

void stFuncClearGoodBuf::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pDefer){
		return ;
	}
	pDefer->m_stateMgr.clearAllGoodBuff();
}

void stFuncClearGoodBuf::doFunc(CBeing * pDefer,stStateInfo * pState)
{
	
}

void stFuncClearGoodBuf::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}

void stFuncSummonNpc::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter ||!pDefer||!pBuff){
		return ;
	}
	if (pBuff->m_paramsVec.size() < 7){
		return ;
	} 

	UINT32 lastTime = pBuff->m_paramsVec[0] / 1000;
	UINT32 npcID = pBuff->m_paramsVec[1];
	UINT32 hpPer = pBuff->m_paramsVec[2];
	//UINT32 damagePer = pBuff->m_paramsVec[3];
	UINT32 actionGroupID = pBuff->m_paramsVec[4];
	UINT16 aminID = pBuff->m_paramsVec[5];

	CNpc * pNpc = pAtter->theNpc();

	if (pAtter->getMap() && pNpc){
		NpcDefine define;
		define.m_actionGroupID = actionGroupID;
		define.m_aminID = aminID;
		define.m_dirAngle = pNpc->getBornDir();
		define.m_id = npcID;
		define.m_lv = 1;
		CPos bornPos = pAtter->getPos();
		define.m_x = bornPos.m_x;
		define.m_y = bornPos.m_y;
		define.m_z = bornPos.m_z;
		define.m_lastTime = lastTime;
		UINT32 initHP = (UINT32)(hpPer / 100.0f * pNpc->getMaxHP());
		pAtter->getMap()->summonNpc(define,initHP);
	}	
}

void stFuncSummonNpc::doFunc(CBeing * pDefer,stStateInfo * pState)
{
	
}

void stFuncSummonNpc::doDelState(CBeing * pDefer,BufferBase * pBuff)
{

}
	
void stFuncChgAppearance::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 2)){
		return ;	
	}
	
	UINT32 roleID = pBuff->m_paramsVec[1];
	UINT32 lasttime = pBuff->m_paramsVec[0]; 

	UINT32 charID = pAtter->getOwnerID();
	GameUser * pUser = GameUserManager::getSingleton().getUser(charID);
	if (pUser){
		pUser->m_stateMgr.addState(pAtter,Map::BEING_STATE_CHG_APPEARANCE,lasttime,(UINT32)-1,pBuff);
		stRoleBase * pRoleBase =  g_roleBaseMgr.get(roleID);
		if (pRoleBase){
			if (pRoleBase->m_normalSkill){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_normalSkill,0);
			}   
			if (pRoleBase->m_skill1){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_skill1,0);
			}   
			if (pRoleBase->m_skill2){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_skill2,0);
			}   
			if (pRoleBase->m_skill3){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_skill3,0);
			}   
			if (pRoleBase->m_skill4){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_skill4,0);
			}
			if (pRoleBase->m_skill5){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_skill5,0);
			}   
			if (pRoleBase->m_skill6){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_skill6,0);
			}   
			if (pRoleBase->m_skill7){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_skill7,0);
			}
			if (pRoleBase->m_skill8){
				pUser->m_skillMgr.addTeampSkill(pRoleBase->m_skill8,0);
			}   
		}
	}
}

void stFuncChgAppearance::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncChgAppearance::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	if (!pDefer){
		return ;
	}	
	pDefer->m_skillMgr.clearTeampSkill();
}


void stFuncChgBangProp::doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff)
{
	if (!pAtter || !pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
	
	UINT32 lastTime = pBuff->m_paramsVec[0];
	UINT8  add = pBuff->m_paramsVec[1];
	UINT32 num = pBuff->m_paramsVec[2];

	INT32 extraNum = num;
	if (!add){
		extraNum = -1 * extraNum;
	}
	pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_CHG_BANG_PROP,lastTime,(UINT32)-1,pBuff,extraNum); 
}

void stFuncChgBangProp::doFunc(CBeing * pDefer,stStateInfo * pState)
{

}

void stFuncChgBangProp::doDelState(CBeing * pDefer,BufferBase * pBuff)
{
	if (!pDefer || !pBuff || (pBuff->m_paramsVec.size() < 3)){
		return ;
	}
}

CFunctionMgr::CFunctionMgr()
{
	bzero(m_pFunVec,sizeof(m_pFunVec));
	m_pFunVec[FUNCTION_TYPE_NULL] = ATHENA_NEW stFunctionNull(); 
	m_pFunVec[FUNCTION_TYPE_ADD_HP] = ATHENA_NEW stFuncAddHP();
	m_pFunVec[FUNCTION_TYPE_SILENCE] = ATHENA_NEW stFuncSilence();
	m_pFunVec[FUNCTION_TYPE_SUMMON_HERO] = ATHENA_NEW stFuncSummonHero();
	m_pFunVec[FUNCTION_TYPE_ADD_IMMUNCE] = ATHENA_NEW stFuncImmunce();
	m_pFunVec[FUNCTION_TYPE_RAGE] = ATHENA_NEW stFuncRage();
	m_pFunVec[FUNCTION_TYPE_LIMIT_DEFENSE] = ATHENA_NEW stFuncLimitDefense();
	m_pFunVec[FUNCTION_TYPE_DEC_VELOCITY]  = ATHENA_NEW stFuncChgVelocity();
	m_pFunVec[FUNCTION_TYPE_DIZZY] = ATHENA_NEW stFuncDizzy();
	m_pFunVec[FUNCTION_TYPE_PROVOCATIVE] = ATHENA_NEW stFuncProvocative();
	m_pFunVec[FUNCTION_TYPE_CHG_DAMAGE_PROP] = ATHENA_NEW stFuncChgDamageProp();
	m_pFunVec[FUNCTION_TYPE_CHG_REF_BANG] = ATHENA_NEW stFuncChgRefBangProp();
	m_pFunVec[FUNCTION_TYPE_CHG_HIT_RATE]  = ATHENA_NEW stFuncChgHitRateProp();
	m_pFunVec[FUNCTION_TYPE_TIMER_DEC_HP] = ATHENA_NEW stFuncTimerDecHP();
	m_pFunVec[FUNCTION_TYPE_CHG_BATI] = ATHENA_NEW stFuncChgBati(); 
	m_pFunVec[FUNCTION_TYPE_TIMER_ADD_HP] = ATHENA_NEW stFuncTimerAddHP();
	m_pFunVec[FUNCTION_TYPE_CHG_DODGE] = ATHENA_NEW stFuncChgDodge();
	m_pFunVec[FUNCTION_TYPE_ICE] = ATHENA_NEW stFuncIce(); 
	m_pFunVec[FUNCTION_TYPE_CLEAR_BUF] = ATHENA_NEW stFuncClearGoodBuf(); 
	m_pFunVec[FUNCTION_TYPE_SUMMONNPC] = ATHENA_NEW stFuncSummonNpc(); 
	m_pFunVec[FUNCTION_TYPE_CHGAPPEARANCE]  = ATHENA_NEW stFuncChgAppearance();
	m_pFunVec[FUNCTION_TYPE_CHG_BANG] = ATHENA_NEW stFuncChgBangProp();
}


CSkillRangeCfg::CSkillRangeCfg()
{}

CSkillRangeCfg::~CSkillRangeCfg()
{}

bool CSkillRangeCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/SkillRange.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->trace("[技能范围配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("Root");
	if (!pRoot){
		return false;
	}
	
	INT32 iVal = 0;
	TiXmlElement * pRange = pRoot->FirstChildElement("Range");
	while (pRange) {
		UINT16 id = 0;
		if (pRange->Attribute("id",&iVal)){
			id = iVal;
		}

		TiXmlElement * pPosNode = pRange->FirstChildElement("Pos");
		while (pPosNode) {
			CPos pos;
			if (pPosNode->Attribute("x",&iVal)) {
				pos.m_x = iVal;
			}

			if (pPosNode->Attribute("y",&iVal)) {
				pos.m_y = iVal;
			}
			m_ranges[id].push_back(pos);
			pPosNode = pPosNode->NextSiblingElement("Pos");
		}
		pRange = pRange->NextSiblingElement("Range");
	}

#ifdef _HDZ_DEBUG
	std::map<UINT16,std::vector<CPos> >::iterator it = m_ranges.begin();
	for (;it != m_ranges.end(); ++it){
		Athena::logger->trace("范围ID=%d,技能攻击点有:",it->first);	
		for (UINT16 i = 0;i < it->second.size(); ++i) {
			Athena::logger->trace("x=%d,y=%d",it->second[i].m_x,it->second[i].m_y);	
		}
	}
#endif
	return true;	
}

std::vector<CPos> &  CSkillRangeCfg::getRangePos(UINT16 id)
{
	std::map<UINT16,std::vector<CPos> >::iterator it = m_ranges.find(id);
	if (it == m_ranges.end()) {
		return m_defaultVec;	
	}
	return it->second;
}


CSkill::CSkill()
{
	m_pSkillBase = NULL;
	m_pSkillGrownBase = NULL;
	m_lastUseTime = 0;
}

CSkill::~CSkill()
{
	
}

UINT32 CSkill::getLastTime()
{
	if (m_pSkillGrownBase && m_pSkillBase){
		if (!m_pSkillBase->m_isNormal){
			return m_pSkillGrownBase->m_coolDown;
		}
		else {
			return 2000;
		}
	}
	else {
		return 100000;
	}
}

UINT32 CSkill::getMaxHitNum()
{
	if (m_pSkillGrownBase){
		return m_pSkillGrownBase->m_attackNum;
	}	
	else {
		return 1;
	}
}

void CSkill::setLastUseTime()
{
	m_lastUseTime = g_gameServer->getMSec();
}

bool CSkill::isCoolDown(UINT32 reduceSec)
{
    if(Athena::global["CountryVersion"] == "Korea"){
        return false;
    }
	if (!m_pSkillGrownBase){
		return true;
	}

    // Athena::logger->trace("上次使用时间 %d, 当前时间 %d, 冷却时间 %d, 时间差 %d",
    //        m_lastUseTime, g_gameServer->getMSec(), m_pSkillGrownBase->m_coolDown, (m_lastUseTime + m_pSkillGrownBase->m_coolDown -g_gameServer->getMSec()));
	if (m_lastUseTime + m_pSkillGrownBase->m_coolDown - reduceSec*1000 > g_gameServer->getMSec()){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("还处于冷却时间,却释放技能! m_lastUseTime[%d], m_pSkillGrownBase->m_coolDown[%d] reduceSec[%d]",m_lastUseTime,m_pSkillGrownBase->m_coolDown, reduceSec);
#endif
		return true;	
	}
	return false;
}

bool CSkill::init()
{
	stSkillBase * pSkillBase = g_skillBaseMgr.get(m_skillInfo.m_skillID);	
	if (!pSkillBase){
		return false;
	}
	m_pSkillBase = pSkillBase;

	m_pSkillGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(m_skillInfo.m_skillID,m_skillInfo.m_level)); 
	
	if (!m_pSkillGrownBase){
		return false;
	}

	this->setID(m_skillInfo.m_skillID);
	this->setName(pSkillBase->m_name);
	return true;
}

void CSkill::getAllDefers(CBeing * pAtter,CPos & attPos,std::vector<CBeing *> & vec)
{
	if (!pAtter||!pAtter->getMap()||!m_pSkillBase) {
		return ;
	}	
}

void CSkill::doSkillFunction(CBeing * pAtter,CBeing * pDefer)
{
	if (!pAtter || !pDefer) {
		return ;
	}

	for (UINT16 i = 0;i < m_pSkillBase->m_buffVec.size() ; ++i){
		BufferBase *pBuff = g_bufferBaseMgr.get(m_pSkillBase->m_buffVec[i]);
		if (pBuff){
			if (pAtter == pDefer){//如果是玩家自己
				if ((pBuff->m_target & eSkill_Target_Self) == 0){//该buff不能对自己起作用
					continue;
				}
			}
			else if (pAtter->getOwnerID() == pDefer->getID()){//防御者是攻击者的主人
				if ((pBuff->m_target & eSkill_Target_Master) == 0){
					continue;
				}	
			}
			else if (pAtter->getCampID() == pDefer->getCampID()){//是友方
				if ((pBuff->m_target & eSkill_Target_Friend) == 0){
					continue;
				}
			}
			else {//是敌方
				if ((pBuff->m_target & eSkill_Target_Enemy) == 0){
					continue;
				}
			}


			stFunctionBase * pOp = CFunctionMgr::getSingleton().getFunction(pBuff->m_opID);
			if (pOp){
				pOp->doThrowState(pAtter,pDefer,pBuff);		
			}
			 #ifdef _HDZ_DEBUG
			 Athena::logger->trace("atter(%s,%d)给defer(%s,%d)用技能:%d投递了效果:%d",pAtter->getName(),pAtter->getID(),pDefer->getName(),pDefer->getID(),m_pSkillBase->m_skillID,pBuff->m_opID);
			 #endif
		}
	}
}

void CSkill::throwBuff(CBeing * pAtter,CBeing * pDefer,UINT32 buffID)
{
	if (!pAtter || !pDefer) {
		return ;
	}

	BufferBase *pBuff = g_bufferBaseMgr.get(buffID);
	if (pBuff){
		stFunctionBase * pOp = CFunctionMgr::getSingleton().getFunction(pBuff->m_opID);
		if (pOp){
			pOp->doThrowState(pAtter,pDefer,pBuff);		
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("(throwBuff) atter(%s,%d)给defer(%s,%d)投递了buffID:%d",pAtter->getName(),pAtter->getID(),pDefer->getName(),pDefer->getID(),buffID);
#endif
	}
}

bool CSkill::checkCost(CBeing * pAtt)
{
	if (!pAtt || !m_pSkillBase || !m_pSkillGrownBase){
		return false;
	}

	if (m_pSkillBase->m_isNormal){
		return true;
	}
	UINT32 ownerID = pAtt->getOwnerID();
	if (ownerID){
		GameUser * pUser = GameUserManager::getSingleton().getUser(ownerID);
		if (pUser){
			UINT32 mpReduce = pUser->getRaiseSkillValue(m_pSkillBase->m_skillID, eRaiseSkill_ReduceCost);
			if (mpReduce < m_pSkillGrownBase->m_costMP)
			{
				return pUser->decMP(m_pSkillGrownBase->m_costMP - mpReduce);
			}else{
				return pUser->decMP(0);
			}
		}
		return false;
	}
	else {
		UINT32 mpReduce = pAtt->getRaiseSkillValue(m_pSkillBase->m_skillID, eRaiseSkill_ReduceCost);
		if (mpReduce < m_pSkillGrownBase->m_costMP)
		{
			return pAtt->decMP(m_pSkillGrownBase->m_costMP - mpReduce);
		}else{
			return pAtt->decMP(0);
		}
	}
}

bool CSkill::checkDis(const CPos & attPos,const CPos & defPos)
{
	if (!m_pSkillBase){
		return false;
	}	
	return true;
}

bool CSkill::isNormal()
{
	if (!m_pSkillBase){
		return false;
	}
	return m_pSkillBase->m_isNormal;
}

bool CSkill::isCalcDam()
{
	if (!m_pSkillBase){
		return false;
	}
	return m_pSkillBase->m_isCalcDam;
}

std::pair<UINT32,UINT32>  CSkill::doAction(CBeing * pAtter,CBeing * pDefer,UINT8 step)
{
	if (!pAtter || !pDefer || !pAtter->getMap() || !pDefer->getMap() || !m_pSkillBase || !m_pSkillGrownBase){
		return std::make_pair(0,0);
	}
	
	pAtter->resetPKData();
	pDefer->resetPKData();
	UINT32  decHP = 0;
	doSkillFunction(pAtter,pDefer);
	
	if (!m_pSkillBase->m_isCalcDam){//不需要计算伤害
		return std::make_pair(0,0);
	}

	pAtter->calcPKData();
	pDefer->calcPKData();
	UINT32 damage = 0;
	UINT32 addDamage = 0;
	UINT32 decBati = 0;

	if (1 == step){
		damage = m_pSkillGrownBase->m_damage1;
		addDamage = m_pSkillGrownBase->m_addDamage1;
		decBati = m_pSkillBase->m_poFang1; 
	}
	else if (2 == step){
		damage = m_pSkillGrownBase->m_damage2;
		addDamage = m_pSkillGrownBase->m_addDamage2;
		decBati = m_pSkillBase->m_poFang2;
	}
	else if (3 == step){
		damage = m_pSkillGrownBase->m_damage3;
		addDamage = m_pSkillGrownBase->m_addDamage3;
		decBati = m_pSkillBase->m_poFang3;
	}
	else if (4 == step){
		damage = m_pSkillGrownBase->m_damage4;
		addDamage = m_pSkillGrownBase->m_addDamage4;
		decBati = m_pSkillBase->m_poFang4;
	}
#ifdef _HDZ_DEBUG
	//Athena::logger->trace("技能的段数:%d,damge=%d,addDamage=%d,decBati=%d",step,damage,addDamage,decBati);
#endif
	pAtter->getPKCalcData().m_skillDamage = damage;
	pAtter->getPKCalcData().m_skillAddDamage = addDamage;
	decHP = pAtter->calcDamage(pDefer);
	
	if (decHP && m_pSkillBase->m_isNormal){//普攻要回血
		if (pAtter->isPlayer()){
			pAtter->addMP(pAtter->getRecoverMP());
		}
		pDefer->decBati(decBati);
		pDefer->hitByNormal();
	}
	
	if ((decHP > 0) && m_pSkillBase->m_isNormal){//普通要计算元素伤害
		//计算是否有火伤害
		stPKCalcData & attData = pAtter->getPKCalcData();
		stPKCalcData & defData = pDefer->getPKCalcData();
		UINT32 rate = 0;
		rate = attData.m_fireRate * attData.m_fireRate / std::max((INT32)(attData.m_fireRate + defData.m_refFireRate),1);

		if (g_selectByPercent(rate)){//触发火强
			pDefer->getPKCalcData().m_attackResult |= Map::eAttResult_Fire_Damage;
			Map::stNotifyAddFireWall fireWallCmd;
			fireWallCmd.set_x(pDefer->getPos().m_x);
			fireWallCmd.set_y(pDefer->getPos().m_y);
			fireWallCmd.set_z(pDefer->getPos().m_z);
			fireWallCmd.set_lasttime(4);
			pAtter->getMap()->sendProto2Map(fireWallCmd);
			pAtter->getMap()->addFireWall(pAtter,pDefer);
		}
		
		rate = attData.m_iceRate * attData.m_iceRate / std::max((INT32)(attData.m_iceRate + defData.m_refIceRate),1);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("攻击者的冰冻概率:%d,防御者的攻击概率:%d,最终概率:%d",attData.m_iceRate,defData.m_refIceRate,rate);
#endif

		if (g_selectByPercent(rate)){//冰冻伤害
			CSkill::throwBuff(pAtter,pDefer,11700);
			//pDefer->m_stateMgr.addState(pAtter,Map::BEING_STATE_ICE,2000,(UINT32)-1);	
			pDefer->getPKCalcData().m_attackResult |= Map::eAttResult_Ice_Damage;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("触发了冰冻状态");
#endif
		}

		rate = attData.m_thunderRate * attData.m_thunderRate / std::max((INT32)(attData.m_thunderRate + defData.m_refThunderRate),1);

		if (g_selectByPercent(rate)){//雷电伤害
			pDefer->getPKCalcData().m_attackResult |= Map::eAttResult_Thunder_Damage;
			Map::stBeginThunderDamage thunderCmd;
			thunderCmd.set_attmodtype(pAtter->getType());
			thunderCmd.set_attmodid(pAtter->getUniqueID());
			thunderCmd.set_deftype(pDefer->getType());
			thunderCmd.set_defmodid(pDefer->getUniqueID());
			pAtter->getMap()->sendProto2Map(thunderCmd);
		}
	}
	return  std::make_pair(decHP,pDefer->getPKCalcData().m_attackResult);
}

CSkillMgr::CSkillMgr(CBeing * pOwner):ElementMgr(false,true,false,false),m_pOwner(pOwner)
{
	m_addBP = 0;	
}

CSkillMgr::~CSkillMgr()
{
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CSkill * pSkill = (CSkill *)it->second;
		SAFE_DELETE(pSkill);
	}
}

void CSkillMgr::synAllSkill()
{
	GameUser * pUser = m_pOwner->theGameUser();
	if (pUser){
		Role::stSynUserAllSkill retCmd;
		TIDMap_IT it = m_idMap.begin();
		for (;it != m_idMap.end();++it){
			CSkill * pSkill = (CSkill *)it->second;
			Role::stSynSkillInfo * pSyn = retCmd.add_skillinfo();
			if (pSyn){
				pSyn->set_skillid(pSkill->m_skillInfo.m_skillID);
				pSyn->set_skilllv(pSkill->m_skillInfo.m_level);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("玩家:%s,id=%d上线发送了技能:%d,lv=%d",pUser->getName(),pUser->getID(),pSkill->m_skillInfo.m_skillID,pSkill->m_skillInfo.m_level);
#endif
			}
		}
		pUser->sendProto2Me(retCmd);
	}	
}

CSkill * CSkillMgr::createSkill(UINT32 skillID,UINT16 lv)
{
	CSkill * pSkill = ATHENA_NEW CSkill();
	if (!pSkill){
		return NULL;
	}
	
	pSkill->m_skillInfo.m_skillID = skillID;
	pSkill->m_skillInfo.m_level = lv;

	if (!pSkill->init()){
		return NULL;
	}
	return pSkill;
}

CSkill * CSkillMgr::getSkillByID(UINT32 skillID)
{
	std::map<UINT32,CSkill *>::iterator it = m_tempSkills.find(skillID);
	if (it != m_tempSkills.end()){
		return it->second;
	}
	return (CSkill*)getElementByID(skillID);	
}

bool CSkillMgr::addSkill(CSkill * pSkill)
{
	if (!pSkill){
		return false;
	}	
	return addElement(pSkill);
}

bool CSkillMgr::removeSkill(CSkill * pSkill)
{
	if (!pSkill){
		return false;
	}
	removeElement(pSkill);
	return true;
}

void CSkillMgr::throwAllPassiveSkill()
{
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it) {
	}
}

UINT32 CSkillMgr::save(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	stSkillSaveInfo * pSaveInfo = (stSkillSaveInfo *)buf;
	pSaveInfo->m_version = s_SKILL_VERSION;
	pSaveInfo->m_num = 0;
	ret += sizeof(stSkillSaveInfo);

	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it) {
		CSkill * pSkill = (CSkill *)it->second;
		bcopy(&pSkill->m_skillInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(SkillInfo));
		pSaveInfo->m_num += 1;
		ret += sizeof(SkillInfo);
	}
	return ret;
}

void CSkillMgr::exeEveryOne(stSkillCallBackBase & cb)
{
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it) {
		CSkill * pSkill = (CSkill *)it->second;
		if (!cb.doIt(pSkill)){
			return ;
		}
	}
}

UINT16 CSkillMgr::getSkillNumByRaiseLv(UINT16 lv)
{
	UINT16 ret = 0;
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it) {
		CSkill * pSkill = (CSkill *)it->second;
		if (pSkill && (pSkill->m_skillInfo.m_level >= lv)){
			ret += 1;
		}
	}
	return ret;
}

UINT32 CSkillMgr::load(byte * buf)
{
	UINT32 ret = 0;
	stSkillSaveInfo * pSaveInfo = (stSkillSaveInfo *)buf;
	ret += sizeof(stSkillSaveInfo);
	
	m_addBP = 0;

	if (pSaveInfo->m_version == s_SKILL_VERSION){
		UINT16 size = pSaveInfo->m_num;
		for (UINT16 i = 0;i < size ; ++i){
			CSkill * pSkill = ATHENA_NEW CSkill();
			if (pSkill){
				bcopy(&pSaveInfo->m_data[i],&pSkill->m_skillInfo,sizeof(SkillInfo));
				ret += sizeof(SkillInfo);
				
				if (!pSkill->init()){
					Athena::logger->error("name=%s,id=%d加载技能失败,初始化技能失败!",m_pOwner->getName(),m_pOwner->getID());
					SAFE_DELETE(pSkill);
					continue;
				}

				if (!addSkill(pSkill)){
					Athena::logger->error("name=%s,id=%d加载技能失败,往容器里添加技能时候失败",m_pOwner->getName(),m_pOwner->getID());
					SAFE_DELETE(pSkill);
					continue;
				}
			}	
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("上线加载了%d个技能",m_idMap.size());
#endif
	return ret;
}

#define Check_Skill_Unlock(skillID,unlockLv) do { \
	if (pUser->getLevel() >= pRoleBase->unlockLv){ \
		CSkill * pSkill =  getSkillByID(pRoleBase->skillID);\
		if (!pSkill){\
			pSkill = createSkill(pRoleBase->skillID);\
			if (!addSkill(pSkill)) { \
				SAFE_DELETE(pSkill);\
				continue;\
			}\
			Role::stAddUserSkill retCmd; \
			retCmd.mutable_data()->set_skillid(pSkill->getID()); \
			retCmd.mutable_data()->set_skilllv(0);	\
			pUser->sendProto2Me(retCmd);\
		}\
	}\
} while (false);

void CSkillMgr::checkSkillUnlock()
{
	GameUser * pUser = m_pOwner->theGameUser();
	if (pUser){//如果是玩家
		stRoleBase * pRoleBase = g_roleBaseMgr.get(pUser->getRoleID());
		if (pRoleBase){
			Check_Skill_Unlock(m_skill1,m_skill1UnlockLv);
			Check_Skill_Unlock(m_skill2,m_skill2UnlockLv);
			Check_Skill_Unlock(m_skill3,m_skill3UnlockLv);
			Check_Skill_Unlock(m_skill4,m_skill4UnlockLv);
			Check_Skill_Unlock(m_skill5,m_skill5UnlockLv);
			Check_Skill_Unlock(m_skill6,m_skill6UnlockLv);
			Check_Skill_Unlock(m_skill7,m_skill7UnlockLv);
			Check_Skill_Unlock(m_skill8,m_skill8UnlockLv);
		}
	}

#ifdef _HDZ_DEBUG
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it) {
		CSkill * pSkill = (CSkill *)it->second;
		if (pSkill){
			Athena::logger->trace("对象(name:%s,id:%d)拥有的技能有:skillID:%d",m_pOwner->getName(),m_pOwner->getID(),pSkill->getID());
		}
	}

#endif
}

bool CSkillMgr::addTeampSkill(UINT32 skillID,UINT16 lv)
{
	if (m_tempSkills.find(skillID) != m_tempSkills.end()){
		Athena::logger->error("创建临时技能失败,已经有该技能了:skillID:%d",skillID);
		return false;
	}	

	CSkill * pSkill = CSkillMgr::createSkill(skillID,lv);
	if (!pSkill){
		return false;
	}
	if (m_tempSkills.insert(std::make_pair(skillID,pSkill)).second){
		return true;
	}
	else {
		SAFE_DELETE(pSkill);
		Athena::logger->error("创建临时技能失败,添加到容器不成功,skillID:%d",skillID);
		return false;
	}
}

void CSkillMgr::clearTeampSkill()
{
	std::map<UINT32,CSkill *>::iterator it = m_tempSkills.begin();
	for (;it != m_tempSkills.end();++it){
		CSkill *pSkill = it->second;
		SAFE_DELETE(pSkill);
	}
	m_tempSkills.clear();
}

void CSkillMgr::calcBP()
{
	m_addBP = 0;
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it) {
		CSkill * pSkill = (CSkill *)it->second;
		if (pSkill && pSkill->m_pSkillGrownBase){
			m_addBP += pSkill->m_pSkillGrownBase->m_addBP;
		}
	}
}

CStateMgr::CStateMgr(CBeing * pOwner) : m_pOwner(pOwner)
{

}

void CStateMgr::timer(UINT64 curTime)
{
	std::map<UINT32,stStateInfo>::iterator it = m_buffs.begin();
	std::vector<UINT32> delVec;

	for (;it != m_buffs.end();++it){
		stStateInfo & stateInfo = it->second;
		if (stateInfo.m_lastDoFuncTime < curTime){
			stateInfo.m_lastDoFuncTime = curTime + stateInfo.m_interval;
			if (it->second.m_pBuff){
				stFunctionBase * pFunc = CFunctionMgr::getSingleton().getFunction(it->second.m_pBuff->m_opID);
				if (pFunc){
					pFunc->doFunc(m_pOwner,&it->second);
				}
			}
		}

		if (stateInfo.m_addTime + stateInfo.m_lastTime < curTime){
			delVec.push_back(it->first);
		}
	}
	
	bool bReInit = false;

	for (UINT16 i = 0;i < delVec.size();++i){
		m_buffs.erase(delVec[i]);
		if (m_pOwner->getMap() && m_pOwner->getMap()->isCopyMap()){
			Map::stDelBufferState cmd;
			cmd.set_motype(m_pOwner->getType());
			cmd.set_moid(m_pOwner->getUniqueID());
			cmd.set_bufferid(delVec[i]);
			m_pOwner->getMap()->sendProto2Map(cmd);
		}
		bReInit = true;
	}
	
	if (bReInit){
		setupBuff();
	}
}

void CStateMgr::addState(CBeing * pAtter,UINT16 stateID,UINT32 lastTime,UINT32 interval,BufferBase * pBuff,INT32 extraVal,bool notify)
{
	if (!pAtter || !pBuff){
		return ;
	}
	
	if (m_pOwner->isSetState(Map::BEING_STATE_IMMUNE_DEBUFF)){//如果有免疫debuff属性
		if (pBuff && pBuff->m_bDebuff){
			return ;
		}
	}

	stStateInfo stateInfo;
	stateInfo.m_addTime = g_gameServer->getMSec();
	stateInfo.m_lastDoFuncTime = stateInfo.m_addTime + interval;
	stateInfo.m_interval = interval;
	stateInfo.m_lastTime = lastTime;
	stateInfo.m_stateID = stateID;
	stateInfo.m_pBuff = pBuff;
	stateInfo.m_value = extraVal;
	m_buffs[pBuff->m_id] = stateInfo;

	if (notify && m_pOwner->getMap() && m_pOwner->getMap()->isCopyMap()){
		Map::stAddBufferState2Me cmd;
		cmd.set_atttype(pAtter->getType());
		cmd.set_attmodid(pAtter->getUniqueID());
		cmd.set_deftype(m_pOwner->getType());
		cmd.set_defmodid(m_pOwner->getUniqueID());
		cmd.set_stateid(stateID);
		cmd.set_extraval(extraVal);
		if (pBuff){
			cmd.set_buffid(pBuff->m_id);
		}
		m_pOwner->getMap()->sendProto2Map(cmd);
	}
	setupBuff();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name:%s,id:%d增加了state:%d,lastTime:%d",m_pOwner->getName(),m_pOwner->getID(),stateID,lastTime);
#endif
}

void CStateMgr::delState(UINT32 buffID,bool notify)
{
	std::map<UINT32,stStateInfo>::iterator it = m_buffs.find(buffID);	
	if (it == m_buffs.end()){
		return ;
	}
	
	if (it->second.m_pBuff){
		stFunctionBase * pFunc = CFunctionMgr::getSingleton().getFunction(it->second.m_pBuff->m_opID);
		if (pFunc){
			pFunc->doDelState(m_pOwner,it->second.m_pBuff);
		}
	}
	UINT16 stateID = it->second.m_stateID;
	m_buffs.erase(it);

	if (notify && m_pOwner->getMap() && m_pOwner->getMap()->isCopyMap()){
		Map::stDelBufferState cmd;
		cmd.set_motype(m_pOwner->getType());
		cmd.set_moid(m_pOwner->getUniqueID());
		cmd.set_stateid(stateID);
		cmd.set_bufferid(buffID);
		m_pOwner->getMap()->sendProto2Map(cmd);
	}
	setupBuff();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name:%s,id:%d删除了状态:%d",m_pOwner->getName(),m_pOwner->getID(),stateID);
#endif
}

void CStateMgr::clearAllDebuff()
{
	std::map<UINT32,stStateInfo>::iterator it = m_buffs.begin();	

	std::vector<UINT32> delVec;
	for (;it != m_buffs.end();++it){
		stStateInfo & stateInfo = it->second;
		if (stateInfo.m_pBuff && stateInfo.m_pBuff->m_bCanClear && stateInfo.m_pBuff->m_bDebuff){
			delVec.push_back(it->first);
		}
	}

	for (UINT16 i = 0;i < delVec.size();++i){
		m_buffs.erase(delVec[i]);
	}
	setupBuff();
}

void CStateMgr::clearAllGoodBuff()
{
	std::map<UINT32,stStateInfo>::iterator it = m_buffs.begin();	

	std::vector<UINT32> delVec;
	for (;it != m_buffs.end();++it){
		stStateInfo & stateInfo = it->second;
		if (stateInfo.m_pBuff && stateInfo.m_pBuff->m_bCanClear && (!stateInfo.m_pBuff->m_bDebuff)){
			delVec.push_back(it->first);
		}
	}

	for (UINT16 i = 0;i < delVec.size();++i){
		m_buffs.erase(delVec[i]);
	}
	setupBuff();
}

void CStateMgr::clearAllBuff()
{
	m_buffs.clear();
	setupBuff();
}

void CStateMgr::setupBuff()
{
	m_pOwner->clearAllState();
	stBufferState & buffData =  m_pOwner->getBuffData();
	buffData.reset();

	std::map<UINT32,stStateInfo>::iterator it = m_buffs.begin();
	for (;it != m_buffs.end();++it){
		stStateInfo & stateInfo = it->second;
		switch (stateInfo.m_stateID){
			case Map::BEING_STATE_ICE :
			case Map::BEING_STATE_SILENCE :
			case Map::BEING_STATE_IMMUNE_DEBUFF :
			case Map::BEING_STATE_RAGE :
			case Map::BEING_STATE_FIRE_SPIRIT :
			case Map::BEING_STATE_DEC_VELOCITY :
			case Map::BEING_STATE_DIZZY :
			case Map::BEING_STATE_PROVATION :
			case Map::BEING_STATE_TIMER_DEC_HP :
			case Map::BEING_STATE_CHG_BATI :
			case Map::BEING_STATE_TIMER_ADD_HP :
				{
					m_pOwner->setState(stateInfo.m_stateID);
				}
				break;
			case Map::BEING_STATE_CHG_DAMAGE_PROP :
				{
					buffData.m_damagePropPer += stateInfo.m_value;	
				}
				break;
			case Map::BEING_STATE_CHG_REF_BANG_PROP :
				{
					buffData.m_refBang += stateInfo.m_value;	
				}
				break;
			case Map::BEING_STATE_CHG_HIT_RATE :
				{
					buffData.m_hitRate += stateInfo.m_value;	
				}
				break;
			case Map::BEING_STATE_LIMIT_DEFENSE :
				{
					buffData.m_damagePer += stateInfo.m_value;	
				}
				break;
			case Map::BEING_STATE_CHG_DODGE :
				{
					buffData.m_dodge += stateInfo.m_value;
				}
				break;
			case Map::BEING_STATE_CHG_BANG_PROP :
				{
					buffData.m_bang += stateInfo.m_value;
				}
				break;
		}
	}
}
