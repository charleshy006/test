#include "PubSvrClient.h"
#include "GameServer.h"
#include "PubGame.pb.h"

extern GameServer * g_gameServer;

PubSvrClientMgr::PubSvrClientMgr()
{
	m_readyToSend = false;
}

PubSvrClientMgr::~PubSvrClientMgr()
{
}

void PubSvrClientMgr::onConnectSvrSucess(TCPComponent  *pTcpCom)
{
	ReconnMgr::onConnectSvrSucess(pTcpCom);
	m_readyToSend = true;
	Athena::logger->trace("PubSvrClientMgr::onConnectSvrSucess");
}

void PubSvrClientMgr::onDisConnect(TCPComponent  *pTcpCom)
{
	ReconnMgr::onDisConnect(pTcpCom);
	m_readyToSend = false;
	Athena::logger->trace("PubSvrClientMgr::onDisConnect");
}

bool PubSvrClientMgr::sendMyInfo(TCPComponent * pConn)
{
	Athena::logger->trace("PubSvrClientMgr::sendMyInfo");
	PubGame::stLoginPubGame  cmd;
	cmd.set_svrid(g_gameServer->getPubServerIndex());
	cmd.set_svrtype(GAMESERVER);
	cmd.set_port(g_gameServer->m_svrInfo.m_port);
	cmd.set_ip(g_gameServer->m_svrInfo.m_ip);
	cmd.set_key(g_gameServer->getPubServerKey()); 
	SEND_PROTO_2_CLIENT(pConn,cmd);
	return true;
}

