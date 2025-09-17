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
#include "PacketProcessor.h"
#include "GlobalSvrClient.h"
#include "Global.pb.h"
#include "Game.pb.h"
#include "Global2GameModHandle.h"

GlobalPacketProcessor::GlobalPacketProcessor()
{
	m_protoModHandles[Global::GAME_MOD]  = ATHENA_NEW Global2GameModHandle();
}

GlobalPacketProcessor::~GlobalPacketProcessor()
{
}

extern GameServer * g_gameServer;

void GlobalPacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	Athena::logger->trace("收到了global(svrID=%d)断开的协议,服务器马上关闭",svrID);
	g_gameServer->stopTimer();
}

bool GlobalPacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
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
