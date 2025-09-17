#include "RuneCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool CRuneCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/Rune.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[符文]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[符文]找不到节点root");
		return false;
	}

	TiXmlElement * pSyn = pRoot->FirstChildElement("synthetic");
    if (!pSyn) {
        Athena::logger->error("[符文]找不到节点synthetic");
        return false;
    }
	TiXmlElement * pEntry = pSyn->FirstChildElement("entry");
    if (!pEntry) {
        Athena::logger->error("[符文]找不到节点entry");
        return false;
    }
    int val = 0;
    RequireItem requireItem;
    CombineItem combineItem;
    while (pEntry) {
        if (!pEntry->Attribute("id", &val)) {
            Athena::logger->error("[符文]entry节点找不到id");
            return false;
        }
        requireItem.m_id = val;
        if (!pEntry->Attribute("materialID", &val)) {
            Athena::logger->error("[符文]entry节点找不到materialID");
            return false;
        }
        requireItem.m_pieceId = val;
        if (!pEntry->Attribute("Firenum", &val)) {
            Athena::logger->error("[符文]entry节点找不到Firenum");
            return false;
        }
        requireItem.m_soulFire = val;

        TiXmlElement * pResult = pEntry->FirstChildElement("result");
        if (!pResult) {
            Athena::logger->error("[符文]找不到节点result");
            return false;
        }
        while (pResult) {
            if (!pResult->Attribute("id", &val)) {
                Athena::logger->error("[符文]result节点找不到id");
                return false;
            }
            combineItem.m_id = val;
            if (!pResult->Attribute("resultID", &val)) {
                Athena::logger->error("[符文]result节点找不到resultID");
                return false;
            }
            combineItem.m_runeId = val;
            if (!pResult->Attribute("RP", &val)) {
                Athena::logger->error("[符文]result节点找不到RP");
                return false;
            }
            combineItem.m_rate = val;
            requireItem.m_totalRate += val;
            requireItem.m_combineVec.push_back(combineItem);
            pResult = pResult->NextSiblingElement("result");
        }
        m_runeCombineMap.insert(std::make_pair(requireItem.m_pieceId, requireItem));
        requireItem.m_totalRate = 0;
        requireItem.m_combineVec.clear();
        pEntry = pEntry->NextSiblingElement("entry");
    }
#ifdef _HDZ_DEBUG
    for (std::map<UINT32, RequireItem>::iterator it = m_runeCombineMap.begin();
            it != m_runeCombineMap.end(); it++) {
        Athena::logger->trace("[符文]id is %d, pieceId is %d, sourFire is %d, totalRate is %d",
                it->second.m_id, it->second.m_pieceId, it->second.m_soulFire, it->second.m_totalRate);
        std::vector<CombineItem> & combineVec = it->second.m_combineVec;
        for (size_t i = 0; i < combineVec.size(); i++) {
            Athena::logger->trace("[符文]id is %d, runeId is %d, rate is %d",
                    combineVec[i].m_id, combineVec[i].m_runeId, combineVec[i].m_rate);
        }
    }
#endif
    return true;
}

CRuneCfg::RequireItem * CRuneCfg::getRequireItem(UINT32 itemId) {
    std::map<UINT32, CRuneCfg::RequireItem>::iterator it = m_runeCombineMap.find(itemId);
    if (it != m_runeCombineMap.end()) {
        return &it->second;
    }
    return NULL;
}
