#ifndef __EXPEDITION_CONFIG_H__
#define __EXPEDITION_CONFIG_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <vector>

class CExpeditionCfg
{
	USE_SINGLETON_NEW(CExpeditionCfg);
public:
    enum ExtraNeedType {
        NONE,           //无
        REMOTE,         //远程型
        NEAR,           //近程型
        ATTACK,         //攻击型
        DEFENSE,        //防御型
        ASSIST,         //辅助型
        TOTAL_POWER,    //战斗力总和
        LVL_GRATER,     //有等级大于
    };
    struct stTask {
        UINT32 m_id;                        //任务id
        UINT32 m_minLvl;                    //最小进入等级
        UINT32 m_maxLvl;                    //最大进入等级
        UINT32 m_needTime;                  //任务需要消耗时间
        UINT32 m_heroNum;                   //随从需求数量
        UINT32 m_basePower;                 //基础战斗力
        UINT32 m_type;                      //额外任务种类
        std::map<UINT32, UINT32> m_baseRewardMap;       //奖励道具
        std::map<UINT32, UINT32> m_extraConditionMap;   //额外奖励条件
        std::map<UINT32, UINT32> m_extraRewardMap;      //额外奖励道具

        stTask() {
            m_id = 0;
            m_minLvl = 0;
            m_maxLvl = 0;
            m_needTime = 0;
            m_heroNum = 0;
            m_basePower = 0;
            m_type = 0;
        }
    };

    struct stExpeditionConfig {
        UINT32 m_minLvl;                    //最小进入等级
        UINT32 m_maxLvl;                    //最大进入等级
        UINT32 m_maxTimes;                  //最大进入次数
    };
private:
    //构造函数
    CExpeditionCfg() {
        m_baseTime = 1;
        m_baseDiamond = 1;
        m_minLvl = 0;
        m_maxLvl = 0;
    }
public:
    //析构函数
	~CExpeditionCfg();
    //获得实例
    static CExpeditionCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    //获取快速远征基础时间
    UINT32 getBaseTime() {
        return m_baseTime;
    }
    //获取快速远征基础消耗钻石
    UINT32 getBaseDiamond() {
        return m_baseDiamond;
    }
    //最大进入次数
    UINT32 getMaxTime(UINT32 level);
    //获得最小进入等级
    UINT16 getMinLvl() {
        return m_minLvl;
    }
    //获得最大进入等级
    UINT16 getMaxLvl() {
        return m_maxLvl;
    }
    //获得任务信息
    const stTask * getTask(UINT32 id);
    //是否是相同类型任务
    bool hasSameTask(UINT32 targetType, std::vector<UINT16> & idVec);
private:
    UINT32 m_baseTime;                              //快速远征基础时间
    UINT32 m_baseDiamond;                           //快速远征消耗钻石
    UINT16 m_minLvl;                                //远征最小等级
    UINT16 m_maxLvl;                                //远征最大等级
    std::vector<stExpeditionConfig> m_expeditionConfigVec; //远征配置信息
    std::map<UINT32, stTask> m_taskMap;             //远征任务信息
};
#endif
