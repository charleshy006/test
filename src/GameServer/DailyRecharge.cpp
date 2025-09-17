#include "DailyRecharge.h"
#include "GameUser.h"         
#include "Log4cxxHelp.h"      
#include "KVPair.h"           
#include <tinyxml/tinyxml.h>

CDailyRecharge::CDailyRecharge()
{}

CDailyRecharge::~CDailyRecharge()
{}

bool CDailyRecharge::loadCfg()
{
	TiXmlDocument doc;                                                         
	std::string fileName = Athena::global["confDir"] + "/dailyrecharge.xml";
	if (!doc.LoadFile(fileName.c_str())){                                      
		Athena::logger->error("[每日充值奖励]打开配置文件%s错误",fileName.c_str());
		return false;                                                          
	}                                                                          

	TiXmlElement * pActivityNode = doc.FirstChildElement("activity");                
	if (!pActivityNode){                                                               
		Athena::logger->error("[每日充值奖励]配置加载错误,找不到activity节点!");       
		return false;                                                          
	}                                                                          

	INT32 iVal = 0;                                                            
	const char * szVal = NULL;                                                 
	TiXmlElement * pEntryNode = pActivityNode->FirstChildElement("entry");       
	while (pEntryNode){
		
		stActivity info;
		
		if (pEntryNode->Attribute("id",&iVal)){ 
			info.m_id = iVal;                      
		}
		
		if (pEntryNode->Attribute("RechargeDiamondNum",&iVal)){
		    info.m_rechargeDiamondNum = iVal;                  
		}

		if ((szVal = pEntryNode->Attribute("StartTime"))){ 
			struct tm _tm;                                    
			if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){    
				info.m_startTime = mktime(&_tm) ;             
			}                                                 
			else {                                            
				return false;                                 
			}                                                 
		}                                                     

		if ((szVal = pEntryNode->Attribute("EndTime"))){   
			struct tm _tm;                                    
			if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){    
				info.m_endTime = mktime(&_tm) ;               
			}                                                 
			else {                                            
				return false;                                 
			}                                                 
		}

		if (pEntryNode->Attribute("RewardItemID1",&iVal)){
		    info.m_rewardItemID1 = iVal;
		}

		if (pEntryNode->Attribute("RewardItemNum1",&iVal)){
			info.m_rewardItemNum1 = iVal;
		}

		if (pEntryNode->Attribute("RewardItemID2",&iVal)){
		    info.m_rewardItemID2 = iVal;
		}

		if (pEntryNode->Attribute("RewardItemNum2",&iVal)){
			info.m_rewardItemNum2 = iVal;
		}

		if (pEntryNode->Attribute("RewardItemID3",&iVal)){
		    info.m_rewardItemID3 = iVal;
		}

		if (pEntryNode->Attribute("RewardItemNum3",&iVal)){
			info.m_rewardItemNum3 = iVal;
		}

		if (pEntryNode->Attribute("BagNum",&iVal)){ 
			info.m_needBagNum = iVal;
		}

		m_activities.insert(std::make_pair(info.m_id, info));

		pEntryNode = pEntryNode->NextSiblingElement("entry");
	}

#ifdef _HDZ_DEBUG                                                                                  
	Athena::logger->trace("加载了每日充值奖励的配置:");

	std::map<UINT32,stActivity>::iterator it = m_activities.begin();

	for (;it != m_activities.end();++it){
		stActivity & activity = it->second;
		Athena::logger->trace("活动ID:%d,充值数量:%d,开始时间:%s,结束时间:%s,ItemID1:%d, ItemNum1:%d, ItemID2:%d, ItemNum2:%d, ItemID3:%d, ItemNum3:%d, needBagNum:%d",activity.m_id,activity.m_rechargeDiamondNum, g_utc2StringDate(activity.m_startTime,"%Y-%m-%d %H:%M:%S").c_str(),g_utc2StringDate(activity.m_endTime,"%Y-%m-%d %H:%M:%S").c_str(), activity.m_rewardItemID1, activity.m_rewardItemNum1, activity.m_rewardItemID2, activity.m_rewardItemNum2, activity.m_rewardItemID3, activity.m_rewardItemNum3, activity.m_needBagNum);
	}
