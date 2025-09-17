#ifndef __H_PUB_2_NORMAL_MOD_HANDLE_H__
#define __H_PUB_2_NORMAL_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Game.pb.h"
#include "net.h"
#include "GameProtoMsgHandleBase.h"

using namespace net;
class Pub2NormalModHandle : public ProtoMsgHandle<Pub2NormalModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	Pub2NormalModHandle();
	//析构
	~Pub2NormalModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//下发至客户端
	void forwardMe(TCPComponent * pCom,PubGame::stPubForwardGate * recvCmd);
	//通知normal用户是否在跨服
	void notifyOnPub(TCPComponent * pCom,PubGame::stNotifyUserOnPub * recvCmd);
	//跨服通知1v1结果
	void notifyOne2OneResult(TCPComponent * pCom,PubGame::stNotifyOne2OneResult * recvCmd);
	//跨服通知实时3v3pk结果
	void notifyRealTimeResult(TCPComponent * pCom,PubGame::stNotifyRealTimeResult * recvCmd);
	//英雄联赛增加报名次数
	void notifyAddRegisterNum(TCPComponent * pCom,PubGame::stAddRegisterNum * recvCmd);
	//英雄联赛跨服角色信息
	void notifyAttendPreUser(TCPComponent * pCom,PubGame::stNotifyHeroLeagueResult *recvCmd);
	//开赛前同步参赛角色信息
	void notifySynRoleInfo(TCPComponent * pCom,PubGame::stSynPubRoleList *recvCmd);
	//同步冠军信息
	void synChampionInfo(TCPComponent * pCom,PubGame::stReqSynChampionInfo *recvCmd);
private :
	// std::map<UINT8,GameProtoMsgHandleBase *> m_protoModHandles;
};
#endif
