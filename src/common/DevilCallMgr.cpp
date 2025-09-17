#include "DevilCallMgr.h"

#include "Log4cxxHelp.h"
#include "KVPair.h"

static bool parseItem(TiXmlElement *pDrawItem, std::vector<CDevilCallMgr::stDrawItem>& itemVec, UINT32 & totalRate) {
    if (!pDrawItem) {
        Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到drawitem节点!");
        return false;
    }
    itemVec.clear();
    INT32 id = 0, num = 0, rate = 0;
    CDevilCallMgr::stDrawItem item;
    while (pDrawItem) {
        if (!pDrawItem->Attribute("ID", &id)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到id");
            return false;
        }
        if (!pDrawItem->Attribute("drawnum", &num)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到drawnum");
            return false;
        }
        if (!pDrawItem->Attribute("drawRP", &rate)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到drawRP");
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

static bool parseCertain(TiXmlElement *pCertain, CDevilCallMgr::stCertainInfo & certainInfo) {
    if (!pCertain) {
        Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到certain节点!");
        return false;
    }
    INT32 baseNum = 0, addNum = 0;
    if (!pCertain->Attribute("baseNum", &baseNum)) {
        Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到baseNum");
        return false;
    }
    if (!pCertain->Attribute("addNum", &addNum)) {
        Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到addNum");
        return false;
    }
    certainInfo.m_baseNum = baseNum;
    certainInfo.m_addNum = addNum;
    TiXmlElement * pDrawItem = pCertain->FirstChildElement("drawitem");
    if (!parseItem(pDrawItem, certainInfo.m_items, certainInfo.m_totalRate)) {
        Athena::logger->error("[魔王召唤]抽奖配置加载错误,drawitem节点解析出错!");
        return false;
    }
    return true;
}

bool CDevilCallMgr::loadCfg() {
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/draw.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[魔王召唤]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到Root节点!");
		return false;
	}

	TiXmlElement * pDevil = pRoot->FirstChildElement("devilcallDraw");
    if (!pDevil) {
		Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到devilcallDraw节点!");
        return false;
    }
    TiXmlElement * pDevilDrawId = pDevil->FirstChildElement("devilcallDrawID");
    if (!pDevilDrawId) {
        Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到devilcallDrawID节点!");
        return false;
    }
    INT32 drawId = 0, moneyType = 0, needMoney = 0, interval = 0;
    while (pDevilDrawId) {
        if (!pDevilDrawId->Attribute("id", &drawId)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到id");
            return false;
        }
        TiXmlElement * pDiamondDraw = pDevilDrawId->FirstChildElement("Diamonddraw01");
        if (!pDiamondDraw) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到devilcallDrawID节点!");
            return false;
        }
        if (!pDiamondDraw->Attribute("Moneytype", &moneyType)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到Moneytype");
            return false;
        }
        if (!pDiamondDraw->Attribute("needMoney", &needMoney)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到needMoney");
            return false;
        }
        if (!pDiamondDraw->Attribute("interval", &interval)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到interval");
            return false;
        }
        stDrawInfo drawInfo;
        drawInfo.m_moneyType = moneyType;
        drawInfo.m_needMoney = needMoney;
        drawInfo.m_interval = interval;

        TiXmlElement * pDrawItem = pDiamondDraw->FirstChildElement("drawitem");
        if (!parseItem(pDrawItem, drawInfo.m_items, drawInfo.m_totalRate)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,drawitem节点解析出错!");
            return false;
        }
        m_drawMap.insert(std::make_pair(drawId, drawInfo));
        std::vector<stCertainInfo> certainVec;
        TiXmlElement * pCertain = pDevilDrawId->FirstChildElement("certain01");
        {
            stCertainInfo certainInfo;
            if (!parseCertain(pCertain, certainInfo)) {
                Athena::logger->error("[魔王召唤]抽奖配置加载错误,certain01节点解析出错!");
                return false;
            }
            certainVec.push_back(certainInfo);
        }
        {
            stCertainInfo certainInfo;
            pCertain = pDevilDrawId->FirstChildElement("certain02");
            if (!parseCertain(pCertain, certainInfo)) {
                Athena::logger->error("[魔王召唤]抽奖配置加载错误,certain02节点解析出错!");
                return false;
            }
            certainVec.push_back(certainInfo);
        }
        m_certainMap.insert(std::make_pair(drawId, certainVec));
        pDevilDrawId = pDevilDrawId->NextSiblingElement("devilcallDrawID");
    }
    if (!loadDevil()) {
		Athena::logger->error("[魔王召唤]解析devil表出错");
		return false;
    }
    return true;
}

bool CDevilCallMgr::loadDevil() {
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/devilcall.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[魔王召唤]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到Root节点!");
		return false;
	}

	TiXmlElement * pDevil = pRoot->FirstChildElement("devil");
    if (!pDevil) {
		Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到devil节点!");
        return false;
    }
    INT32 bossId = 0, openCondition = 0, bossDrawId = 0;
    INT32 type = 0, boxId = 0;
    while (pDevil) {
        if (!pDevil->Attribute("id", &bossId)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到Bossid");
            return false;
        }
        if (!pDevil->Attribute("OpenCondition", &openCondition)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到OpenCondition");
            return false;
        }
        if (!pDevil->Attribute("BossDrawid", &bossDrawId)) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到BossDrawid");
            return false;
        }
        stDevilInfo devilInfo;
        devilInfo.m_bossId = bossId;
        devilInfo.m_bossDrawId = bossDrawId;
        devilInfo.m_openCondition = openCondition;
        TiXmlElement * pReward = pDevil->FirstChildElement("reward");
        if (!pReward) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到reward节点!");
            return false;
        }
        TiXmlElement * pEntry = pReward->FirstChildElement("entry");
        if (!pEntry) {
            Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到entry节点!");
            return false;
        }
        while (pEntry) {
            if (!pEntry->Attribute("RewardType", &type)) {
                Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到RewardType");
                return false;
            }
            if (!pEntry->Attribute("RewardBoxid", &boxId)) {
                Athena::logger->error("[魔王召唤]抽奖配置加载错误,找不到RewardBoxid");
                return false;
            }
            devilInfo.m_rewardMap.insert(std::make_pair(type, boxId));
            pEntry = pEntry->NextSiblingElement("entry");
        }
        m_devilMap.insert(std::make_pair(bossId, devilInfo));
        pDevil = pDevil->NextSiblingElement("devil");
    }
    return true;
}

