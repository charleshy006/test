#include "UserRoleModHandle.h"
#include "SvrInfo.h"
#include "GateServer.h"
#include "GateUserConnMgr.h"
#include "GateUserConnMgr.h"
#include "md5.h"
#include "GateUserManager.h"
#include "SelectUserManager.h"
#include "Global.pb.h"
#include "GlobalSvrClientMgr.h"

extern GateServer * g_gateServer;

UserRoleModHandle::UserRoleModHandle()
{
	registerHandle(&UserRoleModHandle::createRole);
	registerHandle(&UserRoleModHandle::enterGame);
	registerHandle(&UserRoleModHandle::changeRole);
}

UserRoleModHandle::~UserRoleModHandle()
{

}

bool UserRoleModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<UserRoleModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void UserRoleModHandle::createRole(TCPComponent * pCom,Role::ReqCreateRole * reqCreate)
{
	if (!pCom || !reqCreate) {
		return ;
	}
	std::string account =  GateUserConnMgr::getSingleton().getAccountByIoc(pCom);
	GateUser * pUser = SelectUserManager::getSingleton().getUserByAccount(account);
	if (!pUser){
		return ;
	}



	UINT32 serverID = reqCreate->serverid();
	Athena::logger->trace("@@@ reqCreate->serverid is %d", serverID);

	// std::string  serverIDstr = serverID.toString();
	// Athena::logger->trace("@@@Charles $$$$ serverIDstr is %s", serverIDstr.c_str());

	// std::string newName = reqCreate->name() + "_S" + serverIDstr;
	// Athena::logger->trace("@@@Charles $$$$ newName is %s", newName.c_str());


	Global::stCreateRole  createCmd;
	createCmd.set_account(pUser->m_account);
	createCmd.set_account(account);
	// createCmd.set_name(newName);
	createCmd.set_name(reqCreate->name());
	createCmd.set_job(reqCreate->job());
	createCmd.set_sex(reqCreate->sex());
	createCmd.set_gateid(g_gateServer->m_svrInfo.m_svrID);
	createCmd.set_headid(reqCreate->headid());
	createCmd.set_faceid(reqCreate->faceid());
	createCmd.set_clientip(pUser->m_ip);
	createCmd.set_deviceid(pUser->m_deviceID);
	createCmd.set_opid(reqCreate->opid());
	createCmd.set_channelid(pUser->m_channelID);
	createCmd.set_serverid(reqCreate->serverid());

	GlobalSvrClientMgr::getSingleton().broadcastProto2All(createCmd);
}

void UserRoleModHandle::enterGame(TCPComponent * pCom,Role::ReqEnterGame * enterGame)
{
	if (!pCom || !enterGame) {
		return ;
	}


	std::string account =  GateUserConnMgr::getSingleton().getAccountByIoc(pCom);
	GateUser * pUser = SelectUserManager::getSingleton().getUserByAccount(account);
	if (!pUser){
		Athena::logger->error("%s进入游戏时候在 SelectUserManager中找不到玩家",account.c_str());
		return;
	}

	if (!pUser->isGameState(GateUser::GAME_STATE_SELECT)){
		Athena::logger->error("%s进入游戏失败,没有处于GAME_STATE_SELECT状态",pUser->m_account);
		return;
	}

	if (pUser->isGameState((GateUser::GAME_STATE_ENTER))){
		Athena::logger->error("%s进入游戏失败,正在处理进入状态",pUser->m_account);
		return ;		
	}
	
	if (!pUser->initCharInfo(enterGame->charid())){
		Athena::logger->error("%s请求进入游戏失败,初始化角色信息不成功",pUser->m_account);
		return ;		
	}

	pUser->setGameState(GateUser::GAME_STATE_ENTER);
	
	Global::stReqLoadData   reqLoadDataCmd;
	reqLoadDataCmd.set_account(pUser->m_account);
	reqLoadDataCmd.set_account(account);
	reqLoadDataCmd.set_gamesvrid(g_gateServer->assignGameSvr2User(account));
	reqLoadDataCmd.set_gatesvrid(g_gateServer->m_svrInfo.m_svrID);
	reqLoadDataCmd.set_charid(enterGame->charid());
	reqLoadDataCmd.set_ip(pUser->m_ip);
	reqLoadDataCmd.set_deviceid(pUser->m_deviceID);
	reqLoadDataCmd.set_channelid(pUser->m_channelID);
	std::string netIP = GateUserConnMgr::getSingleton().getComponentIP(pCom);
	reqLoadDataCmd.set_netip(netIP);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(reqLoadDataCmd);

	Athena::logger->trace("accountID=%s, netIP=%s,发送了进入游戏协议",pUser->m_account, netIP.c_str());
}

void UserRoleModHandle::changeRole(TCPComponent * pCom,Role::stChangeRole * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	std::string account =  GateUserConnMgr::getSingleton().getAccountByIoc(pCom);
	GateUser * pUser =  GateUserManager::getSingleton().getUserByAccount(account);
	GateUser * pOther = SelectUserManager::getSingleton().getUserByAccount(account);

	if (!pUser || pOther){
		return ;
	}
	
	Role::stGotoSelectRole retCmd;
	retCmd.set_curcharid(pUser->getID());
	std::string retStr;
	retCmd.SerializeToString(&retStr);
	pUser->sendProto(retStr.c_str(),retStr.size(),retCmd.ModID,retCmd.FunID);

	pUser->notifyGlobalLoginOut();
	pUser->notifyGameLoginOut();
	pUser->beginSelect();
	GateUserManager::getSingleton().removeUser(pUser);
	SelectUserManager::getSingleton().addUserByAccount(pUser);
}
