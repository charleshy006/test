#include "EnchantCfg.h"

#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

CEnchantCfg::CEnchantCfg()
    : m_totalRate(0) {
}

CEnchantCfg::~CEnchantCfg() {

}

bool CEnchantCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/enchant.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[时装附魔]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[时装附魔]找不到节点root");
		return false;
	}

	TiXmlElement * pType = pRoot->FirstChildElement("EnchantType");
    if (!pType) {
        Athena::logger->error("[时装附魔]找不到节点EnchantType");
        return false;
    }
	TiXmlElement * pEnchantType = pType->FirstChildElement("enchantType");
    if (!pEnchantType) {
        Athena::logger->error("[时装附魔]找不到节点enchantType");
        return false;
    }
    int val = 0;
	const char * szVal = NULL;
    while (pEnchantType) {
        EnchantType item;
        if (!pEnchantType->Attribute("ID", &val)) {
            Athena::logger->error("[时装附魔]enchantType节点找不到ID");
            return false;
        }
        item.id = val;
        if (!(szVal = pEnchantType->Attribute("enchantItem"))) {
            Athena::logger->error("[时装附魔]enchantType节点找不到enchantItem");
            return false;
        }
        std::vector<std::string> strVec;
        g_stringtok(strVec, szVal, "-");
        if (strVec.size() != 2) {
            Athena::logger->error("[时装附魔]enchantItem节点数据错误");
            return false;
        }
        item.enchantItemType = atoi(strVec[0].c_str());
        item.lockNum = atoi(strVec[1].c_str());

        if (!pEnchantType->Attribute("enchantMoney", &val)) {
            Athena::logger->error("[时装附魔]enchantType节点找不到enchantMoney");
            return false;
        }
        item.needGold = val;
        if (!pEnchantType->Attribute("enchantNum", &val)) {
            Athena::logger->error("[时装附魔]enchantType节点找不到enchantNum");
            return false;
        }
        item.enchantNum = val;
        if (!pEnchantType->Attribute("maxNum", &val)) {
            Athena::logger->error("[时装附魔]enchantType节点找不到maxNum");
            return false;
        }
        item.propNum = val;
        if (!(szVal = pEnchantType->Attribute("enchantRanger"))) {
            Athena::logger->error("[时装附魔]enchantType节点找不到enchantRanger");
            return false;
        }
        strVec.clear();
        g_stringtok(strVec, szVal, "-");
        if (strVec.size() != 2) {
            Athena::logger->error("[时装附魔]enchantRanger节点数据错误");
            return false;
        }
        item.rangeMinId = atoi(strVec[0].c_str());
        item.rangeMaxId = atoi(strVec[1].c_str());
        m_enchantTypeMap[item.enchantItemType][item.lockNum] = item;
        pEnchantType = pEnchantType->NextSiblingElement("enchantType");
    }
	TiXmlElement * pEnchant = pRoot->FirstChildElement("Enchant");
    if (!pEnchant) {
        Athena::logger->error("[时装附魔]找不到节点Enchant");
        return false;
    }
	TiXmlElement * pItem = pEnchant->FirstChildElement("enchant");
    if (!pItem) {
        Athena::logger->error("[时装附魔]找不到节点enchant");
        return false;
    }
    while (pItem) {
        EnchantItemInfo item;
        if (!pItem->Attribute("ID", &val)) {
            Athena::logger->error("[时装附魔]Enchant节点找不到ID");
            return false;
        }
        item.id = val;
        if (!pItem->Attribute("enchantProperty", &val)) {
            Athena::logger->error("[时装附魔]Enchant节点找不到enchantProperty");
            return false;
        }
        item.propType = val;
        if (!(szVal = pItem->Attribute("propertyRanger"))) {
            Athena::logger->error("[时装附魔]Enchant节点找不到propertyRanger");
            return false;
        }
        std::vector<std::string> strVec;
        g_stringtok(strVec, szVal, "-");
        if (strVec.size() != 2) {
            Athena::logger->error("[时装附魔]enchantItem节点数据错误");
            return false;
        }
        item.rangePropMinNum = atoi(strVec[0].c_str());
        item.rangePropMaxNum = atoi(strVec[1].c_str());
        if (!pItem->Attribute("enchantRP", &val)) {
            Athena::logger->error("[时装附魔]enchantType节点找不到enchantRP");
            return false;
        }
        item.rate = val;
        m_totalRate += val;
        if (!pItem->Attribute("color", &val)) {
            Athena::logger->error("[时装附魔]Enchant节点找不到color");
            return false;
        }
        item.color = val;
        item.totalRate = m_totalRate;
        m_itemVec.push_back(item);
        pItem = pItem->NextSiblingElement("enchant");
    }
#ifdef _HDZ_DEBUG
    for (std::map<UINT8, std::map<UINT32, EnchantType> >::iterator it = m_enchantTypeMap.begin();
            it != m_enchantTypeMap.end(); it++) {
        for (std::map<UINT32, EnchantType>::iterator subIt = it->second.begin();
                subIt != it->second.end(); subIt++) {
            Athena::logger->trace("[时装附魔EnchantType]id %d, type %d, lockNum %d, needGold %d, propNum %d, min %d, max %d",
                subIt->second.id, subIt->second.enchantItemType, subIt->second.lockNum, subIt->second.needGold,
                    subIt->second.propNum, subIt->second.rangeMinId, subIt->second.rangeMaxId);
        }
    }
    for (size_t i = 0; i < m_itemVec.size(); i++) {
        Athena::logger->trace("[时装附魔EnchantItem]id %d, type %d, rate %d, color %d, min %d, max %d",
            m_itemVec[i].id, m_itemVec[i].propType, m_itemVec[i].rate, m_itemVec[i].color, m_itemVec[i].rangePropMinNum, m_itemVec[i].rangePropMaxNum);
    }
    Athena::logger->trace("[时装附魔]m_totalRate is %d", m_totalRate);
#endif
    return true;
}

bool CEnchantCfg::getEnchantType(UINT8 enchantItemType, UINT32 lockNum, EnchantType & enchantType) {
    for (std::map<UINT8, std::map<UINT32, EnchantType> >::iterator it =
            m_enchantTypeMap.begin(); it != m_enchantTypeMap.end(); it++) {
        for (std::map<UINT32, EnchantType>::iterator subIt = it->second.begin();
                subIt != it->second.end(); subIt++) {
            if (subIt->second.lockNum == lockNum &&
                    subIt->second.enchantItemType == enchantItemType) {
                enchantType = subIt->second;
                return true;
            }
        }
    }
    return false;
}

bool CEnchantCfg::getEnchantItem(UINT32 id, EnchantItemInfo & item) {
    if (!id || id > m_itemVec.size()) {
        return false;
    }
    item = m_itemVec[id - 1];
    return true;
}

UINT16 CEnchantCfg::randPropId(UINT32 minId, UINT32 maxId) {
    if (!minId || !maxId || minId > m_itemVec.size() || maxId > m_itemVec.size()) {
        return 0;
    }
    INT32 StartNum = 1;
    if (minId > 1)
    {
        StartNum = m_itemVec[minId-2].totalRate +1;
    }
     
    UINT32 rate = g_randBetween(StartNum,m_itemVec[maxId - 1].totalRate);
    for (size_t i = minId - 1; i < maxId; i++) {
        if (rate < m_itemVec[i].totalRate) {
            return i + 1;
        }
    }
    return 0;
}
