#ifndef __H_WORLD_BOSS_H__
#define __H_WORLD_BOSS_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include "SortList.h"
#include <map>
#include <set>
#pragma pack(1)
struct 	stWorldBossActivityDB
{
	UINT32 m_id;				//id标志
	UINT32 m_levelID;			//关卡ID
	UINT32 m_bossID;			//bossID
	UINT32 m_beginTime;			//开始时间
	UINT32 m_endTime;			//结束时间
	UINT8  m_isStart;			//是否已经开启,1表示已经开启

	stWorldBossActivityDB()
	{
		m_id = 0;
		m_levelID = 0;
		m_bossID = 0;
		m_beginTime = 0;
		m_endTime = 0;
		m_isStart = 0;
	}
};

//世界boss伤害排名
struct stWorldBossDamageRankDB
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//名字
	UINT32 m_damage;					//伤害
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID

	stWorldBossDamageRankDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_damage = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};

//玩家世界boss活动购买相关信息
struct stWorldBossBuyDB
{
	UINT32 m_charID ;			//角色ID
	UINT32 m_lastLeaveTime;		//最新一次离开时间
	UINT16 m_buyNum;			//购买次数

	stWorldBossBuyDB()
	{
		m_charID = 0;
		m_lastLeaveTime = 0;
		m_buyNum = 0;
	}
};

//世界boss信息
struct stWorldBossDB
{
	UINT32 m_bossID;
	UINT32 m_hp;
	UINT16 m_stepID;

	stWorldBossDB()
	{
		m_bossID = 0;
		m_hp = 0;
		m_stepID = 0;
	}
};
#pragma pack()

struct stWorldBossActivityInfo
{
	UINT32 m_id;				//id标志
	UINT32 m_levelID;			//关卡ID
	UINT32 m_bossID;			//bossID
	UINT32 m_bossHP;			//boss血量
	UINT16 m_stepID;			//阶段
	UINT32 m_beginTime;			//开始时间
	UINT32 m_endTime;			//结束时间
	UINT8  m_isStart;			//是否已经开启,1表示已经开启

	stWorldBossActivityInfo()
	{
		m_id = 0;
		m_levelID = 0;
		m_bossID = 0;
		m_bossHP = 0;
		m_stepID = 0;
		m_beginTime = 0;
		m_endTime = 0;
		m_isStart = 0;
	}

	void initFromDB(const stWorldBossActivityDB & data);

	void fillDBData(stWorldBossActivityDB & data);
};

struct stWorldBossDamageRankInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//名字
	UINT32 m_damage;					//伤害
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID

	stWorldBossDamageRankInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_damage = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}

	bool operator < (const stWorldBossDamageRankInfo & one) const 
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

	void initFromDB(const stWorldBossDamageRankDB & data);
	void fillDBData(stWorldBossDamageRankDB & data);
};

struct stWorldBossBuyInfo
{
	UINT32 m_charID ;			//角色ID
	UINT32 m_lastLeaveTime;		//最新一次离开时间
	UINT16 m_buyNum;			//购买次数
	
	stWorldBossBuyInfo()
	{
		m_charID  = 0;
		m_lastLeaveTime = 0;
		m_buyNum = 0;
	}

	void initFromDB(const stWorldBossBuyDB & data);
	void fillDBData(stWorldBossBuyDB & data);
};


struct stWorldBossInfo
{
	UINT32 m_bossID;
	UINT32 m_hp;
	UINT16 m_stepID;

	stWorldBossInfo()
	{
		m_bossID = 0;
		m_hp = 0;
		m_stepID = 0;
	}
	void initFromDB(const stWorldBossDB & data);
	void fillDBData(stWorldBossDB & data);
};

typedef CSortList<stWorldBossDamageRankInfo> TWolrdBossDamageSortList;

