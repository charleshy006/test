#ifndef __H_ACTIVITY_CONFIG_MGR_MGR_H__
#define __H_ACTIVITY_CONFIG_MGR_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include <map>
#include <string>
#include <set>
#pragma pack(1)
struct 	stActivityConfigInfoDB
{
	UINT32 m_activityID;
	char   m_config[4096];

	stActivityConfigInfoDB()
	{
		m_activityID = 0;
		bzero(m_config,sizeof(m_config));
	}
};
#pragma pack()

class CActivityConfigMgr
{
	USE_SINGLETON_NEW(CActivityConfigMgr);
private :
	//构造函数
	CActivityConfigMgr()
	{}
public :
	//获得单例
	static CActivityConfigMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CActivityConfigMgr();
	//加载配置
	void load();
	//保存活动配置
	void save(UINT32 id,std::string configJson);
	//删除活动
	void del(UINT32 id);
private :
	std::set<UINT32> m_activities;
};
#endif
