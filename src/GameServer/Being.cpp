#include"Being.h"
#include "Obj.h"
#include"Map.h"
#include "GameUserManager.h"
#include "GameUser.h"
#include "Log4cxxHelp.h"
#include "Skill.h"
#include "HeroMgr.h"
#include "NpcMgr.h"
#include "CopyMap.h"
#include "RobotMgr.h"

CSkillCoolDownMgr::CSkillCoolDownMgr(CBeing * pOwner) : m_pOwner(pOwner)
{
	m_lastUseTime = 0;
	m_lastTime = 0;
	m_useSkillID = 0;
	m_hitNum = 0;
	m_maxHitNum = 0;
}

void CSkillCoolDownMgr::timer(UINT64 cur)
{
	if (m_lastUseTime + m_lastTime < cur){
		m_useSkillID = 0;
		m_hitNum = 0;
	}
}

void CSkillCoolDownMgr::throwSkillSuccess(UINT32 skillID,UINT32 lastTime,UINT32 maxHitNum)
{
	m_useSkillID = skillID;
	m_hitNum = 0;
	m_lastTime = lastTime;
	m_lastUseTime = g_gameServer->getMSec();
	m_maxHitNum = maxHitNum;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("释放技能成功:name:%s,id:%d 技能ID:%d,lasttime:%d,maxHitNum:%d",m_pOwner->getName(),m_pOwner->getID(),m_useSkillID,m_lastTime,m_maxHitNum);
#endif
}

bool CSkillCoolDownMgr::hitValid(UINT32 skillID)
{
	if (skillID != m_useSkillID){
#ifdef _HDZ_DEBUG
		Athena::logger->error("碰撞时候发送过来的技能不对,拒绝响应他的碰撞请求,skillID:%d,useSkillID:%d!",skillID,m_useSkillID);
#endif
		return false;
	}
	
	if (m_hitNum > m_maxHitNum){
#ifdef _HDZ_DEBUG
		Athena::logger->error("客户端发送过来的碰撞已经超过最大次数");
#endif
		return false;
	}
	return true;
}


CBeing::CBeing( ) :m_skillMgr(this),m_stateMgr(this),m_skillCDMgr(this)
{
	m_curTargetID = 0;
	m_curTargetType = CObj::OBJ_TYPE_MAX; 
	m_lastAttackTime = 0;
	m_lastMoveTime = 0;
	bzero(m_state,sizeof(m_state));
	m_hp = 0;
	m_lastHP = 0;
	m_maxHP = 0;                                //血量
	m_mp = 0;									//当前魔法
	m_maxMP = 0;								//最大魔法
	m_bati = 0;									//当前霸体值
	m_maxBati = 0;								//最大霸体值
	m_campID = 0;
    m_coolDown = false;
}

CBeing::~CBeing( )
{
}

//时间片执行回调函数
bool CBeing::heartBeat(UINT64 uTime)
{
	m_stateMgr.timer(uTime);
	m_skillCDMgr.timer(uTime);

	if (canClearMe()){
		setClearState();	
	}
	return true;
}

CSkill * CBeing::getSkillByID(UINT32 skillID)
{
	return m_skillMgr.getSkillByID(skillID);	
}

UINT32 CBeing::decHP(UINT32 hp,CBeing * pAtter)
{
	m_lastHP = m_hp;
	if (m_hp < hp){
		m_hp = 0;
	}
	else {
		m_hp -= hp;
	}
	UINT32 ret = m_lastHP > m_hp ? m_lastHP - m_hp : 0;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name:%s,modID:%d当前的血量:%d,扣除血量:%d",getName(),getUniqueID(),m_hp,hp);
#endif
	if (m_hp <= 0){
		this->processDeath(pAtter);
	}
	return ret;
}

void CBeing::addHP(UINT32 hp)
{
	//UINT32 oldHP = m_hp;
	m_hp += hp;
	m_hp = m_hp > m_maxHP ? m_maxHP : m_hp;
	
	//UINT32 addHP = m_hp - oldHP;
	if (getMap()){
		Map::stMapObjAddHP retCmd;
		retCmd.set_motype(getType());
		retCmd.set_moid(getUniqueID());
		retCmd.set_addhp(hp);
		getMap()->sendProto2Map(retCmd);
	}
}

void CBeing::addMP(UINT32 mp)
{
	UINT32 newVal = m_mp + mp;
	if (newVal > m_maxMP){
		newVal = m_maxMP;
	}
	
	m_mp = newVal;
	if (getMap() && getMap()->isCopyMap()){
		Map::stNotifyObjMP cmd;
		cmd.set_motype(getType());
		cmd.set_moid(getUniqueID());
		cmd.set_num(newVal);
		getMap()->sendProto2Map(cmd);
	}
}

bool CBeing::decMP(UINT32 mp)
{
	if (m_mp < mp){
		return false;			
	}
	
	m_mp -= mp;

	if (getMap() && getMap()->isCopyMap()){
		Map::stNotifyObjMP cmd;
		cmd.set_motype(getType());
		cmd.set_moid(getUniqueID());
		cmd.set_num(m_mp);
		getMap()->sendProto2Map(cmd);
	}
	return true;
}

