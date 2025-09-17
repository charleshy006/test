#include "GateModHandle.h"
#include "SvrInfo.h"
#include "GlobalServer.h"
#include "Player.h"
#include "PlayerManager.h"
#include "AthenaNew.h"
#include "Callback.h"
#include "SqlOperation.h"
#include "MysqlDatabase.h"
#include "Log4cxxHelp.h"
#include "GlobalServer.h"
#include "RoleInfoManager.h"
#include "Global.pb.h"
#include "MiscModHandle.h"
#include "SortListModHandle.h"
#include "FriendModHandle.h"
#include "GuildModHandle.h"
#include "TeamModHandle.h"
#include "RoleModHandle.h"
#include "datacenter_log.h"
#include "log_singleton.h"
#include "KVPair.h"
#include "BlackWhiteMgr.h"
#include "Team.h"
#include "WorldBoss.h"
#include "One2OnePVP.h"

extern GlobalServer * g_globalServer;

GateModHandle::GateModHandle()
{
	m_protoModHandles[Proto::MISC_MOD] = ATHENA_NEW MiscModHandle();
	m_protoModHandles[Proto::MISC2_MOD] = ATHENA_NEW MiscModHandle();
	m_protoModHandles[Proto::SORTLIST_MOD] = ATHENA_NEW SortListModHandle();
	m_protoModHandles[Proto::FRIEND_MOD] = ATHENA_NEW FriendModHandle();	
	m_protoModHandles[Proto::GUILD_MOD] = ATHENA_NEW GuildModHandle();
	m_protoModHandles[Proto::TEAM_MOD] = ATHENA_NEW TeamModHandle();
	m_protoModHandles[Proto::ROLE_MOD] = ATHENA_NEW RoleModHandle();

	registerHandle(&GateModHandle::createRole);
	registerHandle(&GateModHandle::reqSnapData);
	registerHandle(&GateModHandle::reqLoadData);
	registerHandle(&GateModHandle::notifyLogout);
	registerHandle(&GateModHandle::gateForward);
	registerHandle(&GateModHandle::reLoginGlobal);
	registerHandle(&GateModHandle::autoConnect);
	registerHandle(&GateModHandle::clientLostConnect);
	registerHandle(&GateModHandle::askIsNewAccount);
}

GateModHandle::~GateModHandle()
{

}

bool GateModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<GateModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void GateModHandle::createRole(TCPComponent * pCom,Global::stCreateRole * createCmd)
{
	if (!pCom || !createCmd) {
		return ;
	}

	if (strlen(createCmd->name().c_str()) <= 0){
		Athena::logger->error("创建角色的时候名字没有输入任何内容");
		return ;
	}

	stCreateRoleInfo  createInfo;
	strncpy(createInfo.m_account,createCmd->account().c_str(),sizeof(createInfo.m_account));
	createInfo.m_level = 1;
	createInfo.m_job = createCmd->job();
	createInfo.m_sex = createCmd->sex();
	strncpy(createInfo.m_name,createCmd->name().c_str(),sizeof(createInfo.m_name));
	createInfo.m_headID = createCmd->headid();
	createInfo.m_faceID = createCmd->faceid();
	if(Athena::global["isHeFu"] == "true"){
		if(0 == createCmd->serverid()){
			Athena::logger->error("创建角色失败,serverID:%d", createCmd->serverid());
			return ;
		}
		createInfo.m_serverID = createCmd->serverid();
	}else{
		createInfo.m_serverID = (UINT32)atoi(Athena::global["serverID"].c_str());
	}

#ifdef _HDZ_DEBUG                                                   
	Athena::logger->trace("创建角色,serverID:%d",createInfo.m_serverID);    
#endif                                                              
	std::string timestamp = g_utc2StringFormat(g_globalServer->getSec());
	strncpy(createInfo.m_createTime,timestamp.c_str(),sizeof(createInfo.m_createTime));
	createInfo.m_opID = createCmd->opid();
	
	stReqCreateInfo reqCreateInfo;
	reqCreateInfo.m_gateID = createCmd->gateid();
	reqCreateInfo.m_deviceID = createCmd->deviceid();
	reqCreateInfo.m_ip = createCmd->clientip();
	reqCreateInfo.m_channelID = createCmd->channelid();
	reqCreateInfo.m_serverID = createCmd->serverid();

	RoleInfoManager::getSingleton().existInTbl(createInfo,reqCreateInfo);

}

