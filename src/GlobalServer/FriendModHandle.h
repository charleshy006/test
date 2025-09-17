#ifndef __H_FRIEND_MOD_HANDLE_H__
#define __H_FRIEND_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "Player.h"
#include "GlobalProtoMsgHandleBase.h"
#include "Friend.pb.h"

using namespace net;
class FriendModHandle : public ProtoMsgHandle<FriendModHandle,Player>,public GlobalProtoMsgHandleBase
{
public :
	//构造
	FriendModHandle();
	//析构
	~FriendModHandle();
	//处理网络协议
	bool handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求增加一个好友
	void reqAddFriend(Player * pUser,FriendProto::stReqAddOneFriend * recvCmd);
	//请求删除好友
	void reqDelFriend(Player * pUser,FriendProto::stDelFriend * recvCmd);
	//查找添加好友
	void findAndAddFriend(Player * pUser,FriendProto::stFindAndAddFriend * recvCmd);
	//请求申请列表
	void reqApplyList(Player * pUser,FriendProto::stReqApplyAddFriendList * recvCmd);
	//请求推荐好友列表
	void reqRecommondFriend(Player * pUser,FriendProto::stReqRecommondFriend * recvCmd);
	//赠送历练
	void giveLilian(Player * pUser,FriendProto::stGiveLilian2Friend * recvCmd);
	//请求历练列表
	void reqLilianList(Player * pUser,FriendProto::stReqReceiveLilian * recvCmd);
	//提取历练
	void fetchLilian(Player * pUser,FriendProto::stFetchLilian * recvCmd);
	//添加到对方的申请列表
	void add2ApplyList(Player * pUser,FriendProto::stReqAdd2ApplyList * recvCmd);
	//角色对方添加我为好友
	void refuseAddMeFriend(Player * pUser,FriendProto::stRefuseAddFriend * recvCmd);
	//约战邀请
	void invitePK(Player * pUser,FriendProto::stInvitePKWithFriend * recvCmd);
	//答复邀请
	void replyPKInvite(Player * pUser,FriendProto::stReplyFriendPKInvite * recvCmd);
	//求婚
	void reqPropose(Player *pUser, FriendProto::stReqPropose *recvCmd);
	//客户端回复答不答应求婚
	void reqAgreeMarry(Player *pUser, FriendProto::stReqAgreeMarry *recvCmd);
	//其中一方按了大叉
	void reqCancelMarry(Player *pUser, FriendProto::stReqCancelMarry *recvCmd);
};
#endif
