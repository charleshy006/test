#include "MountMgr.h" 
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include "GameServer.h"
#include "GameUser.h"
#include "TblDataMgr.h"

CMountMgr::CMountMgr(GameUser * pOwner):m_pOwner(pOwner)
{
	m_battleMountID	=	0;
	m_battleProps.resize(eMAX_BATTLE_PROP); 
	m_isRiding	=	false;
	m_battlePower	=	0;
}

CMountMgr::~CMountMgr()
{}

UINT32 CMountMgr::loadData(const char * buf)
{
	UINT8 ret = 0;
	UINT32 version = *(UINT32*)(buf + ret);
	if(s_MOUNT_SAVE_VERSION == version){
		ret += sizeof(UINT32);

		m_mount.m_level = *(UINT32*)(buf + ret);	
		ret += sizeof(UINT32);
		m_mount.m_exp = *(UINT32*)(buf + ret);
		ret += sizeof(UINT32);

		m_battleMountID = *(UINT32*)(buf + ret);
		ret += sizeof(UINT32);

		m_isRiding = *(bool*)(buf + ret); 
		ret += sizeof(bool);

		m_battlePower = *(UINT32*)(buf + ret);
		ret += sizeof(UINT32);

		UINT8 num = *(UINT8*)(buf + ret);
		ret += sizeof(UINT8);

		for(UINT8 i = 0; i < num; ++i){
			UINT32 id = *(UINT32*)(buf + ret);
			m_mountMap.insert(std::make_pair(id, id));
			ret += sizeof(UINT32); 
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[坐骑加载]level:%d, exp:%d, battleMountID:%d, isRiding:%d, battlePower:%d, mountMap.size:%d", m_mount.m_level, m_mount.m_exp, m_battleMountID, m_isRiding, m_battlePower, m_mountMap.size());
#endif

	return ret;
}

UINT32 CMountMgr::saveData(char * buf)
{
	UINT8 ret = 0;
	*(UINT32*)(buf + ret) = s_MOUNT_SAVE_VERSION;
	ret += sizeof(UINT32);

	*(UINT32*)(buf + ret) = m_mount.m_level;
	ret += sizeof(UINT32);

	*(UINT32*)(buf + ret) = m_mount.m_exp;
	ret += sizeof(UINT32);

	*(UINT32*)(buf + ret) = m_battleMountID;
	ret += sizeof(UINT32);

	*(bool*)(buf + ret) = m_isRiding;
	ret += sizeof(bool);

	*(UINT32*)(buf + ret) =	m_battlePower;
	ret += sizeof(UINT32); 

	*(UINT8*)(buf + ret) = m_mountMap.size();
	ret += sizeof(UINT8);

	std::map<UINT32, UINT32>::iterator it = m_mountMap.begin();
	for(;it != m_mountMap.end(); ++it){
		*(UINT32*)(buf + ret) = it->second;
		ret += sizeof(UINT32);
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[坐骑加载]level:%d, exp:%d, battleMountID:%d, isRiding:%d, battlePower:%d, mountMap.size:%d", m_mount.m_level, m_mount.m_exp, m_battleMountID, m_isRiding, m_battlePower, m_mountMap.size());
#endif

	return ret;
}

void CMountMgr::userOnline()
{
	MiscProto::stUserOnline retCmd;
	retCmd.set_battlemountid(m_battleMountID);
	retCmd.set_isfollow(m_isRiding);
	retCmd.set_mountlv(m_mount.m_level);

	std::map<UINT32, UINT32>::iterator it = m_mountMap.begin();
	for(;it != m_mountMap.end(); ++it){
		retCmd.add_mountlist(it->second);
	}

	m_pOwner->sendProto2Me(retCmd);	
}

void CMountMgr::openWindow()
{
	MiscProto::stOpenMountGrowWindowResult retCmd;

	retCmd.set_level(m_mount.m_level);
	retCmd.set_exp(m_mount.m_exp);
	retCmd.set_battle(m_battlePower);

	m_pOwner->sendProto2Me(retCmd);
}

void CMountMgr::calcBattlePower()
{
	float battlePower = 0.f;
	m_battleProps.clear();
	m_battleProps.resize(eMAX_BATTLE_PROP);

	//坐骑成长
	stMountgrowBase * pGrowBase = g_stMountGrowBaseMgr.get(m_mount.m_level);
	if(pGrowBase){
		std::map<UINT8, stBattlePropValue>::iterator it = pGrowBase->m_addProp.begin();
		for(;it != pGrowBase->m_addProp.end(); ++it){
			stBattlePropValue & prop = it->second;
			m_battleProps[prop.m_propID] += prop.m_val1;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[战力计算] id：%d, level:%d, Value:%d, total:%d", prop.m_propID, m_mount.m_level, prop.m_val1, m_battleProps[prop.m_propID]);
#endif
		}
	}

	//坐骑幻化
	std::map<UINT32, UINT32>::iterator it = m_mountMap.begin();
	for(;it != m_mountMap.end(); ++it){
		stMountIllusionBase * pMountBase = g_stMountIllusionBaseMgr.get(it->second);
		if(pMountBase){
			std::map<UINT8, stBattlePropValue>::iterator it = pMountBase->m_addProp.begin();
			for(;it != pMountBase->m_addProp.end(); ++it){
				stBattlePropValue & prop = it->second;
				m_battleProps[prop.m_propID] += prop.m_val1;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[战力计算] id：%d, Value:%d, total:%d", prop.m_propID, prop.m_val1, m_battleProps[prop.m_propID]);
#endif
			}
		}
	}

	UINT32 stoneBattle = 0;
	for (UINT8 i = MOUNT_BAG_POS1_INDEX; i <= MOUNT_BAG_POS8_INDEX; ++i){
		CItem * pItem = m_pOwner->m_bagMgr.m_equipBag.getEquipByIndex(i);
		if (pItem){
			pItem->setupPropState();
			stoneBattle += pItem->getBattlePower();
		}
	}

	battlePower += m_battleProps[eBATTLE_PROP_HP] * BP_HP_COEF; 
	battlePower += m_battleProps[eBATTLE_PROP_DAMAGE] * BP_DAMAGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_DODGERATE] * BP_DODGE_COEF;
	battlePower += m_battleProps[eBATTLE_PROP_BANGRATE] * BP_BANG_COEF;
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

	battlePower += std::max((INT32)(m_battleProps[eBATTLE_PROP_MP] - BP_MP_BASE),0) * BP_MP_COEF;
	battlePower += std::max((INT32)(m_battleProps[eBATTLE_PROP_HITRATE] - BP_HITRATE_BASE),0) *BP_HIT_COEF;
	battlePower += std::max((INT32)(m_battleProps[eBATTLE_PROP_BANGDAMAGE] - BP_BANGDAMAGE_BASE), 0) * BP_BANGDAMAGE_COEF;

	m_battlePower = (UINT32)battlePower + stoneBattle;
}

void CMountMgr::addProp()
{
	calcBattlePower();
	for(UINT8 i = 1; i < eMAX_BATTLE_PROP; ++i){
		m_pOwner->m_roleState.m_battleProps[i] += m_battleProps[i];
	}
}

void CMountMgr::mountLevelUp(MiscProto::stReqMountLevelUp *recvCmd)
{
	if(!recvCmd){
		return;
	}

	MiscProto::stMountLevelUpResult retCmd;
	stMountgrowBase * pGrowBase = g_stMountGrowBaseMgr.get(m_mount.m_level);

	if(!recvCmd->iteminfo().id()){
		retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败,道具不足,itemID:0！", m_pOwner->getName(), m_pOwner->getID());
#endif
		return ;
	}


	if(!pGrowBase){
		retCmd.set_result(MiscProto::stMountLevelUpResult::eInner_Error);	
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑成长](name:%s, charId:%d)内部错误,配置不存在", m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	CItem * pItem =  m_pOwner->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1, recvCmd->iteminfo().id());
	if(!pItem){
		retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑成长](name:%s, charId:%d)失败,道具不存在", m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	stMountgrowBase * pNextGrowBase = g_stMountGrowBaseMgr.get(m_mount.m_level + 1);
	if((m_mount.m_exp >= pGrowBase->m_needExp) && !pNextGrowBase){
		if(m_mount.m_exp == pGrowBase->m_needExp){
			retCmd.set_result(MiscProto::stMountLevelUpResult::eMax_Level);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[坐骑成长](name:%s, charId:%d)失败,已到最高等级", m_pOwner->getName(), m_pOwner->getID());
#endif 
			return;
		}
	}

	if((getStarByLv(m_mount.m_level) == 9) && (m_mount.m_exp >= pGrowBase->m_needExp)){

		if(pItem->m_pItemBase->m_type != ITEM_TYPE_MONUT_ADVANCED){
			retCmd.set_result(MiscProto::stMountLevelUpResult::eInner_Error);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[坐骑成长](name:%s, charId:%d)内部错误,道具配置不存在1", m_pOwner->getName(), m_pOwner->getID());
#endif
			return;
		}

		std::vector<std::pair<UINT8, UINT32> >::iterator resIt = pGrowBase->m_orderResVec.begin();
		for(;resIt != pGrowBase->m_orderResVec.end(); ++resIt){
			std::pair<UINT8, UINT32>& res = *resIt;
			if(!m_pOwner->checkResource((eResource)res.first, res.second)){
				retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败,资源不足！", m_pOwner->getName(), m_pOwner->getID());
#endif
				return ;
			}
		}

		std::vector<std::pair<UINT32, UINT32> >::iterator itemIt = pGrowBase->m_orderItemVec.begin();
		for(;itemIt != pGrowBase->m_orderItemVec.end(); ++itemIt){
			std::pair<UINT32, UINT32>& res = *itemIt;
			if(!m_pOwner->checkAssert(res.first, res.second)){
				retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败,道具不足！", m_pOwner->getName(), m_pOwner->getID());
#endif
				return ;
			}
		}

		resIt = pGrowBase->m_orderResVec.begin();
		for(;resIt != pGrowBase->m_orderResVec.end(); ++resIt){
			std::pair<UINT8, UINT32>& res = *resIt;
			if(!m_pOwner->removeResource((eResource)res.first, res.second, "坐骑进阶")){
				retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败,扣除资源失败！", m_pOwner->getName(), m_pOwner->getID());
#endif
				return ;
			}
		}

		itemIt = pGrowBase->m_orderItemVec.begin();
		for(;itemIt != pGrowBase->m_orderItemVec.end(); ++itemIt){
			std::pair<UINT32, UINT32>& res = *itemIt;
			if(!m_pOwner->removeAssert(res.first, res.second,0,"坐骑进阶")){
				retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败,扣除道具失败！", m_pOwner->getName(), m_pOwner->getID());
#endif
				return ;
			}
		}

		++m_mount.m_level;
		m_mount.m_exp = 0;

	}else{

		if(pItem->m_pItemBase->m_type != ITEM_TYPE_MONUT_FEED){
			retCmd.set_result(MiscProto::stMountLevelUpResult::eInner_Error);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[坐骑成长](name:%s, charId:%d)内部错误,道具配置不存在2", m_pOwner->getName(), m_pOwner->getID());
#endif
			return;
		}

		std::vector<std::pair<UINT8, UINT32> >::iterator resIt = pGrowBase->m_costResVec.begin();
		for(;resIt != pGrowBase->m_costResVec.end(); ++resIt){
			std::pair<UINT8, UINT32>& res = *resIt;
			if(!m_pOwner->checkResource((eResource)res.first, res.second)){
				retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑成长](name:%s, charId:%d)失败,资源不足！", m_pOwner->getName(), m_pOwner->getID());
#endif
				return ;
			}
		}

		std::map<UINT32, UINT32>::iterator itemIt = pGrowBase->m_costItemMap.begin();
		for(;itemIt != pGrowBase->m_costItemMap.end(); ++itemIt){
			if(!m_pOwner->checkAssert(itemIt->first,itemIt->second)){
				retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑成长](name:%s, charId:%d)失败,道具不足！", m_pOwner->getName(), m_pOwner->getID());
#endif
				return;
			}
		}

		resIt = pGrowBase->m_costResVec.begin();//资源
		for(;resIt != pGrowBase->m_costResVec.end(); ++resIt){
			std::pair<UINT8, UINT32>& res = *resIt;
			if(!m_pOwner->removeResource((eResource)res.first,res.second,"坐骑成长")){
				retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑成长](name:%s, charId:%d)失败,资源扣除失败！", m_pOwner->getName(), m_pOwner->getID());
#endif
				return ;
			}
		}

		UINT32 needItemNum = 0;
		itemIt = pGrowBase->m_costItemMap.begin();
		for(;itemIt != pGrowBase->m_costItemMap.end(); ++itemIt){
			if(!m_pOwner->removeAssert(itemIt->first,itemIt->second,0,"坐骑成长")){
				retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑成长](name:%s, charId:%d)失败,道具扣除失败！", m_pOwner->getName(), m_pOwner->getID());
#endif
				return;
			}

			if(itemIt->first == pItem->getID()){
				needItemNum += itemIt->second;
			}
		}

		//m_mount.m_exp += pItem->m_pItemBase->m_effect * recvCmd->iteminfo().num();
		m_mount.m_exp += pItem->m_pItemBase->m_effect * needItemNum;
		if(m_mount.m_exp >= pGrowBase->m_needExp){
			if(!pNextGrowBase || (getStarByLv(m_mount.m_level) == 9)){
				m_mount.m_exp = pGrowBase->m_needExp;
			}else{
				m_mount.m_level += m_mount.m_exp / pGrowBase->m_needExp;
				m_mount.m_exp = m_mount.m_exp % pGrowBase->m_needExp;
			}
		}
	}

	m_pOwner->setupRoleState();
	m_pOwner->sendBattleProp();

	MiscProto::stRefreshBattle cmd;
	cmd.set_battle(m_battlePower);
	m_pOwner->sendProto2Me(cmd);  

	retCmd.set_level(m_mount.m_level);
	retCmd.set_exp(m_mount.m_exp);
	retCmd.set_result(MiscProto::stMountLevelUpResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
}

