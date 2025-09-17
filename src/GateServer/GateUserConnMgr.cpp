#include "GateUserConnMgr.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "GateUser.h"
#include "GateServer.h"
#include "SelectUserManager.h"
#include "GateUserManager.h"
#include "GlobalSvrClientMgr.h"
#include "GameSvrClientMgr.h"
#include "Global.pb.h"
#include "Game.pb.h"

extern GateServer * g_gateServer;

GateUserConnMgr::GateUserConnMgr()
{
}

GateUserConnMgr::~GateUserConnMgr()
{
}

void GateUserConnMgr::addComponent(TCPComponent * pIoc)
{
	m_mutex.lock();
	m_tmpComponentSet.insert(pIoc);	
	m_mutex.unlock();
}

void GateUserConnMgr::delComponent(TCPComponent * pIoc)
{
	m_mutex.lock();
	m_tmpComponentSet.erase(pIoc);	
	m_mutex.unlock();
}

bool GateUserConnMgr::forwardGame(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
	if (!pConn){
		return false;
	}
	std::string account = getAccountByIoc(pConn);
	GateUser * pUser = GateUserManager::getSingleton().getUserByAccount(account);
	if (pUser){
		pUser->checkReceviePack(header);	
		Game::stGateForwardGame  forwardCmd;
		forwardCmd.set_charid(pUser->getID());
		forwardCmd.set_modid(header->m_modID);
		forwardCmd.set_funid(header->m_funID);
		forwardCmd.set_cmdlen(header->m_dataLen);
		forwardCmd.set_data((const char *)pNull,header->m_dataLen);
		return GameSvrClientMgr::getSingleton().broadcastByID(pUser->getGameID(),forwardCmd);
	}
	return false;
}

bool GateUserConnMgr::forwardGlobal(TCPComponent * pConn,const PacketHeader * header,const void * pNull)
{
	if (!pConn){
		return false;
	}

	std::string account = getAccountByIoc(pConn);
	GateUser * pUser = GateUserManager::getSingleton().getUserByAccount(account);
	if (!pUser){
		return false;
	}
	pUser->checkReceviePack(header);
	Global::stGateForwardProto  forwardCmd;
	forwardCmd.set_charid(pUser->getID());
	forwardCmd.set_modid(header->m_modID);
	forwardCmd.set_funid(header->m_funID);
	forwardCmd.set_cmdlen(header->m_dataLen);
	forwardCmd.set_data((const char *)pNull,header->m_dataLen);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(forwardCmd);
	return true;
}

void GateUserConnMgr::addAccountComponent(std::string account,TCPComponent * pIoc)
{
	if (!pIoc){
		return;
	}
	MutexGuard guard(m_mutex);
	std::set<TCPComponent *>::iterator it =  m_tmpComponentSet.find(pIoc);
	if (it != m_tmpComponentSet.end()){
		m_tcpComponentAccountMap[pIoc] = account;
		m_account2TCPComponent[account] = pIoc;
		m_tmpComponentSet.erase(it);
	}

#ifdef _HDZ_DEBUG
	if (account == ""){
		Athena::logger->error("有空账号登录!");
	}
#endif
}

std::string  GateUserConnMgr::delAccountComponent(TCPComponent * pIoc)
{
	std::string account;
	MutexGuard guard(m_mutex);
	if (m_tcpComponentAccountMap.find(pIoc) != m_tcpComponentAccountMap.end()){
		account = m_tcpComponentAccountMap[pIoc];
		m_tcpComponentAccountMap.erase(pIoc);
		m_account2TCPComponent.erase(account);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("玩家accountid=%s下线了%p",account.c_str(),pIoc);
#endif
	}
	else {
		Athena::logger->error("GateUserConnMgr::delAccountComponent  查找不到pIoc所对应的账号!%p",pIoc);
	}

	if (account == ""){
		Athena::logger->error("GateUserConnMgr::delAccountComponent  pIoc所对应的账号为0!%p",pIoc);
		return account;
	}
	if (account != ""){
		GateUserManager::getSingleton().addLoginOutAccount(account);
	}
	return account;
}

void GateUserConnMgr::chgTCPComponent(std::string account,TCPComponent * pIoc)
{
	if (!pIoc){
		return ;
	}
	MutexGuard guard(m_mutex);
	Account2TCPComponent_IT it = m_account2TCPComponent.find(account);
	if (it != m_account2TCPComponent.end()){
		TCPComponent * pOldIoc = it->second;
		if (pOldIoc != pIoc){
			it->second = pIoc;
			m_tcpComponentAccountMap[pOldIoc] = "";
			m_tcpComponentAccountMap[pIoc] = account;
			m_account2TCPComponent[account] = pIoc;
			//m_tmpComponentSet.erase(pOldIoc);
		}
	}
	else {
		m_account2TCPComponent[account] = pIoc;
		m_tcpComponentAccountMap[pIoc] = account;
	}
}

