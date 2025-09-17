#ifndef _RES_BACK_CFG_H_
#define _RES_BACK_CFG_H_


#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <list>

class CResBackCfg
{
	USE_SINGLETON_NEW(CResBackCfg);
public:
    enum ResType {
        Big_Sp = 1,     //大体力丹购买次数
        Big_Exp = 2,    //大历练丹购买次数
        Sp_Box = 3,     //每日体力礼包购买次数
        
        Emerald_Dream = 101,    //翡翠梦境
        Fam_Trasure = 102,      //秘境寻宝
        Three_Vs_Three = 103,   //3V3战场
        Escort_Ship = 104,      //护送飞船
        Letai_King = 105,       //擂台霸主

        Team_Map = 201,         //组队副本
        Gold_Map = 202,         //金币副本
        Stone_Map = 203,        //龙晶石副本
        Dragon_Map = 204,       //巨龙巢穴
        Balck_Map = 205,        //黑石深渊
    };

    struct RewardInfo {
        UINT32 itemId;
        UINT32 num;
        RewardInfo () {
            itemId = 0;
            num = 0;
        }
    };
    struct RewardItem {
        UINT32 type;
        UINT32 costDiamond;
        std::vector<RewardInfo> rewardVec;
        RewardItem () {
            type = 0;
            costDiamond = 0;
        }
    };

public:
    //获得实例
    static CResBackCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    //获取最大次数
    UINT32 getMaxNum(UINT32 type, UINT32 vipLvl);
    //获取物品
    bool getItem(UINT32 type, RewardItem & item);
    //获取所有类型
    std::vector<UINT32> & getAllType() {
        return m_typeVec;
    }

private:
    std::vector<UINT32> m_typeVec;
    std::map<UINT32, RewardItem> m_rewardMap;
};
#endif
