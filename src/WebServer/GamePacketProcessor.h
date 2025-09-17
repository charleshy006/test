#ifndef __GAMEPACKETPROCESSOR_H__
#define __GAMEPACKETPROCESSOR_H__
#include "SvrInfo.h"
#include "net.h"
#include "Singleton.h"
#include "ProtoMsgHandle.h"
#include <set>
#include <vector>
using namespace net;

//GameServer的连接客户端
class GamePacketProcessor : public PacketProcessorBase 
{
	USE_SINGLETON_NEW(GamePacketProcessor);
public:
	static GamePacketProcessor & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	GamePacketProcessor();
	virtual ~GamePacketProcessor();
	bool processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	void disConnnectPacketCome(UINT16 svrID);
private :
	std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};
#endif
