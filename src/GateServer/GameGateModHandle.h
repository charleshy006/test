#ifndef __H_GAME_GATE_HANDLE_H__
#define __H_GAME_GATE_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Game.pb.h"
#include "net.h"
#include "CommonMicro.h"


using namespace net;
class GameGateModHandle : public ProtoMsgHandle<GameGateModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	GameGateModHandle();
	//析构
	~GameGateModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//转发协议到客户端
	void forwardProto2Client(TCPComponent * pCom,Game::stGameForwardGate * recvCmd);
	//玩家登录game结果
	void userLoginGameResult(TCPComponent * pCom,Game::stUserLoginGameResult * recvCmd);
	//给所有玩家发送协议
	void sendProto2AllClient(TCPComponent * pCom,Game::stBroadbastProto2World * recvCmd);
	//发送协议给特定的玩家
	void sendProto2UserList(TCPComponent * pCom,Game::stBroadcastProto2UserList * recvCmd);
	//踢玩家下线
	void killOutUser(TCPComponent * pCom,Game::stKillOutUser * recvCmd);
	//同步玩家的等级
	void notifyUserLv(TCPComponent * pCom,Game::stNotifyLevelUp  * recvCmd);
};
#endif
