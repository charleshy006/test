#ifndef __GATEUSERMANAGER_H__
#define __GATEUSERMANAGER_H__
#include "GateUser.h"
#include "Singleton.h"
#include "ElementMgr.h"
#include "ThreadMutex.h"
#include <set>

class GateUserManager : public ElementMgr
{
	USE_SINGLETON_NEW(GateUserManager)
private :
		static const UINT32 s_RECYLE_TIME = 120;
private :
	std::set<std::string> m_loginOutUser;
	std::set<std::string> m_immLoginOutUser;			//立刻下线玩家
	std::map<std::string,UINT32>  m_acc2LoginOutTimeMap;
	typedef __gnu_cxx::hash_map<std::string,GateUser *,my_hash<std::string> >       Account2GateUserMap;
	typedef Account2GateUserMap::iterator                       				  Account2GateUserMap_IT;
	Account2GateUserMap                                         				  m_account2GateUserMap;
	Mutex m_mutex;
	std::set<std::string> m_notifyLostConnects;
private :
	GateUserManager();
public:	
	typedef execElement<Element>			GateUserCallback;

	virtual ~GateUserManager();
	static GateUserManager & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	static void destroyMe()
	{
		return THE_SINGLETON::destroy();
	}
	//添加一个角色指针到管理器
	inline bool addUser(GateUser* pUser) 
	{ 
		if (addElement(pUser)){
			if (!m_account2GateUserMap.insert(std::make_pair(pUser->m_account,pUser)).second){
				removeElement(pUser);
				return false;
			}
		}
		return true;
	}
	//根据角色指针，从管理器删除一个角色
	inline void removeUser(GateUser* pUser) 
	{ 
		removeElement(pUser);
		m_account2GateUserMap.erase(pUser->m_account);
	}
	//根据角色ID，获取一个角色
	inline GateUser* getUser(const UINT32 charid) 
	{ 
		return (GateUser*)getElementByID(charid); 
	}
	//根据角色名称，获取一个角色
	inline GateUser* getUser(char* name) 
	{ 
		return (GateUser*)getElementByName(name); 
	}
	//根据accountID返回角色
	GateUser * getUserByAccount(std::string account);
	//删除所有用户
	void removeAllUser();
	//对所有用户执行回调
	bool execEveryUser(GateUserCallback& cb);
	//给所有玩家发送协议
	void sendProto2World(const void * pstrProto,const UINT32 & cmdLen,UINT8 modID,UINT8 funID);
	//增加一个下线玩家
	void addLoginOutAccount(std::string account);
	//删除下线玩家
	void removeLoginOutAccount(std::string account);
	//增加立即下线的玩家
	void addImmLoginOutAccount(std::string account);
	//取消下线
	void cancelLogout(std::string account);
	//处理所有下线玩家
	void timer();	
};

#endif
