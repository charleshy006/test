#ifndef __H_GATE_2_GAME_MOD_HANDLE_H__
#define __H_GATE_2_GAME_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Global.pb.h"
#include "net.h"

using namespace net;
class Global2GameModHandle : public ProtoMsgHandle<Global2GameModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	Global2GameModHandle();
	//析构
	~Global2GameModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//处理玩家登录game服务器
	void globalRetRoleInfo(TCPComponent * pCom,Global::stRetRoleInfo * recvCmd);
	//重新登录
	void reloginGame(TCPComponent * pCom,Global::stReloginGame * recvCmd);
	//自动重连
	void autoConnectGame(TCPComponent * pCom,Global::stNotifyGameAutoConnect * recvCmd);
	//玩家充值
	void userRecharge(TCPComponent * pCom,Global::stUserRecharge * recvCmd);
	//同步玩家的领取历练次数
	void synFetchLilianNum(TCPComponent * pCom,Global::stSynFetchLilianNum * recvCmd);
	//创建公会成功扣钱
	void createGuildDecMoney(TCPComponent *pCom,Global::stCreateGuildDecMoney * recvCmd);
	//同步公会数据
	void synMyGuildInfo(TCPComponent *pCom,Global::stSynGuildID * recvCmd);
	//祈福
	void guildPray(TCPComponent *pCom,Global::stGuildPray * recvCmd);
	//宝藏合成成功
	void treasureCombineSuccess(TCPComponent *pCom,Global::stTreasureCombineSuccess * recvCmd);
	//确认开始换服
	void confirmChgSvr(TCPComponent * pCom,Global::stConfirmChangeGameSvr * recvCmd);
	//给某个玩家返回其它玩家的详细数据
	void sendOther2Me(TCPComponent * pCom,Global::stRetOtherUserData2Me * recvCmd);
	//获得邮件的附件
	void obtainMailAttach(TCPComponent * pCom,Global::stObtainItemFromMail * recvCmd);
	//发送pk玩家信息
	void pkWithOther(TCPComponent * pCom,Global::stBeginPKWithOther * recvCmd);
	//玩家离开了公会
	void userLeaveGuild(TCPComponent * pCom,Global::stUserLeaveGuild * recvCmd);
	//玩家组队进入副本
	void teamEnterCopymap(TCPComponent * pCom,Global::stTeamEnterCopyMap * recvCmd);
	//巅峰对决进入副本
	void one2OnePVPEnterCopymap(TCPComponent * pCom,Global::stOne2OnePVPEnterCopyMap * recvCmd);
	//one2one 和机器人pk
	void one2OnePKWithRobot(TCPComponent * pCom,Global::stOne2OnePkWithRobot * recvCmd);
	//夺宝奖励
	void captureTreasureReward(TCPComponent * pCom,Global::stCaptureReward * recvCmd);
	//夺宝失败
	void captureTreasureFail(TCPComponent * pCom,Global::stCaptureTreasureFail * recvCmd);
	//实时战场进入副本
	void realTimePKEnterCopymap(TCPComponent * pCom,Global::stRealTimeBattleEnterCopymap * recvCmd);
	//增加历练
	void notifyAddLilian(TCPComponent * pCom,Global::stNotifyAddLilian * recvCmd);
	//进入公会领地
	void enterGuildCopymap(TCPComponent * pCom,Global::stEnterGuildCopymap * recvCmd);
	//设置玩家GM值
	void setGMValue(TCPComponent * pCom,Global::stOnlineUserGMType * recvCmd);
	//封玩家的号
	void setForbidUser(TCPComponent * pCom,Global::stForbidOnlineUser * recvCmd);
	//发送活动配置
	void activityConfig(TCPComponent * pCom,Global::stSynActivityConfig * recvCmd);
	//发送排名信息
	void rankModuleInfo(TCPComponent * pCom,Global::stSynRankModuleInfo * recvCmd);
	//礼包奖励
	void giveGiftBagReward(TCPComponent * pCom,Global::stGiveGiftBagReward * recvCmd);
	//通知服务器配置
	void serverCfg(TCPComponent * pCom,Global::stNotifyGameSvrCfg * recvCmd);
	//上线通知首杀信息
	void userOnlineNotifyFirstKillInfo(TCPComponent * pCom,Global::stUserOnlineSendFirstKillInfo * recvCmd);
	//通知首杀信息
	void notifyFirstKillInfo(TCPComponent * pCom,Global::stTeamFinishFirstKill * recvCmd);
	//抽取武将积分奖励
	void drawHeroScoreReward(TCPComponent * pCom,Global::stDrawHeroScoreReward * recvCmd);
	//抽取武将排名奖励
	void drawHeroRankReward(TCPComponent * pCom,Global::stDrawHeroRankReward * recvCmd);
	//设置补偿ID
	void setCompensationID(TCPComponent * pCom,Global::stSetCompensationID * recvCmd);
	//设置开服天数
	void setOpenServerDay(TCPComponent * pCom,Global::stSetOpenServerDay * recvCmd);
	//设置公会等级
	void synAllMemberGuildLv(TCPComponent * pCom,Global::stSynGuildLv2AllMember * recvCmd);
	//世界boss购买
	void worldBossBuyDecDiamond(TCPComponent * pCom,Global::stWorldBossBuyDecDiamond * recvCmd);
	//通知竞技场奖励
	void notifyArenaReward(TCPComponent * pCom,Global::stNotifyArenaReward * recvCmd);
	//GM修改玩家数据
	void gmModifyUserInfo(TCPComponent * pCom,Global::stGMModifyUserInfo * recvCmd);
	//设置我的竞技场排名
	void notifyMyArenaRank(TCPComponent * pCom,Global::stSynMyArenaRank * recvCmd); 
	//处理竞技场重置请求
	void reqResetArena(TCPComponent * pCom,Global::stReqResetArenaChallengeNum * recvCmd);
	//重置拦截次数
	void reqResetInterceptNum(TCPComponent *pCom,Global::stReqResetInterceptNum * recvCmd);
	//重新刷新宝藏品质
	void refreshGuardQuality(TCPComponent *pCom,Global::stReqRefreshGuardQuality * recvCmd);
	//清空1v1积分
	void clearOne2OneScore(TCPComponent *pCom,Global::stNotifyClearOne2OneScore * recvCmd);
	//获得拦截奖励
	void interceptReward(TCPComponent *pCom,Global::stInterceptReward * recvCmd);
	//直接完成护送扣钱
	void quickFinishGuard(TCPComponent *pCom,Global::stQuickFinishDecDiamond * recvCmd);
	//通知开始护送
	void notifyBeginGuard(TCPComponent *pCom,Global::stNotifyUserBeginGuard * recvCmd);
	//通知拦截结束
	void notifyInterceptResult(TCPComponent *pCom,Global::stNotifyUserInterceptResult * recvCmd);
	//通知内侧奖励
	void notifyCloseBetaReward(TCPComponent *pCom,Global::stNotifyCloseBetaReward * recvCmd);
	//进入领地战地图
	void intoManorFightMap(TCPComponent *pCom,Global::stEnterManorFightMap * recvCmd);
	//改名
	void changeName(TCPComponent *pCom,Global::stExistName * recvCmd);
    //获取远征信息
	void getExpeditionInfo(TCPComponent *pCom, Global::stExpeditionInfo * recvCmd);
    //远征出征
	void reqExpedition(TCPComponent *pCom, Global::stExpedition * recvCmd);
    //领取远征奖励
	void reqExpeditionReward(TCPComponent *pCom, Global::stExpeditionReward * recvCmd);
    //快速远征
	void reqFastExpedition(TCPComponent *pCom, Global::stFastExpedition * recvCmd);
    //取消远征
	void cancelExpedition(TCPComponent *pCom, Global::stCancelExpedition * recvCmd);
    //获取远征摘要
	void reqExpeditionSummary(TCPComponent *pCom, Global::stExpeditionSummary * recvCmd);
    //加载魔王召唤
	void loadDevilCalls(TCPComponent *pCom, Global::stAllDevilCall * recvCmd);
    //领取魔王宝箱奖励
	void drawDevilCallReward(TCPComponent *pCom, Global::stDevilCallReward * recvCmd);
    //魔王召唤
	void devilCall(TCPComponent *pCom, Global::stDevilCall * recvCmd);
    //获取魔王召唤摘要
	void devilCallSummary(TCPComponent *pCom, Global::stDevilCallSummary * recvCmd);
    //赠送好友历练每日任务回调
	void giveLilianCb(TCPComponent *pCom, Global::stGiveLilianDayTaskCb * recvCmd);
    //新加活动领取奖励
	void operateActivityReward(TCPComponent *pCom, Global::stOperateActivityReward * recvCmd);
    //新加活动抽奖
	void operateActivityLotteryReward(TCPComponent *pCom, Global::stOperateActivityLotteryReward * recvCmd);
	//打开翡翠梦境窗口
	void openWindow(TCPComponent *pCom, Global::stUserRankInfoResult * recvCmd);
	//扣除翡翠梦境剩余次数
	void removeNum(TCPComponent *pCom, Global::strRemoveNum * recvCmd);
	//进入翡翠梦境
	void enterEmeraldDream(TCPComponent *pCom, Global::stCopyMapInfoResult * recvCmd);
	//跨服pk 队伍匹配
	void realTimeTeamMatch(TCPComponent *pCom, Global::stRealTimeTeamMatch * recvCmd);
	//扣钱
	void reqDeductMoney(TCPComponent *pCom, Global::stDeductMoney *recvCmd);
	//给奖励
	void reqGiveReward(TCPComponent *pCom, Global::stGiveReward *recvCmd);
	//请求进入任务关卡
	void reqEnterTaskLv(TCPComponent *pCom, Global::stReqEnterTaskLv * recvCmd);
	//请求领取分享好友数量奖励
	void ReqDrawInviteReward(TCPComponent *pCom, Global::stReqDrawInviteReward * recvCmd);
	//请求领取使用分享码奖励
	void ReqDrawUseInviteReward(TCPComponent *pCom, Global::stReqDrawUseInviteReward * recvCmd);
	//请求联赛参加复赛名单列表
	void reqQualifiedList(TCPComponent *pCom, Global::stSynRoleListResult *recvCmd);
	//朝拜
	void reqWorskip(TCPComponent *pCom, Global::stWorskipResult *recvCmd);
	//英雄联赛红点
	void reqHeroLeaugeRedPoint(TCPComponent *pCom, Global::stRedPointResult *recvCmd);
	//发送组队信息给玩家
	void sendRoomInfo(TCPComponent *pCom, Global::stRetRoomInfo *recvCmd);
	//发送新加组队信息给玩家
	void notifyAddTeamMember(TCPComponent *pCom, Global::stRetAddTeamMember *recvCmd);
	//求婚
	void reqPropose(TCPComponent *pCom, Global::stReqPropose *recvCmd);
	//同意结婚
	void reqAgreeMarry(TCPComponent *pCom, Global::stReqAgreeMarry *recvCmd);
	//退出结婚场景
	void reqQuitWedding(TCPComponent *pCom, Global::stReqQuitWedding *recvCmd);
	//重新进入场景
	void reqReEnterWedding(TCPComponent *pCom, Global::stReqReEnterWedding *recvCmd);
	//重新进入场景
	void reqBuyFireWork(TCPComponent *pCom, Global::stReqBuyFireWork *recvCmd);
	//重新进入场景
	void reqAgreeInvite(TCPComponent *pCom, Global::stReqAgreeInvite *recvCmd);
	//重新进入场景
	void reqTakeCandy(TCPComponent *pCom, Global::stReqTakeCandy *recvCmd);
	//重新进入场景
	void reqSendWeddingRing(TCPComponent *pCom, Global::stSendWeedingRing *recvCmd);
	//重新进入场景
	void reqUseFireWork(TCPComponent *pCom, Global::stUseFireWork *recvCmd);
	//重新进入场景
	void reqCoupleInfo(TCPComponent *pCom, Global::stRetMarryInfoToCouple *recvCmd);
	//重新进入场景
	void notifyCoupleMarryCb(TCPComponent *pCom, Global::stNotifyCoupleMarryCb *recvCmd);
	//重新进入场景
	void notifyCoupleForceDivorceCb(TCPComponent *pCom, Global::stRetForceDivorceCb *recvCmd);
	//同步公会黑市等级
	void reqSynGuildBlackMarketLv(TCPComponent *pCom, Global::stSynGuildBlackMarketLvResult *recvCmd);
	//秘境寻宝消耗次数
	void notifyDiscoverConsume(TCPComponent *pCom, Global::stDiscoverConsume *recvCmd);
	//使用等级直升宝箱
	void reqUseLvlBox(TCPComponent *pCom, Global::stUseLvlBoxRes *recvCmd);
	//查询用户nft
	void querynft(TCPComponent *pCom, Global::stGlobal2GameQueryUserNft *recvCmd);
	//修改用户nft道具
	void changeNftItem(TCPComponent *pCom, Global::stGlobal2GameChangeNftItem *recvCmd);
	//查询离线用户nft
	void nftOfflineQuery(TCPComponent *pCom, Global::stRetNftOfflineUserData *recvCmd);
	//修改离线用户nft道具
	void changeNftItemOffline(TCPComponent *pCom, Global::stRetChangeNftOfflineUserData *recvCmd);
};
#endif
