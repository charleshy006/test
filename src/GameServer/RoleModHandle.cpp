#include "RoleModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "MapMgr.h"
#include "CopyMap.h"
#include "Hero.h"
#include "Npc.h"
#include "Skill.h"
#include "Chat.h"
#include "Global.pb.h"
#include "GlobalSvrClient.h"
#include "MiscCfgMgr.h"
#include "GameEventNotify.h"
#include "KoreaLog.h"
#include "EnchantCfg.h"
#include "RuneCfg.h"
#include "GemCfg.h"
#include "MarryCfg.h"
#include "OpenFunction.h"
#include "Friend.pb.h"
#include "MarryMgr.h"
#include "SmeltItemCfg.h"
#include "MountSynthesisCfg.h"
#include "ProFilePhotoCfg.h"
#include "SysLanguages.h"
#include "HeroTrainCfg.h"
#include "RecastEquipCfg.h"

extern GameServer * g_gameServer;

RoleModHandle::RoleModHandle()
{
	registerHandle(&RoleModHandle::processChatCmd);
	registerHandle(&RoleModHandle::heroGoOutForBattle);
	registerHandle(&RoleModHandle::heroChange2WaitState);
	registerHandle(&RoleModHandle::reqCombineHero);
	registerHandle(&RoleModHandle::reqLevelupHero);
	registerHandle(&RoleModHandle::reqHeroEvolution);
	registerHandle(&RoleModHandle::reqHeroStudySkill);
	registerHandle(&RoleModHandle::reqRefineSkill);
	registerHandle(&RoleModHandle::reqReplaceSkill);
	registerHandle(&RoleModHandle::reqUpgradeSkill);
	registerHandle(&RoleModHandle::reqArrangeBag);
	registerHandle(&RoleModHandle::reqSellItem);
	registerHandle(&RoleModHandle::reqRaiseItem);
	registerHandle(&RoleModHandle::reqRefineItem);
	registerHandle(&RoleModHandle::reqSmeltItem);
	registerHandle(&RoleModHandle::reqRecastItem);
	registerHandle(&RoleModHandle::reqCombineItem);
	registerHandle(&RoleModHandle::reqShopList);
	registerHandle(&RoleModHandle::reqBuyItem);
	registerHandle(&RoleModHandle::reqRefreshShoplist);
	registerHandle(&RoleModHandle::reqMoveItem);

	registerHandle(&RoleModHandle::reqFinishLv);
	registerHandle(&RoleModHandle::reqCharpterReward);
	registerHandle(&RoleModHandle::reqRush);
	registerHandle(&RoleModHandle::reqResetRushNum);
	registerHandle(&RoleModHandle::reqUseItem);
	registerHandle(&RoleModHandle::getEquipSkill);
	registerHandle(&RoleModHandle::refineEquipSkill);
	registerHandle(&RoleModHandle::moveEquipSkill2Slot);
	registerHandle(&RoleModHandle::upgradeEquipSkill);

	registerHandle(&RoleModHandle::upgradeUserSkill);
	registerHandle(&RoleModHandle::saveNewBieGuideID);
	registerHandle(&RoleModHandle::reqSPLilianRecoverTime);
	registerHandle(&RoleModHandle::swapHeroBattlePos);

	registerHandle(&RoleModHandle::saveSystemSetting);
	registerHandle(&RoleModHandle::unlockRareSkill);
	registerHandle(&RoleModHandle::upgradeRareSkill);
	registerHandle(&RoleModHandle::levelOpenBox);
	registerHandle(&RoleModHandle::reqFiveTimeRaiseItem);
	registerHandle(&RoleModHandle::reqHero10TimesStudySkill);
	registerHandle(&RoleModHandle::getEquipSkill10Times);
	registerHandle(&RoleModHandle::gotFirstRechargeReward);
	registerHandle(&RoleModHandle::gotFirst10RechargeReward);
	registerHandle(&RoleModHandle::saveSubNewBieGuideID);
	registerHandle(&RoleModHandle::resetTeamCopymap);
	registerHandle(&RoleModHandle::resetRealTimeCopymap);
	registerHandle(&RoleModHandle::reqChgEquipSkill);
	registerHandle(&RoleModHandle::reqUseMonthCard);
	registerHandle(&RoleModHandle::reqUseSelectItemBox);
	registerHandle(&RoleModHandle::reqUseHeroSkillBook);
	registerHandle(&RoleModHandle::reqUseEquipSkillBook);
	registerHandle(&RoleModHandle::reqBreakItem);
	registerHandle(&RoleModHandle::reqRecastHero);
	registerHandle(&RoleModHandle::reqOpenBuyGoldWindow);
	registerHandle(&RoleModHandle::reqBuyGold);
	registerHandle(&RoleModHandle::reqEnchant);
	registerHandle(&RoleModHandle::reqRecastEnchant);
	registerHandle(&RoleModHandle::reqLockEnchant);
	registerHandle(&RoleModHandle::reqHeroRune);
	registerHandle(&RoleModHandle::reqAddHeroRune);
	registerHandle(&RoleModHandle::reqDelHeroRune);
	registerHandle(&RoleModHandle::reqRunePieceCombine);
	registerHandle(&RoleModHandle::reqRuneIntensify);
	registerHandle(&RoleModHandle::reqRuneEvolve);
	registerHandle(&RoleModHandle::reqExtractGem);
	registerHandle(&RoleModHandle::reqWedding);
	registerHandle(&RoleModHandle::reqAgreeWedding);
	registerHandle(&RoleModHandle::reqDivorce);
	registerHandle(&RoleModHandle::reqRejectDivorce);
	registerHandle(&RoleModHandle::reqBuyNum);
	registerHandle(&RoleModHandle::reqBlackMarketList);
	registerHandle(&RoleModHandle::reqBuyBlackMarketItem);
	registerHandle(&RoleModHandle::reqOpenBlackMarkerWindow);
	registerHandle(&RoleModHandle::reqRefreshList);
	registerHandle(&RoleModHandle::reqRefreshNum);
	registerHandle(&RoleModHandle::reqAKeyRaiseUserSkillLv);
	registerHandle(&RoleModHandle::reqStoneSynthesis);
	registerHandle(&RoleModHandle::reqStoneEvolvement);
	registerHandle(&RoleModHandle::reqStoneProp);
	registerHandle(&RoleModHandle::reqChangePhoto);
	registerHandle(&RoleModHandle::reqUnlockPhoto);
	registerHandle(&RoleModHandle::reqHasPhotoIDs);
	registerHandle(&RoleModHandle::reqSpriteInfo);
	registerHandle(&RoleModHandle::reqTrainHero);
	registerHandle(&RoleModHandle::reqGuajiAward);
	registerHandle(&RoleModHandle::reqGuajiStart);
}

RoleModHandle::~RoleModHandle()
{

}

bool RoleModHandle::handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<RoleModHandle,GameUser>::handle(pUser,pData,cmdLen,funID);
}

void RoleModHandle::processChatCmd(GameUser * pUser,Role::stChatCmd * recvCmd)
{
	if (!pUser || !recvCmd) {
		return  ;
	}

	std::string cmd = recvCmd->content();
	if (ChatMgr::exec(pUser,cmd)){//指令执行成功
		return ;
	}

	Role::stChatCmd retCmd;
	retCmd.set_content(recvCmd->content());
	retCmd.set_destcharid(recvCmd->destcharid());
	retCmd.set_srccharid(pUser->getID());
	retCmd.set_srccharname(pUser->getName());
	retCmd.set_type(recvCmd->type());
	retCmd.set_job(pUser->getJob());
	retCmd.set_sex(pUser->getSex());
	retCmd.set_viplv(pUser->getVipLv());
	retCmd.set_voiceurl(recvCmd->voiceurl());
	retCmd.set_isvoice(recvCmd->isvoice());
	retCmd.set_voicelenght(recvCmd->voicelenght());
	retCmd.set_photoid(recvCmd->photoid());

	switch (recvCmd->type()){
		case Role::CHAT_TYPE_WORLD :
			{
				if (pUser->m_lastSpeakTime + CMiscCfgMgr::getSingleton().getChatLimitTime() > g_gameServer->getSec()){
					Role::stChatInWorldCoolDown errorCmd;
					errorCmd.set_remaintime(pUser->m_lastSpeakTime + CMiscCfgMgr::getSingleton().getChatLimitTime() - g_gameServer->getSec());
					pUser->sendProto2Me(errorCmd);
					return ;
				}
				pUser->m_lastSpeakTime = g_gameServer->getSec();
				GameUserManager::getSingleton().sendProto2World(retCmd);
				return ;
			}
			break;
		case Role::CHAT_TYPE_PRIVATE :
			{
				GameUser * pOther = GameUserManager::getSingleton().getUser(const_cast<char *>(recvCmd->destcharname().c_str()));
				if (pOther){
					pOther->sendProto2Me(retCmd);
					pUser->sendProto2Me(retCmd);
				}
				else {
					Role::stPrivateChatNotUser errorCmd;
					errorCmd.set_destname(recvCmd->destcharname());
					pUser->sendProto2Me(errorCmd);
				}
				return ;
			}
			break;
		case Role::CHAT_TYPE_GUILD :
			{
				Global::stBroadcastGuildChatInfo forwardCmd;
				forwardCmd.set_charid(pUser->getID());
				forwardCmd.set_viplv(pUser->getVipLv());
				forwardCmd.set_content(recvCmd->content());
				forwardCmd.set_isvoice(recvCmd->isvoice());
				forwardCmd.set_voiceurl(recvCmd->voiceurl());
				forwardCmd.set_voicelenght(recvCmd->voicelenght());
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(forwardCmd);
			}
			break;
		case Role::CHAT_TYPE_TEAM :
			{
				Global::stSendTeamChatInfo forwardCmd;
				forwardCmd.set_charid(pUser->getID());
				forwardCmd.set_viplv(pUser->getVipLv());
				forwardCmd.set_content(recvCmd->content());
				forwardCmd.set_isvoice(recvCmd->isvoice());
				forwardCmd.set_voiceurl(recvCmd->voiceurl());
				forwardCmd.set_voicelenght(recvCmd->voicelenght());
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(forwardCmd);
			}
			break;
		case Role::CHAT_TYPE_TEAMINFO :
			{

			}
			break;
		case Role::CHAT_TYPE_NOTICE :
			{}
			break;
	}
}

