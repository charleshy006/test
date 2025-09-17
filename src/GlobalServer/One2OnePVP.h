#ifndef __H_ONE2ONEPVP_H__
#define __H_ONE2ONEPVP_H__
#include "Singleton.h"
#include "TypesDef.h"
#include "SortList.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "SortList.pb.h"
#include "HistoryRankMgr.h"

#define ONE2ONE_PVP_REWARD_INFO_LEN    40000

#pragma pack(1)
struct stOne2OnePVPDB
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_score;						//分数
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID

	stOne2OnePVPDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_score = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};

struct stOne2OnePVPLastWeekDB
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_score;						//分数
	UINT16 m_rank;						
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;                   //头像ID

	stOne2OnePVPLastWeekDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_score = 0;
		m_rank = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};

struct stOne2OnePVPHistorySortListDB
{
	UINT32 m_id;
	UINT8  m_rankType;							//排名类型
	UINT32 m_rewardTimestamp;                   //奖励的时间戳
	char   m_data[ONE2ONE_PVP_REWARD_INFO_LEN];   
	
	stOne2OnePVPHistorySortListDB()
	{
		m_id = 0;
		m_rankType = 0;
		m_rewardTimestamp = 0;
		bzero(m_data,sizeof(m_data));
	}
};

struct stOne2OnePVPHasGotRewardCharIDs
{
	//UINT32 m_id;
	UINT32 m_rewardTimestamp;                   	//奖励的时间戳
	char   m_data[ONE2ONE_PVP_REWARD_INFO_LEN];   

	stOne2OnePVPHasGotRewardCharIDs()
	{   
		//m_id = 0;
		m_rewardTimestamp = 0;
		bzero(m_data,sizeof(m_data));
	}   
};

#pragma pack()

struct stOne2OnePVPInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_score;						//分数
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID

	stOne2OnePVPInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_score = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}

	bool operator < (const stOne2OnePVPInfo & one) const 
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

	void fillDBData(stOne2OnePVPDB & data)
	{
		data.m_charID = m_charID;
		data.m_score = m_score;
		strncpy(data.m_name,m_name,sizeof(m_name));
		data.m_job = m_job;
		data.m_sex = m_sex;
		data.m_photoID = m_photoID;
	}
};

struct stOne2OnePVPLastWeekInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_score;						//分数
	UINT32 m_rank;						
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;                   //头像ID 

	stOne2OnePVPLastWeekInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_score = 0;
		m_rank = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}

	void initFromDB(const stOne2OnePVPLastWeekDB & dbData)
	{
		m_charID = dbData.m_charID;
		strncpy(m_name,dbData.m_name,sizeof(m_name));
		m_score = dbData.m_score;
		m_rank = dbData.m_rank;
		m_job = dbData.m_job;
		m_sex = dbData.m_sex;
		m_photoID = dbData.m_photoID;
	}

	void fillDBData(stOne2OnePVPLastWeekDB & dbData)
	{
		dbData.m_charID = m_charID;
		strncpy(dbData.m_name,m_name,sizeof(dbData.m_name));
		dbData.m_score = m_score;
		dbData.m_rank = m_rank;
		dbData.m_job = m_job;
		dbData.m_sex = m_sex;
		dbData.m_photoID = m_photoID;
	}
};

enum eOne2OnePVPRankType
{
	eOne2OnePVP_Day_Rank = 1,	//日排名
	eOne2OnePVP_Week_Rank = 2,	//周排名
};

struct stOne2OnePVPHistorySortList
{
	UINT32 m_id;
	UINT8  m_rankType;							//排名类型
	UINT32 m_rewardTimestamp;                   //奖励的时间戳
	std::map<UINT32,UINT32> m_charID2Rank;		//<charID,Rank>

	stOne2OnePVPHistorySortList()
	{
		m_id = 0;
		m_rankType = 0;
		m_rewardTimestamp = 0;
	}

	void initFromDB(const stOne2OnePVPHistorySortListDB & data);
	void fillDBData(stOne2OnePVPHistorySortListDB & data);
};

typedef CSortList<stOne2OnePVPInfo,40000>  COne2OnePVPSortList;
class Player;
class COne2OnePVPMgr
{
	USE_SINGLETON_NEW(COne2OnePVPMgr);
public :
	static const UINT8 s_SEND_SORTITEM_MAX = 10;
	static const UINT32 s_MAX_REWARD_NUM = 10000;
	static const UINT16 s_MAX_REWARD_DAY = 7;
	static const UINT32 s_CHECK_MATCH_INTERVAL = 2;
	static const UINT16 s_MIN_WAIT_SEX = 3;

	//匹配规则相关
	struct stMatchCfg
	{
		UINT16 m_id;				//ID
		UINT32 m_startPoint;		//积分上线
		UINT32 m_endPoint;			//积分下线
		UINT16 m_matchType;			//匹配类型
		UINT32 m_matchTimeState1;	//匹配时间1
		UINT32 m_matchTimeState2;	//匹配时间2
		UINT16 m_matchRangeID;		//匹配ID

		std::map<UINT32,UINT32> m_waitCharIDs;

		stMatchCfg()
		{
			m_id = 0;
			m_startPoint = 0;
			m_endPoint = 0;
			m_matchType = 0;
			m_matchTimeState1 = 0;
			m_matchTimeState2 = 0;
			m_matchRangeID = 0;
		}
	};
	
