#include "LevelWflfareCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include <string>
#include "UtilFunction.h"

CLevelWflfareCfg::CLevelWflfareCfg()
{
	m_size = 0;
}

CLevelWflfareCfg::~CLevelWflfareCfg()
{

}

bool CLevelWflfareCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string levelWflfareFile = Athena::global["confDir"] + "/levelwflfare.xml";
	if (!doc.LoadFile(levelWflfareFile.c_str())){
		Athena::logger->error("[冲级好礼配置]打开配置文件%s错误",levelWflfareFile.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[冲级好礼配置]读取配置文件找不到节点<root>");
		return false;
	}

	INT32 iVal = 0, level = 0;
	const char * szVal = NULL;

	TiXmlElement * pGroupNode = pRoot->FirstChildElement("group");
	while (pGroupNode){
		if(pGroupNode->Attribute("lv", &iVal)){
			level = iVal;	
		}
		std::map<UINT16, stWflfareInfo> reward;
		TiXmlElement * pEntryNode = pGroupNode->FirstChildElement("entry");
		while (pEntryNode){
			stWflfareInfo one;
			if (pEntryNode->Attribute("type",&iVal)){
				one.m_type = iVal;
			}
			
			if (pEntryNode->Attribute("costDiamond",&iVal)){
				one.m_costDiamond = iVal;
			}

			if((szVal = pEntryNode->Attribute("rewardItem")))
			{
				std::vector<std::string> strVec;
				g_stringtok(strVec,szVal,":");
				for(UINT16 i = 0; i < strVec.size(); ++i){
					std::vector<std::string> strSubVec;
					g_stringtok(strSubVec,strVec[i].c_str(),"-");
					if(strSubVec.size() >= 2){
						UINT32 id = (UINT32)atoi(strSubVec[0].c_str());
						UINT32 num = (UINT32)atoi(strSubVec[1].c_str());
						one.m_rewardItem[id] = num;
					}
				}
			}
			reward[one.m_type] = one;
			++m_size;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
		m_reward[level] = reward;
		pGroupNode = pGroupNode->NextSiblingElement("group");
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("冲级好礼配置");
	std::map<UINT16, std::map<UINT16, stWflfareInfo> >::iterator it = m_reward.begin();
	for(;it != m_reward.end(); ++it){
		std::map<UINT16, stWflfareInfo>::iterator subIt = it->second.begin();
		for(;subIt != it->second.end(); ++subIt){
			stWflfareInfo one = subIt->second;
			std::map<UINT32, UINT32>::iterator mapIt  = one.m_rewardItem.begin();
			std::ostringstream os ;
			for(;mapIt != one.m_rewardItem.end(); ++mapIt){
				os<<mapIt->first<<"-"<<mapIt->second<<":";
			}
			Athena::logger->trace("level=%d, type=%d, costDiamond=%d, rewardItem=%s", it->first, one.m_type, one.m_costDiamond, os.str().c_str());
		}
	}

	Athena::logger->trace("冲级好礼数量:%d", m_size);
#endif
	return true;
}

bool CLevelWflfareCfg::getWflfareInfoByLevel(std::map<UINT16, stWflfareInfo> & rewardMap,UINT16 level)
{
	std::map<UINT16, std::map<UINT16, stWflfareInfo> >::iterator it =  m_reward.find(level);
	if(it != m_reward.end()){
		rewardMap = it->second;
		return true;
	}
	return false;
}


