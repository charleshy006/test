#include "PacketProcessor.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "Chat.h"
#include "Map.h"
#include "MapMgr.h"
#include "GlobalPacketProcessor.h"
#include "GlobalSvrClient.h"
#include "TblDataMgr.h"
#include "Game.pb.h"
#include "LoginGameModHandle.h"
#include "Gate2GameModHandle.h"
#include "WebModHandle.h"

extern GameServer * g_gameServer;

PacketProcessor::PacketProcessor()
{
	m_protoModHandles[Game::LOGIN_MOD] = ATHENA_NEW LoginGameModHandle();
	m_protoModHandles[Game::GATE2GAME_MOD] = ATHENA_NEW Gate2GameModHandle();
	m_protoModHandles[Game::WEBGAME_MOD] = ATHENA_NEW WebModHandle();
}

PacketProcessor::~PacketProcessor()
{
}

extern GameServer * g_gameServer ;

void PacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	GameUserManager::getSingleton().removeUserByGate(svrID);
	Athena::logger->trace("收到了连接断开的协议svrID=%d",svrID);
}

bool PacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
#ifdef _HDZ_DEBUG
	if (g_gameServer->isTraceProto()) {
		Athena::logger->trace("收到的网关协议modID=%d,funID=%d",header->m_modID,header->m_funID);
	}
#endif

	if (PacketProcessorBase::processOneBufferMsg(pConn,header,pNull)) {
		return true;
	}
	
	ProtoMsgHandleBase * pModHandle = m_protoModHandles[header->m_modID];
	if (pModHandle) {
		if (pModHandle->handle(pConn,(char *)pNull,header->m_dataLen,header->m_funID)){
			return true;
		}   
	}   
	Athena::logger->error("%s (%u, %u, %u)", __PRETTY_FUNCTION__,header->m_modID,header->m_funID,header->m_dataLen);
	return false;
}