	enum {
		eMatch_Type1 = 1,
		eMatch_Type2 = 2,
		eMatch_Type3 = 3,
		eMatch_Type4 = 4,
	};

	//匹配的机器人组
	struct stMatchRobotGroup
	{
		UINT16 m_minLv;
		UINT16 m_maxLv;
		UINT16 m_groupID;

		stMatchRobotGroup()
		{
			m_minLv = 0;
			m_maxLv = 0;
			m_groupID = 0;
		}
	};

	//每日奖励
	struct stEverydayReward
	{
		UINT16 m_beginRank;			//开始名次
		UINT16 m_endRank;			//结束名次
		UINT32 m_itemID;			//奖励ID
		UINT32 m_num;				//数量

		stEverydayReward()
		{
			m_beginRank = 0;
			m_endRank = 0;
			m_itemID = 0;
			m_num = 0;
		}
	};

	//每周奖励
	struct stWeekReward
	{
		UINT16 m_beginRank;			//开始名次
		UINT16 m_endRank;			//结束名次
		UINT32 m_itemID1;			//奖励ID
		UINT32 m_num1;				//数量
		UINT32 m_itemID2;
		UINT32 m_num2;
		UINT32 m_itemID3;
		UINT32 m_num3;

		stWeekReward()
		{
			m_beginRank = 0;
			m_endRank = 0;
			m_itemID1 = 0;
			m_num1 = 0;
			m_itemID2 = 0;
			m_num2 = 0;
			m_itemID3 = 0;
			m_num3 = 0;
		}
	};
private :
	COne2OnePVPMgr() 
	{
		m_lastCheckTime = 0;
	}
public :
	//析构函数
	~COne2OnePVPMgr() {}
	//获得排名管理实例
	static COne2OnePVPMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载数据
	void load();	
	//加载配置
	bool loadCfg();
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
	//删除所有记录
	void removeAllRecord();
	//删除所有记录回调
	void removeAllRecordeCB(bool opResult,UINT64 autoID);
	//增加奖励记录
	void addRewardRecord(stOne2OnePVPHistorySortListDB & data);
	//增加奖励记录回调
	void addRewardRecordCB(bool opResult,UINT64 autoID);
	//删除奖励记录
	void removeRewardRecord(UINT32 timestamp);
	//删除奖励记录回调
	void removeRewardRecordCB(bool opResult,UINT64 autoID);
	//删除领取奖励的玩家
	void removeGotRewardCharIDRecordCB(bool opResult,UINT64 autoID);
	//定时回调
	void timer(UINT32 cur);
	//发送排名列表
	void sendScoreSortList(Player * pUser,UINT16 beginRank,SortProto::eOne2OneSortListType type);
	//获得我的排名
	UINT16 getMyRank(UINT32 charID)
	{
		return m_scoreSortList.getMySortRank(charID);
	}
	//增加等待匹配的玩家
	void addMatchUser(Player * pUser);
	//玩家取消匹配
	void cancelMatch(Player * pUser);
	//更新积分
	void updateScore(Player * pUser,UINT32 score);
	//获得排名前3的人
	void getTop3(stHistoryRankInfo & info);
	//刷新昨日数据
	void refreshYestodayData(UINT32 cur);
	//刷新上周数据
	void refreshLastWeekData(UINT32 cur);
	//获得每日奖励
	stEverydayReward * getEverydayRewardByRank(UINT32 rank);
	//获得每周奖励
	stWeekReward * getWeekRewardByRank(UINT32 rank);
	//玩家上线
	void userOnline(Player * pUser);
	//保存领取奖励的玩家
	void saveGotRewardCharID(UINT32 timestamp,UINT32 charID);
	//保存领取奖励玩家ID记录
	void saveGotRewardCharIDRecordCB(bool opResult,UINT64 autoID);
	//更新领取奖励玩家ID记录
	void updateGotRewardCharIDecordCB(bool opResult,UINT64 autoID);
	//获得积分
	UINT32 getScore(UINT32 charID);
	//检查匹配
	void checkMatch();
	//新的匹配检查
	void checkMatchNew();
	//获得匹配机器人组
	UINT16 getRobotGroupID(UINT16 matchType,UINT16 lv);
	//重置每个阶段玩家
	void checkMatchReset();
	//同步所有记录
	void synAllsRecord(Player *pUser);
private :
	COne2OnePVPSortList m_scoreSortList;
	std::vector<stOne2OnePVPLastWeekInfo> m_lastweekTopN;					//上周前几排行
	//std::map<UINT32,UINT16> m_waitMatchUsers;								//等待匹配的玩家
	std::map<UINT16,std::vector<stMatchRobotGroup> > m_matchRobotInfos;		//匹配机器人信息

	std::map<UINT16,stMatchCfg> m_waitMatchUsers;								//匹配规则
	std::vector<stEverydayReward> m_everydayRewards;						//每天奖励
	std::vector<stWeekReward>  m_weekRewards;								//每周奖励
	std::map<UINT32,stOne2OnePVPHistorySortList> m_historyRankRewards;		//历史排名奖励 <timestamp,stOne2OnePVPHistorySortList>
	std::map<UINT32,std::set<UINT32> >			 m_gotRewardCharIDs;		//已经领取奖励的用户
	UINT32  m_lastCheckTime;												//上次检查时间

	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >		m_waitState1Users;			//第一阶段的玩家<matchType,vector<pair<charID,matchID>> >
	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >         m_waitState2Users;			//第二阶段的玩家
	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >       m_waitState3Users;			//第三阶段的玩家
};
#endif
