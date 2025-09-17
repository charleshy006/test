#ifndef __H_GLOBAL_GATE_HANDLE_H__
#define __H_GLOBAL_GATE_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Global.pb.h"
#include "net.h"
#include "CommonMicro.h"


using namespace net;
class GlobalGateModHandle : public ProtoMsgHandle<GlobalGateModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	GlobalGateModHandle();
	//析构
	~GlobalGateModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//处理返回快照数据协议
	void retSnapData(TCPComponent * pCom,Global::stRetSnapData * recvCmd);
	//转发协议到客户端
	void forwardProto2Client(TCPComponent * pCom,Global::stForwardProto2Me * recvCmd);
	//处理用户登录失败
	void userLoginGlobalFail(TCPComponent * pCom,Global::stLoginGlobalFail * recvCmd);
	//创建用户结果
	void createRoleRet(TCPComponent * pCom,Global::stCreateRoleResult *recvCmd);
	//有game启动了
	void gameSvrStartUp(TCPComponent * pCom,Global::stGameSvrStartUp * recvCmd);
	//给所有人发送协议
	void broadcastProto2All(TCPComponent * pCom,Global::stBroadcastProto2AllUser * recvCmd);
	//踢人下线
	void killOutUser(TCPComponent * pCom,Global::stKillUserOut *recvCmd);
	//服务器配置信息
	void serverCfg(TCPComponent * pCom,Global::stNotifyGateSvrCfg * recvCmd);
	//是否是新用户
	void isNewAccount(TCPComponent * pCom,Global::stIsNewAccount * recvCmd);
};
#endif
