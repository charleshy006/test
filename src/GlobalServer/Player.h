#ifndef __PLAYERUSER_H__
#define  __PLAYERUSER_H__
#include "Element.h"
#include "RoleInfo.h"
#include "GlobalServer.h"
#include "Global.pb.h"
#include "FriendMgr.h"
#include "OpenFunction.h"
#include <set>

class Player;
class GlobalOpenFunction : public COpenFunction
{
	public :
		GlobalOpenFunction(Player * pUser);
		virtual ~GlobalOpenFunction();
		virtual UINT16 getLevel();
	private :
		Player *m_pOwner;
};

class Player : public Element
{
	private:
		static UINT32 		m_userCount;					//用户数量
		UINT16 				m_gateID;						//网关ID
		UINT16				m_gameID;						//游戏ID
		std::string			m_account;						//账户
		char 				m_ip[MAX_IP_LENGTH + 1];		//登录的IP
	public :
		UINT8               m_gmType;                   	//gm类型
		UINT16 				m_level;						//等级
		UINT8				m_job;							//职业
		UINT8 				m_sex;							//性别
		UINT32 				m_loginTime;					//登录时间
		CFriendMgr			m_friendMgr;					//好友管理
		UINT32				m_battlePower;					//战力
		UINT32 				m_maxHP;						//最大血量
		UINT32 				m_fetchLilianNum;				//领取历练次数
		UINT32				m_toGameSvrID;					//跳转到的服务器
		UINT16				m_serverID;						//服务器ID
		UINT32				m_opID;							//运营商ID
		std::string			m_deviceID;						//设备ID
		UINT16				m_vipLv;						//vip等级
		UINT32				m_headID;						//头部ID
		UINT32				m_bodyID;						//身体ID
		UINT32 				m_weaponID;						//武器ID
		UINT32				m_cloackID;						//披风ID
		UINT32				m_wingID;						//翅膀ID
		UINT32				m_shoeID;						//鞋子ID
		UINT32				m_fashionHairID;				//时装头发
		UINT32				m_fashionFaceID;				//时装脸ID
		UINT32				m_fashionBodyID;				//时装身体
		UINT32				m_fashionWeaponID;				//时装武器
		UINT32				m_fashionShoeID;				//时装鞋子
		UINT32				m_fashionDecorationID;			//时装饰品
		UINT32				m_compensationID;				//补偿ID
		UINT32 				m_effectID;						//特效ID
		UINT32 				m_interceptID;					//护送劫持的玩家ID
		std::string 		m_channelID;					//渠道ID
		GlobalOpenFunction  m_openFuncMgr;					//开放等级
		UINT32				m_wingBreakLv;					//翅膀突破等级
		UINT8				m_changeNameNum;				//改名次数
		UINT32				m_pubServerIndex;				//PubServerID
		UINT32				m_pubCharID;					//Pub上的charID
		UINT32  			m_areaCode;   			 		//区服ID
		UINT16				m_photoID;						//头像ID
	public:
		//构造函数
		Player(UINT16 gateID,const std::string account,const char * ip);
		//析构函数
		virtual ~Player();
		//初始化数据
		bool init(const RoleInfo & roleInfo);
		//玩家注销
		void unreg();
		//定时回调
		void timerUpdate(UINT32 cur);
		//向网关发送消息
		template <class T>
		bool sendProto2Gate(const T & cmd);
		//向game发送消息
		template <class T>
		bool sendProto2Game(const T & cmd);
		//获得网关连接
		UINT16 getGateID() { return m_gateID ;}
		//设置游戏服务器
		void  setGameID(UINT16 gameID)  { m_gameID = gameID;  }
		//获得游戏服务器的ID
		UINT16 getGameID() { return m_gameID ; }
		//获得账号
		std::string getAccount() {  return m_account ;}
		//根据计数类型获得计数次数
		UINT32 getCountByVip(UINT16 countType);
		//给自己发送协议
		template <class T>
		bool  sendProtoToMe(const T & cmd);
		//获得登录ip
		const char * getIP() { return m_ip ;}
		//设置登录ip
		void setIP(const char * szIP);
};

extern GlobalServer * g_globalServer;
template <class T>
bool Player::sendProto2Gate(const T & cmd)
{
	return g_globalServer->getConnMgr().broadcastByID(m_gateID,cmd);
}
template <class T>
bool Player::sendProto2Game(const T & cmd)
{
	return g_globalServer->getConnMgr().broadcastByID(m_gameID,cmd);
}

template <class T>
bool  Player::sendProtoToMe(const T & cmd)
{
	Global::stForwardProto2Me  forwardMe;
	forwardMe.set_charid(getID());
	forwardMe.set_modid(cmd.ModID);
	forwardMe.set_funid(cmd.FunID);
	std::string retStr;
	cmd.SerializeToString(&retStr);
	forwardMe.set_cmdlen(retStr.size());
	forwardMe.set_data(retStr);
	g_globalServer->getConnMgr().broadcastByID(m_gateID,forwardMe);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送协议 funid:%d modid:%d", cmd.FunID, cmd.ModID);
#endif
	return true;	
}
#endif
