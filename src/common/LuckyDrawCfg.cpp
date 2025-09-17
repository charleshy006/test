#include "LuckyDrawCfg.h"

#include "Log4cxxHelp.h"
#include "KVPair.h"

static bool parseItem(TiXmlElement *pDrawItem, std::vector<CLuckyDrawCfg::stDrawItem>& itemVec, UINT32 & totalRate) {
    if (!pDrawItem) {
        Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到drawitem节点!");
        return false;
    }
    itemVec.clear();
    INT32 id = 0, num = 0, rate = 0;
    CLuckyDrawCfg::stDrawItem item;
    while (pDrawItem) {
        if (!pDrawItem->Attribute("ID", &id)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到id");
            return false;
        }
        if (!pDrawItem->Attribute("drawnum", &num)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到drawnum");
            return false;
        }
        if (!pDrawItem->Attribute("drawRP", &rate)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到drawRP");
            return false;
        }
        item.m_id = id;
        item.m_num = num;
        item.m_rate = rate;
        totalRate += rate;
        itemVec.push_back(item);
        pDrawItem = pDrawItem->NextSiblingElement("drawitem");
    }
    return true;
}

static bool parseCertain(TiXmlElement *pCertain, CLuckyDrawCfg::stCertainInfo & certainInfo) {
    if (!pCertain) {
        Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到certain节点!");
        return false;
    }
    INT32 baseNum = 0, addNum = 0;
    if (!pCertain->Attribute("baseNum", &baseNum)) {
        Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到baseNum");
        return false;
    }
    if (!pCertain->Attribute("addNum", &addNum)) {
        Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到addNum");
        return false;
    }
    certainInfo.m_baseNum = baseNum;
    certainInfo.m_addNum = addNum;
    TiXmlElement * pDrawItem = pCertain->FirstChildElement("drawitem");
    if (!parseItem(pDrawItem, certainInfo.m_items, certainInfo.m_totalRate)) {
        Athena::logger->error("[幸运抽奖]抽奖配置加载错误,drawitem节点解析出错!");
        return false;
    }
    return true;
}

bool CLuckyDrawCfg::loadCfg() {
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/Gravedigger.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[幸运抽奖]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("conf");
	if (!pRoot){
		Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到conf节点!");
		return false;
	}
	TiXmlElement * pId = pRoot->FirstChildElement("Activity");
	if (!pId){
		Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到Activity节点!");
		return false;
	}
    int val = 0;
    int activityId = 0, drawId = 0;
    while (pId) {
        if (!pId->Attribute("Activityid", &activityId)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到Activityid");
            return false;
        }
        TiXmlElement * pDraw = pId->FirstChildElement("drawid");
        if (!pDraw){
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到drawid节点!");
            return false;
        }
        TiXmlElement * pEntry = pDraw->FirstChildElement("entry");
        if (!pEntry){
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到drawid节点!");
            return false;
        }
        if (!pEntry->Attribute("limittimedrawid", &drawId)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到limittimedrawid");
            return false;
        }
        TiXmlElement * pReward = pId->FirstChildElement("pointreward");
        if (!pReward){
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到pointreward节点!");
            return false;
        }
        pEntry = pReward->FirstChildElement("entry");
        if (!pEntry){
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到entry节点!");
            return false;
        }
        std::vector<PointReward> rewardVec;
        while (pEntry) {
            PointReward reward;
            if (!pEntry->Attribute("id", &val)) {
                Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到id");
                return false;
            }
            reward.id = val;
            if (!pEntry->Attribute("pointnum", &val)) {
                Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到pointnum");
                return false;
            }
            reward.num = val;
            if (!pEntry->Attribute("boxid", &val)) {
                Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到boxid");
                return false;
            }
            reward.boxId = val;
            rewardVec.push_back(reward);
            pEntry = pEntry->NextSiblingElement("entry");
        }
        m_pointRewardMap.insert(std::make_pair(activityId, std::make_pair(rewardVec, drawId)));
        pId = pId->NextSiblingElement("Activity");
    }
	TiXmlElement * pActivityOn = pRoot->FirstChildElement("ActivityOn");
	if (!pActivityOn){
		Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到ActivityOn节点!");
		return false;
	}
    TiXmlElement * pEntry = pActivityOn->FirstChildElement("entry");
    if (!pEntry){
        Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到entry节点!");
        return false;
    }
	const char * szVal = NULL;
    while (pEntry) {
        ActivityItem item;
        if (!pEntry->Attribute("id", &val)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到id");
            return false;
        }
        item.id = val;
        if (!pEntry->Attribute("Activityid", &val)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到Activityid");
            return false;
        }
        item.activityId = val;
        if (!(szVal = pEntry->Attribute("Starttime"))) {
            Athena::logger->error("[幸运抽奖]activity节点找不到Starttime");
            return false;
        }
        struct tm _tm;
        if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
             item.start = mktime(&_tm) ;
        }   
        if (!(szVal = pEntry->Attribute("Endtime"))) {
            Athena::logger->error("[幸运抽奖]activity节点找不到Endtime");
            return false;
        }
        if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
             item.end = mktime(&_tm) ;
             item.next = item.end;
        }
        m_activityMap.insert(std::make_pair(item.id, item));
        pEntry = pEntry->NextSiblingElement("entry");
    }
    return loadRewardCfg();
}

