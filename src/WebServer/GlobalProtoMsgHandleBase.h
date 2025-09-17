#ifndef __H_GLOBALPROTO_MSG_HANDLE_BASE_H__
#define __H_GLOBALPROTO_MSG_HANDLE_BASE_H__
#include "Player.h"

class GlobalProtoMsgHandleBase
{
public :
	//构造
	GlobalProtoMsgHandleBase() {}
	//析构
	virtual ~GlobalProtoMsgHandleBase() {}
	//处理协议
	virtual bool handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID) = 0;
};

#endif
