#include "Pub2NormalPacketProcessor.h"
#include "GameServer.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "TblDataMgr.h"
#include "GameUser.h"
#include "GameUserManager.h"
#include "Map.h"
#include "MapMgr.h"
#include "PacketProcessor.h"
#include "GlobalSvrClient.h"
#include "Global.pb.h"
#include "PubGame.pb.h"
#include "Pub2NormalModHandle.h"

Pub2NormalPacketProcessor::Pub2NormalPacketProcessor()
{
	m_protoModHandles[PubGame::PUB2NORMAL_MOD]  = ATHENA_NEW Pub2NormalModHandle();
}

Pub2NormalPacketProcessor::~Pub2NormalPacketProcessor()
{
}

extern GameServer * g_gameServer;

void Pub2NormalPacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	Athena::logger->trace("收到了pubServer(svrID=%d)断开的协议",svrID);
}

bool Pub2NormalPacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
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
