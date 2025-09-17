#ifndef __H_GLOBAL_SVR_CLIENT_H__
#define __H_GLOBAL_SVR_CLIENT_H__
#include "net.h"
#include "TypesDef.h"
#include "Singleton.h"

using namespace net;

class GlobalSvrClientMgr : public ReconnMgr
{
	USE_SINGLETON_NEW(GlobalSvrClientMgr);
	private :
		//构造
		GlobalSvrClientMgr();
	public :
		//析构
		~GlobalSvrClientMgr();
		//获得实例
		static GlobalSvrClientMgr  & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		//发送
		bool sendMyInfo(TCPComponent * pConn);
};
#endif
