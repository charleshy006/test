#ifndef __H_TEAM_MOD_HANDLE_H__
#define __H_TEAM_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "Player.h"
#include "GlobalProtoMsgHandleBase.h"
#include "Team.pb.h"

using namespace net;
class TeamModHandle : public ProtoMsgHandle<TeamModHandle,Player>,public GlobalProtoMsgHandleBase
{
public :
	//构造
	TeamModHandle();
	//析构
	~TeamModHandle();
	//处理网络协议
	bool handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求创建队伍
	void reqCreateTeam(Player * pUser,TeamProto::stReqCreateTeam * recvCmd);
	//请求进入队伍
	void reqEnterTeam(Player * pUser,TeamProto::stReqEnterTeam * recvCmd);
	//离开队伍
	void reqLeaveTeam(Player * pUser,TeamProto::stReqLeaveTeam * recvCmd);
	//解散队伍
	void fireTeam(Player * pUser,TeamProto::stReqFireTeam  *recvCmd);
	//广播发送协议
	void broadcastInviteCmd(Player * pUser,TeamProto::stReqBroadcastInviteCmd * recvCmd);
	//请求自动匹配队伍
	void autoMatchTeam(Player * pUser,TeamProto::stReqAutoMatchTeam * recvCmd);
	//取消自动匹配
	void cancelAutoMatchTeam(Player * pUser,TeamProto::stCancelAutoMatchTeam * recvCmd);
	//邀请加入队伍
	void inviteJoinInTeam(Player * pUser,TeamProto::stInviteJoinInTeam * recvCmd);
	//答复邀请
	void replyInviteJoinTeam(Player *pUser,TeamProto::stReplyJoinInTeam * recvCmd);
	//请求进入副本
	void enterCopyMap(Player * pUser,TeamProto::stReqEnterLv * recvCmd);
	//请求某个关卡的全服首杀信息
	void reqFirstKillInfo(Player * pUser,TeamProto::stReqFirstKillInfoByID * recvCmd);
	//实时战场单人匹配
	void realTimeSingleMatch(Player * pUser,TeamProto::stReqRealTimeSingleMatch * recvCmd);
	//取消单人匹配
	void cancelRealTimeSingleMatch(Player * pUser,TeamProto::stReqCancelRealTimeSingleMatch * recvCmd);
	//实时战场组队匹配
	void realTimeTeamMatch(Player *pUser,TeamProto::stReqRealTimeTeamMatch * recvCmd);
	//取消实时战场组队匹配
	void cancelRealTimeTeamMatch(Player * pUser,TeamProto::stReqCancelRealTimeTeamMatch * recvCmd);
};
#endif
