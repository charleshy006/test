#ifndef __RUNE_CONFIG_H__
#define __RUNE_CONFIG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <vector>

class CRuneCfg
{
	USE_SINGLETON_NEW(CRuneCfg);
public:
    struct CombineItem {
        UINT32 m_id;
        UINT32 m_runeId;
        UINT32 m_rate;
        CombineItem () {
            m_id = 0;
            m_runeId = 0;
            m_rate = 0;
        }
    };

    struct RequireItem {
        UINT32 m_id;
        UINT32 m_pieceId;
        UINT32 m_soulFire;
        UINT32 m_totalRate;
        std::vector<CombineItem> m_combineVec;
        RequireItem () {
            m_id = 0;
            m_pieceId = 0;
            m_soulFire = 0;
            m_totalRate = 0;
        }
    };

public:
    //获得实例
    static CRuneCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    //获取合成信息
    RequireItem * getRequireItem(UINT32 itemId);

private:
    std::map<UINT32, RequireItem> m_runeCombineMap;
};
#endif
