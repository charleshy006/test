#ifndef __LOGINTIMETICK_H__
#define __LOGINTIMETICK_H__

#include <iostream>
#include <string>
#include "TimerMgr.h"
#include "Time.h"

//定时器线程
class LoginTimerCB : public stTimerCB
{
	public :
		LoginTimerCB(unsigned int _interval) : stTimerCB(_interval){}
		virtual ~LoginTimerCB(){}
		virtual bool exec(RealTime & curTime);
};
#endif


