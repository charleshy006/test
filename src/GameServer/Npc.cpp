#include "Npc.h"
#include "Map.h"
#include "AthenaNew.h"
#include "TblDataMgr.h"
#include "Skill.h"
#include "NpcMgr.h"
#include "GameUserManager.h"
#include "CopyMap.h"
#include "AINpc.h"
#include "PathFinder.h"

CNpc::CNpc():m_oneSec(1)
{
	m_pRoleBase = NULL;
	m_pMonsterBase = NULL;
	m_pState = AIStateMgr::getSingleton().getState(AISTATE_IDLE);
	if (!m_pState){
		Athena::logger->error("创建NPC时候状态指针为空");
	}
	m_pNpcAI = NULL;
	m_bHasDropItem = false;
	m_damage = 0;						//伤害
	m_antiBangDamge = 0;				//抗暴击伤害
	m_antiBangRate = 0;					//抗暴击概率
	m_bangDamage = 0;					//暴击伤害
	m_bangRate = 0;						//暴击概率
	m_damageReduce = 0;					//伤害减免
	m_finalDamage = 0;					//最终伤害
	m_hitRate = 0;						//命中率
	m_dodgeRate = 0;					//闪避率
	m_bIsBoss = false;
	m_bornAminID = 0;
	m_bornDir = 0;
	m_lastHitByNormalTime = 0;
	m_batiEmptyTime = 0;
	
	m_fireRate = 0;                      //火强概率
	m_iceRate = 0;                       //冰冻概率
	m_thunderRate = 0;                   //雷电概率
	m_refFireRate = 0;                   //火强概率免疫
	m_refIceRate = 0;                    //冰冻概率免疫
	m_refThunderRate = 0;                //雷电概率免疫
	m_fireDamage = 0;                    //火伤害
	m_iceDamage = 0;                     //冰冻伤害
	m_thunderDamage = 0;                 //雷电伤害
	m_refFireDamage = 0;                 //火伤害免疫
	m_refIceDamage = 0;                  //冰伤害免疫
	m_refThunderDamage = 0;              //雷电伤害免疫
	m_clearMeTime = (UINT32)-1;			 //清空自己时间					
}

CNpc::~CNpc()
{
	SAFE_DELETE(m_pNpcAI);
}

void CNpc::getAppear(Map::NotifyNpcAppear::stNpcData & data)
{
	data.set_npcid(getID());
	data.set_moid(getTempID());
	data.set_x(getPos().m_x);
	data.set_y(getPos().m_y);
	data.set_z(getPos().m_z);
	data.set_hp(getHP());
	data.set_level(m_level);
	data.set_bornaminid(m_bornAminID);
	data.set_isboss(m_bIsBoss);
	data.set_dirangle(m_bornDir);
	data.set_actiongroupid(m_actionGroupID);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("怪物(id:%d,name:%s)的初始血量:%d",getID(),getName(),getHP());
	//Athena::logger->trace("发送的npcID=%d,name=%s,moID=%d,x=%d,y=%d,z=%d,hp=%d,levvel=%d",data.npcid(),data.name().c_str(),data.moid(),data.x(),data.y(),data.z(),data.hp(),data.level());
#endif
}

bool CNpc::heartBeat( UINT64 uTime)
{
	CBeing::heartBeat(uTime);
	if (m_bIsBoss){//如果是boss
		if (m_oneSec(g_gameServer->getRealTime())) {
			if (getMap() && getMap()->isCopyMap()){ 
				if(m_lastHitByNormalTime + 4 < g_gameServer->getSec()){//4秒内不收到攻击要回复
					if (m_batiEmptyTime && (m_batiEmptyTime + 8 > g_gameServer->getSec())){//霸体变空了,8秒内不加霸体

					}           
					else {      
						UINT32 addNum = (UINT32)(this->getMaxBati() * 0.1f); 
						this->addBati(addNum);
#ifdef _HDZ_DEBUG
						Athena::logger->trace("boss4秒没有收到攻击,回复了霸体");
#endif
					}           
				}               

				if (m_batiEmptyTime && (m_batiEmptyTime + 8 < g_gameServer->getSec())){//霸体回复
					m_batiEmptyTime = 0;
					this->addBati(this->getMaxBati());
#ifdef _HDZ_DEBUG
					Athena::logger->trace("霸体空后,8秒后回复满霸体");
#endif
				}               
			}
		}
	}
	return true;
}

