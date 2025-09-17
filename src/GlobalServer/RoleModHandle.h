#ifndef __H_ROLE_MOD_HANDLE_H__
#define __H_ROLE_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "Player.h"
#include "GlobalProtoMsgHandleBase.h"
#include "Role.pb.h"

using namespace net;
class RoleModHandle : public ProtoMsgHandle<RoleModHandle,Player>,public GlobalProtoMsgHandleBase
{
public :
	//构造
	RoleModHandle();
	//析构
	~RoleModHandle();
	//处理网络协议
	bool handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求玩家详细数据
	void reqUserDetailInfo(Player * pUser,Role::stReqUserDetailInfo * recvCmd);
	//请求礼包奖励
	void reqGiftBag(Player * pUser,Role::stReqGiftBag * recvCmd);
	//请求领取奖励
	void reqCloseBetaReward(Player *pUser,Role::stReqGotCloseBetaReward * recvCmd);
	//请求改名
	void reqChangeName(Player *pUser,Role::stReqChangeName * recvCmd);
	//
	void reqInviteToRoom(Player *pUser,Role::stReqInviteToRoom * recvCmd);
	//
	void reqAgreeEnterWedding(Player *pUser,Role::stReqAgreeEnterWedding * recvCmd);
	//
	void reqUseFirework(Player *pUser,Role::stReqUseFirework * recvCmd);
	//
	void reqTakeCandy(Player *pUser,Role::stReqTakeCandy * recvCmd);
	//
	void reqBuyFirework(Player *pUser,Role::stReqBuyFirework * recvCmd);
	//
	void reqEnterWeddingAlone(Player *pUser,Role::stReqEnterWeddingAlone * recvCmd);
	//
	void reqQuitWeddingScene(Player *pUser,Role::stReqQuitWeddingScene * recvCmd);
    //
	void reqDrawWeddingRing(Player *pUser,Role::stReqDrawWeddingRing * recvCmd);

};
#endif