#endif
	return true;
}

stActivity * CDailyRecharge::getRewardInfo()
{
	std::map<UINT32,stActivity>::iterator it = m_activities.begin();
	for (;it != m_activities.end();++it){
		stActivity & activity = it->second;
		if ((g_gameServer->getSec() >= activity.m_startTime) && (g_gameServer->getSec() <= activity.m_endTime)){
			return &it->second;
		}
	}
	return NULL;
}

void CDailyRecharge::receiveReward(GameUser * pUser)
{
	if(!pUser){
		return;
	}

	MiscProto::stReceiveRewardResult retCmd;
	
	if(pUser->m_countMgr.getDayRefreshValue(ECOUNT_REWARD_RECEIVE_NUM) >= 1){
		retCmd.set_result(MiscProto::stReceiveRewardResult::eHas_Got_Reward);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		 Athena::logger->error("[领取每日首冲奖励](name:%s, charID:%d)已经领取了奖励,领取失败!", pUser->getName(), pUser->getID());
#endif
		return;
	}
	
	stActivity * activity = getRewardInfo();
	if(!activity)
	{
		retCmd.set_result(MiscProto::stReceiveRewardResult::eIs_Not_Open);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[领取每日首冲奖励](name:%s, charID:%d)活动暂未开放,领取失败!",pUser->getName(), pUser->getID());
#endif
		return;
	}

	if(pUser->m_countMgr.getDayRefreshValue(ECOUT_TODAY_RECHARGE) < activity->m_rechargeDiamondNum){
		retCmd.set_result(MiscProto::stReceiveRewardResult::eRecharge_Not_Enough);                 
		pUser->sendProto2Me(retCmd);                                                               
#ifdef _HDZ_DEBUG                                                                                  
		Athena::logger->error("[领取每日首冲奖励](name:%s, charID:%d)充值不足,领取失败!",pUser->getName(), pUser->getID());
#endif                                                                                             
		return;                                                                                    
	}

	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < activity->m_needBagNum){
		retCmd.set_result(MiscProto::stReceiveRewardResult::eBag_Full);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[领取每日首冲奖励](name:%s, charID:%d)包裹空格不足,领取失败!", pUser->getName(), pUser->getID());
#endif
		return;
	}

	retCmd.set_result(MiscProto::stReceiveRewardResult::eSuccess);

	Role::stObtainItemTip itemTipCmd; 
	Role::stSynTipItem * pSyn = NULL;

	pUser->giveAssert(activity->m_rewardItemID1,activity->m_rewardItemNum1, 0,"领取每日首冲奖励");
	pSyn = itemTipCmd.add_getitems();
	if(pSyn){
		pSyn->set_itemid(activity->m_rewardItemID1);
		pSyn->set_num(activity->m_rewardItemNum1);
	}

	pUser->giveAssert(activity->m_rewardItemID2,activity->m_rewardItemNum2, 0, "领取每日首冲奖励");
	pSyn = itemTipCmd.add_getitems();
	if(pSyn){                                       
		pSyn->set_itemid(activity->m_rewardItemID2); 
		pSyn->set_num(activity->m_rewardItemNum2);   
	}

	pUser->giveAssert(activity->m_rewardItemID3,activity->m_rewardItemNum3, 0,"领取每日首冲奖励");
	pSyn = itemTipCmd.add_getitems();               
	if(pSyn){                                       
		pSyn->set_itemid(activity->m_rewardItemID3); 
		pSyn->set_num(activity->m_rewardItemNum3);   
	} 
	
	pUser->m_countMgr.addDayRefreshValue(ECOUNT_REWARD_RECEIVE_NUM,1);

	pUser->sendProto2Me(itemTipCmd);
	pUser->sendProto2Me(retCmd);
}
