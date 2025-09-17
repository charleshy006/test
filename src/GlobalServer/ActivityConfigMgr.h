#ifndef __H_ACTIVITY_CONFIG_MGR_MGR_H__
#define __H_ACTIVITY_CONFIG_MGR_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include <map>
#include <string>
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
	//加载全部
	bool load();
	//发送活动配置
	void sendActivityConfig(UINT16 svrID);
private :
	std::map<UINT32,std::string> m_activitiesConfig;
};
#endif
