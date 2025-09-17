#ifndef __H_COPY_MAP_
#define __H_COPY_MAP_
#include "Map.h"
#include "Time.h"
#include "GameUser.h"
#include "TblCopyMapBase.h"
#include <set>

struct stPKCampMem
{
	UINT32 m_charID ;   				//角色ID
	std::string m_name;					//名字
	UINT16 m_level;						//等级
	UINT32 m_battlePower;				//战力等级
	UINT32 m_score;						//分数
	UINT16  m_killNum;					//杀人数量
	UINT16  m_beKillNum;				//被杀次数
	UINT32 m_damage;					//伤害
	bool   m_bLeave;					//离开了

	stPKCampMem()
	{
		m_charID = 0;
		m_level = 0;
		m_battlePower = 0;
		m_score = 0;
		m_killNum = 0;
		m_beKillNum = 0;
		m_damage = 0;
		m_bLeave = false;
	}
};

//pk阵营
struct stPKCamp
{
	UINT8		  					m_campID;			//阵营ID
	std::map<UINT32,stPKCampMem> 	m_memberIDs;		//成员的ID
	bool		  					m_win;				//true胜利,false,失败
	std::set<UINT32> 				m_aliveIDs;			//活着的玩家
	std::set<UINT32> 				m_dieIDs;			//死亡的玩家
	UINT16 							m_score;			//积分

	stPKCamp()
	{
		reset();
	}

	void reset()
	{
		m_campID = 0;
		m_memberIDs.clear();
		m_win = false;
		m_aliveIDs.clear();
		m_dieIDs.clear();
		m_score = 0;
	}
};

//资源
struct stMobaResInstance
{
	UINT16 m_id;
	UINT32 m_summonTime;
	bool   m_bExist;

	stMobaResInstance()
	{
		m_id = 0;
		m_summonTime = 0;
		m_bExist = false;
	}
};

//buff点
struct stMobaBuffInstance
{
	UINT16 m_id;
	UINT32 m_summonTime;
	bool   m_bExist;
	UINT32 m_buffID;

	stMobaBuffInstance()
	{
		m_id = 0;
		m_summonTime = 0;
		m_bExist = false;
		m_buffID = 0;
	}
};


