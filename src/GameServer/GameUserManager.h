#ifndef __GAMEUSERMANAGER_H__
#define __GAMEUSERMANAGER_H__
#include "GameUser.h"
#include "Singleton.h"
#include "ElementMgr.h"
#include "TblQueryResult.h"
#include "SqlOperation.h"

class GameUserManager : public ElementMgr
{
	USE_SINGLETON_NEW(GameUserManager)
public :
	static const INT32 s_MAX_MAIN_CITY_ONLINE_USER = 10;
private :
	GameUserManager();
public:	
	typedef execElement<Element>			GameUserCallback;
	virtual ~GameUserManager();
	static GameUserManager & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	static void destroyMe()
	{
		return THE_SINGLETON::destroy();
	}
	//添加一个角色指针到管理器
	inline bool addUser(GameUser* pUser) 
	{ 
		return addElement(pUser); 
	}
	//根据角色指针，从管理器删除一个角色
	inline void removeUser(GameUser* pUser) 
	{ 
		removeElement(pUser); 
	}
	//根据角色ID，获取一个角色
	inline GameUser* getUser(const UINT32 charid) 
	{ 
		return (GameUser*)getElementByID(charid); 
	}
	//根据角色名称，获取一个角色
	inline GameUser* getUser(char* name) 
	{ 
		return (GameUser*)getElementByName(name); 
	}
	//删除某个网关上面的玩家
	void removeUserByGate(UINT16  svrID);
	//删除所有用户
	void removeAllUser();
	//对所有用户执行回调
	bool execEveryUser(GameUserCallback& cb);
	//给全世界广播消息
	template <class T>
	void sendProto2World(const T & cmd);
	//获得玩家数量
	UINT32 userNum() { return size() ;}
	//给列表里的玩家发送协议
	template <class T>
	void sendProto2UserList(std::vector<UINT32> & userList,const T & cmd);
	//打印所有玩家
	void printAllUser();
	//发送主城在线玩家
	void sendMainCityUser(GameUser * pUser);
	//定时器
	void timer(UINT32 cur);
	//增加登录天数
	void addAllUserLoginDay();
	//清空在线玩家的1v1积分
	void clearAllUserOne2OneScore();
	//根据id获得玩家或机器人
	GameUser * getUserOrRobot(UINT32 charID);
	//所有玩家接受新活动
	void checkAcceptNewActivity();
	//改名
	void changeName(UINT32 charID, const string & name);
private :
	UINT32 m_lastReloginTime;					//重新发送登录日志时间
	std::set<UINT32> m_waitSendLoginLogUsers;	//等待发送登录日志的玩家
};

template <class T>
void GameUserManager::sendProto2World(const T & cmd)
{
	Game::stBroadbastProto2World  retCmd;
	retCmd.set_modid(cmd.ModID);
	retCmd.set_funid(cmd.FunID);
	std::string retStr;
	cmd.SerializeToString(&retStr);
	retCmd.set_cmdlen(retStr.size());
	retCmd.set_data(retStr);
	g_gameServer->getConnMgr().broadcastByType(GATESERVER,retCmd);
}

template <class T>
void GameUserManager::sendProto2UserList(std::vector<UINT32> & userList,const T & cmd)
{
	if (g_gameServer->isPubGameServer()) //如果是跨服，每个用户分发到不同的gameserver转发
	{
		for (UINT32 i = 0; i < userList.size() ; ++i) {
			GameUser* pUser = getUser(userList[i]);
			if (pUser)
			{
				pUser->sendProto2Me(cmd);
			}
		}
	}else{

		Game::stBroadcastProto2UserList  retCmd;
		for (UINT32 i = 0; i < userList.size() ; ++i) {
			retCmd.add_userlist(userList[i]);	
		}
		retCmd.set_modid(cmd.ModID);
		retCmd.set_funid(cmd.FunID);
		std::string retStr;
		cmd.SerializeToString(&retStr);
		retCmd.set_cmdlen(retStr.size());
		retCmd.set_data(retStr);
		g_gameServer->getConnMgr().broadcastByType(GATESERVER,retCmd);
	}
}

#endif
