#include "GameUserManager.h"
#include "GameUser.h"
#include <stdarg.h>
#include "PacketProcessor.h"
#include "GameServer.h"
#include "Map.h"
#include "RobotMgr.h"
#include "MysqlDatabase.h"
#include "MiscCfgMgr.h"
#include "MarryMgr.h"
#include "GlobalSvrClient.h"

extern GameServer  * g_gameServer;

GameUserManager::GameUserManager():ElementMgr(false,true,!g_gameServer->isPubGameServer(),false)
{
	m_lastReloginTime = g_gameServer->getSec();
}

GameUserManager::~GameUserManager()
{
}

void GameUserManager::removeUserByGate(UINT16 svrID)
{
	struct GateUnloadExec : GameUserCallback
	{
		std::list<GameUser *> m_delUser;
		UINT16 m_svrID;
		GateUnloadExec(UINT16 gateID) : m_svrID(gateID)
		{
			
		}

		bool exec(Element* e)
		{
			GameUser *pUser = (GameUser *)e;
			if (pUser->getGateID() == m_svrID){
				m_delUser.push_back(pUser);
			}
			return true;
		}
	};

	GateUnloadExec cb(svrID);
	execEveryUser(cb);

	for (std::list<GameUser *>::iterator it = cb.m_delUser.begin(); it != cb.m_delUser.end(); ++it) {
		GameUser *pUser = *it;
		removeUser(pUser);
		pUser->unreg();
		SAFE_DELETE(pUser);
	}
}

void GameUserManager::removeAllUser()
{
	struct GlobalUnloadExec : GameUserCallback
	{
		std::list<GameUser *> m_delUser;
		GlobalUnloadExec()
		{
			
		}

		bool exec(Element* e)
		{
			GameUser *pUser = (GameUser *)e;
			m_delUser.push_back(pUser);
			return true;
		}
	};

	GlobalUnloadExec cb;
	execEveryUser(cb);

	for (std::list<GameUser *>::iterator it = cb.m_delUser.begin(); it != cb.m_delUser.end(); ++it) {
		GameUser *pUser = *it;
		removeUser(pUser);
		pUser->unreg();
		SAFE_DELETE(pUser);
	}
}

bool GameUserManager::execEveryUser(GameUserCallback& cb) 
{ 
	return execEveryElement(cb); 
}

void GameUserManager::printAllUser()
{
	struct stPrintAllUser : GameUserCallback
	{
		stPrintAllUser()
		{
			
		}

		bool exec(Element* e)
		{
			GameUser *pUser = (GameUser *)e;
			Athena::logger->trace("角色的名字%s,account=%s",pUser->getName(),pUser->getAccount());
			return true;
		}
	};

	stPrintAllUser cb;
	execEveryUser(cb);
}

void GameUserManager::sendMainCityUser(GameUser * pUser)
{
	if (!pUser){
		return ;
	}
	Map::stOnlineUserOnMainCity retCmd;
	for (TIDMap_IT it = m_idMap.begin(); it != m_idMap.end(); ++it){
		GameUser * pOther = (GameUser *)it->second;	
		if (pOther && (retCmd.userlist_size() < s_MAX_MAIN_CITY_ONLINE_USER) && (pUser != pOther) && (pUser->getMap() == pOther->getMap()) && (pOther->getLevel() >= pOther->getMap()->getNeedLv())){
			Map::stPlayerData * pSyn = retCmd.add_userlist();
			pOther->getAppear(*pSyn);
            //时装翅膀
            CItem * pItem = pOther->m_bagMgr.m_equipBag.getWingItem();
            if (pItem) {
                pSyn->set_fashionwingid(pItem->m_itemInfo.m_itemID);
            }
			pSyn->set_couplename(pOther->getCoupleName());
			if(pOther->m_mountMgr.isRiding()){
				pSyn->set_mountid(pOther->m_mountMgr.getBattleMountID());
			}
		}
	}
	pUser->sendProto2Me(retCmd);
}

