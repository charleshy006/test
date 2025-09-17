#ifndef __WEBUSERCONNMGR_H__
#define __WEBUSERCONNMGR_H__
#include "Time.h"
#include "SvrInfo.h"
#include "net.h"
#include "Singleton.h"
#include "UniqueID.h"
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

class WebUserConnMgr : public IConnListner , public PacketSender
{
	USE_SINGLETON_NEW(WebUserConnMgr);
private:
	Mutex		m_mutex;
	typedef __gnu_cxx::hash_map<TCPComponent *,UINT16,my_hash<TCPComponent *>, my_key_equal<TCPComponent *> > TCPComponent2SessionID;
	typedef TCPComponent2SessionID::iterator  TCPComponent2SessionID_IT;
	typedef __gnu_cxx::hash_map<UINT16,TCPComponent *> SessionID2TCPComponent;
	typedef SessionID2TCPComponent::iterator SessionID2TCPComponent_IT;
	TCPComponent2SessionID m_tcpComponent2SessionIDMap;
	SessionID2TCPComponent m_sessionID2TCPComponent;
	std::set<TCPComponent *>    m_tmpComponentSet;
	UniqueIDMgr<UINT16> m_IDAllocater;
public:
	static WebUserConnMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	WebUserConnMgr();
	virtual ~WebUserConnMgr();
	//增加Zone和连接对应关系
	void addSessionComponent(UINT16 sessionID,TCPComponent * pIoc);
	//删除Zone和连接对应关系
	UINT16 delSessionComponent(TCPComponent * pIoc);
	//根据ioc获得连接
	UINT16 getSessionIDByIoc(TCPComponent * pIoc);
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
		delSessionComponent((TCPComponent *)ioc);
	}
	//产生唯一ID
	bool genUniqueID(UINT16 & uniqueID);
	//释放唯一ID
	void putbackUniqueID(UINT16  uniqueID);
	//送送字节流
	bool sendRawData(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen);
	//发送字节流
	bool sendRawData(UINT16 sessionID,const char * pData,const UINT32 & cmdLen);
};
#endif
