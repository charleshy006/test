#ifndef __UPDATE_TICK_H__
#define __UPDATE_TICK_H__

#include <iostream>
#include <string>
#include "TimerMgr.h"
#include "Time.h"

//定时器线程
class UpdateTimerCB : public stTimerCB
{
	public :
		UpdateTimerCB(unsigned int _interval) : stTimerCB(_interval){}
		virtual ~UpdateTimerCB(){}
		virtual bool exec(RealTime & curTime);
};

//1秒定时器
class Update1SecTimerCB : public stTimerCB
{
	public :
		Update1SecTimerCB()  : stTimerCB(1000){}
		virtual ~Update1SecTimerCB() {}
		virtual bool exec(RealTime & curTime);
};

//5分钟定时器
class Update5MinTimerCB : public stTimerCB
{
	public :
		Update5MinTimerCB()  : stTimerCB(1000 * 60 * 5){}
		virtual ~Update5MinTimerCB() {}
		virtual bool exec(RealTime & curTime);
};
#endif
