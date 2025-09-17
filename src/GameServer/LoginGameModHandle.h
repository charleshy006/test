#ifndef __H_LOGIN_GAMEMOD_HANDLE_H__
#define __H_LOGIN_GAMEMOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Game.pb.h"
#include "net.h"

using namespace net;
class LoginGameModHandle : public ProtoMsgHandle<LoginGameModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	LoginGameModHandle();
	//析构
	~LoginGameModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//处理登录
	void loginGame(TCPComponent * pCom,Game::stLoginGame * loginCmd);	
};
#endif
