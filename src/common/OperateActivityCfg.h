#ifndef __OPERATEACTIVITY_CONFIG_H__
#define __OPERATEACTIVITY_CONFIG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <list>

class COperateActivityCfg
{
	USE_SINGLETON_NEW(COperateActivityCfg);
public:
    enum COperateActivityType {
        Common_Map = 1,             //普通副本完成次数
        Elite_Map = 2,              //精英副本完成次数
        Buy_Gold = 3,               //购买金币次数
        Athletics_Challenge = 4,    //竞技场挑战次数
        Group_Map = 5,              //挑战组队副本次数
        Diamond_Lottery = 6,        //钻石开宝箱活动
        One_Vs_One = 7,             //1V1完成活动
        Three_Vs_Three = 8,         //3V3完成活动
        Draw_Box = 9               //抽宝箱活动
    };
    struct ItemInfo {
        UINT8 id;
        UINT32 condition;
        UINT32 boxId;
    };
    struct ActivityItem {
        UINT8 id;
        UINT32 start;
        UINT32 end;
        std::map<UINT8, ItemInfo> itemInfos;
    };
public:
    //析构函数
	~COperateActivityCfg();
    //获得实例
    static COperateActivityCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    //获取已开启活动列表
    void loadActiveIds(UINT32 now, std::map<UINT8, std::list<UINT8> > &idMap);
    //活动是否过期
    bool checkExpire(UINT8 id, UINT32 now);
    //活动是否有效
    bool isValidItem(UINT8 id, UINT8 itemId);
    //获取一个condition
    UINT32 getCondition(UINT8 id, UINT8 itemId);
    //获取boxId
    UINT32 getBoxId(UINT8 id, UINT8 itemId);
    //根据type找id
    UINT8 findIdByType(UINT8 type);
private:
    std::map<UINT8, UINT8> m_type2IdMap;
    std::map<UINT8, ActivityItem> m_activityMap;
};
#endif
