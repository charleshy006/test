#ifndef __DRAGON_NEST_CONFIG_H__
#define __DRAGON_NEST_CONFIG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <list>

class CDragonNestCfg
{
	USE_SINGLETON_NEW(CDragonNestCfg);
public:
    struct DragonNestLvl {
        UINT32 index;
        UINT32 lvlId;
        UINT32 nextLvl;
        UINT32 openWeek;
        bool   firstLvl;
        DragonNestLvl () {
            index = 0;
            lvlId = 0;
            nextLvl = 0;
            openWeek = 0;
            firstLvl = false;
        }
    };
public:
    //获得实例
    static CDragonNestCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    //获取重置消耗价格
    UINT32 getCost(UINT32 times);
    //获取指定level id对应的信息
    const DragonNestLvl * getDragonNest(UINT32 lvlId);
    //找到对应的前置关卡
    UINT32 getPreLvlId(UINT32 lvlId);

private:
    std::map<UINT32, DragonNestLvl> m_dragonMap;
    std::vector<UINT32> m_costVec;
};
#endif
