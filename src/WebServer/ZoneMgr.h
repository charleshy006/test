#ifndef __H_ZONEMGR_H__
#define __H_ZONEMGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <string>
#include <vector>

struct stZoneInfo
{
	std::string m_name;			//名字
	std::string m_status;		//状态
	std::string m_serverID;		//服务器ID
	std::string m_isActive;		//是否激活
	std::string m_isRecommend;	//是否推荐
	std::string m_ip;			//服务器IP
	std::string m_port;			//端口

	stZoneInfo()
	{
	}
};

class CZoneMgr
{
	USE_SINGLETON_NEW(CZoneMgr);
private :
	//析构函数
	~CZoneMgr();
public :
	//构造函数
	CZoneMgr();	
	//返回ZoneMgr对象
	static CZoneMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool load();
	//返回区信息
	std::string getZoneInfo();
	//根据平台返回区服信息
	std::string getZoneInfoByPlatfromID(UINT32 platformID);
	//读取一个文件
	std::string readFile(const char * szPath);
private :
	std::vector<stZoneInfo>  m_zoneInfos;
	std::string m_appServerInfo;
	std::string m_txServerInfo;
	std::string m_gcServerInfo;
};
#endif
