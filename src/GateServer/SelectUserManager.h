#ifndef __H_SELECTUSERMANAGER_H__
#define __H_SELECTUSERMANAGER_H__
#include "TypesDef.h"
#include "GateUser.h"
#include "RoleInfo.h"
#include "UtilFunction.h"
#include "Singleton.h"
#include "Global.pb.h"
#include <string>

class SelectUserManager
{
	USE_SINGLETON_NEW(SelectUserManager);
	public:
		typedef __gnu_cxx::hash_map<std::string,GateUser *,my_hash<std::string> >  Acc2GateUserMap;
		typedef Acc2GateUserMap::iterator  Acc2GateUserMap_IT;
	private:
		Acc2GateUserMap m_acc2GateUser;
	public :
		static SelectUserManager & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		static void destroyMe()
		{
			THE_SINGLETON::destroy();
		}
		//通过accid添加user，仅添加到帐号容器中
		bool addUserByAccount(GateUser* pUser);
		//从管理器中删除一个用户
		void removeUserByAccount(std::string account);
		//通过accid获取一个用户
		GateUser* getUserByAccount(std::string account);
		//加载数据
		bool loginGameResult(std::string account,UINT16 gameID,byte ret);
		//发送协议
		template <class T>
		bool sendProto(std::string account,const T & cmd);
		//踢玩家下线
		void kickUser(std::string account);
		//刷新网关数据
		void refreshGateRoleInfo(std::string account,Global::stRetSnapData * recvCmd);
		//定时检查
		void checkTime(UINT32 cur);		
};

template <class T>
bool SelectUserManager::sendProto(std::string account,const T & cmd)
{
	Acc2GateUserMap_IT  it = m_acc2GateUser.find(account);
	if (it != m_acc2GateUser.end()){
		std::string retStr;
		cmd.SerializeToString(&retStr);
		return it->second->sendProto(retStr.data(),retStr.size(),cmd.ModID,cmd.FunID);
	}
	return false;
}
#endif