void RoleModHandle::heroGoOutForBattle(GameUser * pUser,Role::stReqHeroGoOutForBattle * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::HeroGoOutForBattleResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	bool battle = false;
	if (pUser->m_heroMgr.isBattle(recvCmd->heroid())){
		battle = true;
	}

	if (CMiscCfgMgr::getSingleton().getLvByBattlePetPos(recvCmd->pos()) >  pUser->getLevel()){//等级限制
		retCmd.set_retcode(Role::HeroGoOutForBattleResult::GO_OUT_TOO_MANY);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	/*if (pUser->m_heroMgr.battleHeroNum() >= eHERO_MAX_BATTLE_NUM){//出战人数限制
	  retCmd.set_retcode(Role::HeroGoOutForBattleResult::GO_OUT_TOO_MANY);
	  pUser->sendProto2Me(retCmd);
	  return ;
	  }*/

	UINT8 index = recvCmd->pos() - 1;
	if (!pUser->m_heroMgr.addBattleHero(recvCmd->heroid(),index)){
		retCmd.set_retcode(Role::HeroGoOutForBattleResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_retcode(Role::HeroGoOutForBattleResult::SUCCESS);
	retCmd.set_pos(recvCmd->pos());
	pUser->sendProto2Me(retCmd);
	if(!battle){
		pUser->m_heroMgr.refreshImpactHero(recvCmd->heroid(),true);
	}
	pUser->setupRoleState();
	//pUser->sendMainData2Me();
	pUser->sendBattleProp();
}

void RoleModHandle::heroChange2WaitState(GameUser *pUser,Role::ReqHeroChange2WaitState * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::HeroChange2WaitStateResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	UINT8 index = pUser->m_heroMgr.getBattlePos(recvCmd->heroid());
	if (index == (UINT8)-1){
		retCmd.set_success(false);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->m_heroMgr.delBattleHero(recvCmd->heroid(),index)){
		retCmd.set_success(false);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_success(true);
	pUser->sendProto2Me(retCmd);
	pUser->m_heroMgr.refreshImpactHero(recvCmd->heroid());
	pUser->setupRoleState();
	//pUser->sendMainData2Me();
	pUser->sendBattleProp();
}

void RoleModHandle::reqCombineHero(GameUser *pUser,Role::stReqCombineHero * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stCombineHeroResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());

	if (pHeroInfo){//宠物已经存在了
		retCmd.set_result(Role::stCombineHeroResult::HAS_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将合成]已经存在了武将,合成失败");
#endif
		return ;
	}


	if (!pUser->m_bagMgr.removeItemByItemID(recvCmd->heroid(),HeroInfoMgr::s_COMBINE_NEED_STONE_NUM,"武将合成")){
		retCmd.set_result(Role::stCombineHeroResult::NOT_ENOUGH_STONE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将合成]没有足够石头,合成失败");
#endif
		return ;
	}


	pHeroInfo = pUser->m_heroMgr.createHero(recvCmd->heroid(),0);
	if (!pHeroInfo){
		Athena::logger->error("[武将合成]创建武将不成功,合成失败");
		retCmd.set_result(Role::stCombineHeroResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->m_heroMgr.addHeroInfo(pHeroInfo)){
		Athena::logger->error("[武将合成],把武将添加到容易不成功,合成失败");
		retCmd.set_result(Role::stCombineHeroResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_result(Role::stCombineHeroResult::SUCCESS);
	pUser->sendProto2Me(retCmd);
	pUser->setupRoleState(false);
	pUser->sendBattleProp();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[武将合成]合成成功!");
#endif
}

void RoleModHandle::reqLevelupHero(GameUser * pUser,Role::stReqLevelupHero * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stLevelupHeroResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		retCmd.set_result(Role::stLevelupHeroResult::HERO_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级](name=%s,id=%d)找不到要升级的武将,升级失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	stHeroLvPropBase * pLvBase = g_heroLvPropBaseMgr.get(stHeroLvPropBase::genFindID(pHeroInfo->m_dbData.m_id,pHeroInfo->m_dbData.m_level));

	if (!pLvBase){
		retCmd.set_result(Role::stLevelupHeroResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级](name=%s,id=%d)找不到升级表格信息,升级失败",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (pUser->getLevel() < pLvBase->m_roleLvLimit){//玩家等级限制
		retCmd.set_result(Role::stLevelupHeroResult::ROLE_LV_LIMIT);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级](name=%s,id=%d)玩家等级限制,升级失败",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (HERO_GROWN_TYPE_UPGRADE == pLvBase->m_lvType){//升级
		if (!pUser->removeResource(eResource_GoldCoin,pLvBase->m_costGoldCoin,"武将升级")){
			retCmd.set_result(Role::stLevelupHeroResult::NOT_ENOUGH_MONEY);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[武将升级](name=%s,id=%d)金币不足,升级失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}
	else if (HERO_GROWN_TYPE_BREAK == pLvBase->m_lvType){//突破
		if (!pUser->m_bagMgr.removeItemByItemID(pLvBase->m_levelItemID,pLvBase->m_levelItemNum,"武将突破")){
			retCmd.set_result(Role::stLevelupHeroResult::NOT_ENOUGH_STONE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[武将升级](name=%s,id=%d)突破丹不足,突破失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}
	else if(HERO_GROWN_TYPE_HIGH_BREAK == pLvBase->m_lvType){//高级突破

		if(pUser->m_bagMgr.getItemNumByID(pLvBase->m_levelItemID) < pLvBase->m_levelItemNum){
			retCmd.set_result(Role::stLevelupHeroResult::NOT_ENOUGH_STONE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[武将升级](name=%s,id=%d)突破丹不足,突破失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}

		for(UINT8 i = 0; i < pLvBase->m_costStoneVec.size(); ++i)
		{
			UINT32 itemid = pLvBase->m_costStoneVec[i].first;
			UINT32 hasNum = 0;
			if(CItem::isWuHunShi(itemid)){
				hasNum = pUser->m_bagMgr.getPieceNumByID(itemid);
			}else{
				hasNum = pUser->m_bagMgr.getItemNumByID(itemid);
			}

			UINT32 needNum = pLvBase->m_costStoneVec[i].second;
			if(hasNum < needNum){
				retCmd.set_result(Role::stLevelupHeroResult::NOT_ENOUGH_STONE);
				pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[武将升级](name=%s,id=%d)升级需要的道具不足,升级失败!",pUser->getName(),pUser->getID());
#endif
				return ;
			}
		}

		if (!pUser->m_bagMgr.removeItemByItemID(pLvBase->m_levelItemID,pLvBase->m_levelItemNum,"武将突破")){
			retCmd.set_result(Role::stLevelupHeroResult::NOT_ENOUGH_STONE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[武将升级](name=%s,id=%d)突破丹不足,突破失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}

		for(UINT8 i = 0; i < pLvBase->m_costStoneVec.size(); ++i)
		{
			if (!pUser->m_bagMgr.removeItemByItemID(pLvBase->m_costStoneVec[i].first,pLvBase->m_costStoneVec[i].second,"武将进化")){
				retCmd.set_result(Role::stLevelupHeroResult::NOT_ENOUGH_STONE);
				pUser->sendProto2Me(retCmd);
#ifdef HDZ_DEBUG
				Athena::logger->error("[武将升级](name=%s,id=%d)升级需要的道具不足,升级失败!",pUser->getName(),pUser->getID());
#endif
				return ;
			}
		}
	}
	else {//正确的配置
		retCmd.set_result(Role::stLevelupHeroResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级](name=%s,id=%d)表格中的升级类型配置不正确,升级失败",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	Global::stReqCheckTask cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_squaretype(6);
	cmd.set_battletype(5);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);


	UINT16 oldLv = pHeroInfo->m_dbData.m_level;
	pHeroInfo->levelup();
	retCmd.set_curlv(pHeroInfo->m_dbData.m_level);
	retCmd.set_result(Role::stLevelupHeroResult::SUCCESS);
	pUser->m_heroMgr.refreshHero(pHeroInfo);

	if (pUser->m_heroMgr.isBattle(pHeroInfo->m_dbData.m_id)){
		pUser->m_heroMgr.refreshImpactHero(recvCmd->heroid(),true,false, true);
		// pUser->setupRoleState();
		// pUser->sendBattleProp();
	}

	pUser->sendProto2Me(retCmd);
	pUser->m_taskMgr.onPropChange(TaskProto::ET_HERO_BREAK_INTO_LV);
	//pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_HERO_NUM);
	stRaiseHeroNum raiseHeroNumNotify;
	raiseHeroNumNotify.notify(pUser,1);

	pUser->setupRoleState(false);
	pUser->sendBattleProp();

	//发送升级日志
	Game::stHeroPropChgLog webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_action(1);
	webCmd.set_charid(pUser->getID());
	webCmd.set_heroid(pHeroInfo->m_dbData.m_id);
	webCmd.set_level(pUser->getLevel());
	webCmd.set_name(pUser->getName());
	webCmd.set_oldlv(oldLv);
	webCmd.set_newlv(pHeroInfo->m_dbData.m_level);
	webCmd.set_viplv(pUser->getVipLv());
	webCmd.set_oldevolv(pHeroInfo->m_dbData.m_evolutionLv);
	webCmd.set_newevolv(pHeroInfo->m_dbData.m_evolutionLv);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[武将升级](name=%s,id=%d)成功!",pUser->getName(),pUser->getID());
#endif
}

void RoleModHandle::reqHeroEvolution(GameUser *pUser,Role::stReqHeroEvolution * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stEvolutionHeroResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		retCmd.set_result(Role::stEvolutionHeroResult::HERO_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将进化](name=%s,id=%d)找不到要进化的武将,进化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (pHeroInfo->m_dbData.m_evolutionLv >= HeroInfoMgr::s_MAX_EVOLUTION_LEVEL){
		retCmd.set_result(Role::stEvolutionHeroResult::FULL_LEVEL);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将进化](name=%s,id=%d)进化等级已经满级了,进化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	stHeroEvolutionBase * pEvolutionBase = g_heroEvolutionBaseMgr.get(stHeroEvolutionBase::getFindID(pHeroInfo->m_dbData.m_id,pHeroInfo->m_dbData.m_evolutionLv));
	if (!pEvolutionBase){
		retCmd.set_result(Role::stEvolutionHeroResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将进化](name=%s,id=%d),找不到进化配置文件,进化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->m_bagMgr.removeItemByItemID(pEvolutionBase->m_stoneID,pEvolutionBase->m_stoneNum,"武将进化")){
		retCmd.set_result(Role::stEvolutionHeroResult::NOT_ENOUGH_STONE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将进化](name=%s,id=%d)进化需要的道具不足,进化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT16 oldLv = pHeroInfo->m_dbData.m_evolutionLv;
	pHeroInfo->evolution();
	pUser->m_heroMgr.refreshEvolutionHero(recvCmd->heroid());
	retCmd.set_result(Role::stEvolutionHeroResult::SUCCESS);
	pUser->m_heroMgr.refreshHero(pHeroInfo);
	pUser->m_taskMgr.onPropChange(TaskProto::ET_HERO_LEVELUP_TO_LV);

	if (pUser->m_heroMgr.isBattle(pHeroInfo->m_dbData.m_id)){
		pUser->m_heroMgr.refreshImpactHero(recvCmd->heroid(),true,false, true);
	}

	pUser->setupRoleState();
	pUser->sendBattleProp();

	pUser->sendProto2Me(retCmd);
	Game::stHeroPropChgLog webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_action(2);
	webCmd.set_charid(pUser->getID());
	webCmd.set_heroid(pHeroInfo->m_dbData.m_id);
	webCmd.set_level(pUser->getLevel());
	webCmd.set_name(pUser->getName());
	webCmd.set_oldlv(pHeroInfo->m_dbData.m_level);
	webCmd.set_newlv(pHeroInfo->m_dbData.m_level);
	webCmd.set_newevolv(pHeroInfo->m_dbData.m_evolutionLv);
	webCmd.set_oldevolv(oldLv);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[武将进化](name=%s,id=%d)进化成功!",pUser->getName(),pUser->getID());
#endif
}

void RoleModHandle::reqHeroStudySkill(GameUser *pUser,Role::stReqHeroStudySkill * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stHeroStudySkillResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());

    if (pUser->getLevel() < CMiscCfgMgr::getSingleton().getOperaLvlLimit(CMiscCfgMgr::Limit_Hero_Comprehen)) {
		Athena::logger->error("[武将领悟技能](name=%s,id=%d)找不到要进化的武将,人物等级不够",pUser->getName(),pUser->getID());
		return ;
    }

	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		retCmd.set_result(Role::stHeroStudySkillResult::HERO_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将领悟技能](name=%s,id=%d)找不到要进化的武将,进化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	stRoleBase * pHeroBase = g_roleBaseMgr.get(recvCmd->heroid());
	if (!pHeroBase){
		retCmd.set_result(Role::stHeroStudySkillResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将领悟技能](name=%s,id=%d)找不到配置!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (Role::stReqHeroStudySkill::USE_ITEM == recvCmd->type()){//用道具领悟
		if (!pUser->m_bagMgr.removeItemByItemID(LINGWUDAN_ITEM_ID,0,1,"领悟技能")){
			retCmd.set_result(Role::stHeroStudySkillResult::NOT_ENOUGH_ITEM);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}
	else if (Role::stReqHeroStudySkill::USE_DIAMOND == recvCmd->type()){//用钻石领悟
		UINT32 studyNum = pUser->m_countMgr.getDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM);
		if (studyNum >= pUser->getCountByVip(eVipFun_Hero_Understand_Skill)){
			retCmd.set_result(Role::stHeroStudySkillResult::USE_DIAMOND_NUM_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
			return ;
		}

		UINT32 needDiamond = CMiscCfgMgr::getSingleton().getStudySkillDiamond(studyNum);

		if (!pUser->removeResource(eResource_Diamond,needDiamond,"领悟技能")){
			retCmd.set_result(Role::stHeroStudySkillResult::NOT_ENOUGH_DIAMOND);
			pUser->sendProto2Me(retCmd);
			return ;
		}
		pUser->m_countMgr.addDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM,1);
	}
	else {
		Athena::logger->error("[领悟技能]name=%s,id=%d,领悟技能失败,不存在的领悟原料类型",pUser->getName(),pUser->getID());
		return ;
	}

	stSkillundBase * pSkillundBase = g_heroSkillundBaseMgr.get(pHeroBase->m_id);
	if (!pSkillundBase){
		retCmd.set_result(Role::stHeroStudySkillResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将领悟技能](name=%s,id=%d)找不到配置!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT32 maxVal = 0;
	std::map<UINT32,UINT32> skillOdds;
	for (UINT16 i = 0;i < pSkillundBase->m_randomSkillVec.size();++i){
		UINT32 skillID = pSkillundBase->m_randomSkillVec[i].first;
		UINT32 rate = pSkillundBase->m_randomSkillVec[i].second;
		skillOdds.insert(std::make_pair(skillID,rate));
		maxVal += rate;
	}

	UINT32 skillID = g_selectByMap(skillOdds,maxVal);
	if ((UINT32)-1 == skillID){
		retCmd.set_result(Role::stHeroStudySkillResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将领悟技能](name=%s,id=%d)随机不到技能!",pUser->getName(),pUser->getID());
#endif
		return ;
	}
	pHeroInfo->setStudySkillID(skillID);
	retCmd.set_result(Role::stHeroStudySkillResult::SUCCESS);
	retCmd.set_skillid(skillID);
	retCmd.set_skilllv(0);
	retCmd.set_usediamondnum(pUser->m_countMgr.getDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM));
	pUser->sendProto2Me(retCmd);

	Game::stHeroPropChgLog webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_action(3);
	webCmd.set_charid(pUser->getID());
	webCmd.set_heroid(pHeroInfo->m_dbData.m_id);
	webCmd.set_level(pUser->getLevel());
	webCmd.set_name(pUser->getName());
	webCmd.set_oldlv(pHeroInfo->m_dbData.m_level);
	webCmd.set_newlv(pHeroInfo->m_dbData.m_level);
	webCmd.set_newevolv(pHeroInfo->m_dbData.m_evolutionLv);
	webCmd.set_oldevolv(pHeroInfo->m_dbData.m_evolutionLv);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void RoleModHandle::reqHero10TimesStudySkill(GameUser * pUser,Role::stReqHero10TimeStudySkill * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	Role::stHero10TimesStudySkillResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());

	if ((pUser->getCountByVip(eVipFun_OneKeyStudyPetSkill) < 1) && (pUser->getLevel() < CMiscCfgMgr::getSingleton().getOneKeyStudyHeroSkillLv())){
		retCmd.set_result(Role::stHero10TimesStudySkillResult::eHas_VipRight);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		retCmd.set_result(Role::stHero10TimesStudySkillResult::eHero_Not_Exist);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将10次领悟技能](name=%s,id=%d)找不到武将,领悟失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	stRoleBase * pHeroBase = g_roleBaseMgr.get(recvCmd->heroid());
	if (!pHeroBase){
		retCmd.set_result(Role::stHero10TimesStudySkillResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将10次领悟技能](name=%s,id=%d)找不到配置!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	stSkillundBase * pSkillundBase = g_heroSkillundBaseMgr.get(pHeroBase->m_id);
	if (!pSkillundBase){
		retCmd.set_result(Role::stHero10TimesStudySkillResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将10次领悟技能](name=%s,id=%d)找不到配置!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT16 maxStudyNum = 0;
	UINT16 hasStudyNum = 0;		//已经学习了多少次
	UINT32 studySkillID = 0;	//学习到的技能
	UINT32 stuffNum = pUser->m_bagMgr.getItemNumByID(LINGWUDAN_ITEM_ID);
	if (stuffNum > 10){
		maxStudyNum = 10;
	}
	else {
		maxStudyNum = stuffNum;
	}

	UINT32 addSkillExp = 0;
	UINT32 maxVal = 0;
	std::map<UINT32,UINT32> skillOdds;
	for (UINT16 i = 0;i < pSkillundBase->m_randomSkillVec.size();++i){
		UINT32 skillID = pSkillundBase->m_randomSkillVec[i].first;
		UINT32 rate = pSkillundBase->m_randomSkillVec[i].second;
		skillOdds.insert(std::make_pair(skillID,rate));
		maxVal += rate;
	}

	bool gotYellow = false;

	if (0 == maxStudyNum){//如果没有道具,用钻石
		UINT32 studyNum = pUser->m_countMgr.getDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM);
		if (studyNum >= pUser->getCountByVip(eVipFun_Hero_Understand_Skill)){
			retCmd.set_result(Role::stHero10TimesStudySkillResult::eUse_Diamond_Num_Not_Enough);
			pUser->sendProto2Me(retCmd);
			return ;
		}

		UINT32 useDiamond = 0;
		UINT32 oldHasStudyNum = pUser->m_countMgr.getDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM);
		for (UINT16 i = 0;i < 10; ++i){
			if (oldHasStudyNum + hasStudyNum >= pUser->getCountByVip(eVipFun_Hero_Understand_Skill)){
				break;
			}

			UINT32 needDiamond = CMiscCfgMgr::getSingleton().getStudySkillDiamond(oldHasStudyNum + i);
			if (!pUser->checkResource(eResource_Diamond,useDiamond + needDiamond)){
				break;
			}
			useDiamond += needDiamond;
			studySkillID = g_selectByMap(skillOdds,maxVal);
			hasStudyNum += 1;

			stSkillBase * pSkillBase = g_skillBaseMgr.get(studySkillID);
			if (pSkillBase && (pSkillBase->m_quality >= 3)){//是紫色
				gotYellow = true;
				break;
			}
			SkillGrownBase * pSkillGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(studySkillID,0));
			if (pSkillGrownBase){
				addSkillExp += pSkillGrownBase->m_returnExp;
			}
		}

		if ((!pUser->removeResource(eResource_Diamond,useDiamond,"武将10次技能领悟")) || (hasStudyNum <= 0)){
			retCmd.set_result(Role::stHero10TimesStudySkillResult::eNot_Enough_Diamond);
			pUser->sendProto2Me(retCmd);
			return ;
		}
		pUser->m_countMgr.addDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM,hasStudyNum);
	}
	else {
		for (UINT16 i = 0;i < maxStudyNum; ++i){
			studySkillID = g_selectByMap(skillOdds,maxVal);
			if ((UINT32)-1 == studySkillID){
				break;
			}
			hasStudyNum += 1;
			stSkillBase * pSkillBase = g_skillBaseMgr.get(studySkillID);
			if (pSkillBase && (pSkillBase->m_quality >= 3)){//是橙色
				gotYellow = true;
				break;
			}

			SkillGrownBase * pSkillGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(studySkillID,0));
			if (pSkillGrownBase){
				addSkillExp += pSkillGrownBase->m_returnExp;
			}
		}


		if (!pUser->m_bagMgr.removeItemByItemID(LINGWUDAN_ITEM_ID,0,hasStudyNum,"武将10次领悟技能")){
			retCmd.set_result(Role::stHero10TimesStudySkillResult::eNot_Enough_Item);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}

	if (!gotYellow){
		studySkillID = 0;
	}

	pHeroInfo->setStudySkillID(studySkillID);
	if (addSkillExp > 0){
		pUser->addResource(eResource_Lingwu_Exp,addSkillExp,"10连领悟");
	}
	retCmd.set_result(Role::stHero10TimesStudySkillResult::eSuccess);
	retCmd.set_skillid(studySkillID);
	retCmd.set_addskillexp(addSkillExp);
	retCmd.set_studyskillnum(hasStudyNum);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name:%s,id:%d10次领悟获得了技能:%d",pUser->getName(),pUser->getID(),studySkillID);
#endif
	retCmd.set_usediamondnum(pUser->m_countMgr.getDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM));
	pUser->sendProto2Me(retCmd);

	Game::stHeroPropChgLog webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_action(4);
	webCmd.set_charid(pUser->getID());
	webCmd.set_heroid(pHeroInfo->m_dbData.m_id);
	webCmd.set_level(pUser->getLevel());
	webCmd.set_name(pUser->getName());
	webCmd.set_oldlv(pHeroInfo->m_dbData.m_level);
	webCmd.set_newlv(pHeroInfo->m_dbData.m_level);
	webCmd.set_newevolv(pHeroInfo->m_dbData.m_evolutionLv);
	webCmd.set_oldevolv(pHeroInfo->m_dbData.m_evolutionLv);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void RoleModHandle::reqRefineSkill(GameUser * pUser,Role::stReqRefineHeroSkill * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRefineHeroSkillResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		retCmd.set_result(Role::stRefineHeroSkillResult::HERO_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将熔炼技能](name=%s,id=%d)找不到要熔炼的武将,熔炼失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT32 skillID = pHeroInfo->getStudySkillID();
	if (!skillID){
		retCmd.set_result(Role::stRefineHeroSkillResult::HAS_NO_STUDY_SKILL);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将熔炼技能](name=%s,id=%d)没有领悟技能,熔炼失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	SkillGrownBase * pSkillBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillID,0));
	if (!pSkillBase){
		retCmd.set_result(Role::stRefineHeroSkillResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将熔炼技能](name=%s,id=%d)找不到技能配置文件,熔炼失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	pHeroInfo->setStudySkillID(0);
	pUser->m_heroMgr.addSkillExp(pSkillBase->m_returnExp,"熔炼技能");
	retCmd.set_result(Role::stRefineHeroSkillResult::SUCCESS);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqReplaceSkill(GameUser *pUser,Role::stReqReplaceHeroSkill * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stReplaceHeroSkillResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		retCmd.set_result(Role::stReplaceHeroSkillResult::HERO_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将替换技能](name=%s,id=%d)找不到要替换技能的武将,替换失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT32 skillID = pHeroInfo->getStudySkillID();
	if (!skillID){
		retCmd.set_result(Role::stReplaceHeroSkillResult::STUDY_SKILL_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将替换技能](name=%s,id=%d)找不到武将:%d的领悟技能,替换失败!",pUser->getName(),pUser->getID(),recvCmd->heroid());
#endif
		return ;
	}

	UINT32 returnExp = 0;
	if (pHeroInfo->isExistSkill(skillID)){
		stHeroSkillInfo skillInfo = pHeroInfo->getSkillIDByPos(recvCmd->pos());
		if (skillInfo.m_skillID != skillID){
			retCmd.set_result(Role::stReplaceHeroSkillResult::SKILL_HAS_EQUIP);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[武将替换技能](name=%s,id=%d)已经装备了改类型的技能,替换失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}

	stHeroSkillInfo skillInfo = pHeroInfo->getSkillIDByPos(recvCmd->pos());
	if (skillInfo.m_skillID > 0){
        UINT32 skillMaxLv = 0;
        {
            SkillGrownBase * pGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillInfo.m_skillID, 0));
            if (!pGrownBase){
                retCmd.set_result(Role::stReplaceHeroSkillResult::INNER_ERRRO);
                pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
                Athena::logger->error("[武将替换技能](name=%s,id=%d)找不到技能0级的配置文件,替换失败!",pUser->getName(),pUser->getID());
#endif
                return ;
            }
            skillMaxLv = pGrownBase->m_skillMaxLv;
        }
		SkillGrownBase * pSkillBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillInfo.m_skillID,
                    (skillInfo.m_skillLv > skillMaxLv ? skillMaxLv : skillInfo.m_skillLv)));
		if (!pSkillBase){
			retCmd.set_result(Role::stReplaceHeroSkillResult::INNER_ERRRO);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[武将替换技能](name=%s,id=%d)找不到技能配置文件,替换失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
		returnExp = pSkillBase->m_returnExp;
	}
	pHeroInfo->replaceSkill(recvCmd->pos(),skillID);
	pHeroInfo->setStudySkillID(0);
	pHeroInfo->init();
	pUser->m_heroMgr.refreshHero(pHeroInfo);

	if (returnExp > 0){
		pUser->addResource(eResource_Lingwu_Exp,returnExp,"替换领悟技能");
	}

	retCmd.set_heroid(recvCmd->heroid());
	retCmd.set_pos(recvCmd->pos());
	retCmd.set_skillid(skillID);
	retCmd.set_result(Role::stReplaceHeroSkillResult::SUCCESS);
	pUser->sendProto2Me(retCmd);
	pUser->setupRoleState();
	pUser->sendBattleProp();
}

void RoleModHandle::reqUpgradeSkill(GameUser *pUser,Role::stReqUpgradeHeroSkill * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stUpgradeHeroSkillResult  retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){

		retCmd.set_result(Role::stUpgradeHeroSkillResult::HERO_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级技能](name=%s,id=%d)找不到要升级技能的武将,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	stHeroSkillInfo skillInfo = pHeroInfo->getSkillIDByPos(recvCmd->pos());
	if (!skillInfo.m_skillID){
		retCmd.set_result(Role::stUpgradeHeroSkillResult::SKILL_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级技能](name=%s,id=%d)找不到要升级的技能,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}
    UINT32 skillMaxLv = 0;
    {
        SkillGrownBase * pGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillInfo.m_skillID, 0));
        if (!pGrownBase){
            retCmd.set_result(Role::stUpgradeHeroSkillResult::INNER_ERROR);
            pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
            Athena::logger->error("[武将升级技能](name=%s,id=%d)找不到技能成长配置,升级技能失败!",pUser->getName(),pUser->getID());
#endif
            return ;
        }
        skillMaxLv = pGrownBase->m_skillMaxLv;
    }

    if (skillInfo.m_skillLv >= skillMaxLv) {
		retCmd.set_result(Role::stUpgradeHeroSkillResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级技能](name=%s,id=%d)找不到技能成长配置,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
    }
	SkillGrownBase * pGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillInfo.m_skillID, skillInfo.m_skillLv));
	if (!pGrownBase){
		retCmd.set_result(Role::stUpgradeHeroSkillResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级技能](name=%s,id=%d)找不到技能成长配置,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->m_heroMgr.removeSkillExp(pGrownBase->m_cost,"升级技能")){
		retCmd.set_result(Role::stUpgradeHeroSkillResult::NOT_ENOUGH_EXP);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将升级技能](name=%s,id=%d)扣除技能经验失败,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	pHeroInfo->upgradeSkill(skillInfo.m_skillID, skillMaxLv);
	retCmd.set_skillid(skillInfo.m_skillID);
	retCmd.set_result(Role::stUpgradeHeroSkillResult::SUCCESS);
	retCmd.set_skilllv(skillInfo.m_skillLv + 1);
	pHeroInfo->init();
	pUser->m_heroMgr.refreshHero(pHeroInfo);
	pUser->sendProto2Me(retCmd);
	pUser->setupRoleState();
	pUser->sendBattleProp();

	Game::stSkillChgLog webCmd;
	webCmd.set_action(2);
	webCmd.set_decgoldcoind(pGrownBase->m_cost);
	webCmd.set_oldlv(skillInfo.m_skillLv);
	webCmd.set_newlv(skillInfo.m_skillLv + 1);
	webCmd.set_pos(recvCmd->pos());
	webCmd.set_skillid(skillInfo.m_skillID);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void RoleModHandle::reqArrangeBag(GameUser * pUser,Role::stReqArrangeMainBag * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	if (Role::BAG_TYPE_MAIN1 == recvCmd->bagtype()){
		pUser->m_bagMgr.m_mainBag.reArrange();
		pUser->m_bagMgr.m_mainBag.refreshBagItem(pUser);
	}
	else if (Role::BAG_TYPE_PIECE == recvCmd->bagtype()){
		pUser->m_bagMgr.m_pieceBag.reArrange();
		pUser->m_bagMgr.m_pieceBag.refreshBagItem(pUser);
	}
	else if (Role::BAG_TYPE_FASHION == recvCmd->bagtype()){
		pUser->m_bagMgr.m_fashionBag.reArrange();
		pUser->m_bagMgr.m_fashionBag.refreshBagItem(pUser);
	}
	else if (Role::BAG_TYPE_RUNE == recvCmd->bagtype()){
		pUser->m_bagMgr.m_runeBag.reArrange();
		pUser->m_bagMgr.m_runeBag.refreshBagItem(pUser);
	}
	else if (Role::BAG_TYPE_RUNE_PIECE == recvCmd->bagtype()){
		pUser->m_bagMgr.m_runePieceBag.reArrange();
		pUser->m_bagMgr.m_runePieceBag.refreshBagItem(pUser);
	}else if(Role::BAG_TYPE_MOUNT == recvCmd->bagtype()){
		pUser->m_bagMgr.m_mountBag.reArrange();
		pUser->m_bagMgr.m_mountBag.refreshBagItem(pUser);
	}
}

void RoleModHandle::reqSellItem(GameUser * pUser,Role::stReqSellItem * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stSellItemResult retCmd;
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		retCmd.set_result(Role::stSellItemResult::NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[出售道具](name=%s,id=%d)出售道具失败,找不到道具!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pItem->m_pItemBase->m_canSell){
		return ;
	}

	pUser->m_bagMgr.sellItem(pItem);

	if(pItem->m_pItemBase){
		Role::stObtainItemTip itemTipCmd;
		Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
		if(pSyn){
		    pSyn->set_itemid(eResource_GoldCoin);           
			pSyn->set_num(pItem->m_pItemBase->m_sellPrice * pItem->m_itemInfo.m_num);
		}
		pUser->sendProto2Me(itemTipCmd);   
	}

	retCmd.set_result(Role::stSellItemResult::SUCCESS);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqMoveItem(GameUser * pUser,Role::stMoveItem * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		Athena::logger->error("[移动道具](name=%s,id=%d)移动道具失败,找不到要移动的道具!",pUser->getName(),pUser->getID());
		return ;
	}

	BagPos destPos;
	destPos.m_bagType = recvCmd->bagpos().bagtype();
	destPos.m_pos = recvCmd->bagpos().pos();
	BagPos srcPos = pItem->m_itemInfo.m_pos;
	UINT32 itemID = pItem->getID();
    bool bLimit = false;
    if (srcPos.m_bagType == Role::BAG_TYPE_MAIN1 && destPos.m_bagType == Role::BAG_TYPE_EQUIP) {
        bLimit = true;
    }
    if (bLimit) {
        if (destPos.m_pos >= 51 && destPos.m_pos <= 56) {
            if (pItem->m_pItemBase && (pItem->m_pItemBase->m_type < ITEM_TYPE_HEAD ||
                    pItem->m_pItemBase->m_type > ITEM_TYPE_RING)) {
                Athena::logger->error("[reqMoveItem] catch limit thing");
                return ;
            }
        }
        if (destPos.m_pos == 7) {
            if (!pUser->isValidVipItem(itemID)) {
                Role::stChatCmd retCmd;
                retCmd.set_type(Role::CHAT_TYPE_NOTICE);
                retCmd.set_content(CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_VIP_LVL_NOT_EQUAL));
                //CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_WING_ERR_CODE));
                retCmd.set_destcharid(pUser->getID());
                retCmd.set_destcharname(pUser->m_account);
                retCmd.set_srccharid(0);
                retCmd.set_srccharname("系统");
                pUser->sendProto2Me(retCmd);
                return ;
            }
        }
    }
	if (pUser->m_bagMgr.moveItem(pItem,destPos)){
		Role::stMoveItem retCmd;
		retCmd.set_bagtype(recvCmd->bagtype());
		retCmd.set_instanceid(recvCmd->instanceid());
		retCmd.mutable_bagpos()->set_bagtype(destPos.m_bagType);
		retCmd.mutable_bagpos()->set_pos(destPos.m_pos);
		pUser->sendProto2Me(retCmd);

		if (CEquipBag::isAffectRoleData(srcPos) || CEquipBag::isAffectRoleData(destPos)){
			pUser->setupRoleState();
			//pUser->sendMainData2Me();
			pUser->sendBattleProp();
			pUser->synMyBattlePower();
		}

		if (CEquipBag::isAffectAppearance(srcPos) || CEquipBag::isAffectAppearance(destPos)){//是否影响换装
			Global::stNotifyCurEquipInfo synCmd;
			synCmd.set_index(destPos.m_pos);
			synCmd.set_itemid(itemID);
			synCmd.set_charid(pUser->getID());
			synCmd.set_effectid(pUser->getEffectID());
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(synCmd);
		}
	}
}

#define EXP_PER_YUNTIE 1
#define RAISE_EQUIP_NEED_MONEY_RATE 500

void RoleModHandle::reqRaiseItem(GameUser *pUser,Role::stReqRaiseItem * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem || !pItem->m_pItemBase){
		Athena::logger->error("[强化道具](name=%s,id=%d)强化失败,找不到要强化的道具!",pUser->getName(),pUser->getID());
		return ;
	}

	Role::stRaiseItemResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());

	UINT32 index = 0;
	index = stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,pItem->m_itemInfo.m_raiseLv );

	stItemLevelupBase * pLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,pItem->m_itemInfo.m_raiseLv ));

	if (!pLvUpBase){
		retCmd.set_result(Role::stRaiseItemResult::INNER_ERROR);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->getLevel() < pLvUpBase->m_roleLv){//等级不足
		retCmd.set_result(Role::stRaiseItemResult::ROLE_LV_LIMIT);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	std::map<CItem *,UINT32> delItemVec;
	UINT32 obtainExp = 0;
	UINT32 decYunTieNum =  0;
	if (Role::stReqRaiseItem::USE_STONE == recvCmd->raisetype()){
		if (CItem::isFuwenByType(pItem->m_pItemBase->m_type)){//符文不能用陨铁强化
			Athena::logger->error("[装备强化]符文不能花陨铁强化");
			return ;
		}

		UINT32 needExp = pLvUpBase->m_needExp > pItem->m_itemInfo.m_exp ? pLvUpBase->m_needExp - pItem->m_itemInfo.m_exp : 0;
		obtainExp += EXP_PER_YUNTIE * recvCmd->yuntienum();
		decYunTieNum = recvCmd->yuntienum();
		if (obtainExp >= needExp){
			decYunTieNum = needExp / EXP_PER_YUNTIE + (needExp % EXP_PER_YUNTIE ? 1 : 0);
		}

		//if (!pUser->checkResource (eResource_Yuntie, recvCmd->yuntienum ()))
		if (!pUser->checkResource (eResource_Yuntie, decYunTieNum))
		{
			retCmd.set_result(Role::stRaiseItemResult::STONE_NOT_EXIST);
			pUser->sendProto2Me(retCmd);
			Athena::logger->error ("客户端使用挂了,(name=%s,id=%d),进行强化", pUser->getName (), pUser->getID ());
			return;
		}

		if (!pUser->checkResource(eResource_Yuntie,decYunTieNum)){
			retCmd.set_result(Role::stRaiseItemResult::STONE_NOT_EXIST);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[装备强化](name=%s,id=%d)客户端传过来的石头ID不存在,强化失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}
	else if (Role::stReqRaiseItem::USE_EQUIP == recvCmd->raisetype()){//如果是用装备升级
		if (recvCmd->eatinstanceid_size() > 7){//不能用超过7个道具升级
			retCmd.set_result(Role::stRaiseItemResult::INNER_ERROR);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[装备强化](name=%s,id=%d)不能使用超过8个装备来强化,强化失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}

		for (UINT16 i = 0;i < recvCmd->eatinstanceid_size();++i){
			CItem * pStuffItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->eatinstanceid(i));

			if (pStuffItem == pItem){//如果原料就是自己,出现bug
				Athena::logger->error("[装备强化]强化时候出现错误,客户端发送过来的原料就是自己!");
				return ;
			}

			if(Athena::global["isLYServer"] == "true"){
				if (pStuffItem && (CItem::isEquipByType(pStuffItem->m_pItemBase->m_type))){
					return;
				}
			}

			if (pStuffItem && (CItem::isEquipByType(pStuffItem->m_pItemBase->m_type)
						|| CItem::isFuwenByType(pStuffItem->m_pItemBase->m_type)
						|| CItem::isFuWenExpStone(pStuffItem->m_pItemBase->m_type))){
				stItemLevelupBase * pRaiseBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pStuffItem->m_pItemBase->m_grownID,pStuffItem->m_itemInfo.m_raiseLv));
				obtainExp += pStuffItem->m_itemInfo.m_exp;
				if (pRaiseBase){
					obtainExp += pRaiseBase->m_exp;
				}
				delItemVec.insert(std::make_pair(pStuffItem,1));
			}
			else {
				retCmd.set_result(Role::stRaiseItemResult::EQUIP_NOT_EXIST);
				pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[装备强化](name=%s,id=%d)客户端传过来的装备ID不存在,强化失败!",pUser->getName(),pUser->getID());
#endif
				return ;
			}
		}
	}
	else {
		Athena::logger->error("[装备强化](name=%s,id=%d)强化失败,发送过来使用原来类型不存在",pUser->getName(),pUser->getID());
		return ;
	}

	UINT32 needMoney = 0;
	if (CItem::isFuwenByType(pItem->m_pItemBase->m_type)){//符文不花钱

	}
	else {
		needMoney = (UINT32)(obtainExp * (RAISE_EQUIP_NEED_MONEY_RATE * 1.0f/ 100));
	}

	if (!pUser->checkResource(eResource_GoldCoin,needMoney)){
		retCmd.set_result(Role::stRaiseItemResult::MONEY_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[装备强化](name=%s,id=%d)金币不足,强化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->removeResource(eResource_GoldCoin,needMoney,"强化装备")){
		retCmd.set_result(Role::stRaiseItemResult::MONEY_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[装备强化](name=%s,id=%d)扣钱不成功强化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	std::map<CItem *,UINT32>::iterator delIt =  delItemVec.begin();
	for (;delIt != delItemVec.end(); ++delIt){
		if (!pUser->m_bagMgr.decItem(delIt->first,delIt->second,"强化装备")){
			retCmd.set_result(Role::stRaiseItemResult::INNER_ERROR);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[装备强化](name=%s,id=%d)扣道具不成功强化失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}

	if (decYunTieNum > 0){
		if (!pUser->removeResource(eResource_Yuntie,decYunTieNum,"强化装备")){
			retCmd.set_result(Role::stRaiseItemResult::STONE_NOT_EXIST);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}

	UINT16 oldRaiseLv = pItem->m_itemInfo.m_raiseLv;

	if(CItem::isEquipByType(pItem->m_pItemBase->m_type)){
	    pItem->m_itemInfo.m_exp += obtainExp - pItem->m_itemInfo.m_exp;
	}else{
	    pItem->m_itemInfo.m_exp += obtainExp;
	}
	//pItem->m_itemInfo.m_exp += obtainExp;
	//pItem->m_itemInfo.m_exp += obtainExp - pItem->m_itemInfo.m_exp;

	while (pItem->m_itemInfo.m_exp >= pLvUpBase->m_needExp){
		pItem->m_itemInfo.m_exp -= pLvUpBase->m_needExp;
		pItem->m_itemInfo.m_raiseLv += 1;
		if (CItem::isFuwenByType(pItem->m_pItemBase->m_type)){//如果是符文
			if (pItem->m_itemInfo.m_raiseLv >= CMiscCfgMgr::getSingleton().getMaxRoleLvl()){
				pItem->m_itemInfo.m_exp = 0;
				break;
			}
		}
		else {
			if (pItem->m_itemInfo.m_raiseLv >= (CMiscCfgMgr::getSingleton().getMaxRoleLvl() * 2)){
				pItem->m_itemInfo.m_exp = 0;
				break;
			}
		}

		pLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,pItem->m_itemInfo.m_raiseLv));
		if (!pLvUpBase){
			break;
		}
	}

	pItem->setupPropState();
	pUser->m_bagMgr.refreshItem(pItem);
	retCmd.set_result(Role::stRaiseItemResult::SUCCESS);
	pUser->sendProto2Me(retCmd);

	if (CEquipBag::isAffectRoleData(pItem->m_itemInfo.m_pos)){
		pUser->setupRoleState();
		//pUser->sendMainData2Me();
		pUser->sendBattleProp();
	}

	//pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_EQUIP_TO_LV);
	//pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_EQUIP_NUM);
	//pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_FUWEN_TO_LV);
	//pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_FUWEN_NUM);
	if (CItem::isFuwenByType(pItem->m_pItemBase->m_type)){
		stRaiseFuwenNum raiseFuwenNumNotify;
		raiseFuwenNumNotify.notify(pUser,1);
		pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_FUWEN_TO_LV);
		pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),oldRaiseLv,pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_refineLv,pItem->m_itemInfo.m_refineLv,"强化符文",needMoney,decYunTieNum,0,0,0,0);
		Global::stReqCheckTask cmd;
		cmd.set_charid(pUser->getID());
		cmd.set_squaretype(6);
		cmd.set_battletype(3);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}
	else {
		stRaiseEquipNum raiseEquipNumNotify;
		raiseEquipNumNotify.notify(pUser,1);
		pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_EQUIP_TO_LV);
		pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),oldRaiseLv,pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_refineLv,pItem->m_itemInfo.m_refineLv,"强化装备",needMoney,decYunTieNum,0,0,0,0);
		Global::stReqCheckTask cmd;
		cmd.set_charid(pUser->getID());
		cmd.set_squaretype(6);
		cmd.set_battletype(2);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}
}

void RoleModHandle::reqFiveTimeRaiseItem(GameUser * pUser,Role::stReqFiveRaiseEquip * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了五连强化的协议!");
#endif
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem || !pItem->m_pItemBase){
		Athena::logger->error("[5连强化道具](name=%s,id=%d)强化失败,找不到要强化的道具!",pUser->getName(),pUser->getID());
		return ;
	}

	if (!g_gameServer->isAppVersion()){
		if ((pUser->getCountByVip(eVipFun_One_Key_Raise_Equip) < 1) && (pUser->getLevel() < CMiscCfgMgr::getSingleton().getOneKeyRaiseLv())){
			Athena::logger->error("vip等级不足,不能一键强化!");
			return ;
		}
	}

	Role::stFiveRaiseEquipResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());

	stItemLevelupBase * pToLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,recvCmd->tolv()));

	if (!pToLvUpBase){
		retCmd.set_result(Role::stFiveRaiseEquipResult::eINNER_ERROR);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->getLevel() < pToLvUpBase->m_roleLv){//等级不足
		retCmd.set_result(Role::stFiveRaiseEquipResult::eROLE_LV_LIMIT);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	//获得陨铁数量
	stGetAllBlueAndGreen getEquipCB;
	pUser->m_bagMgr.m_mainBag.exeEveryOne(getEquipCB);

	if(Athena::global["isLYServer"] == "true"){

	}else{
		UINT32 getYunTieNum = 0;
		for (UINT16 i = 0;i < getEquipCB.m_equipVecs.size(); ++i){
			CItem *pDestroyItem = getEquipCB.m_equipVecs[i];
			if (pDestroyItem && (pItem != pDestroyItem)){
				stItemLevelupBase * pDestroyItemLvBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pDestroyItem->m_pItemBase->m_grownID,pDestroyItem->m_itemInfo.m_raiseLv));
				if (pDestroyItemLvBase){
					//UINT32 needMoney = 0;
					if (CItem::isFuwenByType(pItem->m_pItemBase->m_type)){//符文不花钱

					}
					else {
						//needMoney = (UINT32)((getYunTieNum + pDestroyItemLvBase->m_exp + pUser->getYunTie()) * (RAISE_EQUIP_NEED_MONEY_RATE * 1.0f/ 100));
					}

					/*if (!pUser->checkResource(eResource_GoldCoin,needMoney)){//钱不够了
					  break;
					  }*/
					getYunTieNum += pDestroyItemLvBase->m_exp;
#ifdef _HDZ_DEBUG
					Athena::logger->trace("一键强化吃掉的装备:name:%s,id:%d得到经验:%d,当前总经验:%d",pDestroyItem->getName(),pDestroyItem->getID(),pDestroyItemLvBase->m_exp,getYunTieNum);
#endif
				}
				pUser->m_bagMgr.decItem(pDestroyItem,pDestroyItem->m_itemInfo.m_num,"一键强化");
			}
		}

		if (getYunTieNum) {
			pUser->addResource(eResource_Yuntie,getYunTieNum,"一键强化装备转换");
		}
	}

	UINT16 limitLv = 2 * pUser->getLevel();
	UINT32 totalYunTie = pUser->getYunTie();
	UINT32 totalYunTieNeedMoney = (UINT32)(totalYunTie * (RAISE_EQUIP_NEED_MONEY_RATE * 1.0f/ 100));
	if (!pUser->checkResource(eResource_GoldCoin,totalYunTieNeedMoney)){//钱不够
		totalYunTie = (UINT32)(pUser->getGoldCoin() / (RAISE_EQUIP_NEED_MONEY_RATE * 1.0f/ 100));
		if (totalYunTie > pUser->getYunTie()){
			totalYunTie = pUser->getYunTie();
		}
	}
	UINT32 needExp = 0;
	UINT32 totalExp = totalYunTie * EXP_PER_YUNTIE + pItem->m_itemInfo.m_exp;
	UINT16 beginLv = pItem->m_itemInfo.m_raiseLv;
	stItemLevelupBase * pLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,pItem->m_itemInfo.m_raiseLv));
	stItemLevelupBase * pLvUpNext = NULL;
	bool fullLevel = false;
	while (pLvUpBase && (beginLv < limitLv)){
		if (needExp + pLvUpBase->m_needExp > totalExp){//比总经验多,跳出来
			break;
		}
		pLvUpNext = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,beginLv+1));
		if (!pLvUpNext)
		{
			fullLevel = true;
			break;
		}
		
		needExp += pLvUpBase->m_needExp;
		beginLv += 1;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("一键强化:name:%s,id:%d,生到%d,需要经验:%d,累积需要经验:%d",pItem->getName(),pItem->getID(),beginLv,pLvUpBase->m_needExp,needExp);
