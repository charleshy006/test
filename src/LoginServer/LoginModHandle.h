#ifndef __H_LOGIN__HANDLE_H__
#define __H_LOGIN__HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Login.pb.h"
#include "net.h"
#include "CommonMicro.h"

class LoginServerAdapter;
using namespace net;
class LoginModHandle : public ProtoMsgHandle<LoginModHandle>,public ProtoMsgHandleBase,public PacketSender
{
public :
	//构造
	LoginModHandle();
	//析构
	~LoginModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//计算md5
	bool md5String(const char * account,const UINT32 & userID,const UINT32 & curTime,char * outStr);
	//验证账号成功
	void verifyAccount(TCPComponent * pCom,Login::ReqLogin * recvCmd);
};
#endif