class GameUser;
class CCopyMap : public CMap
{
	public:
		static const UINT32 s_WAIT_TIME = 12000;
		static const UINT32 s_SUMMON_BUFF_INTERVAL = 30;
		static const UINT32 s_SUMMON_RES_INTERVAL = 20;
		static const UINT32 s_BATTLE_LASTTIME = 600000;
		static const UINT32 s_WIN_NEED_SCORE = 100;		
		static const UINT32 s_COLLECT_GOT_SCORE = 15;
	public:
		//构造
		CCopyMap();
		//析构
		~CCopyMap();
		//是否是副本
		virtual bool isCopyMap(){return true;} 
		//是否超时
		virtual bool isTimeOut(UINT64 uTime); 
		//获得剩余时间
		virtual UINT32 getRemainTime() {  return (UINT32)-1; }
		//定时回调
		virtual void timerUpdate(UINT64 uTime,UINT16 group);
		//初始化
		void reset();
		//多少时间后，结束副本
		void endGame();
		//进行副本剧情
		virtual void doAct(UINT64 uTime);
		//剧情结束后的动作
		void doEndAct(UINT64 uTime);
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//设置副本相关配置
		void setCopyMapBase(stCopyMapBase * pCopyMapBase);
		//召唤怪物
		void summonBoss();
		//设置owner
		void setOwnerID(UINT32 ownerID) {  m_ownerID = ownerID ; }
		//获得owner
		UINT32 getOwnerID() { return m_ownerID ; }
		//加载资源结束
		void finishLoadRes() {  m_loadResFinish = true ;}
		//检查关卡是否已经完成
		bool checkFinishLv();
		//检查是否失败了
		bool checkFailLv();
		//检查是否死亡
		bool checkIsAllDie(CObj::ObjType eType);
		//传送所有玩家到主城
		void sendAllUser2City();
		//创建阵营
		void createCamp(UINT8 campID);
		//加入阵营
		void joinCamp(UINT8 campID,UINT32 charID);
		//加入阵营
		void joinCamp(UINT8 campID,GameUser* pUser);
		//资源加载完毕
		void resLoadFinish(GameUser * pUser);
		//通知游戏开始
		void notifyBegin();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//同步加入
		virtual void synAdd(CObj *pObj,const CAreaIdx & areaIdx);
		//同步删除
		virtual void synRemove(CObj *pObj,const CAreaIdx & areaIdx);
		//同步移动
		virtual void synMove(CObj *pObj,const CAreaIdx & areaIdxS,const CAreaIdx & areaIdxT);
		//同步瞬移
		virtual void synTeleport(CObj *pObj,const CAreaIdx & areaIdxS,const CAreaIdx & areaIdxT);
		//和好友一起切磋
		void setPKWithFriend()
		{
			m_pkWithFriend = true;
		}
		//是否属于换个副本
		bool isExist(UINT32 charID);
		//刷资源
		void summonRes();
		//刷buff1
		void summonBuff1();
		//刷buff2
		void summonBuff2();
		//某个阵营加积分
		void addCampScore(UINT8 campID,UINT32 score);
		//检查是否触碰到区域
		UINT16 checkCollideZone(CPos & pos,std::map<UINT16,ZoneDefine> & zones);
		//检查是否触碰到资源区
		UINT16 checkCollideResZone(CPos & pos);
		//是否moba地图
		bool isMobaCopyMap()
		{
			if (!m_pCopyMapBase){
				return false;
			}
			return eLevelType_Moba_RealTime == m_pCopyMapBase->m_type; 
		}
		//是否公会大厅
		bool isGuildHall()
		{
			if (!m_pCopyMapBase){
				return false;
			}
			return eLevelType_GuildHall == m_pCopyMapBase->m_type; 
		}
		//获得boss血量
		virtual UINT32 getBossHP() { return m_bossHP; }
		//设置boss血量
		void setBossHP(UINT32 bossHP) { m_bossHP = bossHP; }
		//活得boss等级
		virtual UINT16 getBossLv() { return m_bossLv; }
		//设置boss等级
		void setBossLv(UINT16 bossLv) {  m_bossLv = bossLv ;}
		//初始化后处理
		void afterInit();
		//需要AI执行者
		virtual  bool needAIExector() { return false ;}
		//通知执行AI
		void notifyExector();
		//是否是领地战地图
		virtual bool isManorFightMap()
		{
			return false;
		}
		virtual void setSpecailParam(UINT8 idx, UINT32 value){}
	public://事件类
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
		virtual void onUserRelive(GameUser * pUser);				//玩家复活
		virtual void onResCollect(GameUser * pUser,UINT16 id);		//资源被采集了
	protected:
		Timer m_oneSec;								//秒定时
		Timer m_twoSec;								//2秒定时器
		Timer m_tenSec;								//10秒定时
		UINT64 m_openTime;							//该副本开启时间	
		UINT64 m_lastTime;							//该副本持续时间
		UINT64 m_endTime;							//记录副本提前结束时间
		const  stCopyMapBase *m_pCopyMapBase;		//在地图中的配置
		bool   m_isEnd;								//副本是否已经结束
		UINT32 m_ownerID;							//玩这个关卡的玩家
		bool   m_loadResFinish;						//加载资源结束
		bool   m_bFinish;							//是否结束
		bool   m_bFail;								//失败
		std::map<UINT8,stPKCamp> m_campInfos;		//阵营信息
		UINT16 m_hasLoadResNum;						//已经在家资源的玩家数量
		bool   m_bBegin;							//是否已经开始
		bool   m_pkWithFriend;						//和好友一起切磋
		std::set<UINT32> m_allEnterUsers;			//所有进入的用户
		bool   m_bHasRes;							//是否有资源
		std::vector<stMobaResInstance> m_mobaReses;	//资源
		std::vector<stMobaBuffInstance> m_mobaBuffs;//buff
		UINT32 m_bossHP;							//设置bossHP
		UINT16 m_bossLv;							//boss等级
		UINT64 m_realStartTime;						//该副本真正开启时间,用户在等待对方加载后的时间点	
};

//公会组队
class CGuildTeamMap : public CCopyMap
{
	public :
		static const UINT32 s_GUILD_TEAM_MAP_LASTTIME = 135000;
	public :
		//构造函数
		CGuildTeamMap();
		//析构函数
		~CGuildTeamMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否要重设boss血量
		virtual bool isReSetBossHP() {  return true ;}
		//是否超时
		virtual bool isTimeOut(UINT64 uTime); 
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//返回战斗剩余时间
		virtual UINT32 getRemainTime();
		//是否需要AI执行者
		virtual  bool needAIExector() { return true ;}
	public :
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
	private :
		UINT32 m_firstKillCharID;
};

