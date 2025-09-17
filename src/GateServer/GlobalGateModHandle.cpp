#include "GlobalGateModHandle.h"
#include "SvrInfo.h"
#include "GateServer.h"
#include "GateUserConnMgr.h"
#include "GateUserConnMgr.h"
#include "md5.h"
#include "GateUserManager.h"
#include "SelectUserManager.h"
#include "Global.pb.h"
#include "GlobalSvrClientMgr.h"
#include "Role.pb.h"
#include "Login.pb.h"
#include "KVPair.h"

extern GateServer * g_gateServer;

GlobalGateModHandle::GlobalGateModHandle()
{
	registerHandle(&GlobalGateModHandle::retSnapData);
	registerHandle(&GlobalGateModHandle::forwardProto2Client);
	registerHandle(&GlobalGateModHandle::userLoginGlobalFail);
	registerHandle(&GlobalGateModHandle::createRoleRet);
	registerHandle(&GlobalGateModHandle::gameSvrStartUp);
	registerHandle(&GlobalGateModHandle::broadcastProto2All);
	registerHandle(&GlobalGateModHandle::killOutUser);
	registerHandle(&GlobalGateModHandle::serverCfg);
	registerHandle(&GlobalGateModHandle::isNewAccount);
}

GlobalGateModHandle::~GlobalGateModHandle()
{

}

bool GlobalGateModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<GlobalGateModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void GlobalGateModHandle::retSnapData(TCPComponent * pCom,Global::stRetSnapData * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	SelectUserManager::getSingleton().refreshGateRoleInfo(recvCmd->account(),recvCmd);
}

void GlobalGateModHandle::forwardProto2Client(TCPComponent * pCom,Global::stForwardProto2Me * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	GateUser * pUser = GateUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		pUser->sendProto(recvCmd->data().data(),recvCmd->cmdlen(),recvCmd->modid(),recvCmd->funid());
	}
}

void GlobalGateModHandle::userLoginGlobalFail(TCPComponent * pCom,Global::stLoginGlobalFail * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	Athena::logger->trace("%s登录失败马上要断开连接",recvCmd->account().c_str());
	Login::LoginError errorCmd;
	if (Global::stLoginGlobalFail::HAS_LOGIN_IN){
		errorCmd.set_errortype(Login::LoginError::HAS_LOGIN_IN);
	}
	else {
		errorCmd.set_errortype(Login::LoginError::OTHER);
	}
	SelectUserManager::getSingleton().sendProto(recvCmd->account(),errorCmd);	
	GateUserManager::getSingleton().addImmLoginOutAccount(recvCmd->account());
	GateUserConnMgr::getSingleton().writeFinishClose(recvCmd->account());
}

void GlobalGateModHandle::createRoleRet(TCPComponent * pCom,Global::stCreateRoleResult *recvCmd)
{
	if (!recvCmd) {
		return ;
	}

	GateUser * pUser = SelectUserManager::getSingleton().getUserByAccount(recvCmd->account());
	if (pUser && recvCmd->charid()){
		stGateRoleInfo & roleInfo = pUser->m_gateRoleInfo[recvCmd->charid()];
		strncpy(roleInfo.m_account,recvCmd->account().c_str(),sizeof(roleInfo.m_account));
		roleInfo.m_charID = recvCmd->charid();
		strncpy(roleInfo.m_name,recvCmd->name().c_str(),sizeof(roleInfo.m_name));
		roleInfo.m_level = recvCmd->level();
		roleInfo.m_serverID = recvCmd->serverid();
		roleInfo.m_job = recvCmd->job();
		roleInfo.m_sex = recvCmd->sex();
	}
	
	Role::stSynRoleCreateTime synTimeCmd;
	synTimeCmd.set_createtime(recvCmd->createtime());	
	SelectUserManager::getSingleton().sendProto(recvCmd->account(),synTimeCmd);
	Role::CreateRoleResult createRet;
	createRet.set_retcode(recvCmd->retcode());
	createRet.set_charid(recvCmd->charid());
	createRet.set_job(recvCmd->job());
	createRet.set_sex(recvCmd->sex());
	createRet.set_name(recvCmd->name());
	createRet.set_level(recvCmd->level());
	createRet.set_createtime(recvCmd->createtime());
	SelectUserManager::getSingleton().sendProto(recvCmd->account(),createRet);
}

void GlobalGateModHandle::gameSvrStartUp(TCPComponent * pCom,Global::stGameSvrStartUp * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	SvrInfo * pSvrInfo =  g_gateServer->getSvrInfoByID(recvCmd->svrid());
	if (!pSvrInfo){
		g_gateServer->reloadSvrCfg();		
		return ;
	}

	pSvrInfo =  g_gateServer->getSvrInfoByID(recvCmd->svrid());
	if (!pSvrInfo){
		Athena::logger->error("[game服务器启动]收到game服务器启动消息后找到不到相关配置!");
		return;
	}

	g_gateServer->connectGame(*pSvrInfo);
}

void GlobalGateModHandle::broadcastProto2All(TCPComponent * pCom,Global::stBroadcastProto2AllUser * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	GateUserManager::getSingleton().sendProto2World(recvCmd->data().data(),recvCmd->cmdlen(),recvCmd->modid(),recvCmd->funid());
}

void GlobalGateModHandle::killOutUser(TCPComponent * pCom,Global::stKillUserOut *recvCmd)
{
	if (!recvCmd){
		return ;	
	}

	GateUser * pUser = GateUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	GateUserManager::getSingleton().addImmLoginOutAccount(pUser->m_account);
	GateUserConnMgr::getSingleton().disconnectAccount(pUser->m_account);
}

void GlobalGateModHandle::serverCfg(TCPComponent * pCom,Global::stNotifyGateSvrCfg * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	for (UINT16 i = 0;i < recvCmd->cfgpair_size();++i){
		const Global::stServerCfgPair  cfgPair = recvCmd->cfgpair(i);
		std::string key = cfgPair.key();
		Athena::global[key] = cfgPair.value();
		if (key == "loadStat"){
			g_gateServer->initLoadStat(Athena::global);
		}   
#ifdef _HDZ_DEBUG
		Athena::logger->trace("收到服务器配置:key=%s,value=%s",cfgPair.key().c_str(),cfgPair.value().c_str());
#endif
	}
	Athena::logger->trace("收到了global通知的服务器配置!");
}

void GlobalGateModHandle::isNewAccount(TCPComponent * pCom,Global::stIsNewAccount * recvCmd) {
	if (!recvCmd){
		return ;
    }
    
    Login::stNotifyIsNewAccount retCmd;
    retCmd.set_isnewacc(recvCmd->isnew());
    retCmd.set_account(recvCmd->account());
	SelectUserManager::getSingleton().sendProto(recvCmd->account(), retCmd);	
	Athena::logger->trace("发送给用户isNewAccount, %d %s", recvCmd->isnew(), recvCmd->account().c_str());
}
