#ifndef __H_UPDATEINFOMGR_H__
#define __H_UPDATEINFOMGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "UpdateServer.h"
#include <string>
#include <ext/hash_map>

#pragma pack(1)
struct stInnerUpdateInfoDB
{
	UINT32 m_gameID;
	UINT32 m_opGameID;
	char   m_version[25];
	char   m_upgradeURL[250];

	stInnerUpdateInfoDB()
	{
		m_gameID = 0;
		m_opGameID = 0;
		bzero(m_version,sizeof(m_version));
		bzero(m_upgradeURL,sizeof(m_upgradeURL));
	}
};
#pragma pack()

struct stUpdateInfo
{
	UINT32 m_gameID;
	UINT32 m_opGameID;
	std::string m_version;
	std::string m_upgradeURL;

	stUpdateInfo()
	{
		m_gameID = 0;
		m_opGameID = 0;
	}

	void initFromDB(const stInnerUpdateInfoDB & data);
	void fillDBData(stInnerUpdateInfoDB & data);
};

struct stTotalUpdateInfo
{
	std::string m_appReviewVer;
	std::string m_curVer;
	std::string m_appUrl;

	stTotalUpdateInfo()
	{
	
	}
};

class UpdateInfoMgr
{
	USE_SINGLETON_NEW(UpdateInfoMgr);
private :
	//析构函数
	~UpdateInfoMgr();
public :
	//构造函数
	UpdateInfoMgr();	
	//返回ZoneMgr对象
	static UpdateInfoMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载
	bool load();
	//清空所有
	void clearAll();
	//获得更新地址
	std::string getUpgradeURL(UINT32 gameID,UINT32 opGameID,std::string version);
	//获得是否开始版本信息
	std::string & getOpenAppVersion()
	{
		return m_openAppVersion;
	}
	//获得是否沙箱
	std::string & getSanbox()
	{
		return m_sanbox;
	}
	//直接购买
	std::string getDirectBuy()
	{
		return m_directBuy;
	}

	//获得版本信息
	bool getTotalUpdateInfo(UINT32 opID,std::string & appReviewVer, std::string & curVer,std::string & appUrl);
private :
	typedef __gnu_cxx::hash_map<UINT32,__gnu_cxx::hash_map<std::string,stUpdateInfo *, my_hash<std::string> > > UpdateInfoMap;
	typedef UpdateInfoMap::iterator UpdateInfoMap_It;
	UpdateInfoMap m_updateInfos;
	std::string m_openAppVersion;
	std::string m_sanbox;
	std::string m_directBuy;
	typedef __gnu_cxx::hash_map<UINT32,stTotalUpdateInfo * > TotalUpdateInfoMap;
	typedef TotalUpdateInfoMap::iterator TotalUpdateInfoMap_It;
	TotalUpdateInfoMap  m_totalUpdateInfos;
	//std::string m_appReviewVer;
	//std::string m_curVer;
	//std::string m_appstoreUrl;
};
extern UpdateServer * g_updateServer;
#endif
