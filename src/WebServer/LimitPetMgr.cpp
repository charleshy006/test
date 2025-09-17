#include "LimitPetMgr.h"

#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalSvrClient.h"
#include "GameSvrClientMgr.h"
#include "WebServer.h"
#include "KVPair.h"
#include "Global.pb.h"
#include "Game.pb.h"

CTblField g_limitPetActivityTblDef[] = {
	{ "ID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "GROUPID",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "PETID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "START",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "END",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_createLimitPetActivityTblDef[] = {
	{ "GROUPID",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "PETID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "START",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "END",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

void CLimitPetMgr::load() {
    //过期筛选
	CTblQueryResult * result = g_databaseEngine.select(g_limitPetActivityTblDef,"LIMIT_PET_ACTIVITY",NULL,NULL,0,NULL);
	if (result && result->getData()){                                                                          
		stLimitPetDB * pData = (stLimitPetDB * )result->getData();                                       
		for(UINT32 i = 0; i < result->getRowCount();++i){
            stLimitPet item;
            item.initFromDB(pData[i]);
            m_groupSet.insert(item.m_groupId);
            m_petMap.insert(std::make_pair(item.m_index, item));
		}                                                                                                      
	}                                                                                                          
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG                                                                                              
    for (std::map<UINT32, stLimitPet>::iterator it = m_petMap.begin();
            it != m_petMap.end(); it++) {
        Athena::logger->trace("限时随从活动配置: index %d, groupId %d, petId %d, start %d, end %d",
                it->second.m_index, it->second.m_groupId, it->second.m_petId, it->second.m_start, it->second.m_end);
    }
#endif                                                                                                         
}

void CLimitPetMgr::synGlobal() {
    Global::stSendLimitPetActivityData cmd;
    cmd.set_msg("web");
    for (std::map<UINT32, stLimitPet>::iterator it = m_petMap.begin();
            it != m_petMap.end(); it++) {
        Global::stAcitivityLimitPetItem * pItem = cmd.add_item();
        if (pItem) {
            pItem->set_id(it->second.m_index);
            pItem->set_activiyid(it->second.m_petId);
            pItem->set_starttime(it->second.m_start);
            pItem->set_endtime(it->second.m_end);
        }
    }
    GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void CLimitPetMgr::synGame() {
    Game::stSendLimitPetActivityData cmd;
    cmd.set_msg("web");
    for (std::map<UINT32, stLimitPet>::iterator it = m_petMap.begin();
            it != m_petMap.end(); it++) {
        Game::stAcitivityLimitPetItem * pItem = cmd.add_item();
        if (pItem) {
            pItem->set_id(it->second.m_index);
            pItem->set_activiyid(it->second.m_petId);
            pItem->set_starttime(it->second.m_start);
            pItem->set_endtime(it->second.m_end);
        }
    }
    GameSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

static bool checkInvalid(std::vector<stLimitPet> & petVec, std::map<UINT32, stLimitPet> & petMap) {
    for (size_t i = 0; i < petVec.size(); i++) {
        if (!petVec[i].m_petId || !petVec[i].m_index || !petVec[i].m_groupId) {
            return false;
        }
        if (!petVec[i].m_start || !petVec[i].m_end) {
            return false;
        }
        for (std::map<UINT32, stLimitPet>::iterator it = petMap.begin();
                it != petMap.end(); it++) {
            if (petVec[i].m_petId != it->second.m_petId) {
                continue ;
            }
            if (petVec[i].m_end >= it->second.m_start &&
                    petVec[i].m_start <= it->second.m_end) {
                return false;
            }
        }
    }
    return true;
}

bool CLimitPetMgr::check(UINT32 groupId, std::vector<stLimitPet> & petVec) {
    if (isUpdate(groupId)) {
        //修改记录
        std::map<UINT32, stLimitPet> petMap = m_petMap;
        for (size_t i = 0; i < petVec.size(); i++) {
            petMap.erase(petVec[i].m_index);
        }
        return checkInvalid(petVec, petMap);
    } else {
        //插入记录
        return checkInvalid(petVec, m_petMap);
    }
}

void CLimitPetMgr::insert(UINT32 groupId, std::vector<stLimitPet> & petVec) {
    m_groupSet.insert(groupId);

    for (size_t i = 0; i < petVec.size(); i++) {
        UINT64 autoID;
        stCreateLimitPetDB data;
        data.m_groupId = petVec[i].m_groupId;
        data.m_petId = petVec[i].m_petId;
        data.m_start = petVec[i].m_start;
        data.m_end = petVec[i].m_end;
		g_databaseEngine.insertRecord(g_createLimitPetActivityTblDef,(const char*)&data,"LIMIT_PET_ACTIVITY",autoID);
        petVec[i].m_index = autoID;
        m_petMap.insert(std::make_pair(petVec[i].m_index, petVec[i]));
        Athena::logger->trace("限时随从活动配置: index %d", petVec[i].m_index);
    }
}

void CLimitPetMgr::update(UINT32 groupId, std::vector<stLimitPet> & petVec) {
    //先删除,再添加,完成修改
    m_groupSet.erase(groupId);
    for (size_t i = 0; i < petVec.size(); i++) {
        m_petMap.erase(petVec[i].m_index);
    }

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "GROUPID=%u", groupId);
	g_databaseEngine.deleteRecord("LIMIT_PET_ACTIVITY",where,NULL);

    insert(groupId, petVec);
}

bool CLimitPetMgr::isUpdate(UINT32 groupId) {
    return m_groupSet.find(groupId) != m_groupSet.end();
}

void CLimitPetMgr::del(UINT32 groupId) {
    std::map<UINT32, stLimitPet> petMap;
    for (std::map<UINT32, stLimitPet>::iterator it = m_petMap.begin();
            it != m_petMap.end(); it++) {
        if (it->second.m_groupId != groupId) {
            petMap.insert(std::make_pair(it->first, it->second));
        }
    }
    m_petMap = petMap;
    m_groupSet.erase(groupId);

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "GROUPID=%u", groupId);
	g_databaseEngine.deleteRecord("LIMIT_PET_ACTIVITY",where,NULL);
}
