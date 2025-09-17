#ifndef __H_HISTORY_RANK_MGR_H__
#define __H_HISTORY_RANK_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include <map>
#pragma pack(1)
struct 	stHistoryRankInfoDB
{
	UINT32 m_moduleID;
	char   m_info[100];

	stHistoryRankInfoDB()
	{
		m_moduleID = 0;
		bzero(m_info,sizeof(m_info));
	}
};
#pragma pack()

struct stHistoryRankInfo
{
	UINT32 m_moduleID;							//模块ID
	std::map<UINT32,UINT16> m_charID2Rank;		//角色ID与名次对应关系

	stHistoryRankInfo()
	{
		reset();
	}
	
	void reset()
	{
		m_moduleID = 0;
		m_charID2Rank.clear();
	}

	void initFromDB(const stHistoryRankInfoDB & data);
	void fillDBData(stHistoryRankInfoDB & data);
};

/*enum eRankModule_ID
{
	eRankModule_Level_Rank = 1,			//等级排名
	eRankModule_BattlePower_Rank = 2,	//战力排名
	eRankModule_Copymap_Rank = 3,		//副本排行
	eRankModule_One2One_Rank = 4,		//巅峰排名
	eRankModule_Leitai_Rank = 5,		//擂台排行
};*/

class CHistoryRankMgr
{
	USE_SINGLETON_NEW(CHistoryRankMgr);
private :
	//构造函数
	CHistoryRankMgr()
	{}
public :
	//获得单例
	static CHistoryRankMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CHistoryRankMgr();
	//加载全部
	bool load();
	//新增一个全局记录
	void addRecord(const stHistoryRankInfoDB & data);
	//新增记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一个全局记录
	void updateRecord(const stHistoryRankInfoDB & data);
	//更新一个全局记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//保存数据
	void save(stHistoryRankInfo & info);	
	//定时器
	void timer(UINT32 cur);
	//广播排名信息
	void broadcastRankInfo();
private :
	std::map<UINT32,stHistoryRankInfo > m_moduleRanks;		//模块的排名
};
#endif
