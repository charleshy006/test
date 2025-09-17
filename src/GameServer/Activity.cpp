#include "Activity.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "GameServer.h"
#include "GameUser.h"
#include "Misc.pb.h"

extern GameServer * g_gameServer;

ActivityStep::ActivityStep(Activity * pActivity) : m_pActivity(pActivity)
{
	m_id = 0;
}

ActivityStep::~ActivityStep()
{

}

bool ActivityStep::init(UINT32 id,std::string desc)
{
	m_id = id;
	m_desc = desc;
	return true;
}

bool ActivityStep::addRewardItem(std::string & itemStr,std::string & resStr,UINT16 minLv,UINT16 maxLv,UINT16 minVipLv,UINT16 maxVipLv)
{
#ifdef _HDZ_DEBUG
	//Athena::logger->trace("活动阶段的奖励道具:%s,奖励资源:%s",itemStr.c_str(),resStr.c_str());
#endif
	stActivityStepOptionalReward reward;
	std::vector<std::string> strVec;
	reward.m_minLv = minLv;
	reward.m_maxLv = maxLv;
	reward.m_minVipLv = minVipLv;
	reward.m_maxVipLv = maxVipLv;

	g_stringtok(strVec,itemStr,";");
	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string >	 subStrVec;
		g_stringtok(subStrVec,strVec[i],":");
		if (subStrVec.size() >= 2){
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			reward.m_rewardItems.push_back(std::make_pair(itemID,num));
		}
	}
	
	strVec.clear();
	g_stringtok(strVec,resStr,";");
	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string >	 subStrVec;
		g_stringtok(subStrVec,strVec[i],":");
		if (subStrVec.size() >= 2){
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			reward.m_rewardItems.push_back(std::make_pair(itemID,num));
		}
	}

	if (reward.m_rewardItems.size() > 0){	
		m_rewards.push_back(reward);
	}
	return true;
}

void ActivityStep::print()
{
	Athena::logger->trace("活动的阶段:%d,描述:%s",m_id,m_desc.c_str());
	for (UINT16 i = 0;i < m_rewards.size();++i){
		stActivityStepOptionalReward & reward = m_rewards[i];
		Athena::logger->trace("奖励条件:minLv=%d,maxLv=%d,minVipLv=%d,maxVipLv=%d",reward.m_minLv,reward.m_maxLv,reward.m_minVipLv,reward.m_maxVipLv);

		for (UINT16 j = 0; j < reward.m_rewardItems.size(); ++j){
			Athena::logger->trace("奖励道具:itemID:%d,num:%d",reward.m_rewardItems[j].first,reward.m_rewardItems[j].second);
		}
	}
}

