#ifndef __H_USER_LOGIN_HANDLE_H__
#define __H_USER_LOGIN_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Login.pb.h"
#include "net.h"
#include "CommonMicro.h"
 

struct stLoginTokenInfo
{
	//UINT32 m_accountID;
	char   m_account[MAX_ACCNAMESIZE + 1];
	UINT32 m_time;
	char   m_loginToken[MAX_LOGIN_VERIFY_STR_LEN];
	std::string m_ip;
	std::string m_channelID;					//渠道ID

	stLoginTokenInfo()
	{   
		//m_accountID = 0;
		bzero(m_account,sizeof(m_account));
		m_time = 0;
		bzero(m_loginToken,sizeof(m_loginToken));
	}   
};

using namespace net;
class UserLoginModHandle : public ProtoMsgHandle<UserLoginModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	UserLoginModHandle();
	//析构
	~UserLoginModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//外网的验证方式
	bool formatVerifyToken(const char * loginToken,stLoginTokenInfo & one);
	//验证token
	bool verifyToken(const char * loginToken,stLoginTokenInfo & one);
	//处理用户登录
	void userReqLogin(TCPComponent * pCom,Login::ReqLoginGate  * reqLogin);
	//请求时间
	void reqSvrTime(TCPComponent * pCom,Login::stReqServerTime * recvCmd);
	//自动连接请求
	void autoConnectGate(TCPComponent * pCom,Login::stAutoLoginGate * recvCmd);
	//查询ping值
	void queryPingValue(TCPComponent * pCom,Login::stQueryPingValue * recvCmd);
	//收到了玩家客户端时间
	void recClientTime(TCPComponent * pCom,Login::stSendClient2Svr * recvCmd);	
    //sdk的验证方式
    bool verifySdk(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one);
    //魔域的验证方式
    bool verifyMoeYoo(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one);
	//星彼的验证方式
	bool verifyStarpy(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one);
	//TipcatSDK验证方式
	bool verifyTipcat(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one);
	//小米sdk验证方式
	bool verifyXiaoMi(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one);
	//雪糕sdk验证方式
	bool verifyXueGao(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one);
	//Sagegame sdk验证登录
	bool verifySagegame(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//1tsdk sdk验证登录
	bool verify1tsdk(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//3hgame sdk验证登录
	bool verify3hgame(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//_01game sdk验证登录
	bool verify01game(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//quick sdk验证登录
	bool verifyQuicksdk(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//quick海外版本 sdk验证登录
	bool verifyQuicksdkHW(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//br sdk验证登录
	bool verifyBRsdk(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//xinyu sdk验证登录
	bool verifyXinyu(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//东南亚 sdk验证登录
	bool verifyDongNanYa(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//huawei sdk验证登录
	bool verifyHuaWei(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//51sfsy SDK验证登录
	bool verify51sfsy(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//通用SDK验证登录
	bool verifyCommon(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//Laosiji SDK验证登录
	bool verifyLaosiji(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
	//工口sdk
	bool verifyGongKou(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one);
};
#endif
