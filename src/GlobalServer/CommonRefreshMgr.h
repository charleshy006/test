#ifndef __H_COMMON_REFRESH_MGR_H__
#define __H_COMMON_REFRESH_MGR_H__
#include "RefreshDataBase.h"
#include "Singleton.h"

class CCommonRefreshMgr : public CRefreshDataBase
{
	USE_SINGLETON_NEW(CCommonRefreshMgr);
public :
	//返回当前对象实例
	static CCommonRefreshMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//获得当前utc时间
	virtual UINT32 curTime();
	//刷新数据
	virtual void doRefreshData();
	//设置刷新时间
	void setRefreshTime(UINT32 refreshTime)
	{
		m_lastRefreshTime = refreshTime;
	}
};
#endif
