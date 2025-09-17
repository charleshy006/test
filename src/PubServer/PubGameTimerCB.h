#ifndef __PUBGAME_TICK_H__
#define __PUBGAME_TICK_H__

#include <iostream>
#include <string>
#include "TimerMgr.h"
#include "Time.h"

//定时器线程
class PubGameTimerCB : public stTimerCB
{
	public :
		PubGameTimerCB(unsigned int _interval) : stTimerCB(_interval){}
		virtual ~PubGameTimerCB(){}
		virtual bool exec(RealTime & curTime);
};

//1秒定时器
class PubGame1SecTimerCB : public stTimerCB
{
	public :
		PubGame1SecTimerCB()  : stTimerCB(1000){}
		virtual ~PubGame1SecTimerCB() {}
		virtual bool exec(RealTime & curTime);
};

#endif


