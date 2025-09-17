#ifndef __ARENA_CONFIG_H__
#define __ARENA_CONFIG_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>

class ArenaConfig
{
	USE_SINGLETON_NEW(ArenaConfig);
public :
	//竞技场每天奖励
	struct stArenaEverydayReward
	{
		UINT32 m_id;				//ID
		UINT32 m_startRank;			//起始排名
		UINT32 m_endRank;			//结束排名
		UINT32 m_itemID1;			//奖励道具1
		UINT32 m_num1;				//奖励道具1的数量
		UINT32 m_itemID2;			//道具奖励2
		UINT32 m_num2;				//奖励道具2的数量

		stArenaEverydayReward()
		{
			m_id = 0;
			m_startRank = 0;
			m_endRank = 0;
			m_itemID1 = 0;
			m_num1 = 0;
			m_itemID2 = 0;
			m_num2 = 0;
		}
	};

	//擂台轮数奖励奖励
	struct stLeitaiRoundReward
	{
		UINT16 m_id;
		UINT32 m_itemID1;
		UINT32 m_num1;
		UINT32 m_rate1;
		UINT32 m_itemID2;
		UINT32 m_num2;
		UINT32 m_rate2;
		UINT32 m_itemID3;
		UINT32 m_num3;
		UINT32 m_rate3;
		UINT32 m_itemID4;
		UINT32 m_num4;
		UINT32 m_rate4;

		stLeitaiRoundReward()
		{
			m_id = 0;
			m_itemID1 = 0;
			m_num1 = 0;
			m_rate1 = 0;
			m_itemID2 = 0;
			m_num2 = 0;
			m_rate2 = 0;
			m_itemID3 = 0;
			m_num3 = 0;
			m_rate3 = 0;
			m_itemID4 = 0;
			m_num4 = 0;
			m_rate4 = 0;
		}
	};
	
	//擂台轮数奖励
	struct stLeitaiReward
	{
		UINT32 m_dropID;
		std::map<UINT16,stLeitaiRoundReward> m_roundReward;

		stLeitaiReward()
		{
			m_dropID = 0;
		}
	};

	//擂台匹配规则
	struct stLeitaiMatch
	{
		UINT16 m_id;				//id
		UINT32 m_startRank;			//开始名次
		UINT32 m_endRank;			//结束名次
		UINT32 m_range;				//范围
		UINT32 m_randomMin;			//最小随机
		UINT32 m_randomMax;			//最大随机

		stLeitaiMatch()
		{
			m_id = 0;
			m_startRank = 0;
			m_endRank = 0;
			m_range = 0;
			m_randomMin = 0;
			m_randomMax = 0;
		}
	};
	
	//擂台切换地图
	struct stLeitaiSwitchMap
	{
		UINT32 m_id;
		std::map<UINT32,UINT32> m_level2Rate;

		stLeitaiSwitchMap()
		{
			m_id = 0;
		}
	};

	//实时战场奖励
	struct stRealTimeReward
	{
		UINT32 m_itemID ;
		UINT32 m_itemNum;

		stRealTimeReward()
		{
			m_itemID = 0;
			m_itemNum = 0;
		}
	};

	//巅峰测试服
	struct stOne2OnePVPReward
	{
		UINT32 m_rewardNum;					//奖励数量
		UINT32 m_doubleRewardNum;			//双倍奖励数量

		stOne2OnePVPReward()
		{
			m_rewardNum = 0;
			m_doubleRewardNum = 0;
		}
	};

	//擂台buff信息
	struct stLeitaiBuffInfo
	{
		UINT16 m_buffID;			//buffID
		UINT8  m_type;				//buff类型
		UINT32 m_rate;				//概率

		stLeitaiBuffInfo()
		{
			m_buffID = 0;
			m_type = 0;
			m_rate = 0;
		}
	};

	//擂台buff刷新信息
	struct stLeitaiBuffRefreshInfo
	{
		std::vector<stLeitaiBuffInfo> m_diamondRefreshBuffVecType1; //人物的buff		
		std::vector<stLeitaiBuffInfo> m_diamondRefreshBuffVecType2; //宠物的buff
	
		std::vector<stLeitaiBuffInfo> m_autoRefreshBuffVecType1;	//人物的buff
		std::vector<stLeitaiBuffInfo> m_autoRefreshBuffVecType2;	//宠物的buff

