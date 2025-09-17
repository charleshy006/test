#include "OpenSvrPowerRankCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include <tinyxml/tinyxml.h>

OpenSvrPowerRankCfg::OpenSvrPowerRankCfg()
{
	
}

OpenSvrPowerRankCfg::~OpenSvrPowerRankCfg()
{

}

bool OpenSvrPowerRankCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/powerreward.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[开服战力排行]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[开服战力排行]配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	

	TiXmlElement * pPowerRewardNode = pRoot->FirstChildElement("PowerReward");
	if (pPowerRewardNode){
		TiXmlElement * pRewardNode = pPowerRewardNode->FirstChildElement("Reward");
		while (pRewardNode){
			stReward reward;
			if (pRewardNode->Attribute("Startrank",&iVal)){
				reward.m_minRank = iVal;
			}
			if (pRewardNode->Attribute("Endrank",&iVal)){
				reward.m_maxRank = iVal;
			}
			if (pRewardNode->Attribute("Boxid",&iVal)){
				reward.m_boxID = iVal;
			}
			m_rewardVec.push_back(reward);
			pRewardNode = pRewardNode->NextSiblingElement("Reward");
		}
	}

	 pPowerRewardNode = pRoot->FirstChildElement("PowerReward2");
	if (pPowerRewardNode){
		TiXmlElement * pRewardNode = pPowerRewardNode->FirstChildElement("Reward");
		while (pRewardNode){
			stReward reward;
			if (pRewardNode->Attribute("Startrank",&iVal)){
				reward.m_minRank = iVal;
			}
			if (pRewardNode->Attribute("Endrank",&iVal)){
				reward.m_maxRank = iVal;
			}
			if (pRewardNode->Attribute("Boxid",&iVal)){
				reward.m_boxID = iVal;
			}
			m_rewardVec2.push_back(reward);
			pRewardNode = pRewardNode->NextSiblingElement("Reward");
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印开服战力排行相关信息:");
	
	for (UINT16 i = 0;i < m_rewardVec.size();++i){
		stReward & reward = m_rewardVec[i];
		Athena::logger->trace("minRank:%d,maxRank:%d,boxID:%d",reward.m_minRank,reward.m_maxRank,reward.m_boxID);
	}

	Athena::logger->trace("打印开服战力排行2相关信息:");
	
	for (UINT16 i = 0;i < m_rewardVec2.size();++i){
		stReward & reward = m_rewardVec2[i];
		Athena::logger->trace("minRank:%d,maxRank:%d,boxID:%d",reward.m_minRank,reward.m_maxRank,reward.m_boxID);
	}
#endif
	return true;
}

OpenSvrPowerRankCfg::stReward * OpenSvrPowerRankCfg::getOpenSvrRankReward(UINT16 rank)
{
	for (UINT16 i = 0;i < m_rewardVec.size();++i){
		stReward & reward = m_rewardVec[i];
		if ((rank >= reward.m_minRank) && (rank <= reward.m_maxRank)){
			return &reward;
		}
	}
	return NULL;
}

OpenSvrPowerRankCfg::stReward * OpenSvrPowerRankCfg::getOpenSvrRankReward2(UINT16 rank)
{
	for (UINT16 i = 0;i < m_rewardVec2.size();++i){
		stReward & reward = m_rewardVec2[i];
		if ((rank >= reward.m_minRank) && (rank <= reward.m_maxRank)){
			return &reward;
		}
	}
	return NULL;
}
