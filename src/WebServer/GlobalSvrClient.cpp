#include "GlobalSvrClient.h"
#include "WebServer.h"
#include "Global.pb.h"

extern WebServer * g_webServer;

GlobalSvrClientMgr::GlobalSvrClientMgr()
{
	
}

GlobalSvrClientMgr::~GlobalSvrClientMgr()
{
}

bool GlobalSvrClientMgr::sendMyInfo(TCPComponent * pConn)
{
	Global::stLoginGlobal  cmd;
	cmd.set_svrid(g_webServer->m_svrInfo.m_svrID);
	cmd.set_svrtype(WEBSERVER);
	cmd.set_port(g_webServer->m_svrInfo.m_port);
	cmd.set_ip(g_webServer->m_svrInfo.m_ip);
	SEND_PROTO_2_CLIENT(pConn,cmd);
	return true;
}

