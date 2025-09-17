#ifndef __H_GUILD_MOD_HANDLE_H__
#define __H_GUILD_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "GameUser.h"
#include "GameProtoMsgHandleBase.h"
#include "GuildProto.pb.h"

using namespace net;
class GuildModHandle : public ProtoMsgHandle<GuildModHandle,GameUser>,public GameProtoMsgHandleBase
{
public :
	//构造
	GuildModHandle();
	//析构
	~GuildModHandle();
	//处理网络协议
	bool handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求创建公会
	void createGuild(GameUser *pUser,GuildProto::stReqCreateGuild * recvCmd);
	//加入公会
	void joinGuild(GameUser * pUser,GuildProto::stReqJoinInGuild *recvCmd);
	//捐赠
	void donate(GameUser *pUser,GuildProto::stReqDonate * recvCmd);
	//更改徽章
	void changeBadge(GameUser * pUser,GuildProto::stReqChangeBadge * recvCmd);
};
#endif
