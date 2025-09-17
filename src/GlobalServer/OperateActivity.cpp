#include "OperateActivity.h"

#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "KVPair.h"
#include "Misc.pb.h"
#include "OperateActivityCfg.h"

extern GlobalServer  * g_globalServer;

CTblField g_operateActivityTblDef[] = { 
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "DATA",           CTblField::DB_TYPE_BLOB,    MAX_OPERATE_ACTIVITY_NUM}, 
	{ NULL,             0,                          0 },
};

UINT32 OperateActivity::load(byte * buf) {
    UINT32 ret = 0;
    UINT8 activityNum = *(UINT8 *)(buf + ret);
    ret += sizeof(UINT8);
    UINT8 activityId, itemId, itemNum, contidionVal, rewardStatus;
    for (UINT8 i = 0; i < activityNum; i++) {
        activityId = *(UINT8 *)(buf + ret);
        ret += sizeof(UINT8);
        contidionVal = *(UINT32 *)(buf + ret);
        ret += sizeof(UINT32);
        itemNum = *(UINT8 *)(buf + ret);
        ret += sizeof(UINT8);
        std::map<UINT8, UINT8> itemMap;
        for (UINT8 k = 0; k < itemNum; k++) {
            itemId = *(UINT8 *)(buf + ret);
            ret += sizeof(UINT8);
            rewardStatus = *(UINT8 *)(buf + ret);
            ret += sizeof(UINT8);
            itemMap.insert(std::make_pair(itemId, rewardStatus));
        }
        m_activityMap[activityId] = std::make_pair(contidionVal, itemMap);
    }
    return ret;
}

UINT32 OperateActivity::save(byte * buf) {
	UINT32 ret = 0;
    *(UINT8 *)(buf + ret) = (UINT8)m_activityMap.size();
    ret += sizeof(UINT8);
    for (std::map<UINT8, std::pair<UINT32, std::map<UINT8, UINT8> > >::iterator it =
            m_activityMap.begin(); it != m_activityMap.end(); it++) {
        *(UINT8 *)(buf + ret) = it->first;
        ret += sizeof(UINT8);
        *(UINT32 *)(buf + ret) = (UINT32)it->second.first;
        ret += sizeof(UINT32);
        *(UINT8 *)(buf + ret) = (UINT8)it->second.second.size();
        ret += sizeof(UINT8);
        for (std::map<UINT8, UINT8>::iterator subIt =
                it->second.second.begin(); subIt != it->second.second.end(); subIt++) {
            *(UINT8 *)(buf + ret) = subIt->first;
            ret += sizeof(UINT8);
            *(UINT8 *)(buf + ret) = subIt->second;
            ret += sizeof(UINT8);
        }
    }
    return ret;
}

void OperateActivity::updateActivity(UINT8 acvitityId, UINT32 condition) {
    std::map<UINT8, std::pair<UINT32, std::map<UINT8, UINT8> > >::iterator it =
            m_activityMap.find(acvitityId);
    if (it != m_activityMap.end()) {
        it->second.first += condition;
    } else {
        std::map<UINT8, UINT8> itemMap;
        m_activityMap.insert(std::make_pair(acvitityId, std::make_pair(condition, itemMap)));
    }
}

void OperateActivity::updateItem(UINT8 acvitityId, UINT8 itemId, UINT8 status) {
    std::map<UINT8, std::pair<UINT32, std::map<UINT8, UINT8> > >::iterator it =
            m_activityMap.find(acvitityId);
    if (it != m_activityMap.end()) {
        std::map<UINT8, UINT8>::iterator subIt = it->second.second.find(itemId);
        if (subIt != it->second.second.end()) {
            subIt->second = status;
        } else {
            it->second.second.insert(std::make_pair(itemId, status));
        }
    }
}

bool OperateActivity::loadItem(UINT8 acvitityId, UINT8 itemId, UINT32 & condition, UINT8 & status) {
    std::map<UINT8, std::pair<UINT32, std::map<UINT8, UINT8> > >::iterator it =
            m_activityMap.find(acvitityId);
    if (it != m_activityMap.end()) {
        std::map<UINT8, UINT8>::iterator subIt = it->second.second.find(itemId);
        condition = it->second.first;
        if (subIt != it->second.second.end()) {
            status = subIt->second;
        } else {
            status = 0;
        }
        return true;
    }
    return false;
}

