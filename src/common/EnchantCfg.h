#ifndef __ENCHANT_CONFIG_H__
#define __ENCHANT_CONFIG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>


class CEnchantCfg
{
	USE_SINGLETON_NEW(CEnchantCfg);
public:
    enum EnchantItemType {
        ITEM_FASHION_PIECE,     //时装碎片
        ITEM_ENCHANT,           //附魔石
        ITEM_INVALID,
    };
    struct EnchantType {
        UINT16 id;              //id
        UINT8  enchantItemType; //附魔材料
        UINT8  propNum;         //可以附魔的属性数量
        UINT32 lockNum;         //锁定数量
        UINT32 needGold;        //金币,必有项
        UINT32 enchantNum;      //附魔石数量,和碎片二选一
        UINT16 rangeMinId;      //附魔最小id
        UINT16 rangeMaxId;      //附魔最大id
        EnchantType () {
            id = 0;
            enchantItemType = ITEM_INVALID;
            propNum = 0;
            lockNum = 0;
            needGold = 0;
            enchantNum = 0;
            rangeMinId = 0;
            rangeMaxId = 0;
        }
    };

    struct EnchantItemInfo {
        UINT16 id;              //id
        UINT8  color;           //color
        UINT16 propType;        //增加的属性类型
        UINT32 rangePropMinNum; //增加的属性最小值
        UINT32 rangePropMaxNum; //增加的属性最大值
        UINT32 rate;            //概率
        UINT32 totalRate;       //概率
        EnchantItemInfo () {
            id = 0;
            color = 0;
            propType = 0;
            rangePropMinNum = 0;
            rangePropMaxNum = 0;
            rate = 0;
            totalRate = 0;
        }
    };
public:
    CEnchantCfg();
    ~CEnchantCfg();
    //获得实例
    static CEnchantCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    bool getEnchantType(UINT8 enchantItemType, UINT32 lockNum, EnchantType & enchantType);
    bool getEnchantItem(UINT32 id, EnchantItemInfo & item);
    UINT16 randPropId(UINT32 minId, UINT32 maxId);
private:
    std::map<UINT8, std::map<UINT32, EnchantType> > m_enchantTypeMap;
    std::vector<EnchantItemInfo> m_itemVec;
    UINT32 m_totalRate;
};
#endif