#endif
		pLvUpBase = pLvUpNext;
	}

	UINT32 remainExp = 0;
	UINT32 decYumTieNum = 0;
	if (beginLv < limitLv && !fullLevel){//小于限制等级,说明是材料不足
		remainExp = totalExp > needExp ?totalExp - needExp : 0;
		decYumTieNum = totalYunTie;
	}
	else {
		if (needExp >= pItem->m_itemInfo.m_exp){
			decYumTieNum = (needExp - pItem->m_itemInfo.m_exp) / EXP_PER_YUNTIE;
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("一键强化,最终可以生到的等级:%d,扣除陨铁:%d,剩余经验:%d",beginLv,decYumTieNum,remainExp);
#endif
	UINT32 needMoney = 0;
	if (CItem::isFuwenByType(pItem->m_pItemBase->m_type)){//符文不花钱

	}
	else {
		needMoney = (UINT32)(decYumTieNum * (RAISE_EQUIP_NEED_MONEY_RATE * 1.0f/ 100));
	}

	if (!pUser->checkResource(eResource_Yuntie,decYumTieNum)){
		retCmd.set_result(Role::stFiveRaiseEquipResult::eYUNTIE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[一键强化](name=%s,id=%d)陨铁不足,强化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->checkResource(eResource_GoldCoin,needMoney)){
		retCmd.set_result(Role::stFiveRaiseEquipResult::eMONEY_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[一键强化](name=%s,id=%d)金币不足,强化失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (decYumTieNum > 0){
		if (!pUser->removeResource(eResource_Yuntie,decYumTieNum,"一键强化装备")){
			retCmd.set_result(Role::stFiveRaiseEquipResult::eYUNTIE_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}

	if (needMoney > 0){
		if (!pUser->removeResource(eResource_GoldCoin,needMoney,"一键强化装备")){
			retCmd.set_result(Role::stFiveRaiseEquipResult::eMONEY_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}

	UINT16 oldRaiseLv = pItem->m_itemInfo.m_raiseLv;
	pItem->m_itemInfo.m_raiseLv = beginLv;
	pItem->m_itemInfo.m_exp = remainExp;
	Athena::logger->trace("玩家(%s,%d)把道具(instanceID=%llu,itemID=%d,name=%s)从%d级升到了%d级,花费了%d金币,%d陨铁,当前还剩余%d",pUser->getName(),pUser->getID(),pItem->m_itemInfo.m_instanceID,pItem->getID(),pItem->getName(),oldRaiseLv,beginLv,needMoney,decYumTieNum,remainExp);
	pItem->setupPropState();
	pUser->m_bagMgr.refreshItem(pItem);
	retCmd.set_result(Role::stFiveRaiseEquipResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);

	if (CEquipBag::isAffectRoleData(pItem->m_itemInfo.m_pos)){
		pUser->setupRoleState();
		//pUser->sendMainData2Me();
		pUser->sendBattleProp();
	}

	if (CItem::isFuwenByType(pItem->m_pItemBase->m_type)){
		stRaiseFuwenNum raiseFuwenNumNotify;
		raiseFuwenNumNotify.notify(pUser,5);
		pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_FUWEN_TO_LV);
		pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),oldRaiseLv,pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_refineLv,pItem->m_itemInfo.m_refineLv,"一键强化符文",needMoney,decYumTieNum,0,0,0,0);
	}
	else {
		stRaiseEquipNum raiseEquipNumNotify;
		raiseEquipNumNotify.notify(pUser,5);
		pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_EQUIP_TO_LV);
		pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),oldRaiseLv,pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_refineLv,pItem->m_itemInfo.m_refineLv,"一键强化装备",needMoney,decYumTieNum,0,0,0,0);

		Global::stReqCheckTask cmd;
		cmd.set_charid(pUser->getID());
		cmd.set_squaretype(6);
		cmd.set_battletype(2);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}
}

struct stRefineEquipCB : public stItemCallBack
{
	UINT32 m_itemID;
	UINT32 m_instanceID;
	UINT32 m_num;
	std::set<CItem *> m_vec;

	stRefineEquipCB(UINT32 itemID,UINT64 instanceID) : m_itemID(itemID) ,m_instanceID(instanceID)
	{
		m_num = 0;
	}

	virtual  bool doIt(CItem * pItem)
	{
		if (!pItem){
			return true;
		}

		if (pItem->m_itemInfo.m_exp > 0){
			return true;
		}

		if (pItem->m_itemInfo.m_raiseLv > 0){
			return true;
		}

		if (pItem->m_itemInfo.m_refineLv){
			return true;
		}

		for (UINT8 i = 0;i < MAX_EQUIP_SLOT_NUM; ++i){
			if (pItem->m_itemInfo.m_slot[i].m_skillID > 0){
				return true;
			}
		}

		if ((pItem->m_itemInfo.m_itemID == m_itemID) && (m_instanceID != pItem->m_itemInfo.m_instanceID)){
			m_vec.insert(pItem);
			m_num += pItem->m_itemInfo.m_num;
		}
		return true;
	}
};

void RoleModHandle::reqRefineItem(GameUser *pUser,Role::stReqRefineEquip * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRefineEquipResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		retCmd.set_result(Role::stRefineEquipResult::eEQUIP_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[精炼装备](name=%s,id=%d)精炼失败,找不到要精炼的道具!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	stRefineBase * pRefineBase = g_refineBaseMgr.get(stRefineBase::getFindID(pItem->m_itemInfo.m_itemID,pItem->m_itemInfo.m_refineLv));
	stRefineBase * pNextRefineBase = g_refineBaseMgr.get(stRefineBase::getFindID(pItem->m_itemInfo.m_itemID,pItem->m_itemInfo.m_refineLv + 1));
	if (!pRefineBase || !pNextRefineBase){
		retCmd.set_result(Role::stRefineEquipResult::eINNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[精炼装备](name=%s,id=%d)精炼失败,找不到精炼配置!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pNextRefineBase){
		retCmd.set_result(Role::stRefineEquipResult::eFULL_LV);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[精炼装备](name=%s,id=%d)精炼失败,找不到下一级的信息,已经精炼到最大等级了!",pUser->getName(),pUser->getID());
#endif
		return ;

	}

	for (UINT16 i = 0;i < pRefineBase->m_costResVec.size();++i){
		if (!pUser->checkResource((eResource)pRefineBase->m_costResVec[i].first,pRefineBase->m_costResVec[i].second)){
			retCmd.set_result(Role::stRefineEquipResult::eGOLDCOIN_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[精炼装备](name=%s,id=%d)精炼失败,金币不足!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}

	//std::vector<CItem *> delVec;
	stRefineEquipCB cb(pItem->getID(),pItem->m_itemInfo.m_instanceID);
	for (UINT16 i = 0;i < pRefineBase->m_costItemVec.size(); ++i){
		cb.m_itemID = pRefineBase->m_costItemVec[i].first;
		cb.m_num = 0;
		pUser->m_bagMgr.getItemByCond(cb,Role::BAG_TYPE_MAIN1);
		if (cb.m_num < pRefineBase->m_costItemVec[i].second){//数量不足
			retCmd.set_result(Role::stRefineEquipResult::eITEM_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[精炼装备](name=%s,id=%d)精炼失败,需要道具不足!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}

	UINT32 decGoldCoin = 0;
	UINT32 decItemID = 0;
	for (UINT16 i = 0;i < pRefineBase->m_costResVec.size();++i){
		decGoldCoin = pRefineBase->m_costResVec[i].second;
		if (!pUser->removeResource((eResource)pRefineBase->m_costResVec[i].first,pRefineBase->m_costResVec[i].second,"精炼")){
			retCmd.set_result(Role::stRefineEquipResult::eGOLDCOIN_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
			Athena::logger->error("[精炼装备](name=%s,id=%d)精炼失败,扣除金币失败!",pUser->getName(),pUser->getID());
			return ;
		}
	}

	for (UINT16 i = 0;i < pRefineBase->m_costItemVec.size(); ++i){
		UINT32 delItemID = pRefineBase->m_costItemVec[i].first;
		UINT32 delNum = pRefineBase->m_costItemVec[i].second;
		decItemID = delItemID;
		std::set<CItem *>::iterator it = cb.m_vec.begin();
		for (;it != cb.m_vec.end();){
			if (delItemID == (*it)->getID()){
				if (delNum >= (*it)->m_itemInfo.m_num){
					delNum -= (*it)->m_itemInfo.m_num;
					std::set<CItem *>::iterator tmpIt = it;
					++it;
					pUser->m_bagMgr.decItem(*tmpIt,(*tmpIt)->m_itemInfo.m_num,"装备精炼");
					cb.m_vec.erase(tmpIt);
				}
				else {
					pUser->m_bagMgr.decItem(*it,delNum,"装备精炼");
					delNum = 0;
					++it;
				}

				if (0 == delNum){
					break;
				}
			}
			else {
				++it;
			}
		}
	}

	UINT16 oldRefineLv = pItem->m_itemInfo.m_refineLv;
	pItem->m_itemInfo.m_refineLv += 1;
	pItem->setupPropState();
	pUser->m_bagMgr.refreshItem(pItem);
	retCmd.set_result(Role::stRefineEquipResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);

	if (CEquipBag::isAffectRoleData(pItem->m_itemInfo.m_pos)){
		pUser->setupRoleState();
		//pUser->sendMainData2Me();
		pUser->sendBattleProp();
	}

	pUser->m_taskMgr.onPropChange(TaskProto::ET_REFINE_EQUIP_TO_LV);
	pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_FUWEN_NUM);

	pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_raiseLv,oldRefineLv,pItem->m_itemInfo.m_refineLv,"精炼装备",decGoldCoin,0,decItemID,0,0,0);
}

void RoleModHandle::reqSmeltItem(GameUser *pUser,Role::stReqSmeltEquip * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stSmeltEquipResult retCmd;
	UINT32 heiShiJieJing = 0;
	UINT32 goldCoin = 0;
	UINT32 yunTie = 0;
	std::vector<UINT32> itemIDs;
	std::map<UINT16,UINT8> itemsType;//key:item品阶, value:计数
	std::map<UINT16,UINT8>::iterator mapIt;

	for (UINT16 i = 0;i < recvCmd->instanceid_size();++i){
		CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid(i));
		if (!pItem){
			retCmd.set_result(Role::stSmeltEquipResult::EQUIP_NOT_EXIST);
			pUser->sendProto2Me(retCmd);
			Athena::logger->error("[熔炼道具](name=%s,id=%d)熔炼失败,找不到要熔炼的道具!",pUser->getName(),pUser->getID());
			return ;
		}

		stItemLevelupBase * pLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,0));
		if (!pLvUpBase){
			retCmd.set_result(Role::stSmeltEquipResult::INNER_ERROR);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("熔炼失败,强化表没有找到相关配置文件");
#endif
			return ;
		}


		if (!CItem::isEquipByType(pItem->m_pItemBase->m_type)){
			retCmd.set_result(Role::stSmeltEquipResult::CANNT_SMELT);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[熔炼道具](name=%s,id=%d)熔炼道具失败,放进去的不是装备",pUser->getName(),pUser->getID());
#endif
			return;
		}

		if (pItem->m_itemInfo.m_raiseLv != 0){//不是0级的装备不可以熔炼
			retCmd.set_result(Role::stSmeltEquipResult::CANNT_SMELT);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[熔炼道具](name=%s,id=%d)熔炼道具失败,放进去的装备等级不为0",pUser->getName(),pUser->getID());
#endif
			return ;
		}

		if (!pUser->m_bagMgr.decItem(pItem,pItem->m_itemInfo.m_num,"熔炼道具")){
			Athena::logger->error("[熔炼道具](name=%s,id=%d)熔炼道具失败,扣除熔炼的道具不成功!",pUser->getName(),pUser->getID());
			continue;
		}

		if(Athena::global["isLYServer"] == "true"){
			stItemBase * pItemBase = g_itemBaseMgr.get(pItem->m_itemInfo.m_itemID);
			if(pItemBase){
				UINT16 type = pItemBase->m_classType;

				mapIt = itemsType.find(type);

				if(mapIt != itemsType.end()){
					++mapIt->second;
				}else{
					itemsType.insert(std::make_pair(type, 1));
				}

			}
		}


		heiShiJieJing += pItem->m_pItemBase->m_smeltVal;
		goldCoin += pLvUpBase->m_returnGoldCoin;
		yunTie += pLvUpBase->m_exp;
	}

	pUser->addResource(eResource_Heishijiejing,heiShiJieJing,"熔炼");
	pUser->addResource(eResource_GoldCoin,goldCoin,"熔炼");
	pUser->addResource(eResource_Yuntie,yunTie,"熔炼");
	retCmd.set_result(Role::stSmeltEquipResult::SUCCESS);
	pUser->sendProto2Me(retCmd);

	Role::stObtainItemTip itemTipCmd;
	Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
	if (pSyn){
		pSyn->set_itemid(eResource_Heishijiejing);
		pSyn->set_num(heiShiJieJing);
	}
/*
	pSyn = itemTipCmd.add_getitems();
	if (pSyn){
		pSyn->set_itemid(eResource_GoldCoin);
		pSyn->set_num(goldCoin);
	}
*/

	pSyn = itemTipCmd.add_getitems();
	if (pSyn){
		pSyn->set_itemid(eResource_Yuntie);
		pSyn->set_num(yunTie);
	}

	if(Athena::global["isLYServer"] == "true"){
		mapIt = itemsType.begin();
		for(; mapIt != itemsType.end(); ++mapIt){
			UINT16 id = CSmeltItemCfg::getSingleton().getGroupIdByOrderAndNum(mapIt->first, mapIt->second);
			if(id != (UINT16)-1){
				std::vector<UINT32> vec;
				CSmeltItemCfg::getSingleton().getGroupByGroupID(id,vec);
				std::vector<UINT32>::iterator vecIt = vec.begin();
				for(;vecIt != vec.end(); ++vecIt){
					pUser->giveAssert(*vecIt,1,0,"熔炼");
					pSyn = itemTipCmd.add_getitems();
					if(pSyn){
						pSyn->set_itemid(*vecIt);
						pSyn->set_num(1);
					}
				}
			}
		}
	} else {
        if (goldCoin) {
            pSyn = itemTipCmd.add_getitems();
            if (pSyn){
                pSyn->set_itemid(eResource_GoldCoin);
                pSyn->set_num(goldCoin);
            }
        }
    }

	pUser->sendProto2Me(itemTipCmd);
}

UINT32 CalItemIDFromRealID(UINT32 itemID, UINT32 job, UINT32 sex)
{
	if (job == Role::eJOB_WARRIOR){
		if (sex == Role::eSEX_MAN){
			return itemID;	
		}
		else if (sex == Role::eSEX_FEMALE){
			return itemID - 1;
		}
	}
	else if (job == Role::eJOB_BOWMAN){
		if (sex == Role::eSEX_MAN){
			return itemID - 2;
		}
		else if (sex == Role::eSEX_FEMALE){
			return itemID - 3;
		}
	}
	else if (job == Role::eJOB_MASTER){
		if (sex == Role::eSEX_MAN){
			return itemID - 4;
		}
		else if (sex == Role::eSEX_FEMALE){
			return itemID - 5;
		}
	}
	return itemID;
}

void RoleModHandle::reqRecastItem(GameUser * pUser,Role::stReqRecastEquip * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::RecastEquipResult retCmd;

	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < recvCmd->instanceid_size()){
		retCmd.set_result(Role::RecastEquipResult::BAG_FULL);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[重铸道具](name=%s,id=%d)包裹空间不足!",pUser->getName(),pUser->getID());
#endif
		return ;

	}

	UINT32 returnGoldCoin = 0;
	UINT32 returnStone = 0;
	UINT32 returnJuexingExp = 0;
	std::map<UINT32,UINT32> retAsserts;

	Role::stObtainItemTip itemTipCmd;

	UINT32 needDiamond = 0;
	stRecastDiamond recastCost = CMiscCfgMgr::getSingleton().getRecastCost();

	for (UINT16 i = 0;i< recvCmd->instanceid_size();++i){
		CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid(i));
		if (!pItem){
			retCmd.set_result(Role::RecastEquipResult::EQUIP_NOT_EXIST);
			pUser->sendProto2Me(retCmd);
			Athena::logger->error("[重铸道具](name=%s,id=%d)重铸失败,找不到要重铸的道具!",pUser->getName(),pUser->getID());
			return ;
		}
		UINT8 quality = pItem->m_pItemBase->m_quality;
		if (pItem->isN(quality))
		{
			needDiamond += recastCost.m_n;
		}
		else if (pItem->isR(quality))
		{
			needDiamond += recastCost.m_r; 
		}
		else if (pItem->isSR(quality))
		{
			needDiamond += recastCost.m_sr; 
		}
		else if (pItem->isSSR(quality))
		{
			needDiamond += recastCost.m_ssr; 
		}
		else if (pItem->isUR(quality))
		{
			needDiamond += recastCost.m_ur; 
		}
		else
		{
			needDiamond += 40;
		}
		

		stItemLevelupBase * pLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,pItem->m_itemInfo.m_raiseLv));
		if (!pLvUpBase){
			retCmd.set_result(Role::RecastEquipResult::INNER_ERROR);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[重铸道具]失败,强化表没有找到相关配置文件");
#endif
			return ;
		}

		stRefineBase * pRefineBase = g_refineBaseMgr.get(stRefineBase::getFindID(pItem->getID(),pItem->m_itemInfo.m_refineLv));
		stItemBase * pItemBase = g_itemBaseMgr.get(pItem->m_itemInfo.m_itemID);
		if(pItemBase){
	    	UINT16 type = pItemBase->m_classType;
			if(type != 1 && type!= 2){//既不是绿装也不是蓝装
				if (!pRefineBase){
					retCmd.set_result(Role::RecastEquipResult::INNER_ERROR);
					pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
					Athena::logger->error("[重铸道具]失败,精炼表没有找到相关配置文件");
#endif
					return ;
				}
			}
		}

		if (!CItem::isEquipByType(pItem->m_pItemBase->m_type)){
			retCmd.set_result(Role::RecastEquipResult::CANNT_SMELT);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[重铸道具](name=%s,id=%d)重铸道具失败,放进去的不是装备",pUser->getName(),pUser->getID());
#endif
			return;
		}

		returnGoldCoin += pLvUpBase->m_returnGoldCoin;
		returnStone += pLvUpBase->m_returnStone;

		for (UINT8 i = 0;i < MAX_EQUIP_SLOT_NUM;++i){
			SkillGrownBase * pSkillGrownBase =  g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(pItem->m_itemInfo.m_slot[i].m_skillID,pItem->m_itemInfo.m_slot[i].m_skillLv));
			if (pSkillGrownBase){
				returnJuexingExp += pSkillGrownBase->m_returnExp;
			}
		}

		if (Athena::global["nftServer"] != "true")
		{
			Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
			if (pSyn){
				pSyn->set_itemid(pItem->getID());
				pSyn->set_num(1);
			}
		}

		if(pRefineBase){
			for (UINT8 i = 0;i < pRefineBase->m_returnAssert.size();++i){
				//pUser->giveAssert(pRefineBase->m_returnAssert[i].first,pRefineBase->m_returnAssert[i].second,0,"重铸返还");
				retAsserts[pRefineBase->m_returnAssert[i].first] += pRefineBase->m_returnAssert[i].second;
				Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
				if (pSyn){
					pSyn->set_itemid(pRefineBase->m_returnAssert[i].first);
					pSyn->set_num(pRefineBase->m_returnAssert[i].second);
				}
			}
		}

		bool isOrange = true;
		if(pItem->m_pItemBase->m_type != ITEM_TYPE_WING)//不是翅膀
		{
			UINT8 quality = pItem->m_pItemBase->m_quality;
			if(quality < 22)
			{
				isOrange = false;
			}
		}

		if(isOrange){
			stBreakBase * pBreakBase = g_breakBaseMgr.get(stBreakBase::getFindID(pItem->getID(),pItem->m_itemInfo.m_breakLv));
			if(!pBreakBase){
				retCmd.set_result(Role::RecastEquipResult::INNER_ERROR);
				pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[重铸道具]失败,突破表没有找到相关配置文件");
#endif
				return;
			}
			for(UINT8 i = 0; i < pBreakBase->m_returnAssert.size(); ++i){
				retAsserts[pBreakBase->m_returnAssert[i].first] += pBreakBase->m_returnAssert[i].second;
				Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
				if(pSyn){
					pSyn->set_itemid(pBreakBase->m_returnAssert[i].first);
					pSyn->set_num(pBreakBase->m_returnAssert[i].second);
				}
			}
		}

	}

	if (!pUser->removeResource(eResource_Diamond,needDiamond,"重铸")){
		retCmd.set_result(Role::RecastEquipResult::DIAMOND_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[重铸道具](name=%s,id=%d)钻石不足!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	bool bNeedCalcRole = false;
	for (UINT16 i = 0;i< recvCmd->instanceid_size();++i){
		CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid(i));
		if (pItem){
			if (Athena::global["nftServer"] == "true")
			{
				RecastEquipCfg::stItem * st = RecastEquipCfg::getSingleton().getRecastItem(pItem->m_itemInfo.m_itemID);
				if(st){
					if (!pUser->m_bagMgr.decItem(pItem, pItem->m_itemInfo.m_num,"重铸")){
						Athena::logger->error("[重铸道具](name=%s,id=%d)重铸道具失败,扣除重铸的道具不成功!",pUser->getName(),pUser->getID());
						continue;
					}
					Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
					if(pSyn){
						pSyn->set_itemid(st->m_itemID);
						pSyn->set_num(st->m_num);
					}
					//返还碎片,要根据性别职业,算出itemid
					UINT32 itemID = CalItemIDFromRealID(st->m_itemID,pUser->getJob(),pUser->getSex());
					retAsserts[itemID] += st->m_num;
				}else{
					pItem->m_itemInfo.m_raiseLv = 0;
					pItem->m_itemInfo.m_refineLv = 0;
					pItem->m_itemInfo.m_exp = 0;
					pItem->m_itemInfo.m_breakLv = 0;
					bzero(pItem->m_itemInfo.m_slot,sizeof(pItem->m_itemInfo.m_slot));
					pItem->setupPropState();
					pUser->m_bagMgr.refreshItem(pItem);

					if (CEquipBag::isAffectRoleData(pItem->m_itemInfo.m_pos)){
						bNeedCalcRole = true;
					}
					Athena::logger->trace("[重铸道具](name=%s,id=%d)(itemId=%d)属性重置!",pUser->getName(),pUser->getID(), pItem->m_itemInfo.m_itemID);
				}
			}
			else
			{
				pItem->m_itemInfo.m_raiseLv = 0;
				pItem->m_itemInfo.m_refineLv = 0;
				pItem->m_itemInfo.m_exp = 0;
				pItem->m_itemInfo.m_breakLv = 0;
				bzero(pItem->m_itemInfo.m_slot,sizeof(pItem->m_itemInfo.m_slot));
				pItem->setupPropState();
				pUser->m_bagMgr.refreshItem(pItem);

				if (CEquipBag::isAffectRoleData(pItem->m_itemInfo.m_pos)){
					bNeedCalcRole = true;
				}
				Athena::logger->trace("[重铸道具](name=%s,id=%d)(itemId=%d)属性重置!",pUser->getName(),pUser->getID(), pItem->m_itemInfo.m_itemID);
			}
		}
	}

	std::map<UINT32,UINT32>::iterator it = retAsserts.begin();
	for (;it != retAsserts.end();++it){
		pUser->giveAssert(it->first,it->second,0,"重铸返还");
	}

	if (bNeedCalcRole){
		pUser->setupRoleState();
		pUser->sendBattleProp();
	}

	pUser->addResource(eResource_Juexing_Exp,returnJuexingExp,"重铸");
	pUser->addResource(eResource_GoldCoin,returnGoldCoin,"重铸");
	pUser->addResource(eResource_Yuntie,returnStone,"重铸");

	retCmd.set_result(Role::RecastEquipResult::SUCCESS);
	pUser->sendProto2Me(retCmd);

	Role::stSynTipItem * pSyn = NULL;
	if (returnGoldCoin > 0){
		pSyn = itemTipCmd.add_getitems();
		if (pSyn){
			pSyn->set_itemid(eResource_GoldCoin);
			pSyn->set_num(returnGoldCoin);
		}
	}

	if (returnStone > 0){
		pSyn = itemTipCmd.add_getitems();
		if (pSyn){
			pSyn->set_itemid(eResource_Yuntie);
			pSyn->set_num(returnStone);
		}
	}

	if (returnJuexingExp > 0){
		pSyn = itemTipCmd.add_getitems();
		if (pSyn){
			pSyn->set_itemid(eResource_Juexing_Exp);
			pSyn->set_num(returnJuexingExp);
		}
	}
	pUser->sendProto2Me(itemTipCmd);
}

