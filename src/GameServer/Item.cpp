#include "Item.h"
#include "GameUser.h"
#include "TblDataMgr.h"
#include "GameServer.h"
#include "GameEventNotify.h"
#include "GlobalSvrClient.h"
#include "MiscCfgMgr.h"

UINT16 CItem::s_ITEM_GEN_ID = 0;

CItem::CItem()
{
	m_pItemBase = NULL;
	bzero(&m_itemInfo,sizeof(m_itemInfo));
	m_battleProps.resize(eMAX_BATTLE_PROP);
	m_battlePower = 0;
}

CItem::CItem(stItemBase * pItemBase):m_pItemBase(pItemBase)
{
	bzero(&m_itemInfo,sizeof(m_itemInfo));
	m_battleProps.resize(eMAX_BATTLE_PROP);
	m_battlePower = 0;
}

CItem::~CItem()
{
	
}

bool CItem::init()
{
	m_pItemBase = g_itemBaseMgr.get(m_itemInfo.m_itemID);
	if (!m_pItemBase){
		return false;
	}
	setID(m_pItemBase->m_itemID);
	setName(m_pItemBase->m_name);
	setTempID(m_itemInfo.m_instanceID);
	setupPropState();
	return true;
}

void CItem::setupPropState()
{
	if (!m_pItemBase){
		return ;
	}

	m_battlePower = 0;
	m_battleProps.clear();
	m_battleProps.resize(eMAX_BATTLE_PROP);
	m_fuwenActiveBPVec.clear();
	m_raiseSkillBPVec.clear();

	if (m_pItemBase->m_bpValue.first < eMAX_BATTLE_PROP){
		if (eBATTLE_PROP_FUWEN_ACTIVATE == m_pItemBase->m_bpValue.first){//如果是符文激活属性
			m_fuwenActiveBPVec.push_back(m_pItemBase->m_bpValue.second);	
		}
		else if (eBATTLE_PROP_RAISE_SKILL == m_pItemBase->m_bpValue.first){//如果是技能强化属性
			m_raiseSkillBPVec.push_back(m_pItemBase->m_bpValue.second);	
		}
		else {
			m_battleProps[m_pItemBase->m_bpValue.first] += m_pItemBase->m_bpValue.second.m_val1;
		}
	}


	stItemLevelupBase * pRaiseBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(m_pItemBase->m_grownID,m_itemInfo.m_raiseLv));
	if (pRaiseBase){
		m_battleProps[eBATTLE_PROP_HP] += pRaiseBase->m_hp;
		m_battleProps[eBATTLE_PROP_DAMAGE] += pRaiseBase->m_damage;
        m_battlePower += pRaiseBase->m_addBattlePower;
        /*if (pRaiseBase->m_attrType < eMAX_BATTLE_PROP) {
            m_battleProps[pRaiseBase->m_attrType] += pRaiseBase->m_attrVal;
        }*/

		for(UINT16 i = 0; i < pRaiseBase->m_fuWenProps.size(); ++i){ 
			stBattlePropValue & bpValue = pRaiseBase->m_fuWenProps[i];
			if(bpValue.m_propID < eMAX_BATTLE_PROP){
				m_battleProps[bpValue.m_propID] += bpValue.m_val1;
			}
		}


		for (UINT16 i = 0;i < pRaiseBase->m_addProps.size(); ++i){
			stBattlePropValue & bpValue = pRaiseBase->m_addProps[i];
			if (bpValue.m_propID < eMAX_BATTLE_PROP){
				if (eBATTLE_PROP_FUWEN_ACTIVATE == bpValue.m_propID){//如果是符文激活属性
					m_fuwenActiveBPVec.push_back(bpValue);	
				}
				else if (eBATTLE_PROP_RAISE_SKILL == bpValue.m_propID){//如果是技能强化属性
					m_raiseSkillBPVec.push_back(bpValue);	
				}
				else {
					m_battleProps[bpValue.m_propID] += bpValue.m_val1;
				}
			}
			else {
				Athena::logger->error("计算装备属性时候读出配置表的战斗属性超出了枚举最大值");
			}
		}
	}

	stRefineBase * pRefineBase = g_refineBaseMgr.get(stRefineBase::getFindID(m_itemInfo.m_itemID,m_itemInfo.m_refineLv));
	if (pRefineBase){
        m_battlePower += pRefineBase->m_addBattlePower;
		std::map<UINT8,stBattlePropValue >::iterator it = pRefineBase->m_addProp.begin(); 
		for (;it != pRefineBase->m_addProp.end();++it){
			if (it->first < eMAX_BATTLE_PROP){
				if (eBATTLE_PROP_FUWEN_ACTIVATE == it->first){//如果是符文激活属性
					m_fuwenActiveBPVec.push_back(it->second);	
				}
				else if (eBATTLE_PROP_RAISE_SKILL == it->first){//如果是技能强化属性
					m_raiseSkillBPVec.push_back(it->second);	
				}
				else {
					m_battleProps[it->first] += it->second.m_val1;
				}
			}
			else {
				Athena::logger->error("计算装备属性时候读出配置表的战斗属性超出了枚举最大值");
			}
		}	
		
		it = pRefineBase->m_extraProp.begin();
		for (;it != pRefineBase->m_extraProp.end();++it){
			if (it->first < eMAX_BATTLE_PROP){
				if(eBATTLE_PROP_RAISE_SKILL == it->first){
					m_raiseSkillBPVec.push_back(it->second);
				}else{
					m_battleProps[it->first] += it->second.m_val1;	
				}
			}
			else {
				Athena::logger->error("计算装备属性时候读出配置表的战斗属性超出了枚举最大值");
			}
		}
	}

	stBreakBase * pBreakBase = g_breakBaseMgr.get(stBreakBase::getFindID(m_itemInfo.m_itemID,m_itemInfo.m_breakLv));
	if(pBreakBase){
		std::map<UINT8,stBattlePropValue >::iterator it = pBreakBase->m_addProp.begin();
		for(;it != pBreakBase->m_addProp.end(); ++it){
			if (it->first < eMAX_BATTLE_PROP){
				if(eBATTLE_PROP_FUWEN_ACTIVATE == it->first){//如果是符文激活属性
					m_fuwenActiveBPVec.push_back(it->second);
				}
				else if (eBATTLE_PROP_RAISE_SKILL == it->first){//如果是技能强化属性
					 m_raiseSkillBPVec.push_back(it->second);
				}
				else {
					m_battleProps[it->first] += it->second.m_val1;
				}
			}
			else{
				Athena::logger->error("计算装备属性时候读出配置表的战斗属性超出了枚举最大值");
			}
		}
	}

	for (UINT8 i = 0;i < MAX_EQUIP_SLOT_NUM; ++i){
		if (0 == m_itemInfo.m_slot[i].m_skillID){
			continue;
		}
		SkillGrownBase * pSkillGrown =  g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(m_itemInfo.m_slot[i].m_skillID,m_itemInfo.m_slot[i].m_skillLv));
		if (pSkillGrown){
			if (pSkillGrown->m_battleProp.m_propID < eMAX_BATTLE_PROP){
				m_battleProps[pSkillGrown->m_battleProp.m_propID] += pSkillGrown->m_battleProp.m_val1;
			}	
		}
	}

    if (m_itemInfo.m_pos.m_bagType == Role::BAG_TYPE_RUNE ||
            m_itemInfo.m_pos.m_bagType == Role::BAG_TYPE_HERO_RUNE) {
    } else {
        calcBattlePower();
    }
}

void CItem::calcPercentAdd()
{
	//Athena::logger->trace("计算百分比之前:hp:%d,damage:%d,hpPer:%d,damagePer:%d",m_battleProps[eBATTLE_PROP_HP],m_battleProps[eBATTLE_PROP_DAMAGE],m_battleProps[eBATTLE_PROP_HP_PERCENT],m_battleProps[eBATTLE_PROP_DAMAGE_PERCENT]);
	m_battleProps[eBATTLE_PROP_HP] = (UINT32)(m_battleProps[eBATTLE_PROP_HP] * (1 + m_battleProps[eBATTLE_PROP_HP_PERCENT] / 100.0f));
	m_battleProps[eBATTLE_PROP_DAMAGE] =(UINT32)(m_battleProps[eBATTLE_PROP_DAMAGE] * (1 + m_battleProps[eBATTLE_PROP_DAMAGE_PERCENT] / 100.0f));
	//Athena::logger->trace("计算百分比之后:hp:%d,damage:%d,hpPer:%d,damagePer:%d",m_battleProps[eBATTLE_PROP_HP],m_battleProps[eBATTLE_PROP_DAMAGE],m_battleProps[eBATTLE_PROP_HP_PERCENT],m_battleProps[eBATTLE_PROP_DAMAGE_PERCENT]);
}

void CItem::calcBattlePower()
{
	float battlePower = 0.0f;
	battlePower += m_battleProps[eBATTLE_PROP_HP] * BP_HP_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_MP] * BP_MP_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_DAMAGE] * BP_DAMAGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_HITRATE] * BP_HIT_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_DODGERATE] * BP_DODGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_BANGRATE] * BP_BANG_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_BANGDAMAGE] * BP_BANGDAMAGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_ANTIBANGRATE] * BP_ANTI_BANG_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_ANTIBANGDAMAGE] * BP_ANTI_BANGDAMAGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_FINALDAMAGE] * BP_FINAL_DAMAGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_RECOVERHP] * BP_RECOVERHP_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_ABSORBHP] * BP_ABSORBHP_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_REFLECTDAMAGE] * BP_DAMAGE_REFLECT_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_FIREDAMAGE] * BP_FIRE_DAMAGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_ICEDAMAGE] * BP_ICE_DAMAGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_THUNDERDAMAGE] * BP_THUNDER_DAMAGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_FIRERATE] * BP_FIRE_RATE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_ICERATE] * BP_ICE_RATE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_THUNDERRATE] * BP_THUNDER_RATE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_DAMAGEREDUCE] * BP_DAMAGEREDUCE_COEF;

	m_battlePower = (UINT32)battlePower;
}

bool   CItem::needSendWebLog(stItemBase * pItemBase)
{
	if (!pItemBase){
		return false;
	}
	
	if(CItem::isEquipPieceByType(pItemBase->m_type) 
			|| CItem::isEquipByType(pItemBase->m_type) 
			|| CItem::isFuwenByType(pItemBase->m_type)
			|| CItem::isFuwenPieceByType(pItemBase->m_type)
			|| CItem::isFashionPieceByType(pItemBase->m_type)
			|| CItem::isFashionByType(pItemBase->m_type)){//紫装以上才记录
	
		if (pItemBase->m_quality >= ITEM_CLASSTYPE_PURPLE_MIN_VAL){
			return true;
		}
		return false;
	}
	return true;
}

bool CItem::canOverlayWithItem(const CItem * pItem)
{
	if (!pItem || !pItem->m_pItemBase){
		return false;
	}

	if (pItem->m_itemInfo.m_itemID != m_itemInfo.m_itemID){
		return false;
	}

	if (pItem->m_itemInfo.m_raiseLv != m_itemInfo.m_raiseLv){
		return false;
	}

	if (m_itemInfo.m_num + pItem->m_itemInfo.m_num > pItem->m_pItemBase->m_overlapNum){
		return false;
	}
	
	if (pItem->m_itemInfo.m_exp != m_itemInfo.m_exp){
		return false;
	}

	if (pItem->m_itemInfo.m_refineLv != m_itemInfo.m_refineLv){
		return false;
	}

	if (pItem->m_itemInfo.m_breakLv != m_itemInfo.m_breakLv){
		return false;
	}

	return true;
}

bool  CItem::canBatchUse(UINT16 type)
{
	switch (type){
		case ITEM_TYPE_BOX1 :
		case ITEM_TYPE_BOX2 :
		case ITEM_TYPE_BOX4 :
		case ITEM_TYPE_BOX9 :
		case ITEM_TYPE_GIVE_VIPEXP:
		case ITEM_TYPE_VIPEXP:
			{
				return true;
			}
	}
	return false;
}
	
bool   CItem::isGreen(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_GREEN_MIN_VAL) && (quality <= ITEM_CLASSTYPE_GREEN_MAX_VAL);
}

bool   CItem::isBlue(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_BULE_MIN_VAL) && (quality <= ITEM_CLASSTYPE_BULE_MAX_VAL);
}

bool   CItem::isPurplue(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_PURPLE_MIN_VAL) && (quality <= ITEM_CLASSTYPE_PURPLE_MAX_VAL);
}

bool   CItem::isOrange(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_ORANGE_MIN_VAL) && (quality <= ITEM_CLASSTYPE_ORANGE_MAX_VAL);
}

bool   CItem::isN(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_N_MIN_VAL) && (quality <= ITEM_CLASSTYPE_N_MAX_VAL);
}
bool   CItem::isR(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_R_MIN_VAL) && (quality <= ITEM_CLASSTYPE_R_MAX_VAL);
}
bool   CItem::isSR(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_SR_MIN_VAL) && (quality <= ITEM_CLASSTYPE_SR_MAX_VAL);
}

bool   CItem::isSSR(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_SSR_MIN_VAL) && (quality <= ITEM_CLASSTYPE_SSR_MAX_VAL);
}

bool   CItem::isUR(UINT8 quality)
{
	return (quality >= ITEM_CLASSTYPE_UR_MIN_VAL) && (quality <= ITEM_CLASSTYPE_UR_MAX_VAL);
}

bool CItem::isWuHunShi(UINT32 itemID)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	
	if (ITEM_TYPE_WUHUNSHI == pItemBase->m_type){
		return true;
	}
	return false;
}
	
bool CItem::isWuHunShiByType(UINT16 type)
{
	if (ITEM_TYPE_WUHUNSHI == type){
		return true;
	}
	return false;
}

