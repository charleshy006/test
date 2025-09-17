#ifndef __NORMAL2PUBPACKETPROCESSOR_H__
#define __NORMAL2PUBPACKETPROCESSOR_H__
#include "SvrInfo.h"
#include "net.h"
#include "Singleton.h"
#include "ProtoMsgHandle.h"
#include <set>
#include <vector>
using namespace net;

//NormalGameServer -> PubGameServer
class Normal2PubPacketProcessor : public PacketProcessorBase 
{
	USE_SINGLETON_NEW(Normal2PubPacketProcessor);
public:
	static Normal2PubPacketProcessor & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	Normal2PubPacketProcessor();
	virtual ~Normal2PubPacketProcessor();
	bool processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	void disConnnectPacketCome(UINT16 svrID);
private :
	std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};
#endif

