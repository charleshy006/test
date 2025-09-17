#include "AIConfigMgr.h"
#include <tinyxml/tinyxml.h>
#include "Log4cxxHelp.h"
#include "KVPair.h"


CAIConfigMgr::CAIConfigMgr()
{

}


CAIConfigMgr::~CAIConfigMgr()
{

}

bool CAIConfigMgr::loadCfg()
{

	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/AI.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[AI配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[AI配置]商店配置加载错误,找不到Root节点!");
		return false;
	}  

	INT32 iVal = 0;
	TiXmlElement * pAINode = pRoot->FirstChildElement("AI");
	while (pAINode){
		stAI * pAI = ATHENA_NEW stAI();
		if (pAINode->Attribute("AIid",&iVal)){
			pAI->m_id = iVal;	
		}

		if (pAINode->Attribute("Speed",&iVal)){
			pAI->m_speed = iVal;
		}

		TiXmlElement * pDistanceNode = pAINode->FirstChildElement("Distance");
		while (pDistanceNode){
			stStatus status;
			if (pDistanceNode->Attribute("MinDistance",&iVal)){
				status.m_distance.m_minDistance = iVal;	
			}

			if (pDistanceNode->Attribute("MaxDistance",&iVal)){
				status.m_distance.m_maxDistance = iVal;	
			}
			
			if (pDistanceNode->Attribute("AIStatus",&iVal)){
				status.m_distance.m_aiStatus = iVal;
			}

			TiXmlElement * pActionNode = pDistanceNode->FirstChildElement("Action");
			while (pActionNode){
				stAction action;
				if (pActionNode->Attribute("actionID",&iVal)){
					action.m_actionID = iVal;
				}
				
				if (pActionNode->Attribute("skillType",&iVal)){
					action.m_skillType = iVal;
				}

				if (pActionNode->Attribute("skillStage",&iVal)){
					action.m_skillState = iVal;
				}
				
				if (pActionNode->Attribute("skillCD",&iVal)){
					action.m_skillCD = iVal;
				}

				if (pActionNode->Attribute("skillProbability",&iVal)){
					action.m_rate = iVal;
				}
				status.m_actions.push_back(action);
				pActionNode = pActionNode->NextSiblingElement("Action");
			}

			pAI->m_statuses.push_back(status);
			pDistanceNode = pDistanceNode->NextSiblingElement("Distance");
		}
		m_allAIs[pAI->m_id] = pAI;
		pAINode = pAINode->NextSiblingElement("AI");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印AI配置:");
	std::map<UINT32,stAI *>::iterator it = m_allAIs.begin();
	for (;it != m_allAIs.end();++it){
		stAI * pAI = it->second;
		Athena::logger->trace("AIID=%d,speed=%d",pAI->m_id,pAI->m_speed);
		Athena::logger->trace("AIStatus :");
		
		for (UINT16 i = 0;i < pAI->m_statuses.size();++i){
			stStatus & status = pAI->m_statuses[i];
			Athena::logger->trace("minDis=%d,maxDis=%d,status=%d",status.m_distance.m_minDistance,status.m_distance.m_maxDistance,status.m_distance.m_aiStatus);
			for (UINT16 j = 0;j < status.m_actions.size();++j){
				stAction & action = status.m_actions[i]	;
				Athena::logger->trace("actionID=%d,skillType=%d,skillStage=%d,skillCD=%d,rate=%d",action.m_actionID,action.m_skillType,action.m_skillState,action.m_skillCD,action.m_rate);
			}
		}
	}
#endif
	return true;
}

CAIConfigMgr::stAI * CAIConfigMgr::getAIByID(UINT32 id)
{
	std::map<UINT32,stAI * >::iterator it = m_allAIs.find(id);
	if (it == m_allAIs.end()){
		return NULL;
	}	
	return it->second;
}
