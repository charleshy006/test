#ifndef __H_GLOBAL_VAL_MGR_H__
#define __H_GLOBAL_VAL_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include <map>
#pragma pack(1)
struct 	stGlobalVarInfoDB
{
	UINT32 m_key;
	UINT32 m_value;

	stGlobalVarInfoDB()
	{
		m_key = 0;
		m_value = 0;
	}
};
#pragma pack()

enum eGlobalVar
{
	eGlobalVar_Refresh_HistoryRank = 1,			//刷新历史排名
	eGlobalVar_Refresh_YestoryArenaRank = 2,	//刷新昨日竞技场排名
	eGlobalVar_Refresh_One2OneRank = 3,			//巅峰对排名刷新
	eGlobalVar_Refresh_Week_One2OneRank = 4,	//巅峰周排名刷新
	eGlobalVar_Refresh_Guild_Zero = 5,			//公会零点刷新
	eGlobalVar_Common_RefreshTime = 6,			//统一刷新时间
	eGlobalVar_Refresh_Guild_Score = 7,			//公会积分刷新
	eGlobalVar_Refresh_Week_LeiTaiRank = 8,		//擂台周排名刷新
};

class CGlobalVarMgr
{
	USE_SINGLETON_NEW(CGlobalVarMgr);
private :
	//构造函数
	CGlobalVarMgr()
	{}
public :
	//获得单例
	static CGlobalVarMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CGlobalVarMgr();
	//加载全部
	bool load();
	//获得某个全局变量
	UINT32 getValue(UINT32 key);
	//重载[]
	UINT32 operator [](UINT32 key);
	//设置全局变量
	void setValue(UINT32 key,UINT32 value);
	//新增一个全局记录
	void addRecord(UINT32 key,UINT32 value);
	//新增记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一个全局记录
	void updateRecord(UINT32 key,UINT32 value);
	//更新一个全局记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//判断服务器访问数据库是否成功
	bool verifyDataBase();
private :
	std::map<UINT32,UINT32> m_key2Values;
};
#endif
