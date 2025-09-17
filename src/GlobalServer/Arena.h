#ifndef __ARENA_H__
#define __ARENA_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "RoleInfo.h"
#include "HistoryRankMgr.h"
#include <list>
#include <set>
#include <ext/hash_map>

#define ARENA_REWARD_INFO_LEN    40000

#pragma pack(1)
struct stArenaSortInfoDB
{
	UINT32 m_charID ;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//名字
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT32 m_rank;						//名次
	UINT16 m_photoID;					//头像ID

	stArenaSortInfoDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_job = 0;
		m_sex = 0;
		m_rank = 0;
		m_photoID = 0;
	}
};

struct stYestodayArenaSortListDB
{
	UINT32 m_id;
	UINT32 m_rewardTimestamp;					//奖励的时间戳
	char   m_data[ARENA_REWARD_INFO_LEN];	

	stYestodayArenaSortListDB()
	{
		m_id = 0;
		m_rewardTimestamp = 0;
		bzero(m_data,sizeof(m_data));
	}
};

struct stHasGotRewardCharIDs
{
	//UINT32 m_id;
	UINT32 m_rewardTimestamp;					//奖励的时间戳
	char   m_data[ARENA_REWARD_INFO_LEN];	

	stHasGotRewardCharIDs()
	{
		//m_id = 0;
		m_rewardTimestamp = 0;
		bzero(m_data,sizeof(m_data));
	}
};

//用户的竞技场挑战休息
struct stUserArenaInfoDB
{
	UINT32 m_charID;				//角色ID
	UINT32 m_challengeNum;			//挑战次数	
	UINT32 m_lastPKTime;			//最近一次pk时间
	UINT32 m_resetNum;				//重置次数

	stUserArenaInfoDB()
	{
		m_charID = 0;
		m_challengeNum = 0;
		m_lastPKTime = 0;
		m_resetNum = 0;
	}
};
#pragma pack()

struct stArenaSortInfo
{
	UINT32 m_charID ;					//角色ID
	char m_name[MAX_NAMESIZE + 1];		//名字
	UINT8   m_job;						//职业	
	UINT8   m_sex;						//性别
	UINT32  m_rank;						//名次
	UINT32	m_robotID;					//是否是机器人
	UINT16  m_photoID;					//头像ID

	void fillDBData(stArenaSortInfoDB & dbData);
	void initFromDB(stArenaSortInfoDB & dbData);

	stArenaSortInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_job = 0;
		m_sex = 0;
		m_rank = 0;
		m_robotID = 0;
		m_photoID = 0;
	}
};

struct stZoneArenaSortList
{
	std::map<UINT32,stArenaSortInfo>  m_charID2SortInfo;			//角色与排序信息对应信息
	std::map<UINT32,UINT32>   m_rank2CharID;						//名次与角色ID对应
	UINT32					  m_theLowestRank;						//最低排名

	stZoneArenaSortList()
	{
		m_theLowestRank = 0;
	}
};

struct stUserArenaInfo
{
	UINT32 m_charID;				//角色ID
	UINT32 m_challengeNum;			//挑战次数	
	UINT32 m_lastPKTime;			//最近一次pk时间
	UINT32 m_resetNum;				//重置次数

	stUserArenaInfo()
	{
		m_charID = 0;
		m_challengeNum = 0;
		m_lastPKTime = 0;
		m_resetNum = 0;
	}
	
	//从数据库初始化
	void initFromDB(const stUserArenaInfoDB & data);
	//填充数据库信息
	void fillDBData(stUserArenaInfoDB & data);
};

