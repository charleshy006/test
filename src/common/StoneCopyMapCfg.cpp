#include "StoneCopyMapCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"


bool CStoneCopyMapCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/gem.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[宝石副本]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[宝石副本]找不到节点root");
		return false;
	}

	TiXmlElement * pGemLevelNode = pRoot->FirstChildElement("GemLevel");
    if (!pGemLevelNode) {
        Athena::logger->error("[宝石副本]找不到节点GemLevel");
        return false;
    }
	TiXmlElement * pLevel = pGemLevelNode->FirstChildElement("Level");
    if (!pLevel) {
        Athena::logger->error("[宝石副本]找不到节点Level");
        return false;
    }

    INT32 val = 0;
    
	stConfigurationInfo info;
    
	while (pLevel) {
        if (!pLevel->Attribute("indexID", &val)) {
            Athena::logger->error("[宝石副本]indexID节点找不到indexID");
            return false;
        }

        info.index = val;
        
		if (!pLevel->Attribute("Levelid", &val)) {
            Athena::logger->error("[宝石副本]Levelid节点找不到Levelid");
            return false;
        }

        info.lvlId = val;
        
		if (!pLevel->Attribute("First", &val)) {
            Athena::logger->error("[宝石副本]First节点找不到First");
            return false;
        }

        info.firstLvl = (bool)val;
        
		if (!pLevel->Attribute("NextLevel", &val)) {
            Athena::logger->error("[宝石副本]NextLevel节点找不到NextLevel");
            return false;
        }

        info.nextLvl = val;
        
		m_infoMap.insert(std::make_pair(info.lvlId, info));
        
		pLevel = pLevel->NextSiblingElement("Level");
    }

	TiXmlElement * pReset = pRoot->FirstChildElement("Diamondreset");
    if (!pReset) {
        Athena::logger->error("[宝石副本]找不到节点Diamondreset");
        return false;
    }

	TiXmlElement * pDiamond = pReset->FirstChildElement("Diamond");
    if (!pDiamond) {
        Athena::logger->error("[宝石副本]找不到节点Diamond");
        return false;
    }

    while(pDiamond) {
        if (!pDiamond->Attribute("Diamondnum", &val)) {
            Athena::logger->error("[宝石副本]Diamondnum节点找不到Diamondnum");
            return false;
        }
        m_costVec.push_back(val);
        pDiamond = pDiamond->NextSiblingElement("Diamond");
    }
#ifdef _HDZ_DEBUG
    for (std::map<UINT32, stConfigurationInfo>::iterator it = m_infoMap.begin();
            it != m_infoMap.end(); it++) {
        Athena::logger->trace("[宝石副本]index is %d, lvlId is %d, first lvl is %d, nextLvl is %d",
it->second.index, it->second.lvlId, it->second.firstLvl, it->second.nextLvl);
    }
    for (size_t i = 0; i < m_costVec.size(); i++) {
        Athena::logger->trace("[宝石副本]cost diamond is %d", m_costVec[i]);
    }
#endif
    return true;
}

UINT32 CStoneCopyMapCfg::getCost(UINT32 times) {
    if (!m_costVec.size()) {
        Athena::logger->error("[宝石副本]m_costVec 为空");
        return (UINT32)-1;
    }
    if (times >= m_costVec.size()) {
        return m_costVec[m_costVec.size() - 1];
    }
    return m_costVec[times];
}

const CStoneCopyMapCfg::stConfigurationInfo * CStoneCopyMapCfg::getConfigurationInfo(UINT32 lvlId) {
    std::map<UINT32, stConfigurationInfo>::iterator it = m_infoMap.find(lvlId);
    if (it != m_infoMap.end()) {
        return &it->second;
    }
    return NULL;
}

UINT32 CStoneCopyMapCfg::getPreLvlId(UINT32 lvlId) {
    for (std::map<UINT32, stConfigurationInfo>::iterator it = m_infoMap.begin();
            it != m_infoMap.end(); it++) {
        if (it->second.nextLvl == lvlId) {
            return it->first;
        }
    }
    return 0;
}
