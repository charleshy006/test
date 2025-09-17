#ifndef __H_MISC_MOD_HANDLE_H__
#define __H_MISC_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "Player.h"
#include "GlobalProtoMsgHandleBase.h"
#include "Misc.pb.h"

using namespace net;
class MiscModHandle : public ProtoMsgHandle<MiscModHandle,Player>,public GlobalProtoMsgHandleBase
{
public :
	//构造
	MiscModHandle();
	//析构
	~MiscModHandle();
	//处理网络协议
	bool handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求读邮件
	void readMail(Player * pUser,MiscProto::stReqReadMail * recvCmd);
	//获得附件
	void fetchAttach(Player * pUser,MiscProto::stReqFetchAttach * recvCmd);
	//开打抢夺界面
	void openCaptureTreasureWin(Player * pUser,MiscProto::stReqOpenCaptureTreasure * recvCmd);
	//请求抢夺玩家列表
	void reqChallengeList(Player * pUser,MiscProto::stReqChallengeUserList * recvCmd);
	//请求世界boss伤害排行
	void reqBossWorldDamageSortList(Player * pUser,MiscProto::stReqWorldBossDamageRank * recvCmd);
	//请求世界boss活动匹配
	void reqMatchWorldBossActivity(Player * pUser,MiscProto::stReqBossWorldMatch * recvCmd);
	//请求开打世界boss活动界面
	void reqOpenWorldBossWin(Player * pUser,MiscProto::stReqOpenWorldBossWin * recvCmd);
	//世界boss取消匹配
	void cancelWorldBossMatch(Player * pUser,MiscProto::stCancelWorldBossMatch * recvCmd);
	//世界boss士气鼓舞
	void worldBossRaiseDamage(Player * pUser,MiscProto::stReqRaiseDamageInWorldBoss * recvCmd);
	//查询限时武神活动数据
	void queryDrawHeroInfo(Player * pUser,MiscProto::stQueryDrawHeroActivity * recvCmd);
	//领取限时武神活动积分奖励
	void getDrawHeroScoreReward(Player * pUser,MiscProto::stReqDrawHeroScoreReward * recvCmd);
	//领取限时武神活动排名奖励
	void getDrawHeroRankReward(Player * pUser,MiscProto::stReqDrawHeroRankReward * recvCmd);
	//请求补偿列表
	void reqCompensationList(Player * pUser,MiscProto::stReqCompensationList * recvCmd);
	//请求领取补偿
	void reqGotCompensation(Player * pUser,MiscProto::stReqGotCompensation * recvCmd);
	//打开护送界面
	void openGuardWin(Player * pUser,MiscProto::stReqOpenGuardWin * recvCmd);
	//请求某个护送任务详细信息
	void reqGuardDetailInfo(Player * pUser,MiscProto::stReqGuardDetailInfo * recvCmd);
	//重置拦截次数
	void resetInterceptNum(Player * pUser,MiscProto::stResetInterceptNum * recvCmd);
	//请求进入护送
	void reqIntoGuardWin(Player * pUser,MiscProto::stReqGuard * recvCmd);
	//刷新护送品质
	void refreshGuardQuality(Player *pUser,MiscProto::stReqRefreshGuardQuality * recvCmd);
	//请求可以协助的好友列表
	void reqCanHelpGuardFriend(Player *pUser,MiscProto::stReqCanHelpGuardFriend * recvCmd);
	//邀请好友协助
	void inviteFriendForGuard(Player *pUser,MiscProto::stInviteFriendForHelpGuard * recvCmd);
	//答复邀请
	void replyHelpGuard(Player * pUser,MiscProto::stReplyHelpGuard * recvCmd);
	//开始护送
	void beginGuard(Player * pUser,MiscProto::stBeginGuard * recvCmd);
	//查看战报
	void reqInterceptInfos(Player *pUser,MiscProto::stReqInterceptInfo * recvCmd);
	//请求拦截
	void reqIntercept(Player * pUser,MiscProto::stReqIntercept * recvCmd);
	//拦截结果
	void interceptResult(Player * pUser,MiscProto::stNotifyInterceptResult * recvCmd);
	//快速完成护送
	void quickFinishGuard(Player * pUser,MiscProto::stQuickFinishGuard * recvCmd);
	//一键获得附件
	void akeyfetchAttach(Player * pUser,MiscProto::stReqAKeyFetchAttach * recvCmd);
    //加载魔王召唤
	void loadAllDevilCall(Player * pUser, MiscProto::stReqAllDevilCall * recvCmd);
    //领取魔王召唤奖励
	void drawDevilCallReward(Player * pUser, MiscProto::stReqDevilCallReward * recvCmd);
    //魔王抽奖
	void doDevilCall(Player * pUser, MiscProto::stReqDevilCall * recvCmd);
    //获取魔王召唤摘要
    void doDevilCallSummary(Player * pUser, MiscProto::stReqDevilCallSummary * recvCmd);
	//请求活动具体详情
	void reqOperateActivity(Player * pUser, MiscProto::stReqOperateActvity * recvCmd);
	//请求活动奖励
	void reqOperateActivityReward(Player * pUser, MiscProto::stReqOperateActvityReward * recvCmd);
	//请求打开秘境寻宝窗口
	void reqOpenDiscoverWindow(Player * pUser, MiscProto::stRepOpenDiscoverWindow *recvCmd);
	//掷骰子
	void reqPlayDice(Player * pUser, MiscProto::stReqPlayDice * recvCmd);
	//领取、放弃任务
	void reqGetOrGiveUpTask(Player * pUser, MiscProto::stReqGetOrGiveUpTask * recvCmd);
	//刷新次数
	void reqRefreshNum(Player * pUser, MiscProto::stReqRefreshNum * recvCmd);
	//领取积分奖励
	void reqGetIntegralReward(Player * pUser, MiscProto::stReqGetIntegralReward * recvCmd);
	//领取奖励（非积分奖励）
	void reqGiveReward(Player * pUser, MiscProto::stReqGiveReward * recvCmd);
	//秘境寻宝红点规则
	void reqRedPointRuls(Player * pUser, MiscProto::stReqRedPointRuls *recvCmd);
	//请求分享信息
	void ReqInviteFriend(Player * pUser, MiscProto::stReqInviteFriend *recvCmd);
	//领取分享奖励
	void ReqDrawInviteFriend(Player * pUser, MiscProto::stReqDrawInviteFriend *recvCmd);
	//使用分享码领取奖励
	void ReqUseInviteCode(Player * pUser, MiscProto::stReqUseInviteCode *recvCmd);
};
#endif
