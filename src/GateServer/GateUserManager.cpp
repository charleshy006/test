#include "GateUserManager.h"
#include "GateUser.h"
#include "GateServer.h"
#include "SelectUserManager.h"
#include "GateUserConnMgr.h"
#include "GlobalSvrClientMgr.h"

extern GateServer * g_gateServer;

GateUserManager::GateUserManager() : ElementMgr(false,true,true,false)
{
}

GateUserManager::~GateUserManager()
{
}

void GateUserManager::removeAllUser()
{
	struct UnloadAllExec : GateUserCallback {
		std::list<UINT32> rmlist;
		bool exec(Element* entry)
		{
			GateUser* u = (GateUser*)entry;
			rmlist.push_back(u->getID());
			return true;
		}
	};

	UnloadAllExec cb;
	execEveryElement(cb);

	for (std::list<unsigned int>::iterator it = cb.rmlist.begin(); it != cb.rmlist.end(); ++it) {
		GateUser* pUser = getUser(*it);
		if (pUser){
			pUser->unreg();
			pUser->terminate();
		}
	}
}

bool GateUserManager::execEveryUser(GateUserCallback& cb) 
{ 
	return execEveryElement(cb); 
}

void GateUserManager::sendProto2World(const void * pstrProto,const UINT32 & cmdLen,UINT8 modID,UINT8 funID)
{
	struct SendProto2All : GateUserCallback {
		char * m_zipProto;
		UINT32 m_zipLen;
		UINT8  m_modID;
		UINT8  m_funID;
		UINT32 m_cmdLen;
		std::vector<std::string> m_accountVec;

		SendProto2All(char * zipProto,UINT32 zipLen,UINT8 modID,UINT8 funID,UINT32 cmdLen) : m_zipProto(zipProto),m_zipLen(zipLen),m_modID(modID),m_funID(funID),m_cmdLen(cmdLen)
		{
		}

		bool exec(Element* e)
		{
			GateUser *pUser =(GateUser *)e;
			m_accountVec.push_back(pUser->m_account);
			/*pUser->sendProto(m_zipProto,m_zipLen,m_modID,m_funID,m_cmdLen);
			if (GateUser::s_forwardProtoTrace){	
				Athena::logger->trace("玩家%s,id=%d 发送了modID=%d,funID=%d的协议",pUser->getName(),pUser->getID(),m_modID,m_funID);
			}*/
			return true;
		}
	};

	SendProto2All cb((char *)pstrProto,cmdLen,modID,funID,cmdLen);
	execEveryUser(cb);

	GateUserConnMgr::getSingleton().broadcastByID(cb.m_accountVec,modID,funID,cmdLen,pstrProto);
}

void GateUserManager::addImmLoginOutAccount(std::string account)
{
	m_immLoginOutUser.insert(account);	
}

void GateUserManager::addLoginOutAccount(std::string account)
{
	m_mutex.lock();
	m_loginOutUser.insert(account);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("账号:%s,添加到注销队列",account.c_str());
#endif
	m_mutex.unlock();
}

void GateUserManager::removeLoginOutAccount(std::string account)
{
	m_mutex.lock();
	m_loginOutUser.erase(account);
	//m_acc2LoginOutTimeMap.erase(account);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("账号:%s,从注销队列里删除",account.c_str());
#endif
	m_mutex.unlock();
}

void GateUserManager::cancelLogout(std::string account)
{
	m_acc2LoginOutTimeMap.erase(account);
	m_notifyLostConnects.erase(account);
}

void GateUserManager::timer()
{
	m_mutex.lock();
	std::set<std::string>::iterator it1 = m_loginOutUser.begin();
	for (;it1 != m_loginOutUser.end(); ++it1){
		if (m_immLoginOutUser.find(*it1) != m_immLoginOutUser.end()){
			m_acc2LoginOutTimeMap[*it1] = g_gateServer->getSec();
			m_immLoginOutUser.erase(*it1);
		}
		else {
			m_acc2LoginOutTimeMap[*it1] = g_gateServer->getSec() + s_RECYLE_TIME;	
		}
		m_notifyLostConnects.insert(*it1);
	}
	m_loginOutUser.clear();
	m_mutex.unlock();
	
	if (m_notifyLostConnects.size() > 0){
		UINT16 num = 0;
		Global::stClientLostConnect retCmd;
		it1 = m_notifyLostConnects.begin();
		for (;it1 != m_notifyLostConnects.end();){
			retCmd.add_account(*it1);
			std::set<std::string>::iterator tmpIt = it1;
			++it1;
			m_notifyLostConnects.erase(tmpIt);
			++num;
			if (num >= 100){
				break;
			}
		}
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
	}

	std::vector<std::string> delVec;

	std::map<std::string,UINT32>::iterator it =  m_acc2LoginOutTimeMap.begin();
	for (;it != m_acc2LoginOutTimeMap.end();++it){
		GateUser * pUser = NULL; 
		if (it->second < g_gateServer->getSec()) {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("账号从服务器下线:%s",it->first.c_str());
#endif
			pUser = getUserByAccount(it->first);
			if (pUser){
				pUser->unreg();
				SAFE_DELETE(pUser);
				//delVec.push_back(it->first);
			}
			else {
				pUser = SelectUserManager::getSingleton().getUserByAccount(it->first);
				SelectUserManager::getSingleton().removeUserByAccount(it->first);

				if (pUser){
					pUser->unreg();
					SAFE_DELETE(pUser);
					//delVec.push_back(it->first);
				}
			}
			delVec.push_back(it->first);
		}
		else {
			pUser = SelectUserManager::getSingleton().getUserByAccount(it->first);
			if (pUser){
				SelectUserManager::getSingleton().removeUserByAccount(it->first);
				Athena::logger->trace("玩家(name:%s,id:%d)在选择界面就下线了",pUser->getName(),pUser->getID());
				pUser->unreg(); 
				SAFE_DELETE(pUser);
				delVec.push_back(it->first);
			}
		}
	}

	for (UINT16 i = 0;i < delVec.size(); ++i){
		m_acc2LoginOutTimeMap.erase(delVec[i]);
	}

	Account2GateUserMap_IT accountIt = m_account2GateUserMap.begin();
	for (;accountIt != m_account2GateUserMap.end();++accountIt){
		GateUser * pUser = accountIt->second;
		if (pUser){
			pUser->timer();
		}
	}
}

GateUser * GateUserManager::getUserByAccount(std::string account)
{
	Account2GateUserMap_IT it = m_account2GateUserMap.find(account); 

	if (it != m_account2GateUserMap.end()){
		return it->second;
	}
	else {
		return NULL;
	}
}
