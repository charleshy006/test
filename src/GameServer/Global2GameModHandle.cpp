#include "Global2GameModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "GlobalSvrClient.h"
#include "MapMgr.h"
#include "Map.h"
#include "GuildConfig.h"
#include "GuildProto.pb.h"
#include "CopyMap.h"
#include "DropMgr.h"
#include "datacenter_log.h"
#include "log_singleton.h"
#include "KVPair.h"
#include "RobotMgr.h"
#include "GameEventNotify.h"
#include "HistoryRankMgr.h"
#include "Friend.pb.h"
#include "KoreaLog.h"
#include "MiscCfgMgr.h"
#include "SortList.pb.h"
#include "MiscCfgMgr.h"
#include "GuardCfg.h"
#include "DevilCallCfg.h"
#include "Misc.pb.h"
#include "PubGame.pb.h"
#include "SortList.pb.h"
#include "Team.pb.h"
#include "Chat.h"
#include "ExpeditionConfig.h"
#include "OperateActivityCfg.h"
#include  "EmeraldDreamCfg.h"
#include "ItemBoxCfg.h"
#include <cmath>
#include "HeroLeagueCfg.h"
#include "MarryCfg.h"
#include "MarryMgr.h"
#include "ResBackCfg.h"
#include "NftConfig.h"

extern GameServer * g_gameServer;

Global2GameModHandle::Global2GameModHandle()
{
	registerHandle(&Global2GameModHandle::globalRetRoleInfo);
	registerHandle(&Global2GameModHandle::userRecharge);
	registerHandle(&Global2GameModHandle::synFetchLilianNum);
	registerHandle(&Global2GameModHandle::createGuildDecMoney);
	registerHandle(&Global2GameModHandle::synMyGuildInfo);
	registerHandle(&Global2GameModHandle::guildPray);
	registerHandle(&Global2GameModHandle::confirmChgSvr);
	registerHandle(&Global2GameModHandle::sendOther2Me);
	registerHandle(&Global2GameModHandle::obtainMailAttach);
	registerHandle(&Global2GameModHandle::pkWithOther);
	registerHandle(&Global2GameModHandle::userLeaveGuild);
	registerHandle(&Global2GameModHandle::teamEnterCopymap);
	registerHandle(&Global2GameModHandle::one2OnePVPEnterCopymap);
	registerHandle(&Global2GameModHandle::one2OnePKWithRobot);
	registerHandle(&Global2GameModHandle::captureTreasureReward);
	registerHandle(&Global2GameModHandle::notifyAddLilian);
	registerHandle(&Global2GameModHandle::enterGuildCopymap);
	registerHandle(&Global2GameModHandle::setGMValue);
	registerHandle(&Global2GameModHandle::treasureCombineSuccess);
	registerHandle(&Global2GameModHandle::setForbidUser);
	registerHandle(&Global2GameModHandle::activityConfig);
	registerHandle(&Global2GameModHandle::rankModuleInfo);
	registerHandle(&Global2GameModHandle::giveGiftBagReward);
	registerHandle(&Global2GameModHandle::serverCfg);
	registerHandle(&Global2GameModHandle::realTimePKEnterCopymap);
	registerHandle(&Global2GameModHandle::notifyFirstKillInfo);
	registerHandle(&Global2GameModHandle::userOnlineNotifyFirstKillInfo);
	registerHandle(&Global2GameModHandle::drawHeroScoreReward);
	registerHandle(&Global2GameModHandle::drawHeroRankReward);
	registerHandle(&Global2GameModHandle::setCompensationID);
	registerHandle(&Global2GameModHandle::setOpenServerDay);
	registerHandle(&Global2GameModHandle::synAllMemberGuildLv);
	registerHandle(&Global2GameModHandle::worldBossBuyDecDiamond);
	registerHandle(&Global2GameModHandle::reloginGame);
	registerHandle(&Global2GameModHandle::autoConnectGame);
	registerHandle(&Global2GameModHandle::notifyArenaReward);
	registerHandle(&Global2GameModHandle::gmModifyUserInfo);
	registerHandle(&Global2GameModHandle::notifyMyArenaRank);
	registerHandle(&Global2GameModHandle::reqResetInterceptNum);
	registerHandle(&Global2GameModHandle::clearOne2OneScore);
	registerHandle(&Global2GameModHandle::interceptReward);
	registerHandle(&Global2GameModHandle::quickFinishGuard);
	registerHandle(&Global2GameModHandle::refreshGuardQuality);
	registerHandle(&Global2GameModHandle::notifyBeginGuard);
	registerHandle(&Global2GameModHandle::notifyInterceptResult);
	registerHandle(&Global2GameModHandle::notifyCloseBetaReward);
	registerHandle(&Global2GameModHandle::intoManorFightMap);
	registerHandle(&Global2GameModHandle::changeName);
	registerHandle(&Global2GameModHandle::getExpeditionInfo);
	registerHandle(&Global2GameModHandle::reqExpedition);
	registerHandle(&Global2GameModHandle::reqExpeditionReward);
	registerHandle(&Global2GameModHandle::reqFastExpedition);
	registerHandle(&Global2GameModHandle::cancelExpedition);
	registerHandle(&Global2GameModHandle::reqExpeditionSummary);
	registerHandle(&Global2GameModHandle::loadDevilCalls);
	registerHandle(&Global2GameModHandle::drawDevilCallReward);
	registerHandle(&Global2GameModHandle::devilCall);
	registerHandle(&Global2GameModHandle::devilCallSummary);
	registerHandle(&Global2GameModHandle::captureTreasureFail);
	registerHandle(&Global2GameModHandle::giveLilianCb);
	registerHandle(&Global2GameModHandle::operateActivityReward);
	registerHandle(&Global2GameModHandle::operateActivityLotteryReward);
	registerHandle(&Global2GameModHandle::openWindow);
	registerHandle(&Global2GameModHandle::removeNum);
	registerHandle(&Global2GameModHandle::enterEmeraldDream);
	registerHandle(&Global2GameModHandle::realTimeTeamMatch);
	registerHandle(&Global2GameModHandle::reqDeductMoney);
	registerHandle(&Global2GameModHandle::reqGiveReward);
	registerHandle(&Global2GameModHandle::reqEnterTaskLv);
	registerHandle(&Global2GameModHandle::ReqDrawInviteReward);
	registerHandle(&Global2GameModHandle::ReqDrawUseInviteReward);
	registerHandle(&Global2GameModHandle::reqQualifiedList);
	registerHandle(&Global2GameModHandle::reqWorskip);
	registerHandle(&Global2GameModHandle::reqHeroLeaugeRedPoint);
	registerHandle(&Global2GameModHandle::sendRoomInfo);
	registerHandle(&Global2GameModHandle::notifyAddTeamMember);
	registerHandle(&Global2GameModHandle::reqPropose);
	registerHandle(&Global2GameModHandle::reqAgreeMarry);
	registerHandle(&Global2GameModHandle::reqQuitWedding);
	registerHandle(&Global2GameModHandle::reqReEnterWedding);
	registerHandle(&Global2GameModHandle::reqBuyFireWork);
	registerHandle(&Global2GameModHandle::reqAgreeInvite);
	registerHandle(&Global2GameModHandle::reqTakeCandy);
	registerHandle(&Global2GameModHandle::reqSendWeddingRing);
	registerHandle(&Global2GameModHandle::reqUseFireWork);
	registerHandle(&Global2GameModHandle::reqCoupleInfo);
	registerHandle(&Global2GameModHandle::notifyCoupleMarryCb);
	registerHandle(&Global2GameModHandle::notifyCoupleForceDivorceCb);
	registerHandle(&Global2GameModHandle::reqSynGuildBlackMarketLv);
	registerHandle(&Global2GameModHandle::notifyDiscoverConsume);
	registerHandle(&Global2GameModHandle::reqUseLvlBox);
	registerHandle(&Global2GameModHandle::querynft);
	registerHandle(&Global2GameModHandle::changeNftItem);
	registerHandle(&Global2GameModHandle::nftOfflineQuery);
	registerHandle(&Global2GameModHandle::changeNftItemOffline);
}

Global2GameModHandle::~Global2GameModHandle()
{

}

bool Global2GameModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<Global2GameModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void Global2GameModHandle::reloginGame(TCPComponent * pCom,Global::stReloginGame * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		Athena::logger->error("玩家(id:%d,account:%s)重新登录失败,找不到对象了!",recvCmd->charid(),recvCmd->account().c_str());
		return ;
	}
	//UINT64 mapID = 0x00000000FFFFFFFF & pUser->m_roleInfo.m_mapID;
	CMap * newMap = CMapMgr::getSingleton().getMapByID(NEW_PERSON_MAPID);
	if (!newMap){//找不到所在地图直接丢到新手村
		Athena::logger->error("玩家(id:%d,name:%s,account:%s)重新进入时候找不到新手村",pUser->getID(),pUser->getName(),pUser->getAccount());
		return ;
	}
	pUser->userOnline();
	 if (pUser->intoNewMap(newMap,true)){
	 }
	 else {
	 	 Athena::logger->error("玩家(name:%s,id:%d,account:%s)重新登录失败,找不到对象了!",pUser->getName(),recvCmd->charid(),recvCmd->account().c_str());	
	 }
}

void Global2GameModHandle::autoConnectGame(TCPComponent * pCom,Global::stNotifyGameAutoConnect * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser || !pUser->getMap()){
		Athena::logger->error("玩家(id:%d,account:%s)自动重连失败,找不到对象了!",recvCmd->charid(),recvCmd->account().c_str());
		return ;
	}
	pUser->userOnline();
	CMap * pMap = pUser->getMap();
	
	if (pMap->isCopyMap()){//在副本中才需要重新同步
		//CAreaIdx areaIdx(pUser->getPos());
		//pMap->synAdd(pUser,areaIdx);
		CMap *pNewMap = CMapMgr::getSingleton().getMapByID(NEW_PERSON_MAPID);
		if (pNewMap){
			CMapMgr::getSingleton().changeMap(pUser,pNewMap);
		}
	}
}

void Global2GameModHandle::globalRetRoleInfo(TCPComponent * pCom,Global::stRetRoleInfo * recvCmd)
{
	if (!recvCmd) {
		return ;
	}

	Athena::logger->trace("[进入游戏]收到stRetRoleInfo协议 account=%s charid=%d", recvCmd->account().c_str(), recvCmd->charid());

	Game::stUserLoginGameResult sendGateInfo ;
	sendGateInfo.set_account(recvCmd->account());
	sendGateInfo.set_charid(recvCmd->charid());
	sendGateInfo.set_gameid(g_gameServer->m_svrID);
	sendGateInfo.set_ret(0);
	sendGateInfo.set_type((Game::eLoginGameResultType)recvCmd->eloaddatatype());

	RoleInfoForSave	 forSave;
	if (recvCmd->datasize() < sizeof(RoleInfo)){
		sendGateInfo.set_ret(1);
		g_gameServer->getConnMgr().broadcastByID(recvCmd->gateid(),sendGateInfo);
		return ;
	}

	bcopy(recvCmd->data().data(),&forSave.m_roleInfo,sizeof(RoleInfo));

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		Athena::logger->error("[进入游戏]玩家account=%s已经在游戏区了登录不成功",recvCmd->account().c_str());
		sendGateInfo.set_ret(1);
		g_gameServer->getConnMgr().broadcastByID(recvCmd->gateid(),sendGateInfo);
		return ;
	}

	pUser = ATHENA_NEW GameUser(recvCmd->account().c_str(),recvCmd->gateid(),recvCmd->ip().c_str());
	pUser->setID(forSave.m_roleInfo.m_charID);
	pUser->setName(forSave.m_roleInfo.m_name);
	pUser->m_deviceID = recvCmd->deviceid();
	pUser->m_channelID = recvCmd->channelid();
	pUser->setNetIP(recvCmd->netip().c_str());

	INT32 binSize = recvCmd->datasize() - sizeof(RoleInfo) ;
	if (binSize > 0) {
		bcopy(recvCmd->data().data() + sizeof(RoleInfo),forSave.m_binData,binSize);
	}
	else {
		binSize = 0;
	}


	if (!pUser->initData(forSave.m_roleInfo,forSave.m_binData,binSize)){
		Athena::logger->error("初始化数据失败name=%s,account=%s,id=%d 登录gameserver失败",pUser->getName(),pUser->m_account,pUser->getID());
		sendGateInfo.set_ret(1);
		g_gameServer->getConnMgr().broadcastByID(recvCmd->gateid(),sendGateInfo);
		return;	
	}

	if (!GameUserManager::getSingleton().addUser(pUser)){
#ifdef _THAILAND_VER
		char temp[200];
#else
		char temp[60];
#endif
		bzero(temp,sizeof(temp));
		snprintf(temp,sizeof(temp),"添加到玩家容器里失败(id=%d,name=%s)",pUser->getID(),pUser->getName());
		Athena::logger->trace("%s",temp);
		SAFE_DELETE(pUser);
		sendGateInfo.set_ret(1);
		g_gameServer->getConnMgr().broadcastByID(recvCmd->gateid(),sendGateInfo);
		return ;
	}

	UINT64 mapID = 0x00000000FFFFFFFF & pUser->m_roleInfo.m_mapID;
	CMap * newMap = CMapMgr::getSingleton().getMapByID(mapID);
	if (!newMap){//找不到所在地图直接丢到新手村
		newMap = CMapMgr::getSingleton().getMapByID(NEW_PERSON_MAPID);
	}
	
	if (newMap &&newMap->isCopyMap() && (!((CCopyMap *)newMap)->isExist(pUser->getID()))){//在副本掉线后副本已经关了
		newMap = CMapMgr::getSingleton().getMapByID(NEW_PERSON_MAPID);
	}

	if (!g_gameServer->getConnMgr().broadcastByID(recvCmd->gateid(),sendGateInfo)){
		Athena::logger->error("[进入游戏]向往关发送消息失败account=%s,charID=%d,name=%s",pUser->m_account,pUser->getID(),pUser->getName());
		GameUserManager::getSingleton().removeUser(pUser);
		sendGateInfo.set_ret(1);
		g_gameServer->getConnMgr().broadcastByID(recvCmd->gateid(),sendGateInfo);
		SAFE_DELETE(pUser);
		return ;
	}

	/*MiscProto::stAppStoreCfg retCmd;
	if (g_gameServer->isAppVersion()){
		retCmd.set_openappversion(true);
	}
	else {
		retCmd.set_openappversion(false);
	}

	if (g_gameServer->isSanbox()){
		retCmd.set_issandbox(true);
		Athena::logger->trace("服务器支持沙盒充值!");
	}
	else {
		retCmd.set_issandbox(false);
	}

	if (g_gameServer->isDirectBuy()){
		retCmd.set_directbuy(true);
	}
	else {
		retCmd.set_directbuy(false);
	}
	pUser->sendProto2Me(retCmd);*/
	pUser->userOnline();
	
	 if (pUser->intoNewMap(newMap,true)){
		 Athena::logger->trace("%s,%d 进入地图成功",pUser->getName(),pUser->getID());
		 pUser->synMyInfoToGlobal();
		 pUser->synMyLevel();
		 pUser->synMyBattlePower();
		 pUser->synMyCopymapStarNum();
		 pUser->sendLoginLog();	

		 Athena::logger->trace("玩家name=%s,charID=%d,account=%s登录game成功当前场景有%d人,deviceID=%s",pUser->getName(),pUser->getID(),pUser->getAccount(),GameUserManager::getSingleton().userNum(),pUser->m_deviceID.c_str());
	 }
	 else {
		sendGateInfo.set_ret(1);
		g_gameServer->getConnMgr().broadcastByID(recvCmd->gateid(),sendGateInfo);
		 GameUserManager::getSingleton().removeUser(pUser);
		 Athena::logger->error("(account=%s,name=%s,charID=%d)进入地图失败!",pUser->getAccount(),pUser->getName(),pUser->getID());
		 SAFE_DELETE(pUser);
	 }

}

