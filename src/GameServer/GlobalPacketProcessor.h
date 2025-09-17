#ifndef __GLOBALPACKETPROCESSOR_H__
#define __GLOBALPACKETPROCESSOR_H__
#include "SvrInfo.h"
#include "net.h"
#include "Singleton.h"
#include "ProtoMsgHandle.h"
#include <set>
#include <vector>
using namespace net;

//GlobalServer的连接客户端
class GlobalPacketProcessor : public PacketProcessorBase 
{
	USE_SINGLETON_NEW(GlobalPacketProcessor);
public:
	static GlobalPacketProcessor & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	GlobalPacketProcessor();
	virtual ~GlobalPacketProcessor();
	bool processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	void disConnnectPacketCome(UINT16 svrID);
private :
	std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};
#endif

