#ifndef __H_LITMIT_TIME_H__
#define __H_LITMIT_TIME_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

class LimitTimePetCfg
{
	USE_SINGLETON_NEW(LimitTimePetCfg);
public :
	//积分奖励
	struct stScoreReward
	{
		UINT16 m_id;			//id	
		UINT32 m_score;			//奖励积分
		UINT32 m_boxID;			//boxID

		stScoreReward()
		{
			m_id = 0;
			m_score  = 0;
			m_boxID = 0;
		}
	};
	
	//名次奖励
	struct stRankReward
	{
		UINT16 m_beginRank;		//开始名次
		UINT16 m_endRank;		//结束名次
		UINT32 m_boxID;			//boxID

		stRankReward()
		{
			m_beginRank = 0;
			m_endRank = 0;
			m_boxID = 0;
		}
	};
	//限时武神活动
	struct 	stActivity
	{
		UINT32 m_id;									//活动ID
		UINT32 m_heroID;								//武将ID
		UINT32 m_drawID;								//抽奖ID
		std::map<UINT16,stScoreReward>	m_scoreRewards;	//积分奖励
		std::vector<stRankReward> m_rankRewards;		//名次奖励

		stActivity()
		{
			m_id = 0;
			m_heroID = 0;
			m_drawID = 0;
		}
	};

	
	//活动开启信息
	struct stActivityOpenInfo
	{
		UINT32 m_id;
		UINT32 m_activityID;			//活动ID
		UINT32 m_beginTime;				//开启时间
		UINT32 m_endTime;				//结束时间

		stActivityOpenInfo()
		{
			m_id = 0;
			m_activityID = 0;
			m_beginTime = 0;
			m_endTime = 0;
		}
	};
public :
	 //构造函数
	LimitTimePetCfg();
	//析构函数
	~LimitTimePetCfg();
	//返回管理实例 
	static LimitTimePetCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置 
	bool loadCfg();
	//获得开启的活动信息
	stActivityOpenInfo * getOpenInfo(UINT32 cur);
	//获得活动heroID
	UINT32 getHeroID(UINT32 id);
	//需要的积分
	stScoreReward * getScoreRewardInfo(UINT32 activityID,UINT16 scoreID);
	//根据名次获得奖励boxID
	UINT32 getBoxIDByRank(UINT32 activityID,UINT16 rank);
	//获得活动
	stActivity * getActivity(UINT32 activityID);
	//获得最低积分
	UINT32 getMinScore()
	{
		return m_minScore;
	}
    //设置限时随从活动配置
    void setOpenInfo(std::map<UINT32, stActivityOpenInfo> & petMap);
    //获取显示随从活动配置
    void getOpenInfo(std::map<UINT32, stActivityOpenInfo> & petMap);
private :
	std::map<UINT32,stActivity> m_allActivitys;
	std::map<UINT32,stActivityOpenInfo> m_openInfos;
	UINT32 m_minScore;//排行榜的最低积分
};
#endif