void GateModHandle::reqSnapData(TCPComponent * pCom,Global::stReqSnapData * snapDataCmd)
{
	if (!pCom || !snapDataCmd) {
		return ;
	}
	
	Athena::logger->trace("GateModHandle::reqSnapData account=%s", snapDataCmd->account().c_str());
	Global::stLoginGlobalFail failCmd;
	failCmd.set_account(snapDataCmd->account());
	std::string ip = snapDataCmd->ip();

	if (CBlackWhiteMgr::getSingleton().isBlackName(ip)){//黑名单
		failCmd.set_errocode(Global::stLoginGlobalFail::FORBID_LOGIN);
		g_globalServer->getConnMgr().broadcastByID(snapDataCmd->gateid(),failCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("ip地址在黑名单中,登录失败.");
#endif
		return ;
	}

	if (CBlackWhiteMgr::getSingleton().isMaintenane() && (!CBlackWhiteMgr::getSingleton().isWhiteName(ip))){//维护状态只有白名单才可以进来
		failCmd.set_errocode(Global::stLoginGlobalFail::FORBID_LOGIN);
		g_globalServer->getConnMgr().broadcastByID(snapDataCmd->gateid(),failCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("正在维护中,只有白名单才可以登录!");
#endif
		return ;
	}
	RoleInfoManager::getSingleton().loadSelectRoleInfo(snapDataCmd->account(),snapDataCmd->gateid(),snapDataCmd->deviceid(),snapDataCmd->opid(), snapDataCmd->serverid());
}

void GateModHandle::reqLoadData(TCPComponent * pCom,Global::stReqLoadData * recvCmd)
{
	if (!pCom || !recvCmd) {
		return ;
	}
	Athena::logger->trace("GateModHandle::reqLoadData account=%s ",recvCmd->account().c_str());

	Player *pUser = PlayerManager::getSingleton().getUserByAccount(recvCmd->account());
	Global::stLoginGlobalFail  failCmd;
	failCmd.set_account(recvCmd->account());

	if (pUser){
		Athena::logger->trace("account=%s登录失败,已经在别的地方登录了",recvCmd->account().c_str());
		return ;
	}

	char where[128] = { 0 };
	bzero(where, sizeof(where));
	snprintf(where,sizeof(where), " ACCOUNT='%s' AND CHARID=%d ", recvCmd->account().c_str(),recvCmd->charid());
	stReqLoadInfo reqData;
	reqData.m_account = recvCmd->account();
	reqData.m_gameSvrID = recvCmd->gamesvrid();
	reqData.m_gateSvrID = recvCmd->gatesvrid();
	reqData.m_ip = recvCmd->ip();
	reqData.m_deviceID = recvCmd->deviceid(); 
	reqData.m_channelID = recvCmd->channelid();
	reqData.m_netIP = recvCmd->netip();

	Athena::QueryCallback<RoleInfoManager,stReqLoadInfo,UINT16> * pCallBack = ATHENA_NEW Athena::QueryCallback<RoleInfoManager,stReqLoadInfo,UINT16>(&RoleInfoManager::getSingleton(),&RoleInfoManager::loadRoleInfoCB,NULL,reqData,recvCmd->gatesvrid());
	CQuerySqlOperation * delayCallback = ATHENA_NEW CQuerySqlOperation(pCallBack);
	RoleInfoManager::getSingleton().loadRoleInfo(delayCallback,where);
	
	// RoleInfoManager::getSingleton().notifyLogin2GiftSvr(recvCmd->account(),recvCmd->charid());
}

void GateModHandle::notifyLogout(TCPComponent * pCom,Global::stNotifyLoginOut * loginOutCmd)
{
	if (!pCom || !loginOutCmd) {
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(loginOutCmd->charid());
	if (!pUser){
		pUser = PlayerManager::getSingleton().getUserByAccount(loginOutCmd->account());
	}

	if (!pUser){
		Athena::logger->error("[角色下线]通知角色下线时候找不到角色%d",loginOutCmd->charid());
		return;
	}
	/*--------------- 打生态日志 ----------------*/
	/*uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
	uuzu::logsdk::UserInfo userInfo;
	userInfo.level = pUser->m_level;
	userInfo.passport = pUser->getAccount(); //必须填写
	char szCharID[20] = { 0 };
	snprintf(szCharID,sizeof(szCharID),"%d",pUser->getID());
	userInfo.uid = szCharID;   //除了est没创角前，其他时候必须填写
	userInfo.clientip = pUser->getIP();
	char szOnlineTime[20] = {0};
	snprintf(szOnlineTime,sizeof(szOnlineTime),"%d",g_globalServer->getSec() - pUser->m_loginTime);
	logger.sendDataLog(userInfo,"", "off", szOnlineTime,"","","","","","login");*/
	 /*-------------------------------------------*/
	PlayerManager::getSingleton().removeUser(pUser);
	Athena::logger->trace("[玩家上线](account=%s,name=%s,id=%d)下线了玩家在线时长为 %d",pUser->getAccount().c_str(),pUser->getName(),pUser->getID(),g_globalServer->getSec() - pUser->m_loginTime);
	//注销
	pUser->unreg();
	SAFE_DELETE(pUser);
}

void GateModHandle::gateForward(TCPComponent * pCom,Global::stGateForwardProto * recvCmd)
{
	if (!pCom || !recvCmd) {
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!pUser){
		return;
	}
	
#ifdef _HDZ_DEBUG
	Athena::logger->trace("玩家name=%s,id=%d收到了协议modID=%d,funID=%d",pUser->getName(),pUser->getID(),recvCmd->modid(),recvCmd->funid());
#endif

	GlobalProtoMsgHandleBase  * pHandle = m_protoModHandles[recvCmd->modid()];

	if (pHandle){
		pHandle->handle(pUser,recvCmd->data().data(),recvCmd->cmdlen(),recvCmd->funid());	
	}
}

void GateModHandle::reLoginGlobal(TCPComponent * pCom,Global::stReloginGlobal * recvCmd)
{
	if (!recvCmd){
		return ;
	}	
	
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!pUser){
		return;
	}
	pUser->m_deviceID = recvCmd->device();
	pUser->setIP(recvCmd->ip().c_str());

	Global::stReloginGame cmd;
	cmd.set_account(recvCmd->account());
	cmd.set_charid(recvCmd->charid());
	cmd.set_device(recvCmd->device());
	cmd.set_ip(recvCmd->ip());
	pUser->sendProto2Game(cmd);
}

void GateModHandle::autoConnect(TCPComponent * pCom,Global::stPlayerAutoConnect * recvCmd)
{
	if (!recvCmd){
		return ;
	}	
	
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!pUser){
		return;
	}
	pUser->m_deviceID = recvCmd->device();
	pUser->setIP(recvCmd->ip().c_str());

	Global::stNotifyGameAutoConnect cmd;
	cmd.set_account(recvCmd->account());
	cmd.set_charid(recvCmd->charid());
	cmd.set_device(recvCmd->device());
	cmd.set_ip(recvCmd->ip());
	pUser->sendProto2Game(cmd);
}

void GateModHandle::clientLostConnect(TCPComponent * pCom,Global::stClientLostConnect * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = NULL;
	for (UINT16 i = 0;i < recvCmd->account_size();++i){
		PlayerManager::getSingleton().getUserByAccount(recvCmd->account(i));
		if(!pUser){
			return;
		}

		CTeamMgr::getSingleton().userOffline(pUser);
		CTeamMatchMgr::getSingleton().userOffline(pUser);
		COne2OnePVPMgr::getSingleton().cancelMatch(pUser);
		CWorldBossMgr::getSingleton().userOffline(pUser);
	}
}

void GateModHandle::askIsNewAccount(TCPComponent * pCom,Global::stAskIsNewAccount * recvCmd) {
	if (!recvCmd){
		return ;
	}
    bool bNew = RoleInfoManager::getSingleton().isNewAccount(recvCmd->account());
    Global::stIsNewAccount retCmd;
    retCmd.set_isnew(bNew);
    retCmd.set_account(recvCmd->account());
    g_globalServer->getConnMgr().broadcastByType(GATESERVER, retCmd);
}
