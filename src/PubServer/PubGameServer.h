#ifndef  __NORMALGAMESERVER_H__
#define  __NORMALGAMESERVER_H__
#include <vector>
#include <string>
#include "SvrInfo.h"
#include "Log4cxxHelp.h"
#include "TimerMgr.h"
#include "net.h"
#include "tcpcomponent.h"
#include "ServerBase.h"
#include <map>
#include "GameServer.h"

using namespace net;
class PubGameServer : public GameServer
{
public:
	//构造函数 
    PubGameServer();
	//析构函数
    ~PubGameServer();
	//开始
    // virtual bool start();
	//结束
    virtual void stop();
	//加载所有客户端
	virtual bool loadAllClient();
	//启动监听
	virtual bool startListen();
	//创建定时器
	virtual void createTimer();
	//额外加载
	virtual bool extraLoad();
	//获得连接管理器
//	ConnectionMgr  & getConnMgr() {  return m_connMgr ; }
	//是否跟踪协议包
//	bool isTraceProto() {
//	 	return m_traceProto;
//	}
	//获得开服天数
//	UINT32 getOpenServerDay(){ return m_openServerDay ;}
	//设置开服天数
//	void setOpenServerDay(UINT32 day) {  m_openServerDay = day; }
	//是否创建测试角色
//	bool isCreateTestRole()
//	{
//		return m_bCreateTestRole;
//	}
	virtual bool isPubGameServer()
	{
		return true;
	}
private:
//	CNetConnMgr  m_netConnMgr;
//	ConnectionMgr m_connMgr;
//	bool		m_traceProto;		//跟踪协议
//	UINT32		m_openServerDay;	//开服天数
//	bool		m_bCreateTestRole;	//是否创建测试账号
};
// #define g_databaseEngine  CMysqlDatabaseMgr::instance()
#endif
