#include "TeamModHandle.h"
#include "SvrInfo.h"
#include "Mail.h"
#include "Team.h"
#include "FirstKillMgr.h"
#include "RankMgr.h"
#include "TblDataMgr.h"
#include "TblCopyMapBase.h"

TeamModHandle::TeamModHandle()
{
	registerHandle(&TeamModHandle::reqCreateTeam);
	registerHandle(&TeamModHandle::reqEnterTeam);
	registerHandle(&TeamModHandle::reqLeaveTeam);
	registerHandle(&TeamModHandle::fireTeam);
	registerHandle(&TeamModHandle::autoMatchTeam);
	registerHandle(&TeamModHandle::cancelAutoMatchTeam);
	registerHandle(&TeamModHandle::broadcastInviteCmd);
	registerHandle(&TeamModHandle::inviteJoinInTeam);
	registerHandle(&TeamModHandle::replyInviteJoinTeam);
	registerHandle(&TeamModHandle::enterCopyMap);
	registerHandle(&TeamModHandle::realTimeSingleMatch);
	registerHandle(&TeamModHandle::realTimeTeamMatch);
	registerHandle(&TeamModHandle::cancelRealTimeSingleMatch);
	registerHandle(&TeamModHandle::cancelRealTimeTeamMatch);
	registerHandle(&TeamModHandle::reqFirstKillInfo);
}

TeamModHandle::~TeamModHandle()
{

}

bool TeamModHandle::handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<TeamModHandle,Player>::handle(pUser,pData,cmdLen,funID); 
}

void TeamModHandle::reqCreateTeam(Player * pUser,TeamProto::stReqCreateTeam * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CTeamMgr::getSingleton().createTeam(pUser,recvCmd->copymapid());
}

void TeamModHandle::reqEnterTeam(Player * pUser,TeamProto::stReqEnterTeam * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CTeamMgr::getSingleton().enterTeam(pUser,recvCmd->teamid());
}

void TeamModHandle::reqLeaveTeam(Player * pUser,TeamProto::stReqLeaveTeam * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CTeamMgr::getSingleton().leaveTeam(pUser);
}

void TeamModHandle::fireTeam(Player * pUser,TeamProto::stReqFireTeam  *recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CTeamMgr::getSingleton().fireTeam(pUser);
}

void TeamModHandle::broadcastInviteCmd(Player * pUser,TeamProto::stReqBroadcastInviteCmd * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByCharID(pUser->getID());
	if (!pTeam){
		return ;
	}

	TeamProto::stBroadcastInviteJoinIn retCmd;
	retCmd.set_charid(pUser->getID());	
	retCmd.set_job(pUser->m_job);
	retCmd.set_sex(pUser->m_sex);
	retCmd.set_name(pUser->getName());
	retCmd.set_teamid(pTeam->getID());
	retCmd.set_viplv(pUser->m_vipLv);
	retCmd.set_battlepower(CRankMgr::getSingleton().getBattlePowerByCharID(pUser->getID()));
	retCmd.set_copymapid(pTeam->getCopyMapID());
	retCmd.set_photoid(pUser->m_photoID);
	PlayerManager::getSingleton().sendProto2Zone(pUser->getGateID(),retCmd);
}

void TeamModHandle::autoMatchTeam(Player * pUser,TeamProto::stReqAutoMatchTeam * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	stCopyMapBase * pCopyMapBase =  g_copyMapBaseMgr.get(recvCmd->copymapid());
	if (pCopyMapBase && pUser->m_level < pCopyMapBase->m_limitLv){
        Athena::logger->error("autoMatchTeam level limit!");
        return ;
    }
    CTeamMgr::getSingleton().autoMatch(pUser,recvCmd->copymapid());
}

void TeamModHandle::cancelAutoMatchTeam(Player * pUser,TeamProto::stCancelAutoMatchTeam * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CTeamMgr::getSingleton().cancelMatch(pUser,recvCmd->copymapid());
}

void TeamModHandle::inviteJoinInTeam(Player * pUser,TeamProto::stInviteJoinInTeam * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CTeamMgr::getSingleton().inviteJoinIn(pUser,recvCmd->charid(),recvCmd->pos(),recvCmd->etype());
}

