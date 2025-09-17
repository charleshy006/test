#ifndef __UPDATESERVER_H__
#define __UPDATESERVER_H__
#include <list>
#include <vector>
#include <string>
#include "UpdateTimerCB.h"
#include "UtilFunction.h"
#include "net.h"
#include "TimerMgr.h"
#include "tcpcomponent.h"
#include "ServerBase.h"
using namespace net;
class UpdateServer : public ServerBase
{
public:
	//构造
    UpdateServer();
	//析构
    ~UpdateServer();
	//启动服务器
    bool start();
	//停止服务器
    void stop();
	//获得连接管理器
	ConnectionMgr  & getConnMgr() {  return m_connMgr ; }
	//重新加载
	void setReloadCfg(bool bReloadCfg)
	{
		m_bReloadCfg = bReloadCfg;
	}
	//是否需要重新加载
	bool isReloadCfg()
	{
		return m_bReloadCfg;
	}
private:
	CNetConnMgr  m_netConnMgr;
	ConnectionMgr m_connMgr;
	bool		m_bReloadCfg;
};

extern UpdateServer *g_updateServer;
#define g_databaseEngine  CMysqlDatabaseMgr::instance()
#endif
