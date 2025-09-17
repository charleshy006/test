#ifndef __H_HERO_LEAGUE_H__
#define __H_HERO_LEAGUE_H__
#include "Singleton.h"
#include "TypesDef.h"
#include "SortList.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "SortList.pb.h"
#include "HistoryRankMgr.h"
#include "GameUser.h"

#define HEROLEAGUE_PVP_REWARD_INFO_LEN    40000

class CHeroLeagueMgr;

bool PkWithRobot(Global::stHeroLeaguePkWithRobot * recvCmd);
bool PkWithOther(Global::stHeroLeagueEnterCopyMap * recvCmd, CHeroLeagueMgr *pmgr);

class Player;

struct stUserInfo
{
	UINT32  m_charID;                   //角色ID      
	UINT32  m_pubCharID;                //pub上角色ID 
	UINT32  m_areaCode;                 //区ID        
	char    m_name[MAX_NAMESIZE + 1];   //名字        
	UINT16  m_level;                    //等级        
	UINT16  m_vipLv;                    //VIP等级     
	UINT8   m_job;                      //职业        
	UINT8   m_sex;                      //性别        
	UINT32  m_battlePower;              //战力       
	UINT16	m_photoID;					//头像ID

	stUserInfo()
	{
		m_charID		=	0;               
		m_pubCharID		=	0;            
		m_areaCode		=	0;             
		bzero(m_name, sizeof(m_name));
		m_level			=	0;                
		m_vipLv			=	0;                
		m_job			=	0;                  
		m_sex			=	0;                  
		m_battlePower	=	0;         
		m_photoID		=	0;
	}
};


struct stTeam
{
	UINT32 m_charID1;	//队员1
	UINT32 m_charID2;	//队员2
	UINT32 m_time;		//时间
};

struct stTeamInfo
{
	stUserInfo user1;   //队员1
	stUserInfo user2;   //队员2
};


struct stPreRoll
{
	UINT32 m_charID;		//角色ID
	char   m_name[MAX_NAMESIZE + 1];//名字
	UINT32 m_serverIndex;	//服务器ID
	UINT32 m_score;			//分数

	stPreRoll()
	{
		m_charID		=	0;
		bzero(m_name,sizeof(m_name));
		m_serverIndex	=	0;
		m_score			=	0;
	}
};

struct stRecord
{                                                     
	UINT32  m_charID;                   //角色ID      
	UINT32  m_pubCharID;                //pub上角色ID 
	UINT32  m_areaCode;                 //区ID        
	char    m_name[MAX_NAMESIZE + 1];   //名字        
	UINT16  m_level;                    //等级        
	UINT16  m_vipLv;                    //VIP等级     
	UINT8   m_job;                      //职业        
	UINT8   m_sex;                      //性别        
	UINT32  m_battlePower;              //战力        
	UINT32  m_pubServerIndex;           //所在服务器ID 
	UINT32  m_score;                    //积分
	UINT16	m_photoID;					//头像ID

	stRecord()                                      
	{                                                 
		m_charID            =   0;                    
		m_pubCharID         =   0;                    
		m_areaCode          =   0;                    
		bzero(m_name,sizeof(m_name));                 
		m_level             =   0;                    
		m_vipLv             =   0;                    
		m_job               =   0;                    
		m_sex               =   0;                    
		m_battlePower       =   0;     
		m_pubServerIndex	=	0;
		m_score				=	0;
		m_photoID			=	0;
	}
};

//PubGameServer只负责匹配及战斗，其他逻辑处理在globalServer
class CHeroLeagueMgr
{
	USE_SINGLETON_NEW(CHeroLeagueMgr);
public :
	static const UINT8 s_SEND_SORTITEM_MAX = 10;
	static const UINT32 s_MAX_REWARD_NUM = 10000;
	static const UINT16 s_MAX_REWARD_DAY = 7;
	static const UINT32 s_CHECK_MATCH_INTERVAL = 2;
	static const UINT16 s_MIN_WAIT_SEX = 3;
	static const UINT32 s_ENTER_COPYMAP_INTERVAL = 3;
	static const UINT8	s_PRE_WAIT_TIME = 3;//初赛3秒倒计时
private :
	CHeroLeagueMgr() 
	{
		m_lastCheckTime = 0;
		m_finalUserNum  = 0;
	}
public :
	//析构函数
	~CHeroLeagueMgr() {}
	//获得排名管理实例
	static CHeroLeagueMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	
	//定时回调
	void timer(UINT32 cur);
	//增加等待匹配的玩家
	void addMatchUser(GameUser * pUser);
	//玩家取消匹配
	void cancelMatch(GameUser * pUser);
	//新的匹配检查
	void checkMatchNew();
	//初赛进入地图
	void preEnterMap(UINT32 sec);
	//队伍进入map
	void enterMap();
	//重置每个阶段玩家
	void checkMatchReset();
	//更新角色到名单
	void updateWaitCharIdsMap(PubGame::stSynPubRoleListResult *recvCmd);
	//决赛返回匹配到的角色信息
	void sendMenberInfo(UINT32 charID);
	//决赛是否到战斗时间
	bool canEnterMap(UINT32 cur);

	stRecord * isInRecvrd(UINT32 charID);
	void updateRecvrd(stRecord& record);
	//按积分降序排序
	void scoreSort(UINT8 num);
	//获得人数
	UINT8 getUserNum()
	{
		return m_finalUserNum * 2;
	}
	//对方退出pub 移除模型
	void removeOtherModel(UINT32 charID);

	std::map<UINT32,stUserInfo> &getInfoMap()
	{
		return m_teamInfoMap;
	}
private:
	UINT32 m_lastCheckTime;

	UINT8 m_finalUserNum;

	//charID,charID
	std::map<UINT32, UINT32> m_waitCharIDs;

	//charID, stRecord
	std::map<UINT32, stRecord> m_recvrdMap;
	//保存复赛名单
	std::map<UINT32, UINT32> m_remCharIDs;

	std::vector<stTeam> m_teamVec;
	std::map<UINT32,stUserInfo> m_teamInfoMap;
};
#endif
