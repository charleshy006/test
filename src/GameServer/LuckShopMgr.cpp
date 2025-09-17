#include "LuckShopMgr.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "GameServer.h"
#include "LuckShopCfg.h"
#include "GameUser.h"
#include "ItemBoxCfg.h"

extern GameServer * g_gameServer;

UINT32 CLuckShopMgr::save(byte * buf)
{
	UINT32 ret = 0;
	//保存版本
	*(UINT32 *)(buf + ret) = s_SAVE_VERSION_INFO;
	ret += sizeof(UINT32);
	
	//当前奖励的vip等级
	*(UINT16 *)(buf + ret) = m_curRewardVipLv;
	ret += sizeof(UINT16);

	//奖励的条目ID
	*(UINT16 *)(buf + ret) = m_rewardID;
	ret += sizeof(UINT16);

	//是否已经获得奖励
	*(UINT8 *)(buf + ret) = m_hasGotReward;
	ret += sizeof(UINT8);
	
	//上次刷新时间
	*(UINT32 *)(buf + ret) = m_lastRefreshTime;
	ret += sizeof(UINT32);

	//今天充值多少
	*(UINT32 *)(buf + ret) = m_todayHasRecharge;
	ret += sizeof(UINT32);
	//打开窗口时间
	*(UINT32 *)(buf + ret) = m_openWinTime;
	ret += sizeof(UINT32);

	//满足条件时间
	*(UINT32 *)(buf + ret) = m_meetCondTime;
	ret += sizeof(UINT32);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("CLuckShopMgr::save %d",ret);
#endif
	return ret;
}

