#ifndef __H_SERVER_CONFIG_MGR_H__
#define __H_SERVER_CONFIG_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include <map>
#include <string>
#pragma pack(1)
struct 	stServerConfigInfoDB
{
	UINT32 m_id;
	char   m_config[10240];

	stServerConfigInfoDB()
	{
		m_id = 0;
		bzero(m_config,sizeof(m_config));
	}
};
#pragma pack()

class CServerConfigMgr
{
	USE_SINGLETON_NEW(CServerConfigMgr);
private :
	//构造函数
	CServerConfigMgr()
	{
		m_id = 0;
	}
public :
	//获得单例
	static CServerConfigMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CServerConfigMgr();
	//加载全部
	bool load();
	//获得某个全局变量
	std::string  getValue(const std::string &key);
	//重载[]
	std::string  operator [](const std::string &key);
	//设置全局变量
	void setValue(const std::string &key,const std::string &value,bool bSave = true);
	//更新一个全局记录
	void updateRecord();
	//更新一个全局记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//通知网关服务器配置
	void notifyGate();
	//通知游戏服务器配置
	void notifyGame();
	//notifybill
	void notifyBill(const std::string & billUrl);
private :
	std::map<std::string,std::string> m_key2Values;
	UINT32 m_id;
};
#endif
