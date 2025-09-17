#ifndef __H_LUCK_SHOP_MGR_H__
#define __H_LUCK_SHOP_MGR_H__
#include "TypesDef.h"
#include "RefreshDataBase.h"
#include <map>

/**
 * 幸运商店
 **/
class GameUser;
class CLuckShopMgr : public CRefreshDataBase
{
public :
	static const UINT32 s_SAVE_VERSION_INFO = 20160201;
public :
	//构造函数
	CLuckShopMgr(GameUser * pOwner);
	//析构函数
	~CLuckShopMgr();
	//保存商店数据
	UINT32 save(byte * buf);
	//获得保存数据
	UINT32 getSaveSize();
	//加载商店数据
	UINT32 load(byte * buf);
	//获得utc时间
	virtual UINT32 curTime();
	//刷新数据
	virtual void doRefreshData();
	//玩家登录
	void userOnline();
	//玩家充值了
	void onRecharge(UINT32 num);
	//领取奖励
	void doReward();
	//设置打开窗口时间
	void setOpenWinTime();
private :
	GameUser * m_pOwner;
	UINT16     m_curRewardVipLv;			//当前奖励的vip等级
	UINT16     m_rewardID;					//奖励的条目ID
	UINT8      m_hasGotReward;				//是否已经获得奖励
	UINT32     m_todayHasRecharge;			//今天已经充值了多少
	UINT32     m_openWinTime;				//打开窗口时间
	UINT32	   m_meetCondTime;				//满足条件时间
};
#endif
