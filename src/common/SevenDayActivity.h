#ifndef __H_SEVEN_DAY_ACTIVITY_H__
#define __H_SEVEN_DAY_ACTIVITY_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

struct stSDGoalInfo
{
	UINT16 m_id;
	UINT16 m_cond;						//条件
	UINT32 m_value;						//值
	UINT32 m_extraVal;					//额外的值
	UINT32 m_boxID;						//奖励boxID

	stSDGoalInfo()
	{
		m_id = 0;
		m_cond = 0;
		m_value = 0;
		m_extraVal = 0;
		m_boxID = 0;
	}
};

struct stSDGoalTypeInfo
{
	UINT8 m_goalType;						//目标类型
	std::map<UINT16,stSDGoalInfo> m_goals;	//任务目标

	stSDGoalTypeInfo()
	{
		m_goalType = 0;
	}
};

//七天活动内容
struct stSDActivityInfo
{
	UINT8 m_day;										//天数
	std::map<UINT8,stSDGoalTypeInfo> m_goalTypes;		//目标类型

	stSDActivityInfo()
	{
		m_day = 0;	
	}
};

class SevenDayActivityCfg
{
	USE_SINGLETON_NEW(SevenDayActivityCfg);
public :
public :
	//构造函数 
	static SevenDayActivityCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置 
	bool loadCfg();
	//获得某天配置
	stSDActivityInfo * getDayInfo(UINT8 day);
	//获取某个目标配置
	stSDGoalInfo * getGoalInfo(UINT8 day,UINT8 goalType,UINT16 id);
	//获得某个类型的目标配置
	stSDGoalInfo * getGoalInfoByGoalID(UINT8 day,UINT8 goalType,UINT16 goalID);
	//获得某个类型的目标和目标值为特定值的目标配置
	stSDGoalInfo * getGoalInfoByGoalIDAndExtraVal(UINT8 day,UINT8 goalType,UINT16 goalID,UINT16 extraVal);
private :
	std::map<UINT8,stSDActivityInfo> m_7Days;		//7天奖励
};
#endif
