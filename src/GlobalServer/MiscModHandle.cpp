#include "MiscModHandle.h"
#include "SvrInfo.h"
#include "Mail.h"
#include "CaptureTreasure.h"
#include "WorldBoss.h"
#include "LimitTimePetCfg.h"
#include "DrawHeroScoreMgr.h"
#include "CompensationSys.h"
#include "GuardMgr.h"
#include "GuardCfg.h"
#include "PlayerManager.h"
#include "Role.pb.h"
#include "DevilCallCfg.h"
#include "FirstKillMgr.h"
#include "Expedition.h"
#include "json/reader.h"
#include "json/writer.h"
#include "md5.h"
#include "KVPair.h"
#include "HttpClient.h"
#include "UtilFunction.h"
#include "OperateActivityCfg.h"
#include "OperateActivity.h"
#include "DiscoverMgr.h"
#include "InviteCfg.h"
#include "InviteFriendMgr.h"
#include "HttpParser.h"
#include "HttpClient.h"

#include <getopt.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

MiscModHandle::MiscModHandle()
{
	registerHandle(&MiscModHandle::readMail);
	registerHandle(&MiscModHandle::fetchAttach);
	registerHandle(&MiscModHandle::openCaptureTreasureWin);
	registerHandle(&MiscModHandle::reqChallengeList);
	registerHandle(&MiscModHandle::reqBossWorldDamageSortList);
	registerHandle(&MiscModHandle::reqMatchWorldBossActivity);
	registerHandle(&MiscModHandle::reqOpenWorldBossWin);
	registerHandle(&MiscModHandle::cancelWorldBossMatch);
	registerHandle(&MiscModHandle::worldBossRaiseDamage);
	registerHandle(&MiscModHandle::queryDrawHeroInfo);
	registerHandle(&MiscModHandle::getDrawHeroScoreReward);
	registerHandle(&MiscModHandle::getDrawHeroRankReward);
	registerHandle(&MiscModHandle::reqCompensationList);
	registerHandle(&MiscModHandle::reqGotCompensation);
	registerHandle(&MiscModHandle::openGuardWin);
	registerHandle(&MiscModHandle::reqIntoGuardWin);
	registerHandle(&MiscModHandle::refreshGuardQuality);
	registerHandle(&MiscModHandle::reqCanHelpGuardFriend);
	registerHandle(&MiscModHandle::inviteFriendForGuard);
	registerHandle(&MiscModHandle::replyHelpGuard);
	registerHandle(&MiscModHandle::beginGuard);
	registerHandle(&MiscModHandle::reqInterceptInfos);
	registerHandle(&MiscModHandle::reqIntercept);
	registerHandle(&MiscModHandle::interceptResult);
	registerHandle(&MiscModHandle::reqGuardDetailInfo);
	registerHandle(&MiscModHandle::quickFinishGuard);
	registerHandle(&MiscModHandle::resetInterceptNum);
	registerHandle(&MiscModHandle::akeyfetchAttach);
	registerHandle(&MiscModHandle::loadAllDevilCall);
	registerHandle(&MiscModHandle::drawDevilCallReward);
	registerHandle(&MiscModHandle::doDevilCall);
	registerHandle(&MiscModHandle::doDevilCallSummary);
	registerHandle(&MiscModHandle::reqOperateActivity);
	registerHandle(&MiscModHandle::reqOperateActivityReward);
	registerHandle(&MiscModHandle::reqOpenDiscoverWindow);
	registerHandle(&MiscModHandle::reqPlayDice);
	registerHandle(&MiscModHandle::reqGetOrGiveUpTask);
	registerHandle(&MiscModHandle::reqGetIntegralReward);
	registerHandle(&MiscModHandle::reqRefreshNum);
	registerHandle(&MiscModHandle::reqGiveReward);
	registerHandle(&MiscModHandle::reqRedPointRuls);
	registerHandle(&MiscModHandle::ReqInviteFriend);
	registerHandle(&MiscModHandle::ReqDrawInviteFriend);
	registerHandle(&MiscModHandle::ReqUseInviteCode);
}

MiscModHandle::~MiscModHandle()
{

}

bool MiscModHandle::handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<MiscModHandle,Player>::handle(pUser,pData,cmdLen,funID); 
}

void MiscModHandle::readMail(Player * pUser,MiscProto::stReqReadMail * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;	
	}
	MailManager::getSingleton().readMail(pUser,recvCmd->id());
}

void MiscModHandle::fetchAttach(Player * pUser,MiscProto::stReqFetchAttach * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;	
	}
	MailManager::getSingleton().fetchAttach(pUser,recvCmd->id());
	return ;
}

