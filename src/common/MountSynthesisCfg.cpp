#include "MountSynthesisCfg.h"
#include "Log4cxxHelp.h" 
#include "KVPair.h"      
#include "UtilFunction.h"

bool CMountSynthesisCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/mountSynthesis.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[坐骑装备]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[坐骑装备]抽奖配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	TiXmlElement * pSynthesisNode = pRoot->FirstChildElement("Synthesis");
	if(pSynthesisNode){
		TiXmlElement * pEntryNode = pSynthesisNode->FirstChildElement("entry");
		while(pEntryNode){
			stSynthesis	one;
			
			if (pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}

			if (pEntryNode->Attribute("itemID",&iVal)){
				one.m_itemID = iVal;
			}

			if (pEntryNode->Attribute("ResultitemID",&iVal)){
				one.m_resultitemID = iVal;
			}

			if (pEntryNode->Attribute("synthesisnum",&iVal)){
				one.m_synthesisNum = iVal;
			}

			if (pEntryNode->Attribute("cost",&iVal)){
				one.m_cost = iVal;
			}
			
			m_synthesisMap.insert(std::make_pair(one.m_itemID, one));
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

	TiXmlElement * pEvolvementNode = pRoot->FirstChildElement("evolvement");
	if(pEvolvementNode){
		TiXmlElement * pEntryNode = pEvolvementNode->FirstChildElement("entry");
		while(pEntryNode){
			stEvolvement one;
			
			if (pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}

			if (pEntryNode->Attribute("item1ID",&iVal)){
				one.m_item1ID = iVal;
			}

			if (pEntryNode->Attribute("item2ID",&iVal)){
				one.m_item2ID = iVal;
			}

			if(pEntryNode->Attribute("ResultitemID",&iVal)){
				one.m_resultitemID = iVal;
			}

			if(pEntryNode->Attribute("cost",&iVal)){
				one.m_cost = iVal;
			}
		
			m_evolvementVec.push_back(one);
			pEntryNode = pEntryNode->NextSiblingElement("entry"); 
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[坐骑装备]加载合成表:");
	std::map<UINT32, stSynthesis>::iterator mapIt = m_synthesisMap.begin();
	for(;mapIt != m_synthesisMap.end(); ++mapIt){
		stSynthesis & one = mapIt->second;
		Athena::logger->trace("		[坐骑装备]id:%d, itemID:%d, ResultitemID:%d, synthesisnum:%d, cost:%d", one.m_id, one.m_itemID, one.m_resultitemID, one.m_synthesisNum, one.m_cost);
	}

	Athena::logger->trace("[坐骑装备]加载强化表:");
	std::vector<stEvolvement>::iterator vecIt = m_evolvementVec.begin();
	for(;vecIt != m_evolvementVec.end(); ++vecIt){
		stEvolvement & one = *vecIt;
		Athena::logger->trace("     [坐骑装备]id:%d, item1ID:%d, item2ID:%d, ResultitemID:%d, cost:%d", one.m_id, one.m_item1ID, one.m_item2ID, one.m_resultitemID, one.m_cost);
	}
#endif

	return true;
}

stSynthesis * CMountSynthesisCfg::getSynthesisByitemID(UINT32 itemID)
{
	std::map<UINT32, stSynthesis>::iterator it = m_synthesisMap.find(itemID);
	if(it == m_synthesisMap.end()){
		return NULL;
	}

	return &it->second;
}

stEvolvement * CMountSynthesisCfg::getEvolvementByitemID(UINT32 item1ID, UINT32 item2ID)
{
	std::vector<stEvolvement>::iterator it = m_evolvementVec.begin();
	for(;it != m_evolvementVec.end(); ++it){
		stEvolvement & one = *it;
		if((one.m_item1ID == item1ID) && (one.m_item2ID == item2ID)){
			return &one;
		}

		if((one.m_item1ID == item2ID) && (one.m_item2ID == item1ID)){
			return &one;
		}
	}
	return NULL;
}
