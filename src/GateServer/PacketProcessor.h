#ifndef __PACKETPROCESSOR_H__
#define __PACKETPROCESSOR_H__
#include "Time.h"
#include "SvrInfo.h"
#include "net.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "UserLoginModHandle.h"
#include "ProtoMsgHandle.h"
#include <set>
#include <vector>
#include <ext/hash_map>
using namespace net;


class PacketProcessor : public PacketProcessorBase
{
	USE_SINGLETON_NEW(PacketProcessor);
private:
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
	//处理协议
	bool processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	//断开连接的包
	void disConnnectPacketCome(UINT16 svrID);
private :
	std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};
#endif
