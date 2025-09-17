#include "Hero.h"
#include "GameUser.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "Map.h"
#include "GameUserManager.h"
#include "HeroMgr.h"
#include "CopyMap.h"
#include <tinyxml/tinyxml.h>

CHero::CHero()
{
	m_ownerID = 0;
	m_level = 0;
	m_evolutionLv = 0;
	m_pRoleBase = NULL;
	m_pHeroLvPropBase = NULL;
	m_pEvolutionBase = NULL;
	m_damage = 0;						//伤害
	m_antiBangDamge = 0;				//抗暴击伤害
	m_antiBangRate = 0;					//抗暴击概率
	m_bangDamage = 0;					//暴击伤害
	m_bangRate = 0;						//暴击概率
	m_damageReduce = 0;					//伤害减免
	m_finalDamage = 0;					//最终伤害
	m_hitRate = 0;						//命中率
	m_dodgeRate = 0;					//闪避率
	m_clearMeTime = (UINT32)-1;
	m_summonBySkill = false;

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
	m_damagePer = 100;
}

CHero::~CHero()
{}

void CHero::getAppear(Map::NotifyHeroAppear::stHeroData & pData)
{
	pData.set_heroid(getID());
	pData.set_moid(getTempID());
	pData.set_x(getPos().m_x);
	pData.set_y(getPos().m_y);
	pData.set_z(getPos().m_z);
	pData.set_hp(this->getHP());
	pData.set_maxhp(getMaxHP());
	pData.set_mastermoid(m_ownerID);
	pData.set_campid(m_campID);
	if (m_pHeroLvPropBase) {
		pData.set_level(m_pHeroLvPropBase->m_level);
	}
	pData.set_createbyskill(m_summonBySkill);
#ifdef _HDZ_DEBUG
    Athena::logger->trace("发送的武将血量为%d,heroId=%d,mastermoid=%d",getHP(),getID(),m_ownerID);
#endif
	GameUser * pUser = GameUserManager::getSingleton().getUser(m_ownerID);
    if (!pUser) {
        pData.set_pos(0);
        return ;
    }
    HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(getID());
	if (!pHeroInfo){
        pData.set_pos(0);
        return ;
    }
    pData.set_pos(pHeroInfo->m_dbData.m_pos);
}

bool CHero::heartBeat( UINT64 uTime)
{
	CBeing::heartBeat(uTime);
	return true;
}

void CHero::processDeath(CBeing *pAtter)
{
	if (!this->getMap()){
		return ;
	}
	if (this->isSetState(Map::BEING_STATE_DEATH)){
		return ;
	}
	this->setState(Map::BEING_STATE_DEATH);
	
	GameUser * pOwner = GameUserManager::getSingleton().getUser(m_ownerID);
	if (pOwner){
		pOwner->m_heroMgr.updateBattleHeroHP(getID(),0);
	}

	Map::stNotifyObjDie dieCmd;
	dieCmd.set_motype(this->getType());
	dieCmd.set_moid(this->getUniqueID());
	getMap()->sendProto2Map(dieCmd);
	setClearState();
}

void CHero::setupState()
{
}

void CHero::printData()
{
}

