#include "MiscModHandle.h"
#include "md5.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "GlobalSvrClient.h"
#include "CaptureCfg.h"
#include "MiscCfgMgr.h"
#include "LimitTimePetCfg.h"
#include "ShopActivity.h"
#include "GuardCfg.h"
#include "QuestionCfg.h"
#include "GloryfashionCfg.h"
#include "LuckyDrawCfg.h"
#include "InviteCfg.h"
#include "DailyRecharge.h"
#include "MonsterMgr.h"
#include "LevelWflfareCfg.h"
#include "CommonMicro.h"
#include "HttpParser.h"
#include "HttpClient.h"
#include "json/writer.h"
#include "json/reader.h"

extern GameServer * g_gameServer;

MiscModHandle::MiscModHandle()
{
	registerHandle(&MiscModHandle::reqRewardInfo);
	registerHandle(&MiscModHandle::reqSignReward);
	registerHandle(&MiscModHandle::reqOpenSvrReward);
	registerHandle(&MiscModHandle::reqLvReward);
	registerHandle(&MiscModHandle::reqOnlineReward);

	registerHandle(&MiscModHandle::activeTitle);
	registerHandle(&MiscModHandle::cancelActiveTitle);
	registerHandle(&MiscModHandle::buyStar);
	registerHandle(&MiscModHandle::openDrawWindow);
	registerHandle(&MiscModHandle::reqDraw);
	registerHandle(&MiscModHandle::buyInvestPlan);
	registerHandle(&MiscModHandle::investPlanReward);
	registerHandle(&MiscModHandle::eatDinner);
	registerHandle(&MiscModHandle::treasureCombine);
	registerHandle(&MiscModHandle::buyPeaceModuleTime);
	registerHandle(&MiscModHandle::captureTreasure);
	registerHandle(&MiscModHandle::unlockedSprite);
	registerHandle(&MiscModHandle::levelupSprite);
	registerHandle(&MiscModHandle::spriteFollow);
	registerHandle(&MiscModHandle::reqAvaibleActivity);
	registerHandle(&MiscModHandle::reqActivityReward);
	registerHandle(&MiscModHandle::reqClearWorldBossCD);
	registerHandle(&MiscModHandle::req7DayActivityInfo);
	registerHandle(&MiscModHandle::req7DayActivityReward);
	registerHandle(&MiscModHandle::reqRechargeActivity);
	registerHandle(&MiscModHandle::rechargeActivityReward);
	registerHandle(&MiscModHandle::reqDrawHeroScoreReward);
	registerHandle(&MiscModHandle::reqInvestgateReward);
	registerHandle(&MiscModHandle::reqShopActivityList);
	registerHandle(&MiscModHandle::reqLuckShopReward);
	registerHandle(&MiscModHandle::saveLuckOpenWinTime);
	registerHandle(&MiscModHandle::clearInterceptCD);
	registerHandle(&MiscModHandle::guardInspired);
	registerHandle(&MiscModHandle::summonHighQualityGuard);
	registerHandle(&MiscModHandle::openDrawLimitFashionWin);
	registerHandle(&MiscModHandle::drawLimitFashionWin);
	registerHandle(&MiscModHandle::reqExchangeItem);
	registerHandle(&MiscModHandle::reqRecycleReward);
	registerHandle(&MiscModHandle::reqGloryfashion);
	registerHandle(&MiscModHandle::reqIllusionSprite);
	registerHandle(&MiscModHandle::reqIllusionSpriteEat);
	registerHandle(&MiscModHandle::reqIllusionSpriteSkillUp);
	registerHandle(&MiscModHandle::reqPayType);
	registerHandle(&MiscModHandle::ReqLuckyDraw);
	registerHandle(&MiscModHandle::ReqLuckyDrawReward);
	registerHandle(&MiscModHandle::ReqLuckyDrawPointReward);
	registerHandle(&MiscModHandle::ReqShareInvite);
	registerHandle(&MiscModHandle::ReqDailyShareInvite);
	registerHandle(&MiscModHandle::ReqDrawDailyShareReward);
	registerHandle(&MiscModHandle::ReqDrawOnceShareReward);
	registerHandle(&MiscModHandle::reqDailyReceiveState);
	registerHandle(&MiscModHandle::reqReceiveReward);
	registerHandle(&MiscModHandle::reqDrawLateWalfare);
	registerHandle(&MiscModHandle::reqDrawResGetBack);
	registerHandle(&MiscModHandle::reqDrawAllResGetBack);
	registerHandle(&MiscModHandle::reqOpenMonsterWindow);
	registerHandle(&MiscModHandle::reqAttackMonster);
	registerHandle(&MiscModHandle::reqBuyAttackTime);
	registerHandle(&MiscModHandle::reqAKeyAttackMonster);
	registerHandle(&MiscModHandle::reqMountLevelUp);                                
	registerHandle(&MiscModHandle::reqButtleMount);      
	registerHandle(&MiscModHandle::reqMountFollow);                        
	registerHandle(&MiscModHandle::reqMountUnreal);                          
	registerHandle(&MiscModHandle::reqOpenGrowWindow);
	registerHandle(&MiscModHandle::reqReceiveWflfare);
	registerHandle(&MiscModHandle::reqPaySign);
	registerHandle(&MiscModHandle::reqServerPay);
	registerHandle(&MiscModHandle::reqSummonData);
	registerHandle(&MiscModHandle::reqSummonReward);
	registerHandle(&MiscModHandle::reqRechargeCheck);

}

MiscModHandle::~MiscModHandle()
{

}

bool MiscModHandle::handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<MiscModHandle,GameUser>::handle(pUser,pData,cmdLen,funID); 
}

void MiscModHandle::reqRewardInfo(GameUser *pUser,MiscProto::stReqRewardInfo * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	pUser->m_rewardSys.userOnline();
}

void MiscModHandle::reqSignReward(GameUser *pUser,MiscProto::stReqSignReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_SIGN,pUser,0);
}

void MiscModHandle::reqOpenSvrReward(GameUser *pUser,MiscProto::stReqOpenSvrReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_OPEN_SVR,pUser,0);
}

void MiscModHandle::reqLvReward(GameUser *pUser,MiscProto::stReqLvReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_LEVEL,pUser,recvCmd->lv());
}

void MiscModHandle::reqOnlineReward(GameUser * pUser,MiscProto::stReqOnlineReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_ONLINE,pUser,recvCmd->id());
}

void MiscModHandle::activeTitle(GameUser * pUser,MiscProto::stActiveOneTitle * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	pUser->m_titleMgr.activeTitle(recvCmd->titleid());
}

void MiscModHandle::cancelActiveTitle(GameUser * pUser,MiscProto::stCancelActiveTitle * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_titleMgr.cancelActiveTitle();	
}

void MiscModHandle::buyStar(GameUser * pUser,MiscProto::stReqBuyStar * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
}

void MiscModHandle::openDrawWindow(GameUser * pUser,MiscProto::ReqOpenDrawWindow * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	pUser->m_drawMgr.openDrawWindow();
}

void MiscModHandle::reqDraw(GameUser * pUser,MiscProto::stReqDraw * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_drawMgr.draw(recvCmd->type());
}

void MiscModHandle::buyInvestPlan(GameUser * pUser,MiscProto::stBuyInvestPlan * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CAllRewardMgr::getSingleton().buyInvestPlan(pUser);
}


