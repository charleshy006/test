#include <iostream>
#include <string>
#include "GateTimerCB.h"
#include "Log4cxxHelp.h"
#include "GateServer.h"
#include "PacketProcessor.h"
#include "GlobalPacketProcessor.h"
#include "GamePacketProcessor.h"
#include "GateUserManager.h"
#include "SelectUserManager.h"

extern GateServer * g_gateServer;

GateTimerCB::GateTimerCB(UINT64 _interval) : stTimerCB(_interval)
{
	//#ifdef _HDZ_DEBUG
	if (g_gateServer->isLoadStat()) {
		m_loadStat.setPrintMaxVal(10);
		m_loadStat(std::string("PacketProcessor::processAllBufferMsg"))
		(std::string("PacketProcessor::processAllBufferMsg"))
		(std::string("GlobalPacketProcessor::processAllBufferMsg"))
		(std::string("GamePacketProcessor::processAllBufferMsg"));
	}
	//#endif
}

bool GateTimerCB::exec(RealTime & curTime)
{
//#ifdef _HDZ_DEBUG
	if (g_gateServer->isLoadStat()) {
		m_loadStat.init();
		m_loadStat.begin("PacketProcessor::processAllBufferMsg");
	}
//#endif
	PacketProcessor::getSingleton().processAllPacket();
//#ifdef _HDZ_DEBUG
	if (g_gateServer->isLoadStat()) {
		m_loadStat.end("PacketProcessor::processAllBufferMsg");

		m_loadStat.begin("GlobalPacketProcessor::processAllBufferMsg");
	}
//#endif
	GlobalPacketProcessor::getSingleton().processAllPacket();	
//#ifdef _HDZ_DEBUG
	if (g_gateServer->isLoadStat()) {
		m_loadStat.end("GlobalPacketProcessor::processAllBufferMsg");

		m_loadStat.begin("GamePacketProcessor::processAllBufferMsg");
	}
//#endif
	GamePacketProcessor::getSingleton().processAllPacket();
//#ifdef _HDZ_DEBUG
	if (g_gateServer->isLoadStat()) {
		m_loadStat.end("GamePacketProcessor::processAllBufferMsg");
	}
//#endif

//#ifdef _HDZ_DEBUG
	if (g_gateServer->isLoadStat()){
		m_loadStat.print();
	}
//#endif
	GateUserManager::getSingleton().timer();
	SelectUserManager::getSingleton().checkTime(curTime.sec());

	if(g_gateServer->isStatProto()){
		g_gateServer->m_protoStat.print(curTime);
	}
	return true;
}

bool GateTimerCB10Sec::exec(RealTime & curTime)
{
	return true;
}

bool GateTimerCB30Sec::exec(RealTime & curTime)
{
	return true;
}

