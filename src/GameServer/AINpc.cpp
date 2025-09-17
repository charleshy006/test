#include "AINpc.h"
#include "Map.h"
#include "Being.h"
#include "NpcMgr.h"
#include "GameUserManager.h"
#include "HeroMgr.h"
#include "PathFinder.h"

struct stCheckMapBlockCB : public MapMoveAbleExec
{
	CMap * m_pCurMap;

	stCheckMapBlockCB(CMap * pMap) : m_pCurMap(pMap)
	{
	
	}
	
	virtual ~stCheckMapBlockCB()
	{}

	virtual bool operator()(const CCellIdx& pos)
	{
		if (!m_pCurMap){
			return true;
		}
		return m_pCurMap->checkBlock(pos,BLOCK_TYPE_NPC);
	}
};

CAINpc::CAINpc(CNpc * pNpc,NpcAIBase * pNpcAIBase) :m_pOwner(pNpc),m_pNpcAIBase(pNpcAIBase)
{
	m_lifeTimes = 0;
	m_deathTime = 0;
}

void CAINpc::changeState(ENUM_AI_STATE eState)
{
	m_pOwner->changeState(eState);
}

bool CAINpc::isOutOfRange()
{	
	//是否离开了活动区域
	CCellIdx curIdx(m_pOwner->getPos());

	if(!m_activeArea.isInArea(curIdx))
		return true;

	return false;
}

void CAINpc::setActiveArea( const CRectArea& area )
{ 
	m_activeArea = area;
#ifdef _HDZ_DEBUG
	//Athena::logger->trace("设置了NPC移动区域:x=%d,y=%d,width=%d,height=%d",area.pos.m_x,area.pos.m_y,area.width,area.height);
#endif
}

bool CAINpc::isArrived(const CCellIdx& dest)
{
	if (!m_pOwner->getMap()){
		return false;
	}

	CCellIdx curIdx(m_pOwner->getPos());
	//计算记录
	INT16 distance = m_pOwner->getMap()->getDistance(dest,curIdx);
	if(distance <= 3){
		return true;
	}
	return false;
}

bool CAINpc::isArrived()
{
	if (!m_pOwner){
		return false;
	}
	CCellIdx curIdx(m_pOwner->getPos());
	if (m_destPos.m_x == curIdx.m_x) {
		return true;
	}	
	return false;
}

bool CAINpc::randomMove(INT16 step)//随机移动,移动步长，默认移动一格子
{
	return true;
}

bool CAINpc::runToEnemyAndAttack(CBeing* pBeing,UINT64 uTime)
{      
	if (!pBeing || !pBeing->getMap() || !m_pOwner){
		return false;
	}
	CCellIdx tarIdx(pBeing->getPos());
	CCellIdx srcIdx(m_pOwner->getPos());
	
	if (CMap::getDistance(tarIdx,srcIdx) > m_pOwner->attackDis()){
		if (!m_pOwner->isAttackColdDown(uTime)){
			if ( m_pOwner->move(pBeing->getPos(),true)){
				m_pOwner->setLastMoveTime(uTime);
				return true;
			}
		}
		return true;
	}

	//如果技能CD准备好了，则立即释放技能
	if(!skillColdDown(uTime)){
		m_pOwner->trigerSkill();
#ifdef _HDZ_DEBUG
		//Athena::logger->trace("npc的攻击点为x=%d,y=%d",attCellIdx.m_x,attCellIdx.m_y);
#endif
		m_lastSkillTime = uTime;
		return true;
	}
	return false;
} 

bool CAINpc::skillColdDown(UINT64 uTime)//技能冷却时间到
{
	return 5000 + m_lastSkillTime < uTime;//超过两秒
}

