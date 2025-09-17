#ifndef __H_SORTLIST_MOD_HANDLE_H__
#define __H_SORTLIST_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "Player.h"
#include "GlobalProtoMsgHandleBase.h"
#include "SortList.pb.h"

using namespace net;
class SortListModHandle : public ProtoMsgHandle<SortListModHandle,Player>,public GlobalProtoMsgHandleBase
{
public :
	//构造
	SortListModHandle();
	//析构
	~SortListModHandle();
	//处理网络协议
	bool handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求等级排行
	void reqLevelSortList(Player * pUser,SortProto::stReqLevelSortList * recvCmd);
	//请求战力排行
	void reqBattlePowerSortList(Player * pUser,SortProto::stReqBattlePowerSortList * recvCmd);
	//请求副本排行
	void reqCopymapSortList(Player * pUser,SortProto::stReqCopymapSortList * recvCmd);	
	//请求竞技场排行榜
	void reqArenaSortList(Player * pUser,SortProto::stReqArenaSortList * recvCmd);
	//请求竞技场pk对象列表
	void reqArenaPKList(Player * pUser,SortProto::stReqArenaPKList * recvCmd);
	//请求巅峰对决排名
	void reqOne2OneSortList(Player * pUser,SortProto::stReqOne2OnePVPSortList * recvCmd);
	//竞技场请求挑战某个玩家
	void reqChallengeArenaUser(Player * pUser, SortProto::stChallengeArenaUser * recvCmd);
	//请求擂台排行榜信息
	void reqLeitaiRank(Player * pUser,SortProto::stReqLeitaiSortList * recvCmd);
	//请求武神抽奖排行
	void reqDrawHeroSortList(Player * pUser,SortProto::stReqDrawHeroSortList * recvCmd);
	//请求巅峰对决匹配
	void reqOne2OnePVPMatch(Player * pUser,SortProto::stReqOne2OnePVPMatch * recvCmd);
	//取消巅峰对决
	void cancelOne2OnePVPMatch(Player * pUser,SortProto::stCancelOne2OnePVPMatch * recvCmd);
	//返回自己的排名信息
	void sendMySortInfo(Player * pUser,SortProto::stReqMySortListInfo * recvCmd);
	//请求重置挑战次数
	void resetChallengeNum(Player * pUser,SortProto::stReqResetArenaChallengeNum *recvCmd);
	//开服战力排行
	void reqOpenSvrBPRank(Player * pUser,SortProto::stReqOpenSvrBPRank * recvCmd);
	//请求荣耀时装排名
	void reqGloryfashionSortList(Player * pUser,SortProto::stReqGloryfashionSortList * recvCmd);
    //请求远征信息
	void reqExpeditionInfo(Player * pUser, SortProto::stReqExpeditionInfo * recvCmd);
    //请求远征
	void reqExpedition(Player * pUser, SortProto::stReqExpedition * recvCmd);
    //请求远征奖励
	void reqExpeditionReward(Player * pUser, SortProto::stReqExpeditionReward * recvCmd);
    //请求快速完成远征
	void reqFastExpedition(Player * pUser, SortProto::stReqFastExpedition * recvCmd);
    //请求取消远征
	void reqCancelExpedition(Player * pUser, SortProto::stReqCancelExpedition * recvCmd);
    //请求远征摘要
	void reqExpeditionSummary(Player * pUser, SortProto::stReqExpeditionSummary * recvCmd);
	//请求翡翠梦境排行
	void reqEmeraldDreamSortList(Player * pUser, SortProto::stReqEmeraldDreamSortList * recvCmd);
	//请求打开英雄联赛窗口
	void reqOpenHeroLeagueWindow(Player * pUser, SortProto::stReqOpenLeagueWindow *recvCmd);
	//请求联赛资格名单
	void reqQualifiedList(Player * pUser,SortProto::stReqQualifiedList * recvCmd);
	//历届英雄列表
	void reqHeroList(Player * pUser,SortProto::stReqHeroList * recvCmd);
};
#endif
