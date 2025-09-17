#ifndef __H_REFRESH_DATA_BASE_H__
#define __H_REFRESH_DATA_BASE_H__
#include "Time.h"
#include "TypesDef.h"

class CRefreshDataBase
{
public :
	//构造函数
	CRefreshDataBase()
	{
		m_lastRefreshTime = 0;
	}
	//析构函数
	virtual ~CRefreshDataBase()
	{}
	//计算刷新时间
	void calcRefreshTime();
	//检查是否刷新数据
	void checkDataRefresh();
	//获得当前utc时间
	virtual UINT32 curTime() = 0;
	//刷新数据
	virtual void doRefreshData() = 0;
protected :
	UINT32 m_lastRefreshTime;
};
#endif