class Player;
class ArenaMgr
{
	USE_SINGLETON_NEW(ArenaMgr);
public :
	static const UINT32 s_MAX_REWARD_NUM = 5000;				//最大奖励人数
	static const UINT32 s_MAX_PKLIST_NUM = 10;					//pk列表人数
	static const UINT32 s_PKLIST_BEFORE_NUM = 8;				//pk列表前面的8人
	static const UINT32 s_PKLIST_AFTER_NUM = 2;					//pk列表的后2人
	static const UINT32 s_INIT_PK_RANK = 3000;					//开始进行pk时候的排名
	static const UINT32 s_HISTORY_RANK_LEVELUP_REWARD = 10000;	//历史排名奖励
	static const UINT32 s_MAX_REWARD_DAY = 7;					//最大奖励天数
private :
	//构造
	ArenaMgr();
public :
	//析构
	~ArenaMgr();
	//返回实例
	static ArenaMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载所有数据
	void loadAll();
	//返回排在我前面的四个人
	void sendPKList(Player * pUser);
	//发送前20名
	void sendSortList(Player * pUser,UINT16 beginRank);
	//替换名次
	void replaceRank(Player * pUser,UINT32 rank,UINT32 historyRank);
	//定时器
	void timer(UINT32 cur);
	//插入新记录
	void insertRecord(const stArenaSortInfoDB & data);
	//插入记录回调
	void insertRecordCB(bool opResult,UINT64 autoID);
	//更新数据
	void updateRecord(const stArenaSortInfoDB & data);
	//更新记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除数据
	void deleteRecord(UINT32 charID);
	//删除记录回调
	void deleteRecordCB(bool opResult,UINT64 autoID);
	//更新昨日排行信息
	void updateYestodaySortList();
	//插入一条昨天奖励数据回调
	void insertYestorySortListCB(bool opResult,UINT64 autoID);
	//删除一条昨日奖励回调
	void deleteYestorySortListCB(bool opResult,UINT64 autoID);
	//删除已经领取奖励角色ID回调
	void deleteGotRewardCharIDCB(bool opResult,UINT64 autoID);
	//获得我自己的排名
	UINT32 getMyRank(UINT32 charID);
	//挑战
	void challege(Player * pUser,UINT32 charID);
	//用户登录了
	void userOnline(Player * pUser);
	//保存已经领取奖励的角色ID
	void saveGotRewardCharID(UINT32 timestamp,UINT32 charID);
	//增加领取角色ID记录
	void addGotRewardCharIDRecordCB(bool opResult,UINT64 autoID);
	//更新领取角色ID记录
	void updateGotRewardCharIDRecordCB(bool opResult,UINT64 autoID);
	//获取前三名
	void getTop3(stHistoryRankInfo & info);
	//发送我的名次到game
	void notifyMyRank2Game(Player * pUser);
	//新增玩家竞技场信息
	void addArenaUserInfo(const stUserArenaInfoDB & data);
	//新增玩家竞技场信息回调
	void addArenaUserInfoCB(bool opResult,UINT64 autoID);
	//更新玩家竞技场信息
	void updateArenaUserInfo(const stUserArenaInfoDB & data,const char * szWhere);
	//更新玩家竞技场信息回调
	void updateArenaUserInfoCB(bool opResult,UINT64 autoID);
	//重置竞技场场玩家数据
	void resetUserArenaInfo();
	//删除所有玩家信息回调
	void deleteArenaUserInfoCB(bool opResult,UINT64 autoID);
	//获得玩家竞技场信息
	stUserArenaInfo * getUserArenaInfo(UINT32 charID);
	//更新玩家竞技场信息
	void saveUserArenaInfo(stUserArenaInfo & data);
	//重置挑战次数
	void resetChallengeNum(Player * pUser);
	//清除cd
	void clearCD(Player * pUser);
	//竞技场结算后,改变竞技场信息
	void changeUserArenaInfo(Player * pUser);
	//同步信息
	void synRecord(Player * pUser);
private :
	std::map<UINT32,stArenaSortInfo>  m_charID2SortInfo;			//角色与排序信息对应信息
	std::map<UINT32,UINT32>   m_rank2CharID;						//名次与角色ID对应
	std::map<UINT32,std::map<UINT32,UINT32> > m_historyCharID2Rank; //历史ID与名次对应关系.<timestamp,<rank,charID>>
	std::map<UINT32,std::set<UINT32> >	m_hasRewardCharID;			//已经领取奖励的角色ID. <timestamp,<charID>>
	UINT32 m_theLowestRank;											//最低名次

	typedef __gnu_cxx::hash_map<UINT32,stUserArenaInfo>  UserArenaInfoMap;
	typedef UserArenaInfoMap::iterator UserArenaInfoMap_IT;
	UserArenaInfoMap m_userArenaInfos;
};
#endif
