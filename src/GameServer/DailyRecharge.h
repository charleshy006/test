#ifndef __TODAY_REWARD_H__
#define __TODAY_REWARD_H__

#include "TypesDef.h"  
#include "Singleton.h" 
#include <map>

struct stActivity
{
	UINT32 m_id;					//ID
	UINT32 m_rechargeDiamondNum;	//充值钻石数量
	UINT32 m_startTime;				//开始时间
	UINT32 m_endTime;				//结束时间
	UINT32 m_rewardItemID1;			//ItemID1
	UINT32 m_rewardItemNum1;		//ItemNum1
	UINT32 m_rewardItemID2;         //ItemID2
	UINT32 m_rewardItemNum2;        //ItemNum2
	UINT32 m_rewardItemID3;         //ItemID3
	UINT32 m_rewardItemNum3;        //ItemNum3
	UINT16 m_needBagNum;			//需要背包空间
	
	stActivity()
	{
		m_id					=	0;                
		m_rechargeDiamondNum	=	0;
		m_startTime				=	0;         
		m_endTime				=	0;           
		m_rewardItemID1			=	0; 
		m_rewardItemNum1		=	0;
		m_rewardItemID2			=	0; 
		m_rewardItemNum2		=	0;
		m_rewardItemID3			=	0; 
		m_rewardItemNum3		=	0;
		m_needBagNum			=	0;
	}

};

class GameUser; 

class CDailyRecharge
{
	USE_SINGLETON_NEW(CDailyRecharge);
public:
	//获得单例
	static CDailyRecharge & getSingleton() 
	{
		return THE_SINGLETON::Instance();
	}
	//构造函数
	CDailyRecharge();
	//析构函数
	~CDailyRecharge();
	//加载配置
	bool loadCfg();
	//活动是否开启                          
	stActivity * getRewardInfo();
	//领取奖励
	void receiveReward(GameUser * pUser); 
private:
	std::map<UINT32, stActivity> m_activities;
};

#endif
