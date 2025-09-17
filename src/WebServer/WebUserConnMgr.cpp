#include "WebUserConnMgr.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "WebServer.h"

extern WebServer * g_webServer;

WebUserConnMgr::WebUserConnMgr() : m_IDAllocater(1,(UINT16)-1)
{
}

WebUserConnMgr::~WebUserConnMgr()
{
}

void WebUserConnMgr::addComponent(TCPComponent * pIoc)
{
	m_mutex.lock();
	m_tmpComponentSet.insert(pIoc);	
	m_mutex.unlock();
}

void WebUserConnMgr::delComponent(TCPComponent * pIoc)
{
	m_mutex.lock();
	m_tmpComponentSet.erase(pIoc);	
	m_mutex.unlock();
}

void WebUserConnMgr::addSessionComponent(UINT16 zoneID,TCPComponent * pIoc)
{
	if (!pIoc){
		return;
	}
	MutexGuard guard(m_mutex);
	std::set<TCPComponent *>::iterator it =  m_tmpComponentSet.find(pIoc);
	if (it != m_tmpComponentSet.end()){
		m_tcpComponent2SessionIDMap[pIoc] = zoneID;
		m_sessionID2TCPComponent[zoneID] = pIoc;
		m_tmpComponentSet.erase(it);
	}
}

UINT16  WebUserConnMgr::delSessionComponent(TCPComponent * pIoc)
{
	UINT16 zoneID = 0;
	MutexGuard guard(m_mutex);
	if (m_tcpComponent2SessionIDMap.find(pIoc) != m_tcpComponent2SessionIDMap.end()){
		zoneID = m_tcpComponent2SessionIDMap[pIoc];
		m_tcpComponent2SessionIDMap.erase(pIoc);
		m_sessionID2TCPComponent.erase(zoneID);
	}
	else {
		Athena::logger->error("WebUserConnMgr::delComponent  查找不到pIoc所对应的账号!%p",pIoc);
	}

	return zoneID;
}

UINT16  WebUserConnMgr::getSessionIDByIoc(TCPComponent * pIoc)
{
	UINT16 zoneID = 0; 
	MutexGuard guard(m_mutex);
	 if (m_tcpComponent2SessionIDMap.find(pIoc) != m_tcpComponent2SessionIDMap.end()){
	 	zoneID = m_tcpComponent2SessionIDMap[pIoc];
	 }
	 return zoneID;
}

bool WebUserConnMgr::sendRawData(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen)
{
	if (!pCom){
		return false;
	}

	MutexGuard guard(m_mutex);
	if (m_tmpComponentSet.find(pCom) != m_tmpComponentSet.end()){
		return sendClientProto(pCom,0,0,pData,cmdLen);
	}

	if (m_tcpComponent2SessionIDMap.find(pCom) != m_tcpComponent2SessionIDMap.end()){
		return sendClientProto(pCom,0,0,pData,cmdLen);	
	}
	return false;
}

bool WebUserConnMgr::sendRawData(UINT16 sessionID,const char * pData,const UINT32 & cmdLen)
{
	MutexGuard guard(m_mutex);
	SessionID2TCPComponent_IT it = m_sessionID2TCPComponent.find(sessionID);
	if (it != m_sessionID2TCPComponent.end()){
		return sendClientProto(it->second,0,0,pData,cmdLen);	
	}
	return false;
}

bool WebUserConnMgr::genUniqueID(UINT16 & uniqueID)
{
	uniqueID = m_IDAllocater.genID();
	if (m_IDAllocater.isValidID(uniqueID)){
		return true;
	}
	return false;
}

void WebUserConnMgr::putbackUniqueID(UINT16  uniqueID)
{
	m_IDAllocater.putID(uniqueID);	
}
