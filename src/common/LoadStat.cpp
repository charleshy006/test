#include "LoadStat.h"
#include "Log4cxxHelp.h"

void LoadStat::print()
{
    /*
	std::map<std::string,stStatInfo>::iterator it =  m_funName2Time.begin();
	std::map<std::string,stStatInfo>::iterator end =  m_funName2Time.end();
	m_curRT.now();
	UINT64 totalTime = m_curRT.msecs() - m_preRT.msecs();
	if (totalTime < m_printMaxVal){
		return;
	}
	Athena::logger->trace("m_printMaxVal=%d,在本循环里一共花费了时间%d毫秒",m_printMaxVal,totalTime);
	for (;it != end;++it){
		Athena::logger->trace("函数:%s在一次循环消耗的时间为:%d",it->first.c_str(),it->second.m_totalTime);	
	}
    */
}

//开始记录
void LoadStat::begin(std::string funName)
{
	std::map<std::string,stStatInfo>::iterator it =  m_funName2Time.find(funName);	
	if (it != m_funName2Time.end()){
		it->second.m_preRT.now();
	}
}

//结束记录
void LoadStat::end(std::string funName)
{
	std::map<std::string,stStatInfo>::iterator it =  m_funName2Time.find(funName);	
	if (it != m_funName2Time.end()){
		it->second.m_curRT.now();
		it->second.m_totalTime = it->second.m_curRT.msecs() - it->second.m_preRT.msecs();
	}
}

LoadStat & LoadStat::operator()(std::string  funName)
{
	stStatInfo one;
	m_funName2Time.insert(std::make_pair(funName,one));
	return *this;
}

stScopeTimeStat::stScopeTimeStat(std::string blockName) : m_blockName(blockName)
{
	m_preRT.now();
	m_curRT.now();
}

stScopeTimeStat::~stScopeTimeStat()
{
	m_curRT.now();
	Athena::logger->trace("[耗费时间统计]运行了%s花费了%d毫秒",m_blockName.c_str(),m_curRT.msecs() - m_preRT.msecs());
}

void stNetFlowHelp::print(RealTime & cur)
{
	if (m_calcTimer(cur)){
		Athena::logger->trace("[总的网络流量统计]到目前为止发送总量%d,接受总量:%d,本次间隔发送:%d,接收:%d,发送速度:%dB/s,接受速度:%d B/s",m_sendTotal,m_rcvTotal,m_sendTotal - m_lastSendTotal,m_rcvTotal - m_lastRcvTotal,(m_sendTotal - m_lastSendTotal) / m_interval,(m_rcvTotal - m_lastRcvTotal) / m_interval);
		m_lastSendTotal = m_sendTotal;
		m_lastRcvTotal = m_rcvTotal ;
	}
}

void stProtoStatHelp::print(RealTime & cur)
{
	if (m_timer(cur)){
		ProtoID2Size::iterator it = m_sendProtoID2Size.begin();
		Athena::logger->trace("**************打印发送协议统计数据开始**********************");
		for (;it != m_sendProtoID2Size.end();++it){
			UINT16 tmp = it->first;
			Athena::logger->trace("modID =%d,funID=%d ,size=%d",(tmp & 0xFF00) >> 8,tmp & 0x00FF,it->second);	
		}
		Athena::logger->trace("**************打印发送协议统计数据结束**********************");
	
		Athena::logger->trace("**************打印接收协议统计数据开始**********************");
		it = m_rcvProtoID2Size.begin();
		for (;it != m_rcvProtoID2Size.end();++it){
			UINT16 tmp = it->first;
			Athena::logger->trace("modID =%d,funID=%d ,size=%d",(tmp & 0xFF00) >> 8,tmp & 0x00FF,it->second);	
		}
		Athena::logger->trace("**************打印接收协议统计数据结束**********************");
	}	
}

void stProtoStatHelp::addSend(UINT8 modID,UINT8 funID,UINT32 size)
{
	UINT16 tmp = modID;
	tmp = tmp<< 8 ;
	tmp |= funID ;
	m_sendProtoID2Size[tmp] += size;
}

void stProtoStatHelp::addRcv(UINT8 modID,UINT8 funID,UINT32 size)
{
	UINT16 tmp = modID;
	tmp = tmp<< 8 ;
	tmp |= funID ;

	m_rcvProtoID2Size[tmp] += size;
}

