#ifndef __H_GLOBAL_SVR_CLIENTMGR_H__
#define __H_GLOBAL_SVR_CLIENTMGR_H__
#include "net.h"
#include "Singleton.h"

using namespace net;

class GlobalSvrClientMgr  : public ReconnMgr
{
	USE_SINGLETON_NEW(GlobalSvrClientMgr);
	private :
		GlobalSvrClientMgr();
	public :
		//析构
		~GlobalSvrClientMgr();
		//获得实例
		static GlobalSvrClientMgr & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		//发送自己的信息
		bool sendMyInfo(TCPComponent * pConn);
};
#endif