std::string  GateUserConnMgr::getAccountByIoc(TCPComponent * pIoc)
{
	std::string account; 
	 MutexGuard guard(m_mutex);
	 if (m_tcpComponentAccountMap.find(pIoc) != m_tcpComponentAccountMap.end()){
	 	account = m_tcpComponentAccountMap[pIoc];
	 }
	 return account;
}

TCPComponent * GateUserConnMgr::getComponentByAccount(std::string account)
{
	MutexGuard guard(m_mutex);
	Account2TCPComponent_IT it = m_account2TCPComponent.find(account);
	if (it != m_account2TCPComponent.end()){
		return it->second;
	}
	return NULL;
}

void GateUserConnMgr::disconnectComponent(TCPComponent * pIoc)
{
	MutexGuard guard(m_mutex);
	if (m_tmpComponentSet.find(pIoc) != m_tmpComponentSet.end()){
		g_gateServer->m_netConnMgr.disconnect(pIoc->getTCPNetIO());
	}
	else if (m_tcpComponentAccountMap.find(pIoc) != m_tcpComponentAccountMap.end()){
		g_gateServer->m_netConnMgr.disconnect(pIoc->getTCPNetIO());	
	}
}

void GateUserConnMgr::writeFinishCloseComponent(TCPComponent * pCom)
{
	if (!pCom){
		return ;
	}
	MutexGuard guard(m_mutex);
	if (m_tmpComponentSet.find(pCom) != m_tmpComponentSet.end()){
		g_gateServer->m_netConnMgr.writeFinishClose(pCom->getTCPNetIO());
	}
	else if (m_tcpComponentAccountMap.find(pCom) != m_tcpComponentAccountMap.end()){
		g_gateServer->m_netConnMgr.writeFinishClose(pCom->getTCPNetIO());	
	}
}

std::string GateUserConnMgr::getComponentIP(TCPComponent *pIoc)
{
	std::string ret;
	MutexGuard guard(m_mutex);
	if (m_tmpComponentSet.find(pIoc) != m_tmpComponentSet.end()){
		ret = pIoc->getSocket()->getIP();
	}
	else if (m_tcpComponentAccountMap.find(pIoc) != m_tcpComponentAccountMap.end()){
		ret = pIoc->getSocket()->getIP();
	}
	return ret;
}

void GateUserConnMgr::disconnectAccount(std::string account)
{
	MutexGuard guard(m_mutex);
	if (m_account2TCPComponent.find(account) == m_account2TCPComponent.end()){
		return;
	}
	TCPComponent * pTcpCom = m_account2TCPComponent[account];	
	g_gateServer->m_netConnMgr.disconnect(pTcpCom->getTCPNetIO());
}

void GateUserConnMgr::writeFinishClose(std::string account)
{
	MutexGuard guard(m_mutex);
	if (m_account2TCPComponent.find(account) == m_account2TCPComponent.end()){
		return;
	}
	TCPComponent * pTcpCom = m_account2TCPComponent[account];	
	g_gateServer->m_netConnMgr.writeFinishClose(pTcpCom->getTCPNetIO());
}

bool GateUserConnMgr::broadcastByID(std::string account,UINT8 modID,UINT8 funID,UINT32 cmdLen,const void * pData)
{
	MutexGuard guard(m_mutex);
	if (m_account2TCPComponent.find(account) == m_account2TCPComponent.end()){
#ifdef _HDZ_DEBUG
		Athena::logger->error("转发包出现错误,根据账号ID找不到客户端的连接,account:%s!",account.c_str());
#endif
		return false;
	}
	if (g_gateServer->isStatProto()){
		g_gateServer->m_protoStat.addSend(modID,funID,cmdLen);
	}
	TCPComponent * pIoc = m_account2TCPComponent[account];	
	return sendClientProto(pIoc,modID,funID,(const char *)pData,cmdLen);
}

bool GateUserConnMgr::broadcastByID(std::vector<std::string> & acccountVec,UINT8 modID,UINT8 funID,UINT32 cmdLen,const void * pData)
{
	MutexGuard guard(m_mutex);
	
	for (UINT16 i = 0;i < acccountVec.size();++i){
		Account2TCPComponent_IT it = m_account2TCPComponent.find(acccountVec[i]);
		if (it != m_account2TCPComponent.end()){
			if (g_gateServer->isStatProto()){
				g_gateServer->m_protoStat.addSend(modID,funID,cmdLen);
			}
			sendClientProto(it->second,modID,funID,(const char *)pData,cmdLen);	
		}
	}
	return true;
}