void MiscModHandle::investPlanReward(GameUser *pUser,MiscProto::stReqInvestPlanReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_INVESTPLAN,pUser,recvCmd->lv());
}

//【召唤计划】当前进度 请求 
void MiscModHandle::reqSummonData(GameUser * pUser, MiscProto::stReqSummonData * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}

	CAllRewardMgr::getSingleton().sendSummonData(pUser, recvCmd->summontype());
}

//【召唤计划】领奖 请求
void MiscModHandle::reqSummonReward(GameUser * pUser, MiscProto::stReqSummonReward * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	
	CAllRewardMgr::getSingleton().doSummonReward(pUser, recvCmd->countid(), recvCmd->summontype(), recvCmd->rewardtype());
}

//【充值】校验 请求 
void MiscModHandle::reqRechargeCheck(GameUser * pUser, MiscProto::stReqRechargeCheck * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}

	// CRMBShopMgr::getSingleton().RechargeCheck(recvCmd->rechargeid());


	UINT32 rechargeId = recvCmd->rechargeid();
	UINT32 buyCount = recvCmd->buycount();


	MiscProto::stRechargeCheckResult retCmd;
	
	std::map<UINT32,UINT32>::iterator it = pUser->m_buyTreasureCount.find(rechargeId);

	if (it != pUser->m_buyTreasureCount.end()) {//已经买过累加
		UINT32 tempcount = 0;
		tempcount = it->second + 1;

		Athena::logger->trace("rechargeId :%d tempcount ： %d,cpmnum ：%d", rechargeId, tempcount, buyCount);

		if (tempcount > buyCount) {//tempcount 大于配置 false
			retCmd.set_result(MiscProto::stRechargeCheckResult::eNo_Times);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}

	// 检查背包剩余格子数量
    if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 2) {
        retCmd.set_result(MiscProto::stRechargeCheckResult::eBag_Full);
        pUser->sendProto2Me(retCmd);
        return ;
    }

	retCmd.set_result(MiscProto::stRechargeCheckResult::eSuccess);

	pUser->sendProto2Me(retCmd);

	UINT32 tOpenServerTime = (UINT32)atoi(Athena::global["OpenServer_Time"].c_str());
	Athena::logger->trace("开服时间utc:%d,%s",tOpenServerTime,g_utc2StringFormat(tOpenServerTime).c_str());

}

void MiscModHandle::eatDinner(GameUser *pUser,MiscProto::stReqEatDinner * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_EAT_DINNER,pUser,0);
}

/*void MiscModHandle::everydayRechargeReward(GameUser * pUser,MiscProto::stReqEveryDayRechargeReward * recvCmd)
  {
  if (!pUser || !recvCmd){
  return ;
  }

  CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_EVERAYDAY_RECHARGE,pUser,recvCmd->rechargenum());
  }

  void MiscModHandle::consumeReward(GameUser * pUser,MiscProto::stReqConsumeReward * recvCmd)
  {
  if (!pUser || !recvCmd){
  return ;
  }

  CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_CONSUME,pUser,recvCmd->consumenum());
  }

  void MiscModHandle::cumulativeLoginReward(GameUser *pUser,MiscProto::stReqCumulativeReward * recvCmd)
  {
  if (!pUser || !recvCmd){
  return ;
  }

//CAllRewardMgr::getSingleton().doReward(REWARD_TYPE_CUMULATIVE_LOGIN,pUser,recvCmd->daynum());
}*/

void MiscModHandle::treasureCombine(GameUser * pUser,MiscProto::stReqCombineTreasure * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stCombineTreasureResult retCmd;

	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 1){
		retCmd.set_result(MiscProto::stCombineTreasureResult::eBAG_FULL);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	Global::stTreasureCombine cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_combinetype(recvCmd->combinetype());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}


