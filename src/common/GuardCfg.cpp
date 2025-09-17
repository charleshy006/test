#include "GuardCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include <tinyxml/tinyxml.h>

GuardCfg::GuardCfg()
{
	
}

GuardCfg::~GuardCfg()
{

}

bool GuardCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/escort.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[护送任务配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[护送任务配置]配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	TiXmlElement * pBasicNumNode = pRoot->FirstChildElement("basisnum");
	if (pBasicNumNode){
		TiXmlElement * pNumNode = pBasicNumNode->FirstChildElement("num");
		if (pNumNode){
			if (pNumNode->Attribute("EscortNum",&iVal)){
				m_basicNum.m_guardNum = iVal;	
			}

			if (pNumNode->Attribute("InterceptNum",&iVal)){
				m_basicNum.m_interceptNum = iVal;
			}

			if (pNumNode->Attribute("HelpNum",&iVal)){
				m_basicNum.m_helpNum = iVal;
			}
			
			if (pNumNode->Attribute("InterceptCD",&iVal)){
				m_basicNum.m_interceptCD = iVal;
			}
			
			if (pNumNode->Attribute("ByInterceptNum",&iVal)){
				m_basicNum.m_beInterceptNum = iVal;
			}
		}
	}

	TiXmlElement * pEscortInfoNode = pRoot->FirstChildElement("escortinfo");

	if (pEscortInfoNode){
		TiXmlElement * pInfoNode = pEscortInfoNode->FirstChildElement("info");
		while (pInfoNode){
			stGuardInfo info;
			if (pInfoNode->Attribute("id",&iVal)){
				info.m_id = iVal;
			}
			if (pInfoNode->Attribute("Quality",&iVal)){
				info.m_quality = iVal;		
			}
			if (pInfoNode->Attribute("EscortTime",&iVal)){
				info.m_guardTime = iVal;	
			}
			if (pInfoNode->Attribute("ByInterceptNum",&iVal)){
				info.m_byInterceptNum = iVal;	
			}
			if (pInfoNode->Attribute("RewardType1",&iVal)){
				info.m_resID1 = iVal;	
			}
			if (pInfoNode->Attribute("RewardNum1",&iVal)){
				info.m_resNum1 = iVal;	
			}
			if (pInfoNode->Attribute("RewardType2",&iVal)){
				info.m_resID2 = iVal;
			}
			if (pInfoNode->Attribute("RewardNum2",&iVal)){
				info.m_resNum2 = iVal;
			}
			if (pInfoNode->Attribute("RewardType3",&iVal)){
				info.m_resID3 = iVal;
			}
			if (pInfoNode->Attribute("RewardNum3",&iVal)){
				info.m_resNum3 = iVal;
			}
			m_guardInfos[info.m_quality] = info;
			pInfoNode = pInfoNode->NextSiblingElement("info");
		}
	}
	
	TiXmlElement * pInterceptrewardNode = pRoot->FirstChildElement("interceptreward");
	if (pInterceptrewardNode){
		TiXmlElement * pRewardNode = pInterceptrewardNode->FirstChildElement("reward");
		while (pRewardNode){
			stInterceptReward reward;
			if (pRewardNode->Attribute("StartLevel",&iVal)){
				reward.m_mixLv = iVal;
			}
			
			if (pRewardNode->Attribute("EndLevel",&iVal)){
				reward.m_maxLv = iVal;
			}
		
			if (pRewardNode->Attribute("RewardPercent1",&iVal)){
				reward.m_rewardPer1 = iVal;
			}
			
			if (pRewardNode->Attribute("RewardPercent2",&iVal)){
				reward.m_rewardPer2 = iVal;
			}
			m_interceptRewards.push_back(reward);
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
	}

	TiXmlElement * pHelpRewardNode = pRoot->FirstChildElement("helpreward");
	if (pHelpRewardNode){
		TiXmlElement * pRewardNode = pHelpRewardNode->FirstChildElement("reward");
		while (pRewardNode){
			UINT16 isByIntercept = 0;
			UINT16 rewardPer1 = 0;
			UINT16 rewardPer2 = 0;

			if (pRewardNode->Attribute("IsByIntercept",&iVal)){
				isByIntercept = iVal;
			}

			if (pRewardNode->Attribute("RewardPercent1",&iVal)){
				rewardPer1 = iVal;
			}
			
			if (pRewardNode->Attribute("RewardPercent2",&iVal)){
				rewardPer2 = iVal;
			}
			
			if (!isByIntercept){
				m_winHelpReward.m_rewardPer1 = rewardPer1;
				m_winHelpReward.m_rewardPer2 = rewardPer2;
			}
			else {
				m_loseHelpReward.m_rewardPer1 = rewardPer1;
				m_loseHelpReward.m_rewardPer2 = rewardPer2;
			}
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
	}

	TiXmlElement * pEscortreFreshNode = pRoot->FirstChildElement("escortrefresh");
	if (pEscortreFreshNode){
		TiXmlElement * pRefreshNode = pEscortreFreshNode->FirstChildElement("refresh");
		while (pRefreshNode){
			UINT16 quality = 0;
			UINT32 rate = 0;
			
			if (pRefreshNode->Attribute("Quality",&iVal)){
				quality = iVal;	
			}
			
			if (pRefreshNode->Attribute("RefreshSuccessPercent",&iVal)){
				rate = iVal;	
			}
			m_quality2Rates[quality] = rate;
			pRefreshNode = pRefreshNode->NextSiblingElement("refresh");
		}
	}
	
	TiXmlElement * pEscortComsumeNode = pRoot->FirstChildElement("escortcomsume");
	if (pEscortComsumeNode){
		TiXmlElement * pConsumeNode = pEscortComsumeNode->FirstChildElement("comsume");
		if (pConsumeNode){
			if (pConsumeNode->Attribute("refreshdiamondnum",&iVal)){
				m_guardConsume.m_refreshDiamond = iVal;	
			}
			if (pConsumeNode->Attribute("calldiamondnum",&iVal)){
				m_guardConsume.m_callDiamond = iVal;
			}
			if (pConsumeNode->Attribute("inspirediamondnum",&iVal)){
				m_guardConsume.m_inspireDiamond = iVal;
			}
			if (pConsumeNode->Attribute("quickdiamondnum",&iVal)){
				m_guardConsume.m_quickDiamond = iVal;
			}
			if (pConsumeNode->Attribute("clearCDdiamondnum",&iVal)){
				m_guardConsume.m_clearCDDiamond = iVal;
			}
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印运镖相关信息:");

	Athena::logger->trace("基本信息:guardNum:%d,basicNum:%d,interceptNum:%d",m_basicNum.m_guardNum,m_basicNum.m_helpNum,m_basicNum.m_interceptNum);
	
	Athena::logger->trace("运镖相关信息:");
	std::map<UINT16,stGuardInfo >::iterator it = m_guardInfos.begin();
	for (;it != m_guardInfos.end();++it){
		stGuardInfo & info = it->second;
		Athena::logger->trace("id:%d,guardTime:%d,m_byInterceptNum:%d,quality:%d,resID1:%d,resNum1:%d,resID2:%d,resNum2:%d,resID3:%d,resNum3:%d",info.m_id,info.m_guardTime,info.m_byInterceptNum,info.m_quality,info.m_resID1,info.m_resNum1,info.m_resID2,info.m_resNum2,info.m_resID3,info.m_resNum3);
	}

	Athena::logger->trace("拦截奖励");
	for (UINT16 i = 0;i < m_interceptRewards.size();++i){
		stInterceptReward & reward = m_interceptRewards[i];	
		Athena::logger->trace("minLV:%d,maxLv:%d,per1:%d,per2:%d",reward.m_mixLv,reward.m_maxLv,reward.m_rewardPer1,reward.m_rewardPer2);
	}

	Athena::logger->trace("胜利协助奖励:per1:%d,per2:%d",m_winHelpReward.m_rewardPer1,m_winHelpReward.m_rewardPer2);
	Athena::logger->trace("失败协助奖励:per1:%d,per2:%d",m_loseHelpReward.m_rewardPer1,m_loseHelpReward.m_rewardPer2);
	
	Athena::logger->trace("刷新品质概率");
	std::map<UINT16,UINT32>::iterator subIt = m_quality2Rates.begin();
	for (;subIt != m_quality2Rates.end();++subIt){
		Athena::logger->trace("quality:%d,rates:%d",subIt->first,subIt->second);
	}

	Athena::logger->trace("护送消耗:");
	Athena::logger->trace("m_callDiamond:%d,m_inspireDiamond:%d,m_quickDiamond:%d,m_refreshDiamond:%d,clearCDdiamondnum:%d",m_guardConsume.m_callDiamond,m_guardConsume.m_inspireDiamond,m_guardConsume.m_quickDiamond,m_guardConsume.m_refreshDiamond,m_guardConsume.m_clearCDDiamond);
#endif
	return true;
}

GuardCfg::stGuardInfo * GuardCfg::getGuardInfo(UINT16 quality)
{
	std::map<UINT16,stGuardInfo >::iterator it = m_guardInfos.find(quality);	

	if (it != m_guardInfos.end()){
		return &it->second;
	}
	return NULL;
}

UINT32 GuardCfg::getRefreshQualityRate(UINT16 quality)
{
	std::map<UINT16,UINT32>::iterator it = m_quality2Rates.find(quality);	

	if (it != m_quality2Rates.end()){
		return it->second;
	}
	return 0;
}

GuardCfg::stInterceptReward * GuardCfg::getInterceptReward(UINT16 lvDiff)
{
	for (UINT16 i = 0;i < m_interceptRewards.size();++i){
		stInterceptReward & reward = m_interceptRewards[i];

		if ((lvDiff >= reward.m_mixLv) && (lvDiff <= reward.m_maxLv)){
			return & reward;
		}
	}
	return NULL;
}