bool CLuckyDrawCfg::loadRewardCfg() {
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/draw.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[幸运抽奖]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到Root节点!");
		return false;
	}

	TiXmlElement * pLucky = pRoot->FirstChildElement("luckyDraw");
    if (!pLucky) {
		Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到luckyDraw节点!");
        return false;
    }
    TiXmlElement * pLuckyDrawId = pLucky->FirstChildElement("luckyDrawID");
    if (!pLuckyDrawId) {
        Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到luckyDrawID节点!");
        return false;
    }
    int drawId = 0, moneyType = 0, needMoney = 0, interval = 0, drawType = 0;
	const char * szVal = NULL;
    while (pLuckyDrawId) {
        if (!pLuckyDrawId->Attribute("id", &drawId)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到id");
            return false;
        }
        TiXmlElement * pLuckyDraw = pLuckyDrawId->FirstChildElement("luckyDraw01");
        if (!pLuckyDraw) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到luckyDraw01节点!");
            return false;
        }
        while (pLuckyDraw) {
            if (!(szVal = pLuckyDraw->Attribute("id"))) {
                Athena::logger->error("[幸运抽奖]activity节点找不到StartTime");
                return false;
            }
            std::string str = szVal;
            if (str == "01") {
                drawType = Draw_Once;
            } else if (str == "02") {
                drawType = Draw_Ten;
            } else {
                Athena::logger->error("[幸运抽奖]luckyDraw01 unknown id %s", szVal);
                return false;
            }
            if (!pLuckyDraw->Attribute("Moneytype", &moneyType)) {
                Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到Moneytype");
                return false;
            }
            if (!pLuckyDraw->Attribute("needMoney", &needMoney)) {
                Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到needMoney");
                return false;
            }
            if (!pLuckyDraw->Attribute("interval", &interval)) {
                Athena::logger->error("[幸运抽奖]抽奖配置加载错误,找不到interval");
                return false;
            }
            stDrawInfo drawInfo;
            drawInfo.m_drawType = drawType;
            drawInfo.m_moneyType = moneyType;
            drawInfo.m_needMoney = needMoney;
            drawInfo.m_interval = interval;

            TiXmlElement * pDrawItem = pLuckyDraw->FirstChildElement("drawitem");
            if (!parseItem(pDrawItem, drawInfo.m_items, drawInfo.m_totalRate)) {
                Athena::logger->error("[幸运抽奖]抽奖配置加载错误,drawitem节点解析出错!");
                return false;
            }
            pLuckyDraw = pLuckyDraw->NextSiblingElement("luckyDraw01");
            if (drawType == Draw_Once) {
                m_drawMap[drawId].first = drawInfo;
            } else {
                m_drawMap[drawId].second = drawInfo;
            }
        }
        std::vector<stCertainInfo> certainVec;
        TiXmlElement * pCertain = pLuckyDrawId->FirstChildElement("certain01");
        stCertainInfo certainInfo;
        if (!parseCertain(pCertain, certainInfo)) {
            Athena::logger->error("[幸运抽奖]抽奖配置加载错误,certain01节点解析出错!");
            return false;
        }
        m_certainMap.insert(std::make_pair(drawId, certainInfo));
        pLuckyDrawId = pLuckyDrawId->NextSiblingElement("luckyDrawID");
    }
    std::map<UINT32, ActivityItem>::iterator tmpIt;
    for (std::map<UINT32, ActivityItem>::iterator it = m_activityMap.begin();
            it != m_activityMap.end(); it++) {
        tmpIt = it;
        tmpIt++;
        if (tmpIt != m_activityMap.end()) {
            it->second.next = tmpIt->second.start;
        } else {
            break;
        }
    }
