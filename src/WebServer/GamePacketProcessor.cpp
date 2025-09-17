#include "GamePacketProcessor.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "WebServer.h"
#include "Game2WebModHandle.h"

GamePacketProcessor::GamePacketProcessor()
{
	m_protoModHandles[Game::WEBGAME_MOD]  = ATHENA_NEW Game2WebModHandle();
}

GamePacketProcessor::~GamePacketProcessor()
{
}

extern WebServer * g_webServer;

void GamePacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	SvrInfo * pSvrInfo =  g_webServer->getSvrInfoByID(svrID);
	if (!pSvrInfo){
		return ;
	}
	Athena::logger->trace("收到了game(svrID=%d)断开的协议,服务器马上关闭",svrID);
	g_webServer->stopTimer();
}

bool GamePacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
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
	Athena::logger->error("%s (%u, %u, %u)", __PRETTY_FUNCTION__,header->m_modID,header->m_funID,header->m_dataLen);
	return false;
}
