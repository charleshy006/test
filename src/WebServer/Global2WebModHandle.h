#ifndef __H_GLOBAL2WEB_MOD_HANDLE_H__
#define __H_GLOBAL2WEB_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Global.pb.h"
#include "net.h"

using namespace net;
class Global2WebModHandle : public ProtoMsgHandle<Global2WebModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	Global2WebModHandle();
	//析构
	~Global2WebModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//踢人返回
	void kickUserOutResult(TCPComponent * pCom,Global::stKillUserOutResult * recvCmd);
	//设置GM
	void setGM(TCPComponent * pCom,Global::stSetUserGMTypeResult * recvCmd);
	//封号解封
	void banUser(TCPComponent * pCom,Global::stBanUserResult * recvCmd);
	//发送邮件返回
	void sendMailResult(TCPComponent * pCom,Global::stSendMail2UserResult * recvCmd);
	//返回服务器配置
	void retSvrCfg(TCPComponent * pCom,Global::stRetSvrConfig * recvCmd);	
	//获取在线人数
	void updateOnlineNum(TCPComponent * pCom,Global::stNotifyWebOnlineNum * recvCmd);
	//进入了选择界面
	void enterSelectPage(TCPComponent * pCom,Global::stEnterSelectPage * recvCmd);
	//账号创建了角色
	void onCreateRole(TCPComponent * pCom,Global::stNotifyWebCreateRole * recvCmd);
	//公会日志
	void guildLog(TCPComponent * pCom,Global::stSendGuildLog * recvCmd);
	//公会资源日志
	void guildResLog(TCPComponent * pCom,Global::sstSendGuildResLog * recvCmd);
	//模块日志
	void moduleLog(TCPComponent * pCom,Global::stSendModuleLog * recvCmd);
	//修改玩家信息
	void modifyUserInfo(TCPComponent * pCom,Global::stWebMotifyUserInfoRet * recvCmd);
	//领取腾讯福利中心礼包返回
	void retGetTxGiftBag(TCPComponent * pCom,Global::stRetGetTxGiftBag * recvCmd);
	//离线资源变化日志
	void resourceChgLog(TCPComponent * pCom, Global::stResourceChgLog * recvCmd);
	//使用分享码返回
	void useInviteCodeCb(TCPComponent * pCom, Global::stUseInviteCodeCb * recvCmd);
	//请求同步限时随从活动数据
	void reqLimitPetActivityData(TCPComponent * pCom, Global::stReqLimitPetActivityData * recvCmd);
	//查询nft结果
	void queryNftResult(TCPComponent * pCom, Global::stGlobal2WebQueryUserNftResult * recvCmd);
	//修改nftItem结果
	void changeNftItemResult(TCPComponent * pCom, Global::stGlobal2WebChangeNftItemResult * recvCmd);

};
#endif
