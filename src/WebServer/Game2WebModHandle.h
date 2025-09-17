#ifndef __H_GAME_WEB_HANDLE_H__
#define __H_GAME_WEB_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Game.pb.h"
#include "net.h"
#include "CommonMicro.h"


using namespace net;
class Game2WebModHandle : public ProtoMsgHandle<Game2WebModHandle>,public ProtoMsgHandleBase
{
public :
	//构造
	Game2WebModHandle();
	//析构
	~Game2WebModHandle();
	//处理网络协议
	bool handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//玩家升级日志
	void userLevelupLog(TCPComponent * pCom,Game::stUserLevelupLog * recvCmd); 
	//玩家登录日志
	void userLoginLog(TCPComponent * pCom,Game::stUserLoginLog * recvCmd);
	//vip等级提升日志
	void vipLevelupLog(TCPComponent * pCom,Game::stVipLevelupLog * recvCmd);
	//荣誉变化日志
	void honourChgLog(TCPComponent * pCom,Game::stHonourChgLog * recvCmd);
	//道具变化日志
	void itemChgLog(TCPComponent * pCom,Game::stItemChgLog * recvCmd);
	//资源变化日志
	void resourceChgLog(TCPComponent * pCom,Game::stResourceChgLog * recvCmd);
	//日常任务
	void dailyTaskLog(TCPComponent * pCom,Game::stDailyTaskLog * recvCmd);
	//充值日志
	void recharegeLog(TCPComponent * pCom,Game::stRechargeLog * recvCmd);
	//商店购买日志
	void mallBuyLog(TCPComponent * pCom,Game::stMallBuyLog * recvCmd);
	//武将变化日志
	void heroPropChgLog(TCPComponent * pCom,Game::stHeroPropChgLog * recvCmd);
	//发布活动返回
	void publishActivityResult(TCPComponent * pCom,Game::stPublishActivityResult * recvCmd);
	//下线活动返回
	void unloadActivityResult(TCPComponent * pCom,Game::stUnloadActivityResult * recvCmd);
	//公会日志
	void guildLog(TCPComponent * pCom,Game::stSendGuildLog * recvCmd);
	//公会资源日志
	void guildResLog(TCPComponent * pCom,Game::sstSendGuildResLog * recvCmd);
	//模块日志
	void moduleLog(TCPComponent * pCom,Game::stSendModuleLog * recvCmd);
	//技能变化日志
	void skillChgLog(TCPComponent * pCom,Game::stSkillChgLog * recvCmd);
	//精灵变化
	void spriteLog(TCPComponent * pCom,Game::stSpriteLog * recvCmd);
	//副本日志
	void copymapLog(TCPComponent * pCom,Game::stCopymapLog * recvCmd);
	//装备日志
	void equipLog(TCPComponent * pCom,Game::stEquipChgLog * recvCmd);
	//活动日志
	void activityLog(TCPComponent * pCom,Game::stActivityLog * recvCmd);
	//玩家删除返回
	void userCancelResult(TCPComponent * pCom, Game::stUserCancelRes * recvCmd);
	//保存玩家订单号等信息
	void reqPayType(TCPComponent * pCom, Game::stReqPayType * recvCmd);
};
#endif