void RoleModHandle::reqCombineItem(GameUser *pUser,Role::stReqCombineItem * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stCombineItemResult retCmd;

	ItemCombineBase * pCombineBase = g_combineBaseMgr.get(recvCmd->pieceid());
	if (!pCombineBase){
		retCmd.set_result(Role::stCombineItemResult::FAILURE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[道具合成]找不到配置表格信息,合成失败!");
#endif
		return ;
	}

	if (COMBINE_TYPE_ITEM != pCombineBase->m_combineType){
		retCmd.set_result(Role::stCombineItemResult::FAILURE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[道具合成]合成类型不正确,合成失败!");
#endif
		return ;
	}

	UINT8 bagType = Role::BAG_TYPE_MAIN1;
	if (CItem::isFashion(pCombineBase->m_combineItemID)){//如果是时装
		bagType = Role::BAG_TYPE_FASHION;
	}

	if (pUser->m_bagMgr.getFreeSlotNum(bagType) <= 0){
		retCmd.set_result(Role::stCombineItemResult::BAG_FULL);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[道具合成](name=%s,id=%d)包裹空间不足!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->m_bagMgr.removeItemByItemID(recvCmd->pieceid(),pCombineBase->m_pieceNum * recvCmd->combinenum(),"合成装备")){
		retCmd.set_result(Role::stCombineItemResult::NOT_ENOUGH_PIECE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[道具合成](name=%s,id=%d)碎片数量不足不足!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	CItem * pCombineItem = CBagMgr::createItem(pCombineBase->m_combineItemID,recvCmd->combinenum(),0);
	if (!pCombineItem){
		Athena::logger->error("[道具合成](name=%s,id=%d)道具合成时创建道具失败",pUser->getName(),pUser->getID());
		return ;
	}

	pCombineItem->m_itemInfo.m_pos.m_pos = pUser->m_bagMgr.getBagFreeIndex(pCombineItem->m_itemInfo.m_pos.m_bagType);

	if (!pUser->m_bagMgr.addItem2Bag(pCombineItem)){
		Athena::logger->error("[道具合成](name=%s,id=%d)道具合成时添加到包裹失败!",pUser->getName(),pUser->getID());
		SAFE_DELETE(pCombineItem);
		return ;
	}
	retCmd.set_result(Role::stCombineItemResult::SUCCESS);
	pUser->sendProto2Me(retCmd);

	pUser->m_bagMgr.sendEquipChgLog(pCombineBase->m_combineItemID,0,0,0,0,"装备合成",0,0,recvCmd->pieceid(),0,0,pCombineBase->m_combineItemID);
}

void RoleModHandle::reqShopList(GameUser * pUser,Role::stReqOpenShop * recvCmd)
{
	if (!pUser || !recvCmd)	{
		return ;
	}

	pUser->m_shopMgr.sendShopList2Me(recvCmd->shopid());
}

void RoleModHandle::reqBuyItem(GameUser * pUser,Role::stReqBuyItem * recvCmd)
{
	if (!pUser || !recvCmd)	{
		return ;
	}

	if(recvCmd->isfashion()){
		Athena::logger->trace("[物品购买]时装商店");
		pUser->m_shopMgr.buyItem(recvCmd->shopid(),recvCmd->index() - 1 ,recvCmd->num(),recvCmd->isfashion());
	}
	else{
		Athena::logger->trace("[物品购买]普通商店");
		pUser->m_shopMgr.buyItem(recvCmd->shopid(),recvCmd->index(),recvCmd->num());
	}
}

void RoleModHandle::reqRefreshShoplist(GameUser * pUser,Role::stReqRefreshShoplist * recvCmd)
{
	if (!pUser || !recvCmd)	{
		return ;
	}
	pUser->m_shopMgr.reqRefreshShopList(recvCmd->shopid());
}

void RoleModHandle::reqFinishLv(GameUser * pUser,Role::stReqFinishLv * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	pUser->m_levelMgr.finishLv(recvCmd);
}

void RoleModHandle::reqCharpterReward(GameUser * pUser,Role::stReqCharpterReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_levelMgr.reqCharpterReward(recvCmd->charpterid(),recvCmd->step());
}

void RoleModHandle::reqRush(GameUser * pUser,Role::stReqRushLevel * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	if (Role::eRush_Type_1 == recvCmd->type()){
		pUser->m_levelMgr.rushLv(recvCmd->levelid(),1,recvCmd->type());
	}
	else if (Role::eRush_Type_2 == recvCmd->type()){
		pUser->m_levelMgr.rushLv(recvCmd->levelid(),recvCmd->num(),recvCmd->type());
	}
}

void RoleModHandle::reqResetRushNum(GameUser * pUser,Role::stReqResetRushNum * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_levelMgr.resetRushNum(recvCmd->levelid());
}

void RoleModHandle::reqUseItem(GameUser * pUser,Role::stUseItem * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stUseItemError errorCmd;
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem || !pItem->m_pItemBase){
		errorCmd.set_result(Role::stUseItemError::eItem_Not_Exist);
		pUser->sendProto2Me(errorCmd);
		return ;
	}
    errorCmd.set_itemid(pItem->m_itemInfo.m_itemID);

	bool useSuccess = false;
	UINT32 useNum = 1;

	if (ITEM_TYPE_BOX1 == pItem->m_pItemBase->m_type
			|| ITEM_TYPE_BOX2 == pItem->m_pItemBase->m_type
			|| ITEM_TYPE_GIFTBOX == pItem->m_pItemBase->m_type
			|| ITEM_TYPE_BOX3 == pItem->m_pItemBase->m_type){//宝箱类
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}
		if (recvCmd->batchuse()){
			useNum = pItem->m_itemInfo.m_num > 5 ? 5 : pItem->m_itemInfo.m_num;
		}

		if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < (pUser->m_boxMgr.needSlotNum(pItem->m_pItemBase->m_boxID) * useNum)){//包裹空格不足
			errorCmd.set_result(Role::stUseItemError::eBag_Slot_Not_Enough);
			pUser->sendProto2Me(errorCmd);
			return ;
		}

		if (pItem->m_pItemBase->m_mediaID) {
			UINT32 mediaNum = pUser->m_bagMgr.getItemNumByID(pItem->m_pItemBase->m_mediaID);
			if (useNum > mediaNum){//需要消耗物不足
				useNum = mediaNum;
			}

			if (useNum <= 0){
				errorCmd.set_result(Role::stUseItemError::eBag_Need_Media_Not_Exist);
				pUser->sendProto2Me(errorCmd);
				return ;
			}

			if (!pUser->m_bagMgr.removeItemByItemID(pItem->m_pItemBase->m_mediaID,useNum,"开箱子")){
				errorCmd.set_result(Role::stUseItemError::eBag_Need_Media_Not_Exist);
				pUser->sendProto2Me(errorCmd);
				return ;
			}
		}

		pUser->m_boxMgr.openBox(pItem->m_pItemBase->m_boxID,useNum,true,pItem->getID());
		useSuccess = true;
	}
	else if (ITEM_TYPE_TILI == pItem->m_pItemBase->m_type){//体力丹
		pUser->addResource(eResource_Sp,pItem->m_pItemBase->m_effect,"使用体力丹");
		useSuccess = true;
	}
	else if (ITEM_TYPE_TRAINTILI == pItem->m_pItemBase->m_type){//调教体力丹
		pUser->addResource(eResource_TrainSp,pItem->m_pItemBase->m_effect,"使用调教体力丹");
		useSuccess = true;
	}
	else if (ITEM_TYPE_LILIAN == pItem->m_pItemBase->m_type){//历练丹
		pUser->addResource(eResource_Lilian,pItem->m_pItemBase->m_effect,"使用历练丹");
		useSuccess = true;
	}
	else if (ITEM_TYPE_TITLE == pItem->m_pItemBase->m_type){
		pUser->m_titleMgr.addTitle(pItem->m_pItemBase->m_effect);
		useSuccess = true;
	}
	else if (ITEM_TYPE_GUAJI == pItem->m_pItemBase->m_type){//挂机时间道具
		stGuajiLimit limit = CMiscCfgMgr::getSingleton().getGuajiLimit();
		if (pUser->m_roleInfo.m_guajiEnd - pUser->m_roleInfo.m_guajiStart >= limit.m_maxtime)
		{
			Athena::logger->error("使用道具失败,挂机时间已到上限!");
			errorCmd.set_result(Role::stUseItemError::eUse_Item_Invalid);
        	pUser->sendProto2Me(errorCmd);
			return;
		}
		if (pUser->m_roleInfo.m_guajiStart == 0)
		{
			pUser->m_roleInfo.m_guajiStart = g_gameServer->getSec();
			pUser->m_roleInfo.m_guajiEnd = pUser->m_roleInfo.m_guajiStart + pItem->m_pItemBase->m_effect;
		}else{
			pUser->m_roleInfo.m_guajiEnd +=  pItem->m_pItemBase->m_effect;
			if (pUser->m_roleInfo.m_guajiEnd - pUser->m_roleInfo.m_guajiStart > limit.m_maxtime){
				pUser->m_roleInfo.m_guajiEnd = pUser->m_roleInfo.m_guajiStart + limit.m_maxtime;
			}
		}
		errorCmd.set_result(Role::stUseItemError::eSuccess);
        pUser->sendProto2Me(errorCmd);
		pUser->sendGuaji();
		useSuccess = true;
	}
	else if (ITEM_TYPE_EXP_BOOK == pItem->m_pItemBase->m_type ||
            ITEM_TYPE_RATE_BOOK == pItem->m_pItemBase->m_type){
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}

		if (recvCmd->batchuse()){
			useNum = pItem->m_itemInfo.m_num > 5 ? 5 : pItem->m_itemInfo.m_num;
		}

		CMiscCfgMgr::stExpLevelLimit* info = CMiscCfgMgr::getSingleton().getLimitLevel(pItem->getID());

		if(!info){
#ifdef _HDZ_DEBUG
            Athena::logger->error("[使用经验书](name:%s, id:%d)经验书使用失败,道具不存在！",pUser->getName(),pUser->getID());
#endif
			return ;
		}

#ifdef _HDZ_DEBUG
		Athena::logger->trace("[使用经验书](name:%s, id:%d, level = %d)经验书id:%d, 限制最低等级：%d， 限制最高等级：%d",pUser->getName(), pUser->getID(),pUser->getLevel(),pItem->getID(), info->m_minLevel, info->m_maxLevel);
#endif

		if(pUser->getLevel() < info->m_minLevel){
#ifdef _HDZ_DEBUG
			Athena::logger->error("[使用经验书](name:%s, id:%d, level:%d)经验书使用失败，等级过低：需要最低等级:%d",pUser->getName(),pUser->getID(),pUser->getLevel(),info->m_minLevel);
#endif
			return;
		}

		if(pUser->getLevel() > info->m_maxLevel){
#ifdef _HDZ_DEBUG
			Athena::logger->error("[使用经验书](name:%s, id:%d, level:%d)经验书使用失败，等级过高：允许的最高等级:%d",pUser->getName(),pUser->getID(),pUser->getLevel(),info->m_maxLevel);
#endif
			return;
		}
        if (ITEM_TYPE_RATE_BOOK == pItem->m_pItemBase->m_type) {

            pUser->addExp(pItem->m_pItemBase->m_effect * useNum * pUser->getLevel());
        } else {
            pUser->addExp(pItem->m_pItemBase->m_effect * useNum);
        }
		useSuccess = true;
	}
	else if (ITEM_TYPE_GIVE_VIPEXP == pItem->m_pItemBase->m_type){
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}

		if (recvCmd->batchuse()){
			useNum = pItem->m_itemInfo.m_num > 5 ? 5 : pItem->m_itemInfo.m_num;
		}

		pUser->addGiveVipExp(pItem->m_pItemBase->m_effect * useNum);
		useSuccess = true;
	}
	else if (ITEM_TYPE_VIPEXP == pItem->m_pItemBase->m_type){
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}

		if (recvCmd->batchuse()){
			useNum = pItem->m_itemInfo.m_num > 5 ? 5 : pItem->m_itemInfo.m_num;
		}

		pUser->addVipExp(pItem->m_pItemBase->m_effect * useNum);
		useSuccess = true;
	}
    else if (ITEM_TYPE_BOX4 <= pItem->m_pItemBase->m_type && pItem->m_pItemBase->m_type < ITEM_TYPE_BOX9) {
        //累计消耗钻石(周消耗活动)
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}
		if (recvCmd->batchuse()){
			useNum = pItem->m_itemInfo.m_num > 5 ? 5 : pItem->m_itemInfo.m_num;
		}
        MiscProto::eActivityType type = MiscProto::eActivityType_Invalid;
        switch (pItem->m_pItemBase->m_type) {
            case ITEM_TYPE_BOX4:
                {
                    type = MiscProto::eActivityType_WeeklyConsume;
                    break;
                }
            case ITEM_TYPE_BOX5:
                {
                    type = MiscProto::eActivityType_AccRechargeDay;
                    break;
                }
            default :
                {
                    Athena::logger->error("使用道具失败,无效的道具类型 %d", pItem->m_pItemBase->m_type);
                    return ;
                }
        }
        if (pUser->m_activityDataMgr.addActivityVal(pItem->m_pItemBase->m_effect * useNum, type)) {
            useSuccess = true;
        }
    }
    else if (ITEM_TYPE_BOX9 == pItem->m_pItemBase->m_type) {
        //成长计划 福利卡 月卡 年卡等
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}
		if (recvCmd->batchuse()){
			useNum = pItem->m_itemInfo.m_num > 5 ? 5 : pItem->m_itemInfo.m_num;
		}
        for (UINT32 i = 0; i < useNum; i++) {
            UINT32 money = CRMBShop::getItemMoney(pItem->m_pItemBase->m_effect);
            if (!money) {
                Athena::logger->error("使用道具失败,找不到对应的money,id is %d!", pItem->m_pItemBase->m_effect);
                return ;
            }
            std::ostringstream os ;
            os << pUser->getAccount() << "testid" << i << g_gameServer->getSec();
            std::string tid = os.str();
            pUser->m_rmbShop.buy(money, pItem->m_pItemBase->m_effect, tid);
            useSuccess = true;
        }
    }
    else if (ITEM_TYPE_FORCE_FINISH_TASK == pItem->m_pItemBase->m_type) {
        //跳关卡任务
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}
        pUser->m_taskMgr.forceFinish(pItem->m_pItemBase->m_effect, true);
        useSuccess = true;
    }
    else if (ITEM_TYPE_REMOVE_ALL_ITEM == pItem->m_pItemBase->m_type) {
        //一键卸除灵魂祭坛装备
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}
        useSuccess = true;
    }
    else if (ITEM_TYPE_FIRE_WORK == pItem->m_pItemBase->m_type) {
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}
        errorCmd.set_result(Role::stUseItemError::eSuccess);
        pUser->sendProto2Me(errorCmd);
        useSuccess = true;
    }
    else if (ITEM_TYPE_BOX6 == pItem->m_pItemBase->m_type) {
		if ((recvCmd->batchuse()) && (!CItem::canBatchUse(pItem->m_pItemBase->m_type))){
			Athena::logger->error("使用道具失败,改道具不可以批量使用!");
			return ;
		}
        Global::stUseLvlBox cmd;
        cmd.set_charid(pUser->getID());
        cmd.set_bagtype(recvCmd->bagtype());
        cmd.set_instanceid(recvCmd->instanceid());
        GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
        return ;
    }
	else {
		return ;
	}

	pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (useSuccess && pItem){
		pUser->m_bagMgr.decItem(pItem,useNum,"道具使用");
	}
}

void RoleModHandle::getEquipSkill(GameUser * pUser,Role::stReqGetEquipSkill * recvCmd)
{
	if (!pUser || !recvCmd)	{
		return ;
	}

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		Athena::logger->error("[装备觉醒](name=%s,id=%d)觉醒失败,找不到要觉醒的装备!",pUser->getName(),pUser->getID());
		return ;
	}

	Role::stGetEquipSkillResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());

    if (pUser->getLevel() < CMiscCfgMgr::getSingleton().getOperaLvlLimit(CMiscCfgMgr::Limit_Equip_Awake)) {
		Athena::logger->error("[装备觉醒](name=%s,id=%d)觉醒失败,人物等级不够!",pUser->getName(),pUser->getID());
		return ;
    }


	if (pItem->m_itemInfo.m_hasGotSkill){
		retCmd.set_result(Role::stGetEquipSkillResult::HAS_GOT_SKILL);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[装备觉醒](name=%s,id=%d)觉醒失败,觉醒位置已经有技能了!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->m_bagMgr.removeItemByItemID(JUEXING_ITEM_ID,0,1,"装备觉醒")){
		UINT16 jueXingNum = pUser->m_countMgr.getDayRefreshValue(ECOUNT_EQUIP_JUEXING);
		if (pUser->m_bagMgr.getItemNumByID(JUEXING_ITEM_ID) && (jueXingNum >= pUser->getCountByVip(eVipFun_Equip_Juexing))){
			retCmd.set_result(Role::stGetEquipSkillResult::TIME_USE_OUT);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[装备觉醒](name=%s,id=%d)觉醒失败,觉醒次数用完了!",pUser->getName(),pUser->getID());
#endif
			return ;
		}

		UINT32 needDiamond = CMiscCfgMgr::getSingleton().getDiamondByJueXing(jueXingNum);
		if ((needDiamond == (UINT32)-1) || !pUser->removeResource(eResource_Diamond,needDiamond,"需要钻石")){
			retCmd.set_result(Role::stGetEquipSkillResult::NOT_ENOUGH_DIAMOND);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[装备觉醒](name=%s,id=%d)觉醒失败,觉醒位置已经有技能了!",pUser->getName(),pUser->getID());
#endif
			return ;
		}

		pUser->m_countMgr.addDayRefreshValue(ECOUNT_EQUIP_JUEXING,1);
	}

	stSkillundBase * pSkillundBase = g_heroSkillundBaseMgr.get(1);//约定是1
	if (!pSkillundBase){
#ifdef _HDZ_DEBUG
		Athena::logger->error("[装备觉醒](name=%s,id=%d)找不到配置!",pUser->getName(),pUser->getID());
#endif
		return ;

	}

	UINT32 maxVal = 0;
	std::map<UINT32,UINT32> skillOdds;
	for (UINT16 i = 0;i < pSkillundBase->m_randomSkillVec.size();++i){
		UINT32 skillID = pSkillundBase->m_randomSkillVec[i].first;
		UINT32 rate = pSkillundBase->m_randomSkillVec[i].second;
		skillOdds.insert(std::make_pair(skillID,rate));
		maxVal += rate;
	}

	UINT32 skillID = g_selectByMap(skillOdds,maxVal);
	if ((UINT32)-1 == skillID){
#ifdef _HDZ_DEBUG
		Athena::logger->error("[装备觉醒](name=%s,id=%d)随机不到技能!",pUser->getName(),pUser->getID());
#endif

		return ;
	}

	pItem->m_itemInfo.m_hasGotSkill = skillID;
	retCmd.set_result(Role::stGetEquipSkillResult::SUCCESS);
	retCmd.set_skillid(skillID);
	pUser->sendProto2Me(retCmd);
	//pUser->setupRoleState();
	//pUser->sendBattleProp();
	pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_refineLv,pItem->m_itemInfo.m_refineLv,"装备觉醒",0,0,0,0,0,0);
}

void RoleModHandle::getEquipSkill10Times(GameUser * pUser,Role::st10TimesGetEquipSkill * recvCmd)
{
	if (!pUser || !recvCmd)	{
		return ;
	}

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		Athena::logger->error("[10次装备觉醒](name=%s,id=%d)觉醒失败,找不到要觉醒的装备!",pUser->getName(),pUser->getID());
		return ;
	}

	Role::st10TimesGetEquipSkillResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());

	if ((pUser->getCountByVip(eVipFun_OneKeyGotEquipSkill) < 1) && (pUser->getLevel() < CMiscCfgMgr::getSingleton().getOneKeyStudyEquipSkillLv())){
		retCmd.set_result(Role::st10TimesGetEquipSkillResult::eHas_No_VipRight);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pItem->m_itemInfo.m_hasGotSkill){
		retCmd.set_result(Role::st10TimesGetEquipSkillResult::eHas_Exist_Skill);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[10次装备觉醒](name=%s,id=%d)觉醒失败,觉醒位置已经有技能了!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	stSkillundBase * pSkillundBase = g_heroSkillundBaseMgr.get(1);//约定是1
	if (!pSkillundBase){
		retCmd.set_result(Role::st10TimesGetEquipSkillResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[10次装备觉醒](name=%s,id=%d)找不到配置!",pUser->getName(),pUser->getID());
#endif
		return ;
	}


	UINT16 maxStudyNum = 0;
	UINT16 hasStudyNum = 0;		//已经学习了多少次
	UINT32 studySkillID = 0;	//学习到的技能
	UINT32 stuffNum = pUser->m_bagMgr.getItemNumByID(JUEXING_ITEM_ID);
	if (stuffNum > 10){
		maxStudyNum = 10;
	}
	else {
		maxStudyNum = stuffNum;
	}

	UINT32 addSkillExp = 0;

	UINT32 maxVal = 0;
	std::map<UINT32,UINT32> skillOdds;
	for (UINT16 i = 0;i < pSkillundBase->m_randomSkillVec.size();++i){
		UINT32 skillID = pSkillundBase->m_randomSkillVec[i].first;
		UINT32 rate = pSkillundBase->m_randomSkillVec[i].second;
		skillOdds.insert(std::make_pair(skillID,rate));
		maxVal += rate;
	}

	bool gotYellow = false;

	if (0 == maxStudyNum){//如果没有道具,用钻石
		UINT16 jueXingNum = pUser->m_countMgr.getDayRefreshValue(ECOUNT_EQUIP_JUEXING);
		if (jueXingNum >= pUser->getCountByVip(eVipFun_Equip_Juexing)){
			retCmd.set_result(Role::st10TimesGetEquipSkillResult::eTime_Use_Out);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[10次装备觉醒](name=%s,id=%d)觉醒失败,次数已经用完了!",pUser->getName(),pUser->getID());
#endif
			return ;
		}

		UINT32 useDiamond = 0;
		UINT32 oldHasStudyNum = pUser->m_countMgr.getDayRefreshValue(ECOUNT_EQUIP_JUEXING);
		for (UINT16 i = 0;i < 10; ++i){
			if (oldHasStudyNum + hasStudyNum >= pUser->getCountByVip(eVipFun_Equip_Juexing)){//次数已经用完了
				break;
			}

			UINT32 needDiamond = CMiscCfgMgr::getSingleton().getDiamondByJueXing(oldHasStudyNum + i);
			if (!pUser->checkResource(eResource_Diamond,useDiamond + needDiamond)){
				break;
			}
			useDiamond += needDiamond;
			studySkillID = g_selectByMap(skillOdds,maxVal);
			hasStudyNum += 1;

			stSkillBase * pSkillBase = g_skillBaseMgr.get(studySkillID);
			if (pSkillBase && (pSkillBase->m_quality >= 3)){//是紫色
				gotYellow = true;
				break;
			}

			SkillGrownBase * pSkillGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(studySkillID,0));
			if (pSkillGrownBase){
				addSkillExp += pSkillGrownBase->m_returnExp;
			}
		}

		if ((!pUser->removeResource(eResource_Diamond,useDiamond,"10次装备觉醒")) || (hasStudyNum <= 0)){
			retCmd.set_result(Role::st10TimesGetEquipSkillResult::eDiamond_Not_Enough);
			pUser->sendProto2Me(retCmd);
			return ;
		}
		pUser->m_countMgr.addDayRefreshValue(ECOUNT_EQUIP_JUEXING,hasStudyNum);
	}
	else {
		for (UINT16 i = 0;i < maxStudyNum; ++i){
			studySkillID = g_selectByMap(skillOdds,maxVal);
			if ((UINT32)-1 == studySkillID){
				break;
			}
			hasStudyNum += 1;
			stSkillBase * pSkillBase = g_skillBaseMgr.get(studySkillID);
			if (pSkillBase && (pSkillBase->m_quality >= 3)){//是橙色
				gotYellow = true;
				break;
			}

			SkillGrownBase * pSkillGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(studySkillID,0));
			if (pSkillGrownBase){
				addSkillExp += pSkillGrownBase->m_returnExp;
			}
		}

		if (!pUser->m_bagMgr.removeItemByItemID(JUEXING_ITEM_ID,0,hasStudyNum,"10次装备觉醒")){
			retCmd.set_result(Role::st10TimesGetEquipSkillResult::eItem_Not_Enough);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}

	if (!gotYellow){
		studySkillID = 0;
	}

	pItem->m_itemInfo.m_hasGotSkill = studySkillID;
	if (addSkillExp > 0){
		pUser->addResource(eResource_Juexing_Exp,addSkillExp,"10连觉醒");
	}
	retCmd.set_getjuexingexp(addSkillExp);
	retCmd.set_result(Role::st10TimesGetEquipSkillResult::eSuccess);
	retCmd.set_skillid(studySkillID);
	retCmd.set_juexingnum(hasStudyNum);
	pUser->sendProto2Me(retCmd);
	//pUser->setupRoleState();
	//pUser->sendBattleProp();
}

