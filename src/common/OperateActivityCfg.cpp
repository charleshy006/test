#include "OperateActivityCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool COperateActivityCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/operateactivity.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[新加活动]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[新加活动]找不到节点root");
		return false;
	}

	TiXmlElement * pActivity = pRoot->FirstChildElement("activity");
    if (!pActivity) {
        Athena::logger->error("[新加活动]找不到节点activity");
        return false;
    }
    int val = 0;
	const char * szVal = NULL;
    while (pActivity) {
        ActivityItem item;
        if (!pActivity->Attribute("id", &val)) {
            Athena::logger->error("[新加活动]activity节点找不到id");
            return false;
        }
        item.id = val;
        if (!pActivity->Attribute("ActivityType", &val)) {
            Athena::logger->error("[新加活动]activity节点找不到ActivityType");
            return false;
        }
        m_type2IdMap.insert(std::make_pair(val, item.id));
        if (!(szVal = pActivity->Attribute("StartTime"))) {
            Athena::logger->error("[新加活动]activity节点找不到StartTime");
            return false;
        }
        struct tm _tm;
        if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
             item.start = mktime(&_tm) ;
        }   
        if (!(szVal = pActivity->Attribute("EndTime"))) {
            Athena::logger->error("[新加活动]activity节点找不到EndTime");
            return false;
        }
        if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
             item.end = mktime(&_tm) ;
        }   
        TiXmlElement * pEntry = pActivity->FirstChildElement("entry");
        if (!pEntry) {
            Athena::logger->error("[新加活动]找不到节点entry");
            return false;
        }
        while (pEntry) {
            ItemInfo itemInfo;
            if (!pEntry->Attribute("id", &val)) {
                Athena::logger->error("[新加活动]entry节点找不到id");
                return false;
            }
            itemInfo.id = val;
            if (!pEntry->Attribute("ActivityCondition", &val)) {
                Athena::logger->error("[新加活动]entry节点找不到ActivityCondition");
                return false;
            }
            itemInfo.condition = val;
            if (!pEntry->Attribute("RewardBoxID", &val)) {
                Athena::logger->error("[新加活动]entry节点找不到RewardBoxID");
                return false;
            }
            itemInfo.boxId = val;
            item.itemInfos.insert(std::make_pair(itemInfo.id, itemInfo));
            pEntry = pEntry->NextSiblingElement("entry");
        }
        m_activityMap.insert(std::make_pair(item.id, item));
        pActivity = pActivity->NextSiblingElement("activity");
    }
#ifdef _HDZ_DEBUG
    for (std::map<UINT8, UINT8>::iterator it = m_type2IdMap.begin();
            it != m_type2IdMap.end(); it++) {
        Athena::logger->trace("[新加活动]type is %d, id is %d", it->first, it->second);
    }
#endif
    return true;
}

void COperateActivityCfg::loadActiveIds(UINT32 now, std::map<UINT8, std::list<UINT8> > &idMap) {
    for (std::map<UINT8, ActivityItem>::iterator it = m_activityMap.begin();
            it != m_activityMap.end(); it++) {
        if (it->second.start <= now && now <= it->second.end) {
            std::list<UINT8> idList;
            for (std::map<UINT8, ItemInfo>::iterator subIt = it->second.itemInfos.begin();
                    subIt != it->second.itemInfos.end(); subIt++) {
                idList.push_back(subIt->first);
            }
            idMap.insert(std::make_pair(it->first, idList));
        }
    }
}

bool COperateActivityCfg::checkExpire(UINT8 id, UINT32 now) {
    std::map<UINT8, ActivityItem>::iterator it = m_activityMap.find(id);
    if (it != m_activityMap.end()) {
        if (it->second.start <= now && now < it->second.end) {
            return false;
        }
    }
    return true;
}

bool COperateActivityCfg::isValidItem(UINT8 id, UINT8 itemId) {
    std::map<UINT8, ActivityItem>::iterator it = m_activityMap.find(id);
    if (it != m_activityMap.end()) {
        std::map<UINT8, ItemInfo>::iterator subIt = it->second.itemInfos.find(itemId);
        if (subIt != it->second.itemInfos.end()) {
            return true;
        }
    }
    return false;
}

UINT32 COperateActivityCfg::getCondition(UINT8 id, UINT8 itemId) {
    std::map<UINT8, ActivityItem>::iterator it = m_activityMap.find(id);
    if (it != m_activityMap.end()) {
        std::map<UINT8, ItemInfo>::iterator subIt = it->second.itemInfos.find(itemId);
        if (subIt != it->second.itemInfos.end()) {
            return subIt->second.condition;
        }
    }
    return 0;
}

UINT32 COperateActivityCfg::getBoxId(UINT8 id, UINT8 itemId) {
    std::map<UINT8, ActivityItem>::iterator it = m_activityMap.find(id);
    if (it != m_activityMap.end()) {
        std::map<UINT8, ItemInfo>::iterator subIt = it->second.itemInfos.find(itemId);
        if (subIt != it->second.itemInfos.end()) {
            return subIt->second.boxId;
        }
    }
    return 0;
}

UINT8 COperateActivityCfg::findIdByType(UINT8 type) {
    std::map<UINT8, UINT8>::iterator it = m_type2IdMap.find(type);
    if (it != m_type2IdMap.end()) {
        return it->second;
    }
    return 0;
}
