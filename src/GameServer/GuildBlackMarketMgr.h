#ifndef __GUILD_BLACK_MARKET_MGR_H__
#define __GUILD_BLACK_MARKET_MGR_H__

#include "Singleton.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "ShopMgr.h"
#include <map>

class CGuildBlackMarketMgr : public CShopMgr 
{
public:
	//构造函数
	CGuildBlackMarketMgr(GameUser * pOwner);
	//析构函数
	~CGuildBlackMarketMgr();
	//加载数据
	UINT32 loadAllShop(const char * buf);
	//保存数据
	UINT32 saveAllShop(char * buf);
	//打开窗口
	void openWindow();
	//购买商品
	void buyItem(UINT32 index);
	//给自己发送商店列表                                                  
	void sendShopList2Me(); 
	//发送商店列表
	void sendShopList2Me(stShopDB & shopDB,const stShop & shopCfg);
	//刷新商店列表
	void reqRefreshShopList();
	//是否可以刷新
	bool canRefresh(stShopDB & shopDB,const stShop shopCfg);        
	//设置上一次刷新时间
	void setLastRefreshTime(stShopDB & shopDB,const stShop shopCfg);
	//获得购买次数	
	UINT32 getBuyNum();
	//获得刷新次数
	UINT32 getRefreshNum();
	//获得钻石刷新次数
	UINT32 getDiamondNum();
};

#endif