bool CMountMgr::checkMaterial(stMountgrowBase * pGrowBase)
{
	if(!pGrowBase){
		return false;
	}

	std::vector<std::pair<UINT8, UINT32> >::iterator resIt = pGrowBase->m_costResVec.begin();
	for(;resIt != pGrowBase->m_costResVec.end(); ++resIt){
		if(!m_pOwner->checkResource((eResource)(*resIt).first, (*resIt).second)){
#ifdef _HDZ_DEBUG
			Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)失败，资源不足！",m_pOwner->getName(), m_pOwner->getID());
#endif
			return false;
		}
	}

	std::map<UINT32, UINT32>::iterator itemIt = pGrowBase->m_costItemMap.begin();
	for(;itemIt != pGrowBase->m_costItemMap.end(); ++itemIt){
		if(!m_pOwner->checkAssert(itemIt->first, itemIt->second)){
#ifdef _HDZ_DEBUG
			Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)失败，道具不足！",m_pOwner->getName(), m_pOwner->getID());
#endif
			return false;
		}
	}
	return true;
}

void CMountMgr::akeyLevelUp(MiscProto::stReqMountLevelUp *recvCmd)
{
	MiscProto::stMountLevelUpResult retCmd;

	stMountgrowBase * pGrowBase = g_stMountGrowBaseMgr.get(m_mount.m_level);
	if(!pGrowBase){
		retCmd.set_result(MiscProto::stMountLevelUpResult::eInner_Error); 
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)内部错误,配置不存在", m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	stMountgrowBase * pNextGrowBase = g_stMountGrowBaseMgr.get(m_mount.m_level + 1);
	if((m_mount.m_exp >= pGrowBase->m_needExp) && !pNextGrowBase){
		if(m_mount.m_exp == pGrowBase->m_needExp){
			retCmd.set_result(MiscProto::stMountLevelUpResult::eMax_Level);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[坐骑成长](name:%s, charId:%d)失败,已到最高等级", m_pOwner->getName(),m_pOwner->getID());
#endif
			return;
		}
	}

	UINT8 nowStar = getStarByLv(m_mount.m_level);

	if((9 == nowStar) && (m_mount.m_exp >= pGrowBase->m_needExp)){
		retCmd.set_result(MiscProto::stMountLevelUpResult::eStar_max);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)失败，请先升阶！",m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	CItem * pItem =  m_pOwner->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1, recvCmd->iteminfo().id());

	if(!pItem || pItem->m_pItemBase->m_type != ITEM_TYPE_MONUT_FEED){
		retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)失败，经验物品不存在！",m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	} 

	if(!checkMaterial(pGrowBase)){
		retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough); 
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)失败，道具不足！",m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	UINT32 obtainExp = pItem->m_pItemBase->m_effect; 

	std::map<UINT8, UINT32> resMap;
	std::map<UINT32, UINT32> itemMap;

	std::map<UINT8, UINT32>::iterator resTotalIt;
	std::map<UINT32, UINT32>::iterator itemTotalIt;

	std::vector<std::pair<UINT8, UINT32> >::iterator resIt;
	std::map<UINT32, UINT32>::iterator itemIt;

	UINT32  needExpItemNum  =   0;                                                                               
	UINT32  tmpLv           =   m_mount.m_level;                                                                 
	UINT32  tmpExp          =   m_mount.m_exp; 

	for(UINT8 i = nowStar; i < 10; ++i){
		pGrowBase = g_stMountGrowBaseMgr.get(tmpLv); 
		if(pGrowBase){
			UINT32 loopNum = 0;
			
			bool    jumpOutRes  =   false;
            bool    jumpOutItem =   false;
			resMap.clear();
			itemMap.clear();

			if((getStarByLv(tmpLv) == 9) && (tmpExp >= pGrowBase->m_needExp)){
				tmpExp = pGrowBase->m_needExp;
				break;
			}

			UINT32 subExp = pGrowBase->m_needExp - tmpExp;
			itemIt = pGrowBase->m_costItemMap.find(pItem->getID());
			if(itemIt != pGrowBase->m_costItemMap.end()){
				UINT32 itemExp = itemIt->second * obtainExp;
				if(!itemExp){
					break;
				}

				loopNum = subExp / itemExp;
				if(subExp % itemExp){//剩余不足一次 按一次算
					++loopNum;
				}
			
				if(!loopNum){
#ifdef _HDZ_DEBUG
					Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)subExp:%d, itemExp:%d",m_pOwner->getName(), m_pOwner->getID(), subExp, itemExp);
#endif
					break;
				}

				UINT32 count = 0;
				for(UINT16 i = 0; i < loopNum; ++i){
					itemIt = pGrowBase->m_costItemMap.begin();
					for(;itemIt != pGrowBase->m_costItemMap.end(); ++itemIt){
						itemTotalIt = itemMap.find(itemIt->first);
						
						if(itemTotalIt == itemMap.end()){
							itemMap.insert(std::make_pair(itemIt->first, itemIt->second));
						}else{
							itemTotalIt->second += itemIt->second;
						}

						itemTotalIt = itemMap.find(itemIt->first);
						if(!m_pOwner->checkAssert(itemTotalIt->first, itemTotalIt->second)){ 
#ifdef _HDZ_DEBUG
							Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)道具不足！has:%d, needNum:%d, count:%d", m_pOwner->getName(), m_pOwner->getID(), m_pOwner->m_bagMgr.getItemNumByID(itemTotalIt->first) , itemTotalIt->second, count);
#endif
							jumpOutItem = true;
							break;
						}
						++count;
					}

					if(jumpOutItem){//道具不足，不用再循环了
						break;
					}
				}

				UINT8   itemSize =pGrowBase->m_costItemMap.size();
				UINT32  itemSufficientNum = count / itemSize;//保证这么多次道具充足
				count = 0;
				
				if(!itemSufficientNum){
#ifdef _HDZ_DEBUG
					Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)本级道具不足以涨一次经验，跳出",m_pOwner->getName(), m_pOwner->getID());
#endif
					break;//本级道具不足以涨一次经验，跳出
				}

				for(UINT16 i = 0; i < itemSufficientNum; ++i){
					resIt = pGrowBase->m_costResVec.begin();
					for(; resIt != pGrowBase->m_costResVec.end(); ++resIt){
						resTotalIt = resMap.find((*resIt).first);
						if(resTotalIt == resMap.end()){
							resMap.insert(std::make_pair((*resIt).first, (*resIt).second));
						}else{
							resTotalIt->second += (*resIt).second;
						}

						resTotalIt = resMap.find((*resIt).first);
						if(!m_pOwner->checkResource((eResource)resTotalIt->first, resTotalIt->second)){
#ifdef _HDZ_DEBUG
							Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)资源不足！has:%d, needNum:%d, count:%d",m_pOwner->getName(), m_pOwner->getID(), m_pOwner->getGoldCoin(), resTotalIt->second, count);
#endif
							jumpOutRes = true; 
							break;
						}
						++count;
					}

					if(jumpOutRes){
						break;//资源不足，不用再循环了
					}
				}

				UINT8   resSize =pGrowBase->m_costResVec.size();
				UINT32  resSufficientNum = count / resSize;//保证这么多次资源充足
				if(!resSufficientNum){
#ifdef _HDZ_DEBUG
					Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)本级金币不足以涨一次经验，跳出", m_pOwner->getName(), m_pOwner->getID());