bool CHero::init(UINT32 heroID,UINT32 ownerID,UINT16 level,UINT16 evolutionLv, CMap* pMap)
{
	 CBeing* pBeing = CBeing::getBeing(CObj::OBJ_TYPE_PLAYER,ownerID, pMap);//GameUserManager::getSingleton().getUser(ownerID);
	if (!pBeing){
		Athena::logger->error("召唤宠物失败,宠物主人不在线!");
		return false;
	}
	
	GameUser *pOwner = pBeing->theGameUser();
	if (!pOwner){
		return false;
	}
	
	/*HeroInfo *pHeroInfo = pOwner->m_heroMgr.getHeroInfoByID(heroID);
	
	if (!pHeroInfo){
		Athena::logger->error("召唤宠物失败,宠物信息不存在!");
		return false;
	}*/
	m_level = level;
	m_evolutionLv = evolutionLv;

	m_pRoleBase = g_roleBaseMgr.get(heroID);;
	if (!m_pRoleBase){
		Athena::logger->error("初始化id=%d,失败,找不到该Hero的基础配置",heroID);
		return false;   
	}   

	m_pHeroLvPropBase = g_heroLvPropBaseMgr.get(stHeroLvPropBase::genFindID(heroID,level));

	if (!m_pHeroLvPropBase){
		Athena::logger->error("初始化宠物id=%d等级数据失败!",heroID);
		return false;
	}
	
	m_pEvolutionBase = g_heroEvolutionBaseMgr.get(stHeroEvolutionBase::getFindID(heroID,evolutionLv));
	if (!m_pEvolutionBase){
		Athena::logger->error("初始化宠物id=%d的进化数据失败",heroID);
		return false;
	}

	UINT32 tempID = 0;
	if (!CHeroMgr::getSingleton().genTempID(tempID)){
		Athena::logger->error("初始化Hero时候获得临时id失败");
		return false;
	}   

	setupState();
	m_ownerID = ownerID;

	m_campID = pOwner->getCampID();	

	setID(m_pRoleBase->m_id);
	setName(m_pRoleBase->m_name);
	setTempID(tempID);
	
	HeroInfo * pHeroInfo = pOwner->m_heroMgr.getHeroInfoByID(heroID);
	if (pHeroInfo){
		m_maxHP= m_lastHP = m_hp=pHeroInfo->m_maxHP;	
		m_damage = pHeroInfo->m_damage; 
	}
	else {
		m_maxHP= m_pHeroLvPropBase->m_maxHP ;
		for (UINT16 i = 0;i < m_pEvolutionBase->m_propVec.size();++i){
			stEvolutionAddProp & prop = m_pEvolutionBase->m_propVec[i];
			if (eEvolution_Prop_Target_Hero == prop.m_propTarget) {
				if (eBATTLE_PROP_HP == prop.m_propID){//加血
					m_maxHP +=  prop.m_value;
				}   

				if (eBATTLE_PROP_DAMAGE == prop.m_propID){//加战力
					m_damage += prop.m_value;
				}   
			}   
		}
		m_lastHP = m_hp = m_maxHP;
	}
	
	m_antiBangDamge = m_pHeroLvPropBase->m_antiBangDamge;	//抗暴击伤害
	m_antiBangRate = m_pHeroLvPropBase->m_antiBangRate;	//抗暴击概率
	m_bangDamage = m_pHeroLvPropBase->m_bangDamage;		//暴击伤害
	m_bangRate = m_pHeroLvPropBase->m_bangRate;			//暴击概率
	m_damageReduce = m_pHeroLvPropBase->m_damageReduce;	//伤害减免
	m_finalDamage = m_pHeroLvPropBase->m_finalDamage;		//最终伤害
	m_hitRate = m_pHeroLvPropBase->m_hitRate;				//命中率
	m_dodgeRate = m_pHeroLvPropBase->m_dodgeRate;			//闪避率

	m_fireRate = m_pHeroLvPropBase->m_fireRate;                    //火强概率
	m_iceRate = m_pHeroLvPropBase->m_iceRate;                      //冰冻概率
	m_thunderRate = m_pHeroLvPropBase->m_thunderRate;              //雷电概率
	m_refFireRate = m_pHeroLvPropBase->m_antiFireRate;             //火强概率免疫
	m_refIceRate = m_pHeroLvPropBase->m_antiIceRate;               //冰冻概率免疫
	m_refThunderRate = m_pHeroLvPropBase->m_antiThunderRate;       //雷电概率免疫
	m_fireDamage = m_pHeroLvPropBase->m_fireDamage;                //火伤害
	m_iceDamage = m_pHeroLvPropBase->m_iceDamage;                  //冰冻伤害
	m_thunderDamage = m_pHeroLvPropBase->m_thunderDamage;          //雷电伤害
	m_refFireDamage = m_pHeroLvPropBase->m_antiFireDamage;         //火伤害免疫
	m_refIceDamage = m_pHeroLvPropBase->m_antiIceDamage;           //冰伤害免疫
	m_refThunderDamage = m_pHeroLvPropBase->m_antiThunderDamage;   //雷电伤害免疫

	//创建普通攻击
	CSkill * pSkill = CSkillMgr::createSkill(m_pRoleBase->m_normalSkill,0);
	if (pSkill){//普通攻击
		if (!m_skillMgr.addSkill(pSkill)){
			SAFE_DELETE(pSkill);
			// Athena::logger->error("宠物(id:%d,name:%s)的普通攻击(skill:%d)添加到容易失败!",getID(),getName(),m_pRoleBase->m_normalSkill);
		}	
	}
	else {
		// Athena::logger->error("宠物(id:%d,name:%s)的普通攻击(skill:%d)创建不成功!",getID(),getName(),m_pRoleBase->m_normalSkill);
	}

	if(heroID != 8116121){
		for (UINT8 i = 0;i < MAX_SKILL_NUM;++i){
			UINT32 skillID = pHeroInfo->m_dbData.m_skillVec[i].m_skillID;
			UINT16 skillLv = pHeroInfo->m_dbData.m_skillVec[i].m_skillLv;
	
			if (skillID > 0){
				pSkill = CSkillMgr::createSkill(skillID,skillLv);
				if (!pSkill){
					Athena::logger->error("宠物出现时候创建技能不成功skillID=%d,skillLv=%d!",skillID,skillLv);
					continue;
				}
	
				if (!m_skillMgr.addSkill(pSkill)){//添加失败
					Athena::logger->error("宠物技能添加到容器失败,skillID=%d,skillLv=%d",skillID,skillLv);
					SAFE_DELETE(pSkill);
					continue;
				}
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("(name:%s,id:%d)创建技能:skill=%d,skillLv=%d",getName(),getID(),skillID,skillLv);
#endif
		}
	}else{
		m_clearMeTime = g_gameServer->getSec() + 25;
	}
	return true;
}

void CHero::calcPKData()
{
	m_pkData.m_damage = (UINT32)(m_damage * (m_damagePer / 100.0f));	
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

void CHero::onEnterMap(CMap *pMap)
{
	if (!pMap){
		return ;
	}

	if (pMap->isCopyMap()){
		((CCopyMap*)pMap)->onHeroEnter(this);
	}   
}

void CHero::summonSelf(UINT32 lastTime,UINT32 hpPer,UINT32 damagePer)
{
	if (!getMap()){
		return ;
	}
	CPos bornPos = getPos();
	UINT32 curHP = (UINT32)(hpPer / 100.0f * m_maxHP);
	m_damagePer = damagePer;
	CHero * pHero = getMap()->summonHero(getID(),m_ownerID,m_level,m_evolutionLv,bornPos,curHP,true);	
	if (pHero){
		pHero->setClearMeTime(g_gameServer->getSec() + lastTime);
	}
}

bool CHero::canClearMe()
{
	if (m_clearMeTime < g_gameServer->getSec()){
		return true;	
	}	
	else {
		return false;
	}
}

