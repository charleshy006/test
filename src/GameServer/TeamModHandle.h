#ifndef __H_TEAM_MOD_HANDLE_H__
#define __H_TEAM_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "GameUser.h"
#include "GameProtoMsgHandleBase.h"
#include "Team.pb.h"

using namespace net;
class TeamModHandle : public ProtoMsgHandle<TeamModHandle,GameUser>,public GameProtoMsgHandleBase
{
public :
	//构造
	TeamModHandle();
	//析构
	~TeamModHandle();
	//处理网络协议
	bool handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);

	//实时战场单人匹配
	void realTimeSingleMatch(GameUser * pUser,TeamProto::stReqRealTimeSingleMatch * recvCmd);
	//取消单人匹配
	void cancelRealTimeSingleMatch(GameUser * pUser,TeamProto::stReqCancelRealTimeSingleMatch * recvCmd);
	//取消实时战场组队匹配
	void cancelRealTimeTeamMatch(GameUser * pUser,TeamProto::stReqCancelRealTimeTeamMatch * recvCmd);
};
#endif
