#include "SortListModHandle.h"
#include "SvrInfo.h"
#include "RankMgr.h"
#include "Arena.h"
#include "One2OnePVP.h"
#include "DrawHeroScoreMgr.h"
#include "GloryfashionMgr.h"
#include "Expedition.h"
#include "EmeraldDreamMgr.h"
#include "EmeraldDreamCfg.h"
#include "Player.h"
#include "PlayerManager.h"
#include "HeroLeagueMgr.h"

SortListModHandle::SortListModHandle()
{
	registerHandle(&SortListModHandle::reqLevelSortList);
	registerHandle(&SortListModHandle::reqBattlePowerSortList);
	registerHandle(&SortListModHandle::reqCopymapSortList);
	registerHandle(&SortListModHandle::reqArenaSortList);
	registerHandle(&SortListModHandle::reqArenaPKList);
	registerHandle(&SortListModHandle::reqOne2OneSortList);
	registerHandle(&SortListModHandle::reqChallengeArenaUser);
	registerHandle(&SortListModHandle::reqLeitaiRank);
	registerHandle(&SortListModHandle::reqDrawHeroSortList);
	registerHandle(&SortListModHandle::reqOne2OnePVPMatch);
	registerHandle(&SortListModHandle::cancelOne2OnePVPMatch);
	registerHandle(&SortListModHandle::sendMySortInfo);
	registerHandle(&SortListModHandle::resetChallengeNum);
	registerHandle(&SortListModHandle::reqOpenSvrBPRank);
	registerHandle(&SortListModHandle::reqGloryfashionSortList);
	registerHandle(&SortListModHandle::reqExpeditionInfo);
	registerHandle(&SortListModHandle::reqExpedition);
	registerHandle(&SortListModHandle::reqExpeditionReward);
	registerHandle(&SortListModHandle::reqFastExpedition);
	registerHandle(&SortListModHandle::reqCancelExpedition);
	registerHandle(&SortListModHandle::reqExpeditionSummary);
	registerHandle(&SortListModHandle::reqEmeraldDreamSortList);
	registerHandle(&SortListModHandle::reqOpenHeroLeagueWindow);
	registerHandle(&SortListModHandle::reqQualifiedList);
	registerHandle(&SortListModHandle::reqHeroList);
}

SortListModHandle::~SortListModHandle()
{

}

bool SortListModHandle::handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<SortListModHandle,Player>::handle(pUser,pData,cmdLen,funID); 
}

void SortListModHandle::reqLevelSortList(Player * pUser,SortProto::stReqLevelSortList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}	

	CRankMgr::getSingleton().sendLevelSortList(pUser,recvCmd->fromrank());
}

void SortListModHandle::reqBattlePowerSortList(Player * pUser,SortProto::stReqBattlePowerSortList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CRankMgr::getSingleton().sendBattlePowerSortList(pUser,recvCmd->fromrank());
}

void SortListModHandle::reqCopymapSortList(Player * pUser,SortProto::stReqCopymapSortList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CRankMgr::getSingleton().sendCopymapSortList(pUser,recvCmd->fromrank());
}

void SortListModHandle::reqArenaSortList(Player * pUser,SortProto::stReqArenaSortList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	//return ;
	ArenaMgr::getSingleton().sendSortList(pUser,recvCmd->beginrank());
}

void SortListModHandle::reqArenaPKList(Player * pUser,SortProto::stReqArenaPKList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	ArenaMgr::getSingleton().sendPKList(pUser);
}

void SortListModHandle::reqOne2OneSortList(Player * pUser,SortProto::stReqOne2OnePVPSortList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	if (SortProto::eOne2OneMyRank == recvCmd->etype()){
		UINT16 beginRank = COne2OnePVPMgr::getSingleton().getMyRank(pUser->getID());	
		COne2OnePVPMgr::getSingleton().sendScoreSortList(pUser,beginRank,recvCmd->etype());
	}
	else {
		COne2OnePVPMgr::getSingleton().sendScoreSortList(pUser,recvCmd->beginrank(),recvCmd->etype());
	}
}

