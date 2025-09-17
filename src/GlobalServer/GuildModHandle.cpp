#include "GuildModHandle.h"
#include "SvrInfo.h"
#include "Guild.h"
#include "Team.h"
#include "Team.pb.h"
#include "GuildConfig.h"

GuildModHandle::GuildModHandle()
{
	registerHandle(&GuildModHandle::reqGuildList);
	registerHandle(&GuildModHandle::reqGuildDetail);
	registerHandle(&GuildModHandle::reqApplyList);
	registerHandle(&GuildModHandle::replyApplier);
	registerHandle(&GuildModHandle::reqLeaveGuild);

	registerHandle(&GuildModHandle::assignTitle);
	registerHandle(&GuildModHandle::cancelTitle);
	registerHandle(&GuildModHandle::giveLeader2Other);
	registerHandle(&GuildModHandle::fireGuild);
	registerHandle(&GuildModHandle::fireGuildMember);

	registerHandle(&GuildModHandle::chgSign);
	registerHandle(&GuildModHandle::reqMemberInfo);
	registerHandle(&GuildModHandle::cancelFireGuild);
	registerHandle(&GuildModHandle::cancelJoinIn);
	registerHandle(&GuildModHandle::reqLevelupHall);
	registerHandle(&GuildModHandle::reqLevelupPrayBuilding);
	registerHandle(&GuildModHandle::reqLevelupShopLv);
	registerHandle(&GuildModHandle::reqGuildTrace);
	registerHandle(&GuildModHandle::chgNotice);
	registerHandle(&GuildModHandle::guildPray);
	//registerHandle(&GuildModHandle::changeBadge);
	registerHandle(&GuildModHandle::intoGuildCopymap);
	registerHandle(&GuildModHandle::searchGuild);
	registerHandle(&GuildModHandle::openGuildCopymap);
	registerHandle(&GuildModHandle::reqDamageSortlist);
	registerHandle(&GuildModHandle::reqGuildCopySortlist);
	registerHandle(&GuildModHandle::reqEnterGuildCopymap);
	registerHandle(&GuildModHandle::startGuildCopymap);
	registerHandle(&GuildModHandle::teamInvite);

	registerHandle(&GuildModHandle::reqGuildManorFightWin);
	registerHandle(&GuildModHandle::reqGuildManorFightApplyWin);
	registerHandle(&GuildModHandle::applyGuildFight);
	registerHandle(&GuildModHandle::openManorFightWin);
	registerHandle(&GuildModHandle::manorFightFormation);
	registerHandle(&GuildModHandle::leaveManorFightWin);
	registerHandle(&GuildModHandle::enterManorFightTeam);
	registerHandle(&GuildModHandle::manorFightChgTeamPos);
	registerHandle(&GuildModHandle::reqLevelupBlackMarketLv);
}

GuildModHandle::~GuildModHandle()
{

}

bool GuildModHandle::handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<GuildModHandle,Player>::handle(pUser,pData,cmdLen,funID); 
}

void GuildModHandle::reqGuildList(Player * pUser,GuildProto::stReqGuild * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CGuildMgr::getSingleton().sendGuildList(pUser,recvCmd->rank());
}

void GuildModHandle::reqGuildDetail(Player * pUser,GuildProto::stReqGuildDetailInfo * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuildMgr::getSingleton().reqGuildDetail(pUser,recvCmd->guildid());
}

void GuildModHandle::reqApplyList(Player * pUser,GuildProto::stReqApplyJoinInList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return ;
	}
	pGuild->sendApplyList(pUser);
}

void GuildModHandle::replyApplier(Player * pUser,GuildProto::stReplyApplier * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	GuildProto::stReplyApplerError errorRet;
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if (pGuild){//该玩家已经有了,公会了
		pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
		if (pGuild){
			pGuild->delFromApplyList(recvCmd->charid());
		}
		errorRet.set_result(GuildProto::REPLY_ERROR_HAS_GUILD);
		pUser->sendProtoToMe(errorRet);
		return;    
	}
	pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		pGuild->replyApplier(pUser,recvCmd->charid(),recvCmd->agree()); 
	}
}

void GuildModHandle::reqLeaveGuild(Player * pUser,GuildProto::stReqLeaveGuild * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		pGuild->leaveGuild(pUser); 
	}
}

void GuildModHandle::assignTitle(Player * pUser,GuildProto::stAssignGuildTitle * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		pGuild->assignTitle(pUser,recvCmd->charid(),recvCmd->etitle());
	}
}

void GuildModHandle::cancelTitle(Player * pUser,GuildProto::stCancelTitle * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		pGuild->cancelTitle(pUser,recvCmd->charid());
	}
}

void GuildModHandle::giveLeader2Other(Player * pUser,GuildProto::stGiveLeader2Other * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		pGuild->giveLeader(pUser,recvCmd->charid());
	}
}

