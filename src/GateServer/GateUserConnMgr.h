#ifndef __GATEUSERCONNMGR_H__
#define __GATEUSERCONNMGR_H__
#include "Time.h"
#include "SvrInfo.h"
#include "net.h"
#include "Singleton.h"
#include <set>
#include <vector>
#include <ext/hash_map>
using namespace net;
template<>
inline bool my_key_equal<TCPComponent *>::operator () (TCPComponent * pComm1,TCPComponent * pComm2) const
{
	if (pComm1 == pComm2){
		return true;
	}
	return false;
}

template<>
inline size_t my_hash<TCPComponent *>::operator()(TCPComponent * p) const
{
	 return (reinterpret_cast<size_t>(p));
}

class GateUserConnMgr : public IConnListner , public PacketSender
{
	USE_SINGLETON_NEW(GateUserConnMgr);
private:
	Mutex		m_mutex;
	typedef __gnu_cxx::hash_map<TCPComponent *,std::string, 
			my_hash<TCPComponent *>, my_key_equal<TCPComponent *> > TCPComponent2Account;
	typedef TCPComponent2Account::iterator  TCPComponent2Account_IT;
	typedef __gnu_cxx::hash_map<std::string,TCPComponent *,my_hash<std::string> > Account2TCPComponent;
	typedef Account2TCPComponent::iterator Account2TCPComponent_IT;
	TCPComponent2Account m_tcpComponentAccountMap;
	Account2TCPComponent m_account2TCPComponent;
	std::set<TCPComponent *>    m_tmpComponentSet;
public:
	static GateUserConnMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	GateUserConnMgr();
	virtual ~GateUserConnMgr();
	//增加account登录和连接对应关系
	void addAccountComponent(std::string account,TCPComponent * pIoc);
	//删除account登录和连接对应关系
	std::string delAccountComponent(TCPComponent * pIoc);
	//根据ioc获得账号ID
	std::string getAccountByIoc(TCPComponent * pIoc);
	//根据账号ID获得TCPComponent
	TCPComponent * getComponentByAccount(std::string account);
	//连接来了通知
	void onAddComponent(IOComponent *ioc) { addComponent((TCPComponent *)ioc); }
	//增加component
	void addComponent(TCPComponent * pIoc);
	//删除component
	void delComponent(TCPComponent * pIoc);
	//连接断开了通知 
	void onRemoveComponent(IOComponent *ioc)
	{
		delComponent((TCPComponent *)ioc);
		delAccountComponent((TCPComponent *)ioc);
	}
	//切换账号的TCPComponent
	void chgTCPComponent(std::string account,TCPComponent * pIoc);
	//断开某个账号的连接
	void disconnectAccount(std::string account);
	//某个账号写完数据再断开
	void writeFinishClose(std::string account);
	//根据某个连接断开
	void disconnectComponent(TCPComponent * pIoc);
	//某个连接写完再断开
	void writeFinishCloseComponent(TCPComponent * pCom);
	//转发到game
	bool forwardGame(TCPComponent  *pConn,const PacketHeader * header,const void * pNull);
	//转发到global
	bool forwardGlobal(TCPComponent *pConn,const PacketHeader * header,const void * pNull);
	//根据某account广播
	bool broadcastByID(std::string account,UINT8 modID,UINT8 funID,UINT32 cmdLen,const void * pData);
	//获得ip
	std::string getComponentIP(TCPComponent *pConn);
	//给列表的账号发送协议
	bool broadcastByID(std::vector<std::string> & acccountVec,UINT8 modID,UINT8 funID,UINT32 cmdLen,const void * pData);
	//给某个连接发送协议
	template <class T>
	bool sendPacket2Commponet(TCPComponent * pCom, T & cmd);
};

template <class T>
bool GateUserConnMgr::sendPacket2Commponet(TCPComponent * pCom,T & cmd)
{
	if (!pCom){
		return false;
	}

	MutexGuard guard(m_mutex);
	std::string retStr;
	cmd.SerializeToString(&retStr);
	if (m_tmpComponentSet.find(pCom) != m_tmpComponentSet.end()){
		return sendClientProto(pCom,cmd.ModID,cmd.FunID,retStr.c_str(),retStr.size());
	}

	if (m_tcpComponentAccountMap.find(pCom) != m_tcpComponentAccountMap.end()){
		return sendClientProto(pCom,cmd.ModID,cmd.FunID,retStr.c_str(),retStr.size());	
	}
	return false;
}
#endif