void Global2GameModHandle::userRecharge(TCPComponent * pCom,Global::stUserRecharge * recvCmd)
{
	if (!recvCmd){
		return ;	
	}	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser) {
		for (UINT16 i = 0;i < recvCmd->rechargeinfo_size(); ++i){
			const Global::stRechargeInfo & info = recvCmd->rechargeinfo(i);
#ifdef _YUNDING_TONGJI
			pUser->m_rmbShop.buy(info.money(),info.itemid(),info.tid(), info.data(), recvCmd->isextend());	
#else
			pUser->m_rmbShop.buy(info.money(),info.itemid(),info.tid());
#endif
		}
	}
}

void Global2GameModHandle::synFetchLilianNum(TCPComponent * pCom,Global::stSynFetchLilianNum * recvCmd)
{
	if (!recvCmd){
		return ;
	}	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		pUser->m_countMgr.addDayRefreshValue(ECOUNT_FETCH_LILIAN_NUM,1);	
	}
}

void Global2GameModHandle::createGuildDecMoney(TCPComponent *pCom,Global::stCreateGuildDecMoney * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		if (pUser->m_eCreateGuildRes == GuildProto::stReqCreateGuild::eDiamond){
			pUser->removeResource(eResource_Diamond,CGuildConfig::getSingleton().createNeedDiamond(),"创建公会");		
		}
		else {
			pUser->removeResource(eResource_GoldCoin,CGuildConfig::getSingleton().createNeedGoldCoin(),"创建公会");
		}
	}
}

void Global2GameModHandle::synMyGuildInfo(TCPComponent *pCom,Global::stSynGuildID * recvCmd)
{
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		pUser->m_guildID = recvCmd->guildid();
		pUser->m_guildLv = recvCmd->guildlv();
		pUser->m_joinInTime = recvCmd->joinintime();
		pUser->m_blackMacketLv = recvCmd->blackmarketlv();

		GuildProto::stGuildDonateState retCmd;
		if (pUser->m_countMgr.getDayRefreshValue(ECOUNT_GUILD_DONATE)) {
			retCmd.set_hasdonate(true);
		}
		else {
			retCmd.set_hasdonate(false);
		}
		pUser->sendProto2Me(retCmd);

		GuildProto::stGuildPrayState retCmd1;
		if (pUser->m_countMgr.getDayRefreshValue(ECOUNT_GUILD_PRAY)) {
			retCmd1.set_haspray(true);
		}
		else {
			retCmd1.set_haspray(false);
		}

		pUser->sendProto2Me(retCmd1);
	}
}

void Global2GameModHandle::guildPray(TCPComponent *pCom,Global::stGuildPray * recvCmd)
{
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		CGuildConfig::stPrayInfo *pPrayInfo = CGuildConfig::getSingleton().getPrayInfo(recvCmd->buildinglv());
		if (!pPrayInfo){
			return ;
		}

		GuildProto::stPrayResult retCmd;
		if (pUser->m_countMgr.getDayRefreshValue(ECOUNT_GUILD_PRAY) >= pPrayInfo->m_maxPrayNum){
			retCmd.set_result(GuildProto::stPrayResult::eHAS_PRAY);	
			pUser->sendProto2Me(retCmd);
			return ;
		}

		pUser->m_countMgr.addDayRefreshValue(ECOUNT_GUILD_PRAY,1);
		retCmd.set_result(GuildProto::stPrayResult::eSUCCESS);	
		pUser->sendProto2Me(retCmd);

		pUser->giveAssert(pPrayInfo->m_item1ID,pPrayInfo->m_item1Num,0,"祈福");
		pUser->giveAssert(pPrayInfo->m_item2ID,pPrayInfo->m_item2Num,0,"祈福");
		pUser->giveAssert(pPrayInfo->m_item3ID,pPrayInfo->m_item3Num,0,"祈福");

		if (recvCmd->extraboxid()){
			pUser->m_boxMgr.openBox(recvCmd->extraboxid(),1);
		}
        stGuildPrayNum guildPrayNumNotify;
        guildPrayNumNotify.notify(pUser, 1);
	}
}

void Global2GameModHandle::treasureCombineSuccess(TCPComponent *pCom,Global::stTreasureCombineSuccess * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}

	pUser->giveAssert(recvCmd->itemid(),1,0,"宝藏合成");
}

void Global2GameModHandle::confirmChgSvr(TCPComponent * pCom,Global::stConfirmChangeGameSvr * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	Athena::logger->trace("name=%s,id=%d,account=%s,换服确认成功",pUser->getName(),pUser->getID(),pUser->getAccount());
	GameUserManager::getSingleton().removeUser(pUser);
	pUser->unreg(Global::SAVE_CHGSVR);
	SAFE_DELETE(pUser);
	
}

void Global2GameModHandle::sendOther2Me(TCPComponent * pCom,Global::stRetOtherUserData2Me * recvCmd)
{
	if (!recvCmd){
		return ;	
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->mycharid());
	if (!pUser){
		return ;
	}
	Role::stUserDetailInfo retCmd;
	retCmd.set_guildname(recvCmd->guildname());
	GameUser * pOther = GameUserManager::getSingleton().getUser(recvCmd->othercharid());
	if (pOther){//如果他在线,则发送
		pOther->fillDetailInfo(&retCmd);
        for (int i = 0; i < retCmd.itemdata_size(); i++) {
            pOther->synEnchant((Role::stSynItemInfo *)&retCmd.itemdata(i));
        }
        retCmd.set_couplename(pOther->getCoupleName());
		if(pOther->m_mountMgr.isRiding()){
			retCmd.set_mountid(pOther->m_mountMgr.getBattleMountID());
		}
		pUser->sendProto2Me(retCmd);
	}
	else if (recvCmd->isrobot()){
		stRobotInfo * pRobotInfo = CRobotMgr::getSingleton().getRobot(recvCmd->othercharid());	
		if (pRobotInfo && pRobotInfo->m_pUser){
			pRobotInfo->m_pUser->fillDetailInfo(&retCmd);
			pUser->sendProto2Me(retCmd);
		}
	}
	else {
		RoleInfoForSave	 forSave;
		bcopy(recvCmd->data().data(),&forSave.m_roleInfo,sizeof(RoleInfo));
		pOther = ATHENA_NEW GameUser("",0,"");

		INT32 binSize = recvCmd->datasize() - sizeof(RoleInfo) ;
		if (binSize > 0) {
			bcopy(recvCmd->data().data() + sizeof(RoleInfo),forSave.m_binData,binSize);
		}
		else {
			binSize = 0;
		}
		pOther->initData(forSave.m_roleInfo,forSave.m_binData,binSize);
		pOther->setName(forSave.m_roleInfo.m_name);
		pOther->fillDetailInfo(&retCmd);
        retCmd.set_couplename(forSave.m_roleInfo.m_coupleName);
        for (int i = 0; i < retCmd.itemdata_size(); i++) {
            pOther->synEnchant((Role::stSynItemInfo *)&retCmd.itemdata(i));
        }

		if(pOther->m_mountMgr.isRiding()){
			retCmd.set_mountid(pOther->m_mountMgr.getBattleMountID());
		}
		pUser->sendProto2Me(retCmd);
		SAFE_DELETE(pOther);
	}
}

void Global2GameModHandle::obtainMailAttach(TCPComponent * pCom,Global::stObtainItemFromMail * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}

	for (UINT16 i = 0;i < recvCmd->attachs_size();++i){
		const Global::stMailAttach	& attach = recvCmd->attachs(i);
		pUser->giveAssert(attach.itemid(),attach.num(),0,"提取附件");
	}
}

void Global2GameModHandle::pkWithOther(TCPComponent * pCom,Global::stBeginPKWithOther * recvCmd)
{
	if (!recvCmd){
		return;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->mycharid());
	if (!pUser){
		return ;
	}

	GameUser* pRobot = NULL;
	Role::stSendPKUserData retCmd;
	retCmd.set_pktype((Role::stSendPKUserData_ePKType)recvCmd->type());
	retCmd.set_challengerank(recvCmd->challengerank());
	GameUser * pOther = GameUserManager::getSingleton().getUser(recvCmd->othercharid());
	if (pOther){//如果他在线,则发送
		pOther->fillMainData(retCmd.mutable_maindata());
		pOther->m_bagMgr.m_equipBag.fillMyEquip(&retCmd);
        for (int i = 0; i < retCmd.itemdata_size(); i++) {
            pOther->synEnchant((Role::stSynItemInfo *)&retCmd.itemdata(i));
        }
		pOther->m_heroMgr.fillMyHero(&retCmd);
		pUser->sendProto2Me(retCmd);

		if (Athena::global["checkPvP"] == "true")
		{
			pRobot = ATHENA_NEW GameUser("",0,"");

			RoleInfoForSave  forSave;
			bzero(&forSave,sizeof(forSave));
			bcopy(&pOther->m_roleInfo,&forSave.m_roleInfo,sizeof(pOther->m_roleInfo));
			UINT32 binSize = pOther->compressSaveData(forSave.m_binData);

			pRobot->initData(forSave.m_roleInfo,forSave.m_binData,binSize);
			pRobot->setName(forSave.m_roleInfo.m_name);
			pRobot->setID(forSave.m_roleInfo.m_charID & 0x00002FFF); //客户端根据ID范围判断是非机器人
			pRobot->setupRoleState(true);
		}
	}
	else if (recvCmd->robotid() > 0){//是机器人
		stRobotInfo * pRobotInfo = CRobotMgr::getSingleton().getRobot(recvCmd->robotid());	
		if (pRobotInfo && pRobotInfo->m_pUser){
			pRobotInfo->m_pUser->fillMainData(retCmd.mutable_maindata());
			pRobotInfo->m_pUser->m_bagMgr.m_equipBag.fillMyEquip(&retCmd);
			pRobotInfo->m_pUser->m_heroMgr.fillMyHero(&retCmd);
			pUser->sendProto2Me(retCmd);

			if (Athena::global["checkPvP"] == "true")
			{
				pRobot = ATHENA_NEW GameUser("",0,"");

				RoleInfoForSave  forSave;
				bzero(&forSave,sizeof(forSave));
				bcopy(&pRobotInfo->m_pUser->m_roleInfo,&forSave.m_roleInfo,sizeof(pRobotInfo->m_pUser->m_roleInfo));
				UINT32 binSize = pRobotInfo->m_pUser->compressSaveData(forSave.m_binData);

				pRobot->initData(forSave.m_roleInfo,forSave.m_binData,binSize);
				pRobot->setName(pRobotInfo->m_pUser->getName());
				pRobot->setID(pRobotInfo->m_pUser->getID());
				pRobot->setupRoleState(true);
			}
		}
	}
	else {
		if (recvCmd->datasize() < sizeof(RoleInfo)){
			Athena::logger->error("(name:%s,id:%d)挑战镜像出现错误,2进制数据太短不合理",pUser->getName(),pUser->getID());
			return ;
		}

		RoleInfoForSave	 forSave;
		bcopy(recvCmd->data().data(),&forSave.m_roleInfo,sizeof(RoleInfo));
		pOther = ATHENA_NEW GameUser("",0,"");

		INT32 binSize = recvCmd->datasize() - sizeof(RoleInfo) ;
		if (binSize > 0) {
			bcopy(recvCmd->data().data() + sizeof(RoleInfo),forSave.m_binData,binSize);
		}
		else {
			binSize = 0;
		}
		pOther->initData(forSave.m_roleInfo,forSave.m_binData,binSize);
		pOther->setName(forSave.m_roleInfo.m_name);
		pOther->setID(forSave.m_roleInfo.m_charID);
		pOther->setupRoleState(true);
		pOther->fillMainData(retCmd.mutable_maindata());
		pOther->m_bagMgr.m_equipBag.fillMyEquip(&retCmd);
        for (int i = 0; i < retCmd.itemdata_size(); i++) {
            pOther->synEnchant((Role::stSynItemInfo *)&retCmd.itemdata(i));
        }
		pOther->m_heroMgr.fillMyHero(&retCmd);
		pUser->sendProto2Me(retCmd);

		if (Athena::global["checkPvP"] == "true"){
			pRobot = pOther;
			pRobot->setID(forSave.m_roleInfo.m_charID & 0x00002FFF); //客户端根据ID范围判断是非机器人
		}else{
			SAFE_DELETE(pOther);
		}
	}

	if (Global::stBeginPKWithOther::eArena_PK == recvCmd->type()) {
		pUser->m_levelMgr.arenaChallengeRank(recvCmd->challengerank());
		if (Athena::global["checkPvP"] == "true")
		{
			pUser->m_levelMgr.enterArenaLv(ARENA_PK_LEVELID_SERVERCHECK, pRobot);
		}else{
			pUser->m_levelMgr.enterArenaLv(ARENA_PK_LEVELID, NULL);
		}
	}
	else if (Global::stBeginPKWithOther::eIntercept_PK == recvCmd->type()){
		
		if (Athena::global["checkPvP"] == "true")
		{
			pUser->m_levelMgr.enterInterceptLv_ServerCheck(INTERCEPT_LEVELID_SERVERCHECK, pRobot, recvCmd->challengerank());
		}else{
			pUser->m_levelMgr.enterInterceptLv(INTERCEPT_LEVELID);	
		}	
	}
	else {
		//pUser->m_levelMgr.enterLeitaiLv(pUser->m_leitaiMgr.getPKMode());
		pUser->m_levelMgr.setLeitaiCharID(recvCmd->othercharid());
		pUser->m_levelMgr.setLeitaiRobot(pRobot);
		pUser->m_leitaiMgr.setChallengeCharID(recvCmd->othercharid());
#ifdef _HDZ_DEBUG
		Athena::logger->trace("擂台发送了玩家:%d",pUser->getID());
#endif
	}
}

void Global2GameModHandle::userLeaveGuild(TCPComponent * pCom,Global::stUserLeaveGuild * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	for (UINT16 i = 0;i < recvCmd->charids_size();++i){
		GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charids(i));
		if (pUser){
			pUser->m_roleInfo.m_leaveGuildTime = g_gameServer->getSec();
			pUser->m_guildID = 0;
			pUser->m_guildLv = 0;
		}
	}
}

void Global2GameModHandle::teamEnterCopymap(TCPComponent * pCom,Global::stTeamEnterCopyMap * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(recvCmd->copymapid());
	if (!pCopyMapBase){
		Athena::logger->error("[组队进入副本]找不到关卡配置!");
		return ;
	}

	if (recvCmd->memerinfo_size() <= 0){//开始游戏失败,连队长都没有
		Athena::logger->error("[组队进入副本]进入副本失败,连队长都没有!");
		return ;
	}
	
	UINT32 needRobotNum = 0;
	
	if (pCopyMapBase->m_type == eLevelType_Team || pCopyMapBase->m_type == eLevelType_World_Boss){
		needRobotNum = recvCmd->memerinfo_size() < 3 ? 3 - recvCmd->memerinfo_size() : 0; 
	}

	const Global::stEnterCopymapMemInfo & leaderInfo = recvCmd->memerinfo(0);

	GameUser * pUser = GameUserManager::getSingleton().getUser(leaderInfo.charid());
	if (!pUser){
		Athena::logger->error("[组队进入副本]进入副本失败,队长已经不在线了");
		return ;	
	}

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser->getName(),recvCmd->copymapid());
	if (!pCopyMap){
		Athena::logger->error("[组队进入副本]进入副本失败,创建副本不成功!");
		return;
	}  
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->setBossHP(recvCmd->bosshp());
	pCopyMap->setBossLv(recvCmd->bosslv());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("设置血量为:%d",recvCmd->bosshp());
