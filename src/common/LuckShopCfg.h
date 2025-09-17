#ifndef __H_LUCKSHOP_CFG_H__
#define __H_LUCKSHOP_CFG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <map>

class LuckShopCfg
{
	USE_SINGLETON_NEW(LuckShopCfg);
public :
	struct stRewardItem
	{
		UINT16 m_id;			//奖励项目的ID
		UINT32 m_rate;			//概率
		UINT32 m_needRecharge;	//需要充值
		UINT32 m_boxID;			//礼包ID

		stRewardItem()
		{
			m_id = 0;
			m_rate = 0;
			m_needRecharge = 0;
			m_boxID = 0;
		}
	};

	struct stScreenInfo
	{
		UINT16 m_vipLv;								//vip等级
		std::map<UINT16,stRewardItem> m_rewards;	//可能获得的奖励

		stScreenInfo()
		{
			m_vipLv = 0;
		}
	};
private :
	//构造函数
	LuckShopCfg();
public :
	//析构函数
	~LuckShopCfg();
	//获得实例
	static LuckShopCfg & getSingleton()
	{   
		return THE_SINGLETON::Instance();
	} 
	//加载所有配置
	bool loadCfg();
	//关闭时间
	bool isClose(UINT32 secOfDay)
	{
		return (secOfDay >= m_closeBeginTime) && (secOfDay <= m_closeEndTime);
	}
	//获得刷新时间
	UINT32 getRefreshTime()
	{
		return m_freshTime;
	}
	//获得某个vip等级的奖励
	stRewardItem  * getRewardItem(UINT16 vipLv,UINT16 id);
	//随机某个vip等级一条奖励
	UINT16 randVipRewardID(UINT16 vipLv);	
private :
	UINT32 m_closeBeginTime;					//关闭开始时间
	UINT32 m_closeEndTime;						//关闭时间
	UINT32 m_freshTime;							//刷新时间
	std::map<UINT16,stScreenInfo> m_screenInfos;//vip等级相关
};
#endif
