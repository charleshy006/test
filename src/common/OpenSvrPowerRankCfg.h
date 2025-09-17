#ifndef __H_OPEN_SVR_POWER_CFG_H__
#define __H_OPEN_SVR_POWER_CFG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <vector>

class OpenSvrPowerRankCfg
{
	USE_SINGLETON_NEW(OpenSvrPowerRankCfg);
public :
	struct stReward
	{
		UINT16 m_minRank;
		UINT16 m_maxRank;
		UINT32 m_boxID;

		stReward()
		{
			m_minRank = 0;
			m_maxRank = 0;
			m_boxID = 0;
		}
	};
private :
	//构造函数
	OpenSvrPowerRankCfg();
public :
	//析构函数
	~OpenSvrPowerRankCfg();
	//获得实例
	static OpenSvrPowerRankCfg & getSingleton()
	{   
		return THE_SINGLETON::Instance();
	} 
	//加载所有配置
	bool loadCfg();
	//开服战力排名奖励
	stReward * getOpenSvrRankReward(UINT16 rank);
	stReward * getOpenSvrRankReward2(UINT16 rank);
private :
	std::vector<stReward> m_rewardVec;
	std::vector<stReward> m_rewardVec2;
};
#endif
