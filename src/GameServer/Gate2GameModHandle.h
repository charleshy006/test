#ifndef __H_GATE_2_GAME_MOD_HANDLE_H__
#define __H_GATE_2_GAME_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Game.pb.h"
#include "net.h"
#include "GameProtoMsgHandleBase.h"

using namespace net;
class Gate2GameModHandle : public ProtoMsgHandle<Gate2GameModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	Gate2GameModHandle();
	//析构
	~Gate2GameModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//处理下线
	void loginOut(TCPComponent * pCom,Game::stKillOutUser * revCmd);
	//出来来自网关的协议
	void gateForwardGame(TCPComponent * pCom,Game::stGateForwardGame * recvCmd);
private :
	std::map<UINT8,GameProtoMsgHandleBase *> m_protoModHandles;
};
#endif