		stLeitaiBuffRefreshInfo()
		{
		
		}
	};
private :
	ArenaConfig()
	{
		m_mobaBuffTotalRate = 0;
		m_mobaCollectTime = 0;
		m_mobaReliveTime = 0;
	}
public :
	//析构函数
	~ArenaConfig(){}
	//获得实例
	static ArenaConfig & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
	//历史排名奖励
	UINT32 getHistoryRankReward(UINT16 oldRank,UINT16 newRank);
	//解析奖励
	void parseReward(TiXmlElement *pNode,stLeitaiReward & reward);
	//解析匹配
	void parseMatch(TiXmlElement *pNode,std::map<UINT16,stLeitaiMatch> & matchCnt);
	//获得每日奖励配置
	stArenaEverydayReward * getArenaEverydayReward(UINT16 rank);
	//实时战场奖励
	stRealTimeReward * getRealTimeReward(bool win);
	//1vs1奖励配置
	stOne2OnePVPReward  * getOne2OnePVPReward(bool win);
	//获得简单模式下的关卡
	stLeitaiSwitchMap * getEasySwitchMap()
	{
		return &m_easyLeitaiSwitchMap;
	}
	//获得复杂模式下的关卡
	stLeitaiSwitchMap * getDiffSwitchMap()
	{
		return &m_difficultLeitaiSwitchMap;
	}
	//获得匹配信息
	stLeitaiMatch * getMatchInfo(UINT8 mode,UINT16 rank);
	//获得百分比
	UINT16 getPercentCoef(UINT8 mode,UINT16 round);
	//获得简单擂台奖励
	stLeitaiReward * getEasyLeitaiReward()
	{
		return &m_easyLeitaiReward;
	}
	//获得复杂擂台奖励
	stLeitaiReward * getDifficultLeitaiReward()
	{
		return &m_difficultLeitaiReward;
	}
	//随机一个moba战场的buffID
	UINT32 getMobaBuffID();
	//获得moba采集时间
	UINT32 getMobaCollectTime()
	{
		return m_mobaCollectTime;
	}
	//获得moba复活时间
	UINT32 getMobaReliveTime()
	{
		return m_mobaReliveTime;
	}
	//1v1是否开启
	bool isOne2OneOpen(UINT32 sec );
	//1v1双倍是否开启
	bool isOne2OneDoubleOpen(UINT32 sec);
	//实时是否开启
	bool isRealTimeOpen(UINT32 sec);
	//moba是否开启
	bool isMobaOpen(UINT32 sec);
	//随机擂台buffID
	UINT16 randomLeitaiBuffID(bool bEasy,bool bAuto,UINT8 type);
private :
	std::map<UINT16,UINT32> m_rank2TotalReward;
	std::map<UINT16,stArenaEverydayReward> m_everydayReward;
	stLeitaiReward			m_easyLeitaiReward;					//简单擂台奖励
	stLeitaiReward			m_difficultLeitaiReward;			//复杂擂台奖励
	std::map<UINT16,stLeitaiMatch> m_easyLeitaiMatch;			//简单擂台匹配
	std::map<UINT16,stLeitaiMatch>	m_difficultLeitaiMatch;		//复杂擂台匹配
	stRealTimeReward        m_realTimeWinReward;				//实时战场胜者奖励
	stRealTimeReward		m_realTimeLoseReward;				//实时战场失败者奖励
	stOne2OnePVPReward		m_one2OneWinReward;					//一对一胜利奖励
	stOne2OnePVPReward      m_one2OneLoseReward;				//一对一失败奖励
	std::map<UINT16,UINT16> m_easyLeitaiPercent;				//简单模式下百分比
	std::map<UINT16,UINT16> m_difficultLeitaiPercent;			//困难模式下百分比
	stLeitaiSwitchMap		m_easyLeitaiSwitchMap;				//简单模式切换地图
	stLeitaiSwitchMap		m_difficultLeitaiSwitchMap;			//困难模式切换地图
	std::map<UINT32,UINT32> m_mobaBuffRates;					//moba的buff刷新概率
	UINT32					m_mobaBuffTotalRate;				//moba的buff总的概率
	UINT32					m_mobaCollectTime;					//moba采集时间
	UINT32					m_mobaReliveTime;					//moba复活时间

	std::vector<std::pair<UINT32,UINT32> > one2OneOpenTimes;		//1v1开启时间
	std::vector<std::pair<UINT32,UINT32> > one2OneDoubleOpenTimes;	//1v1双倍开启时间

	std::vector<std::pair<UINT32,UINT32> > m_realTimeOpenTimes;		//实时开启时间
	std::vector<std::pair<UINT32,UINT32> > m_mobaOpenTimes;			//moba开启时间

	stLeitaiBuffRefreshInfo  m_leitaiBuffEasyMode;					//简单模式的buff
	stLeitaiBuffRefreshInfo  m_leitaiBuffDiffMode;					//困难模式的buff
};
#endif