#ifdef _HDZ_DEBUG
    for (std::map<UINT32, ActivityItem>::iterator it = m_activityMap.begin();
            it != m_activityMap.end(); it++) {
        Athena::logger->trace("[幸运抽奖]id is %d, activityId is %d, start is %d, end is %d, next is %d",
                it->second.id, it->second.activityId, it->second.start, it->second.end, it->second.next);
    }
    for (std::map<UINT8, std::pair<std::vector<PointReward>, UINT8> >::iterator it =
            m_pointRewardMap.begin(); it != m_pointRewardMap.end(); it++) {
        std::vector<PointReward> & rewardVec = it->second.first;
        Athena::logger->trace("[幸运抽奖]activityId is %d, pointReward size is %d, drawId is %d",
                it->first, rewardVec.size(), it->second.second);
        for (size_t i = 0; i < rewardVec.size(); i++) {
            Athena::logger->trace("[幸运抽奖]PointReward : id is %d, num is %d, boxId is %d",
                    rewardVec[i].id, rewardVec[i].num, rewardVec[i].boxId);
        }
    }
    for (std::map<UINT32, std::pair<stDrawInfo, stDrawInfo> >::iterator it =
            m_drawMap.begin(); it != m_drawMap.end(); it++) {
        Athena::logger->trace("[幸运抽奖]drawId is %d", it->first);
        stDrawInfo & drawInfo = it->second.first;
        Athena::logger->trace("[幸运抽奖]单抽 : type is %d, money type is %d, need is %d, interval is %d, totalRate is %d",
                drawInfo.m_drawType, drawInfo.m_moneyType, drawInfo.m_needMoney, drawInfo.m_interval, drawInfo.m_totalRate);
        for (size_t i = 0; i < drawInfo.m_items.size(); i++) {
            Athena::logger->trace("[幸运抽奖]单抽列表 : id is %d, num is %d, rate is %d",
                    drawInfo.m_items[i].m_id, drawInfo.m_items[i].m_num, drawInfo.m_items[i].m_rate);
        }
        {
            stDrawInfo & drawInfo = it->second.second;
            Athena::logger->trace("[幸运抽奖]十连抽 : type is %d, money type is %d, need is %d, interval is %d, totalRate is %d",
                    drawInfo.m_drawType, drawInfo.m_moneyType, drawInfo.m_needMoney, drawInfo.m_interval, drawInfo.m_totalRate);
            for (size_t i = 0; i < drawInfo.m_items.size(); i++) {
                Athena::logger->trace("[幸运抽奖]十连抽列表 : id is %d, num is %d, rate is %d",
                        drawInfo.m_items[i].m_id, drawInfo.m_items[i].m_num, drawInfo.m_items[i].m_rate);
            }
        }
    }
    for (std::map<UINT32, stCertainInfo>::iterator it = m_certainMap.begin();
            it != m_certainMap.end(); it++) {
        Athena::logger->trace("[幸运抽奖]draw id is %d, base is %d, add is %d, total rate is %d",
                it->first, it->second.m_baseNum, it->second.m_addNum, it->second.m_totalRate);
        std::vector<stDrawItem> & itemVec = it->second.m_items;
        for (size_t i = 0; i < itemVec.size(); i++) {
            Athena::logger->trace("[幸运抽奖]額外列表 : id is %d, num is %d, rate is %d",
                    itemVec[i].m_id, itemVec[i].m_num, itemVec[i].m_rate);
        }
    }
