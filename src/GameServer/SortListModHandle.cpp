#include "SortListModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "Global.pb.h"
#include "GlobalSvrClient.h"
#include "ArenaConfig.h"
#include "MiscCfgMgr.h"
#include "ExpeditionConfig.h"
#include "GameEventNotify.h"
#include "TblRoleBase.h"
#include "EmeraldDreamCfg.h"
#include <cmath>
#include "One2OnePVP.h"
#include "DragonNest.h"
#include "CopyMap.h"
#include "MapMgr.h"
#include "StoneCopyMapCfg.h"

extern GameServer * g_gameServer;

SortListModHandle::SortListModHandle()
{
	registerHandle(&SortListModHandle::changeLeiTaiUser);
	registerHandle(&SortListModHandle::openLeitaiWindow);
	registerHandle(&SortListModHandle::leitaiResult);
	registerHandle(&SortListModHandle::selectLeitaiMode);
	registerHandle(&SortListModHandle::resetLeitaiInfo);
	registerHandle(&SortListModHandle::cancelGoOut);
	registerHandle(&SortListModHandle::moveHero);
	registerHandle(&SortListModHandle::commitArenaResult);
	registerHandle(&SortListModHandle::enterLeitaiScene);
	registerHandle(&SortListModHandle::refreshLeitaiBuff);
	registerHandle(&SortListModHandle::reqRefreshBuffTime);
	registerHandle(&SortListModHandle::clearArenaCD);
	registerHandle(&SortListModHandle::reqExpeditionInfo);
	registerHandle(&SortListModHandle::reqExpedition);
	registerHandle(&SortListModHandle::reqExpeditionReward);
	registerHandle(&SortListModHandle::reqFastExpedition);
	registerHandle(&SortListModHandle::reqCancelExpedition);
	registerHandle(&SortListModHandle::reqExpeditionSummary);
	registerHandle(&SortListModHandle::reqEnterEmeraldDream);
	registerHandle(&SortListModHandle::reqResetFightNum);
	registerHandle(&SortListModHandle::reqSurplusNum);
	registerHandle(&SortListModHandle::reqOne2OnePVPMatch);
	registerHandle(&SortListModHandle::cancelOne2OnePVPMatch);
	registerHandle(&SortListModHandle::reqHeroLeagueMatch);
	registerHandle(&SortListModHandle::cancelHeroLeagueMatch);
	registerHandle(&SortListModHandle::reqReqMenberInfo);
	registerHandle(&SortListModHandle::reqWorskip);
	registerHandle(&SortListModHandle::reqRegisterNum);
	registerHandle(&SortListModHandle::reqPubLogIn);
	registerHandle(&SortListModHandle::reqPubLogOut); 
	registerHandle(&SortListModHandle::reqHeroLeagueRedPoint);
	registerHandle(&SortListModHandle::reqDragonNest);
	registerHandle(&SortListModHandle::reqChallengeDragonNest);
	registerHandle(&SortListModHandle::reqResetChallengeDragonNest);
	registerHandle(&SortListModHandle::reqStoneCopyMapInfo);
	registerHandle(&SortListModHandle::reqChallengeStoneCopyMap);
	registerHandle(&SortListModHandle::reqResetStoneCopyMap);
}

SortListModHandle::~SortListModHandle()
{

}

bool SortListModHandle::handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<SortListModHandle,GameUser>::handle(pUser,pData,cmdLen,funID); 
}

void SortListModHandle::changeLeiTaiUser(GameUser * pUser,SortProto::stChallegeLeitaiUser * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	return ;	
	UINT32 pkRound = pUser->m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_ROUND);
	if (pkRound >= CLeitaiMgr::s_MAX_ROUND_NUM){//已经打完了
		return ;
	}

	Global::stUserLeitaiChallenge cmd;
	cmd.set_round(pkRound + 1);
	cmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void SortListModHandle::openLeitaiWindow(GameUser * pUser,SortProto::stOpenLeitaiWindow * recvCmd)
{
	if (!pUser ||!recvCmd){
		return ;
	}
	
	if (pUser->getLevel() < 37){
		return ;
	}

	pUser->m_leitaiMgr.sendLeitaiInfo();
}