void CAINpc::actionIdle( UINT64 uTime )
{
	//主动怪，才会去搜索敌人
	std::vector<CBeing*> enemies;
	if(this->skillColdDown(uTime)) {
		//警戒
		m_pOwner->getEnemies(enemies,NPC_ALERT_RANGE);
	}	

	if(!enemies.empty()) {
		//将本次搜索的结果记录到队列中，便于下次直接搜索，而不是9屏搜索
		//如果有敌人，则随机选择一个
		UINT16 index = g_randBetween(0,enemies.size() - 1);
		//设置当前目标
		m_pOwner->setCurTarget(enemies[index]->getUniqueID(),enemies[index]->getType());
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[NPC AI]%u 空闲状态，发现敌人(%u)，进入战斗状态",m_pOwner->getTempID(),enemies[index]->getID());
#endif
		//进入战斗状态
		changeState(AISTATE_COMBAT);
	}
	else if (!m_pOwner->isMoveColdDown(uTime)){
		randomMove();	
		m_pOwner->setLastMoveTime(uTime);
	}
}

UINT64 CAINpc::getBodyClearTime()   //死亡时,尸体清除时间
{
	return m_pOwner->getCorpseTime() + m_deathTime;
}

UINT64 CAINpc::getReliveTime()//死亡后，复活时间，该时间必须比清除尸体时间晚一些
{
	return m_pOwner->getReliveTime()  + m_deathTime;
}

bool CAINpc::canClearAfterDie(UINT64 uTime)//可以消失了   
{
	return uTime >=  getBodyClearTime();
}

bool CAINpc::canReliveAfterDie(UINT64 uTime)//可以重生了
{
	return uTime >= getReliveTime();
}

void CAINpc::actionDead( UINT64 uTime )
{
	if (!m_pOwner){
		return ;
	}
	
	if(!m_pOwner->isDie())//是否为死亡状态
	{
		changeState(AISTATE_IDLE);//切换到空闲状态
		return ;
	}
	
	if (!m_pOwner->m_bHasDropItem){
		m_pOwner->dropItem();
		m_pOwner->m_bHasDropItem = true;
#ifdef _HDZ_DEBUG
		//Athena::logger->trace("NPC开始掉落物品");
#endif
	}

	//死亡阶段：清除尸体阶段
	if(getDieState() == DIE_STATE_NO)
	{
	}
	else if(getDieState() == DIE_STATE_ROT)
	{
		//到达清除尸体时间
		if(canClearAfterDie(uTime))
		{
		//清除尸体	
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[NPC AI]%u,%s 死亡状态，清除尸体",m_pOwner->getTempID(),m_pOwner->getName());
#endif
			/*Map::NotifyObjDisappear cmd;
			Map::NotifyObjDisappear_stObjDisappearInfo * disappearInfo = cmd.add_data();
			if (!disappearInfo){
				return ;
			} 

			disappearInfo->set_motype(m_pOwner->getType());
			disappearInfo->set_moid(m_pOwner->getUniqueID());
			if (m_pOwner->getMap()){
				m_pOwner->getMap()->sendProto2Nine(m_pOwner,cmd);
			}*/
			m_pOwner->setClearState();	
			//setDieState(DIE_STATE_RELIVE);
		}
	}
	else if(getDieState() == DIE_STATE_RELIVE)//尸体重生阶段
	{
		//如果生命次数为0了，则不进入重生阶段
		if(getLeftLifeTimes())
		{
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[NPC AI]%u,%s 死亡状态，重生",m_pOwner->getName());
#endif
			m_pOwner->relive();
			//减少生命
			m_lifeTimes --;
		}
		else
		{	
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[NPC AI]%u 死亡,清除回收",m_pOwner->getTempID());
#endif
			m_pOwner->setClearState();
		}
	}

}

