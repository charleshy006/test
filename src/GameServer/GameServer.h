#ifndef  __GAMESERVER_H__
#define  __GAMESERVER_H__
#include <vector>
#include <string>
#include "SvrInfo.h"
#include "Log4cxxHelp.h"
#include "TimerMgr.h"
#include "net.h"
#include "tcpcomponent.h"
#include "ServerBase.h"
#include <map>

using namespace net;
class GlobalPacketHandler : public IPacketHandler
{
	public :
		bool handlePacket(IOComponent * pConn, Packet *packet);
};

class GameServer : public ServerBase
{
public:
	//构造函数 
    GameServer();
	//析构函数
    ~GameServer();
	//开始
    virtual bool start();
	//结束
    virtual void stop();
	//加载所有客户端
	virtual	bool loadAllClient();
	//启动监听
	virtual bool startListen();
	//创建定时器
	virtual void createTimer();
	//额外加载
	virtual bool extraLoad();
	//获得连接管理器
	ConnectionMgr  & getConnMgr() {  return m_connMgr ; }
	//是否跟踪协议包
	bool isTraceProto() {
	 	return m_traceProto;
	}
	//获得开服天数
	UINT32 getOpenServerDay(){ return m_openServerDay ;}
	//设置开服天数
	void setOpenServerDay(UINT32 day) {  m_openServerDay = day; }
	//是否创建测试角色
	bool isCreateTestRole()
	{
		return m_bCreateTestRole;
	}
	virtual bool isPubGameServer()
	{
		return false;
	}
	UINT32 getPubServerIndex()
	{
		return m_pubServerIndex;
	}
	std::string getPubServerKey()
	{
		return m_pubServerKey;
	}
	UINT32 getAreaCode()
	{
		return m_areaCode;
	}
protected:
	CNetConnMgr  m_netConnMgr;
	ConnectionMgr m_connMgr;
	bool		m_traceProto;		//跟踪协议
	UINT32		m_openServerDay;	//开服天数
	bool		m_bCreateTestRole;	//是否创建测试账号

	UINT32		m_pubServerIndex;	//本服在pubserver中的序号
	std::string		m_pubServerKey;		//登录pubserver所用key

	UINT32		m_areaCode;			//区服ID
};
#define g_databaseEngine  CMysqlDatabaseMgr::instance()
#endif
