#include "HeroInfo.h"
#include "GameUser.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "Map.h"
#include "GameUserManager.h"
#include <tinyxml/tinyxml.h>
#include "HeroTrainCfg.h"

UINT8 HeroInfoMgr::g_goOutGroup[2][2] = { { eHERO_BATTLE_POS1 ,eHERO_BATTLE_POS3},{ eHERO_BATTLE_POS2 , eHERO_BATTLE_POS4}};			//出战位置

HeroInfo::HeroInfo(HeroInfoMgr  * pHeroInfoMgr)
{
	bzero(&m_dbData,sizeof(m_dbData));
	m_pHeroBase = NULL;
	m_pHeroLvPropBase = NULL;
	m_pEvolutionBase = NULL;
	m_pOwner = pHeroInfoMgr->getOwner();
	reset();
}

HeroInfo::~HeroInfo()
{

}

void HeroInfo::reset(bool clearall)
{
	m_battlePower = 0;
	m_maxHP = 0;
	m_damage = 0;
	m_addOwnerBattleProps.clear();
	m_addOwnerBattleProps.resize(eMAX_BATTLE_PROP);
	m_addHeroBattleProps.clear();
	m_addHeroBattleProps.resize(eMAX_BATTLE_PROP);
	m_battleProps.clear();
	m_battleProps.resize(eMAX_BATTLE_PROP);
	m_addProps.clear();
	if(m_impactProps.size()==0 || clearall)
	{
		m_impactProps.clear();
		m_impactProps.resize(eMAX_BATTLE_PROP);
	}
}

bool HeroInfo::init(bool clearall)
{
	reset(clearall);
	m_pHeroBase = g_roleBaseMgr.get(m_dbData.m_id);
	if (!m_pHeroBase) {
		Athena::logger->error("初始武将id=%d失败,找不到基础配置信息",m_dbData.m_id);
		return false;
	}
	
	m_pHeroLvPropBase = g_heroLvPropBaseMgr.get(stHeroLvPropBase::genFindID(m_dbData.m_id,m_dbData.m_level));
	if (!m_pHeroLvPropBase) {
		Athena::logger->error("初始化武将id=%d失败,找不到武将等级信息",m_dbData.m_id);
		return false;
	}
	
	m_pEvolutionBase = g_heroEvolutionBaseMgr.get(stHeroEvolutionBase::getFindID(m_dbData.m_id,m_dbData.m_evolutionLv));
	if (!m_pEvolutionBase){
		Athena::logger->error("初始化武将id=%d失败,找不到武将进化等级信息",m_dbData.m_id);
		return false;	
	}
	
	if (m_pHeroLvPropBase){
		//m_maxHP += m_pHeroLvPropBase->m_maxHP;
		//m_damage += m_pHeroLvPropBase->m_damage;
		m_battlePower += m_pHeroLvPropBase->m_battlePower;

		m_battleProps[eBATTLE_PROP_HP] += m_pHeroLvPropBase->m_maxHP;
		m_battleProps[eBATTLE_PROP_DAMAGE] += m_pHeroLvPropBase->m_damage;
		m_battleProps[eBATTLE_PROP_HITRATE] += m_pHeroLvPropBase->m_hitRate;
		m_battleProps[eBATTLE_PROP_DODGERATE] += m_pHeroLvPropBase->m_dodgeRate;
		m_battleProps[eBATTLE_PROP_BANGRATE] += m_pHeroLvPropBase->m_bangRate;
		m_battleProps[eBATTLE_PROP_BANGDAMAGE] += m_pHeroLvPropBase->m_bangDamage;
		m_battleProps[eBATTLE_PROP_ANTIBANGRATE] += m_pHeroLvPropBase->m_antiBangRate;
		m_battleProps[eBATTLE_PROP_ANTIBANGDAMAGE] += m_pHeroLvPropBase->m_antiBangDamge;
		m_battleProps[eBATTLE_PROP_FINALDAMAGE] += m_pHeroLvPropBase->m_finalDamage;
		m_battleProps[eBATTLE_PROP_RECOVERHP] += m_pHeroLvPropBase->m_recoverHP;
		m_battleProps[eBATTLE_PROP_ABSORBHP] += m_pHeroLvPropBase->m_absorbHP;
		m_battleProps[eBATTLE_PROP_REFLECTDAMAGE] += m_pHeroLvPropBase->m_reflectDamage;
		m_battleProps[eBATTLE_PROP_FIREDAMAGE] += m_pHeroLvPropBase->m_fireDamage;
		m_battleProps[eBATTLE_PROP_ICEDAMAGE] += m_pHeroLvPropBase->m_iceDamage;
		m_battleProps[eBATTLE_PROP_THUNDERDAMAGE] += m_pHeroLvPropBase->m_thunderDamage;
		m_battleProps[eBATTLE_PROP_FIRERATE] += m_pHeroLvPropBase->m_fireRate;
		m_battleProps[eBATTLE_PROP_ICERATE] += m_pHeroLvPropBase->m_iceRate;
		m_battleProps[eBATTLE_PROP_THUNDERRATE] += m_pHeroLvPropBase->m_thunderRate;
		m_battleProps[eBATTLE_PROP_DAMAGEREDUCE] += m_pHeroLvPropBase->m_damageReduce;
		m_battleProps[eBATTLE_PROP_MP] += m_pHeroLvPropBase->m_maxMP;
	}

	//加羁绊数据
	for (int i = eBATTLE_PROP_HP; i < eMAX_BATTLE_PROP; i++)
	{
		m_battleProps[i] += m_impactProps[i];
	}
	

	if (m_pEvolutionBase){//加进化数据
		for (UINT16 i = 0;i < m_pEvolutionBase->m_propVec.size();++i){
			stEvolutionAddProp & prop = m_pEvolutionBase->m_propVec[i];
			if (prop.m_propID < eMAX_BATTLE_PROP) {
				if (eEvolution_Prop_Target_Hero == prop.m_propTarget) {
					m_battleProps[prop.m_propID] += prop.m_value;
#ifdef _HDZ_DEBUG
					Athena::logger->trace("宠物增加了宠物的属性:propID=%d,propVal:%d",prop.m_propID,prop.m_value);
#endif
				}
				else if (eEvolution_Prop_Target_Owner == prop.m_propTarget) {
					m_addOwnerBattleProps[prop.m_propID] += prop.m_value;
#ifdef _HDZ_DEBUG
//					if (811615 == m_pEvolutionBase->m_id) {
					Athena::logger->trace("宠物增加了主人的属性:propID=%d,propVal:%d",prop.m_propID,prop.m_value);
//					}
#endif
				}
				else {
					//m_battleProps[prop.m_propID] += prop.m_value;
					m_addProps.push_back(prop);
#ifdef _HDZ_DEBUG
					Athena::logger->trace("影响其它宠物的属性:heroID=%d,propID=%d,propVal:%d",prop.m_propTarget,prop.m_propID,prop.m_value);
#endif
				}
			}
		}

		m_battlePower += m_pEvolutionBase->m_battlePower;
	}

	//宠物符文属性
	if(m_pOwner){
		UINT16 slotIndex = (UINT16)-1;
		for (size_t i = 0; i < MAX_HERO_RUNE_NUM; i++) {
			slotIndex = m_runeDbData.m_runeVec[i];
			if (slotIndex == (UINT16)-1) {
				continue ;
			}
			CItem * pItem = m_pOwner->m_bagMgr.m_heroRuneBag.getItemBySlot(slotIndex);
			if (!pItem) {
				continue ;
			}
			m_battlePower += pItem->m_battlePower;
			for (UINT8 i = 1; i < m_battleProps.size(); i++) {
				m_battleProps[i] += pItem->m_battleProps[i];
			}	
		}
	}

	std::map<UINT8,stHeroSkillInfo>::iterator it = m_skillCnt.begin();
	for (;it != m_skillCnt.end();++it){
		stHeroSkillInfo & skillInfo = it->second;
		SkillGrownBase * pSkillGrown =  g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillInfo.m_skillID,skillInfo.m_skillLv));
		if (pSkillGrown){
			if (pSkillGrown->m_battleProp.m_propID < eMAX_BATTLE_PROP){
				m_battleProps[pSkillGrown->m_battleProp.m_propID] += pSkillGrown->m_battleProp.m_val1;
#ifdef _HDZ_DEBUG
				Athena::logger->trace("宠物领悟技能添加的属性:propID:%d,val:%d",pSkillGrown->m_battleProp.m_propID,pSkillGrown->m_battleProp.m_val1);
#endif
			}
		}

		stSkillBase *pSkillBase = g_skillBaseMgr.get(skillInfo.m_skillID);
		if (pSkillBase){
			if (SKILL_QUALITY_GREEN == pSkillBase->m_quality){
				m_battlePower += (10 * (skillInfo.m_skillLv + 1));	
			}
			else if (SKILL_QUALITY_BLUE == pSkillBase->m_quality){
				m_battlePower += (15 * (skillInfo.m_skillLv + 1));	
			}
			else if (SKILL_QUALITY_PURPLE == pSkillBase->m_quality){
				m_battlePower += (20 * (skillInfo.m_skillLv + 1));
			}
			else if (SKILL_QUALITY_ORANGE == pSkillBase->m_quality){
				m_battlePower += (200 * (skillInfo.m_skillLv + 1));
			}
		}
	}
	m_maxHP = m_battleProps[eBATTLE_PROP_HP];
	m_damage = m_battleProps[eBATTLE_PROP_DAMAGE];
 
	return true;	
}

