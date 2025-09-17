#include "GamePacketProcessor.h"
#include "GateServer.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "TblDataMgr.h"
#include "GateUserManager.h"
#include "SelectUserManager.h"
#include "Game.pb.h"
#include "GameGateModHandle.h"

GamePacketProcessor::GamePacketProcessor()
{
	m_protoModHandles[Game::GATE2GAME_MOD] = 	ATHENA_NEW GameGateModHandle();
}

GamePacketProcessor::~GamePacketProcessor()
{
}

extern GateServer * g_gateServer;

void GamePacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	SvrInfo * pSvrInfo =  g_gateServer->getSvrInfoByID(svrID);
	if (!pSvrInfo){
		return ;
	}

	if (pSvrInfo && (GAMESERVER == pSvrInfo->m_svrType)) {//gameserver关闭,不需要关闭网关
		return ;		
	}
	Athena::logger->trace("收到了game(svrID=%d)断开的协议,服务器马上关闭",svrID);
	g_gateServer->stopTimer();
}

bool GamePacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
	if (PacketProcessorBase::processOneBufferMsg(pConn,header,pNull)) {
		return true;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到来自game的协议:modID=%d,funID=%d",header->m_modID,header->m_funID);
#endif
	ProtoMsgHandleBase * pModHandle = m_protoModHandles[header->m_modID];
	if (pModHandle) {
		if (pModHandle->handle(pConn,(char *)pNull,header->m_dataLen,header->m_funID)){
			return true;
		}
	}
	Athena::logger->error("%s (%u, %u, %u)", __PRETTY_FUNCTION__,header->m_modID,header->m_funID,header->m_dataLen);
	return false;
}
