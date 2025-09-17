#ifndef __H_GAMEMOD_HANDLE_H__
#define __H_GAMEMOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Global.pb.h"
#include "net.h"

using namespace net;
class GameModHandle : public ProtoMsgHandle<GameModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	GameModHandle();
	//析构
	~GameModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//用户登录game成功
	void userLoginGameSuccess(TCPComponent * pCom,Global::stUserEnterMapSuccess  * recvCmd);
	//保存玩家数据
	void saveRoleData(TCPComponent * pCom,Global::stSaveRoleInfo * recvCmd);
	//处理来自客户端的gm指令
	void processGM(TCPComponent * pCom,Global::stProcessGM *recvCmd);
	//创建公会
	void createGuild(TCPComponent * pCom,Global::stCreateGuild * recvCmd);
	//请求加入公会
	void joinGuild(TCPComponent *pCom,Global::stForwardJoininGuild * recvCmd);
	//请求给公会加贡献度
	void addGuildContribute(TCPComponent *pCom,Global::stAddGuildContribute * recvCmd);
	//公会捐赠成功
	void guildDonateSuccess(TCPComponent *pCom,Global::stGuildDonateSuccess * recvCmd);
	//同步自己的贡献度
	void synGuildContribute(TCPComponent *pCom,Global::stSynMyContribyte * recvCmd);
	//转发公会聊天消息
	void broadcastGuildChat(TCPComponent *pCom,Global::stBroadcastGuildChatInfo * recvCmd);	
	//收到用户等级
	void synUserLv(TCPComponent *pCom,Global::stSynUserLevel * recvCmd);
	//副本星星数
	void synCopymapStarNum(TCPComponent *pCom,Global::stSynCopymapStarNum * recvCmd);
	//玩家战力
	void synUserBattlePower(TCPComponent *pCom,Global::stSynUserBattlePower * recvCmd);
	//同步武神抽奖积分
	void synUserDrawHeroScore(TCPComponent *pCom,Global::stSynDrawHeroScore   * recvCmd);
	//转发组队聊天
	void broadcastTeamChat(TCPComponent * pCom,Global::stSendTeamChatInfo * recvCmd);
	//离开关卡
	void teamLeaveCopymap(TCPComponent * pCom,Global::stTeamLeaveCopyMap * recvCmd);
	//宝藏合成
	void treasureCombine(TCPComponent * pCom,Global::stTreasureCombine * recvCmd);
	//购买和平时间成功
	void buyPeaceModuleSuccess(TCPComponent * pCom,Global::stBuyPeaceModubleTime * recvCmd);
	//请求换服
	void reqChangeSvr(TCPComponent * pCom,Global::stReqChangeGameSvr * recvCmd);
	//增加抢夺资源
	void addTreasure(TCPComponent * pCom,Global::stSynAddTreasure * recvCmd);
	//挑战擂台玩家
	void challengeLeitaiUser(TCPComponent * pCom,Global::stUserLeitaiChallenge * recvCmd);
	//竞技场挑战结果
	void arenaPKResult(TCPComponent * pCom,Global::stArenaPKResult * recvCmd);
	//更新擂台时间
	void updateLeitaiUseTime(TCPComponent * pCom,Global::stUpLeitaiUseTime * recvCmd);
	//处理夺宝
	void captureTreasure(TCPComponent * pCom,Global::stForwardCapture * recvCmd);
	//one2one结果
	void one2OneFinish(TCPComponent * pCom,Global::stOne2OnePVPFinish * recvCmd);
	//装备信息改变了
	void equipInfoChg(TCPComponent * pCom,Global::stNotifyCurEquipInfo * recvCmd);
	//组队副本结束
	void teamCopyMapFinish(TCPComponent * pCom,Global::stTeamCopyMapFinish * recvCmd);
	//设置副本清cd时间
	void setWorldBossCDValue(TCPComponent * pCom,Global::stSetClearWorldBossTime * recvCmd);
	//设置副本伤害加成值
	void setWorldBossBuyNum(TCPComponent * pCom,Global::stSetWorldBossBuyNum * recvCmd);
	//玩家消耗体力
	void userDecSP(TCPComponent * pCom,Global::stUserDecSP * recvCmd);
	//玩家消耗历练
	void userDecLilian(TCPComponent * pCom,Global::stUsreDecLilian * recvCmd);
	//公会组队副本失败
	void guildTeamCopymapFail(TCPComponent * pCom,Global::stGuildCopymapFail * recvCmd);
	//公会组队副本杀死怪
	void guildTeamCopymapSuccess(TCPComponent * pCom,Global::stGuildCopymapKillBoss * recvCmd);
	//世界boss失败
	void worldBossFail(TCPComponent * pCom,Global::stWorldBossCopymapFail * recvCmd);
	//杀死世界boss
	void killWorldBoss(TCPComponent * pCom,Global::stKillWorldBoss * recvCmd);
	//离开世界boss副本
	void leaveWorldBossMap(TCPComponent * pCom,Global::stUserLeaveWorldBoss * recvCmd);
	//更换徽章
	void chgBadge(TCPComponent * pCom,Global::stForwardChgBadge * recvCmd);
	//请求积分奖励
	void reqScoreReward(TCPComponent * pCom,Global::stForwardDrawHeroScoreReward * recvCmd);
	//收到问卷调查结果
	void investgateResult(TCPComponent * pCom,Global::ForwardInvestgateResult * recvCmd);
	//同步vip等级
	void synVipLv(TCPComponent * pCom,Global::stSynVipLv * recvCmd);
	//清除竞技场cd
	void clearArenaCD(TCPComponent * pCom,Global::stNotifyClearArenaCD *recvCmd);
	//处理竞技场重置通知
	void resetArenaChallengeNum(TCPComponent * pCom,Global::stNotfiyResetChallengeNum * recvCmd);
	//通知清空护送cd
	void clearInterceptCD(TCPComponent * pCom,Global::stNotifyClearInterceptCD * recvCmd);
	//重置拦截次数
	void resetInterceptNum(TCPComponent * pCom,Global::stNotifyResetInterceptNum * recvCmd);
	//召唤护送的最高等级
	void setGuardHighQuality(TCPComponent * pCom,Global::stNotifyGuardQuality * recvCmd);
	//鼓舞士气成功
	void inspiredSuccess(TCPComponent * pCom,Global::stNotifyGuardInspiredSuccess * recvCmd);
	//清空在线玩家one2one积分成功
	void clearOne2OneScoreSuccess(TCPComponent * pCom,Global::stClearOne2OneScoreSuccess * recvCmd);
	//快速完成扣钻石完成
	void quickFinishDecDiamond(TCPComponent * pCom,Global::stQuickFinishDiamondSuccess * recvCmd);
	//领地战完成了
	void finishManorFight(TCPComponent * pCom,Global::stFinishManorFight * recvCmd);
	//同步名字
	void synName(TCPComponent * pCom,Global::stSynName * recvCmd);
	//同步荣耀时装排行
	void synUserGloryfashion(TCPComponent *pCom,Global::stSynGloryfashionCharID * recvCmd);
	//更新远征信息
	void addExpedition(TCPComponent *pCom,Global::stExpeditionRet * recvCmd);
	//更新远征信息
	void delExpedition(TCPComponent *pCom,Global::stExpeditionRewardRet* recvCmd);
	//快速远征
	void fastExpedition(TCPComponent *pCom,Global::stFastExpeditionRet* recvCmd);
	//取消远征
	void cancelExpedition(TCPComponent *pCom,Global::stCancelExpeditionRet* recvCmd);
	//更新大魔王獎勵狀態
	void updateDevilReward(TCPComponent *pCom,Global::stDevilCallRewardRet* recvCmd);
	//更新大魔王次數
	void updateDevilDrawNum(TCPComponent *pCom,Global::stDevilCallRet* recvCmd);
	//更新翡翠梦境伤害
	void updateEmeraldDreamDamage(TCPComponent *pCom,Global::stKillEmeraldDreamBoss* recvCmd);
	//更新新加活动
	void updateOperateActivity(TCPComponent *pCom,Global::stOperateActivityCb* recvCmd);
	//新加活动领取奖励回调
	void OperateActivityRewardCb(TCPComponent *pCom,Global::stOperateActivityRewardCb* recvCmd);
	//新加活动抽奖回调
	void OperateActivityLotteryRewardCb(TCPComponent *pCom,Global::stOperateActivityLotteryRewardCb* recvCmd);
	//请求翡翠梦境个人排名信息
	void reqUserRankInfo(TCPComponent *pCom,Global::stReqUserRankInfo* recvCmd);
	//请求翡翠梦境副本信息
	void reqCopyMapInfo(TCPComponent *pCom,Global::stCopyMapInfo *recvCmd);
	//跨服,离开队伍
	void reqOnPubLeaveTeam(TCPComponent *pCom,Global::stOnPubLeaveTeam *recvCmd);
	//秘境寻宝刷新
	void reqRefreshNum(TCPComponent *pCom,Global::stDeductMoneyResult *recvCmd);
	//秘境寻宝获得积分奖励
	void reqIntegralReward(TCPComponent *pCom,Global::stGiveRewardResult * recvCmd);
	//秘境寻宝任务检查（SquareType id=2/3/4）
	void reqMissionCheck(TCPComponent *pCom,Global::stReqCheckMission * recvCmd);
	////秘境寻宝任务检查（SquareType id=5/6）
	void reqTaskCheck(TCPComponent *pCom,Global::stReqCheckTask * recvCmd);
	//请求解散队伍
	void reqDisbandTeam(TCPComponent *pCom, Global::stReqDisbandTeam *recvCmd);
	//请求分享好友人数奖励回调
	void ReqDrawInviteRewardCb(TCPComponent *pCom, Global::stReqDrawInviteRewardCb *recvCmd);
	//更新跨服角色信息
	void updateRegisterInfo(TCPComponent *pCom, Global::stHeroLeagueResult *recvCmd);
	//请求同步联赛参赛角色列表
	void reqSynRoleList(TCPComponent *pCom, Global::stSynRoleList *recvCmd);
	//同步冠军信息
	void synChampionInfo(TCPComponent *pCom, Global::stReqSynChampionInfo *recvCmd);
	//朝拜
	void reqWorskip(TCPComponent *pCom, Global::stReqWorskip *recvCmd);
	//英雄联赛红点
	void reqHeroLeagueRedPoint(TCPComponent *pCom, Global::stReqRedPoint *recvCmd);
	//拦截结果上报
	void notifyInterceptResult(TCPComponent *pCom,Global::stInterceptResultToGlobal * recvCmd);
    //通知结婚
	void notifyCoupleMarry(TCPComponent *pCom,Global::stNotifyCoupleMarry * recvCmd);
    //通知结婚
	void notifyMarryInfoCb(TCPComponent *pCom,Global::stMarryInfoCb * recvCmd);
    //通知结婚
	void notifyForceDivorce(TCPComponent *pCom,Global::stRetForceDivorce * recvCmd);
    //通知结婚
	void notifyRejectDivorce(TCPComponent *pCom,Global::stRetRejectDivorce * recvCmd);
    //通知结婚
	void resetMarryRecord(TCPComponent *pCom,Global::stRetDelUserOnlineMarryInfo * recvCmd);
	//请求公会黑市等级
	void reqGuildBlackMarketLv(TCPComponent *pCom,Global::stReqSynGuildBlackMarketLv *recvCmd);
	//改名同步夫妇信息
	void reqsynCoupleInfo(TCPComponent *pCom,Global::stSynCoupleInfo *recvCmd);
	//更换头像同步
	void reqSynPhotoID(TCPComponent *pCom,Global::stSynUserPhotoID * recvCmd);
	//使用等级直升宝箱
	void reqUseLvlBox(TCPComponent *pCom,Global::stUseLvlBox * recvCmd);
	//云顶充值统计
	void reqRechangeStatistics(TCPComponent *pCom,Global::stRechangeStatistics * recvCmd);
	//nft查询结果
	void queryNftResult(TCPComponent *pCom,Global::stGame2GlobalQueryUserNftResult * recvCmd);
	//nft道具修改结果
	void changeNftItemResult(TCPComponent *pCom,Global::stGame2GlobalChangeNftItemResult * recvCmd);
	//发送nftVIP邮件
	void sendNftVipMail(TCPComponent *pCom,Global::stGame2GlobalNftVipGift * recvCmd);
};
#endif
