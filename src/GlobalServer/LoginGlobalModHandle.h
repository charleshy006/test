#ifndef __H_LOGIN_GLOBALMOD_HANDLE_H__
#define __H_LOGIN_GLOBALMOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Global.pb.h"
#include "net.h"

using namespace net;
class LoginGlobalModHandle : public ProtoMsgHandle<LoginGlobalModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	LoginGlobalModHandle();
	//析构
	~LoginGlobalModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//处理登录
	void loginGlobal(TCPComponent * pCom,Global::stLoginGlobal * loginCmd);	
};
#endif
