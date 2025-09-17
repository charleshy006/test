#ifndef __H_WELFARECENTER_CFG_H__
#define __H_WELFARECENTER_CFG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <map>
#include <vector>

class WelfareCenterCfg
{
	USE_SINGLETON_NEW(WelfareCenterCfg);
public :
	struct stRewardItem
	{
		UINT32 m_id;
		UINT32 m_num;

		stRewardItem()
		{
			m_id = 0;
			m_num = 0;
		}
	};		

	struct stCondition
	{
		UINT32 m_taskID;
		UINT32 m_awardID;
		UINT16 m_level;
		std::vector<stRewardItem> m_rewardVec;

		stCondition()
		{
			m_taskID = 0;
			m_awardID = 0;
			m_level = 0;
		}
	};
private :
	//构造函数
	WelfareCenterCfg();
public :
	//析构函数
	~WelfareCenterCfg();
	//获得实例
	static WelfareCenterCfg & getSingleton()
	{   
		return THE_SINGLETON::Instance();
	} 
	//加载所有配置
	bool loadCfg();
	//获得
	stCondition * getCondition(UINT32 taskID,UINT32 awardID);
private :
	std::vector<stCondition> m_conditions;	
};
#endif
