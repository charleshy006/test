#ifndef __H_OPEN_SERVER_MGR_H__
#define __H_OPEN_SERVER_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#pragma pack(1)
struct 	stOpenServerInfoDB
{
	UINT32 m_id;
	UINT32 m_openTime;			//开服时间
	UINT32 m_openDay;			//开服时间
	UINT32 m_lastRefreshTime;	//上次刷新时间

	stOpenServerInfoDB()
	{
		m_id = 0;
		m_openTime = 0;
		m_openDay = 0;
		m_lastRefreshTime = 0;
	}
};
#pragma pack()

struct stOpenServerInfo
{
	UINT32 m_id;
	UINT32 m_openTime;			//开服时间
	UINT32 m_openDay;			//开服时间
	UINT32 m_lastRefreshTime;	//上次刷新时间

	stOpenServerInfo()
	{
		m_id = 0;
		m_openTime = 0;
		m_openDay = 0;
		m_lastRefreshTime = 0;
	}

	void initFromDB(const stOpenServerInfoDB & data);
	void fillDBData(stOpenServerInfoDB & data);
};

class COpenServerMgr
{
	USE_SINGLETON_NEW(COpenServerMgr);
private :
	//构造函数
	COpenServerMgr()
	{}
public :
	//获得单例
	static COpenServerMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~COpenServerMgr();
	//加载全部
	bool load();
	//定时器
	void timer(UINT32 cur);
	//更新数据
	void updateRecord(stOpenServerInfoDB & data);
	//更新数据回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//发送
	void sendOpenServerInfo();
	//开服时间
	UINT32 getOpenSvrTime()
	{
		return m_openServerInfo.m_openTime;
	}
private :
	stOpenServerInfo m_openServerInfo;
};
#endif
