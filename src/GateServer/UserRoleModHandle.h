#ifndef __H_USER_ROLE_HANDLE_H__
#define __H_USER_ROLE_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Role.pb.h"
#include "net.h"
#include "CommonMicro.h"

using namespace net;
class UserRoleModHandle : public ProtoMsgHandle<UserRoleModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	UserRoleModHandle();
	//析构
	~UserRoleModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//创建角色
	void createRole(TCPComponent * pCom,Role::ReqCreateRole * reqCreate);
	//进入游戏
	void enterGame(TCPComponent * pCom,Role::ReqEnterGame * enterGame);
	//切换角色
	void changeRole(TCPComponent * pCom,Role::stChangeRole * recvCmd);
};
#endif