void MiscModHandle::buyPeaceModuleTime(GameUser *pUser,MiscProto::stBuyPeaceModuleTime * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stBuyPeaceModuleTimeResult retCmd;
	UINT32 addTime = 0;

	if (MiscProto::stBuyPeaceModuleTime::e1_HOUR_ITEM == recvCmd->type()){

		if (!pUser->m_bagMgr.removeItemByItemID(CCaptureCfg::getSingleton().get1HourPrice().m_itemID,0,1,"抢夺资源")){
			retCmd.set_result(MiscProto::stBuyPeaceModuleTimeResult::eHAS_NO_ITEM);
			pUser->sendProto2Me(retCmd);
			return ;
		}	
		addTime = 3600;
	}
	else if (MiscProto::stBuyPeaceModuleTime::e3_HOUR_ITEM == recvCmd->type()){
		if (!pUser->m_bagMgr.removeItemByItemID(CCaptureCfg::getSingleton().get3HourPrice().m_itemID,0,1,"抢夺资源")){
			retCmd.set_result(MiscProto::stBuyPeaceModuleTimeResult::eHAS_NO_ITEM);
			pUser->sendProto2Me(retCmd);
			return ;
		}	
		addTime = 3600 * 3;
	}
	else if (MiscProto::stBuyPeaceModuleTime::e12_HOUR_ITEM == recvCmd->type()){
		if (!pUser->m_bagMgr.removeItemByItemID(CCaptureCfg::getSingleton().get12HourPrice().m_itemID,0,1,"抢夺资源")){
			retCmd.set_result(MiscProto::stBuyPeaceModuleTimeResult::eHAS_NO_ITEM);
			pUser->sendProto2Me(retCmd);
			return ;
		}	
		addTime = 3600 * 12;
	}
	else if (MiscProto::stBuyPeaceModuleTime::e10_DIAMOND == recvCmd->type()){
		UINT16 costType = 0;
		UINT32 costNum = 0;

		if (1 == recvCmd->hour()){
			costType = CCaptureCfg::getSingleton().get1HourPrice().m_costType;
			costNum = CCaptureCfg::getSingleton().get1HourPrice().m_costNum;
		}

		if (3 == recvCmd->hour()){
			costType = CCaptureCfg::getSingleton().get3HourPrice().m_costType;
			costNum = CCaptureCfg::getSingleton().get3HourPrice().m_costNum;
		}

		if (12 == recvCmd->hour()){
			costType = CCaptureCfg::getSingleton().get12HourPrice().m_costType;
			costNum = CCaptureCfg::getSingleton().get12HourPrice().m_costNum;
		}

		if (!pUser->removeResource((eResource)costType,costNum,"抢夺资源")){
			retCmd.set_result(MiscProto::stBuyPeaceModuleTimeResult::eDIAMOND_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
			return ;
		}	
		addTime = 3600 * recvCmd->hour();
	}
	else {
		return ;
	}

	retCmd.set_remaintime(addTime);
	retCmd.set_result(MiscProto::stBuyPeaceModuleTimeResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
	Global::stBuyPeaceModubleTime cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_addtime(addTime);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}


void MiscModHandle::captureTreasure(GameUser * pUser,MiscProto::stCaptureUserTreasure * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stCaptureUserTreasureResult retCmd;

	Global::stForwardCapture cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_otherid(recvCmd->charid());
	cmd.set_restype(recvCmd->restype());
	cmd.set_combineid(recvCmd->combineid());

	UINT32 needLilian = 0;
	UINT32 needSlot = 0;

	if (MiscProto::eOneTime_Capture == recvCmd->type()){
		needLilian = Capture_Need_Lilian;
		needSlot = 1;
		cmd.set_num(1);
		cmd.set_multicapture(false);
	}
	else if (MiscProto::eFiveTime_Capture == recvCmd->type()){
		if (!g_gameServer->isAppVersion()) {
			if ((pUser->getCountByVip(eVipFun_5Times_Capture_Treasure) < 1) && (pUser->getLevel() < CMiscCfgMgr::getSingleton().getOneKeyCaptureLv())){
				Athena::logger->error("vip等级不足,不可以5连扫荡");
				return ;
			}
		}
		UINT32 captureNum = pUser->getLilian() / Capture_Need_Lilian;
		if (captureNum > 5){
			captureNum = 5;
		}
		needLilian = Capture_Need_Lilian * captureNum;
		needSlot = captureNum;
		cmd.set_num(captureNum);
		cmd.set_multicapture(true);
	}
	if (cmd.num() <= 0){
		retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eSP_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->m_bagMgr.getMainFreeSlotNum() < needSlot){
		retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eBAG_FULL);
		pUser->sendProto2Me(retCmd);
		Athena::logger->trace("抢夺合成需要的包裹空格:needSlot:%d,包裹空格:%d",needSlot,pUser->m_bagMgr.getBagFreeIndex(Role::BAG_TYPE_MAIN1));
		return ;
	}

	if (!pUser->checkResource(eResource_Lilian,needLilian)){//体力不足
		retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eSP_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void MiscModHandle::unlockedSprite(GameUser * pUser,MiscProto::stReqUnlockedSprite * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_spriteSysMgr.unlockedSprite(recvCmd->spriteid());
}

void MiscModHandle::levelupSprite(GameUser * pUser,MiscProto::stReqLevelupSprite * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_spriteSysMgr.levelupSprite(recvCmd->spriteid());
}

void MiscModHandle::spriteFollow(GameUser * pUser,MiscProto::stSpriteFollowMaster * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_spriteSysMgr.followMaster(recvCmd->spriteid());
}

void MiscModHandle::reqAvaibleActivity(GameUser * pUser,MiscProto::stReqAvaibleActivity * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_activityDataMgr.sendAllAvaibleActivity();
}

void MiscModHandle::reqActivityReward(GameUser * pUser,MiscProto::stReqAcitivityReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_activityDataMgr.doActivityStepReward(recvCmd->id(),recvCmd->stepid(),recvCmd->selindex());
	//pUser->m_activityDataMgr.sendAllAvaibleActivity();
}

void MiscModHandle::reqClearWorldBossCD(GameUser * pUser,MiscProto::stReqClearWorldBossActivityCD * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stClearWorldBossActivityCDResult retCmd;
	if (!pUser->removeResource(eResource_Diamond,30,"世界boss清CD")){
		retCmd.set_result(MiscProto::stClearWorldBossActivityCDResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_result(MiscProto::stClearWorldBossActivityCDResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	Global::stSetClearWorldBossTime cmd;
	cmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void MiscModHandle::req7DayActivityInfo(GameUser * pUser,MiscProto::stReq7DayActivityInfo * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_sevenDayActivityMgr.send7DayActivityInfo(recvCmd->day());
}

void MiscModHandle::req7DayActivityReward(GameUser * pUser,MiscProto::stReqSevenDayReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_sevenDayActivityMgr.got7DayActivityReward(recvCmd->day(),recvCmd->goaltype(),recvCmd->id());
}

void MiscModHandle::reqRechargeActivity(GameUser * pUser,MiscProto::stReqRechargeActivityList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	//pUser->m_activityDataMgr.sendAllAvaibleActivity();
	pUser->m_rechargeActivityMgr.sendActivityList();
}

void MiscModHandle::rechargeActivityReward(GameUser * pUser,MiscProto::stReqRechargeActivtiyReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_rechargeActivityMgr.doReward(recvCmd->activityid(),recvCmd->stepid());
}

void MiscModHandle::reqDrawHeroScoreReward(GameUser * pUser,MiscProto::stReqDrawHeroScoreReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stDrawHeroScoreRewardRet retCmd;
	LimitTimePetCfg::stActivityOpenInfo *pOpenInfo =  LimitTimePetCfg::getSingleton().getOpenInfo(g_gameServer->getSec());
	if (!pOpenInfo){
		retCmd.set_result(MiscProto::stDrawHeroScoreRewardRet::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	LimitTimePetCfg::stScoreReward *pScoreReward =  LimitTimePetCfg::getSingleton().getScoreRewardInfo(pOpenInfo->m_activityID,recvCmd->id());
	if (!pScoreReward){
		retCmd.set_result(MiscProto::stDrawHeroScoreRewardRet::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
	} 

	if (pUser->m_boxMgr.needSlotNum(pScoreReward->m_boxID) > pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1)){
		retCmd.set_result(MiscProto::stDrawHeroScoreRewardRet::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	Global::stForwardDrawHeroScoreReward cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_scoreid(recvCmd->id());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void MiscModHandle::reqInvestgateReward(GameUser *pUser,MiscProto::stReqInvestgationReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	if (pUser->m_questInvestigateMgr.giveReward(recvCmd->questid(),recvCmd->level())){//成功
		Global::ForwardInvestgateResult cmd;
		cmd.set_questid(recvCmd->questid());
		cmd.set_charid(pUser->getID());
		UINT16 maxAnswers = CQuestionCfg::getSingleton().getProblemNum(recvCmd->questid(),recvCmd->level());
		if (recvCmd->answers_size() > maxAnswers){
			Athena::logger->trace("提交的答案数量比配置的数量多,不正确!(answers_size:%d,maxAnswers:%d)",recvCmd->answers_size(),maxAnswers);
		}
		cmd.set_usetime(recvCmd->usetime());
		for (UINT16 i = 0;(i < recvCmd->answers_size()) &&(i < maxAnswers) ;++i){
			Global::stForwardInvestgateAnswer * pSyn = cmd.add_infos();
			if (pSyn){
				const MiscProto::stInvestAnswer & pAnswer = recvCmd->answers(i);
				pSyn->set_problemid(pAnswer.problemid());
				pSyn->set_answera(pAnswer.answera());
				pSyn->set_answerb(pAnswer.answerb());
				pSyn->set_answerc(pAnswer.answerc());
				pSyn->set_answerd(pAnswer.answerd());
				pSyn->set_answere(pAnswer.answere());
				pSyn->set_answerf(pAnswer.answerf());
				pSyn->set_answerg(pAnswer.answerg());
				pSyn->set_answerh(pAnswer.answerh());
				pSyn->set_answeri(pAnswer.answeri());
				pSyn->set_answerj(pAnswer.answerj());
#ifdef _HDZ_DEBUG
				std::string answerDesc;
				if (pAnswer.answera()){
					answerDesc = "A";
				}
				else if (pAnswer.answerb()){
					answerDesc = "B";
				}
				else if (pAnswer.answerc()){
					answerDesc = "C";
				}
				else if (pAnswer.answerd()){
					answerDesc = "D";
				}
				else if (pAnswer.answere()){
					answerDesc = "E";
				}
				else if (pAnswer.answerf()){
					answerDesc = "F";
				}
				else if (pAnswer.answerg()){
					answerDesc = "G";
				}
				else if (pAnswer.answerh()){
					answerDesc = "H";
				}
				else if (pAnswer.answeri()){
					answerDesc = "I";
				}
				else if (pAnswer.answerj()){
					answerDesc = "J";
				}
				Athena::logger->trace("questID:%d,level:%d,problemID:%d,选择了:%s",recvCmd->questid(),recvCmd->level(),pAnswer.problemid(),answerDesc.c_str());
#endif
			}
		}
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}
}

void MiscModHandle::reqShopActivityList(GameUser * pUser,MiscProto::stReqActivityShop * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CShopActivityMgr::getSingleton().sendAllActivity(pUser);
}

void MiscModHandle::reqLuckShopReward(GameUser *pUser,MiscProto::stReqLuckShopReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_luckShopMgr.doReward();
}

void MiscModHandle::saveLuckOpenWinTime(GameUser * pUser,MiscProto::stSaveOpenLuckShopWinTime *recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	pUser->m_luckShopMgr.setOpenWinTime();
}

void MiscModHandle::clearInterceptCD(GameUser *pUser,MiscProto::stResetInterceptCD * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stResetInterceptCDResult retCmd;
	UINT32 needDiamond = GuardCfg::getSingleton().getClearCDNeedDiamond();
	if (!pUser->removeResource(eResource_Diamond,needDiamond,"清空拦截cd")){
		retCmd.set_result(MiscProto::stResetInterceptCDResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_result(MiscProto::stResetInterceptCDResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	Global::stNotifyClearInterceptCD cmd;
	cmd.set_charid(recvCmd->charid());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void MiscModHandle::guardInspired(GameUser * pUser,MiscProto::stReqGuardInspired * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stGuardInspiredResult retCmd;

	UINT32 needDiamond = GuardCfg::getSingleton().getInspiredDiamond();
	if (!pUser->removeResource(eResource_Diamond,needDiamond,"护送鼓舞士气")){
		retCmd.set_result(MiscProto::stGuardInspiredResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	//retCmd.set_result(MiscProto::stGuardInspiredResult::eSuccess);
	//pUser->sendProto2Me(retCmd);

	Global::stNotifyGuardInspiredSuccess cmd;
	cmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void MiscModHandle::summonHighQualityGuard(GameUser *pUser,MiscProto::stReqSummonHighQualityGuard * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stSummonHighQualityGuardResult retCmd;
	UINT32 needDiamond = GuardCfg::getSingleton().getCallDiamond();
	if (!pUser->removeResource(eResource_Diamond,needDiamond,"召唤最高品质护送")){
		retCmd.set_result(MiscProto::stSummonHighQualityGuardResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return;
	}

	retCmd.set_result(MiscProto::stSummonHighQualityGuardResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	Global::stNotifyGuardQuality cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_quality(eGuard_Qaulity_5);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void MiscModHandle::openDrawLimitFashionWin(GameUser * pUser,MiscProto::stReqOpenLimitFashionDrawWin * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_limitFashionDrawMgr.openDrawWin();
}

void MiscModHandle::drawLimitFashionWin(GameUser * pUser,MiscProto::stReqDrawLimitFashion * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_limitFashionDrawMgr.draw();
}


void MiscModHandle::reqExchangeItem(GameUser * pUser,MiscProto::stReqExchangeItem * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_activityDataMgr.doActivityStepReward(recvCmd->activityid(),recvCmd->stepid(),0);
}

void MiscModHandle::reqRecycleReward(GameUser * pUser,MiscProto::stReqRecycleRechargeActReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_activityDataMgr.doActivityStepReward(recvCmd->activityid(),recvCmd->stepid(),0);
}

void MiscModHandle::reqGloryfashion(GameUser * pUser,MiscProto::stReqGloryfashion * recvCmd)
{
	MiscProto::stGloryfashionResult retCmd;

	CGloryfashionCfg::stActivityInfo * info = CGloryfashionCfg::getSingleton().getActicityInfo(g_gameServer->getSec());
	if(!info)
	{
		retCmd.set_result(MiscProto::stGloryfashionResult::eNot_Open_Time);
		pUser->sendProto2Me(retCmd);
		return;
	}

	if (pUser->m_boxMgr.needSlotNum(info->m_fashionitemid) > pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_FASHION)){
		retCmd.set_result(MiscProto::stGloryfashionResult::eBag_Full);	
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->removeResource((eResource)info->m_costResId,info->m_costResNum,"荣耀时装"))
	{
		retCmd.set_result(MiscProto::stGloryfashionResult::eRecharge_Not_Enough);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[荣耀时装](name=%s,id=%d)购买失败,扣除资源失败!",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if(info->m_fashionitemid > 0){
		pUser->m_boxMgr.openBox(info->m_fashionitemid);
	}

	Global::stSynGloryfashionCharID charCmd;
	charCmd.set_charid(pUser->getID());
	charCmd.set_buytime(g_gameServer->getSec());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(charCmd);

	retCmd.set_result(MiscProto::stGloryfashionResult::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void MiscModHandle::reqIllusionSprite(GameUser * pUser,MiscProto::stReqIllusionSprite * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_spriteSysMgr.sendIllusionSprite(recvCmd->id());
}

void MiscModHandle::reqIllusionSpriteEat(GameUser * pUser,MiscProto::stReqIllusionSpriteEat * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_spriteSysMgr.illusionSpriteEat(recvCmd);
}

void MiscModHandle::reqIllusionSpriteSkillUp(GameUser * pUser,MiscProto::stReqIllusionSpriteSkillUp * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_spriteSysMgr.illusionSpriteSkillUp(recvCmd->id(), recvCmd->skillid());
}

void MiscModHandle::reqPayType(GameUser * pUser, MiscProto::stReqPayType * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	MiscProto::stRetPayType retCmd;
	retCmd.set_itemid(recvCmd->itemid());
	retCmd.set_paytype(Athena::global["payType"]);
	std::stringstream os;
	os << recvCmd->serverid() << "_" << pUser->getID() << "_" << g_gameServer->getSec();
	std::string orderId = os.str();
	if (!pUser->m_rmbShop.existsItem(recvCmd->itemid())) {
		retCmd.set_res(MiscProto::stRetPayType::eNo_Such_Item);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	Game::stReqPayType cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_itemid(recvCmd->itemid());
	cmd.set_orderid(orderId);
	cmd.set_paytype(retCmd.paytype());
	cmd.set_isextend(recvCmd->isextend());
	cmd.set_data(recvCmd->data());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("请求充值渠道配置: isextend is %d, data is %s",
			cmd.isextend(), cmd.data().c_str());
#endif
}

void MiscModHandle::ReqLuckyDraw(GameUser * pUser, MiscProto::stReqLuckyDraw * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	MiscProto::stRetLuckyDraw retCmd;
	UINT32 id = CLuckyDrawCfg::getSingleton().getActiveId(g_gameServer->getSec());
	if (id && pUser->m_luckyDrawMgr.m_curId != id) {
		pUser->m_luckyDrawMgr.m_curId = id;
		pUser->m_luckyDrawMgr.m_diamond = 0;
		pUser->m_luckyDrawMgr.m_lastFreeDraw = 0;
		pUser->m_luckyDrawMgr.m_drawNum = 0;
		pUser->m_luckyDrawMgr.m_drawMap.clear();
	}
	retCmd.set_activityid(pUser->m_luckyDrawMgr.m_curId);
	retCmd.set_lastfreetime(pUser->m_luckyDrawMgr.m_lastFreeDraw);
	for (std::map<UINT8, bool>::iterator it = pUser->m_luckyDrawMgr.m_drawMap.begin();
			it != pUser->m_luckyDrawMgr.m_drawMap.end(); it++) {
		MiscProto::stLuckyDrawPointRewardInfo * pReward = retCmd.add_reward();
		pReward->set_id(it->first);
		pReward->set_status((int)it->second);
	}
	retCmd.set_diamond(pUser->m_luckyDrawMgr.m_diamond);
	std::vector<LuckyDrawRecord> recordVec;
	GameUser::loadLuckyDrawRecord(recordVec);
	for (size_t i = 0; i < recordVec.size(); i++) {
		MiscProto::stLuckyDrawUserInfo *player = retCmd.add_player();
		player->set_id(recordVec[i].id);
		player->set_num(recordVec[i].num);
		player->set_name(recordVec[i].name);
	}
	pUser->sendProto2Me(retCmd);
}

void MiscModHandle::ReqLuckyDrawReward(GameUser * pUser, MiscProto::stReqLuckyDrawReward * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	MiscProto::stRetLuckyDrawReward retCmd;
	retCmd.set_id(recvCmd->id());
	retCmd.set_diamond(pUser->m_luckyDrawMgr.m_diamond);
	UINT8 drawType = CLuckyDrawCfg::Draw_Once;
	if (!recvCmd->type()) {
		drawType = CLuckyDrawCfg::Draw_Ten;
	}
	UINT32 id = CLuckyDrawCfg::getSingleton().getActiveId(g_gameServer->getSec());
	if (id != recvCmd->id()) {
		retCmd.set_result(MiscProto::stRetLuckyDrawReward::eActivity_Expire);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	if (id && pUser->m_luckyDrawMgr.m_curId != id) {
		pUser->m_luckyDrawMgr.m_curId = id;
		pUser->m_luckyDrawMgr.m_diamond = 0;
		pUser->m_luckyDrawMgr.m_lastFreeDraw = 0;
		pUser->m_luckyDrawMgr.m_drawNum = 0;
		pUser->m_luckyDrawMgr.m_drawMap.clear();
	}
	CLuckyDrawCfg::ActivityItem item;
	if (!CLuckyDrawCfg::getSingleton().getActivityItem(recvCmd->id(), item)) {
		retCmd.set_result(MiscProto::stRetLuckyDrawReward::eNo_Such_Id);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	UINT8 drawId = CLuckyDrawCfg::getSingleton().getDrawId(item.activityId);
	if (!drawId) {
		retCmd.set_result(MiscProto::stRetLuckyDrawReward::eNo_Such_Id);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	CLuckyDrawCfg::stDrawInfo info;
	if (!CLuckyDrawCfg::getSingleton().getDrawInfo(drawId, drawType, info)) {
		retCmd.set_result(MiscProto::stRetLuckyDrawReward::eNo_Such_Id);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	CLuckyDrawCfg::stCertainInfo certain;
	if (!CLuckyDrawCfg::getSingleton().getCertainInfo(drawId, certain)) {
		retCmd.set_result(MiscProto::stRetLuckyDrawReward::eNo_Such_Id);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	UINT32 freeNum = 0;
	UINT32 maxTime = 0;
	if (drawType == CLuckyDrawCfg::Draw_Once) {
		freeNum = 2;
		maxTime = 1;
	} else {
		freeNum = 10;
		maxTime = 10;
	}
	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < freeNum) {
		retCmd.set_result(MiscProto::stRetLuckyDrawReward::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	bool useFree = false;
	UINT32 costDiamond = 0;
	if (drawType == CLuckyDrawCfg::Draw_Once) {
		if (pUser->m_luckyDrawMgr.m_lastFreeDraw &&
				pUser->m_luckyDrawMgr.m_lastFreeDraw + 86400 >= g_gameServer->getSec()) {
			if (!pUser->removeAssert(info.m_moneyType, info.m_needMoney, 0, "幸运抽奖消耗")) {
				retCmd.set_result(MiscProto::stRetLuckyDrawReward::eDiamond_Less);
				pUser->sendProto2Me(retCmd);
				return ;
			}
			costDiamond = info.m_needMoney;
		} else {
			useFree = true;
		}
	} else {
		if (!pUser->removeAssert(info.m_moneyType, info.m_needMoney, 0, "幸运抽奖消耗")) {
			retCmd.set_result(MiscProto::stRetLuckyDrawReward::eDiamond_Less);
			pUser->sendProto2Me(retCmd);
			return ;
		}
		costDiamond = info.m_needMoney;
	}
	UINT32 rate = 0, tmpRate = 0;
	UINT32 drawNum = pUser->m_luckyDrawMgr.m_drawNum;
	size_t i = 0;
	bool inCertain = false;
	LuckyDrawRecord record;
	std::vector<std::pair<UINT32, UINT32> > recordVec;;
	std::vector<LuckyDrawRecord> notifyVec;;
	for (UINT32 k = 0; k < maxTime; k++) {
		drawNum++;
		inCertain = false;
		if (certain.m_baseNum != certain.m_addNum) {
			if (drawNum % certain.m_addNum == certain.m_baseNum) {
				//certain中抽奖
				inCertain = true;
			}
		} else {
			if (drawNum % certain.m_baseNum == 0) {
				//certain中抽奖
				inCertain = true;
			}
		}
		tmpRate = 0;
		if (inCertain) {
			rate = g_randBetween(1, certain.m_totalRate);
			std::vector<CLuckyDrawCfg::stDrawItem> & item = certain.m_items;
			for (i = 0; i < item.size(); i++) {
				tmpRate += item[i].m_rate;
				if (tmpRate >= rate) {
					break;
				}
			}
			if (i >= item.size()) {
				continue;
			}
			recordVec.push_back(std::make_pair(item[i].m_id, item[i].m_num));
		} else {
			rate = g_randBetween(1, info.m_totalRate);
			std::vector<CLuckyDrawCfg::stDrawItem> &item = info.m_items;
			for (i = 0; i < item.size(); i++) {
				tmpRate += item[i].m_rate;
				if (tmpRate >= rate) {
					break;
				}
			}
			if (i >= item.size()) {
				continue;
			}
			recordVec.push_back(std::make_pair(item[i].m_id, item[i].m_num));
		}
	}
	pUser->m_luckyDrawMgr.m_drawNum = drawNum;
	pUser->m_luckyDrawMgr.m_diamond += costDiamond;
	if (useFree) {
		pUser->m_luckyDrawMgr.m_lastFreeDraw = g_gameServer->getSec();
	}
	Role::stObtainItemTip itemTipCmd;
	Role::stSynTipItem * pSyn = NULL;
	for (i = 0; i < recordVec.size(); i++) {
		MiscProto::stLuckyDrawRewardInfo * pReward = retCmd.add_reward();
		pReward->set_id(recordVec[i].first);
		pReward->set_num(recordVec[i].second);
		pSyn = itemTipCmd.add_getitems();
		pSyn->set_itemid(recordVec[i].first);
		pSyn->set_num(recordVec[i].second);
		pUser->giveAssert(recordVec[i].first, recordVec[i].second, 0, "领取幸运抽奖奖励");
	}
	stItemBase * pItemBase = NULL;
	for (i = 0; i < recordVec.size(); i++) {
		pItemBase = g_itemBaseMgr.get(recordVec[i].first);
		if (!pItemBase || !pItemBase->m_bNeedNotify){
			continue;
		}
		record.id = recordVec[i].first;
		record.num = recordVec[i].second;
		record.name = pUser->m_roleInfo.m_name;
		notifyVec.push_back(record);
	}
	GameUser::addLuckyDrawRecord(notifyVec);
	GameUser::loadLuckyDrawRecord(notifyVec);
	for (i = 0; i < notifyVec.size(); i++) {
		MiscProto::stLuckyDrawUserInfo * player = retCmd.add_player();
		player->set_id(notifyVec[i].id);
		player->set_num(notifyVec[i].num);
		player->set_name(notifyVec[i].name);
	}
	retCmd.set_lastfreetime(pUser->m_luckyDrawMgr.m_lastFreeDraw);
	retCmd.set_diamond(pUser->m_luckyDrawMgr.m_diamond);
	pUser->sendProto2Me(retCmd);
	pUser->sendProto2Me(itemTipCmd);
}

void MiscModHandle::ReqLuckyDrawPointReward(GameUser * pUser, MiscProto::stReqLuckyDrawPointReward * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	MiscProto::stRetLuckyDrawPointReward retCmd;
	retCmd.set_activityid(recvCmd->activityid());
	retCmd.set_pointid(recvCmd->pointid());
	if (CLuckyDrawCfg::getSingleton().isExpireId(recvCmd->activityid(), g_gameServer->getSec())) {
		retCmd.set_result(MiscProto::stRetLuckyDrawPointReward::eActivity_Expire);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	if (pUser->m_luckyDrawMgr.m_curId != recvCmd->activityid()) {
		retCmd.set_result(MiscProto::stRetLuckyDrawPointReward::eNo_Such_Id);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	CLuckyDrawCfg::PointReward reward;
	if (!CLuckyDrawCfg::getSingleton().getPointReward(
				CLuckyDrawCfg::getSingleton().getActivityId(recvCmd->activityid()), recvCmd->pointid(), reward)) {
		retCmd.set_result(MiscProto::stRetLuckyDrawPointReward::eNo_Such_Id);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	if (pUser->m_luckyDrawMgr.m_diamond < reward.num) {
		retCmd.set_result(MiscProto::stRetLuckyDrawPointReward::eDiamond_Less);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 5) {
		retCmd.set_result(MiscProto::stRetLuckyDrawPointReward::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	if (!pUser->m_luckyDrawMgr.drawReward(recvCmd->pointid())) {
		retCmd.set_result(MiscProto::stRetLuckyDrawPointReward::eAlready_Get);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	pUser->m_boxMgr.openBox(reward.boxId);
	retCmd.set_result(MiscProto::stRetLuckyDrawPointReward::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void MiscModHandle::ReqShareInvite(GameUser * pUser, MiscProto::stReqShareInvite * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	MiscProto::stRetShareInvite cmd;
	pUser->m_dailyInvite.addNum();
	pUser->m_dailyInvite.setOnceReward();
	cmd.set_num(pUser->m_dailyInvite.getNum());
	pUser->sendProto2Me(cmd);
}

void MiscModHandle::ReqDailyShareInvite(GameUser * pUser, MiscProto::stReqDailyShareInvite * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_dailyInvite.checkSameDay();
	MiscProto::stRetDailyShareInvite cmd;
	cmd.set_num(pUser->m_dailyInvite.getNum());
	cmd.set_oncereward(pUser->m_dailyInvite.isDrawOnceReward());
	std::set<UINT32> & drawSet = pUser->m_dailyInvite.getDrawList();
	for (std::set<UINT32>::iterator it = drawSet.begin();
			it != drawSet.end(); it++) {
		cmd.add_id(*it);
	}
	pUser->sendProto2Me(cmd);
}

void MiscModHandle::ReqDrawDailyShareReward(GameUser * pUser, MiscProto::stReqDrawDailyShareReward * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	MiscProto::stRetDrawDailyShareReward cmd;
	cmd.set_id(recvCmd->id());
	pUser->m_dailyInvite.checkSameDay();
	if (pUser->m_dailyInvite.hasDrawDailyReward(recvCmd->id())) {
		cmd.set_result(MiscProto::stRetDrawDailyShareReward::eAlready_Get);
		pUser->sendProto2Me(cmd);
		return ;
	}
	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 1) {
		cmd.set_result(MiscProto::stRetDrawDailyShareReward::eBag_Full);
		pUser->sendProto2Me(cmd);
		return ;
	}
	const CInviteCfg::ShareReward * pReward = CInviteCfg::getSingleton().getSharedReward(recvCmd->id());
	if (!pReward) {
		cmd.set_result(MiscProto::stRetDrawDailyShareReward::eNot_Find_Id);
		pUser->sendProto2Me(cmd);
		return ;
	}
	if (pUser->m_dailyInvite.getNum() < pReward->shareNum) {
		cmd.set_result(MiscProto::stRetDrawDailyShareReward::eLess_Of_Num);
		pUser->sendProto2Me(cmd);
		return ;
	}
	pUser->m_dailyInvite.drawDailyReward(recvCmd->id());
	pUser->m_boxMgr.openBox(pReward->rewardBox);
	cmd.set_result(MiscProto::stRetDrawDailyShareReward::eSuccess);
	pUser->sendProto2Me(cmd);
}

void MiscModHandle::ReqDrawOnceShareReward(GameUser * pUser, MiscProto::stReqDrawOnceShareReward * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	MiscProto::stRetDrawOnceShareReward cmd;
	pUser->m_dailyInvite.checkSameDay();
	if (pUser->m_dailyInvite.isDrawOnceReward()) {
		cmd.set_result(MiscProto::stRetDrawOnceShareReward::eAlready_Get);
		pUser->sendProto2Me(cmd);
		return ;
	}
	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 1) {
		cmd.set_result(MiscProto::stRetDrawOnceShareReward::eBag_Full);
		pUser->sendProto2Me(cmd);
		return ;
	}
	if (!pUser->m_dailyInvite.hasOnceReward()) {
		cmd.set_result(MiscProto::stRetDrawOnceShareReward::eLess_Of_Num);
		pUser->sendProto2Me(cmd);
		return ;
	}
	pUser->m_dailyInvite.drawOnceReward();
	pUser->m_boxMgr.openBox(CInviteCfg::getSingleton().getDailySharedRewardBox());
	cmd.set_result(MiscProto::stRetDrawOnceShareReward::eSuccess);
	pUser->sendProto2Me(cmd);
}

void MiscModHandle::reqDailyReceiveState(GameUser * pUser, MiscProto::stReqReceiveState *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	MiscProto::stReceiveStateResult retCmd;
	if(pUser->m_countMgr.getDayRefreshValue(ECOUNT_REWARD_RECEIVE_NUM) >= 1){
		retCmd.set_isreceive(true);
	}else{
		retCmd.set_isreceive(false);
	}
	pUser->sendProto2Me(retCmd);
}

void MiscModHandle::reqReceiveReward(GameUser * pUser, MiscProto::stReqReceiveReward *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	CDailyRecharge::getSingleton().receiveReward(pUser);
}

void MiscModHandle::reqDrawLateWalfare(GameUser * pUser, MiscProto::stReqDrawLateWalfare *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
	pUser->drawLateWalfare(recvCmd->rewardid());
}

void MiscModHandle::reqDrawResGetBack(GameUser * pUser, MiscProto::stReqDrawResGetBack *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
	pUser->drawBackRes(recvCmd->type(), recvCmd->num());
}

void MiscModHandle::reqDrawAllResGetBack(GameUser * pUser, MiscProto::stReqDrawAllResGetBack *recvCmd) {
	if(!pUser || !recvCmd){
		return;
	}
	pUser->drawAllBackRes();
}


void MiscModHandle::reqOpenMonsterWindow(GameUser * pUser, MiscProto::stReqOpenMonsterWindow *recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	pUser->m_monsterMgr.openWindow();
}

void MiscModHandle::reqAttackMonster(GameUser * pUser, MiscProto::stReqAttackMonster * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	pUser->m_monsterMgr.attackMonster();
}

void MiscModHandle::reqBuyAttackTime(GameUser * pUser, MiscProto::stReqBuyAttackTime * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_monsterMgr.buyAttackTime();
}

void MiscModHandle::reqAKeyAttackMonster(GameUser * pUser, MiscProto::stReqAKeyAttackMonster * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_monsterMgr.aKeyAttackMonster();
}

void MiscModHandle::reqMountLevelUp(GameUser * pUser, MiscProto::stReqMountLevelUp *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	if(recvCmd->type() == MiscProto::eNormalLeveUp){
		pUser->m_mountMgr.mountLevelUp(recvCmd);
	}

	if(recvCmd->type() == MiscProto::eAKeyLevelUp){
		pUser->m_mountMgr.akeyLevelUp(recvCmd);
	}
}

void MiscModHandle::reqButtleMount(GameUser * pUser, MiscProto::stReqButtleMount * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_mountMgr.battleMount(recvCmd->id());
}

void MiscModHandle::reqMountFollow(GameUser * pUser, MiscProto::stReqMountFollow * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_mountMgr.followMaster();
}

void MiscModHandle::reqMountUnreal(GameUser * pUser, MiscProto::stReqMountUnreal * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_mountMgr.mountUnreal(recvCmd->mountid());
}

void MiscModHandle::reqOpenGrowWindow(GameUser * pUser, MiscProto::stReqOpenMountGrowWindow * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_mountMgr.openWindow();
}

void MiscModHandle::reqReceiveWflfare(GameUser * pUser, MiscProto::stReqReceiveWflfare * recvCmd)
{
	if(!pUser || !recvCmd){
		return;		
	}

	MiscProto::stReceiveWflfareResult retCmd;

	bool isOpen = pUser->m_wflfareMgr.isOpen();
	if(!isOpen){
		retCmd.set_result(MiscProto::stReceiveWflfareResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[冲级好礼](name=%s,charid=%d)失败，活动未开放",pUser->getName(), pUser->getID());
#endif
		return;
	}

	UINT16 level = recvCmd->level();
	UINT16 type = recvCmd->type();

	if(pUser->getLevel() < level){
		retCmd.set_result(MiscProto::stReceiveWflfareResult::eLevel_Too_Low);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[冲级好礼](name=%s,charid=%d)失败，等级不足",pUser->getName(), pUser->getID());
#endif
		return;
	}

	bool hasGot = pUser->m_wflfareMgr.checkHasGotReward(level, type);	
	if(hasGot){
		retCmd.set_result(MiscProto::stReceiveWflfareResult::eHas_Got_Reward);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[冲级好礼](name=%s,charid=%d)失败，已被领取",pUser->getName(), pUser->getID());
#endif
		return;
	}


	std::map<UINT16, stWflfareInfo> rewardMap;
	bool success = CLevelWflfareCfg::getSingleton().getWflfareInfoByLevel(rewardMap, level);
	if(!success){
		retCmd.set_result(MiscProto::stReceiveWflfareResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[冲级好礼](name=%s,charid=%d)失败，找不到配置(level=%d)!", pUser->getName(), pUser->getID(), level);
#endif
		return;
	}

	std::map<UINT16, stWflfareInfo>::iterator it = rewardMap.find(type);
	if(it == rewardMap.end()){
		retCmd.set_result(MiscProto::stReceiveWflfareResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[冲级好礼](name=%s,charid=%d)失败，找不到配置(type=%d)!", pUser->getName(), pUser->getID(), type);
#endif
		return;
	}

	stWflfareInfo  info = it->second;
	if (!pUser->removeResource(eResource_Diamond, info.m_costDiamond, "冲级好礼")){
		retCmd.set_result(MiscProto::stReceiveWflfareResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[冲级好礼](name=%s,charid=%d)失败，钻石不足!", pUser->getName(), pUser->getID());
#endif
		return;
	}

	pUser->m_wflfareMgr.addInfoToMap(level, type);

	Role::stObtainItemTip itemTipCmd;
	std::map<UINT32, UINT32>::iterator itemIt = info.m_rewardItem.begin();
	for(;itemIt != info.m_rewardItem.end(); ++itemIt){
		pUser->giveAssert(itemIt->first,itemIt->second,0,"冲级好礼");
		Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
		if (pSyn){
			pSyn->set_itemid(itemIt->first);
			pSyn->set_num(itemIt->second);
		}
	}
	pUser->sendProto2Me(itemTipCmd);

	retCmd.set_result(MiscProto::stReceiveWflfareResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	pUser->m_wflfareMgr.sendHasGotReward();
}

void MiscModHandle::reqPaySign(GameUser * pUser, MiscProto::stReqPaySign * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}
	char tokenBuf[1024] = { 0 };
	int  bufLen = 0;
	char digest[64] = {0};
	MiscProto::stRetPaySign retCmd;
	switch (recvCmd->sdkid())
	{
	case Laosiji:

		snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s%s",recvCmd->data().c_str(),"bq6w1g68");
		bufLen = strlen(tokenBuf);
		utils::Md5(tokenBuf,bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		retCmd.set_sign(digest);
		break;
	
	default:
		break;
	}
 
	pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("请求充值签名: data is %d, sign is %s",
			recvCmd->data().c_str(), retCmd.sign().c_str());
#endif
}

struct stReqHoneyP
{
	UINT32 m_charID; 
	UINT32 m_itemID; 
	UINT32 m_money;  
	std::string m_orderID;

	stReqHoneyP()
	{   
		m_charID = 0;
		m_itemID = 0;
		m_money = 0;
	}   
};

static void g_HttpRequestCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("获得的http响应信息%s",ret.c_str());
		}
	}
	else {
		Athena::logger->error("获得的http响应失败!");
	}
}

static void g_HoneyPCB(CHttpClient* client, CHttpResponse* response)
{
	MiscProto::stRetServerPay retCmd;stReqHoneyP * pData = NULL; 
	if (response && response->getHttpRequest()&& response->getHttpRequest()->getUserData()){
		pData = (stReqHoneyP *)response->getHttpRequest()->getUserData();
		response->getHttpRequest()->setUserData(NULL);
	}

	if (!pData)
	{
		Athena::logger->trace("请求工口扣点失败, 玩家信息丢失");
		SAFE_DELETE(pData);
		return;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(pData->m_charID);
	if (!pUser)
	{
		Athena::logger->fatal("请求工口扣点失败, 玩家%d不在线",pData->m_charID);
		SAFE_DELETE(pData);
		return;
	}
	
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){ 
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("请求工口扣点返回:%s",ret.c_str());
 
            Json::Value configJson;
            Json::Reader reader;

            std::string code;
            std::string msg; 
            if (reader.parse(ret, configJson)) {

                if (configJson["result"].isString()){
                    code = configJson["result"].asString();
                }
                if (configJson["reason"].isString()){
                    msg = configJson["reason"].asString();
                }
            }
            if (code == "0000") {
                // Athena::logger->trace("扣点成功 user:%d, money:%d, itemid:%d",pData->m_charID, pData->m_money, pData->m_itemID);

				// pUser->m_rmbShop.buy(pData->m_money, pData->m_itemID, pData->m_orderID);
				// //上报tapdb
				// {
				// 	CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
				// 	pRequest->setUrl("https://e.tapdb.net/event");
				// 	pRequest->setRequestType(CHttpRequest::kHttpPost); 

				// 	Json::FastWriter writer; 
				// 	Json::Value content;	
					
				// 	content["module"] = "GameAnalysis"; 
				// 	content["name"] = "charge";  
				// 	content["index"] =  "lqolbg9tf7i2zq9o"; 
					  
				// 	content["identify"] = pUser->m_roleInfo.m_account;
				// 	content["type"] = "track"; 
				// 	content["user_id"] = pUser->m_roleInfo.m_account;//pData->m_charID;
				
				// 	Json::Value value;	
					
				// 	value["order_id"] = pData->m_orderID;
				// 	value["amount"] =  pData->m_money*10; //工口按人民币计算 乘100变分,除10是honeyp与人民币比率
				// 	value["virtual_currency_amount"] = 0;
				// 	value["currency_type"] =  "CNY";
					
				// 	std::ostringstream os;
				// 	os <<  pData->m_itemID;
				// 	value["product"] = os.str();
				
				// 	content["properties"] = value;
				// 	std::string json_file = writer.write(content);
				// 	Athena::logger->trace("上报内容文本是%s",json_file.c_str());
				// 	std::string code = HttpUtility::URLEncode(json_file);

				// 	pRequest->setRequestData(code.c_str(),code.size());
				// 	pRequest->setResponseCallback(g_HttpRequestCB);
				// 	CHttpClient::getSingleton().asynSend(pRequest);
				// }
				// retCmd.set_orderid( pData->m_orderID);
				// retCmd.set_result(MiscProto::stRetServerPay::eSuccess);
				// pUser->sendProto2Me(retCmd);

            }else if (code=="0101"){
                Athena::logger->trace("扣点失败, 余额不足 user:%d, money:%d, itemid:%d",pData->m_charID, pData->m_money, pData->m_itemID);
				retCmd.set_result(MiscProto::stRetServerPay::eMoney_Not_Enough);
				pUser->sendProto2Me(retCmd);
			}else{
				Athena::logger->trace("扣点失败, 错误码%s,原因%s, user:%d, money:%d, itemid:%d",code.c_str(),msg.c_str(), pData->m_charID, pData->m_money, pData->m_itemID);
				retCmd.set_result(MiscProto::stRetServerPay::eInner_Error);
				pUser->sendProto2Me(retCmd);
			}
		}  
	}
	else {
		Athena::logger->trace("请求工口扣点失败, user:%d, money:%d, itemid:%d", pData->m_charID, pData->m_money, pData->m_itemID);
		retCmd.set_result(MiscProto::stRetServerPay::eInner_Error);
		pUser->sendProto2Me(retCmd);
	}
	SAFE_DELETE(pData);
}


void GongKouHoneyP(GameUser * pUser, UINT32 itemid, UINT32 money, const char * token)
{
	// CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
 
	// pRequest->setUrl("https://gen.exchangcall.com/api/exchangeCoins"); 
	// pRequest->setRequestType(CHttpRequest::kHttpPost);

	// std::vector<std::string> headerVec;
	// headerVec.push_back("Content-Type:application/x-www-form-urlencoded");
	// headerVec.push_back("Accept:application/json");
	// char a[1000] = {0};
	// snprintf(a, sizeof(a) - 1,"Authorization:%s", token);
	// headerVec.push_back(a);
	// pRequest->setHeaders(headerVec);

	// //生成订单号
	// std::stringstream os;
	// os << Athena::global["serverID"] << "_" << pUser->getID() << "_" << g_gameServer->getSec();
	// std::string orderId = os.str();
	
	// //拼接request
	// char r[1024] = {0};
	// snprintf(r, sizeof(r) - 1,"{\n\"transaction_id\" : \"%s\",\n\"exchange_coins\" : %d,\n}\n",orderId.c_str(),money);
	// std::string request = r;
	// // Athena::logger->trace("拼接request: %s",request.c_str());
	// request = g_encode64(request);
	// // Athena::logger->trace("base64后request: %s",request.c_str());
	
	// char tokenBuf[1024] = {0};
	// snprintf(tokenBuf, sizeof(tokenBuf) - 1, "game_id:94&request:%s&key:1184DED920ACE583F02A9C3DC0533531", request.c_str());
	// // Athena::logger->trace("拼接签名串: %s",tokenBuf);
	// int bufLen = strlen(tokenBuf);
	// char digest[64] = {0};

	// utils::Md5(tokenBuf, bufLen, digest);
	// utils::HexString::bin2asc(digest,16,digest,64,bufLen);
	// digest[bufLen] = 0;
	// // Athena::logger->trace("MD5签名串: %s",digest);
	// std::string MD5String = digest;
	// std::transform(MD5String.begin(), MD5String.end(), MD5String.begin(), ::toupper);
	// // Athena::logger->trace("MD5签名串: %s",MD5String.c_str());

	// char szBuff[512] = { 0 };	
	// snprintf(szBuff,sizeof(szBuff),"game_id=94&signMsg=%s&request=%s",MD5String.c_str(),request.c_str());
	// pRequest->setRequestData(szBuff,strlen(szBuff));
  
	// stReqHoneyP * pData = ATHENA_NEW stReqHoneyP();
	// pData->m_charID = pUser->getID(); 
	// pData->m_itemID = itemid;
	// pData->m_money = money;
	// pData->m_orderID = orderId;
	// pRequest->setUserData(pData);

	// pRequest->setResponseCallback(g_HoneyPCB);
	// CHttpClient::getSingleton().asynSend(pRequest);
}

void MiscModHandle::reqServerPay(GameUser * pUser, MiscProto::stReqServerPay * recvCmd) {
	if (!pUser || !recvCmd){
		return ;
	}

	MiscProto::stRetServerPay retCmd;

	//校验
	if (!pUser->m_rmbShop.existsItem(recvCmd->itemid())) {
		retCmd.set_result(MiscProto::stRetServerPay::eItem);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	switch (recvCmd->sdkid())
	{
	case GongKou:
		//工口,opid=1007
		if (!pUser->m_rmbShop.checkOpItemMoney(recvCmd->itemid(), 1007, recvCmd->money())) {
			retCmd.set_result(MiscProto::stRetServerPay::eMoney);
			pUser->sendProto2Me(retCmd);
			return ;
		}
		GongKouHoneyP(pUser, recvCmd->itemid(), recvCmd->money(), recvCmd->token().c_str());
		break;
	
	default:
		break;
	}
}