void CBeing::addBati(UINT32 bati)
{
	UINT32 newVal = bati + m_bati;
	if (newVal > m_maxBati){
		newVal = m_maxBati;
	}

	m_bati = newVal;

	if (getMap() && getMap()->isCopyMap()){
		Map::stNotifyObjBati cmd;
		cmd.set_motype(getType());
		cmd.set_moid(getUniqueID());
		cmd.set_num(m_bati);
		getMap()->sendProto2Map(cmd);
	}
}

bool CBeing::decBati(UINT32 bati)
{
	if (!m_bati){
		return false;
	}

	if (m_bati < bati){
		bati = m_bati;
	}
	
	UINT32 oldVal = m_bati;

	m_bati -= bati;

	if (getMap() && getMap()->isCopyMap()){
		Map::stNotifyObjBati cmd;
		cmd.set_motype(getType());
		cmd.set_moid(getUniqueID());
		cmd.set_num(m_bati);
		getMap()->sendProto2Map(cmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("(name:%s,)扣除了霸体:%d,当前霸体是:%d",getName(),bati,m_bati);
#endif
	}

	if ((oldVal > 0) && (m_bati <= 0)){//扣到0了
		batiEmpty();	
	}
	return true;
}

UINT32 CBeing::calcDamage(CBeing * pDefer)
{
	if (!pDefer) {
		return 0;
	}
	UINT32 dam = 0;
	stBufferState & attBuff =  m_buffData;
	stBufferState & defBuff = pDefer->m_buffData;
	stPKCalcData & attData = m_pkData;
	stPKCalcData & defData = pDefer->m_pkData;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("计算技能buff之前的战斗属性:atter(m_damagePer:%d,m_damagePropPer:%d,m_hitRate:%d,m_refBang:%d) defer:(m_damagePer:%d,m_damagePropPer:%d,m_hitRate:%d,m_refBang:%d)",attBuff.m_damagePer,attBuff.m_damagePropPer,attBuff.m_hitRate,attBuff.m_refBang,defBuff.m_damagePer,defBuff.m_damagePropPer,defBuff.m_hitRate,defBuff.m_refBang);
#endif
	attData.m_damage = (UINT32)(attData.m_damage * std::max((1 + attBuff.m_damagePropPer / 100.0f),0.0f));
	//buff作用应该是加法运算而非乘法，质疑下两行代码  --李德振 2017-1-5
	// defData.m_antiBangRate = (UINT32)(defData.m_antiBangRate * std::max(1 + defBuff.m_refBang / 100.0f,0.0f));	
	// attData.m_hitRate = (UINT32)(attData.m_hitRate * std::max(1 + attBuff.m_hitRate / 100.0f,0.0f));
#ifdef _HDZ_DEBUG
	Athena::logger->trace("计算技能buff之后的战斗属性:atter(m_damagePer:%d,m_damagePropPer:%d,m_hitRate:%d,m_refBang:%d) defer:(m_damagePer:%d,m_damagePropPer:%d,m_hitRate:%d,m_refBang:%d)",attBuff.m_damagePer,attBuff.m_damagePropPer,attBuff.m_hitRate,attBuff.m_refBang,defBuff.m_damagePer,defBuff.m_damagePropPer,defBuff.m_hitRate,defBuff.m_refBang);

#endif
	//先计算命中率
	//--李德振 2017-1-5
	UINT32  hitRate =  std::max(20,(int)(attData.m_hitRate + attBuff.m_hitRate - defData.m_dodgeRate - defBuff.m_dodge));	
	// UINT32  hitRate =  std::max(20,(int)(attData.m_hitRate - defData.m_dodgeRate));	
	if (g_selectByPercent(hitRate)){//是否命中
		//--李德振 2017-1-5
		UINT32 bangRate = std::max((INT32)attData.m_bangRate + attBuff.m_bang - (INT32)defData.m_antiBangRate - defBuff.m_refBang,0);		
		// UINT32 bangRate = std::max((INT32)attData.m_bangRate + attBuff.m_bang - (INT32)defData.m_antiBangRate,0);		
		float coef = 1.0f;
		if (g_selectByPercent(bangRate)){//出现暴击
			coef += (std::max((INT32)attData.m_bangDamage - (INT32)defData.m_antiBangDamge,0) / 100.0f);
			defData.m_attackResult |= Map::eAttResult_Bang;
		}
		
		dam = (UINT32)(attData.m_damage * ((attData.m_skillDamage)/ 10000.0f) * coef + attData.m_finalDamage);
		dam = dam > defData.m_damageReduce ? dam - defData.m_damageReduce : 1;
		dam += attData.m_skillAddDamage;
		//dam = std::max((INT32)(attData.m_damage * ((attData.m_skillDamage)/ 10000.0f) + attData.m_finalDamage - defData.m_damageReduce),(INT32)1) + attData.m_skillAddDamage;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("此次攻击:atter(name:%s,damage:%d,hitRate:%d,bangRate:%d,bangDamage:%d,skillDamage:%d),defer:(name:%s,antiHitRate:%d,antiBangRate:%d)计算出来的hitRate:%d,bangRate:%f",this->getName(),attData.m_damage,attData.m_hitRate,attData.m_bangRate,attData.m_bangDamage,attData.m_skillDamage,pDefer->getName(),defData.m_dodgeRate,defData.m_antiBangRate,hitRate,coef);
#endif
		//再加一点随机
		dam = (UINT32)(dam * (g_randBetween(95,105) / 100.0f));
	}
	else {
		defData.m_attackResult |= Map::eAttResult_Missing;
	}

	dam = (UINT32)(dam * std::max(defBuff.m_damagePer / 100.0f +1,0.0f));
	dam = dam == 0 ? 1: dam;	
	UINT32 realDam = pDefer->decHP(dam,this);
	
	if (this->getMap() && this->getMap()->isCopyMap()) {//在公会副本
		if ((this != pDefer) && pDefer->theNpc()){
			((CCopyMap*)this->getMap())->onNpcDecHP(this,realDam);	
		}
	}

	/*if (pDefer->getHP() <= 0){//处理死亡
		pDefer->processDeath(this);
	}*/
	return dam;
}

bool CBeing::move(CPos & pos,bool inInstant)
{
	if (!this->getMap()){
		return false;
	}
	return this->getMap()->move(this,pos,inInstant);
}

bool CBeing::isMoveColdDown(UINT64 uTime)
{
	if(uTime <= m_lastMoveTime +  getMoveSpeed()){
		return true;
	}

	return false;
}

bool CBeing::isAttackColdDown(UINT64 uTime)
{
	if(uTime <= m_lastAttackTime +  getAttackSpeed()){
		return true;
	}
	return false;
}


CBeing* CBeing::getCurTarget()
{
	CBeing* pBeing = NULL;
	if (m_curTargetType == CObj::OBJ_TYPE_PLAYER){
		pBeing = GameUserManager::getSingleton().getUser(m_curTargetID);
	}
	else if (m_curTargetType == CObj::OBJ_TYPE_MONSTER){
		pBeing = CNpcMgr::getSingleton().getNpcByTempID(m_curTargetID);
	}
	else if (m_curTargetType == CObj::OBJ_TYPE_HERO){
		pBeing = CHeroMgr::getSingleton().getHeroByTempID(m_curTargetID);	
	}
	return pBeing;
}

void CBeing::interruptSkillPlay(std::string nextAction)
{
	if (!getMap()){
		return ;
	}

	Map::stInterruptPlaySkill retCmd;
	retCmd.set_attid(getUniqueID());
	retCmd.set_atttype(getType());
	retCmd.set_nextaction(nextAction);
	getMap()->sendProto2Map(retCmd,getID());
}

void CBeing::changeDir(UINT32 dir)
{
	if (!getMap()){
		return ;
	}

	Map::stMapObjectChgDir retCmd;
	retCmd.set_motype(getType());
	retCmd.set_moid(getUniqueID());
	retCmd.set_dir(dir);
	getMap()->sendProto2Map(retCmd,getID());
}

CBeing * CBeing::getBeing(UINT8 type,UINT32 moID, CMap* pMap)
{
	CBeing * pRet = NULL;
	if (pMap != NULL)
	{
		pRet = pMap->getBeing(type, moID);
		if (pRet != NULL)
		{
			return pRet;
		}
	}
	if (CObj::OBJ_TYPE_PLAYER == type){
		if (moID > MAX_ROBOT_CHARID){
			pRet = GameUserManager::getSingleton().getUser(moID);
		}
		else {
			pRet = CRobotMgr::getSingleton().getRobotByID(moID);
#ifdef _HDZ_DEBUG
			if (!pRet){
				Athena::logger->trace("机器人id:%d,找不到",moID);
			}
#endif
		}
	}   
	else if (CObj::OBJ_TYPE_MONSTER == type){
		pRet = CNpcMgr::getSingleton().getNpcByTempID(moID);
	}
	else if (CObj::OBJ_TYPE_HERO == type){
		pRet = CHeroMgr::getSingleton().getHeroByTempID(moID);
	}   
	return pRet;
}

void  CBeing:: clearAllState()
{
	bool bDie = isDie();
	bzero(m_state,sizeof(m_state));
	if (bDie){
		setState(Map::BEING_STATE_DEATH);	
	}
}

bool CBeing::isRobot()
{
	if (this->theGameUser() && (getID() <= MAX_ROBOT_CHARID)){
		return true;
	}

	if ((getOwnerID() > 0) && ( getOwnerID() <= MAX_ROBOT_CHARID)){
		return true;
	}
	return false;
}

UINT32 CBeing::getRaiseSkillValue(UINT32 skillID, UINT32 raiseType){
	return 0;
}