void MiscModHandle::openCaptureTreasureWin(Player * pUser,MiscProto::stReqOpenCaptureTreasure * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CCaptureTreasureMgr::getSingleton().sendUserTreasure(pUser);
}

void MiscModHandle::reqChallengeList(Player * pUser,MiscProto::stReqChallengeUserList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CCaptureTreasureMgr::getSingleton().sendChallegeList(pUser,recvCmd->resid());
}

void MiscModHandle::reqBossWorldDamageSortList(Player * pUser,MiscProto::stReqWorldBossDamageRank * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CWorldBossMgr::getSingleton().sendDamageSortList(pUser,recvCmd->fromrank());
}

void MiscModHandle::reqMatchWorldBossActivity(Player * pUser,MiscProto::stReqBossWorldMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CWorldBossMgr::getSingleton().userEnterMatch(pUser);
}

void MiscModHandle::reqOpenWorldBossWin(Player * pUser,MiscProto::stReqOpenWorldBossWin * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CWorldBossMgr::getSingleton().openWin(pUser);
}

void MiscModHandle::cancelWorldBossMatch(Player * pUser,MiscProto::stCancelWorldBossMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CWorldBossMgr::getSingleton().cancelMatch(pUser);
}

void MiscModHandle::worldBossRaiseDamage(Player * pUser,MiscProto::stReqRaiseDamageInWorldBoss * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CWorldBossMgr::getSingleton().reqBuy(pUser);
}

