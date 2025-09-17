#ifndef __H_DevilCall_H__
#define __H_DevilCall_H__

#include "TypesDef.h"
#include "SortList.pb.h"
#include <map>
#include <set>
#include <vector>

class GameUser;
class CDevilCall
{
public:
    //构造函数
    CDevilCall() {}
    //构造函数
    CDevilCall(GameUser * pOwner) : m_pOwner(pOwner) {}
    //析构函数
    ~CDevilCall() {}
	//加载数据
	UINT32 load(byte * buf);
	//保存数据
	UINT32 save(byte * buf, UINT32 needSpace);
    //获取抽奖次数
    UINT16 getDrawNum(UINT8 id);
    //更新获奖次数
    void updateDrawNum(UINT8 id, UINT16 num);
    //获取奖励状态
    UINT8 getRewardStatus(UINT8 id, UINT8 rewardType);
    //更新奖励状态
    void updateRewardStatus(UINT8 id, UINT8 rewardType, UINT8 rewardStatus);
private:
	GameUser * m_pOwner;
    std::map<UINT8, UINT16> m_drawNumMap;       //抽奖次数
    std::map<UINT16, UINT8> m_devilRewardMap;    //魔王召唤奖励
};

#endif