void ActivityStep::doReward(GameUser * pUser,UINT16 selIndex)
{
	if (!pUser || !m_pActivity){
		return ;
	}

	MiscProto::stActivityRewardResult retCmd;
	retCmd.set_id(m_pActivity->getID());
	retCmd.set_stepid(m_id);
	
	if (!isCondMeet(pUser->m_activityDataMgr.getActivityStepVal(m_pActivity->getActivityType(),m_pActivity->getID()))){
		retCmd.set_result(MiscProto::stActivityRewardResult::eCond_Not_Meet);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	if (pUser->m_activityDataMgr.isStepFinish(m_pActivity->getID(),m_id)){
		retCmd.set_result(MiscProto::stActivityRewardResult::eHas_Got_Reward);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	if (selIndex >= m_rewards.size()){
		retCmd.set_result(MiscProto::stActivityRewardResult::eSelect_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	stActivityStepOptionalReward & reward = m_rewards[selIndex];
	if (pUser->m_bagMgr.getMainFreeSlotNum() < reward.m_rewardItems.size()){
		retCmd.set_result(MiscProto::stActivityRewardResult::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return ;
	}	
	
	Role::stObtainItemTip tipCmd;
	for (UINT16 i = 0;i < reward.m_rewardItems.size();++i){
		std::pair<UINT32,UINT32> & itemPair = reward.m_rewardItems[i];
		pUser->giveAssert(itemPair.first,itemPair.second,0,"活动奖励");
		Role::stSynTipItem * pSyn = tipCmd.add_getitems();
		if (pSyn){
			pSyn->set_itemid(itemPair.first);
			pSyn->set_num(itemPair.second);
		}
	}
	pUser->sendProto2Me(tipCmd);	
	pUser->m_activityDataMgr.addStepID(m_pActivity->getID(),m_id);
	retCmd.set_result(MiscProto::stActivityRewardResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	Game::stActivityLog webCmd;
	webCmd.set_charid(pUser->getID());
	webCmd.set_activityid(m_pActivity->getID());
	webCmd.set_activityname(m_pActivity->getTitle());
	webCmd.set_stepid(getID());
	webCmd.set_desc(getDesc());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void ActivityStep::fillActivityStepInfo(MiscProto::stSynActivityStepInfo & pSyn)
{
	pSyn.set_stepid(m_id);
	pSyn.set_stepdesc(m_desc);

	for (UINT16 i = 0;i < m_rewards.size();++i){
		stActivityStepOptionalReward & optional = m_rewards[i];
		MiscProto::stSynActivityStepOptionalReward * pSynOptional = pSyn.add_optionals();
		if (pSynOptional){
			for (UINT16 j = 0;j < optional.m_rewardItems.size();++j){
				std::pair<UINT32,UINT32> & reward = optional.m_rewardItems[j];
				MiscProto::stSynActivityStepReward * pSynReward = pSynOptional->add_rewards();
				if (pSynReward){
					pSynReward->set_itemid(reward.first);
					pSynReward->set_num(reward.second);
				}
			}	
		}
	}
}

ConsumeActivityStep::ConsumeActivityStep(Activity * pActivity):ActivityStep(pActivity)
{
	
}

ConsumeActivityStep::~ConsumeActivityStep()
{

}

bool ConsumeActivityStep::isCondMeet(UINT32 curStepVal)
{
	return curStepVal >= m_id;	
}

WeeklyConsumeActivityStep::WeeklyConsumeActivityStep(Activity * pActivity) : ActivityStep(pActivity)
{

}

WeeklyConsumeActivityStep::~WeeklyConsumeActivityStep()
{

}

bool WeeklyConsumeActivityStep::isCondMeet(UINT32 curStepVal)
{
	return curStepVal >= m_id;	
}

CumuLoginActivityStep::CumuLoginActivityStep(Activity * pActivity) : ActivityStep(pActivity)
{

}

CumuLoginActivityStep::~CumuLoginActivityStep()
{
	
}

SpecialTimeLoginActivityStep::SpecialTimeLoginActivityStep(Activity * pActivity) : ActivityStep(pActivity)
{

}

SpecialTimeLoginActivityStep::~SpecialTimeLoginActivityStep()
{

}

bool SpecialTimeLoginActivityStep::isCondMeet(UINT32 curStepVal)
{
	return curStepVal >= m_id;	
}

bool CumuLoginActivityStep::isCondMeet(UINT32 curStepVal)
{
	return curStepVal >= m_id;
}

ExchangeActivityStep::ExchangeActivityStep(Activity * pActivity) : ActivityStep(pActivity)
{

}

ExchangeActivityStep::~ExchangeActivityStep()
{

}

void ExchangeActivityStep::doReward(GameUser * pUser,UINT16 selIndex)
{
	if (!m_pActivity || !pUser){
		return ;
	}

	MiscProto::stExchangeItemResult retCmd;
	retCmd.set_activityid(m_pActivity->getID());
	retCmd.set_stepid(m_id);
	
	if (!m_pActivity->isActive()){
		retCmd.set_result(MiscProto::stExchangeItemResult::eActivity_Finish);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	if (pUser->m_bagMgr.getMainFreeSlotNum() < m_gotItems.size()){
		retCmd.set_result(MiscProto::stExchangeItemResult::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return ;
	}	
	
	if (pUser->m_activityDataMgr.getStepDoTime(m_pActivity->getID(),m_id) >= m_maxExchangeNum){
		retCmd.set_result(MiscProto::stExchangeItemResult::eTime_Use_Out);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	for (UINT16 i= 0;i < m_needItems.size();++i){
        Athena::logger->trace("[兑换活动checkAssert]id is %d, num is %d", 
                m_needItems[i].first, pUser->m_bagMgr.getPieceNumByID(m_needItems[i].first));
		if (!pUser->checkAssert(m_needItems[i].first, m_needItems[i].second)) {
            if (pUser->m_bagMgr.getPieceNumByID(m_needItems[i].first) < m_needItems[i].second) {
                retCmd.set_result(MiscProto::stExchangeItemResult::eHas_Not_Enough_Item);
                pUser->sendProto2Me(retCmd);
                return ;
            }
        }
	}
	
	for (UINT16 i= 0;i < m_needItems.size();++i){
        Athena::logger->trace("[兑换活动removeAssert]id is %d, num is %d", 
                m_needItems[i].first, pUser->m_bagMgr.getPieceNumByID(m_needItems[i].first));
		if (!pUser->removeAssert(m_needItems[i].first, m_needItems[i].second,0,"兑换")) {
            if (!pUser->m_bagMgr.removeItemByItemID(m_needItems[i].first, m_needItems[i].second, "兑换")){
                retCmd.set_result(MiscProto::stExchangeItemResult::eHas_Not_Enough_Item);
                pUser->sendProto2Me(retCmd);
                return ;
            }
		}	
	}
	
	pUser->m_activityDataMgr.addStepDoTime(m_pActivity->getID(),m_id);

	Role::stObtainItemTip tipCmd;
	for (UINT16 i = 0;i < m_gotItems.size();++i){
		std::pair<UINT32,UINT32> & itemPair = m_gotItems[i];
		pUser->giveAssert(itemPair.first,itemPair.second,0,"兑换活动");
		Role::stSynTipItem * pSyn = tipCmd.add_getitems();
		if (pSyn){
			pSyn->set_itemid(itemPair.first);
			pSyn->set_num(itemPair.second);
		}
	}
	pUser->sendProto2Me(tipCmd);	
	retCmd.set_result(MiscProto::stExchangeItemResult::eSuccess);
	pUser->sendProto2Me(retCmd);
	
	Game::stActivityLog webCmd;
	webCmd.set_charid(pUser->getID());
	webCmd.set_activityid(m_pActivity->getID());
	webCmd.set_activityname(m_pActivity->getTitle());
	webCmd.set_stepid(getID());
	webCmd.set_desc(getDesc());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void ExchangeActivityStep::print()
{
	ActivityStep::print();
	
	Athena::logger->trace("最大兑换次数:%d",m_maxExchangeNum);
	for (UINT16 i = 0;i < m_needItems.size();++i){
		Athena::logger->trace("兑换需要的道具:itemID:%d,itemNum:%d",m_needItems[i].first,m_needItems[i].second);
	}

	for (UINT16 i = 0;i < m_gotItems.size();++i){
		Athena::logger->trace("兑换获得道具:itemID:%d,itemNum:%d",m_gotItems[i].first,m_gotItems[i].second);
	}
}

bool ExchangeActivityStep::isCondMeet(UINT32 curStepVal)
{
	return curStepVal < m_maxExchangeNum;
}

bool ExchangeActivityStep::addNeedItems(std::string & itemStr,std::string & resStr)
{
	std::vector<std::string> strVec;

	g_stringtok(strVec,itemStr,";");
	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string >	 subStrVec;
		g_stringtok(subStrVec,strVec[i],":");
		if (subStrVec.size() >= 2){
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			m_needItems.push_back(std::make_pair(itemID,num));
		}
	}
	
	strVec.clear();
	g_stringtok(strVec,resStr,";");
	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string >	 subStrVec;
		g_stringtok(subStrVec,strVec[i],":");
		if (subStrVec.size() >= 2){
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			m_needItems.push_back(std::make_pair(itemID,num));
		}
	}
	return true;
}

void ExchangeActivityStep::fillStepInfo(MiscProto::stSynExchangeActivityiStep * pSyn)
{
	pSyn->set_maxexchgnum(m_maxExchangeNum);

	std::vector<std::pair<UINT32,UINT32> >::iterator it = m_needItems.begin();
	for (;it != m_needItems.end();++it){
		MiscProto::stExchangeItem * pSubSyn = pSyn->add_needitems();
		if (pSubSyn){
			pSubSyn->set_itemid(it->first);
			pSubSyn->set_num(it->second);
		}
	}

	it = m_gotItems.begin();
	for (;it != m_gotItems.end();++it){
		MiscProto::stExchangeItem * pSubSyn = pSyn->add_gotitems();
		if (pSubSyn){
			pSubSyn->set_itemid(it->first);
			pSubSyn->set_num(it->second);
		}
	}

	pSyn->set_stepid(m_id);
	pSyn->set_desc(m_desc);
}

bool ExchangeActivityStep::addGotItems(std::string & itemStr,std::string & resStr)
{
	std::vector<std::string> strVec;

	g_stringtok(strVec,itemStr,";");
	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string >	 subStrVec;
		g_stringtok(subStrVec,strVec[i],":");
		if (subStrVec.size() >= 2){
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			m_gotItems.push_back(std::make_pair(itemID,num));
		}
	}
	
	strVec.clear();
	g_stringtok(strVec,resStr,";");
	for (UINT16 i = 0;i < strVec.size();++i){
		std::vector<std::string >	 subStrVec;
		g_stringtok(subStrVec,strVec[i],":");
		if (subStrVec.size() >= 2){
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			m_gotItems.push_back(std::make_pair(itemID,num));
		}
	}
	return true;
}

OneDayOneRechargeStep::OneDayOneRechargeStep(Activity * pActivity) : ActivityStep(pActivity)
{

}

OneDayOneRechargeStep::~OneDayOneRechargeStep()
{

}

bool OneDayOneRechargeStep::isCondMeet(UINT32 curStepVal)
{
	return  curStepVal >= m_id;	
}

OneDayAccRechargeStep::OneDayAccRechargeStep(Activity * pActivity) : ActivityStep(pActivity)
{

}

OneDayAccRechargeStep::~OneDayAccRechargeStep()
{

}

bool OneDayAccRechargeStep::isCondMeet(UINT32 curStepVal)
{
	return  curStepVal >= m_id;	
}

OneDayRecycleRechargeStep::OneDayRecycleRechargeStep(Activity * pActivity) : ActivityStep(pActivity)
{
	m_rewardNum = 0;			//奖励次数
}

OneDayRecycleRechargeStep::~OneDayRecycleRechargeStep()
{}

bool OneDayRecycleRechargeStep::isCondMeet(UINT32 curStepVal)
{
	return  curStepVal >= m_id;	
}

void OneDayRecycleRechargeStep::doReward(GameUser * pUser,UINT16 selIndex)
{
	if (!pUser || !m_pActivity){
		return ;
	}	

	MiscProto::stRetRecycleRechargeActReward retCmd;
	retCmd.set_activityid(m_pActivity->getID());
	
	if (!m_pActivity->isActive()){
		retCmd.set_result(MiscProto::stRetRecycleRechargeActReward::eNot_Begin);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	UINT32 rewardNum = pUser->m_activityDataMgr.getActivityExtStepVal(m_pActivity->getActivityType(),m_pActivity->getID());
	if (rewardNum >= m_rewardNum){
		retCmd.set_result(MiscProto::stRetRecycleRechargeActReward::eTime_Use_Out);
		pUser->sendProto2Me(retCmd);
		return;
	}
	
	UINT32 stepVal = pUser->m_activityDataMgr.getActivityStepVal(m_pActivity->getActivityType(),m_pActivity->getID());
	if (stepVal < ((rewardNum + 1) * getID())){
		retCmd.set_result(MiscProto::stRetRecycleRechargeActReward::eRecharge_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}	

	if (selIndex >= m_rewards.size()){
		retCmd.set_result(MiscProto::stRetRecycleRechargeActReward::eSelect_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	stActivityStepOptionalReward & reward = m_rewards[selIndex];
	if (pUser->m_bagMgr.getMainFreeSlotNum() < reward.m_rewardItems.size()){
		retCmd.set_result(MiscProto::stRetRecycleRechargeActReward::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return ;
	}	
	
	pUser->m_activityDataMgr.incActivityExtStepVal(m_pActivity->getActivityType(),m_pActivity->getID());

	Role::stObtainItemTip tipCmd;
	for (UINT16 i = 0;i < reward.m_rewardItems.size();++i){
		std::pair<UINT32,UINT32> & itemPair = reward.m_rewardItems[i];
		pUser->giveAssert(itemPair.first,itemPair.second,0,"每日循环充值活动");
		Role::stSynTipItem * pSyn = tipCmd.add_getitems();
		if (pSyn){
			pSyn->set_itemid(itemPair.first);
			pSyn->set_num(itemPair.second);
		}
	}
	pUser->sendProto2Me(tipCmd);
	retCmd.set_result(MiscProto::stRetRecycleRechargeActReward::eSuccess);
	pUser->sendProto2Me(retCmd);
	
	Game::stActivityLog webCmd;
	webCmd.set_charid(pUser->getID());
	webCmd.set_activityid(m_pActivity->getID());
	webCmd.set_activityname(m_pActivity->getTitle());
	webCmd.set_stepid(getID());
	webCmd.set_desc(getDesc());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void OneDayRecycleRechargeStep::print()
{
	ActivityStep::print();
	Athena::logger->trace("每日循环充值最大奖励次数:%d",m_rewardNum);
}

MutilDayAccRechargeStep::MutilDayAccRechargeStep(Activity * pActivity) : ActivityStep(pActivity)
{
	
}

MutilDayAccRechargeStep::~MutilDayAccRechargeStep()
{
	
}

bool MutilDayAccRechargeStep::isCondMeet(UINT32 curStepVal)
{
	return  curStepVal >= m_id;
}

AccRechargeDayStep::AccRechargeDayStep(Activity * pActivity) : ActivityStep(pActivity)
{
	
}

AccRechargeDayStep::~AccRechargeDayStep()
{

}

bool AccRechargeDayStep::isCondMeet(UINT32 curStepVal)
{
	return  curStepVal >= m_id;
}

bool Activity::isActive()
{
	if ((g_gameServer->getSec() >= m_startTime) && (g_gameServer->getSec() <= m_endTime)){
		return true;
	}	
	else {
		return false;
	}
}

void Activity::doReward(GameUser * pUser,UINT32 step,UINT16 selIndex)
{
	if (!pUser){
		return ;
	}

	std::map<UINT32,ActivityStep *>::iterator it = m_steps.find(step);
	if (it == m_steps.end()){
		return ;
	}
	
	if (!isActive()){
		Athena::logger->error("试图领取一个没有激活的属性:name:%s,id:%d",pUser->getName(),pUser->getID());
		return ;
	}

	it->second->doReward(pUser,selIndex);
}

bool Activity::initActivityTime(const std::string & beginTime,const std::string & endTime)
{
	char szTime[120] = { 0 };
	snprintf(szTime,sizeof(szTime),"%s:00",beginTime.c_str());
	
	{
		struct tm _tm;
		if (strptime(szTime,"%Y-%m-%d %H:%M:%S",&_tm)){
			m_startTime = mktime(&_tm) ;//- 8 * 3600;
		}
		else {
			return false;
		}
	}

	bzero(szTime,sizeof(szTime));
	snprintf(szTime,sizeof(szTime),"%s:00",endTime.c_str());

	{
		struct tm _tm;
		if (strptime(szTime,"%Y-%m-%d %H:%M:%S",&_tm)){
			m_endTime = mktime(&_tm); //- 8 * 3600;
		}
		else {
			return false;
		}
	}
#ifdef _HDZ_DEBUG
	//Athena::logger->trace("活动开始时间跟活动结束时间:beginTime:%d,endTime:%d",m_startTime,m_endTime);
#endif
	return true;
}

bool Activity::init(Json::Value & configJson)
{
	std::map<UINT32,ActivityStep *>::iterator stepIt = m_steps.begin();
	for (;stepIt != m_steps.end();++stepIt){
		SAFE_DELETE(stepIt->second);
	}
	m_steps.clear();

	UINT32 activityID = 0;
	std::string title;

	if (configJson["id"].isString()){
		activityID = (UINT32)atoi(configJson["id"].asString().c_str());
	}
	
	if (!activityID){
		Athena::logger->error("解析活动的id为0,解析失败!");
		return false;
	}
	
	m_id = activityID;

	if (configJson["activityName"].isString()){
		title = configJson["activityName"].asString();
	}
	
	if (configJson["activityDesc"].isString()){
		m_desc = configJson["activityDesc"].asString();
	}

	/*if (configJson["roundID"].isString()){
		m_curRoundID = (UINT32)atoi(configJson["roundID"].asString().c_str());
	}*/

	m_title = title;

	std::string startTime;
	std::string endTime;

	if (configJson["activityTime_start"].isString()){
		startTime = configJson["activityTime_start"].asString();
	}

	if (configJson["activityTime_end"].isString()){
		endTime = configJson["activityTime_end"].asString();
	}
	
	if (!initActivityTime(startTime,endTime)){
		Athena::logger->error("解析活动的时间失败!");
		return false;
	}
	return true ;
}

bool CreateRoleRelActivity::initCreateRoleTime(const std::string & beginTime,const std::string & endTime)
{
	char szTime[120] = { 0 };
	snprintf(szTime,sizeof(szTime),"%s:00",beginTime.c_str());
	
	{
		struct tm _tm;
		if (strptime(szTime,"%Y-%m-%d %H:%M:%S",&_tm)){
			m_createRoleBeginTime = mktime(&_tm) ;//- 8 * 3600;
		}
		else {
			return false;
		}
	}

	bzero(szTime,sizeof(szTime));
	snprintf(szTime,sizeof(szTime),"%s:00",endTime.c_str());

	{
		struct tm _tm;
		if (strptime(szTime,"%Y-%m-%d %H:%M:%S",&_tm)){
			m_createRoleEndTime = mktime(&_tm); //- 8 * 3600;
		}
		else {
			return false;
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("活动创角开始时间跟活动结束时间:beginTime:%d,endTime:%d",m_createRoleBeginTime,m_createRoleEndTime);
#endif
	return true;
}

bool CreateRoleRelActivity::roleCreateTimeValid(UINT32 createTime)
{
	if ((createTime >= m_createRoleBeginTime) && (createTime <= m_createRoleEndTime)){
		return true;
	}	
	else {
		return false;
	}
}

bool CreateRoleRelActivity::init(Json::Value & configJson)
{
	if (!Activity::init(configJson)){
		return false;	
	}	
	
	std::string startTime;
	std::string endTime;

	if (configJson["createRoleTime_start"].isString()){
		startTime = configJson["createRoleTime_start"].asString();
	}

	if (configJson["createRoleTime_end"].isString()){
		endTime = configJson["createRoleTime_end"].asString();
	}
	
	if (!initCreateRoleTime(startTime,endTime)){
		Athena::logger->error("解析活动的创角时间失败!");
		return false;
	}
	return true; 
}

ConsumeActivity::ConsumeActivity()
{

}

ConsumeActivity::~ConsumeActivity()
{

}

bool ConsumeActivity::init(Json::Value & configJson)
{
	if (!Activity::init(configJson)){
		return false;
	}

	if (configJson["stepRewards"].isArray()){
		Json::Value allStep = configJson["stepRewards"]; 	
		Json::Value::iterator it = allStep.begin();
		for (;it != allStep.end();++it){
			Json::Value & step = *it;
			ConsumeActivityStep *pActivityStep = ATHENA_NEW ConsumeActivityStep(this);
			UINT32 id = 0;
			if (step["consumeNum"].isString()){
				id = (UINT32)atoi(step["consumeNum"].asString().c_str());
			}
			
			std::string desc;
			if (step["stepDesc"].isString()){
				desc = step["stepDesc"].asString();
			}

			if (!pActivityStep->init(id,desc)){
				Athena::logger->error("初始化活动阶段奖励失败!");
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			std::string resStr;
			std::string itemStr;
			
			if (step["reward1_goods_list"].isString()){
				resStr = step["reward1_goods_list"].asString();
			}

			if (step["reward1_item_list"].isString()){
				itemStr = step["reward1_item_list"].asString();
			}
			
			if (itemStr.size() > 0 || resStr.size() > 0) {
				if (!pActivityStep->addRewardItem(itemStr,resStr)){
					SAFE_DELETE(pActivityStep);
					return false;
				}
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward2_goods_list"].isString()){
				resStr = step["reward2_goods_list"].asString();
			}

			if (step["reward2_item_list"].isString()){
				itemStr = step["reward2_item_list"].asString();
			}
			
			if (itemStr.size() > 0 || resStr.size() > 0) {
				if (!pActivityStep->addRewardItem(itemStr,resStr)){
					SAFE_DELETE(pActivityStep);
					return false;
				}
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward3_goods_list"].isString()){
				resStr = step["reward3_goods_list"].asString();
			}

			if (step["reward3_item_list"].isString()){
				itemStr = step["reward3_item_list"].asString();
			}
			
			if (itemStr.size() > 0 || resStr.size() > 0) {
				if (!pActivityStep->addRewardItem(itemStr,resStr)){
					SAFE_DELETE(pActivityStep);
					return false;
				}
			}
			m_steps[id] = pActivityStep;	
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif
	return true;	
}

WeeklyConsumeActivity::WeeklyConsumeActivity()
{
	
}

WeeklyConsumeActivity::~WeeklyConsumeActivity()
{
	
}

bool WeeklyConsumeActivity::init(Json::Value & configJson)
{
	if (!Activity::init(configJson)){
		return false;
	}

	if (configJson["stepRewards"].isArray()){
		Json::Value allStep = configJson["stepRewards"]; 	
		Json::Value::iterator it = allStep.begin();
		for (;it != allStep.end();++it){
			Json::Value & step = *it;
			ConsumeActivityStep *pActivityStep = ATHENA_NEW ConsumeActivityStep(this);
			UINT32 id = 0;
			if (step["consumeNum"].isString()){
				id = (UINT32)atoi(step["consumeNum"].asString().c_str());
			}
			
			std::string desc;
			if (step["stepDesc"].isString()){
				desc = step["stepDesc"].asString();
			}

			if (!pActivityStep->init(id,desc)){
				Athena::logger->error("初始化活动阶段奖励失败!");
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			std::string resStr;
			std::string itemStr;
			
			if (step["reward1_goods_list"].isString()){
				resStr = step["reward1_goods_list"].asString();
			}

			if (step["reward1_item_list"].isString()){
				itemStr = step["reward1_item_list"].asString();
			}
			
			if (itemStr.size() > 0 || resStr.size() > 0) {
				if (!pActivityStep->addRewardItem(itemStr,resStr)){
					SAFE_DELETE(pActivityStep);
					return false;
				}
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward2_goods_list"].isString()){
				resStr = step["reward2_goods_list"].asString();
			}

			if (step["reward2_item_list"].isString()){
				itemStr = step["reward2_item_list"].asString();
			}
			
			if (itemStr.size() > 0 || resStr.size() > 0) {
				if (!pActivityStep->addRewardItem(itemStr,resStr)){
					SAFE_DELETE(pActivityStep);
					return false;
				}
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward3_goods_list"].isString()){
				resStr = step["reward3_goods_list"].asString();
			}

			if (step["reward3_item_list"].isString()){
				itemStr = step["reward3_item_list"].asString();
			}
			
			if (itemStr.size() > 0 || resStr.size() > 0) {
				if (!pActivityStep->addRewardItem(itemStr,resStr)){
					SAFE_DELETE(pActivityStep);
					return false;
				}
			}
			m_steps[id] = pActivityStep;	
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif
	return true;	
}

CumuLoginActivity::CumuLoginActivity()
{

}

CumuLoginActivity::~CumuLoginActivity()
{
	
}

bool CumuLoginActivity::init(Json::Value & configJson)
{
	if (!Activity::init(configJson)){
		return false;
	}

	if (configJson["stepRewards"].isArray()){
		Json::Value allStep = configJson["stepRewards"]; 	
		Json::Value::iterator it = allStep.begin();
		for (;it != allStep.end();++it){
			Json::Value & step = *it;
			CumuLoginActivityStep *pActivityStep = ATHENA_NEW CumuLoginActivityStep(this);
			UINT32 id = 0;
			if (step["cumuLoginNum"].isString()){
				id = (UINT32)atoi(step["cumuLoginNum"].asString().c_str());
			}
			
			std::string desc;
			if (step["stepDesc"].isString()){
				desc = step["stepDesc"].asString();
			}

			if (!pActivityStep->init(id,desc)){
				Athena::logger->error("累积登录初始化活动阶段奖励失败!");
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			Json::Value optionalReward = step["optionalReward"];
			Json::Value::iterator subIt = optionalReward.begin();
			for (;subIt != optionalReward.end();++subIt){
				Json::Value & reward = *subIt;
				UINT16 minLv = 0;
				UINT16 maxLv = (UINT16)-1;
				UINT16 minVipLv = 0;
				UINT16 maxVipLv = (UINT16)-1;

				if (reward["minLv"].isString()){
					minLv = (UINT16)atoi(reward["minLv"].asString().c_str());	
				}	

				if (reward["maxLv"].isString()){
					maxLv = (UINT16)atoi(reward["maxLv"].asString().c_str());	
				}	

				if (reward["minVipLv"].isString()){
					minVipLv = (UINT16)atoi(reward["minVipLv"].asString().c_str());	
				}	
				
				if (reward["maxVipLv"].isString()){
					maxVipLv = (UINT16)atoi(reward["maxVipLv"].asString().c_str());	
				}
				
				std::string resStr;
				std::string itemStr;
			
				if (reward["reward_goods_list"].isString()){
					resStr = reward["reward_goods_list"].asString();
				}

				if (reward["reward_item_list"].isString()){
					itemStr = reward["reward_item_list"].asString();
				}
			
				if (!pActivityStep->addRewardItem(itemStr,resStr,minLv,maxLv,minVipLv,maxVipLv)){
					SAFE_DELETE(pActivityStep);
					return false;
				}
			}
			m_steps[id] = pActivityStep;	
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif
	return true;
}

SpecialTimeLoginActivity::SpecialTimeLoginActivity()
{
	
}

SpecialTimeLoginActivity::~SpecialTimeLoginActivity()
{

}

bool SpecialTimeLoginActivity::init(Json::Value & configJson)
{
	if (!Activity::init(configJson)){
		return false;
	}
	
	UINT32 id = 0;
	std::string stepDes;
	
	if (configJson["stepDesc"].isString()){
		stepDes = configJson["stepDesc"].asString();
	}	

	SpecialTimeLoginActivityStep *pActivityStep = ATHENA_NEW SpecialTimeLoginActivityStep(this);
	if (!pActivityStep->init(id,stepDes)){
		Athena::logger->error("限时登录初始化活动阶段奖励失败!");
		SAFE_DELETE(pActivityStep);
		return false;
	}

	if (configJson["rewards"].isArray()){
		Json::Value allReward = configJson["rewards"]; 	
		Json::Value::iterator it = allReward.begin();
		for (;it != allReward.end();++it){
			Json::Value & reward = *it;
			UINT16 minLv = 0;
			UINT16 maxLv = (UINT16)-1;
			UINT16 minVipLv = 0;
			UINT16 maxVipLv = (UINT16)-1;

			if (reward["minLv"].isString()){
				minLv = (UINT16)atoi(reward["minLv"].asString().c_str());	
			}	

			if (reward["maxLv"].isString()){
				maxLv = (UINT16)atoi(reward["maxLv"].asString().c_str());	
			}	

			if (reward["minVipLv"].isString()){
				minVipLv = (UINT16)atoi(reward["minVipLv"].asString().c_str());	
			}	

			if (reward["maxVipLv"].isString()){
				maxVipLv = (UINT16)atoi(reward["maxVipLv"].asString().c_str());	
			}

			std::string resStr;
			std::string itemStr;

			if (reward["reward_goods_list"].isString()){
				resStr = reward["reward_goods_list"].asString();
			}

			if (reward["reward_item_list"].isString()){
				itemStr = reward["reward_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr,minLv,maxLv,minVipLv,maxVipLv)){
				SAFE_DELETE(pActivityStep);
				return false;
			}
		}
		m_steps[id] = pActivityStep;	
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif
	return true;	
}
	
ExchangeActivity::ExchangeActivity()
{
	m_bDayFresh = false;
}

ExchangeActivity::~ExchangeActivity()
{

}

bool ExchangeActivity::init(Json::Value & configJson)
{
	if (!Activity::init(configJson)){
		Athena::logger->error("解析兑换活动基础数据失败!");
		return false;
	}
	
	if (configJson["dailyRefresh"].isString()){
		UINT16 dailyRefresh = (UINT16)atoi(configJson["dailyRefresh"].asString().c_str());	
		if (dailyRefresh){
			m_bDayFresh = true;
		}
	}
	
	if (configJson["exchangeStep"].isArray()){
		Json::Value allReward = configJson["exchangeStep"]; 	
		Json::Value::iterator it = allReward.begin();
		for (;it != allReward.end();++it){
			Json::Value & reward = *it;
			UINT16 stepID = 0;
			UINT16 maxExhchangeNum = 0;
			std::string stepDesc;

			if (reward["stepID"].isString()){
				stepID = (UINT16)atoi(reward["stepID"].asString().c_str());	
			}	
			
			if (reward["exchangeNum"].isString()){
				maxExhchangeNum  = (UINT16)atoi(reward["exchangeNum"].asString().c_str());	
			}	
		
			if (reward["stepDesc"].isString()){
				stepDesc  = reward["stepDesc"].asString();	
			}	


			ExchangeActivityStep * pActivityStep = ATHENA_NEW ExchangeActivityStep(this);
			if (!pActivityStep->init(stepID,stepDesc)){
				Athena::logger->error("解析兑换活动的阶段失败:stepID:%d",stepID);
				return false;
			}
			
			pActivityStep->setMaxExchangeNum(maxExhchangeNum);

			std::string resStr;
			std::string itemStr;

			if (reward["needItems_goods_list"].isString()){
				resStr = reward["needItems_goods_list"].asString();
			}

			if (reward["needItems_item_list"].isString()){
				itemStr = reward["needItems_item_list"].asString();
			}

			if (!pActivityStep->addNeedItems(itemStr,resStr)){
				SAFE_DELETE(pActivityStep);
				Athena::logger->error("解析兑换活动需要道具失败");
				return false;
			}
			
			resStr = "";
			itemStr = "";

			if (reward["gotItems_goods_list"].isString()){
				resStr = reward["gotItems_goods_list"].asString();
			}

			if (reward["gotItems_item_list"].isString()){
				itemStr = reward["gotItems_item_list"].asString();
			}

			if (!pActivityStep->addGotItems(itemStr,resStr)){
				SAFE_DELETE(pActivityStep);
				Athena::logger->error("解析兑换道具获得失败!");
				return false;
			}
			m_steps[stepID] = pActivityStep;
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("兑换活动 id:%d,title:%s,开始时间:%d,结束时间:%d,是否每日刷新:%s",m_id,m_title.c_str(),m_startTime,m_endTime,m_bDayFresh ? "是": "否");

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif
	return true;
}
	
OneDayOneRechargeActivity::OneDayOneRechargeActivity()
{
	
}

OneDayOneRechargeActivity::~OneDayOneRechargeActivity()
{

}

bool OneDayOneRechargeActivity::init(Json::Value & configJson)
{
	if (!CreateRoleRelActivity::init(configJson)){
		return false;
	}

	if (configJson["stepRewards"].isArray()){
		Json::Value allStep = configJson["stepRewards"]; 	
		Json::Value::iterator it = allStep.begin();
		for (;it != allStep.end();++it){
			Json::Value & step = *it;
			OneDayOneRechargeStep *pActivityStep = ATHENA_NEW OneDayOneRechargeStep(this);
			UINT32 id = 0;
			if (step["oneRechargeNum"].isString()){
				id = (UINT32)atoi(step["oneRechargeNum"].asString().c_str());
			}
			
			std::string desc;
			if (step["rechargeDesc"].isString()){
				desc = step["rechargeDesc"].asString();
			}

			if (!pActivityStep->init(id,desc)){
				Athena::logger->error("每日单笔充值初始化活动阶段奖励失败!");
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			std::string resStr;
			std::string itemStr;

			if (step["reward1_goods_list"].isString()){
				resStr = step["reward1_goods_list"].asString();
			}

			if (step["reward1_item_list"].isString()){
				itemStr = step["reward1_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日单笔充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}

			resStr = "";
			itemStr = "";
			if (step["reward2_goods_list"].isString()){
				resStr = step["reward2_goods_list"].asString();
			}

			if (step["reward2_item_list"].isString()){
				itemStr = step["reward2_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日单笔充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward3_goods_list"].isString()){
				resStr = step["reward3_goods_list"].asString();
			}

			if (step["reward3_item_list"].isString()){
				itemStr = step["reward3_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日单笔充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			m_steps[id] = pActivityStep;	
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("单日单笔充值 id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif

	return true;
}

OneDayAccRechargeActivity::OneDayAccRechargeActivity()
{}

OneDayAccRechargeActivity::~OneDayAccRechargeActivity()
{}

bool OneDayAccRechargeActivity::init(Json::Value & configJson)
{
	if (!CreateRoleRelActivity::init(configJson)){
		return false;
	}

	if (configJson["stepRewards"].isArray()){
		Json::Value allStep = configJson["stepRewards"]; 	
		Json::Value::iterator it = allStep.begin();
		for (;it != allStep.end();++it){
			Json::Value & step = *it;
			OneDayAccRechargeStep *pActivityStep = ATHENA_NEW OneDayAccRechargeStep(this);
			UINT32 id = 0;
			if (step["accNum"].isString()){
				id = (UINT32)atoi(step["accNum"].asString().c_str());
			}
			
			std::string desc;
			if (step["accDesc"].isString()){
				desc = step["accDesc"].asString();
			}

			if (!pActivityStep->init(id,desc)){
				Athena::logger->error("每日累计充值初始化活动阶段奖励失败!");
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			std::string resStr;
			std::string itemStr;

			if (step["reward1_goods_list"].isString()){
				resStr = step["reward1_goods_list"].asString();
			}

			if (step["reward1_item_list"].isString()){
				itemStr = step["reward1_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日累计充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}

			resStr = "";
			itemStr = "";
			if (step["reward2_goods_list"].isString()){
				resStr = step["reward2_goods_list"].asString();
			}

			if (step["reward2_item_list"].isString()){
				itemStr = step["reward2_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日累计充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward3_goods_list"].isString()){
				resStr = step["reward3_goods_list"].asString();
			}

			if (step["reward3_item_list"].isString()){
				itemStr = step["reward3_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日累计充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			m_steps[id] = pActivityStep;	
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("单日累计充值 id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif

	return true;
}

OneDayRecyleRechargeActivity::OneDayRecyleRechargeActivity()
{

}

OneDayRecyleRechargeActivity::~OneDayRecyleRechargeActivity()
{

}

bool OneDayRecyleRechargeActivity::init(Json::Value & configJson)
{
	if (!CreateRoleRelActivity::init(configJson)){
		return false;
	}

	if (configJson["stepRewards"].isArray()){
		Json::Value allStep = configJson["stepRewards"]; 	
		Json::Value::iterator it = allStep.begin();
		for (;it != allStep.end();++it){
			Json::Value & step = *it;
			OneDayRecycleRechargeStep *pActivityStep = ATHENA_NEW OneDayRecycleRechargeStep(this);
			UINT32 id = 0;
			UINT16 rewardNum = 0;

			if (step["rechargeNum"].isString()){
				id = (UINT32)atoi(step["rechargeNum"].asString().c_str());
			}
			
			if (step["rewardNum"].isString()){
				rewardNum = (UINT16)atoi(step["rewardNum"].asString().c_str());
			}

			std::string desc;
			if (step["rechargeDesc"].isString()){
				desc = step["rechargeDesc"].asString();
			}
			pActivityStep->setRewardNum(rewardNum);

			if (!pActivityStep->init(id,desc)){
				Athena::logger->error("每日循环充值初始化活动阶段奖励失败!");
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			std::string resStr;
			std::string itemStr;

			if (step["reward1_goods_list"].isString()){
				resStr = step["reward1_goods_list"].asString();
			}

			if (step["reward1_item_list"].isString()){
				itemStr = step["reward1_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日循环充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}

			resStr = "";
			itemStr = "";
			if (step["reward2_goods_list"].isString()){
				resStr = step["reward2_goods_list"].asString();
			}

			if (step["reward2_item_list"].isString()){
				itemStr = step["reward2_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日循环充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward3_goods_list"].isString()){
				resStr = step["reward3_goods_list"].asString();
			}

			if (step["reward3_item_list"].isString()){
				itemStr = step["reward3_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("每日循环充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			m_steps[id] = pActivityStep;	
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("单日循环充值 id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif
	return true;
}

MultiDayRechargeActivity::MultiDayRechargeActivity()
{

}

MultiDayRechargeActivity::~MultiDayRechargeActivity()
{}

bool MultiDayRechargeActivity::init(Json::Value & configJson)
{
	if (!CreateRoleRelActivity::init(configJson)){
		return false;
	}

	if (configJson["stepRewards"].isArray()){
		Json::Value allStep = configJson["stepRewards"]; 	
		Json::Value::iterator it = allStep.begin();
		for (;it != allStep.end();++it){
			Json::Value & step = *it;
			MutilDayAccRechargeStep *pActivityStep = ATHENA_NEW MutilDayAccRechargeStep(this);
			UINT32 id = 0;
			if (step["accNum"].isString()){
				id = (UINT32)atoi(step["accNum"].asString().c_str());
			}
			
			std::string desc;
			if (step["accDesc"].isString()){
				desc = step["accDesc"].asString();
			}

			if (!pActivityStep->init(id,desc)){
				Athena::logger->error("多日累计充值初始化活动阶段奖励失败!");
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			std::string resStr;
			std::string itemStr;

			if (step["reward1_goods_list"].isString()){
				resStr = step["reward1_goods_list"].asString();
			}

			if (step["reward1_item_list"].isString()){
				itemStr = step["reward1_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("多日累计充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}

			resStr = "";
			itemStr = "";
			if (step["reward2_goods_list"].isString()){
				resStr = step["reward2_goods_list"].asString();
			}

			if (step["reward2_item_list"].isString()){
				itemStr = step["reward2_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("多日累计充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward3_goods_list"].isString()){
				resStr = step["reward3_goods_list"].asString();
			}

			if (step["reward3_item_list"].isString()){
				itemStr = step["reward3_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("多日累计充值阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			m_steps[id] = pActivityStep;	
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("多日累计充值 id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif

	return true;

}

AccRechargeDayActivity::AccRechargeDayActivity()
{
	m_rechargeNum = 0;
}

AccRechargeDayActivity::~AccRechargeDayActivity()
{

}

bool AccRechargeDayActivity::init(Json::Value & configJson)
{
	if (!CreateRoleRelActivity::init(configJson)){
		return false;
	}

	if (configJson["stepRewards"].isArray()){
		Json::Value allStep = configJson["stepRewards"]; 	
		Json::Value::iterator it = allStep.begin();
		for (;it != allStep.end();++it){
			Json::Value & step = *it;
			UINT32 rechargeNum = 0;

			AccRechargeDayStep *pActivityStep = ATHENA_NEW AccRechargeDayStep(this);
			UINT32 id = 0;
			if (step["accNum"].isString()){
				id = (UINT32)atoi(step["accNum"].asString().c_str());
			}
			
			std::string desc;
			if (step["accDesc"].isString()){
				desc = step["accDesc"].asString();
			}
			
			if (step["rechargeNum"].isString()){
				rechargeNum = (UINT32)atoi(step["rechargeNum"].asString().c_str());
			}
			
			setRechargeNum(rechargeNum);
			if (!pActivityStep->init(id,desc)){
				Athena::logger->error("累计充值天数初始化活动阶段奖励失败!");
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			std::string resStr;
			std::string itemStr;

			if (step["reward1_goods_list"].isString()){
				resStr = step["reward1_goods_list"].asString();
			}

			if (step["reward1_item_list"].isString()){
				itemStr = step["reward1_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("累计充值天数阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}

			resStr = "";
			itemStr = "";
			if (step["reward2_goods_list"].isString()){
				resStr = step["reward2_goods_list"].asString();
			}

			if (step["reward2_item_list"].isString()){
				itemStr = step["reward2_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("累计充值天数阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			
			resStr = "";
			itemStr = "";
			if (step["reward3_goods_list"].isString()){
				resStr = step["reward3_goods_list"].asString();
			}

			if (step["reward3_item_list"].isString()){
				itemStr = step["reward3_item_list"].asString();
			}

			if (!pActivityStep->addRewardItem(itemStr,resStr)){
				Athena::logger->error("累计充值天数阶段:%d添加奖励道具失败",id);
				SAFE_DELETE(pActivityStep);
				return false;
			}
			m_steps[id] = pActivityStep;	
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("累计充值天数 id:%d,title:%s,开始时间:%d,结束时间:%d",m_id,m_title.c_str(),m_startTime,m_endTime);

		std::map<UINT32,ActivityStep *>::iterator it = m_steps.begin();

		for (;it != m_steps.end();++it){
			it->second->print();
		}
	}
#endif
	return true;
}

ActivityCfgMgr::ActivityCfgMgr()
{

}

ActivityCfgMgr::~ActivityCfgMgr()
{
	
}

void ActivityCfgMgr::delActivity(UINT32 id)
{
	std::map<UINT32,Activity * >::iterator it = m_activities.find(id);	
	if (it != m_activities.end()){
		SAFE_DELETE(it->second);
		m_activities.erase(it);
	}
}

bool ActivityCfgMgr::init(std::string config)
{
	Json::Value configJson;
	Json::Reader reader;

	if (!reader.parse(config,configJson)){
		return false;
	}
	
	UINT32 activityID = 0;
	if (configJson["id"].isString()){
		activityID = (UINT32)atoi(configJson["id"].asString().c_str());
	}

	std::string pluginID;
	if (configJson["plugin_id"].isString()){
		pluginID = configJson["plugin_id"].asString();
	}

	Activity * pActivity = NULL;
	std::map<UINT32,Activity * >::iterator it = m_activities.find(activityID);
	if (it == m_activities.end()){
		if (pluginID == "consumePlugin") {
			pActivity = ATHENA_NEW ConsumeActivity();
		}
		else if (pluginID == "loginRewardPlugin"){
			pActivity = ATHENA_NEW CumuLoginActivity();
		}
		else if (pluginID == "specialTimeLoginPlugin"){
			pActivity = ATHENA_NEW SpecialTimeLoginActivity(); 
		}
		else if (pluginID == "exchangePlugin"){
			pActivity = ATHENA_NEW ExchangeActivity(); 
		}
		else if (pluginID == "weeklyConsumePlugin"){
			pActivity = ATHENA_NEW WeeklyConsumeActivity();
		}
		else if (pluginID == "oneDayOneRechargePlugin"){
			pActivity = ATHENA_NEW OneDayOneRechargeActivity(); 
		}
		else if (pluginID == "oneDayAccPlugin"){
			pActivity = ATHENA_NEW OneDayAccRechargeActivity();
		}
		else if (pluginID == "everyDayRecyleRechargePlugin"){
			pActivity = ATHENA_NEW OneDayRecyleRechargeActivity(); 
		}	
		else if (pluginID == "multiDayAccPlugin"){
			pActivity = ATHENA_NEW MultiDayRechargeActivity(); 
		}
		else if (pluginID == "accRechargeDayPlugin"){
			pActivity = ATHENA_NEW AccRechargeDayActivity(); 
		}

		if (!pActivity){
			return false;
		}
	}
	else {
		pActivity = it->second;	
	}

	if (!pActivity){
		return false;
	}
	if (!pActivity->init(configJson)){
		Athena::logger->error("解析活动:%d失败",activityID);
		SAFE_DELETE(pActivity);
		return false;
	}
	m_activities[pActivity->getID()] = pActivity;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("活动id:%d初始花成功!",activityID);
#endif
	return true;
}

Activity * ActivityCfgMgr::getActivity(UINT32 id)
{
	std::map<UINT32,Activity * >::iterator it = m_activities.find(id);	
	if (it == m_activities.end()){
		return NULL;
	}
	return it->second;
}

CActivityDataMgr::CActivityDataMgr(GameUser *pOwner) : m_pOwner(pOwner)
{
	
}

CActivityDataMgr::~CActivityDataMgr()
{

}

UINT32 CActivityDataMgr::saveAll(byte * buf,UINT32 needSpace)
{
	UINT32 offset = 0;
	//保存版本
	 *(UINT32 *)(buf + offset) = s_SAVE_VERSION_INFO;
	 offset += sizeof(UINT32);
	 //保存活动数量
	*(UINT16 *)(buf + offset) = m_activityDatas.size();
	offset += sizeof(UINT16);

	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.begin();
	for (;it != m_activityDatas.end();++it){
		ActivityData & activity = it->second;
		//保存活动ID
		*(UINT32 *)(buf + offset) = activity.m_id;
		offset += sizeof(UINT32);
		
		//保存阶段值
		*(UINT32 *)(buf + offset) = activity.m_stepVal;
		offset += sizeof(UINT32);
		
		//保存扩展的值
		*(UINT32 *)(buf + offset) = activity.m_stepValExt;
		offset += sizeof(UINT32);
		
		//保存刷新的值
		*(UINT32 *)(buf + offset) = activity.m_refreshTime;
		offset += sizeof(UINT32);
		
		//阶段数量
		*(UINT16 *)(buf + offset) = activity.m_steps.size();
		offset += sizeof(UINT16);
		std::map<UINT32,UINT16>::iterator subIt = activity.m_steps.begin();
		for (;subIt != activity.m_steps.end();++subIt){
			*(UINT32 *)(buf + offset) = subIt->first;
			offset += sizeof(UINT32);
			*(UINT16 *)(buf + offset) = subIt->second;
			offset += sizeof(UINT16);
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("保存了 CActivityDataMgr::saveAll:%d 字节",offset);
#endif
	return offset;
}

UINT32 CActivityDataMgr::loadAll(byte * buf)
{
	UINT32 offset = 0;
	//加载版本
	UINT32 version = *(UINT32 *)(buf + offset);
	offset += sizeof(UINT32);

	if (s_SAVE_VERSION_INFO == version) {
		//加载数量
		UINT16 activityNum = *(UINT16 *)(buf + offset);
		offset += sizeof(UINT16);

		for (UINT16 i = 0;i < activityNum; ++i){
			ActivityData activity;
			//加载ID
			activity.m_id = *(UINT32 *)(buf + offset);
			offset += sizeof(UINT32);
			//加载了阶段						
			activity.m_stepVal = *(UINT32 *)(buf + offset);
			offset += sizeof(UINT32);
			//加载了扩展阶段
			activity.m_stepValExt = *(UINT32 *)(buf + offset);
			offset += sizeof(UINT32);
			//加载刷新时间
			activity.m_refreshTime = *(UINT32 *)(buf + offset);
			offset += sizeof(UINT32);
			//阶段数量
			UINT16 stepNum = *(UINT16 *)(buf + offset);
			offset += sizeof(UINT16);
			for (UINT16 j = 0;j < stepNum;++j){
				UINT32 stepID = *(UINT32*)(buf + offset);
				offset += sizeof(UINT32);
				UINT16 finishNum = *(UINT16*)(buf + offset);
				offset += sizeof(UINT16);
				activity.m_steps.insert(std::make_pair(stepID,finishNum));
			}

			Activity * pActivityCfg = ActivityCfgMgr::getSingleton().getActivity(activity.m_id);
			if (!pActivityCfg || (!pActivityCfg->isActive())){//不在时间范围内了,不应该加载了
				continue;
			}
			m_activityDatas.insert(std::make_pair(activity.m_id,activity));
		}
	}
	checkActivityRefresh();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("保存了 CActivityDataMgr::loadAll:%d 字节",offset);
#endif
	return offset;
}

void CActivityDataMgr::checkActivityRefresh()
{
	std::vector<UINT32> delID;
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.begin();
	
	for (;it != m_activityDatas.end();++it){
		ActivityData & activity = it->second;
		
		Activity * pCfg = ActivityCfgMgr::getSingleton().getActivity(activity.m_id);
		if (pCfg){
			/*if (pCfg->getCurRound() != activity.m_curRound){
				activity.reset();
				activity.m_curRound = activity.m_curRound;
			}*/
			//ActivityData & activityData = it->second;
			
			if ((g_gameServer->getSec() > pCfg->getStartTime()) && (g_gameServer->getSec() < pCfg->getEndTime())){
				checkActivityReset(pCfg->getActivityType(),pCfg->getID(), pCfg->getStartTime());
			}
			else {
				delID.push_back(activity.m_id);
			}
		}
		else {
			delID.push_back(activity.m_id);
		}
	}

	for (UINT16 i = 0;i < delID.size();++i){
		m_activityDatas.erase(delID[i]);	
	}
}

void CActivityDataMgr::acceptNewActivity()
{
	std::map<UINT32,Activity * > & allActivities = ActivityCfgMgr::getSingleton().getAllActivities();
	std::map<UINT32,Activity * >::iterator it = allActivities.begin();

	for (;it != allActivities.end();++it){
		Activity * pActivity = it->second;
		if (pActivity->isActive() && pActivity->roleCreateTimeValid(m_pOwner->m_firstLogin)){//是否活跃
			std::map<UINT32,ActivityData>::iterator subIt =  m_activityDatas.find(pActivity->getID());
			if (subIt == m_activityDatas.end()){
				ActivityData & data = m_activityDatas[pActivity->getID()];
				data.m_id = pActivity->getID();
			}
		}
	}
}

void CActivityDataMgr::sendAllAvaibleActivity()
{
	checkActivityRefresh();
	MiscProto::stRetAvaibleActivity retCmd;
	MiscProto::stRetExchangeActivity  exchgRetCmd;
	MiscProto::stSendRecycleRechargeActivity recycleActCmd;

	std::map<UINT32,Activity * > & allActivities = ActivityCfgMgr::getSingleton().getAllActivities();
	std::map<UINT32,Activity * >::iterator it = allActivities.begin();

	for (;it != allActivities.end();++it){
		Activity * pActivity = it->second;
		if (pActivity->isActive() && (pActivity->roleCreateTimeValid(m_pOwner->m_firstLogin))){//是否活跃
			if (MiscProto::eActivityType_Exchange == pActivity->getActivityType()){//兑换活动特殊协议
				MiscProto::stSynExchangeActivity * pSyn = exchgRetCmd.add_activity();
				pSyn->set_activityid(pActivity->getID());
				pSyn->set_desc(pActivity->getDesc());
				pSyn->set_titile(pActivity->getTitle());
				ExchangeActivity * pExhgActivity = dynamic_cast<ExchangeActivity *>(pActivity);
				if (pExhgActivity){
					pSyn->set_dailyrefresh(pExhgActivity->isDailyRefresh());
				}

				std::map<UINT32,ActivityStep *>& allStep = pActivity->getAllStep();
				std::map<UINT32,ActivityStep *>::iterator step_It = allStep.begin();

				for (;step_It != allStep.end();++step_It){
					ActivityStep * pAcitivityStep = step_It->second;

					MiscProto::stSynExchangeActivityiStep * pStepSyn = pSyn->add_step();
					if (pStepSyn){
						std::map<UINT32,ActivityData>::iterator subIt =  m_activityDatas.find(pActivity->getID());
						if (subIt != m_activityDatas.end()){
							std::map<UINT32,UINT16>::iterator stepDataIt = subIt->second.m_steps.find(pAcitivityStep->getID());
							if (stepDataIt != subIt->second.m_steps.end()){
								pStepSyn->set_hasexchgnum(stepDataIt->second);
							}
						}

						ExchangeActivityStep * pExchgStep = dynamic_cast<ExchangeActivityStep*>(pAcitivityStep);
						if (pExchgStep){
							pExchgStep->fillStepInfo(pStepSyn);
						}
					}
				}
			}
			else if (MiscProto::eActivityType_OneDayRecyleRecharge == pActivity->getActivityType()){
				MiscProto::stRecycleRechargeActivity * pSyn = recycleActCmd.add_activties();
				pSyn->set_titile(pActivity->getTitle());
				pSyn->set_desc(pActivity->getDesc());
				pSyn->set_activityid(pActivity->getID());
				OneDayRecyleRechargeActivity * pRecyleAct = dynamic_cast<OneDayRecyleRechargeActivity *>(pActivity);
				if (pRecyleAct){
					std::map<UINT32,ActivityStep *>& allStep = pActivity->getAllStep();
					std::map<UINT32,ActivityStep *>::iterator step_It = allStep.begin();
					
					if (step_It != allStep.end()){
						ActivityStep * pAcitivityStep = step_It->second;
						pAcitivityStep->fillActivityStepInfo(*pSyn->mutable_step());
						OneDayRecycleRechargeStep * pRecycleActStep = dynamic_cast<OneDayRecycleRechargeStep *>(pAcitivityStep);
						if (pRecycleActStep){
							pSyn->set_maxrewardnum(pRecycleActStep->getRewardNum());	
						}
						pSyn->set_gotrewardnum(getActivityExtStepVal(pActivity->getActivityType(),pActivity->getID()));
						pSyn->set_stepval(getActivityStepVal(pActivity->getActivityType(),pActivity->getID()));
					}
				}
			}
			else {
				MiscProto::stSynActivityInfo *pSyn = retCmd.add_activities();
				if (pSyn){
					pSyn->set_id(pActivity->getID());
					pSyn->set_title(pActivity->getTitle());
					pSyn->set_type(pActivity->getActivityType());
					pSyn->set_curval(getActivityStepVal(pActivity->getActivityType(),pActivity->getID()));
					pSyn->set_desc(pActivity->getDesc());

					std::map<UINT32,ActivityData>::iterator subIt =  m_activityDatas.find(pActivity->getID());
					if (subIt != m_activityDatas.end()){//完成的阶段
						ActivityData & data = subIt->second;
						std::map<UINT32,UINT16>::iterator thdIt = data.m_steps.begin();
						for (;thdIt != data.m_steps.end();++thdIt){
							pSyn->add_finishstep(thdIt->first);	
						}
					}

					std::map<UINT32,ActivityStep *>& allStep = pActivity->getAllStep();
					std::map<UINT32,ActivityStep *>::iterator step_It = allStep.begin();

					for (;step_It != allStep.end();++step_It){
						ActivityStep * pAcitivityStep = step_It->second;
						MiscProto::stSynActivityStepInfo * pSynStep = pSyn->add_avaiblestep();	
						pAcitivityStep->fillActivityStepInfo(*pSynStep);
					}
				}
			}
		}
	}
	m_pOwner->sendProto2Me(retCmd);
	m_pOwner->sendProto2Me(exchgRetCmd);
	m_pOwner->sendProto2Me(recycleActCmd);
}

void CActivityDataMgr::doActivityStepReward(UINT32 id,UINT32 stepID,UINT16 selIndex)
{
	checkActivityRefresh(); 
	Activity * pCfg = ActivityCfgMgr::getSingleton().getActivity(id);
	if (pCfg){
		pCfg->doReward(m_pOwner,stepID,selIndex);
	}
}

bool CActivityDataMgr::isStepFinish(UINT32 id,UINT32 stepID)
{
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.find(id);
	if (it == m_activityDatas.end()){
		return false;
	}

	if (it->second.m_steps.find(stepID) == it->second.m_steps.end()){
		return false;
	}
	return true;
}

UINT32 CActivityDataMgr::getActivityStepVal(UINT16 type,UINT32 id)
{
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.find(id);
	if (it != m_activityDatas.end()){
		return it->second.m_stepVal;
	}
	return 0;
}

void   CActivityDataMgr::incActivityStepVal(UINT16 type,UINT32 id)
{
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.find(id);
	if (it != m_activityDatas.end()){
		it->second.m_stepVal += 1;
	}
}

UINT32 CActivityDataMgr::getActivityExtStepVal(UINT16 type,UINT32 id)
{
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.find(id);
	if (it != m_activityDatas.end()){
		return it->second.m_stepValExt;
	}
	return 0;
}

void  CActivityDataMgr::incActivityExtStepVal(UINT16 type,UINT32 id)
{
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.find(id);
	if (it != m_activityDatas.end()){
		it->second.m_stepValExt += 1;
	}
}

UINT16 CActivityDataMgr::getStepDoTime(UINT32 id,UINT32 stepID)
{
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.find(id);
	if (it != m_activityDatas.end()){
		std::map<UINT32,UINT16>::iterator subIt = it->second.m_steps.find(stepID);
		if (subIt != it->second.m_steps.end()){
			return subIt->second;
		}
	}
	return 0;
}

void  CActivityDataMgr::addStepDoTime(UINT32 id,UINT32 stepID)
{
	m_activityDatas[id].m_steps[stepID] += 1;
}

void  CActivityDataMgr::checkActivityReset(UINT16 type,UINT32 id, UINT32 startTime)
{
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.find(id);
	if (it == m_activityDatas.end()){
		return ;
	}

	if (MiscProto::eActivityType_Consume == type){//消耗奖励
		if (!g_isTheSameDay(it->second.m_refreshTime,g_gameServer->getSec())){
			it->second.m_stepVal = 0;
			it->second.m_stepValExt = 0;
			it->second.m_refreshTime = g_gameServer->getSec();
			it->second.m_steps.clear();
		}
	}
	else if (MiscProto::eActivityType_CumuLogin == type){//累积登录
	
	}
	else if (MiscProto::eActivityType_SpecialTimeLogin == type){//限时登录
	
	}
	else if (MiscProto::eActivityType_Exchange == type){//兑换活动
		Activity * pCfg = ActivityCfgMgr::getSingleton().getActivity(id);
		if (pCfg && (pCfg->getActivityType() == MiscProto::eActivityType_Exchange)){
			ExchangeActivity * pExchActivity = dynamic_cast<ExchangeActivity *>(pCfg);
			if (pExchActivity && pExchActivity->isDailyRefresh()){
				if (!g_isTheSameDay(it->second.m_refreshTime,g_gameServer->getSec())){
					it->second.m_stepVal = 0;
					it->second.m_stepValExt = 0;
					it->second.m_refreshTime = g_gameServer->getSec();
					it->second.m_steps.clear();
				}
			}
		}
	}
	else if (MiscProto::eActivityType_WeeklyConsume == type){//周消耗奖励
        if (!it->second.m_refreshTime) {
            it->second.m_refreshTime = startTime + 604800;//下一次刷新时间,86400 * 7 = 604800
        }
        if (g_gameServer->getSec() >= it->second.m_refreshTime) {
			it->second.m_stepVal = 0;		
			it->second.m_refreshTime += 604800;
			it->second.m_stepValExt = 0;
			it->second.m_steps.clear();
		}
	}
	else if (MiscProto::eActivityType_OneDayOneRecharge == type){//每日单笔充值
		if (!g_isTheSameDay(it->second.m_refreshTime,g_gameServer->getSec())){
			it->second.m_stepVal = 0;		
			it->second.m_refreshTime = g_gameServer->getSec();
			it->second.m_stepValExt = 0;
			it->second.m_steps.clear();
		}
	}
	else if (MiscProto::eActivityType_OneDayAccRecharge == type){//每日累积充值
		if (!g_isTheSameDay(it->second.m_refreshTime,g_gameServer->getSec())){
			it->second.m_stepVal = 0;		
			it->second.m_refreshTime = g_gameServer->getSec();
			it->second.m_stepValExt = 0;
			it->second.m_steps.clear();
		}
	}
	else if (MiscProto::eActivityType_OneDayRecyleRecharge == type){//每日循环充值
		if (!g_isTheSameDay(it->second.m_refreshTime,g_gameServer->getSec())){
			it->second.m_stepVal = 0;		
			it->second.m_refreshTime = g_gameServer->getSec();
			it->second.m_stepValExt = 0;
			it->second.m_steps.clear();
		}
	}
	else if (MiscProto::eActivityType_MultiAccRecharge == type){//多日累积充值
	
	}
	else if (MiscProto::eActivityType_AccRechargeDay == type){//累积充值天数
	
	}
}

void CActivityDataMgr::addStepID(UINT32 id,UINT32 step)
{
	ActivityData & activityData = m_activityDatas[id];
	activityData.m_id = id;
	activityData.m_steps[step] += 1;
}

/*void CActivityDataMgr::resetActivity(UINT32 id)
{
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.find(id);
	if (it != m_activityDatas.end()){
		ActivityData & data = it->second;
		data.reset();
	}
}*/

void CActivityDataMgr::onUserLogin()
{
	acceptNewActivity();
	checkActivityRefresh();

	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.begin();
	
	for (;it != m_activityDatas.end();++it){
		ActivityData & activity = it->second;
	
		Activity * pActivityCfg = ActivityCfgMgr::getSingleton().getActivity(activity.m_id);
		if (!pActivityCfg){
			continue;
		}

		if (MiscProto::eActivityType_CumuLogin == pActivityCfg->getActivityType()){//累积登录
			if (!g_isTheSameDay(activity.m_refreshTime,g_gameServer->getSec())){
				activity.m_stepVal += 1;		
				activity.m_refreshTime = g_gameServer->getSec();
			}
		}
		else if (MiscProto::eActivityType_SpecialTimeLogin == pActivityCfg->getActivityType()){//限时登录
			if (!g_isTheSameDay(activity.m_refreshTime,g_gameServer->getSec())){
				activity.m_stepVal += 1;		
				activity.m_refreshTime = g_gameServer->getSec();
			}
		}
	}
}

void CActivityDataMgr::onUseDiamond(UINT32 num)
{
	checkActivityRefresh();

	bool bNeedNotify = false;
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.begin();
	MiscProto::stRefreshCommonActInfo retCmd;

	for (;it != m_activityDatas.end();++it){
		ActivityData & activity = it->second;
		
		Activity * pActivityCfg = ActivityCfgMgr::getSingleton().getActivity(activity.m_id);
		if (!pActivityCfg){
			continue;
		}

		if (MiscProto::eActivityType_Consume == pActivityCfg->getActivityType()){//消耗奖励
			activity.m_refreshTime = g_gameServer->getSec();
			activity.m_stepVal += num;
			bNeedNotify = true;
		}
		else if (MiscProto::eActivityType_WeeklyConsume == pActivityCfg->getActivityType()){//周消耗奖励
			//activity.m_refreshTime = g_gameServer->getSec();
			activity.m_stepVal += num;
			bNeedNotify = true;
		}

		if (bNeedNotify){
			MiscProto::stCommonActInfoItem * pSyn = retCmd.add_items();
			if (pSyn){
				pSyn->set_type(pActivityCfg->getActivityType());
				pSyn->set_id(pActivityCfg->getID());
				pSyn->set_val(activity.m_stepVal);
			}
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}

void CActivityDataMgr::onRecharge(UINT32 num)
{
	checkActivityRefresh();
	std::map<UINT32,ActivityData>::iterator it = m_activityDatas.begin();
	
	MiscProto::stRefreshCommonActInfo retCmd;

	for (;it != m_activityDatas.end();++it){
		Activity * pActivityCfg = ActivityCfgMgr::getSingleton().getActivity(it->first);
		if (!pActivityCfg){
			continue;
		}
		ActivityData & activity = it->second;
		
		bool bNeedNotify = false;

		if (MiscProto::eActivityType_OneDayOneRecharge == pActivityCfg->getActivityType()){//每日单笔充值
			if (num > activity.m_stepVal){
				activity.m_stepVal = num;
			}
			activity.m_refreshTime = g_gameServer->getSec();
			bNeedNotify = true;
		}
		else if (MiscProto::eActivityType_OneDayAccRecharge == pActivityCfg->getActivityType()){//每日累积充值
			activity.m_stepVal += num;
			activity.m_refreshTime = g_gameServer->getSec();
			bNeedNotify = true;
		}
		else if (MiscProto::eActivityType_OneDayRecyleRecharge == pActivityCfg->getActivityType()){//每日循环充值
			activity.m_stepVal += num;
			activity.m_refreshTime = g_gameServer->getSec();
			bNeedNotify = true;
		}
		else if (MiscProto::eActivityType_MultiAccRecharge == pActivityCfg->getActivityType()){//多日累积充值
			activity.m_stepVal += num;
			activity.m_refreshTime = g_gameServer->getSec();
			bNeedNotify = true;
		}
		else if (MiscProto::eActivityType_AccRechargeDay == pActivityCfg->getActivityType()){//累积充值天数
			AccRechargeDayActivity * pAccRechargeDayAct = dynamic_cast<AccRechargeDayActivity *>(pActivityCfg);
			if (pAccRechargeDayAct){
				if (num >= pAccRechargeDayAct->getRechargeNum()){
					if (!g_isTheSameDay(activity.m_refreshTime,g_gameServer->getSec())){
						activity.m_stepVal += 1;
						bNeedNotify = true;
					}
					activity.m_refreshTime = g_gameServer->getSec();
				}
			}
		}

		if (bNeedNotify){
			MiscProto::stCommonActInfoItem * pSyn = retCmd.add_items();
			if (pSyn){
				pSyn->set_type(pActivityCfg->getActivityType());
				pSyn->set_id(pActivityCfg->getID());
				pSyn->set_val(activity.m_stepVal);
			}
		}
	}

	m_pOwner->sendProto2Me(retCmd);
}

bool CActivityDataMgr::addActivityVal(UINT32 num, MiscProto::eActivityType type) {
    Activity * pCfg = NULL;
    UINT32 now = g_gameServer->getSec();
	for (std::map<UINT32,ActivityData>::iterator it = m_activityDatas.begin();
            it != m_activityDatas.end(); ++it) {
		pCfg = ActivityCfgMgr::getSingleton().getActivity(it->second.m_id);
		if (!pCfg){
            continue;
        }
        if (pCfg->getActivityType() != type) {
            continue;
        }
        if (now < pCfg->getStartTime() || now > pCfg->getEndTime()) {
            continue;
        }
        if (num) {
            MiscProto::stRefreshCommonActInfo retCmd;
			MiscProto::stCommonActInfoItem * pSyn = retCmd.add_items();
			if (pSyn){
                it->second.m_stepVal += num;
				pSyn->set_type(pCfg->getActivityType());
				pSyn->set_id(pCfg->getID());
				pSyn->set_val(it->second.m_stepVal);
                m_pOwner->sendProto2Me(retCmd);
                return true;
			}
        }
    }
    return false;
}
