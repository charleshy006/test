#include "RewardSys.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "GameServer.h"
#include "GameUser.h"
#include "Misc.pb.h"

extern GameServer * g_gameServer;

CRewardBase::CRewardBase()
{
	
}

CRewardBase::~CRewardBase()
{

}

CSignReward::CSignReward()
{}

CSignReward::~CSignReward()
{}

bool CSignReward::doReward(GameUser *pUser,UINT32 val)
{
	if (!pUser){
		return false;	
	}
	
	pUser->m_rewardSys.checkSignRefresh();

	MiscProto::stGetSignRewardResult retCmd;
	if (g_isTheSameDay(g_gameServer->getSec(),pUser->m_rewardSys.m_lastSignRewardTime)){//已经领过了
		retCmd.set_result(MiscProto::stGetSignRewardResult::HAS_GET_REWARD);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	
	UINT16 day = pUser->m_rewardSys.m_signRewardNum + 1;
	
	if (day > m_signRewardInfos.size()){//领完又进入循环
		day = 0;
	}
	
	std::vector<stSignRewardInfo> &  rewardVec = m_signRewardInfos[day];
	
	for (UINT16 i = 0;i < rewardVec.size(); ++i){
		stSignRewardInfo & signInfo = rewardVec[i];
		if (pUser->m_boxMgr.needSlotNum(signInfo.m_boxID) > pUser->m_bagMgr.getMainFreeSlotNum()){
			retCmd.set_result(MiscProto::stGetSignRewardResult::BAG_FULL);
			pUser->sendProto2Me(retCmd);
			return false;	
		}
	}
	
	for (UINT16 i = 0;i < rewardVec.size(); ++i){
		stSignRewardInfo & signInfo = rewardVec[i];
		pUser->m_boxMgr.openBox(signInfo.m_boxID);
	}

	pUser->m_rewardSys.m_signRewardNum += 1;
	pUser->m_rewardSys.m_lastSignRewardTime = g_gameServer->getSec();
	retCmd.set_result(MiscProto::stGetSignRewardResult::SUCCESS);
	retCmd.set_day(day);
	pUser->sendProto2Me(retCmd);
	return true;
}


UINT32 CSignReward::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	*(UINT16 *)buf = pRewardSys->m_signRewardNum;
	UINT32 ret = sizeof(UINT16);

	*(UINT32 *)(buf + ret) = pRewardSys->m_lastSignRewardTime;
	ret += sizeof(UINT32);
	return ret;
}

UINT32 CSignReward::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	pRewardSys->m_signRewardNum = *(UINT16 *)buf;
	ret += sizeof(UINT16);

	pRewardSys->m_lastSignRewardTime = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);
	return ret;
}

bool CSignReward::loadCfg(TiXmlElement * pNode)
{
	if (!pNode){
		return false;
	}

	TiXmlElement * pDayNode = pNode->FirstChildElement("Day");
	while (pDayNode){
		INT32 iVal = 0;
		UINT16 day = 0;
		std::vector<stSignRewardInfo>  vec;
		if (pDayNode->Attribute("id",&iVal)){
			day = iVal;
		}

		TiXmlElement *  pRewardNode = pDayNode->FirstChildElement("Reward");
		while (pRewardNode){
			stSignRewardInfo rewardItem;
			if (pRewardNode->Attribute("boxid",&iVal)){
				rewardItem.m_boxID = iVal;
			}
			vec.push_back(rewardItem);
			pRewardNode  = pRewardNode->NextSiblingElement("Reward");
		}

		m_signRewardInfos[day]  = vec;
		pDayNode = pDayNode->NextSiblingElement("Day");
	}

#ifdef _HDZ_DEBUG
	RewardItem_IT it = m_signRewardInfos.begin();
	Athena::logger->trace("打印签到奖励:");
	for (;it != m_signRewardInfos.end(); ++it){
		Athena::logger->trace("day=%d的奖励",it->first);
		for (UINT16 i = 0;i < it->second.size(); ++i){
			stSignRewardInfo & item = it->second[i];
			Athena::logger->trace("boxID=%d",item.m_boxID);
		}
	}
#endif
	return true;
}

	
COpenSvrReward::COpenSvrReward()
{}

COpenSvrReward::~COpenSvrReward()
{}

bool COpenSvrReward::doReward(GameUser *pUser,UINT32 val)
{
	if (!pUser){
		return false;
	}

	MiscProto::stGetOpenSvrRewardResult retCmd;
	if (g_isTheSameDay(g_gameServer->getSec(),pUser->m_rewardSys.m_lastOpenSvrRewardTime)){//已经领过了
		retCmd.set_result(MiscProto::stGetOpenSvrRewardResult::eHAS_GET_REWARD);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	
	UINT16 day = pUser->m_rewardSys.m_openSvrRewardNum + 1;
	
	if (day > m_openSvrRewardInfos.size()){//领完又进入循环
		day = 0;
	}
	
	stOpenSvrRewardInfo &  openSvrCfg = m_openSvrRewardInfos[day];

	UINT32 needSlotNum = 0;
	for (UINT16 i = 0;i < openSvrCfg.m_boxVec.size(); ++i){
		needSlotNum += pUser->m_boxMgr.needSlotNum(openSvrCfg.m_boxVec[i]);
	}

	if (needSlotNum > pUser->m_bagMgr.getMainFreeSlotNum()){
		retCmd.set_result(MiscProto::stGetOpenSvrRewardResult::eBAG_FULL);
		pUser->sendProto2Me(retCmd);
		return false;	
	}
	
	for (UINT16 i = 0;i < openSvrCfg.m_boxVec.size(); ++i){
		pUser->m_boxMgr.openBox(openSvrCfg.m_boxVec[i]);
	}

	pUser->m_rewardSys.m_openSvrRewardNum += 1;
	pUser->m_rewardSys.m_lastOpenSvrRewardTime = g_gameServer->getSec();
	retCmd.set_result(MiscProto::stGetOpenSvrRewardResult::eSUCCESS);
	retCmd.set_day(day);
	pUser->sendProto2Me(retCmd);
	return true;
}

UINT32 COpenSvrReward::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	*(UINT16 *)(buf + ret) = pRewardSys->m_openSvrRewardNum;
	ret += sizeof(UINT16);

	*(UINT32 *)(buf + ret) = pRewardSys->m_lastOpenSvrRewardTime;
	ret += sizeof(UINT32);
	return ret;
}

UINT32 COpenSvrReward::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;

	pRewardSys->m_openSvrRewardNum =  *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);

	pRewardSys->m_lastOpenSvrRewardTime = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);
	return ret;	
}

