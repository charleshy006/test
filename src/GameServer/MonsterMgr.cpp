#include "MonsterMgr.h"
#include "AthenaNew.h"    
#include "Log4cxxHelp.h"  
#include "KVPair.h"
#include "Misc.pb.h"
#include "UtilFunction.h"
#include "GameServer.h"
#include "GameUser.h"
#include "M78.h"

extern GameServer * g_gameServer;

CMonsterMgr::CMonsterMgr(GameUser * pOwner)
:m_pOwner(pOwner),
tag(false)
{
}

CMonsterMgr::~CMonsterMgr()
{}


UINT32 CMonsterMgr::loadData(const char * buf)
{
	UINT32 ret = 0;
	UINT32 version = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	if (version == s_MONSTER_DB_VERSION){
		m_bossInfo.m_bossLv			=	*(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
		m_bossInfo.m_bossHp			=	*(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
		m_bossInfo.m_time			=	*(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
		m_bossInfo.m_attackTime		=	*(UINT32 *)(buf + ret);
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了(name=%s,id=%d)的怪兽数据bossLv:%d, bossHp:%d, time:%d, attackTime:%d",m_pOwner->getName(),m_pOwner->getID(),m_bossInfo.m_bossLv,m_bossInfo.m_bossHp, m_bossInfo.m_time,m_bossInfo.m_attackTime); 
#endif
	return ret;
}

UINT32 CMonsterMgr::saveData(char * buf)
{
	UINT32 ret = 0;
	*(UINT32*)(buf + ret) = s_MONSTER_DB_VERSION;
	ret += sizeof(UINT32);
	*(UINT32*)(buf + ret) = m_bossInfo.m_bossLv;
	ret += sizeof(UINT32);
	*(UINT32*)(buf + ret) = m_bossInfo.m_bossHp;
	ret += sizeof(UINT32);
	*(UINT32*)(buf + ret) = m_bossInfo.m_time;
	ret += sizeof(UINT32);
	*(UINT32*)(buf + ret) = m_bossInfo.m_attackTime;

#ifdef _HDZ_DEBUG
	Athena::logger->trace("保存了(name=%s,id=%d)的怪兽数据bossLv:%d, bossHp:%d, time:%d, attackTime:%d",m_pOwner->getName(),m_pOwner->getID(),m_bossInfo.m_bossLv,m_bossInfo.m_bossHp, m_bossInfo.m_time,m_bossInfo.m_attackTime); 
#endif
	return ret;
}

void CMonsterMgr::openWindow()
{
	MiscProto::stOpenMonsterWindowResult retCmd;

	if(!m_bossInfo.m_bossLv){//第一次打开
		CM78Cfg::stBoss* boss = CM78Cfg::getSingleton().getBossByLv(1);

		if(!boss){
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[打开小怪兽窗口](name:%s, charId:%d)错误，当前配置不存在!",m_pOwner->getName(),m_pOwner->getID());
#endif
			return;
		}

		m_bossInfo.m_bossLv = 1;
		m_bossInfo.m_bossHp = boss->m_hp;
		if (Athena::global["nftServer"] == "true"){
			m_bossInfo.m_attackTime = 0;
		}else{
			m_bossInfo.m_attackTime = CM78Cfg::getSingleton().getMaxCount();
		}
		m_bossInfo.m_time = g_gameServer->getSec();
	}

	retCmd.set_bosslv(m_bossInfo.m_bossLv);
	retCmd.set_bosshp(m_bossInfo.m_bossHp);
	retCmd.set_time(m_bossInfo.m_time + CM78Cfg::getSingleton().getRecoveryTime() - g_gameServer->getSec());
	retCmd.set_attacktime(m_bossInfo.m_attackTime);
	retCmd.set_buytime(m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_MONSTER_BUY_COUNT));
	m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打开小怪兽窗口成功");
#endif
}

void CMonsterMgr::attackMonster()
{
	MiscProto::stAttackMonsterResult retCmd;

	if(0 == m_bossInfo.m_attackTime){
		retCmd.set_result(MiscProto::stAttackMonsterResult::eTime_Out);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG 
		 Athena::logger->error("[打小怪兽](name:%s, charId:%d)攻击失败，攻击次数不足！",m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	--m_bossInfo.m_attackTime;
	
	Role::stObtainItemTip itemTipCmd;
	MiscProto::stObtainItemTip itemCmd;

	if(m_bossInfo.m_bossHp > 0){
		--m_bossInfo.m_bossHp;

		CM78Cfg::stBoss* boss = CM78Cfg::getSingleton().getBossByLv(m_bossInfo.m_bossLv);
		if(!boss){
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[打小怪兽](name:%s, charId:%d)错误，当前配置不存在!",m_pOwner->getName(),m_pOwner->getID());
#endif
			return;
		}

		UINT32 drawNum = boss->m_countNum;
		UINT32 drawID = boss->m_drawID;
		std::vector<std::pair<UINT32, UINT32> > vec;
		CDrawInfoCfg::getSingleton().getAttackRewardById(drawID, drawNum, false, vec);
		std::vector<std::pair<UINT32, UINT32> >::iterator it = vec.begin();
		for(;it != vec.end(); ++it){
			std::pair<UINT32, UINT32> & one = *it;
			MiscProto::stSynTipItem * pSyn = itemCmd.add_getitems();
			if(pSyn){
				m_pOwner->giveAssert(one.first,one.second,0,"攻击小怪兽");
				pSyn->set_itemid(one.first);
				pSyn->set_num(one.second);
			}
		}

		vec.clear();

		if(0 == m_bossInfo.m_bossHp){
			CM78Cfg::stBoss* nextBoss = CM78Cfg::getSingleton().getBossByLv(m_bossInfo.m_bossLv+1);
			CDrawInfoCfg::getSingleton().getDeadRewardById(boss->m_deadID, 1, vec);
			it = vec.begin();
			for(;it != vec.end(); ++it){                                  
				std::pair<UINT32, UINT32> & one = *it;                    
				Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();  
				if(pSyn){                                                 
					m_pOwner->giveAssert(one.first,one.second,0,"攻击小怪兽死亡");
					pSyn->set_itemid(one.first);
					pSyn->set_num(one.second);
				}                                                         
			}      

			if(nextBoss){
				m_bossInfo.m_bossLv = m_bossInfo.m_bossLv + 1;
				m_bossInfo.m_bossHp = nextBoss->m_hp;
			}else{
				m_bossInfo.m_bossHp = boss->m_hp;
			}
		}else{
			vec.clear();
			CDrawInfoCfg::getSingleton().getAttackRewardByIdAndRP(boss->m_exDraw, boss->m_exrp, 1, vec);
			it = vec.begin();
			for(;it != vec.end(); ++it){                                  
				std::pair<UINT32, UINT32> & one = *it;
				Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
				if(pSyn){
					m_pOwner->giveAssert(one.first,one.second,0,"攻击小怪兽额外");
					pSyn->set_itemid(one.first);
					pSyn->set_num(one.second);
				}                                                         
			}    
		}
	}

	retCmd.set_result(MiscProto::stAttackMonsterResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
	m_pOwner->sendProto2Me(itemTipCmd);
	m_pOwner->sendProto2Me(itemCmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("攻击小怪兽成功");
#endif
}

void CMonsterMgr::aKeyAttackMonster()
{
	MiscProto::stAKeyAttackMonsterResult retCmd;

	if(!m_pOwner->getCountByVip(eVipFun_AKay_PaPaPa)){
		retCmd.set_result(MiscProto::stAKeyAttackMonsterResult::eVip_Lv_Too_Low);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[打小怪兽](name:%s, charId:%d)一键攻击失败,Vip等级不足！",m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	if(0 == m_bossInfo.m_attackTime){ 
		retCmd.set_result(MiscProto::stAKeyAttackMonsterResult::eTime_Out);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG 
		Athena::logger->error("[打小怪兽](name:%s, charId:%d)一键攻击失败，攻击次数不足！",m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}


	std::map<UINT32, UINT32> itemMap;

	UINT32 attackNum = m_bossInfo.m_attackTime > m_bossInfo.m_bossHp ? m_bossInfo.m_bossHp : m_bossInfo.m_attackTime;

	CM78Cfg::stBoss* boss = CM78Cfg::getSingleton().getBossByLv(m_bossInfo.m_bossLv);
	if(!boss){
#ifdef _HDZ_DEBUG 
		Athena::logger->error("[一键攻击小怪兽](name:%s, charId:%d)错误，当前配置不存在!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return;
	}

	UINT32 drawNum = boss->m_countNum;
	UINT32 drawID = boss->m_drawID;
	std::vector<std::pair<UINT32, UINT32> > vec;
	CDrawInfoCfg::getSingleton().getAttackRewardById(drawID, attackNum*drawNum, true, vec);
	std::vector<std::pair<UINT32, UINT32> >::iterator it = vec.begin();
	for(;it != vec.end(); ++it){
		std::pair<UINT32, UINT32> & one = *it;
		/*
		Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
		if(pSyn){
			m_pOwner->giveAssert(one.first,one.second,0,"一键攻击小怪兽");
			pSyn->set_itemid(one.first);
			pSyn->set_num(one.second);
		}
		*/
		std::map<UINT32, UINT32>::iterator mapIt = itemMap.find(one.first);
		if(mapIt == itemMap.end()){
			itemMap[one.first] = one.second;
		}else{
			mapIt->second += one.second;
		}

	}

	vec.clear();
	CDrawInfoCfg::getSingleton().getAttackRewardByIdAndRP(boss->m_exDraw, boss->m_exrp, attackNum, vec);
	it = vec.begin();
	for(;it != vec.end(); ++it){
		std::pair<UINT32, UINT32> & one = *it;
		/*
		Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
		if(pSyn){
			m_pOwner->giveAssert(one.first,one.second,0,"一键攻击小怪兽额外");
			pSyn->set_itemid(one.first);
			pSyn->set_num(one.second);
		}
		*/

		std::map<UINT32, UINT32>::iterator mapIt = itemMap.find(one.first);
		if(mapIt == itemMap.end()){
			itemMap[one.first] = one.second;
		}else{
			mapIt->second += one.second;
		}
	}

	m_bossInfo.m_attackTime -= attackNum;
	m_bossInfo.m_bossHp -= attackNum;

	if(0 == m_bossInfo.m_bossHp){
		CM78Cfg::stBoss* nextBoss = CM78Cfg::getSingleton().getBossByLv(m_bossInfo.m_bossLv + 1);
		if(nextBoss){
			m_bossInfo.m_bossLv = m_bossInfo.m_bossLv + 1;
			m_bossInfo.m_bossHp = nextBoss->m_hp;
		}else{
			m_bossInfo.m_bossHp = boss->m_hp;
		}

		vec.clear();
		CDrawInfoCfg::getSingleton().getDeadRewardById(boss->m_deadID, 1, vec);
		it = vec.begin();
		for(;it != vec.end(); ++it){
			std::pair<UINT32, UINT32> & one = *it;
			/*
			   Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();  
			   if(pSyn){                                                 
			   m_pOwner->giveAssert(one.first,one.second,0,"一键攻击小怪兽死亡");
			   pSyn->set_itemid(one.first);
			   pSyn->set_num(one.second);
			   }  
			   */

			std::map<UINT32, UINT32>::iterator mapIt = itemMap.find(one.first);
			if(mapIt == itemMap.end()){
				itemMap[one.first] = one.second;
			}else{
				mapIt->second += one.second;
			}
		}
	}

	Role::stObtainItemTip itemTipCmd;
	std::map<UINT32, UINT32>::iterator mapIt = itemMap.begin();
	for(;mapIt != itemMap.end(); ++mapIt){
		Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();  
		if(pSyn){                                                 
			m_pOwner->giveAssert(mapIt->first,mapIt->second,0,"一键攻击小怪兽");
			pSyn->set_itemid(mapIt->first);
			pSyn->set_num(mapIt->second);
		}  
	}

	retCmd.set_result(MiscProto::stAKeyAttackMonsterResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
	m_pOwner->sendProto2Me(itemTipCmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("一键攻击小怪兽成功");
#endif
}

void CMonsterMgr::buyAttackTime()
{
	MiscProto::stBuyAttackTimeResult retCmd;

	UINT32 hasBuyNum = m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_MONSTER_BUY_COUNT);
	UINT32 needItemID = CM78Cfg::getSingleton().getCostItemID();
	UINT32 needDimaond = CM78Cfg::getSingleton().getNeedDiamondByNum(hasBuyNum);
	if(needDimaond == (UINT32)-1){
		retCmd.set_result(MiscProto::stBuyAttackTimeResult::eMax_Count);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_debug
		Athena::logger->error("[打小怪兽](name:%s, charId:%d)购买失败,配置不存在,已达上限！",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (!m_pOwner->removeAssert(needItemID,needDimaond,0,"购买攻击次数")){
		if (needItemID == eResource_Diamond)
		{
			retCmd.set_result(MiscProto::stBuyAttackTimeResult::eDiamond_Not_Enough);
		}
		else
		{
			retCmd.set_result(MiscProto::stBuyAttackTimeResult::eItem_Not_Enough);
		}
		
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_debug 
		Athena::logger->error("[打小怪兽](name:%s, charId:%d)购买失败，资源不足！",m_pOwner->getName(), m_pOwner->getID());
#endif
		return;
	}

	m_bossInfo.m_time = g_gameServer->getSec();

	m_bossInfo.m_attackTime += CM78Cfg::getSingleton().getBuyCount();

	m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_MONSTER_BUY_COUNT,1);

	retCmd.set_result(MiscProto::stBuyAttackTimeResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("购买攻击小怪兽次数成功");
#endif
}

void CMonsterMgr::timer(UINT32 cur)
{
	UINT32 maxCount = CM78Cfg::getSingleton().getMaxCount();
	UINT32 recoveryTime = CM78Cfg::getSingleton().getRecoveryTime();

	if (Athena::global["nftServer"] != "true")
	{
		if(!g_isTheSameDay(m_bossInfo.m_time, cur)){
			if(m_bossInfo.m_attackTime < maxCount){
				m_bossInfo.m_attackTime = maxCount;
				m_bossInfo.m_time = cur;
			}
		}
	/*
		//离线时间
		if(!tag && (m_bossInfo.m_attackTime < maxCount)){
			UINT32 subTime = cur - m_bossInfo.m_time;
			UINT32 recoveryCount = subTime / recoveryTime;
			if(recoveryCount){
				m_bossInfo.m_attackTime += recoveryCount * CM78Cfg::getSingleton().getBuyCount();
				if(m_bossInfo.m_attackTime > maxCount){
					m_bossInfo.m_attackTime = maxCount;
				}
				m_bossInfo.m_time = cur;
			}
			tag = true;
		}
	*/
		if(cur >= m_bossInfo.m_time + recoveryTime){
			if(m_bossInfo.m_attackTime < maxCount){
				UINT32 subTime = cur - m_bossInfo.m_time;
				UINT32 recoveryCount = subTime / recoveryTime;
				if(recoveryCount){
					m_bossInfo.m_attackTime += recoveryCount;
					if(m_bossInfo.m_attackTime > maxCount){
						m_bossInfo.m_attackTime = maxCount;
					}
					m_bossInfo.m_time = cur;
				}
			}
		}
	}
}