UINT32 CLuckShopMgr::load(byte * buf)
{
	UINT32 ret = 0;
	//读版本
	UINT32 version = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	if (version == s_SAVE_VERSION_INFO){
		//当前奖励的vip等级
		m_curRewardVipLv = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);

		//奖励的条目ID
		m_rewardID = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);

		//是否已经获得奖励
		m_hasGotReward = *(UINT8 *)(buf + ret);
		ret += sizeof(UINT8);

		//上次刷新时间
		m_lastRefreshTime = *(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
		
		//今天充值了多少
		m_todayHasRecharge = *(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);

		//打开窗口时间
		m_openWinTime = *(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
		
		//满足条件时间
		m_meetCondTime = *(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
	}
	else {
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("CLuckShopMgr::load %d",ret);
#endif
	return ret;
}

UINT32 CLuckShopMgr::getSaveSize()
{
	UINT32 ret = 0;
	ret = sizeof(UINT32) 
		  + sizeof(m_curRewardVipLv) 
		  + sizeof(m_rewardID) 
		  + sizeof(m_hasGotReward) 
		  + sizeof(m_lastRefreshTime)
		  + sizeof(m_todayHasRecharge)
		  + sizeof(m_openWinTime)
		  + sizeof(m_meetCondTime);
	return ret;
}

UINT32 CLuckShopMgr::curTime()
{
	return g_gameServer->getSec();
}

void CLuckShopMgr::doRefreshData()
{
	m_curRewardVipLv = 0;
	m_rewardID = 0;
	m_hasGotReward = 0;
	m_openWinTime = 0;				//打开窗口时间
	m_meetCondTime = 0;				//满足条件时间
	m_todayHasRecharge = 0;
}

CLuckShopMgr::CLuckShopMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_curRewardVipLv = 0;			//当前奖励的vip等级
	m_rewardID = 0;					//奖励的条目ID
	m_hasGotReward = 0;				//是否已经获得奖励
	m_todayHasRecharge = 0;			//今天已经充值了多少
	m_openWinTime = 0;				//打开窗口时间
	m_meetCondTime = 0;				//满足条件时间
}

CLuckShopMgr::~CLuckShopMgr()
{

}

void CLuckShopMgr::userOnline()
{
	checkDataRefresh();

	MiscProto::stLuchShopInfo retCmd;
	retCmd.set_currewardviplv(m_curRewardVipLv);
	retCmd.set_randrewardentryid(m_rewardID);
	retCmd.set_hasgotreward(m_hasGotReward ? true : false);
	if (g_gameServer->isAppVersion()){
		retCmd.set_hasgotreward(true);
	}
	retCmd.set_todayhasrecharge(m_todayHasRecharge);
	retCmd.set_openwindowtime(m_openWinTime);
	retCmd.set_rechargemeetcondtime(m_meetCondTime);
	retCmd.set_opensvrday(g_gameServer->getOpenServerDay());
	m_pOwner->sendProto2Me(retCmd);
}

void CLuckShopMgr::onRecharge(UINT32 num)
{
	checkDataRefresh();
	
	if (m_rewardID <= 0){//还没有刷出来
		return ;
	}

	UINT32 oldTodayRecharge = m_todayHasRecharge;
	m_todayHasRecharge += num;

	MiscProto::stSynLuckShopTodayRecharge retCmd;
	retCmd.set_num(m_todayHasRecharge);
	m_pOwner->sendProto2Me(retCmd);

	LuckShopCfg::stRewardItem  *pReward =  LuckShopCfg::getSingleton().getRewardItem(m_curRewardVipLv,m_rewardID);
	if (pReward && (oldTodayRecharge < pReward->m_needRecharge) && (m_todayHasRecharge >= pReward->m_needRecharge)){
		MiscProto::stLuckShopRechargeMeetCondTime retCmd1;
		m_meetCondTime = g_gameServer->getSec();
		retCmd1.set_meetcondtime(m_meetCondTime);
		m_pOwner->sendProto2Me(retCmd1);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("幸运商店的充值数量:%d",oldTodayRecharge);
#endif
}

void CLuckShopMgr::doReward()
{
	MiscProto::stLuckShopRewardResult retCmd;

	if (m_hasGotReward){
		retCmd.set_result(MiscProto::stLuckShopRewardResult::eHas_Got_Reward);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	
	LuckShopCfg::stRewardItem  *pReward =  LuckShopCfg::getSingleton().getRewardItem(m_curRewardVipLv,m_rewardID);
	if (!pReward){
		retCmd.set_result(MiscProto::stLuckShopRewardResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		Athena::logger->error("玩家(id:%d,name:%s)领取幸运商店奖励时候找不到配置",m_pOwner->getID(),m_pOwner->getName());
		return ;
	}
	
	ItemBoxCfg::stItemBox * pItemBox = ItemBoxCfg::getSingleton().getItemBox(pReward->m_boxID);
	if (!pItemBox){
		retCmd.set_result(MiscProto::stLuckShopRewardResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		Athena::logger->error("玩家(id:%d,name:%s)领取幸运商店奖励时候找不到box",m_pOwner->getID(),m_pOwner->getName());
		return ;	
	}
	
	if (m_pOwner->m_boxMgr.needSlotNum(pReward->m_boxID) > m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1)){
		retCmd.set_result(MiscProto::stLuckShopRewardResult::eBag_Full);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	
	if (m_todayHasRecharge < pReward->m_needRecharge){
		retCmd.set_result(MiscProto::stLuckShopRewardResult::eRecharge_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
		return ;	
	}

	m_hasGotReward = 1;
	m_pOwner->m_boxMgr.openBox(pReward->m_boxID);
	retCmd.set_result(MiscProto::stLuckShopRewardResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
}

void CLuckShopMgr::setOpenWinTime()
{
	checkDataRefresh();
	
	if (m_rewardID > 0){//已经刷新了
		return ;
	}

	m_openWinTime = g_gameServer->getSec();
	m_curRewardVipLv = m_pOwner->getVipLv();
	m_rewardID = LuckShopCfg::getSingleton().randVipRewardID(m_pOwner->getVipLv());

	MiscProto::stLuckShopRefreshResult retCmd;
	retCmd.set_currefreshviplv(m_curRewardVipLv);
	retCmd.set_entryid(m_rewardID);
	m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("设置了打开窗口时间");
#endif
}

