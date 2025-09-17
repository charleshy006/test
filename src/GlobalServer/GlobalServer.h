#ifndef __GLOBALSERVER_H__
#define __GLOBALSERVER_H__
#include <list>
#include <vector>
#include <string>
#include "GlobalTimerCB.h"
#include "UtilFunction.h"
#include "net.h"
#include "TimerMgr.h"
#include "tcpcomponent.h"
#include "ServerBase.h"
using namespace net;

class BillPacketHandler : public IPacketHandler
{
public :
	bool handlePacket(IOComponent * pConn, Packet *packet);
};

class GlobalServer : public ServerBase
{
public:
	//构造
    GlobalServer();
	//析构
    ~GlobalServer();
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
	//根据charID获得pubcharID
	UINT32 getPubCharIDByID(UINT32 charID)
	{
		return m_pubServerIndex << 22 | charID << 10 >> 10 ;
	}
	UINT32 getPubServerIndex(){return m_pubServerIndex;}
	UINT32 getAreaCode()
	{
		    return m_areaCode;
	}
private:
	CNetConnMgr  m_netConnMgr;
	ConnectionMgr m_connMgr;
	std::pair<UINT16,UINT16> m_zoneRange;		//区范围

	UINT32      m_pubServerIndex;   //本服在pubserver中的序号
	UINT32      m_areaCode;         //区服ID
};

#define g_databaseEngine  CMysqlDatabaseMgr::instance()
#endif