bool CNpc::init(UINT32 id,UINT16 level)
{
	m_pRoleBase = g_roleBaseMgr.get(id);
	if (!m_pRoleBase){
		Athena::logger->error("初始化id=%d,失败,找不到该npc的基础配置",id);
		return false;	
	}
	
	m_pMonsterBase = g_monsterBaseMgr.get(stMonsterBase::getFindID(id,level));
	
	if (!m_pMonsterBase) {
		Athena::logger->error("召唤Npc id=%d失败,找不到等级属性配置",id);
		return false;
	}
	

	UINT32 tempID = 0;
	if (!CNpcMgr::getSingleton().genTempID(tempID)){
		Athena::logger->error("初始化npc时候获得临时id失败");
		return false;
	}
	m_level = level;
	setupState();	
	setID(m_pRoleBase->m_id);
	setName(m_pRoleBase->m_name);
	setTempID(tempID);
	m_hp = m_pMonsterBase->m_hp;
	m_damage = m_pMonsterBase->m_damage;						//伤害
	m_antiBangDamge = m_pMonsterBase->m_antiBangDamge;			//抗暴击伤害
	m_antiBangRate = m_pMonsterBase->m_antiBangRate;			//抗暴击概率
	m_bangDamage = m_pMonsterBase->m_bangDamage;				//暴击伤害
	m_bangRate = m_pMonsterBase->m_bangRate;					//暴击概率
	m_damageReduce = m_pMonsterBase->m_damageReduce;			//伤害减免
	m_finalDamage = m_pMonsterBase->m_finalDamage;				//最终伤害
	m_hitRate = m_pMonsterBase->m_hitRate;						//命中率
	m_dodgeRate = m_pMonsterBase->m_dodgeRate;					//闪避率
	m_maxHP= m_lastHP = m_hp;
	m_maxBati = m_bati = m_pMonsterBase->m_baTi;
	
	m_fireRate = m_pMonsterBase->m_fireRate;                    //火强概率
	m_iceRate = m_pMonsterBase->m_iceRate;                      //冰冻概率
	m_thunderRate = m_pMonsterBase->m_thunderRate;              //雷电概率
	m_refFireRate = m_pMonsterBase->m_antiFireRate;             //火强概率免疫
	m_refIceRate = m_pMonsterBase->m_antiIceRate;               //冰冻概率免疫
	m_refThunderRate = m_pMonsterBase->m_antiThunderRate;       //雷电概率免疫
	m_fireDamage = m_pMonsterBase->m_fireDamage;                //火伤害
	m_iceDamage = m_pMonsterBase->m_iceDamage;                  //冰冻伤害
	m_thunderDamage = m_pMonsterBase->m_thunderDamage;          //雷电伤害
	m_refFireDamage = m_pMonsterBase->m_antiFireDamage;         //火伤害免疫
	m_refIceDamage = m_pMonsterBase->m_antiIceDamage;           //冰伤害免疫
	m_refThunderDamage = m_pMonsterBase->m_antiThunderDamage;   //雷电伤害免疫

	createSkill();
	this->m_skillMgr.throwAllPassiveSkill();
	
	NpcAIBase * pAIBase = g_npcAIBaseMgr.get(1);
	if (pAIBase) {
		setAI(ATHENA_NEW CAINpc(this,pAIBase));
	}
	return true;
}

