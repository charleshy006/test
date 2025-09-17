#ifndef __H_RMB_SHOP_H__
#define __H_RMB_SHOP_H__
#include "TypesDef.h"
// #include "Singleton.h"
#include <map>
#include <set>

/**
 * 人民币商店
 **/
class GameUser;
class CRMBShop
{
public :
	static const UINT32 s_SAVE_VERSION_INFO = 20150201;
public :
	enum {
		eItem_Month_Card = 20,	//月卡
		eItem_Quarter_Card = 2, //季卡
		eItem_Year_Card = 3,	//年卡
	};
	enum {
		eType_Summon_Equip = 51,	//装备
		eType_Summon_Hero = 52, //英雄
	};
	enum {
		eType_Treasure_Box_Type1 = 1,	//表示每日刷新 0点
		eType_Treasure_Box_Type2 = 2, //表示间隔时间刷新 开服时间 
		eType_Treasure_Box_Type3 = 3,	//表示不刷新 永久 
		eType_Treasure_Box_Type4 = 4, //表示每周星期一凌晨 0点
	};
	enum {
		eType_Time_Year = 1,	//月卡季卡年卡等有时间限制的卡
		eType_Forever = 2,		//永久生效的福利卡
		eType_Normal = 3,		//普通卡
		eType_Grown = 4,		//成长计划,只能买一次
		eType_Summon = 5,		//战令, 配表type5没使用
		eType_Treasure_Box = 6, //宝箱
	};
	struct stRMBShopItem
	{
		UINT32 m_id ;						//id唯一标志
		std::string m_productID;			//产品ID
		UINT8  m_type;						//类型
		UINT32 m_needRMB;					//需要人民币
		UINT32 m_obtainDiamond; 			//获得钻石
		UINT8  m_hasFirstReward;			//是否有首次奖励
		UINT32 m_firstBuyExtraDiamond;		//是否购买获得额外钻石
		UINT32 m_extraDiamond;				//额外获得钻石
		UINT32 m_boxID;						//宝箱id
		UINT32 m_boxNum;					//宝箱数量
		UINT32 m_buyCount;					//购买次数
		UINT32 m_refreshType;				//刷新规则类型
		UINT32 m_refreshTime;				//最后截止时间

		stRMBShopItem()
		{
			m_id = 0;
			m_type = 0;
			m_needRMB = 0;
			m_obtainDiamond = 0;
			m_hasFirstReward = 0;
			m_firstBuyExtraDiamond = 0;
			m_extraDiamond = 0;
			m_boxID = 0;
			m_boxNum = 0;
			m_buyCount = 0;
			m_refreshType = 0;
			m_refreshTime = 0;
		}
	};
	
	struct stShopItem
	{
		UINT16 m_id;			//商品ID
		UINT32 m_endTime;		//结束时间

		stShopItem()
		{
			m_id = 0;
			m_endTime = (UINT32)-1;
		}
	};
public :
	//构造函数
	CRMBShop(GameUser * pOwner);
	//析构函数
	~CRMBShop();
	//保存商店数据
	UINT32 save(byte * buf,UINT32 needSpace);
	//加载商店数据
	UINT32 load(byte * buf);
	//上线同步人民币商店信息
	void synRMBInfo2Client();
	//上线同步人民币商店信息2
	void synRMBInfo2Client2(UINT32 lastLoginTime);
	//购买商店
	void buy(UINT32 money,UINT16 id,std::string  tid, const std::string & data = "", bool isExtend = false);
	//是否购买了购买道具
	bool isBuyItem(UINT16 itemID);
	//道具结束时间
	UINT32 getItemEndTime(UINT16 itemID);
	//是否购买了福利卡
	bool  isBuyForeverItem();
    //按类型删除玩家卡片
    bool cancelCard(UINT16 type);
    //item id是否合法
    bool existsItem(UINT32 itemId);
	//check opid对应的item的money
	bool checkOpItemMoney(UINT32 itemID, UINT32 opID, UINT32 money);
    //根据id找到对应的配置
    static UINT32 getItemMoney(UINT32 itemId);
	//加载配置
	static bool loadCfg();
    //加载货币配置
	static bool loadMoneyCfg();
	//获得首冲礼包ID
	static UINT32 firstRechargeBoxID()
	{
		return s_firstRechargeBoxID;
	}
	//首次10元充值礼包
	static UINT32 first10RechargeBoxID()
	{
		return s_first10RechargeBoxID;
	}
    //根据itemId获得对应国家的货币
	static UINT32 getMoney(UINT32 itemId);
private :
	static std::map<UINT32,stRMBShopItem> s_shopItemCfgs;		//物品ID配置
    static std::map<UINT32, UINT32> s_id2MoneyCfgs;             //itemId对应国家货币的配置
	std::map<UINT16,stShopItem>	m_buyShopInfo;					//购买的信息
	GameUser * m_pOwner;
	static UINT32 s_firstRechargeBoxID;
	static UINT32 s_first10RechargeBoxID;
	static std::map<UINT32,UINT32> s_money2ItemID;
	static std::map<UINT32, std::set<UINT32> > m_id2MoneyMap;   //id和所有渠道金额的映射
	static std::map<UINT32, std::map<UINT32, UINT32> > m_opid2MoneyMap; 
};

// class CRMBShopMgr
// {
// 	USE_SINGLETON_NEW(CRMBShopMgr);
// private :
// 	//构造函数
// 	CRMBShopMgr();
// public :
// 	//析构函数
// 	~CRMBShopMgr();
// 	//获得实例
// 	static CRMBShopMgr & getSingleton()
// 	{   
// 		return THE_SINGLETON::Instance();
// 	} 

// 	void RechargeCheck(UINT32 rechargeId);

// private :
// 	GameUser * mgr_pOwner;		
// 	CRMBShop * m_rmb_shop;					
// };

#endif
