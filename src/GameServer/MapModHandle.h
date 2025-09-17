#ifndef __H_MAP_MOD_HANDLE_H__
#define __H_MAP_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Map.pb.h"
#include "net.h"
#include "GameUser.h"
#include "GameProtoMsgHandleBase.h"

using namespace net;
class MapModHandle : public ProtoMsgHandle<MapModHandle,GameUser>,public GameProtoMsgHandleBase
{
public :
	//构造
	MapModHandle();
	//析构
	~MapModHandle();
	//处理网络协议
	bool handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求进入关卡
	void reqEnterLevelMap(GameUser * pUser,Map::ReqEnterLevelMap * recvCmd);
	//请求离开关卡
	void reqLeaveLevelMap(GameUser * pUser,Map::stReqLeaveLevel * recvCmd);
	//请求移动
	void reqMove(GameUser * pUser,Map::ReqMove * recvCmd);
	//请求攻击
	void reqAttack(GameUser *pUser,Map::stThrowSkill * );
	//客户端加载完资源
	void clientFinishLoadRes(GameUser * pUser,Map::stClientLoadResFinish * recvCmd);
	//技能播放被打断
	void interruptPlaySkill(GameUser * pUser,Map::stInterruptPlaySkill * recvCmd);
	//切换方向
	void changeDir(GameUser * pUser,Map::stMapObjectChgDir * recvCmd);
	//碰撞
	void attackHit(GameUser * pUser,Map::stNotifyHit * recvCmd);
	//返回主城在线玩家
	void sendMainCityOnlineUser(GameUser * pUser,Map::stReqOnlineUserOnMainCity * recvCmd);
	//请求切换宠物
	void changeHeroGroup(GameUser * pUser,Map::stChangeHeroGroup * recvCmd);
	//切换主城
	void changeMainCity(GameUser * pUser,Map::stReqChangeMainCity * recvCmd);
	//计算雷电伤害
	void calcThunderDamage(GameUser * pUser,Map::stCalcThunderDamage * recvCmd);
};
#endif