#endif
	UINT16 posID = 1;
	for (UINT16 i = 0;i < recvCmd->memerinfo_size(); ++i){
		const Global::stEnterCopymapMemInfo & memInfo = recvCmd->memerinfo(i);
		pUser = GameUserManager::getSingleton().getUser(memInfo.charid());
		if (pUser){
			if (pCopyMapBase->m_type == eLevelType_Team) {
				if (pUser->m_levelMgr.getTeamCopymapNum() >= pUser->getCountByVip(eVipFun_Team_Copymap)){//组队进入失败,次数用完了
					Athena::logger->error("玩家(name:%s,id:%d)组队进入副本失败:",pUser->getName(),pUser->getID());
					continue;
				}
			}
			
			if (!pUser->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
				Athena::logger->error("[组队进入副本]玩家(name:%s,id:%d)历练不足进入组队副本失败!",pUser->getName(),pUser->getID());
				continue;
			}

			pUser->setCampID(Map::eCamp_Red);
			pCopyMap->joinCamp(Map::eCamp_Red,pUser->getID());
			pUser->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
			pUser->resetBattleProp();
			
			CMapMgr::getSingleton().changeMap(pUser,pCopyMap, posID++);
			pUser->setDamagePer(memInfo.adddamageper());

			if (0 == i){
				/*Map::stNotifyExecAI cmd;
				cmd.set_charid(pUser->getID());
				pUser->sendProto2Me(cmd);*/
				//pCopyMap->notifyExector();
			}
		}
	}

	for (UINT16 i = 0;i < needRobotNum;++i){
		stRobotInfo * pRobot = CRobotMgr::getSingleton().getRobotByLevelID(recvCmd->copymapid(),i + 1);
		if (pRobot && pRobot->m_pUser) {
			pRobot->m_pUser->setCampID(Map::eCamp_Red);
			pCopyMap->joinCamp(Map::eCamp_Red,pRobot->m_pUser->getID());
			pRobot->m_pUser->resetBattleProp();
			CRobotMgr::getSingleton().useRobot(pRobot->m_pUser->getID(),pRobot);
			CMapMgr::getSingleton().robotChangeMap(pRobot->m_pUser,pCopyMap,posID++);
			//CRobotMgr::getSingleton().useRobot(pRobot->m_pUser->getID(),pRobot);
		}
		else {
			Athena::logger->trace("放机器人时候,没有机器人了!");
		}
	}
	pCopyMap->notifyExector();
}

void Global2GameModHandle::one2OnePKWithRobot(TCPComponent * pCom,Global::stOne2OnePkWithRobot * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stCopyMapBase * pCopyMapBase = NULL;
	pCopyMapBase = g_copyMapBaseMgr.get(ONE2ONE_PVP_MAPID);
	if (!pCopyMapBase){
		Athena::logger->error("[和机器人巅峰对决]找不到关卡配置!");
		return ;
	}

	GameUser * pUser1 = GameUserManager::getSingleton().getUser(recvCmd->charid());
	
	if (!pUser1){
		Athena::logger->error("[和机器人巅峰对决]有一方已经不在线了,进入失败!");
		return ;
	}
	
	if (!pUser1->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
		Athena::logger->error("[和机器人巅峰对决]玩家(name:%s,id:%d)历练不足进入1v1失败!",pUser1->getName(),pUser1->getID());
		return;
	}

	if (pUser1->getMap() && pUser1->getMap()->isCopyMap() && (!((CCopyMap *)pUser1->getMap())->isGuildHall())){
		Athena::logger->error("[和机器人巅峰对决]被邀请一方已经在副本了");
		return ;
	}
	
	pUser1->setCampID(Map::eCamp_Red);
	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser1->getName(),pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[和机器人巅峰对决]进入副本失败,创建副本不成功!");
		return;
	}
	
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);
	
	pCopyMap->joinCamp(Map::eCamp_Red,pUser1->getID());

	pUser1->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	pUser1->resetBattleProp();
	CMapMgr::getSingleton().changeMap(pUser1,pCopyMap,1);

	stRobotInfo * pRobot = CRobotMgr::getSingleton().getOne2OneRobot(ONE2ONE_PVP_MAPID,recvCmd->robotgroup());
	if (pRobot && pRobot->m_pUser) {
		pRobot->m_pUser->setCampID(Map::eCamp_Black);
		//pCopyMap->joinCamp(Map::eCamp_Red,pRobot->m_pUser->getID());
		pRobot->m_pUser->resetBattleProp();
		CRobotMgr::getSingleton().useRobot(pRobot->m_pUser->getID(),pRobot);
		pCopyMap->joinCamp(Map::eCamp_Black,pRobot->m_pUser->getID());
		CMapMgr::getSingleton().robotChangeMap(pRobot->m_pUser,pCopyMap,2);
	}
	else {
		Athena::logger->trace("1v1 放机器人时候,没有机器人了!");
	}
}

void Global2GameModHandle::one2OnePVPEnterCopymap(TCPComponent * pCom,Global::stOne2OnePVPEnterCopyMap * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stCopyMapBase * pCopyMapBase = NULL;
	
	if (recvCmd->pkwithfriend()){
		pCopyMapBase = g_copyMapBaseMgr.get(PKWITH_FRIEND_MAPID);
		//pCopyMapBase = g_copyMapBaseMgr.get(ONE2ONE_PVP_MAPID);
	}
	else {
		pCopyMapBase = g_copyMapBaseMgr.get(ONE2ONE_PVP_MAPID);
	}

	if (!pCopyMapBase){
		Athena::logger->error("[巅峰对决]找不到关卡配置!");
		return ;
	}

	if (recvCmd->memberids_size() < 2){
		Athena::logger->error("[巅峰对决]人数不足两人,进入失败!");
		return ;
	}
	
	GameUser * pUser1 = GameUserManager::getSingleton().getUser(recvCmd->memberids(0));
	GameUser * pUser2 = GameUserManager::getSingleton().getUser(recvCmd->memberids(1));
	
	if (!pUser1 || !pUser2){
		Athena::logger->error("[巅峰对决]有一方已经不在线了,进入失败!");
		return ;
	}

	if (pUser1 == pUser2){
		Athena::logger->error("[巅峰约战]是同一个玩家!");
		return ;
	}
	
	if (!recvCmd->pkwithfriend()) {
		if (!pUser1->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
			Athena::logger->error("玩家(name:%s,id:%d)历练不足进入1v1失败!",pUser1->getName(),pUser1->getID());
			return;
		}
		
		if (!pUser2->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
			Athena::logger->error("玩家(name:%s,id:%d)历练不足进入1v1失败!",pUser2->getName(),pUser2->getID());
			return;
		}
	}

	if (pUser1->getMap() && pUser1->getMap()->isCopyMap() && (!((CCopyMap *)pUser1->getMap())->isGuildHall())){
		Athena::logger->error("[巅峰约战]被邀请一方已经在副本了");
		return ;
	}
	
	if (pUser2->m_levelMgr.isInLv()){
		Athena::logger->error("[巅峰约战]发起邀请一方已经pve场景");
		FriendProto::stReplyFriendPKInviteResult retCmd;
		retCmd.set_result(FriendProto::stReplyFriendPKInviteResult::eNot_Invited_Has_Enter);
		pUser1->sendProto2Me(retCmd);
		return ;
	}

	if (pUser2->getMap() && pUser2->getMap()->isCopyMap() && (!((CCopyMap *)pUser2->getMap())->isGuildHall())){
		Athena::logger->error("[巅峰约战]发起邀请一方已经在副本了");
		FriendProto::stReplyFriendPKInviteResult retCmd;
		retCmd.set_result(FriendProto::stReplyFriendPKInviteResult::eNot_Invited_Has_Enter);
		pUser1->sendProto2Me(retCmd);
		return ;
	}

	pUser1->setCampID(Map::eCamp_Red);
	pUser2->setCampID(Map::eCamp_Black);

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser1->getName(),pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[巅峰对决]进入副本失败,创建副本不成功!");
		return;
	}
	
	if (recvCmd->pkwithfriend()){
		pCopyMap->setPKWithFriend();
	}

	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);
	
	pCopyMap->joinCamp(Map::eCamp_Red,pUser1->getID());
	pCopyMap->joinCamp(Map::eCamp_Black,pUser2->getID());

	pUser1->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	//pUser1->resetHP();
	pUser1->resetBattleProp();
	pUser2->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	//pUser2->resetHP();
	pUser2->resetBattleProp();
	CMapMgr::getSingleton().changeMap(pUser1,pCopyMap,1);
	CMapMgr::getSingleton().changeMap(pUser2,pCopyMap,2);
}

void Global2GameModHandle::captureTreasureReward(TCPComponent * pCom,Global::stCaptureReward * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	MiscProto::stCaptureUserTreasureResult retCmd;
	if (recvCmd->multicapture()){
		retCmd.set_type(MiscProto::eFiveTime_Capture);	
	}
	else {
		retCmd.set_type(MiscProto::eOneTime_Capture);
	}

	if (!pUser->removeResource(eResource_Lilian,Capture_Need_Lilian * recvCmd->num(),"夺宝")){
		retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eSP_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
		return ;
	} 

	retCmd.set_obtainpiece(recvCmd->obtainpiece());
	retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eSUCCESS);
	
	UINT32 exp = pUser->getLevel() * recvCmd->expcoef();
	UINT32 money = pUser->getLevel() * recvCmd->moneycoef();

	std::vector<CDropMgr::stDropEntry> dropVec;
	CDropMgr::getSingleton().processDropGroup(recvCmd->dropid(),dropVec,recvCmd->num());

	for (UINT8 i = 0;i < dropVec.size();++i){
		MiscProto::stSynCaptureReward * pSyn = retCmd.add_rewards();
		if (pSyn){
			CDropMgr::stDropEntry & dropInfo = dropVec[i];
			pSyn->set_exp(exp);
			pSyn->set_goldcoin(money);
			pSyn->set_itemid(dropInfo.m_itemID);
			pSyn->set_num(dropInfo.m_num);
		}
	}	
	
	pUser->addExp(exp * dropVec.size());
	pUser->addResource(eResource_GoldCoin,money * dropVec.size(),"夺宝");
	for (UINT8 i = 0;i < dropVec.size();++i){
		pUser->m_bagMgr.addItem2Bag(dropVec[i].m_itemID,dropVec[i].m_num,0,"夺宝");
	}
	pUser->sendProto2Me(retCmd);

	stCaptureTreasureNum gameEvent;
	gameEvent.notify(pUser,recvCmd->num());
}

void Global2GameModHandle::captureTreasureFail(TCPComponent * pCom,Global::stCaptureTreasureFail * recvCmd)
{
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	
	MiscProto::stCaptureUserTreasureResult retCmd;
	retCmd.set_obtainpiece(false);

	if (!pUser->removeResource(eResource_Lilian,Capture_Need_Lilian,"夺宝")){
		retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eSP_NOT_ENOUGH);
		pUser->sendProto2Me(retCmd);
		return ;
	} 
	stCaptureTreasureNum gameEvent;
	gameEvent.notify(pUser,1);
}

void Global2GameModHandle::realTimePKEnterCopymap(TCPComponent * pCom,Global::stRealTimeBattleEnterCopymap * recvCmd)
{
	if (!recvCmd){
		return ;
	}
		
	stCopyMapBase * pCopyMapBase = NULL;
	
	if (recvCmd->pktype() == Global::eRealTimePKType_KillAll) {
		pCopyMapBase = g_copyMapBaseMgr.get(REALTIME_PVP_MAPID);
	}
	else {
		pCopyMapBase = g_copyMapBaseMgr.get(REALTIME_MOBA_MAPID);
	}

	if (!pCopyMapBase){
		Athena::logger->error("[实时战场]找不到关卡配置!");
		return ;
	}
	
	std::vector<GameUser *> teamAUserVec;
	std::vector<GameUser *> teamBUserVec;

	for (UINT8 i = 0;i < recvCmd->teamamembers_size();++i){
		if (recvCmd->teamamembers(i) == 0)
		{
			teamAUserVec.push_back(NULL);
		}
		else
		{
			GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->teamamembers(i));
			if (pUser){
				teamAUserVec.push_back(pUser);			
			}
			else {
				Athena::logger->error("[实时战场]A队伍有人已经下线了,进入失败!");
				return ;
			}
		}
	}
	
	for (UINT8 i = 0;i < recvCmd->teambmembers_size();++i){
		if (recvCmd->teambmembers(i) == 0)
		{
			teamBUserVec.push_back(NULL);	
		}
		else
		{
			GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->teambmembers(i));
			if (pUser){
				teamBUserVec.push_back(pUser);			
			}
			else {
				Athena::logger->error("[实时战场]B队伍有人已经下线了,进入失败!");
				return ;
			}
		}
	}
	
	if ((teamAUserVec.size() <= 0) || !teamAUserVec[0]){
		Athena::logger->error("[实时战场]A队伍的队长不在线了,进入失败!");
		return ;
	}

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(teamAUserVec[0]->getName(),pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[实时战场]进入副本失败,创建副本不成功!");
		return;
	}
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);

	for (UINT8 i = 0;i < teamAUserVec.size();++i){
		if (teamAUserVec[i]) {
			if (teamAUserVec[i]->m_countMgr.getDayRefreshValue(ECOUNT_REAL_TIME_NUM) >= teamAUserVec[i]->getCountByVip(eVipFun_BattleGround)){
				Athena::logger->error("玩家(name:%s,id:%d)次数已经用尽,不能进入战场",teamAUserVec[i]->getName(),teamAUserVec[i]->getID());
				continue;
			}

			if (!teamAUserVec[i]->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
				Athena::logger->error("玩家(name:%s,id:%d),历练不足进入实时战场失败",teamAUserVec[i]->getName(),teamAUserVec[i]->getID());
				continue;
			}
			teamAUserVec[i]->setCampID(Map::eCamp_Red);
			pCopyMap->joinCamp(Map::eCamp_Red,teamAUserVec[i]->getID());
			teamAUserVec[i]->resetBattleProp();
			CMapMgr::getSingleton().changeMap(teamAUserVec[i],pCopyMap,i + 1);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("玩家:%s进入了实时战场加入了:campID:%d",teamAUserVec[i]->getName(),Map::eCamp_Red);
#endif
		}else{
			stRobotInfo * pRobot = CRobotMgr::getSingleton().getRobotByLevelID(pCopyMapBase->m_id, 1);
			if (pRobot && pRobot->m_pUser) {
				pRobot->m_pUser->setCampID(Map::eCamp_Red);
				pCopyMap->joinCamp(Map::eCamp_Red,pRobot->m_pUser->getID());
				pRobot->m_pUser->resetBattleProp();
				CRobotMgr::getSingleton().useRobot(pRobot->m_pUser->getID(),pRobot);
				CMapMgr::getSingleton().robotChangeMap(pRobot->m_pUser,pCopyMap, i+1);
			}
			else {
				Athena::logger->trace("放机器人时候,没有机器人了!");
			}
		}
	}
	
	for (UINT8 i = 0;i < teamBUserVec.size();++i){
		if (teamBUserVec[i]) {
			if (teamBUserVec[i]->m_countMgr.getDayRefreshValue(ECOUNT_REAL_TIME_NUM) >= teamBUserVec[i]->getCountByVip(eVipFun_BattleGround)){
				Athena::logger->error("玩家(name:%s,id:%d)次数已经用尽,不能进入战场",teamBUserVec[i]->getName(),teamBUserVec[i]->getID());
				continue;
			}

			if (!teamBUserVec[i]->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
				Athena::logger->error("玩家(name:%s,id:%d)历练不足进入实时战场失败!",teamBUserVec[i]->getName(),teamBUserVec[i]->getID());
				continue;
			}
			teamBUserVec[i]->setCampID(Map::eCamp_Black);
			pCopyMap->joinCamp(Map::eCamp_Black,teamBUserVec[i]->getID());
			teamBUserVec[i]->resetBattleProp();
			CMapMgr::getSingleton().changeMap(teamBUserVec[i],pCopyMap,i + 4);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("玩家:%s进入了实时战场加入了:campID:%d",teamBUserVec[i]->getName(),Map::eCamp_Black);
#endif
		}else{
			stRobotInfo * pRobot = CRobotMgr::getSingleton().getRobotByLevelID(pCopyMapBase->m_id, 2);
			if (pRobot && pRobot->m_pUser) {
				pRobot->m_pUser->setCampID(Map::eCamp_Black);
				pCopyMap->joinCamp(Map::eCamp_Black,pRobot->m_pUser->getID());
				pRobot->m_pUser->resetBattleProp();
				CRobotMgr::getSingleton().useRobot(pRobot->m_pUser->getID(),pRobot);
				CMapMgr::getSingleton().robotChangeMap(pRobot->m_pUser,pCopyMap, i+4);
			}
			else {
				Athena::logger->trace("放机器人时候,没有机器人了!");
			}
		}
	}
}

void Global2GameModHandle::notifyAddLilian(TCPComponent * pCom,Global::stNotifyAddLilian * recvCmd)
{
	if (!recvCmd){
		return ;
	}	
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}

	pUser->addResource(eResource_Lilian,recvCmd->num(),"好友赠送");
	pUser->m_countMgr.addDayRefreshValue(ECOUNT_FETCH_LILIAN_NUM,recvCmd->num() / GIVE_LILIAN_NUM_PER_TIME);
}

