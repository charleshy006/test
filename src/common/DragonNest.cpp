#include "DragonNest.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"


bool CDragonNestCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/DragonNest.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[巨龙巢穴]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[巨龙巢穴]找不到节点root");
		return false;
	}

	TiXmlElement * pDragon = pRoot->FirstChildElement("DragonNest");
    if (!pDragon) {
        Athena::logger->error("[巨龙巢穴]找不到节点DragonNest");
        return false;
    }
	TiXmlElement * pLevel = pDragon->FirstChildElement("Level");
    if (!pLevel) {
        Athena::logger->error("[巨龙巢穴]找不到节点Level");
        return false;
    }
    int val;
    char * szVal = NULL;
    std::vector<std::string> weekVec;
    DragonNestLvl dragonInfo;
    while (pLevel) {
        if (!pLevel->Attribute("indexID", &val)) {
            Athena::logger->error("[巨龙巢穴]indexID节点找不到indexID");
            return false;
        }
        dragonInfo.index = val;
        if (!pLevel->Attribute("Levelid", &val)) {
            Athena::logger->error("[巨龙巢穴]Levelid节点找不到Levelid");
            return false;
        }
        dragonInfo.lvlId = val;
        if (!pLevel->Attribute("First", &val)) {
            Athena::logger->error("[巨龙巢穴]First节点找不到First");
            return false;
        }
        dragonInfo.firstLvl = (bool)val;
        if (!pLevel->Attribute("NextLevel", &val)) {
            Athena::logger->error("[巨龙巢穴]NextLevel节点找不到NextLevel");
            return false;
        }
        dragonInfo.nextLvl = val;
        szVal = (char *)pLevel->Attribute("Openweek");
        if (!szVal) {
            Athena::logger->error("[巨龙巢穴]Openweek节点找不到Openweek");
            return false;
        }
        val = 0;
        weekVec.clear();
        g_stringtok(weekVec, szVal, ":");
        for (size_t i = 0; i < weekVec.size(); i++) {
            val |= (1 << (UINT32)atoi(weekVec[i].c_str()));
        }
        dragonInfo.openWeek = val;
        m_dragonMap.insert(std::make_pair(dragonInfo.lvlId, dragonInfo));
        pLevel = pLevel->NextSiblingElement("Level");
    }
	TiXmlElement * pReset = pRoot->FirstChildElement("Diamondreset");
    if (!pReset) {
        Athena::logger->error("[巨龙巢穴]找不到节点Diamondreset");
        return false;
    }
	TiXmlElement * pDiamond = pReset->FirstChildElement("Diamond");
    if (!pDiamond) {
        Athena::logger->error("[巨龙巢穴]找不到节点Diamond");
        return false;
    }
    while(pDiamond) {
        if (!pDiamond->Attribute("Diamondnum", &val)) {
            Athena::logger->error("[巨龙巢穴]Diamondnum节点找不到Diamondnum");
            return false;
        }
        m_costVec.push_back(val);
        pDiamond = pDiamond->NextSiblingElement("Diamond");
    }
#ifdef _HDZ_DEBUG
    std::stringstream ss;
    for (std::map<UINT32, DragonNestLvl>::iterator it = m_dragonMap.begin();
            it != m_dragonMap.end(); it++) {
        for (UINT32 i = 0; i < 8; i++) {
            if (it->second.openWeek & (1 << i)) {
                ss <<  i << " ";
            }
        }
        Athena::logger->trace("[巨龙巢穴]index is %d, lvlId is %d, first lvl is %d, nextLvl is %d, openWeek is %s",
                it->second.index, it->second.lvlId, it->second.firstLvl, it->second.nextLvl, ss.str().c_str());
        ss.str("");
    }
    for (size_t i = 0; i < m_costVec.size(); i++) {
        Athena::logger->trace("[巨龙巢穴]cost diamond is %d", m_costVec[i]);
    }
#endif
    return true;
}

UINT32 CDragonNestCfg::getCost(UINT32 times) {
    if (!m_costVec.size()) {
        Athena::logger->error("[巨龙巢穴]m_costVec 为空");
        return (UINT32)-1;
    }
    if (times >= m_costVec.size()) {
        return m_costVec[m_costVec.size() - 1];
    }
    return m_costVec[times];
}

const CDragonNestCfg::DragonNestLvl * CDragonNestCfg::getDragonNest(UINT32 lvlId) {
    std::map<UINT32, DragonNestLvl>::iterator it = m_dragonMap.find(lvlId);
    if (it != m_dragonMap.end()) {
        return &it->second;
    }
    return NULL;
}

UINT32 CDragonNestCfg::getPreLvlId(UINT32 lvlId) {
    for (std::map<UINT32, DragonNestLvl>::iterator it = m_dragonMap.begin();
            it != m_dragonMap.end(); it++) {
        if (it->second.nextLvl == lvlId) {
            return it->first;
        }
    }
    return 0;
}
