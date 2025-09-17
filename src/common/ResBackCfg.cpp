#include "ResBackCfg.h"

#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool CResBackCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/resourcegetback.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[资源找回]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pConf = doc.FirstChildElement("conf");
	if (!pConf){
		Athena::logger->error("[资源找回]找不到节点conf");
		return false;
	}

	TiXmlElement * pType = pConf->FirstChildElement("type");
    if (!pType) {
        Athena::logger->error("[资源找回]找不到节点type");
        return false;
    }

    int val = 0;
	const char * szVal = NULL;
    while (pType) {
        TiXmlElement * pEntry = pType->FirstChildElement("entry");
        if (!pEntry) {
            Athena::logger->error("[资源找回]找不到节点entry");
            return false;
        }
        while (pEntry) {
            RewardItem item;
            if (!pEntry->Attribute("Type", &val)) {
                Athena::logger->error("[资源找回]entry节点找不到Type");
                return false;
            }
            item.type = val;
            if (!pEntry->Attribute("CostDiamond", &val)) {
                Athena::logger->error("[资源找回]entry节点找不到CostDiamond");
                return false;
            }
            item.costDiamond = val;

            if (!(szVal = pEntry->Attribute("GetBackItem"))) {
                Athena::logger->error("[资源找回]entry 节点找不到GetBackItem");
                return false;
            }
            std::vector<std::string> strVec;
            g_stringtok(strVec, szVal, ":");
            for (size_t i = 0; i < strVec.size(); i++) {
                RewardInfo info;
                std::vector<std::string> dataVec;
                g_stringtok(dataVec, strVec[i], "-");
                if (dataVec.size() != 2) {
                    Athena::logger->error("[资源找回]GetBackItem格式错误");
                    return false;
                }
                info.itemId = (UINT32)atoi(dataVec[0].c_str());
                info.num = (UINT32)atoi(dataVec[1].c_str());
                item.rewardVec.push_back(info);
            }
            m_typeVec.push_back(item.type);
            m_rewardMap.insert(std::make_pair(item.type, item));
            pEntry = pEntry->NextSiblingElement("entry");
        }
        pType = pType->NextSiblingElement("type");
    }
#ifdef _HDZ_DEBUG
    for (size_t i = 0; i < m_typeVec.size(); i++) {
        Athena::logger->trace("[资源找回]type %d", m_typeVec[i]);
    }

    for (std::map<UINT32, RewardItem>::iterator it = m_rewardMap.begin();
            it != m_rewardMap.end(); it++) {
        Athena::logger->trace("[资源找回]type %d, costDiamond %d", it->second.type, it->second.costDiamond);
        std::vector<RewardInfo> & rewardVec = it->second.rewardVec;
        for (size_t i = 0; i < rewardVec.size(); i++) {
            Athena::logger->trace("[资源找回]item %d, num %d", rewardVec[i].itemId, rewardVec[i].num);
        }
    }
#endif
    return true;
}

UINT32 CResBackCfg::getMaxNum(UINT32 type, UINT32 vipLvl) {
    switch (type) {
        case Big_Sp:
            {
                if (vipLvl < 7) {
                    return vipLvl + 1;
                } else if (vipLvl < 15) {
                    return vipLvl + vipLvl - 5;
                }
                return 25;
            }
        case Big_Exp:
            {
                if (vipLvl < 15) {
                    return vipLvl + 5;
                }
                return 20;
            }
        case Sp_Box:
            return 10;

        case Emerald_Dream:
            return 1;

        case Fam_Trasure:
            return 20;

        case Three_Vs_Three:
            return 10;

        case Escort_Ship:
            return 3;

        case Letai_King:
            return 10;

        case Team_Map:
            return 3;

        case Gold_Map:
            return 3;

        case Stone_Map:
            return 3;

        case Dragon_Map:
            return 6;

        case Balck_Map:
            return 6;

        default:
            break;

    }
    return 0;
}

bool CResBackCfg::getItem(UINT32 type, RewardItem & item) {
    std::map<UINT32, RewardItem>::iterator it = m_rewardMap.find(type);
    if (it != m_rewardMap.end()) {
        item = it->second;
        return true;
    }
    return false;
}
