#ifndef __H_RECHARGE_ACTIVITY_CFG_H__
#define __H_RECHARGE_ACTIVITY_CFG_H__
#include <string>
#include <map>
#include "tinyxml/tinyxml.h"
#include "Singleton.h"

class RechargeActivityCfg
{
	USE_SINGLETON_NEW(RechargeActivityCfg);
public:
	struct stActivityStepInfo
	{
		UINT8   m_id;
		UINT32  m_rechargeNum;
		UINT32  m_boxID;

		stActivityStepInfo()
		{
			m_id = 0;
			m_rechargeNum = 0;
			m_boxID = 0;
		}
	};

	struct stActivityInfo
	{
		UINT32 m_id;							//ID
		UINT32 m_beginTime;						//开始时间
		UINT32 m_endTime;						//结束时间
		UINT32 m_interval;						//间隔
		std::map<UINT8,stActivityStepInfo> m_steps;//阶段

		stActivityInfo()
		{
			m_id = 0;
			m_beginTime = 0;
			m_endTime = 0;
			m_interval = 0;
		}
	};
private:
	//构造函数
	RechargeActivityCfg();
public :
	//获得实例
	static RechargeActivityCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~RechargeActivityCfg();
	//加载配置
	bool loadCfg();
	//获得所有开启的活动
	std::map<UINT32,stActivityInfo> & getAllActivities()
	{
		return m_activities;
	}
	//活动某个活动的配置
	stActivityInfo * getActivity(UINT16 activityID);	
private :
	std::map<UINT32,stActivityInfo> m_activities;		//所有活动配置
};
#endif
