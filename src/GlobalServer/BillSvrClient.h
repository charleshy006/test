#ifndef __H_BILL_SVR_CLIENT_H__
#define __H_BILL_SVR_CLIENT_H__
#include "net.h"
#include "TypesDef.h"
#include "Singleton.h"

using namespace net;

class BillSvrClientMgr : public ReconnMgr
{
	USE_SINGLETON_NEW(BillSvrClientMgr);
	private :
		//构造
		BillSvrClientMgr();
	public :
		//析构
		~BillSvrClientMgr();
		//获得实例
		static BillSvrClientMgr  & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		//发送
		bool sendMyInfo(TCPComponent * pConn);
};
#endif
