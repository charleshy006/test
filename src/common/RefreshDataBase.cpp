#include "RefreshDataBase.h"
#include "UtilFunction.h"

void CRefreshDataBase::calcRefreshTime()
{
	struct tm _tm; 
	RealTime::getLocalTime(_tm,curTime());
	UINT32 curSecOfDay =  _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;
	UINT32 refreshSecOfDay = 5 * 3600;
	if (curSecOfDay > refreshSecOfDay){
		m_lastRefreshTime = curTime() - (curSecOfDay - refreshSecOfDay);     
	}    
	else {
		m_lastRefreshTime = curTime() - 86400 + (refreshSecOfDay - curSecOfDay);
	}   
}

void CRefreshDataBase::checkDataRefresh()
{
	if (0 == m_lastRefreshTime){//刷新时间为0,说明第一次登录
		calcRefreshTime();
		doRefreshData();
		return ;
	}    
	UINT32  interval = 0;
	interval = curTime() > m_lastRefreshTime ? curTime() - m_lastRefreshTime : 0; 

	if (interval > 86400){
		calcRefreshTime();
		doRefreshData();
	}
}