void GuildModHandle::fireGuild(Player * pUser,GuildProto::stFireGuild * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	
	GuildProto::stFireGuildError fireGroupError;

	CGuildMember *pMember =  pGuild->getGuildMemberByID(pUser->getID());
	if (!pMember){
		Athena::logger->error("解散公会失败,不是公会成员!");
		fireGroupError.set_result(GuildProto::FIRE_MEMBER_ERROR_NO_POWER);
		pUser->sendProtoToMe(fireGroupError);
		return ;
	}    

	if (!CGuildConfig::getSingleton().hasRight(pMember->getTitle(),CGuildConfig::eRight_Dissolve)){//没有权限 
		Athena::logger->error("解散公会不成功,没有权限!");
		fireGroupError.set_result(GuildProto::FIRE_MEMBER_ERROR_NO_POWER);
		pUser->sendProtoToMe(fireGroupError);
		return ;
	}    
	UINT32 guildID = pGuild->getID();
	pGuild = NULL;
	CGuildMgr::getSingleton().fireGuild(guildID);
	/*pGuild->setFireTime(g_globalServer->getSec()  + CGuildMgr::s_FIRE_GUILD_LASTTIME);
	stGuildDB dbData;
	pGuild->fillDBdata(dbData);
	CGuildMgr::getSingleton().updateGuild(dbData);*/

	/*GuildProto::stNotifyFireRemainTime cmd;
	cmd.set_remaintime(CGuildMgr::s_FIRE_GUILD_LASTTIME);
	pUser->sendProtoToMe(cmd);*/
}

void GuildModHandle::fireGuildMember(Player * pUser,GuildProto::stKillOutGroupMember * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}

	if (pGuild){
		pGuild->killOutMember(pUser,recvCmd->charid());
	}
}

void GuildModHandle::chgSign(Player * pUser,GuildProto::stChgGuildSign * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	pGuild->chgSign(pUser,recvCmd->sign().c_str());
}

void GuildModHandle::reqMemberInfo(Player * pUser,GuildProto::stReqGuildMemInfo * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}

	CGuildMember * pMem = pGuild->getGuildMemberByID(recvCmd->charid());
	if (!pMem){
		return;
	}
	pMem->sendDetailInfo(pUser);
}

void GuildModHandle::cancelFireGuild(Player * pUser,GuildProto::stCancelFireGuild * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}

	GuildProto::stCancelFireGuildResult cancelResult;
	if (!pGuild->isLeader(pUser->getID())){
		cancelResult.set_result(GuildProto::stCancelFireGuildResult::eHAS_NOT_POWER);
		pUser->sendProtoToMe(cancelResult);
		return;
	}
	
	pGuild->setFireTime(0);
	stGuildDB dbData;
	pGuild->fillDBdata(dbData);
	CGuildMgr::getSingleton().updateGuild(dbData);
	cancelResult.set_result(GuildProto::stCancelFireGuildResult::eSUCCESS);
	pUser->sendProtoToMe(cancelResult);
}

void GuildModHandle::cancelJoinIn(Player * pUser,GuildProto::stCancelJoinInGuild * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	GuildProto::stCancelJoinInGuildResult retCmd;
	retCmd.set_guildid(recvCmd->guildid());

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByID(recvCmd->guildid());
	if (!pGuild){
		retCmd.set_result(GuildProto::stCancelJoinInGuildResult::eFAIL);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	
	pGuild->delFromApplyList(pUser->getID());
	retCmd.set_result(GuildProto::stCancelJoinInGuildResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);
}

void GuildModHandle::reqLevelupHall(Player * pUser,GuildProto::stReqLevelUpHallLv * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	pGuild->levelupHall(pUser);
}

void GuildModHandle::reqLevelupPrayBuilding(Player * pUser,GuildProto::stReqPrayBuildingLv * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}

	pGuild->levelupPrayBuilding(pUser);
}

void GuildModHandle::reqLevelupShopLv(Player * pUser,GuildProto::stReqLevelUpShopBuildingLv * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	pGuild->levelupShop(pUser);
}

void GuildModHandle::reqGuildTrace(Player * pUser,GuildProto::stReqGuildTrace * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	pGuild->sendTraceList(pUser);
}

void GuildModHandle::chgNotice(Player * pUser,GuildProto::stChgNotice * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}

	pGuild->chgNotice(pUser,recvCmd->notice().c_str());
}

void GuildModHandle::guildPray(Player * pUser,GuildProto::stReqPray * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}

	pGuild->pray(pUser);
}

void GuildModHandle::changeBadge(Player * pUser,GuildProto::stReqChangeBadge * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	
	pGuild->changeBadge(pUser,recvCmd->badgeid());
}

void GuildModHandle::intoGuildCopymap(Player * pUser,GuildProto::stIntoGuildCopymap * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	GuildProto::stIntoGuildCopymapError retCmd;

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		retCmd.set_result(GuildProto::stIntoGuildCopymapError::eHAS_NOT_GUILD);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	
	Global::stEnterGuildCopymap cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_guildname(pGuild->getName());
	pUser->sendProto2Game(cmd);
}

