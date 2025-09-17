#ifndef __INVITE_CONFIG_H__
#define __INVITE_CONFIG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <list>

class CInviteCfg
{
	USE_SINGLETON_NEW(CInviteCfg);
public:
    struct InviteReward {
        UINT8 id;
        UINT32 inviteNum;
        UINT32 rewardBox;
    };
    struct ShareReward {
        UINT8 id;
        UINT32 shareNum;
        UINT32 rewardBox;
    };
public:
    //获得实例
    static CInviteCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    //获取分享奖励
    const InviteReward * getInviteReward(UINT8 id);
    UINT32 getInviteRewardBox() {
        return m_invitedRewardBox;
    }
    UINT32 getDailySharedRewardBox() {
        return m_dailySharedRewardBox;
    }
    //获取每日分享奖励
    const ShareReward * getSharedReward(UINT8 id);
    UINT32 getLimitLvl() {
        return m_limitLevel;
    }

private:
    UINT32 m_limitLevel;                                //邀请码使用等级
    UINT32 m_invitedRewardBox;                          //输入其他玩家邀请码获得奖励
    std::map<UINT8, InviteReward> m_inviteRewardMap;   //邀请人邀请累计数量奖励
    UINT32 m_dailySharedRewardBox;                           //每日分享奖励
    std::map<UINT8, ShareReward> m_totalShareRewardMap;     //累计分享奖励

};
#endif
