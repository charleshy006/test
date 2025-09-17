#ifndef __H_DRAW_SCOREMGR_H__
#define __H_DRAW_SCOREMGR_H__
#include "Singleton.h"
#include "TypesDef.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "SortList.h"
#include <ext/hash_map>
#include <map>
#include <set>

#pragma pack(1)
struct stDrawHeroScoreDB
{
	UINT32 m_charID;					//角色ID
	UINT32 m_score;						//分数
	char   m_rewards[50];				//已经领取的ID

	stDrawHeroScoreDB()
	{
		m_charID = 0;
		m_score = 0;
		bzero(m_rewards,sizeof(m_rewards));
	}
};
#pragma pack()

struct stDrawHeroScoreInfo
{
	UINT32 m_charID ;
	UINT32 m_score;
	std::set<UINT16> m_rewardIDs;
	bool	m_gotRankReward;

	stDrawHeroScoreInfo()
	{
		m_charID = 0;
		m_score = 0;
		m_gotRankReward = false;
	}

	void fillDBData(stDrawHeroScoreDB & data);
	void initFromData(stDrawHeroScoreDB & data);
};

#pragma pack(1)
struct stDrawHeroRankDB
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_score;						//等级
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别

	stDrawHeroRankDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_score = 0;
		m_job = 0;
		m_sex = 0;
	}
};
#pragma pack()

struct stDrawHeroRankInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_score;						//等级
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别

	bool operator < (const stDrawHeroRankInfo & one) const 
	{
		return m_score > one.m_score; 
	}

	UINT32 getKey() const 
	{
		return m_charID;
	}

	UINT32 getCmpValue() const 
	{
		return m_score;
	}

	void fillDBData(stDrawHeroRankDB & data)
	{
		data.m_charID = m_charID;
		strncpy(data.m_name,m_name,sizeof(m_name));
		data.m_score = m_score;
		data.m_job = m_job;
		data.m_sex = m_sex;
	}
};

#pragma pack(1)
struct stDrawHeroActivityDB
{
	UINT32 m_id ;
	UINT32 m_activityID;
	UINT8  m_isStart;

	stDrawHeroActivityDB()
	{
		m_id = 0;
		m_activityID = 0;
		m_isStart = 0;
	}
};
#pragma pack(0)

struct stDrawHeroActivityInfo
{
	UINT32 m_id ;
	UINT32 m_activityID;
	UINT8  m_isStart;

	stDrawHeroActivityInfo()
	{
		m_id = 0;
		m_activityID = 0;
		m_isStart = 0;
	}

	void initFromDB(stDrawHeroActivityDB & data);
	void fillDBData(stDrawHeroActivityDB & data);
};


typedef CSortList<stDrawHeroRankInfo> CDrawHeroSortList;

class Player;
class CDrawHeroRankMgr
{
	USE_SINGLETON_NEW(CDrawHeroRankMgr);
public :
	static const UINT8 s_SEND_SORTITEM_MAX = 15;
private :
	//析构函数
	~CDrawHeroRankMgr() {}
public :
	//构造函数
	CDrawHeroRankMgr() {};
	//返回管理实例
	static CDrawHeroRankMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载
	void load();
	//增加一个记录
	void addRecord(const char * pData);
	//增加记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一条记录
	void updateRecord(UINT32 key,const char * pData);
	//更新一条记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除一条记录
	void removeRecord(UINT32 key);
	//删除一条记录回调
	void removeRecordCB(bool opResult,UINT64 autoID);
	//更新限时武神排名
	void updateDrawHeroScore(Player * pUser,UINT32 score);
	//删除所有限时武神的排名
	void removeAllDrawHeroRecord();
	//删除所有限时武神的排名回调
	void removeAllDrawHeroRecordCB(bool opResult,UINT64 autoID);
	//发送武神抽奖排行
	void sendDrawHeroSortList(Player *pUser,UINT32 beginRank);
	//获取我的排名
	UINT16 getMyRank(UINT32 charID)
	{
		return m_drawHeroSortList.getMySortRank(charID);
	}
	//保存活动信息
	void updateActivityInfo();
	//保存活动回调
	void updateActivityInfoCB(bool opResult,UINT64 autoID);
	//定时器
	void timer(UINT32 cur);
	//活动开始
	void activityStart();
	//活动结束
	void activityEnd();
	//获得活动ID
	UINT32 getActivityID()
	{
		return m_activityInfo.m_activityID;
	}
	//同步记录
	void synRecord(Player * pUser);
private :
	CDrawHeroSortList      m_drawHeroSortList;
	stDrawHeroActivityInfo	m_activityInfo;
};

class CDrawHeroScoreMgr
{
	USE_SINGLETON_NEW(CDrawHeroScoreMgr);
private :
	CDrawHeroScoreMgr() {}
public :
	//析构函数
	~CDrawHeroScoreMgr() {}
	//获得排名管理实例
	static CDrawHeroScoreMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载数据
	void load();	
	//增加一个记录
	void addRecord(const stDrawHeroScoreDB & );
	//增加记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一条记录
	void updateRecord(const stDrawHeroScoreDB &);
	//更新一条记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除一条记录
	void removeRecord(UINT32 charID);
	//删除一条记录回调
	void removeRecordCB(bool opResult,UINT64 autoID);
	//根据玩家获得他的积分
	UINT32 getScoreByID(UINT32 charID);
	//玩家获得积分
	void userIncScore(UINT32 charID,UINT32 score);
	//玩家积分奖励
	void scoreReward(Player * pUser,UINT16 scoreID);
	//排名奖励
	void rankReward(Player * pUser);
	//获得玩家的抽奖信息
	stDrawHeroScoreInfo * getDrawHeroScoreInfo(UINT32 charID);
	//清空所有玩家的抽奖积分
	void clearAllDrawHeroScore();
	//清空所有抽奖积分回调
	void clearAllDrawHeroScoreCB(bool opResult,UINT64 autoID);
private :
	typedef __gnu_cxx::hash_map<UINT32,stDrawHeroScoreInfo>::iterator CharID2ScoreMap_IT;
	__gnu_cxx::hash_map<UINT32,stDrawHeroScoreInfo> m_charID2ScoreMap;
};
#endif
