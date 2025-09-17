#include <iostream>
#include <string>
#include "UpdateTimerCB.h"
#include "PacketProcessor.h"
#include "SqlDelayThreadPool.h"
#include "UpdateCache.h"

bool UpdateTimerCB::exec(RealTime & curTime)
{
	PacketProcessor::getSingleton().processAllPacket();
	SqlDelayThreadPool::getSingleton().processAllCB();

	if (g_updateServer->isReloadCfg()){
		g_updateServer->setReloadCfg(false);
	}
	return true;
}

bool Update1SecTimerCB::exec(RealTime & curTime)
{
	return true;
}

bool Update5MinTimerCB::exec(RealTime & curTime)
{
    UpdateCacheMgr::getSingleton().timer(curTime.sec());
	return true;
}
