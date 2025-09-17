#ifndef __PACKETPROCESSOR_H__
#define __PACKETPROCESSOR_H__
#include "Time.h"
#include "SvrInfo.h"
#include "net.h"
#include "tcpcomponent.h"
#include "Singleton.h"
#include "WebServer.h"
#include "packetprocessorbase.h"
#include "ProtoMsgHandle.h"
#include <set>
#include <vector>
using namespace net;

class Player;
class PacketProcessor : public PacketProcessorBase
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
	bool processHttpMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	template <class T>
	bool broadcast2Game(const T & cmd);
	template <class T>
	bool broadcast2Gate(const T & cmd);
	void disConnnectPacketCome(UINT16 svrID);
private :
	std::map<UINT8,ProtoMsgHandleBase *> m_protoModHandles;
};

extern WebServer * g_webServer ;

template <class T>
bool PacketProcessor::broadcast2Game(const T & cmd)
{
	return g_webServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

template <class T>
bool PacketProcessor::broadcast2Gate(const T & cmd)
{
	return g_webServer->getConnMgr().broadcastByType(GATESERVER,cmd);
}
#endif
