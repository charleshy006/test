#ifndef __H_ROOM_H__
#define __H_ROOM_H__
#include "TypesDef.h"
#include "GameUser.h"
#include "Team.pb.h"
#include "UniqueID.h"
#include "GameUserManager.h"
#include <ext/hash_map>
#include <set>

// void teamEnterCopymap(Global::stTeamEnterCopyMap * recvCmd);
void realTimePKEnterCopymap(Global::stRealTimeBattleEnterCopymap * recvCmd);

struct stTeamMember
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_level;						//玩家等级
	UINT8  m_job;						//玩家职业
	UINT8  m_sex;						//性别

	stTeamMember()
	{
		reset();
	}

	void reset()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_job = 0;
		m_sex = 0;
	}

	void fillSynInfo(TeamProto::stSynMemberInfo  & data);
};

class CTeam
{
public :
	static const UINT8 s_MAX_MEMBER = 3;	//一个房间的最大成员数量 
public :
	//构造函数
	CTeam();
	//析构函数 
	~CTeam();
	//返回id
	UINT32 getID() { return m_id ;}
	//设置房间ID
	void setID(UINT32 id) { m_id = id;}
	//获得leaderID
	UINT32 getLeaderID() { return m_members[0].m_charID ;}
	//返回队伍人数
	UINT8 getMemSize();
	//出事化房间
	bool initTeam(GameUser *pOwner,UINT32 copyMapID);
	//发送数据给客户端
	void sendTeamInfo(GameUser * pUser);
	//用户上线
	void userOnline(GameUser * pUser);
	//用户下线
	void userOffline(GameUser *pUser);
	//加入房间
	bool joinInTeam(GameUser * pUser,UINT8 pos);
	//给所有人发送协议
	template <class T>
	void sendProto2All(const T & cmd,UINT32 except = 0);
	//离开房间
	bool leaveTeam(UINT32 charID);
	//定时回调
	void timer(UINT32 cur);
	//增加一个邀请名单
	void addInviteInfo(UINT32 charID);
	//是否要邀请队列中
	bool isInInviteList(UINT32 charID);
	//从邀请队列中删除
	void eraseCharIDFromInviteList(UINT32 charID);
	//设置新的地图
	void setCopymapID(UINT32 copyMapID) {  m_copyMapID = copyMapID ;}
	//进入副本
	void enterCopyMap();
	//离开队伍
	void leaveCopyMap();
	//是否在副本中
	bool isInCopyMap() { return m_hasEnterCopyMap ; }
	//获得某个位置上的人charID
	UINT32 getCharID(UINT8 pos)
	{
		if (pos < s_MAX_MEMBER){
			return m_members[pos].m_charID;
		}
		return (UINT32)-1;
	}
	//获得关卡ID
	UINT32 getCopyMapID()
	{
		return m_copyMapID;
	}
	//获得队伍平均等级
	UINT16	getMemberAvgLevel();
	//设置战场匹配状态
	void setMatchStatus(bool status)
	{
		m_teamMatching = status;
	}
private :
	UINT32 m_id;								//房间ID,为房主的角色ID
	UINT32 m_copyMapID;							//副本ID
	stTeamMember m_members[s_MAX_MEMBER];		//成员,默认第一个人对队长
	std::map<UINT32,UINT32> m_inviteList;		//邀请队列
	bool   m_hasEnterCopyMap;					//已经进入了副本
	bool	m_teamMatching;						//是否在匹配，用在战场
};

struct stWaitMatchInfo
{
	UINT32 m_beginWaitTime;
	std::set<UINT32> m_waitIDs;					//等待的玩家
	std::map<UINT32,UINT32> m_waitTeamIDs;		//等待的队伍 <teamID,time>

	stWaitMatchInfo()
	{
		m_beginWaitTime = 0;
	}
};

class CTeamMgr
{
	USE_SINGLETON_NEW(CTeamMgr);
public :
	static const UINT16 s_ROOM_NUM_ONE_PAGE = 20;
	static const UINT32 s_MATCH_MAX_WAIT_TIME = 15;
private :
	//构造
	CTeamMgr();
public :
	//析构
	~CTeamMgr();
	//返回管理器对象
	static CTeamMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//添加一个房间
	bool addTeam(CTeam * pTeam);
	//删除一个房间
	bool removeTeam(CTeam * pTeam);
	//添加角色与房间的对应关系
	void addCharID2Team(UINT32 charID,UINT32 teamID);
	//删除角色与房间的对应关系
	void removeCharID2Team(UINT32 charID);
	//根据角色获得房间
	CTeam * getTeamByCharID(UINT32 charID);
	//根据房间ID获得房间
	CTeam * getTeamByID(UINT32 teamID);
	//用户上线
	void userOnline(GameUser * pUser);
	//用户下线
	void userOffline(GameUser * pUser);
	//获得一个房间ID
	bool genTeamID(UINT32 & id);
	//归还一个房间ID
	void returnTeamID(UINT32 id);
	//创建队伍
	void createTeam(GameUser * pUser,UINT32 copyMapID); 
	//请求进入队伍
	void enterTeam(GameUser * pUser,UINT32 teamID);
	//请求离开队伍
	void leaveTeam(GameUser * pUser);
	//解散队伍
	// void fireTeam(GameUser * pUser,bool bNotify = true);
	//邀请加入
	// void inviteJoinIn(GameUser * pUser,UINT32 charID,UINT8 pos,TeamProto::eTeamType eType);
	//答复邀请
	// void replyInvite(GameUser * pUser,UINT32 teamID,bool agree,UINT8 pos,TeamProto::eTeamType eType,bool timeUseOut);
	//进入关卡
	// void enterLv(GameUser * pUser);
	//自动匹配
	// void autoMatch(GameUser * pUser,UINT32 copyMapID);
	//取消自动匹配
	// void cancelMatch(GameUser * pUser,UINT32 copyMapID,bool bNotify = true);
	//组队定时器
	// void timer(UINT32 cur);
private :
	__gnu_cxx::hash_map<UINT32,CTeam *> m_allTeam;										//所有房间
	typedef __gnu_cxx::hash_map<UINT32,CTeam *>::iterator Team_IT;		
	__gnu_cxx::hash_map<UINT32,UINT32>  m_charID2TeamID;								//角色与房间的对应关系
	typedef __gnu_cxx::hash_map<UINT32,UINT32>::iterator  CharID2TeamID_IT;

