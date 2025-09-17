#ifndef __GAMETIMER_H__
#define __GAMETIMER_H__

#include <iostream>
#include <string>
#include "TimerMgr.h"
#include "Time.h"
#include "LoadStat.h"

class GameTimerCB : public stTimerCB
{
	public :
		GameTimerCB(UINT64 _interval);
		~GameTimerCB(){}
		virtual bool exec(RealTime & curTime);
//#ifdef _HDZ_DEBUG
		LoadStat m_loadStat;
//#endif
};

class GameTimer1SecCB : public stTimerCB
{
	private :
		bool m_zeroRefresh;
	public :
		GameTimer1SecCB() : stTimerCB(1000),m_zeroRefresh(false){}
		~GameTimer1SecCB() {}
		virtual bool exec(RealTime & curTime);
};

#endif