void MiscModHandle::queryDrawHeroInfo(Player * pUser,MiscProto::stQueryDrawHeroActivity * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	MiscProto::stRetDrawHeroActivityInfo retCmd;
	LimitTimePetCfg::stActivityOpenInfo *pOpenInfo =  LimitTimePetCfg::getSingleton().getOpenInfo(g_globalServer->getSec());

	if (pOpenInfo != NULL){//活动开启
		UINT32 heroID = LimitTimePetCfg::getSingleton().getHeroID(pOpenInfo->m_activityID);
		retCmd.set_heroid(heroID);
		retCmd.set_begintime(pOpenInfo->m_beginTime);
		retCmd.set_endtime(pOpenInfo->m_endTime);
	}
	else {
		
	}
	stDrawHeroScoreInfo * pDrawHeroInfo = CDrawHeroScoreMgr::getSingleton().getDrawHeroScoreInfo(pUser->getID());
	if (pDrawHeroInfo){
		retCmd.set_gotrankreward(pDrawHeroInfo->m_gotRankReward);	
		std::set<UINT16>::iterator it = pDrawHeroInfo->m_rewardIDs.begin();
		for (;it != pDrawHeroInfo->m_rewardIDs.end();++it){
			retCmd.add_rewardids(*it);	
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void MiscModHandle::getDrawHeroScoreReward(Player * pUser,MiscProto::stReqDrawHeroScoreReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CDrawHeroScoreMgr::getSingleton().scoreReward(pUser,recvCmd->id());
}

void MiscModHandle::getDrawHeroRankReward(Player * pUser,MiscProto::stReqDrawHeroRankReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CDrawHeroScoreMgr::getSingleton().rankReward(pUser);
}

void MiscModHandle::reqCompensationList(Player * pUser,MiscProto::stReqCompensationList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CCompensationMgr::getSingleton().sendList2Me(pUser);
}


void MiscModHandle::reqGotCompensation(Player * pUser,MiscProto::stReqGotCompensation * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CCompensationMgr::getSingleton().gotCompensation(pUser,recvCmd->id());
}

void MiscModHandle::openGuardWin(Player * pUser,MiscProto::stReqOpenGuardWin * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CGuardMgr::getSingleton().openGuardWin(pUser);
}

void MiscModHandle::reqGuardDetailInfo(Player * pUser,MiscProto::stReqGuardDetailInfo * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuardMgr::getSingleton().sendGuardDetailInfo(pUser,recvCmd->charid());
}

void MiscModHandle::resetInterceptNum(Player * pUser,MiscProto::stResetInterceptNum * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	stGuardUserInfo *pUserInfo = CGuardMgr::getSingleton().getGuardUserInfo(recvCmd->charid());

	Global::stReqResetInterceptNum cmd;
	cmd.set_charid(recvCmd->charid());
	if (pUserInfo){
		cmd.set_resetnum(pUserInfo->m_resetNum);	
	}
	pUser->sendProto2Game(cmd);
}

void MiscModHandle::reqIntoGuardWin(Player * pUser,MiscProto::stReqGuard * recvCmd)
{
    	Athena::logger->trace("reqIntoGuardWin 1");

	if (!pUser || !recvCmd){
		return ;
	}
	CGuardMgr::getSingleton().sendGuardWin(pUser);
}

void MiscModHandle::refreshGuardQuality(Player *pUser,MiscProto::stReqRefreshGuardQuality * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	stGuardInfo *pGuardInfo = CGuardMgr::getSingleton().getGuardInfo(pUser->getID());

	if (!pGuardInfo){
		return;
	}

	Global::stReqRefreshGuardQuality cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_quality(pGuardInfo->m_quality);
	pUser->sendProto2Game(cmd);
}

void MiscModHandle::reqCanHelpGuardFriend(Player *pUser,MiscProto::stReqCanHelpGuardFriend * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_friendMgr.sendCanHelpGuardFriend(pUser);
}

void MiscModHandle::inviteFriendForGuard(Player *pUser,MiscProto::stInviteFriendForHelpGuard * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	stGuardUserInfo *pGuardUserInfo = CGuardMgr::getSingleton().getGuardUserInfo(recvCmd->charid());

		
	MiscProto::stInviteFriendForHelpGuardResult retCmd;
	if (pGuardUserInfo && (pGuardUserInfo->m_helpNum >= GuardCfg::getSingleton().getHelpNum())){
		retCmd.set_result(MiscProto::stInviteFriendForHelpGuardResult::eHelp_Num_Use_Out);
		pUser->sendProtoToMe(retCmd);
		return;
	}

	Player *pOther = PlayerManager::getSingleton().getUserByID(recvCmd->charid()); 	
	if (!pOther){
		retCmd.set_result(MiscProto::stInviteFriendForHelpGuardResult::eNot_Online);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	
	if (pOther->m_level <  CGuardMgr::s_NEED_LEVEL){
		Athena::logger->error("邀请好友协助护送失败,等级不足!");
		return ;
	}

	retCmd.set_result(MiscProto::stInviteFriendForHelpGuardResult::eSuccess);
	pUser->sendProtoToMe(retCmd);

	MiscProto::stAskFriendHelpGuard cmd;
	cmd.set_inviterid(pUser->getID());
	cmd.set_invitername(pUser->getName());
	pOther->sendProtoToMe(cmd);
}

void MiscModHandle::replyHelpGuard(Player * pUser,MiscProto::stReplyHelpGuard * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	stGuardInfo *pGuardInfo = CGuardMgr::getSingleton().getGuardInfo(recvCmd->charid());

	if (!pGuardInfo){
		Athena::logger->trace("还有没有护送记录,却收到回复");
		return;
	}
	
	MiscProto::stReplyHelpGuardResult retCmd;
	if (pGuardInfo->m_endTime > 0){//已经开始护送了
		retCmd.set_result(MiscProto::stReplyHelpGuardResult::eHas_Start);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	stGuardUserInfo *pGuardUserInfo = CGuardMgr::getSingleton().getGuardUserInfo(pUser->getID());
	if (!pGuardUserInfo){
		Athena::logger->error("同意协助护送时候错误,找不到改玩家的护送信息!");
		return ;
	}
	if (pGuardUserInfo && (pGuardUserInfo->m_helpNum >= GuardCfg::getSingleton().getHelpNum())){//次数已经用完
		retCmd.set_result(MiscProto::stReplyHelpGuardResult::eHelp_Num_Use_Out);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	
	if (pGuardUserInfo->m_bHelpGuarding){
		Athena::logger->error("同意协助失败,玩家已经在协助其它玩家了!");
		return;
	}

	if (pGuardInfo->m_helpFriendID){
		retCmd.set_result(MiscProto::stReplyHelpGuardResult::eHas_Friend_Help);
		pUser->sendProtoToMe(retCmd);
		return;
	}

	if (!recvCmd->agree()){//拒绝了
		Player *pInviter = PlayerManager::getSingleton().getUserByID(recvCmd->charid()); 	
		if (pInviter){
			MiscProto::stInviteFriendForHelpGuardResult retCmd1;
			retCmd1.set_result(MiscProto::stInviteFriendForHelpGuardResult::eRefuse);
			pInviter->sendProtoToMe(retCmd1);
		}
		return ;
	}
	
	pGuardUserInfo->m_helpNum += 1;
	pGuardUserInfo->m_bHelpGuarding = true;
	stGuardUserInfoDB dbData1;
	pGuardUserInfo->fillDBData(dbData1);
	CGuardMgr::getSingleton().updateGuardUserInfo(dbData1);

	pGuardInfo->m_helpFriendID = pUser->getID();
	stGuardInfoDB dbData;
	pGuardInfo->fillDBData(dbData);
	CGuardMgr::getSingleton().updateGuard(dbData);
	retCmd.set_result(MiscProto::stReplyHelpGuardResult::eSuccess);
	pUser->sendProtoToMe(retCmd);


	Player *pInviter = PlayerManager::getSingleton().getUserByID(recvCmd->charid());

	if (pInviter){
		MiscProto::stUserAgreeHelpGuard notifyCmd;
		notifyCmd.set_charid(pUser->getID());
		notifyCmd.set_name(pUser->getName());
		pInviter->sendProtoToMe(notifyCmd);
	}
}

void MiscModHandle::beginGuard(Player * pUser,MiscProto::stBeginGuard * recvCmd)
{
    	Athena::logger->trace("beginGuard 1");

	if (!pUser || !recvCmd){
		return ;
	}
	CGuardMgr::getSingleton().beginGuard(pUser);
}

void MiscModHandle::reqInterceptInfos(Player *pUser,MiscProto::stReqInterceptInfo * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuardMgr::getSingleton().sendInterceptInfos(pUser);	
}

void MiscModHandle::interceptResult(Player * pUser,MiscProto::stNotifyInterceptResult * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了上报拦截结果协议");
#endif
	if (Athena::global["checkPvP"] == "true")
	{
		return;
	}
	CGuardMgr::getSingleton().interceptResult(pUser,recvCmd->win());	
}

void MiscModHandle::reqIntercept(Player * pUser,MiscProto::stReqIntercept * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGuardMgr::getSingleton().interceptGuard(pUser,recvCmd->charid());
}

void MiscModHandle::quickFinishGuard(Player * pUser,MiscProto::stQuickFinishGuard * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CGuardMgr::getSingleton().quickFinish(pUser);
}

void MiscModHandle::akeyfetchAttach(Player * pUser,MiscProto::stReqAKeyFetchAttach * recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return;
	}

	MiscProto::stAKeyFetchAttachResult retCmd;
	if(recvCmd->id_size() <= 0)
	{
		retCmd.set_result(MiscProto::stAKeyFetchAttachResult::ATTACH_NOT_EXIST);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	for(UINT16 i = 0; i < recvCmd->id_size(); ++i)
	{
		MailManager::getSingleton().autofetchAttach(pUser,recvCmd->id(i));
	}

	retCmd.set_result(MiscProto::stAKeyFetchAttachResult::SUCCESS);
	pUser->sendProtoToMe(retCmd);
}

void MiscModHandle::loadAllDevilCall(Player * pUser, MiscProto::stReqAllDevilCall * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    UINT32 id = pUser->getID();
    CDevilCall * pDevil = DevilCallMgr::getSingleton().getDevilCall(id);
    if (!pDevil) {
        DevilCallMgr::getSingleton().addDevilCall(id);
        pDevil = DevilCallMgr::getSingleton().getDevilCall(id);
    }
    std::map<UINT32, UINT32> devilsMap;
    CDevilCallCfg::stDevilCallInfo dc;
    Global::stAllDevilCall cmd;
    Global::stDevilCallItem * pItem = NULL;
    CDevilCallCfg::getSingleton().getAllDevilCall(devilsMap);
    for (std::map<UINT32, UINT32>::iterator it = devilsMap.begin();
            it != devilsMap.end(); it++) {
        pItem = cmd.add_item();
        pItem->set_itemid(it->first);
        if (CFirstKillMgr::getSingleton().getRecord(it->second, dc)) {
            pItem->set_haspass(true);
            pItem->set_name1(dc.name1);
            pItem->set_charid1(dc.m_charID1);
            pItem->set_name2(dc.name2);
            pItem->set_charid2(dc.m_charID2);
            pItem->set_name3(dc.name3);
            pItem->set_charid3(dc.m_charID3);
        } else {
            pItem->set_haspass(false);
        }
        if (id == dc.m_charID1 || id == dc.m_charID2 || id == dc.m_charID3) {
            //在首杀名单中
            if (pDevil->getRewardStatus(it->first, CDevilCallCfg::Boss_Reward)
                    == MiscProto::stDevilCallItem::eNo_Reward) {
                pDevil->updateRewardStatus(it->first,
                        CDevilCallCfg::Boss_Reward, MiscProto::stDevilCallItem::eHas_Reward);
            }
        }
        //检查普通奖励
        if (pDevil->getRewardStatus(it->first,
                    CDevilCallCfg::Common_Reward) == MiscProto::stDevilCallItem::eNo_Reward) {
            pDevil->updateRewardStatus(it->first,
                    CDevilCallCfg::Common_Reward, MiscProto::stDevilCallItem::eHas_Reward);
        }
        pItem->set_bossdrawstatus(pDevil->getRewardStatus(
                    it->first, CDevilCallCfg::Boss_Reward));
        pItem->set_commondrawstatus(pDevil->getRewardStatus(
                    it->first, CDevilCallCfg::Common_Reward));
    }
    cmd.set_charid(pUser->getID());
	pUser->sendProto2Game(cmd);
}

void MiscModHandle::drawDevilCallReward(Player * pUser, MiscProto::stReqDevilCallReward * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    CDevilCall * pDevil = DevilCallMgr::getSingleton().getDevilCall(pUser->getID());
    if (!pDevil) {
        return;
    }
    std::map<UINT32, UINT32> devilsMap;
    Global::stDevilCallReward cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_bossid(recvCmd->bossid());
    cmd.set_rewardtype(recvCmd->rewardtype());
    cmd.set_status(pDevil->getRewardStatus(recvCmd->bossid(), recvCmd->rewardtype()));
    if (CFirstKillMgr::getSingleton().gotPass(
                CDevilCallCfg::getSingleton().getOpenCondition(recvCmd->bossid()))) {
        cmd.set_gotpass(1);
    } else {
        cmd.set_gotpass(0);
    }
	pUser->sendProto2Game(cmd);
}

void MiscModHandle::doDevilCall(Player * pUser, MiscProto::stReqDevilCall * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    CDevilCall * pDevil = DevilCallMgr::getSingleton().getDevilCall(pUser->getID());
    if (!pDevil) {
        return;
    }
    Global::stDevilCall cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_bossid(recvCmd->bossid());
    cmd.set_drawnum(pDevil->getDrawNum(recvCmd->bossid()));
    if (CFirstKillMgr::getSingleton().gotPass(
             CDevilCallCfg::getSingleton().getOpenCondition(recvCmd->bossid()))) {
        cmd.set_gotpass(1);
    } else {
        cmd.set_gotpass(0);
    }
	pUser->sendProto2Game(cmd);
}

void MiscModHandle::doDevilCallSummary(Player * pUser, MiscProto::stReqDevilCallSummary * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    UINT32 id = pUser->getID();
    CDevilCall * pDevil = DevilCallMgr::getSingleton().getDevilCall(id);
    if (!pDevil) {
        DevilCallMgr::getSingleton().addDevilCall(id);
        pDevil = DevilCallMgr::getSingleton().getDevilCall(id);
    }
    std::map<UINT32, UINT32> devilsMap;
    CDevilCallCfg::stDevilCallInfo dc;
    Global::stDevilCallSummary cmd;
    Global::stDevilCallItem * pItem = NULL;
    CDevilCallCfg::getSingleton().getAllDevilCall(devilsMap);
    for (std::map<UINT32, UINT32>::iterator it = devilsMap.begin();
            it != devilsMap.end(); it++) {
        pItem = cmd.add_item();
        pItem->set_itemid(it->first);
        if (CFirstKillMgr::getSingleton().getRecord(it->second, dc)) {
            pItem->set_haspass(true);
            pItem->set_charid1(dc.m_charID1);
            pItem->set_charid2(dc.m_charID2);
            pItem->set_charid3(dc.m_charID3);
        } else {
            pItem->set_haspass(false);
        }
        if (id == dc.m_charID1 || id == dc.m_charID2 || id == dc.m_charID3) {
            //在首杀名单中
            if (pDevil->getRewardStatus(it->first, CDevilCallCfg::Boss_Reward)
                    == MiscProto::stDevilCallItem::eNo_Reward) {
                pDevil->updateRewardStatus(it->first,
                        CDevilCallCfg::Boss_Reward, MiscProto::stDevilCallItem::eHas_Reward);
            }
        }
        //检查普通奖励
        if (pDevil->getRewardStatus(it->first,
                    CDevilCallCfg::Common_Reward) == MiscProto::stDevilCallItem::eNo_Reward) {
            pDevil->updateRewardStatus(it->first,
                    CDevilCallCfg::Common_Reward, MiscProto::stDevilCallItem::eHas_Reward);
        }
        pItem->set_bossdrawstatus(pDevil->getRewardStatus(
                    it->first, CDevilCallCfg::Boss_Reward));
        pItem->set_commondrawstatus(pDevil->getRewardStatus(
                    it->first, CDevilCallCfg::Common_Reward));
    }
    cmd.set_charid(pUser->getID());
	pUser->sendProto2Game(cmd);
}

void MiscModHandle::reqOperateActivity(Player * pUser, MiscProto::stReqOperateActvity * recvCmd){
	if (!pUser || !recvCmd){
		return ;
	}
    std::map<UINT8, std::list<UINT8> > idMap;
    COperateActivityCfg::getSingleton().loadActiveIds(g_globalServer->getSec(), idMap);
    OperateActivity* pActivity = OperateActivityMgr::getSingleton().getOperateActivity(pUser->getID());
    if (pActivity) {
        pActivity->sendActivity(pUser, idMap);
    } else {
        Athena::logger->error("[MiscModHandle] new error");
    }
}

void MiscModHandle::reqOperateActivityReward(Player * pUser, MiscProto::stReqOperateActvityReward * recvCmd){
	if (!pUser || !recvCmd){
		return ;
	}
    MiscProto::stRetOperateActvityReward retCmd;
    retCmd.set_activityid(recvCmd->activityid());
    retCmd.set_itemid(recvCmd->itemid());
    if (COperateActivityCfg::getSingleton().checkExpire(recvCmd->activityid(), g_globalServer->getSec())) {
        Athena::logger->error("[MiscModHandle] activityid %d expire", recvCmd->activityid());
        retCmd.set_res(MiscProto::stRetOperateActvityReward::ACTIVITY_EXPIRE);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    if (!COperateActivityCfg::getSingleton().isValidItem(recvCmd->activityid(), recvCmd->itemid())) {
        Athena::logger->error("[MiscModHandle] item id %d invalid ", recvCmd->itemid());
        retCmd.set_res(MiscProto::stRetOperateActvityReward::INVALID_ID);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    UINT32 condition = COperateActivityCfg::getSingleton().getCondition(recvCmd->activityid(), recvCmd->itemid());
    if (!condition) {
        Athena::logger->error("[MiscModHandle] not find condition");
        retCmd.set_res(MiscProto::stRetOperateActvityReward::INVALID_ID);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    OperateActivity* pActivity = OperateActivityMgr::getSingleton().getOperateActivity(pUser->getID());
    if (!pActivity) {
        Athena::logger->error("[MiscModHandle] new error");
        retCmd.set_res(MiscProto::stRetOperateActvityReward::INNER_ERROR);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    UINT32 conditionNum = 0;
    UINT8 status = 0;
    if (recvCmd->activityid() == COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Diamond_Lottery)) {
        pActivity->updateActivity(recvCmd->activityid(), 0);//init
    }
    if (!pActivity->loadItem(recvCmd->activityid(), recvCmd->itemid(), conditionNum, status)) {
        Athena::logger->error("[MiscModHandle] load error");
        retCmd.set_res(MiscProto::stRetOperateActvityReward::INVALID_ID);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    if (status > 0) {
        Athena::logger->error("[MiscModHandle] already get the reward");
        retCmd.set_res(MiscProto::stRetOperateActvityReward::REWARD_ALREAD_DRAW);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    UINT32 boxId = COperateActivityCfg::getSingleton().getBoxId(recvCmd->activityid(), recvCmd->itemid());
    if (!boxId) {
        Athena::logger->error("[MiscModHandle] not find condition");
        retCmd.set_res(MiscProto::stRetOperateActvityReward::INVALID_ID);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    if (recvCmd->activityid() == COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Diamond_Lottery)) {
        if (conditionNum + 1 != recvCmd->itemid()) {
            Athena::logger->error("[MiscModHandle] lottery reward : invalid item id %d", recvCmd->itemid());
            retCmd.set_res(MiscProto::stRetOperateActvityReward::INVALID_ID);
            pUser->sendProtoToMe(retCmd);
            return ;
        }
        Global::stOperateActivityLotteryReward gameCmd;
        gameCmd.set_charid(pUser->getID());
        gameCmd.set_itemid(recvCmd->itemid());
        gameCmd.set_cost(condition);
        gameCmd.set_boxid(boxId);
        g_globalServer->getConnMgr().broadcastByType(GAMESERVER, gameCmd);
        return ;
    }
    if (conditionNum < condition) {
        Athena::logger->error("[MiscModHandle] condition limit");
        retCmd.set_res(MiscProto::stRetOperateActvityReward::CONDITION_LIMIT);
        pUser->sendProtoToMe(retCmd);
        return ;
    }

    Global::stOperateActivityReward gameCmd;
    gameCmd.set_charid(pUser->getID());
    gameCmd.set_activityid(recvCmd->activityid());
    gameCmd.set_itemid(recvCmd->itemid());
    gameCmd.set_status(status);
    gameCmd.set_boxid(boxId);
    g_globalServer->getConnMgr().broadcastByType(GAMESERVER, gameCmd);
    OperateActivityMgr::getSingleton().updateActivity(pUser->getID());
}

void MiscModHandle::reqOpenDiscoverWindow(Player * pUser, MiscProto::stRepOpenDiscoverWindow *recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	CDiscoverMgr::getSingleton().sendActivityInfo(pUser);
}

void MiscModHandle::reqPlayDice(Player * pUser, MiscProto::stReqPlayDice * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	CDiscoverMgr::getSingleton().playDice(pUser);
}

void MiscModHandle::reqGetOrGiveUpTask(Player * pUser, MiscProto::stReqGetOrGiveUpTask * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	if(1 == recvCmd->tag()){//接任务
		CDiscoverMgr::getSingleton().getTask(pUser);
	}
	
	if(2 == recvCmd->tag()){//放弃任务
		CDiscoverMgr::getSingleton().giveUpTask(pUser);	
	}
}

void MiscModHandle::reqRefreshNum(Player * pUser, MiscProto::stReqRefreshNum *recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}
	CDiscoverMgr::getSingleton().refreshNum(pUser);
}

void MiscModHandle::reqGetIntegralReward(Player * pUser, MiscProto::stReqGetIntegralReward * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	CDiscoverMgr::getSingleton().getIntegralReward(pUser, recvCmd->rewardid());
}

void MiscModHandle::reqGiveReward(Player * pUser, MiscProto::stReqGiveReward * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	CDiscoverMgr::getSingleton().giveReward(pUser,recvCmd);
}

void MiscModHandle::reqRedPointRuls(Player * pUser, MiscProto::stReqRedPointRuls *recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	CDiscoverMgr::getSingleton().redPointRuls(pUser);
}

void MiscModHandle::ReqInviteFriend(Player * pUser, MiscProto::stReqInviteFriend *recvCmd) {
	if(!pUser || !recvCmd){
		return ;
	}
    InviteFriend * pFriend = CInviteFriendMgr::getSingleton().getInviteFriend(pUser->getAccount());
    if (!pFriend) {
        CInviteFriendMgr::getSingleton().addInviteFriend(pUser->getAccount(), pUser);
    } else {
        pFriend->sendData(pUser);
    }
}

void MiscModHandle::ReqDrawInviteFriend(Player * pUser, MiscProto::stReqDrawInviteFriend *recvCmd) {
	if(!pUser || !recvCmd){
		return ;
	}
    MiscProto::stRetDrawInviteFriend retCmd;
    retCmd.set_id(recvCmd->id());
    InviteFriend * pFriend = CInviteFriendMgr::getSingleton().getInviteFriend(pUser->getAccount());
    if (!pFriend) {
        retCmd.set_result(MiscProto::stRetDrawInviteFriend::eNot_Find_Record);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    const CInviteCfg::InviteReward * pRewardCfg = CInviteCfg::getSingleton().getInviteReward(recvCmd->id());
    if (!pRewardCfg) {
        retCmd.set_result(MiscProto::stRetDrawInviteFriend::eNo_Such_Id);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    if (pFriend->getInviteCount() < pRewardCfg->inviteNum) {
        retCmd.set_result(MiscProto::stRetDrawInviteFriend::eLess_Of_Invite_Num);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    if (pFriend->isDrawReward(recvCmd->id())) {
        retCmd.set_result(MiscProto::stRetDrawInviteFriend::eAlready_Get);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    Global::stReqDrawInviteReward gameCmd;
    gameCmd.set_charid(pUser->getID());
    gameCmd.set_boxid(pRewardCfg->rewardBox);
    gameCmd.set_id(recvCmd->id());
    gameCmd.set_account(pUser->getAccount());
    g_globalServer->getConnMgr().broadcastByType(GAMESERVER, gameCmd);
#ifdef _HDZ_DEBUG
    Athena::logger->trace("转发stReqDrawInviteReward");
#endif
}

void MiscModHandle::ReqUseInviteCode(Player * pUser, MiscProto::stReqUseInviteCode *recvCmd) {
	if(!pUser || !recvCmd){
		return ;
	}
    MiscProto::stRetUseInviteCode retCmd;
    InviteFriend * pFriend = CInviteFriendMgr::getSingleton().getInviteFriend(pUser->getAccount());
    if (!pFriend) {
        retCmd.set_result(MiscProto::stRetUseInviteCode::eNot_Find_Record);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    std::string code = recvCmd->code();
    if (code == pFriend->getCode()) {
        retCmd.set_result(MiscProto::stRetUseInviteCode::eUse_Self_Code);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    if (!CInviteFriendMgr::getSingleton().checkCode(code)) {
        retCmd.set_result(MiscProto::stRetUseInviteCode::eNo_Such_Code);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    if (pFriend->isInvited()) {
        retCmd.set_result(MiscProto::stRetUseInviteCode::eAlready_Get);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    if (pUser->m_level < CInviteCfg::getSingleton().getLimitLvl()) {
        retCmd.set_result(MiscProto::stRetUseInviteCode::eLevel_Limit);
        pUser->sendProtoToMe(retCmd);
        return ;
    }
    //这里区分是本服还是其他区服兑换码
    std::string gameId = CInviteFriendMgr::getGameId();
    std::string codeGameId = code.substr(code.length() - 4, 4);
#ifdef _HDZ_DEBUG
    Athena::logger->trace("self game id is %s, code game id is %s", gameId.c_str(), codeGameId.c_str());
#endif
    if (gameId == codeGameId) {
        //本服
        std::string codeOwner;
        if (!CInviteFriendMgr::getSingleton().getOwner(code, codeOwner)) {
            retCmd.set_result(MiscProto::stRetUseInviteCode::eNo_Such_Code);
            pUser->sendProtoToMe(retCmd);
            return ;
        }
        InviteFriend * pOwner = CInviteFriendMgr::getSingleton().getInviteFriend(codeOwner);
        if (!pOwner) {
            retCmd.set_result(MiscProto::stRetUseInviteCode::eNo_Such_Code);
            pUser->sendProtoToMe(retCmd);
            return ;
        }
        pOwner->addInviteCount();
        pFriend->setBeInvited(true);
        stInviteFriendInfoDB ownerDb, selfDb;
        pOwner->fillDBData(ownerDb);
        pFriend->fillDBData(selfDb);
        CInviteFriendMgr::getSingleton().updateRecord(ownerDb);
        CInviteFriendMgr::getSingleton().updateRecord(selfDb);
        Global::stReqDrawUseInviteReward gameCmd;
        gameCmd.set_charid(pUser->getID());
        gameCmd.set_boxid(CInviteCfg::getSingleton().getInviteRewardBox());
        g_globalServer->getConnMgr().broadcastByType(GAMESERVER, gameCmd);
        retCmd.set_result(MiscProto::stRetUseInviteCode::eSuccess);
        pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
        Athena::logger->trace("转发stReqDrawUseInviteReward");
#endif
    } else {
        //其他服
        UINT32 now = g_globalServer->getSec();
        std::stringstream ss;
        ss << pUser->getAccount()
            << pUser->getID()
            << recvCmd->code()
            << now
            << Athena::global["signKey"];
        char tokenBuf[1024] = { 0 };
        int  bufLen = 0;
        snprintf(tokenBuf, sizeof(tokenBuf) - 1, "%s", ss.str().c_str());
        bufLen = strlen(tokenBuf);
        char digest[64] = {0};
        utils::Md5(tokenBuf,bufLen,digest);
        utils::HexString::bin2asc(digest,16,digest,64,bufLen);
        digest[bufLen] = 0;
        std::string subDigest(&digest[3],24);

        ss.str("");
        ss << "user_account=" << pUser->getAccount()
            << "&user_id=" << pUser->getID()
            << "&active_code=" << recvCmd->code()
            << "&timestamp=" << now
            << "&sign=" << subDigest;
        CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setRequestType(CHttpRequest::kHttpGet);
        char buf[1024] = { 0 };
        snprintf(buf, sizeof(buf) - 1, "%sreqActiveCode?%s", Athena::global["ApiURL"].c_str(), ss.str().c_str());
#ifdef _HDZ_DEBUG
        Athena::logger->trace("final buf is %s", buf);
#endif
        pRequest->setUrl(buf);
        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
#ifdef _HDZ_DEBUG
                Athena::logger->trace("config data is %s", configData.c_str());
#endif
            }
        }
        SAFE_DELETE(pRes);
        Json::Value configJson;
        Json::Reader reader;
        std::string msg;
        if (reader.parse(configData, configJson)) {
            if (configJson["return_code"].isInt()) {
                if (!configJson["return_code"].asInt()) {
                    Global::stReqDrawUseInviteReward gameCmd;
                    gameCmd.set_charid(pUser->getID());
                    gameCmd.set_boxid(CInviteCfg::getSingleton().getInviteRewardBox());
                    g_globalServer->getConnMgr().broadcastByType(GAMESERVER, gameCmd);
                    retCmd.set_result(MiscProto::stRetUseInviteCode::eSuccess);
                    pUser->sendProtoToMe(retCmd);
                    pFriend->setBeInvited(true);
                    stInviteFriendInfoDB selfDb;
                    pFriend->fillDBData(selfDb);
                    CInviteFriendMgr::getSingleton().updateRecord(selfDb);
#ifdef _HDZ_DEBUG
                    Athena::logger->trace("转发stReqDrawUseInviteReward");
#endif
                    return ;
                }
            }
            if (configJson["return_msg"].isString()) {
                msg = configJson["return_msg"].asString();
            }
        }
        if (msg == std::string("no_such_code")) {
            retCmd.set_result(MiscProto::stRetUseInviteCode::eNo_Such_Code);
        } else {
            retCmd.set_result(MiscProto::stRetUseInviteCode::eInner_error);
        }
        pUser->sendProtoToMe(retCmd);
    }
}
