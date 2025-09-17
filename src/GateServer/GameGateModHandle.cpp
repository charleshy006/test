#include "GameGateModHandle.h"
#include "SvrInfo.h"
#include "GateServer.h"
#include "GateUserConnMgr.h"
#include "GateUserConnMgr.h"
#include "GateUserManager.h"
#include "SelectUserManager.h"
#include "Game.pb.h"
#include "GlobalSvrClientMgr.h"
#include "Role.pb.h"

extern GateServer * g_gateServer;

GameGateModHandle::GameGateModHandle()
{
	registerHandle(&GameGateModHandle::forwardProto2Client);
	registerHandle(&GameGateModHandle::userLoginGameResult);
	registerHandle(&GameGateModHandle::sendProto2AllClient);
	registerHandle(&GameGateModHandle::sendProto2UserList);
	registerHandle(&GameGateModHandle::notifyUserLv);
}

GameGateModHandle::~GameGateModHandle()
{

}

bool GameGateModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<GameGateModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void GameGateModHandle::forwardProto2Client(TCPComponent * pCom,Game::stGameForwardGate * recvCmd)
{
	if (!recvCmd) {
		return ;
	}	
	GateUser * pUser = GateUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return;
	}
	pUser->sendProto(recvCmd->data().data(),recvCmd->cmdlen(),recvCmd->modid(),recvCmd->funid());
}

void GameGateModHandle::userLoginGameResult(TCPComponent * pCom,Game::stUserLoginGameResult * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	
	if (Game::eLoginResult_Login == recvCmd->type()) {
		if (!SelectUserManager::getSingleton().loginGameResult(recvCmd->account(),recvCmd->gameid(),recvCmd->ret())){
			Athena::logger->error("account=%s game返回登录结果错误",recvCmd->account().c_str());
			return;
		}
	}
	else if (Game::eLoginResult_ChgSvr == recvCmd->type()){
		GateUser * pUser = GateUserManager::getSingleton().getUser(recvCmd->charid());	
		if (pUser){
			pUser->setGameID(recvCmd->gameid());
		}
	}
#ifdef  _HDZ_DEBUG
	Athena::logger->trace("(account=%s)收到协议stUserLoginGameSuccess",recvCmd->account().c_str());
#endif
}

void GameGateModHandle::sendProto2AllClient(TCPComponent * pCom,Game::stBroadbastProto2World * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	
	GateUserManager::getSingleton().sendProto2World(recvCmd->data().data(),recvCmd->cmdlen(),recvCmd->modid(),recvCmd->funid());
}

void GameGateModHandle::sendProto2UserList(TCPComponent * pCom,Game::stBroadcastProto2UserList * recvCmd)
{
	if (!recvCmd) {
		return ;
	}

	for (INT32 i = 0;i < recvCmd->userlist_size(); ++i){
		UINT32 charID = recvCmd->userlist(i);
		GateUser * pUser = GateUserManager::getSingleton().getUser(charID);
		if (!pUser){
			continue;
		}
		pUser->sendProto(recvCmd->data().data(),recvCmd->cmdlen(),recvCmd->modid(),recvCmd->funid());
	}
}

void GameGateModHandle::killOutUser(TCPComponent * pCom,Game::stKillOutUser * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	
	GateUser * pUser = GateUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	Athena::logger->trace("GameGateModHandle::killOutUser 玩家:account:%s,id:%d,name:%s被下线",pUser->m_account,pUser->getID(),pUser->getName());
	GateUserConnMgr::getSingleton().disconnectAccount(pUser->m_account);
}

void GameGateModHandle::notifyUserLv(TCPComponent * pCom,Game::stNotifyLevelUp  * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	
	GateUser * pUser = GateUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}

	std::map<UINT32,stGateRoleInfo>::iterator it =  pUser->m_gateRoleInfo.find(recvCmd->charid());

	if (it != pUser->m_gateRoleInfo.end()){
		stGateRoleInfo & roleInfo = it->second;
		roleInfo.m_level = recvCmd->level();
#ifdef _HDZ_DEBUG
		Athena::logger->trace("玩家(charID:%d,name:%s)升级了同步到网关的等级为:%d",roleInfo.m_charID,roleInfo.m_name,roleInfo.m_level);
#endif
	}
}

