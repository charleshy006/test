#ifndef __BILLPACKETPROCESSOR_H__
#define __BILLPACKETPROCESSOR_H__
#include "SvrInfo.h"
#include "net.h"
#include "Singleton.h"
#include "ProtoMsgHandle.h"
#include <set>
#include <vector>
using namespace net;

//BillServer的连接客户端
class BillPacketProcessor : public PacketProcessorBase 
{
	USE_SINGLETON_NEW(BillPacketProcessor);
public:
	static BillPacketProcessor & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	BillPacketProcessor();
	virtual ~BillPacketProcessor();
	bool processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	void disConnnectPacketCome(UINT16 svrID);
private :
	std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};
#endif