//世界boss
class CWorldBossMap : public CCopyMap
{
	public :
		static const UINT32 s_WORLD_BOSS_MAP_LASTIME = 120000;
	public :
		//构造函数
		CWorldBossMap();
		//析构函数
		~CWorldBossMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否要重设boss血量
		virtual bool isReSetBossHP() {  return true ;}
		//是否超时
		virtual bool isTimeOut(UINT64 uTime); 
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//返回战斗剩余时间
		virtual UINT32 getRemainTime();
		//是否需要AI执行者
		virtual  bool needAIExector() { return true ;}

	public:
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
};

//实时map
class CRealTimeMap : public CCopyMap
{
	public :
		static const  UINT32 s_REALTIME_LASTTIME = 600000;
	public :
		//构造函数
		CRealTimeMap();
		//析构函数
		~CRealTimeMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//剩余时间
		virtual UINT32 getRemainTime();
		//定时回调
		virtual void doAct(UINT64 uTime);
	public :
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
};

//组队副本
class CTeamMap : public CCopyMap
{
	public :
		static const UINT32 s_TEAM_MAP_LASTTIME = 600000;
	public :
		//构造函数
		CTeamMap();
		//析构函数
		~CTeamMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否超时
		virtual bool isTimeOut(UINT64 uTime); 
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//返回战斗剩余时间
		virtual UINT32 getRemainTime();
		//是否需要AI执行者
		virtual  bool needAIExector() { return true ;}
	public :	
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
};

//1vs1副本
class COne2OneMap : public CCopyMap
{
	public :
		static const UINT32 s_ONE2ONE_LASTTIME = 600000;
	public :
		//构造函数
		COne2OneMap();
		//析构函数
		~COne2OneMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//剩余时间
		virtual UINT32 getRemainTime();
		//定时处理
		virtual void doAct(UINT64 uTime);
	public :
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
};

//moba实时
class CMobaRealTime : public CCopyMap
{
	public :
		//构造函数
		CMobaRealTime();
		//析构函数
		~CMobaRealTime();
		//定时
		virtual void doAct(UINT64 uTime);
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//返回战斗剩余时间
		virtual UINT32 getRemainTime();
	public :
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
		virtual void onUserRelive(GameUser * pUser);				//玩家复活
};

//领地战
class CManorFightMap : public CCopyMap
{
	public :
		static const UINT32 s_MANOR_FIGHT_LASTTIME = 290000;
	public :
		//构造函数
		CManorFightMap();
		//析构函数
		~CManorFightMap();
		//定时
		virtual void doAct(UINT64 uTime);
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//返回战斗剩余时间
		virtual UINT32 getRemainTime();
		//是否是领地战地图
		virtual bool isManorFightMap()
		{
			return true;
		}
		//设置信息
		void setManorInfo(UINT32 manorID,UINT8 formation,UINT8 teamAID,UINT8 teamBID)
		{
			m_manorID = manorID;
			m_formation = formation;
			m_teamAID = teamAID;
			m_teamBID = teamBID;
		}
	public :
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
		//virtual void onUserRelive(GameUser * pUser);				//玩家复活
	private :
		UINT32 m_manorID;											//领地ID
		UINT8  m_formation;											//阵容
		UINT8  m_teamAID;
		UINT8  m_teamBID;											
};

//翡翠梦境
class CEmeraldDreamMap : public CCopyMap
{
	public:
		static const UINT32 s_EMERALD_DREAM_MAP_LASTTIME = 180000;
	public:
		//构造函数
		CEmeraldDreamMap();
		//析构函数
		~CEmeraldDreamMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否要重设boss血量
		virtual bool isReSetBossHP() {  return true ;}
		//是否超时
		virtual bool isTimeOut(UINT64 uTime);
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//返回战斗剩余时间
		virtual UINT32 getRemainTime();
		//是否需要AI执行者
		virtual  bool needAIExector() { return true ;}

	public:
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing *pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser *pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing *pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser* pUser);					//玩家移动
};

//英雄联赛
class CHeroLeagueMap : public CCopyMap
{
public :
	static const UINT32 s_HERO_LEAGUE_LASTTIME = 600000;
public :
	//构造函数
	CHeroLeagueMap();
	//析构函数
	~CHeroLeagueMap();
	//完成
	virtual void doFinish();
	//失败
	virtual void doFail();
	//获得乘系数之前的血量
	virtual UINT32 getOrgHP(UINT32 hp);
	//是否限时宠物
	virtual bool isShowHero() { return true ; }
	//剩余时间
	virtual UINT32 getRemainTime();
	//定时处理
	virtual void doAct(UINT64 uTime);
public :
	virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
	virtual void onHeroDie(CHero *);							//武将死亡
	virtual void onHeroEnter(CHero *);							//武将进入
	virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
	virtual void onUserEnter(GameUser * pUser);					//玩家进入
	virtual void onUserLeave(GameUser * pUser);					//玩家离开
	virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
	virtual void onUserMove(GameUser * pUser);					//玩家移动
};