void SortListModHandle::leitaiResult(GameUser * pUser,SortProto::stSendLeitaiResult * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}

	//暂时不开服务器校验
	// if (Athena::global["checkPvP"] == "true")
	// {
	// 	return;
	// }

	/*if (pUser->m_leitaiMgr.getUserHP() < recvCmd->curuserhp()){//比原来的血量高不正确
		return ;
	}*/
	
	Game::stSendModuleLog  webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_charid(pUser->getID());
	webCmd.set_level(pUser->getLevel());
	webCmd.set_module("荣耀宝座");
	webCmd.set_opid(pUser->m_roleInfo.m_opID);
	webCmd.set_othercharid(g_int2string(pUser->m_levelMgr.getLeitaiCharID()));
	webCmd.set_viplv(pUser->getVipLv());

	/*if (!pUser->m_levelMgr.finishLeitai(0)){
		return ;
	}*/

	if (recvCmd->curuserhp() <= 0){//失败了
		webCmd.set_result(false);
		g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

		SortProto::stRetSendLeitaiResult retCmd;
		retCmd.set_result(SortProto::stRetSendLeitaiResult::SUCCESS);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	else {
		pUser->m_leitaiMgr.setUserHP(recvCmd->curuserhp());
		for (UINT16 i = 0;i < recvCmd->herohp_size();++i){
			const SortProto::stSynHeroHP & heroHP = recvCmd->herohp(i);	
			pUser->m_leitaiMgr.setHeroHP(heroHP.heroid(),heroHP.curhp());
		}
	}
	
	webCmd.set_result(false);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

	if (pUser->m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_ROUND) < CLeitaiMgr::s_MAX_ROUND_NUM){
		pUser->m_levelMgr.finishLeitai(0);
	}
	else {
		SortProto::stRetSendLeitaiResult retCmd;
		retCmd.set_result(SortProto::stRetSendLeitaiResult::SUCCESS);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pUser->m_leitaiMgr.setBuffID(ArenaConfig::getSingleton().randomLeitaiBuffID(pUser->m_leitaiMgr.getPKMode() == SortProto::eLeitaiMode_Normal,true,1));
	pUser->m_leitaiMgr.setChallengeCharID(0);
	pUser->m_countMgr.addDayRefreshValue(ECOUNT_LEITAI_ROUND,1);
	pUser->m_countMgr.addDayRefreshValue(ECOUNT_LEITAI_USETIME,recvCmd->usetime());

	if (pUser->m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_ROUND) >= CLeitaiMgr::s_MAX_ROUND_NUM){//轮次打完了
		if (SortProto::eLeitaiMode_Difficult == pUser->m_leitaiMgr.getPKMode()){//困难模式,才排名
			Global::stUpLeitaiUseTime  cmd;
			cmd.set_charid(pUser->getID());
			cmd.set_usetime(pUser->m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_USETIME));
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		}
	}
}

void SortListModHandle::selectLeitaiMode(GameUser *pUser,SortProto::stSelectLeiTaiMode * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	
	pUser->m_leitaiMgr.setPKMode(recvCmd->mode());
	SortProto::stSelectLeiTaiModeResult retCmd;
	retCmd.set_result(SortProto::stSelectLeiTaiModeResult::eSuccess);
	pUser->sendProto2Me(retCmd);
	pUser->m_leitaiMgr.sendLeitaiInfo();
}



void SortListModHandle::resetLeitaiInfo(GameUser * pUser,SortProto::stResetLeitaiInfo * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	pUser->m_leitaiMgr.resetInfo();
}

void SortListModHandle::moveHero(GameUser * pUser,SortProto::stLeitaiMoveHero2BattlePos * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	
	pUser->m_leitaiMgr.moveHeroPos(recvCmd->heroid(),recvCmd->pos());
}

void SortListModHandle::cancelGoOut(GameUser *pUser,SortProto::stLeitaiCancelGoOutForBattle * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	
	pUser->m_leitaiMgr.cancelGoOut(recvCmd->pos());
}

void SortListModHandle::commitArenaResult(GameUser * pUser,SortProto::stCommitArenaResult * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	
	pUser->m_levelMgr.finishArena(ARENA_PK_LEVELID,recvCmd->win());
}

void SortListModHandle::enterLeitaiScene(GameUser * pUser,SortProto::stReqEnterLeitaiScene * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	pUser->m_levelMgr.enterLeitaiLv(pUser->m_leitaiMgr.getPKMode());
}

void SortListModHandle::refreshLeitaiBuff(GameUser * pUser,SortProto::stReqResetLeitaiBuff * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	pUser->m_leitaiMgr.refreshBuff();
}

void SortListModHandle::reqRefreshBuffTime(GameUser * pUser,SortProto::stReqBuffResetTime * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	pUser->m_leitaiMgr.checkRefreshBuffTime();
	pUser->m_leitaiMgr.sendRefreshBuffTime();
}