bool CItem::isPiece(UINT32 itemID)
{
	return isFuwenPiece(itemID) || isEquipPiece(itemID) ||isFashionPiece(itemID);
}

bool CItem::isFuwenPieceByType(UINT16 type)
{
	switch (type){
		case ITEM_TYPE_FUWEN_FIRE_PIECE:
		case ITEM_TYPE_FUWEN_WATER_PIECE:
		case ITEM_TYPE_FUWEN_WIND_PIECE:
		case ITEM_TYPE_FUWEN_SOIL_PIECE:
		case ITEM_TYPE_FUWEN_LIFE_BOOK_PIECE:
			{
				return true;
			}
			break;
	}
	return false;
}

bool CItem::isFuwenPiece(UINT32 itemID)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	return isFuwenPieceByType(pItemBase->m_type);	
}

bool CItem::isFuwen(UINT32 itemID)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	return isFuwenByType(pItemBase->m_type);
}

bool CItem::isFuwenByType(UINT16 type)
{
	switch (type){
		case ITEM_TYPE_FUWEN_CORE:
		case ITEM_TYPE_FUWEN_FIRE:
		case ITEM_TYPE_FUWEN_WATER:
		case ITEM_TYPE_FUWEN_WING:
		case ITEM_TYPE_FUWEN_SOIL:
			{
				return true;
			}
			break;
	}
	return false;
}

bool CItem::isFashionPiece(UINT32 itemID)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	return isFashionPieceByType(pItemBase->m_type);
}

bool CItem::isFashionPieceByType(UINT16 type)
{
	switch (type){
		case ITEM_TYPE_FASHION_HEAD_PIECE :
		case ITEM_TYPE_FASHION_BODY_PIECE :
		case ITEM_TYPE_FASHION_WEAPON_PIECE :
		case ITEM_TYPE_FASHION_SHOE_PIECE :
		case ITEM_TYPE_FASHION_FACE_PIECE :
		case ITEM_TYPE_FASHION_CLOAK_PIECE :
		case ITEM_TYPE_FASHION_RING_PIECE :
		case ITEM_TYPE_FASHION_WING_PIECE :
		case ITEM_TYPE_FASHION_SUIT_PIECE :
		{
			return true;
		}
		break;
	}
	return false;
}

bool CItem::isFashion(UINT32 itemID)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	return isFashionByType(pItemBase->m_type);
}

bool CItem::isFashionByType(UINT16 type)
{
	switch (type){
		case ITEM_TYPE_FASHION_HEAD :        //时装头部
		case ITEM_TYPE_SPECIAL_FASHION_HEAD : //特殊头部时装
		case ITEM_TYPE_FASHION_BODY :        //时装上衣
		case ITEM_TYPE_FASHION_WEAPON :      //时装武器
		case ITEM_TYPE_FASHION_SHOE:         //时装鞋子
		case ITEM_TYPE_FASHION_FACE:	     //时装表情
		case ITEM_TYPE_FASHION_CLOAK:        //时装披风
		case ITEM_TYPE_FASHION_RING:         //时装戒子
		case ITEM_TYPE_FASHION_WING:         //时装翅膀
		case ITEM_TYPE_FASHION_SUIT:         //时装套装
			{
				return true;
			}
			break;
	}	
	return false;
}

bool CItem::isRune(UINT32 itemID) {
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	return isRuneByType(pItemBase->m_type);
}

bool CItem::isRuneByType(UINT16 type) {
    switch (type) {
        case ITEM_TYPE_FUWEN_STONE_FIRE:    //高阶火符文
        case ITEM_TYPE_FUWEN_STONE_ICE:     //高阶冰符文
        case ITEM_TYPE_FUWEN_STONE_THUNDER: //高阶雷符文
        case ITEM_TYPE_FUWEN_STONE_EXP:     //高阶符文经验丹
            {
                return true;
            }
            break;
    }
    return false;
}

bool CItem::isRunePiece(UINT32 itemID) {
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	return isRunePieceByType(pItemBase->m_type);
}

bool CItem::isRunePieceByType(UINT16 type) {
    switch (type) {
        case ITEM_TYPE_FUWEN_STONE_PIECE_FIRE:    //高阶火符文原石
        case ITEM_TYPE_FUWEN_STONE_PIECE_ICE:     //高阶冰符文原石
        case ITEM_TYPE_FUWEN_STONE_PIECE_THUNDER: //高阶雷符文原石
            {
                return true;
            }
            break;
    }
    return false;
}

bool CItem::isMountStone(UINT32 itemID)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);
	if (!pItemBase){
		return false;
	}
	return isMountStoneByType(pItemBase->m_type);
}

bool CItem::isMountStoneByType(UINT16 type)
{
	switch (type) {
		case ITEM_TYPE_MOUNT_LIFE_STONE:
		case ITEM_TYPE_MOUNT_DAMAGE_STONE:
		case ITEM_TYPE_MOUNT_HIT_STONE:
		case ITEM_TYPE_MOUNT_CRIT_STONE:
		case ITEM_TYPE_MOUNT_RESIST_STONE:
		case ITEM_TYPE_MOUNT_FIRE_STONE:
		case ITEM_TYPE_MOUNT_ICE_STONE:
		case ITEM_TYPE_MOUNT_RAY_STONE:
			{
				return true;
			}
			break;
	}
	return false;
}

bool CItem::isEquipPieceByType(UINT16 type)
{
	switch (type){
		case ITEM_TYPE_HEAD_PIECE :
		case ITEM_TYPE_BODY_PIECE :
		case ITEM_TYPE_WEAPON_PIECE :
		case ITEM_TYPE_SHOE_PIECE :
		case ITEM_TYPE_NECKLACE_PIECE :
		case ITEM_TYPE_CLOAK_PIECE :
		case ITEM_TYPE_RING_PIECE:
		case ITEM_TYPE_WING_PIECE :
			{
				return true;
			}
			break;
	}
	return false;
}

bool CItem::isEquipPiece(UINT32 itemID)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	return isEquipPieceByType(pItemBase->m_type);
}

bool CItem::isFuWenExpStone(UINT16 type)
{
	switch (type){
		case ITEM_TYPE_FUWEN_EXP_STONE :
		{
			return true;
		}
	}
	return false;
}

bool CItem::isRuneExpStone(UINT16 type) {
	switch (type){
		case ITEM_TYPE_FUWEN_STONE_EXP :
		{
			return true;
		}
	}
	return false;
}

bool CItem::isEquipByType(UINT16 type)
{
	switch (type){
		case ITEM_TYPE_HEAD :
		case ITEM_TYPE_BODY :
		case ITEM_TYPE_WEAPON :
		case ITEM_TYPE_SHOE :
		case ITEM_TYPE_NECKLACE :
		case ITEM_TYPE_CLOAK :
		case ITEM_TYPE_RING :
		case ITEM_TYPE_WING :
		case ITEM_TYPE_SUIT :
			{
				return true;
			}
			break;
	}
	return false;
}

bool CItem::isEquipByID(UINT32 itemID)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return false;
	}
	return isEquipByType(pItemBase->m_type);
}

UINT32 CItem::realItemIDByJobAndSex(UINT32 itemID,UINT8 job,UINT8 sex)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);	
	if (!pItemBase){
		return itemID;
	}
	
	if (!(isEquipByType(pItemBase->m_type) 
				|| isEquipPiece(itemID) 
				|| isFashionPieceByType(pItemBase->m_type)
				|| isFashionByType(pItemBase->m_type))
                || isRuneByType(pItemBase->m_type)
                || isRunePiece(pItemBase->m_type)){
		return itemID;
	}

	if (job == Role::eJOB_WARRIOR){
		if (sex == Role::eSEX_MAN){
			return itemID;	
		}
		else if (sex == Role::eSEX_FEMALE){
			return itemID + 1;
		}
	}
	else if (job == Role::eJOB_BOWMAN){
		if (sex == Role::eSEX_MAN){
			return itemID + 2;
		}
		else if (sex == Role::eSEX_FEMALE){
			return itemID + 3;
		}
	}
	else if (job == Role::eJOB_MASTER){
		if (sex == Role::eSEX_MAN){
			return itemID + 4;
		}
		else if (sex == Role::eSEX_FEMALE){
			return itemID + 5;
		}
	}
	return itemID;
}

extern GameServer * g_gameServer;

UINT64 CItem::genInstanceID()
{
	UINT64 ret = 0;
	UINT64 cur = g_gameServer->getSec();

	UINT64 svrID = g_gameServer->m_svrID;
	UINT64 genID = ++s_ITEM_GEN_ID; 
	
	ret = (cur << 32) | (svrID << 16) | genID;
	return ret;
}

bool CItem::hasEquipSkill()
{
	for (UINT16 i = 0;i < MAX_EQUIP_SLOT_NUM;++i){
		if (m_itemInfo.m_slot[i].m_skillID){
			return true;
		}
	}	
	return false;
}

void CItem::exechangegSkill(CItem * pItem)
{
	if (!pItem){
		return ;
	}	
	
	UINT32 skillID = 0;
	UINT16 skillLv = 0;
	for (UINT16 i = 0;i < MAX_EQUIP_SLOT_NUM;++i){
		skillID = m_itemInfo.m_slot[i].m_skillID;
		skillLv = m_itemInfo.m_slot[i].m_skillLv;
		m_itemInfo.m_slot[i].m_skillID = pItem->m_itemInfo.m_slot[i].m_skillID;
		m_itemInfo.m_slot[i].m_skillLv = pItem->m_itemInfo.m_slot[i].m_skillLv;

		pItem->m_itemInfo.m_slot[i].m_skillID = skillID;
		pItem->m_itemInfo.m_slot[i].m_skillLv = skillLv;
	}
}

void CItem::fillSynData(Role::stSynItemInfo & data)
{
	data.set_itemid(getID());
	data.set_instanceid(getTempID());
	data.set_damage(m_battleProps[eBATTLE_PROP_DAMAGE]);
	data.set_maxhp(m_battleProps[eBATTLE_PROP_HP]);
	data.set_num(m_itemInfo.m_num);
	data.set_raiselv(m_itemInfo.m_raiseLv);
	data.mutable_bagpos()->set_bagtype(m_itemInfo.m_pos.m_bagType);
	data.mutable_bagpos()->set_pos(m_itemInfo.m_pos.m_pos);
	data.set_exp(m_itemInfo.m_exp);
	data.set_refinelv(m_itemInfo.m_refineLv);
	data.set_obtainskill(m_itemInfo.m_hasGotSkill);
	data.set_breaklv(m_itemInfo.m_breakLv);

	for (UINT8 i = 0;i < MAX_EQUIP_SLOT_NUM; ++i){
		Role::stItemSkillInfo * pSyn = data.add_skillinfo();
		if (pSyn){
			pSyn->set_skillid(m_itemInfo.m_slot[i].m_skillID);
			pSyn->set_skilllv(m_itemInfo.m_slot[i].m_skillLv);
		}
	}
	data.set_battlepower(m_battlePower);
	
	for (UINT16 i = 0;i < m_raiseSkillBPVec.size();++i){
		stBattlePropValue & prop = m_raiseSkillBPVec[i];
		Role::stRaiseSkillProp *pSyn = data.add_raiseskillprop();
		if (pSyn){
			pSyn->set_skillid(prop.m_val1);
			pSyn->set_raisetype(prop.m_val2);
			pSyn->set_raisevalue(prop.m_val3);
		}
	}

	if (m_itemInfo.m_expireTime > 0){
		UINT32 remainTime = m_itemInfo.m_expireTime > g_gameServer->getSec() ? m_itemInfo.m_expireTime - g_gameServer->getSec() : 0 ;
		data.set_remaintime(remainTime);
	}
}

struct stGetItemCallBackByCond : public stItemCallBack
{
	std::vector<CItem * > & m_allItemVec;
	UINT32 m_itemID;
	UINT16  m_raiseLv;

	stGetItemCallBackByCond(std::vector<CItem * > & allItem,UINT32 itemID,UINT16 raiseLv) : m_allItemVec(allItem),m_itemID(itemID),m_raiseLv(raiseLv)
	{}
	
	bool doIt(CItem * pItem)
	{
		if (!pItem){
			return true;
		}	
		
		if (pItem->getID() != m_itemID){
			return true;	
		}

		if (m_raiseLv){
			if (pItem->m_itemInfo.m_raiseLv != m_raiseLv){
				return true;
			}
		}

		m_allItemVec.push_back(pItem);
		return true;
	}
};

CBag::CBag(UINT16 slotNum,UINT8 bagType) : ElementMgr(true,true,false,true),m_bagSize(slotNum),m_capacity(slotNum)
{
	m_pSlot  = ATHENA_NEW CItem *[slotNum];
	bzero(m_pSlot,sizeof(CItem *) * slotNum);
	m_bagType = bagType;
}

CBag::~CBag()
{
	if (m_pSlot){
		for (UINT16 i = 0;i < m_capacity; ++i){
			SAFE_DELETE(m_pSlot[i]);	
		}	
	}
	SAFE_DELETE_ARRAY(m_pSlot);
}

bool CBag::addItem(CItem * pItem)
{
	if (!pItem){
		return false;
	}
	
	if (!checkAddItem(pItem)){
		return false;
	}
	
	UINT8 index = pItem->m_itemInfo.m_pos.m_pos;
	m_pSlot[index] = pItem;
	return true;
}

