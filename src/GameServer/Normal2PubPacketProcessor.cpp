#include "GlobalPacketProcessor.h"
#include "GameServer.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "TblDataMgr.h"
#include "GameUser.h"
#include "GameUserManager.h"
#include "Map.h"
#include "MapMgr.h"
#include "Normal2PubPacketProcessor.h"
#include "PubSvrClient.h"
#include "PubGame.pb.h"
#include "Game.pb.h"
#include "Normal2PubModHandle.h"

Normal2PubPacketProcessor::Normal2PubPacketProcessor()
{
	m_protoModHandles[PubGame::NORMAL2PUB_MOD]  = ATHENA_NEW Normal2PubModHandle();
}

Normal2PubPacketProcessor::~Normal2PubPacketProcessor()
{
}

extern GameServer * g_gameServer;

void Normal2PubPacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	if (svrID > 0)
	{
		Athena::logger->trace("收到了normalServer(svrID=%d)断开的协议",svrID);
	}
}

bool Normal2PubPacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
	if (PacketProcessorBase::processOneBufferMsg(pConn,header,pNull)) {
		return true;
	}
	ProtoMsgHandleBase * pModHandle = m_protoModHandles[header->m_modID];
	if (pModHandle) {
		if (pModHandle->handle(pConn,(char *)pNull,header->m_dataLen,header->m_funID)){
			return true;
		}
	}
	Athena::logger->error("%s (%u, %u, %u)", __PRETTY_FUNCTION__, header->m_modID,header->m_funID,header->m_dataLen);
	return false;
}