bool HeroInfo::addActiveSkill(UINT32 skillID)
{
	if (isActiveSkillExist(skillID)){
		return false;
	}
	
	for (UINT8 i = 0; i < MAX_SKILL_NUM; ++i){
		if (!m_dbData.m_skillVec[i].m_skillID){
			m_dbData.m_skillVec[i].m_skillID = skillID;
			m_dbData.m_skillVec[i].m_skillLv = 0;
			return true;
		}
	}
	return false;
}

void HeroInfo::upgradeActiveSkill(UINT32 skillID)
{
	for (UINT8 i = 0; i < MAX_SKILL_NUM; ++i){
		if (m_dbData.m_skillVec[i].m_skillID == skillID){
			m_dbData.m_skillVec[i].m_skillLv += 1;
			break;
		}
	}
}

UINT16 HeroInfo::getActiviSkillLv(UINT32 skillID)
{
	for (UINT8 i = 0; i < MAX_SKILL_NUM; ++i){
		if (m_dbData.m_skillVec[i].m_skillID == skillID){
			return m_dbData.m_skillVec[i].m_skillLv;
		}
	}

	return 0;
}

bool HeroInfo::isActiveSkillExist(UINT32 skillID)
{
	for (UINT8 i = 0; i < MAX_SKILL_NUM; ++i){
		if (skillID == m_dbData.m_skillVec[i].m_skillID){
			return true;	
		}	
	}	
	return false;
}

void HeroInfo::checkSkillUnlock()
{
	if (!m_pHeroBase){
		return ;	
	}

	if (m_dbData.m_level >= m_pHeroBase->m_skill1UnlockLv){
		addActiveSkill(m_pHeroBase->m_skill1);	
	}
	
	if (m_dbData.m_level >= m_pHeroBase->m_skill2UnlockLv){
		addActiveSkill(m_pHeroBase->m_skill2);	
	}
	
	if (m_dbData.m_level >= m_pHeroBase->m_skill3UnlockLv){
		addActiveSkill(m_pHeroBase->m_skill3);	
	}
	
	/*if (m_dbData.m_level >= m_pHeroBase->m_skill4UnlockLv){
		addActiveSkill(m_pHeroBase->m_skill4);	
	}*/
}

UINT32 HeroInfo::load(byte * buf, UINT32 version)
{
	UINT32 ret = 0;
	bcopy(buf,&m_dbData,sizeof(m_dbData));
	ret += sizeof(m_dbData);
	
	UINT16 skillNum = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);
	for (UINT16 i = 0;i < skillNum ; ++i){
		UINT8 pos = *(UINT8 *)(buf + ret);
		ret += sizeof(UINT8);
		stHeroSkillInfo * pSkillInfo = (stHeroSkillInfo *)(buf + ret);
		ret += sizeof(stHeroSkillInfo);
		m_skillCnt.insert(std::make_pair(pos,*pSkillInfo));
	}
    if (version == HeroInfoMgr::s_HERO_SAVE_VERSION_SECOND) {
        bcopy(buf + ret,&m_runeDbData,sizeof(m_runeDbData));
        ret += sizeof(m_runeDbData);
    }
	return ret;
}