class CWorldBossMgr
{
	USE_SINGLETON_NEW(CWorldBossMgr);
public :
	static const UINT32 s_SEND_SORTITEM_MAX = 10;
	static const UINT32 s_MAX_MATCH_NUM = 3;
	static const UINT32 s_CHALLEGE_CD = 30;
	static const UINT32 s_ADD_DAMAGE_PER_TIME = 50;			//士气鼓舞一次加的伤害
	static const UINT32 s_MATCH_ROBOT = 15;	//15秒匹配不到就匹配机器人
private :
	//构造函数
	CWorldBossMgr()
	{
		m_checkMatchTick = 0;
	}
public :
	//获得单例
	static CWorldBossMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CWorldBossMgr();
	//加载全部
	bool load();
	//新增一个全局记录
	void addRecord(stWorldBossActivityDB & data);
	//新增记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一个全局记录
	void updateRecord(stWorldBossActivityDB & data);
	//更新一个全局记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//定时回调
	void timer(UINT32 cur);
	//活动开启
	void activityStart();
	//活动结束
	void activityEnd();
	//更新我的血量
	void updateMyDamage(Player * pUser,UINT32 damage);
	//发送伤害排名
	void sendDamageSortList(Player * pUser,UINT32 fromRank);
	//玩家下线了
	void userOffline(Player * pUser);
	//玩家开始匹配
	void userEnterMatch(Player * pUser);
	//取消匹配
	void cancelMatch(Player * pUser);
	//打开界面
	void openWin(Player * pUser);
	//清cd
	void clearCD(UINT32 charID);
	//设置最近一次挑战时间
	void setLastLeaveTime(UINT32 charID,UINT32 cur);
	//获得上次挑战时间
	UINT32 getLastLeaveTime(UINT32 charID);
	//请求购买士气鼓舞
	void reqBuy(Player * pUser);
	//设置伤害加成
	void setBuyNum(UINT32 charID,UINT16 buyNum);
	//获得某个玩家的伤害加成
	UINT16 getAddDamagePer(UINT32 charID);
	//增加玩家购买记录
	void addBuyInfoRecord(stWorldBossBuyDB & data);
	//增加玩家购买记录回调
	void addBuyInfoRecordCB(bool opResult,UINT64 autoID);
	//更新玩家购买记录
	void updateBuyInfoRecord(stWorldBossBuyDB & data);
	//更新购买玩家记录回调
	void updateBuyInfoRecordCB(bool opResult,UINT64 autoID);
	//更新boss血量
	void updateBossHP(UINT32 hp);
	//活得世界boss血量
	UINT32 getWorldBossHP();
	//增加阶段
	void incBossStep();
	//清空伤害排名表
	void dropDamageSortListTable();
	//清空伤害排名表回调
	void dropDamageSortListTableCB(bool opResult,UINT64 autoID);
	//清空购买信息
	void dropBuyInfo();
	//清空购买信息回调
	void dropBuyInfoCB(bool opResult,UINT64 autoID);
	//增加世界boss信息
	void addWorldBossRecord(stWorldBossDB & data);
	//增加世界boss信息回调
	void addWorldBossRecordCB(bool opResult,UINT64 autoID);
	//更新世界boss信息
	void updateWorldBossRecord(stWorldBossDB & data);
	//更新世界boss回调
	void updateWorldBossRecordCB(bool opResult,UINT64 autoID);
	//是否是否开始
	bool isActivityStart()
	{
		return m_bossActivityInfo.m_isStart ? true : false;
	}
	//设置击杀者名字
	void setKillerName(const char * szName)
	{
		if (!szName){
			return ;
		}
		m_killerName = szName;
	}
	//同步记录
	void synRecord(Player * pUser);
	//检测匹配
	void checkMatch(bool force=false);
private :
	stWorldBossActivityInfo m_bossActivityInfo;
	TWolrdBossDamageSortList m_damageSortList;
	std::set<UINT32> m_waitMatches;					//等待匹配
	std::map<UINT32,stWorldBossBuyInfo> m_buyInfos;	//购买信息
	std::map<UINT32,stWorldBossInfo>  m_bossInfos;
	std::string m_killerName;						//击杀者名字
	INT32 m_checkMatchTick;	//检测匹配计数器
};
#endif