#endif
    return true;
}

UINT32 CLuckyDrawCfg::getActiveId(UINT32 now) {
    for (std::map<UINT32, ActivityItem>::iterator it =
            m_activityMap.begin(); it != m_activityMap.end(); it++) {
        if (it->second.start <= now && now <= it->second.end) {
            return it->first;
        }
    }
    return 0;
}

bool CLuckyDrawCfg::getActivityItem(UINT32 id, ActivityItem & item) {
    std::map<UINT32, ActivityItem>::iterator it = m_activityMap.find(id);
    if (it != m_activityMap.end()) {
        item = it->second;
        return true;
    }
    return false;
}

UINT8 CLuckyDrawCfg::getDrawId(UINT8 id) {
    std::map<UINT8, std::pair<std::vector<PointReward>, UINT8> >::iterator it =
        m_pointRewardMap.find(id);
    if (it != m_pointRewardMap.end()) {
        return it->second.second;
    }
    return 0;
}

bool CLuckyDrawCfg::getDrawInfo(UINT32 drawId, UINT8 drawType, stDrawInfo & info) {
    std::map<UINT32, std::pair<stDrawInfo, stDrawInfo> >::iterator it =
        m_drawMap.find(drawId);
    if (it != m_drawMap.end()) {
        if (drawType == Draw_Once) {
            info = it->second.first;
        } else {
            info = it->second.second;
        }
        return true;
    }
    return false;
}

bool CLuckyDrawCfg::getCertainInfo(UINT32 drawId, stCertainInfo & certain) {
    std::map<UINT32, stCertainInfo>::iterator it = m_certainMap.find(drawId);
    if (it != m_certainMap.end()) {
        certain = it->second;
        return true;
    }
    return false;
}

void CLuckyDrawCfg::getAllActivityItem(std::vector<ActivityItem> & itemVec) {
    for (std::map<UINT32, ActivityItem>::iterator it = m_activityMap.begin();
            it != m_activityMap.end(); it++) {
        itemVec.push_back(it->second);
    }
}

bool CLuckyDrawCfg::isExpireId(UINT32 id, UINT32 now) {
    for (std::map<UINT32, ActivityItem>::iterator it =
            m_activityMap.begin(); it != m_activityMap.end(); it++) {
        if (it->second.start <= now && now < it->second.next) {
            return false;
        }
    }
    return true;
}

bool CLuckyDrawCfg::getPointReward(UINT8 id, UINT8 pointId, PointReward & reward) {
    std::map<UINT8, std::pair<std::vector<PointReward>, UINT8> >::iterator it =
        m_pointRewardMap.find(id);
    if (it == m_pointRewardMap.end()) {
        return false;
    }
    std::vector<PointReward> & rewardVec = it->second.first;
    for (size_t i = 0; i < rewardVec.size(); i++) {
        if (rewardVec[i].id == pointId) {
            reward = rewardVec[i];
            return true;
        }
    }
    return false;
}

UINT8 CLuckyDrawCfg::getActivityId(UINT32 id) {
    std::map<UINT32, ActivityItem>::iterator it = m_activityMap.find(id);
    if (it != m_activityMap.end()) {
        return it->second.activityId;
    }
    return 0;
}
