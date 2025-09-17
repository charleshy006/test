#ifndef __H_WEBMOD_HANDLE_H__
#define __H_WEBMOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Global.pb.h"
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
	//处理踢人
	void killUserOut(TCPComponent * pCom,Global::stReqKillUserOut * recvCmd);
	//设置GM
	void setGM(TCPComponent * pCom,Global::stReqSetUserGMType * recvCmd);
	//封号,解封
	void banUser(TCPComponent * pCom,Global::stReqBanUser * recvCmd);
	//发送邮件
	void sendMail(TCPComponent * pCom,Global::stSendMail2User * recvCmd);
	//修改玩家信息
	void modifyUserInfo(TCPComponent * pCom,Global::stWebMotifyUserInfo * recvCmd);
	//增加或修改公告
	void opNotice(TCPComponent * pCom,Global::stAddOrUpdateNotice * recvCmd);
	//删除公告
	void delNotice(TCPComponent * pCom,Global::stDelNotice * recvCmd);
	//通知充值
	void notifyRecharge(TCPComponent * pCom,Global::stWebNotifyRechargeInfo * recvCmd);
	//增加礼包配置
	void addGiftBagConfig(TCPComponent * pCom,Global::stAddGiftBagCfg * recvCmd);
	//删除礼包配置
	void removeGiftBagConfig(TCPComponent * pCom,Global::stDelGiftBagCfg * recvCmd);
	//设置黑白名单
	void addBlackWhite(TCPComponent * pCom,Global::stAddBlackAndWhite * recvCmd);
	//设置维护时间
	void setMaintanenceTime(TCPComponent * pCom,Global::stSetServerMaintenanceTime * recvCmd);
	//获得服务器配置
	void getSvrCfg(TCPComponent * pCom,Global::stGetSvrConfig * recvCmd);
	//更新服务器配置
	void updateSvrCfg(TCPComponent * pCom,Global::stUpdateSvrCfg * recvCmd);
	//领取tx福利中心礼包
	void reqGetTxGiftBag(TCPComponent * pCom,Global::stReqGetTxGiftBag * recvCmd);
	//更新服务器开服时间
	void updateSvrOpenTime(TCPComponent * pCom,Global::stUpdateSvrOpenTime * recvCmd);
	//请求使用分享码
	void reqUseInviteCode(TCPComponent * pCom,Global::stUseInviteCode * recvCmd);
    //更新限时随从活动数据
    void updateLimitPetActivityData(TCPComponent * pCom, Global::stSendLimitPetActivityData * recvCmd);
    //根据指令关闭服务器
    void sendShutDown(TCPComponent * pCom, Global::stSendShutDown * recvCmd);
	
    //查询nft
    void queryNft(TCPComponent * pCom, Global::stWeb2GlobalQueryUserNft * recvCmd);
    //修改nft道具
    void changeNftItem(TCPComponent * pCom, Global::stWeb2GlobalChangeNftItem * recvCmd);
};
#endif
