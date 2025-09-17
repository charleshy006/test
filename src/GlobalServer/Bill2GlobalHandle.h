#ifndef __H_BILL2GLOBAL_HANDLE_H__
#define __H_BILL2GLOBAL_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Bill.pb.h"
#include "net.h"

using namespace net;
class Bill2GlobalHandle : public ProtoMsgHandle<Bill2GlobalHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	Bill2GlobalHandle();
	//析构
	~Bill2GlobalHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//通知充值
	void notifyRecharge(TCPComponent * pCom,Bill::stNotifyUserRecharge * recvCmd);
};
#endif
