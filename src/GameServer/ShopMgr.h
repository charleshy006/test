#ifndef __H_SHOP_MGR_H__
#define __H_SHOP_MGR_H__
#include "TypesDef.h"
#include <map>

class GameUser;
class CShopMgr
{
public :
	const static UINT32 s_SHOP_DB_VERSION = 20150130;
	const static UINT32 s_DIAMOND_REFRESH_COST = 20;
public :
	enum {
		SHOP_NORMAL = 1,	//普通商店
		SHOP_WELFARE = 2,	//福利商店
		SHOP_HEISHI = 3,	//黑市商店
		SHOP_GUILD = 9,		//公会商店
		SHOP_VIPWEEK = 10,  //VIP周礼包
		SHOP_ARENARANK = 13,//竞技场排名
		SHOP_HEISHI2 = 102,  //黑市商店
	};

	enum {
		REFRESH_TYPE_EVERYDAY = 1,		//每天刷新
		REFRESH_TYPE_INTERVAL = 2,		//刷新间隔
		REFRESH_TYPE_INFINITE = 3,		//永久不刷新
		REFRESH_TYPE_WEEK = 4,			//每日刷新
	};

	struct stShopItem
	{
		stShopItem()
		{
			m_uniqueID = 0;
			m_itemID = 0;
			m_rate = 0;
			m_maxBuyNum = 0;
			m_buyNumOneTime = 0;
			m_costResType = 0;
			m_curPrice = 0;
			m_oriPrice = 0;
			m_vipNumID = 0;
			m_priceID = 0;
			m_retainBuyNum = false;
		}
		
		UINT32 m_uniqueID;				//唯一ID
		UINT32 m_itemID;				//道具ID
		UINT32 m_rate;					//概率
		UINT32 m_maxBuyNum;				//最大购买数量
		UINT32 m_buyNumOneTime;			//每次购买数量
		UINT8  m_costResType;			//花费资源类型
		UINT32 m_curPrice;				//当前价钱
		UINT32 m_oriPrice;				//原来的价格
		UINT16 m_buyLimitLv;			//购买等级限制
		UINT16 m_vipNumID;				//vip表里对应的ID
		UINT16 m_priceID;				//价格表对应的ID
		bool   m_retainBuyNum;			//保留购买次数
	};

	struct stShop
	{
		stShop()
		{
			m_shopID = 0;
			m_refreshTime = 0;
			m_costResType = 0;
			m_costResNum = 0;
			m_sellNum = 0;
			m_openLv = 0;
			m_isRandom = false;
			m_refreshType = REFRESH_TYPE_INFINITE;
		}

		UINT32 m_shopID;				//商店ID
		std::string m_shopName;			//商店名字
		UINT8  m_refreshType;			//刷新类型
		UINT32 m_refreshTime;			//刷新时间
		UINT32 m_costResType;			//资源类型
		UINT32 m_costResNum;			//花费资源数量
		UINT16 m_sellNum;				//出售列表数量
		UINT16 m_openLv;				//开放等级
		bool   m_isRandom;				//是否随机

		std::map<UINT32,stShopItem> m_itemVec; //出售的物品
	};

	struct stShopItemDB
	{
		stShopItemDB()
		{
			m_uniqueID = 0;
			m_itemID = 0;
			m_buyNum = 0;
			m_buyNumPerTime = 0;
		}

		UINT32 m_uniqueID;		//唯一ID
		UINT32 m_itemID;		//道具ID
		UINT32 m_buyNum;		//已经购买数量
		UINT16 m_buyNumPerTime; //每次买多少
	};

	struct stShopDB
	{
		stShopDB()
		{
			m_shopID = 0;
			m_lastRefreshTime = 0;
		}

		UINT32 m_shopID;			//商店ID
		UINT32 m_lastRefreshTime;	//最近一次刷新时间

		std::vector<stShopItemDB> m_itemVec;	
	};
public :
	//构造函数
	CShopMgr(GameUser * pOwner);
	//析构函数
	~CShopMgr();
	//保存商店数据
	UINT32 saveAllShop(byte * buf,UINT32 needSpace);
	//加载商店数据
	UINT32 loadAllShop(byte * buf);
	//加载所有配置
	static bool loadCfg();
	//随机商店列表
	void ramdomShopList(stShopDB & shopDB,const stShop & shopCfg);
	//自动刷新商店列表
	void autoRefreshShopList(stShopDB & shopDB,const stShop & shopCfg);
	//发送商店列表
	void sendShopList2Me(stShopDB & shopDB,const stShop & shopCfg);
	//给自己发送商店列表
	void sendShopList2Me(UINT32 shopID);
	//购买道具
	void buyItem(UINT32 shopID,UINT32 index,UINT32 num, bool isFashion = false);	
	//刷新商店列表
	void reqRefreshShopList(UINT32 shopID);
	//是否可以刷新
	bool canRefresh(stShopDB & shopDB,const stShop shopCfg);
	//设置上一次刷新时间
	void setLastRefreshTime(stShopDB & shopDB,const stShop shopCfg);
	//获得剩余时间
	UINT32 getRemainTime(stShopDB & shopDB,const stShop shopCfg);
	//钻石购买金币
	void diamondBuyGold();
public :
	GameUser * m_pOwner;
	std::map<UINT32,stShopDB> m_shopDB;
	static std::map<UINT32,stShop> m_shopCfg;
};
#endif
