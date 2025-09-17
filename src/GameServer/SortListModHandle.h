#ifndef __H_SORTLIST_MOD_HANDLE_H__
#define  __H_SORTLIST_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "GameUser.h"
#include "GameProtoMsgHandleBase.h"
#include "SortList.pb.h"

using namespace net;
class SortListModHandle : public ProtoMsgHandle<SortListModHandle,GameUser>,public GameProtoMsgHandleBase
{
public :
	//构造
	SortListModHandle();
	//析构
	~SortListModHandle();
	//处理网络协议
	bool handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//挑战擂台玩家
	void changeLeiTaiUser(GameUser * pUser,SortProto::stChallegeLeitaiUser * recvCmd);
	//打开擂台界面
	void openLeitaiWindow(GameUser * pUser,SortProto::stOpenLeitaiWindow * recvCmd);
	//战斗结果
	void leitaiResult(GameUser * pUser,SortProto::stSendLeitaiResult * recvCmd);
	//选择模式
	void selectLeitaiMode(GameUser *pUser,SortProto::stSelectLeiTaiMode * recvCmd);
	//重置擂台信息
	void resetLeitaiInfo(GameUser * pUser,SortProto::stResetLeitaiInfo * recvCmd);
	//擂台里面移动宠物位置
	void moveHero(GameUser * pUser,SortProto::stLeitaiMoveHero2BattlePos * recvCmd);
	//取消出战
	void cancelGoOut(GameUser *pUser,SortProto::stLeitaiCancelGoOutForBattle * recvCmd);
	//提交竞技场结果
	void commitArenaResult(GameUser * pUser,SortProto::stCommitArenaResult * recvCmd);
	//请求进入擂台场景
	void enterLeitaiScene(GameUser * pUser,SortProto::stReqEnterLeitaiScene * recvCmd);
	//刷新擂台buff
	void refreshLeitaiBuff(GameUser * pUser,SortProto::stReqResetLeitaiBuff * recvCmd);
	//请求刷新buff时间
	void reqRefreshBuffTime(GameUser * pUser,SortProto::stReqBuffResetTime * recvCmd);
	//清除cd
	void clearArenaCD(GameUser * pUser,SortProto::stReqClearArenaCD * recvCmd);
    //请求远征信息
	void reqExpeditionInfo(GameUser * pUser, SortProto::stReqExpeditionInfo * recvCmd);
    //请求远征
	void reqExpedition(GameUser * pUser, SortProto::stReqExpedition * recvCmd);
    //请求远征奖励
	void reqExpeditionReward(GameUser * pUser, SortProto::stReqExpeditionReward * recvCmd);
    //请求快速完成远征
	void reqFastExpedition(GameUser * pUser, SortProto::stReqFastExpedition * recvCmd);
    //请求取消远征
	void reqCancelExpedition(GameUser * pUser, SortProto::stReqCancelExpedition * recvCmd);
    //请求远征摘要
	void reqExpeditionSummary(GameUser * pUser, SortProto::stReqExpeditionSummary * recvCmd);
	//请求1v1匹配
	void reqOne2OnePVPMatch(GameUser * pUser,SortProto::stReqOne2OnePVPMatch * recvCmd);
	//取消1v1匹配
	void cancelOne2OnePVPMatch(GameUser * pUser,SortProto::stCancelOne2OnePVPMatch * recvCmd);
	//请求进入翡翠梦境
	void reqEnterEmeraldDream(GameUser * pUser, SortProto::stReqEnterEmeraldDream * recvCmd);
	//重置翡翠梦境挑战次数
	void reqResetFightNum(GameUser *pUser, SortProto::stResetFightNum * recvCmd);
	//请求翡翠梦境剩余次数
	void reqSurplusNum(GameUser *pUser, SortProto::stReqSurplusNum * recvCmd);
	//英雄联赛匹配
	void reqHeroLeagueMatch(GameUser * pUser,SortProto::stReqHeroLeagueMatch * recvCmd);
	//取消英雄联赛匹配
	void cancelHeroLeagueMatch(GameUser * pUser,SortProto::stCancelHeroLeagueMatch * recvCmd);
	//请求匹配的角色
	void reqReqMenberInfo(GameUser * pUser,SortProto::stReqMenberInfo * recvCmd);
	//朝拜
	void reqWorskip(GameUser * pUser,SortProto::stReqWorship *recvCmd);
	//英雄联赛 初赛、复赛报名次数
	void reqRegisterNum(GameUser * pUser,SortProto::stReqRegister *recvCmd);
	//pub上线
	void reqPubLogIn(GameUser * pUser,SortProto::stReqPubLogIn *recvCmd);
	//pub下线
	void reqPubLogOut(GameUser * pUser,SortProto::stReqPubLogOut *recvCmd);
	//英雄联赛红点
	void reqHeroLeagueRedPoint(GameUser * pUser,SortProto::stReqRedPoint *recvCmd);
	//请求巨龙巢穴
	void reqDragonNest(GameUser * pUser,SortProto::stReqDragonNest *recvCmd);
	//挑战巨龙巢穴
	void reqChallengeDragonNest(GameUser * pUser,SortProto::stReqChallengeDragonNest *recvCmd);
	//重置次数
	void reqResetChallengeDragonNest(GameUser * pUser,SortProto::stReqResetChallengeDragonNest *recvCmd);
	//请求宝石副本信息
	void reqStoneCopyMapInfo(GameUser * pUser,SortProto::stReqStoneCopyMapInfo *recvCmd);
	//挑战宝石副本
	void reqChallengeStoneCopyMap(GameUser * pUser,SortProto::stReqChallengeStoneCopyMap *recvCmd);
	//重置宝石副本次数
	void reqResetStoneCopyMap(GameUser * pUser,SortProto::stReqResetStoneCopyMapNum *recvCmd);
};
#endif
