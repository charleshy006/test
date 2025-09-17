#include "GlobalSvrClient.h"
#include "GameServer.h"
#include "Global.pb.h"

extern GameServer * g_gameServer;

GlobalSvrClientMgr::GlobalSvrClientMgr()
{
	
}

GlobalSvrClientMgr::~GlobalSvrClientMgr()
{
}

bool GlobalSvrClientMgr::sendMyInfo(TCPComponent * pConn)
{
	Global::stLoginGlobal  cmd;
	cmd.set_svrid(g_gameServer->m_svrInfo.m_svrID);
	cmd.set_svrtype(GAMESERVER);
	cmd.set_port(g_gameServer->m_svrInfo.m_port);
	cmd.set_ip(g_gameServer->m_svrInfo.m_ip);
	SEND_PROTO_2_CLIENT(pConn,cmd);
	return true;
}

