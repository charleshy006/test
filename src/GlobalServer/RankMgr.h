#ifndef __H_RANKMGR_H__
#define __H_RANKMGR_H__
#include "Singleton.h"
#include "TypesDef.h"
#include "SortList.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "HistoryRankMgr.h"
#include <ext/hash_map>
#include <map>

#pragma pack(1)
struct stLevelRankDB
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_level;						//等级
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID	

	stLevelRankDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};
#pragma pack()

struct stLevelRankInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_level;						//等级
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;                   //头像ID

	stLevelRankInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}

	bool operator < (const stLevelRankInfo & one) const 
	{
		return m_level > one.m_level; 
	}

	UINT32 getKey() const 
	{
		return m_charID;
	}

	UINT32 getCmpValue() const 
	{
		return m_level;
	}

	void fillDBData(stLevelRankDB & data)
	{
		data.m_charID = m_charID;
		data.m_level = m_level;
		strncpy(data.m_name,m_name,sizeof(m_name));
		data.m_job = m_job;
		data.m_sex = m_sex;
		data.m_photoID = m_photoID;
	}
};

#pragma pack(1)
struct stBattlePowerRankDB
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_battlePower;				//战力
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;                   //头像ID
	
	stBattlePowerRankDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_battlePower = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};
#pragma pack()

struct stBattlePowerRankInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_battlePower;				//战力
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;                   //头像ID  

	stBattlePowerRankInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_battlePower = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}

	bool operator < (const stBattlePowerRankInfo & one) const 
	{
		return m_battlePower > one.m_battlePower; 
	}

	UINT32 getKey() const 
	{
		return m_charID;
	}

	UINT32 getCmpValue() const
	{
		return m_battlePower;
	}
	
	void fillDBData(stBattlePowerRankDB & data)
	{
		data.m_charID = m_charID;
		data.m_battlePower = m_battlePower;
		strncpy(data.m_name,m_name,sizeof(m_name));
		data.m_job = m_job;
		data.m_sex = m_sex;
		data.m_photoID = m_photoID;
	}
};

#pragma pack(1)
struct stCopyMapRankDB
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_starNum;					//等级
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID
	
	stCopyMapRankDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_starNum = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};
#pragma pack()

struct stCopyMapRankInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_starNum;					//星星数量
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID
	
	stCopyMapRankInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_starNum = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}

	bool operator < (const stCopyMapRankInfo & one) const 
	{
		return m_starNum > one.m_starNum; 
	}

	UINT32 getKey() const 
	{
		return m_charID;
	}

	UINT32 getCmpValue() const 
	{
		return m_starNum;
	}

	void fillDBData(stCopyMapRankDB & data)
	{
		data.m_charID = m_charID;
		data.m_starNum = m_starNum;
		strncpy(data.m_name,m_name,sizeof(m_name));
		data.m_job = m_job;
		data.m_sex = m_sex;
		data.m_photoID = m_photoID;
	}
};
#pragma pack(1)
struct stLeitaiRankDB
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_useTime;					//使用时间
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID

	stLeitaiRankDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_useTime = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};
#pragma pack()

struct stLeitaiRankInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_useTime;					//使用时间
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID

	bool operator < (const stLeitaiRankInfo & one) const 
	{
		return m_useTime < one.m_useTime; 
	}

	UINT32 getKey() const 
	{
		return m_charID;
	}

	UINT32 getCmpValue() const 
	{
		return m_useTime;
	}

	void fillDBData(stLeitaiRankDB & data)
	{
		data.m_charID = m_charID;
		strncpy(data.m_name,m_name,sizeof(m_name));
		data.m_useTime = m_useTime;
		data.m_job = m_job;
		data.m_sex = m_sex;
		data.m_photoID = m_photoID;
	}
};

#define OPEN_SVR_POWER_RANK 50
#pragma pack(1)
//开始战力排名
struct stOpenSvrPowerRankDB
{
	UINT32 m_id;
	UINT8  m_hasReward;
	union {
		struct
		{
			UINT16 m_rank;
			UINT32 m_charID;
			UINT32 m_bpVal;
		}data[OPEN_SVR_POWER_RANK];
		char   m_binData[1024];
	}data;

	stOpenSvrPowerRankDB()
	{
		m_id = 0;
		m_hasReward = 0;
		bzero(data.m_binData,sizeof(data.m_binData));
	}
};
#pragma pack()

struct stOpenSvrBPRankItem
{
	UINT32 m_charID;
	UINT16 m_bpVal;

	stOpenSvrBPRankItem()
	{
		m_charID = 0;
		m_bpVal = 0;
	}
};

struct stOpenSvrPowerRank
{
	UINT32 m_id;
	UINT8  m_hasReward;
	std::map<UINT16,stOpenSvrBPRankItem> m_rank2CharIDs;

	stOpenSvrPowerRank()
	{
		m_id = 0;
		m_hasReward = 0;
	}

