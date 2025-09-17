#include "GlobalPacketProcessor.h"
#include "WebServer.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "TblDataMgr.h"
#include "PacketProcessor.h"
#include "GlobalSvrClient.h"
#include "Global.pb.h"
#include "Global2WebModHandle.h"

GlobalPacketProcessor::GlobalPacketProcessor()
{
	m_protoModHandles[Global::WEB_MOD] = ATHENA_NEW Global2WebModHandle();
}

GlobalPacketProcessor::~GlobalPacketProcessor()
{
}

extern WebServer * g_webServer;

void GlobalPacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	Athena::logger->trace("收到了global(svrID=%d)断开的协议,服务器马上关闭",svrID);
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
