#ifndef __H_GAMESVR_CLIENTMGR_H__
#define __H_GAMESVR_CLIENTMGR_H__
#include "Singleton.h"
#include "TypesDef.h"
#include "net.h"

#include <set>
using namespace net;

class GameSvrClientMgr : public ReconnMgr
{
	USE_SINGLETON_NEW(GameSvrClientMgr);
	public :
		//析构
		~GameSvrClientMgr();
		//获得实例
		static GameSvrClientMgr & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
	private :
		//构造
		GameSvrClientMgr();
	public :
		//发送自己的信息
		bool sendMyInfo(TCPComponent * pConn);
		//增加一个连接ID
		void addConnectSvrID(UINT16 svrID);
		//是否已经连接了
		bool isConnected(UINT16 svrID);
	private :
		//已经连接的服务器ID
		std::set<UINT16> m_connectSvrIDs;
};
#endif