void CDevilCallMgr::getAllDevilCall(std::map<UINT32, UINT32> & devilsMap) {
    for (std::map<UINT32, stDevilInfo>::iterator it =
            m_devilMap.begin(); it != m_devilMap.end(); it++) {
        devilsMap.insert(std::make_pair(it->first, it->second.m_openCondition));
    }
}

UINT32 CDevilCallMgr::getDevilReward(UINT32 bossId, UINT8 rewardType) {
    std::map<UINT32, stDevilInfo>::iterator it =
        m_devilMap.find(bossId);
    if (it != m_devilMap.end()) {
        std::map<UINT32, UINT32>::iterator rewardIt =
            it->second.m_rewardMap.find(rewardType);
        if (rewardIt != it->second.m_rewardMap.end()) {
            return rewardIt->second;
        }
    }
    return 0;
}

UINT32 CDevilCallMgr::getOpenCondition(UINT32 bossId) {
    std::map<UINT32, stDevilInfo>::iterator it =
        m_devilMap.find(bossId);
    if (it != m_devilMap.end()) {
        return it->second.m_openCondition;
    }
    return 0;
}

UINT32 CDevilCallMgr::getDrawId(UINT32 bossId) {
    std::map<UINT32, stDevilInfo>::iterator it =
        m_devilMap.find(bossId);
    if (it != m_devilMap.end()) {
        return it->second.m_bossDrawId;
    }
    return 0;
}

void CDevilCallMgr::getDrawInfo(UINT32 drawId, UINT32 & moneyType, UINT32 & needMoney) {
    std::map<UINT32, stDrawInfo>::iterator it = m_drawMap.find(drawId);
    if (it != m_drawMap.end()) {
        moneyType = it->second.m_moneyType;
        needMoney = it->second.m_needMoney;
    }
}

void CDevilCallMgr::drawItem(UINT32 drawId, std::map<UINT32, UINT32> & drawMap) {
    std::map<UINT32, stDrawInfo>::iterator it = m_drawMap.find(drawId);
    if (it == m_drawMap.end()) {
        return ;
    }
    srand(time(NULL));
    UINT32 totalRate = 0;
    UINT32 rate = rand() % it->second.m_totalRate;
    std::vector<stDrawItem> & itemVec = it->second.m_items;
    for (size_t i = 0; i < itemVec.size(); i++) {
        totalRate += itemVec[i].m_rate;
        if (rate < totalRate) {
            std::map<UINT32, UINT32>::iterator it = drawMap.find(itemVec[i].m_id);
            if (it != drawMap.end()) {
                it->second += itemVec[i].m_num;
            } else {
                drawMap.insert(std::make_pair(itemVec[i].m_id, itemVec[i].m_num));
            }
            break;
        }
    }
}

void CDevilCallMgr::drawCertailItem(UINT32 drawId, UINT32 drawNum, std::map<UINT32, UINT32> & drawMap) {
    std::map<UINT32, std::vector<stCertainInfo> >::iterator it =
            m_certainMap.find(drawId);
    if (it == m_certainMap.end()) {
        return ;
    }
    std::vector<stCertainInfo> & certainVec = it->second;
    for (size_t i = 0; i < certainVec.size(); i++) {
        if (certainVec[i].m_baseNum != certainVec[i].m_addNum) {
            if (drawNum % certainVec[i].m_addNum != certainVec[i].m_baseNum) {
                continue ;
            }
        } else {
            if (drawNum % certainVec[i].m_baseNum != 0 || !drawNum) {
                continue ;
            }
        }
        srand(time(NULL));
        UINT32 totalRate = 0;
        UINT32 rate = rand() % certainVec[i].m_totalRate;
        std::vector<stDrawItem> & itemVec = certainVec[i].m_items;
        for (size_t k = 0; k < itemVec.size(); k++) {
            totalRate += itemVec[k].m_rate;
            if (rate < totalRate) {
                std::map<UINT32, UINT32>::iterator it = drawMap.find(itemVec[k].m_id);
                if (it != drawMap.end()) {
                    it->second += itemVec[k].m_num;
                } else {
                    drawMap.insert(std::make_pair(itemVec[k].m_id, itemVec[k].m_num));
                }
                break;
            }
        }
    }
}
