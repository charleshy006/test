#include <iostream>
#include <string>
#include "LoginTimerCB.h"
#include "UserManager.h"

bool LoginTimerCB::exec(RealTime & curTime)
{
	UserManager::getSingleton().timer(curTime.sec());
	return true;
}