UINT32 HeroInfo::save(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	bcopy(&m_dbData,buf,sizeof(m_dbData));
	ret += sizeof(m_dbData);

	*(UINT16 *)(buf + ret) = m_skillCnt.size();
	ret += sizeof(UINT16);

	std::map<UINT8,stHeroSkillInfo>::iterator it =  m_skillCnt.begin();
	for (;it != m_skillCnt.end(); ++it){
		*(UINT8*)(buf + ret) = it->first;
		ret += sizeof(UINT8);
		bcopy(&it->second,buf + ret,sizeof(stHeroSkillInfo));
		ret += sizeof(stHeroSkillInfo);
	}
	bcopy(&m_runeDbData,buf + ret,sizeof(m_runeDbData));
	ret += sizeof(m_runeDbData);
	return ret;
}

void HeroInfo::fillSynInfo(GameUser * pOwner, Role::stSynHeroInfo & heroInfo)
{
	heroInfo.set_heroid(m_dbData.m_id);
	heroInfo.set_level(m_dbData.m_level);
	heroInfo.set_state((Role::E_HERO_STATE)m_dbData.m_state);
	heroInfo.set_pos(m_dbData.m_pos);
	heroInfo.set_evolutionlv(m_dbData.m_evolutionLv);
	heroInfo.set_studyskillid(m_dbData.m_studySkillID);
	heroInfo.set_trainlv(m_dbData.m_trainLv);
	heroInfo.set_trainexp(m_dbData.m_trainExp);

	for (UINT8 i = 0;i < MAX_SKILL_NUM; ++i){
		stHeroSkillInfo & skillInfo = m_dbData.m_skillVec[i];
		if (!skillInfo.m_skillID){
			continue;
		}
		Role::stSynHeroSkill * pSynSkill = heroInfo.add_skillvec();
		if (pSynSkill){
			pSynSkill->set_skillid(skillInfo.m_skillID);
			pSynSkill->set_skilllv(skillInfo.m_skillLv);
			pSynSkill->set_skillpos(0);
		}
	}

	std::map<UINT8,stHeroSkillInfo>::iterator it = m_skillCnt.begin();
	for (;it != m_skillCnt.end();++it){
		stHeroSkillInfo & skillInfo = it->second;
		Role::stSynHeroSkill * pSynSkill = heroInfo.add_skillvec();
		if (pSynSkill){
			pSynSkill->set_skillid(skillInfo.m_skillID);
			pSynSkill->set_skilllv(skillInfo.m_skillLv);
			pSynSkill->set_skillpos(it->first);
		}
	}
	
	if (m_pHeroBase && m_pHeroBase->m_normalSkill){
		Role::stSynHeroSkill * pSynSkill = heroInfo.add_skillvec();
		if (pSynSkill){
			pSynSkill->set_skillid(m_pHeroBase->m_normalSkill);
			pSynSkill->set_skilllv(0);
			pSynSkill->set_skillpos(0);
		}
	}

	heroInfo.set_maxhp(m_maxHP);
	heroInfo.set_damage(m_damage);

    std::vector<UINT32> runeBattlePropVec;
	runeBattlePropVec.resize(eMAX_BATTLE_PROP);
    UINT32 runeBattlePower = 0;
    getRuneAdd(pOwner, runeBattlePower, runeBattlePropVec, true);
	heroInfo.set_battlepower(m_battlePower + runeBattlePower);

	for (UINT8 i = 1; i < m_battleProps.size();++i){
		Role::stBattleProp * pSyn = heroInfo.add_battleprop();
		if (pSyn){
			pSyn->set_propid(i);
			pSyn->set_propval(m_battleProps[i] + runeBattlePropVec[i]);
		}
	}
}

void HeroInfo::getRuneAdd(GameUser * pOwner, UINT32 & runeBattlePower, std::vector<UINT32> & runeBattlePropVec, bool getAllAdd) {
    // UINT16 slotIndex = (UINT16)-1;
    // for (size_t i = 0; i < MAX_HERO_RUNE_NUM; i++) {
    //     slotIndex = m_runeDbData.m_runeVec[i];
    //     if (slotIndex == (UINT16)-1) {
    //         continue ;
    //     }
    //     CItem * pItem = pOwner->m_bagMgr.m_heroRuneBag.getItemBySlot(slotIndex);
    //     if (!pItem) {
    //         continue ;
    //     }
    //     runeBattlePower += pItem->m_battlePower;
    //     if (getAllAdd) {
    //         for (UINT8 i = 1; i < runeBattlePropVec.size(); i++) {
    //             runeBattlePropVec[i] += pItem->m_battleProps[i];
    //         }
    //     }
    // }
}

bool HeroInfo::hasRune(UINT16 slot) {
    for (UINT32 i = 0; i < MAX_HERO_RUNE_NUM; i++) {
        if (m_runeDbData.m_runeVec[i] == slot) {
            return true;
        }
    }
    return false;
}

void HeroInfo::levelup()
{
	m_dbData.m_level += 1;
	checkSkillUnlock();
	init(true);
}

void HeroInfo::evolution()
{
	m_dbData.m_evolutionLv += 1;
	init(true);
}

void HeroInfo::replaceSkill(UINT8 pos,UINT32 skillID)
{
	stHeroSkillInfo one;
	one.m_skillID = skillID;
	one.m_skillLv = 0;	
	std::map<UINT8,stHeroSkillInfo>::iterator it = m_skillCnt.find(pos);
	if (it == m_skillCnt.end()){
		m_skillCnt.insert(std::make_pair(pos,one));
	}
	else {
		m_skillCnt.erase(it);
		m_skillCnt.insert(std::make_pair(pos,one));
	}
}

bool HeroInfo::isExistSkill(UINT32 skillID)
{
	std::map<UINT8,stHeroSkillInfo>::iterator it = m_skillCnt.begin();

	for (;it != m_skillCnt.end();++it){
		if (it->second.m_skillID == skillID){
			return true;
		}	
	}
	return false;
}

stHeroSkillInfo HeroInfo::getSkillIDByPos(UINT8 pos)
{
	std::map<UINT8,stHeroSkillInfo>::iterator it = m_skillCnt.find(pos);
	if (it == m_skillCnt.end()){
		return stHeroSkillInfo();
	}
	return  it->second;
}

void HeroInfo::upgradeSkill(UINT32 skillID, UINT32 maxSkillLv)
{
	std::map<UINT8,stHeroSkillInfo>::iterator it = m_skillCnt.begin();
	for (;it != m_skillCnt.end();++it){
		if (it->second.m_skillID == skillID && it->second.m_skillLv < maxSkillLv){
			it->second.m_skillLv += 1;
		}
	}
}

