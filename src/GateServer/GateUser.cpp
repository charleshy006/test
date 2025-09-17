#include "GateUser.h"
#include "GateUserManager.h"
#include "Log4cxxHelp.h"
#include "SelectUserManager.h"
#include "md5.h"
#include "GateServer.h"
#include "GlobalPacketProcessor.h"
#include "PacketProcessor.h"
#include "GamePacketProcessor.h"
#include "PacketProcessor.h"
#include "GlobalSvrClientMgr.h"
#include "GameSvrClientMgr.h"
#include "GateUserConnMgr.h"
#include "Game.pb.h"
#include "Global.pb.h"

bool GateUser::s_forwardProtoTrace = false;
extern GateServer * g_gateServer ;

GateUser::GateUser(const char * account):m_gameState(GAME_STATE_INIT)
{
	m_gameID = 0;
	if (account){
		strncpy(m_account,account,sizeof(m_account));
	}
	m_enterSelectTime = g_gateServer->getSec();
	bzero(m_ip,sizeof(m_ip));
	m_lastSendPackTime = 0;
	m_sendPackErrorNum = 0;
	m_bReqClientTime = false;
	m_reqClientTimeSendPacketTime = g_gateServer->getSec();
	m_clientTime = 0;
	m_recvClientTime = 0;
	m_beginCheckPackNumTime = g_gateServer->getSec();
	m_recvPacketNum = 0;
	m_bNeedRelogin = false;
}

GateUser::~GateUser()
{
}

void GateUser::notifyGlobalLoginOut()
{
	Global::stNotifyLoginOut cmd;
	cmd.set_charid(this->getID());
	cmd.set_account(m_account);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GateUser::notifyGameLoginOut()
{
	Game::stKillOutUser  cmd;
	cmd.set_charid(this->getID());
	GameSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GateUser::terminate() 
{
	 GateUserConnMgr::getSingleton().disconnectAccount(m_account);
}

bool GateUser::initCharInfo(UINT32 charID)
{
	std::map<UINT32,stGateRoleInfo>::iterator it = m_gateRoleInfo.find(charID);
	if (it != m_gateRoleInfo.end()) {
		this->setID(it->second.m_charID);
		this->setName(it->second.m_name);
		return true;
	}
	else {
		return false;
	}
}

void GateUser::unreg(bool force)
{
	if (isGameState(GAME_STATE_UNREG)){//已经进行过注销了
		return;
	}
	UINT8  oldState = m_gameState;

	setGameState(GAME_STATE_UNREG);
	Athena::logger->debug("[%s结束]:name=%s, account=%s, charid=%u",force? "被迫":"",this->getName(), this->m_account, this->getID());
	
	GateUserManager::getSingleton().removeUser(this);
	SelectUserManager::getSingleton().removeUserByAccount(this->m_account);
	
	if (oldState == GAME_STATE_INIT){//如果是初始状态,说明其它服务器没有登录
		notifyGlobalLoginOut();
	}
	else if (GAME_STATE_SELECT == oldState){
		notifyGlobalLoginOut();	
	}
	else if (GAME_STATE_PLAY == oldState
			|| GAME_STATE_ENTER == oldState){
		notifyGlobalLoginOut();
		notifyGameLoginOut();
	}
}

extern GateServer * g_gateServer;

bool GateUser::beginSelect()
{
	setGameState(GAME_STATE_SELECT);
	m_enterSelectTime = g_gateServer->getSec();
	return true;
}

bool GateUser::sendProto(const void* pstrProto, const unsigned int cmdLen,UINT8 modID,UINT8 funID,const bool hasPacked)
{
	if (!hasPacked){
	}
#ifdef _HDZ_DEBUG
	//Athena::logger->trace("account=%s收到了协议(mod=%d,funID=%d)",m_account,modID,funID);
#endif
	return  GateUserConnMgr::getSingleton().broadcastByID(m_account,modID,funID,cmdLen,pstrProto);;
}

void GateUser::checkReceviePack(const PacketHeader *pHeader)
{
	if (!pHeader){
		return ;
	}
	return ;

	UINT32 curClientTime = m_clientTime + (g_gateServer->getMSec() > m_recvClientTime  ? g_gateServer->getMSec() - m_recvClientTime  : 0);
	if (pHeader->m_sendTime > curClientTime + s_NETWORK_DELAY){
		++m_sendPackErrorNum;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("(account:%s,name:%s,id:%d)检测到错误的次数:%d,收到的客户端发送过来的时间:pHeader->m_sendTime:%d,m_clientTime:%d,curClientTime:%d,m_recvClientTime:%llu,g_gateServer->getMSec():%llu",m_account,getName(),getID(),m_sendPackErrorNum,pHeader->m_sendTime,m_clientTime,curClientTime,m_recvClientTime,g_gateServer->getMSec());
#endif
	}

	if (m_sendPackErrorNum > s_MAX_SENDPACKET_ERROR_NUM){//切断
		m_sendPackErrorNum = 0;
		Athena::logger->error("(account:%s,name:%s,id:%d)使用了加速器要切断",m_account,getName(),getID());
		GateUserManager::getSingleton().addImmLoginOutAccount(m_account);
		GateUserConnMgr::getSingleton().disconnectAccount(m_account);
		m_bNeedRelogin = true;
	}
}

void GateUser::timer()
{
	if (m_bReqClientTime){//已经发包了
		if (m_reqClientTimeSendPacketTime + s_REQ_CLIENT_TIME_MAX_WAITTIME  < g_gateServer->getSec()){//已经超时了.要切断
			Athena::logger->error("(account:%s,name:%s,id:%d) 玩家请求了客户端时间超时了还没有返回,要切断玩家",m_account,getName(),getID());
			//m_bNeedRelogin = true;
			//GateUserManager::getSingleton().addImmLoginOutAccount(m_account);
			//GateUserConnMgr::getSingleton().disconnectAccount(m_account);
			m_bReqClientTime = false;
			return ;		
		}
	}
	
	if (isGameState(GAME_STATE_PLAY)){//游戏状态
		if (m_reqClientTimeSendPacketTime + s_REQ_CLIENT_TIME_INTERVAL < g_gateServer->getSec()){//到时间了要请求客户端时间了
			m_reqClientTimeSendPacketTime = g_gateServer->getSec();
			m_bReqClientTime = true;
			Login::stReqClientTime cmd;
			cmd.set_curtime(1);
			std::string retStr;
			cmd.SerializeToString(&retStr);
			sendProto(retStr.c_str(),retStr.size(),cmd.ModID,cmd.FunID);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("(account:%s,name:%s,id:%d) 玩家向客户端请求了时间",m_account,getName(),getID());
#endif
		}	
	}

	if (m_beginCheckPackNumTime + s_CHECK_PACKET_AMOUNT_INTERVAL < g_gateServer->getSec()){
		m_beginCheckPackNumTime = g_gateServer->getSec();
		m_sendPackErrorNum = 0;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("(account:%s,name:%s,id:%d)时间到了可以重置错误数量",m_account,getName(),getID());
#endif
	}
}
