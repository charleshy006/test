#ifndef __H_NORMAL_2_PUB_MOD_HANDLE_H__
#define __H_NORMAL_2_PUB_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "PubGame.pb.h"
#include "net.h"
#include "GameProtoMsgHandleBase.h"

using namespace net;
class Normal2PubModHandle : public ProtoMsgHandle<Normal2PubModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	Normal2PubModHandle();
	//析构
	~Normal2PubModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//normal登录pub
	void loginPubGame(TCPComponent * pCom,PubGame::stLoginPubGame * loginCmd);
	//处理normalgame转发来的消息
	void normalForwardPub(TCPComponent * pCom,PubGame::stNormalForwardPub * recvCmd);
	void getRoleInfo(TCPComponent * pCom,PubGame::stRoleInfo * recvCmd);
	//logout
	void logout(TCPComponent * pCom,PubGame::stRoleLogout * recvCmd);
	//实时pvp 组队匹配
	void realTimeTeamMatch(TCPComponent * pCom,PubGame::stRealTimeTeamMatch * recvCmd);
	//英雄联赛匹配
	void reqHeroLeagueMatch(TCPComponent * pCom,PubGame::stHeroLeagueMatch * recvCmd);
	//取消匹配
	void reqcancelMatch(TCPComponent * pCom,PubGame::stHeroLeagueCancelMatch *recvCmd);
	//参加复赛角色列表
	void reqQualifiedList(TCPComponent * pCom,PubGame::stSynPubRoleListResult *recvCmd);
	//请求匹配的角色
	void reqReqMenberInfo(TCPComponent * pCom,PubGame::stReqMenberInfo * recvCmd);
private :
	std::map<UINT8,GameProtoMsgHandleBase *> m_protoModHandles;
};
#endif