void TeamModHandle::replyInviteJoinTeam(Player *pUser,TeamProto::stReplyJoinInTeam * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
		
	CTeamMgr::getSingleton().replyInvite(pUser,recvCmd->teamid(),recvCmd->agree(),recvCmd->pos(),recvCmd->etype(),recvCmd->timesuseout());
}

void TeamModHandle::enterCopyMap(Player * pUser,TeamProto::stReqEnterLv * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CTeamMgr::getSingleton().enterLv(pUser);
}

void TeamModHandle::reqFirstKillInfo(Player * pUser,TeamProto::stReqFirstKillInfoByID * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CFirstKillMgr::getSingleton().sendFirstKillInfo(pUser,recvCmd->levelid());
}

void TeamModHandle::realTimeSingleMatch(Player * pUser,TeamProto::stReqRealTimeSingleMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	if (TeamProto::eRealTimePKType_KillAll == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().addMatchUser(pUser,Global::eRealTimePKType_KillAll);
	}
	else if (TeamProto::eRealTimePKType_Finish == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().addMatchUser(pUser,Global::eRealTimePKType_Finish);
	}
}

void TeamModHandle::cancelRealTimeSingleMatch(Player * pUser,TeamProto::stReqCancelRealTimeSingleMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	if (TeamProto::eRealTimePKType_KillAll == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().removeMatchUser(pUser,Global::eRealTimePKType_KillAll);
	}
	else if (TeamProto::eRealTimePKType_Finish == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().removeMatchUser(pUser,Global::eRealTimePKType_Finish);
	}

	TeamProto::stCancelRealTimeSingleMatchResult retCmd;
	retCmd.set_success(true);
	pUser->sendProtoToMe(retCmd);
}

void TeamModHandle::realTimeTeamMatch(Player *pUser,TeamProto::stReqRealTimeTeamMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	//新的跨服匹配逻辑
	Global::eRealTimePKType type;
	if (TeamProto::eRealTimePKType_KillAll == recvCmd->etype()){
		type = Global::eRealTimePKType_KillAll;
	}
	else if (TeamProto::eRealTimePKType_Finish == recvCmd->etype()){
		type = Global::eRealTimePKType_Finish;
	}

	if (!CTeamMatchMgr::getSingleton().isOpen(type)){
		TeamProto::stRealTimePKNotOpen failCmd;
		failCmd.set_etype(recvCmd->etype());
		pUser->sendProtoToMe(failCmd);
		return ;
	}


	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(recvCmd->teamid());
	if (!pTeam)
	{
		return;
	}

	Global::stRealTimeTeamMatch cmd;
	cmd.set_etype(type);
	cmd.set_teamid(recvCmd->teamid());

	for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER;++i){
		cmd.add_memberid(pTeam->getCharID(i));
	}
	pUser->sendProto2Game(cmd);

	if (CTeamMgr::getSingleton().removeTeam(pTeam)){
		SAFE_DELETE(pTeam);
	}

	//旧的本服匹配逻辑
	// if (TeamProto::eRealTimePKType_KillAll == recvCmd->etype()){
	// 	CTeamMatchMgr::getSingleton().addMatchTeam(pUser,recvCmd->teamid(),Global::eRealTimePKType_KillAll);
	// }
	// else if (TeamProto::eRealTimePKType_Finish == recvCmd->etype()){
	// 	CTeamMatchMgr::getSingleton().addMatchTeam(pUser,recvCmd->teamid(),Global::eRealTimePKType_Finish);
	// }
}

void TeamModHandle::cancelRealTimeTeamMatch(Player * pUser,TeamProto::stReqCancelRealTimeTeamMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	if (TeamProto::eRealTimePKType_KillAll == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().removeMatchTeam(recvCmd->teamid(),Global::eRealTimePKType_KillAll);
	}
	else if (TeamProto::eRealTimePKType_Finish == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().removeMatchTeam(recvCmd->teamid(),Global::eRealTimePKType_Finish);
	}
}

