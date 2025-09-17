#ifndef __PACKETPROCESSOR_H__
#define __PACKETPROCESSOR_H__
#include "Time.h"
#include "SvrInfo.h"
#include "net.h"
#include "GameUser.h"
#include "Singleton.h"
#include "ProtoMsgHandle.h"
#include <set>
#include <vector>
using namespace net;
class Player;
class PacketProcessor  : public PacketProcessorBase
{
	USE_SINGLETON_NEW(PacketProcessor);
public:
	static PacketProcessor & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	PacketProcessor();
	virtual ~PacketProcessor();
	bool processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	void disConnnectPacketCome(UINT16 svrID);
private :
	std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};
#endif