void Global2GameModHandle::enterGuildCopymap(TCPComponent * pCom,Global::stEnterGuildCopymap * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		Athena::logger->trace("玩家进入副本失败,玩家已经下线了!");
		return ;
	}

	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(GUILD_MAP_ID);
	if (!pCopyMapBase){
		Athena::logger->error("[进入公会领地]找不到关卡配置!");
		return ;
	}
	
	char mapName[125] = { 0 };
	snprintf(mapName,sizeof(mapName),"%d%s",pCopyMapBase->m_id,recvCmd->guildname().c_str());

	CMap * pMap = CMapMgr::getSingleton().getMapByName(mapName);
	if (!pMap){//不存在地图的情况
		pMap = CMapMgr::getSingleton().createCopyMap(recvCmd->guildname().c_str(),GUILD_MAP_ID); 
	}

	if (!pMap){
		Athena::logger->error("进入公会副本失败,找不到副本对象!");
		return ;
	}

	pUser->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	CMapMgr::getSingleton().changeMap(pUser,pMap);
}

void Global2GameModHandle::setGMValue(TCPComponent * pCom,Global::stOnlineUserGMType * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		pUser->m_roleInfo.m_gm = recvCmd->gmvalue();
	}
}

void Global2GameModHandle::setForbidUser(TCPComponent * pCom,Global::stForbidOnlineUser * recvCmd)
{
	if (!recvCmd){
		return ;
	}	

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		pUser->m_roleInfo.m_bitmask |= BITMASK_FORBID_LOGIN;

		Game::stKillOutUser cmd;
		cmd.set_charid(pUser->getID());
		pUser->sendProto2Me(cmd);
	}
}

void Global2GameModHandle::activityConfig(TCPComponent * pCom,Global::stSynActivityConfig * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	Athena::logger->trace("收到了来自global的活动配置");
	for (UINT16 i = 0;i < recvCmd->data_size();++i){
		const Global::stActivityConfig & activity = recvCmd->data(i);
		Athena::logger->trace("id:%d,config:%s",activity.activityid(),activity.cofing().c_str());
		ActivityCfgMgr::getSingleton().init(activity.cofing());
	}
	GameUserManager::getSingleton().checkAcceptNewActivity();
}
	
void Global2GameModHandle::rankModuleInfo(TCPComponent * pCom,Global::stSynRankModuleInfo * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	CHistoryRankMgr::getSingleton().initHistoryRankInfo(recvCmd);
	Athena::logger->trace("收到了来自global的排名模块信息");
}

void Global2GameModHandle::giveGiftBagReward(TCPComponent * pCom,Global::stGiveGiftBagReward * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		Role::stObtainItemTip retCmd;
		for (UINT16 i = 0;i < recvCmd->rewards_size();++i){
			const Global::stGiftBagReward  & reward = recvCmd->rewards(i);
			pUser->giveAssert(reward.itemid(),reward.num(),0,"礼包奖励");
			Role::stSynTipItem * pSyn = retCmd.add_getitems();
			if (pSyn){
				pSyn->set_itemid(CItem::realItemIDByJobAndSex(reward.itemid(),pUser->getJob(),pUser->getSex()));
				pSyn->set_num(reward.num());
			}
		}
		pUser->sendProto2Me(retCmd);
	}
}

void Global2GameModHandle::serverCfg(TCPComponent * pCom,Global::stNotifyGameSvrCfg * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	for (UINT16 i = 0;i < recvCmd->cfgpair_size();++i){
		const Global::stServerCfgPair  cfgPair = recvCmd->cfgpair(i);
		std::string key = cfgPair.key();
		Athena::global[key] = cfgPair.value();
		if (key == "loadStat"){
			g_gameServer->initLoadStat(Athena::global);
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("收到服务器配置:key=%s,value=%s",cfgPair.key().c_str(),cfgPair.value().c_str());
#endif
	} 

	Athena::logger->trace("收到了global通知的服务器配置!");
}

void Global2GameModHandle::userOnlineNotifyFirstKillInfo(TCPComponent * pCom,Global::stUserOnlineSendFirstKillInfo * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	for (UINT16 i = 0;i < recvCmd->copymapid_size();++i){
		pUser->m_titleMgr.checkObtainTitle(eTitleGotType_FirstKill,recvCmd->copymapid(i),false,false);
	}
}

void Global2GameModHandle::notifyFirstKillInfo(TCPComponent * pCom,Global::stTeamFinishFirstKill * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid1());
	if (pUser){
		pUser->m_titleMgr.checkObtainTitle(eTitleGotType_FirstKill,recvCmd->copymapid(),false);
	}
	
	pUser = GameUserManager::getSingleton().getUser(recvCmd->charid2());
	if (pUser){
		pUser->m_titleMgr.checkObtainTitle(eTitleGotType_FirstKill,recvCmd->copymapid(),false);
	}

	pUser = GameUserManager::getSingleton().getUser(recvCmd->charid3());
	if (pUser)
	{
		pUser->m_titleMgr.checkObtainTitle(eTitleGotType_FirstKill,recvCmd->copymapid(),false);
	}
}

void Global2GameModHandle::drawHeroScoreReward(TCPComponent * pCom,Global::stDrawHeroScoreReward * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->m_boxMgr.openBox(recvCmd->boxid(),1);
}

void Global2GameModHandle::drawHeroRankReward(TCPComponent * pCom,Global::stDrawHeroRankReward * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->m_boxMgr.openBox(recvCmd->boxid(),1);
}

void Global2GameModHandle::setCompensationID(TCPComponent * pCom,Global::stSetCompensationID * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->m_compensationID = recvCmd->compensationid();

	for (UINT16 i = 0;i < recvCmd->items_size();++i){
		const Global::stCompensantionItem & one = recvCmd->items(i);
		pUser->giveAssert(one.itemid(),one.num(),0,"系统补偿");
	}
}

void Global2GameModHandle::setOpenServerDay(TCPComponent * pCom,Global::stSetOpenServerDay * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	g_gameServer->setOpenServerDay(recvCmd->day());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到开服时间设置:%d",g_gameServer->getOpenServerDay());
#endif
}

void Global2GameModHandle::synAllMemberGuildLv(TCPComponent * pCom,Global::stSynGuildLv2AllMember * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	for (UINT16 i = 0;i < recvCmd->members_size();++i){
		GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->members(i));
		if (!pUser){
			continue;
		}
		pUser->m_guildLv = recvCmd->guildlv(); 
	}
}

