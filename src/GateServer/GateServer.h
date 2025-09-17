#ifndef __GATESERVER_H__
#define __GATESERVER_H__
#include <vector>
#include <string>
#include "SvrInfo.h"
#include "Log4cxxHelp.h"
#include "net.h"
#include "ServerBase.h"
#include "LoadStat.h"
#include <map>
using namespace net;
class GateServer : public ServerBase {
public:
	//构造函数 
    GateServer();
	//析构函数
    ~GateServer();
	//开始
    bool start();
	//结束
    void stop();
	//加载所有客户端
	bool loadAllClient();
	//分配game服务器
	UINT16 assignGameSvr2User(std::string account);
	//连接game服务器
	bool connectGame(SvrInfo & svrInfo);	
	//链接global服务器
	bool connectGlobal(SvrInfo & svrInfo);
	//是否内部验证方式
	bool isInnerVerify()
	{
		return m_innerVerify;
	}
	//是否统计协议
	bool isStatProto()
	{
		return m_forwardProtoTrace;
	}
	//禁止登录
	bool isBanLogin()
	{
		return m_banLogin;
	}
public :
	CNetConnMgr m_netConnMgr;
	stProtoStatHelp m_protoStat;									//协议统计
private :
	UINT32 m_maxGateUser;											//网关最大人数	
	bool m_statNetProto;
	bool m_needVerifyToken;
	bool m_forwardProtoTrace;
	bool m_innerVerify;												//内部验证方式
	//std::map<UINT16,UINT16> m_zoneID2GameID;
	std::vector<UINT16> m_gameSvrIDVec;
	bool m_banLogin;												//禁止登录
};
#endif

