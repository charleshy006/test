#ifndef __GLOBAL_TICK_H__
#define __GLOBAL_TICK_H__

#include <iostream>
#include <string>
#include "TimerMgr.h"
#include "Time.h"

//定时器线程
class GlobalServerTimerCB : public stTimerCB
{
	public :
		GlobalServerTimerCB(unsigned int _interval) : stTimerCB(_interval){}
		virtual ~GlobalServerTimerCB(){}
		virtual bool exec(RealTime & curTime);
};

//1秒定时器
class GlobalServer1SecTimerCB : public stTimerCB
{
	public :
		GlobalServer1SecTimerCB()  : stTimerCB(1000){}
		virtual ~GlobalServer1SecTimerCB() {}
		virtual bool exec(RealTime & curTime);
};

//5分钟定时器
class GlobalServer5MinTimerCB : public stTimerCB
{
	public :
		GlobalServer5MinTimerCB() : stTimerCB(300 * 1000){}
		virtual ~GlobalServer5MinTimerCB(){}
		virtual bool exec(RealTime & curTime);
};
#endif


