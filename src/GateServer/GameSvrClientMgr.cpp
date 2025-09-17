#include "GameSvrClientMgr.h"
#include "GateServer.h"
#include "Game.pb.h"

extern GateServer * g_gateServer;

GameSvrClientMgr::GameSvrClientMgr()
{

}

GameSvrClientMgr::~GameSvrClientMgr()
{
	
}

bool GameSvrClientMgr::sendMyInfo(TCPComponent * pConn)
{
	Game::stLoginGame cmd;
	cmd.set_svrid(g_gateServer->m_svrInfo.m_svrID);
	cmd.set_svrtype(GATESERVER);
	cmd.set_port(g_gateServer->m_svrInfo.m_port);
	cmd.set_ip(g_gateServer->m_svrInfo.m_ip);
	SEND_PROTO_2_CLIENT(pConn,cmd);
	return true;
}

void GameSvrClientMgr::addConnectSvrID(UINT16 svrID)
{
	m_connectSvrIDs.insert(svrID);	
}

bool GameSvrClientMgr::isConnected(UINT16 svrID)
{
	if (m_connectSvrIDs.find(svrID) == m_connectSvrIDs.end()){
		return false;
	}
	return true;
}