void Global2GameModHandle::worldBossBuyDecDiamond(TCPComponent * pCom,Global::stWorldBossBuyDecDiamond * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	
	MiscProto::stRaiseDamageInWorldBossResult retCmd;
	
	UINT32 needDiamond = CMiscCfgMgr::getSingleton().getWorldBossPriceByTime(recvCmd->hasbuynum() + 1);
		
	if (!pUser->removeResource(eResource_Diamond,needDiamond,"世界boss士气鼓舞")){
		retCmd.set_result(MiscProto::stRaiseDamageInWorldBossResult::eDaimond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;    
	}   

	retCmd.set_result(MiscProto::stRaiseDamageInWorldBossResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	Global::stSetWorldBossBuyNum cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_buynum(recvCmd->hasbuynum() + 1);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::notifyArenaReward(TCPComponent * pCom,Global::stNotifyArenaReward * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->addResource((eResource)recvCmd->restype(),recvCmd->resnum(),"竞技场历史排名奖励");
}

void Global2GameModHandle::gmModifyUserInfo(TCPComponent * pCom,Global::stGMModifyUserInfo * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
#ifdef _HDZ_DEBUG
    Athena::logger->trace("[gm在线扣除前]gold is %d, diamond is %d, give diamond is %d, vip point is %d",
            pUser->m_roleInfo.m_goldCoin, pUser->m_roleInfo.m_diamond, pUser->m_roleInfo.m_giveDiamond, pUser->m_roleInfo.m_rechargeNum);
#endif
	pUser->m_roleInfo.m_newBieGuideID = recvCmd->newbieguideid();
    if (recvCmd->goldcoin()) {
        UINT32 oldVal = pUser->m_roleInfo.m_goldCoin;
        if (recvCmd->goldcoin() >= oldVal) {
            pUser->m_roleInfo.m_goldCoin = 0;
        } else {
            pUser->m_roleInfo.m_goldCoin -= recvCmd->goldcoin();
        }
        UINT32 newVal = pUser->m_roleInfo.m_goldCoin;

        if (oldVal != newVal) {
            Game::stResourceChgLog webCmd;
            webCmd.set_account(pUser->m_roleInfo.m_account);
            webCmd.set_charid(pUser->getID());
            webCmd.set_name(pUser->getName());
            webCmd.set_oldvalue(oldVal);
            webCmd.set_newvalue(newVal);
            webCmd.set_resid(eResource_GoldCoin);
            webCmd.set_reason("gm在线扣除金币");
            g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

            Role::stSynResource retCmd;
            retCmd.set_restype(eResource_GoldCoin);
            retCmd.set_num(newVal);
            pUser->sendProto2Me(retCmd);
        }
    }

    if (recvCmd->diamond()) {
        UINT32 decGiveDiamond = 0, decDiamond = 0;
        UINT32 oldVal = pUser->m_roleInfo.m_diamond + pUser->m_roleInfo.m_giveDiamond;
        if (recvCmd->diamond() >= oldVal) {
            decDiamond = pUser->m_roleInfo.m_diamond;
            decGiveDiamond = pUser->m_roleInfo.m_giveDiamond;
            pUser->m_roleInfo.m_diamond = 0;
            pUser->m_roleInfo.m_giveDiamond = 0;
        } else if (recvCmd->diamond() >= pUser->m_roleInfo.m_giveDiamond) {
            decDiamond = recvCmd->diamond() - pUser->m_roleInfo.m_giveDiamond;
            decGiveDiamond = pUser->m_roleInfo.m_giveDiamond;
            pUser->m_roleInfo.m_diamond -= decDiamond;
            pUser->m_roleInfo.m_giveDiamond = 0;
        } else {
            decGiveDiamond = recvCmd->diamond();
            pUser->m_roleInfo.m_giveDiamond -= decGiveDiamond;
        }
        UINT32 newVal = pUser->m_roleInfo.m_diamond + pUser->m_roleInfo.m_giveDiamond;

        if (oldVal != newVal) {
            Game::stResourceChgLog webCmd;
            webCmd.set_account(pUser->m_roleInfo.m_account);
            webCmd.set_charid(pUser->getID());
            webCmd.set_name(pUser->getName());
            webCmd.set_oldvalue(oldVal);
            webCmd.set_newvalue(newVal);
            webCmd.set_resid(eResource_Diamond);
            webCmd.set_reason("gm在线扣除钻石");
            g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

            Role::stSynResource retCmd;
            retCmd.set_restype(eResource_Diamond);
            retCmd.set_num(newVal);
            pUser->sendProto2Me(retCmd);

            UINT32 num = (oldVal >= newVal ? (oldVal - newVal) : (newVal - oldVal));
            pUser->sendDiamondDataLog(num, decGiveDiamond, decDiamond, "gm在线扣除钻石");
        }
    }
    
    if (recvCmd->vippoint()) {
        UINT32 oldVal = pUser->m_roleInfo.m_rechargeNum;
        if (recvCmd->vippoint() >= pUser->m_roleInfo.m_rechargeNum) {
            pUser->m_roleInfo.m_rechargeNum = 0;
        } else {
            pUser->m_roleInfo.m_rechargeNum -= recvCmd->vippoint();
        }
        UINT32 newVal = pUser->m_roleInfo.m_rechargeNum;
        if (oldVal != newVal) {
            pUser->calcVipLv();
            pUser->synTotalRecharge();
        }
    }
#ifdef _HDZ_DEBUG
    Athena::logger->trace("[gm在线扣除后]gold is %d, diamond is %d, give diamond is %d, vip point is %d",
            pUser->m_roleInfo.m_goldCoin, pUser->m_roleInfo.m_diamond, pUser->m_roleInfo.m_giveDiamond, pUser->m_roleInfo.m_rechargeNum);
#endif
}

void Global2GameModHandle::notifyMyArenaRank(TCPComponent * pCom,Global::stSynMyArenaRank * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->m_arenaRank = recvCmd->rank();
}

void Global2GameModHandle::reqResetArena(TCPComponent * pCom,Global::stReqResetArenaChallengeNum * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}

	SortProto::stResetArenaChallengeNunResult retCmd;	
	if (recvCmd->curresetnum() >= pUser->getCountByVip(eVipFun_ArenaResetNum)){
		retCmd.set_result(SortProto::stResetArenaChallengeNunResult::eResetNum_Use_Out);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	UINT32 needDiamond = CMiscCfgMgr::getSingleton().getNeedDiamondByNum(recvCmd->curresetnum() + 1);
	if (!pUser->removeResource(eResource_Diamond,needDiamond,"重置竞技场挑战次数")){
		retCmd.set_result(SortProto::stResetArenaChallengeNunResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	retCmd.set_result(SortProto::stResetArenaChallengeNunResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	Global::stNotfiyResetChallengeNum cmd;
	cmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::reqResetInterceptNum(TCPComponent *pCom,Global::stReqResetInterceptNum * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	MiscProto::stResetInterceptNumResult retCmd;
	if (recvCmd->resetnum() >= pUser->getCountByVip(eVipFun_Guard_Reset_Num)){
		retCmd.set_result(MiscProto::stResetInterceptNumResult::eNum_Use_Out);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	UINT32 needDiamond = CMiscCfgMgr::getSingleton().getInterceptNeedDiamond(recvCmd->resetnum() + 1);

	if (!pUser->removeResource(eResource_Diamond,needDiamond,"拦截重置次数")){
		retCmd.set_result(MiscProto::stResetInterceptNumResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return;
	}
	retCmd.set_result(MiscProto::stResetInterceptNumResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	Global::stNotifyResetInterceptNum cmd;
	cmd.set_charid(recvCmd->charid());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::refreshGuardQuality(TCPComponent *pCom,Global::stReqRefreshGuardQuality * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}

	MiscProto::stRetRefreshGuardQualityResult retCmd;
	UINT32 needDiamond = GuardCfg::getSingleton().getRefreshQualityDiamond();
	if (!pUser->m_bagMgr.removeItemByItemID(122208,0,1,"刷宝藏品质")){
		if (!pUser->removeResource(eResource_Diamond,needDiamond,"刷新宝藏品质")){
			retCmd.set_result(MiscProto::stRetRefreshGuardQualityResult::eDiamond_Not_Enough);
			pUser->sendProto2Me(retCmd);
			return;
		}
	}

	UINT32 rate = GuardCfg::getSingleton().getRefreshQualityRate(recvCmd->quality());
	if (g_selectByTenThound(rate)){//刷成功
				
		if (recvCmd->quality() < eGuard_Qaulity_5) {
			Global::stNotifyGuardQuality cmd;
			cmd.set_charid(pUser->getID());
			cmd.set_quality(recvCmd->quality() + 1);
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			retCmd.set_quality(recvCmd->quality() + 1);
		}
		retCmd.set_result(MiscProto::stRetRefreshGuardQualityResult::eSuccess);
		pUser->sendProto2Me(retCmd);
	}
	else {
		retCmd.set_result(MiscProto::stRetRefreshGuardQualityResult::eFail);
		pUser->sendProto2Me(retCmd);
	}
}

void Global2GameModHandle::clearOne2OneScore(TCPComponent *pCom,Global::stNotifyClearOne2OneScore * recvCmd)
{
	if (!recvCmd){
		return;
	}
	GameUserManager::getSingleton().clearAllUserOne2OneScore();

	Global::stClearOne2OneScoreSuccess cmd;
	cmd.set_flag(1);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::interceptReward(TCPComponent *pCom,Global::stInterceptReward * recvCmd)
{
	if (!recvCmd){
		return;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->giveAssert(recvCmd->resid1(),recvCmd->resnum1(),0,"拦截胜利奖励");
	pUser->giveAssert(recvCmd->resid2(),recvCmd->resnum2(),0,"拦截胜利奖励");
}

void Global2GameModHandle::quickFinishGuard(TCPComponent *pCom,Global::stQuickFinishDecDiamond * recvCmd)
{
	if (!recvCmd){
		return;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}

	MiscProto::stQuickFinishGuardResult retCmd;
	UINT32 needDimaond = GuardCfg::getSingleton().getQuickFinishDiamond();
	if (!pUser->removeResource(eResource_Diamond,needDimaond,"直接完成护送")){
		retCmd.set_result(MiscProto::stQuickFinishGuardResult::eDiamond_Not_Enough);
		pUser->sendProto2Me(retCmd);
		return;
	}

	Global::stQuickFinishDiamondSuccess cmd;
	cmd.set_charid(pUser->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}
	
void Global2GameModHandle::notifyBeginGuard(TCPComponent *pCom,Global::stNotifyUserBeginGuard * recvCmd)
{
	if (!recvCmd){
		return;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	
	stGuardNum notify;
	notify.notify(pUser,1);
    pUser->m_resBackMgr.addBackRes(CResBackCfg::Escort_Ship, 1, pUser->getVipLv());
}

void Global2GameModHandle::notifyInterceptResult(TCPComponent *pCom,Global::stNotifyUserInterceptResult * recvCmd)
{
	if (!recvCmd){
		return;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}

	stInterceptNum notify;
	notify.notify(pUser,1);
}

void Global2GameModHandle::notifyCloseBetaReward(TCPComponent *pCom,Global::stNotifyCloseBetaReward * recvCmd)
{
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->closeBetaReward(recvCmd->boxid(),recvCmd->givediamond(),recvCmd->giverecharge());
}

void Global2GameModHandle::intoManorFightMap(TCPComponent *pCom,Global::stEnterManorFightMap * recvCmd)
{
	if (!recvCmd){
		return ;
	}
		
	stCopyMapBase * pCopyMapBase = NULL;
	pCopyMapBase = g_copyMapBaseMgr.get(MANOR_FIGHT_MAPID);
	

	if (!pCopyMapBase){
		Athena::logger->error("[进入领地战场]失败,找不到关卡配置!");
		return ;
	}
	
	std::vector<GameUser *> teamAUserVec;
	std::vector<GameUser *> teamBUserVec;
	std::map<UINT32,UINT32> charID2HP;

	for (UINT8 i = 0;i < recvCmd->guildamem_size();++i){
		const Global::stEnterManorFightMapUserInfo & userInfo = recvCmd->guildamem(i);
		charID2HP[userInfo.charid()]  = userInfo.hp();
		GameUser * pUser = GameUserManager::getSingleton().getUser(userInfo.charid());
		if (pUser){
			teamAUserVec.push_back(pUser);			
		}
		else {
			Athena::logger->error("[进入领地战场]公会:%d有人已经下线了,进入失败!",userInfo.charid());
			return ;
		}
	}
	
	for (UINT8 i = 0;i < recvCmd->guildbmem_size();++i){
		const Global::stEnterManorFightMapUserInfo & userInfo = recvCmd->guildbmem(i);
		charID2HP[userInfo.charid()]  = userInfo.hp();
		GameUser * pUser = GameUserManager::getSingleton().getUser(userInfo.charid());
		if (pUser){
			teamBUserVec.push_back(pUser);			
		}
		else {
			Athena::logger->error("[进入领地战场]公会:%d有人已经下线了,进入失败!",userInfo.charid());
			return ;
		}
	}
	
	if ((teamAUserVec.size() <= 0) || (teamBUserVec.size() <= 0)){
		Athena::logger->error("[进入领地战场]有一方已经没有玩家了,进入失败!");
		return ;
	}

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(teamAUserVec[0]->getName(),pCopyMapBase->m_id);
	if (!pCopyMap || !pCopyMap->isManorFightMap()){
		Athena::logger->error("[进入领地战场]进入副本失败,创建副本不成功!");
		return;
	}
	
	((CManorFightMap *)pCopyMap)->setManorInfo(recvCmd->manorid(),recvCmd->fomation(),recvCmd->teamaid(),recvCmd->teambid());
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);

	for (UINT8 i = 0;i < teamAUserVec.size();++i){
		if (teamAUserVec[i]) {
			teamAUserVec[i]->setCampID(Map::eCamp_Red);
			pCopyMap->joinCamp(Map::eCamp_Red,teamAUserVec[i]->getID());
			teamAUserVec[i]->resetBattleProp();
			UINT32 initHP = charID2HP[teamAUserVec[i]->getID()];
			if (initHP > 0){
				teamAUserVec[i]->forceSetHP(initHP);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("name:%s,account:%s,charID:%d 进入领地战后初始血量是:%d",teamAUserVec[i]->getName(),teamAUserVec[i]->getAccount(),teamAUserVec[i]->getName(),initHP);
#endif
			}
			CMapMgr::getSingleton().changeMap(teamAUserVec[i],pCopyMap,i + 1);
		}
	}
	
	for (UINT8 i = 0;i < teamBUserVec.size();++i){
		if (teamBUserVec[i]) {
			teamBUserVec[i]->setCampID(Map::eCamp_Black);
			pCopyMap->joinCamp(Map::eCamp_Black,teamBUserVec[i]->getID());
			teamBUserVec[i]->resetBattleProp();
			UINT32 initHP = charID2HP[teamBUserVec[i]->getID()];
			if (initHP > 0){
				teamBUserVec[i]->forceSetHP(initHP);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("name:%s,account:%s,charID:%d 进入领地战后初始血量是:%d",teamBUserVec[i]->getName(),teamBUserVec[i]->getAccount(),teamBUserVec[i]->getName(),initHP);
#endif
			}
			CMapMgr::getSingleton().changeMap(teamBUserVec[i],pCopyMap,i + 4);
		}
	}
}

void Global2GameModHandle::changeName(TCPComponent *pCom,Global::stExistName * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	Role::stChangeNameResult retCmd;
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());

	if(recvCmd->result() == Global::stExistName::eName_Repetition)
	{
		retCmd.set_result(Role::stChangeNameResult::eName_Occupied);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	GameUserManager::getSingleton().changeName(recvCmd->charid(), recvCmd->name());
}

void Global2GameModHandle::getExpeditionInfo(TCPComponent *pCom, Global::stExpeditionInfo * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
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
    for (int i = 0; i < recvCmd->items_size(); i++) {
        SortProto::stExpeditionItem * pItem = retCmd.add_items();
        pItem->set_expeditionid(recvCmd->items(i).expeditionid());
        pItem->set_finishtime(recvCmd->items(i).finishtime());
        for (int k = 0; k < recvCmd->items(i).heroids_size(); k++) {
            pItem->add_heroids(recvCmd->items(i).heroids(k));
        }
    }
    pUser->sendProto2Me(retCmd);
}

void Global2GameModHandle::reqExpedition(TCPComponent *pCom, Global::stExpedition * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
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
    for (int i = 0; i < recvCmd->expeids_size(); i++) {
        idVec.push_back(recvCmd->expeids(i));
    }
    if (CExpeditionCfg::getSingleton().hasSameTask(pTask->m_type, idVec)) {
        retCmd.set_result(SortProto::stRetExpedition::eSame_Type);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    HeroInfo * pHero = NULL;
    stExpeditionHero stHero;
    std::set<UINT32> heroSet;
    UINT32 runeBattlePower = 0;
    std::vector<UINT32> runeBattlePropVec;
    std::map<UINT32, std::pair<UINT32, UINT32> > usedHeroMap;
    for (INT32 i = 0; i < recvCmd->heroids_size(); i++) {
        pHero = pUser->m_heroMgr.getHeroInfoByID(recvCmd->heroids(i));
        if (pHero) {
            stHero.m_id = recvCmd->heroids(i);
            stHero.m_lvl = pHero->m_dbData.m_level;
            runeBattlePower = 0;
            pHero->getRuneAdd(pUser, runeBattlePower, runeBattlePropVec, false);
            stHero.m_power = pHero->m_battlePower + runeBattlePower;
            if (heroSet.insert(recvCmd->heroids(i)).second) {
                usedHeroMap.insert(std::make_pair(stHero.m_id,
                            std::make_pair(stHero.m_lvl, stHero.m_power)));
            }
        }
    }
    if (heroSet.size() != pTask->m_heroNum) {
        retCmd.set_result(SortProto::stRetExpedition::eHero_Num_Limit);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    std::set<UINT32> usedHeroSet;
    for (int i = 0; i < recvCmd->usedhero_size(); i++) {
        usedHeroSet.insert(recvCmd->usedhero(i));
    }
    for (std::set<UINT32>::iterator it = heroSet.begin();
            it != heroSet.end(); it++) {
        if (usedHeroSet.find((*it)) != usedHeroSet.end()) {
            retCmd.set_result(SortProto::stRetExpedition::eHero_In_Used);
            pUser->sendProto2Me(retCmd);
            return ;
        }
    }
    Global::stExpeditionRet cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_expeditionid(pTask->m_id);
    cmd.set_finishtime(g_gameServer->getSec() + pTask->m_needTime);
    for (std::map<UINT32, std::pair<UINT32, UINT32> >::iterator it =
            usedHeroMap.begin(); it != usedHeroMap.end(); it++) {
        Global::stExpeditionHero * pHero = cmd.add_usedhero();
        pHero->set_id(it->first);
        pHero->set_lvl(it->second.first);
        pHero->set_power(it->second.second);
    }
    retCmd.set_result(SortProto::stRetExpedition::eSuccess);
    pUser->m_countMgr.addDayRefreshValue(ECOUNT_EXPEDITION_NUM, 1);
    pUser->sendProto2Me(retCmd);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::reqExpeditionReward(TCPComponent *pCom, Global::stExpeditionReward * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    SortProto::stRetExpeditionReward retCmd;
    retCmd.set_expeditionid(recvCmd->expeditionid());
    if (!recvCmd->expeditionid()) {
        retCmd.set_result(SortProto::stRetExpeditionReward::eInvalid_Id);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (g_gameServer->getSec() < recvCmd->finishtime()) {
        retCmd.set_result(SortProto::stRetExpeditionReward::eTime_Not_Up);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    const CExpeditionCfg::stTask * pTask =
        CExpeditionCfg::getSingleton().getTask(recvCmd->expeditionid());
    if (!pTask) {
        retCmd.set_result(SortProto::stRetExpeditionReward::eInvalid_Id);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    //base reward
    UINT32 rewardNum = 0;
    UINT32 totalPower = 0;
    std::vector<UINT32> heroVec;
    for (int i = 0; i < recvCmd->usedhero_size(); i++) {
        heroVec.push_back(recvCmd->usedhero(i).id());
        totalPower += recvCmd->usedhero(i).power();
    }
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
        } else if (it->first == CExpeditionCfg::LVL_GRATER) {
            for (int i = 0; i < recvCmd->usedhero_size(); i++) {
                if (recvCmd->usedhero(i).lvl() >= it->second) {
                    conditionCount++;
                    break;
                }
            }
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
    pUser->sendProto2Me(retCmd);
	pUser->sendProto2Me(itemTipCmd);
    stExpeditionNum expeditionNumNotify;
    expeditionNumNotify.notify(pUser, 1);

    Global::stExpeditionRewardRet cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_expeditionid(pTask->m_id);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::reqFastExpedition(TCPComponent *pCom, Global::stFastExpedition * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    SortProto::stRetFastExpedition retCmd;
    retCmd.set_expeditionid(recvCmd->expeditionid());
    if (!recvCmd->expeditionid()) {
        retCmd.set_result(SortProto::stRetFastExpedition::eInvalid_Id);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    UINT32 now = g_gameServer->getSec();
    UINT32 finishTime = recvCmd->finishtime();
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
    retCmd.set_result(SortProto::stRetFastExpedition::eSuccess);
    pUser->sendProto2Me(retCmd);

    Global::stFastExpeditionRet cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_expeditionid(recvCmd->expeditionid());
    cmd.set_finishtime(now);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::cancelExpedition(TCPComponent *pCom, Global::stCancelExpedition * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    SortProto::stRetCancelExpedition retCmd;
    retCmd.set_expeditionid(recvCmd->expeditionid());
    if (!recvCmd->expeditionid()) {
        retCmd.set_result(SortProto::stRetCancelExpedition::eInvalid_Id);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (g_gameServer->getSec() >= recvCmd->finishtime()) {
        retCmd.set_result(SortProto::stRetCancelExpedition::eTime_Out);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    retCmd.set_result(SortProto::stRetCancelExpedition::eSuccess);
    pUser->sendProto2Me(retCmd);

    Global::stCancelExpeditionRet cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_expeditionid(recvCmd->expeditionid());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::reqExpeditionSummary(TCPComponent *pCom, Global::stExpeditionSummary * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
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
    for (int i = 0; i < recvCmd->items_size(); i++) {
        SortProto::stExpeditionItem * pItem = retCmd.add_items();
        pItem->set_expeditionid(recvCmd->items(i).expeditionid());
        pItem->set_finishtime(recvCmd->items(i).finishtime());
    }
    pUser->sendProto2Me(retCmd);
}

void Global2GameModHandle::loadDevilCalls(TCPComponent *pCom, Global::stAllDevilCall * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    MiscProto::stDevilCallItem * pItem = NULL;
    MiscProto::stRetAllDevilCall retCmd;
    for (int i = 0; i < recvCmd->item_size(); i++) {
        pItem = retCmd.add_item();
        pItem->set_itemid(recvCmd->item(i).itemid());
        pItem->set_haspass(recvCmd->item(i).haspass());
        if (!recvCmd->item(i).haspass()) {
            continue ;
        }
        pItem->set_name1(recvCmd->item(i).name1());
        pItem->set_name2(recvCmd->item(i).name2());
        pItem->set_name3(recvCmd->item(i).name3());
        pItem->set_bossdrawstatus(recvCmd->item(i).bossdrawstatus());
        pItem->set_commondrawstatus(recvCmd->item(i).commondrawstatus());
    }
    retCmd.set_result(MiscProto::stRetAllDevilCall::eSuccess);
    pUser->sendProto2Me(retCmd);
}

void Global2GameModHandle::drawDevilCallReward(TCPComponent *pCom, Global::stDevilCallReward * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    MiscProto::stRetDevilCallReward retCmd;
    retCmd.set_bossid(recvCmd->bossid());
    retCmd.set_rewardtype((MiscProto::stRetDevilCallReward::eType)recvCmd->rewardtype());
    if (!recvCmd->gotpass() || recvCmd->status() == MiscProto::stDevilCallItem::eNo_Reward) {
        retCmd.set_result(MiscProto::stRetDevilCallReward::eNot_That_Reward);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (recvCmd->status() == MiscProto::stDevilCallItem::eGot_Reward) {
        retCmd.set_result(MiscProto::stRetDevilCallReward::eAlready_Draw);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    UINT32 boxId = CDevilCallCfg::getSingleton().getDevilReward(recvCmd->bossid(), recvCmd->rewardtype());
    if (!boxId) {
        retCmd.set_result(MiscProto::stRetDevilCallReward::eNot_That_Reward);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) <  pUser->m_boxMgr.needSlotNum(boxId)) {
        retCmd.set_result(MiscProto::stRetDevilCallReward::eBag_Full);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    pUser->m_boxMgr.openBox(boxId);
    retCmd.set_result(MiscProto::stRetDevilCallReward::eSuccess);

    pUser->sendProto2Me(retCmd);
    Global::stDevilCallRewardRet cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_bossid(recvCmd->bossid());
    cmd.set_rewardtype(recvCmd->rewardtype());
    cmd.set_status(MiscProto::stDevilCallItem::eGot_Reward);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::devilCall(TCPComponent *pCom, Global::stDevilCall * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    MiscProto::stRetDevilCall retCmd;
    retCmd.set_id(recvCmd->bossid());
    if (!recvCmd->gotpass()) {
        retCmd.set_result(MiscProto::stRetDevilCall::eNot_Open);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 3) {
        retCmd.set_result(MiscProto::stRetDevilCall::eBag_Full);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    UINT32 drawId = CDevilCallCfg::getSingleton().getDrawId(recvCmd->bossid());
    if (!drawId) {
        retCmd.set_result(MiscProto::stRetDevilCall::eNo_Such_Id);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    UINT32 moneyType = 0, needMoney = 0;
    CDevilCallCfg::getSingleton().getDrawInfo(drawId, moneyType, needMoney);
    if (!moneyType || !needMoney) {
        retCmd.set_result(MiscProto::stRetDevilCall::eNo_Such_Id);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    if (!pUser->removeAssert(moneyType, needMoney, 0, "魔王召唤抽奖消耗")) {
        retCmd.set_result(MiscProto::stRetDevilCall::eRes_Not_Enough);
        pUser->sendProto2Me(retCmd);
        return ;
    }

    std::map<UINT32, UINT32> drawMap;
    CDevilCallCfg::getSingleton().drawItem(drawId, drawMap);
    CDevilCallCfg::getSingleton().drawCertailItem(drawId, recvCmd->drawnum() + 1, drawMap);

	for (std::map<UINT32, UINT32>::iterator it = drawMap.begin();
            it != drawMap.end(); it++) {
		pUser->giveAssert(it->first,it->second,0,"魔王召唤奖励");
		ChatMgr::sendObtainItemNotify(pUser,"system_notice_1",it->first,it->second);
		MiscProto::stSynDrawItem * pSynDrawItem = retCmd.add_items();
		if (pSynDrawItem){
			pSynDrawItem->set_itemid(CItem::realItemIDByJobAndSex(it->first, pUser->getJob(), pUser->getSex()));
			pSynDrawItem->set_num(it->second);
		}
	}
    retCmd.set_result(MiscProto::stRetDevilCall::eSuccess);
    pUser->sendProto2Me(retCmd);

    Global::stDevilCallRet cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_bossid(recvCmd->bossid());
    cmd.set_drawnum(recvCmd->drawnum() + 1);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::devilCallSummary(TCPComponent *pCom, Global::stDevilCallSummary * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    MiscProto::stDevilCallItem * pItem = NULL;
    MiscProto::stRetDevilCallSummary retCmd;
    for (int i = 0; i < recvCmd->item_size(); i++) {
        pItem = retCmd.add_item();
        pItem->set_itemid(recvCmd->item(i).itemid());
        pItem->set_haspass(recvCmd->item(i).haspass());
        if (!recvCmd->item(i).haspass()) {
            continue ;
        }
        pItem->set_name1(recvCmd->item(i).name1());
        pItem->set_name2(recvCmd->item(i).name2());
        pItem->set_name3(recvCmd->item(i).name3());
        pItem->set_bossdrawstatus(recvCmd->item(i).bossdrawstatus());
        pItem->set_commondrawstatus(recvCmd->item(i).commondrawstatus());
    }
    pUser->sendProto2Me(retCmd);
}

void Global2GameModHandle::giveLilianCb(TCPComponent *pCom, Global::stGiveLilianDayTaskCb * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    stGiveLilianNum giveLilianNumNotify;
    giveLilianNumNotify.notify(pUser, 1);
}

void Global2GameModHandle::operateActivityReward(TCPComponent *pCom, Global::stOperateActivityReward * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    MiscProto::stRetOperateActvityReward retCmd;
    retCmd.set_activityid(recvCmd->activityid());
    retCmd.set_itemid(recvCmd->itemid());
	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < pUser->m_boxMgr.needSlotNum(recvCmd->boxid())){
        retCmd.set_res(MiscProto::stRetOperateActvityReward::BAG_FULL);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    retCmd.set_res(MiscProto::stRetOperateActvityReward::SUCCESS);
	pUser->sendProto2Me(retCmd);
	pUser->m_boxMgr.openBox(recvCmd->boxid());

    Global::stOperateActivityRewardCb globalCmd;
    globalCmd.set_charid(recvCmd->charid());
    globalCmd.set_activityid(recvCmd->activityid());
    globalCmd.set_itemid(recvCmd->itemid());
    globalCmd.set_status(recvCmd->status());
    globalCmd.set_boxid(recvCmd->boxid());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
}

void Global2GameModHandle::operateActivityLotteryReward(TCPComponent *pCom, Global::stOperateActivityLotteryReward * recvCmd) {
	if(!recvCmd){
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser){
		return ;
	}
    MiscProto::stRetOperateActvityReward retCmd;
    retCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Diamond_Lottery));
    retCmd.set_itemid(recvCmd->itemid());
    if (!pUser->removeResource(eResource_Diamond, recvCmd->cost(), "新加活动抽奖消耗")) {
        Athena::logger->error("[Global2GameModHandle] diamond not enough");
        retCmd.set_res(MiscProto::stRetOperateActvityReward::RESOURCE_LIMIT);
        pUser->sendProto2Me(retCmd);
        return ;
    }
	if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < pUser->m_boxMgr.needSlotNum(recvCmd->boxid())){
        Athena::logger->error("[Global2GameModHandle] bag full");
        retCmd.set_res(MiscProto::stRetOperateActvityReward::BAG_FULL);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    retCmd.set_res(MiscProto::stRetOperateActvityReward::SUCCESS);
	pUser->sendProto2Me(retCmd);
	pUser->m_boxMgr.openBox(recvCmd->boxid());

    Global::stOperateActivityLotteryRewardCb globalCmd;
    globalCmd.set_charid(recvCmd->charid());
    globalCmd.set_itemid(recvCmd->itemid());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
}

void Global2GameModHandle::openWindow(TCPComponent *pCom, Global::stUserRankInfoResult * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	SortProto::stSurplusNumResult retCmd;

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());

	if(!pUser){
		return ;
	}
	if(0 == pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM)){
		pUser->m_countMgr.addDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM, CEmeraldDreamCfg::getSingleton().getDailyNum());
		retCmd.set_num(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM));
	}else{
		retCmd.set_num(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM) - pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_CURRENT_NUM));
	}
	retCmd.set_damage(recvCmd->damage());
	retCmd.set_rank(recvCmd->rank());
	retCmd.set_resetnum(pUser->m_countMgr.getDayRefreshValue(ECOUNT_EMERALD_DREAM_RESETNUM));
	retCmd.set_endtime(recvCmd->endtime());
	retCmd.set_levelid(recvCmd->levelid());
	
	for(UINT8 i = 0; i < recvCmd->info_size(); ++i){
		 SortProto::userSortInfo *info = retCmd.add_info();
		 if(info){
		 	info->set_name(recvCmd->info(i).name());
		 }
	}
	pUser->sendProto2Me(retCmd);
}

void Global2GameModHandle::removeNum(TCPComponent *pCom, Global::strRemoveNum * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	
	if(!pUser){
		return ;
	}

	pUser->m_countMgr.addDayRefreshValue(ECOUNT_EMERALD_DREAM_CURRENT_NUM,1);
}

void Global2GameModHandle::enterEmeraldDream(TCPComponent *pCom, Global::stCopyMapInfoResult * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return ;
	}
	pUser->m_levelMgr.enterEmeraldDream(recvCmd->levelid());
}

void Global2GameModHandle::realTimeTeamMatch(TCPComponent *pCom, Global::stRealTimeTeamMatch * recvCmd)
{
	if(!recvCmd){
		return ;
	}
	// Athena::logger->error("[realTimeTeamMatch] %d,%d",recvCmd->etype(),  recvCmd->teamid());
	PubGame::stRealTimeTeamMatch cmd;
	for (UINT16 i = 0;i < recvCmd->memberid_size(); ++i){
		UINT32 memberid = recvCmd->memberid(i);
		if (memberid == 0)
		{
			cmd.add_memberid(0);
			continue;
		}

		GameUser* pUser = GameUserManager::getSingleton().getUser(memberid);
		if (pUser)
		{
			pUser->sendData2PubGame();
			cmd.add_memberid(pUser->getUniquePubID());
		}
		else
		{
			cmd.add_memberid(0);
			Athena::logger->error("[realTimeTeamMatch] 没有找到玩家 charid=%d",memberid);
		}
	}

	PubGame::eRealTimePKType type;
	if (recvCmd->etype() == Global::eRealTimePKType_KillAll)
	{
		type = PubGame::eRealTimePKType_KillAll;
	}else{
		type = PubGame::eRealTimePKType_Finish;
	}

	cmd.set_etype(type);
	cmd.set_teamid(recvCmd->teamid());

	PubSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::reqDeductMoney(TCPComponent *pCom, Global::stDeductMoney *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	GameUser* pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return ;
	}

	Global::stDeductMoneyResult retCmd;

	retCmd.set_charid(recvCmd->charid());
	if (!pUser->removeResource(eResource_Diamond,recvCmd->num(),"秘境寻宝刷新")){
		retCmd.set_result(Global::stDeductMoneyResult::DIAMOND_NOT_ENOUGH);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d,name:%s)扣除钻石失败，砖石不足",pUser->getID(), pUser->getName());
#endif
		return;
	}

	retCmd.set_result(Global::stDeductMoneyResult::eSuccess);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
}

void Global2GameModHandle::reqGiveReward(TCPComponent *pCom, Global::stGiveReward *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	Global::stGiveRewardResult retCmd;

	GameUser* pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	
	if(!pUser){
		return ;
	}

	retCmd.set_charid(recvCmd->charid());
	retCmd.set_rewardid(recvCmd->rewardid());
	retCmd.set_isintegralreward(recvCmd->isintegralreward());

	if (pUser->m_boxMgr.needSlotNum(recvCmd->boxid()) > pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1)){
		retCmd.set_result(Global::stGiveRewardResult::eBAG_SLOT_NOT_ENOUGH);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
#ifdef  _HDZ_DEBUG
		 Athena::logger->error("[秘境寻宝](charID:%d,name:%s)添加资源失败，背包满了!",pUser->getID(),pUser->getName());
#endif
		return ;
	}
	
	pUser->m_boxMgr.openBox(recvCmd->boxid());
/*
	ItemBoxCfg::stItemBox *pItemBox = ItemBoxCfg::getSingleton().getItemBox(recvCmd->boxid());
	if(!pItemBox){
		Athena::logger->trace("开箱子时候找不到配置:%d",recvCmd->boxid());
		return ;
	}
	
	Role::stObtainItemTip itemTipCmd;
	for(UINT16 i = 0; i < pItemBox->m_itemVec.size(); ++i){	
		Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
		if (pSyn){
			pSyn->set_itemid(pItemBox->m_itemVec[i].m_itemID);
			pSyn->set_num(pItemBox->m_itemVec[i].m_num);
#ifdef  _HDZ_DEBUG
			Athena::logger->trace("[秘境寻宝](charID:%d,name:%s)获得宝箱:%d:ItemID:%d,num:%d",pUser->getID(),pUser->getName(),recvCmd->boxid(),pItemBox->m_itemVec[i].m_itemID,pItemBox->m_itemVec[i].m_num);
#endif
		}
	}
	pUser->sendProto2Me(itemTipCmd);
*/	
	retCmd.set_result(Global::stGiveRewardResult::eSuccess);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
}

void Global2GameModHandle::reqEnterTaskLv(TCPComponent *pCom, Global::stReqEnterTaskLv * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	GameUser* pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return ;
	}

	pUser->m_levelMgr.enterLv(recvCmd->levelid());
}

void Global2GameModHandle::ReqDrawInviteReward(TCPComponent *pCom, Global::stReqDrawInviteReward * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	GameUser* pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return ;
	}
    Global::stReqDrawInviteRewardCb cb;
    cb.set_charid(recvCmd->charid());
    cb.set_id(recvCmd->id());
    cb.set_account(recvCmd->account());
    if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 3) {
        cb.set_res(false);
    } else {
        cb.set_res(true);
        pUser->m_boxMgr.openBox(recvCmd->boxid());
    }
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cb);
#ifdef _HDZ_DEBUG
    Athena::logger->trace("转发stReqDrawInviteRewardCb");
#endif
}

void Global2GameModHandle::ReqDrawUseInviteReward(TCPComponent *pCom, Global::stReqDrawUseInviteReward * recvCmd) {
	if(!recvCmd){
		return ;
	}

	GameUser* pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return ;
	}
    pUser->m_boxMgr.openBox(recvCmd->boxid());
    Athena::logger->trace("领取使用分享码奖励");
}

void Global2GameModHandle::reqQualifiedList(TCPComponent *pCom, Global::stSynRoleListResult *recvCmd)
{
	if(!recvCmd){
		return ;
	}
	
	PubGame::stSynPubRoleListResult retCmd;
	if(recvCmd->isfinal()){
		for(UINT16 i = 0; i < recvCmd->team_size(); ++i){
			PubGame::stTeam * pTeam = retCmd.add_team();

			pTeam->mutable_user1()->set_charid(recvCmd->team(i).user1().charid());
			pTeam->mutable_user1()->set_pubcharid(recvCmd->team(i).user1().pubcharid());
			pTeam->mutable_user1()->set_areacode(recvCmd->team(i).user1().areacode());
			pTeam->mutable_user1()->set_name(recvCmd->team(i).user1().name().c_str());
			pTeam->mutable_user1()->set_level(recvCmd->team(i).user1().level());
			pTeam->mutable_user1()->set_viplv(recvCmd->team(i).user1().viplv());
			pTeam->mutable_user1()->set_job(recvCmd->team(i).user1().job());
			pTeam->mutable_user1()->set_sex(recvCmd->team(i).user1().sex());
			pTeam->mutable_user1()->set_battlepower(recvCmd->team(i).user1().battlepower());
			pTeam->mutable_user1()->set_photoid(recvCmd->team(i).user1().photoid());

			pTeam->mutable_user2()->set_charid(recvCmd->team(i).user2().charid());
			pTeam->mutable_user2()->set_pubcharid(recvCmd->team(i).user2().pubcharid());
			pTeam->mutable_user2()->set_areacode(recvCmd->team(i).user2().areacode());
			pTeam->mutable_user2()->set_name(recvCmd->team(i).user2().name().c_str());
			pTeam->mutable_user2()->set_level(recvCmd->team(i).user2().level());
			pTeam->mutable_user2()->set_viplv(recvCmd->team(i).user2().viplv());
			pTeam->mutable_user2()->set_job(recvCmd->team(i).user2().job());
			pTeam->mutable_user2()->set_sex(recvCmd->team(i).user2().sex());
			pTeam->mutable_user2()->set_battlepower(recvCmd->team(i).user2().battlepower());
			pTeam->mutable_user2()->set_photoid(recvCmd->team(i).user2().photoid());
		}
	
	}else{
		for(UINT16 i = 0; i < recvCmd->charids_size(); ++i){
			PubGame::stPubRoleInfo * info = retCmd.add_charids();
			if(info){
				info->set_charid(recvCmd->charids(i).charid());
				info->set_pubcharid(recvCmd->charids(i).pubcharid());
				info->set_areacode(recvCmd->charids(i).areacode());
				info->set_name(recvCmd->charids(i).name().c_str());
				info->set_level(recvCmd->charids(i).level());
				info->set_viplv(recvCmd->charids(i).viplv());
				info->set_job(recvCmd->charids(i).job());
				info->set_sex(recvCmd->charids(i).sex());
				info->set_battlepower(recvCmd->charids(i).battlepower());
				info->set_pubserverindex(recvCmd->charids(i).pubserverindex());
				info->set_score(recvCmd->charids(i).score());
				info->set_photoid(recvCmd->charids(i).photoid());
			}
		}
	}

	retCmd.set_isfinal(recvCmd->isfinal());

	PubSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
}

void Global2GameModHandle::reqWorskip(TCPComponent *pCom, Global::stWorskipResult *recvCmd)
{
	if(!recvCmd){
		return;
	}

	GameUser* pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return;
	}

	SortProto::stWorskipResult retCmd;
	
	if(pUser->m_countMgr.getDayRefreshValue(ECOUNT_WORSKIP_NUM) >= 1){
		retCmd.set_result(SortProto::stWorskipResult::eTimeUseOut);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[朝拜](name:%s, cahrID:%d)朝拜次数用完，朝拜失败！", pUser->getName(), pUser->getID());
#endif
		return;
	}


	if(recvCmd->result() == recvCmd->eNoBody){
		retCmd.set_result(SortProto::stWorskipResult::eNoBody);
		pUser->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[朝拜](name:%s, cahrID:%d)没有人可供朝拜，朝拜失败！", pUser->getName(), pUser->getID());
#endif
		return;
	}

	stWorship config = CHeroLeagueCfg::getSingleton().getWorskipConfig();
	if(recvCmd->issameareacode()){
		pUser->m_boxMgr.openBox(config.m_denticalitem);		
	}else{
		pUser->m_boxMgr.openBox(config.m_differentitem);
	}

	pUser->m_countMgr.addDayRefreshValue(ECOUNT_WORSKIP_NUM, 1);

	retCmd.set_result(SortProto::stWorskipResult::eSuccess);
	pUser->sendProto2Me(retCmd);
}

void Global2GameModHandle::reqHeroLeaugeRedPoint(TCPComponent *pCom, Global::stRedPointResult *recvCmd)
{
	if(!recvCmd){
		return;
	}
	
	GameUser* pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return;
	}

	SortProto::stRedPointResult retCmd;
	if(recvCmd->canworship()){
		if(pUser->m_countMgr.getDayRefreshValue(ECOUNT_WORSKIP_NUM) >= 1){
			retCmd.set_canworship(false);
		}else{
			retCmd.set_canworship(true);
		}
	}else{
		retCmd.set_canworship(false);
	}

	retCmd.set_isqualified(recvCmd->isqualified());
	pUser->sendProto2Me(retCmd);
}

static void fillSynInfo(const Global::stSynMemberInfo  & member, TeamProto::stSynMemberInfo  * data)
{
    if (!data) {
        return ;
    }
	data->set_charid(member.charid());
	data->set_job(member.job());
	data->set_level(member.level());
	data->set_name(member.name());
	data->set_sex(member.sex());
	GameUser * pUser = GameUserManager::getSingleton().getUser(member.charid());
    data->set_fashionwingid(0);
	if (pUser){
        CItem * pItem = pUser->m_bagMgr.m_equipBag.getWingItem();
        if (pItem) {
            data->set_fashionwingid(pItem->m_itemInfo.m_itemID);
        }
    }
    data->set_battlepower(member.battlepower());
    data->set_headid(member.headid());
    data->set_bodyid(member.bodyid());
    data->set_cloackid(member.cloackid());
    data->set_shoeid(member.shoeid());
    data->set_weaponid(member.weaponid());
    data->set_wingid(member.wingid());
    data->set_fashionhair(member.fashionhair());
    data->set_fashionface(member.fashionface());
    data->set_fashionbody(member.fashionbody());
    data->set_fashiondecoration(member.fashiondecoration());
    data->set_fashionshoe(member.fashionshoe());
    data->set_fashionweapon(member.fashionweapon());
    data->set_effectid(member.effectid());
    data->set_wingbreaklv(member.wingbreaklv());
    data->set_leader(member.leader());
    data->set_pos(member.pos());
}

void Global2GameModHandle::sendRoomInfo(TCPComponent *pCom, Global::stRetRoomInfo *recvCmd) {
	TeamProto::stRoomInfo retCmd;
	retCmd.set_copymapid(recvCmd->copymapid());
	retCmd.set_teamid(recvCmd->teamid());
    for (int i = 0; i < recvCmd->members_size(); i++) {
        if (recvCmd->members(i).charid()) {
            TeamProto::stSynMemberInfo * pSyn = retCmd.add_members();
            fillSynInfo(recvCmd->members(i), pSyn);
        }
    }
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
        pUser->sendProto2Me(retCmd);
    }
}

void Global2GameModHandle::notifyAddTeamMember(TCPComponent *pCom, Global::stRetAddTeamMember *recvCmd) {
	TeamProto::stAddTeamMember retCmd;
    retCmd.set_pos(recvCmd->pos());
    fillSynInfo(recvCmd->memberinfo(), retCmd.mutable_memberinfo());
    for (int i = 0; i < recvCmd->charid_size(); i++) {
        Athena::logger->trace("收到notifyAddTeamMember charid is %d, exceptId is %d",
                recvCmd->charid(i), recvCmd->exceptid());
        GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid(i));
        if (pUser && (pUser->getID() != recvCmd->exceptid())) {
            pUser->sendProto2Me(retCmd);
        }
    }
}

void Global2GameModHandle::reqPropose(TCPComponent *pCom, Global::stReqPropose *recvCmd)
{
#undef REJECT_RETURN
#define REJECT_RETURN(error) do {	\
	packet.set_result(FriendProto::RejectMarry::RESULT_ERROR_##error);	\
	pUser1->sendProto2Me(packet);	\
	return;	\
} while(0)

#undef MARRY_SET_INFO
#define MARRY_SET_INFO(name) do {	\
	if (recvCmd->info().name()) {	\
		info->set_##name(recvCmd->info().name());	\
	} else {	\
		info->set_##name(0);	\
	}	\
} while(0)

	if (recvCmd) {
		FriendProto::RejectMarry packet;
		UINT32 id1 = recvCmd->id1();
		UINT32 id2 = recvCmd->id2();
        GameUser *pUser1 = GameUserManager::getSingleton().getUser(id1);
        GameUser *pUser2 = GameUserManager::getSingleton().getUser(id2);
		if (pUser1 && pUser2) {
			UINT32 proposeid1 = pUser1->getCoupleId();
			UINT32 proposeid2 = pUser2->getCoupleId();
			UINT32 proposecd1 = 0;
			UINT32 proposecd2 = 0;
			if (proposeid1 || proposeid2) {
				REJECT_RETURN(MARRIED);
			}
			if (proposecd1) {
				REJECT_RETURN(TICK1);
			}
			if (proposecd2) {
				REJECT_RETURN(TICK2);
			}
			packet.set_result(FriendProto::RejectMarry::RESULT_SUCCESS);
			pUser1->sendProto2Me(packet);

			FriendProto::stReqPropose pkg;
			pkg.set_id(id1);
			FriendProto::MarryObjInfo *info = pkg.mutable_info();
			MARRY_SET_INFO(charid);
			info->set_name(recvCmd->info().name());
			MARRY_SET_INFO(level);
			MARRY_SET_INFO(job);
			MARRY_SET_INFO(sex);
			MARRY_SET_INFO(battlepower);
			MARRY_SET_INFO(headid);
			MARRY_SET_INFO(bodyid);
			MARRY_SET_INFO(weaponid);
			MARRY_SET_INFO(cloackid);
			MARRY_SET_INFO(wingid);
			MARRY_SET_INFO(shoeid);
			MARRY_SET_INFO(fashionhair);
			MARRY_SET_INFO(fashionface);
			MARRY_SET_INFO(fashionbody);
			MARRY_SET_INFO(fashionweapon);
			MARRY_SET_INFO(fashionshoe);
			MARRY_SET_INFO(fashiondecoration);
			MARRY_SET_INFO(effectid);
			MARRY_SET_INFO(wingbreaklv);
			MARRY_SET_INFO(fashionwingid);

			pUser2->sendProto2Me(pkg);
		}
	}
#undef MARRY_SET_INFO
#undef REJECT_RETURN
}

void Global2GameModHandle::reqAgreeMarry(TCPComponent *pCom, Global::stReqAgreeMarry *recvCmd)
{
#undef MARRY_SET_INFO
#define MARRY_SET_INFO(name) do {	\
	if (recvCmd->info().name()) {	\
		info->set_##name(recvCmd->info().name());	\
	} else {	\
		info->set_##name(0);	\
	}	\
} while(0)

	if (recvCmd) {
		FriendProto::ProposeResult packet1;
		packet1.set_result(FriendProto::ProposeResult::RESULT_SUCCESS);
		UINT32 id1 = recvCmd->id1();
		UINT32 id2 = recvCmd->id2();
        GameUser * pUser1 = GameUserManager::getSingleton().getUser(id1);
        GameUser * pUser2 = GameUserManager::getSingleton().getUser(id2);
		if (pUser1 && pUser2) {
			FriendProto::MarryObjInfo *info = packet1.mutable_info();
			MARRY_SET_INFO(charid);
			info->set_name(recvCmd->info().name());
			MARRY_SET_INFO(level);
			MARRY_SET_INFO(job);
			MARRY_SET_INFO(sex);
			MARRY_SET_INFO(battlepower);
			MARRY_SET_INFO(headid);
			MARRY_SET_INFO(bodyid);
			MARRY_SET_INFO(weaponid);
			MARRY_SET_INFO(cloackid);
			MARRY_SET_INFO(wingid);
			MARRY_SET_INFO(shoeid);
			MARRY_SET_INFO(fashionhair);
			MARRY_SET_INFO(fashionface);
			MARRY_SET_INFO(fashionbody);
			MARRY_SET_INFO(fashionweapon);
			MARRY_SET_INFO(fashionshoe);
			MARRY_SET_INFO(fashiondecoration);
			MARRY_SET_INFO(effectid);
			MARRY_SET_INFO(wingbreaklv);
			/* MARRY_SET_INFO(fashionwingid); */
			CItem *pItem = pUser2->m_bagMgr.m_equipBag.getWingItem();
			if (pItem) {
				info->set_fashionwingid(pItem->m_itemInfo.m_itemID);
			} else {
				info->set_fashionwingid(0);
			}
			pUser1->sendProto2Me(packet1);
		}

		FriendProto::NotifyAgree pkg;
		pkg.set_result(FriendProto::NotifyAgree::RESULT_SUCCESS);
		pUser2->sendProto2Me(pkg);

        if (pUser1) {
            pUser1->setDivorceExpireTime(false);
        }
        if (pUser2) {
            pUser2->setDivorceExpireTime(false);
        }
	}
#undef MARRY_SET_INFO
}

void Global2GameModHandle::reqQuitWedding(TCPComponent *pCom, Global::stReqQuitWedding *recvCmd) {
	Role::stRetQuitWeddingScene retCmd;
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser) {
		Athena::logger->trace("stReqQuitWedding not find user");
		return ;
	}
    MarryMgr::getSingleton().delUserToRoom(pUser->getID());

	retCmd.set_result(Role::stRetQuitWeddingScene::eSuccess);
	pUser->sendProto2Me(retCmd);

	Map::stNotifyEnterMapInfo loadSceneCmd;
	loadSceneCmd.set_mapid(recvCmd->levelid());
	pUser->sendProto2Me(loadSceneCmd);

    CMap * pMap = CMapMgr::getSingleton().getMapByID(recvCmd->levelid());
	CMapMgr::getSingleton().changeMap(pUser, pMap);

    std::vector<UINT32> userList;
    for (int i = 0; i < recvCmd->id_size(); i++) {
        userList.push_back(recvCmd->id(i));
    }
    if (userList.empty()) {
		Athena::logger->trace("stReqQuitWedding no user in secene");
        return ;
    }
	Map::NotifyObjDisappear cmd;
	Map::NotifyObjDisappear_stObjDisappearInfo * disappearInfo = cmd.add_data();
	if (!disappearInfo){
        return ;
    }
	disappearInfo->set_motype(pUser->getType());
	disappearInfo->set_moid(pUser->getUniqueID());
	GameUserManager::getSingleton().sendProto2UserList(userList, cmd);
}

void Global2GameModHandle::reqReEnterWedding(TCPComponent *pCom, Global::stReqReEnterWedding *recvCmd) {
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser) {
		Athena::logger->trace("reqReEnterWedding not find user");
		return ;
	}

    MarryMgr::getSingleton().addUserToRoom(pUser->getID(), recvCmd->roomid());
	Map::stNotifyEnterMapInfo loadSceneCmd;
	loadSceneCmd.set_mapid(5);
	pUser->sendProto2Me(loadSceneCmd);

    CMap * pMap = CMapMgr::getSingleton().getMapByID(5);
	CMapMgr::getSingleton().changeMap(pUser, pMap);

    std::vector<UINT32> userList;
    for (int i = 0; i < recvCmd->id_size(); i++) {
        userList.push_back(recvCmd->id(i));
    }
    if (userList.empty()) {
		Athena::logger->trace("reqReEnterWedding no user in secene");
        return ;
    }

    Map::NotifyPlayerAppear playerCmd;
    Map::stPlayerData  * pPlayerData = playerCmd.add_data();
    if (!pPlayerData){
        return ;
    }
    pUser->getAppear(*pPlayerData);
    //时装翅膀
    CItem * pItem = pUser->m_bagMgr.m_equipBag.getWingItem();
    if (pItem) {
        pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
    }
    GameUserManager::getSingleton().sendProto2UserList(userList, playerCmd);

    for (size_t i = 0; i < userList.size(); i++) {
        if (userList[i] == recvCmd->charid()) {
            continue ;
        }
        GameUser * pOther = GameUserManager::getSingleton().getUser(userList[i]);
        if (!pOther) {
            continue ;
        }
        pPlayerData = playerCmd.add_data();
        if (!pPlayerData){
            continue ;
        }
        pOther->getAppear(*pPlayerData);
        //时装翅膀
        CItem * pItem = pOther->m_bagMgr.m_equipBag.getWingItem();
        if (pItem) {
            pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
        }
    }
	pUser->sendProto2Me(playerCmd);
}

void Global2GameModHandle::reqBuyFireWork(TCPComponent *pCom, Global::stReqBuyFireWork *recvCmd) {
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser) {
		Athena::logger->trace("stReqBuyFireWork not find user");
		return ;
	}
	Role::ResultBuyFirework retCmd;
    CMarryCfg::FireWorkInfo fwInfo;
    if (!CMarryCfg::getSingleton().getFwInfo(recvCmd->id(), fwInfo)) {
		Athena::logger->trace("stReqBuyFireWork invalid fire work id %d", recvCmd->id());
        retCmd.set_result(Role::ResultBuyFirework::RESULT_FAILURE);
        pUser->sendProto2Me(retCmd);
		return ;
    }
    if (!pUser->checkAssert((eResource)fwInfo.costType, fwInfo.cost)) {
        retCmd.set_result(Role::ResultBuyFirework::RESULT_FAILURE);
        pUser->sendProto2Me(retCmd);
		return ;
    }
    if (!pUser->removeAssert((eResource)fwInfo.costType, fwInfo.cost, 0, "购买烟火")) {
        retCmd.set_result(Role::ResultBuyFirework::RESULT_FAILURE);
        pUser->sendProto2Me(retCmd);
		return ;
    }
	pUser->m_bagMgr.addItem2Bag(fwInfo.itemId, 1, 0, "购买烟火");
	retCmd.set_result(Role::ResultBuyFirework::RESULT_SUCCESS);
	pUser->sendProto2Me(retCmd);
}

void Global2GameModHandle::reqAgreeInvite(TCPComponent *pCom, Global::stReqAgreeInvite *recvCmd) {

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser) {
		Athena::logger->trace("reqAgreeInvite not find user");
		return ;
	}
    MarryMgr::getSingleton().addUserToRoom(pUser->getID(), recvCmd->roomid());

	Map::stNotifyEnterMapInfo loadSceneCmd;
	loadSceneCmd.set_mapid(5);
	pUser->sendProto2Me(loadSceneCmd);

    CMap * pMap = CMapMgr::getSingleton().getMapByID(5);
	CMapMgr::getSingleton().changeMap(pUser, pMap);

    std::vector<UINT32> userList;
    for (int i = 0; i < recvCmd->id_size(); i++) {
        userList.push_back(recvCmd->id(i));
    }
    if (userList.empty()) {
		Athena::logger->trace("reqReEnterWedding no user in secene");
        return ;
    }

    Map::NotifyPlayerAppear playerCmd;
    Map::stPlayerData  * pPlayerData = playerCmd.add_data();
    if (!pPlayerData){
        return ;
    }
    pUser->getAppear(*pPlayerData);
    //时装翅膀
    CItem * pItem = pUser->m_bagMgr.m_equipBag.getWingItem();
    if (pItem) {
        pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
    }
    GameUserManager::getSingleton().sendProto2UserList(userList, playerCmd);
    
    for (size_t i = 0; i < userList.size(); i++) {
        if (userList[i] == recvCmd->charid()) {
            continue ;
        }
        GameUser * pOther = GameUserManager::getSingleton().getUser(userList[i]);
        if (!pOther) {
            continue ;
        }
        pPlayerData = playerCmd.add_data();
        if (!pPlayerData){
            continue ;
        }
        pOther->getAppear(*pPlayerData);
        //时装翅膀
        CItem * pItem = pOther->m_bagMgr.m_equipBag.getWingItem();
        if (pItem) {
            pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
        }
    }
	pUser->sendProto2Me(playerCmd);
}

void Global2GameModHandle::reqTakeCandy(TCPComponent *pCom, Global::stReqTakeCandy *recvCmd) {
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser) {
		Athena::logger->trace("reqTakeCandy not find user");
		return ;
	}
}

void Global2GameModHandle::reqSendWeddingRing(TCPComponent *pCom, Global::stSendWeedingRing *recvCmd) {
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser) {
		Athena::logger->trace("reqSendWeddingRing not find user");
		return ;
	}
    Role::stRetDrawWeddingRing retCmd;
    CMarryCfg::MarryInfo marryInfo;
    if (!CMarryCfg::getSingleton().getMarryInfo(recvCmd->id(), marryInfo)) {
        Athena::logger->trace("reqSendWeddingRing 拿不到婚礼配置");
		retCmd.set_result(Role::stRetDrawWeddingRing::eInner_Error);
		pUser->sendProto2Me(retCmd);
		return ;
    }
    if (!pUser->m_bagMgr.addItem2Bag(marryInfo.boxId, 1, 0, "结婚礼物返还")) {
        retCmd.set_result(Role::stRetDrawWeddingRing::eInner_Error);
        pUser->sendProto2Me(retCmd);
        return ;
    }
    retCmd.set_result(Role::stRetDrawWeddingRing::eSuccess);
    pUser->sendProto2Me(retCmd);

    Global::stMarryInfoCb cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_roomid(recvCmd->roomid());
    cmd.set_id(marryInfo.boxId);
    GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void Global2GameModHandle::reqUseFireWork(TCPComponent *pCom, Global::stUseFireWork *recvCmd) {
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (!pUser) {
		Athena::logger->trace("stUseFireWork not find user");
		return ;
	}
	Role::ResultUseFirework retCmd;
    if (!pUser->checkAssert(recvCmd->index(), 1)) {
		Athena::logger->trace("stUseFireWork 资源不足");
        retCmd.set_result(Role::ResultUseFirework::RESULT_FAILURE);
        pUser->sendProto2Me(retCmd);
		return ;
    }
    if (!pUser->removeAssert(recvCmd->index(), 1, 0, "结婚放烟火")) {
		Athena::logger->trace("stUseFireWork 结婚放烟火失败");
        retCmd.set_result(Role::ResultUseFirework::RESULT_FAILURE);
        pUser->sendProto2Me(retCmd);
		return ;
    }
	Role::NotifyFirework cmd;
	cmd.set_fire(CMarryCfg::getSingleton().getIndexOfFw(recvCmd->index()));
	cmd.set_name(pUser->getName());
	pUser->sendProto2Me(cmd);
    for (INT32 i = 0; i < recvCmd->id_size(); i++) {
		GameUser * pOther = GameUserManager::getSingleton().getUser(recvCmd->id(i));
		if (!pOther){
			continue ;
		}
        pOther->sendProto2Me(cmd);
    }
	retCmd.set_result(Role::ResultUseFirework::RESULT_SUCCESS);
	pUser->sendProto2Me(retCmd);
}

void Global2GameModHandle::reqCoupleInfo(TCPComponent *pCom, Global::stRetMarryInfoToCouple *recvCmd) {
	GameUser * pHus = GameUserManager::getSingleton().getUser(recvCmd->husband());
	GameUser * pWife = GameUserManager::getSingleton().getUser(recvCmd->wife());
	if (!pHus || ! pWife) {
		Athena::logger->trace("reqCoupleInfo not find couple");
		return ;
	}
    Map::NotifyPlayerAppear husCmd;
    Map::stPlayerData  * pPlayerData = husCmd.add_data();
    if (!pPlayerData){
        return ;
    }
    pWife->getAppear(*pPlayerData);
    //时装翅膀
    CItem * pItem = pWife->m_bagMgr.m_equipBag.getWingItem();
    if (pItem) {
        pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
    }
    pHus->sendProto2Me(husCmd);

    Map::NotifyPlayerAppear wifeCmd;
    pPlayerData = wifeCmd.add_data();
    if (!pPlayerData){
        return ;
    }
    pWife->getAppear(*pPlayerData);
    //时装翅膀
    pItem = pWife->m_bagMgr.m_equipBag.getWingItem();
    if (pItem) {
        pPlayerData->set_fashionwingid(pItem->m_itemInfo.m_itemID);
    }
    pHus->sendProto2Me(wifeCmd);
}

void Global2GameModHandle::notifyCoupleMarryCb(TCPComponent *pCom, Global::stNotifyCoupleMarryCb *recvCmd) {
    MarryMgr::getSingleton().addUserToRoom(recvCmd->husband(), recvCmd->roomid());
    MarryMgr::getSingleton().addUserToRoom(recvCmd->wife(), recvCmd->roomid());
}

void Global2GameModHandle::notifyCoupleForceDivorceCb(TCPComponent *pCom, Global::stRetForceDivorceCb *recvCmd) {
    Map::stNotifyEnterMapInfo loadSceneCmd;
    loadSceneCmd.set_mapid(1);
    for (int i = 0; i < recvCmd->id_size(); i++) {
        MarryMgr::getSingleton().delUserToRoom(recvCmd->id(i));
        GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->id(i));
        if (!pUser){
            continue;
        }
        pUser->sendProto2Me(loadSceneCmd);

        CMap * pMap = CMapMgr::getSingleton().getMapByID(1);
        CMapMgr::getSingleton().changeMap(pUser, pMap);
    }
    for (int i = 0; i < recvCmd->coupleid_size(); i++) {
        MarryMgr::getSingleton().delUserToRoom(recvCmd->coupleid(i));
        GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->coupleid(i));
        if (!pUser){
            continue;
        }
        pUser->forceDivorce();
        pUser->setDivorceExpireTime(false);
    }
}

void Global2GameModHandle::reqSynGuildBlackMarketLv(TCPComponent *pCom, Global::stSynGuildBlackMarketLvResult * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return;
	}
 	Athena::logger->trace("同步公会黑市等级Lv:%d",recvCmd->lv());
	pUser->m_blackMacketLv = recvCmd->lv();
}

void Global2GameModHandle::notifyDiscoverConsume(TCPComponent *pCom, Global::stDiscoverConsume *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return;
	}
    pUser->m_resBackMgr.addBackRes(CResBackCfg::Fam_Trasure, 1, pUser->getVipLv());
}

void Global2GameModHandle::reqUseLvlBox(TCPComponent *pCom, Global::stUseLvlBoxRes *recvCmd) {
	if(!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){
		return;
	}
	Role::stUseItemError errorCmd;
	CItem * pItem = pUser->m_bagMgr.getItemByInstaceID(recvCmd->bagtype(),recvCmd->instanceid());
	if (!pItem || !pItem->m_pItemBase){
        Athena::logger->trace("使用等级直升宝箱升级pItem is %x", pItem);
		errorCmd.set_result(Role::stUseItemError::eItem_Not_Exist);
		pUser->sendProto2Me(errorCmd);
		return ;
	}
    errorCmd.set_itemid(pItem->m_itemInfo.m_itemID);

    if (pUser->getLevel() + pItem->m_pItemBase->m_effect >= recvCmd->maxlvl()) {
		errorCmd.set_result(Role::stUseItemError::eUse_Item_Invalid);
		pUser->sendProto2Me(errorCmd);
		return ;
    }
    UINT16 loop = recvCmd->maxlvl() - pItem->m_pItemBase->m_effect - pUser->getLevel();
#ifdef _HDZ_DEBUG
    Athena::logger->trace("使用等级直升宝箱升级cur level=%d, loop=%d", pUser->getLevel(), loop);
#endif
    for (UINT16 i = 0; i < loop ; ++i ){
        pUser->levelup();	
    }

    Role::stSynRoleLevel  synCmd;
    synCmd.set_curexp(pUser->m_roleInfo.m_exp);
    synCmd.set_curlevel(pUser->m_roleInfo.m_level);
    pUser->sendProto2Me(synCmd);

    pUser->m_bagMgr.decItem(pItem, 1, "道具使用");
}

void querynftDo(GameUser* pUser, UINT32 charid, UINT32 webconnid){
	Global::stGame2GlobalQueryUserNftResult  webCmd;
	webCmd.set_charid(charid);
	webCmd.set_webconnid(webconnid);

	if(!pUser){
		webCmd.set_result(1);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
		return;
	}

	webCmd.set_result(0); 
	webCmd.set_vip(pUser->m_roleInfo.m_nftVip);
	
	std::map<UINT32,stNftItem*> m = NftConfig::getSingleton().getItemMap();
	std::map<UINT32,stNftItem*>::iterator it = m.begin();
	for (;it != m.end();++it){
		//Athena::logger->trace("nft道具:id=%d,url=%s",it->first,it->second->m_url);
		Global::stNftItem* p = webCmd.add_data();
		p->set_itemid(it->second->m_id);
		UINT32 realID = CItem::realItemIDByJobAndSex(it->second->m_id,pUser->getJob(),pUser->getSex());
		if(realID == eResource_Diamond) {
			p->set_itemcnt(pUser->getDiamond());
		}else if(CItem::isEquipPiece(realID)){
			p->set_itemcnt(pUser->m_bagMgr.getPieceNumByID(realID));
		}else{
			UINT32 c = pUser->m_bagMgr.getItemNumByID(realID);
			std::vector<CItem * >  vec;
			c += pUser->m_bagMgr.m_equipBag.getItemByID(realID,0,vec);
			p->set_itemcnt(c);
		}
		p->set_url(it->second->m_url);

		stItemBase * pItemBase =  g_itemBaseMgr.get(it->second->m_id);
		if (pItemBase){
			p->set_itemname(pItemBase->m_name); 
		}
	}

	GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
}
//查询用户nft
void Global2GameModHandle::querynft(TCPComponent *pCom, Global::stGlobal2GameQueryUserNft *recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("收到stGlobal2GameQueryUserNft charid=%d", recvCmd->charid());
	if (Athena::global["nftServer"] != "true")
	{
		return;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	querynftDo(pUser, recvCmd->charid(),recvCmd->webconnid());
}

void changeNftItemDo(GameUser* pUser, bool offline, UINT32 charid, UINT32 webconnid, UINT32 itemID, INT32 count)
{
	if (Athena::global["nftServer"] != "true")
	{
		return;
	}
	Global::stGame2GlobalChangeNftItemResult  webCmd;
	webCmd.set_charid(charid);
	webCmd.set_webconnid(webconnid);

	if(!pUser){
		webCmd.set_result(1);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
		return;
	}

	//约定0代表购买vip
	if (itemID==0)
	{
		if (pUser->m_roleInfo.m_nftVip >= count)
		{
			webCmd.set_result(5);
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
			return;
		}

		pUser->m_roleInfo.m_nftVip = count;
		pUser->m_roleInfo.m_nftVipGiftTime = g_gameServer->getSec() - g_getSecOfDay(g_gameServer->getSec());

		if (!offline)
		{
			//下发客户端
			Role::stNftVipNtf cmd;
			cmd.set_isnftvip(pUser->m_roleInfo.m_nftVip);
			pUser->sendProto2Me(cmd);
		} 
	}else
	{
		std::map<UINT32,stNftItem*> m = NftConfig::getSingleton().getItemMap();
		std::map<UINT32,stNftItem*>::iterator it = m.find(itemID);
		if(itemID != eResource_Diamond && it == m.end()){
			webCmd.set_result(4);
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
			return;
		}
		if (it->second->m_flag != 0)
		{
			webCmd.set_result(4);
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
			return;
		}
		

		UINT32 realID = CItem::realItemIDByJobAndSex(itemID,pUser->getJob(),pUser->getSex());
		if(count > 0) {
			//增加
			pUser->giveAssert(itemID,count,0,"nft充值");
		}else{
			count = -count;
			if(realID == eResource_Diamond) {
				if(pUser->getDiamond() < UINT32(count)){
					webCmd.set_result(2);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
					return;
				}
				if (!pUser->removeResource(eResource_Diamond,count,"nft上链")){
					webCmd.set_result(3);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
					return ;
				}
			}else if(CItem::isEquipPiece(realID)){
				UINT32 nowCount = pUser->m_bagMgr.getPieceNumByID(realID);
				if(nowCount < UINT32(count)){
					webCmd.set_result(2);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
					return;
				}
				if(!pUser->m_bagMgr.removeItemByItemID(realID,count,"nft上链")){
					webCmd.set_result(3);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
					return;
				}
			}
			else
			{
				UINT32 nowCount = pUser->m_bagMgr.getItemNumByID(realID);
				if(nowCount < UINT32(count)){
					webCmd.set_result(2);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
					return;
				}
				if(!pUser->m_bagMgr.removeItemByItemID(realID,count,"nft上链")){
					webCmd.set_result(3);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
					return;
				}
			}
		}
	}

	if (offline)
	{
		pUser->save(Global::SAVE_FORCE);
	}
	
	 
	webCmd.set_result(0);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
}

//修改用户nft道具
void Global2GameModHandle::changeNftItem(TCPComponent *pCom, Global::stGlobal2GameChangeNftItem *recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("收到stGlobal2GameChangeNftItem charid=%d itemid=%d, count=%d", recvCmd->charid(), recvCmd->itemid(), recvCmd->count());
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	changeNftItemDo(pUser, false, recvCmd->charid(), recvCmd->webconnid(), recvCmd->itemid(), recvCmd->count());
}


void Global2GameModHandle::nftOfflineQuery(TCPComponent * pCom,Global::stRetNftOfflineUserData * recvCmd)
{
	if (!recvCmd){
		return ;	
	}
	
	GameUser * pOther = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pOther){ //人物在线
		querynftDo(pOther, recvCmd->charid(),recvCmd->webconnid());
		return ;
	}
   
	RoleInfoForSave	 forSave;
	bcopy(recvCmd->data().data(),&forSave.m_roleInfo,sizeof(RoleInfo));
	pOther = ATHENA_NEW GameUser("",0,"");

	INT32 binSize = recvCmd->datasize() - sizeof(RoleInfo) ;
	if (binSize > 0) {
		bcopy(recvCmd->data().data() + sizeof(RoleInfo),forSave.m_binData,binSize);
	}
	else {
		return;
	}
	pOther->initData(forSave.m_roleInfo,forSave.m_binData,binSize);
	pOther->setName(forSave.m_roleInfo.m_name);
	pOther->setID(forSave.m_roleInfo.m_charID);

	querynftDo(pOther, recvCmd->charid(),recvCmd->webconnid());
	SAFE_DELETE(pOther); 
}


//修改离线用户nft道具
void Global2GameModHandle::changeNftItemOffline(TCPComponent *pCom, Global::stRetChangeNftOfflineUserData *recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("收到stRetChangeNftOfflineUserData charid=%d itemid=%d, count=%d", recvCmd->charid(), recvCmd->itemid(), recvCmd->count());
	if (Athena::global["nftServer"] != "true")
	{
		return;
	}

	GameUser * pOther = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pOther){ //人物在线
		changeNftItemDo(pOther, false, recvCmd->charid(), recvCmd->webconnid(), recvCmd->itemid(), recvCmd->count());
		return ;
	}
   
	RoleInfoForSave	 forSave;
	bcopy(recvCmd->data().data(),&forSave.m_roleInfo,sizeof(RoleInfo));
	pOther = ATHENA_NEW GameUser(forSave.m_roleInfo.m_account,0,"");

	INT32 binSize = recvCmd->datasize() - sizeof(RoleInfo) ;
	if (binSize > 0) {
		bcopy(recvCmd->data().data() + sizeof(RoleInfo),forSave.m_binData,binSize);
	}
	else {
		Global::stGame2GlobalChangeNftItemResult  webCmd;
		webCmd.set_charid(recvCmd->charid());
		webCmd.set_webconnid(recvCmd->webconnid());
		webCmd.set_result(1);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(webCmd);
		return;
	}
	pOther->initData(forSave.m_roleInfo,forSave.m_binData,binSize);
	pOther->setName(forSave.m_roleInfo.m_name); 
	pOther->setID(forSave.m_roleInfo.m_charID);

	changeNftItemDo(pOther, true, recvCmd->charid(), recvCmd->webconnid(), recvCmd->itemid(), recvCmd->count());
	SAFE_DELETE(pOther); 
}