#endif
					break;//本级金币不足以涨一次经验，跳出
				}
				
				itemIt = pGrowBase->m_costItemMap.begin();
				for(;itemIt != pGrowBase->m_costItemMap.end(); ++itemIt){
					if(!m_pOwner->removeAssert(itemIt->first, itemIt->second * resSufficientNum, 0, "坐骑一键升级")){
						retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough); 
						m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
						Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)失败，扣除道具失败！",m_pOwner->getName(), m_pOwner->getID());
#endif
						return;
					}

					if(pItem->getID() == itemIt->first){
						needExpItemNum = itemIt->second * resSufficientNum;
					}
				}

				resIt = pGrowBase->m_costResVec.begin(); 
				for(;resIt != pGrowBase->m_costResVec.end(); ++resIt){
					if(!m_pOwner->removeResource((eResource)(*resIt).first, (*resIt).second * resSufficientNum, "坐骑一键升级")){
						retCmd.set_result(MiscProto::stMountLevelUpResult::eRes_Not_Enough);
						m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
						Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)失败, 扣除资源失败！",m_pOwner->getName(), m_pOwner->getID());
#endif
						return;
					}
				}

				tmpExp += needExpItemNum * obtainExp;
				if(tmpExp >= pGrowBase->m_needExp){
					if(getStarByLv(tmpLv) == 9){
						tmpExp = pGrowBase->m_needExp;	
					}else{
						tmpLv += tmpExp / pGrowBase->m_needExp;
						tmpExp = tmpExp % pGrowBase->m_needExp;
					}
				}

				if(jumpOutRes || jumpOutItem){
					Athena::logger->error("[坐骑一键成长](name:%s, charId:%d)jumpOutRes, jumpOutItem", m_pOwner->getName(),m_pOwner->getID());
					break;
				}
			}else{
				break;
			}
		}else{
			break;
		}
	}

	UINT32 oldLv = m_mount.m_level;
	UINT32 oldExp = m_mount.m_exp;

	m_mount.m_level = tmpLv;
	m_mount.m_exp = tmpExp;

