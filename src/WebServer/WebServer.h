#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__
#include <list>
#include <vector>
#include <string>
#include "WebTimerCB.h"
#include "UtilFunction.h"
#include "net.h"
#include "TimerMgr.h"
#include "tcpcomponent.h"
#include "ServerBase.h"

using namespace net;
class GlobalPacketHandler : public IPacketHandler
{
 public :
	bool handlePacket(IOComponent * pConn, Packet *packet);
};

class GamePacketHandler : public IPacketHandler
{
public :
	bool handlePacket(IOComponent * pConn, Packet *packet);
};

class WebServer : public ServerBase
{
public:
	//构造
    WebServer();
	//析构
    ~WebServer();
	//启动服务器
    bool start();
	//停止服务器
    void stop();
	//获得连接管理器
	ConnectionMgr  & getConnMgr() {  return m_connMgr ; }
	//区范围
	std::pair<UINT16,UINT16> getZoneRange() { return m_zoneRange ; }
	//加载所有客户端
	bool loadAllClient();
private:
	CNetConnMgr  m_netConnMgr;
	ConnectionMgr m_connMgr;
	std::pair<UINT16,UINT16> m_zoneRange;		//区范围
};

#define g_databaseEngine  CMysqlDatabaseMgr::instance()
#endif
