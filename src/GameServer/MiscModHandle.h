#ifndef __H_MISC_MOD_HANDLE_H__
#define __H_MISC_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Map.pb.h"
#include "net.h"
#include "GameUser.h"
#include "GameProtoMsgHandleBase.h"
#include "Misc.pb.h"

using namespace net;
class MiscModHandle : public ProtoMsgHandle<MiscModHandle,GameUser>,public GameProtoMsgHandleBase
{
public :
	//构造
	MiscModHandle();
	//析构
	~MiscModHandle();
	//处理网络协议
	bool handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求奖励数据
	void reqRewardInfo(GameUser *pUser,MiscProto::stReqRewardInfo * recvCmd);
	//请求领取签到奖励
	void reqSignReward(GameUser *pUser,MiscProto::stReqSignReward * recvCmd);
	//请求领取开服奖励
	void reqOpenSvrReward(GameUser *pUser,MiscProto::stReqOpenSvrReward * recvCmd);
	//请求等级奖励
	void reqLvReward(GameUser *pUser,MiscProto::stReqLvReward * recvCmd);
	//请求在线奖励
	void reqOnlineReward(GameUser * pUser,MiscProto::stReqOnlineReward * recvCmd);
	//激活某个称号
	void activeTitle(GameUser * pUser,MiscProto::stActiveOneTitle * recvCmd);
	//取消激活某个称号
	void cancelActiveTitle(GameUser * pUser,MiscProto::stCancelActiveTitle * recvCmd);
	//请求打星
	void buyStar(GameUser * pUser,MiscProto::stReqBuyStar * recvCmd);
	//请求打开抽奖界面
	void openDrawWindow(GameUser * pUser,MiscProto::ReqOpenDrawWindow * recvCmd);
	//请求进行抽奖
	void reqDraw(GameUser * pUser,MiscProto::stReqDraw * recvCmd);
	//购买投资计划
	void buyInvestPlan(GameUser * pUser,MiscProto::stBuyInvestPlan * recvCmd);
	//请求投资计划奖励
	void investPlanReward(GameUser *pUser,MiscProto::stReqInvestPlanReward * recvCmd);
	//吃大餐
	void eatDinner(GameUser *pUser,MiscProto::stReqEatDinner * recvCmd);
	//每日充值奖励
	//void everydayRechargeReward(GameUser * pUser,MiscProto::stReqEveryDayRechargeReward * recvCmd);
	//请求
	//void consumeReward(GameUser * pUser,MiscProto::stReqConsumeReward * recvCmd);
	//宝藏合成
	void treasureCombine(GameUser * pUser,MiscProto::stReqCombineTreasure * recvCmd);
	//购买和平模式时间
	void buyPeaceModuleTime(GameUser *pUser,MiscProto::stBuyPeaceModuleTime * recvCmd);
	//累积登录奖励
	//void cumulativeLoginReward(GameUser *pUser,MiscProto::stReqCumulativeReward * recvCmd);
	//抢夺
	void captureTreasure(GameUser * pUser,MiscProto::stCaptureUserTreasure * recvCmd);
	//解锁精灵
	void unlockedSprite(GameUser * pUser,MiscProto::stReqUnlockedSprite * recvCmd);
	//升级精灵
	void levelupSprite(GameUser * pUser,MiscProto::stReqLevelupSprite * recvCmd);
	//精灵跟随
	void spriteFollow(GameUser * pUser,MiscProto::stSpriteFollowMaster * recvCmd);
	//请求可以进行的活动
	void reqAvaibleActivity(GameUser * pUser,MiscProto::stReqAvaibleActivity * recvCmd);
	//请求活动奖励
	void reqActivityReward(GameUser * pUser,MiscProto::stReqAcitivityReward * recvCmd);
	//世界boss清cd
	void reqClearWorldBossCD(GameUser * pUser,MiscProto::stReqClearWorldBossActivityCD * recvCmd);
	//7天活动信息
	void req7DayActivityInfo(GameUser * pUser,MiscProto::stReq7DayActivityInfo * recvCmd);
	//请求领取7填活动奖励
	void req7DayActivityReward(GameUser * pUser,MiscProto::stReqSevenDayReward * recvCmd);
	//请求充值奖励活动列表
	void reqRechargeActivity(GameUser * pUser,MiscProto::stReqRechargeActivityList * recvCmd);
	//充值活动奖励
	void rechargeActivityReward(GameUser * pUser,MiscProto::stReqRechargeActivtiyReward * recvCmd);
	//【召唤计划】当前进度 请求 
	void reqSummonData(GameUser * pUser, MiscProto::stReqSummonData * recvCmd);
	//【召唤计划】领奖 请求
	void reqSummonReward(GameUser * pUser, MiscProto::stReqSummonReward * recvCmd);
	//【充值】校验 请求 
	void reqRechargeCheck(GameUser * pUser, MiscProto::stReqRechargeCheck * recvCmd);
	//限时武神积分奖励
	void reqDrawHeroScoreReward(GameUser * pUser,MiscProto::stReqDrawHeroScoreReward * recvCmd);
	//问卷调查
	void reqInvestgateReward(GameUser *pUser,MiscProto::stReqInvestgationReward * recvCmd);	
	//请求商店活动列表
	void reqShopActivityList(GameUser * pUser,MiscProto::stReqActivityShop * recvCmd);
	//请求领取幸运商店
	void reqLuckShopReward(GameUser *pUser,MiscProto::stReqLuckShopReward * recvCmd);
	//保存幸运商店时间
	void saveLuckOpenWinTime(GameUser * pUser,MiscProto::stSaveOpenLuckShopWinTime *recvCmd);
	//清除拦截cd
	void clearInterceptCD(GameUser *pUser,MiscProto::stResetInterceptCD * recvCmd);
	//护送鼓舞士气
	void guardInspired(GameUser * pUser,MiscProto::stReqGuardInspired * recvCmd);
	//召唤最高品质的护送宝藏
	void summonHighQualityGuard(GameUser *pUser,MiscProto::stReqSummonHighQualityGuard * recvCmd);
	//打开抽奖限时时装界面
	void openDrawLimitFashionWin(GameUser * pUser,MiscProto::stReqOpenLimitFashionDrawWin * recvCmd);
	//请求限时时装
	void drawLimitFashionWin(GameUser * pUser,MiscProto::stReqDrawLimitFashion * recvCmd);
	//请求兑换物品
	void reqExchangeItem(GameUser * pUser,MiscProto::stReqExchangeItem * recvCmd);
	//请求领取每日循环充值奖励
	void reqRecycleReward(GameUser * pUser,MiscProto::stReqRecycleRechargeActReward * recvCmd);
	//请求购买荣耀时装
	void reqGloryfashion(GameUser * pUser,MiscProto::stReqGloryfashion * recvCmd);
    //请求精灵幻化信息
	void reqIllusionSprite(GameUser * pUser,MiscProto::stReqIllusionSprite * recvCmd);
    //请求喂养幻化精灵
	void reqIllusionSpriteEat(GameUser * pUser,MiscProto::stReqIllusionSpriteEat * recvCmd);
    //请求升级幻化精灵技能
	void reqIllusionSpriteSkillUp(GameUser * pUser,MiscProto::stReqIllusionSpriteSkillUp * recvCmd);
	//请求充值渠道配置
	void reqPayType(GameUser * pUser, MiscProto::stReqPayType * recvCmd);
	//
	void ReqLuckyDraw(GameUser * pUser, MiscProto::stReqLuckyDraw * recvCmd);
	//
	void ReqLuckyDrawReward(GameUser * pUser, MiscProto::stReqLuckyDrawReward * recvCmd);
	//
	void ReqLuckyDrawPointReward(GameUser * pUser, MiscProto::stReqLuckyDrawPointReward * recvCmd);
	void ReqShareInvite(GameUser * pUser, MiscProto::stReqShareInvite * recvCmd);
	void ReqDailyShareInvite(GameUser * pUser, MiscProto::stReqDailyShareInvite * recvCmd);
	void ReqDrawDailyShareReward(GameUser * pUser, MiscProto::stReqDrawDailyShareReward * recvCmd);
	void ReqDrawOnceShareReward(GameUser * pUser, MiscProto::stReqDrawOnceShareReward * recvCmd);
	//请求每日首冲奖励领取状态
	void reqDailyReceiveState(GameUser * pUser, MiscProto::stReqReceiveState *recvCmd);
	//请求领取每日首冲奖励领
	void reqReceiveReward(GameUser * pUser, MiscProto:: stReqReceiveReward *recvCmd);
	//请求领取限时福利礼包
	void reqDrawLateWalfare(GameUser * pUser, MiscProto::stReqDrawLateWalfare *recvCmd);
	//请求找回次数
	void reqDrawResGetBack(GameUser * pUser, MiscProto::stReqDrawResGetBack *recvCmd);
	//请求一键找回
	void reqDrawAllResGetBack(GameUser * pUser, MiscProto::stReqDrawAllResGetBack *recvCmd);
	//请求打开小怪兽窗口
	void reqOpenMonsterWindow(GameUser * pUser, MiscProto::stReqOpenMonsterWindow *recvCmd);
	//请求攻击小怪兽
	void reqAttackMonster(GameUser * pUser, MiscProto::stReqAttackMonster * recvCmd);
	//请求购买次数
	void reqBuyAttackTime(GameUser * pUser, MiscProto::stReqBuyAttackTime * recvCmd);
	//请求一键攻击
	void reqAKeyAttackMonster(GameUser * pUser, MiscProto::stReqAKeyAttackMonster * recvCmd);
	//请求坐骑升级
	void reqMountLevelUp(GameUser * pUser, MiscProto::stReqMountLevelUp *recvCmd);
	//请求装备坐骑
	void reqButtleMount(GameUser * pUser, MiscProto::stReqButtleMount * recvCmd);
	//请求骑乘坐骑
	void reqMountFollow(GameUser * pUser, MiscProto::stReqMountFollow * recvCmd);
	//请求坐骑幻化
	void reqMountUnreal(GameUser * pUser, MiscProto::stReqMountUnreal * recvCmd);
	//请求打开坐骑成长窗口
	void reqOpenGrowWindow(GameUser * pUser, MiscProto::stReqOpenMountGrowWindow * recvCmd);
	//请求领取冲级好礼
	void reqReceiveWflfare(GameUser * pUser, MiscProto::stReqReceiveWflfare * recvCmd);
	//请求充值签名
	void reqPaySign(GameUser * pUser, MiscProto::stReqPaySign * recvCmd);
	//请求服务器充值
	void reqServerPay(GameUser * pUser, MiscProto::stReqServerPay * recvCmd);
};
#endif