HeroInfoMgr::HeroInfoMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	bzero(m_aBattleHero,sizeof(m_aBattleHero));
	m_skillExp = 0;
	m_lastChgHeroTime = 0;
	m_curGroupID = 0;
	bzero(m_aBattleHeroHP,sizeof(m_aBattleHeroHP));
	bzero(m_aBattleHero_bak,sizeof(m_aBattleHero_bak));
}

HeroInfoMgr::~HeroInfoMgr()
{
	Hero_IT it = m_allHero.begin();
	for (;it != m_allHero.end();++it){
		SAFE_DELETE(it->second);
	}
}

bool HeroInfoMgr::addBattleHero(UINT32 heroID,UINT8 pos)
{
	/*if (isBattle(heroID)){
		return false;
	}*/

	if (pos >= s_MAX_BATTLE_HERO_NUM){
		return false;
	}
	HeroInfo *pHeroInfo =  getHeroInfoByID(heroID);
	if (!pHeroInfo){
		return false;
	}
	
	if (pHeroInfo->m_dbData.m_pos  > 0){//已经出战
		UINT8 srcPos = pHeroInfo->m_dbData.m_pos - 1;
		if (srcPos < s_MAX_BATTLE_HERO_NUM){
			m_aBattleHero[srcPos] = 0;
		}
	}

	UINT32 oldHeroID = m_aBattleHero[pos];
	if (oldHeroID){
		pHeroInfo =  getHeroInfoByID(oldHeroID);	
		if (pHeroInfo){
			pHeroInfo->m_dbData.m_pos = 0;
			pHeroInfo->m_dbData.m_state = Role::HERO_STATE_SUMMONED;
			refreshImpactHero(oldHeroID);
		}
	}

	m_aBattleHero[pos] = heroID;
	
	pHeroInfo =  getHeroInfoByID(heroID);
	if (pHeroInfo){
		pHeroInfo->m_dbData.m_pos = pos + 1;
		pHeroInfo->m_dbData.m_state = Role::HERO_STATE_BATTLE;
	}
	return true;
}

bool HeroInfoMgr::delBattleHero(UINT32 heroID,UINT8 pos)
{
	if (pos >= s_MAX_BATTLE_HERO_NUM){
		return false;
	}

	if (heroID != m_aBattleHero[pos]){
		return false;
	}
	
	UINT32 oldHeroID = m_aBattleHero[pos];
	if (oldHeroID){
		HeroInfo *pHeroInfo =  getHeroInfoByID(oldHeroID);	
		if (pHeroInfo){
			pHeroInfo->m_dbData.m_pos = 0;
			pHeroInfo->m_dbData.m_state = Role::HERO_STATE_SUMMONED; 
		}
	}
	m_aBattleHero[pos] = 0;
	return true;
}

UINT8 HeroInfoMgr::getBattlePos(UINT32 heroID)
{
	for (UINT16 i = 0;i < s_MAX_BATTLE_HERO_NUM; ++i){
		if (m_aBattleHero[i] == heroID){
			return i;
		}
	}
	return (UINT8)-1;
}

bool HeroInfoMgr::isBattle(UINT32 heroID)
{
	for (UINT16 i = 0;i < s_MAX_BATTLE_HERO_NUM; ++i){
		if (m_aBattleHero[i] == heroID){
			return true;
		}
	}
	return false;
}

UINT16 HeroInfoMgr::battleHeroNum()
{
	UINT16 ret = 0;
	for (UINT16 i = 0;i < s_MAX_BATTLE_HERO_NUM; ++i){
		if (m_aBattleHero[i]){
			ret += 1;
		}
	}
	return ret;
}

