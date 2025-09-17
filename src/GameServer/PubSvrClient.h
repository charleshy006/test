#ifndef __H_PUB_SVR_CLIENT_H__
#define __H_PUB_SVR_CLIENT_H__
#include "net.h"
#include "TypesDef.h"
#include "Singleton.h"

using namespace net;

class PubSvrClientMgr : public ReconnMgr
{
	USE_SINGLETON_NEW(PubSvrClientMgr);
	private :
		//构造
		PubSvrClientMgr();
	public :
		//析构
		~PubSvrClientMgr();
		//获得实例
		static PubSvrClientMgr  & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		//发送
		virtual bool sendMyInfo(TCPComponent * pConn);

		virtual void onDisConnect(TCPComponent  *pTcpCom);

		virtual void onConnectSvrSucess(TCPComponent  *pTcpCom);

		template <class T>
		bool broadcast(const T & cmd);
	private :
		// TCPComponent * m_pConn;
		// bool	m_needSendMyInfo;
		bool  m_readyToSend;
};

template <class T>
bool PubSvrClientMgr::broadcast(const T & cmd)
{
    if (m_readyToSend)
    {
    	return ReconnMgr::broadcastProto2All(cmd);
    }
    return false;
}

#endif
