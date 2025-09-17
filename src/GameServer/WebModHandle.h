#ifndef __H_WEBMOD_HANDLE_H__
#define __H_WEBMOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Game.pb.h"
#include "net.h"

using namespace net;
class WebModHandle : public ProtoMsgHandle<WebModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	WebModHandle();
	//析构
	~WebModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//接受发布活动
	void publishActivity(TCPComponent * pCom,Game::stPublishActivity * recvCmd);
	//卸载活动
	void unloadActivity(TCPComponent * pCom,Game::stUnloadActivity * recvCmd);
	//玩家删除
	void userCancel(TCPComponent * pCom, Game::stUserCancel * recvCmd);
	//获取订单查询结果
	void retPayType(TCPComponent * pCom, Game::stReqPayTypeCb * recvCmd);
	//更新服务器开服时间
	void updateSvrOpenTime(TCPComponent * pCom, Game::stUpdateSvrOpenTime * recvCmd);
	//更新限时随从活动数据
	void updateLimitPetActivityData(TCPComponent * pCom, Game::stSendLimitPetActivityData * recvCmd);
};
#endif
