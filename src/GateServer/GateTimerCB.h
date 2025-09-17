#ifndef __GATETIMETICK_H__
#define __GATETIMETICK_H__

#include <iostream>
#include <string>
#include "TimerMgr.h"
#include "Time.h"
#include "LoadStat.h"

class GateTimerCB : public stTimerCB
{
	public :
		GateTimerCB(UINT64 _interval);
		~GateTimerCB(){}
		virtual bool exec(RealTime & curTime);
	private :
//#ifdef _HDZ_DEBUG
		LoadStat m_loadStat;
//#endif
};

//网关定时器线程
class GateTimerCB10Sec : public stTimerCB
{
	public :
		GateTimerCB10Sec(UINT64 _interval = 1000 * 10) : stTimerCB(_interval){}
		~GateTimerCB10Sec(){}
		virtual bool exec(RealTime & curTime);
};

//20秒定时器
class GateTimerCB30Sec : public stTimerCB
{
	public :
		GateTimerCB30Sec(UINT64 _interval = 1000 * 30) : stTimerCB(_interval){}
		virtual ~GateTimerCB30Sec() {}
		virtual bool exec(RealTime & curTime);
};
#endif


