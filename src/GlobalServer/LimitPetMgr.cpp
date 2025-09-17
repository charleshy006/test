#include "LimitPetMgr.h"

#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "KVPair.h"
#include "GlobalServer.h"
#include "LimitTimePetCfg.h"
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

void CLimitPetMgr::load() {
    //过期筛选
	CTblQueryResult * result = g_databaseEngine.select(g_limitPetActivityTblDef,"LIMIT_PET_ACTIVITY",NULL,NULL,0,NULL);
	if (result && result->getData()){                                                                          
		stLimitPetDB * pData = (stLimitPetDB * )result->getData();                                       
		for(UINT32 i = 0; i < result->getRowCount();++i){
            stLimitPet item;
            item.initFromDB(pData[i]);
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

void CLimitPetMgr::synInfo() {
    std::map<UINT32, LimitTimePetCfg::stActivityOpenInfo> itemMap;
    for (std::map<UINT32, stLimitPet>::iterator it = m_petMap.begin();
            it != m_petMap.end(); it++) {
        LimitTimePetCfg::stActivityOpenInfo item;
        item.m_id = it->second.m_index;
        item.m_activityID = it->second.m_petId;
        item.m_beginTime = it->second.m_start;
        item.m_endTime = it->second.m_end;
        itemMap.insert(std::make_pair(item.m_id, item));
    }
    LimitTimePetCfg::getSingleton().setOpenInfo(itemMap);
}
