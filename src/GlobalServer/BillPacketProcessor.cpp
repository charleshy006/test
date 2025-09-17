#include "BillPacketProcessor.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "TblDataMgr.h"
#include "PacketProcessor.h"
#include "GlobalServer.h"
#include "Bill2GlobalHandle.h"
#include "Bill.pb.h"

BillPacketProcessor::BillPacketProcessor()
{
	m_protoModHandles[Bill::GLOBAL2BILL_MOD] = ATHENA_NEW Bill2GlobalHandle();
}

BillPacketProcessor::~BillPacketProcessor()
{
}

extern GlobalServer * g_globalServer;

void BillPacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	Athena::logger->trace("收到了global(svrID=%d)断开的协议,服务器马上关闭",svrID);
	//g_globalServer->stopTimer();
}

bool BillPacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
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