void GameUserManager::timer(UINT32 cur)
{
    bool refresh = false;
	if (!g_isTheSameDay(m_lastReloginTime,cur)){//不在同一天
		TIDMap_IT it = m_idMap.begin();
		for (;it != m_idMap.end();++it){
			GameUser * pUser = ( GameUser * )it->second;
			if (pUser){
				pUser->m_activityDataMgr.acceptNewActivity();
                pUser->setBackResData();
				m_waitSendLoginLogUsers.insert(pUser->getID());
			}
		}
		m_lastReloginTime = cur;
        refresh = true;
		Athena::logger->trace("有%d个玩家等待发送登录协议",m_waitSendLoginLogUsers.size());	
		addAllUserLoginDay();
	}

	UINT16 sendNum = 0;
	while ((sendNum <= 20) && (m_waitSendLoginLogUsers.size() > 0)){
		++sendNum;
		UINT32 charID = *m_waitSendLoginLogUsers.begin();
		
		GameUser *pUser = getUser(charID);
		if (pUser){
			pUser->sendLoginLog(true);
		}
		m_waitSendLoginLogUsers.erase(charID);
	}

    if (!refresh) {
		TIDMap_IT it = m_idMap.begin();
		for (;it != m_idMap.end();++it){
			GameUser * pUser = ( GameUser * )it->second;
			if (pUser){
				pUser->m_activityDataMgr.acceptNewActivity();
			}
		}
    }
}

void GameUserManager::addAllUserLoginDay()
{
	for (TIDMap_IT it = m_idMap.begin(); it != m_idMap.end(); ++it){
		GameUser * pUser = (GameUser *)it->second;	
		if (pUser){
			pUser->m_sevenDayActivityMgr.addLoginDay(pUser->getCreateRoleDay());
		}
	}
}

void GameUserManager::checkAcceptNewActivity()
{
	for (TIDMap_IT it = m_idMap.begin(); it != m_idMap.end(); ++it){
		GameUser * pUser = (GameUser *)it->second;	
		if (pUser){
			pUser->m_activityDataMgr.acceptNewActivity();
		}
	}
}

void GameUserManager::clearAllUserOne2OneScore()
{
	for (TIDMap_IT it = m_idMap.begin(); it != m_idMap.end(); ++it){
		GameUser * pUser = (GameUser *)it->second;	
		if (pUser){
			pUser->clearOne2OneScore();
		}
	}
}

GameUser * GameUserManager::getUserOrRobot(UINT32 charID)
{
	GameUser * pRet = NULL;
	if (charID > MAX_ROBOT_CHARID){
		pRet = GameUserManager::getSingleton().getUser(charID);
	}
	else {
		pRet = CRobotMgr::getSingleton().getRobotByID(charID);
	}
	return pRet;
}

void GameUserManager::changeName(UINT32 charID, const string & name)
{
	if(name.empty())
	{
		return;
	}

	Role::stChangeNameResult retCmd;
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(charID);

	UINT8 num = pUser->m_changeNameNum;

	UINT32 needDiamond = CMiscCfgMgr::getSingleton().getChangeNameNeedDiamond(num);
	
	if (!pUser->removeResource(eResource_Diamond,needDiamond,"角色改名")){
		retCmd.set_result(Role::stChangeNameResult::eNot_Enough_Diamond);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if(pUser->m_changeNameNum < 5)
	{
		pUser->m_changeNameNum += 1;
	}
	
	std::string oldName = pUser->m_roleInfo.m_name;

	pUser->changeName(name.c_str());

	retCmd.set_name(name.c_str());
	retCmd.set_result(Role::stChangeNameResult::eSuccess);
	pUser->sendProto2Me(retCmd);


	if(pUser->getCoupleId()){
		GameUser * pCoupleUser = GameUserManager::getSingleton().getUser(pUser->getCoupleId());
		if(pCoupleUser){//在线
			strncpy(pCoupleUser->m_roleInfo.m_coupleName, name.c_str(), MAX_NAMESIZE);
		}else{
			Global::stSynCoupleInfo cmd;
			cmd.set_coupleid(pUser->getCoupleId());
			cmd.set_charid(pUser->getID());
			cmd.set_mynewname(name.c_str());
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		}
	}
	Athena::logger->trace("改名成功，角色名[%s]已改为[%s],已更改%d次",oldName.c_str(),name.c_str(),num);
}
