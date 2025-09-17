#ifndef __H_SERVERBASE_H__
#define __H_SERVERBASE_H__
#include "TypesDef.h"
#include "SvrInfo.h"
#include "TimerMgr.h"
#include "Noncopyable.h"
#include "KVPair.h"
#include <map>

class ServerBase : public Noncopyable
{
public :
	//构造函数
	ServerBase();
	//析构函数
	virtual ~ServerBase();
	//开始
	virtual bool start();
	//结束
	virtual void stop() {}
	////计算指定时间0点的时间戳
	UINT32 timestamp_zerohour_time(UINT32 ts) {  
		time_t t = (time_t)ts;
		struct tm * tm= localtime(&t);
		tm->tm_hour = 0;
		tm->tm_min = 0;
		tm->tm_sec = 0;
		return  mktime(tm);
	}
	//获得当前时间(秒为单位)
	UINT32 getSec() {  return m_timerMgr.currentTime.sec() ; }
	//获得当前时间(毫秒为单位)
	UINT64 getMSec(){  return m_timerMgr.currentTime.msecs(); }
	//获得当前时间(RealTime对象)
	RealTime & getRealTime() { return m_timerMgr.currentTime ;}
	//检测是否合法
	SvrInfo * verifyServerInfo(const UINT16 type,const UINT16 svrID,const char* ip,const UINT16 port);
	//根据svrID获得SvrInfo
	SvrInfo * getSvrInfoByID(UINT16 svrID);
	//根据type获得SvrInfo
	SvrInfo * getSvrInfoByType(UINT16 type);
	//运行服务器
	void run();
	//停止定时器
	void stopTimer() {  m_timerMgr .stop() ;}
	//重新加载服务器配置
	void reloadSvrCfg();
	//是否进行负载统计
	bool isLoadStat()
	{
		return m_bLoadStat;
	}
	//获得端口
	UINT16 getPort()
	{
		return m_svrInfo.m_port;
	}
	//初始化是否统计负载均衡
	void initLoadStat(const KVPair & kvPair);
	//是否app版本
	bool isAppVersion()
	{
		return m_appVersion;
	}
	//是否是沙盒
	bool isSanbox()
	{
		return m_sanbox;
	}
	//是否直接购买
	bool isDirectBuy()
	{
		return m_directBuy;
	}
	//获得区ID
	UINT16 getZoneID()
	{
		return m_zoneID;
	}
	//获得平台ID
	UINT32 getPlatformID()
	{
		return m_platformID;
	}
	//开服时间
	UINT32 getOpenServerTime()
	{
		return m_openServerTime;
	}
	//修改开服时间
	void setOpenServerTime(UINT32 time)
	{
		m_openServerTime = time;
	}
protected :
	TimerMgr  m_timerMgr;
	bool m_isLoadSvrList;
	std::map<UINT16,SvrInfo>                    m_serverCtn;        //所有server的配置信息<id,SvrInfo>
	bool m_bLoadStat;												//是否进行负载统计
	bool m_appVersion;												//是否是app版本
	bool m_sanbox;													//是否是沙盒
	bool m_directBuy;												//是否直接购买
	UINT32 m_platformID;											//平台ID
	UINT32 m_openServerTime;										//开服时间
public :        
	SvrInfo                                     m_svrInfo;          //本server的配置信息
	UINT32 m_svrID;                                                 //服务器ID
	UINT16 m_zoneID;												//区的ID
};
#endif