void GuildModHandle::searchGuild(Player * pUser,GuildProto::stSearchGuild * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CGuildMgr::getSingleton().searchGuild(pUser,recvCmd->guildname().c_str());
}
	
void GuildModHandle::openGuildCopymap(Player * pUser,GuildProto::stReqOpenGuildCopymapWin * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	pGuild->sendCopymapWin(pUser);
}

void GuildModHandle::reqDamageSortlist(Player * pUser,GuildProto::stReqDamageSortlist * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	pGuild->sendDamageSortlist(pUser,recvCmd->beginrank());
}

void GuildModHandle::reqGuildCopySortlist(Player * pUser,GuildProto::stReqGuildCopymapScoreSortlist * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CGuildMgr::getSingleton().sendCopymapSortList(pUser,recvCmd->copymapid(),recvCmd->beginrank());
}

void GuildModHandle::reqEnterGuildCopymap(Player * pUser,GuildProto::stReqEnterGuildCopymap * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	
	pGuild->enterCopyMap(pUser,recvCmd->copymapid());
}

void GuildModHandle::startGuildCopymap(Player * pUser,GuildProto::stStartGuildCopymap * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	pGuild->startCopyMap(pUser,recvCmd->copymapid());
}

void GuildModHandle::teamInvite(Player * pUser,GuildProto::stInviteTeamInGuildChannel * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){
		return;
	}
	
	CTeam *pTeam = CTeamMgr::getSingleton().getTeamByCharID(pUser->getID());
	if (!pTeam){
		return ;
	}

	TeamProto::stBroadcastInviteJoinIn retCmd;
	retCmd.set_battlepower(pUser->m_battlePower);
	retCmd.set_charid(pUser->getID());
	retCmd.set_copymapid(pTeam->getCopyMapID());
	retCmd.set_job(pUser->m_job);
	retCmd.set_name(pUser->getName());
	retCmd.set_sex(pUser->m_sex);
	retCmd.set_teamid(pTeam->getID());
	retCmd.set_viplv(pUser->m_vipLv);
	retCmd.set_photoid(pUser->m_photoID);
	pGuild->sendProto2Mem(retCmd);
}

void GuildModHandle::reqGuildManorFightWin(Player * pUser,GuildProto::stReqGuildManorFightWin * recvCmd)
{
	if (!pUser || !recvCmd){
		return;
	}
	CManorInfoMgr::getSingleton().sendManorFightWinInfo(pUser);
}

void GuildModHandle::reqGuildManorFightApplyWin(Player * pUser,GuildProto::stReqOpenApplyManorFightWin * recvCmd)
{
	if (!pUser || !recvCmd){
		return;
	}
	CManorApplyMgr::getSingleton().sendManorApplyList(pUser,recvCmd->manorid());
}

void GuildModHandle::applyGuildFight(Player * pUser,GuildProto::stApplyManorFight * recvCmd)
{
	if (!pUser || !recvCmd){
		return;
	}
	CManorApplyMgr::getSingleton().reqChallengeManor(pUser,recvCmd->manorid(),recvCmd->contribute());
}

void GuildModHandle::openManorFightWin(Player * pUser,GuildProto::stReqManorFightWin * recvCmd)
{
	if (!pUser || !recvCmd){
		return;
	}

	CManorInfoMgr::getSingleton().sendManorFightInfo(pUser,recvCmd->manorid());
}

void GuildModHandle::manorFightFormation(Player * pUser,GuildProto::stReqManorFightFormation * recvCmd)
{
	if (!pUser || !recvCmd){
		return;
	}
	CManorInfoMgr::getSingleton().sendFormation(pUser,recvCmd->manorid(),recvCmd->formation());
}

void GuildModHandle::enterManorFightTeam(Player * pUser,GuildProto::stEnterManorFightTeam * recvCmd)
{
	if (!pUser || !recvCmd){
		return;
	}
	
	CManorInfoMgr::getSingleton().enterManorFightTeam(pUser,recvCmd->manorid(),recvCmd->formation(),recvCmd->teamid(),recvCmd->pos());
}

void GuildModHandle::leaveManorFightWin(Player * pUser,GuildProto::stNotifyLeaveManorFightWin * recvCmd)
{
	if (!pUser || !recvCmd){
		return;
	}

	CManorInfoMgr::getSingleton().userLeaveTeam(pUser);
}

void GuildModHandle::manorFightChgTeamPos(Player * pUser,GuildProto::stReqChgManorFightTeamPos * recvCmd)
{
	if (!pUser || !recvCmd){
		return;
	}
	
	CManorInfoMgr::getSingleton().chgTeamPos(pUser,recvCmd->destteamid(),recvCmd->destpos());
}

void GuildModHandle::reqLevelupBlackMarketLv(Player * pUser,GuildProto::stReqLevelUpBlackMarketLv * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (!pGuild){                                                                
		    return;                                                                  
	}                                                                            
	pGuild->levelupBlackMarket(pUser);
}