bool CBag::removeItem(CItem * pItem)
{
	if (!pItem){
		return false;
	}

	if (pItem->m_itemInfo.m_pos.m_bagType != m_bagType){
		Athena::logger->error("包裹删除道具失败,包裹类型不正确");
		return false;
	}

	UINT8 index = pItem->m_itemInfo.m_pos.m_pos;
	if (index >= m_capacity){
		Athena::logger->error("包裹删除道具失败,道具的pos坐标不正确!");
		return false;
	}

	if (!m_pSlot[index]){
		Athena::logger->error("包裹删除道具失败,所在位置已经有道具了");
		return false;
	}

	removeElement(pItem);
	m_pSlot[index] = NULL;
	return true;
}
	
UINT16 CBag::getItemSize()
{
	return ElementMgr::size();	
}

bool CBag::checkAddItem(CItem * pItem)
{
	if (!pItem){
		Athena::logger->error("包裹放置道具失败,道具指针为空");
		return false;
	}
	
	if (pItem->m_itemInfo.m_pos.m_bagType != m_bagType){
		Athena::logger->error("包裹放置道具失败,包裹类型不正确");
		return false;
	}
	
	UINT8 index = pItem->m_itemInfo.m_pos.m_pos ;
	if (index >= m_bagSize){
		Athena::logger->error("包裹放置道具失败,道具的pos坐标不正确!");
		return false;
	}

	if (m_pSlot[index]){
		Athena::logger->error("包裹放置道具失败,所在位置已经有道具了");
		return false;
	}

	if (!addElement(pItem)){
		Athena::logger->error("包裹放置道具失败,往索引容器添加道具时候失败!");
		return false;
	}
	return true;
}

UINT16 CBag::getFreeSlotIndex()
{
	if (!m_pSlot){
		return (UINT16)-1;
	}
	
	for (UINT16 i = 0;i < m_bagSize; ++i){
		if (!m_pSlot[i]){
			return i;
		}		
	}
	return (UINT16)-1;
}

CItem * CBag::getItemBySlotIndex(UINT16 index)
{
	if (index >= m_capacity){
		return NULL;
	}
	return m_pSlot[index];	
}

bool  CBag::checkSlotIndex(UINT16 index)
{
	if (index >= m_capacity){
		return false;
	}
	return true;
}

UINT32 CBag::getItemByID(UINT32 itemID,UINT64 exceptInstanceID,std::vector<CItem * > & vec)
{
	UINT32 ret = 0;
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (m_pSlot[i]){
			CItem * pItem = m_pSlot[i];
			if ((pItem->m_itemInfo.m_itemID == itemID) && (exceptInstanceID != pItem->m_itemInfo.m_instanceID)){
				vec.push_back(pItem);
				ret += pItem->m_itemInfo.m_num;
			}
		}		
	}
	return ret;
}

void CBag::exeEveryOne(stItemCallBack & cb)
{
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (m_pSlot[i]){
			CItem * pItem = m_pSlot[i];
			if (!cb.doIt(pItem)){
				return ;
			}
		}		
	}
}

UINT16 CBag::getFreeSlotNum()
{
	if (m_bagSize <= getItemSize()){
		return  0;
	}	

	return m_bagSize - getItemSize();
}

CItem * CBag::getOverlayItem(CItem * pItem)
{
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (m_pSlot[i]){
			CItem * pDestItem = m_pSlot[i];
			if (pItem->canOverlayWithItem(pDestItem)){
				return pDestItem;
			}
		}		
	}
	return NULL;
}

void CBag::refreshBagItem(GameUser * pUser)
{
	if (!pUser){
		return ;
	}

	Role::stRefreshMainBag retCmd;
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (m_pSlot[i]){
			CItem * pDestItem = m_pSlot[i];
			Role::stSynItemInfo * pSynInfo = retCmd.add_data();
			if (pSynInfo) {
				pDestItem->fillSynData(*pSynInfo);
                pUser->synEnchant(pSynInfo);
			}
		}		
	}
	pUser->sendProto2Me(retCmd);
}

UINT32 CBag::getFashionNum() {
    UINT32 num = 0;
	for (UINT16 i = 0;i < m_bagSize; ++i){
        CItem * pItem = m_pSlot[i];
        if (pItem && pItem->isFashionByType(pItem->m_pItemBase->m_type) &&
                pItem->m_pItemBase->m_expireTime <= 0) {
            num++;
        }
    }
    return num;
}

/*UINT16 CBag::fillAllItem(Proto::stSynItemInfo  * data)
{
	if (!data){
		return 0;
	}
	UINT16 ret = 0;
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (m_pSlot[i]){
			CItem * pDestItem = m_pSlot[i];
			pDestItem->fillSynData(data[ret]);
			ret += 1;
		}		
	}
	return ret;
}*/

CMainBag::CMainBag(UINT8 bagType,UINT16 slotNum) : CBag(slotNum,bagType)
{
}

CMainBag::~CMainBag()
{
}

void CMainBag::reArrange()
{
	std::map<UINT32,std::vector<CItem *> > sortMap[2];
	//先排序
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (m_pSlot[i]){
			CItem * pItem = m_pSlot[i];
			UINT32 sortID = pItem->m_pItemBase->m_sortID;
			ItemCombineBase * pCombineBase = g_combineBaseMgr.get(pItem->getID());
			if (pCombineBase && (pItem->m_itemInfo.m_num >= pCombineBase->m_pieceNum)){
				sortMap[0][sortID].push_back(pItem);
			}
			else {
				sortMap[1][sortID].push_back(pItem);
			}
		}		
	}
	
	for (UINT8 k = 0;k < 2;++k){
		//合并
		std::map<UINT32,std::vector<CItem *> >::iterator it =  sortMap[k].begin();	
		for (;it != sortMap[k].end(); ++it){
			std::vector<CItem *> & vec = it->second;
			for (UINT16 i = 0;i < vec.size(); ++i){
				for (UINT16 j = i+ 1; j < vec.size(); ++j){
					CItem * pItemi = vec[i];
					CItem * pItemj = vec[j];
					if (pItemi && pItemj){
						if (pItemi->canOverlayWithItem(pItemj)){
							pItemi->m_itemInfo.m_num += pItemj->m_itemInfo.m_num;	
							removeElement(pItemj);
							SAFE_DELETE(pItemj);
							vec[j] = NULL;
						}
					}	
				}	
			}
		}
	}

	bzero(m_pSlot,sizeof(CItem *) * m_capacity);
	UINT16 index = 0;
	for (UINT8 k = 0;k < 2;++k){
		std::map<UINT32,std::vector<CItem *> >::iterator it = sortMap[k].begin();
		for (;it != sortMap[k].end() ; ++it){
			for (UINT16 i = 0;i < it->second.size(); ++i){
				CItem * pItem = it->second[i];
				if (pItem){
					pItem->m_itemInfo.m_pos.m_pos = index;
					m_pSlot[index] = pItem;
					index += 1;
				}
			}
		}
	}
}

CEquipBag::CEquipBag(UINT8 type) : CBag(s_EQUIP_BAG_SLOT_NUM, type)
{
}

CEquipBag::~CEquipBag()
{
}

bool CEquipBag::isAffectAppearance(BagPos & pos)
{
	if (pos.m_bagType != Role::BAG_TYPE_EQUIP){
		return false;
	}
	
	UINT32 index = pos.m_pos;

	if ((index >= HEAD_SLOT_INDEX) && (index <= FASHION_WING_SLOT_INDEX)){
		return true;
	}
	return false;
}

bool CEquipBag::isAffectRoleData(BagPos & pos)
{
	if (pos.m_bagType != Role::BAG_TYPE_EQUIP){
		return false;
	}
	return true;
}


bool CEquipBag::checkAddItem(CItem * pItem)
{
	if (!pItem){
		return false;
	}
	
	if (CItem::isFuwenByType(pItem->m_pItemBase->m_type)){//符文的判断方法
		UINT8 beginPos = 0;
		if (EQUIP_POS_FUWEN_CORE == pItem->m_pItemBase->m_equipPart){
			beginPos = HEAD_CORE_FUWEN_INDEX;		
		}
		else if (EQUIP_POS_FUWEN_FIRE == pItem->m_pItemBase->m_equipPart){
			beginPos = HEAD_FIRE_FUWEN_INDEX; 
		}
		else if (EQUIP_POS_FUWEN_WATER == pItem->m_pItemBase->m_equipPart){
			beginPos = HEAD_WATER_FUWEN_INDEX;	
		}
		else if (EQUIP_POS_FUWEN_WIND == pItem->m_pItemBase->m_equipPart){
			beginPos = HEAD_WIND_FUWEN_INDEX; 
		}
		else if (EQUIP_POS_FUWEN_SOIL == pItem->m_pItemBase->m_equipPart){
			beginPos = HEAD_SOIL_FUWEN_INDEX;	
		}
		else {
			return false;
		}

		if (((pItem->m_itemInfo.m_pos.m_pos - beginPos) % 5) != 0){
			return false;
		}
	}
	else {//装备判断方法
		UINT8 slotIndex = getSlotIndexByItemType(pItem->m_pItemBase->m_type);
		if (slotIndex != pItem->m_itemInfo.m_pos.m_pos){
			if (pItem->m_itemInfo.m_pos.m_pos < SOUL_BAG_POS1_INDEX
					|| pItem->m_itemInfo.m_pos.m_pos > MOUNT_BAG_POS8_INDEX){
				Athena::logger->error("添加道具到包裹错误,该装备位置不对");
				return false;
			}
		}
	}
	return CBag::checkAddItem(pItem);
}

