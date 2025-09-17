#ifndef __H_SHOP_ACTIVITY_H__
#define __H_SHOP_ACTIVITY_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <map>

class GameUser;
class CShopActivityMgr
{
	USE_SINGLETON_NEW(CShopActivityMgr);
public :
	struct stShopActivity
	{
		UINT32 m_id;
		UINT32 m_shopID;
		UINT32 m_startTime;
		UINT32 m_endTime;

		stShopActivity()
		{
			m_id = 0;
			m_shopID = 0;
			m_startTime = 0;
			m_endTime = 0;
		}
	};
public :
	const static UINT32 s_SHOP_ACTIVITY_VERSION = 20150130;
public :
	//获得单例
	static CShopActivityMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//构造函数
	CShopActivityMgr();
	//析构函数
	~CShopActivityMgr();
	//加载配置
	bool loadCfg();
	//活动是否开启
	bool isShopOpen(UINT32 shopID);
	//发送所有活动
	void sendAllActivity(GameUser * pUser);
public :
	std::map<UINT32,stShopActivity> m_activities;
};
#endif
