#include "LoginGlobalModHandle.h"
#include "SvrInfo.h"
#include "GlobalServer.h"
#include "ActivityConfigMgr.h"
#include "HistoryRankMgr.h"
#include "ServerConfig.h"
#include "OpenServerInfo.h"

extern GlobalServer * g_globalServer;

LoginGlobalModHandle::LoginGlobalModHandle()
{
	registerHandle(&LoginGlobalModHandle::loginGlobal);
}

LoginGlobalModHandle::~LoginGlobalModHandle()
{

}

bool LoginGlobalModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<LoginGlobalModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void LoginGlobalModHandle::loginGlobal(TCPComponent * pCom,Global::stLoginGlobal * loginCmd)
{
	if (!loginCmd) {
        Athena::logger->error("gl error return!");
		return ;
	}

        Athena::logger->error("gl loginCmd info:");
            Athena::logger->error("gl svrType=%u", loginCmd->svrtype());
                Athena::logger->error("gl svrID=%u", loginCmd->svrid());
                    Athena::logger->error("gl ip=%s", loginCmd->ip().c_str());
                        Athena::logger->error("gl port=%u", loginCmd->port());

	SvrInfo * p = g_globalServer->verifyServerInfo(loginCmd->svrtype(),loginCmd->svrid(),loginCmd->ip().c_str(),loginCmd->port());
	if (p) {
		if (g_globalServer->getConnMgr().verifyComponent(pCom,*p)){
			Athena::logger->trace("[客户端连接验证通过]svrType=%u,svrID=%u,ip=%s, port=%u",loginCmd->svrtype(),loginCmd->svrid(),loginCmd->ip().c_str(),loginCmd->port());
			if (GAMESERVER == loginCmd->svrtype()) {//gameserver启动了
				Global::stGameSvrStartUp retCmd;
				retCmd.set_svrid(loginCmd->svrid());
				g_globalServer->getConnMgr().broadcastByType(GATESERVER,retCmd);
				CActivityConfigMgr::getSingleton().sendActivityConfig(loginCmd->svrid());
				CHistoryRankMgr::getSingleton().broadcastRankInfo();
				CServerConfigMgr::getSingleton().notifyGame();
				COpenServerMgr::getSingleton().sendOpenServerInfo();
			}
			else if (GATESERVER == loginCmd->svrtype()){
				CServerConfigMgr::getSingleton().notifyGate();
			}
			else if (WEBSERVER == loginCmd->svrtype()){
                Global::stReqLimitPetActivityData retCmd;
                retCmd.set_msg(std::string("global"));
                g_globalServer->getConnMgr().broadcastByType(WEBSERVER, retCmd);
                Athena::logger->trace("web server启动了");
			}
		}
		return ;
	}
	else {
		Athena::logger->error("连接验证失败!");
		return;
	}
	return;
}
