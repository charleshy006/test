#ifndef __PUB2NORMALPACKETPROCESSOR_H__
#define __PUB2NORMALPACKETPROCESSOR_H__
#include "SvrInfo.h"
#include "net.h"
#include "Singleton.h"
#include "ProtoMsgHandle.h"
#include <set>
#include <vector>
using namespace net;

//GlobalServer的连接客户端
class Pub2NormalPacketProcessor : public PacketProcessorBase 
{
	USE_SINGLETON_NEW(Pub2NormalPacketProcessor);
public:
	static Pub2NormalPacketProcessor & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	Pub2NormalPacketProcessor();
	virtual ~Pub2NormalPacketProcessor();
	bool processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	void disConnnectPacketCome(UINT16 svrID);
private :
	std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};
#endif

