#include "ServerBase.h"
#include "UtilFunction.h"
#include "Log4cxxHelp.h"

ServerBase::ServerBase()
{
	m_isLoadSvrList  = false;
	m_svrID = 0;
	m_bLoadStat = false;
	m_appVersion = false;
	m_sanbox = false;
	m_directBuy = 0;
	m_zoneID = 0;
	m_platformID = 0;
	m_openServerTime = 0;
}

ServerBase::~ServerBase()
{
	
}
	
void ServerBase::reloadSvrCfg()
{
	m_serverCtn.clear();
	g_initSvrList(m_serverCtn);
}

bool ServerBase::start()
{
	if (!m_isLoadSvrList){
		m_isLoadSvrList = true;
		g_initSvrList(m_serverCtn);
        for (std::map<UINT16,SvrInfo>::iterator it = m_serverCtn.begin();
                it != m_serverCtn.end(); it++) {
            Athena::logger->trace("svrid %d, name %s, svrType %d, ip %s, port %d",
                    it->second.m_svrID, it->second.m_name, it->second.m_svrType, it->second.m_ip, it->second.m_port);
        }
		std::map<UINT16,SvrInfo>::iterator it = m_serverCtn.find((UINT16)atoi(Athena::global["svrID"].c_str()));
		if (it != m_serverCtn.end()){
			m_svrInfo = it->second;
            Athena::logger->trace("svrid %d", it->first);
		}   
	}
    m_svrID = m_svrInfo.m_svrID;
	
	if (Athena::global["loadStat"] == "true"){
		m_bLoadStat = true;	
	}
	else {
		m_bLoadStat = false;
	}

	if (Athena::global["isApp"] == "true"){
		m_appVersion = true;	
	}
	else {
		m_appVersion = false;
	}
	
	if (Athena::global["sanbox"] == "true"){
		m_sanbox = true;	
	}
	else {
		m_sanbox = false;
	}
	
	if (Athena::global["DirectBuy"] == "true"){
		m_directBuy = true;	
	}
	else {
		m_directBuy = false;
	}
	
	std::string serverID = Athena::global["serverID"];
	if (serverID.size() > 4){
		std::string strZoneID = serverID.substr(serverID.size() - 4);
		m_zoneID = (UINT16)atoi(strZoneID.c_str());
#ifdef _HDZ_DEBUG
		Athena::logger->trace("启动计算出来的serverID:%s,zoneID:%d",strZoneID.c_str(),m_zoneID);
#endif
	}
	else {
		m_zoneID = 1;
	}
	//m_zoneID = (UINT16)atoi(Athena::global["ZoneID"].c_str());
	m_platformID = (UINT32)atoi(Athena::global["platformID"].c_str());
	m_openServerTime = (UINT32)atoi(Athena::global["OpenServer_Time"].c_str());
	Athena::logger->trace("开服时间utc:%d,%s",m_openServerTime,g_utc2StringFormat(m_openServerTime).c_str());
	return true;
}

void ServerBase::initLoadStat(const KVPair & kvPair)
{
	if (Athena::global["loadStat"] == "true"){
		m_bLoadStat = true;	
	}
	else {
		m_bLoadStat = false;
	}
}

SvrInfo *  ServerBase::verifyServerInfo(const UINT16 type,const UINT16 svrID,const char* ip,const UINT16 port)
{
	std::map<UINT16,SvrInfo>::iterator it = m_serverCtn.find(svrID);
	if (it == m_serverCtn.end()){
		return NULL;
	}

	if (type == it->second.m_svrType && svrID == it->second.m_svrID && 0 == strncmp(ip, it->second.m_ip, MAX_IP_LENGTH) && port == it->second.m_port) {
		return &(it->second);
	}
	return NULL;
}

SvrInfo * ServerBase::getSvrInfoByID(UINT16 svrID)
{
	std::map<UINT16,SvrInfo>::iterator it = m_serverCtn.find(svrID);
	if (it == m_serverCtn.end()){
		return NULL;
	}
	return &(it->second);
}

SvrInfo * ServerBase::getSvrInfoByType(UINT16 type)
{
	std::map<UINT16,SvrInfo>::iterator it = m_serverCtn.begin();
	for  (;it != m_serverCtn.end();++it){
		SvrInfo & svrInfo = it->second;
		if (type == svrInfo.m_svrType){
			return &(it->second);
		}
	}
	return NULL;
}

void ServerBase::run()
{
	if (!start()){
		return ;
	}
	m_timerMgr.run();
	stop();
}
