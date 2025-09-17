#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__
#include "TypesDef.h"
#include "LoginInfo.h"
#include "Singleton.h"
#include "net.h"
#include <map>

struct stSpecialAccountInfo
{
	std::string m_account;
	UINT32      m_opID;
	std::string m_ip;
	std::string m_device;

	stSpecialAccountInfo()
	{
		m_opID = 0;
	}
};

using namespace net;
class UserManager
{
	USE_SINGLETON_NEW(UserManager)
	public :
		static const UINT16 s_LOGIN_CONN_TIMEOUT = 5;
	private :
		UserManager(){}
	public :
		~UserManager(){}
		static UserManager & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		static void destroyMe()
		{
			THE_SINGLETON::destroy();
		}
		//根据帐号名字跟平台找玩家数据
		bool loginVerify(LoginInfo & info,const char * account,const char * password);
		//创建正式账号
		bool createAccount(const char * account,const char * password,const char * machineID,const char * country);
		//查找正式账号是否存在 
		bool findUserByAccount(LoginInfo & info,const char * account);
		//更新账号信息
		bool updateLoginInfo(LoginInfo & info);
		//增加一个登录连接
		void addLoginConn(NetIO *pConn);
		//删除一个登录连接
		void removeLoginConn(NetIO *pConn);
		//定时检查 
		void timer(UINT32 cur);
		//加载配置
		void load();
		//获得账号信息
		stSpecialAccountInfo * getAccountInfo(std::string account);
	private :
		std::map<NetIO *,UINT32> m_loginConnMap;   // <time,connection*>
		Mutex  m_mutex;
		std::map<std::string,stSpecialAccountInfo>   m_specialAccounts; 
};
#endif
