#ifndef __LOGINSERVER_H__
#define __LOGINSERVER_H__
#include <list>
#include <vector>
#include <string>
#include "LoginTimerCB.h"
#include "UtilFunction.h"
#include "net.h"
#include "ServerBase.h"
#include "ProtoMsgHandle.h"

using namespace net;
class CConnListner : public IConnListner
{
public :
	CConnListner(){}
	~CConnListner() {}
	//连接通知
	void onAddComponent(IOComponent *ioc);
	//端口通知
	void onRemoveComponent(IOComponent *ioc);
};

class LoginServerAdapter : public IPacketHandler , public PacketSender
{
	public :
		//构造
		LoginServerAdapter();
		//处理包
		bool handlePacket(IOComponent * pConn, Packet *packet);
	private :
		std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};

class LoginServer : public ServerBase
{
public:
    LoginServer();
    ~LoginServer();
    bool start();
    void stop();
private:
	CNetConnMgr m_netConnMgr;
};
#define g_databaseEngine  CMysqlDatabaseMgr::instance()
#endif