void SortListModHandle::reqChallengeArenaUser(Player * pUser, SortProto::stChallengeArenaUser * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	ArenaMgr::getSingleton().challege(pUser,recvCmd->charid());
}

void SortListModHandle::resetChallengeNum(Player * pUser,SortProto::stReqResetArenaChallengeNum *recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	UINT32 curNum = 0;
	stUserArenaInfo *pUserArenaInfo = ArenaMgr::getSingleton().getUserArenaInfo(recvCmd->charid());
	if (pUserArenaInfo){
		curNum = pUserArenaInfo->m_resetNum;
	}
	Global::stReqResetArenaChallengeNum cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_curresetnum(curNum);
	ArenaMgr::getSingleton().resetChallengeNum(pUser);
}


void SortListModHandle::reqLeitaiRank(Player * pUser,SortProto::stReqLeitaiSortList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CRankMgr::getSingleton().sendLeitaiSortList(pUser,recvCmd->beginrank());
}

void SortListModHandle::reqDrawHeroSortList(Player * pUser,SortProto::stReqDrawHeroSortList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CDrawHeroRankMgr::getSingleton().sendDrawHeroSortList(pUser,recvCmd->fromrank());
}

void SortListModHandle::reqOne2OnePVPMatch(Player * pUser,SortProto::stReqOne2OnePVPMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	COne2OnePVPMgr::getSingleton().addMatchUser(pUser);
}

void SortListModHandle::cancelOne2OnePVPMatch(Player * pUser,SortProto::stCancelOne2OnePVPMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	COne2OnePVPMgr::getSingleton().cancelMatch(pUser);
}

void SortListModHandle::sendMySortInfo(Player * pUser,SortProto::stReqMySortListInfo * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	SortProto::stRetMySortListInfo retCmd;

	retCmd.set_copymaprank(CRankMgr::getSingleton().getMyCopyMapRank(pUser->getID()));
	retCmd.set_battlepowerrank(CRankMgr::getSingleton().getMyBattlePowerRank(pUser->getID()));
	retCmd.set_arenapkrank(ArenaMgr::getSingleton().getMyRank(pUser->getID()));
	retCmd.set_one2onepvprank(COne2OnePVPMgr::getSingleton().getMyRank(pUser->getID()));
	pUser->sendProtoToMe(retCmd);
}

void SortListModHandle::reqOpenSvrBPRank(Player * pUser,SortProto::stReqOpenSvrBPRank * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CRankMgr::getSingleton().sendOpenSvrBPRank(pUser);
}

void SortListModHandle::reqGloryfashionSortList(Player * pUser,SortProto::stReqGloryfashionSortList * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	CGloryfashionRankMgr::getSingleton().sendGloryfashionSortList(pUser,recvCmd->fromrank());
}

void SortListModHandle::reqExpeditionInfo(Player * pUser, SortProto::stReqExpeditionInfo * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	Global::stExpeditionInfo retCmd;
	CExpedition * pExp = ExpeditionMgr::getSingleton().getExpedition(pUser->getID());
	if (pExp) {
		pExp->loadItems(retCmd);
	}
	retCmd.set_charid(pUser->getID());
	pUser->sendProto2Game(retCmd);
}

void SortListModHandle::reqExpedition(Player * pUser, SortProto::stReqExpedition * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	Global::stExpedition retCmd;
	retCmd.set_expeditionid(recvCmd->expeditionid());
	CExpedition * pExp = ExpeditionMgr::getSingleton().getExpedition(pUser->getID());
	if (pExp) {
		std::vector<UINT16> idVec;
		std::vector<UINT32> heroVec;
		pExp->loadIds(idVec);
		for (size_t i = 0; i < idVec.size(); i++) {
			retCmd.add_expeids(idVec[i]);
		}
		pExp->loadUsedHeros(heroVec);
		for (size_t i = 0; i < heroVec.size(); i++) {
			retCmd.add_usedhero(heroVec[i]);
		}
	}
	for (INT32 i = 0; i < recvCmd->heroids_size(); i++) {
		retCmd.add_heroids(recvCmd->heroids(i));
	}
	retCmd.set_charid(pUser->getID());
	pUser->sendProto2Game(retCmd);
}

