#ifndef __H_GAMEPROTO_MSG_HANDLE_BASE_H__
#define __H_GAMEPROTO_MSG_HANDLE_BASE_H__
#include "GameUser.h"

class GameProtoMsgHandleBase
{
public :
	//构造
	GameProtoMsgHandleBase() {}
	//析构
	virtual ~GameProtoMsgHandleBase() {}
	//处理协议
	virtual bool handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID) = 0;
};

#endif
