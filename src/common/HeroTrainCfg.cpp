#include "HeroTrainCfg.h"
#include "Log4cxxHelp.h" 
#include "KVPair.h"      
#include "UtilFunction.h"

bool CHeroTrainCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/herotrain.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[随从调教]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[随从调教]抽奖配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	const char * szVal = NULL;
	TiXmlElement * pEntryNode = pRoot->FirstChildElement("constant");
	if(pEntryNode){
		TiXmlElement * pExpNode = pEntryNode->FirstChildElement("entry");
		if(pExpNode){
			int initTrainLv = 0;
			if(pExpNode->Attribute("inittrainlevel",&initTrainLv))
			{
				m_initHeroTrainLvl = initTrainLv;
#ifdef _HDZ_DEBUG
				Athena::logger->trace("init hero train lvl is %d", m_initHeroTrainLvl);
#endif
			}
			int maxLvl = 0;
			if (pExpNode->Attribute("maxtrainlevel", &maxLvl)){
				m_maxHeroTrainLvl = maxLvl;
#ifdef _HDZ_DEBUG
				Athena::logger->trace("max hero train lvl is %d", m_maxHeroTrainLvl);
#endif
			}
		}

		TiXmlElement * pAwardNode = pEntryNode->FirstChildElement("levelaward");
		if(pAwardNode){
			TiXmlElement * pEntryNode = pAwardNode->FirstChildElement("entry");
			while(pEntryNode){
				stTrainLvAward one;
				
				if (pEntryNode->Attribute("heroid",&iVal)){
					one.m_heroID = iVal;
				}

				if (pEntryNode->Attribute("trainlv",&iVal)){
					one.m_level = iVal;
				}

				if ((szVal = pEntryNode->Attribute("award"))){
					std::vector<std::string> strVec;
					g_stringtok(strVec, szVal, ";");
					std::vector<std::string>::iterator vecIt = strVec.begin();
					for(;vecIt != strVec.end(); ++vecIt){
						stTrainLvAwardItem elem;
						std::vector<std::string> strVec2;
						g_stringtok(strVec2, *vecIt, "-");
						if(strVec2.size() >= 3){
							elem.m_type = (UINT8)atoi(strVec2[0].c_str());
							elem.m_id = (UINT32)atoi(strVec2[1].c_str());
							elem.m_count = (UINT32)atoi(strVec2[2].c_str());

							one.m_awards.push_back(elem);
						}
					}

				}
				m_trainLvAwards[one.m_heroID][one.m_level] = one;
				pEntryNode = pEntryNode->NextSiblingElement("entry"); 
			}
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[随从调教]加载等级奖励表:");
		std::map<UINT32,std::map<UINT32,stTrainLvAward> >::iterator mapIt = m_trainLvAwards.begin();
		for(;mapIt != m_trainLvAwards.end(); ++mapIt){
			std::map<UINT32,stTrainLvAward>::iterator secondIt = mapIt->second.begin();
			for(;secondIt != mapIt->second.end(); ++secondIt){
				std::vector<stTrainLvAwardItem>::iterator vecIt = secondIt->second.m_awards.begin();
				for(;vecIt != secondIt->second.m_awards.end(); ++vecIt){
					Athena::logger->trace("		[随从调教等级奖励]heroid:%d, level:%d, awardType:%d, awarditemid:%d, count:%d", mapIt->first, secondIt->first,vecIt->m_type,vecIt->m_id,vecIt->m_count);
				}
			}
		}
	}

#endif

	return true;
}

stTrainLvAward* CHeroTrainCfg::GetAwardCfg(UINT32 heroid, UINT32 level){
	std::map<UINT32,std::map<UINT32,stTrainLvAward> >::iterator it = m_trainLvAwards.find(heroid);
	if(it == m_trainLvAwards.end()){
		return NULL;
	}

	std::map<UINT32,stTrainLvAward>::iterator itSecond = it->second.find(level);
	if(itSecond == it->second.end()){
		return NULL;
	}

	return &itSecond->second;
}