void SortListModHandle::reqExpeditionReward(Player * pUser, SortProto::stReqExpeditionReward * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	Global::stExpeditionReward retCmd;
	retCmd.set_charid(pUser->getID());
	retCmd.set_expeditionid(recvCmd->expeditionid());
	CExpedition * pExp = ExpeditionMgr::getSingleton().getExpedition(pUser->getID());
	if (!pExp) {
		retCmd.set_expeditionid(0);
	} else {
		const CExpeditionItem * pItem = pExp->getExpedition(recvCmd->expeditionid());
		if (!pItem) {
			retCmd.set_expeditionid(0);
		} else {
			std::vector<stExpeditionHero> heroVec;
			((CExpeditionItem * )pItem)->getUsedHero(heroVec);
			for (size_t i = 0; i < heroVec.size(); i++) {
				Global::stExpeditionHero * pItem = retCmd.add_usedhero();
				pItem->set_id(heroVec[i].m_id);
				pItem->set_lvl(heroVec[i].m_lvl);
				pItem->set_power(heroVec[i].m_power);
			}
		}
	}
	pUser->sendProto2Game(retCmd);
}

void SortListModHandle::reqFastExpedition(Player * pUser, SortProto::stReqFastExpedition * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	Global::stFastExpedition retCmd;
	retCmd.set_charid(pUser->getID());
	retCmd.set_expeditionid(recvCmd->expeditionid());
	CExpedition * pExp = ExpeditionMgr::getSingleton().getExpedition(pUser->getID());
	if (!pExp) {
		retCmd.set_expeditionid(0);
	} else {
		const CExpeditionItem * pItem = pExp->getExpedition(recvCmd->expeditionid());
		if (!pItem) {
			retCmd.set_expeditionid(0);
		} else {
			retCmd.set_finishtime(((CExpeditionItem *)pItem)->getFinishTime());
		}
	}
	pUser->sendProto2Game(retCmd);
}

void SortListModHandle::reqCancelExpedition(Player * pUser, SortProto::stReqCancelExpedition * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	Global::stCancelExpedition retCmd;
	retCmd.set_charid(pUser->getID());
	retCmd.set_expeditionid(recvCmd->expeditionid());
	CExpedition * pExp = ExpeditionMgr::getSingleton().getExpedition(pUser->getID());
	if (!pExp) {
		retCmd.set_expeditionid(0);
	} else {
		const CExpeditionItem * pItem = pExp->getExpedition(recvCmd->expeditionid());
		if (!pItem) {
			retCmd.set_expeditionid(0);
		} else {
			retCmd.set_expeditionid(recvCmd->expeditionid());
			retCmd.set_finishtime(((CExpeditionItem *)pItem)->getFinishTime());
		}
	}
	pUser->sendProto2Game(retCmd);
}

void SortListModHandle::reqExpeditionSummary(Player * pUser, SortProto::stReqExpeditionSummary * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	Global::stExpeditionSummary retCmd;
	retCmd.set_charid(pUser->getID());
	CExpedition * pExp = ExpeditionMgr::getSingleton().getExpedition(pUser->getID());
	if (pExp) {
		pExp->getSummary(retCmd);
	}
	pUser->sendProto2Game(retCmd);
}

void SortListModHandle::reqEmeraldDreamSortList(Player * pUser, SortProto::stReqEmeraldDreamSortList * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	CEmeraldDreamMgr::getSingleton().sendEmeraldDreamSortList(pUser, recvCmd->fromrank());
}

void SortListModHandle::reqOpenHeroLeagueWindow(Player * pUser, SortProto::stReqOpenLeagueWindow *recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}
	
	CHeroLeagueMgr::getSingleton().openWindow(pUser);
}

void SortListModHandle::reqQualifiedList(Player * pUser,SortProto::stReqQualifiedList * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	CHeroLeagueMgr::getSingleton().sendQualifiedList(pUser,recvCmd);
}

void SortListModHandle::reqHeroList(Player * pUser,SortProto::stReqHeroList * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	CHeroLeagueMgr::getSingleton().sendHeroesList(pUser, recvCmd);
}
