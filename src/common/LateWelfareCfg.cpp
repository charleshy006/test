#include "LateWelfareCfg.h"

#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool CLateWelfareCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/latewelfare.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[限时福利]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[限时福利]找不到节点root");
		return false;
	}

	TiXmlElement * pConf = pRoot->FirstChildElement("conf");
    if (!pConf) {
        Athena::logger->error("[限时福利]找不到节点conf");
        return false;
    }
    int val = 0;
    if (!pConf->Attribute("IsOpen", &val)) {
        Athena::logger->error("[限时福利]conf节点找不到IsOpen");
        return false;
    }
    m_isOpen = (bool)val;

	const char * szVal = NULL;
	TiXmlElement * pGruop = pConf->FirstChildElement("Gruop");
    if (!pGruop) {
        Athena::logger->error("[限时福利]找不到节点Gruop");
        return false;
    }

    while (pGruop) {
        WalfareItem item;
        if (!pGruop->Attribute("id", &val)) {
            Athena::logger->error("[限时福利]Gruop 节点找不到id");
            return false;
        }
        item.id = val;

        if (!(szVal = pGruop->Attribute("EntryTime"))) {
            Athena::logger->error("[限时福利]Gruop 节点找不到EntryTime");
            return false;
        }
        std::vector<std::string> strVec;
        g_stringtok(strVec, szVal, "-");
        if (strVec.size() != 2) {
            Athena::logger->error("[限时福利]GruopTime格式错误");
            return false;
        }
        item.startDay = (UINT32)atoi(strVec[0].c_str());
        item.duration = (UINT32)atoi(strVec[1].c_str());

        if (!pGruop->Attribute("ActivityTime", &val)) {
            Athena::logger->error("[限时福利]Gruop 节点找不到ActivityTime");
            return false;
        }
        item.activityTime = val;

        if (!pGruop->Attribute("Lv", &val)) {
            Athena::logger->error("[限时福利]Gruop 节点找不到Lv");
            return false;
        }
        item.showLv = val;

        TiXmlElement * pEntry = pGruop->FirstChildElement("entry");
        while (pEntry) {
            RewardItem rewardItem;
            if (!pEntry->Attribute("id", &val)) {
                Athena::logger->error("[限时福利]entry 节点找不到id");
                return false;
            }
            rewardItem.id = val;
            if (!pEntry->Attribute("CostDiamond", &val)) {
                Athena::logger->error("[限时福利]entry 节点找不到CostDiamond");
                return false;
            }
            rewardItem.costDiamond = val;

            if (!(szVal = pEntry->Attribute("RewardItem"))) {
                Athena::logger->error("[限时福利]entry 节点找不到RewardItem");
                return false;
            }
            strVec.clear();
            g_stringtok(strVec, szVal, ":");
            for (size_t i = 0; i < strVec.size(); i++) {
                std::vector<std::string> itemVec;
                g_stringtok(itemVec, strVec[i], "-");
                if (itemVec.size() != 2) {
                    Athena::logger->error("[限时福利]RewardItem数据格式错误");
                    return false;
                }
                RewardInfo reward;
                reward.itemId = (UINT32)atoi(itemVec[0].c_str());
                reward.num = (UINT32)atoi(itemVec[1].c_str());
                rewardItem.rewardVec.push_back(reward);
            }
            item.itemMap.insert(std::make_pair(rewardItem.id, rewardItem));
            pEntry = pEntry->NextSiblingElement("entry");
        }
        m_walfareMap.insert(std::make_pair(item.id, item));
        pGruop = pGruop->NextSiblingElement("Gruop");
    }

#ifdef _HDZ_DEBUG
    Athena::logger->trace("[限时福利]is open %d", m_isOpen);
    for (std::map<UINT32, WalfareItem>::iterator it = m_walfareMap.begin();
            it != m_walfareMap.end(); it++) {
        Athena::logger->trace("[限时福利]id %d, startDay %d, duration %d, activityTime %d, showLv %d",
                it->second.id, it->second.startDay, it->second.duration, it->second.activityTime, it->second.showLv);
        std::map<UINT32, RewardItem> & itemMap = it->second.itemMap;
        for (std::map<UINT32, RewardItem>::iterator subIt = itemMap.begin();
                subIt != itemMap.end(); subIt++) {
            Athena::logger->trace("[限时福利]id %d, CostDiamond %d", subIt->second.id, subIt->second.costDiamond);
            std::vector<RewardInfo> & rewardVec = subIt->second.rewardVec;
            for (size_t i = 0; i < rewardVec.size(); i++) {
                Athena::logger->trace("[限时福利]item %d, id %d", rewardVec[i].itemId, rewardVec[i].num);
            }
        }
    }
#endif
    return true;
}

bool CLateWelfareCfg::getWalfare(UINT32 day, WalfareItem & item) {
    for (std::map<UINT32, WalfareItem>::iterator it = m_walfareMap.begin();
            it != m_walfareMap.end(); it++) {
        if (it->second.startDay <= day && (day < it->second.startDay + it->second.duration)) {
            item = it->second;
            return true;
        }
    }
    return false;
}

bool CLateWelfareCfg::getRewardItem(UINT32 activityId, UINT32 rewardId, RewardItem & item) {
    std::map<UINT32, WalfareItem>::iterator it = m_walfareMap.find(activityId);
    if (it != m_walfareMap.end()) {
        std::map<UINT32, RewardItem> & itemMap = it->second.itemMap;
        std::map<UINT32, RewardItem>::iterator subIt = itemMap.find(rewardId);
        if (subIt != itemMap.end()) {
            item = subIt->second;
            return true;
        }
    }
    return false; 
}