UINT32 HeroInfoMgr::loadAllHero(byte * buf)
{
	UINT32 ret = sizeof(stHeroSaveInfo);
	stHeroSaveInfo * pSaveInfo = (stHeroSaveInfo *)buf;
	m_skillExp = pSaveInfo->m_skillExp;
	if (pSaveInfo->m_version == s_HERO_SAVE_VERSION){
		for (UINT16 i = 0;i < pSaveInfo->m_num; ++i){
			HeroInfo * pHeroInfo = ATHENA_NEW HeroInfo(this);
			ret += pHeroInfo->load(buf + ret, s_HERO_SAVE_VERSION);
			if (!pHeroInfo->init()){
				Athena::logger->error("account=%s,name=%s,id=%d上线加载武将数据,初始化时候失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
				SAFE_DELETE(pHeroInfo);
				continue;
			}

			if (!m_allHero.insert(std::make_pair(pHeroInfo->m_dbData.m_id,pHeroInfo)).second){
				Athena::logger->error("account=%s,name=%s,id=%d上线加载武将数据,添加到容器失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
				SAFE_DELETE(pHeroInfo);
				continue;
			}
			if (Role::HERO_STATE_BATTLE == pHeroInfo->m_dbData.m_state){
				addBattleHero(pHeroInfo->m_dbData.m_id,pHeroInfo->m_dbData.m_pos - 1);
			}
		}	
	}
    else if (pSaveInfo->m_version == s_HERO_SAVE_VERSION_SECOND){
		for (UINT16 i = 0;i < pSaveInfo->m_num; ++i){
			HeroInfo * pHeroInfo = ATHENA_NEW HeroInfo(this);
			ret += pHeroInfo->load(buf + ret, s_HERO_SAVE_VERSION_SECOND);
			if (!pHeroInfo->init()){
				Athena::logger->error("account=%s,name=%s,id=%d上线加载武将数据,初始化时候失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
				SAFE_DELETE(pHeroInfo);
				continue;
			}

			if (!m_allHero.insert(std::make_pair(pHeroInfo->m_dbData.m_id,pHeroInfo)).second){
				Athena::logger->error("account=%s,name=%s,id=%d上线加载武将数据,添加到容器失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
				SAFE_DELETE(pHeroInfo);
				continue;
			}
			if (Role::HERO_STATE_BATTLE == pHeroInfo->m_dbData.m_state){
				addBattleHero(pHeroInfo->m_dbData.m_id,pHeroInfo->m_dbData.m_pos - 1);
			}
		}	
	}
	else {
		ret += pSaveInfo->m_num * sizeof(stHeroDBData);
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了%d个武将",m_allHero.size());
#endif
	return ret;
}

UINT32 HeroInfoMgr::saveAllHero(byte * buf,UINT32 needSpace)
{
	UINT32 ret = sizeof(stHeroSaveInfo);
	stHeroSaveInfo * pSaveInfo = (stHeroSaveInfo *)buf;	
	pSaveInfo->m_version = s_HERO_SAVE_VERSION_SECOND;
	pSaveInfo->m_num = 0;
	pSaveInfo->m_skillExp = m_skillExp;
	Hero_IT it = m_allHero.begin();
	for (;it != m_allHero.end();++it){
		HeroInfo * pHeroInfo = it->second;
		ret += pHeroInfo->save(buf + ret,needSpace);
		pSaveInfo->m_num += 1;
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("保存了%d个武将",pSaveInfo->m_num);
#endif
	return ret;
}

void HeroInfoMgr::calcAllHero()
{
	Hero_IT it = m_allHero.begin();
	for (;it != m_allHero.end();++it){
		HeroInfo * pHeroInfo = it->second;
		if (pHeroInfo) {
			pHeroInfo->init(true);
		}
	}
	for(UINT8 i = 0; i < s_MAX_BATTLE_HERO_NUM; ++i){
		refreshImpactHero(m_aBattleHero[i], true, true);
	}
}

void HeroInfoMgr::synAllHero()
{
	Role::stInitAllHero cmd;
	cmd.set_skillexp(m_skillExp);
	cmd.set_usediamondnum(m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM));
	Hero_IT it = m_allHero.begin();
	for (;it != m_allHero.end();++it){
		HeroInfo * pHeroInfo = it->second;
		Role::stSynHeroInfo  * pData = cmd.add_data();
		if (pHeroInfo) {
			pHeroInfo->fillSynInfo(m_pOwner, *pData);
		}
	}

	m_pOwner->sendProto2Me(cmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("上线同步了%d个武将",m_allHero.size());
#endif
}

void HeroInfoMgr::notifyAddHero(HeroInfo * pHeroInfo)
{
	if (!pHeroInfo){
		return ;
	}
	
	Role::stNotifyAddHero cmd;
	Role::stSynHeroInfo  * pData = cmd.mutable_data();
	pHeroInfo->fillSynInfo(m_pOwner, *pData);
	m_pOwner->sendProto2Me(cmd);
}

void HeroInfoMgr::refreshHero(HeroInfo * pHeroInfo)
{
	if (!pHeroInfo){
		return ;
	}

	Role::stRefreshHero cmd;
	Role::stSynHeroInfo  * pData = cmd.mutable_data();
	pHeroInfo->fillSynInfo(m_pOwner, *pData);
	m_pOwner->sendProto2Me(cmd);
}

void HeroInfoMgr::refreshEvolutionHero(UINT32 heroID)
{
	if(!isBattle(heroID)){
		return;
	}
	Role::stSendImpactHeroInfo cmd;
	HeroInfo * pHeroInfo = getHeroInfoByID(heroID);
	if(!pHeroInfo){
		 return ;
	}
 
	if(pHeroInfo->m_addProps.empty()){
		return ;
	}
	UINT8 psize = pHeroInfo->m_addProps.size()-1;
	UINT32 heroId = pHeroInfo->m_addProps[psize].m_propTarget;
	HeroInfo * pImpactHeroInfo = getHeroInfoByID(heroId);
	if(!pImpactHeroInfo){
		return;
	}

	stHeroEvolutionBase * pUpEvolutionBase = g_heroEvolutionBaseMgr.get(stHeroEvolutionBase::getFindID(pHeroInfo->m_dbData.m_id,pHeroInfo->m_dbData.m_evolutionLv - 1));
	stHeroEvolutionBase * pNowEvolutionBase = g_heroEvolutionBaseMgr.get(stHeroEvolutionBase::getFindID(pHeroInfo->m_dbData.m_id,pHeroInfo->m_dbData.m_evolutionLv));
	if(!pUpEvolutionBase || !pNowEvolutionBase){
		return;
	}

	std::vector<std::pair<stEvolutionAddProp,stEvolutionAddProp> > samePropVec;
	std::vector<stEvolutionAddProp > diffPropVec;

	UINT8 i = 0;
	UINT8 upVecSize = pUpEvolutionBase->m_propVec.size();
	for(i = 0; i < upVecSize; ++i){
		stEvolutionAddProp & nowProp = pNowEvolutionBase->m_propVec[i];
		stEvolutionAddProp & upProp = pUpEvolutionBase->m_propVec[i];
		if((upProp.m_propTarget == nowProp.m_propTarget) && (upProp.m_propID == nowProp.m_propID) && (nowProp.m_propTarget > eEvolution_Prop_Target_Hero)){
			samePropVec.push_back(make_pair(nowProp, upProp));
		}
	}

	UINT8 nowVecSize = pNowEvolutionBase->m_propVec.size();
	if(nowVecSize > upVecSize){
		if(pNowEvolutionBase->m_propVec[i].m_propTarget > eEvolution_Prop_Target_Hero){
			diffPropVec.push_back(pNowEvolutionBase->m_propVec[i]);
		}
	}

	for(i = 0; i < samePropVec.size(); ++i){
		pImpactHeroInfo->m_battleProps[samePropVec[i].first.m_propID] += samePropVec[i].first.m_value - samePropVec[i].second.m_value;
	}

	for(i = 0; i < diffPropVec.size(); ++i){
		pImpactHeroInfo->m_battleProps[diffPropVec[i].m_propID] += diffPropVec[i].m_value;
	}

	for(i = 0; i < pHeroInfo->m_addProps.size(); ++i){
		Role::stImpactHeroInfo *info  = cmd.add_data();
		if(info){	
			info->set_heroid(heroId);
			Role::stBattleProp * prop = info->add_battleprop();
			if(prop){
				prop->set_propid(pHeroInfo->m_addProps[i].m_propID);
				prop->set_propval(pImpactHeroInfo->m_battleProps[pHeroInfo->m_addProps[i].m_propID]);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[出战宠物属性影响]出战宠物ID = %d，受影响宠物(ID = %d, propID = %d, value = %u, nowValue = %u)", heroID, heroId, pHeroInfo->m_addProps[i].m_propID, pHeroInfo->m_addProps[i].m_value, pImpactHeroInfo->m_battleProps[pHeroInfo->m_addProps[i].m_propID]);
#endif
			}
		}
	}
	m_pOwner->sendProto2Me(cmd);
}

void HeroInfoMgr::refreshImpactHero(UINT32 heroID, bool isFight, bool onlyAffectsOthers, bool isUp, bool send)
{
	Role::stSendImpactHeroInfo cmd;

	HeroInfo * pHeroInfo = getHeroInfoByID(heroID);
	if(!pHeroInfo){
		return ;
	}
	//计算我对别人的影响
	for(UINT8 i = 0; i < pHeroInfo->m_addProps.size(); ++i){
		//受影响武将ID
		UINT32 heroId = pHeroInfo->m_addProps[i].m_propTarget;
		if(isBattle(heroId)){
			HeroInfo * pImpactHeroInfo = getHeroInfoByID(heroId);
			if(!pImpactHeroInfo){
				return ;
			}
			if(isUp){
			
			}else{
				if(isFight){
					pImpactHeroInfo->m_impactProps[pHeroInfo->m_addProps[i].m_propID] += pHeroInfo->m_addProps[i].m_value;
					pImpactHeroInfo->m_battleProps[pHeroInfo->m_addProps[i].m_propID] += pHeroInfo->m_addProps[i].m_value;
				}
				else{
					pImpactHeroInfo->m_impactProps[pHeroInfo->m_addProps[i].m_propID] -= pHeroInfo->m_addProps[i].m_value ;
					pImpactHeroInfo->m_battleProps[pHeroInfo->m_addProps[i].m_propID] -= pHeroInfo->m_addProps[i].m_value ;
				}
			}

			Role::stImpactHeroInfo *info  = cmd.add_data();
			if(info){
				info->set_heroid(heroId);
				Role::stBattleProp * prop = info->add_battleprop();
				if(prop){
					prop->set_propid(pHeroInfo->m_addProps[i].m_propID);
					prop->set_propval(pImpactHeroInfo->m_battleProps[pHeroInfo->m_addProps[i].m_propID]);

#ifdef _HDZ_DEBUG
					Athena::logger->trace("[出战宠物属性影响]出战宠物ID = %d，受影响宠物(ID = %d, propID = %d, value = %u, nowValue = %u)", heroID, heroId, pHeroInfo->m_addProps[i].m_propID, pHeroInfo->m_addProps[i].m_value, pImpactHeroInfo->m_battleProps[pHeroInfo->m_addProps[i].m_propID]);
#endif

				}
			}
		}
	}
	if(!onlyAffectsOthers){
		//计算别人对我的影响
		for(UINT8 i = 0; i < s_MAX_BATTLE_HERO_NUM; ++i){
			HeroInfo * pOtherHeroInfo = getHeroInfoByID(m_aBattleHero[i]);
			if(pOtherHeroInfo && (m_aBattleHero[i] != heroID)){
				for(UINT8 j = 0; j < pOtherHeroInfo->m_addProps.size(); ++j)
				{
					if(pOtherHeroInfo->m_addProps[j].m_propTarget == heroID){
						if(isFight){
							pHeroInfo->m_impactProps[pOtherHeroInfo->m_addProps[j].m_propID] += pOtherHeroInfo->m_addProps[j].m_value;
							pHeroInfo->m_battleProps[pOtherHeroInfo->m_addProps[j].m_propID] += pOtherHeroInfo->m_addProps[j].m_value;
						}
						else{
							pHeroInfo->m_impactProps[pOtherHeroInfo->m_addProps[j].m_propID] -= pOtherHeroInfo->m_addProps[j].m_value;
							pHeroInfo->m_battleProps[pOtherHeroInfo->m_addProps[j].m_propID] -= pOtherHeroInfo->m_addProps[j].m_value;
						}
	
						Role::stImpactHeroInfo *info  = cmd.add_data();
						if(info){
							info->set_heroid(heroID);
							Role::stBattleProp * prop = info->add_battleprop();
							if(prop){
								prop->set_propid(pOtherHeroInfo->m_addProps[j].m_propID);
								prop->set_propval(pHeroInfo->m_battleProps[pOtherHeroInfo->m_addProps[j].m_propID]);
	
#ifdef _HDZ_DEBUG
								Athena::logger->trace("[出战宠物属性影响]出战宠物ID = %d，影响我的宠物(ID = %d, propID = %d, value = %u, nowValue = %u)", heroID, m_aBattleHero[i], pOtherHeroInfo->m_addProps[j].m_propID,  pOtherHeroInfo->m_addProps[j].m_value, pHeroInfo->m_battleProps[pOtherHeroInfo->m_addProps[j].m_propID] );
#endif 

							}
						}
					}
				}
			}
		}
	}
	if(send) {
		m_pOwner->sendProto2Me(cmd);
	}
}

HeroInfo * HeroInfoMgr::createHero(UINT32 heroID,UINT16 lv)
{
	if (m_allHero.find(heroID) != m_allHero.end()){
#ifdef _HDZ_DEBUG
		Athena::logger->error("account=%s,name=%s,id=%d,已经有武将了还试图创建武将",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
#endif
		return NULL;
	}

	stRoleBase * pHeroBase = g_roleBaseMgr.get(heroID);
	if (!pHeroBase){
		return NULL;
	}
	
	HeroInfo * pHeroInfo = ATHENA_NEW HeroInfo(this);
	pHeroInfo->m_dbData.m_id = heroID;
	pHeroInfo->m_dbData.m_level = lv;
	pHeroInfo->m_dbData.m_state = Role::HERO_STATE_SUMMONED;
	pHeroInfo->m_dbData.m_evolutionLv = 0;
	pHeroInfo->m_dbData.m_pos = 0;
	pHeroInfo->m_dbData.m_trainLv = CHeroTrainCfg::getSingleton().getInitHeroTrainLvl();
	pHeroInfo->m_dbData.m_trainExp = 0;

	if (!pHeroInfo->init()){
		Athena::logger->error("account=%s,name=%s,id=%d,创建武将失败,初始化HeroInfo失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		SAFE_DELETE(pHeroInfo);
		return NULL;
	}
	pHeroInfo->checkSkillUnlock();
	return pHeroInfo;
}

bool HeroInfoMgr::addHeroInfo(HeroInfo * pHeroInfo)
{
	if (!pHeroInfo){
		return false;
	}

	if (m_allHero.find(pHeroInfo->m_dbData.m_id) != m_allHero.end()){
		return false;
	}

	m_allHero[pHeroInfo->m_dbData.m_id] = pHeroInfo;
	notifyAddHero(pHeroInfo);
	return true;
}

HeroInfo * HeroInfoMgr::getHeroInfoByID(UINT32 heroID)
{
	std::map<UINT32,HeroInfo *>::iterator it = m_allHero.find(heroID);
	if (it != m_allHero.end()){
		return it->second;
	}
	return NULL;
}

bool   HeroInfoMgr::addSkillExp(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}
	m_skillExp += num;
	Athena::logger->trace("name=%s,id=%d,因为%s增加了技能经验num=%d",m_pOwner->getName(),m_pOwner->getID(),desc,num);
	//Role::stSynSkillExp retCmd;
	//retCmd.set_curexp(m_skillExp);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Lingwu_Exp);
	retCmd.set_num(m_skillExp);
	m_pOwner->sendProto2Me(retCmd);
	return true;
}

bool   HeroInfoMgr::removeSkillExp(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}
	if (m_skillExp < num){
		return false;
	}
	m_skillExp -= num;
	Athena::logger->trace("name=%s,id=%d,因为%s扣除了技能经验num=%d",m_pOwner->getName(),m_pOwner->getID(),desc,num);
	/*Role::stSynSkillExp retCmd;
	retCmd.set_curexp(m_skillExp);
	m_pOwner->sendProto2Me(retCmd);*/
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Lingwu_Exp);
	retCmd.set_num(m_skillExp);
	m_pOwner->sendProto2Me(retCmd);
	return true;
}

void HeroInfoMgr::summonHero()
{
	if (!m_pOwner->getMap()){
		return ;
	}
	
	if (m_pOwner->getMap()->isStatic()){
		return ;
	}

	bzero(m_aBattleHeroHP,sizeof(m_aBattleHeroHP));

	for (UINT8 i = 0;i < s_MAX_BATTLE_HERO_NUM;++i){
		if (m_aBattleHero[i]){
			HeroInfo * pHero = getHeroInfoByID(m_aBattleHero[i]);
			if (pHero){
				m_aBattleHeroHP[i] = pHero->m_maxHP;	
			}
		}
	}
	//开始召唤第一组
	if (isGroupGoOut(1)){
		changeHero(1);	
	}
	else if (isGroupGoOut(2)){
		changeHero(2);
	}
}

bool HeroInfoMgr::isGroupGoOut(UINT8 group)
{
	if (group > 2 || group < 1){
		return false;
	}
	
	UINT8 index1 = g_goOutGroup[group - 1][0] - 1;
	UINT8 index2 = g_goOutGroup[group - 1][1] - 1;

	if (m_aBattleHero[index1] || m_aBattleHero[index2]){
		return true;
	}
	return false;
}

void HeroInfoMgr::updateBattleHeroHP(UINT32 heroID,UINT32 hp)
{
	for (UINT8 i = 0;i < s_MAX_BATTLE_HERO_NUM;++i){
		if (heroID == m_aBattleHero[i]){
			m_aBattleHeroHP[i] = hp;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("清空hero,更新了:heroID:%d,血量:%d",heroID,hp);
#endif
		}
	}
#ifdef _HDZ_DEBUG
	for (UINT8 i = 0;i < s_MAX_BATTLE_HERO_NUM;++i){
		Athena::logger->trace("当前宠物:%d,血量:%d",m_aBattleHero[i],m_aBattleHeroHP[i]);
	}
#endif
}

void HeroInfoMgr::changeHero(UINT8 group)
{
	if (group > 2 || group < 1){
		return ;
	}
#ifdef _HDZ_DEBUG
	/*for (UINT8 i = 0;i < s_MAX_BATTLE_HERO_NUM;++i){
		Athena::logger->trace("当前宠物:%d,血量:%d",m_aBattleHero[i],m_aBattleHeroHP[i]);
	}*/
#endif
	CPos bornPos = m_pOwner->getPos();
	if (m_pOwner->getMap()){
		UINT8 index = g_goOutGroup[group - 1][0] - 1;
		if (m_aBattleHero[index] && (m_aBattleHeroHP[index] > 0)){
#ifdef _HDZ_DEBUG
			//Athena::logger->trace("1召唤第:%d组的宠物:%d,初始血量是:%d",group,m_aBattleHero[index],m_aBattleHeroHP[index]);
#endif
			HeroInfo * pHeroInfo = getHeroInfoByID(m_aBattleHero[index]);
			if (pHeroInfo){
				CHero * pHero = m_pOwner->getMap()->summonHero(m_aBattleHero[index],m_pOwner->getID(),pHeroInfo->m_dbData.m_level,pHeroInfo->m_dbData.m_evolutionLv,bornPos,m_aBattleHeroHP[index]);
				if (pHero){
					pHero->setCampID(m_pOwner->getCampID());
				}
			}
		}
		
		index = g_goOutGroup[group - 1][1] - 1;
		if (m_aBattleHero[index] && (m_aBattleHeroHP[index] > 0)){
#ifdef _HDZ_DEBUG
			//Athena::logger->trace("2召唤第:%d组的宠物:%d,初始血量是:%d",group,m_aBattleHero[index],m_aBattleHeroHP[index]);
#endif
			HeroInfo * pHeroInfo = getHeroInfoByID(m_aBattleHero[index]);
			if (pHeroInfo){
				CHero *pHero = m_pOwner->getMap()->summonHero(m_aBattleHero[index],m_pOwner->getID(),pHeroInfo->m_dbData.m_level,pHeroInfo->m_dbData.m_evolutionLv,bornPos,m_aBattleHeroHP[index]);	
				if (pHero){
					pHero->setCampID(m_pOwner->getCampID());
				}
			}
		}
	}
	m_curGroupID = group;
}

bool HeroInfoMgr::isColdDown()	
{
	if (m_lastChgHeroTime + s_CHANGE_HERO_CD > g_gameServer->getSec()){
		return true;
	}
	else {
		return false;
	}
}

void HeroInfoMgr::setColdDown()
{
	m_lastChgHeroTime = g_gameServer->getSec();
}

void HeroInfoMgr::exeEveryOne(stHeorInfoCBBase & cb)
{
	Hero_IT it = m_allHero.begin();
	for (;it != m_allHero.end();++it){
		if (!cb.doIt(it->second)){
			return;
		}				
	}
}

UINT32  HeroInfoMgr::getBattlePower()
{
    UINT32 runeBattlePower = 0;
    std::vector<UINT32> runeBattlePropVec;
	runeBattlePropVec.resize(eMAX_BATTLE_PROP);

	UINT32 ret = 0;
	for (UINT16 i = 0;i < s_MAX_BATTLE_HERO_NUM ; ++i){
		if (m_aBattleHero[i]) {
			HeroInfo *pHeroInfo = getHeroInfoByID(m_aBattleHero[i]);		
			if (pHeroInfo){
                runeBattlePower = 0;
                runeBattlePropVec.clear();
                pHeroInfo->getRuneAdd(m_pOwner, runeBattlePower, runeBattlePropVec, false);
				ret += pHeroInfo->m_battlePower + runeBattlePower;
			}
		}
	}

	return ret;
}

void HeroInfoMgr::addProp2Owner()
{
	//所有随从的血量和伤害加成给主人
	Hero_IT it = m_allHero.begin();
	for (;it != m_allHero.end();++it){
		HeroInfo * pHeroInfo = it->second;
		if(pHeroInfo){
			m_pOwner->m_roleState.m_battleProps[eBATTLE_PROP_HP] += pHeroInfo->m_battleProps[eBATTLE_PROP_HP];
			m_pOwner->m_roleState.m_battleProps[eBATTLE_PROP_DAMAGE] += pHeroInfo->m_battleProps[eBATTLE_PROP_DAMAGE];
		}
	}
}

bool HeroInfoMgr::swapBattlePos(UINT32 srcHeroID,UINT32 destHeroID)
{
	UINT8 srcPos = getBattlePos(srcHeroID);
	UINT8 destPos = getBattlePos(destHeroID);

	if ((UINT8)-1 == srcPos || (UINT8)-1 == destPos){
		return false;	
	}
	
	HeroInfo * pSrcHeroInfo = getHeroInfoByID(srcHeroID);
	HeroInfo * pDestHeroInfo = getHeroInfoByID(destHeroID);
	
	if (!pSrcHeroInfo || !pDestHeroInfo){
		return false;
	}
	
	pSrcHeroInfo->m_dbData.m_pos = destPos + 1;
	pDestHeroInfo->m_dbData.m_pos = srcPos + 1;

	m_aBattleHero[srcPos] = destHeroID;
	m_aBattleHero[destPos] =  srcHeroID;
	return true;
}

//临时替换上阵宠物
void HeroInfoMgr::useLeitaiBattleHero()
{
	bzero(m_aBattleHero_bak,sizeof(m_aBattleHero_bak));
	for (UINT32 i = 0; i < s_MAX_BATTLE_HERO_NUM; ++i)
	{
		m_aBattleHero_bak[i] = m_aBattleHero[i];
		m_aBattleHero[i] = m_pOwner->m_leitaiMgr.getHeroIDByPos(i);
	}
}

//还原擂台上阵宠物
void HeroInfoMgr::restoreLeitaiBattleHero()
{
	for (UINT32 i = 0; i < s_MAX_BATTLE_HERO_NUM; ++i)
	{
		m_aBattleHero[i] = m_aBattleHero_bak[i];
	}
}

	
// 获取战斗宝宝id
UINT32 HeroInfoMgr::getBattleHeroID(UINT32 pos)
{
	if( pos >= s_MAX_BATTLE_HERO_NUM){
		return 0;
	}else{
		return m_aBattleHero[pos];
	}
}
// 获取战斗宝宝血量
UINT32 HeroInfoMgr::getBattleHeroHP(UINT32 pos)
{
	if(pos >= s_MAX_BATTLE_HERO_NUM){
		return 0;
	}else{
		return m_aBattleHeroHP[pos];
	}
}

UINT32 HeroInfoMgr::GetHeroTrainLevelUpExp(UINT32 heroID, UINT32 lv)
{
	stHeroTrain* cfg = g_heroTrainExpMgr.get(heroID * HEROTRAIN_NUM_COEF + lv);
	if(!cfg) {
		return 0;
	}
	return cfg->m_exp;
}



//校验是否调教升级
bool HeroInfoMgr::checkTrainLevelUp(HeroInfo* pHeroInfo)
{
	bool up = false;
	UINT32 needexp = this->GetHeroTrainLevelUpExp(pHeroInfo->m_dbData.m_id, pHeroInfo->m_dbData.m_trainLv +1);
	while(needexp > 0 
		&& pHeroInfo->m_dbData.m_trainExp >= needexp
		&& pHeroInfo->m_dbData.m_trainLv < CHeroTrainCfg::getSingleton().getMaxHeroTrainLvl()) {
		up = true;
		pHeroInfo->m_dbData.m_trainExp -= needexp;
		pHeroInfo->m_dbData.m_trainLv++;
		this->onTrainLevelUp(pHeroInfo,pHeroInfo->m_dbData.m_trainLv);

		needexp = this->GetHeroTrainLevelUpExp(pHeroInfo->m_dbData.m_id, pHeroInfo->m_dbData.m_trainLv +1);
	}
	//满级,清空溢出经验
	if(pHeroInfo->m_dbData.m_trainLv == CHeroTrainCfg::getSingleton().getMaxHeroTrainLvl()) {
		pHeroInfo->m_dbData.m_trainExp = 0;
	}
	return up;
}

//调教升级
void HeroInfoMgr::onTrainLevelUp(HeroInfo* pHeroInfo, UINT32 lvl)
{
	stTrainLvAward* awardCfg = CHeroTrainCfg::getSingleton().GetAwardCfg(pHeroInfo->m_dbData.m_id, lvl);
	if(!awardCfg) {
		return;
	}
	std::vector<stTrainLvAwardItem>::iterator vecIt = awardCfg->m_awards.begin();
	Role::stObtainItemTip retCmd;
	for(;vecIt != awardCfg->m_awards.end(); ++vecIt){
		if((*vecIt).m_type == TRAIN_LEVELUP_AWARD_ITEM) {
			m_pOwner->giveAssert((*vecIt).m_id,(*vecIt).m_count,0,"调教升级");
			Role::stSynTipItem * pSyn = retCmd.add_getitems();
			if (pSyn){
				pSyn->set_itemid(CItem::realItemIDByJobAndSex((*vecIt).m_id,m_pOwner->getJob(),m_pOwner->getSex()));
				pSyn->set_num((*vecIt).m_count);
			}
		}
	}
	if(retCmd.getitems_size() > 0){
		m_pOwner->sendProto2Me(retCmd);
	}
}
