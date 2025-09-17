#include "Expedition.h"

#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "KVPair.h"
#include "Misc.pb.h"

extern GlobalServer  * g_globalServer;

CTblField g_expeditionTblDef[] = { 
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "DATA",           CTblField::DB_TYPE_BLOB,    MAX_EXPEDITION_SIZE}, 
	{ NULL,             0,                          0 },
};

CTblField g_devilCallTblDef[] = { 
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "DATA",           CTblField::DB_TYPE_BLOB,    MAX_DEVILCALL_SIZE}, 
	{ NULL,             0,                          0 },
};

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

UINT32 CExpeditionItem::save(byte * buf) {
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

void CExpeditionItem::setUsedHero(Global::stExpeditionRet * recvCmd) {
    for (int i = 0; i < recvCmd->usedhero_size(); i++) {
        stExpeditionHero hero;
        hero.m_id = (UINT32)(recvCmd->usedhero(i).id());
        hero.m_lvl = (UINT16)(recvCmd->usedhero(i).lvl());
        hero.m_power = (UINT32)(recvCmd->usedhero(i).power());
        m_usedHeroVec.push_back(hero);
    }
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

CExpedition::CExpedition() {

}

CExpedition::~CExpedition() {
    for (std::map<UINT16, CExpeditionItem*>::iterator it = m_expeditionMap.begin();
            it != m_expeditionMap.end(); it++) {
        SAFE_DELETE(it->second);
    }
}

UINT32 CExpedition::loadAll(byte * buf) {
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

UINT32 CExpedition::saveAll(byte * buf) {
    *(UINT8 *)(buf) = (UINT8)m_expeditionMap.size();
    UINT32 ret = sizeof(UINT8), tmpRet = 0;
    for (std::map<UINT16, CExpeditionItem*>::iterator it = m_expeditionMap.begin();
            it != m_expeditionMap.end(); it++) {
        tmpRet = it->second->save(buf + ret);
        ret += tmpRet;
    }
    return ret;
}

void CExpedition::loadItems(Global::stExpeditionInfo & retCmd) {
    std::vector<UINT32> heroVec;
    for (std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.begin(); it != m_expeditionMap.end(); it++) {
        Global::stExpeditionItem * pItem = retCmd.add_items();
        pItem->set_expeditionid(it->first);
        pItem->set_finishtime(it->second->getFinishTime());
        it->second->getUsedHero(heroVec);
        for (size_t i = 0; i < heroVec.size(); i++) {
            pItem->add_heroids(heroVec[i]);
        }
        heroVec.clear();
    }
}

void CExpedition::getSummary(SortProto::stRetExpeditionSummary& retCmd) {
    for (std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.begin(); it != m_expeditionMap.end(); it++) {
        SortProto::stExpeditionItem * pItem = retCmd.add_items();
        pItem->set_expeditionid(it->first);
        pItem->set_finishtime(it->second->getFinishTime());
    }
}

void CExpedition::getSummary(Global::stExpeditionSummary& retCmd) {
    for (std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.begin(); it != m_expeditionMap.end(); it++) {
        Global::stExpeditionItem * pItem = retCmd.add_items();
        pItem->set_expeditionid(it->first);
        pItem->set_finishtime(it->second->getFinishTime());
    }
}

void CExpedition::loadIds(std::vector<UINT16> & idVec) {
    for (std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.begin(); it != m_expeditionMap.end(); it++) {
        idVec.push_back(it->first);
    }
}

void CExpedition::loadUsedHeros(std::vector<UINT32> & idVec) {
    for (std::set<UINT32>::iterator it = m_usedHeroSet.begin();
            it != m_usedHeroSet.end(); it++) {
        idVec.push_back(*it);
    }
}

bool CExpedition::heroInUsed(std::set<UINT32> & heroSet) {
    for (std::set<UINT32>::iterator it = heroSet.begin();
            it != heroSet.end(); it++) {
        if (m_usedHeroSet.find((*it)) != m_usedHeroSet.end()) {
            return true;
        }
    }
    return false;
}

void CExpedition::addUsedHero(std::set<UINT32> & heroSet) {
    for (std::set<UINT32>::iterator it = heroSet.begin();
            it != heroSet.end(); it++) {
        m_usedHeroSet.insert(*it);
    }
}

void CExpedition::addUsedHero(Global::stExpeditionRet * recvCmd) {
    for (int i = 0; i < recvCmd->usedhero_size(); i++) {
        m_usedHeroSet.insert(recvCmd->usedhero(i).id());
    }
}

void CExpedition::addExpedition(CExpeditionItem * pItem) {
    m_expeditionMap.insert(std::make_pair(pItem->getId(), pItem));
}

const CExpeditionItem * CExpedition::getExpedition(UINT16 id) {
    std::map<UINT16, CExpeditionItem*>::iterator it =
            m_expeditionMap.find(id);
    if (it != m_expeditionMap.end()) {
        return it->second;
    }
    return NULL;
}

void CExpedition::deleteExpedition(UINT16 id) {
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

void ExpeditionMgr::load() {
	CTblQueryResult* result = g_databaseEngine.select(g_expeditionTblDef,"EXPEDITION",NULL,NULL,0,NULL); 
	if (result){
		if (result->getData()){
			stExpeditionInfoDB * expeditionInfoDB = (stExpeditionInfoDB*) result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
                CExpedition * pExp = ATHENA_NEW CExpedition();
                pExp->loadAll((byte*)expeditionInfoDB[i].m_data);
                m_expeditionMgrMap.insert(std::make_pair(expeditionInfoDB[i].m_charID, pExp));
            }
        }
    }
	SAFE_DELETE(result);
}

void ExpeditionMgr::insertRecord(const stExpeditionInfoDB & data) {
	Athena::CUDSqlCallBack<ExpeditionMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ExpeditionMgr>(this,&ExpeditionMgr::insertRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_expeditionTblDef,(const char *)&data,"EXPEDITION",autoID,pDelaySql);
}

void ExpeditionMgr::insertRecordCB(bool opResult,UINT64 autoID) {

}

void ExpeditionMgr::updateRecord(const stExpeditionInfoDB & data) {
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",data.m_charID);
	
	Athena::CUDSqlCallBack<ExpeditionMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<ExpeditionMgr>(this,&ExpeditionMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_expeditionTblDef,(const char *)&data,"EXPEDITION",where,pDelaySql);
}

void ExpeditionMgr::updateRecordCB(bool opResult,UINT64 autoID) {

}

CExpedition * ExpeditionMgr::getExpedition(UINT32 id) {
    std::map<UINT32, CExpedition * >::iterator it =
            m_expeditionMgrMap.find(id);
    if (it != m_expeditionMgrMap.end()) {
        return it->second;
    }
    return NULL;
}

ExpeditionMgr::~ExpeditionMgr() {
    for (std::map<UINT32, CExpedition * >::iterator it = m_expeditionMgrMap.begin();
            it != m_expeditionMgrMap.end(); it++) {
        SAFE_DELETE(it->second);
    }
}

void ExpeditionMgr::addExpedition(UINT32 id, Global::stExpeditionRet * recvCmd) {
    CExpeditionItem * pItem = ATHENA_NEW CExpeditionItem();
    pItem->setId(recvCmd->expeditionid());
    pItem->setFinishTime(recvCmd->finishtime());
    pItem->setUsedHero(recvCmd);
    stExpeditionInfoDB data;
    data.m_charID = id;
    std::map<UINT32, CExpedition * >::iterator it =
            m_expeditionMgrMap.find(id);
    if (it == m_expeditionMgrMap.end()) {
        CExpedition * pExp = ATHENA_NEW CExpedition();
        m_expeditionMgrMap.insert(std::make_pair(id, pExp));
        pExp->addExpedition(pItem);
        pExp->addUsedHero(recvCmd);
        pExp->saveAll((byte*)data.m_data);
        insertRecord(data);
    } else {
        it->second->addExpedition(pItem);
        it->second->addUsedHero(recvCmd);
        it->second->saveAll((byte*)data.m_data);
        updateRecord(data);
    }
}

void ExpeditionMgr::delExpedition(UINT32 id, Global::stExpeditionRewardRet* recvCmd) {
    std::map<UINT32, CExpedition * >::iterator it =
            m_expeditionMgrMap.find(id);
    if (it != m_expeditionMgrMap.end()) {
        it->second->deleteExpedition((UINT16)recvCmd->expeditionid());
        stExpeditionInfoDB data;
        data.m_charID = id;
        it->second->saveAll((byte*)data.m_data);
        updateRecord(data);
    }
}

void ExpeditionMgr::fastExpedition(UINT32 id, Global::stFastExpeditionRet* recvCmd) {
    std::map<UINT32, CExpedition * >::iterator it =
            m_expeditionMgrMap.find(id);
    if (it != m_expeditionMgrMap.end()) {
        const CExpeditionItem * pItem = it->second->getExpedition((UINT16)recvCmd->expeditionid());
        if (pItem) {
            ((CExpeditionItem * )pItem)->fastFinish(recvCmd->finishtime());
            stExpeditionInfoDB data;
            data.m_charID = id;
            it->second->saveAll((byte*)data.m_data);
            updateRecord(data);
        }
    }
}

void ExpeditionMgr::cancelExpedition(UINT32 id, Global::stCancelExpeditionRet* recvCmd) {
    std::map<UINT32, CExpedition * >::iterator it =
            m_expeditionMgrMap.find(id);
    if (it != m_expeditionMgrMap.end()) {
        it->second->deleteExpedition((UINT16)recvCmd->expeditionid());
        stExpeditionInfoDB data;
        data.m_charID = id;
        it->second->saveAll((byte*)data.m_data);
        updateRecord(data);
    }
}

DevilCallMgr::~DevilCallMgr() {
    for (std::map<UINT32, CDevilCall* >::iterator it =
            m_devilMap.begin(); it != m_devilMap.end(); it++) {
        SAFE_DELETE(it->second);
    }
}

void DevilCallMgr::load() {
	CTblQueryResult* result = g_databaseEngine.select(g_devilCallTblDef,"DEVILCALL",NULL,NULL,0,NULL); 
	if (result){
		if (result->getData()){
			stDevilCallInfoDB * devilInfoDB = (stDevilCallInfoDB*) result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
                CDevilCall * pDevil = ATHENA_NEW CDevilCall();
                pDevil->load((byte*)devilInfoDB[i].m_data);
                m_devilMap.insert(std::make_pair(devilInfoDB[i].m_charID, pDevil));
            }
        }
    }
	SAFE_DELETE(result);
}

void DevilCallMgr::insertRecord(const stDevilCallInfoDB & data) {
	Athena::CUDSqlCallBack<DevilCallMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<DevilCallMgr>(this,&DevilCallMgr::insertRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_devilCallTblDef,(const char *)&data,"DEVILCALL",autoID,pDelaySql);
}

void DevilCallMgr::insertRecordCB(bool opResult,UINT64 autoID) {

}

void DevilCallMgr::updateRecord(const stDevilCallInfoDB & data) {
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",data.m_charID);
	
	Athena::CUDSqlCallBack<DevilCallMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<DevilCallMgr>(this,&DevilCallMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_devilCallTblDef,(const char *)&data,"DEVILCALL",where,pDelaySql);
}

void DevilCallMgr::updateRecordCB(bool opResult,UINT64 autoID) {

}

UINT32 CDevilCall::load(byte * buf) {
	UINT32 ret = 0;
    UINT8 num = *(UINT8 *)(buf + ret);
    ret += sizeof(UINT8);
    UINT16 id = 0;
    UINT32 drawNum = 0;
    for (UINT8 i = 0; i < num; i++) {
        id = *(UINT8 *)(buf + ret);
        ret += sizeof(UINT8);
        drawNum = *(UINT32 *)(buf + ret);
        ret += sizeof(UINT32);
        m_drawNumMap.insert(std::make_pair(id, drawNum));
    }
    num = *(UINT8 *)(buf + ret);
    ret += sizeof(UINT8);
    UINT8 rewardStatus = 0;
    for (UINT8 i = 0; i < num; i++) {
        id = *(UINT16 *)(buf + ret);
        ret += sizeof(UINT16);
        rewardStatus = *(UINT8 *)(buf + ret);
        ret += sizeof(UINT8);
        m_devilRewardMap.insert(std::make_pair(id, rewardStatus));
    }
    return ret;
}

UINT32 CDevilCall::save(byte * buf) {
    UINT32 ret = 0;
    *(UINT8 *)(buf + ret) = (UINT8)m_drawNumMap.size();
    ret += sizeof(UINT8);
    for (std::map<UINT8, UINT32>::iterator it =
            m_drawNumMap.begin(); it != m_drawNumMap.end(); it++) {
        *(UINT8 *)(buf + ret) = it->first;
        ret += sizeof(UINT8);
        *(UINT32 *)(buf + ret) = it->second;
        ret += sizeof(UINT32);
    }
    *(UINT8 *)(buf + ret) = (UINT8)m_devilRewardMap.size();
    ret += sizeof(UINT8);
    for (std::map<UINT16, UINT8>::iterator it =
            m_devilRewardMap.begin(); it != m_devilRewardMap.end(); it++) {
        *(UINT16 *)(buf + ret) = it->first;
        ret += sizeof(UINT16);
        *(UINT8 *)(buf + ret) = it->second;
        ret += sizeof(UINT8);
    }
    return ret;
}

UINT32 CDevilCall::getDrawNum(UINT8 id) {
    std::map<UINT8, UINT32>::iterator it =
        m_drawNumMap.find(id);
    if (it != m_drawNumMap.end()) {
        return it->second;
    }
    return 0;
}

void CDevilCall::updateDrawNum(UINT8 id, UINT32 num) {
    std::map<UINT8, UINT32>::iterator it =
        m_drawNumMap.find(id);
    if (it != m_drawNumMap.end()) {
        it->second = num;
    } else {
        m_drawNumMap.insert(std::make_pair(id, num));
    }
}

UINT8 CDevilCall::getRewardStatus(UINT8 id, UINT8 rewardType) {
    UINT16 compId = id;
    compId <<= 8;
    compId |= rewardType;
    std::map<UINT16, UINT8>::iterator it =
        m_devilRewardMap.find(compId);
    if (it != m_devilRewardMap.end()) {
        return it->second;
    }
    return MiscProto::stDevilCallItem::eNo_Reward;
}

void CDevilCall::updateRewardStatus(UINT8 id, UINT8 rewardType, UINT8 rewardStatus) {
    UINT16 compId = id;
    compId <<= 8;
    compId |= rewardType;
    std::map<UINT16, UINT8>::iterator it =
        m_devilRewardMap.find(compId);
    if (it != m_devilRewardMap.end()) {
        it->second = rewardStatus;
    } else {
        m_devilRewardMap.insert(std::make_pair(compId, rewardStatus));
    }
}

CDevilCall * DevilCallMgr::getDevilCall(UINT32 id) {
    std::map<UINT32, CDevilCall* >::iterator it =
        m_devilMap.find(id);
    if (it != m_devilMap.end()) {
        return it->second;
    }
    return NULL;
}

void DevilCallMgr::addDevilCall(UINT32 id) {
    CDevilCall * pDevil = ATHENA_NEW CDevilCall();
    m_devilMap.insert(std::make_pair(id, pDevil));
    stDevilCallInfoDB data;
    data.m_charID = id;
    pDevil->save((byte*)data.m_data);
    insertRecord(data);
}

void DevilCallMgr::updateDevilCallReward(UINT32 id, Global::stDevilCallRewardRet* recvCmd) {
    std::map<UINT32, CDevilCall* >::iterator it =
        m_devilMap.find(id);
    if (it != m_devilMap.end()) {
        it->second->updateRewardStatus(recvCmd->bossid(), recvCmd->rewardtype(), recvCmd->status());
        stDevilCallInfoDB data;
        data.m_charID = id;
        it->second->save((byte*)data.m_data);
        updateRecord(data);
    }
}

void DevilCallMgr::updateDevilCallDrawNum(UINT32 id, Global::stDevilCallRet* recvCmd) {
    std::map<UINT32, CDevilCall* >::iterator it =
        m_devilMap.find(id);
    if (it != m_devilMap.end()) {
        it->second->updateDrawNum(recvCmd->bossid(), recvCmd->drawnum());
        stDevilCallInfoDB data;
        data.m_charID = id;
        it->second->save((byte*)data.m_data);
        updateRecord(data);
    }
}
