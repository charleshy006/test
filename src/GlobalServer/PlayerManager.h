#ifndef __PLAYERMANAGER_H__
#define __PLAYERMANAGER_H__
#include "Player.h"
#include "UtilFunction.h"
#include "ElementMgr.h"
#include "Singleton.h"
#include <string>
#include <ext/hash_map>
#include <map>
#include <set>

class PlayerManager : public ElementMgr
{
	USE_SINGLETON_NEW(PlayerManager);
private :
	PlayerManager() : ElementMgr(false,true,true,false) 
	{
	}

	typedef __gnu_cxx::hash_map<UINT32,UINT32>  			 CharID2ZoneID;
	typedef __gnu_cxx::hash_map<UINT32,UINT32>::iterator     CharID2ZoneID_IT;
	CharID2ZoneID   m_charID2ZoneIDMap;

	typedef std::map<UINT32,std::set<Player *> >     		  ZoneID2Player;
	typedef ZoneID2Player::iterator ZoneID2Player_IT;
	ZoneID2Player  m_zonePlayers;

	typedef __gnu_cxx::hash_map<std::string,Player *,my_hash<std::string> >		  Account2PlayerMap;
	typedef Account2PlayerMap::iterator 					  Account2PlayerMap_IT;
	Account2PlayerMap										  m_account2PlayerMap;
public:
	~PlayerManager();
	static PlayerManager & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	inline Player* getUserByName(char* name) { return (Player*)getElementByName(name); }
	inline Player* getUserByID(const UINT32 charid) { return (Player*)getElementByID(charid); }
	Player* getUserByAccount(std::string account);
	//加载玩家与区对应关系
	void load();
	//玩家注册后
	void afterCreateRole(UINT32 charID,UINT16 zoneID);
	//玩家数量
	UINT32 userNum() { return size(); }
	//添加一个角色指针到管理器
	bool addUser(Player* pUser);
	//根据角色指针，从管理器删除一个角色
	void removeUser(Player* pUser);
	//删除一个conn上的所有用户会话
	void removeAllUserByConn(UINT16 svrID);
	//时间回调
	void timerUpdate(UINT32 cur);
	//更新在线人数
	void updateOnlineNum(UINT32 sec);
	//更新在线人数回调
	void updateOnlineNumCB(bool opResult,UINT64 autoID);
	//获得推荐玩家列表
	void getRecommondList(std::vector<Player *> & vec,UINT32 zoneID);
	template <class T>
	bool sendProto2Zone(UINT16 svrID,const T & cmd);
	template <class T>
	bool sendProto2AllZone(const T & cmd);
};

extern GlobalServer * g_globalServer;
template <class T>
bool PlayerManager::sendProto2Zone(UINT16 svrID,const T & cmd)
{
	Global::stBroadcastProto2AllUser retCmd;
	retCmd.set_funid(cmd.FunID);
	retCmd.set_modid(cmd.ModID);
	std::string retStr;
	cmd.SerializeToString(&retStr);
	retCmd.set_cmdlen(retStr.size());
	retCmd.set_data(retStr);
	return g_globalServer->getConnMgr().broadcastByID(svrID,retCmd);
}

template <class T>
bool PlayerManager::sendProto2AllZone(const T & cmd)
{
	Global::stBroadcastProto2AllUser retCmd;
	retCmd.set_funid(cmd.FunID);
	retCmd.set_modid(cmd.ModID);
	std::string retStr;
	cmd.SerializeToString(&retStr);
	retCmd.set_cmdlen(retStr.size());
	retCmd.set_data(retStr);
	return g_globalServer->getConnMgr().broadcastByType(GATESERVER,retCmd);
}
#endif
