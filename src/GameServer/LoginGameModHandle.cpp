#include "LoginGameModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"

extern GameServer * g_gameServer;

LoginGameModHandle::LoginGameModHandle()
{
	registerHandle(&LoginGameModHandle::loginGame);
}

LoginGameModHandle::~LoginGameModHandle()
{

}

bool LoginGameModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<LoginGameModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void LoginGameModHandle::loginGame(TCPComponent * pCom,Game::stLoginGame * loginCmd)
{
	if (!loginCmd) {
		Athena::logger->error("stLoginGame error!");
		return ;
	}

	Athena::logger->error("game loginCmd info:");
	Athena::logger->error("game svrType=%u", loginCmd->svrtype());
	Athena::logger->error("game svrID=%u", loginCmd->svrid());
	Athena::logger->error("game ip=%s", loginCmd->ip().c_str());
	Athena::logger->error("game port=%u", loginCmd->port());


	SvrInfo * p = g_gameServer->verifyServerInfo(loginCmd->svrtype(),loginCmd->svrid(),loginCmd->ip().c_str(),loginCmd->port());
	if (p) {
		if (g_gameServer->getConnMgr().verifyComponent(pCom,*p)){
			Athena::logger->trace("[客户端连接验证通过]svrType=%u,svrID=%u,ip=%s, port=%u",loginCmd->svrtype(),loginCmd->svrid(),loginCmd->ip().c_str(),loginCmd->port());
		}
		return ;
	}
	else {
		Athena::logger->error("连接验证失败!");
		return;
	}
	return;
}
