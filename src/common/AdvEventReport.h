#ifndef __H_ADV_EVENT_REPORT_H__
#define __H_ADV_EVENT_REPORT_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <string>

class AdvEventReport
{
	USE_SINGLETON_NEW(AdvEventReport);
public :
	static const std::string  s_IOS_REPORT_URL;
	static const std::string  s_ANDRIOD_REPORT_URL;
public :
	//构造函数
	AdvEventReport();	
	//析构函数
	~AdvEventReport();
public :
	//构造函数 
	static AdvEventReport & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//初始化
	bool init(UINT32 platformID,UINT16 level);
	//上报创建角色
	void reportCreateRole(UINT32 cur,UINT32 opID,std::string devicID,std::string ip,UINT32 gameID,UINT32 serverID,std::string  bundleID,std::string gameVersion,std::string  account,std::string name);
	//上报角色登录
	void reportLogin(UINT32 cur,UINT32 gameID,UINT32 serverID,std::string deviceID,std::string bundleID,std::string gameVersion,std::string account,std::string name);
	//上报升级
	void reportLevelup(UINT32 cur,UINT32 gameID,UINT32 serverID,std::string bundleID,std::string account,std::string name,UINT16 level);
private :
	bool m_bIOS;		//是否是ios
	UINT16 m_level;
};
#endif
