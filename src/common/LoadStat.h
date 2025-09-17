#ifndef __H_LOAD_STATIC_H__
#define __H_LOAD_STATIC_H__
#include "Time.h"
#include "Log4cxxHelp.h"
#include <map>

class LoadStat
{
	public : 
		LoadStat()
		{
		}
	public :
		struct stStatInfo
		{
			RealTime m_preRT;
			RealTime m_curRT;
			UINT64   m_totalTime;
			stStatInfo()
			{
				m_totalTime = 0;
			}
		};

		~LoadStat()
		{
		}
		void init()
		{
			m_preRT.now();
		}

		//开始记录
		void begin(std::string funName);
		//结束记录
		void end(std::string funName);
		//打印
		void print();
		//注册要统计的函数名字
		LoadStat & operator()(std::string  funName);
		//设置打印阀值
		void setPrintMaxVal(UINT64 printMaxVal)
		{
			m_printMaxVal = printMaxVal;
		}
	private :
		RealTime m_preRT;
		RealTime m_curRT;
		UINT64   m_printMaxVal;			//打印的阀值		
		std::map<std::string,stStatInfo> m_funName2Time;
};

struct stScopeTimeStat
{
	RealTime m_preRT;
	RealTime m_curRT;
	std::string m_blockName;

	stScopeTimeStat(std::string blockName);
	~stScopeTimeStat();
};

struct stNetFlowHelp
{
	UINT32 m_sendTotal;
	UINT32 m_rcvTotal;
	UINT32 m_lastSendTotal;
	UINT32 m_lastRcvTotal;
	UINT32 m_interval;
	Timer  m_calcTimer;

	stNetFlowHelp(UINT32 interval = 120):m_interval(interval),m_calcTimer(m_interval,m_interval)
	{
		m_sendTotal = 0;
		m_rcvTotal = 0;
		m_lastSendTotal = 0;
		m_lastRcvTotal = 0;
	}
	
	template <class T>
	void calc(T * tcpConn);
	void print(RealTime & cur);
};

template <class T>
void stNetFlowHelp::calc(T * tcpConn)
{
	m_sendTotal += tcpConn->getNetSendDataStat().getTotal();
	m_rcvTotal += tcpConn->getNetRevDataStat().getTotal();
}

struct stProtoStatHelp
{
	typedef __gnu_cxx::hash_map<UINT16,UINT32>  ProtoID2Size;
	ProtoID2Size m_sendProtoID2Size;
	ProtoID2Size m_rcvProtoID2Size;
	Timer m_timer;

	stProtoStatHelp(): m_timer(180)
	{
		m_sendProtoID2Size.clear();
		m_rcvProtoID2Size.clear();
	}

	void print(RealTime & cur);
	void addSend(UINT8 modID,UINT8 funID,UINT32 size);
	void addRcv(UINT8 modID,UINT8 funID,UINT32 size);
};
#endif