//天梯
class CArenaMap : public CCopyMap
{
	public :
		static const UINT32 s_ARENA_LASTTIME = 600000;
	public :
		//构造函数
		CArenaMap();
		//析构函数
		~CArenaMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否显示宠物
		virtual bool isShowHero() { return true ; }
		//剩余时间
		virtual UINT32 getRemainTime();
		//定时处理
		virtual void doAct(UINT64 uTime);
	public :
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		// virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		// virtual void onUserMove(GameUser * pUser);					//玩家移动
		virtual CBeing* getBeing(UINT8 type,UINT32 moID);
	private:
		GameUser*	m_pRobot;						//玩家对战的机器人，都是新创建的临时数据，结束时删除
};


//拦截
class CInterceptMap : public CCopyMap
{
	public :
		static const UINT32 s_INTERCEPT_LASTTIME = 600000;
	public :
		//构造函数
		CInterceptMap();
		//析构函数
		~CInterceptMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否显示宠物
		virtual bool isShowHero() { return true ; }
		//剩余时间
		virtual UINT32 getRemainTime();
		//定时处理
		virtual void doAct(UINT64 uTime);
	public :
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		// virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		// virtual void onUserMove(GameUser * pUser);					//玩家移动
		virtual CBeing* getBeing(UINT8 type,UINT32 moID);
		virtual void setSpecailParam(UINT8 idx, UINT32 value){m_buffCnt = value;}
	private:
		UINT32 		m_buffCnt;						//防守方鼓舞次数
		GameUser*	m_pRobot;						//玩家对战的机器人，都是新创建的临时数据，结束时删除
};


//擂台
class CLeitaiMap : public CCopyMap
{
	public :
		static const UINT32 s_LEITAI_LASTTIME = 600000;
	public :
		//构造函数
		CLeitaiMap();
		//析构函数
		~CLeitaiMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否显示宠物
		virtual bool isShowHero() { return true ; }
		//剩余时间
		virtual UINT32 getRemainTime();
		//定时处理
		virtual void doAct(UINT64 uTime);
	public :
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		// virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		// virtual void onUserMove(GameUser * pUser);					//玩家移动
		virtual CBeing* getBeing(UINT8 type,UINT32 moID);
		// virtual void setSpecailParam(UINT8 idx, UINT32 value){m_buffCnt = value;}
		UINT32 getUseTime();
	private:
		GameUser*	m_pRobot;						//玩家对战的机器人，都是新创建的临时数据，结束时删除
};

//组队副本
class CDragonNestMap : public CCopyMap
{
	public :
		static const UINT32 s_DRAGON_NEST_MAP_LASTTIME = 600000;
	public :
		//构造函数
		CDragonNestMap();
		//析构函数
		~CDragonNestMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否超时
		virtual bool isTimeOut(UINT64 uTime); 
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//返回战斗剩余时间
		virtual UINT32 getRemainTime();
		//是否需要AI执行者
		virtual  bool needAIExector() { return true ;}
	public :	
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
};

//宝石副本
class CStoneCopyMap : public CCopyMap
{
	public :
		static const UINT32 s_STONE_COPYMAP_MAP_LASTTIME = 600000;
	public :
		//构造函数
		CStoneCopyMap();
		//析构函数
		~CStoneCopyMap();
		//完成
		virtual void doFinish();
		//失败
		virtual void doFail();
		//获得乘系数之前的血量
		virtual UINT32 getOrgHP(UINT32 hp);
		//是否超时
		virtual bool isTimeOut(UINT64 uTime); 
		//是否限时宠物
		virtual bool isShowHero() { return true ; }
		//返回战斗剩余时间
		virtual UINT32 getRemainTime();
		//是否需要AI执行者
		virtual  bool needAIExector() { return true ;}
	public :	
		virtual void onNpcDie(CNpc*,CBeing * pAtter);				//NPC死亡
		virtual void onHeroDie(CHero *);							//武将死亡
		virtual void onHeroEnter(CHero *);							//武将进入
		virtual void onPlayerDie(CBeing * pAtter,GameUser * pUser);	//玩家死亡
		virtual void onUserEnter(GameUser * pUser);					//玩家进入
		virtual void onUserLeave(GameUser * pUser);					//玩家离开
		virtual void onNpcDecHP(CBeing * pAtter,UINT32 decHP);		//当npc减血
		virtual void onUserMove(GameUser * pUser);					//玩家移动
};

#endif

