#ifndef __EMERALD_DREAM_CFG_H__
#define __EMERALD_DREAM_CFG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

struct stRankAward
{
	UINT32 m_id;            // id
	UINT32 m_startRank;     //开始排名等级
	UINT32 m_endRank;       //结束排名等级
	UINT32 m_itemID;        //奖励发放物品ID
	UINT32 m_boxID;         //Box表引用ID

	stRankAward()
	{
		m_id = 0;
		m_startRank = 0;
		m_endRank = 0;
		m_itemID = 0;
		m_boxID = 0;
	}
};

struct stReset                         
{
	UINT8  m_num;       //重置次数
	UINT32 m_diamondNum;//钻石消耗数量

	stReset()
	{
		m_num = 0;
		m_diamondNum = 0;              
	}   
};

struct stChangeId                   
{
	UINT32 m_id;        //id        
	UINT32 m_levelId;  //副本ID
	UINT32 m_openTime; //持续天数	

	stChangeId()
	{
		m_id = 0;
		m_levelId = 0;
		m_openTime = 0;            
	}
};                                  

class CEmeraldDreamCfg
{
	USE_SINGLETON_NEW(CEmeraldDreamCfg);
private :
	CEmeraldDreamCfg();
public:
	//构造函数 
	static CEmeraldDreamCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	
	//加载配置 
	bool loadCfg();
	//获取排名奖信息
	stRankAward * getRankAward(UINT32 rank);
	//获取重置消耗砖石信息
	UINT32 getReset(UINT32 num);
	//获取副本开启时间信息
	stChangeId * getChangeId(UINT8 id);
	//获取当日次数
	UINT8 getDailyNum(){return m_dailyNum;}
private:
	std::map<UINT32, stRankAward> m_rankAwardMap;
	std::map<UINT8, UINT32> m_resetMap;
	std::map<UINT8, stChangeId> m_changeIdMap;
	UINT16 m_dailyNum;
};
#endif
