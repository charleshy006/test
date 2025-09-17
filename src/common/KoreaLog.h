#ifndef __H_KOREAlOG_H__
#define __H_KOREAlOG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"

class CKoreaLogMgr
{
	USE_SINGLETON_NEW(CKoreaLogMgr);
private :
	//构造函数
	CKoreaLogMgr();
public :
	enum 
	{
		LOG_CREATE_ROLE = 1001,			//创建角色
		LOG_LOGIN_ON = 1002,			//登录
		LOG_LOGIN_OUT = 1003,			//用户登出
		LOG_USER_LEVELUP = 1010,		//用户升级
		LOG_USER_NEWGUIDE = 1011,		//用户教程
		LOG_RECHARGE = 1020,			//用户充值
		LOG_CONSUME = 1021,				//用户消费
		LOG_HERO_LEVELUP = 2001,		//武将升级
	};
public :
	//析构函数
	~CKoreaLogMgr();
	//返回单例
	static CKoreaLogMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//初始化
	bool init(std::string & fileName);
	//创建角色
	void createRole(std::string account,UINT32 charID,std::string ip,UINT32 op_id,std::string roleName,UINT32 curTime,std::string deviceType);
	//用户登录
	void loginOn(UINT32 curTime,std::string account,UINT32 charID,std::string ip,UINT16 level);
	//用户登出
	void loginOff(UINT32 curTime,std::string account,UINT32 charID,UINT16 level);
	//用户升级
	void userLevelup(UINT32 curTime,std::string account,UINT32 charID,UINT16 level);
	//用户教程
	void userGuide(UINT32 curTime,std::string account,UINT32 charID,UINT16 level,UINT16 stepID);
	//用户充值
	void recharge(UINT32 curTime,std::string account,UINT32 charID,UINT16 level,std::string payID,std::string orderID,UINT32 amount,UINT32 coin,UINT32 totalRecharge);
	//用户消费
	void consume(UINT32 curTime,std::string account,UINT32 charID,UINT16 level,UINT16 type,UINT32 coin,UINT32 currentCoin);	
private :
	Log4cxxHelp* m_pLogger;
};
#endif