void OperateActivity::sendActivity(Player * pUser, std::map<UINT8, std::list<UINT8> > & idMap) {
    UINT8 status = 0;
    UINT32 condition = 0;
    MiscProto::stRetOperateActvity retCmd;
    MiscProto::stOperateActvityItems *pActivity = NULL;
    MiscProto::stOperateActvityItemInfo *pItem = NULL;
    for (std::map<UINT8, std::list<UINT8> >::iterator it = idMap.begin();
            it != idMap.end(); it++) {
        pActivity = retCmd.add_items();
        pActivity->set_activityid(it->first);
        condition = 0;
        for (std::list<UINT8>::iterator itemIt = it->second.begin();
                itemIt != it->second.end(); itemIt++) {
            pItem = pActivity->add_iteminfo();
            pItem->set_itemid(*itemIt);
            if (loadItem(it->first, *itemIt, condition, status)) {
                pItem->set_status(status);
            } else {
                pItem->set_status(0);
            }
            pActivity->set_condition(condition);
        }
    }
	pUser->sendProtoToMe(retCmd);
}

OperateActivityMgr::~OperateActivityMgr() {
    for (std::map<UINT32, OperateActivity* >::iterator it = m_operateMap.begin();
            it != m_operateMap.end(); it++) {
        SAFE_DELETE(it->second);
    }
}

void OperateActivityMgr::load() {
	CTblQueryResult* result = g_databaseEngine.select(g_operateActivityTblDef,"OPERATE_ACTIVITY",NULL,NULL,0,NULL); 
	if (result){
		if (result->getData()){
			stOperateActivityInfoDB * db = (stOperateActivityInfoDB*) result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
                OperateActivity * pItem = ATHENA_NEW OperateActivity();
                pItem->load((byte*)db[i].m_data);
                m_operateMap.insert(std::make_pair(db[i].m_charID, pItem));
            }
        }
    }
	SAFE_DELETE(result);
}

void OperateActivityMgr::insertRecord(const stOperateActivityInfoDB & data) {
	Athena::CUDSqlCallBack<OperateActivityMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<OperateActivityMgr>(this,&OperateActivityMgr::insertRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_operateActivityTblDef,(const char *)&data,"OPERATE_ACTIVITY",autoID,pDelaySql);
}

void OperateActivityMgr::insertRecordCB(bool opResult,UINT64 autoID) {
}

void OperateActivityMgr::updateRecord(const stOperateActivityInfoDB & data) {
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",data.m_charID);
	
	Athena::CUDSqlCallBack<OperateActivityMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<OperateActivityMgr>(this,&OperateActivityMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_operateActivityTblDef,(const char *)&data,"OPERATE_ACTIVITY",where,pDelaySql);
}

void OperateActivityMgr::updateRecordCB(bool opResult,UINT64 autoID) {
}

//如果不存在则创建
OperateActivity* OperateActivityMgr::getOperateActivity(UINT32 id) {
    std::map<UINT32, OperateActivity* >::iterator it =
            m_operateMap.find(id);
    if (it != m_operateMap.end()) {
        return it->second;
    }
    OperateActivity * pItem = ATHENA_NEW OperateActivity();
    if (pItem) {
        m_operateMap.insert(std::make_pair(id, pItem));
        stOperateActivityInfoDB data;
        data.m_charID = id;
        pItem->save((byte*)data.m_data);
        insertRecord(data);
    }
    return pItem;
}

void OperateActivityMgr::checkOperateActivity(UINT8 activityId, UINT32 id, UINT32 value) {
    if (COperateActivityCfg::getSingleton().checkExpire(activityId, g_globalServer->getSec())) {
        Athena::logger->trace("[OperateActivityMgr]checkOperateActivity not find activityId %d", activityId);
        return ;
    }
    std::map<UINT32, OperateActivity* >::iterator it =
            m_operateMap.find(id);
    if (it != m_operateMap.end()) {
        it->second->updateActivity(activityId, value);
        stOperateActivityInfoDB data;
        data.m_charID = id;
        it->second->save((byte*)data.m_data);
        updateRecord(data);
        return ;
    }
    OperateActivity * pItem = ATHENA_NEW OperateActivity();
    if (pItem) {
        m_operateMap.insert(std::make_pair(id, pItem));
        pItem->updateActivity(activityId, value);
        stOperateActivityInfoDB data;
        data.m_charID = id;
        pItem->save((byte*)data.m_data);
        insertRecord(data);
    }
}

void OperateActivityMgr::updateActivity(UINT32 id) {
    std::map<UINT32, OperateActivity* >::iterator it =
            m_operateMap.find(id);
    if (it != m_operateMap.end()) {
        stOperateActivityInfoDB data;
        data.m_charID = id;
        it->second->save((byte*)data.m_data);
        updateRecord(data);
    }
}