void CNpc::initHP(UINT32 hp)
{
	m_lastHP = m_hp = hp;	
}

void CNpc::processDeath(CBeing *pAtter)
{
	if (!this->getMap()){
		return ;
	}
	
	if (this->isSetState(Map::BEING_STATE_DEATH)){
		return ;
	}

	this->setState(Map::BEING_STATE_DEATH);
	//this->setClearState();
	
	if (this->getAI()){
		this->getAI()->setDeathTime(g_gameServer->getMSec());
		this->getAI()->onDie();
	}

	if (this->getMap()->isCopyMap()){
		((CCopyMap *)this->getMap())->onNpcDie(this,pAtter);	
	}

	Map::stNotifyObjDie dieCmd;
	dieCmd.set_motype(this->getType());
	dieCmd.set_moid(this->getUniqueID());
	getMap()->sendProto2Map(dieCmd);
	setClearState();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name=%s,ID=%d死亡了",getName(),getID());
#endif
}

void CNpc::createSkill()
{
	if (!m_pRoleBase || !m_pMonsterBase) {
		return ;
	}

	CSkill * pSkill = CSkillMgr::createSkill(m_pRoleBase->m_normalSkill,0);
	if (pSkill){//普通攻击
		if (!m_skillMgr.addSkill(pSkill)){
			SAFE_DELETE(pSkill);
			Athena::logger->error("怪物的普通攻击添加到容器失败!");
		}   
	}   
	else {
		// Athena::logger->error("怪物的普通攻击创建不成功!");
	}  
	
	std::vector<UINT32> skillIDVec;
	
	if (m_pRoleBase->m_skill1){
		skillIDVec.push_back(m_pRoleBase->m_skill1);
	}
	
	if (m_pRoleBase->m_skill2){
		skillIDVec.push_back(m_pRoleBase->m_skill2);
	}
	
	if (m_pRoleBase->m_skill3){
		skillIDVec.push_back(m_pRoleBase->m_skill3);
	}
	
	if (m_pRoleBase->m_skill4){
		skillIDVec.push_back(m_pRoleBase->m_skill4);
	}

	if (m_pRoleBase->m_skill5){
		skillIDVec.push_back(m_pRoleBase->m_skill5);
	}
	
	if (m_pRoleBase->m_skill6){
		skillIDVec.push_back(m_pRoleBase->m_skill6);
	}
	
	if (m_pRoleBase->m_skill7){
		skillIDVec.push_back(m_pRoleBase->m_skill7);
	}
	
	if (m_pRoleBase->m_skill8){
		skillIDVec.push_back(m_pRoleBase->m_skill8);
	}

	for (UINT16 i = 0;i < skillIDVec.size();++i){
		pSkill = CSkillMgr::createSkill(skillIDVec[i],0);
		if (pSkill){
			if (!m_skillMgr.addSkill(pSkill)){
				SAFE_DELETE(pSkill);
				Athena::logger->error("怪物(id=%d,name=%s)的技能:%d添加到容器失败!",getID()	,getName(),skillIDVec[i]);
			}	
		}
		else {
			Athena::logger->trace("怪物(id=%d,name=%s)创建技能:%d失败!",getID() ,getName(),skillIDVec[i]);	
		}
	}
}

void CNpc::setupState()
{
	if (!m_pRoleBase || !m_pMonsterBase) {
		return ;
	}
}

UINT8  CNpc::getRelation(CBeing * pOther)
{ 
	if (!pOther) {
		return 0;
	}

	if (this == pOther) {
		return TARGET_SELF;	
	}
	else if (getType() == pOther->getType()){//友方
		return TARGET_FRIEND;
	}
	else if (getType() != pOther->getType()){
		return TARGET_EMENY;
	}
	else {
		return 0;
	}
}

