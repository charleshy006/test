#ifndef __H_GATEMOD_HANDLE_H__
#define __H_GATEMOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "GlobalProtoMsgHandleBase.h"
#include "Global.pb.h"
#include "net.h"

using namespace net;
class GateModHandle : public ProtoMsgHandle<GateModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	GateModHandle();
	//析构
	~GateModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//创建角色
	void createRole(TCPComponent * pCom,Global::stCreateRole * createCmd);
	//请求快照
	void reqSnapData(TCPComponent * pCom,Global::stReqSnapData * snapDataCmd);	
	//请求加载数据
	void reqLoadData(TCPComponent * pCom,Global::stReqLoadData * recvCmd);
	//通知玩家下线
	void notifyLogout(TCPComponent * pCom,Global::stNotifyLoginOut * loginOutCmd);
	//网管转发
	void gateForward(TCPComponent * pCom,Global::stGateForwardProto * recvCmd);
	//重新登录
	void reLoginGlobal(TCPComponent * pCom,Global::stReloginGlobal * recvCmd);
	//自动重连
	void autoConnect(TCPComponent * pCom,Global::stPlayerAutoConnect * recvCmd);
	//玩家失去连接了
	void clientLostConnect(TCPComponent * pCom,Global::stClientLostConnect * recvCmd);
    //是否是新创用户
    void askIsNewAccount(TCPComponent * pCom,Global::stAskIsNewAccount * recvCmd);
private :
	std::map<UINT8,GlobalProtoMsgHandleBase *> m_protoModHandles;
};
#endif
