#ifndef __H_BLACK_WHITE_MGR_H__
#define __H_BLACK_WHITE_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "TemplateDB.h"
#include <string>
#include <set>

#pragma pack(1)
struct stLoginIPTypeDB
{
	UINT16 m_type;                  //ip类型
	char   m_ip[50];                //ip

	stLoginIPTypeDB() 
	{   
		m_type = 0;
		bzero(m_ip,sizeof(m_ip));
	}   
}; 

struct stMaintennaceDB
{
	UINT32 m_id;
	UINT32 m_beginTime;
	UINT32 m_lastTime;

	stMaintennaceDB()
	{
		m_id = 0;
		m_beginTime = 0;
		m_lastTime = 0;
	}
};
#pragma pack()

class CBlackWhiteMgr
{
	USE_SINGLETON_NEW(CBlackWhiteMgr);
private :
	//析构函数
	~CBlackWhiteMgr();
public :
	//构造函数
	CBlackWhiteMgr();
	//获得实例
	static CBlackWhiteMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//是否是黑名单
	bool isBlackName(std::string & ip);
	//是否白名单
	bool isWhiteName(std::string & ip);
	//是否处于维护
	bool isMaintenane();
	//加载配置
	void load();
	//添加黑名单
	void addBlackName(const std::string & ip);
	//添加白名单
	void addWhiteName(const std::string & ip);
	//设置维护时间
	void setMaintanenceTime(UINT32 maintennaceTime,UINT32 lastTime)
	{
		m_maintennaceTime = maintennaceTime;
		m_maintenaceLastTime = lastTime;
		updateMaintennaceInfo();
	}
	//清空黑白名单
	void clear()
	{
		m_blackLists.clear();
		m_whiteLists.clear();
	}
	//更新维护信息
	void updateMaintennaceInfo();
	//更新维护信息回调
	void updateMaintennaceInfoCB(bool opResult,UINT64 autoID);
private :
	std::set<std::string> m_blackLists;			//黑名单
	std::set<std::string> m_whiteLists;			//白名单
	UINT32  m_keyID;								//id
	UINT32 m_maintennaceTime;					//维护开始时间
	UINT32 m_maintenaceLastTime;				//维护持续时间
};
#endif
