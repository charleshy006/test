#include "TeamModHandle.h"
#include "SvrInfo.h"
// #include "Mail.h"
#include "Team.h"
// #include "FirstKillMgr.h"
// #include "RankMgr.h"
#include "GlobalSvrClient.h"

TeamModHandle::TeamModHandle()
{
	registerHandle(&TeamModHandle::realTimeSingleMatch);
	registerHandle(&TeamModHandle::cancelRealTimeSingleMatch);
	registerHandle(&TeamModHandle::cancelRealTimeTeamMatch);
}

TeamModHandle::~TeamModHandle()
{

}

bool TeamModHandle::handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<TeamModHandle,GameUser>::handle(pUser,pData,cmdLen,funID); 
}

void TeamModHandle::realTimeSingleMatch(GameUser * pUser,TeamProto::stReqRealTimeSingleMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	Global::stReqDisbandTeam cmd;
	cmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	//如果不是跨服，转发消息
	if (!g_gameServer->isPubGameServer())
	{
		pUser->sendData2PubGame();
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}
	
	Athena::logger->trace("[stReqRealTimeSingleMatch] charid=%d name=%s type=%d",pUser->getID(), pUser->getName(), (UINT32)recvCmd->etype());

	if (TeamProto::eRealTimePKType_KillAll == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().addMatchUser(pUser,Global::eRealTimePKType_KillAll);
	}
	else if (TeamProto::eRealTimePKType_Finish == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().addMatchUser(pUser,Global::eRealTimePKType_Finish);
	}
}

void TeamModHandle::cancelRealTimeSingleMatch(GameUser * pUser,TeamProto::stReqCancelRealTimeSingleMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	//如果不是跨服，转发消息
	if (!g_gameServer->isPubGameServer())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}
	
	Athena::logger->trace("[stReqCancelRealTimeSingleMatch] charid=%d name=%s type=%d",pUser->getID(), pUser->getName(), (UINT32)recvCmd->etype());

	if (TeamProto::eRealTimePKType_KillAll == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().removeMatchUser(pUser,Global::eRealTimePKType_KillAll);
	}
	else if (TeamProto::eRealTimePKType_Finish == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().removeMatchUser(pUser,Global::eRealTimePKType_Finish);
	}

	TeamProto::stCancelRealTimeSingleMatchResult retCmd;
	retCmd.set_success(true);
	pUser->sendProto2Me(retCmd);

	pUser->unreg();
	SAFE_DELETE(pUser);
}


void TeamModHandle::cancelRealTimeTeamMatch(GameUser * pUser,TeamProto::stReqCancelRealTimeTeamMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	//如果不是跨服，转发消息
	if (!g_gameServer->isPubGameServer())
	{
		pUser->sendProto2PubGame(*recvCmd);
		return;
	}

	Athena::logger->trace("[stReqCancelRealTimeTeamMatch] charid=%d name=%s type=%d",pUser->getID(), pUser->getName(), (UINT32)recvCmd->etype());

	
	if (TeamProto::eRealTimePKType_KillAll == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().removeMatchTeam(recvCmd->teamid(),Global::eRealTimePKType_KillAll);
	}
	else if (TeamProto::eRealTimePKType_Finish == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().removeMatchTeam(recvCmd->teamid(),Global::eRealTimePKType_Finish);
	}
}

