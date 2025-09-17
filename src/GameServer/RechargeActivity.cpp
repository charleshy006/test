#include "RMBShop.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "GameUser.h"
#include "RechargeActivityCfg.h"

UINT32 CRechargeActivityMgr::save(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	//保存版本
	*(UINT32 *)(buf + ret) = s_SAVE_VERSION_INFO;
	ret += sizeof(UINT32);
	//保存活动数量
	*(UINT16 *)(buf + ret) = m_activities.size();
	ret += sizeof(UINT16);
	
	std::map<UINT16,stRechargeActivityDB >::iterator it = m_activities.begin();
	for (;it != m_activities.end();++it){
		//活动ID
		*(UINT16 *)(buf + ret) = it->first;
		ret += sizeof(UINT16);
		
		//每个充值活动内容
		stRechargeActivityDB & activityDB = it->second;
		//充值数量
		*(UINT32 *)(buf + ret) = activityDB.m_rechargeNum;
		ret += sizeof(UINT32);
		//刷新时间
		*(UINT32 *)(buf + ret) = activityDB.m_refreshTime;
		ret += sizeof(UINT32);
		//已经领取数量
		*(UINT8*)(buf + ret) = activityDB.m_rewardSteps.size();
		ret += sizeof(UINT8);
		
		std::set<UINT8>::iterator subIt = activityDB.m_rewardSteps.begin();
		for (;subIt != it->second.m_rewardSteps.end();++subIt){
			*(UINT8 *)(buf + ret) = *subIt;
			ret += sizeof(UINT8);
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("(name:%s,%d)上线保存了充值活动相关数据:%d",m_pOwner->getName(),m_pOwner->getID(),ret);
#endif
	return ret;
}

UINT32 CRechargeActivityMgr::load(byte * buf)
{
	UINT32 ret = 0;
	//版本号
	UINT32 version = *(UINT32*)(buf + ret);
	ret += sizeof(UINT32);
	if (s_SAVE_VERSION_INFO == version){
		//活动数量
		UINT16 activityNum = *(UINT16 *)(buf + ret);	
		ret += sizeof(UINT16);

		for (UINT16 i = 0;i < activityNum;++i){
			//活动ID
			UINT16 activityID = *(UINT16 *)(buf + ret);
			ret += sizeof(UINT16);
			
			stRechargeActivityDB activityDB;
			//充值数量
			activityDB.m_rechargeNum = *(UINT32 *)(buf + ret);
			ret += sizeof(UINT32);
			//刷新时间
			activityDB.m_refreshTime = *(UINT32 *)(buf + ret);
			ret += sizeof(UINT32);
			//领取的阶段数量
			UINT8 stepNum = *(UINT8 *)(buf + ret);
			ret += sizeof(UINT8);
			for (UINT8 i = 0;i < stepNum;++i){
				UINT8 stepID = *(UINT8 *)(buf + ret);
				ret += sizeof(UINT8);
				activityDB.m_rewardSteps.insert(stepID);
			}
			m_activities[activityID] = activityDB;
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("(name:%s,id:%d)上线加载充值奖励数据:%d",m_pOwner->getName(),m_pOwner->getID(),ret);
#endif
	return ret;
}

CRechargeActivityMgr::CRechargeActivityMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
}

CRechargeActivityMgr::~CRechargeActivityMgr()
{

}

void CRechargeActivityMgr::onRecharge(UINT32 rechargeNum)
{
	refreshData();
	std::map<UINT32,RechargeActivityCfg::stActivityInfo> activities = RechargeActivityCfg::getSingleton().getAllActivities();
	std::map<UINT32,RechargeActivityCfg::stActivityInfo>::iterator it = activities.begin();
	
	MiscProto::stNotifyRechargeActivityInfo retCmd;

	for (;it != activities.end();++it){
		RechargeActivityCfg::stActivityInfo & info = it->second;
		if ((g_gameServer->getSec() > info.m_beginTime) && (g_gameServer->getSec() < info.m_endTime)){//在开启时间内
			
			MiscProto::stRechargeActivityInfoItem *pSyn = retCmd.add_items();
			pSyn->set_id(it->first);

			std::map<UINT16,stRechargeActivityDB >::iterator subIt = m_activities.find(it->first);
			if (subIt != m_activities.end()){
				subIt->second.m_rechargeNum += rechargeNum;
				pSyn->set_rechargenum(subIt->second.m_rechargeNum);
			}
			else {
				stRechargeActivityDB & actvityDB = m_activities[it->first];
				actvityDB.m_rechargeNum += rechargeNum;
				actvityDB.m_refreshTime = g_gameServer->getSec() - g_getSecOfDay(g_gameServer->getSec());
				pSyn->set_rechargenum(actvityDB.m_rechargeNum);
			}
		}		
	}
	m_pOwner->sendProto2Me(retCmd);
}

void CRechargeActivityMgr::refreshData()
{
	std::vector<UINT16> delID;
	std::map<UINT16,stRechargeActivityDB >::iterator it = m_activities.begin();	
	for (;it != m_activities.end();++it){
		RechargeActivityCfg::stActivityInfo *pCfg = RechargeActivityCfg::getSingleton().getActivity(it->first);
		if (!pCfg){
			delID.push_back(it->first);
			continue;
		}
		
		stRechargeActivityDB & dbInfo = it->second;
		if ((g_gameServer->getSec() > pCfg->m_beginTime) && (g_gameServer->getSec() < pCfg->m_endTime)){
			UINT32 diff = g_gameServer->getSec() > dbInfo.m_refreshTime ? g_gameServer->getSec() - dbInfo.m_refreshTime : 0;
			if (diff > pCfg->m_interval){
				dbInfo.m_refreshTime = g_gameServer->getSec() - g_getSecOfDay(g_gameServer->getSec()); 
				dbInfo.m_rechargeNum = 0;
				dbInfo.m_rewardSteps.clear();
			}
		}
	}

	for (UINT16 i = 0;i < delID.size();++i){
		m_activities.erase(delID[i]);
	}
}

void CRechargeActivityMgr::sendActivityList()
{
	refreshData();
	MiscProto::stRetRechargeActivityList retCmd;
	std::map<UINT32,RechargeActivityCfg::stActivityInfo> activities = RechargeActivityCfg::getSingleton().getAllActivities();
	std::map<UINT32,RechargeActivityCfg::stActivityInfo>::iterator it = activities.begin();
	for (;it != activities.end();++it){
		RechargeActivityCfg::stActivityInfo & info = it->second;
		if ((g_gameServer->getSec() > info.m_beginTime) && (g_gameServer->getSec() < info.m_endTime)){//在开启时间内
			MiscProto::stSynRechargeActivityInfo * pSyn = retCmd.add_infos();
			if (pSyn){
				pSyn->set_id(info.m_id);
				
				std::map<UINT16,stRechargeActivityDB >::iterator  subIt = m_activities.find(info.m_id);
				if (subIt != m_activities.end()){
					pSyn->set_rechargenum(subIt->second.m_rechargeNum);
					std::set<UINT8>::iterator thdIt = subIt->second.m_rewardSteps.begin();
					for (;thdIt != subIt->second.m_rewardSteps.end();++thdIt){
						pSyn->add_gotrewardsteps(*thdIt);	
					}
				}
			}
		}		
	}
	m_pOwner->sendProto2Me(retCmd);
}


void CRechargeActivityMgr::doReward(UINT32 id,UINT8 step)
{
	refreshData();
	MiscProto::stRetRechargeActivityReward retCmd;
	retCmd.set_activityid(id);
	retCmd.set_stepid(step);
	
	RechargeActivityCfg::stActivityInfo *pCfg = RechargeActivityCfg::getSingleton().getActivity(id);
	if (!pCfg){
		retCmd.set_result(MiscProto::stRetRechargeActivityReward::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	
	std::map<UINT8,RechargeActivityCfg::stActivityStepInfo>::iterator stepCfgIt = pCfg->m_steps.find(step);
	if (stepCfgIt == pCfg->m_steps.end()){
		retCmd.set_result(MiscProto::stRetRechargeActivityReward::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	std::map<UINT16,stRechargeActivityDB >::iterator it = m_activities.find(id);	
	if (it != m_activities.end()){
		if (it->second.m_rewardSteps.find(step) != it->second.m_rewardSteps.end()){
			retCmd.set_result(MiscProto::stRetRechargeActivityReward::eHas_Got_Reward);
			m_pOwner->sendProto2Me(retCmd);
			return ;
		}	
	}
	
	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < m_pOwner->m_boxMgr.needSlotNum(stepCfgIt->second.m_boxID)){
		retCmd.set_result(MiscProto::stRetRechargeActivityReward::eBagFull);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	
	if (it->second.m_rechargeNum < stepCfgIt->second.m_rechargeNum){
		retCmd.set_result(MiscProto::stRetRechargeActivityReward::eCond_Not_Meet);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}		
	
	m_pOwner->m_boxMgr.openBox(stepCfgIt->second.m_boxID);
	it->second.m_rewardSteps.insert(step);
	retCmd.set_result(MiscProto::stRetRechargeActivityReward::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
}
