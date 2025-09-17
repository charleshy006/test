#include "ExpeditionMgr.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"

CExpeditionItem::~CExpeditionItem() {

}

UINT32 CExpeditionItem::load(byte * buf) {
	UINT32 ret = 0;
    m_expeditionId = *(UINT16 *)(buf);
    ret += sizeof(UINT16);
    m_finishTime = *(UINT32 *)(buf + ret);
    ret += sizeof(UINT32);
    UINT8 num = *(UINT8 *)(buf + ret);
    ret += sizeof(UINT8);
    stExpeditionHero hero;
    for (UINT8 i = 0; i < num; i++) {
        hero.m_id = *(UINT32 *)(buf + ret);
        ret += sizeof(UINT32);
        hero.m_lvl = *(UINT16 *)(buf + ret);
        ret += sizeof(UINT16);
        hero.m_power = *(UINT32 *)(buf + ret);
        ret += sizeof(UINT32);
        m_usedHeroVec.push_back(hero);
    }
    return ret;
}

UINT32 CExpeditionItem::save(byte * buf,UINT32 needSpace) {
	UINT32 ret = 0;
    *(UINT16 *)(buf + ret) = m_expeditionId;
    ret += sizeof(UINT16);
    *(UINT32 *)(buf + ret) = m_finishTime;
    ret += sizeof(UINT32);
    *(UINT8 *)(buf + ret) = (UINT8)m_usedHeroVec.size();
    ret += sizeof(UINT8);
    for (size_t i = 0; i < m_usedHeroVec.size(); i++) {
        *(UINT32 *)(buf + ret) = m_usedHeroVec[i].m_id;
        ret += sizeof(UINT32);
        *(UINT16 *)(buf + ret) = m_usedHeroVec[i].m_lvl;
        ret += sizeof(UINT16);
        *(UINT32 *)(buf + ret) = m_usedHeroVec[i].m_power;
        ret += sizeof(UINT32);
    }
    return ret;
}

void CExpeditionItem::getUsedHero(std::vector<UINT32> & heroVec) {
    for (std::vector<stExpeditionHero>::iterator it = m_usedHeroVec.begin();
            it != m_usedHeroVec.end(); it++) {
        heroVec.push_back((*it).m_id);
    }
}

UINT32 CExpeditionItem::getTotalHeroPower() {
    UINT32 power = 0;
    for (std::vector<stExpeditionHero>::iterator it = m_usedHeroVec.begin();
            it != m_usedHeroVec.end(); it++) {
        power += (*it).m_power;
    }
    return power;
}

bool CExpeditionItem::greatorLvl(UINT16 lvl) {
    for (std::vector<stExpeditionHero>::iterator it = m_usedHeroVec.begin();
            it != m_usedHeroVec.end(); it++) {
        if ((*it).m_lvl >= lvl) {
            return true;
        }
    }
    return false;
}

CExpeditionMgr::CExpeditionMgr(GameUser * pOwner) : m_pOwner(pOwner) {

}

CExpeditionMgr::~CExpeditionMgr() {
    for (std::map<UINT16, CExpeditionItem*>::iterator it = m_expeditionMap.begin();
            it != m_expeditionMap.end(); it++) {
        SAFE_DELETE(it->second);
    }
}

UINT32 CExpeditionMgr::loadAll(byte * buf) {
	UINT32 ret = 0, tmpRet = 0;
	UINT8 num = *(UINT8 *)(buf + ret);
	ret += sizeof(UINT8);
	for (UINT8 i = 0;i < num; ++i){
        CExpeditionItem * pItem = ATHENA_NEW CExpeditionItem();
        if (!pItem) {
            SAFE_DELETE(pItem);
            continue;
        }
        tmpRet = pItem->load(buf + ret);
        m_expeditionMap.insert(std::make_pair(pItem->getId(), pItem));
        ret += tmpRet;
    }
    std::vector<UINT32> heroVec;
    for (std::map<UINT16, CExpeditionItem*>::iterator it = m_expeditionMap.begin();
            it != m_expeditionMap.end(); it++) {
        it->second->getUsedHero(heroVec);
        for (size_t i = 0; i < heroVec.size(); i++) {
            m_usedHeroSet.insert(heroVec[i]);
        }
        heroVec.clear();
    }
    return ret;
}

UINT32 CExpeditionMgr::saveAll(byte * buf,UINT32 needSpace) {
    *(UINT8 *)(buf) = (UINT8)m_expeditionMap.size();
    UINT32 ret = sizeof(UINT8), tmpRet = 0;
    for (std::map<UINT16, CExpeditionItem*>::iterator it = m_expeditionMap.begin();
            it != m_expeditionMap.end(); it++) {
        tmpRet = it->second->save(buf + ret, needSpace);
        ret += tmpRet;
    }
    return ret;
}

void CExpeditionMgr::loadItems(SortProto::stRetExpeditionInfo& retCmd) {
    std::vector<UINT32> heroVec;
    for (std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.begin(); it != m_expeditionMap.end(); it++) {
        SortProto::stExpeditionItem * pItem = retCmd.add_items();
        pItem->set_expeditionid(it->first);
        pItem->set_finishtime(it->second->getFinishTime());
        it->second->getUsedHero(heroVec);
        for (size_t i = 0; i < heroVec.size(); i++) {
            pItem->add_heroids(heroVec[i]);
        }
        heroVec.clear();
    }
}

void CExpeditionMgr::getSummary(SortProto::stRetExpeditionSummary& retCmd) {
    for (std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.begin(); it != m_expeditionMap.end(); it++) {
        SortProto::stExpeditionItem * pItem = retCmd.add_items();
        pItem->set_expeditionid(it->first);
        pItem->set_finishtime(it->second->getFinishTime());
    }
}

void CExpeditionMgr::loadIds(std::vector<UINT16> & idVec) {
    for (std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.begin(); it != m_expeditionMap.end(); it++) {
        idVec.push_back(it->first);
    }
}

bool CExpeditionMgr::heroInUsed(std::set<UINT32> & heroSet) {
    for (std::set<UINT32>::iterator it = heroSet.begin();
            it != heroSet.end(); it++) {
        if (m_usedHeroSet.find((*it)) != m_usedHeroSet.end()) {
            return true;
        }
    }
    return false;
}

void CExpeditionMgr::addUsedHero(std::set<UINT32> & heroSet) {
    for (std::set<UINT32>::iterator it = heroSet.begin();
            it != heroSet.end(); it++) {
        m_usedHeroSet.insert(*it);
    }
}

void CExpeditionMgr::addExpedition(CExpeditionItem * pItem) {
    m_expeditionMap.insert(std::make_pair(pItem->getId(), pItem));
}

const CExpeditionItem * CExpeditionMgr::getExpedition(UINT16 id) {
    std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.find(id);
    if (it != m_expeditionMap.end()) {
        return it->second;
    }
    return NULL;
}

void CExpeditionMgr::deleteExpedition(UINT16 id) {
    std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.find(id);
    if (it == m_expeditionMap.end()) {
        return ;
    }
    std::vector<UINT32> heroVec;
    it->second->getUsedHero(heroVec);
    for (size_t i = 0; i < heroVec.size(); i++) {
        m_usedHeroSet.erase(heroVec[i]);
    }
    CExpeditionItem * pItem = it->second;
    m_expeditionMap.erase(it);
    SAFE_DELETE(pItem);
}
