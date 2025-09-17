#ifndef __WEB_TICK_H__
#define __WEB_TICK_H__

#include <iostream>
#include <string>
#include "TimerMgr.h"
#include "Time.h"

//定时器线程
class WebServerTimerCB : public stTimerCB
{
	public :
		WebServerTimerCB(unsigned int _interval) : stTimerCB(_interval){}
		virtual ~WebServerTimerCB(){}
		virtual bool exec(RealTime & curTime);
};

//1秒定时器
class WebServer1SecTimerCB : public stTimerCB
{
	public :
		WebServer1SecTimerCB()  : stTimerCB(1000){}
		virtual ~WebServer1SecTimerCB() {}
		virtual bool exec(RealTime & curTime);
};

//1小时定时器
class WebServer1HourTimerCB : public stTimerCB
{
	public :
		WebServer1HourTimerCB()  : stTimerCB(1000 * 3600){}
		virtual ~WebServer1HourTimerCB() {}
		virtual bool exec(RealTime & curTime);
};
#endif
