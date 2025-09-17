#include <iostream>
#include <string>
#include "PubGameTimerCB.h"
#include "One2OnePVP.h"
#include "Team.h" 
#include "HeroLeague.h"

bool PubGameTimerCB::exec(RealTime & curTime)
{
	return true;
}

bool PubGame1SecTimerCB::exec(RealTime & curTime)
{
	COne2OnePVPMgr::getSingleton().timer(curTime.sec());
	CTeamMatchMgr::getSingleton().timer(curTime.sec());
	CHeroLeagueMgr::getSingleton().timer(curTime.sec());
	return true;
}

