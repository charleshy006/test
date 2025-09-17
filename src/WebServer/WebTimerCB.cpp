#include <iostream>
#include <string>
#include "WebTimerCB.h"
#include "SqlDelayThreadPool.h"
#include "PacketProcessor.h"
#include "GlobalPacketProcessor.h"
#include "GamePacketProcessor.h"
#include "GameStaticsMgr.h"
#include "RoleInfoManager.h"
#include "ServerListener.h"

bool WebServerTimerCB::exec(RealTime & curTime)
{
	PacketProcessor::getSingleton().processAllPacket();
	GlobalPacketProcessor::getSingleton().processAllPacket();
	GamePacketProcessor::getSingleton().processAllPacket();
	SqlDelayThreadPool::getSingleton().processAllCB();
	return true;
}

bool WebServer1SecTimerCB::exec(RealTime & curTime)
{
	CGameStaticsMgr::getSingleton().timer(curTime.sec());
	RoleInfoManager::getSingleton().timer(curTime.sec());
	return true;
}

bool WebServer1HourTimerCB::exec(RealTime & curTime)
{
    ServerListener::getSingleton().timer(curTime.sec());
    return true;
}
