#include "PacketProcessor.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "md5.h"
#include "GateUser.h"
#include "GateServer.h"
#include "GlobalPacketProcessor.h"
#include "SelectUserManager.h"
#include "GateUserManager.h"
#include "GamePacketProcessor.h"
#include "GlobalSvrClientMgr.h"
#include "GameSvrClientMgr.h"
#include "GateUserConnMgr.h"
#include "Login.pb.h"
#include "Role.pb.h"
#include "Global.pb.h"
#include "ModID.pb.h"
#include "Map.pb.h"
#include "UserLoginModHandle.h"
#include "UserRoleModHandle.h"
#include "Misc.pb.h"
#include "SortList.pb.h"
#include "Friend.pb.h"
#include "GuildProto.pb.h"
#include "Team.pb.h"
#include "Task.pb.h"

extern GateServer * g_gateServer;

PacketProcessor::PacketProcessor()
{
	m_protoModHandles[Login::ReqLoginGate::ModID] = ATHENA_NEW UserLoginModHandle();
	m_protoModHandles[Proto::ROLE_MOD ] = ATHENA_NEW UserRoleModHandle();
}

PacketProcessor::~PacketProcessor()
{
}

void PacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
}

bool PacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到的modID=%d,funID=%d",header->m_modID,header->m_funID);
#endif

	if (PacketProcessorBase::processOneBufferMsg(pConn,header,pNull)) {
		return true;
	}
	
	if (g_gateServer->isStatProto()){//是否统计协议
		g_gateServer->m_protoStat.addRcv(header->m_modID,header->m_funID,header->m_dataLen);
	}
	
	ProtoMsgHandleBase * pModHandle = m_protoModHandles[header->m_modID];
	if (pModHandle) {
		if (pModHandle->handle(pConn,(char *)pNull,header->m_dataLen,header->m_funID)){
			return true;
		}
	}
	
	{//处理转发
		switch (header->m_modID) {
			case Proto::ROLE_MOD :
				{
					switch (header->m_funID) {
						case Role::stReqOpenShop::FunID :
						case Role::stReqBuyItem::FunID :
						case Role::stChatCmd::FunID :
						case Role::stReqHeroGoOutForBattle::FunID:
						case Role::ReqHeroChange2WaitState::FunID:
						case Role::stReqCombineHero::FunID:
						case Role::stReqLevelupHero::FunID:
						case Role::stReqHeroEvolution::FunID:
						case Role::stReqHeroStudySkill::FunID:
						case Role::stReqRefineHeroSkill::FunID:
						case Role::stReqReplaceHeroSkill::FunID:
						case Role::stReqUpgradeHeroSkill::FunID:
						case Role::stReqSellItem::FunID:
						case Role::stReqArrangeMainBag::FunID:	
						case Role::stMoveItem::FunID:
						case Role::stReqRaiseItem::FunID:
						case Role::stReqSmeltEquip::FunID:
						case Role::stReqRecastEquip::FunID:
						case Role::stReqCombineItem::FunID:
						case Role::stReqRefreshShoplist::FunID:
						case Role::stReqFinishLv::FunID:
						case Role::stReqCharpterReward::FunID:
						case Role::stReqRushLevel::FunID:
						case Role::stReqResetRushNum::FunID:
						case Role::stReqRefineEquip::FunID:
						case Role::stUseItem::FunID:
						case Role::stReqGetEquipSkill::FunID:
						case Role::stReqRefineEquipSkill::FunID:
						case Role::stMoveEquipSkill2Slot::FunID:
						case Role::stReqUpgradeEquipSkill::FunID:
						case Role::stReqRaiseUserSkillLv::FunID:
						case Role::stSaveNewBieGuideStep::FunID:
						case Role::stOpenLevelBox::FunID:
						case Role::stReqRecoverRemainTime::FunID:
						case Role::stSwapHeroBattlePos::FunID:
						case Role::stReqSaveSystemSetting::FunID:
						case Role::stUnlockRareSkill::FunID:
						case Role::stUpgradeRareSkill::FunID:
						case Role::stReqFiveRaiseEquip::FunID:
						case Role::stReqHero10TimeStudySkill::FunID:
						case Role::st10TimesGetEquipSkill::FunID:
						case Role::stReqFirstRechargeReward::FunID:
						case Role::stReqFirst10RechargeReward::FunID:
						case Role::stSubNewBieGuideID::FunID :
						case Role::stResetTeamCopyMapNum::FunID :
						case Role::stResetRealTimeCopyMapNum::FunID :
						case Role::stReqChgEquipSkill::FunID:
						case Role::stReqUseMonthCardItem::FunID:
						case Role::stReqUseSelectItemBox::FunID:
						case Role::stReqUseHeroSkillBook::FunID:
						case Role::stReqUseEquipSkillBook::FunID:
						case Role::stReqBreakEquip::FunID:
						case Role::stReqRecastHero::FunID:
						case Role::stReqOpenBuyGoldWindow::FunID:
						case Role::stReqBuyGold::FunID:
						case Role::stReqEnchant::FunID:
						case Role::stReqRecastEnchant::FunID:
						case Role::stReqLockEnchant::FunID:
						case Role::stReqHeroRune::FunID:
						case Role::stReqAddHeroRune::FunID:
						case Role::stReqDelHeroRune::FunID:
						case Role::stReqRunePieceCombine::FunID:
						case Role::stReqRuneIntensify::FunID:
						case Role::stReqRuneEvolve::FunID:
						case Role::stReqExtractGem::FunID:
						case Role::stReqWedding::FunID:
						case Role::stReqAgreeWedding::FunID:
						case Role::stReqDivorce::FunID:
						case Role::stReqRejectDivorce::FunID:
						case Role::stReqBuyNum::FunID:
						case Role::stReqColumnList::FunID:
						case Role::stReqMarketBuyItem::FunID:
						case Role::stReqOpenWindow::FunID:
						case Role::stReqRefreshList::FunID:
						case Role::stReqRefreshNum::FunID:
						case Role::stReqAKeyRaiseUserSkillLv::FunID:
						case Role::stReqStoneSynthesis::FunID:
						case Role::stReqStoneEvolvement::FunID:
						case Role::stReqStoneProp::FunID:
						case Role::stReqChangePhoto::FunID:
						case Role::stReqHasPhotoID::FunID:
						case Role::stReqUnlockPhoto::FunID:
						case Role::stSpriteInfo::FunID:
						case Role::stTrainHeroReq::FunID:
						case Role::stGuaJiAwardReq::FunID:
						case Role::stGuaJiStartReq::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGame(pConn,header,pNull);
								return true;
							}
							break;
						case Role::stReqUserDetailInfo::FunID:
						case Role::stReqGiftBag::FunID:
						case Role::stReqGotCloseBetaReward::FunID:
						case Role::stReqChangeName::FunID:
						case Role::stReqInviteToRoom::FunID:
						case Role::stReqAgreeEnterWedding::FunID:
						/* case Role::stReqRejectDivorce::FunID: */
						case Role::stReqUseFirework::FunID:
						case Role::stReqTakeCandy::FunID:
						case Role::stReqBuyFirework::FunID:
						case Role::stReqEnterWeddingAlone::FunID:
						case Role::stReqQuitWeddingScene::FunID:
						case Role::stReqDrawWeddingRing::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGlobal(pConn,header,pNull);
								return true;
							}
							break;
					}
				}
				break;
			case Proto::MAP_MOD :
				{
					switch (header->m_funID) {
						case Map::ReqEnterLevelMap::FunID :
						case Map::stReqLeaveLevel::FunID :
						case Map::ReqMove::FunID :
						case Map::stThrowSkill::FunID :
						case Map::stClientLoadResFinish::FunID :
						case Map::stInterruptPlaySkill::FunID :
						case Map::stMapObjectChgDir::FunID :
						case Map::stNotifyHit::FunID :
						case Map::stReqOnlineUserOnMainCity::FunID:
						case Map::stChangeHeroGroup::FunID:
						case Map::stReqChangeMainCity::FunID:
						case Map::stCalcThunderDamage::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGame(pConn,header,pNull);
								return true;
							}
							break;
					}
				}
				break;
			case Proto::MISC_MOD :
				{
					switch (header->m_funID){
						case MiscProto::stActiveOneTitle::FunID :
						case MiscProto::stReqBuyStar::FunID:
						case MiscProto::ReqOpenDrawWindow::FunID:
						case MiscProto::stReqDraw::FunID:
						case MiscProto::stBuyInvestPlan::FunID:
						case MiscProto::stReqInvestPlanReward::FunID:
						case MiscProto::stReqEatDinner::FunID:
						case MiscProto::stBuyPeaceModuleTime::FunID:
						case MiscProto::stReqCombineTreasure::FunID:
						case MiscProto::stReqRewardInfo::FunID:
						case MiscProto::stReqSignReward::FunID:
						case MiscProto::stReqOpenSvrReward::FunID:
						case MiscProto::stReqLvReward::FunID:
						case MiscProto::stReqOnlineReward::FunID:
						case MiscProto::stCaptureUserTreasure::FunID:
						case MiscProto::stCancelActiveTitle::FunID:
						case MiscProto::stReqUnlockedSprite::FunID:
						case MiscProto::stReqLevelupSprite::FunID:
						case MiscProto::stSpriteFollowMaster::FunID:
						case MiscProto::stReqAvaibleActivity::FunID:
						case MiscProto::stReqAcitivityReward::FunID:
						case MiscProto::stReqClearWorldBossActivityCD::FunID:
						case MiscProto::stReq7DayActivityInfo::FunID:
						case MiscProto::stReqSevenDayReward::FunID:
						case MiscProto::stReqRechargeActivityList::FunID:
						case MiscProto::stReqRechargeActivtiyReward::FunID:
						case MiscProto::stReqInvestgationReward::FunID:
						case MiscProto::stReqActivityShop::FunID:
						case MiscProto::stReqLuckShopReward::FunID:
						case MiscProto::stSaveOpenLuckShopWinTime::FunID:
						case MiscProto::stResetInterceptCD::FunID:
						case MiscProto::stReqGuardInspired::FunID:
						case MiscProto::stReqSummonHighQualityGuard::FunID:
						case MiscProto::stReqOpenLimitFashionDrawWin::FunID:
						case MiscProto::stReqDrawLimitFashion::FunID:
						case MiscProto::stReqExchangeItem::FunID:
						case MiscProto::stSendRecycleRechargeActivity::FunID:
						case MiscProto::stReqRecycleRechargeActReward::FunID:
						case MiscProto::stReqGloryfashion::FunID:
						case MiscProto::stReqIllusionSprite::FunID:
						case MiscProto::stReqIllusionSpriteEat::FunID:
						case MiscProto::stReqIllusionSpriteSkillUp::FunID:
						case MiscProto::stReqPayType::FunID:
						case MiscProto::stReqLuckyDraw::FunID:
						case MiscProto::stReqLuckyDrawReward::FunID:
						case MiscProto::stReqLuckyDrawPointReward::FunID:
						case MiscProto::stReqShareInvite::FunID:
						case MiscProto::stReqDailyShareInvite::FunID:
						case MiscProto::stReqDrawDailyShareReward::FunID:
						case MiscProto::stReqDrawOnceShareReward::FunID:
						case MiscProto::stReqReceiveState::FunID:
						case MiscProto::stReqReceiveReward::FunID:
						case MiscProto::stReqDrawLateWalfare::FunID:
						case MiscProto::stReqDrawResGetBack::FunID:
						case MiscProto::stReqDrawAllResGetBack::FunID:
						case MiscProto::stReqOpenMonsterWindow::FunID:
						case MiscProto::stReqAttackMonster::FunID:
						case MiscProto::stReqBuyAttackTime::FunID:
						case MiscProto::stReqAKeyAttackMonster::FunID:
						case MiscProto::stReqMountLevelUp::FunID:
						case MiscProto::stReqButtleMount::FunID:
						case MiscProto::stReqMountFollow::FunID:
						case MiscProto::stReqMountUnreal::FunID:
						case MiscProto::stReqOpenMountGrowWindow::FunID:
						case MiscProto::stReqReceiveWflfare::FunID:
						case MiscProto::stReqPaySign::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGame(pConn,header,pNull);
								return true;
							}
							break;
						case MiscProto::stReqReadMail::FunID:
						case MiscProto::stReqFetchAttach::FunID:
						case MiscProto::stReqOpenCaptureTreasure::FunID:
						case MiscProto::stReqChallengeUserList::FunID:
						case MiscProto::stReqWorldBossDamageRank::FunID:
						case MiscProto::stReqBossWorldMatch::FunID:
						case MiscProto::stReqOpenWorldBossWin::FunID:
						case MiscProto::stCancelWorldBossMatch::FunID:
						case MiscProto::stQueryDrawHeroActivity::FunID:
						case MiscProto::stReqDrawHeroScoreReward::FunID:
						case MiscProto::stReqDrawHeroRankReward::FunID:
						case MiscProto::stReqCompensationList::FunID:
						case MiscProto::stReqGotCompensation::FunID:
						case MiscProto::stReqRaiseDamageInWorldBoss::FunID:
						case MiscProto::stReqOpenGuardWin::FunID:
						case MiscProto::stReqGuardDetailInfo::FunID:
						case MiscProto::stReqIntercept::FunID:
						case MiscProto::stReqGuard::FunID:
						case MiscProto::stReqCanHelpGuardFriend::FunID:
						case MiscProto::stInviteFriendForHelpGuard::FunID:
						case MiscProto::stReplyHelpGuard::FunID:
						case MiscProto::stBeginGuard::FunID:
						case MiscProto::stReqInterceptInfo::FunID:
						case MiscProto::stResetInterceptNum::FunID:
						case MiscProto::stReqRefreshGuardQuality::FunID:
						case MiscProto::stNotifyInterceptResult::FunID:
						case MiscProto::stQuickFinishGuard::FunID:
						case MiscProto::stReqAKeyFetchAttach::FunID:
						case MiscProto::stReqAllDevilCall::FunID:
						case MiscProto::stReqDevilCallReward::FunID:
						case MiscProto::stReqDevilCall::FunID:
						case MiscProto::stReqDevilCallSummary::FunID:
						case MiscProto::stReqOperateActvity::FunID:
                        case MiscProto::stReqOperateActvityReward::FunID:
						case MiscProto::stRepOpenDiscoverWindow::FunID:
						case MiscProto::stReqPlayDice::FunID:
						case MiscProto::stReqGetOrGiveUpTask::FunID:
						case MiscProto::stReqRefreshNum::FunID:
						case MiscProto::stReqGetIntegralReward::FunID:
						case MiscProto::stReqGiveReward::FunID:
						case MiscProto::stReqRedPointRuls::FunID:
						case MiscProto::stReqInviteFriend::FunID:
						case MiscProto::stReqDrawInviteFriend::FunID:
						case MiscProto::stReqUseInviteCode::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGlobal(pConn,header,pNull);
								return true;
							}
							break;
					}
				}
				break;
			case Proto::MISC2_MOD:
				{
					switch (header->m_funID)
					{ 
						case MiscProto::stReqServerPay::FunID:
						case MiscProto::stReqSummonData::FunID:
						case MiscProto::stReqSummonReward::FunID:
						case MiscProto::stReqRechargeCheck::FunID:

							{
								GateUserConnMgr::getSingleton().forwardGame(pConn,header,pNull);
								return true;
							}
							break;
						
						default:
							break;
					}
				}
				break;
			case Proto::SORTLIST_MOD :
				{
					switch (header->m_funID){
						case SortProto::stReqLevelSortList::FunID :
						case SortProto::stReqBattlePowerSortList::FunID:
						case SortProto::stReqCopymapSortList::FunID:
						case SortProto::stReqArenaPKList::FunID:
						case SortProto::stReqArenaSortList::FunID:
						case SortProto::stReqOne2OnePVPSortList::FunID:
						case SortProto::stChallengeArenaUser::FunID:
						case SortProto::stReqLeitaiSortList::FunID:
						case SortProto::stReqDrawHeroSortList::FunID:
						// case SortProto::stReqOne2OnePVPMatch::FunID:
						// case SortProto::stCancelOne2OnePVPMatch::FunID:
						case SortProto::stReqMySortListInfo::FunID:
						case SortProto::stReqResetArenaChallengeNum::FunID:
						case SortProto::stReqOpenSvrBPRank::FunID:
						case SortProto::stReqGloryfashionSortList::FunID:
						case SortProto::stReqEmeraldDreamSortList::FunID:
						case SortProto::stReqHeroList::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGlobal(pConn,header,pNull);
								return true;	
							}
							break;
						case SortProto::stChallegeLeitaiUser::FunID:
						case SortProto::stOpenLeitaiWindow::FunID:
						case SortProto::stSendLeitaiResult::FunID:
						case SortProto::stSelectLeiTaiMode::FunID:
						case SortProto::stResetLeitaiInfo::FunID:
						case SortProto::stLeitaiMoveHero2BattlePos::FunID:
						case SortProto::stLeitaiCancelGoOutForBattle::FunID:
						case SortProto::stCommitArenaResult::FunID:
						case SortProto::stReqEnterLeitaiScene::FunID:
						case SortProto::stReqResetLeitaiBuff::FunID:
						case SortProto::stReqBuffResetTime::FunID:
						case SortProto::stReqClearArenaCD::FunID:
						case SortProto::stReqEnterEmeraldDream::FunID:
						case SortProto::stResetFightNum::FunID:
						case SortProto::stReqSurplusNum::FunID:
						case SortProto::stReqOne2OnePVPMatch::FunID:
						case SortProto::stCancelOne2OnePVPMatch::FunID:
						case SortProto::stReqMenberInfo::FunID:
						case SortProto::stReqWorship::FunID:
						case SortProto::stReqRegister::FunID:
						case SortProto::stReqHeroLeagueMatch::FunID:
						case SortProto::stCancelHeroLeagueMatch::FunID:
						case SortProto::stReqPubLogIn::FunID:
						case SortProto::stReqPubLogOut::FunID:
						case SortProto::stReqRedPoint::FunID:
						case SortProto::stReqDragonNest::FunID:
						case SortProto::stReqChallengeDragonNest::FunID:
						case SortProto::stReqResetChallengeDragonNest::FunID:
						case SortProto::stReqStoneCopyMapInfo::FunID:
						case SortProto::stReqChallengeStoneCopyMap::FunID:
						case SortProto::stReqResetStoneCopyMapNum::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGame(pConn,header,pNull);
								return true;
							}
							break;
						case SortProto::stReqExpeditionInfo::FunID:
						case SortProto::stReqExpedition::FunID:
						case SortProto::stReqExpeditionReward::FunID:
						case SortProto::stReqFastExpedition::FunID:
						case SortProto::stReqCancelExpedition::FunID:
						case SortProto::stReqExpeditionSummary::FunID:
						case SortProto::stReqOpenLeagueWindow::FunID:
						case SortProto::stReqQualifiedList::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGlobal(pConn,header,pNull);
								return true;
							}
							break;
					}
				}
				break;
			case Proto::FRIEND_MOD :
				{
					switch (header->m_funID){
						case FriendProto::stReqAddOneFriend::FunID :
						case FriendProto::stDelFriend::FunID:
						case FriendProto::stFindAndAddFriend::FunID:
						case FriendProto::stReqApplyAddFriendList::FunID:
						case FriendProto::stReqRecommondFriend::FunID:
						case FriendProto::stGiveLilian2Friend::FunID:
						case FriendProto::stReqReceiveLilian::FunID:
						case FriendProto::stFetchLilian::FunID:
						case FriendProto::stReqAdd2ApplyList::FunID:
						case FriendProto::stRefuseAddFriend::FunID:
						case FriendProto::stInvitePKWithFriend::FunID:
						case FriendProto::stReplyFriendPKInvite::FunID:
						case FriendProto::stReqPropose::FunID:
						case FriendProto::stReqAgreeMarry::FunID:
						case FriendProto::stReqCancelMarry::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGlobal(pConn,header,pNull);
								return true;
							}
							break;
					}
				}
				break;
			case Proto::GUILD_MOD :
				{
					switch (header->m_funID){
						case GuildProto::stReqGuild::FunID :
						case GuildProto::stReqGuildDetailInfo::FunID:
						case GuildProto::stReqApplyJoinInList::FunID:
						case GuildProto::stReplyApplier::FunID:
						case GuildProto::stReqLeaveGuild::FunID:
						case GuildProto::stAssignGuildTitle::FunID:
						case GuildProto::stCancelTitle::FunID:
						case GuildProto::stGiveLeader2Other::FunID:
						case GuildProto::stFireGuild::FunID:
						case GuildProto::stKillOutGroupMember::FunID:
						case GuildProto::stChgGuildSign::FunID:
						case GuildProto::stReqGuildMemInfo::FunID:
						case GuildProto::stCancelFireGuild::FunID:
						case GuildProto::stCancelJoinInGuild::FunID:
						case GuildProto::stReqLevelUpHallLv::FunID:
						case GuildProto::stReqPrayBuildingLv::FunID:
						case GuildProto::stReqLevelUpShopBuildingLv::FunID:
						case GuildProto::stReqGuildTrace::FunID:
						case GuildProto::stChgNotice::FunID:
						case GuildProto::stReqPray::FunID:
						case GuildProto::stIntoGuildCopymap::FunID:
						case GuildProto::stSearchGuild::FunID:
						case GuildProto::stReqOpenGuildCopymapWin::FunID:
						case GuildProto::stReqDamageSortlist::FunID:
						case GuildProto::stReqGuildCopymapScoreSortlist::FunID:
						case GuildProto::stReqEnterGuildCopymap::FunID:
						case GuildProto::stStartGuildCopymap::FunID:
						case GuildProto::stInviteTeamInGuildChannel::FunID:
						case GuildProto::stReqGuildManorFightWin::FunID:
						case GuildProto::stReqOpenApplyManorFightWin::FunID:
						case GuildProto::stApplyManorFight::FunID:
						case GuildProto::stReqManorFightWin::FunID:
						case GuildProto::stReqManorFightFormation::FunID:
						case GuildProto::stNotifyLeaveManorFightWin::FunID:
						case GuildProto::stRefreshManorApplyInfo::FunID:
						case GuildProto::stEnterManorFightTeam::FunID:
						case GuildProto::stReqChgManorFightTeamPos::FunID:
						case GuildProto::stReqLevelUpBlackMarketLv::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGlobal(pConn,header,pNull);
								return true;
							}
							break;
						case GuildProto::stReqCreateGuild::FunID:
						case GuildProto::stReqJoinInGuild::FunID:
						case GuildProto::stReqDonate::FunID:
						case GuildProto::stChangeBadge::FunID:
						case GuildProto::stReqChangeBadge::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGame(pConn,header,pNull);
								return true;
							}
							break;
					}	
				}
				break;
			case Proto::TEAM_MOD :
				{
					switch (header->m_funID){
						case TeamProto::stReqCreateTeam::FunID :
						case TeamProto::stReqEnterTeam::FunID :
						case TeamProto::stReqLeaveTeam::FunID :
						case TeamProto::stReqFireTeam::FunID :
						case TeamProto::stInviteJoinInTeam::FunID :
						case TeamProto::stReplyJoinInTeam::FunID :
						case TeamProto::stReqEnterLv::FunID :
						case TeamProto::stReqBroadcastInviteCmd::FunID:
						case TeamProto::stReqAutoMatchTeam::FunID:
						case TeamProto::stCancelAutoMatchTeam::FunID:
						case TeamProto::stReqFirstKillInfoByID::FunID:
						// case TeamProto::stReqRealTimeSingleMatch::FunID:
						case TeamProto::stReqRealTimeTeamMatch::FunID:
						// case TeamProto::stReqCancelRealTimeSingleMatch::FunID:
						// case TeamProto::stReqCancelRealTimeTeamMatch::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGlobal(pConn,header,pNull);
								return true;
							}
							break;

						case TeamProto::stReqRealTimeSingleMatch::FunID:
						case TeamProto::stReqCancelRealTimeSingleMatch::FunID:
						case TeamProto::stReqCancelRealTimeTeamMatch::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGame(pConn,header,pNull);
								return true;
							}
							break;
					}	
				}
				break;
			case Proto::TASK_MOD :
				{
					switch (header->m_funID){
						case TaskProto::stReqAcceptTask::FunID :
						case TaskProto::stReqCommitTask::FunID :
						case TaskProto::stChgTaskSubStatus::FunID:
						case TaskProto::stSaveCurrentTask::FunID:
						case TaskProto::stReqBatchAcceptTask::FunID:
						case TaskProto::stReqDailyTaskScoreReward::FunID:
						case TaskProto::stReqOpenTaskWindow::FunID:
							{
								GateUserConnMgr::getSingleton().forwardGame(pConn,header,pNull);
								return true;
							}
							break;
					}	
				}
				break;
		}	
	}
	Athena::logger->error("%s (%u, %u, %u)", __PRETTY_FUNCTION__,header->m_modID,header->m_funID,header->m_dataLen);
	return false;
}