	void fillDataDB(stOpenSvrPowerRankDB & data);
	void initFromDB(stOpenSvrPowerRankDB & data);
};

enum
{
	LEVEL_RANK = 1,				//等级排名
	BATTLEPOWER_RANK = 2 ,	//战力排名
	COPYMAP_RANK = 3,			//副本排名
	LETAI_RANK = 4,				//擂台排名
};

typedef CSortList<stLevelRankInfo>  CLevelSortList;
typedef CSortList<stBattlePowerRankInfo,2000>  CBattleAbilitySortList;
typedef CSortList<stCopyMapRankInfo>  CCopyMapStarSortList;
typedef CSortList<stLeitaiRankInfo>   CLeitaiSortList;

class Player;
class CRankMgr
{
	USE_SINGLETON_NEW(CRankMgr);
public :
	static const UINT8 s_SEND_SORTITEM_MAX = 15;
private :
	CRankMgr() {}
public :
	//析构函数
	~CRankMgr() {}
	//获得排名管理实例
	static CRankMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载数据
	void load();	
	//增加一个记录
	void addRecord(const char * pData,UINT8 rankType);
	//增加记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一条记录
	void updateRecord(UINT32 key,const char * pData,UINT8 rankType);
	//更新一条记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除一条记录
	void removeRecord(UINT32 key,UINT8 rankType);
	//删除一条记录回调
	void removeRecordCB(bool opResult,UINT64 autoID);
	//更新我的等级
	void updateMyLevel(Player * pUser,UINT16 level);
	//更行的星星数
	void updateMyStarNum(Player * pUser,UINT16 starNum);
	//更新我的战力
	void updateMyBattlePower(Player * pUser,UINT32 battlePower);
	//更新的擂台使用时间
	void updateLeitaiUseTime(Player * pUser,UINT32 useTime);
	//发送等级排行榜
	void sendLevelSortList(Player * pUser,UINT32 beginRank);
	//发送战力排行帮数据
	void sendBattlePowerSortList(Player * pUser,UINT32 beginRank);
	//发送等级排行榜数据
	void sendCopymapSortList(Player * pUser,UINT32 beginRank);
	//发送擂台排行榜
	void sendLeitaiSortList(Player * pUser,UINT32 beginRank);
	//定时回调
	void timer(UINT32 cur);
	//重置擂台玩家数据
	void resetUserLeitaiInfo();
	//置擂台玩家数据回调
	void deleteLeitaiUserInfoCB(bool opResult,UINT64 autoID);
	//角色创建完成
	void afterRoleCreate(UINT32 charID,UINT16 level)
	{
		m_charID2LevelMap[charID] = level;
	}
	//获得某个人等级
	UINT16 getLevelByCharID(UINT32 charID);
	//获得战力榜上某个排名的玩家
	UINT32 getBattlePowerCharIDByRank(UINT32 rank);
	//获得战力排行人数
	UINT32 getBattlePowerNum()
	{
		return m_battlePowerSortList.getListNum();
	}
	//获得某个人的战力
	UINT32 getBattlePowerByCharID(UINT32 charID);
	//获得我的副本排名
	UINT16 getMyCopyMapRank(UINT32 charID);
	//获得我的战力排名
	UINT16 getMyBattlePowerRank(UINT32 charID);
	//获得排名系统的前3名
	void getLevelSortListTop3(stHistoryRankInfo & info);
	//获得战力排行的前3名
	void getBattlePowerSortListTop3(stHistoryRankInfo & info);
	//获得副本排名的前3名
	void getCopymapSortListTop3(stHistoryRankInfo & info);
	//获得擂台排名前3名
	void getLeitaiSortListTop3(stHistoryRankInfo & info);
	//检查开服战力排名
	void checkOpenSvrPowerRank();
	//更新开服战力排名
	void updateOpenSvrPowerRank(const stOpenSvrPowerRankDB & data);
	//更新开服战力排名回调
	void updateOpenSvrPowerRankCB(bool opResult,UINT64 autoID);
	//获得开服战力排行
	void sendOpenSvrBPRank(Player * pUser);
	//同步记录
	void synRecord(Player * pUser);
	//获取等级排行榜第一名
	UINT32 getMaxLevelId();
private :
	CLevelSortList m_levelSortList;
	CBattleAbilitySortList m_battlePowerSortList;
	CCopyMapStarSortList  m_copymapStarSortlList;
	CLeitaiSortList	   m_leitaiSortList;
	typedef __gnu_cxx::hash_map<UINT32,UINT32>::iterator CharID2LevelMap_IT;
	__gnu_cxx::hash_map<UINT32,UINT32> m_charID2LevelMap;
	typedef __gnu_cxx::hash_map<UINT32,UINT32>::iterator  CharID2BattlePower_IT;
	__gnu_cxx::hash_map<UINT32,UINT32> m_charID2BattlePowerMap;
	stOpenSvrPowerRank m_openSvrPowerRank;
};
#endif
