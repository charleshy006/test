#include "GlobalSvrClientMgr.h"
#include "GateServer.h"
#include "Global.pb.h"

GlobalSvrClientMgr::GlobalSvrClientMgr()
{}

GlobalSvrClientMgr::~GlobalSvrClientMgr()
{}

extern GateServer * g_gateServer;

bool GlobalSvrClientMgr::sendMyInfo(TCPComponent * pConn)
{
	Global::stLoginGlobal  cmd;
	cmd.set_svrid(g_gateServer->m_svrInfo.m_svrID);
	cmd.set_svrtype(GATESERVER);
	cmd.set_port(g_gateServer->m_svrInfo.m_port);
	cmd.set_ip(g_gateServer->m_svrInfo.m_ip);
	SEND_PROTO_2_CLIENT(pConn,cmd);
	return true;	
}