void CNpc::calcPKData()
{
	m_pkData.m_damage = m_damage;
	m_pkData.m_antiBangDamge = m_antiBangDamge; 
	m_pkData.m_antiBangRate =  m_antiBangRate ;
	m_pkData.m_bangDamage = m_bangDamage;
	m_pkData.m_bangRate =  m_bangRate;
	m_pkData.m_damageReduce = m_damageReduce;
	m_pkData.m_finalDamage =  m_finalDamage;
	m_pkData.m_hitRate = m_hitRate;
	m_pkData.m_dodgeRate = m_dodgeRate;
	m_pkData.m_fireRate = m_fireRate;
	m_pkData.m_iceRate = m_iceRate;	
	m_pkData.m_thunderRate = m_thunderRate;
	m_pkData.m_refFireRate = m_refFireRate;
	m_pkData.m_refIceRate = m_refIceRate;
	m_pkData.m_refThunderRate = m_refThunderRate;
	m_pkData.m_iceDamage = m_iceDamage;
	m_pkData.m_fireDamage = m_fireDamage;
	m_pkData.m_thunderDamage = m_thunderDamage;
	m_pkData.m_refFireDamage = m_refFireDamage;
	m_pkData.m_refIceDamage = m_refIceDamage;
	m_pkData.m_refThunderDamage = m_refThunderDamage;
}

void CNpc::setAI(CAINpc *  pNpcAI)
{
	if (m_pNpcAI){
		SAFE_DELETE(m_pNpcAI);
	}
	m_pNpcAI = pNpcAI;
}

CAINpc * CNpc::getAI()
{
	return m_pNpcAI; 
}

void CNpc::doAIAction(UINT64 uTime)
{
	if (!m_pState || !m_pNpcAI){
		return;
	}
	return ;
	m_pState->doAction(m_pNpcAI,uTime);	
}

void CNpc::changeState(ENUM_AI_STATE eState)
{
	if (!m_pState){
		Athena::logger->error("切换状态时候错误,改状态指针为空");
		return ;
	}

	if(eState == m_pState->getStateType() ) {//同一个状态
		return ;
	}   

	AIState* s = AIStateMgr::getSingleton().getState(eState);
	if (s) {
		m_pState = s;
	}   
}

void CNpc::trigerSkill()
{}

INT16 CNpc::getEnemies(std::vector<CBeing*>& enemies,INT16 radius)
{
	struct EnemyCallBack:public MapObjCallBack
	{
		CBeing* m_being;
		std::vector<CBeing*>& m_result;
		UINT16 m_radius;
		
		EnemyCallBack(CBeing* b,std::vector<CBeing*>& enemiesList,UINT16 r):
			m_being(b),
			m_result(enemiesList),
			m_radius(r)
		{
		}

		bool operator()(CObj* pObj)
		{
			CCellIdx myIdx(m_being->getPos());
			CCellIdx otherIdx(pObj->getPos());

			if(	pObj!=m_being	//不是自己	
					&&pObj->isBeing()				
					&&(!((CBeing*)pObj)->isDie()) //还活着
					&&(m_being->getMap()->getDistance(myIdx,otherIdx) <= m_radius)//距离满足要求
					&&m_being->isEnemy((CBeing*)pObj)//属于敌对关系
			  )
			{
				m_result.push_back((CBeing*)pObj);
			}
			return true;
		}
	};

	EnemyCallBack callBack(this,enemies,radius);
	getMap()->execEveryObjInMap(callBack,CObj::OBJ_TYPE_PLAYER);
	return enemies.size();
}

void CNpc::dropItem()
{}

bool CNpc::relive()
{
	return true;
}

void CNpc::speak()
{
	
}

bool CNpc::isEnemy(CBeing* pObj)
{
	if (!pObj){
		return false;
	}

	if (pObj == this){
		return false;
	}

	if (pObj->theGameUser() || pObj->theHero()){
		return true;
	}
	else {
		return false;
	}
}

