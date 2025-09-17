#include "MarryCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool CMarryCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/marry.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[结婚场景]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[结婚场景]找不到节点root");
		return false;
	}

	TiXmlElement * pMarry = pRoot->FirstChildElement("Marry");
    if (!pMarry) {
        Athena::logger->error("[结婚场景]找不到节点Marry");
        return false;
    }
    
	TiXmlElement * pType = pMarry->FirstChildElement("Type");
    if (!pType) {
        Athena::logger->error("[结婚场景]找不到节点Type");
        return false;
    }
    int val = 0;
    while (pType) {
        MarryInfo marryInfo;
        if (!pType->Attribute("id", &val)) {
            Athena::logger->error("[结婚场景]Type节点找不到id");
            return false;
        }
        marryInfo.id = val;
        if (!pType->Attribute("Levelid", &val)) {
            Athena::logger->error("[结婚场景]Type节点找不到Levelid");
            return false;
        }
        marryInfo.levelId = val;
        if (!pType->Attribute("Boxid", &val)) {
            Athena::logger->error("[结婚场景]Type节点找不到Boxid");
            return false;
        }
        marryInfo.boxId = val;
        if (!pType->Attribute("Cost", &val)) {
            Athena::logger->error("[结婚场景]Type节点找不到Cost");
            return false;
        }
        marryInfo.cost = val;
        if (!pType->Attribute("CostType", &val)) {
            Athena::logger->error("[结婚场景]Type节点找不到CostType");
            return false;
        }
        marryInfo.costType = val;
        if (!pType->Attribute("Sweetid", &val)) {
            Athena::logger->error("[结婚场景]Type节点找不到Sweetid");
            return false;
        }
        marryInfo.candyId = val;
        if (!pType->Attribute("Sweetnum", &val)) {
            Athena::logger->error("[结婚场景]Type节点找不到Sweetnum");
            return false;
        }
        marryInfo.maxDrawTimes = val;
        m_marryInfoMap.insert(std::make_pair(marryInfo.id, marryInfo));
        pType = pType->NextSiblingElement("Type");
    }

	TiXmlElement * pFWshop = pRoot->FirstChildElement("FWshop");
    if (!pFWshop) {
        Athena::logger->error("[结婚场景]找不到节点FWshop");
        return false;
    }

	TiXmlElement * pFirework = pFWshop->FirstChildElement("Firework");
    if (!pFirework) {
        Athena::logger->error("[结婚场景]找不到节点Firework");
        return false;
    }
    while (pFirework) {
        FireWorkInfo fwInfo;
        if (!pFirework->Attribute("id", &val)) {
            Athena::logger->error("[结婚场景]Firework节点找不到id");
            return false;
        }
        fwInfo.id = val;
        if (!pFirework->Attribute("itemid", &val)) {
            Athena::logger->error("[结婚场景]Firework节点找不到itemid");
            return false;
        }
        fwInfo.itemId = val;
        if (!pFirework->Attribute("Cost", &val)) {
            Athena::logger->error("[结婚场景]Firework节点找不到Cost");
            return false;
        }
        fwInfo.cost = val;
        if (!pFirework->Attribute("CostType", &val)) {
            Athena::logger->error("[结婚场景]Firework节点找不到CostType");
            return false;
        }
        fwInfo.costType = val;
        m_fireWorkInfoMap.insert(std::make_pair(fwInfo.id, fwInfo));
        pFirework = pFirework->NextSiblingElement("Firework");
    }

	TiXmlElement * pDivorce = pRoot->FirstChildElement("Divorce");
    if (!pDivorce) {
        Athena::logger->error("[结婚场景]找不到节点Divorce");
        return false;
    }

	TiXmlElement * pEntry = pDivorce->FirstChildElement("entry");
    if (!pEntry) {
        Athena::logger->error("[结婚场景]找不到节点entry");
        return false;
    }
    while (pEntry) {
        DivorceInfo dvInfo;
        if (!pEntry->Attribute("id", &val)) {
            Athena::logger->error("[结婚场景]entry节点找不到id");
            return false;
        }
        dvInfo.id = val;
        if (!pEntry->Attribute("Cost", &val)) {
            Athena::logger->error("[结婚场景]entry节点找不到Cost");
            return false;
        }
        dvInfo.cost = val;
        if (!pEntry->Attribute("CostType", &val)) {
            Athena::logger->error("[结婚场景]entry节点找不到CostType");
            return false;
        }
        dvInfo.costType = val;
        if (!pEntry->Attribute("Time", &val)) {
            Athena::logger->error("[结婚场景]entry节点找不到Time");
            return false;
        }
        dvInfo.time = val;
        m_divorceInfoMap.insert(std::make_pair(dvInfo.id, dvInfo));
        pEntry = pEntry->NextSiblingElement("entry");
    }

#ifdef _HDZ_DEBUG
    for (std::map<UINT32, MarryInfo>::iterator it = m_marryInfoMap.begin();
            it != m_marryInfoMap.end(); it++) {
        Athena::logger->trace("[结婚场景] MarryInfo : id %d, levelId %d, boxId %d, cost %d, costType %d, candyId %d, maxDrawTimes %d",
                it->second.id, it->second.levelId, it->second.boxId, it->second.cost, it->second.costType, it->second.candyId, it->second.maxDrawTimes); 
    }
    for (std::map<UINT32, FireWorkInfo>::iterator it = m_fireWorkInfoMap.begin();
            it != m_fireWorkInfoMap.end(); it++) {
        Athena::logger->trace("[结婚场景] FireWorkInfo : id %d, itemId %d, cost %d, costType %d",
                it->second.id, it->second.itemId, it->second.cost, it->second.costType);
    }
    for (std::map<UINT32, DivorceInfo>::iterator it = m_divorceInfoMap.begin();
            it != m_divorceInfoMap.end(); it++) {
        Athena::logger->trace("[结婚场景] FireWorkInfo : id %d, cost %d, costType %d, time %d",
                it->second.id, it->second.cost, it->second.costType, it->second.time);
    }
#endif

    return true;
}

bool CMarryCfg::getMarryInfo(UINT32 id, MarryInfo & info) {
    std::map<UINT32, MarryInfo>::iterator it = m_marryInfoMap.find(id);
    if (it != m_marryInfoMap.end()) {
        info = it->second;
        return true;
    }
    return false;
}

bool CMarryCfg::getFwInfo(UINT32 id, FireWorkInfo & info) {
    std::map<UINT32, FireWorkInfo>::iterator it = m_fireWorkInfoMap.find(id);
    if (it != m_fireWorkInfoMap.end()) {
        info = it->second;
        return true;
    }
    return false;
}

UINT32 CMarryCfg::getIndexOfFw(UINT32 itemId) {
    for (std::map<UINT32, FireWorkInfo>::iterator it =
            m_fireWorkInfoMap.begin(); it != m_fireWorkInfoMap.end(); it++) {
        if (itemId == it->second.itemId) {
            return it->first;
        }
    }
    return 0;
}

bool CMarryCfg::getDivorceInfo(bool isForce, DivorceInfo & divorceInfo) {
    UINT32 id = 0;
    if (isForce) {
        id = Divorce_Force;
    } else {
        id = Divorce_Common;
    }
    std::map<UINT32, DivorceInfo>::iterator it = m_divorceInfoMap.find(id);
    if (it != m_divorceInfoMap.end()) {
        divorceInfo = it->second;
        return true;
    }
    return false;
}