UINT8 CEquipBag::getSlotIndexByItemType(UINT16 type)
{
	switch (type) {	
		case ITEM_TYPE_FASHION_HEAD :		//时装头部
		case ITEM_TYPE_SPECIAL_FASHION_HEAD :   //特殊时装头部
			{
				return FASHION_HEAD_SLOT_INDEX;	
			}
			break;
		case ITEM_TYPE_FASHION_BODY :		//时装上衣
			{
				return FASHION_BODY_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_FASHION_SUIT :
			{
				return FASHION_BODY_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_FASHION_WEAPON :	//时装武器
			{
				return FASHION_WEAPON_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_FASHION_SHOE :		//时装鞋子
			{
				return FASHION_SHOE_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_FASHION_FACE : //时装项链
			{
				return FASHION_FACE_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_FASHION_CLOAK :	//时装披风
			{
				return FASHION_CLOAK_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_FASHION_RING :		//时装戒子
			{
				return FASHION_RING_SLOT_INDEX;	
			}
			break;
		case ITEM_TYPE_FASHION_WING :	//时装翅膀
			{
				return FASHION_WING_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_HEAD :			//头部
			{
				return HEAD_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_BODY : 		//衣服
			{
				return BODY_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_WEAPON :		//武器
			{
				return WEAPON_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_SHOE :		//鞋子
			{
				return SHOE_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_NECKLACE :	//项链
			{
				return NECKLACE_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_CLOAK :			//披风
			{
				return CLOAK_SLOT_INDEX;
			}
			break;
		case ITEM_TYPE_RING :			//戒子
			{
				return RING_SLOT_INDEX;		
			}
			break;
		case ITEM_TYPE_WING:			//翅膀
			{
				return WING_SLOT_INDEX;
			}
			break;
	}
	return (UINT8)-1;
}

CItem * CEquipBag::getEquipByIndex(UINT8 index)
{
	if (index >= s_EQUIP_BAG_SLOT_NUM){
		return NULL;		
	}	
	return m_pSlot[index];
}

UINT8 CEquipBag::getFuwenSlotIndex(UINT16 type,UINT16 indexOffset)
{
	if (ITEM_TYPE_FUWEN_CORE == type){
		return  indexOffset;
	}
	else if (ITEM_TYPE_FUWEN_FIRE == type){
		return 1 + indexOffset;	
	}
	else if (ITEM_TYPE_FUWEN_WATER == type){
		return 2 + indexOffset;
	}
	else if (ITEM_TYPE_FUWEN_WING == type){
		return 3 + indexOffset;
	}
	else if (ITEM_TYPE_FUWEN_SOIL == type){
		return 4 + indexOffset;
	}
	return (UINT8)-1;
}

UINT8 CEquipBag::getEquipIndexByFuwenIndex(UINT8 index)
{
	switch (index){
		case HEAD_CORE_FUWEN_INDEX :			//头部核心符文
		case HEAD_FIRE_FUWEN_INDEX :			//头部火符文
		case HEAD_WATER_FUWEN_INDEX :			//头部水符文
		case HEAD_WIND_FUWEN_INDEX :			//头部风符文
		case HEAD_SOIL_FUWEN_INDEX :			//头部土符文
			{
				return HEAD_SLOT_INDEX;	
			}
			break;
		case BODY_CORE_FUWEN_INDEX :			//衣服核心符文
		case BODY_FIRE_FUWEN_INDEX :			//衣服火符文
		case BODY_WATER_FUWEN_INDEX :			//衣服水符文
		case BODY_WIND_FUWEN_INDEX :			//衣服风符文
		case BODY_SOIL_FUWEN_INDEX :			//衣服土符文
			{
				return BODY_SLOT_INDEX;	
			}
			break;
		case WEAPON_CORE_FUWEN_INDEX :			//武器核心符文
		case WEAPON_FIRE_FUWEN_INDEX :			//武器火符文
		case WEAPON_WATER_FUWEN_INDEX :			//武器水符文
		case WEAPON_WIND_FUWEN_INDEX :			//武器风符文
		case WEAPON_SOIL_FUWEN_INDEX :			//武器土符文
			{
				return WEAPON_SLOT_INDEX;
			}
			break;
		case SHOE_CORE_FUWEN_INDEX :			//鞋核心符文
		case SHOE_FIRE_FUWEN_INDEX :			//鞋火符文
		case SHOE_WATER_FUWEN_INDEX :			//鞋水符文
		case SHOE_WIND_FUWEN_INDEX :			//鞋风符文
		case SHOE_SOIL_FUWEN_INDEX :			//鞋土符文
			{
				return SHOE_SLOT_INDEX;
			}
			break;
		case FACE_CORE_FUWEN_INDEX :			//脸核心符文
		case FACE_FIRE_FUWEN_INDEX :			//脸火符文
		case FACE_WATER_FUWEN_INDEX :			//脸水符文
		case FACE_WIND_FUWEN_INDEX :			//脸风符文
		case FACE_SOIL_FUWEN_INDEX :			//脸土符文
			{
				return NECKLACE_SLOT_INDEX;
			}
			break;
		case CLOAK_CORE_FUWEN_INDEX :			//披风核心符文
		case CLOAK_FIRE_FUWEN_INDEX :			//披风火符文
		case CLOAK_WATER_FUWEN_INDEX :			//披风水符文
		case CLOAK_WIND_FUWEN_INDEX :			//披风风符文
		case CLOAK_SOIL_FUWEN_INDEX :			//披风土符文
			{
				return CLOAK_SLOT_INDEX;	
			}
			break;
		case RING_CORE_FUWEN_INDEX :			//戒子核心符文
		case RING_FIRE_FUWEN_INDEX :			//戒子火符文
		case RING_WATER_FUWEN_INDEX :			//戒子水符文
		case RING_WIND_FUWEN_INDEX :			//戒子风符文
		case RING_SOIL_FUWEN_INDEX :			//戒子土符文
			{
				return RING_SLOT_INDEX;
			}
			break;
	}
	return (UINT8)-1;
}

void CEquipBag::calcEquipData()
{
	m_data.reset();
	//先计算灵魂属性
	std::vector<CItem *> wearEquipVec;
	std::vector<UINT32> allEquipIDVec;

	UINT32 totalEquipRaiseLv = 0;
	UINT16 totalEquipRefineLv = 0;
	UINT16 totalEquipBreakLv = 0;
	for (UINT8 i = HEAD_SLOT_INDEX;i <= WING_SLOT_INDEX;++i){
		CItem * pItem = m_pSlot[i];
		if (pItem){
			wearEquipVec.push_back(pItem);
			allEquipIDVec.push_back(pItem->getID());

			if (WING_SLOT_INDEX == i){//是翅膀
				m_data.m_wingAveRefineLv = pItem->m_itemInfo.m_refineLv;
				m_data.m_wingAveBreakLv = pItem->m_itemInfo.m_breakLv;
			}
			else {//非翅膀
				totalEquipRefineLv += pItem->m_itemInfo.m_refineLv;
				totalEquipBreakLv += pItem->m_itemInfo.m_breakLv;
			}
			totalEquipRaiseLv += pItem->m_itemInfo.m_raiseLv; 
		}
	}
	
	if (wearEquipVec.size() > 0){
		m_data.m_equipAveRaiseLv = (UINT16)((totalEquipRaiseLv * 1.0f) / wearEquipVec.size() + 0.5f);
		m_data.m_equipAveRefineLv = (UINT8)((totalEquipRefineLv * 1.0f )/ wearEquipVec.size() + 0.5f);
		m_data.m_equipAveBreakLv = (UINT8)((totalEquipBreakLv * 1.0f)/ wearEquipVec.size() + 0.5f);
	}

	for (UINT8 i = SOUL_BAG_POS1_INDEX;i <= SOUL_BAG_POS6_INDEX; ++i){
		CItem * pItem = m_pSlot[i];
		if (pItem){
			allEquipIDVec.push_back(pItem->getID());
		}
	}
	
	//计算装备的套装
	for (UINT8 i = 0;i < wearEquipVec.size();++i){
		CItem * pItem = wearEquipVec[i];
		if (pItem){
			pItem->setupPropState();
			for (UINT8 j = 1;j <= 6; ++j){
				stSoulinkBase * pSoulinkBase = g_soulinkBaseMgr.get(stSoulinkBase::getFindID(pItem->m_pItemBase->m_grownID,j));
				UINT8 equipNum = 0;
				if (pSoulinkBase){
					std::set<UINT32> & needEquipID = pSoulinkBase->getNeedEquiID(getOwnerJob(),getOwnerSex());
					for (UINT8 k = 0;k < allEquipIDVec.size();++k){
						if (needEquipID.find(allEquipIDVec[k]) != needEquipID.end()){//有一件装备存在
							equipNum += 1;
						}
					}

					if (equipNum >= pSoulinkBase->m_needNum){//激活了套装
						if (pSoulinkBase->m_prop.first < pItem->m_battleProps.size()){
							pItem->m_battleProps[pSoulinkBase->m_prop.first] += pSoulinkBase->m_prop.second;	
						}

						pItem->calcBattlePower();

#ifdef _HDZ_DEBUG
						Athena::logger->trace("%d,%s,装备激活了套装(%d,%d) (equipNum:%d,needNum:%d) ",pItem->getID(),pItem->getName(),pSoulinkBase->m_id,j,equipNum,pSoulinkBase->m_needNum);
						if (202624 == pItem->getID()){
							Athena::logger->trace("贤者法帽的套装需要的装备:");
							std::set<UINT32>::iterator needIt = needEquipID.begin(); 
							for (;needIt != needEquipID.end();++needIt){
								Athena::logger->trace("itemID:%d",*needIt);	
							}

							Athena::logger->trace("身上传的装备有:");
							for (UINT16 f = 0;f < allEquipIDVec.size();++f){
								Athena::logger->trace("itemID:%d",allEquipIDVec[f]);	
							}
						}
#endif
					}
				}
			}
			pItem->calcPercentAdd();
		}
	}
	
	UINT32 totalFuwenRaiseLv = 0;
	UINT16 totalFuwenQuality = 0;
	
	//符文激活属性
	for (UINT8 i = 0;i <= RING_SOIL_FUWEN_INDEX;++i){
		CItem * pItem = m_pSlot[i];
		bool bAddProp  = true;
		if (pItem){
			if ((i>= HEAD_CORE_FUWEN_INDEX) && (i <= RING_SOIL_FUWEN_INDEX)){//如果是符文,计算符文的激活属性
				totalFuwenRaiseLv += pItem->m_itemInfo.m_raiseLv;
				totalFuwenQuality += pItem->m_pItemBase->m_quality;
				pItem->setupPropState();
				bool needCalcBP = false;
				for (UINT8 j = 0;j < pItem->m_fuwenActiveBPVec.size();++j){
					stBattlePropValue & bpPropVal = pItem->m_fuwenActiveBPVec[j];
					UINT8 equipIndex = getEquipIndexByFuwenIndex(i);
					CItem *pEquip = getEquipByIndex(equipIndex);
					if (pEquip) {
						if (bpPropVal.m_val1 == ActivePropType_Raise){//检查强化属性
							if (pEquip->m_itemInfo.m_raiseLv >= bpPropVal.m_val2){//激活了
								if (bpPropVal.m_val3 < eMAX_BATTLE_PROP){
									pItem->m_battleProps[bpPropVal.m_val3] += bpPropVal.m_val4;
									needCalcBP = true;
#ifdef _HDZ_DEBUG
									//Athena::logger->trace("itemID:%d,强化激活了:propID:%d,propVal:%d",pItem->getID(),bpPropVal.m_val3,bpPropVal.m_val4);								
#endif
								}	
							}
						}
						else if (bpPropVal.m_val1 == ActivePropType_Refine){//精炼属性
							if (pEquip->m_itemInfo.m_refineLv >= bpPropVal.m_val2){//激活了
								if (bpPropVal.m_val3 < eMAX_BATTLE_PROP){
									pItem->m_battleProps[bpPropVal.m_val3] += bpPropVal.m_val4;
									needCalcBP = true;
#ifdef _HDZ_DEBUG
									//Athena::logger->trace("itemID:%d,精炼激活了:propID:%d,propVal:%d",pItem->getID(),bpPropVal.m_val3,bpPropVal.m_val4);								
#endif

								}	
							}
						}
					}
					else {
						bAddProp = false;
					}
				}

				if (needCalcBP) {
					pItem->calcBattlePower();
				}
			}
			
			if (bAddProp) {
				for (UINT16 k = 0;k < eMAX_BATTLE_PROP; ++k){
					m_data.m_battleProps[k] += pItem->m_battleProps[k];
				}
				m_data.m_battlePower += pItem->getBattlePower();
			}
		}
	}
	
	std::vector<UINT32> allFuwenIDs;
	//计算符文套装
	for (UINT16 i = HEAD_CORE_FUWEN_INDEX;i < RING_SOIL_FUWEN_INDEX;i += 5){
		allFuwenIDs.clear();
		for (UINT16 j = i;j < i+ 5; ++j){
			CItem * pItem =  m_pSlot[j];
			if (pItem){
				allFuwenIDs.push_back(pItem->getID());			
			}
		}
		
		for (UINT16 k = i;k < i + 3 ;++k){
			CItem * pItem = m_pSlot[k];
			bool bActive = false;
			if (pItem){
				for (UINT16 j = 1;j <= 3; ++j){
					stSoulinkBase * pSoulinkBase = g_soulinkBaseMgr.get(stSoulinkBase::getFindID(pItem->m_pItemBase->m_grownID,j));	
					if (pSoulinkBase){
						std::set<UINT32> & needEquipID = pSoulinkBase->getNeedEquiID(Role::eJOB_WARRIOR,Role::eSEX_MAN);	
						UINT8 equipNum = 0;
						for (UINT16 l = 0;l < allFuwenIDs.size();++l){
							if (needEquipID.find(allFuwenIDs[l]) != needEquipID.end()){
								equipNum += 1;
							}	
						}

						if (equipNum >= pSoulinkBase->m_needNum){
							if (pSoulinkBase->m_prop.first < m_data.m_battleProps.size()){
								m_data.m_battleProps[pSoulinkBase->m_prop.first] += pSoulinkBase->m_prop.second;	
							}
							
							if (pSoulinkBase->m_prop1.first < m_data.m_battleProps.size()){
								m_data.m_battleProps[pSoulinkBase->m_prop1.first] += pSoulinkBase->m_prop1.second;	
							}

#ifdef _HDZ_DEBUG
							Athena::logger->trace("符文(name:%s,id:%d)套装(%d)增加了:prop:%d,num:%d",pItem->getName(),pItem->getID(),j,pSoulinkBase->m_prop.first, pSoulinkBase->m_prop.second);
#endif
							bActive = true;
						}
					}
				}
			}

			if (bActive){//只要一个激活就要跳出了
				break;
			}
		}
	}

	UINT16 unLockFunwenNum = CMiscCfgMgr::getSingleton().getUnlockedFuwenEquipNum(getOwnerLv());
	if (unLockFunwenNum > 0 ){
		m_data.m_fuwenAveRaiseLv = (UINT16)(totalFuwenRaiseLv * 1.0f / unLockFunwenNum + 0.5f);
		m_data.m_fuwenAveQuality = (UINT8)(totalFuwenQuality * 1.0f / unLockFunwenNum + 0.5f);
	}
#ifdef _HDZ_DEBUG
	/*for (UINT8 i = 0;i <= RING_SOIL_FUWEN_INDEX;++i){
		CItem * pItem = m_pSlot[i];
		if (pItem){
			Athena::logger->trace("计算出来的装备战力:id=%d,name=%s,battlePower=%d",pItem->getID(),pItem->getName(),pItem->m_battlePower);	
		}
	}*/
#endif

	//技能强化属性
	for (UINT8 i = HEAD_SLOT_INDEX;i <= WING_SLOT_INDEX;++i){
		CItem * pItem = m_pSlot[i];
		if (pItem){
			for (UINT16 i = 0;i < pItem->m_raiseSkillBPVec.size();++i){
				stBattlePropValue & prop = pItem->m_raiseSkillBPVec[i];
				m_data.m_raiseSkillBPVec.push_back(prop);
			}
		}
	}

	for (UINT8 i = MOUNT_BAG_POS1_INDEX;i <= MOUNT_BAG_POS8_INDEX;++i){
		CItem * pItem = m_pSlot[i];
		if (pItem){
			if (CItem::isMountStone(pItem->getID())){
				pItem->setupPropState();
				pItem->calcBattlePower();

				for (UINT16 k = 1; k < eMAX_BATTLE_PROP; ++k){
					m_data.m_battleProps[k] += pItem->m_battleProps[k];
				}
				m_data.m_battlePower += pItem->getBattlePower();
			}
		}
	}
}

UINT16 CEquipBag::getWearFullFuwenEquip()
{
	UINT16 num = 0;
	for (UINT8 i = HEAD_SLOT_INDEX; i < WING_SLOT_INDEX; ++i){
		UINT8 index1 = i * 5 + HEAD_CORE_FUWEN_INDEX;
		UINT8 index2 = index1 + 1;
		UINT8 index3 = index2 + 1;
		UINT8 index4 = index3 + 1;
		UINT8 index5 = index4 + 1;

		if ((index1 < s_EQUIP_BAG_SLOT_NUM)
				||(index2 < s_EQUIP_BAG_SLOT_NUM)
				||(index3 < s_EQUIP_BAG_SLOT_NUM)
				||(index4 < s_EQUIP_BAG_SLOT_NUM)
				||(index5 < s_EQUIP_BAG_SLOT_NUM)){
			if (m_pSlot[index1] && m_pSlot[index2] && m_pSlot[index3] && m_pSlot[index4] && m_pSlot[index5]){
				num += 1;
			}	
		}
	}	
	return num;
}

UINT16 CEquipBag::getTotalRefineLv(UINT16 lv)
{
	UINT16 ret = 0;
	UINT8 i = HEAD_SLOT_INDEX;
	for (;i <= WING_SLOT_INDEX;++i){
		CItem * pItem = m_pSlot[i];
		if (pItem && (pItem->m_itemInfo.m_refineLv >= lv)){
			ret += 1;
		}
	}
	return ret;
}

UINT16 CEquipBag::getTotalRaiseLv(UINT16 lv)
{
	UINT16 ret = 0;
	UINT8 i = HEAD_SLOT_INDEX;
	for (;i <= WING_SLOT_INDEX;++i){
		CItem * pItem = m_pSlot[i];
		if (pItem && (pItem->m_itemInfo.m_raiseLv >= lv)){
			ret += 1;
		}
	}
	return ret;
}

UINT16 CEquipBag::purpleFuwenNum()
{
	UINT16 ret = 0;
	UINT8 i = HEAD_CORE_FUWEN_INDEX;
	for (;i <= RING_SOIL_FUWEN_INDEX;++i){
		CItem * pItem = m_pSlot[i];
		if (pItem && pItem->m_pItemBase &&(pItem->m_pItemBase->m_quality >= ITEM_CLASSTYPE_PURPLE_MIN_VAL)){
			ret += 1;	
		}
	}
	return ret;
}

CRoleEquipBag::CRoleEquipBag(GameUser *pOwner) : CEquipBag(Role::BAG_TYPE_EQUIP),m_pOwner(pOwner)
{}

CRoleEquipBag::~CRoleEquipBag()
{

}
	
UINT8 CRoleEquipBag::getOwnerJob() 
{
	return m_pOwner->getJob() ;
}

UINT8 CRoleEquipBag::getOwnerSex() 
{
	return m_pOwner->getSex();
}

UINT16 CRoleEquipBag::getOwnerLv()
{
	return m_pOwner->getLevel();
}

CItem * CRoleEquipBag::getWingItem() {
	for (UINT16 i = 0;i < m_bagSize; ++i){
		if (m_pSlot[i] && m_pSlot[i]->m_itemInfo.m_pos.m_pos == 15) {
            return m_pSlot[i];
        }
    }
    return NULL;
}

bool CRoleEquipBag::checkAddItem(CItem * pItem)
{
	if (!pItem){
		return false;
	}
	return CEquipBag::checkAddItem(pItem);
}

CBagMgr::CBagMgr(GameUser * pOwner) : m_pOwner(pOwner),m_mainBag(Role::BAG_TYPE_MAIN1,CMainBag::s_MAIN_BAG_SLOT_NUM),m_pieceBag(Role::BAG_TYPE_PIECE,CMainBag::s_PIECE_BAG_SLOT_NUM),m_heroPieceBag(Role::BAG_TYPE_HERO_PIECE,CMainBag::s_HERO_PIECE_BAG_SLOT_NUM),m_equipBag(pOwner),m_fashionBag(Role::BAG_TYPE_FASHION,CMainBag::s_FASHION_BAG_SLOT_NUM), m_runeBag(Role::BAG_TYPE_RUNE, CRuneBag::s_RUNE_BAG_SLOT_NUM), m_heroRuneBag(Role::BAG_TYPE_HERO_RUNE, CRuneBag::s_HERO_RUNE_BAG_SLOT_NUM), m_runePieceBag(Role::BAG_TYPE_RUNE_PIECE, CMainBag::s_RUNE_PIECE_BAG_SLOT_NUM), m_mountBag(Role::BAG_TYPE_MOUNT, CMainBag::s_MOUNT_BAG_SLOT_NUM)
{
	m_lastArrangeMainBagTime = 0;
}

CBagMgr::~CBagMgr()
{
}

UINT32 CBagMgr::saveAllItem(byte * buf,UINT32 needSpace)
{
	stItemSaveInfo * pSaveInfo = (stItemSaveInfo *)buf;
	pSaveInfo->m_version = s_SAVE_BIN_VERSION_SECOND;
	pSaveInfo->m_mainBagSize = getBagSize(Role::BAG_TYPE_MAIN1);
	pSaveInfo->m_fashionBagSize = getBagSize(Role::BAG_TYPE_FASHION);
	pSaveInfo->m_num = 0;
	UINT32 offset = sizeof(stItemSaveInfo);
	if (!m_mainBag.m_pSlot || !m_equipBag.m_pSlot){
		return offset;
	}
		
	//先保存主包裹里面的数据
	for (UINT16 i = 0;i < CMainBag::s_MAIN_BAG_SLOT_NUM ;++i){
		CItem * pItem = m_mainBag.m_pSlot[i];
		if (pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo));	
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}			
	
	//先保存碎片包裹里面的数据
	for (UINT16 i = 0;i < CMainBag::s_PIECE_BAG_SLOT_NUM ;++i){
		CItem * pItem = m_pieceBag.m_pSlot[i];
		if (pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo));	
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}	
	
	//先保存武将碎片包裹里面的数据
	for (UINT16 i = 0;i < CMainBag::s_HERO_PIECE_BAG_SLOT_NUM ;++i){
		CItem * pItem = m_heroPieceBag.m_pSlot[i];
		if (pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo));	
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}
	
	//先保时装包裹里面的数据
	for (UINT16 i = 0;i < CMainBag::s_FASHION_BAG_SLOT_NUM ;++i){
		CItem * pItem = m_fashionBag.m_pSlot[i];
		if (pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo));	
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}

	//再保存装备包裹里的数据
	for (UINT16 i = 0; i < CEquipBag::s_EQUIP_BAG_SLOT_NUM; ++i ){
		CItem * pItem = m_equipBag.m_pSlot[i];
		if (pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo));	
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}

	//保存符文包裹里面的数据
	for (UINT16 i = 0;i < CRuneBag::s_RUNE_BAG_SLOT_NUM;++i){
		CItem * pItem = m_runeBag.m_pSlot[i];
		if (pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo));	
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}

	//保存随从符文包裹里面的数据
	for (UINT16 i = 0;i < CRuneBag::s_HERO_RUNE_BAG_SLOT_NUM;++i){
		CItem * pItem = m_heroRuneBag.m_pSlot[i];
		if (pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo));	
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}

	//保存符文碎片包裹里面的数据
	for (UINT16 i = 0;i < CMainBag::s_RUNE_PIECE_BAG_SLOT_NUM;++i){
		CItem * pItem = m_runePieceBag.m_pSlot[i];
		if (pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo));	
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}

	//保存坐骑包裹里面的数据
	for(UINT16 i = 0; i < CMainBag::s_MOUNT_BAG_SLOT_NUM; ++i){
		CItem * pItem = m_mountBag.m_pSlot[i];
		if(pItem){
			bcopy(&pItem->m_itemInfo,&pSaveInfo->m_data[pSaveInfo->m_num],sizeof(ItemInfo)); 
			offset += sizeof(ItemInfo);
			pSaveInfo->m_num += 1;
		}
	}
	return offset;		
}

UINT32  CBagMgr::loadAllItem(byte * buf)
{
	UINT32 offset = sizeof(stItemSaveInfo);
	stItemSaveInfo * pSaveInfo = (stItemSaveInfo *)buf;
	setBagSize(Role::BAG_TYPE_MAIN1,pSaveInfo->m_mainBagSize);
	setBagSize(Role::BAG_TYPE_FASHION,pSaveInfo->m_fashionBagSize);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载包裹大小%d",pSaveInfo->m_mainBagSize);
#endif
    CItem * pwingItem = NULL;
	if (pSaveInfo->m_version == s_SAVE_BIN_VERSION ||
            pSaveInfo->m_version == s_SAVE_BIN_VERSION_SECOND){//是这个版本
		for (UINT16 i = 0;i < pSaveInfo->m_num; ++i){
			ItemInfo * pItemInfo = &pSaveInfo->m_data[i];
			
			
			CItem * pItem = ATHENA_NEW CItem();
			bcopy(pItemInfo,&pItem->m_itemInfo,sizeof(ItemInfo));
			offset += sizeof(ItemInfo);
			if (!pItem->init()){//初始化失败
				Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,初始化道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
				SAFE_DELETE(pItem);
				continue;
			}
			
			if ((pItemInfo->m_expireTime > 0) && (pItemInfo->m_expireTime < g_gameServer->getSec())){//过期了
#ifdef _HDZ_DEBUG
				Athena::logger->trace("玩家(account:%s,name:%s,charID:%d)的物品(instanceID:%llu,id:%d)",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
#endif
				SAFE_DELETE(pItem);
				continue;
			}

            if (pItem->m_itemInfo.m_pos.m_pos == 7 && Role::BAG_TYPE_EQUIP == pItemInfo->m_pos.m_bagType) {
                /*m_pOwner->calcVipLv();
                if (!m_pOwner->isValidVipItem(pItem->m_itemInfo.m_itemID)) {
                    pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_MAIN1;
                    pwingItem = pItem;
                    continue;
                }*/
            }
			if (Role::BAG_TYPE_MAIN1 == pItemInfo->m_pos.m_bagType){//主包裹
				if (!m_mainBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
			else if (Role::BAG_TYPE_PIECE == pItemInfo->m_pos.m_bagType){//包裹碎片
				if (!m_pieceBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
			else if (Role::BAG_TYPE_HERO_PIECE == pItemInfo->m_pos.m_bagType){//武将碎片
				if (!m_heroPieceBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
			else if (Role::BAG_TYPE_EQUIP == pItemInfo->m_pos.m_bagType){//装备包裹
				if (!m_equipBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
			else if (Role::BAG_TYPE_FASHION == pItemInfo->m_pos.m_bagType){//时装包裹
				if (!m_fashionBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
			else if (Role::BAG_TYPE_RUNE_PIECE == pItemInfo->m_pos.m_bagType){//符文碎片包裹
				if (!m_runePieceBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往符文碎片包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
			else if (Role::BAG_TYPE_RUNE == pItemInfo->m_pos.m_bagType){//符文包裹
				if (!m_runeBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往符文包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
			else if (Role::BAG_TYPE_HERO_RUNE == pItemInfo->m_pos.m_bagType){//随从符文包裹
				if (!m_heroRuneBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往符文包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
			else if(Role::BAG_TYPE_MOUNT == pItemInfo->m_pos.m_bagType){//坐骑包裹
				if(!m_mountBag.addItem(pItem)){
					Athena::logger->trace("account=%s,name=%s,charID=%d,上线加载道具时候,往坐骑包裹里放置道具thisID=%llu,itemID=%d时候不成功",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItemInfo->m_instanceID,pItemInfo->m_itemID);
					SAFE_DELETE(pItem);
					continue;
				}
			}
		}	
	}
	else {
		Athena::logger->error("加载道具数据错误,不存在的版本号");
	}
    if (pwingItem) {
        pwingItem->m_itemInfo.m_pos.m_pos = getBagFreeIndex(Role::BAG_TYPE_MAIN1);
        if (!m_mainBag.addItem(pwingItem)){
            Athena::logger->trace("上线加载道具时候,往包裹里放置道具不成功");
            SAFE_DELETE(pwingItem);
        }
    }
#ifdef _HDZ_DEBUG
	Athena::logger->trace("account=%s,name=%s,id=%d,上线加载了%d个道具,mainBagSize=%d",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pSaveInfo->m_num,pSaveInfo->m_mainBagSize);
#endif
	return offset;
}

void CBagMgr::synAllData()
{
	Role::stInitAllItem  synCmd;

	//主包裹里面的数据
	for (UINT16 i = 0;i < CMainBag::s_MAIN_BAG_SLOT_NUM ;++i){
		CItem * pItem = m_mainBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
		}
	}			
	
	//碎片包裹
	for (UINT16 i = 0;i < CMainBag::s_PIECE_BAG_SLOT_NUM ;++i){
		CItem * pItem = m_pieceBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
		}
	}

	//武将碎片包裹
	for (UINT16 i = 0;i < CMainBag::s_HERO_PIECE_BAG_SLOT_NUM ;++i){
		CItem * pItem = m_heroPieceBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
		}
	}
	
	//时装包裹
	for (UINT16 i = 0;i < CMainBag::s_FASHION_BAG_SLOT_NUM ;++i){
		CItem * pItem = m_fashionBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
            m_pOwner->synEnchant(pSynInfo);
		}
	}

	//装备包裹里的数据
	for (UINT16 i = 0; i < CEquipBag::s_EQUIP_BAG_SLOT_NUM; ++i ){
		CItem * pItem = m_equipBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
            m_pOwner->synEnchant(pSynInfo);
		}
	}

	//符文包裹里的数据
	for (UINT16 i = 0; i < CRuneBag::s_RUNE_BAG_SLOT_NUM; ++i ){
		CItem * pItem = m_runeBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
		}
    }

	//随从符文包裹里的数据
	for (UINT16 i = 0; i < CRuneBag::s_HERO_RUNE_BAG_SLOT_NUM; ++i ){
		CItem * pItem = m_heroRuneBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
		}
    }

	//符文碎片包裹里的数据
	for (UINT16 i = 0; i < CMainBag::s_RUNE_PIECE_BAG_SLOT_NUM; ++i ){
		CItem * pItem = m_runePieceBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
		}
    }

	//坐骑包裹里的数据
	for(UINT16 i = 0; i < CMainBag::s_MOUNT_BAG_SLOT_NUM; ++i){
		CItem * pItem = m_mountBag.m_pSlot[i];
		if (pItem){
			Role::stSynItemInfo  * pSynInfo = synCmd.add_data();
			pItem->fillSynData(*pSynInfo);
		}
	}
	m_pOwner->sendProto2Me(synCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("上线同步道具");
#endif
}

CItem * CBagMgr::createItem(UINT32 itemID,UINT32 num,UINT8 raiseLv, bool virtualBag)
{
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);
	if (!pItemBase){
		return NULL;
	}

	if (num > pItemBase->m_overlapNum){
		Athena::logger->error("创建道具itemID=%d失败,创建的数量超过了叠加数量",itemID);
		return NULL;
	}

	CItem * pItem = ATHENA_NEW CItem(pItemBase);
	
	if (!pItem){
		return NULL;
	}

	pItem->m_itemInfo.m_itemID = itemID;
	pItem->m_itemInfo.m_instanceID = CItem::genInstanceID();
	pItem->m_itemInfo.m_raiseLv = raiseLv;
	pItem->m_itemInfo.m_num = num;
    if (virtualBag) {
        if (CItem::isRuneByType(pItemBase->m_type)) {
            pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_HERO_RUNE;
            Athena::logger->trace("createItem BAG_TYPE_RUNE, virtualBag true");
        } else {
            Athena::logger->trace("createItem virtualBag true, invalid type");
            return NULL;
        }
    } else {
        if (pItemBase->m_type == ITEM_TYPE_WUHUNSHI){ //武魂石头
            pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_HERO_PIECE;
        }
        else if (CItem::isPiece(itemID)){
            pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_PIECE;
        }
        else if (CItem::isFashionByType(pItemBase->m_type)){
            pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_FASHION;
        }
        else if (CItem::isRuneByType(pItemBase->m_type)){
            pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_RUNE;
            Athena::logger->trace("createItem BAG_TYPE_RUNE, virtualBag false");
        }
        else if (CItem::isRunePieceByType(pItemBase->m_type)){
            pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_RUNE_PIECE;
            Athena::logger->trace("createItem BAG_TYPE_RUNE_PIECE, virtualBag false");
        }
		else if(CItem::isMountStoneByType(pItemBase->m_type)){
			pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_MOUNT;
			Athena::logger->trace("createItem BAG_TYPE_MOUNT, virtualBag false"); 
		}
        else {
            pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_MAIN1;
        }
    }

	if (!pItem->init()){
		Athena::logger->error("创建道具失败,初始化不成功!");
		SAFE_DELETE(pItem);
		return NULL;	
	}

	if (pItemBase->m_expireTime > 0){//有时效时间
		pItem->m_itemInfo.m_expireTime	= g_gameServer->getSec() + pItemBase->m_expireTime;  
	}
	return pItem;
}


bool  CBagMgr::addItem2Bag(CItem * & pItem)
{
	if (!pItem){
		return false;
	}
	
	bool ret = false;
	UINT8 bagType = pItem->m_itemInfo.m_pos.m_bagType; 
	if (Role::BAG_TYPE_MAIN1 == bagType){//主包裹
		CItem * pOverlayItem = m_mainBag.getOverlayItem(pItem);
		if (pOverlayItem){
			pOverlayItem->m_itemInfo.m_num += pItem->m_itemInfo.m_num;
			refreshItem(pOverlayItem);
			notifyDeleteItem(pItem);
			SAFE_DELETE(pItem);
			return true;
		}
		else {
			ret  = m_mainBag.addItem(pItem);
		}
	}
	else if (Role::BAG_TYPE_PIECE == bagType){//碎片包裹
		CItem * pOverlayItem = m_pieceBag.getOverlayItem(pItem);
		if (pOverlayItem){
			pOverlayItem->m_itemInfo.m_num += pItem->m_itemInfo.m_num;
			refreshItem(pOverlayItem);
			notifyDeleteItem(pItem);
			SAFE_DELETE(pItem);
			return true;
		}
		else {
			ret  = m_pieceBag.addItem(pItem);
		}
	}
	else if (Role::BAG_TYPE_HERO_PIECE   == bagType){
		CItem * pOverlayItem = m_heroPieceBag.getOverlayItem(pItem);
		if (pOverlayItem){
			pOverlayItem->m_itemInfo.m_num += pItem->m_itemInfo.m_num;
			refreshItem(pOverlayItem);
			notifyDeleteItem(pItem);
			SAFE_DELETE(pItem);
			return true;
		}
		else {
			ret  = m_heroPieceBag.addItem(pItem);
		}
	}
	else if (Role::BAG_TYPE_EQUIP == bagType){
		ret = m_equipBag.addItem(pItem);
	}
	else if (Role::BAG_TYPE_FASHION == bagType){
		ret = m_fashionBag.addItem(pItem);
		stCollectFashionNum notify;
        notify.notify(m_pOwner);
	}
	else if (Role::BAG_TYPE_RUNE_PIECE == bagType){
		CItem * pOverlayItem = m_runePieceBag.getOverlayItem(pItem);
		if (pOverlayItem){
			pOverlayItem->m_itemInfo.m_num += pItem->m_itemInfo.m_num;
			refreshItem(pOverlayItem);
			notifyDeleteItem(pItem);
			SAFE_DELETE(pItem);
			return true;
		}
		else {
			ret  = m_runePieceBag.addItem(pItem);
		}
	}
	else if (Role::BAG_TYPE_RUNE == bagType){
		ret = m_runeBag.addItem(pItem);
	}
	else if (Role::BAG_TYPE_HERO_RUNE == bagType){
		ret = m_heroRuneBag.addItem(pItem);
	}else if(Role::BAG_TYPE_MOUNT == bagType){
		ret = m_mountBag.addItem(pItem);
	}

	if (ret){
		notifyAddItem(pItem);
	}
	return ret;
}

UINT32 CBagMgr::getItemNumByID(UINT32 itemID,UINT16 raiseLv)
{
	std::vector<CItem *> vec;
	stGetItemCallBackByCond getCB(vec,itemID,raiseLv);
	m_mainBag.exeEveryOne(getCB);
	UINT32 ret = 0;
	for (UINT8 i = 0;i < vec.size();++i){
		ret += vec[i]->m_itemInfo.m_num;
	}
	return ret;
}

UINT32 CBagMgr::getPieceNumByID(UINT32 pieceID)
{
	std::vector<CItem *> vec;
	stGetItemCallBackByCond getCB(vec,pieceID,0);
    if (CItem::isPiece(pieceID)) {
        m_pieceBag.exeEveryOne(getCB);
    } else {
        m_heroPieceBag.exeEveryOne(getCB);
    }
	UINT32 ret = 0;
	for (UINT8 i = 0;i < vec.size();++i){
		ret += vec[i]->m_itemInfo.m_num;
	}
	return ret;
} 

UINT32 CBagMgr::getItemNumByID(UINT32 itemID)
{
	return getItemNumByID(itemID,0);
}

UINT32 CBagMgr::getMountNumByID(UINT32 itemID)
{
	std::vector<CItem *> vec;
	stGetItemCallBackByCond getCB(vec,itemID,0);
	m_mountBag.exeEveryOne(getCB);
	UINT32 ret = 0;
	for (UINT8 i = 0;i < vec.size();++i){
		ret += vec[i]->m_itemInfo.m_num;
	}
	return ret;
}

bool CBagMgr::addItem2Bag(UINT32 itemID,UINT32 num,UINT16 raiseLv,const char * desc)
{
	if (!desc){
		return false;
	}
	
	itemID = CItem::realItemIDByJobAndSex(itemID,m_pOwner->getJob(),m_pOwner->getSex()); //首先判断如果是装备,会根据职业跟性别替换吊ID

	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);
	if (!pItemBase || !pItemBase->m_overlapNum){
		Athena::logger->error("找不到该道具id:%d,的配置表",itemID);
		return false;
	}
	
	if ((pItemBase->m_type >= ITEM_TYPE_YUNTIE_PIECE)
		//	&&(pItemBase->m_type <= ITEM_TYPE_FUWEN_EXP_PIECE)){//抢夺碎片
		&&(pItemBase->m_type <= ITEM_TYPE_GOLDCOIN_CARD)){//抢夺碎片
		Global::stSynAddTreasure cmd;
		cmd.set_charid(m_pOwner->getID());
		cmd.set_resid(itemID);
		cmd.set_num(num);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		return true;
	}

	UINT32 itemNumMod = num % pItemBase->m_overlapNum;
	UINT32 itemNum = (num / pItemBase->m_overlapNum) + (itemNumMod ? 1 : 0); 
	UINT32 numPerItem = 0;
	if (num > pItemBase->m_overlapNum){
		numPerItem = pItemBase->m_overlapNum;
	}
	else {
		numPerItem = num;
	}
	
	for (UINT32 i = 0;i < itemNum; ++i){
		CItem * pItem = NULL;
		if ((itemNumMod > 0) && (i == (itemNum - 1))){
			pItem = createItem(itemID,itemNumMod,raiseLv);	
		}		
		else {
			pItem = createItem(itemID,numPerItem,raiseLv);
		}
		if (!pItem){
			Athena::logger->error("因为%s给玩家account=%s,name=%s,id=%d道具时候创建道具不成功!",desc,m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
			return false;
		}
		BagPos dstPos;
		dstPos.m_bagType = pItem->m_itemInfo.m_pos.m_bagType;
		dstPos.m_pos = getBagFreeIndex(dstPos.m_bagType);
		if ((UINT8)-1 == dstPos.m_pos){
			Athena::logger->error("因为%s给玩家account=%s,name=%s,id=%d时候,包裹位置满了!",desc,m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
			SAFE_DELETE(pItem);
			return false;
		}
		pItem->m_itemInfo.m_pos = dstPos;
		addItem2Bag(pItem);
	}
	Athena::logger->trace("account=%s,name=%s,id=%d,因为%s给玩家%d个道具itemID=%d num=%d",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),desc,itemNum,itemID,num);
	
	if (CItem::needSendWebLog(pItemBase)){//紫装以上才记录
		Game::stItemChgLog webCmd;
		webCmd.set_account(m_pOwner->getAccount());
		webCmd.set_name(m_pOwner->getName());
		webCmd.set_charid(m_pOwner->getID());
		webCmd.set_itemid(itemID);
		webCmd.set_num(num);
		webCmd.set_reason(desc);
		webCmd.set_isadd(true);
		webCmd.set_userlv(m_pOwner->getLevel());
		webCmd.set_viplv(m_pOwner->getVipLv());
		webCmd.set_raiselv(0);
		webCmd.set_refinelv(0);
		g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	}
	return true;
}

bool  CBagMgr::removeItemByItemID(UINT32 itemID,UINT16 raiseLv,UINT32 num,const char * desc)
{
	std::vector<CItem *> vec;
	stGetItemCallBackByCond getCB(vec,itemID,raiseLv);
	CMainBag * pBag = NULL;
	if (CItem::isWuHunShi(itemID)){ //武魂石头
		pBag = &m_heroPieceBag;
	}
	else if (CItem::isPiece(itemID)){//碎片
		pBag = &m_pieceBag;
	}
	else if (CItem::isFashion(itemID)){//时装包裹
		pBag = &m_fashionBag;	
	}
	else if (CItem::isRunePiece(itemID)){//符文碎片包裹
		pBag = &m_runePieceBag;	
	}
	else if(CItem::isMountStone(itemID)){//坐骑包裹
		pBag = &m_mountBag;
	}
	else {//主包裹
		pBag = &m_mainBag;
	}
	
	if (!pBag){
		return false;
	}
	pBag->exeEveryOne(getCB);

	UINT32 hasNum = 0;
	for (UINT8 i = 0;i < vec.size();++i){
		hasNum += vec[i]->m_itemInfo.m_num;
	}

	if (hasNum < num){
		return false;
	}
	
	UINT32 decNum = num;
	for (UINT16 i = 0;i < vec.size(); ++i){
		CItem * pItem = vec[i];
		if (pItem->m_itemInfo.m_num <= decNum){
			decNum -= pItem->m_itemInfo.m_num;
			if (pBag->removeItem(pItem)){
				notifyDeleteItem(pItem);
				SAFE_DELETE(pItem);
			}
		}
		else {
			pItem->m_itemInfo.m_num -= decNum;
			decNum = 0;
			refreshItem(pItem);
		}

		if (0 == decNum){
			break;
		}
	}
	Athena::logger->trace("因为%s玩家account=%s,name=%s,id=%d,扣除了itemID=%d,num=%d",desc,m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),itemID,num);
	stItemBase * pItemBase = g_itemBaseMgr.get(itemID);
	if (!pItemBase){
		Athena::logger->error("找不到该道具id:%d,的配置表",itemID);
		return false;
	}
	
	if (CItem::needSendWebLog(pItemBase)){//紫装以上才记录
		Game::stItemChgLog webCmd;
		webCmd.set_account(m_pOwner->getAccount());
		webCmd.set_name(m_pOwner->getName());
		webCmd.set_charid(m_pOwner->getID());
		webCmd.set_itemid(itemID);
		webCmd.set_num(num);
		webCmd.set_reason(desc);
		webCmd.set_raiselv(raiseLv);
		webCmd.set_refinelv(0);
		webCmd.set_isadd(false);
		webCmd.set_userlv(m_pOwner->getLevel());
		webCmd.set_viplv(m_pOwner->getVipLv());
		g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	}
	return true;
}

bool  CBagMgr::removeItemByItemID(UINT32 itemID,UINT32 num,const char * desc)
{
	return removeItemByItemID(itemID,0,num,desc);
}

bool  CBagMgr::decItem(CItem *pItem,UINT32 num,const char * desc)
{
	BagPos & pos = pItem->m_itemInfo.m_pos;
	if (pos.m_bagType != Role::BAG_TYPE_MAIN1){//不能扣除主包裹外的道具
		return false;
	}

	if (pItem->m_itemInfo.m_num < num){
		return false;
	}
	else {
		pItem->m_itemInfo.m_num -= num;
	}

	if (pItem->m_itemInfo.m_num){//还有道具
		Athena::logger->trace("account=%s,name=%s,id=%d,因为%s扣除了道具instanceID=%llu,itemID=%d,num=%d",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),desc,pItem->m_itemInfo.m_instanceID,pItem->getID(),num);
		refreshItem(pItem);	
	}
	else {
		if (m_mainBag.removeItem(pItem)){
			notifyDeleteItem(pItem);
			Athena::logger->trace("account=%s,name=%s,id=%d,因为%s扣除了道具instanceID=%llu,itemID=%d,num=%d",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),desc,pItem->m_itemInfo.m_instanceID,pItem->getID(),num);
			SAFE_DELETE(pItem);
		}
		else {
			Athena::logger->trace("account=%s,name=%s,id=%d,扣除道具时候,道具已经为0了但是删除道具时候没有找到道具",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		}
	}
	return true;
}

CItem * CBagMgr::getItemByInstaceID(UINT8 bagType,UINT64 instanceID)
{
	if (Role::BAG_TYPE_EQUIP == bagType){
		 return m_equipBag.getItemByInstaceID(instanceID);
	}	
	else if (Role::BAG_TYPE_MAIN1 == bagType) {
		return m_mainBag.getItemByInstaceID(instanceID);
	}
	else if (Role::BAG_TYPE_PIECE == bagType){
		return m_pieceBag.getItemByInstaceID(instanceID); 
	}
	else if (Role::BAG_TYPE_HERO_PIECE == bagType){
		return m_heroPieceBag.getItemByInstaceID(instanceID); 
	}
	else if (Role::BAG_TYPE_FASHION == bagType){
		return m_fashionBag.getItemByInstaceID(instanceID); 
	}
	else if (Role::BAG_TYPE_RUNE == bagType){
		return m_runeBag.getItemByInstaceID(instanceID); 
	}
	else if (Role::BAG_TYPE_HERO_RUNE == bagType){
		return m_heroRuneBag.getItemByInstaceID(instanceID); 
	}
	else if (Role::BAG_TYPE_RUNE_PIECE == bagType){
		return m_runePieceBag.getItemByInstaceID(instanceID); 
	}
	else if(Role::BAG_TYPE_MOUNT == bagType){
		return m_mountBag.getItemByInstaceID(instanceID);
	}

	Athena::logger->trace("道具id=%d,type=%d有问题",instanceID, bagType);

	return NULL;
}

UINT16 CBagMgr::getBagFreeIndex(UINT8 bagType)
{
	if (Role::BAG_TYPE_EQUIP == bagType){
		return m_equipBag.getFreeSlotIndex();
	}	
	else if (Role::BAG_TYPE_MAIN1 == bagType) {
		return m_mainBag.getFreeSlotIndex(); 
	}
	else if (Role::BAG_TYPE_PIECE == bagType){
		return m_pieceBag.getFreeSlotIndex(); 
	}
	else if (Role::BAG_TYPE_HERO_PIECE == bagType){
		return m_heroPieceBag.getFreeSlotIndex();
	}
	else if (Role::BAG_TYPE_FASHION == bagType){
		return m_fashionBag.getFreeSlotIndex();
	}
	else if (Role::BAG_TYPE_RUNE_PIECE == bagType){
		return m_runePieceBag.getFreeSlotIndex();
	}
	else if (Role::BAG_TYPE_RUNE == bagType){
		return m_runeBag.getFreeSlotIndex();
	}
	else if (Role::BAG_TYPE_HERO_RUNE == bagType){
		return m_heroRuneBag.getFreeSlotIndex();
	}
	else if(Role::BAG_TYPE_MOUNT == bagType){
		return m_mountBag.getFreeSlotIndex();
	}
	return (UINT16)-1;
}

void CBagMgr::setBagSize(UINT8 bagType,UINT16 size)
{
	if (Role::BAG_TYPE_EQUIP == bagType){
		m_equipBag.setBagSize(size);
	}	
	else if (Role::BAG_TYPE_MAIN1 == bagType) {
		m_mainBag.setBagSize(size); 
	}
	else if (Role::BAG_TYPE_PIECE == bagType){
		m_pieceBag.setBagSize(size);
	}
	else if (Role::BAG_TYPE_HERO_PIECE == bagType){
		m_heroPieceBag.setBagSize(size);
	}
	else if (Role::BAG_TYPE_FASHION == bagType){
		m_fashionBag.setBagSize(size);	
	}
	else if (Role::BAG_TYPE_RUNE_PIECE == bagType){
		m_runePieceBag.setBagSize(size);	
	}
	else if (Role::BAG_TYPE_RUNE == bagType){
		m_runeBag.setBagSize(size);	
	}
	else if (Role::BAG_TYPE_HERO_RUNE == bagType){
		m_heroRuneBag.setBagSize(size);	
	}
	else if(Role::BAG_TYPE_MOUNT == bagType){
		m_mountBag.setBagSize(size);
	}
}

UINT16 CBagMgr::getBagSize(UINT8 bagType)
{
	if (Role::BAG_TYPE_EQUIP == bagType){
		return m_equipBag.getBagSize();
	}	
	else if (Role::BAG_TYPE_MAIN1 == bagType) {
		return m_mainBag.getBagSize(); 
	}
	else if (Role::BAG_TYPE_PIECE == bagType){
		return m_pieceBag.getBagSize();	
	}
	else if (Role::BAG_TYPE_HERO_PIECE == bagType){
		return m_heroPieceBag.getBagSize();
	}
	else if (Role::BAG_TYPE_FASHION == bagType){
		return m_fashionBag.getBagSize();
	}
	else if(Role::BAG_TYPE_MOUNT == bagType){
		return m_mountBag.getBagSize();
	}
	return 0;
}

UINT16 CBagMgr::getFreeSlotNum(UINT8 bagType)
{
	if (Role::BAG_TYPE_EQUIP == bagType){
		return m_equipBag.getFreeSlotNum();
	}	
	else if (Role::BAG_TYPE_MAIN1 == bagType) {
		return m_mainBag.getFreeSlotNum(); 
	}
	else if (Role::BAG_TYPE_PIECE == bagType){
		return m_pieceBag.getFreeSlotNum();
	}
	else if (Role::BAG_TYPE_HERO_PIECE == bagType){
		return m_heroPieceBag.getFreeSlotNum();
	}
	else if (Role::BAG_TYPE_FASHION == bagType){
		return m_fashionBag.getFreeSlotNum();
	}
	else if (Role::BAG_TYPE_RUNE == bagType){
		return m_runeBag.getFreeSlotNum();
	}
	else if (Role::BAG_TYPE_HERO_RUNE == bagType){
		return m_heroRuneBag.getFreeSlotNum();
	}
	else if (Role::BAG_TYPE_RUNE_PIECE == bagType){
		return m_runePieceBag.getFreeSlotNum();
	}
	else if(Role::BAG_TYPE_MOUNT == bagType){
		return m_mountBag.getFreeSlotNum();
	}
	return 0;
}

bool CBagMgr::checkHasFreeSlot(std::vector<std::pair<UINT32,UINT32> > & itemID2Num)
{
	UINT16 mainPackNeed = 0;
	UINT16 equipPieceNeed = 0;
	UINT16 heroPieceNedd = 0;
	UINT16 fashPackNeed = 0;
	UINT16 mountPackNeed = 0;

	for (UINT16 i = 0;i < itemID2Num.size(); ++i){
		if (CItem::isPiece(itemID2Num[i].first)){
			equipPieceNeed += itemID2Num[i].second;
		}
		else if (CItem::isWuHunShi(itemID2Num[i].first)){
			heroPieceNedd += itemID2Num[i].second;
		}
		else if (CItem::isFashion(itemID2Num[i].first)){
			fashPackNeed += itemID2Num[i].second;
		}
		else if(CItem::isMountStone(itemID2Num[i].first)){
			mountPackNeed += itemID2Num[i].second;
		}
		else {
			mainPackNeed += itemID2Num[i].second;
		}
	}

	if (getFreeSlotNum(Role::BAG_TYPE_MAIN1) < mainPackNeed){
		return false;
	}

	if (getFreeSlotNum(Role::BAG_TYPE_PIECE) < equipPieceNeed){
		return false;
	}

	if (getFreeSlotNum(Role::BAG_TYPE_HERO_PIECE) < heroPieceNedd){
		return false;
	}

	if (getFreeSlotNum(Role::BAG_TYPE_FASHION) < fashPackNeed){
	
	}

	if(getFreeSlotNum(Role::BAG_TYPE_MOUNT) < mountPackNeed){
		return false;
	}

	return true;
}

UINT16 CBagMgr::getMainFreeSlotNum()
{
	return getFreeSlotNum(Role::BAG_TYPE_MAIN1);	
}


void CBagMgr::sellItem(CItem * pItem)
{
	if (!pItem){
		return ;
	}
	
	UINT8 bagType = pItem->m_itemInfo.m_pos.m_bagType;
	bool ret = false;
	if (Role::BAG_TYPE_EQUIP == bagType){
		ret = m_equipBag.removeItem(pItem);
	}
	else if (Role::BAG_TYPE_MAIN1 == bagType) {
		ret = m_mainBag.removeItem(pItem);
	}
	else if (Role::BAG_TYPE_PIECE == bagType){
		ret = m_pieceBag.removeItem(pItem); 
	}
	else if (Role::BAG_TYPE_HERO_PIECE == bagType){
		ret = m_heroPieceBag.removeItem(pItem);  
	}
	else if (Role::BAG_TYPE_FASHION == bagType){
		ret = m_fashionBag.removeItem(pItem); 
	}
	else if (Role::BAG_TYPE_RUNE == bagType){
		ret = m_runeBag.removeItem(pItem); 
	}
	else if (Role::BAG_TYPE_HERO_RUNE == bagType){
		ret = m_heroRuneBag.removeItem(pItem); 
	}
	else if (Role::BAG_TYPE_RUNE_PIECE == bagType){
		ret = m_runePieceBag.removeItem(pItem); 
	}
	else if(Role::BAG_TYPE_MOUNT == bagType){
		ret = m_mountBag.removeItem(pItem);
	}

	if (ret){//从包裹中删除成功
		notifyDeleteItem(pItem);
		if (pItem->m_pItemBase){//给玩家价钱
			m_pOwner->addResource(eResource_GoldCoin,pItem->m_pItemBase->m_sellPrice * pItem->m_itemInfo.m_num,"丢弃物品获得钱!");
		}
		SAFE_DELETE(pItem);
        if (Role::BAG_TYPE_FASHION == bagType){
            stCollectFashionNum notify;
            notify.notify(m_pOwner);
        }
	}
	else {
		Athena::logger->error("account=%s,name=%s,charID=%d,丢弃道具时候从包裹里删除道具失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());	
	}
}

void CBagMgr::notifyAddItem(CItem * pItem)
{
	if (!pItem){
		return ;
	}
	Role::stNotifyAddItem addItemCmd;
	Role::stSynItemInfo  *pSynItemInfo = addItemCmd.mutable_data();
	pItem->fillSynData(*pSynItemInfo);
    m_pOwner->synEnchant(pSynItemInfo);
	m_pOwner->sendProto2Me(addItemCmd);
}

void CBagMgr::notifyDeleteItem(CItem * pItem)
{
	if (!pItem){
		return ;
	}
	Role::stDeleteItem cmd;
	cmd.set_instanceid(pItem->m_itemInfo.m_instanceID);
	cmd.mutable_bagpos()->set_bagtype(pItem->m_itemInfo.m_pos.m_bagType);
	cmd.mutable_bagpos()->set_pos(pItem->m_itemInfo.m_pos.m_pos);
	m_pOwner->sendProto2Me(cmd);
}

void CBagMgr::refreshItem(CItem * pItem)
{
	if (!pItem){
		return ;
	}

	Role::stRefreshItem cmd;
	Role::stSynItemInfo  *pSynItemInfo = cmd.mutable_data();
	pItem->fillSynData(*pSynItemInfo);
    m_pOwner->synEnchant(pSynItemInfo);
	m_pOwner->sendProto2Me(cmd);
}

bool CBagMgr::moveItem(CItem * pItem,const BagPos & dstPos)
{
	if (!pItem){
		return false;
	}
	
	BagPos  srcPos = pItem->m_itemInfo.m_pos;
	CBag * pSrcBag = NULL;
	CBag * pDstBag = NULL; 
	
	if (Role::BAG_TYPE_EQUIP == srcPos.m_bagType){
		pSrcBag = &m_equipBag;
	}
	else if (Role::BAG_TYPE_MAIN1 == srcPos.m_bagType) {
		pSrcBag =  &m_mainBag;
	}
	else if (Role::BAG_TYPE_PIECE == srcPos.m_bagType){
		pSrcBag = &m_pieceBag;
	}
	else if (Role::BAG_TYPE_FASHION == srcPos.m_bagType){
		pSrcBag = &m_fashionBag;	
	}
	else if(Role::BAG_TYPE_MOUNT == srcPos.m_bagType){
		pSrcBag = &m_mountBag;
	}
	
	if (Role::BAG_TYPE_EQUIP == dstPos.m_bagType){
		pDstBag = &m_equipBag;
	}
	else if (Role::BAG_TYPE_MAIN1 == dstPos.m_bagType) {
		pDstBag =  &m_mainBag;
	}
	else if (Role::BAG_TYPE_PIECE == dstPos.m_bagType){
		pDstBag = &m_pieceBag;
	}
	else if (Role::BAG_TYPE_FASHION == dstPos.m_bagType){
		pDstBag = & m_fashionBag;	
	}
	else if(Role::BAG_TYPE_MOUNT == dstPos.m_bagType){
		pDstBag = &m_mountBag;
	}
	
	if (!pSrcBag || !pDstBag){
		Athena::logger->error("(account=%s,name=%s,charID=%d)移动包裹错误,源包裹或目的包裹不存在",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		return false;
	}
	
	if (!pDstBag->checkSlotIndex(dstPos.m_pos)){
		Athena::logger->error("(account=%s,name=%s,charID=%d)移动包裹错误,客户端传进来的目的包裹地址不正确",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		return false;
	}

	CItem * pDstItem = pDstBag->getItemBySlotIndex(dstPos.m_pos);

	if (!pDstItem){//目的格子不存在道具
		if (pSrcBag->removeItem(pItem)){//删除道具
			pItem->m_itemInfo.m_pos = dstPos;
			if (pDstBag->addItem(pItem)){//添加道具
				return true;		
			}
			else {//往新的包裹添加道具失败
				pItem->m_itemInfo.m_pos = srcPos;
				Athena::logger->error("(account=%s,name=%s,charID=%d)移动包裹错误,往新包裹添加道具失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
				if (!pSrcBag->addItem(pItem)){
					Athena::logger->error("(account=%s,name=%s,charID=%d)移动包裹错误,往源包裹再添加道具(%d)失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItem->getID());
				}
			}
		}
		else {
			Athena::logger->error("(account=%s,name=%s,charID=%d)移动包裹错误,源包裹中删除道具失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		}
	}
	else {//目的格子已经存在道具
		if (pSrcBag->removeItem(pItem) && pDstBag->removeItem(pDstItem)){
			pItem->m_itemInfo.m_pos = dstPos;
			pDstItem->m_itemInfo.m_pos = srcPos;
			if (pSrcBag->addItem(pDstItem) && pDstBag->addItem(pItem)){
				/*Role::stMoveItem moveCmd;
				moveCmd.set_bagtype(dstPos.m_bagType);
				moveCmd.set_instanceid(pDstItem->m_itemInfo.m_instanceID);
				moveCmd.mutable_bagpos()->set_bagtype(srcPos.m_bagType);
				moveCmd.mutable_bagpos()->set_pos(srcPos.m_pos);
				m_pOwner->sendProto2Me(moveCmd);*/
				return true;	
			}
			else {
				pSrcBag->removeItem(pDstItem);
				pDstBag->removeItem(pItem);
				pItem->m_itemInfo.m_pos = srcPos;
				pDstItem->m_itemInfo.m_pos = dstPos;
				pSrcBag->addItem(pItem);
				pDstBag->addItem(pDstItem);
				Athena::logger->error("(account=%s,name=%s,charID=%d)移动包裹错误,(srcItem=%d,dstItem=%d)从源包裹和目的包裹中分别添加道具失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItem->getID(),pDstItem->getID());
			}
		}
		else {
			pSrcBag->addItem(pItem);
			pDstBag->addItem(pDstItem);
			Athena::logger->error("(account=%s,name=%s,charID=%d)移动包裹错误,(srcItem=%d,dstItem=%d)从源包裹和目的包裹中分别删除道具失败!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),pItem->getID(),pDstItem->getID());
		}
	}

	return false;
}


UINT32 CBagMgr::getItemByID(UINT32 itemID,UINT64 exceptInstanceID,UINT8 bagType,std::vector<CItem * > & vec)
{
	if (Role::BAG_TYPE_MAIN1 == bagType){
		return m_mainBag.getItemByID(itemID,exceptInstanceID,vec);	
	}	
	return 0;
}

void CBagMgr::getItemByCond(stItemCallBack & cb,UINT8 bagType)
{
	if (Role::BAG_TYPE_MAIN1 == bagType){
		m_mainBag.exeEveryOne(cb);
	}
}

void CBagMgr::sendEquipChgLog(UINT32 itemID,UINT16 oldRaiseLv,UINT16 newRaiseLv,UINT16 oldRefineLv,UINT16 newRefineLv,const char * szAction,UINT32 decGoldcoin,UINT32 decYuntie,UINT32 decItemID,UINT32 addGoldcoin,UINT32 addYuntie,UINT32 addItemID)
{
	if (!szAction){
		return ;
	}
	Game::stEquipChgLog webCmd;
	webCmd.set_action(szAction);
	webCmd.set_addgoldcoin(addGoldcoin);
	webCmd.set_additemid(addItemID);
	webCmd.set_addyuntie(addYuntie);
	webCmd.set_decgoldcoin(decGoldcoin);
	webCmd.set_decitemid(decItemID);
	webCmd.set_decyuntie(decYuntie);
	webCmd.set_itemid(itemID);
	webCmd.set_newraiselv(newRaiseLv);
	webCmd.set_oldraiselv(oldRaiseLv);
	webCmd.set_oldrefinelv(oldRefineLv);
	webCmd.set_newrefinelv(newRefineLv);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

bool CBagMgr::moveItemForRune(GameUser * pUser, CItem * pItem, UINT16 & slotIndex) {
	if (!pItem) {
		return false;
	}
    UINT64 instanceId = pItem->m_itemInfo.m_instanceID;
    UINT8 srcBagType = pItem->m_itemInfo.m_pos.m_bagType;
    UINT8 dstBagType = 0;
    if (Role::BAG_TYPE_RUNE == pItem->m_itemInfo.m_pos.m_bagType) {
        //符文背包移动到随从符文背包
        UINT16 index = m_heroRuneBag.getFreeSlotIndex();
        if (index == (UINT16)-1) {
            Athena::logger->error("[符文背包移动到随从符文背包]失败, 无空位");
            return false;
        }
        if (!m_runeBag.removeItem(pItem)) {
            Athena::logger->error("[符文背包移动到随从符文背包]失败, 移除失败");
            return false;
        }
        //notifyDeleteItem(pItem);
        pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_HERO_RUNE;
        pItem->m_itemInfo.m_pos.m_pos = index;
        slotIndex = index;
        if (!m_heroRuneBag.addItem(pItem)) {
            Athena::logger->error("[符文背包移动到随从符文背包]失败, 添加失败");
            return false;
        }
        dstBagType = Role::BAG_TYPE_HERO_RUNE;
    } else if (Role::BAG_TYPE_HERO_RUNE == pItem->m_itemInfo.m_pos.m_bagType) {
        //随从符文背包移动到符文背包
        UINT16 index = m_runeBag.getFreeSlotIndex();
        if (index == (UINT16)-1) {
            Athena::logger->error("[随从符文背包移动到符文背包]失败, 无空位");
            return false;
        }
        if (!m_heroRuneBag.removeItem(pItem)) {
            Athena::logger->error("[随从符文背包移动到符文背包]失败, 移除失败");
            return false;
        }
        //notifyDeleteItem(pItem);
        pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_RUNE;
        pItem->m_itemInfo.m_pos.m_pos = index;
        slotIndex = index;
        if (!m_runeBag.addItem(pItem)) {
            Athena::logger->error("[随从符文背包移动到符文背包]失败, 添加失败");
            return false;
        }
        dstBagType = Role::BAG_TYPE_RUNE;
    } else {
        Athena::logger->error("invalite bag type for moveItemForRune");
        return false;
    }
    Role::stMoveItem retCmd;
    retCmd.set_bagtype(srcBagType);
    retCmd.set_instanceid(instanceId);
    retCmd.mutable_bagpos()->set_bagtype(dstBagType);
    retCmd.mutable_bagpos()->set_pos(slotIndex );
    pUser->sendProto2Me(retCmd);
    return true;
}
/*
bool CBagMgr::moveItemForRune(CItem * pItem, UINT16 & slotIndex) {
	if (!pItem) {
		return false;
	}
    if (Role::BAG_TYPE_RUNE == pItem->m_itemInfo.m_pos.m_bagType) {
        //符文背包移动到随从符文背包
        UINT16 index = m_heroRuneBag.getFreeSlotIndex();
        if (index == (UINT16)-1) {
            Athena::logger->error("[符文背包移动到随从符文背包]失败, 无空位");
            return false;
        }
        CItem * pNewItem = m_heroRuneBag.copyItem(pItem, Role::BAG_TYPE_HERO_RUNE);
        if (!pNewItem) {
            Athena::logger->error("[符文背包移动到随从符文背包]失败, copy Item 失败");
            return false;
        }
        if (!pNewItem->init()) {
            Athena::logger->error("[符文背包移动到随从符文背包]失败, init Item 失败");
            return false;
        }
        if (!m_runeBag.removeItem(pItem)) {
            Athena::logger->error("[符文背包移动到随从符文背包]失败, 移除失败");
            return false;
        }
        notifyDeleteItem(pItem);
        pNewItem->m_itemInfo.m_pos.m_pos = index;
        slotIndex = index;
        if (!addItem2Bag(pNewItem)) {
            Athena::logger->error("[符文背包移动到随从符文背包]失败, 添加失败");
            return false;
        }
        Athena::logger->trace("[符文背包移动到随从符文背包]成功, pItem is %x", pItem);
    } else if (Role::BAG_TYPE_HERO_RUNE == pItem->m_itemInfo.m_pos.m_bagType) {
        //随从符文背包移动到符文背包
        UINT16 index = m_runeBag.getFreeSlotIndex();
        if (index == (UINT16)-1) {
            Athena::logger->error("[随从符文背包移动到符文背包]失败, 无空位");
            return false;
        }
        CItem * pNewItem = m_heroRuneBag.copyItem(pItem, Role::BAG_TYPE_RUNE);
        if (!pNewItem) {
            Athena::logger->error("[随从符文背包移动到符文背包]失败, copy Item 失败");
            return false;
        }
        if (!pNewItem->init()) {
            Athena::logger->error("[随从符文背包移动到符文背包]失败, init Item 失败");
            return false;
        }
        if (!m_heroRuneBag.removeItem(pItem)) {
            Athena::logger->error("[随从符文背包移动到符文背包]失败, 移除失败");
            return false;
        }
        notifyDeleteItem(pItem);
        pNewItem->m_itemInfo.m_pos.m_pos = index;
        slotIndex = index;
        if (!addItem2Bag(pNewItem)) {
            Athena::logger->error("[随从符文背包移动到符文背包]失败, 添加失败");
            return false;
        }
        Athena::logger->trace("[随从符文背包移动到符文背包]成功, pItem is %x", pItem);
    } else {
        Athena::logger->error("invalite bag type for moveItemForRune");
        return false;
    }
    return true;
}
*/

bool CBagMgr::removeRune(CItem * pItem) {
    if (!pItem) {
        return false;
    }
    if (!m_runeBag.removeItem(pItem)) {
        Athena::logger->error("[符文背包删除符文]失败");
        return false;
    }
    notifyDeleteItem(pItem);
    return true;
}

CRuneBag::CRuneBag(UINT8 type, UINT16 slotNum) : CBag(slotNum, type)
{
}

CRuneBag::~CRuneBag()
{
}

void CRuneBag::reArrange()
{
	std::map<UINT32,std::vector<CItem *> > sortMap[2];
	//先排序
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (m_pSlot[i]){
			CItem * pItem = m_pSlot[i];
			UINT32 sortID = pItem->m_pItemBase->m_sortID;
			ItemCombineBase * pCombineBase = g_combineBaseMgr.get(pItem->getID());
			if (pCombineBase && (pItem->m_itemInfo.m_num >= pCombineBase->m_pieceNum)){
				sortMap[0][sortID].push_back(pItem);//达到合成数量的碎片优先显示在前面
			}
			else {
				sortMap[1][sortID].push_back(pItem);
			}
		}		
	}
	
	for (UINT8 k = 0;k < 2;++k){
		//合并
		std::map<UINT32,std::vector<CItem *> >::iterator it =  sortMap[k].begin();	
		for (;it != sortMap[k].end(); ++it){
			std::vector<CItem *> & vec = it->second;
			for (UINT16 i = 0;i < vec.size(); ++i){
				for (UINT16 j = i+ 1; j < vec.size(); ++j){
					CItem * pItemi = vec[i];
					CItem * pItemj = vec[j];
					if (pItemi && pItemj){
						if (pItemi->canOverlayWithItem(pItemj)){
							pItemi->m_itemInfo.m_num += pItemj->m_itemInfo.m_num;	
							removeElement(pItemj);
							SAFE_DELETE(pItemj);
							vec[j] = NULL;
						}
					}	
				}	
			}
		}
	}

	bzero(m_pSlot,sizeof(CItem *) * m_capacity);
	UINT16 index = 0;
	for (UINT8 k = 0;k < 2;++k){
		std::map<UINT32,std::vector<CItem *> >::iterator it = sortMap[k].begin();
		for (;it != sortMap[k].end() ; ++it){
			for (UINT16 i = 0;i < it->second.size(); ++i){
				CItem * pItem = it->second[i];
				if (pItem){
					pItem->m_itemInfo.m_pos.m_pos = index;
					m_pSlot[index] = pItem;
					index += 1;
				}
			}
		}
	}
}

void CRuneBag::collectRune(UINT64 instanceId, UINT32 itemId, std::vector<CItem *> & itemVec) {
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (!m_pSlot[i] || m_pSlot[i]->m_itemInfo.m_itemID != itemId ||
                m_pSlot[i]->m_itemInfo.m_instanceID == instanceId) {
            continue;
        }
        if (!m_pSlot[i]->m_itemInfo.m_raiseLv &&
                !m_pSlot[i]->m_itemInfo.m_refineLv) {
            itemVec.push_back(m_pSlot[i]);
        }
    }
}

CItem * CRuneBag::copyItem(CItem * pSrcItem, UINT8 dstBagType) {
    if (!pSrcItem || !pSrcItem->m_pItemBase) {
        return NULL;
    }
	CItem * pItem = ATHENA_NEW CItem(pSrcItem->m_pItemBase);
	if (!pItem){
		return NULL;
    }
    pItem->m_itemInfo = pSrcItem->m_itemInfo;
	pItem->m_itemInfo.m_instanceID = CItem::genInstanceID();
    pItem->m_itemInfo.m_pos.m_bagType = dstBagType;
    return pItem;
}

CItem * CRuneBag::getItemBySlot(UINT16 slotIndex) {
    if (slotIndex >= m_capacity) {
        return NULL;
    }
    return m_pSlot[slotIndex];
}
