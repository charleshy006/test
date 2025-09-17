#ifndef __GATEUSER_H__
#define __GATEUSER_H__
#include "Element.h"
#include "CommonMicro.h"
#include "RoleInfo.h"
#include "PacketProcessor.h"
#include <map>

class GateUser : public Element
{
public :
	enum eGateState
	{
		GAME_STATE_INIT,									/// 初始状态
		GAME_STATE_SELECT,									/// 选择角色状态
		GAME_STATE_ENTER,									/// 等待进入状态 
		GAME_STATE_PLAY,									/// 游戏状态
		GAME_STATE_UNREG									/// 退出角色状态
	};
	static const UINT32 s_REQ_CLIENT_TIME_MAX_WAITTIME = 30; //等待15秒
	static const UINT32 s_REQ_CLIENT_TIME_INTERVAL = 900;	 //15分钟
	static const UINT32 s_MAX_SENDPACKET_ERROR_NUM = 3;
	static const UINT32 s_MAX_SEND_PACKET_PER_INTERVAL = 600; //最大发包数量
	static const UINT32 s_CHECK_PACKET_AMOUNT_INTERVAL = 15; //15秒	
	static const UINT32 s_NETWORK_DELAY = 50;				 //延迟
public :
	char       m_account[MAX_ACCNAMESIZE + 1];				//玩家帐号
private:
	eGateState				m_gameState;					//游戏状态
	UINT16 					m_gameID;						//游戏服务器ID
public :
	std::map<UINT32,stGateRoleInfo>		m_gateRoleInfo;		//网关的角色数据
	static bool 			s_forwardProtoTrace;			//转发协议打印
	UINT32					m_enterSelectTime;				//进入选择界面时间
	char					m_ip[MAX_IP_LENGTH + 1];		//登录者的ip
	std::string				m_deviceID;						//设备ID
	std::string 			m_channelID;					//渠道ID
	UINT32					m_lastSendPackTime;				//上次发送包时间
	UINT32					m_sendPackErrorNum;				//发包次数
	bool					m_bReqClientTime;				//是否请求了客户端时间
	UINT32					m_reqClientTimeSendPacketTime;	//请求客户端时间
	UINT32 					m_clientTime;					//客户端时间
	UINT64					m_recvClientTime;				//收到客户端时间的时间
	UINT32 					m_beginCheckPackNumTime;		//开始检查包时间
	UINT32					m_recvPacketNum;				//收到的包数量
	bool					m_bNeedRelogin;					//需要重新登录
public:
	GateUser(const char * account);
	virtual ~GateUser();
	//设置游戏状态
	void setGameState(eGateState _state) { m_gameState = _state; 		}
	//检查游戏状态
	bool isGameState(eGateState _state)  { return _state == m_gameState ;}
	//取得唯一ID
	virtual UINT32 getUniqueID()const
	{
		return this->getID();
	}
	//断开连接
	void terminate();
	//初始化角色信息到客户端
	bool initCharInfo(UINT32 charID);
	//注销角色
	void unreg(bool force = false);
	//进入选则角色阶段
	bool beginSelect();
	//开始游戏阶段
	void beginGame() {  setGameState(GAME_STATE_PLAY); }
	//发送消息给客户端
	bool sendProto(const void* pstrProto, const unsigned int cmdLen,UINT8 modID,UINT8 funID,const bool hasPacked = false);
	//通知从global下线
	void notifyGlobalLoginOut();
	//通知从game下线
	void notifyGameLoginOut();
	//设置游戏服务器ID
	void setGameID(UINT16 gameID) { m_gameID = gameID ;}
	//返回游戏服务器ID
	UINT16 getGameID() { return m_gameID ;}
	//收包检查
	void checkReceviePack(const PacketHeader *pHeader);
	//定时器
	void timer();
};

#endif
