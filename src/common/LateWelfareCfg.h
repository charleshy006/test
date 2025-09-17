#ifndef _LATE_WELFARE_CFG_H_
#define _LATE_WELFARE_CFG_H_


#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <list>

class CLateWelfareCfg
{
	USE_SINGLETON_NEW(CLateWelfareCfg);
public:
    struct RewardInfo {
        UINT32 itemId;
        UINT32 num;
        RewardInfo () {
            itemId = 0;
            num = 0;
        }
    };
    struct RewardItem {
        UINT32 id;
        UINT32 costDiamond;
        std::vector<RewardInfo> rewardVec;
    };
    struct WalfareItem {
        UINT32 id;              //活动id
        UINT32 startDay;        //开服第几天
        UINT32 duration;        //持续天数
        UINT32 activityTime;    //活动持续时间
        UINT32 showLv;          //显示等级
        std::map<UINT32, RewardItem> itemMap;//奖励信息

        WalfareItem () {
            id = 0;
            startDay = 0;
            duration = 0;
            activityTime = 0;
            showLv = 0;
        }
    };

public:
    //获得实例
    static CLateWelfareCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    //活动是否开放
    bool isOpen() {
        return m_isOpen;
    }
    //根据创建角色距离开服时间获取配置
    bool getWalfare(UINT32 day, WalfareItem & item);
    //根据id获取配置
    bool getRewardItem(UINT32 activityId, UINT32 rewardId, RewardItem & item);

private:
    bool m_isOpen;
    std::map<UINT32, WalfareItem> m_walfareMap;
};
#endif