//敌人选择规则,不同的控制器可能选取不同给则
CBeing* CAINpc::chooseEnemy()//得到一个敌人
{
	if (!m_pNpcAIBase){
		return NULL;
	}

	CBeing * pBeing = NULL;
	if (m_pNpcAIBase->m_targetType){//找仇恨值最高的
		for(std::set<AIEnemyItem>::iterator itr = m_enemiesList.begin() ;itr != m_enemiesList.end();++itr)
		{
			if(itr->type == CObj::OBJ_TYPE_PLAYER) {
				pBeing = GameUserManager::getSingleton().getUser(itr->objID);	
			}
			else if(itr->type == CObj::OBJ_TYPE_MONSTER) {
				pBeing = CNpcMgr::getSingleton().getNpcByTempID(itr->objID);
			}
			else if (itr->type == CObj::OBJ_TYPE_HERO){
				pBeing = CHeroMgr::getSingleton().getHeroByTempID(itr->objID);
			}

			if(pBeing){
				break;
			}		
		}

	}
	if (!pBeing){//没有仇恨值最高的人
		pBeing = m_pOwner->getCurTarget();	
	}
	//当前敌人是否还符合被攻击的要求
	if(pBeing && pBeing->isDie()){
		pBeing = NULL;
	}
	return pBeing;
}

void CAINpc::actionCombat( UINT64 uTime )
{
	CBeing* pBeing = chooseEnemy();

	if(pBeing) {
		//尝试靠近和攻击
		runToEnemyAndAttack(pBeing,uTime);
	}

	//如果出界,且没有攻击对象,需要回到活动范围
	if(isOutOfRange() && (!pBeing)) {
#ifdef _HDZ_DEBUG
		//Athena::logger->trace("[NPC AI]%u 战斗状态，追出边界,进入返回状态（GoHome）",m_pOwner->getTempID());
#endif
		changeState(AISTATE_GOHOME);
	}
	else if(!pBeing) {
			changeState(AISTATE_IDLE);
#ifdef _HDZ_DEBUG
		//Athena::logger->trace("[NPC AI]%u 战斗状态---》空闲状态",m_pOwner->getTempID());
#endif
	}
}

void CAINpc::actionGohome(UINT64 uTime)
{
	//判断当前位置是否已经在家附近
	//CRectArea area = getActiveArea();
	
	if(/*isArrived(area.m_pos)*/false)//是否到达附近
	{
#ifdef _HDZ_DEBUG
		//Athena::logger->trace("[NPC AI]%u 返回状态，到达目标(%d,%d)，进入空闲状态",m_pOwner->getTempID(),area.pos.m_x,area.pos.m_y);
#endif
		//清除攻击目标
		m_pOwner->setCurTarget(0,CObj::OBJ_TYPE_MAX);
		//如果是巡逻NPC，继续切换到巡逻状态
		changeState(AISTATE_IDLE);
	}
	else {
#ifdef _HDZ_DEBUG
		//Athena::logger->trace("[NPC AI]%u 返回状态，继续向目标(%d,%d)移动",m_pOwner->getTempID(),area.pos.m_x,area.pos.m_y);
#endif
	}
}

void CAINpc::onDie()
{
	//切换到死亡AI
	changeState(AISTATE_DEAD);
	//开始尸体腐烂
	setDieState(DIE_STATE_ROT);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[状态切换]进入死亡状态");
#endif
}

void  CAINpc::onHP(UINT32 lastPercent,UINT32 curPercent)
{
	if (!m_pNpcAIBase){
		return ;
	}	

	for (UINT16 i = 0;i < m_pNpcAIBase->m_trigerCond.size();++i){
		if ((AI_TRIGER_COND_HP == m_pNpcAIBase->m_trigerCond[i].first)
				&& (m_pNpcAIBase->m_trigerCond[i].second > curPercent )
				&& ( m_pNpcAIBase->m_trigerCond[i].second < lastPercent)){//触发了血量事件
			trigerEvent();			
			break;	
		}
	}
}

void CAINpc::trigerEvent()
{
	if (!m_pNpcAIBase){
		return ;
	}

	for (UINT16 i = 0;i < m_pNpcAIBase->m_trigerEvent.size();++i){
		switch (m_pNpcAIBase->m_trigerEvent[i]){
			case AI_TRIGER_EVENT_USESKILL ://使用技能
				{
					CBeing* pBeing = chooseEnemy();
					if (pBeing){
						m_pOwner->trigerSkill();					
					}
				}
				break;
			case AI_TRIGER_EVENT_SPEAK :
				{
					m_pOwner->speak();		
				}
				break;
		}	
	}	
}