void SortListModHandle::clearArenaCD(GameUser * pUser,SortProto::stReqClearArenaCD * recvCmd)
{
	if (!pUser||!recvCmd){
		return ;
	}
	
	SortProto::stClearArenaCDResult retCmd;
	if (!pUser->removeResource(eResource_Diamond,CMiscCfgMgr::getSingleton().getNeedDiamond(),"清除竞技场cd")){
		retCmd.set_result(SortProto::stClearArenaCDResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	Global::stNotifyClearArenaCD cmd;
	cmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void SortListModHandle::reqExpeditionInfo(GameUser * pUser, SortProto::stReqExpeditionInfo * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	SortProto::stRetExpeditionInfo retCmd;
    if (pUser->getLevel() < CExpeditionCfg::getSingleton().getMinLvl()) {
        retCmd.set_result(SortProto::stRetExpeditionInfo::eLvl_Limit);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    retCmd.set_result(SortProto::stRetExpeditionInfo::eSuccess);
    retCmd.set_usednum(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EXPEDITION_NUM));
    pUser->m_expeditionMgr.loadItems(retCmd);
    pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqExpedition(GameUser * pUser, SortProto::stReqExpedition * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
    SortProto::stRetExpedition retCmd;
    retCmd.set_expeditionid(recvCmd->expeditionid());
    const CExpeditionCfg::stTask * pTask = CExpeditionCfg::getSingleton().getTask(recvCmd->expeditionid());
    if (!pTask) {
        retCmd.set_result(SortProto::stRetExpedition::eInvalid_Id);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->m_countMgr.getDayRefreshValue(ECOUNT_EXPEDITION_NUM) >=
            CExpeditionCfg::getSingleton().getMaxTime(pUser->getLevel())) {
        retCmd.set_result(SortProto::stRetExpedition::eTime_Used_Out);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if(pUser->getLevel() < pTask->m_minLvl || pUser->getLevel() > pTask->m_maxLvl) {
        retCmd.set_result(SortProto::stRetExpedition::eLvl_Limit);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    std::vector<UINT16> idVec;
    pUser->m_expeditionMgr.loadIds(idVec);
    if (CExpeditionCfg::getSingleton().hasSameTask(pTask->m_type, idVec)) {
        retCmd.set_result(SortProto::stRetExpedition::eSame_Type);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    HeroInfo * pHero = NULL;
    stExpeditionHero stHero;
    std::set<UINT32> heroSet;
    std::vector<stExpeditionHero> usedHeroVec;
    for (INT32 i = 0; i < recvCmd->heroids_size(); i++) {
        pHero = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroids(i));
        if (pHero) {
            stHero.m_id = recvCmd->heroids(i);
            stHero.m_lvl = pHero->m_dbData.m_level;
            stHero.m_power = pHero->m_battlePower;
            if (heroSet.insert(recvCmd->heroids(i)).second) {
                usedHeroVec.push_back(stHero);
            }
        }
    }
    if (heroSet.size() != pTask->m_heroNum) {
        retCmd.set_result(SortProto::stRetExpedition::eHero_Num_Limit);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->m_expeditionMgr.heroInUsed(heroSet)) {
        retCmd.set_result(SortProto::stRetExpedition::eHero_In_Used);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    CExpeditionItem * pItem = ATHENA_NEW CExpeditionItem();
    pItem->setId(pTask->m_id);
    pItem->setFinishTime(g_gameServer->getSec() + pTask->m_needTime);
    pItem->setUsedHero(usedHeroVec);
    pUser->m_expeditionMgr.addExpedition(pItem);
    pUser->m_expeditionMgr.addUsedHero(heroSet);
    retCmd.set_result(SortProto::stRetExpedition::eSuccess);
    pUser->m_countMgr.addDayRefreshValue(ECOUNT_EXPEDITION_NUM, 1);
    pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqExpeditionReward(GameUser * pUser, SortProto::stReqExpeditionReward * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
    SortProto::stRetExpeditionReward retCmd;
    retCmd.set_expeditionid(recvCmd->expeditionid());
    const CExpeditionItem * pItem = pUser->m_expeditionMgr.getExpedition(recvCmd->expeditionid());
    if (!pItem) {
        retCmd.set_result(SortProto::stRetExpeditionReward::eInvalid_Id);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (g_gameServer->getSec() < ((CExpeditionItem *)pItem)->getFinishTime()) {
        retCmd.set_result(SortProto::stRetExpeditionReward::eTime_Not_Up);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    const CExpeditionCfg::stTask * pTask =
        CExpeditionCfg::getSingleton().getTask(((CExpeditionItem *)pItem)->getId());
    //base reward
    UINT32 rewardNum = 0;
    UINT32 totalPower = ((CExpeditionItem *)pItem)->getTotalHeroPower();
    double rate = log(0.6 + (totalPower / (double)pTask->m_basePower)) / log(1.6);
    if (rate < 0.01) {
        rate = 0.01;
    } else if (rate > 1.2) {
        rate = 1.2;
    }
    Role::stObtainItemTip itemTipCmd;
    Role::stSynTipItem * pSyn = NULL;
    for (std::map<UINT32, UINT32>::const_iterator it = pTask->m_baseRewardMap.begin();
            it != pTask->m_baseRewardMap.end(); it++) {
        rewardNum = (UINT32)ceil(rate * it->second);
        pSyn = itemTipCmd.add_getitems();
        pSyn->set_itemid(it->first);
        pSyn->set_num(rewardNum);
        pUser->giveAssert(it->first, rewardNum, 0, "领取远征奖励");
    }
    //extra reward
    UINT32 heroCount = 0, conditionCount = 0;
    HeroInfo * pHero = NULL;
    std::vector<UINT32> heroVec;
    ((CExpeditionItem *)pItem)->getUsedHero(heroVec);
    for (std::map<UINT32, UINT32>::const_iterator it = pTask->m_extraConditionMap.begin();
            it != pTask->m_extraConditionMap.end(); it++) {
        heroCount = 0;
        for (size_t i = 0; i < heroVec.size(); i++) {
            pHero = pUser->m_heroMgr.getHeroInfoByID(heroVec[i]);
            if (!pHero || !pHero->m_pHeroBase) {
                continue ;
            }
            if (it->first == CExpeditionCfg::REMOTE &&
                    pHero->m_pHeroBase->m_battleType == eBattle_Romote) {
                heroCount++;
            } else if (it->first == CExpeditionCfg::NEAR &&
                    pHero->m_pHeroBase->m_battleType == eBattle_Near) {
                heroCount++;
            } else if (it->first == CExpeditionCfg::ATTACK &&
                    pHero->m_pHeroBase->m_outPutType == eOut_Put_Attack) {
                heroCount++;
            } else if (it->first == CExpeditionCfg::DEFENSE &&
                    pHero->m_pHeroBase->m_outPutType == eOut_Put_Defense) {
                heroCount++;
            } else if (it->first == CExpeditionCfg::ASSIST &&
                    pHero->m_pHeroBase->m_outPutType == eOut_Put_Assist) {
                heroCount++;
            }
        }
        if (heroCount >= it->second) {
            conditionCount++;
        }
        if (it->first == CExpeditionCfg::TOTAL_POWER &&
                totalPower >= it->second) {
            conditionCount++;
        } else if (it->first == CExpeditionCfg::LVL_GRATER &&
                ((CExpeditionItem *)pItem)->greatorLvl(it->second)) {
            conditionCount++;
        }
    }
    if (conditionCount && conditionCount == pTask->m_extraConditionMap.size()) {
        pSyn = itemTipCmd.add_getitems();
        for (std::map<UINT32, UINT32>::const_iterator it = pTask->m_extraRewardMap.begin();
                it != pTask->m_extraRewardMap.end(); it++) {
            pSyn->set_itemid(it->first);
            pSyn->set_num(it->second);
            pUser->giveAssert(it->first, it->second, 0, "领取远征额外奖励");
        }
    }
    retCmd.set_result(SortProto::stRetExpeditionReward::eSuccess);
    pUser->m_expeditionMgr.deleteExpedition(((CExpeditionItem *)pItem)->getId());
    pUser->sendProto2Me(retCmd);
	pUser->sendProto2Me(itemTipCmd);
    stExpeditionNum expeditionNumNotify;
    expeditionNumNotify.notify(pUser, 1);
}

void SortListModHandle::reqFastExpedition(GameUser * pUser, SortProto::stReqFastExpedition * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
    SortProto::stRetFastExpedition retCmd;
    retCmd.set_expeditionid(recvCmd->expeditionid());
    const CExpeditionItem * pItem = pUser->m_expeditionMgr.getExpedition(recvCmd->expeditionid());
    if (!pItem) {
        retCmd.set_result(SortProto::stRetFastExpedition::eInvalid_Id);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    UINT32 now = g_gameServer->getSec();
    UINT32 finishTime = ((CExpeditionItem *)pItem)->getFinishTime();
    if (now >= finishTime) {
        retCmd.set_result(SortProto::stRetFastExpedition::eTime_Out);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    double baseTime = CExpeditionCfg::getSingleton().getBaseTime();
    UINT32 baseDiamond = CExpeditionCfg::getSingleton().getBaseDiamond();
    UINT32 consumeDiamond = (UINT32)ceil((finishTime - now) / baseTime) * baseDiamond;
    if (!pUser->removeResource(eResource_Diamond, consumeDiamond, "快速远征消耗")) {
        retCmd.set_result(SortProto::stRetFastExpedition::eLess_Of_Diamond);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    ((CExpeditionItem *)pItem)->fastFinish(now);
    retCmd.set_result(SortProto::stRetFastExpedition::eSuccess);
    pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqCancelExpedition(GameUser * pUser, SortProto::stReqCancelExpedition * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
    SortProto::stRetCancelExpedition retCmd;
    retCmd.set_expeditionid(recvCmd->expeditionid());
    const CExpeditionItem * pItem = pUser->m_expeditionMgr.getExpedition(recvCmd->expeditionid());
    if (!pItem) {
        retCmd.set_result(SortProto::stRetCancelExpedition::eInvalid_Id);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (g_gameServer->getSec() >= ((CExpeditionItem *)pItem)->getFinishTime()) {
        retCmd.set_result(SortProto::stRetCancelExpedition::eTime_Out);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    pUser->m_expeditionMgr.deleteExpedition(recvCmd->expeditionid());
    retCmd.set_result(SortProto::stRetCancelExpedition::eSuccess);
    pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqExpeditionSummary(GameUser * pUser, SortProto::stReqExpeditionSummary * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	SortProto::stRetExpeditionSummary retCmd;
    if (pUser->getLevel() < CExpeditionCfg::getSingleton().getMinLvl()) {
        retCmd.set_result(SortProto::stRetExpeditionSummary::eLvl_Limit);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    retCmd.set_result(SortProto::stRetExpeditionSummary::eSuccess);
    retCmd.set_usedtimes(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EXPEDITION_NUM));
    pUser->m_expeditionMgr.getSummary(retCmd);
    pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqEnterEmeraldDream(GameUser * pUser, SortProto::stReqEnterEmeraldDream * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	Global::stCopyMapInfo  retCmd;
	retCmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);

 	//pUser->m_levelMgr.enterEmeraldDream(recvCmd->levelid());
}

void SortListModHandle::reqResetFightNum(GameUser *pUser, SortProto::stResetFightNum * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}
	
	SortProto::stResetFightNumResult retCmd;
			   
	UINT32 needDiamond = CEmeraldDreamCfg::getSingleton().getReset(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_RESETNUM) + 1);
	if (!pUser->removeResource(eResource_Diamond, needDiamond, "翡翠梦境重置")){
		retCmd.set_result(SortProto::stResetFightNumResult::eGoldCoin_Not_Enough);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[翡翠梦境重置](name=%s,id=%d)翡翠梦境挑战次数重置失败，钻石不足!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM) - pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CURRENT_NUM) > 0){
		retCmd.set_result(SortProto::stResetFightNumResult::eNot_Time_Use_Out);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[翡翠梦境重置](name=%s,id=%d)翡翠梦境挑战次数重置失败，次数还没用完!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	pUser->m_countMgr.addDayRefreshValue(ECOUNT_EMERALD_DREAM_RESETNUM,1);
	pUser->m_countMgr.addDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM, CEmeraldDreamCfg::getSingleton().getDailyNum());

	retCmd.set_result(SortProto::stResetFightNumResult::SUCCESS);
	pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->error("[翡翠梦境重置](name=%s,id=%d)翡翠梦境挑战次数重置成功",pUser->getName(),pUser->getID());
#endif
}

void SortListModHandle::reqSurplusNum(GameUser *pUser, SortProto::stReqSurplusNum * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	Global::stReqUserRankInfo retCmd;
	retCmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);

	/*
	SortProto::stSurplusNumResult retCmd;

	
	if(0 == m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM)){
	        m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM, CEmeraldDreamCfg::getSingleton().getDailyNum());
			retCmd.set_num(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM));
	}else{
		retCmd.set_num(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM) - pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CURRENT_NUM));
	}

	pUser->sendProto2Me(retCmd);
	*/
}

	
void SortListModHandle::reqOne2OnePVPMatch(GameUser * pUser,SortProto::stReqOne2OnePVPMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[1v1匹配](name=%s,id=%d)",pUser->getName(),pUser->getID());
#endif
	if(g_gameServer->isPubGameServer())
	{
		COne2OnePVPMgr::getSingleton().addMatchUser(pUser);
	}else{
		//normalGameServer预校验，避免无谓的发送数据到pub
		SortProto::stOne2OnePVPMatchResult retCmd;
		// struct tm _tm; 
		// RealTime::getLocalTime(_tm,g_gameServer->getSec());
		// UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

		// if (!ArenaConfig::getSingleton().isOne2OneOpen(secNum)){//时间未到
		// 	retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eNot_In_Time);
		// 	pUser->sendProto2Me(retCmd);
		// 	return ;
		// }

		if (Athena::global["nftServer"] == "true")
	{ 
			// 裝備等值規則 ：1件ssr=2件sr（ur也屬於ssr條件）
			UINT32 srValue = 0;
			UINT32 srNeed = 1;
			for (int i = HEAD_SLOT_INDEX; i <= WING_SLOT_INDEX; i++)
			{
				CItem *pItem = pUser->m_bagMgr.m_equipBag.getEquipByIndex(i);
				if (pItem != NULL)
				{
					UINT8 quality = pItem->m_pItemBase->m_quality;
					if (pItem->isSR(quality))
					{
						srValue++;
						break;
					}
					else if (pItem->isSSR(quality) || pItem->isUR(quality))
					{
						srValue +=2;
						break;
					} 
				}
			} 
			if (srValue < srNeed)
			{
				retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eEquip);
				pUser->sendProto2Me(retCmd);	
				return;
			}   
	}
		
		if (pUser->getLevel() < 18){
			retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eFail);
			pUser->sendProto2Me(retCmd);
			return ;
		}

		pUser->sendData2PubGame();
		pUser->sendProto2PubGame(*recvCmd);
	}
	
}

void SortListModHandle::cancelOne2OnePVPMatch(GameUser * pUser,SortProto::stCancelOne2OnePVPMatch * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[1v1匹配取消](name=%s,id=%d)",pUser->getName(),pUser->getID());
#endif
	
	if(g_gameServer->isPubGameServer())
	{
		COne2OnePVPMgr::getSingleton().cancelMatch(pUser);

	}else{
		pUser->sendProto2PubGame(*recvCmd);
	}
}

void SortListModHandle::reqHeroLeagueMatch(GameUser * pUser,SortProto::stReqHeroLeagueMatch * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[英雄联赛匹配](name=%s,id=%d)",pUser->getName(),pUser->getID());
#endif

	PubGame::stHeroLeagueMatch retCmd;
	retCmd.set_charid(pUser->getUniquePubID());
//	pUser->sendData2PubGame();
	PubSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
}

void SortListModHandle::cancelHeroLeagueMatch(GameUser * pUser,SortProto::stCancelHeroLeagueMatch * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	PubGame::stHeroLeagueCancelMatch retCmd;
	retCmd.set_charid(pUser->getUniquePubID());
	PubSvrClientMgr::getSingleton().broadcastProto2All(retCmd); 
}

void SortListModHandle::reqReqMenberInfo(GameUser * pUser,SortProto::stReqMenberInfo * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}
/*
	PubGame::stReqMenberInfo retCmd;
	retCmd.set_charid(pUser->getUniquePubID());
	pUser->sendProto2PubGame(retCmd);
	*/
}

void SortListModHandle::reqWorskip(GameUser * pUser,SortProto::stReqWorship *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}
	
	Global::stReqWorskip retCmd;
	retCmd.set_charid(pUser->getID());

	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd); 
}

void SortListModHandle::reqRegisterNum(GameUser * pUser,SortProto::stReqRegister *recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	UINT32 num = pUser->m_countMgr.getDayRefreshValue(ECOUNT_HERO_LEAGUE_LIMIT_NUM);
	
	SortProto::stRegisrerResult retCmd;
	retCmd.set_number(num);
	pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqPubLogIn(GameUser * pUser,SortProto::stReqPubLogIn *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}
	
	pUser->sendData2PubGame();

	PubGame::stReqMenberInfo retCmd;           
	retCmd.set_charid(pUser->getUniquePubID());
	PubSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
}

void SortListModHandle::reqPubLogOut(GameUser * pUser,SortProto::stReqPubLogOut *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->NotifyPubLogout();
}

void SortListModHandle::reqHeroLeagueRedPoint(GameUser * pUser,SortProto::stReqRedPoint *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	Global::stReqRedPoint retCmd;
	retCmd.set_charid(pUser->getID());
	retCmd.set_pubcharid(pUser->getUniquePubID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd); 
}

void SortListModHandle::reqDragonNest(GameUser * pUser,SortProto::stReqDragonNest *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
    pUser->m_dragonNest.checkReset();
    SortProto::stRetDragonNest retCmd;
    retCmd.set_usedtimes(pUser->m_dragonNest.usedTimes);
    retCmd.set_maxtimes(DragonNestInfo::Max_Used_Times);
    retCmd.set_resettimes(pUser->m_dragonNest.resetTimes);
    pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqChallengeDragonNest(GameUser * pUser,SortProto::stReqChallengeDragonNest *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
    pUser->m_dragonNest.checkReset();
    SortProto::stRetChallengeDragonNest retCmd;
    retCmd.set_levelid(recvCmd->levelid());
    if (pUser->m_dragonNest.usedTimes >= DragonNestInfo::Max_Used_Times) {
        retCmd.set_result(SortProto::stRetChallengeDragonNest::eLess_Of_Challenge_Time);
        pUser->sendProto2Me(retCmd);
        return ;
    }
	if (pUser->m_levelMgr.isInLv() || (pUser->getMap() && pUser->getMap()->isCopyMap())) {
        Athena::logger->error("[请求巨龙巢穴]已经在关卡中");
        return ;
    }
    const CDragonNestCfg::DragonNestLvl * pDragonNest = CDragonNestCfg::getSingleton().getDragonNest(recvCmd->levelid());
    if (!pDragonNest) {
        retCmd.set_result(SortProto::stRetChallengeDragonNest::eLvl_Id_Not_Exist);
        pUser->sendProto2Me(retCmd);
        return ;
    }
	struct tm curTm;
	RealTime::getLocalTime(curTm, g_gameServer->getSec());
    UINT32 realDay = 0;
    if (curTm.tm_wday) {
        realDay = curTm.tm_wday;
    } else {
        //sunday
        realDay = 7;
    }
    if (!(pDragonNest->openWeek & (1 << realDay))) {
        retCmd.set_result(SortProto::stRetChallengeDragonNest::eTime_not_Open);
        pUser->sendProto2Me(retCmd);
        return ;
    }
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(recvCmd->levelid());
	if (!pCopyMapBase){
        retCmd.set_result(SortProto::stRetChallengeDragonNest::eLvl_Id_Not_Exist);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->getLevel() < pCopyMapBase->m_limitLv){
        retCmd.set_result(SortProto::stRetChallengeDragonNest::eLvl_Limit);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (!pUser->m_levelMgr.isLevelFinish(pCopyMapBase->m_preLvID)){
        retCmd.set_result(SortProto::stRetChallengeDragonNest::ePre_Lvl_Id_Not_Pass);
        pUser->sendProto2Me(retCmd);
        return ;
    }

	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_RUNE) < 6){
        retCmd.set_result(SortProto::stRetChallengeDragonNest::eBag_Full);
        pUser->sendProto2Me(retCmd);
        return ;
    }

    if (pCopyMapBase->m_costSP > 0) {
        if (!pUser->checkResource(eResource_Sp, pCopyMapBase->m_costSP)){
            retCmd.set_result(SortProto::stRetChallengeDragonNest::eLess_Of_Sp);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    }
	if (pCopyMapBase->m_costLiLian > 0) {
        if (!pUser->checkResource(eResource_Lilian, pCopyMapBase->m_costLiLian)){
            retCmd.set_result(SortProto::stRetChallengeDragonNest::eLess_Of_Lilian);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    }
	stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(pCopyMapBase->m_bossInfo.m_npcID,1));
	if(!pMonsterBase){
		Athena::logger->error("[巨龙巢穴]创建BOSS失败,进入关卡失败!");
        return ;
    }
	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser->getName(), pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[巨龙巢穴]进入巨龙失败, 创建副本不成功!");
		return;
	}  
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->setBossHP(pMonsterBase->m_hp);
	pCopyMap->setBossLv(pMonsterBase->m_level);
 	pUser->setCampID(Map::eCamp_Red);
  	pCopyMap->joinCamp(Map::eCamp_Red,pUser);
   	pUser->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
    pUser->resetBattleProp();

	CMapMgr::getSingleton().changeMap(pUser,pCopyMap, 1);

	pUser->m_levelMgr.setInLvFlag(true);
    retCmd.set_result(SortProto::stRetChallengeDragonNest::eSuccess);
    pUser->sendProto2Me(retCmd);

	pCopyMap->notifyExector();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("请求进入巨龙巢id=%d成功！", pUser->getName(), recvCmd->levelid());
#endif
}

void SortListModHandle::reqResetChallengeDragonNest(GameUser * pUser,SortProto::stReqResetChallengeDragonNest *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
    pUser->m_dragonNest.checkReset();
    SortProto::stRetResetChallengeDragonNest retCmd;
    if (pUser->m_dragonNest.usedTimes < DragonNestInfo::Max_Used_Times) {
        retCmd.set_result(SortProto::stRetResetChallengeDragonNest::eTimes_Remain);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    UINT32 costDiamond = CDragonNestCfg::getSingleton().getCost(pUser->m_dragonNest.resetTimes);
	if (!pUser->removeResource(eResource_Diamond, costDiamond, "巨龙巢穴重置次数")) {
		retCmd.set_result(SortProto::stRetResetChallengeDragonNest::eLess_of_Diamond);
		pUser->sendProto2Me(retCmd);
		return ;
	}
    pUser->m_dragonNest.usedTimes = 0;
    pUser->m_dragonNest.resetTimes++;
    pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqStoneCopyMapInfo(GameUser * pUser,SortProto::stReqStoneCopyMapInfo *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
    pUser->m_stoneCopyMap.checkReset();

    SortProto::stStoneCopyMapInfoResult retCmd;
    retCmd.set_usedtimes(pUser->m_stoneCopyMap.usedTimes);
    retCmd.set_maxtimes(StoneCopyMapInfo::Max_Used_Times);
    retCmd.set_resettimes(pUser->m_stoneCopyMap.resetTimes);
    pUser->sendProto2Me(retCmd);
}

void SortListModHandle::reqChallengeStoneCopyMap(GameUser * pUser,SortProto::stReqChallengeStoneCopyMap *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
    pUser->m_stoneCopyMap.checkReset();
    SortProto::stChallengeStoneCopyMap retCmd;
    retCmd.set_levelid(recvCmd->levelid());
    if (pUser->m_stoneCopyMap.usedTimes >= StoneCopyMapInfo::Max_Used_Times) {
        retCmd.set_result(SortProto::stChallengeStoneCopyMap::eLess_Of_Challenge_Time);
        pUser->sendProto2Me(retCmd);
        return ;
    }
	if (pUser->m_levelMgr.isInLv() || (pUser->getMap() && pUser->getMap()->isCopyMap())) {
        Athena::logger->error("[请求宝石副本]已经在关卡中");
        return ;
    }
	//TODO
    const CStoneCopyMapCfg::stConfigurationInfo * pInfo = CStoneCopyMapCfg::getSingleton().getConfigurationInfo(recvCmd->levelid());
    if (!pInfo) {
        retCmd.set_result(SortProto::stChallengeStoneCopyMap::eLvl_Id_Not_Exist);
        pUser->sendProto2Me(retCmd);
        return ;
    }

	/*
	struct tm curTm;
	RealTime::getLocalTime(curTm, g_gameServer->getSec());
    UINT32 realDay = 0;
    if (curTm.tm_wday) {
        realDay = curTm.tm_wday;
    } else {
        //sunday
        realDay = 7;
    }
    if (!(pInfo->openWeek & (1 << realDay))) {
        retCmd.set_result(SortProto::stChallengeStoneCopyMap::eTime_not_Open);
        pUser->sendProto2Me(retCmd);
        return ;
    }
	*/

	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(recvCmd->levelid());
	if (!pCopyMapBase){
        retCmd.set_result(SortProto::stChallengeStoneCopyMap::eLvl_Id_Not_Exist);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->getLevel() < pCopyMapBase->m_limitLv){
        retCmd.set_result(SortProto::stChallengeStoneCopyMap::eLvl_Limit);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (!pUser->m_levelMgr.isLevelFinish(pCopyMapBase->m_preLvID)){
        retCmd.set_result(SortProto::stChallengeStoneCopyMap::ePre_Lvl_Id_Not_Pass);
        pUser->sendProto2Me(retCmd);
        return ;
    }

	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_RUNE) < 6){
        retCmd.set_result(SortProto::stChallengeStoneCopyMap::eBag_Full);
        pUser->sendProto2Me(retCmd);
        return ;
    }

    if (pCopyMapBase->m_costSP > 0) {
        if (!pUser->checkResource(eResource_Sp, pCopyMapBase->m_costSP)){
            retCmd.set_result(SortProto::stChallengeStoneCopyMap::eLess_Of_Sp);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    }
	if (pCopyMapBase->m_costLiLian > 0) {
        if (!pUser->checkResource(eResource_Lilian, pCopyMapBase->m_costLiLian)){
            retCmd.set_result(SortProto::stChallengeStoneCopyMap::eLess_Of_Lilian);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    }

	stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(pCopyMapBase->m_bossInfo.m_npcID,1));
	if(!pMonsterBase){
		Athena::logger->error("[宝石副本]创建BOSS失败,进入关卡失败!");
        return ;
    }

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser->getName(), pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[宝石副本]进入副本失败, 创建副本不成功!");
		return;
	}

	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->setBossHP(pMonsterBase->m_hp);
	pCopyMap->setBossLv(pMonsterBase->m_level);
 	pUser->setCampID(Map::eCamp_Red);
  	pCopyMap->joinCamp(Map::eCamp_Red,pUser);
   	pUser->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
    pUser->resetBattleProp();

	CMapMgr::getSingleton().changeMap(pUser,pCopyMap, 1);

	pUser->m_levelMgr.setInLvFlag(true);
    retCmd.set_result(SortProto::stChallengeStoneCopyMap::eSuccess);
    pUser->sendProto2Me(retCmd);

	pCopyMap->notifyExector();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("请求进入宝石副本id=%d成功！", pUser->getName(), recvCmd->levelid());
#endif
}

void SortListModHandle::reqResetStoneCopyMap(GameUser * pUser,SortProto::stReqResetStoneCopyMapNum *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
    pUser->m_stoneCopyMap.checkReset();
    SortProto::stResetStoneCopyMapNum retCmd;
    if (pUser->m_stoneCopyMap.usedTimes < StoneCopyMapInfo::Max_Used_Times) {
        retCmd.set_result(SortProto::stResetStoneCopyMapNum::eTimes_Remain);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    UINT32 costDiamond = CStoneCopyMapCfg::getSingleton().getCost(pUser->m_stoneCopyMap.resetTimes);
	if (!pUser->removeResource(eResource_Diamond, costDiamond, "宝石副本重置次数")) {
		retCmd.set_result(SortProto::stResetStoneCopyMapNum::eLess_of_Diamond);
		pUser->sendProto2Me(retCmd);
		return ;
	}
    pUser->m_stoneCopyMap.usedTimes = 0;
    pUser->m_stoneCopyMap.resetTimes++;
    pUser->sendProto2Me(retCmd);
}
