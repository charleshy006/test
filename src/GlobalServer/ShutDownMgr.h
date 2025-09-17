#ifndef __H_SHUTDOWN_H__
#define __H_SHUTDOWN_H__
#include "TypesDef.h"
#include "Singleton.h"

class CShutDownMgr
{
	USE_SINGLETON_NEW(CShutDownMgr);
public:
	static const UINT32 s_SHUTDOWN_LASTTIME = 30 * 60;			//停机公告发送后多少秒关闭
	static const UINT16 s_SEND_NOTIFY_INTERVAL = 5 * 60;		//停机公告间隔
private :
	//构造函数
	CShutDownMgr()
	{
		reset();
	}
public :
	//析构函数
	~CShutDownMgr(){}
	//返回实例
	static CShutDownMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//定时器
	void timer(UINT32 cur);
	//开始停机
	void beginStop(bool stopNow = false);
	//通知公告
	void sendNotify(UINT32 sec);
	//清空变量
	void reset();
private :
	bool m_bBeginStopSvr;					//是否开始停机
	UINT32 m_stopSvrTime;					//开始停机时间
	UINT32 m_nextSendNotifyTime;			//下次发公告时间
};
#endif
