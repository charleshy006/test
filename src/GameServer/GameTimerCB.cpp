#include <iostream>
#include <string>
#include "GameTimerCB.h"
#include "PacketProcessor.h"
#include "GameServer.h"
#include "Log4cxxHelp.h"
#include "GlobalPacketProcessor.h"
#include "Pub2NormalPacketProcessor.h"
#include "Normal2PubPacketProcessor.h"
#include "MapMgr.h"
#include "GameUserManager.h"
#include "SqlOperation.h"
#include "MysqlDatabase.h"
#include "SqlDelayThreadPool.h"
#include "HttpClient.h"
#ifdef _USE_BEHAVIAC_AI
#include "BehaviacAI.h"
#endif
//#include "google/malloc_extension.h"

GameTimerCB::GameTimerCB(UINT64 _interval) : stTimerCB(_interval)
{
	//#ifdef _HDZ_DEBUG
	if (g_gameServer->isLoadStat()) {
		m_loadStat.setPrintMaxVal(10);
		m_loadStat(std::string("PacketProcessor::processAllBufferMsg"))
		(std::string("GlobalClientMgr::processAllBufferMsg"))
		(std::string("CMapMgr::timerUpdate"));
	}
	//#endif
}

bool GameTimerCB::exec(RealTime & curTime)
{
	if (g_gameServer->isPubGameServer())
	{
		Normal2PubPacketProcessor::getSingleton().processAllPacket();
	}else
	{
//#ifdef _HDZ_DEBUG
		if (g_gameServer->isLoadStat()) {
			m_loadStat.init();
			m_loadStat.begin("PacketProcessor::processAllBufferMsg");
		}
//#endif
		CHttpClient::getSingleton().processResponse(curTime.sec());
		PacketProcessor::getSingleton().processAllPacket();
//#ifdef _HDZ_DEBUG
	
		if (g_gameServer->isLoadStat()) {
			m_loadStat.end("PacketProcessor::processAllBufferMsg");

			m_loadStat.begin("GlobalPacketProcessor::processAllBufferMsg");
		}
//#endif
		GlobalPacketProcessor::getSingleton().processAllPacket();	
//#ifdef _HDZ_DEBUG
		if (g_gameServer->isLoadStat()) {
			m_loadStat.end("GlobalPacketProcessor::processAllBufferMsg");
		}
//#endif
		Pub2NormalPacketProcessor::getSingleton().processAllPacket();
	}

//#ifdef _HDZ_DEBUG
	if (g_gameServer->isLoadStat()) {
		m_loadStat.begin("CMapMgr::timerUpdate");
	}
//#endif

	CMapMgr::getSingleton().timerUpdate(curTime.msecs());
//#ifdef _HDZ_DEBUG
	if (g_gameServer->isLoadStat()) {
		m_loadStat.end("CMapMgr::timerUpdate");
	}
#ifdef _USE_BEHAVIAC_AI
	if (g_gameServer->isLoadStat()){
		m_loadStat.begin("CBehaviacAIMgr::timer");
	}

	CBehaviacAIMgr::getSingleton().timer(curTime.sec());

	if (g_gameServer->isLoadStat()){
		m_loadStat.end("CBehaviacAIMgr::timer");
	}
#endif

//#endif
//#ifdef _HDZ_DEBUG
	if (g_gameServer->isLoadStat()) {
		m_loadStat.print();
	}
//#endif
	return true;
}

bool GameTimer1SecCB::exec(RealTime & curTime)
{
	struct tm curTm;
	RealTime::getLocalTime(curTm,curTime.sec());
	if ((curTm.tm_hour == 0)
			&& (curTm.tm_min == 0)
			&& (curTm.tm_sec < 10)) {
		if (!m_zeroRefresh) {
			//MallocExtension::instance()->ReleaseFreeMemory();
			m_zeroRefresh = true;
			Athena::logger->trace("tcmalloc 进行了内存回收!");
		}
	}
	else {
		m_zeroRefresh = false;	
	}
	GameUserManager::getSingleton().timer(curTime.sec());
	return true;
}
