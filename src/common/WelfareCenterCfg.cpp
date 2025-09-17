#include "WelfareCenterCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include <tinyxml/tinyxml.h>

WelfareCenterCfg::WelfareCenterCfg()
{
	
}

WelfareCenterCfg::~WelfareCenterCfg()
{

}

bool WelfareCenterCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/welfarecenter.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[应用宝福利中心配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("conf");
	if (!pRoot){
		Athena::logger->error("[应用宝福利中心配置]宝箱配置加载错误,找不到conf节点!");
		return false;
	}

	INT32 iVal = 0;
	TiXmlElement * pConditionNode = pRoot->FirstChildElement("condition");
	while (pConditionNode){
		stCondition cond;
		if (pConditionNode->Attribute("taskid",&iVal)){
			cond.m_taskID = iVal;
		}
		
		if (pConditionNode->Attribute("awardid",&iVal)){
			cond.m_awardID = iVal;
		}
		
		if (pConditionNode->Attribute("level",&iVal)){
			cond.m_level = iVal;
		}


		TiXmlElement * pRewardNode = pConditionNode->FirstChildElement("reward");
		while (pRewardNode){
			stRewardItem reward;
			if (pRewardNode->Attribute("itemid",&iVal)){
				reward.m_id = iVal;
			}
			
			if (pRewardNode->Attribute("itemnum",&iVal)){
				reward.m_num = iVal;
			}
			cond.m_rewardVec.push_back(reward);
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
		m_conditions.push_back(cond);
		pConditionNode = pConditionNode->NextSiblingElement("condition");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("腾讯福利中心礼包:");
	for (UINT32 i = 0;i < m_conditions.size();++i){
		stCondition & cond = m_conditions[i];
		Athena::logger->trace("taskID:%d,awardid:%d,level:%d",cond.m_taskID,cond.m_awardID,cond.m_level);
		for (UINT32 j = 0;j < cond.m_rewardVec.size();++j){
			stRewardItem & reward = cond.m_rewardVec[j]	;
			Athena::logger->trace("奖励id:%d,num:%d",reward.m_id,reward.m_num);
		}
	}
#endif
	return true;
}

WelfareCenterCfg::stCondition * WelfareCenterCfg::getCondition(UINT32 taskID,UINT32 awardID)
{
	for (UINT32 i = 0;i < m_conditions.size();++i){
		stCondition & cond =   m_conditions[i];
		if ((taskID == cond.m_taskID) && (awardID == cond.m_awardID)){
			return & cond;
		}
	}
	return NULL;
}