bool CNpc::move( INT8 dir,INT16 dist)
{
	//判断某个点是否可以移动
	struct MapMoveAbleCallBack:public MapMoveAbleExec
	{
		MapMoveAbleCallBack(CMap* pM):m_pMap(pM)
		{

		}

		bool operator()(const CCellIdx& pos)
		{
			return !m_pMap->checkBlock(pos);
		}
		CMap* m_pMap;
	};

	CPos curPos = this->getPos();
	CCellIdx curIndex(curPos);
	static CCellIdx posNode[MAX_CHAR_PATH_NODE_NUMBER];
	int numNode = 0;

	//按照方格寻路
	MapMoveAbleCallBack callBack(getMap());
	getMap()->getPathFinder()->findPath(curIndex,dir,dist,callBack,posNode,numNode);
	if(numNode && (!getMap()->checkBlock(posNode[numNode - 1])))
	{
		//记录下导航路径:底层接口有9屏发送
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[NPC移动]%u(%d,%d) --> (%d,%d)",getTempID(),curIndex.m_x,curIndex.m_z,posNode[numNode-1].m_x,posNode[numNode-1].m_z);		
#endif
		CCellPos newPos(posNode[numNode-1]);
		newPos.pos.m_y = this->getPos().m_y;
		if(getMap()->move(this,newPos.pos,false))
		{
			return true;
		}
	}
	return true;
}

bool CNpc::telePort(const CPos& tarPos)
{
	//判断某个点是否可以移动
	CCellIdx tarIdx(tarPos);
	if(!getMap()->checkBlock(tarIdx,getDriveLevel()))
	{
#ifdef _HDZ_DEBUG
		CCellIdx srcIdx(this->getPos());
		Athena::logger->trace("[NPC瞬移]%u(%d,%d) --> (%d,%d)",this->getID(),srcIdx.m_x,srcIdx.m_z,tarIdx.m_x,tarIdx.m_z);	
#endif
		CPos pos(tarPos);
		pos.m_y = this->getPos().m_y;
		return getMap()->move(this,pos,true);
	}
	return false;
}

bool CNpc::move(const CPos& tarPos,bool useAStar)
{
	struct MapMoveAbleCallBack:public MapMoveAbleExec
	{
		MapMoveAbleCallBack(CMap* pM):_pMap(pM)
		{

		}

		bool operator()(const CCellIdx& pos)
		{
			return !_pMap->checkBlock(pos);
		}
		CMap* _pMap;
	};

	CPos curPos = this->getPos();
	CPos destPos = tarPos;
	int numNode = 0;
	static CCellIdx posNode[MAX_CHAR_PATH_NODE_NUMBER];
	CCellIdx curIndex(curPos);
	CCellIdx tarIndex(tarPos);

	if(curIndex == tarIndex)
	{
		return getMap()->move(this,destPos,false);
	}
	else
	{              
		MapMoveAbleCallBack callBack(getMap());
		getMap()->getPathFinder()->findPath(curIndex,tarIndex,callBack,posNode,numNode);
		if(numNode >= 2 && (!getMap()->checkBlock(posNode[numNode-2]))){
			CCellPos newPos(posNode[numNode-2]);
			newPos.pos.m_y = curPos.m_y;
			if (getMap()->move(this,newPos.pos,false)){
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[NPC移动]%u(%d,%d) --> (%d,%d)",getTempID(),curIndex.m_x,curIndex.m_z,posNode[numNode-2].m_x,posNode[numNode-2].m_z);
#endif
				return true;
			}
		}
	}
	return true;
}

void   CNpc::hitByNormal()
{
	    m_lastHitByNormalTime = g_gameServer->getSec(); 
}

void   CNpc::batiEmpty()
{
	    m_batiEmptyTime = g_gameServer->getSec();   
}

bool CNpc::canClearMe()
{
	if (m_clearMeTime < g_gameServer->getSec()){
		return true;	
	}	
	else {
		return false;
	}
}
