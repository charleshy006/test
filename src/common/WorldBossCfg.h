#ifndef __H_WORLD_BOSS_CFG_H__
#define __H_WORLD_BOSS_CFG_H__
#include <string>
#include <map>
#include "tinyxml/tinyxml.h"
#include "Singleton.h"

class CWorldBossCfg
{
	USE_SINGLETON_NEW(CWorldBossCfg);
public:
	struct stActivityOpenInfo
	{
		UINT32 m_id;
		UINT32 m_beginTime;			//开始小时,一天中的第几秒
		UINT32 m_endTime;			//结束时间
		UINT32 m_copymapID;			//副本ID
		UINT32 m_bossID;			//bossID

		stActivityOpenInfo()
		{
			m_id = 0;
			m_beginTime = 0;
			m_endTime = 0;
			m_copymapID = 0;
			m_bossID = 0;
		}
	};

	struct stRewardInfo
	{
		UINT32 m_beginRank;			//开始名次
		UINT32 m_endRank;			//结束名次
		UINT32 m_itemID;			//礼包ID

		stRewardInfo()
		{
			m_beginRank = 0;
			m_endRank = 0;
			m_itemID = 0;
		}
	};
private:
	//构造函数
	CWorldBossCfg();
public :
	//获得实例
	static CWorldBossCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CWorldBossCfg();
	//加载配置
	bool loadCfg();
	//是否在开启时间
	bool isInOpenTime(UINT32 curSec);
	//获得当前进行的活动信息
	stActivityOpenInfo * getCurActivityInfo(UINT32 curSec);
	//获得世界boss奖励
	std::vector<stRewardInfo> & getReward()
	{
		return m_rewardVec;
	}
	//获得击杀奖励
	UINT32 getKillReward()
	{
		return m_killReward;
	}
	//获得下一个开启活动时间
	stActivityOpenInfo * getNextActivityInfo(UINT32 curSec);
private :
	std::map<UINT32,stActivityOpenInfo> m_openTimes;
	std::vector<stRewardInfo>  m_rewardVec;
	UINT32 m_killReward;									//击杀奖励
};
#endif
