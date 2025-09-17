#include "GuildModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "GuildConfig.h"
#include "GlobalSvrClient.h"

extern GameServer * g_gameServer;

GuildModHandle::GuildModHandle()
{
	registerHandle(&GuildModHandle::createGuild);
	registerHandle(&GuildModHandle::joinGuild);
	registerHandle(&GuildModHandle::donate);
	registerHandle(&GuildModHandle::changeBadge);
}

GuildModHandle::~GuildModHandle()
{

}

bool GuildModHandle::handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<GuildModHandle,GameUser>::handle(pUser,pData,cmdLen,funID); 
}

void GuildModHandle::createGuild(GameUser *pUser,GuildProto::stReqCreateGuild * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	GuildProto::stCreateGuildError errorRet;
	if (pUser->getLevel() < CGuildConfig::getSingleton().createNeedLv()){//等级不足
		errorRet.set_result(GuildProto::CREATE_GUILD_ERROR_LV_LOW);
		pUser->sendProto2Me(errorRet);
		return;
	}

	if (pUser->m_guildID){//已经有团了 
		errorRet.set_result(GuildProto::CREATE_GUILD_ERROR_HAS_GUILD);
		pUser->sendProto2Me(errorRet);
		return;
	}

	
	if (GuildProto::stReqCreateGuild::eDiamond == recvCmd->etype()){
		if (!pUser->checkResource(eResource_Diamond,CGuildConfig::getSingleton().createNeedDiamond())){
			errorRet.set_result(GuildProto::CREATE_GUILD_ERROR_NO_DIAMOND);
			pUser->sendProto2Me(errorRet);
			return ;	
		}
	}
	else if (GuildProto::stReqCreateGuild::eGoldCoin == recvCmd->etype()){
		if (!pUser->checkResource(eResource_GoldCoin,CGuildConfig::getSingleton().createNeedGoldCoin())){
			errorRet.set_result(GuildProto::CREATE_GUILD_ERROR_NO_GOLDCOIN);
			pUser->sendProto2Me(errorRet);
			return ;
		}	
	}
	else {
		return ;
	}
	
	pUser->m_eCreateGuildRes = recvCmd->etype();

	Global::stCreateGuild cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_name(recvCmd->guildname());
	cmd.set_badge(recvCmd->badge());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GuildModHandle::joinGuild(GameUser * pUser,GuildProto::stReqJoinInGuild *recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	/*GuildProto::stJoinInGuildResult result;
	if (pUser->m_roleInfo.m_leaveGuildTime 
			&& (pUser->m_roleInfo.m_leaveGuildTime + LEAVE_GUILD_COOLDOWN > g_gameServer->getSec())){
		result.set_result(GuildProto::REQ_JOIN_GUILD_ERROR_CD);	
		pUser->sendProto2Me(result); 
		return;
	}*/

	Global::stForwardJoininGuild cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_guildid(recvCmd->guildid());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GuildModHandle::donate(GameUser *pUser,GuildProto::stReqDonate * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	GuildProto::stDonateResult retCmd;

	if (!pUser->m_guildID){
		retCmd.set_result(GuildProto::stDonateResult::eHAS_NO_GUILD);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	if ((GuildProto::stReqDonate::eType3 == recvCmd->type())&&(pUser->getCountByVip(eVipFun_GuildDonate) < 1)){
		retCmd.set_result(GuildProto::stDonateResult::eHAS_NO_VIPRIGHT);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	CGuildConfig::stDonateInfo * pDonateInfo =  CGuildConfig::getSingleton().getDonateInfo(recvCmd->type());	
	if (!pDonateInfo){
		retCmd.set_result(GuildProto::stDonateResult::eINNER_ERROR);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	if (pUser->m_countMgr.getDayRefreshValue(ECOUNT_GUILD_DONATE)){
		retCmd.set_result(GuildProto::stDonateResult::eHAS_DONATE);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->removeResource((eResource)pDonateInfo->m_costResType,pDonateInfo->m_costResNum,"公会捐赠")){
		retCmd.set_result(GuildProto::stDonateResult::eMONEY_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pUser->m_countMgr.addDayRefreshValue(ECOUNT_GUILD_DONATE,1);
	pUser->addResource(eResource_Guild_Contri,pDonateInfo->m_donateNum,"捐赠");
	//retCmd.set_result(GuildProto::stDonateResult::eSUCCESS);
	//pUser->sendProto2Me(retCmd);
	//Global::stAddGuildContribute cmd;
	//cmd.set_charid(pUser->getID());
	//cmd.set_contribute(pDonateInfo->m_addContribute);
	//cmd.set_todaydaycontribute(pDonateInfo->m_donateNum);
	//GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	
	Global::stGuildDonateSuccess cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_addcontribute(pDonateInfo->m_addContribute);
	cmd.set_todaycontribute(pDonateInfo->m_donateNum);
	cmd.set_donatetype(recvCmd->type());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	Game::stSendGuildLog webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_charid(pUser->getID());
	webCmd.set_action("捐赠");
	char szOpObj[20] = {0};
	snprintf(szOpObj,sizeof(szOpObj),"%d",recvCmd->type());
	webCmd.set_opobjid(szOpObj);
	webCmd.set_guildid(pUser->m_guildID);
	webCmd.set_level(pUser->getLevel());
	webCmd.set_viplv(pUser->getVipLv());
	webCmd.set_opid(pUser->m_roleInfo.m_opID);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void GuildModHandle::changeBadge(GameUser * pUser,GuildProto::stReqChangeBadge * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	GuildProto::stChangeBadge retCmd;
	if (!pUser->removeResource(eResource_Diamond,500,"更换徽章")){
		retCmd.set_result(GuildProto::stChangeBadge::eNot_Enough_Diamond);
		pUser->sendProto2Me(retCmd);
		return ;	
	}

	Global::stForwardChgBadge cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_badgeid(recvCmd->badgeid());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}