bool COpenSvrReward::loadCfg(TiXmlElement * pNode)
{
	if (!pNode){
		return false;
	}
	
	INT32 iVal = 0;

	TiXmlElement  * pDayNode = pNode->FirstChildElement("Day");
	while (pDayNode){
		stOpenSvrRewardInfo  openSvrInfo;
		if (pDayNode->Attribute("id",&iVal)){
			openSvrInfo.m_day = iVal;
		}
		
		TiXmlElement  * pRewardNode = pDayNode->FirstChildElement("Reward");
		while (pRewardNode){
			if (pRewardNode->Attribute("boxid",&iVal)){
				openSvrInfo.m_boxVec.push_back(iVal);
			} 
			pRewardNode = pRewardNode->NextSiblingElement("Reward"); 
		}

		m_openSvrRewardInfos[openSvrInfo.m_day] = openSvrInfo;
		pDayNode = pDayNode->NextSiblingElement("Day");
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("签到奖励:");

	std::map<UINT16,stOpenSvrRewardInfo>::iterator it =   m_openSvrRewardInfos.begin();
	for (;it != m_openSvrRewardInfos.end(); ++it){
		stOpenSvrRewardInfo & info = it->second;
		Athena::logger->trace("第%d天的签到信息:",info.m_day);
		for (UINT16 i = 0;i < info.m_boxVec.size() ; ++i){
			Athena::logger->trace("boxID=%d",info.m_boxVec[i]);
		}
	}
#endif
	return true;
}

CLevelReward::CLevelReward()
{

}

CLevelReward::~CLevelReward()
{
	
}

bool CLevelReward::doReward(GameUser *pUser,UINT32 val)
{
	if (!pUser){
		return false;
	}
	
	MiscProto::stLvRewardResult retCmd;
	if (pUser->m_rewardSys.m_hasRewardLv.find(val) !=pUser->m_rewardSys.m_hasRewardLv.end()){
		retCmd.set_result(MiscProto::stLvRewardResult::eHAS_NO_REWARD_LV);
		pUser->sendProto2Me(retCmd);
		return false;
	}	

	if (val > pUser->getLevel()){//领取的等级大于玩家等级
		retCmd.set_result(MiscProto::stLvRewardResult::eLEVEL_TOO_LOW);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	
	std::map<UINT32,stLevelReward>::iterator it =  m_lvRewardCfg.find(val);
	if (it == m_lvRewardCfg.end()){
		retCmd.set_result(MiscProto::stLvRewardResult::eINNER_ERROR);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	stLevelReward & levelRewardCfg = it->second;

	UINT32 needSlotNum = 0;
	for (UINT16 i = 0;i < levelRewardCfg.m_boxIDVec.size(); ++i){
		needSlotNum += pUser->m_boxMgr.needSlotNum(levelRewardCfg.m_boxIDVec[i]);
	}
	
	if (needSlotNum > pUser->m_bagMgr.getMainFreeSlotNum()) {
		retCmd.set_result(MiscProto::stLvRewardResult::eBAG_SLOT_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
		return false;
	}

	for (UINT16 i = 0;i < levelRewardCfg.m_boxIDVec.size();++i){
		pUser->m_boxMgr.openBox(levelRewardCfg.m_boxIDVec[i]);
	}
	
	//领取成功
	pUser->m_rewardSys.m_hasRewardLv.insert(val);
	retCmd.set_getrewardlv(val);
	retCmd.set_result(MiscProto::stLvRewardResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
	return true;
}

UINT32 CLevelReward::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	*(UINT16*)(buf + ret) = pRewardSys->m_hasRewardLv.size();
	ret += sizeof(UINT16);

	std::set<UINT16>::iterator it = pRewardSys->m_hasRewardLv.begin();
	for  (;it != pRewardSys->m_hasRewardLv.end(); ++it){
		*(UINT16*)(buf + ret) = *it;
		ret += sizeof(UINT16);
	}
	return ret;
}

UINT32 CLevelReward::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	UINT16 num = *(UINT16*)(buf + ret);
	ret += sizeof(UINT16);

	for (UINT16 i = 0;i < num; ++i){
		pRewardSys->m_hasRewardLv.insert(*(UINT16 *)(buf + ret));
		ret += sizeof(UINT16);			
	}
	return ret;
}

bool CLevelReward::loadCfg(TiXmlElement * pNode)
{
	INT32 iVal = 0;

	TiXmlElement  * pLevelNode = pNode->FirstChildElement("Level");
	while (pLevelNode){
		stLevelReward one;

		if (pLevelNode->Attribute("val",&iVal)){
			one.m_level = iVal;
		}
		
		TiXmlElement  * pRewardNode = pLevelNode->FirstChildElement("Reward");
		while (pRewardNode){
			if (pRewardNode->Attribute("boxid",&iVal)){
				one.m_boxIDVec.push_back(iVal);
			} 
			pRewardNode = pRewardNode->NextSiblingElement("Reward"); 
		}
		m_lvRewardCfg[one.m_level] = one;
		pLevelNode = pLevelNode->NextSiblingElement("Level");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载的升级奖励如下:");
	std::map<UINT32,stLevelReward>::iterator it = m_lvRewardCfg.begin();
	for (;it != m_lvRewardCfg.end();++it){
		stLevelReward & one = it->second;
		Athena::logger->trace("level=%d",one.m_level);
		for (UINT16 i = 0;i < one.m_boxIDVec.size() ; ++i){
			Athena::logger->trace("boxID=%d",one.m_boxIDVec[i]);	
		}
	}
#endif
	return true;
}


COnlineReward::COnlineReward()
{

}
	
COnlineReward::~COnlineReward()
{

}

UINT32 COnlineReward::getTotalOnline(UINT32 id)
{
	std::map<UINT32,stOnlineReward>::iterator it = m_onlineRewardCfg.find(id);
	if (it != m_onlineRewardCfg.end()){
		return it->second.m_onlineTime;
	}
	return (UINT32)-1;
}

bool COnlineReward::doReward(GameUser *pUser,UINT32 val)
{
	MiscProto::stGetOnlineRewardResult retCmd;
	retCmd.set_id(val);
	retCmd.set_onlinetime(pUser->m_rewardSys.m_getOnlineRewardPassTime);

	UINT32 needOnlineTime = getTotalOnline(val);
	if (pUser->m_rewardSys.m_getOnlineRewardPassTime < needOnlineTime){
		retCmd.set_result(MiscProto::stGetOnlineRewardResult::eHAS_REMAIN_TIME);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	std::map<UINT32,stOnlineReward>::iterator it = m_onlineRewardCfg.find(val);
	if (it == m_onlineRewardCfg.end()){
		Athena::logger->trace("领取在线奖励出现错误,配置文件没有改奖励ID");
		retCmd.set_result(MiscProto::stGetOnlineRewardResult::eINNER_ERROR);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	
	if (pUser->m_rewardSys.m_onlineRewards.find(val) != pUser->m_rewardSys.m_onlineRewards.end()){
		retCmd.set_result(MiscProto::stGetOnlineRewardResult::eHAS_GOT_REWARD);
		pUser->sendProto2Me(retCmd);
		return false;
	}

	stOnlineReward & reward = it->second;	
	
	UINT32 needSlotNum = 0;
	for (UINT16 i = 0;i < reward.m_boxIDVec.size(); ++i){
		needSlotNum += pUser->m_boxMgr.needSlotNum(reward.m_boxIDVec[i]);
	}

	if (needSlotNum > pUser->m_bagMgr.getMainFreeSlotNum()){
		retCmd.set_result(MiscProto::stGetOnlineRewardResult::eBAG_FULL);
		pUser->sendProto2Me(retCmd);
		return false;	
	}
	
	for (UINT16 i = 0;i < reward.m_boxIDVec.size(); ++i){
		pUser->m_boxMgr.openBox(reward.m_boxIDVec[i]);
	}

	pUser->m_rewardSys.m_getOnlineRewardTime = g_gameServer->getSec();

	retCmd.set_result(MiscProto::stGetOnlineRewardResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
	pUser->m_rewardSys.m_onlineRewards.insert(val);;
	return true;
}

UINT32 COnlineReward::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	
	*(UINT32 *)(buf + ret) = pRewardSys->m_getOnlineRewardTime;
	ret += sizeof(UINT32);

	*(UINT8 *)(buf + ret) = pRewardSys->m_onlineRewards.size();
	ret += sizeof(UINT8);

	std::set<UINT8>::iterator it =  pRewardSys->m_onlineRewards.begin();
	for (;it != pRewardSys->m_onlineRewards.end();++it){
		*(UINT8 *)(buf + ret) = *it;
		ret += sizeof(UINT8);
	}
	return ret;	
}

UINT32 COnlineReward::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	
	pRewardSys->m_getOnlineRewardTime = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	UINT8 size = *(UINT8*)(buf + ret);
	ret += sizeof(UINT8);

	for (UINT8 i = 0;i < size;++i){
		pRewardSys->m_onlineRewards.insert(*(UINT8*)(buf + ret));	
		ret += sizeof(UINT8);
	}
	return ret;	
}

bool COnlineReward::loadCfg(TiXmlElement * pNode)
{
	INT32 iVal = 0;

	TiXmlElement  * pTimeNode = pNode->FirstChildElement("Time");
	while (pTimeNode){
		stOnlineReward one;
		
		if (pTimeNode->Attribute("id",&iVal)){
			one.m_id = iVal;
		}

		if (pTimeNode->Attribute("val",&iVal)){
			one.m_onlineTime = iVal;
		}
		
		TiXmlElement  * pRewardNode = pTimeNode->FirstChildElement("Reward");
		while (pRewardNode){
			if (pRewardNode->Attribute("boxid",&iVal)){
				one.m_boxIDVec.push_back(iVal);
			} 
			pRewardNode = pRewardNode->NextSiblingElement("Reward"); 
		}
		m_onlineRewardCfg.insert(std::make_pair(one.m_id,one));
		pTimeNode = pTimeNode->NextSiblingElement("Time");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印在线奖励信息:");
	std::map<UINT32,stOnlineReward>::iterator it = m_onlineRewardCfg.begin();
	for (;it != m_onlineRewardCfg.end(); ++it){
		Athena::logger->trace("id=%d,onlineTime=%d,包含的boxID有:",it->second.m_id,it->second.m_onlineTime);	
		for (UINT16 i = 0;i < it->second.m_boxIDVec.size();++i){
			Athena::logger->trace("boxID=%d",it->second.m_boxIDVec[i]);
		}
	}
#endif
	return true;
}

CInvestPlan::CInvestPlan()
{

}

CInvestPlan::~CInvestPlan()
{
	
}

void CInvestPlan::buyInvestPlan(GameUser * pUser)
{
	if (!pUser){
		return ;
	}	
	return ;
	MiscProto::stBuyInvestPlanResult retCmd;
	if (!g_gameServer->isAppVersion()){
		if (pUser->getVipLv() < m_cfgs.m_needVip) {
			retCmd.set_result(MiscProto::stBuyInvestPlanResult::eVIP_LV_TOO_LOW);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}
	
	if (pUser->m_buyInvestPlan){
		retCmd.set_result(MiscProto::stBuyInvestPlanResult::eHAS_BUY);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->removeResource((eResource)m_cfgs.m_resType,m_cfgs.m_resNum,"购买投资计划")){
		retCmd.set_result(MiscProto::stBuyInvestPlanResult::eDIAMOND_NOT_ENOGUH);
		pUser->sendProto2Me(retCmd);
		return ;	
	}

	pUser->m_buyInvestPlan = 1;
	retCmd.set_result(MiscProto::stBuyInvestPlanResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
}

bool CInvestPlan::doReward(GameUser *pUser,UINT32 val)
{
	if (!pUser){
		return false;
	}

	MiscProto::stInvestPlanRewardResult retCmd;
	retCmd.set_lv(val);

	if(pUser->getLevel() < val){
		return false;
	}

	if (!pUser->m_buyInvestPlan){
		retCmd.set_result(MiscProto::stInvestPlanRewardResult::eHAS_NOT_BUY_INVEST_PLAN);
		pUser->sendProto2Me(retCmd);
		return false;
	}

	if (pUser->m_rewardSys.m_getInvestRewardLvs.find(val) != pUser->m_rewardSys.m_getInvestRewardLvs.end()){
		retCmd.set_result(MiscProto::stInvestPlanRewardResult::eHAS_GET_REWARD);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	
	retCmd.set_result(MiscProto::stInvestPlanRewardResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
	pUser->m_rewardSys.m_getInvestRewardLvs.insert(val);

	std::map<UINT16,stInvestReward >::iterator it = m_cfgs.m_rewards.find(val);
	if (it != m_cfgs.m_rewards.end()){
		stInvestReward & reward = it->second;
		pUser->addResource((eResource)reward.m_itemID,reward.m_num,"领取投资奖励!");
	}
	return true;	
}

UINT32 CInvestPlan::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	*(UINT16*)(buf + ret) = pRewardSys->m_getInvestRewardLvs.size();
	ret += sizeof(UINT16);

	std::set<UINT16>::iterator it = pRewardSys->m_getInvestRewardLvs.begin();
	for (;it != pRewardSys->m_getInvestRewardLvs.end();++it){
		*(UINT16 *)(buf + ret) = *it;
		ret += sizeof(UINT16);
	}
	return 	ret;
}
	
UINT32 CInvestPlan::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	UINT16 size = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);

	for (UINT16 i = 0;i < size ; ++i){

		Athena::logger->trace("[CInvestPlan] insert](%d)", *(UINT16 *)(buf + ret));

		pRewardSys->m_getInvestRewardLvs.insert(*(UINT16 *)(buf + ret));
		ret += sizeof(UINT16);
	}
	return ret;
}
	
bool CInvestPlan::loadCfg(TiXmlElement * pNode)
{
	if (!pNode){
		return false;
	}
	
	const char *szVal = NULL;
	INT32 iVal = 0;

	if ((szVal = pNode->Attribute("Starttime"))){
		std::vector<std::string> strVec;
		g_stringtok(strVec,szVal,"-");
		if (strVec.size() >= 2){
			m_cfgs.m_beginTime = (UINT32)atoi(strVec[0].c_str());
			m_cfgs.m_endTime = (UINT32)atoi(strVec[1].c_str());
		}
	}

	if (pNode->Attribute("costResType",&iVal)){
		m_cfgs.m_resType = iVal;
	}
	
	if (pNode->Attribute("Price",&iVal)){
		m_cfgs.m_resNum = iVal;
	}

	TiXmlElement * pLvNode = pNode->FirstChildElement("Lv");
	while (pLvNode){
		stInvestReward reward;
		UINT16 lv = 0;
		if (pLvNode->Attribute("id",&iVal)){
			lv = iVal;
		}

		if (pLvNode->Attribute("itemid",&iVal)){
			reward.m_itemID = iVal;
		}

		if (pLvNode->Attribute("itemnum",&iVal)){
			reward.m_num = iVal;
		}

		m_cfgs.m_rewards.insert(std::make_pair(lv,reward));
		pLvNode = pLvNode->NextSiblingElement("Lv");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("投资计划配置,beginTime=%d,endTime=%d,resType=%d,resNum=%d",m_cfgs.m_beginTime,m_cfgs.m_endTime,m_cfgs.m_resType,m_cfgs.m_resNum);

	std::map<UINT16,stInvestReward >::iterator it = m_cfgs.m_rewards.begin();
	for (;it != m_cfgs.m_rewards.end();++it){
		stInvestReward & reward = it->second;
		Athena::logger->trace("投资奖励 lv=%d,itemID=%d,itemNum=%d",it->first,reward.m_itemID,reward.m_num);	
	}
#endif
	return true;	
}

CSummonPlan::CSummonPlan()
{

}

CSummonPlan::~CSummonPlan()
{
	
}

bool CSummonPlan::doReward(GameUser *pUser, UINT32 val)
{

}

void CSummonPlan::sendSummonData(GameUser *pUser, UINT32 summonType)
{

	// optional uint32 summonType = 1;		//召唤的类型  1钻石，2英雄
	MiscProto::stSummonDataResult retCmd;
	
	if (summonType == 1) {
		if (pUser->m_buySummonPlan){
			retCmd.set_hasbuysummonment(true);
		}
		else {
			retCmd.set_hasbuysummonment(false);
		}

		std::set<UINT16>::iterator it1 =  pUser->m_freeSummonSet.begin();
		for (;it1 != pUser->m_freeSummonSet.end();++it1){
			retCmd.add_freesummonrewardlist(*it1);	
		}
		std::set<UINT16>::iterator it2 =  pUser->m_rechargeSummonSet.begin();
		for (;it2 != pUser->m_rechargeSummonSet.end();++it2){
			retCmd.add_rechargesummonrewardlist(*it2);	
		}

		retCmd.set_summontype(summonType);
		retCmd.set_countid(0);
		retCmd.set_countnum(pUser->m_buySummonNum);
		retCmd.set_begintime(1717055687);
		retCmd.set_endtime(1719734087);
	} else if (summonType == 2) {
		if (pUser->m_buyLimitSummonPlan){
			retCmd.set_hasbuysummonment(true);
		}
		else {
			retCmd.set_hasbuysummonment(false);
		}

		std::set<UINT16>::iterator it1 =  pUser->m_freeLimitSummonSet.begin();
		for (;it1 != pUser->m_freeLimitSummonSet.end();++it1){
			retCmd.add_freesummonrewardlist(*it1);	
		}
		std::set<UINT16>::iterator it2 =  pUser->m_rechargeLimitSummonSet.begin();
		for (;it2 != pUser->m_rechargeLimitSummonSet.end();++it2){
			retCmd.add_rechargesummonrewardlist(*it2);	
		}

		retCmd.set_summontype(summonType);
		retCmd.set_countid(0);
		retCmd.set_countnum(pUser->m_buyLimitSummonNum);
		retCmd.set_begintime(1717055687);
		retCmd.set_endtime(1719734087);
	}




	pUser->sendProto2Me(retCmd);

}

bool CSummonPlan::doSummonReward(GameUser *pUser, UINT32 countid ,UINT32 summontype ,UINT32 rewardtype)
{
	if (!pUser){
		return false;
	}

	MiscProto::stSummonRewardResult retCmd;

	// 检查背包剩余格子数量
    if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 2) {
		retCmd.set_result(MiscProto::stSummonRewardResult::eBAG_FULL);
		pUser->sendProto2Me(retCmd);
		return false;
    }

	retCmd.set_countid(countid);
	retCmd.set_summontype(summontype);
	retCmd.set_rewardtype(rewardtype);

	// if(pUser->getLevel() < val){
	// 	return false;
	// }
	// todo 计算是否可以领着一档  通过 count次数计算 跟表格 模仿 lv

	if (summontype == 1) {//钻石
		if (rewardtype == 1) {//免费
			if (pUser->m_freeSummonSet.find(countid) != pUser->m_freeSummonSet.end()){
				retCmd.set_result(MiscProto::stSummonRewardResult::eHAS_GET_REWARD);
				pUser->sendProto2Me(retCmd);
				return false;
			}

			pUser->m_freeSummonSet.insert(countid);
			std::map<UINT16,stSummonReward >::iterator it = m_cfgs.m_rewards.find(countid);
			if (it != m_cfgs.m_rewards.end()){
				stSummonReward & reward = it->second;

				if (reward.m_itemID < 30) 
				{
					pUser->addResource((eResource)reward.m_itemID, reward.m_num, "领取免费【钻石战令】召唤奖励!");
				}
				else
				{
					pUser->giveAssert(reward.m_itemID, reward.m_num, 0, "领取免费【钻石战令】召唤奖励");
				}
			}
		}
		else if (rewardtype == 2) {//收费
			if (!pUser->m_buySummonPlan){
				retCmd.set_result(MiscProto::stSummonRewardResult::eHAS_NOT_BUY_INVEST_PLAN);
				pUser->sendProto2Me(retCmd);
				return false;
			}

			if (pUser->m_rechargeSummonSet.find(countid) != pUser->m_rechargeSummonSet.end()){
				retCmd.set_result(MiscProto::stSummonRewardResult::eHAS_GET_REWARD);
				pUser->sendProto2Me(retCmd);
				return false;
			}

			pUser->m_rechargeSummonSet.insert(countid);
			std::map<UINT16,stSummonRechargeReward >::iterator it = m_cfgs.m_r_rewards.find(countid);
			if (it != m_cfgs.m_r_rewards.end()){
				stSummonRechargeReward & r_reward = it->second;
				if (r_reward.m_r_itemID < 30) 
				{
					pUser->addResource((eResource)r_reward.m_r_itemID, r_reward.m_r_num, "领取收费【钻石战令】召唤资源奖励!");
				}
				else
				{
					pUser->giveAssert(r_reward.m_r_itemID, r_reward.m_r_num, 0, "领取收费【钻石战令】召唤道具奖励");
				}
			}
		}
	} else if (summontype == 2) {//英雄
		if (rewardtype == 1) {//免费
			if (pUser->m_freeLimitSummonSet.find(countid) != pUser->m_freeLimitSummonSet.end()){
				retCmd.set_result(MiscProto::stSummonRewardResult::eHAS_GET_REWARD);
				pUser->sendProto2Me(retCmd);
				return false;
			}

			pUser->m_freeLimitSummonSet.insert(countid);
			std::map<UINT16,stSummonReward >::iterator it = m_cfgs.m_rewards.find(countid);
			if (it != m_cfgs.m_rewards.end()){
				stSummonReward & reward = it->second;

				if (reward.m_itemID < 30) 
				{
					pUser->addResource((eResource)reward.m_itemID, reward.m_num, "领取免费【限时战令】召唤奖励!");
				}
				else
				{
					pUser->giveAssert(reward.m_itemID, reward.m_num, 0, "领取免费【限时战令】召唤奖励");
				}
			}
		}
		else if (rewardtype == 2) {//收费
			if (!pUser->m_buyLimitSummonPlan){
				retCmd.set_result(MiscProto::stSummonRewardResult::eHAS_NOT_BUY_INVEST_PLAN);
				pUser->sendProto2Me(retCmd);
				return false;
			}

			if (pUser->m_rechargeLimitSummonSet.find(countid) != pUser->m_rechargeLimitSummonSet.end()){
				retCmd.set_result(MiscProto::stSummonRewardResult::eHAS_GET_REWARD);
				pUser->sendProto2Me(retCmd);
				return false;
			}

			pUser->m_rechargeLimitSummonSet.insert(countid);
			std::map<UINT16,stSummonRechargeReward >::iterator it = m_cfgs.m_r_rewards.find(countid);
			if (it != m_cfgs.m_r_rewards.end()){
				stSummonRechargeReward & r_reward = it->second;
				if (r_reward.m_r_itemID < 30) 
				{
					pUser->addResource((eResource)r_reward.m_r_itemID, r_reward.m_r_num, "领取收费【限时战令】召唤资源奖励!");
				}
				else
				{
					pUser->giveAssert(r_reward.m_r_itemID, r_reward.m_r_num, 0, "领取收费【限时战令】召唤道具奖励");
				}
			}
		}
	}
	

	retCmd.set_result(MiscProto::stSummonRewardResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);



	return true;
}

UINT32 CSummonPlan::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	return ret;
}
	
UINT32 CSummonPlan::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	return ret;
}
	
bool CSummonPlan::loadCfg(TiXmlElement * pNode)
{
	if (!pNode){
		return false;
	}
	
	const char *szVal = NULL;
	INT32 iVal = 0;

	if ((szVal = pNode->Attribute("Starttime"))){
		std::vector<std::string> strVec;
		g_stringtok(strVec,szVal,"-");
		if (strVec.size() >= 2){
			m_cfgs.m_beginTime = (UINT32)atoi(strVec[0].c_str());
			m_cfgs.m_endTime = (UINT32)atoi(strVec[1].c_str());
		}
	}

	if (pNode->Attribute("costResType",&iVal)){
		m_cfgs.m_resType = iVal;
	}
	
	if (pNode->Attribute("Price",&iVal)){
		m_cfgs.m_resNum = iVal;
	}

	TiXmlElement * pLvNode = pNode->FirstChildElement("SummonCount");
	while (pLvNode){
		UINT16 summoncount = 0;
		if (pLvNode->Attribute("id",&iVal)){
			summoncount = iVal;
		}

		stSummonReward reward;

		if (pLvNode->Attribute("itemId",&iVal)){
			reward.m_itemID = iVal;
		}

		if (pLvNode->Attribute("itemNum",&iVal)){
			reward.m_num = iVal;
		}

		stSummonRechargeReward r_reward;
		if (pLvNode->Attribute("boxId",&iVal)){
			r_reward.m_r_itemID = iVal;
		}

		if (pLvNode->Attribute("boxNum",&iVal)){
			r_reward.m_r_num = iVal;
		}

		m_cfgs.m_rewards.insert(std::make_pair(summoncount,reward));
		m_cfgs.m_r_rewards.insert(std::make_pair(summoncount,r_reward));

		pLvNode = pLvNode->NextSiblingElement("SummonCount");
	}

	return  true;	
}

CEatDinner::CEatDinner()
{

}

CEatDinner::~CEatDinner()
{

}

bool CEatDinner::doReward(GameUser *pUser,UINT32 val)
{
	struct tm _tm;
	RealTime::getLocalTime(_tm,g_gameServer->getSec());

	UINT32 sec = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;
	
	bool flag = false;
	UINT8  intervalIndex = 0;

	for (;intervalIndex < m_cfgs.m_timeLimits.size();++intervalIndex){
		if ((sec >= m_cfgs.m_timeLimits[intervalIndex].first) && (sec <= m_cfgs.m_timeLimits[intervalIndex].second)){
			flag = true;
			break;
		}	
	}
	
	MiscProto::stEatDinnerResult retCmd;

	if (!flag){
		retCmd.set_result(MiscProto::stEatDinnerResult::eTIME_LIMIT);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	
	UINT32 eatFlag = pUser->m_countMgr.getDayRefreshValue(ECOUT_EAT_BIG_DINNER);

	if (eatFlag & (1 << intervalIndex)){
		retCmd.set_result(MiscProto::stEatDinnerResult::eHAS_EAT);
		pUser->sendProto2Me(retCmd);
		return false;
	}

	/*if (pUser->m_countMgr.getDayRefreshValue(ECOUT_EAT_BIG_DINNER) >= m_cfgs.m_timeLimits.size()){
		retCmd.set_result(MiscProto::stEatDinnerResult::eHAS_EAT);
		pUser->sendProto2Me(retCmd);
		return false;
	}*/
	
	eatFlag |= (1 << intervalIndex);
	pUser->m_countMgr.resetValue(ECOUT_EAT_BIG_DINNER);
	pUser->m_countMgr.addDayRefreshValue(ECOUT_EAT_BIG_DINNER,eatFlag);
	pUser->sendProto2Me(retCmd);

	for (UINT8 i = 0;i < m_cfgs.m_rewards.size();++i){
		stRewardItem & reward = m_cfgs.m_rewards[i];
		if (g_selectByPercent(reward.m_rate)) {
			pUser->giveAssert(reward.m_itemID,reward.m_num,0,"吃大餐");
		}
	}
	//pUser->addSP(m_cfgs.m_sp,"吃大餐活动");
	pUser->addResource(eResource_Sp,m_cfgs.m_sp,"吃大餐活动");
    retCmd.set_haseatednum(intervalIndex + 1);
	retCmd.set_result(MiscProto::stEatDinnerResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
	return true;
}

UINT32 CEatDinner::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	return ret;
}

UINT32 CEatDinner::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	return ret;
}
	
bool CEatDinner::loadCfg(TiXmlElement * pNode)
{
	if (!pNode){
		return false;
	}
	const char * szVal = NULL;
	INT32 iVal = 0;

	if ((szVal = pNode->Attribute("Starttime"))){
		std::vector<std::string> strVec;
		g_stringtok(strVec,szVal,";");
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i],"-");
			if (subStrVec.size() >= 2){
				UINT32 beginTime = (UINT32)atoi(subStrVec[0].c_str());
				UINT32 endTime = (UINT32)atoi(subStrVec[1].c_str());
				m_cfgs.m_timeLimits.push_back(std::make_pair(beginTime,endTime));
			}
		}
	}
	
	if (pNode->Attribute("Physicalnum",&iVal)){
		m_cfgs.m_sp = iVal;
	}

	TiXmlElement * pItemNode = pNode->FirstChildElement("item");
	while (pItemNode){
		stRewardItem reward;
		if (pItemNode->Attribute("id",&iVal)){
			reward.m_itemID = iVal;
		}

		if (pItemNode->Attribute("itemnum",&iVal)){
			reward.m_num = iVal;
		}

		if (pItemNode->Attribute("rate",&iVal)){
			reward.m_rate = iVal;
		}
		m_cfgs.m_rewards.push_back(reward);
		pItemNode = pItemNode->NextSiblingElement("item");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("吃大餐配置,sp=%d",m_cfgs.m_sp);
	for (UINT16 i = 0;i < m_cfgs.m_timeLimits.size();++i){
		Athena::logger->trace("吃大餐时间限制:beginTime=%d,endTime=%d",m_cfgs.m_timeLimits[i].first,m_cfgs.m_timeLimits[i].second);
	}
	
	for (UINT16 i = 0;i < m_cfgs.m_rewards.size();++i){
		stRewardItem & reward = m_cfgs.m_rewards[i];
		Athena::logger->trace("吃大餐奖励:itemID=%d,num=%d,rate=%d",reward.m_itemID,reward.m_num,reward.m_rate);
	}
#endif
	return true;	
}

/*CEveryDayRechargeReward::CEveryDayRechargeReward()
{
	
}

CEveryDayRechargeReward::~CEveryDayRechargeReward()
{

}

bool CEveryDayRechargeReward::doReward(GameUser *pUser,UINT32 val)
{
	if (!pUser){
		return false;
	}

	MiscProto::stEveryDayRechargeRewardResult retCmd;
	retCmd.set_rechargenum(val);
	if (pUser->m_rewardSys.m_getRewardsRecharges.find(val) != pUser->m_rewardSys.m_getRewardsRecharges.end()){
		retCmd.set_result(MiscProto::stEveryDayRechargeRewardResult::eHAS_GET_REWARD);
		pUser->sendProto2Me(retCmd);
		return false;		
	}
	
	if (val > pUser->m_countMgr.getDayRefreshValue(ECOUT_TODAY_RECHARGE)){
		retCmd.set_result(MiscProto::stEveryDayRechargeRewardResult::eRECHARGE_NOT_ENOGUH);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	
	
	std::map<UINT32,UINT32>::iterator it = m_cfgs.m_rewards.find(val);
	if (it == m_cfgs.m_rewards.end()){
		retCmd.set_result(MiscProto::stEveryDayRechargeRewardResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
		Athena::logger->error("每日充值领取奖励时候错误,找不到配置");
		return false;
	}
	
	if (pUser->m_boxMgr.needSlotNum(it->second) > pUser->m_bagMgr.getMainFreeSlotNum()){
		retCmd.set_result(MiscProto::stEveryDayRechargeRewardResult::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return false;	
	}

	retCmd.set_result(MiscProto::stEveryDayRechargeRewardResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
	pUser->m_rewardSys.m_getRewardsRecharges.insert(val);
	pUser->m_boxMgr.openBox(it->second);
	return true;
}

UINT32 CEveryDayRechargeReward::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	*(UINT16*)(buf + ret) = pRewardSys->m_getRewardsRecharges.size();
	ret += sizeof(UINT16);

	std::set<UINT32>::iterator it = pRewardSys->m_getRewardsRecharges.begin();
	for (;it != pRewardSys->m_getRewardsRecharges.end();++it){
		*(UINT32*)(buf + ret) = *it;
		ret += sizeof(UINT32);
	}	
	return ret;
}

UINT32 CEveryDayRechargeReward::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	UINT16 size = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);
	
	for (UINT16 i = 0;i < size ; ++i){
		pRewardSys->m_getRewardsRecharges.insert(*(UINT32*)(buf + ret));
		ret += sizeof(UINT32);	
	}	
	return ret;
}

bool CEveryDayRechargeReward::loadCfg(TiXmlElement * pNode)
{
	if (!pNode){
		return false;
	}	
	
	const char * szVal = NULL;
	INT32 iVal = 0;

	if ((szVal = pNode->Attribute("Starttime"))){
		std::vector<std::string> strVec;
		g_stringtok(strVec,szVal,"-");
		if (strVec.size() >= 2){
			m_cfgs.m_beginTime = (UINT32)atoi(strVec[0].c_str());
			m_cfgs.m_endTime = (UINT32)atoi(strVec[1].c_str());
		}
	}	

	TiXmlElement * pConNode = pNode->FirstChildElement("Conditions");
	while (pConNode){
		UINT32 rechargeNum = 0;
		if (pConNode->Attribute("Rechargenum",&iVal)){
			rechargeNum = iVal;
		}
		
		UINT32 boxID = 0;

		if (pConNode->Attribute("boxid",&iVal)){
			boxID = iVal;
		}
		m_cfgs.m_rewards[rechargeNum] = boxID;
		pConNode = pConNode->NextSiblingElement("Conditions");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("单日充值配置:beginTime=%d,endTime=%d",m_cfgs.m_beginTime,m_cfgs.m_endTime);
	std::map<UINT32,UINT32>::iterator it = m_cfgs.m_rewards.begin();
	for (;it != m_cfgs.m_rewards.end();++it){
		Athena::logger->trace("rechargeNum=%d,boxID=%d",it->first,it->second);
	}
#endif
	return true;
}*/

/*CConsumeReward::CConsumeReward()
{
	
}
	
CConsumeReward::~CConsumeReward()
{
	
}

bool CConsumeReward::doReward(GameUser *pUser,UINT32 val)
{
	if (!pUser){
		return false;
	}

	MiscProto::stConsumeRewardResult retCmd;

	if (pUser->m_rewardSys.m_consumeRewards.find(val) != pUser->m_rewardSys.m_consumeRewards.end()){
		retCmd.set_result(MiscProto::stConsumeRewardResult::eHAS_GOT_REWARD);
		pUser->sendProto2Me(retCmd);
		return false;	
	}
	
	if (val > pUser->m_roleInfo.m_totlaConsume){
		retCmd.set_result(MiscProto::stConsumeRewardResult::eCONSUME_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
		return false;	
	}
	
	std::map<UINT32,UINT32>::iterator it = m_cfgs.m_rewards.find(val);
	
	if (it == m_cfgs.m_rewards.end()){
		return false;
	}
	
	if (pUser->m_boxMgr.needSlotNum(it->second) > pUser->m_bagMgr.getMainFreeSlotNum()){
		retCmd.set_result(MiscProto::stConsumeRewardResult::eBAG_FULL);
		pUser->sendProto2Me(retCmd);
		return false;
	}

	retCmd.set_consumenum(val);
	retCmd.set_result(MiscProto::stConsumeRewardResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
	pUser->m_rewardSys.m_consumeRewards.insert(val);
	
	m_cfgs.m_rewards.find(val);
	if (it != m_cfgs.m_rewards.end()){
		//pUser->giveAssert(reward.m_itemID,reward.m_itemNum,0,"消费奖励");
		pUser->m_boxMgr.openBox(it->second);
	}
	return true;
}
	
UINT32 CConsumeReward::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	*(UINT16 *)(buf + ret) = pRewardSys->m_consumeRewards.size();
	ret += sizeof(UINT16);

	std::set<UINT32>::iterator it = pRewardSys->m_consumeRewards.begin();

	for (;it != pRewardSys->m_consumeRewards.end();++it){
		*(UINT32*)(buf + ret) = *it;
		ret += sizeof(UINT32);
	}	
	return ret ;
}

UINT32 CConsumeReward::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	UINT16 size = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);

	for (UINT16 i = 0;i < size ; ++i){
		pRewardSys->m_consumeRewards.insert(*(UINT32*)(buf + ret));
		ret += sizeof(UINT32);
	}
	return ret ;
}
	
bool CConsumeReward::loadCfg(TiXmlElement * pNode)
{
	if (!pNode){
		return false;
	}

	const char * szVal = NULL;
	INT32  iVal = 0;
	
	if ((szVal = pNode->Attribute("Starttime"))){
		std::vector<std::string> strVec;
		g_stringtok(strVec,szVal,"-");
		if (strVec.size() >= 2){
			m_cfgs.m_beginTime = (UINT32)atoi(strVec[0].c_str());
			m_cfgs.m_endTime = (UINT32)atoi(strVec[1].c_str());
		}
	}

	TiXmlElement * pConNode = pNode->FirstChildElement("Conditions");
	while(pConNode){
		UINT32 consumeNum = 0;
		if (pConNode->Attribute("consumptionnum",&iVal)){
			consumeNum = iVal;
		}
		
		TiXmlElement *pEntryNode = pConNode->FirstChildElement("entry");
		if (pEntryNode){
			if (pEntryNode->Attribute("boxid",&iVal)){
				m_cfgs.m_rewards.insert(std::make_pair(consumeNum,iVal));	
			}		
		}
		pConNode = pConNode->NextSiblingElement("Conditions");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("消费奖励配置:beginTime=%d,endTime=%d",m_cfgs.m_beginTime,m_cfgs.m_endTime);
	std::map<UINT32,UINT32>::iterator it = m_cfgs.m_rewards.begin();
	for (;it != m_cfgs.m_rewards.end();++it){
		Athena::logger->trace("consumeNum=%d,boxID=%d",it->first,it->second);
	}
#endif
	return true;
}*/


/*CCumulativeLoginReward::CCumulativeLoginReward()
{

}
	
CCumulativeLoginReward::~CCumulativeLoginReward()
{

}

bool CCumulativeLoginReward::doReward(GameUser *pUser,UINT32 val)
{
	MiscProto::stCumulativeRewardResult retCmd;
	retCmd.set_day(val);	
	std::map<UINT32,stReward>::iterator it = m_cfg.m_rewards.find(val);
	if (it == m_cfg.m_rewards.end()){
		retCmd.set_result(MiscProto::stCumulativeRewardResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return false;
	}

	if (pUser->m_rewardSys.m_cumulativeRewards.find(val) !=  pUser->m_rewardSys.m_cumulativeRewards.end()){
		retCmd.set_result(MiscProto::stCumulativeRewardResult::eHas_Got_Reward);
		pUser->sendProto2Me(retCmd);
		return false;
	}

	if (pUser->m_rewardSys.m_cumulativeLogin < val){
		retCmd.set_result(MiscProto::stCumulativeRewardResult::eCond_Not_Meet);
		pUser->sendProto2Me(retCmd);
		return false;
	}
	
	if (pUser->m_boxMgr.needSlotNum(it->second.m_boxID) > pUser->m_bagMgr.getMainFreeSlotNum()){
		retCmd.set_result(MiscProto::stCumulativeRewardResult::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return false;
	}

	pUser->m_rewardSys.m_cumulativeRewards.insert(val);
	retCmd.set_result(MiscProto::stCumulativeRewardResult::eSuccess);
	pUser->sendProto2Me(retCmd);
	
	pUser->m_boxMgr.openBox(it->second.m_boxID);
	return true;
}

UINT32 CCumulativeLoginReward::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	*(UINT32 *)(buf + ret) = pRewardSys->m_yestodayFirstLoginTime;
	ret += sizeof(UINT32);

	*(UINT16 *)(buf + ret) = pRewardSys->m_cumulativeLogin;
	ret += sizeof(UINT16);

	*(UINT16 *)(buf + ret) = pRewardSys->m_cumulativeRewards.size();
	ret += sizeof(UINT16);

	std::set<UINT16>::iterator it =  pRewardSys->m_cumulativeRewards.begin();
	for (;it != pRewardSys->m_cumulativeRewards.end();++it){
		*(UINT16 *)(buf + ret) = *it;
		ret += sizeof(UINT16);
	}
	return ret;
}

UINT32 CCumulativeLoginReward::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	pRewardSys->m_yestodayFirstLoginTime = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	pRewardSys->m_cumulativeLogin = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);

	UINT16 num = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);
	
	for (UINT16 i = 0;i < num; ++i){
		UINT16 dayNum = *(UINT16*)(buf + ret);
		ret += sizeof(UINT16);
		pRewardSys->m_cumulativeRewards.insert(dayNum);
	}	
	return ret;
}

bool CCumulativeLoginReward::loadCfg(TiXmlElement * pNode)
{
	if (!pNode){
		return false;
	}
	
	INT32 iVal = 0;
	const char *szVal = NULL;

	if ((szVal = pNode->Attribute("time"))){
		struct tm _tm;
		if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)) {
			m_cfg.m_beginTime = mktime(&_tm) - 8 * 3600; 
		}
	}

	if (pNode->Attribute("lastime",&iVal)){
		m_cfg.m_lastTime = iVal;
	}

	TiXmlElement * pCondNode = pNode->FirstChildElement("Conditions");
	while(pCondNode){
		stReward reward;
		if (pCondNode->Attribute("Landingdays",&iVal)){
			reward.m_dayNum = iVal;
		}

		TiXmlElement *pEntryNode = pCondNode->FirstChildElement("entry");
		if (pEntryNode){
			if (pEntryNode->Attribute("boxid",&iVal)){
				reward.m_boxID = iVal;
			}	
		}
		m_cfg.m_rewards[reward.m_dayNum] = reward;
		pCondNode = pCondNode->NextSiblingElement("Conditions");
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印累积登录奖励信息:beginTime=%d,lasttime=%d,对应的奖励如下",m_cfg.m_beginTime,m_cfg.m_lastTime);
	std::map<UINT32,stReward>::iterator it = m_cfg.m_rewards.begin();
	for (;it != m_cfg.m_rewards.end();++it){
		Athena::logger->trace("累积登录天数:%d,奖励的boxID:%d",it->second.m_dayNum,it->second.m_boxID);	
	}
#endif
	return true;
}*/

CAllRewardMgr::CAllRewardMgr()
{
	m_subRewards[REWARD_TYPE_SIGN] = ATHENA_NEW CSignReward();  
	m_subRewards[REWARD_TYPE_OPEN_SVR] = ATHENA_NEW COpenSvrReward();
	m_subRewards[REWARD_TYPE_LEVEL] =  ATHENA_NEW CLevelReward();
	m_subRewards[REWARD_TYPE_ONLINE] = ATHENA_NEW COnlineReward();
	m_subRewards[REWARD_TYPE_INVESTPLAN] = ATHENA_NEW CInvestPlan();
	m_subRewards[REWARD_TYPE_EAT_DINNER] = ATHENA_NEW CEatDinner();
	//m_subRewards[REWARD_TYPE_EVERAYDAY_RECHARGE] = ATHENA_NEW CEveryDayRechargeReward();
	//m_subRewards[REWARD_TYPE_CONSUME] = ATHENA_NEW CConsumeReward();
	//m_subRewards[REWARD_TYPE_CUMULATIVE_LOGIN] = ATHENA_NEW CCumulativeLoginReward(); 
	m_subRewards[REWARD_TYPE_SUMMON] = ATHENA_NEW CSummonPlan();

}

CAllRewardMgr::~CAllRewardMgr()
{

}

UINT32 CAllRewardMgr::saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	ret += m_subRewards[REWARD_TYPE_SIGN]->saveData(pRewardSys,buf + ret,needSpace - ret);
	ret += m_subRewards[REWARD_TYPE_OPEN_SVR]->saveData(pRewardSys,buf + ret,needSpace - ret);
	ret += m_subRewards[REWARD_TYPE_LEVEL]->saveData(pRewardSys,buf + ret,needSpace - ret);
	ret += m_subRewards[REWARD_TYPE_ONLINE]->saveData(pRewardSys,buf + ret,needSpace - ret);
	ret += m_subRewards[REWARD_TYPE_INVESTPLAN]->saveData(pRewardSys,buf + ret,needSpace - ret);
	ret += m_subRewards[REWARD_TYPE_EAT_DINNER]->saveData(pRewardSys,buf + ret,needSpace - ret);
	//ret += m_subRewards[REWARD_TYPE_EVERAYDAY_RECHARGE]->saveData(pRewardSys,buf + ret,needSpace - ret);
	//ret += m_subRewards[REWARD_TYPE_CONSUME]->saveData(pRewardSys,buf + ret,needSpace - ret);
	//ret += m_subRewards[REWARD_TYPE_CUMULATIVE_LOGIN]->saveData(pRewardSys,buf + ret,needSpace - ret);
	ret += m_subRewards[REWARD_TYPE_SUMMON]->saveData(pRewardSys,buf + ret,needSpace - ret);

	return ret;
}

UINT32 CAllRewardMgr::loadData(CRewardSys *pRewardSys,byte * buf)
{
	UINT32 ret = 0;
	ret += m_subRewards[REWARD_TYPE_SIGN]->loadData(pRewardSys,buf + ret);
	ret += m_subRewards[REWARD_TYPE_OPEN_SVR]->loadData(pRewardSys,buf + ret);
	ret += m_subRewards[REWARD_TYPE_LEVEL]->loadData(pRewardSys,buf + ret);
	ret += m_subRewards[REWARD_TYPE_ONLINE]->loadData(pRewardSys,buf + ret);

	ret += m_subRewards[REWARD_TYPE_INVESTPLAN]->loadData(pRewardSys,buf + ret);
	ret += m_subRewards[REWARD_TYPE_EAT_DINNER]->loadData(pRewardSys,buf + ret);
	//ret += m_subRewards[REWARD_TYPE_EVERAYDAY_RECHARGE]->loadData(pRewardSys,buf + ret);
	//ret += m_subRewards[REWARD_TYPE_CONSUME]->loadData(pRewardSys,buf + ret);
	//ret += m_subRewards[REWARD_TYPE_CUMULATIVE_LOGIN]->loadData(pRewardSys,buf + ret);
	ret += m_subRewards[REWARD_TYPE_SUMMON]->loadData(pRewardSys,buf + ret);

	return ret;
}

bool CAllRewardMgr::doReward(UINT8 type,GameUser * pUser,UINT32 val)
{
	if (m_subRewards.find(type) == m_subRewards.end()){
		return false;
	}
	m_subRewards[type]->doReward(pUser,val);
	return true;
}

CRewardBase * CAllRewardMgr::getRewardByType(UINT8 type) {
    std::map<UINT8,CRewardBase *>::iterator it = m_subRewards.find(type);
    if (it != m_subRewards.end()) {
        return it->second;
    }
    return NULL;
}

bool CAllRewardMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/Reward.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[奖励系统配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("RewardConf");
	if (!pRoot){
		Athena::logger->error("[奖励系统配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}
	
	TiXmlElement * pSubRewardNode = pRoot->FirstChildElement("SubReward");
	while (pSubRewardNode){
		const char  * szVal = NULL;
		if ((szVal = pSubRewardNode->Attribute("index"))){
			std::string indexName = szVal;
			if (indexName == "OpenSvrReward"){
				m_subRewards[REWARD_TYPE_OPEN_SVR]->loadCfg(pSubRewardNode);		
			}
			else if (indexName == "SignInReward"){
				m_subRewards[REWARD_TYPE_SIGN]->loadCfg(pSubRewardNode);  
			}
			else if (indexName == "LevelReward"){
				m_subRewards[REWARD_TYPE_LEVEL]->loadCfg(pSubRewardNode);  
			}
			else if (indexName == "OnlineReward"){
				m_subRewards[REWARD_TYPE_ONLINE]->loadCfg(pSubRewardNode);  
			}

			else if (indexName == "Purchaseplan"){
				m_subRewards[REWARD_TYPE_INVESTPLAN]->loadCfg(pSubRewardNode);  
			}
			else if (indexName == "Eatdinner"){
				m_subRewards[REWARD_TYPE_EAT_DINNER]->loadCfg(pSubRewardNode);  
			}
			/*else if (indexName == "Rechargegift"){
				m_subRewards[REWARD_TYPE_EVERAYDAY_RECHARGE]->loadCfg(pSubRewardNode);  
			}
			else if (indexName == "consumption"){
				m_subRewards[REWARD_TYPE_CONSUME]->loadCfg(pSubRewardNode);  
			}
			else if (indexName == "TheCumulativeLanding"){
				m_subRewards[REWARD_TYPE_CUMULATIVE_LOGIN]->loadCfg(pSubRewardNode);
			}*/
			else if (indexName == "EquipmentRecruitment"){
				m_subRewards[REWARD_TYPE_SUMMON]->loadCfg(pSubRewardNode);  
			}
		}
		pSubRewardNode  = pSubRewardNode->NextSiblingElement("SubReward");
	}
	return true;	
}

CRewardSys::CRewardSys(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_signRewardNum = 0;
	m_lastSignRewardTime = 0;
	m_openSvrRewardNum = 0;
	m_lastOpenSvrRewardTime = 0;
	m_getOnlineRewardPassTime = 0;
	m_getOnlineRewardTime = 0;
	//m_cumulativeLogin = 0;
	//m_yestodayFirstLoginTime = 0;
}

CRewardSys::~CRewardSys()
{
}
	
UINT32 CRewardSys::saveData(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	ret = CAllRewardMgr::getSingleton().saveData(this,buf,needSpace);
	return ret;
}

UINT32 CRewardSys::loadData(byte * buf)
{
	UINT32 ret = 0;
	ret = CAllRewardMgr::getSingleton().loadData(this,buf);
	return ret;
}

void CRewardSys::exeEverySec()
{
	m_getOnlineRewardPassTime += 1;
	//Athena::logger->trace("在线奖励经过了时间:%d",m_getOnlineRewardPassTime);
}


void  CRewardSys::zeroRefresh()
{
	m_lastSignRewardTime = 0;
	m_lastOpenSvrRewardTime = 0;
}

void CRewardSys::userOnline()
{

	/*if (!g_isTheSameDay(g_gameServer->getSec(),m_yestodayFirstLoginTime)){//刷新累积登录时间
		m_yestodayFirstLoginTime = g_gameServer->getSec();
		m_cumulativeLogin += 1;
	}*/
	checkSignRefresh();

	MiscProto::stRetRewardInfo retCmd;
	retCmd.set_getsignrewardnum(m_signRewardNum);
	retCmd.set_hasgetsignreward(g_isTheSameDay(g_gameServer->getSec(),m_lastSignRewardTime));

	retCmd.set_getopensvrrewardnum(m_openSvrRewardNum);
	retCmd.set_hasgetopensvrreward(g_isTheSameDay(g_gameServer->getSec(),m_lastOpenSvrRewardTime));
	std::set<UINT16>::iterator it = m_hasRewardLv.begin();
	for (;it != m_hasRewardLv.end();++it){
		retCmd.add_hasgetrewardlv(*it);	
	}
	
	if (!g_isTheSameDay(g_gameServer->getSec(),m_getOnlineRewardTime)){
		m_getOnlineRewardPassTime = 0;
		m_onlineRewards.clear();
		m_getOnlineRewardTime = g_gameServer->getSec();
	}
	retCmd.set_onlinetimes(m_getOnlineRewardPassTime);
	std::set<UINT8>::iterator subIt =  m_onlineRewards.begin();
	for (;subIt != m_onlineRewards.end();++subIt){
		retCmd.add_hasgetonlinereward(*subIt);	
	}
	m_pOwner->sendProto2Me(retCmd);


	//发送活动奖励数据
	MiscProto::stSynActivityData retCmd1;
	if (m_pOwner->m_buyInvestPlan){
		retCmd1.set_hasbuyinvestment(true);
	}
	else {
		retCmd1.set_hasbuyinvestment(false);
	}
	
	std::set<UINT16>::iterator it1 =  m_getInvestRewardLvs.begin();
	for (;it1 != m_getInvestRewardLvs.end();++it1){
		retCmd1.add_investgetrewardlv(*it1);	
	}
	
	/*std::set<UINT32>::iterator it2 =  m_getRewardsRecharges.begin();

	for (;it2 != m_getRewardsRecharges.end();++it2){
		retCmd1.add_rechargereward(*it2);	
	}

	std::set<UINT32>::iterator it3 = m_consumeRewards.begin();	
	for (;it3 != m_consumeRewards.end(); ++it3){
		retCmd1.add_consumereward(*it3);	
	}*/
	
    UINT8 eatNum = 0;
    CEatDinner * pEat = (CEatDinner *)CAllRewardMgr::getSingleton().getRewardByType(REWARD_TYPE_EAT_DINNER);
    if (pEat) {
        struct tm _tm;
        RealTime::getLocalTime(_tm,g_gameServer->getSec());

        UINT32 sec = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;
        
        UINT8 intervalIndex = 0;
        UINT32 maxNum = 0;
        bool inEatTime = false;
        UINT8 leftIndex = 0, rightIndex = 0;
        CEatDinner::EatDinnerCfg cfgs = pEat->getCfgs();
        for (; intervalIndex < cfgs.m_timeLimits.size();++intervalIndex){
            maxNum++;
            if (sec < cfgs.m_timeLimits[intervalIndex].first) {
                leftIndex = intervalIndex + 1;
            }
            if (sec > cfgs.m_timeLimits[intervalIndex].second) {
                rightIndex = intervalIndex + 1;
            }
            if ((sec >= cfgs.m_timeLimits[intervalIndex].first) && (sec <= cfgs.m_timeLimits[intervalIndex].second)){
                inEatTime = true;
                break;
            }	
        }
        UINT32 eatFlag = m_pOwner->m_countMgr.getDayRefreshValue(ECOUT_EAT_BIG_DINNER);
        if (maxNum > 0) {
            if (inEatTime) {
                if (eatFlag & (1 << (maxNum - 1))) {//只看最后一次
                    eatNum = maxNum;
                } else {
                    eatNum = maxNum - 1;
                }
            } else {
                if (leftIndex) {
                    eatNum = cfgs.m_timeLimits.size() - leftIndex;
                }
                if (rightIndex) {
                    eatNum = rightIndex;
                }
            }
        }
#ifdef _HDZ_DEBUG
        Athena::logger->trace("eatFlag is %d, maxNum is %d, eatNum is %d, inEatTime is %s, leftIndex is %d, rightIndex is %d",
                eatFlag, maxNum, eatNum, (inEatTime ? "true" : "false"), leftIndex, rightIndex);
#endif
    }
	retCmd1.set_eatdinnernum(eatNum);
	
	/*retCmd1.set_cumulativeloginnum(m_cumulativeLogin);
	std::set<UINT16>::iterator it4 =  m_cumulativeRewards.begin();
	for (;it4 != m_cumulativeRewards.end(); ++it4){
		retCmd1.add_rewardcumulative(*it4);	
	}*/

	m_pOwner->sendProto2Me(retCmd1);
}

void CRewardSys::checkSignRefresh()
{
	if (!m_pOwner->m_countMgr.getMonthRefreshValue(ECOUNT_SIGN_RESET)){
		m_pOwner->m_countMgr.addMonthRefreshValue(ECOUNT_SIGN_RESET,1);
		m_signRewardNum = 0;
	}	
}