//	calcBattlePower();
	m_pOwner->setupRoleState();
	m_pOwner->sendBattleProp();

	MiscProto::stRefreshBattle cmd;
	cmd.set_battle(m_battlePower);
	m_pOwner->sendProto2Me(cmd);

	retCmd.set_result(MiscProto::stMountLevelUpResult::eSuccess);
	retCmd.set_level(m_mount.m_level);
	retCmd.set_exp(m_mount.m_exp);
	m_pOwner->sendProto2Me(retCmd);
 
	Athena::logger->trace("[坐骑一键成长](name:%s, charId:%d)一键升级成功！oldLv:%d --> newLv:%d, oldExp:%d --> newExp:%d",m_pOwner->getName(), m_pOwner->getID(), oldLv, m_mount.m_level, oldExp, m_mount.m_exp);

} 

void CMountMgr::followMaster()
{
	MiscProto::stMountFollowResult retCmd;
	if(!m_battleMountID){
		retCmd.set_result(MiscProto::stMountFollowResult::eIs_Not_Battle);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG 
		Athena::logger->error("[坐骑跟随](name:%s, charId:%d),跟随失败, 还没有装备坐骑!",m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	m_isRiding = m_isRiding ? false : true;

	retCmd.set_isfollow(m_isRiding);
	retCmd.set_result(MiscProto::stMountFollowResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
}

void CMountMgr::battleMount(UINT32 mountID)
{
	MiscProto::stButtleMountResult retCmd;
	if(m_battleMountID == mountID){
		retCmd.set_result(MiscProto::stButtleMountResult::eHas_Battle);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[幻化坐骑](name:%s, charId:%d)失败,该坐骑(%d)已幻化！", m_pOwner->getName(), m_pOwner->getID(), mountID);
#endif
		return;
	}

	stRoleBase * pHeroBase = g_roleBaseMgr.get(mountID);
	if(!pHeroBase){
		retCmd.set_result(MiscProto::stButtleMountResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[幻化坐骑](name:%s, charId:%d)失败,找不到(%d)的配置！", m_pOwner->getName(), m_pOwner->getID(), mountID);
#endif
		return;
	}

	std::map<UINT32, UINT32>::iterator it = m_mountMap.find(mountID);
	if(it == m_mountMap.end()){
		retCmd.set_result(MiscProto::stButtleMountResult::eIs_Not_Unlock);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG 
		Athena::logger->error("[幻化坐骑](name:%s, charId:%d)失败,该坐骑(%d)不存在！",m_pOwner->getName(), m_pOwner->getID(), mountID);
#endif
		return;
	}

	m_battleMountID = mountID;

	retCmd.set_result(MiscProto::stButtleMountResult::eSuccess);
	retCmd.set_mountid(m_battleMountID);
	m_pOwner->sendProto2Me(retCmd);
}

void CMountMgr::mountUnreal(UINT32 mountID)
{
	MiscProto::stMountUnrealResult  retCmd;

	stRoleBase * pHeroBase = g_roleBaseMgr.get(mountID);
	if(!pHeroBase){
		retCmd.set_result(MiscProto::stMountUnrealResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败,找不到mountID=%d的配置！", m_pOwner->getName(), m_pOwner->getID(), mountID);
#endif
		return;
	}

	stMountIllusionBase * pIllusionBase = g_stMountIllusionBaseMgr.get(mountID);
	if(!pIllusionBase){
		retCmd.set_result(MiscProto::stMountUnrealResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败,找不到mountID=%d的幻化配置！", m_pOwner->getName(), m_pOwner->getID(), mountID);
#endif
		return;
	}

	if(!pIllusionBase->m_unrealID){
		retCmd.set_result(MiscProto::stMountUnrealResult::eMax_Unreal);
		m_pOwner->sendProto2Me(retCmd); 
#ifdef _HDZ_DEBUG
		Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败,已经进阶到顶了！", m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	std::vector<std::pair<UINT8, UINT32> >::iterator resVecIt = pIllusionBase->m_costResVec.begin();
	for(;resVecIt != pIllusionBase->m_costResVec.end(); ++resVecIt){
		if(!m_pOwner->checkResource((eResource)(*resVecIt).first, (*resVecIt).second)){
			retCmd.set_result(MiscProto::stMountUnrealResult::eRes_Not_Enough);
			m_pOwner->sendProto2Me(retCmd); 
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败, 资源不足!",m_pOwner->getName(), m_pOwner->getID());
#endif
			return;
		}
	}

	std::vector<std::pair<UINT32, UINT32> >::iterator itemVecIt = pIllusionBase->m_costItemVec.begin();
	for(; itemVecIt != pIllusionBase->m_costItemVec.end(); ++itemVecIt){
		if(!m_pOwner->checkAssert((*itemVecIt).first, (*itemVecIt).second)){
			retCmd.set_result(MiscProto::stMountUnrealResult::eRes_Not_Enough);
			m_pOwner->sendProto2Me(retCmd); 
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败, 道具不足!",m_pOwner->getName(), m_pOwner->getID());
#endif
			return;
		}
	}

	std::vector<stUnloackCondition>::iterator unLockIt = pIllusionBase->m_unlock.begin();
	for(;unLockIt != pIllusionBase->m_unlock.end(); ++unLockIt){
		stUnloackCondition & cond = *unLockIt;
		if(1 == cond.m_type){
			if(m_mount.m_level < cond.m_cond){
				retCmd.set_result(MiscProto::stMountUnrealResult::eCond_Not_Meet);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败, 条件不满足!",m_pOwner->getName(), m_pOwner->getID());
#endif
				return;
			}
		}else if(3 == cond.m_type){
			std::map<UINT32, UINT32>::iterator mapIt = m_mountMap.find(cond.m_cond);
			if(mapIt == m_mountMap.end()){
				retCmd.set_result(MiscProto::stMountUnrealResult::eCond_Not_Meet);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败, 条件不满足!",m_pOwner->getName(), m_pOwner->getID());
#endif
				return;
			}
		}
	}

	std::map<UINT32, UINT32>::iterator mapIt = m_mountMap.find(mountID);
	if(mapIt != m_mountMap.end()){
		m_mountMap.erase(mapIt);
		m_mountMap.insert(std::make_pair(pIllusionBase->m_unrealID, pIllusionBase->m_unrealID));
	}else{
		Athena::logger->trace("[坐骑进阶](name:%s, charId:%d)解锁 id = %d",m_pOwner->getName(), m_pOwner->getID(), mountID);
		m_mountMap.insert(std::make_pair(pIllusionBase->m_unrealID, pIllusionBase->m_unrealID));
	}

	resVecIt = pIllusionBase->m_costResVec.begin();
	for(;resVecIt != pIllusionBase->m_costResVec.end(); ++resVecIt){
		if(!m_pOwner->removeResource((eResource)(*resVecIt).first, (*resVecIt).second, "坐骑进阶")){
			retCmd.set_result(MiscProto::stMountUnrealResult::eRes_Not_Enough);
			m_pOwner->sendProto2Me(retCmd); 
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败, 扣除资源失败!",m_pOwner->getName(), m_pOwner->getID());
#endif
			return;
		}
	}

	itemVecIt = pIllusionBase->m_costItemVec.begin();
	for(; itemVecIt != pIllusionBase->m_costItemVec.end(); ++itemVecIt){
		if(!m_pOwner->removeAssert((*itemVecIt).first, (*itemVecIt).second, 0, "坐骑进阶")){
			retCmd.set_result(MiscProto::stMountUnrealResult::eRes_Not_Enough);
			m_pOwner->sendProto2Me(retCmd); 
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[坐骑进阶](name:%s, charId:%d)失败, 扣除道具失败!",m_pOwner->getName(), m_pOwner->getID());
#endif
			return;
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[坐骑进阶](name:%s, charId:%d)成功! oldId:%d, newId:%d",m_pOwner->getName(), m_pOwner->getID(), mountID, pIllusionBase->m_unrealID);
#endif

	m_pOwner->setupRoleState();
	m_pOwner->sendBattleProp();

	//m_battleMountID = pIllusionBase->m_unrealID;

	MiscProto::stRefreshBattle cmd;
	cmd.set_battle(m_battlePower);
	m_pOwner->sendProto2Me(cmd);

	retCmd.set_oldmountid(mountID);
	retCmd.set_mountid(pIllusionBase->m_unrealID);
	retCmd.set_result(MiscProto::stMountUnrealResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);	
}

void CMountMgr::sendStoneProp()
{
	Role::stStonePropResult retCmd;

	std::vector<UINT32>  battleProps;         //战斗属性
	battleProps.resize(eMAX_BATTLE_PROP);

	for (UINT8 i = MOUNT_BAG_POS1_INDEX; i <= MOUNT_BAG_POS8_INDEX; ++i){
		CItem * pItem = m_pOwner->m_bagMgr.m_equipBag.getEquipByIndex(i);
		if (pItem){
			pItem->setupPropState();
			for(UINT8 k = 1; k < pItem->m_battleProps.size(); ++k){
				battleProps[k] += pItem->m_battleProps[k];
			}
		}
	} 
	
	for(UINT8 i = 1; i < battleProps.size(); ++i){
		if(battleProps[i]){
			Role::stBattleProp * pSynProp = retCmd.add_battleprop();
			if (pSynProp){
				pSynProp->set_propid(i);
				pSynProp->set_propval(battleProps[i]);
			}
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}
