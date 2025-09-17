#ifndef __EMERALD_DREAM_MGR_H__
#define __EMERALD_DREAM_MGR_H__

#include "Singleton.h"
#include "TypesDef.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "SortList.h"
#include <ext/hash_map>
#include <map>
#include <set>


#pragma pack(1)
struct stEmeraldDreamRankDB
{
	UINT32 m_charID;                    //角色ID
	char   m_name[MAX_NAMESIZE + 1];    //角色名字
	UINT32 m_damage;                    //伤害
	UINT8  m_job;                       //职业
	UINT8  m_sex;                       //性别
	UINT16 m_level;                     //等级
	UINT16 m_photoID;					//头像ID

	stEmeraldDreamRankDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_damage = 0;
		m_job = 0;
		m_sex = 0;
		m_level = 0;
		m_photoID = 0;
	}
};
#pragma pack()

struct stEmeraldDreamRankInfo
{
	UINT32 m_charID;                    //角色ID
	char   m_name[MAX_NAMESIZE + 1];    //角色名字
	UINT32 m_damage;                    //伤害
	UINT8  m_job;                       //职业
	UINT8  m_sex;                       //性别
	UINT16 m_level;                     //等级
	UINT16 m_photoID;                   //头像ID

	bool operator < (const stEmeraldDreamRankInfo & one) const
	{
		return m_damage > one.m_damage;
	}

	UINT32 getKey() const
	{
		    return m_charID;
	}

	UINT32 getCmpValue() const
	{
		return m_damage;
	}

	void fillDBData(stEmeraldDreamRankDB & data)
	{
		data.m_charID = m_charID;
		strncpy(data.m_name,m_name,sizeof(m_name));
		data.m_sex = m_sex;
		data.m_job = m_job;
		data.m_level = m_level;
		data.m_damage = m_damage;
		data.m_photoID = m_photoID;
	}
};


#pragma pack(1)
struct stEmeraldDreamOpenTimeDB
{
	UINT32 m_id;						//id
	UINT32 m_activityID;				//活动ID
	UINT32 m_beginTime;					//开始时间
	UINT32 m_endTime;					//结束时间

	stEmeraldDreamOpenTimeDB()
	{
		m_id = 0;
		m_activityID = 0;
		m_beginTime	 = 0;
		m_endTime	 = 0;
	}
};
#pragma pack()

struct stEmeraldDreamOpenTimeInfo
{
	UINT32 m_id;						//id
	UINT32 m_activityID;                //活动ID
	UINT32 m_beginTime;                 //开始时间
	UINT32 m_endTime;                   //结束时间

	stEmeraldDreamOpenTimeInfo()
	{
		m_id = 0;
		m_activityID = 0;
		m_beginTime  = 0;
		m_endTime    = 0;
	}

	void initFromDB(stEmeraldDreamOpenTimeDB & data);
	void fillDBData(stEmeraldDreamOpenTimeDB & data);
};

struct stRankListInfo
{
	char	m_name[MAX_NAMESIZE + 1];   //角色名字
	UINT32	m_damage;					//伤害
	UINT32  m_rank;						//名次

	stRankListInfo()
	{
		bzero(m_name,sizeof(m_name));
		m_damage = 0;
		m_rank = 0;
	}
};

typedef CSortList<stEmeraldDreamRankInfo, 10000> CEmeraldDreamSortList;

class Player;
class CEmeraldDreamMgr
{
	USE_SINGLETON_NEW(CEmeraldDreamMgr);
	static const UINT32 s_SEND_SORTITEM_MAX = 100;
private :
	//析构函数
	CEmeraldDreamMgr() {}
public:
	//构造函数
	~CEmeraldDreamMgr() {}
	//返回管理实例
	static CEmeraldDreamMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	
	void load();
	//增加一个记录                                 
	void addRecord(const char * pData);
	//增加记录回调                                 
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一条记录                                   
	void updateRecord(UINT32 key,const char * pData);
	//更新一条记录回调                               
	void updateRecordCB(bool opResult,UINT64 autoID);
	//更新翡翠梦境记录
	void updateEmeraldDreamInfo(Player * pUser,UINT32 damage);
	//根据我的排名选择需要显示玩家排名
	UINT16 switchRank(UINT16 myRank);
	//根据排名获得玩家信息
	bool getPlayerInfo(stEmeraldDreamRankInfo& info, UINT16 rank);
	//定时回调
	void timer(UINT32 cur);
	//奖励发放
	void giveReward();
	//重置排名数据
	void delRankInfo();
	//重置排名数据回调
	void delRankInfoCB(bool opResult,UINT64 autoID);
	//发送排行
	void sendEmeraldDreamSortList(Player *pUser,UINT32 beginRank, UINT32 num = s_SEND_SORTITEM_MAX);
	//根据charID获取排名信息
	void getRankInfo(UINT32 charID);
	//更新一条记录                                   
	void updateTimeRecord(stEmeraldDreamOpenTimeDB &data);
	//更新一条记录回调                               
	void updateTimeRecordCB(bool opResult,UINT64 autoID);
	//获得副本信息
	stEmeraldDreamOpenTimeInfo getCopyMapInfo(){
		return m_emeraldDreamOpenTimeInfo;
	}

	//同步记录
	void synRecord(Player * pUser);
private:
	CEmeraldDreamSortList		m_emeraldDreamSortList;
	stEmeraldDreamOpenTimeInfo	m_emeraldDreamOpenTimeInfo;
	bool						m_zeroRefresh;		
};

#endif