void RoleModHandle::refineEquipSkill(GameUser * pUser,Role::stReqRefineEquipSkill * recvCmd)
{
	if (!pUser || !recvCmd)	{
		return ;
	}

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		Athena::logger->error("[觉醒技能熔炼](name=%s,id=%d)觉醒熔炼失败,找不到装备!",pUser->getName(),pUser->getID());
		return ;
	}

	Role::stRefineEquipSkillResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());

	if (!pItem->m_itemInfo.m_hasGotSkill){
		retCmd.set_result(Role::stRefineEquipSkillResult::eSkill_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	SkillGrownBase * pSkillGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(pItem->m_itemInfo.m_hasGotSkill,0));
	if (!pSkillGrownBase){
		retCmd.set_result(Role::stRefineEquipSkillResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_result(Role::stRefineEquipSkillResult::eSuccess);
	pUser->sendProto2Me(retCmd);
	pItem->m_itemInfo.m_hasGotSkill = 0;
	pUser->addResource(eResource_Juexing_Exp,pSkillGrownBase->m_returnExp,"熔炼装备技能");
}

void RoleModHandle::moveEquipSkill2Slot(GameUser *pUser,Role::stMoveEquipSkill2Slot * recvCmd)
{
	if (!pUser || !recvCmd)	{
		return ;
	}

	Role::stMoveEquipSkillResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());
	retCmd.set_index(recvCmd->index());

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		Athena::logger->error("[觉醒移动技能](name=%s,id=%d)觉醒移动技能失败,找不到装备!",pUser->getName(),pUser->getID());
		return ;
	}

	if (recvCmd->index() >= MAX_EQUIP_SLOT_NUM){
		return ;
	}

	UINT16 needLv = CMiscCfgMgr::getSingleton().getNeedLvByPos(recvCmd->index() + 1);
	if ((needLv == (UINT16)-1) || (needLv > pUser->getLevel())){
		retCmd.set_result(Role::stMoveEquipSkillResult::eSlot_Was_Locked);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	UINT32 oldSkillID = pItem->m_itemInfo.m_slot[recvCmd->index()].m_skillID;
	UINT32 oldSkillLv = pItem->m_itemInfo.m_slot[recvCmd->index()].m_skillLv;
	UINT32 returnExp = 0;

	if (oldSkillID){
		SkillGrownBase * pSkillBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(oldSkillID,oldSkillLv));
		if (!pSkillBase){
#ifdef _HDZ_DEBUG
			Athena::logger->error("[觉醒移动技能](name=%s,id=%d)找不到技能配置文件,替换失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
		returnExp = pSkillBase->m_returnExp;
	}

	if (returnExp > 0){
		pUser->giveAssert(eResource_Juexing_Exp,returnExp,0,"替换觉醒技能");
	}

	pItem->m_itemInfo.m_slot[recvCmd->index()].m_skillID = pItem->m_itemInfo.m_hasGotSkill;
	pItem->m_itemInfo.m_slot[recvCmd->index()].m_skillLv = 0;
	pItem->m_itemInfo.m_hasGotSkill = 0;
	pUser->m_bagMgr.refreshItem(pItem);

	retCmd.set_skillid(pItem->m_itemInfo.m_hasGotSkill);
	retCmd.set_result(Role::stMoveEquipSkillResult::eSuccess);
	pUser->sendProto2Me(retCmd);
	pUser->setupRoleState();
	pUser->sendBattleProp();
}

void RoleModHandle::upgradeEquipSkill(GameUser *pUser,Role::stReqUpgradeEquipSkill * recvCmd)
{
	if (!pUser || !recvCmd)	{
		return ;
	}

	Role::stUpgradeEquipSkillResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());
	retCmd.set_index(recvCmd->index());

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		Athena::logger->error("[升级觉醒技能](name=%s,id=%d)升级觉醒技能失败,找不到装备!",pUser->getName(),pUser->getID());
		return ;
	}

	UINT32 skillID = pItem->m_itemInfo.m_slot[recvCmd->index()].m_skillID;
	UINT8  lv  = pItem->m_itemInfo.m_slot[recvCmd->index()].m_skillLv;
	SkillGrownBase * pSkillGrownBase1 = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillID,lv));
	SkillGrownBase * pSkillGrownBase2 = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillID,lv + 1));
	if (!pSkillGrownBase1){
		retCmd.set_result(Role::stUpgradeEquipSkillResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[升级觉醒技能](name=%s,id=%d)升级觉醒技能失败,找不到配置!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pSkillGrownBase2){
		retCmd.set_result(Role::stUpgradeEquipSkillResult::eFull_Lv);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[升级觉醒技能](name=%s,id=%d)升级觉醒技能失败,已经生到最高级了!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->removeResource((eResource)pSkillGrownBase1->m_costType,pSkillGrownBase1->m_cost,"觉醒技能升级")){
		retCmd.set_result(Role::stUpgradeEquipSkillResult::eMoney_Not_Enough);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[升级觉醒技能](name=%s,id=%d)升级觉醒技能失败,已经生到最高级了!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	retCmd.set_result(Role::stUpgradeEquipSkillResult::eSuccess);
	pItem->m_itemInfo.m_slot[recvCmd->index()].m_skillLv += 1;
	pUser->m_bagMgr.refreshItem(pItem);
	pUser->sendProto2Me(retCmd);
	pUser->setupRoleState();
	pUser->sendBattleProp();

	Game::stSkillChgLog webCmd;
	webCmd.set_action(4);
	webCmd.set_decgoldcoind(pSkillGrownBase1->m_cost);
	webCmd.set_oldlv(lv);
	webCmd.set_newlv(lv + 1);
	webCmd.set_pos(recvCmd->index() + 1);
	webCmd.set_skillid(skillID);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

}

void RoleModHandle::upgradeUserSkill(GameUser *pUser,Role::stReqRaiseUserSkillLv * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRaiseUserSkillLvResult retCmd;
	retCmd.set_skillid(recvCmd->skillid());
	CSkill *pSkill =  pUser->m_skillMgr.getSkillByID(recvCmd->skillid());
	if (!pSkill){
		retCmd.set_result(Role::stRaiseUserSkillLvResult::eSkill_Not_Exist);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[升级玩家技能](name=%s,id=%d)找不到升级的技能,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	SkillGrownBase * pGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(pSkill->m_skillInfo.m_skillID,pSkill->m_skillInfo.m_level));

	if (!pGrownBase){
		retCmd.set_result(Role::stRaiseUserSkillLvResult::eInner_Cfg_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[升级玩家技能](name=%s,id=%d)找不到技能成长配置,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (pUser->getLevel() < pGrownBase->m_needLv){//等级限制
		retCmd.set_result(Role::stRaiseUserSkillLvResult::eRole_Lv_Limit);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[升级玩家技能](name=%s,id=%d)玩家等级限制,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->removeResource(eResource_GoldCoin,pGrownBase->m_cost,"强化玩家技能")){
		retCmd.set_result(Role::stRaiseUserSkillLvResult::eGoldCoin_Not_Enough);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[升级玩家技能](name=%s,id=%d)金币不足,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT16 oldLv = pSkill->m_skillInfo.m_level;
	pSkill->m_skillInfo.m_level += 1;
	pSkill->init();

	retCmd.set_result(Role::stRaiseUserSkillLvResult::eSuccess);
	retCmd.set_skilllv(pSkill->m_skillInfo.m_level);
	pUser->sendProto2Me(retCmd);

	pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_SKILL_TO_LV);
	//pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_SKILL_NUM);
	stRaiseSkillNum raiseSkillNumNotify;
	raiseSkillNumNotify.notify(pUser,1);
	pUser->setupRoleState();
	pUser->sendBattleProp();

	Game::stSkillChgLog webCmd;
	webCmd.set_action(1);
	webCmd.set_decgoldcoind(pGrownBase->m_cost);
	webCmd.set_oldlv(oldLv);
	webCmd.set_newlv(oldLv + 1);
	webCmd.set_pos(0);
	webCmd.set_skillid(recvCmd->skillid());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void RoleModHandle::saveNewBieGuideID(GameUser * pUser,Role::stSaveNewBieGuideStep * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	pUser->m_roleInfo.m_newBieGuideID =  recvCmd->stepid();
	/*----韩国项目日志----*/
	CKoreaLogMgr::getSingleton().userGuide(g_gameServer->getSec(),pUser->getAccount(),pUser->getID(),pUser->getLevel(),recvCmd->stepid());
	/*----韩国项目日志----*/
}

void RoleModHandle::levelOpenBox(GameUser * pUser,Role::stOpenLevelBox * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	pUser->m_levelMgr.openBox();
}

void RoleModHandle::reqSPLilianRecoverTime(GameUser * pUser,Role::stReqRecoverRemainTime * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stSPAndLilianRecoverRemainTime retCmd;

	if (pUser->m_roleInfo.m_sp < pUser->m_roleState.m_maxSP){
		UINT32 remainTime = 0;
		if (pUser->m_recoverSPLilianTime + GameUser::s_RECOVER_SP_LILIAN_INTERVAL > g_gameServer->getSec()){
			remainTime = pUser->m_recoverSPLilianTime + GameUser::s_RECOVER_SP_LILIAN_INTERVAL - g_gameServer->getSec();
		}
		retCmd.set_spremaintime(remainTime);
	}
	else {
		retCmd.set_spremaintime(0);
	}

	if (pUser->m_roleInfo.m_trainSp < pUser->m_roleState.m_maxTrainSP){
		UINT32 remainTime = 0;
		if (pUser->m_recoverSPLilianTime + GameUser::s_RECOVER_SP_LILIAN_INTERVAL > g_gameServer->getSec()){
			remainTime = pUser->m_recoverSPLilianTime + GameUser::s_RECOVER_SP_LILIAN_INTERVAL - g_gameServer->getSec();
		}
		retCmd.set_trainspremaintime(remainTime);
	}
	else {
		retCmd.set_trainspremaintime(0);
	}

	if (pUser->m_roleInfo.m_lilian < pUser->m_roleState.m_maxLilian){
		UINT32 remainTime = 0;
		if (pUser->m_recoverSPLilianTime + GameUser::s_RECOVER_SP_LILIAN_INTERVAL > g_gameServer->getSec()){
			remainTime = pUser->m_recoverSPLilianTime + GameUser::s_RECOVER_SP_LILIAN_INTERVAL - g_gameServer->getSec();
		}
		retCmd.set_lilianremaintime(remainTime);
	}
	else {
		retCmd.set_lilianremaintime(0);
	}

	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::swapHeroBattlePos(GameUser * pUser,Role::stSwapHeroBattlePos * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stSwapHeroBattlePosError errorCmd;

	HeroInfo * pSrcHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->srcheroid());
	HeroInfo * pDstHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->destheroid());

	if (!pSrcHeroInfo || !pDstHeroInfo){
		errorCmd.set_result(Role::stSwapHeroBattlePosError::eHERO_NOT_EXIST);
		pUser->sendProto2Me(errorCmd);
		return ;
	}

	if ((!pUser->m_heroMgr.isBattle(recvCmd->srcheroid())) || (!pUser->m_heroMgr.isBattle(recvCmd->destheroid()))){
		errorCmd.set_result(Role::stSwapHeroBattlePosError::eHERO_NOT_GOOUT);
		pUser->sendProto2Me(errorCmd);
		return ;
	}

	if (pUser->m_heroMgr.swapBattlePos(recvCmd->srcheroid(),recvCmd->destheroid())){
		Role::stSwapHeroBattlePos retCmd;
		retCmd.set_srcheroid(recvCmd->srcheroid());
		retCmd.set_destheroid(recvCmd->destheroid());
		retCmd.set_srcpos(recvCmd->srcpos());
		retCmd.set_destpos(recvCmd->destpos());
		pUser->sendProto2Me(retCmd);
	}
}

void RoleModHandle::saveSystemSetting(GameUser * pUser,Role::stReqSaveSystemSetting * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	pUser->m_systemSetting = recvCmd->systemsetting();
}

void RoleModHandle::unlockRareSkill(GameUser * pUser,Role::stUnlockRareSkill * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stUnlockRareSkillResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo || !pHeroInfo->m_pHeroBase){
		retCmd.set_result(Role::stUnlockRareSkillResult::eHero_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_skillid(pHeroInfo->m_pHeroBase->m_skill4);

	SkillGrownBase * pSkillGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(pHeroInfo->m_pHeroBase->m_skill4,0));

	if (!pSkillGrownBase){
		retCmd.set_result(Role::stUnlockRareSkillResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->getLevel() < pHeroInfo->m_pHeroBase->m_skill4UnlockLv){
		retCmd.set_result(Role::stUnlockRareSkillResult::eLevel_Limit);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->removeAssert(pSkillGrownBase->m_costType,pSkillGrownBase->m_cost,0,"解锁稀有技能")){
		retCmd.set_result(Role::stUnlockRareSkillResult::eHas_No_GoldCoin);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pHeroInfo->addActiveSkill(pHeroInfo->m_pHeroBase->m_skill4);
	pHeroInfo->upgradeActiveSkill(pHeroInfo->m_pHeroBase->m_skill4);
	pUser->m_heroMgr.refreshHero(pHeroInfo);
	retCmd.set_result(Role::stUnlockRareSkillResult::eSuccess);
	pUser->sendProto2Me(retCmd);
	return ;
}

void RoleModHandle::upgradeRareSkill(GameUser * pUser,Role::stUpgradeRareSkill * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stUpgradeRareSkillResult retCmd;
	retCmd.set_heroid(recvCmd->heroid());
	retCmd.set_skillid(recvCmd->skillid());

	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		retCmd.set_result(Role::stUpgradeRareSkillResult::eHero_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	UINT16 curLv = pHeroInfo->getActiviSkillLv(recvCmd->skillid());

	SkillGrownBase * pSkillGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(recvCmd->skillid(),curLv));
	SkillGrownBase * pSkillGrownNextBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(recvCmd->skillid(),curLv + 1));

	if (!pSkillGrownBase || !pSkillGrownNextBase){
		retCmd.set_result(Role::stUpgradeRareSkillResult::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->getLevel() < pSkillGrownBase->m_needLv){
		retCmd.set_result(Role::stUpgradeRareSkillResult::eLevel_Limit);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->removeAssert(pSkillGrownBase->m_costType,pSkillGrownBase->m_cost,0,"升级稀有技能")){
		retCmd.set_result(Role::stUpgradeRareSkillResult::eHas_No_GoldCoin);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pHeroInfo->upgradeActiveSkill(recvCmd->skillid());
	pUser->m_heroMgr.refreshHero(pHeroInfo);
	retCmd.set_result(Role::stUpgradeRareSkillResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	Game::stSkillChgLog webCmd;
	webCmd.set_action(3);
	webCmd.set_decgoldcoind(pSkillGrownBase->m_cost);
	webCmd.set_oldlv(curLv);
	webCmd.set_newlv(curLv + 1);
	webCmd.set_pos(0);
	webCmd.set_skillid(recvCmd->skillid());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void RoleModHandle::gotFirstRechargeReward(GameUser * pUser,Role::stReqFirstRechargeReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRetFirstRechargeReward retCmd;
	if (pUser->m_roleInfo.m_bitmask & BITMASK_GOT_FIRST_RECHARGE_REWARD){
		retCmd.set_result(Role::stRetFirstRechargeReward::eHas_Got_Reward);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->getRechargeNum() <= 0){
		retCmd.set_result(Role::stRetFirstRechargeReward::eHas_Not_Recharge);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->m_boxMgr.needSlotNum(CRMBShop::firstRechargeBoxID()) > pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1)){
		retCmd.set_result(Role::stRetFirstRechargeReward::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pUser->m_roleInfo.m_bitmask |= BITMASK_GOT_FIRST_RECHARGE_REWARD;
	pUser->m_boxMgr.openBox(CRMBShop::firstRechargeBoxID());

	retCmd.set_result(Role::stRetFirstRechargeReward::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::gotFirst10RechargeReward(GameUser * pUser,Role::stReqFirst10RechargeReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRetFirst10RechargeReward retCmd;
	if (pUser->m_roleInfo.m_bitmask & BITMASK_GOT_10FIRST_RECHARGE_REWARD){
		retCmd.set_result(Role::stRetFirst10RechargeReward::eHas_Got_Reward);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	UINT32 diamond = 60;
	// if (Athena::global["CountryVersion"] == std::string("Korea")) {
	// 	diamond = 300;
	// }
	
	if (pUser->getRechargeNum() < diamond){
		retCmd.set_result(Role::stRetFirst10RechargeReward::eHas_Not_Enough_Recharge);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->m_boxMgr.needSlotNum(CRMBShop::first10RechargeBoxID()) > pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1)){
		retCmd.set_result(Role::stRetFirst10RechargeReward::eBag_Full);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_result(Role::stRetFirst10RechargeReward::eSuccess);
	pUser->sendProto2Me(retCmd);

	pUser->m_roleInfo.m_bitmask |= BITMASK_GOT_10FIRST_RECHARGE_REWARD;
	pUser->m_boxMgr.openBox(CRMBShop::first10RechargeBoxID());
}

void RoleModHandle::saveSubNewBieGuideID(GameUser * pUser,Role::stSubNewBieGuideID * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	pUser->m_roleInfo.m_subNewBieGuideID = recvCmd->id();
}

void RoleModHandle::resetTeamCopymap(GameUser * pUser,Role::stResetTeamCopyMapNum * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stResetTeampCopyMapNumResult retCmd;

	UINT32 resetNum = pUser->m_countMgr.getDayRefreshValue(ECOUNT_TEAM_COPYMAP_RESETNUM);
	if (resetNum >= pUser->getCountByVip(eVipFun_TeamCopyResetNum)){
		retCmd.set_result(Role::stResetTeampCopyMapNumResult::eReset_Time_Use_Out);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	UINT32 needDiamond = CMiscCfgMgr::getSingleton().getTeamCopyResetCostByNum(resetNum + 1);
	if (!pUser->removeResource(eResource_Diamond,needDiamond,"重置副本")){
		retCmd.set_result(Role::stResetTeampCopyMapNumResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pUser->m_countMgr.addDayRefreshValue(ECOUNT_TEAM_COPYMAP_RESETNUM,1);
	//pUser->m_countMgr.resetValue(ECOUNT_TEAM_COPYMAP_NUM);
	pUser->m_levelMgr.resetTeamCopymapNum();
	retCmd.set_result(Role::stResetTeampCopyMapNumResult::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::resetRealTimeCopymap(GameUser * pUser,Role::stResetRealTimeCopyMapNum * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stResetRealTimeCopymapNumResult retCmd;

	UINT32 resetNum = pUser->m_countMgr.getDayRefreshValue(ECOUNT_REALTIME_RESETNUM);

	if (resetNum >= pUser->getCountByVip(eVipFun_RealTimeResetNum)){
		retCmd.set_result(Role::stResetRealTimeCopymapNumResult::eReset_Time_Use_Out);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	UINT32 needDaimond = CMiscCfgMgr::getSingleton().getRealTimeResetCostByNum(resetNum + 1);
	if (!pUser->removeResource(eResource_Diamond,needDaimond,"重置实时战场")){
		retCmd.set_result(Role::stResetRealTimeCopymapNumResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pUser->m_countMgr.addDayRefreshValue(ECOUNT_REALTIME_RESETNUM,1);
	pUser->m_countMgr.resetValue(ECOUNT_REAL_TIME_NUM);
	retCmd.set_result(Role::stResetRealTimeCopymapNumResult::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqChgEquipSkill(GameUser * pUser,Role::stReqChgEquipSkill * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRetChgEquipSkillResult retCmd;
	retCmd.set_srcinstanceid(recvCmd->srcinstanceid());
	retCmd.set_dstinstanceid(recvCmd->dstinstanceid());

	CItem * pSrcItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->srcinstanceid());
	CItem * pDstItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->dstinstanceid());

	if (!pSrcItem || !pSrcItem->m_pItemBase){
		retCmd.set_result(Role::stRetChgEquipSkillResult::eSrc_Item_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pDstItem || !pDstItem->m_pItemBase){
		retCmd.set_result(Role::stRetChgEquipSkillResult::eDst_Item_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	CItem * pChgSoulItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->chgsouliteminstanceid());
	if (!pChgSoulItem || !pChgSoulItem->m_pItemBase || (pChgSoulItem->m_pItemBase->m_type != ITEM_TYPE_CHG_SOUL)){
		retCmd.set_result(Role::stRetChgEquipSkillResult::eHas_Not_Chg_Soul_Item);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if ((pSrcItem->m_pItemBase->m_quality < ITEM_CLASSTYPE_ORANGE_MIN_VAL)
			|| (pDstItem->m_pItemBase->m_quality < ITEM_CLASSTYPE_ORANGE_MIN_VAL)){
		retCmd.set_result(Role::stRetChgEquipSkillResult::eCond_Not_Meet);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if ((!pSrcItem->hasEquipSkill())){
		retCmd.set_result(Role::stRetChgEquipSkillResult::eCond_Not_Meet);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->m_bagMgr.decItem(pChgSoulItem,1,"换魂")){
		retCmd.set_result(Role::stRetChgEquipSkillResult::eDec_Chg_Soul_Item_Fail);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pSrcItem->exechangegSkill(pDstItem);
	pUser->m_bagMgr.refreshItem(pSrcItem);
	pUser->m_bagMgr.refreshItem(pDstItem);
	retCmd.set_result(Role::stRetChgEquipSkillResult::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqUseMonthCard(GameUser * pUser,Role::stReqUseMonthCardItem * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRetUseMonthCardItem retCmd;
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->instanceid());
	if (!pItem){
		retCmd.set_result(Role::stRetUseMonthCardItem::eItem_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (g_isTheSameDay(pItem->m_itemInfo.m_exp,g_gameServer->getSec())){
		retCmd.set_result(Role::stRetUseMonthCardItem::eToday_Has_Use);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (g_gameServer->getSec() > pItem->m_itemInfo.m_expireTime){
		retCmd.set_result(Role::stRetUseMonthCardItem::eExpire);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	pItem->m_itemInfo.m_exp = g_gameServer->getSec();
	pUser->m_bagMgr.refreshItem(pItem);
	pUser->m_boxMgr.openBox(pItem->m_pItemBase->m_boxID,1,false,pItem->getID());
	retCmd.set_result(Role::stRetUseMonthCardItem::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqUseSelectItemBox(GameUser *pUser,Role::stReqUseSelectItemBox * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRetUseSelectItemBox retCmd;
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->instanceid());
	if (!pItem){
		retCmd.set_result(Role::stRetUseSelectItemBox::eItem_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	UINT32 boxID = pItem->m_pItemBase->m_boxID;
	if (!pUser->m_bagMgr.decItem(pItem,1,"使用可选道具箱子")){
		retCmd.set_result(Role::stRetUseSelectItemBox::eDec_Item_Fail);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	pUser->m_boxMgr.openBox(boxID,1,false,recvCmd->selectindex());
	retCmd.set_result(Role::stRetUseSelectItemBox::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqUseHeroSkillBook(GameUser *pUser,Role::stReqUseHeroSkillBook * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRetUseHeroSkillBook retCmd;
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->instanceid());
	if (!pItem){
		retCmd.set_result(Role::stRetUseHeroSkillBook::eItem_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		retCmd.set_result(Role::stRetUseHeroSkillBook::eHero_Not_Exist);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[使用领悟技能书](name=%s,id=%d)找不到武将,使用失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT32 skillID = pItem->m_pItemBase->m_effect;
	if (!pUser->m_bagMgr.decItem(pItem,1,"使用领悟技能书")){
		retCmd.set_result(Role::stRetUseHeroSkillBook::eDec_Item_Fail);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	UINT32 oldSkillID = pHeroInfo->getStudySkillID();
	pHeroInfo->setStudySkillID(skillID);
	pUser->m_heroMgr.refreshHero(pHeroInfo);
	retCmd.set_result(Role::stRetUseHeroSkillBook::eSuccess);
	pUser->sendProto2Me(retCmd);

	if (oldSkillID > 0){
		SkillGrownBase * pSkillBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(oldSkillID,0));
		if (!pSkillBase){
			return ;
		}
		UINT32 returnExp = pSkillBase->m_returnExp;
		if (returnExp > 0){
			pUser->addResource(eResource_Lingwu_Exp,returnExp,"使用领悟技能书");
		}
	}

}

void RoleModHandle::reqUseEquipSkillBook(GameUser *pUser,Role::stReqUseEquipSkillBook * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	Role::stRetUseEquipSkillBook retCmd;

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->instanceid());
	CItem * pDestItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->destbagtype(),recvCmd->destinstanceid());

	if (!pItem || (pItem->m_pItemBase->m_type != ITEM_TYPE_JUEXING_BOOK)){
		retCmd.set_result(Role::stRetUseEquipSkillBook::eItem_Not_Exist);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pDestItem || !CItem::isEquipByType(pDestItem->m_pItemBase->m_type) || (pDestItem->m_pItemBase->m_quality < ITEM_CLASSTYPE_PURPLE_MIN_VAL)){
		retCmd.set_result(Role::stRetUseEquipSkillBook::eEquip_Cond_Not_Meet);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	UINT32 newSkillID = pItem->m_pItemBase->m_effect;

	if (!pUser->m_bagMgr.decItem(pItem,1,"使用觉醒技能书")){
		retCmd.set_result(Role::stRetUseEquipSkillBook::eDec_Item_Fail);
		pUser->sendProto2Me(retCmd);
		return;
	}

	UINT32 oldSkillID = pDestItem->m_itemInfo.m_hasGotSkill;
	pDestItem->m_itemInfo.m_hasGotSkill = newSkillID;
	pUser->m_bagMgr.refreshItem(pDestItem);
	retCmd.set_result(Role::stRetUseEquipSkillBook::eSuccess);
	pUser->sendProto2Me(retCmd);


	if (oldSkillID > 0){
		SkillGrownBase * pSkillBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(oldSkillID,0));
		if (!pSkillBase){
			return ;
		}
		UINT32 returnExp = pSkillBase->m_returnExp;
		if (returnExp > 0){
			pUser->addResource(eResource_Juexing_Exp,returnExp,"使用觉醒技能书");
		}
	}
}

struct stBreakEquipCB : public stItemCallBack
{
	UINT32 m_itemID;
	UINT32 m_num;
	//UINT32 m_instanceID;
	std::set<CItem *> m_vec;

	stBreakEquipCB():m_itemID(0),m_num(0)
	{}

	virtual  bool doIt(CItem * pItem)
	{
		if (pItem && pItem->m_itemInfo.m_itemID == m_itemID)
		{
			m_vec.insert(pItem);
			m_num += pItem->m_itemInfo.m_num;
		}
		return true;

	}
};

void RoleModHandle::reqBreakItem(GameUser *pUser, Role::stReqBreakEquip * recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return ;
	}

	Role::stBreakEquipResult retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());

    if (pUser->getLevel() < CMiscCfgMgr::getSingleton().getOperaLvlLimit(CMiscCfgMgr::Limit_Equip_Break)) {
		Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,人物等级不够",pUser->getName(),pUser->getID());
		return ;
    }

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(), recvCmd->instanceid());

	if(!pItem || !pItem->m_pItemBase)
	{
		retCmd.set_result(Role::stBreakEquipResult::eEQUIP_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,要突破的装备不存在!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if(!CItem::isEquipByType(pItem->m_pItemBase->m_type))
	{
		retCmd.set_result(Role::stBreakEquipResult::eEQUIP_NOT_EXIST);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,不是装备!",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if(pItem->m_pItemBase->m_type != ITEM_TYPE_WING)//不是翅膀
	{
		UINT8 quality = pItem->m_pItemBase->m_quality;
		if(quality < 22)
		{
			retCmd.set_result(Role::stBreakEquipResult::eITEM_NOT_ORANGE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,装备不是品质22以上的橙色装备!",pUser->getName(),pUser->getID());
#endif
			return;
		}
	}

	stBreakBase * pBreakBase = g_breakBaseMgr.get(stBreakBase::getFindID(pItem->m_itemInfo.m_itemID,pItem->m_itemInfo.m_breakLv));
	stBreakBase * pNextBreakBase = g_breakBaseMgr.get(stBreakBase::getFindID(pItem->m_itemInfo.m_itemID,pItem->m_itemInfo.m_breakLv + 1));

	if(!pBreakBase || !pNextBreakBase)
	{
		retCmd.set_result(Role::stBreakEquipResult::eINNER_ERROR);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,找不到配置!",pUser->getName(),pUser->getID());
#endif
		return;
	}


	if(!pNextBreakBase)
	{
		retCmd.set_result(Role::stBreakEquipResult::eFULL_LV);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,已突破到最大等级!",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if (pItem->m_itemInfo.m_raiseLv < pBreakBase->m_raiseLvLimit){
		retCmd.set_result(Role::stBreakEquipResult::eRAISE_LV_LIMIT);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,强化等级限制，强化等级不足!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	for(UINT8 i = 0; i < pBreakBase->m_costResVec.size(); ++i)
	{
		if(!pUser->checkResource((eResource)pBreakBase->m_costResVec[i].first, pBreakBase->m_costResVec[i].second))
		{
			retCmd.set_result(Role::stBreakEquipResult::eGOLDCOIN_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,金币不足!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}

	stBreakEquipCB cb;
	for(UINT8 i = 0; i < pBreakBase->m_costItemVec.size(); ++i)
	{
		cb.m_itemID = pBreakBase->m_costItemVec[i].first;
		cb.m_num = 0;
		pUser->m_bagMgr.getItemByCond(cb,Role::BAG_TYPE_MAIN1);
		if (cb.m_num < pBreakBase->m_costItemVec[i].second){//数量不足
			retCmd.set_result(Role::stBreakEquipResult::eITEM_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,需要道具不足!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
	}

	UINT32 decGoldCoin = 0;
	UINT32 decItemID = 0;
	for (UINT16 i = 0;i < pBreakBase->m_costResVec.size();++i)
	{
		decGoldCoin = pBreakBase->m_costResVec[i].second;
		if (!pUser->removeResource((eResource)pBreakBase->m_costResVec[i].first,pBreakBase->m_costResVec[i].second,"突破"))
		{
			retCmd.set_result(Role::stBreakEquipResult::eGOLDCOIN_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[突破装备](name=%s,id=%d)突破失败,扣除金币失败!",pUser->getName(),pUser->getID());
#endif
		}
	}

	for (UINT16 i = 0;i < pBreakBase->m_costItemVec.size(); ++i)
	{
		UINT32 delItemID = pBreakBase->m_costItemVec[i].first;
		UINT32 delNum = pBreakBase->m_costItemVec[i].second;
		decItemID = delItemID;
		std::set<CItem *>::iterator it = cb.m_vec.begin();
		for (;it != cb.m_vec.end();)
		{
			if (delItemID == (*it)->getID())
			{
				if (delNum >= (*it)->m_itemInfo.m_num)
				{
					delNum -= (*it)->m_itemInfo.m_num;
					std::set<CItem *>::iterator tmpIt = it;
					++it;
					pUser->m_bagMgr.decItem(*tmpIt,(*tmpIt)->m_itemInfo.m_num,"装备突破");
					cb.m_vec.erase(tmpIt);
				}
				else
				{
					pUser->m_bagMgr.decItem(*it,delNum,"装备突破");
					delNum = 0;
					++it;
				}
				if (0 == delNum)
				{
					break;
				}
			}
			else
			{
				++it;
			}
		}
	}

	UINT16 oldBreakLv = pItem->m_itemInfo.m_breakLv;
	pItem->m_itemInfo.m_breakLv += 1;
	pItem->setupPropState();
	pUser->m_bagMgr.refreshItem(pItem);
	retCmd.set_result(Role::stBreakEquipResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);

	if (CEquipBag::isAffectRoleData(pItem->m_itemInfo.m_pos))
	{
		pUser->setupRoleState();
		//pUser->sendMainData2Me();
		pUser->sendBattleProp();
	}

	pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),pItem->m_itemInfo.m_breakLv,pItem->m_itemInfo.m_breakLv,oldBreakLv,pItem->m_itemInfo.m_breakLv,"装备突破",decGoldCoin,0,decItemID,0,0,0);

}

void RoleModHandle::reqRecastHero(GameUser *pUser, Role::stReqRecastHero * recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return;
	}

	Role::stRecastHeroResult retCmd;

	std::map<UINT32, UINT32> retAsserts;
	Role::stObtainItemTip itemTipCmd;

	if(pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < recvCmd->heroid_size())
	{
		retCmd.set_result(Role::stRecastHeroResult::BAG_FULL);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将重铸](name=%s,id=%d)包裹空间不足，重铸失败!",pUser->getName(),pUser->getID());
#endif
		return;
	}

	for(UINT16 i = 0; i < recvCmd->heroid_size(); ++i){
		HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid(i));
		if(!pHeroInfo)
		{
			retCmd.set_result(Role::stRecastHeroResult::HERO_NOT_EXIST);
			pUser->sendProto2Me(retCmd);
			Athena::logger->error("[武将重铸](name=%s,id=%d)找不到要重铸的武将,重铸失败!",pUser->getName(),pUser->getID());
			return ;
		}

		stHeroLvPropBase * pLvBase = g_heroLvPropBaseMgr.get(stHeroLvPropBase::genFindID(pHeroInfo->m_dbData.m_id,pHeroInfo->m_dbData.m_level));
		if(!pLvBase){
			retCmd.set_result(Role::stRecastHeroResult::INNER_ERROR);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[武将重铸](name=%s,id=%d)找不到升级表格信息,重铸失败!",pUser->getName(),pUser->getID());
#endif
			return ;
		}

		stHeroEvolutionBase * pEvolutionBase = g_heroEvolutionBaseMgr.get(stHeroEvolutionBase::getFindID(pHeroInfo->m_dbData.m_id,pHeroInfo->m_dbData.m_evolutionLv));
		if (!pEvolutionBase){
			retCmd.set_result(Role::stRecastHeroResult::INNER_ERROR);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[武将重铸](name=%s,id=%d)找不到进化表格信息，重铸失败!",pUser->getName(),pUser->getID());
#endif
			return;
		}

		for(UINT16 i = 0;  i < pLvBase->m_returnAssert.size(); ++i){
			retAsserts[pLvBase->m_returnAssert[i].first] += pLvBase->m_returnAssert[i].second;
			Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
			if(pSyn)
			{
				pSyn->set_itemid(pLvBase->m_returnAssert[i].first);
				pSyn->set_num(pLvBase->m_returnAssert[i].second);
			}
		}
		if(pEvolutionBase->m_retStoneNum > 0)
		{
			retAsserts[pEvolutionBase->m_stoneID] += pEvolutionBase->m_retStoneNum;
			Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
			pSyn->set_itemid(pEvolutionBase->m_stoneID);
			pSyn->set_num(pEvolutionBase->m_retStoneNum);
		}
	}

	if (!pUser->removeResource(eResource_Diamond,200 * recvCmd->heroid_size(),"武将重铸")){
		retCmd.set_result(Role::stRecastHeroResult::DIAMOND_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将重铸](name=%s,id=%d)钻石不足!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	for(UINT16 i = 0; i < recvCmd->heroid_size(); ++i){
		HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid(i));
		if(pHeroInfo)
		{
			pHeroInfo->m_dbData.m_evolutionLv = 0;
			pHeroInfo->m_dbData.m_level = 0;
			pHeroInfo->init();
			pUser->m_heroMgr.refreshHero(pHeroInfo);
		}

		if (pUser->m_heroMgr.isBattle(pHeroInfo->m_dbData.m_id)){
			
			pUser->sendBattleProp();
		}
	}

	std::map<UINT32,UINT32>::iterator it = retAsserts.begin();
	for (;it != retAsserts.end();++it){
		pUser->giveAssert(it->first,it->second,0,"武将重铸返还");

	}

	pUser->setupRoleState();
	pUser->sendBattleProp();

	retCmd.set_result(Role::stRecastHeroResult::SUCCESS);
	pUser->sendProto2Me(retCmd);

	pUser->sendProto2Me(itemTipCmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[武将重铸](name=%s,id=%d)成功!",pUser->getName(),pUser->getID());
#endif
}
void RoleModHandle::reqOpenBuyGoldWindow(GameUser *pUser, Role::stReqOpenBuyGoldWindow * recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return;
	}

	Role::stOpenBuyGoldWindowResult retCmd;

	UINT32 count = pUser->m_countMgr.getDayRefreshValue(ECOUNT_DIAMOND_BUY_GOLD_NUM);

	retCmd.set_buycount(count);

	Athena::logger->trace("[当日宝石购买金币次数](name=%s,id=%d)，当日次数:%d",pUser->getName(),pUser->getID(),count);

	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqBuyGold(GameUser *pUser, Role::stReqBuyGold * recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return;
	}

	pUser->m_shopMgr.diamondBuyGold();
}

//请求附魔
void RoleModHandle::reqEnchant(GameUser *pUser, Role::stReqEnchant * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::stRetEnchant retCmd;
    retCmd.set_bagtype(recvCmd->bagtype());
    retCmd.set_id(recvCmd->id());
    if (!pUser->getEnchantById(recvCmd->id()) && pUser->getEnchantSize() >= 48) {
		retCmd.set_result(Role::stRetEnchant::eMax_Enchant_Num);
		pUser->sendProto2Me(retCmd);
		return ;
    }
	CItem * pFashion = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->id());
	if (!pFashion){
		retCmd.set_result(Role::stRetEnchant::eNo_Such_Fashion);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    CItem * pStone = NULL;
    std::vector<std::pair<CItem *, UINT32> > itemVec;
    UINT8 enchantItemType = CEnchantCfg::ITEM_INVALID;
    UINT32 lockNum = 0, ownNum = 0, recvNum = 0;
    if (recvCmd->enchantid()) {
        //附魔石
        pStone = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1, recvCmd->enchantid());
        if (!pStone){
            retCmd.set_result(Role::stRetEnchant::eEnchant_Less);
            pUser->sendProto2Me(retCmd);
            return ;
        }
        enchantItemType = CEnchantCfg::ITEM_ENCHANT;
        lockNum = pStone->m_itemInfo.m_itemID;
        ownNum = pStone->m_itemInfo.m_num;
    } else {
        //碎片
        for (int i = 0; i < recvCmd->item_size(); i++) {
            CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_PIECE, recvCmd->item(i).id());
            if (pItem && recvCmd->item(i).num() > 0) {
                if (recvCmd->item(i).num() > pItem->m_itemInfo.m_num) {
                    retCmd.set_result(Role::stRetEnchant::ePiece_Less);
                    pUser->sendProto2Me(retCmd);
                    return ;
                }
                recvNum += recvCmd->item(i).num();
                ownNum += pItem->m_itemInfo.m_num;
                itemVec.push_back(std::make_pair(pItem, recvCmd->item(i).num()));
            } else {
                retCmd.set_result(Role::stRetEnchant::ePiece_Less);
                pUser->sendProto2Me(retCmd);
                return ;
            }
        }
        enchantItemType = CEnchantCfg::ITEM_FASHION_PIECE;
        lockNum = pUser->getEnchantLockNum(recvCmd->id());
    }
    CEnchantCfg::EnchantType enchantType;
    if (!CEnchantCfg::getSingleton().getEnchantType(enchantItemType, lockNum, enchantType)) {
        retCmd.set_result(Role::stRetEnchant::eNo_Such_Config);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    UINT8 propNum = pUser->getEnchantPropNum(recvCmd->id());
    if (propNum >= enchantType.propNum && pUser->getEnchantLockNum(recvCmd->id()) >= enchantType.propNum) {
        retCmd.set_result(Role::stRetEnchant::eMax_Prop_Num);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->getGoldCoin() < enchantType.needGold) {
        retCmd.set_result(Role::stRetEnchant::eGold_Less);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (enchantType.enchantItemType == CEnchantCfg::ITEM_FASHION_PIECE) {
        if (enchantType.enchantNum > ownNum) {
            retCmd.set_result(Role::stRetEnchant::ePiece_Less);
            pUser->sendProto2Me(retCmd);
            return ;
        }
        if (enchantType.enchantNum != recvNum) {
            retCmd.set_result(Role::stRetEnchant::ePiece_Wrong_Num);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    } else {
        if (enchantType.enchantNum > ownNum) {
            retCmd.set_result(Role::stRetEnchant::eEnchant_Less);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    }
    UINT16 id = CEnchantCfg::getSingleton().randPropId(enchantType.rangeMinId, enchantType.rangeMaxId);
    CEnchantCfg::EnchantItemInfo itemInfo;
    if (!CEnchantCfg::getSingleton().getEnchantItem(id, itemInfo)) {
        retCmd.set_result(Role::stRetEnchant::eNo_Such_Config);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (recvCmd->enchantid()) {
        //附魔石
        pUser->m_bagMgr.decItem(pStone, enchantType.enchantNum, "时装附魔消耗附魔石");
    } else {
        for (size_t i = 0; i < itemVec.size(); i++) {
            pUser->m_bagMgr.removeItemByItemID(itemVec[i].first->m_itemInfo.m_itemID,
                    0, itemVec[i].second, "时装附魔消耗碎片");
        }
    }
    pUser->removeAssert(eResource_GoldCoin, enchantType.needGold, 0, "时装附魔消耗金币");
    EnchantItem item;
    item.color = itemInfo.color;
    item.propType = itemInfo.propType;
    item.propVal = g_randBetween(itemInfo.rangePropMinNum, itemInfo.rangePropMaxNum);
    UINT8 index = propNum + 1;
    if (propNum >= enchantType.propNum) {
        //替换
        index = pUser->replaceEnchant(recvCmd->id(), item);
    } else {
        pUser->addEnchant(recvCmd->id(), item);
    }
    retCmd.set_result(Role::stRetEnchant::eSuccess);
    Role::stEnchantInfo enchantInfo;
    enchantInfo.set_color(item.color);
    enchantInfo.set_locked(item.locked);
    enchantInfo.set_proptype(item.propType);
    enchantInfo.set_propval(item.propVal);
    enchantInfo.set_index(index);
    *retCmd.mutable_enchant() = enchantInfo;
    pUser->sendProto2Me(retCmd);
    pUser->m_bagMgr.refreshItem(pFashion);
	pUser->setupRoleState();
    pUser->sendBattleProp();
}
//请求时装重铸
void RoleModHandle::reqRecastEnchant(GameUser *pUser, Role::stReqRecastEnchant * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::stRetRecastEnchant retCmd;
    retCmd.set_bagtype(recvCmd->bagtype());
    retCmd.set_instanceid(recvCmd->instanceid());
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		retCmd.set_result(Role::stRetRecastEnchant::eNo_Such_Fashion);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (pItem->m_pItemBase->m_expireTime > 0) {
		retCmd.set_result(Role::stRetRecastEnchant::eLimited_Fashion);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_PIECE) < 2) {
        retCmd.set_result(Role::stRetRecastEnchant::eBag_Full);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->getDiamond() < 40) {
        retCmd.set_result(Role::stRetRecastEnchant::eDiamond_Less);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    char buf[64] = {'\0'};
    sprintf(buf, "%d", pItem->m_itemInfo.m_itemID);
    buf[0] += 1;
    buf[strlen(buf) - 1] = '0';
    UINT32 newId = atoi(buf);
    bool refresh = false;
    if (pUser->getEnchantById(recvCmd->instanceid())) {
        refresh = true;
        pUser->removeEnchantById(recvCmd->instanceid());
    }
    pUser->removeAssert(eResource_Diamond, 40, 0, "时装附魔重铸消耗钻石");
	Athena::logger->trace("[时装附魔重铸](name=%s,id=%d) 时装Id=%d,instanceId=%d",
            pUser->getName(), pUser->getID(), pItem->m_itemInfo.m_itemID, recvCmd->instanceid());
    if (pUser->m_bagMgr.m_fashionBag.removeItem(pItem)) {
        pUser->m_bagMgr.notifyDeleteItem(pItem);
        SAFE_DELETE(pItem);
    }
    //pUser->m_bagMgr.m_fashionBag.refreshBagItem(pUser);
    pUser->m_bagMgr.addItem2Bag(newId, 20, 0, "时装重铸返回碎片");
    retCmd.set_result(Role::stRetRecastEnchant::eSuccess);
    pUser->sendProto2Me(retCmd);
    Role::stObtainItemTip itemTipCmd;
    Role::stSynTipItem * pSyn = NULL;
    pSyn = itemTipCmd.add_getitems();
    pSyn->set_itemid(newId);
    pSyn->set_num(20);
	pUser->sendProto2Me(itemTipCmd);
    if (refresh) {
        pUser->setupRoleState();
        pUser->sendBattleProp();
    }
}
//请求锁定附魔时装
void RoleModHandle::reqLockEnchant(GameUser *pUser, Role::stReqLockEnchant * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::stRetLockEnchant retCmd;
    retCmd.set_bagtype(recvCmd->bagtype());
    retCmd.set_instanceid(recvCmd->instanceid());
    retCmd.set_index(recvCmd->index());
    retCmd.set_islock(recvCmd->islock());
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		retCmd.set_result(Role::stRetLockEnchant::eNo_Such_Fashion);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (!pUser->getEnchantById(recvCmd->instanceid())) {
		retCmd.set_result(Role::stRetLockEnchant::eNo_Such_Fashion);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (recvCmd->index() > pUser->getEnchantPropNum(recvCmd->instanceid())) {
		retCmd.set_result(Role::stRetLockEnchant::eNo_Such_Index);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (!pUser->lockEnchantById(recvCmd->instanceid(), recvCmd->index(), recvCmd->islock())) {
		retCmd.set_result(Role::stRetLockEnchant::eNo_Such_Index);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    retCmd.set_result(Role::stRetLockEnchant::eSuccess);
    pUser->sendProto2Me(retCmd);
}

//随从装备的符文
void RoleModHandle::reqHeroRune(GameUser *pUser, Role::stReqHeroRune * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::stRetHeroRune retCmd;
    retCmd.set_heroid(recvCmd->heroid());
    HeroInfo * pHero = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
    if (!pHero || !pHero->m_pHeroBase) {
        Athena::logger->trace("proto reqHeroRune not find hero for id %d", recvCmd->heroid());
		pUser->sendProto2Me(retCmd);
        return ;
    }

    CItem * pItem = NULL;
    UINT16 slotIndex = (UINT16)-1;
    Role::stHeroRuneInfo * pRune = NULL;
    for (UINT32 i = 0; i < MAX_HERO_RUNE_NUM; i++) {
        slotIndex = pHero->m_runeDbData.m_runeVec[i];
        pItem = pUser->m_bagMgr.m_heroRuneBag.getItemBySlotIndex(slotIndex);//从随从符文包裹里查找
        if (!pItem) {
            Athena::logger->trace("slotIndex is %d, no Item", slotIndex);
            continue;
        }
        pRune = retCmd.add_runes();
        if (pRune) {
            pRune->set_itemid(pItem->m_itemInfo.m_itemID);
            pRune->set_instanceid(pItem->m_itemInfo.m_instanceID);
            pRune->set_slot_index(slotIndex);
            pRune->set_pos(i);
        }
    }
    pUser->sendProto2Me(retCmd);
}

//给随从添加符文
void RoleModHandle::reqAddHeroRune(GameUser *pUser, Role::stReqAddHeroRune * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::stRetAddHeroRune retCmd;
    retCmd.set_heroid(recvCmd->heroid());
    retCmd.set_instanceid(recvCmd->instanceid());
    retCmd.set_pos(recvCmd->pos());
    HeroInfo * pHero = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
    if (!pHero || !pHero->m_pHeroBase) {
        Athena::logger->trace("proto reqAddHeroRune not find hero for id %d", recvCmd->heroid());
        retCmd.set_result(Role::stRetAddHeroRune::eNo_Such_Hero);
		pUser->sendProto2Me(retCmd);
        return ;
    }
    if (recvCmd->pos() >= MAX_HERO_RUNE_NUM) {
        Athena::logger->trace("invalid pos %d", recvCmd->pos());
        retCmd.set_result(Role::stRetAddHeroRune::eNo_Such_Pos);
		pUser->sendProto2Me(retCmd);
        return ;
    }
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_RUNE, recvCmd->instanceid());
    CItem * pSrcItem = pItem;
	if (!pItem || !pItem->m_pItemBase){
        Athena::logger->trace("proto reqAddHeroRune item not exsits for instance id %d", recvCmd->instanceid());
		retCmd.set_result(Role::stRetAddHeroRune::eNo_Such_Rune);
		pUser->sendProto2Me(retCmd);
        return ;
    }
    stItemLevelupBase * pRaiseBase = g_itemLevelupBaseMgr.get(
            stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID, pItem->m_itemInfo.m_raiseLv));
	if (!pRaiseBase){
        Athena::logger->trace("proto reqAddHeroRune item not exsits for instance id %d", recvCmd->instanceid());
		retCmd.set_result(Role::stRetAddHeroRune::eNo_Such_Rune);
		pUser->sendProto2Me(retCmd);
        return ;
    }
    //UINT16 srcAttrType = pRaiseBase->m_attrType;
	UINT16 srcAttrType = 0;
	if(pRaiseBase->m_fuWenProps.size()){
		srcAttrType = pRaiseBase->m_fuWenProps[0].m_propID;
	}

    //属性是否重复
    Role::stHeroRuneInfo stRune;
    std::set<UINT16> attrSet;
    std::vector<Role::stHeroRuneInfo> runesVec;
    UINT16 slotIndex = (UINT16)-1;
    UINT16 dstAttrType = 0;
    CItem * pDstItem = NULL;
    for (UINT32 i = 0; i < MAX_HERO_RUNE_NUM; i++) {
        slotIndex = pHero->m_runeDbData.m_runeVec[i];
        CItem * pItem = pUser->m_bagMgr.m_heroRuneBag.getItemBySlotIndex(slotIndex);//从随从符文包裹里查找
        if (!pItem || !pItem->m_pItemBase) {
            Athena::logger->trace("slotIndex is %d, no Item", slotIndex);
            continue;
        }
        stItemLevelupBase * pRaiseBase = g_itemLevelupBaseMgr.get(
                stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID, pItem->m_itemInfo.m_raiseLv));
        if (!pRaiseBase){
            Athena::logger->trace("slotIndex is %d, no RaiseItem", slotIndex);
            continue;
        }
        if (i != recvCmd->pos()) {
			if(pRaiseBase->m_fuWenProps.size()){
				attrSet.insert(pRaiseBase->m_fuWenProps[0].m_propID);
			}else{
				attrSet.insert(0);
			}
            //attrSet.insert(pRaiseBase->m_attrType);
            stRune.set_itemid(pItem->m_itemInfo.m_itemID);
            stRune.set_instanceid(pItem->m_itemInfo.m_instanceID);
            stRune.set_slot_index(slotIndex);
            stRune.set_pos(i);
            runesVec.push_back(stRune);
        } else {
            //剔除目标位置属性
			if(pRaiseBase->m_fuWenProps.size()){
            	dstAttrType = pRaiseBase->m_fuWenProps[0].m_propID;
			}else{
				dstAttrType = 0;
			}
			//dstAttrType = pRaiseBase->m_attrType;
            pDstItem = pItem;
        }
    }
    if (attrSet.find(srcAttrType) != attrSet.end()) {
        Athena::logger->trace("proto reqAddHeroRune eRune_Same_Attr");
        retCmd.set_result(Role::stRetAddHeroRune::eRune_Same_Attr);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    UINT16 & dstRune = pHero->m_runeDbData.m_runeVec[recvCmd->pos()];

    if (!pUser->m_bagMgr.moveItemForRune(pUser, pSrcItem, slotIndex)) {
        retCmd.set_result(Role::stRetAddHeroRune::eInner_Error);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    stRune.set_itemid(pSrcItem->m_itemInfo.m_itemID);
    stRune.set_instanceid(pSrcItem->m_itemInfo.m_instanceID);
    stRune.set_slot_index(slotIndex);
    stRune.set_pos(recvCmd->pos());
    runesVec.push_back(stRune);
    if (dstRune != (UINT16)-1) {//目标格不为空,则替换物品
        if (!pUser->m_bagMgr.moveItemForRune(pUser, pDstItem, slotIndex)) {
            retCmd.set_result(Role::stRetAddHeroRune::eInner_Error);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    }
    dstRune = pSrcItem->m_itemInfo.m_pos.m_pos;

    Role::stHeroRuneInfo * pRune = NULL;
    for (size_t i = 0; i < runesVec.size(); i++) {
        pRune = retCmd.add_runes();
        if (pRune) {
            pRune->set_itemid(runesVec[i].itemid());
            pRune->set_instanceid(runesVec[i].instanceid());
            pRune->set_slot_index(runesVec[i].slot_index());
            pRune->set_pos(runesVec[i].pos());
        }
    }
    retCmd.set_result(Role::stRetAddHeroRune::eSuccess);
	pSrcItem->setupPropState();
	pUser->m_bagMgr.refreshItem(pSrcItem);
    pUser->sendProto2Me(retCmd);
	pHero->init();
    pUser->m_heroMgr.refreshHero(pHero); 
	pUser->setupRoleState();
	pUser->sendBattleProp(); 
    return ;
}

//给随从移除符文
void RoleModHandle::reqDelHeroRune(GameUser *pUser, Role::stReqDelHeroRune * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::stRetDelHeroRune retCmd;
    retCmd.set_heroid(recvCmd->heroid());
    retCmd.set_instanceid(recvCmd->instanceid());
    retCmd.set_pos(recvCmd->pos());

	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_RUNE) < 1){
		retCmd.set_result(Role::stRetDelHeroRune::eRun_Bag_Full);
		pUser->sendProto2Me(retCmd);
        return ;
    }
    HeroInfo * pHero = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
    if (!pHero || !pHero->m_pHeroBase) {
        Athena::logger->trace("proto reqDelHeroRune not find hero for id %d", recvCmd->heroid());
        retCmd.set_result(Role::stRetDelHeroRune::eNo_Such_Hero);
		pUser->sendProto2Me(retCmd);
        return ;
    }
    if (recvCmd->pos() >= MAX_HERO_RUNE_NUM) {
        Athena::logger->trace("invalid pos %d", recvCmd->pos());
        retCmd.set_result(Role::stRetDelHeroRune::eNo_Such_Pos);
		pUser->sendProto2Me(retCmd);
        return ;
    }
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_HERO_RUNE, recvCmd->instanceid());
    if (!pItem) {
        Athena::logger->trace("proto reqDelHeroRune item not exsits for instance id %d", recvCmd->instanceid());
		retCmd.set_result(Role::stRetDelHeroRune::eNo_Such_Rune);
		pUser->sendProto2Me(retCmd);
        return ;
    }

    Role::stHeroRuneInfo stRune;
    std::vector<Role::stHeroRuneInfo> runesVec;
    UINT16 slotIndex = (UINT16)-1;
    for (UINT32 i = 0; i < MAX_HERO_RUNE_NUM; i++) {
        slotIndex = pHero->m_runeDbData.m_runeVec[i];
        CItem * pItem = pUser->m_bagMgr.m_heroRuneBag.getItemBySlotIndex(slotIndex);//从随从符文包裹里查找
        if (!pItem) {
            Athena::logger->trace("slotIndex is %d, no Item", slotIndex);
            continue;
        }
        if (i != recvCmd->pos()) {
            stRune.set_itemid(pItem->m_itemInfo.m_itemID);
            stRune.set_instanceid(pItem->m_itemInfo.m_instanceID);
            stRune.set_slot_index(slotIndex);
            stRune.set_pos(i);
            runesVec.push_back(stRune);
        }
    }

    if (!pUser->m_bagMgr.moveItemForRune(pUser, pItem, slotIndex)) {
        retCmd.set_result(Role::stRetDelHeroRune::eInner_Error);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    pHero->m_runeDbData.m_runeVec[recvCmd->pos()] = (UINT16)-1;

    Role::stHeroRuneInfo * pRune = NULL;
    for (size_t i = 0; i < runesVec.size(); i++) {
        pRune = retCmd.add_runes();
        if (pRune) {
            pRune->set_itemid(runesVec[i].itemid());
            pRune->set_instanceid(runesVec[i].instanceid());
            pRune->set_slot_index(runesVec[i].slot_index());
            pRune->set_pos(runesVec[i].pos());
        }
    }
    retCmd.set_result(Role::stRetDelHeroRune::eSuccess);
    pUser->sendProto2Me(retCmd);
	pHero->init();
    pUser->m_heroMgr.refreshHero(pHero); 
	pUser->setupRoleState();
	pUser->sendBattleProp();
}

//符文碎片合成符文
void RoleModHandle::reqRunePieceCombine(GameUser *pUser, Role::stReqRunePieceCombine * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::stRetRunePieceCombine retCmd;
    retCmd.set_instanceid(recvCmd->instanceid());
    CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_RUNE_PIECE, recvCmd->instanceid());
    if (!pItem || pItem->m_itemInfo.m_num <= 0) {
        retCmd.set_result(Role::stRetRunePieceCombine::eLess_Of_Rune_Piece);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    CRuneCfg::RequireItem * pRequire = CRuneCfg::getSingleton().getRequireItem(pItem->m_itemInfo.m_itemID);
    if (!pRequire) {
        retCmd.set_result(Role::stRetRunePieceCombine::eNo_Such_Rune_Piece);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_RUNE) <= 0) {
        retCmd.set_result(Role::stRetRunePieceCombine::eRune_Bag_Full);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (!pUser->removeResource(eResource_Soul_Fire, pRequire->m_soulFire, "符文合成")) {
        retCmd.set_result(Role::stRetRunePieceCombine::eLess_Of_Soul_Fire);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    UINT32 tmpRate = 0;
    UINT32 rate = g_randBetween(1, pRequire->m_totalRate);
    std::vector<CRuneCfg::CombineItem> & combineVec = pRequire->m_combineVec;
    UINT32 runeId = 0;
    for (size_t i = 0; i < combineVec.size(); i++) {
        tmpRate += combineVec[i].m_rate;
        if (tmpRate >= rate) {
            runeId = combineVec[i].m_runeId;
            break;
        }
    }
	CItem * pRune = CBagMgr::createItem(runeId, 1, 0);
	if (!pRune){
		Athena::logger->error("[符文合成](name=%s,id=%d)符文合成时创建符文失败",pUser->getName(),pUser->getID());
        retCmd.set_result(Role::stRetRunePieceCombine::eInner_Error);
        pUser->sendProto2Me(retCmd);
        return ;
	}

    UINT32 decItemId = pItem->m_itemInfo.m_itemID;
    if (!pUser->m_bagMgr.removeItemByItemID(decItemId, 0, 1, "符文合成")) {
        retCmd.set_result(Role::stRetRunePieceCombine::eNo_Such_Rune_Piece);
        pUser->sendProto2Me(retCmd);
		return ;
    }
	pRune->m_itemInfo.m_pos.m_pos = pUser->m_bagMgr.getBagFreeIndex(pRune->m_itemInfo.m_pos.m_bagType);

	if (!pUser->m_bagMgr.addItem2Bag(pRune)){
		Athena::logger->error("[符文合成](name=%s,id=%d)符文合成时添加到包裹失败!",pUser->getName(),pUser->getID());
		SAFE_DELETE(pRune);
        retCmd.set_result(Role::stRetRunePieceCombine::eInner_Error);
        pUser->sendProto2Me(retCmd);
		return ;
	}
    retCmd.set_itemid(runeId);
    retCmd.set_result(Role::stRetRunePieceCombine::eSuccess);
	pUser->sendProto2Me(retCmd);

	pUser->m_bagMgr.sendEquipChgLog(runeId,0,0,0,0,"符文碎片合成符文",0,0,decItemId,0,0,runeId);
}

//符文强化
void RoleModHandle::reqRuneIntensify(GameUser *pUser, Role::stReqRuneIntensify * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
	Role::stRetRuneIntensify retCmd;
	retCmd.set_instanceid(recvCmd->instanceid());

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(), recvCmd->instanceid());
	if (!pItem || !pItem->m_pItemBase){
		retCmd.set_result(Role::stRetRuneIntensify::eNo_Such_Rune);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	stItemLevelupBase * pLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,pItem->m_itemInfo.m_raiseLv ));

	if (!pLvUpBase){
		retCmd.set_result(Role::stRetRuneIntensify::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (pUser->getLevel() < pLvUpBase->m_roleLv){//等级不足
		retCmd.set_result(Role::stRetRuneIntensify::eMax_Run_Lvl);
		pUser->sendProto2Me(retCmd);
		return ;
	}
    if (pItem->m_itemInfo.m_raiseLv >= CMiscCfgMgr::getSingleton().getIntensifyLvlLimit()){
		retCmd.set_result(Role::stRetRuneIntensify::eMax_Run_Lvl);
		pUser->sendProto2Me(retCmd);
		return ;
    }

	std::vector<CItem *> delItemVec;
	UINT32 obtainExp = 0;
    if (recvCmd->eatinstanceid_size() > 7){//不能用超过7个道具升级
        retCmd.set_result(Role::stRetRuneIntensify::eMax_Eat_Num);
        pUser->sendProto2Me(retCmd);
        return ;
    }

    for (UINT16 i = 0;i < recvCmd->eatinstanceid_size();++i){
        CItem * pStuffItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_RUNE, recvCmd->eatinstanceid(i));
        if (pStuffItem == pItem){//如果原料就是自己,出现bug
            retCmd.set_result(Role::stRetRuneIntensify::eEat_Self);
            pUser->sendProto2Me(retCmd);
            return ;
        }
        if (pStuffItem && pStuffItem->m_pItemBase) {
            stItemLevelupBase * pRaiseBase =
                g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pStuffItem->m_pItemBase->m_grownID,pStuffItem->m_itemInfo.m_raiseLv));
            obtainExp += pStuffItem->m_itemInfo.m_exp;
            if (pRaiseBase){
                obtainExp += pRaiseBase->m_exp;
            }
            delItemVec.push_back(pStuffItem);
        }
        else {
            retCmd.set_result(Role::stRetRuneIntensify::eNot_Exist_Eat_Rune);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    }

	UINT32 needMoney = (UINT32)(obtainExp);

	if (!pUser->checkResource(eResource_GoldCoin,needMoney)){
		retCmd.set_result(Role::stRetRuneIntensify::eLess_Of_Gold);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pUser->removeResource(eResource_GoldCoin,needMoney,"符文强化")){
		retCmd.set_result(Role::stRetRuneIntensify::eLess_Of_Gold);
		pUser->sendProto2Me(retCmd);
		return ;
	}

    for (size_t i = 0; i < delItemVec.size(); i++) {
        if (!pUser->m_bagMgr.removeRune(delItemVec[i])) {
			retCmd.set_result(Role::stRetRuneIntensify::eInner_Error);
			pUser->sendProto2Me(retCmd);
			Athena::logger->trace("[符文强化](name=%s,id=%d)扣道具不成功强化失败!",pUser->getName(),pUser->getID());
			return ;
		}
	}

	UINT16 oldRaiseLv = pItem->m_itemInfo.m_raiseLv;
	pItem->m_itemInfo.m_exp += obtainExp;
	while (pItem->m_itemInfo.m_exp >= pLvUpBase->m_needExp){
		pItem->m_itemInfo.m_exp -= pLvUpBase->m_needExp;
		pItem->m_itemInfo.m_raiseLv += 1;
        if (pItem->m_itemInfo.m_raiseLv >= CMiscCfgMgr::getSingleton().getIntensifyLvlLimit()){
            pItem->m_itemInfo.m_raiseLv = CMiscCfgMgr::getSingleton().getIntensifyLvlLimit();
            pLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,pItem->m_itemInfo.m_raiseLv));
            if (pLvUpBase && pItem->m_itemInfo.m_exp >= pLvUpBase->m_needExp){
                pItem->m_itemInfo.m_exp = pLvUpBase->m_needExp;
            }
            break;
        }
		pLvUpBase = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(pItem->m_pItemBase->m_grownID,pItem->m_itemInfo.m_raiseLv));
		if (!pLvUpBase){
			break;
		}
	}

	pItem->setupPropState();
	pUser->m_bagMgr.refreshItem(pItem);
	retCmd.set_result(Role::stRetRuneIntensify::eSuccess);
	pUser->sendProto2Me(retCmd);

    HeroInfo * pHero = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
    if (pHero && pHero->hasRune(pItem->m_itemInfo.m_pos.m_pos)) {
		pHero->init();
        pUser->m_heroMgr.refreshHero(pHero); 
		pUser->setupRoleState();
		pUser->sendBattleProp();
    }

    stRaiseEquipNum raiseEquipNumNotify;
    raiseEquipNumNotify.notify(pUser,1);
    pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_EQUIP_TO_LV);
    pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),oldRaiseLv,pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_refineLv,pItem->m_itemInfo.m_refineLv,"强化符文",needMoney,0,0,0,0,0);
    Global::stReqCheckTask cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_squaretype(6);
    cmd.set_battletype(2);
    GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

//符文进化
void RoleModHandle::reqRuneEvolve(GameUser *pUser, Role::stReqRuneEvolve * recvCmd) {
	if(!pUser || !recvCmd)
	{
		return;
	}
	Role::stRetRuneEvolve retCmd;
	retCmd.set_bagtype(recvCmd->bagtype());
	retCmd.set_instanceid(recvCmd->instanceid());

	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem){
		retCmd.set_result(Role::stRetRuneEvolve::eNo_Such_Rune);
		pUser->sendProto2Me(retCmd);
		return ;
	}
    if (pUser->getLevel() < CMiscCfgMgr::getSingleton().getRuneRoleLvl(pItem->m_itemInfo.m_refineLv + 1)) {
		retCmd.set_result(Role::stRetRuneEvolve::eRole_Lvl_Limit);
		pUser->sendProto2Me(retCmd);
		return ;
    }

	stRefineBase * pRefineBase = g_refineBaseMgr.get(stRefineBase::getFindID(pItem->m_itemInfo.m_itemID,pItem->m_itemInfo.m_refineLv));
	stRefineBase * pNextRefineBase = g_refineBaseMgr.get(stRefineBase::getFindID(pItem->m_itemInfo.m_itemID,pItem->m_itemInfo.m_refineLv + 1));
	if (!pRefineBase || !pNextRefineBase){
		retCmd.set_result(Role::stRetRuneEvolve::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	if (!pNextRefineBase){
		retCmd.set_result(Role::stRetRuneEvolve::eRune_Max_Lvl);
		pUser->sendProto2Me(retCmd);
		return ;

	}

	for (UINT16 i = 0;i < pRefineBase->m_costResVec.size();++i){
		if (!pUser->checkResource((eResource)pRefineBase->m_costResVec[i].first,pRefineBase->m_costResVec[i].second)){
			retCmd.set_result(Role::stRetRuneEvolve::eLess_Of_Gold);
			pUser->sendProto2Me(retCmd);
			return ;
		}
	}

    std::list< std::pair<std::vector<CItem *>, UINT32>  > itemsList;
	for (UINT16 i = 0;i < pRefineBase->m_costItemVec.size(); ++i){
        std::vector<CItem *> itemVec;
        pUser->m_bagMgr.m_runeBag.collectRune(recvCmd->instanceid(), pRefineBase->m_costItemVec[i].first, itemVec);
		if (itemVec.size() < pRefineBase->m_costItemVec[i].second){//数量不足
			retCmd.set_result(Role::stRetRuneEvolve::eLess_Of_Rune);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[精炼装备](name=%s,id=%d)精炼失败,需要道具不足!",pUser->getName(),pUser->getID());
#endif
			return ;
		}
        itemsList.push_back(std::make_pair(itemVec, pRefineBase->m_costItemVec[i].second));
	}

	UINT32 decGoldCoin = 0;
	UINT32 decItemID = 0;
	for (UINT16 i = 0;i < pRefineBase->m_costResVec.size();++i){
		decGoldCoin = pRefineBase->m_costResVec[i].second;
		if (!pUser->removeResource((eResource)pRefineBase->m_costResVec[i].first,pRefineBase->m_costResVec[i].second,"精炼")){
			retCmd.set_result(Role::stRetRuneEvolve::eLess_Of_Gold);
			pUser->sendProto2Me(retCmd);
			Athena::logger->error("[精炼装备](name=%s,id=%d)精炼失败,扣除金币失败!",pUser->getName(),pUser->getID());
			return ;
		}
	}

    for (std::list< std::pair<std::vector<CItem *>, UINT32 > >::iterator it = itemsList.begin();
            it != itemsList.end(); it++) {
        std::vector<CItem *> & itemVec = (*it).first;
        UINT32 itemId = 0;
        for (size_t i = 0; i < itemVec.size() && i < (*it).second; i++) {
            itemId = itemVec[i]->m_itemInfo.m_itemID;
            decItemID = itemId;
            if (pUser->m_bagMgr.m_runeBag.removeItem(itemVec[i])) {
                pUser->m_bagMgr.notifyDeleteItem(itemVec[i]);
                SAFE_DELETE(itemVec[i]);
            }
        }
    }

	UINT16 oldRefineLv = pItem->m_itemInfo.m_refineLv;
	pItem->m_itemInfo.m_refineLv += 1;
	pItem->setupPropState();
	pUser->m_bagMgr.refreshItem(pItem);
	retCmd.set_result(Role::stRetRuneEvolve::eSuccess);
	pUser->sendProto2Me(retCmd);

    HeroInfo * pHero = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
    if (pHero && pHero->hasRune(pItem->m_itemInfo.m_pos.m_pos)) {
		pHero->init();
        pUser->m_heroMgr.refreshHero(pHero);
		pUser->setupRoleState();
		pUser->sendBattleProp();
    }

	pUser->m_taskMgr.onPropChange(TaskProto::ET_REFINE_EQUIP_TO_LV);
	pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_FUWEN_NUM);

	pUser->m_bagMgr.sendEquipChgLog(pItem->getID(),pItem->m_itemInfo.m_raiseLv,pItem->m_itemInfo.m_raiseLv,oldRefineLv,pItem->m_itemInfo.m_refineLv,"精炼符文",decGoldCoin,0,decItemID,0,0,0);
}

void RoleModHandle::reqExtractGem(GameUser *pUser, Role::stReqExtractGem *recvCmd)
{
#undef GEM_ERROR_RETURN
#define GEM_ERROR_RETURN(error)                                            \
	do {                                                                   \
		packet.set_result(Role::ExtractGemResult::RESULT_ERROR_##error);   \
		pUser->sendProto2Me(packet);                                       \
		return;                                                            \
	} while (0)

	if (!(pUser && recvCmd))
		return;

    Role::ExtractGemResult packet;

	UINT32 id;
	UINT32 key, index;
	struct GemEntityInfo data;
	UINT32 delgold;
	UINT32 size;
	std::vector<CItem*> src_list;
	CItem *success_item;
	CItem *result;
	UINT32 exppoll(0);

	if (!pUser->m_openFuncMgr.checkFunOpen(OpenFunID_Gem))
		GEM_ERROR_RETURN(LEVEL);
	id = recvCmd->id();
	key = id / 10;
	if (!(key >= 1 && key <= GemGroupDataSize()))
		GEM_ERROR_RETURN(ID);
	index = id % 10;
	if (!(index >= 1 && index <= GemEntitySize(key)))
		GEM_ERROR_RETURN(ID);
	--index;
	GetGemEntityInfo((int)key, (int)index, &data);
	if ((delgold = data.consumegold) > (pUser->m_roleInfo.m_goldCoin))
		GEM_ERROR_RETURN(GOLD);
	size = (UINT32)data.Synthesisnum;
	if (size != (UINT32)recvCmd->pay_size())
		GEM_ERROR_RETURN(LIST);
    //去重
    std::set<UINT64> itemSet;
    for (UINT32 i(0); i < size; ++i) {
        itemSet.insert(recvCmd->pay(i));
    }
    if (size != itemSet.size()) {
        GEM_ERROR_RETURN(LIST);
    }
    //去重
	src_list.reserve(size);
	for (UINT32 i(0); i < size; ++i) {
		UINT64 tmp = recvCmd->pay(i);
		CItem *pItem =
			pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1, tmp);
		if (!pItem)
			GEM_ERROR_RETURN(MISS);
		UINT32 _id = pItem->m_itemInfo.m_itemID;
		do {
			if (_id == (UINT32)data.MaterialsID[0])
				break;
			if (_id == (UINT32)data.MaterialsID[1])
				break;
			if (_id == (UINT32)data.MaterialsID[2])
				break;
			if (_id == (UINT32)data.MaterialsID[3])
				break;
			GEM_ERROR_RETURN(LIST);
		} while (0);
		src_list.push_back(pItem);
		{
			UINT32 _sum(0);
			stItemLevelupBase *p;
			for (UINT32 _lv(pItem->m_itemInfo.m_raiseLv) ; _lv; --_lv) {
				if (!(p = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(_id, (_lv-1)))))
					GEM_ERROR_RETURN(UNKNOWN);
				_sum += p->m_needExp;
			}
			exppoll += pItem->m_itemInfo.m_exp;
			exppoll += _sum;
		}
	}
	UINT32 success_id = (UINT32)data.needitem;
	UINT32 success_size = (UINT32)data.neednum;
	{
		UINT64 tmp = recvCmd->successpay();
		success_item =
			pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1, tmp);
		if (!success_item)
			GEM_ERROR_RETURN(SUCCESSMISS);
		if (success_item->m_itemInfo.m_num < success_size)
			GEM_ERROR_RETURN(SUCCESSMISS);

		UINT32 _id = success_item->m_itemInfo.m_itemID;
		if (_id != success_id)
			GEM_ERROR_RETURN(SUCCESSLIST);
	}
	if (!(result = CBagMgr::createItem(data.resultID, 1, 0)))
		GEM_ERROR_RETURN(UNKNOWN);
	result->m_itemInfo.m_pos.m_pos =
		pUser->m_bagMgr.getBagFreeIndex(result->m_itemInfo.m_pos.m_bagType);
	{
		stItemLevelupBase *p;
		UINT32 _lv(result->m_itemInfo.m_raiseLv);

		if (!(p = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(data.resultID, _lv))))
			GEM_ERROR_RETURN(UNKNOWN);
		if (pUser->getLevel() < p->m_roleLv)
			GEM_ERROR_RETURN(LEVEL);
REDO:
		if (!(p = g_itemLevelupBaseMgr.get(stItemLevelupBase::genFindID(data.resultID, _lv))))
			goto GOON;
		if (exppoll >= p->m_needExp) {
			exppoll -= p->m_needExp;
			++_lv;
			goto REDO;
		}
GOON:
		if (_lv >= (UINT32)GetGemMax()) {
			result->m_itemInfo.m_raiseLv = (UINT32)GetGemMax();
			result->m_itemInfo.m_exp = 0;
		} else {
			result->m_itemInfo.m_raiseLv = _lv;
			result->m_itemInfo.m_exp = exppoll;
		}
	}
	if (!pUser->m_bagMgr.addItem2Bag(result)) {
		SAFE_DELETE(result);
		GEM_ERROR_RETURN(UNKNOWN);
	}
	pUser->m_bagMgr.notifyAddItem(result);
	packet.set_result(Role::ExtractGemResult::RESULT_SUCCESS);
	packet.set_id(id);
	pUser->sendProto2Me(packet);

	for (UINT32 i(0); i < size; ++i) {
		pUser->m_bagMgr.m_mainBag.removeItem(src_list[i]);
		pUser->m_bagMgr.notifyDeleteItem(src_list[i]);
		SAFE_DELETE(src_list[i]);
	}
	/*if (!(success_item->m_itemInfo.m_num)) {
		pUser->m_bagMgr.m_mainBag.removeItem(success_item);
	}*/
	pUser->m_bagMgr.removeItemByItemID(success_item->m_itemInfo.m_itemID, success_size, "合成宝石扣除必成道具数量");

    pUser->removeAssert(eResource_GoldCoin, delgold, 0, "宝石合成扣除金币");

#undef GEM_ERROR_RETURN
}

void RoleModHandle::reqWedding(GameUser *pUser, Role::stReqWedding *recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return;
	}
    CMarryCfg::MarryInfo marryInfo;
    Role::ResultWedding retCmd;
    if (!CMarryCfg::getSingleton().getMarryInfo(recvCmd->cfg1(), marryInfo)) {
        Athena::logger->trace("[请求结婚]拿不到婚礼配置");
		retCmd.set_result(Role::ResultWedding::RESULT_ERROR_UNKNOWN);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    GameUser * pOther = GameUserManager::getSingleton().getUser(recvCmd->id());
    if (!pOther) {
        Athena::logger->trace("[请求结婚]对方不在线或找不到该玩家");
		retCmd.set_result(Role::ResultWedding::RESULT_ERROR_UNKNOWN);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 1 ||
            pOther->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 1) {
		retCmd.set_result(Role::ResultWedding::RESULT_ERROR_BAG);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (!pUser->checkResource((eResource)marryInfo.costType, marryInfo.cost)) {
		retCmd.set_result(Role::ResultWedding::RESULT_ERROR_MONEY);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    retCmd.set_result(Role::ResultWedding::RESULT_SUCCESS_SEND);
    pUser->sendProto2Me(retCmd);

    Role::stReqWedding pkg;
    pkg.set_id(pUser->getID());
    pkg.set_cfg1(recvCmd->cfg1());
    pkg.set_cfg2(recvCmd->cfg2());
    pOther->sendProto2Me(pkg);
}

void RoleModHandle::reqAgreeWedding(GameUser *pUser, Role::stReqAgreeWedding *recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return;
	}
    UINT32 inviter_id = recvCmd->id();
    UINT32 invitee_id = pUser->m_roleInfo.m_charID;
    Role::ResultWedding retCmd;

    CMarryCfg::MarryInfo marryInfo;
    if (!CMarryCfg::getSingleton().getMarryInfo(recvCmd->cfg1(), marryInfo)) {
        Athena::logger->trace("[结婚]拿不到婚礼配置");
		retCmd.set_result(Role::ResultWedding::RESULT_ERROR_UNKNOWN);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    GameUser * pOther  = GameUserManager::getSingleton().getUser(inviter_id);
    if (!pOther) {
        Athena::logger->trace("[同意结婚]对方不在线或找不到该玩家");
        retCmd.set_result(Role::ResultWedding::RESULT_ERROR_UNKNOWN);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (!pOther->checkResource((eResource)marryInfo.costType, marryInfo.cost)) {
        Athena::logger->trace("[同意结婚]对方资源不足");
		retCmd.set_result(Role::ResultWedding::RESULT_ERROR_MONEY);
		pUser->sendProto2Me(retCmd);
		return ;
    }

    if (pUser->getCoupleId() == pOther->m_roleInfo.m_charID) {
        return;
    }
    if (pOther->getCoupleId() == pUser->m_roleInfo.m_charID) {
        return;
    }

    Role::BoardcastMarryInfo board;
    board.set_name1(pUser->getName());
    board.set_name2(pOther->getName());
    GameUserManager::getSingleton().sendProto2World(board);

    pOther->removeResource((eResource)marryInfo.costType, marryInfo.cost, "结婚扣除");

    retCmd.set_result(Role::ResultWedding::RESULT_SUCCESS);
    pOther->sendProto2Me(retCmd);

    Role::ResultWedding pkg;
    pkg.set_result(Role::ResultWedding::RESULT_SUCCESS);
    pUser->sendProto2Me(pkg);

    pUser->setCoupleId(pOther->getID());
    pUser->setCoupleName(pOther->getName());
    pOther->setCoupleId(pUser->getID());
    pOther->setCoupleName(pUser->getName());

    Map::stNotifyEnterMapInfo map1;
    Map::stNotifyEnterMapInfo map2;
    map1.set_mapid(marryInfo.levelId);
    map2.set_mapid(marryInfo.levelId);
    pOther->sendProto2Me(map1);
    pUser->sendProto2Me(map2);

    CMap * pMap = CMapMgr::getSingleton().getMapByID(marryInfo.levelId);
    CMapMgr::getSingleton().changeMap(pUser, pMap);
    CMapMgr::getSingleton().changeMap(pOther, pMap);

    Map::NotifyPlayerAppear husCmd;
    Map::stPlayerData  * pPlayerData = husCmd.add_data();
    if (!pPlayerData){
        return ;
    }
    pOther->getAppear(*pPlayerData);
    //时装翅膀
    CItem * pItem = pOther->m_bagMgr.m_equipBag.getWingItem();
    if (pItem) {
        pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
    }

    pPlayerData = husCmd.add_data();
    if (!pPlayerData){
        return ;
    }
    pUser->getAppear(*pPlayerData);
    //时装翅膀
    pItem = pUser->m_bagMgr.m_equipBag.getWingItem();
    if (pItem) {
        pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
    }
    pUser->sendProto2Me(husCmd);
    pOther->sendProto2Me(husCmd);

    Role::MarryInviterPlace packet3;
    packet3.set_state(Role::MarryInviterPlace::STATE_INVITER);
    packet3.set_cfg1(recvCmd->cfg1());
    packet3.add_id(inviter_id);
    packet3.add_id(invitee_id);
    packet3.set_name1(pOther->getName());
    packet3.set_name2(pUser->getName());
    pOther->sendProto2Me(packet3);

    Role::MarryInviterPlace packet4;
    packet4.set_state(Role::MarryInviterPlace::STATE_INVITER);
    packet4.set_cfg1(recvCmd->cfg1());
    packet4.add_id(inviter_id);
    packet4.add_id(invitee_id);
    packet4.set_name1(pOther->getName());
    packet4.set_name2(pUser->getName());
    pUser->sendProto2Me(packet4);

    Global::stNotifyCoupleMarry cmd;
    if (pUser->getSex() < pOther->getSex()) {
        cmd.set_husband(pUser->getID());
        cmd.set_wife(pOther->getID());
        cmd.set_husname(pUser->getCoupleName());
        cmd.set_wifename(pOther->getCoupleName());
    } else {
        cmd.set_husband(pOther->getID());
        cmd.set_wife(pUser->getID());
        cmd.set_husname(pOther->getCoupleName());
        cmd.set_wifename(pUser->getCoupleName());
    }
    cmd.set_id(recvCmd->cfg1());
    GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void RoleModHandle::reqDivorce(GameUser *pUser, Role::stReqDivorce *recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::RejectDivorce retCmd;
    if (pUser->isSetDivorceExpireTime()) {
        Athena::logger->trace("[reqDivorce] already setDivorceExpireTime");
        retCmd.set_result(Role::RejectDivorce::eCouple_Not_Onlie);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    CMarryCfg::DivorceInfo divorceInfo;
    if (!CMarryCfg::getSingleton().getDivorceInfo(recvCmd->isforce(), divorceInfo)) {
        Athena::logger->trace("[reqDivorce] not find config");
        return ;
    }
    if (!pUser->checkResource((eResource)divorceInfo.costType, divorceInfo.cost)) {
        Athena::logger->trace("[reqDivorce] cost Type %d not enough", divorceInfo.costType);
        retCmd.set_result(Role::RejectDivorce::eLess_Of_Diamond);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (!pUser->getCoupleId()) {
        retCmd.set_result(Role::RejectDivorce::eNo_Couple);
        pUser->sendProto2Me(retCmd);
        return;
    }
    std::string remarkData;
    if (recvCmd->isforce()) {
        remarkData = std::string("强制离婚");
    } else {
        remarkData = std::string("普通离婚");
    }
    if (!pUser->removeResource((eResource)divorceInfo.costType, divorceInfo.cost, remarkData.c_str())) {
        Athena::logger->trace("[reqDivorce] force divorce type %d, cost Type %d not enough", recvCmd->isforce(), divorceInfo.costType);
        retCmd.set_result(Role::RejectDivorce::eLess_Of_Diamond);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    pUser->setDivorceExpireTime(true);
    GameUser * pOther = GameUserManager::getSingleton().getUser(pUser->getCoupleId());
    if (recvCmd->isforce()) {
        MarryMgr::getSingleton().delUserToRoom(pUser->getID());
        MarryMgr::getSingleton().delUserToRoom(pUser->getCoupleId());

        /*
        Global::stRetForceDivorce globalCmd;
        globalCmd.set_couplea(pUser->getID());
        globalCmd.set_coupleb(pUser->getCoupleId());
        globalCmd.set_coupleonline(false);
        */

        pUser->forceDivorce();
        if (pOther) {
            pOther->forceDivorce();
            //globalCmd.set_coupleonline(true);
        }
        Role::stRetRejectDivorce cmd;
        cmd.set_result(Role::stRetRejectDivorce::eSuccess);
        pUser->sendProto2Me(cmd);

        //GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
        Global::stRetRejectDivorce forwardCmd;
        forwardCmd.set_coupleid(pUser->getID());
        forwardCmd.set_time(g_gameServer->getSec());
        GlobalSvrClientMgr::getSingleton().broadcastProto2All(forwardCmd);

        return ;
    }
    retCmd.set_result(Role::RejectDivorce::eSuccess);
    pUser->sendProto2Me(retCmd);

    if (CMarryCfg::getSingleton().getDivorceInfo(false, divorceInfo)) {
        Global::stRetRejectDivorce forwardCmd;
        forwardCmd.set_coupleid(pUser->getCoupleId());
        forwardCmd.set_time(divorceInfo.time + g_gameServer->getSec());
        GlobalSvrClientMgr::getSingleton().broadcastProto2All(forwardCmd);
    }

    if (pOther) {
        Role::stReqDivorce cmd;
        cmd.set_isforce(false);
        cmd.set_coupleid(pUser->getID());
        pOther->sendProto2Me(cmd);
    }
}

void RoleModHandle::reqRejectDivorce(GameUser *pUser, Role::stReqRejectDivorce *recvCmd)
{
	if(!pUser || !recvCmd)
	{
		return;
	}
    Role::stRetRejectDivorce retCmd;
    if (!pUser->getCoupleId()) {
        retCmd.set_result(Role::stRetRejectDivorce::eNot_Marry);
        pUser->sendProto2Me(retCmd);
        return;
    }
    bool setExpireTime = false;
    GameUser * pOther = GameUserManager::getSingleton().getUser(pUser->getCoupleId());
    if (pOther) {
        Role::stRetRejectDivorce cmd;
        if (recvCmd->refused()) {
            cmd.set_result(Role::stRetRejectDivorce::eReject);
            setExpireTime = true;
            pOther->setDivorceExpireTime(false);
        } else {
            cmd.set_result(Role::stRetRejectDivorce::eSuccess);
        }
        pOther->sendProto2Me(cmd);
    } else {
        //清空离婚过期时间
        if (recvCmd->refused()) {
            setExpireTime = true;
        }
    }
    if (setExpireTime) {
        Global::stRetRejectDivorce forwardCmd;
        forwardCmd.set_coupleid(pUser->getCoupleId());
        forwardCmd.set_time(0);
        GlobalSvrClientMgr::getSingleton().broadcastProto2All(forwardCmd);
    }
    if (recvCmd->refused()) {
        retCmd.set_result(Role::stRetRejectDivorce::eReject);
    } else {
        retCmd.set_result(Role::stRetRejectDivorce::eSuccess);
        Global::stRetRejectDivorce forwardCmd;
        forwardCmd.set_coupleid(pUser->getCoupleId());
        forwardCmd.set_time(g_gameServer->getSec());
        GlobalSvrClientMgr::getSingleton().broadcastProto2All(forwardCmd);
    }
    pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqBuyNum(GameUser *pUser, Role::stReqBuyNum *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	Role::stBuyNumResult retCmd;
	retCmd.set_buynum(pUser->m_guildBlackMarketMgr.getBuyNum());
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqBlackMarketList(GameUser *pUser, Role::stReqColumnList *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_guildBlackMarketMgr.sendShopList2Me();
}

void RoleModHandle::reqBuyBlackMarketItem(GameUser *pUser, Role::stReqMarketBuyItem *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_guildBlackMarketMgr.buyItem(recvCmd->index());
}

void RoleModHandle::reqOpenBlackMarkerWindow(GameUser *pUser, Role::stReqOpenWindow *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_guildBlackMarketMgr.openWindow();
}

void RoleModHandle::reqRefreshList(GameUser *pUser, Role::stReqRefreshList *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_guildBlackMarketMgr.reqRefreshShopList();
}

void RoleModHandle::reqRefreshNum(GameUser *pUser, Role::stReqRefreshNum *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	Role::stRefreshNumResult retCmd;
	retCmd.set_diamondrefreshnum(pUser->m_guildBlackMarketMgr.getDiamondNum());
	retCmd.set_refreshnum(pUser->m_guildBlackMarketMgr.getRefreshNum());
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqAKeyRaiseUserSkillLv(GameUser *pUser, Role::stReqAKeyRaiseUserSkillLv * recvCmd)
{
	if(!pUser && !recvCmd){
		return;
	}

	Role::stAKeyRaiseUserSkillLvResult retCmd;
	retCmd.set_skillid(recvCmd->skillid());
	CSkill *pSkill =  pUser->m_skillMgr.getSkillByID(recvCmd->skillid());
	if (!pSkill){
		retCmd.set_result(Role::stAKeyRaiseUserSkillLvResult::eSkill_Not_Exist);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[一键升级玩家技能](name=%s,id=%d)找不到升级的技能,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	SkillGrownBase * pGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(pSkill->m_skillInfo.m_skillID,pSkill->m_skillInfo.m_level));

	if (!pGrownBase){
		retCmd.set_result(Role::stAKeyRaiseUserSkillLvResult::eInner_Cfg_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[一键升级玩家技能](name=%s,id=%d)找不到技能成长配置,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (pUser->getLevel() < pGrownBase->m_needLv){//等级限制
		retCmd.set_result(Role::stAKeyRaiseUserSkillLvResult::eRole_Lv_Limit);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[一键升级玩家技能](name=%s,id=%d)玩家等级限制,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	pGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(pSkill->m_skillInfo.m_skillID,pSkill->m_skillInfo.m_level + 1));
	if(!pGrownBase){
		retCmd.set_result(Role::stAKeyRaiseUserSkillLvResult::eInner_Cfg_Error);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[一键升级玩家技能](name=%s,id=%d)找不到技能成长配置,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if (!pUser->checkResource(eResource_GoldCoin,pGrownBase->m_cost)){
		retCmd.set_result(Role::stAKeyRaiseUserSkillLvResult::eGoldCoin_Not_Enough);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[一键升级玩家技能](name=%s,id=%d)金币不足,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT32 needGoldCoin = 0;
	UINT32 canAchieveLv = 0;

	for(UINT32 i = pSkill->m_skillInfo.m_level + 1; i <= pUser->getLevel(); ++i){
		pGrownBase = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(pSkill->m_skillInfo.m_skillID, i));
		if(pGrownBase){
			needGoldCoin += pGrownBase->m_cost;
			if(!pUser->checkResource(eResource_GoldCoin, needGoldCoin)){
				needGoldCoin -= pGrownBase->m_cost;
				break;
			}

			++canAchieveLv;
		}else{
			break;
		}
	}

	if(!canAchieveLv)
	{
		return;
	}

	if (!pUser->removeResource(eResource_GoldCoin,needGoldCoin,"一键升级玩家技能")){
		retCmd.set_result(Role::stAKeyRaiseUserSkillLvResult::eGoldCoin_Not_Enough);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[一键升级玩家技能](name=%s,id=%d)扣除金币失败,升级技能失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	UINT16 oldLv = pSkill->m_skillInfo.m_level;
	pSkill->m_skillInfo.m_level += canAchieveLv;
	pSkill->init();

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[一键升级玩家技能]升级成功! oldLv:%d, nowLv:%d, up:%d",  oldLv, pSkill->m_skillInfo.m_level, canAchieveLv);
#endif

	retCmd.set_result(Role::stAKeyRaiseUserSkillLvResult::eSuccess);
	retCmd.set_skilllv(pSkill->m_skillInfo.m_level);
	pUser->sendProto2Me(retCmd);

	pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_SKILL_TO_LV);
	//pUser->m_taskMgr.onPropChange(TaskProto::ET_RAISE_SKILL_NUM);
	stRaiseSkillNum raiseSkillNumNotify;
	raiseSkillNumNotify.notify(pUser,canAchieveLv);
	pUser->setupRoleState();
	pUser->sendBattleProp();

	Game::stSkillChgLog webCmd;
	webCmd.set_action(1);
	webCmd.set_decgoldcoind(needGoldCoin);
	webCmd.set_oldlv(oldLv);
	webCmd.set_newlv(oldLv + canAchieveLv);
	webCmd.set_pos(0);
	webCmd.set_skillid(recvCmd->skillid());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void RoleModHandle::reqStoneSynthesis(GameUser * pUser, Role::stReqStoneSynthesis * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	Role::stStoneSynthesisResult retCmd;
	
	bool hasEquitItem = false;
	BagPos srcPos;
	std::vector<CItem*> itemVec;

	for(UINT8 i = 0; i < recvCmd->items_size(); ++i){
		const Role::stMountItemInfo & one = recvCmd->items(i);
		CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(one.bagtype(), one.instanceid());
		if(!pItem){
			retCmd.set_result(Role::stStoneSynthesisResult::eFAILURE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[晶石合成](name=%s,id=%d)失败，道具(instanceID:%lld, bagType:%d)不存在",pUser->getName(),pUser->getID(), one.instanceid(), one.bagtype());
#endif
			return;
		}

		if(one.bagtype() == Role::BAG_TYPE_EQUIP){
			srcPos = pItem->m_itemInfo.m_pos;
			
			BagPos destPos;
			destPos.m_bagType = Role::BAG_TYPE_MOUNT;
			destPos.m_pos = pUser->m_bagMgr.getBagFreeIndex(Role::BAG_TYPE_MOUNT);

			if (!pUser->m_bagMgr.moveItem(pItem,destPos)){
				retCmd.set_result(Role::stStoneSynthesisResult::eFAILURE);
				pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[晶石合成](name=%s,id=%d)失败, 移入装备背包出错",pUser->getName(),pUser->getID());
#endif
				return;
			}else{
				Role::stMoveItem retCmd;
				retCmd.set_bagtype(Role::BAG_TYPE_EQUIP);
				retCmd.set_instanceid(pItem->m_itemInfo.m_instanceID);
				retCmd.mutable_bagpos()->set_bagtype(destPos.m_bagType);
				retCmd.mutable_bagpos()->set_pos(destPos.m_pos);
				pUser->sendProto2Me(retCmd);
			}

			hasEquitItem = true;
			pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MOUNT, one.instanceid());
			if(!pItem){
				retCmd.set_result(Role::stStoneSynthesisResult::eFAILURE);
				pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[晶石合成](name=%s,id=%d)失败，道具(instanceID:%lld)不存在",pUser->getName(),pUser->getID(), one.instanceid());
#endif
				return;
			}
			itemVec.push_back(pItem);
		}else if(one.bagtype() == Role::BAG_TYPE_MOUNT){
			itemVec.push_back(pItem);
		}else{
			return;
		}
	}

	UINT32 _itemid = 0;
	if(itemVec.size()){
		_itemid = itemVec[0]->getID();
	}
	std::vector<CItem*>::iterator it = itemVec.begin();
	for(; it != itemVec.end(); ++it){
		if(_itemid != (*it)->getID()){
			retCmd.set_result(Role::stStoneSynthesisResult::eFAILURE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[晶石合成](name=%s,id=%d)失败, ID不一致",pUser->getName(),pUser->getID());
#endif
			return;
		}
	}

	stSynthesis * info = CMountSynthesisCfg::getSingleton().getSynthesisByitemID(_itemid);
	if(!info){
		retCmd.set_result(Role::stStoneSynthesisResult::eFAILURE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石合成](name=%s,id=%d)失败，配置不存在",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if(itemVec.size() < info->m_synthesisNum){
		retCmd.set_result(Role::stStoneSynthesisResult::eRESOURCE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石合成](name=%s,id=%d)失败，资源不足",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if(!pUser->checkResource(eResource_GoldCoin, info->m_cost)){
		retCmd.set_result(Role::stStoneSynthesisResult::eRESOURCE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石合成](name=%s,id=%d)失败，资源不足",pUser->getName(),pUser->getID());
#endif
		return;
	}

	it = itemVec.begin();
	for(;it != itemVec.end(); ++it){
		if(pUser->m_bagMgr.m_mountBag.removeItem(*it)){
			pUser->m_bagMgr.notifyDeleteItem(*it);
			SAFE_DELETE(*it);
		}else{
			retCmd.set_result(Role::stStoneSynthesisResult::eRESOURCE_NOT_ENOUGH);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[晶石合成](name=%s,id=%d)失败，资源不足",pUser->getName(),pUser->getID());
#endif
			return;
		}
	}

	CItem * newItem = CBagMgr::createItem(info->m_resultitemID, 1, 0);
	if(!newItem){
		retCmd.set_result(Role::stStoneSynthesisResult::eFAILURE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石合成](name=%s,id=%d)失败,创建道具不成功!",pUser->getName(),pUser->getID());
#endif
		return;
	}

	newItem->m_itemInfo.m_pos.m_pos = pUser->m_bagMgr.getBagFreeIndex(newItem->m_itemInfo.m_pos.m_bagType);
	if (!pUser->m_bagMgr.addItem2Bag(newItem)){
		Athena::logger->error("[晶石合成](name=%s,id=%d)晶石合成时添加到包裹失败!",pUser->getName(),pUser->getID());
		SAFE_DELETE(newItem);
		retCmd.set_result(Role::stStoneSynthesisResult::eFAILURE);
		pUser->sendProto2Me(retCmd);
		return;
	}

	if(!pUser->removeResource(eResource_GoldCoin, info->m_cost,"晶石合成")){
		retCmd.set_result(Role::stStoneSynthesisResult::eRESOURCE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石合成](name=%s,id=%d)失败，资源不足",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if(hasEquitItem){
		if (!pUser->m_bagMgr.moveItem(newItem,srcPos)){
			retCmd.set_result(Role::stStoneSynthesisResult::eFAILURE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[晶石合成](name=%s,id=%d)失败, 移入装备背包出错",pUser->getName(),pUser->getID());
#endif
			return;
		}else{
			Role::stMoveItem retCmd;
			retCmd.set_bagtype(Role::BAG_TYPE_MOUNT);
			retCmd.set_instanceid(newItem->m_itemInfo.m_instanceID);
			retCmd.mutable_bagpos()->set_bagtype(srcPos.m_bagType);
			retCmd.mutable_bagpos()->set_pos(srcPos.m_pos);
			pUser->sendProto2Me(retCmd);
		}
	}
	
	newItem->setupPropState();
	pUser->m_bagMgr.refreshItem(newItem);
	if (hasEquitItem && CEquipBag::isAffectRoleData(newItem->m_itemInfo.m_pos)){
		pUser->setupRoleState();
		pUser->sendBattleProp();
	}

	Role::stObtainItemTip itemTipCmd;
	Role::stSynTipItem * pSyn = itemTipCmd.add_getitems(); 
	if(pSyn){
		pSyn->set_itemid(info->m_resultitemID);
		pSyn->set_num(1);
	}
	pUser->sendProto2Me(itemTipCmd);

	retCmd.set_result(Role::stStoneSynthesisResult::eSUCCESS);
	retCmd.set_itemid(info->m_resultitemID);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqStoneEvolvement(GameUser * pUser, Role::stReqStoneEvolvement * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	Role::stStoneEvolvementResult retCmd;

	if(recvCmd->items_size() < 2){
		retCmd.set_result(Role::stStoneEvolvementResult::eRESOURCE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石进化](name=%s,id=%d)失败，道具数量不足",pUser->getName(),pUser->getID());
#endif
		return;
	}

	BagPos srcPos;
	bool hasEquitItem = false;

	for(UINT8 i = 0; i < recvCmd->items_size(); ++i){
		const Role::stMountItemInfo &one = recvCmd->items(i);
		CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(one.bagtype(), one.instanceid());
		
		if(!pItem){
			retCmd.set_result(Role::stStoneEvolvementResult::eFAILURE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[晶石合成](name=%s,id=%d)失败，道具(instanceID:%lld, bagType:%d)不存在",pUser->getName(),pUser->getID(), one.instanceid(), one.bagtype());
#endif
			return;
		}

		if(one.bagtype() == Role::BAG_TYPE_EQUIP){
			srcPos = pItem->m_itemInfo.m_pos;

			BagPos destPos;
			destPos.m_bagType = Role::BAG_TYPE_MOUNT;
			destPos.m_pos = pUser->m_bagMgr.getBagFreeIndex(Role::BAG_TYPE_MOUNT);

			if (!pUser->m_bagMgr.moveItem(pItem,destPos)){
				retCmd.set_result(Role::stStoneEvolvementResult::eFAILURE);
				pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[晶石合成](name=%s,id=%d)失败, 移出装备背包出错",pUser->getName(),pUser->getID());
#endif
				return;
			}else{
				Role::stMoveItem retCmd;                                 
				retCmd.set_bagtype(Role::BAG_TYPE_EQUIP);                
				retCmd.set_instanceid(pItem->m_itemInfo.m_instanceID);   
				retCmd.mutable_bagpos()->set_bagtype(destPos.m_bagType); 
				retCmd.mutable_bagpos()->set_pos(destPos.m_pos);         
				pUser->sendProto2Me(retCmd);                             
			}

			pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MOUNT, one.instanceid());
			if(!pItem){
				retCmd.set_result(Role::stStoneEvolvementResult::eFAILURE);
				pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[晶石进化](name=%s,id=%d)失败，道具不存在",pUser->getName(),pUser->getID());
#endif
				return;
			}

			hasEquitItem = true;
		}else if(one.bagtype() == Role::BAG_TYPE_MOUNT){
		
		}else{
			return;
		}
	}

	CItem * pItem1 = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MOUNT, recvCmd->items(0).instanceid());
	if(!pItem1){
		return;
	}
	CItem * pItem2 = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MOUNT, recvCmd->items(1).instanceid());
	if(!pItem2){
		return;
	}
	stEvolvement * info = CMountSynthesisCfg::getSingleton().getEvolvementByitemID(pItem1->getID(), pItem2->getID());
	if(!info){
		retCmd.set_result(Role::stStoneEvolvementResult::eFAILURE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石进化](name=%s,id=%d)失败，配置不存在",pUser->getName(),pUser->getID());
#endif
		return;
	}


	if(!pUser->checkResource(eResource_GoldCoin, info->m_cost)){
		retCmd.set_result(Role::stStoneEvolvementResult::eRESOURCE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石进化](name=%s,id=%d)失败，资源不足",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if(pUser->m_bagMgr.m_mountBag.removeItem(pItem1)){
		pUser->m_bagMgr.notifyDeleteItem(pItem1);
		SAFE_DELETE(pItem1);
	}else{
		retCmd.set_result(Role::stStoneEvolvementResult::eRESOURCE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石进化](name=%s,id=%d)失败，扣除进化的道具不成功",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if(pUser->m_bagMgr.m_mountBag.removeItem(pItem2)){
		pUser->m_bagMgr.notifyDeleteItem(pItem2);
		SAFE_DELETE(pItem2);
	}else{
		retCmd.set_result(Role::stStoneEvolvementResult::eRESOURCE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石进化](name=%s,id=%d)失败，扣除进化的道具不成功",pUser->getName(),pUser->getID());
#endif
		return;
	}

	CItem * newItem = CBagMgr::createItem(info->m_resultitemID, 1, 0);
	if(!newItem){
		retCmd.set_result(Role::stStoneEvolvementResult::eFAILURE);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石进化](name=%s,id=%d)失败,创建道具不成功!",pUser->getName(),pUser->getID());
#endif
		return;
	}

	newItem->m_itemInfo.m_pos.m_pos = pUser->m_bagMgr.getBagFreeIndex(newItem->m_itemInfo.m_pos.m_bagType);

	if (!pUser->m_bagMgr.addItem2Bag(newItem)){
		Athena::logger->error("[晶石进化](name=%s,id=%d)晶石合成时添加到包裹失败!",pUser->getName(),pUser->getID());
		SAFE_DELETE(newItem);
		retCmd.set_result(Role::stStoneEvolvementResult::eFAILURE);
		pUser->sendProto2Me(retCmd);
		return;
	}

	if(!pUser->removeResource(eResource_GoldCoin, info->m_cost,"晶石进化")){
		retCmd.set_result(Role::stStoneEvolvementResult::eRESOURCE_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[晶石进化](name=%s,id=%d)失败，资源不足",pUser->getName(),pUser->getID());
#endif
		return;
	}

	if(hasEquitItem){
		if (!pUser->m_bagMgr.moveItem(newItem,srcPos)){
			retCmd.set_result(Role::stStoneEvolvementResult::eFAILURE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[晶石进化](name=%s,id=%d)失败,移入 装备背包出错",pUser->getName(),pUser->getID());
#endif
			return;
		}else{
			Role::stMoveItem retCmd;
			retCmd.set_bagtype(Role::BAG_TYPE_MOUNT);
			retCmd.set_instanceid(newItem->m_itemInfo.m_instanceID);
			retCmd.mutable_bagpos()->set_bagtype(srcPos.m_bagType);
			retCmd.mutable_bagpos()->set_pos(srcPos.m_pos);
			pUser->sendProto2Me(retCmd);
		}
	}

	newItem->setupPropState();
	pUser->m_bagMgr.refreshItem(newItem);
	if (hasEquitItem && CEquipBag::isAffectRoleData(newItem->m_itemInfo.m_pos)){
		pUser->setupRoleState();
		pUser->sendBattleProp();
	}
	
	Role::stObtainItemTip itemTipCmd;
	Role::stSynTipItem * pSyn = itemTipCmd.add_getitems(); 
	if(pSyn){
		pSyn->set_itemid(info->m_resultitemID);
		pSyn->set_num(1);
	}
	pUser->sendProto2Me(itemTipCmd);

	retCmd.set_result(Role::stStoneEvolvementResult::eSUCCESS);
	retCmd.set_itemid(info->m_resultitemID);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqStoneProp(GameUser * pUser, Role::stReqStoneProp *recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_mountMgr.sendStoneProp();
}

void RoleModHandle::reqChangePhoto(GameUser *pUser, Role::stReqChangePhoto * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}
	
	Role::stChangePhotoResult retCmd;
	retCmd.set_id(recvCmd->id());
	if(!pUser->m_photoMgr.isExists(recvCmd->id())){
		retCmd.set_result(Role::stChangePhotoResult::eFAILURE);	
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[更换头像](name=%s,id=%d)内部错误, id:%d 不存在",pUser->getName(),pUser->getID(), recvCmd->id());
#endif
		return;	
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[更换头像](name=%s,id=%d)成功, id=%d",pUser->getName(),pUser->getID(), recvCmd->id());
#endif
	pUser->m_photoMgr.setUsePhotoID(recvCmd->id());
	pUser->synMyPhotoID();
	retCmd.set_result(Role::stChangePhotoResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqHasPhotoIDs(GameUser *pUser, Role::stReqHasPhotoID * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	pUser->m_photoMgr.sendPhotoList();
}

void RoleModHandle::reqUnlockPhoto(GameUser *pUser, Role::stReqUnlockPhoto * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}	
	
	Role::stUnlockPhotoResult retCmd;
	if(!pUser->m_photoMgr.isExists(recvCmd->id())){
		stProFilePhotoInfo * info = CProFilePhotoCfg::getSingleton().getConfigurationByID(pUser->getRoleID(), recvCmd->id());
		if(!info){
			retCmd.set_result(Role::stUnlockPhotoResult::eFAILURE);
			pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[解锁头像](name=%s,id=%d)内部错误, 配置不存在",pUser->getName(),pUser->getID());
#endif
			return;
		}

		switch(info->m_unlockCondition)
		{
			case 1: 
				{
					if(info->m_conditionNum > pUser->getLevel()){
						retCmd.set_result(Role::stUnlockPhotoResult::eCOND_NOT_ENOUGH);
						pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
						Athena::logger->error("[解锁头像](name=%s,id=%d)失败，等级不足",pUser->getName(),pUser->getID());
#endif
						return;
					}
				}
				break;
			case 2:
				{
					if(!pUser->removeResource(eResource_Diamond, info->m_conditionNum, "解锁头像")){
						retCmd.set_result(Role::stUnlockPhotoResult::eCOND_NOT_ENOUGH);
						pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
						Athena::logger->error("[解锁头像](name=%s,id=%d)失败，钻石不足",pUser->getName(),pUser->getID());
#endif
						return;
					}
				}
				break;
			case 3:
				{
					if(!pUser->removeAssert(info->m_conditionNum, 1, 0, "解锁头像")){
						retCmd.set_result(Role::stUnlockPhotoResult::eCOND_NOT_ENOUGH);
						pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
						Athena::logger->error("[解锁头像](name=%s,id=%d)失败，道具(id:%d)不足",pUser->getName(),pUser->getID(), info->m_conditionNum);
#endif
						return;
					}		
				}
				break;
			default:
				return;
		}
		pUser->m_photoMgr.addIDToSet(recvCmd->id());
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[解锁头像](name=%s,id=%d)成功, id=%d",pUser->getName(),pUser->getID(), recvCmd->id());
#endif
	pUser->m_photoMgr.sendPhotoList();
	retCmd.set_result(Role::stUnlockPhotoResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqSpriteInfo(GameUser *pUser, Role::stSpriteInfo * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	Role::stSpriteInfoResult retCmd;
	UINT32 id = pUser->m_spriteSysMgr.getFollowSprite();
	retCmd.set_id(id);
	retCmd.set_level(pUser->m_spriteSysMgr.getSpriteLvl(id));
	retCmd.set_result(Role::stSpriteInfoResult::eSUCCESS);
	pUser->sendProto2Me(retCmd);
}

void RoleModHandle::reqTrainHero(GameUser *pUser, Role::stTrainHeroReq * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	Role::stTrainHeroResult result;
	result.set_heroid(recvCmd->heroid());
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,recvCmd->instanceid());
	if (!pItem || !pItem->m_pItemBase || pItem->m_pItemBase->m_type != ITEM_TYPE_TRAIN){
		result.set_result(Role::stTrainHeroResult::eItem_Not_Exist);
		pUser->sendProto2Me(result);
		return ;
	}

	if (!pUser->checkResource(eResource_TrainSp, 1))
	{
		result.set_result(Role::stTrainHeroResult::eTrainSP);
		pUser->sendProto2Me(result);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将调教](name=%s,id=%d)调教体力不足,调教失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	HeroInfo * pHeroInfo = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroid());
	if (!pHeroInfo){
		result.set_result(Role::stTrainHeroResult::eHero_Not_Exist);
		pUser->sendProto2Me(result);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[武将调教](name=%s,id=%d)找不到要调教的武将,调教失败!",pUser->getName(),pUser->getID());
#endif
		return ;
	}

	if(pHeroInfo->m_dbData.m_trainLv >= CHeroTrainCfg::getSingleton().getMaxHeroTrainLvl()) {
		result.set_result(Role::stTrainHeroResult::eHero_Max_Train_Level);
		pUser->sendProto2Me(result);
		return ;
	}

	//加经验
	pHeroInfo->m_dbData.m_trainExp += pItem->m_pItemBase->m_effect;

	//扣道具
	pUser->m_bagMgr.decItem(pItem,1,"调教随从");

	//扣体力
	pUser->removeResource(eResource_TrainSp,1,"调教随从");

	bool lvup = pUser->m_heroMgr.checkTrainLevelUp(pHeroInfo);

	pUser->m_heroMgr.refreshHero(pHeroInfo);

	result.set_levelup(lvup);
	result.set_newlevel(pHeroInfo->m_dbData.m_trainLv);
	result.set_result(Role::stTrainHeroResult::eSuccess);
	pUser->sendProto2Me(result);
}

void RoleModHandle::reqGuajiAward(GameUser *pUser, Role::stGuaJiAwardReq * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	UINT32 internal = 60;
	if (Athena::global["nftServer"] == "true")
	{
		internal = 720;
	}

	Role::stGuaJiAwardResult result;
	if(0 == pUser->m_roleInfo.m_guajiStart) //没挂机
	{
		result.set_result(Role::stGuaJiAwardResult::eTime);
		pUser->sendProto2Me(result);
		return ;
	}

	UINT32 now = g_gameServer->getSec();

	if(now - pUser->m_roleInfo.m_guajiStart < internal)
	{
		result.set_result(Role::stGuaJiAwardResult::eTime);
		pUser->sendProto2Me(result);
		return ;
	}
	UINT32 lvID = pUser->m_levelMgr.getGuajiLv();
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(lvID);
	if (!pCopyMapBase){
		Athena::logger->error("[领取挂机奖励]找不到关卡配置! lvID=%d", lvID);
		return ;
	}
	stGuajiLimit limit = CMiscCfgMgr::getSingleton().getGuajiLimit();

	UINT32 minute = (now - pUser->m_roleInfo.m_guajiStart) / internal;
	if (now > pUser->m_roleInfo.m_guajiEnd) {
		minute = (pUser->m_roleInfo.m_guajiEnd - pUser->m_roleInfo.m_guajiStart) / internal;
	}

	if (Athena::global["nftServer"] == "true")
	{ 
		pUser->addResource(eResource_Sp, 1,"领取挂机奖励");
	}
	else{
		pUser->addExp(minute*limit.m_exp);

		UINT32 money = pCopyMapBase->m_goldCoin * minute / limit.m_goldRate;
		pUser->addResource(eResource_GoldCoin, money,"领取挂机奖励");
		pUser->m_roleInfo.m_guajiStart += minute*internal;
	}

	if(pUser->m_roleInfo.m_guajiStart >= pUser->m_roleInfo.m_guajiEnd){
		pUser->m_roleInfo.m_guajiStart = 0;
		pUser->m_roleInfo.m_guajiEnd = 0;
	}
	result.set_result(Role::stGuaJiAwardResult::eSuccess);
	pUser->sendProto2Me(result);
	pUser->sendGuaji();
}


//开始挂机
void RoleModHandle::reqGuajiStart(GameUser *pUser, Role::stGuaJiStartReq * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}
	if (Athena::global["nftServer"] != "true")
	{
		return;
	}
	Athena::logger->error("[开始挂机](name=%s,id=%d)",pUser->getName(),pUser->getID());
	Role::stGuaJiStartResult result;

	if (pUser->m_roleInfo.m_guajiTime > 0)
	{
		result.set_result(Role::stGuaJiStartResult::eTodaylimit);
		pUser->sendProto2Me(result);
		return;
	}
	if (pUser->m_roleInfo.m_guajiStart != 0)
	{
		result.set_result(Role::stGuaJiStartResult::eInGuaji);
		pUser->sendProto2Me(result);
		return; 
	}
	UINT32 lvID = pUser->m_levelMgr.getGuajiLv();
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(lvID);
	if (!pCopyMapBase)
	{
		result.set_result(Role::stGuaJiStartResult::eInner_Error);
		pUser->sendProto2Me(result);
		return;
	}
	stGuajiLimit limit = CMiscCfgMgr::getSingleton().getGuajiLimit();

	std::map<UINT32,UINT32>::iterator it = limit.m_mjCoin.find(pCopyMapBase->m_chapter);
	if (it == limit.m_mjCoin.end()){
		result.set_result(Role::stGuaJiStartResult::eInner_Error);
		pUser->sendProto2Me(result);
		return;
	}
	
	UINT32 coin = it->second;
	if (!pUser->checkResource(eResource_Diamond, coin))
	{
		result.set_result(Role::stGuaJiStartResult::eCoin);
		pUser->sendProto2Me(result);
		return;
	}
	
	if (!pUser->removeResource(eResource_Diamond,coin,"开始挂机"))
	{
		result.set_result(Role::stGuaJiStartResult::eCoin);
		pUser->sendProto2Me(result);
		return;
	}


	pUser->m_roleInfo.m_guajiStart = g_gameServer->getSec();
	pUser->m_roleInfo.m_guajiEnd = pUser->m_roleInfo.m_guajiStart + 3600*5;

	pUser->m_roleInfo.m_guajiTime = g_gameServer->getSec();

	result.set_result(Role::stGuaJiStartResult::eSuccess);
	pUser->sendProto2Me(result);
	pUser->sendGuaji();
}