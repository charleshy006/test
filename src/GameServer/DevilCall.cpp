#include "DevilCall.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "DevilCallMgr.h"
#include "Misc.pb.h"

UINT32 CDevilCall::load(byte * buf) {
	UINT32 ret = 0;
    UINT8 num = *(UINT8 *)(buf + ret);
    ret += sizeof(UINT8);
    UINT16 id = 0, drawNum = 0;
    for (UINT8 i = 0; i < num; i++) {
        id = *(UINT8 *)(buf + ret);
        ret += sizeof(UINT8);
        drawNum = *(UINT16 *)(buf + ret);
        ret += sizeof(UINT16);
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

UINT32 CDevilCall::save(byte * buf, UINT32 needSpace) {
    UINT32 ret = 0;
    *(UINT8 *)(buf + ret) = (UINT8)m_drawNumMap.size();
    ret += sizeof(UINT8);
    for (std::map<UINT8, UINT16>::iterator it =
            m_drawNumMap.begin(); it != m_drawNumMap.end(); it++) {
        *(UINT8 *)(buf + ret) = it->first;
        ret += sizeof(UINT8);
        *(UINT16 *)(buf + ret) = it->second;
        ret += sizeof(UINT16);
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

UINT16 CDevilCall::getDrawNum(UINT8 id) {
    std::map<UINT8, UINT16>::iterator it =
        m_drawNumMap.find(id);
    if (it != m_drawNumMap.end()) {
        return it->second;
    }
    return 0;
}

void CDevilCall::updateDrawNum(UINT8 id, UINT16 num) {
    std::map<UINT8, UINT16>::iterator it =
        m_drawNumMap.find(id);
    if (it != m_drawNumMap.end()) {
        it->second += num;
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
