#include "Gate2GameModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "MapModHandle.h"
#include "Map.pb.h"
#include "RoleModHandle.h"
#include "MiscModHandle.h"
#include "GuildModHandle.h"
#include "TaskModHandle.h"
#include "SortListModHandle.h"
#include "TeamModHandle.h"

extern GameServer * g_gameServer;

Gate2GameModHandle::Gate2GameModHandle()
{
	m_protoModHandles[Proto::MAP_MOD] =  ATHENA_NEW MapModHandle();
	m_protoModHandles[Proto::ROLE_MOD] = ATHENA_NEW RoleModHandle();
	m_protoModHandles[Proto::MISC_MOD]  = ATHENA_NEW MiscModHandle();
	m_protoModHandles[Proto::MISC2_MOD]  = ATHENA_NEW MiscModHandle();
	m_protoModHandles[Proto::GUILD_MOD] = ATHENA_NEW GuildModHandle();
	m_protoModHandles[Proto::TASK_MOD] = ATHENA_NEW TaskModHandle();
	m_protoModHandles[Proto::SORTLIST_MOD] = ATHENA_NEW SortListModHandle();
	m_protoModHandles[Proto::TEAM_MOD] = ATHENA_NEW TeamModHandle();

	registerHandle(&Gate2GameModHandle::loginOut);
	registerHandle(&Gate2GameModHandle::gateForwardGame);
}

Gate2GameModHandle::~Gate2GameModHandle()
{

}

bool Gate2GameModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<Gate2GameModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void Gate2GameModHandle::loginOut(TCPComponent * pCom,Game::stKillOutUser * revCmd)
{
	if (!pCom || !revCmd) {
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(revCmd->charid());
	if (pUser){
		GameUserManager::getSingleton().removeUser(pUser);
		pUser->unreg();
		SAFE_DELETE(pUser);
	}
}

void Gate2GameModHandle::gateForwardGame(TCPComponent * pCom,Game::stGateForwardGame * recvCmd)
{
	if (!pCom || !recvCmd) {
		return ;
	}
#ifdef _HDZ_DEBUG
	if (g_gameServer->isTraceProto()) {
		Athena::logger->trace("客户端发过来的协议是modID=%d,funID=%d",recvCmd->modid(),recvCmd->funid());
	}
#endif
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		GameProtoMsgHandleBase  * pHandle = m_protoModHandles[recvCmd->modid()];
		if (pHandle) {
			GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
			if (pUser) {
				pHandle->handle(pUser,recvCmd->data().data(),recvCmd->cmdlen(),recvCmd->funid());			
			}
		}
	}
}
