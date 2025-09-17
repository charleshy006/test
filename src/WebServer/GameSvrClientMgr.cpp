#include "GameSvrClientMgr.h"
#include "WebServer.h"
#include "Game.pb.h"


GameSvrClientMgr::GameSvrClientMgr()
{

}

GameSvrClientMgr::~GameSvrClientMgr()
{
	
}

extern WebServer * g_webServer;

bool GameSvrClientMgr::sendMyInfo(TCPComponent * pConn)
{
	Game::stLoginGame cmd;
	cmd.set_svrid(g_webServer->m_svrInfo.m_svrID);
	cmd.set_svrtype(WEBSERVER);
	cmd.set_port(g_webServer->m_svrInfo.m_port);
	cmd.set_ip(g_webServer->m_svrInfo.m_ip);
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