	static UINT32  s_teamIDGenID;
	std::map<UINT32, stWaitMatchInfo >  m_waitMatchQueue;								//等待匹配的个人
	std::map<UINT32, stWaitMatchInfo >  m_waitMatchTeamQueue;							//等待匹配的队伍
};

template <class T>
void CTeam::sendProto2All(const T & cmd,UINT32 except)
{
	for (UINT8 i = 0; i< s_MAX_MEMBER;++i){
		if (m_members[i].m_charID){
			GameUser * pUser = GameUserManager::getSingleton().getUser(m_members[i].m_charID);
			if (pUser && (pUser->getID() != except)){
				pUser->sendProto2Me(cmd);	
			}
		}
	}
}

struct stPvpWaitMatchInfo
{
	UINT32	m_Level;
	std::map<UINT32, UINT32> m_waitIDs;		//<id, begintime>

	stPvpWaitMatchInfo()
	{
		m_Level = 0;
	}
};

//队伍的匹配,用在战场
class CTeamMatchMgr
{
	USE_SINGLETON_NEW(CTeamMatchMgr);
	public :
		static const INT32 s_MAX_PK_NUM = 6; 
		static const UINT32 s_CHECK_MATCH_INTERVAL = 2;
		static const UINT32 s_WAIT_TIME_1 = 10;
		static const UINT32 s_WAIT_TIME_2 = 30;
		static const INT32 s_MAX_GAME_TYPE = 2; 	//总共几种游戏模式需要匹配
	private :
		//构造函数
		CTeamMatchMgr();
	public :
		//析构函数
		~CTeamMatchMgr();
		//返回管理器对象
		static CTeamMatchMgr & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		//定时回调
		void timer(UINT32 cur);
		//增加匹配队伍
		void addMatchTeam(GameUser *pUser,CTeam *pTeam,Global::eRealTimePKType type);
		//取消队伍匹配
		bool removeMatchTeam(UINT32 teamID,Global::eRealTimePKType type);
		//增加一个等待玩家
		void addMatchUser(GameUser * pUser,Global::eRealTimePKType type);
		//取消一个等待玩家
		void removeMatchUser(GameUser * pUser,Global::eRealTimePKType type);
		//玩家下线
		void userOffline(GameUser * pUser);
		//是否开放
		bool isOpen(Global::eRealTimePKType type);
		//检查组队进入副本
		bool checkCanEnterCopyMap(Global::eRealTimePKType type,
			std::map<UINT32, UINT32> *mapUser,		//in 传入待匹配用户队列 first:用户ID，second不同含义 等级／时间
			std::map<UINT32, UINT32> *mapTeam,		//in 传入待匹配队伍队列 first:队伍ID，second不同含义 等级／时间
			std::map<UINT32, UINT32> *backUser,		//out 返回匹配成功的用户队列 first:用户ID，second不同含义 等级／时间
			std::map<UINT32, UINT32> *backTeam,	//out 返回匹配成功的队伍队列 first:队伍ID，second不同含义 等级／时间
			bool forceMatch=false);				//in, 是否强制匹配机器人
		//匹配检查
		void checkMatch();
		//检查匹配前重置
		void checkMatchReset();
		//队伍队员离队，需要重新按等级匹配
		void OnTeamMemberLeave(UINT32 teamID, UINT32 oldLevel);
	private :

		UINT32  m_lastCheckTime;												//上次检查时间

		std::map<UINT16, stPvpWaitMatchInfo > m_waitMatchTeams[s_MAX_GAME_TYPE];		//两类的等待匹配的队伍
		std::map<UINT16, stPvpWaitMatchInfo > m_waitMatchUser[s_MAX_GAME_TYPE];		//两类等待匹配的人

		std::map<UINT16, std::map<UINT32, UINT32> >	m_matchUserStep2;	//处于2阶段的玩家 <level ,<id, level>>
		std::map<UINT32, UINT32> 	m_matchUserStep3;	//处于3阶段的玩家 <id, level>
		std::map<UINT16, std::map<UINT32, UINT32> >	m_matchTeamStep2;	//处于2阶段的队伍 <level ,<teamId, level>>
		std::map<UINT32, UINT32> 	m_matchTeamStep3;	//处于3阶段的队伍	<teamID, level>
};
#endif
