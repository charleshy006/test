#ifndef __H_LEVEL_MGR_H__
#define __H_LEVEL_MGR_H__
#include "TypesDef.h"
#include "Role.pb.h"
#include "TblDataMgr.h"
#include "DropMgr.h"
#include "SortList.pb.h"
#include <set>
#include <map>

enum 
{
	ARENA_PK_LEVELID = 110001,		//天梯pk关卡ID
	ARENA_PK_LEVELID_SERVERCHECK = 110002,		//天梯pk关卡ID 服务器校验模式
	INTERCEPT_LEVELID = 20002,		//拦截关卡ID
	INTERCEPT_LEVELID_SERVERCHECK = 20003,		//拦截关卡ID 服务器校验模式
	LEITAI_LEVELID_SERVERCHECK = 20401,	//擂台关卡ID 服务器校验模式
};

class GameUser;
class CLevelMgr
{
public :
	static const UINT32 s_ARENA_PK_MAX_LASTIME = 600;		//竞技场战斗最多持续时间
public :
	struct stArenaPKInfo
	{
		UINT32 m_rank;			//挑战的名次
		UINT32 m_challengeTime; //挑战的时间

		stArenaPKInfo()
		{
			reset();
		}

		void reset()
		{
			m_rank = 0;
			m_challengeTime = 0;
		}
	};
	
	struct stLeitaiPKInfo
	{
		UINT32 m_levelID;
		UINT32 m_charID;		//挑战的角色ID
		bool	m_fighting;
		GameUser* m_pRobot;

		stLeitaiPKInfo()
		{
			reset();
		}

		void reset()
		{
			m_levelID = 0;
			m_charID = 0;
			m_fighting = false;
			m_pRobot = NULL;
		}
	};

	struct stCharpterInfo
	{
		UINT32 m_charpterID;		//章节ID
		std::set<UINT32> m_lvIDs;	//关卡ID

		stCharpterInfo()
		{
			m_charpterID = 0;
		}
	};


	struct stLevelInfoDB
	{
		UINT32 m_levelID;			//关卡ID
		UINT8  m_starNum;			//星数
		UINT16 m_rushNum;			//扫荡次数
		UINT16 m_resetNum;			//重置次数

		stLevelInfoDB()
		{
			m_levelID = 0;
			m_starNum = 0;
			m_rushNum = 0;
			m_resetNum = 0;
		}
	};

	struct stCharpterInfoDB
	{
		UINT32 m_charpterID ;							//章节ID
		std::map<UINT32,stLevelInfoDB> m_levelInfos;	//关卡信息
		UINT8  m_charpterRewardStep;					//章节奖励阶段

		stCharpterInfoDB()
		{
			m_charpterID = 0;
			m_charpterRewardStep = 0;
		}
	};
public :
	//构造函数
	CLevelMgr(GameUser * pOwner);
	//析构函数
	~CLevelMgr();
	//保存数据
	UINT32 save(byte * buf,UINT32 needSpace);
	//加载数据
	UINT32 load(byte * buf);
	//玩家上线
	void userOnline();
	//完成条件检查
	bool checkFinish();
	//完成天梯
	void finishArena(UINT32 copymapID,bool bWin);
	//完成天梯(服务器校验模式)
	void finishArenaNew(UINT32 copymapID,bool bWin, bool bTimeout);
	//完成擂台
	bool finishLeitai(UINT32 copymapID);	
	//完成某个关卡pve关卡
	void finishLv(Role::stReqFinishLv * recvCmd);
	//扫荡
	void rushLv(UINT32 levelID,UINT8 num,Role::eRushType eType);
	//请求章节奖励
	void reqCharpterReward(UINT32 charpterID,UINT8 step);
	//请求重置次数
	void resetRushNum(UINT32 levelID);
	//获得等级数量
	UINT16 getStarNum();
	//检查某个关卡是否完成
	bool isLevelFinish(UINT32 levelID);
	//设置是否在关卡
	void setInLvFlag(bool lvFlag)
	{
		m_isInLv = lvFlag;
	}
	//是否在关卡中
	bool isInLv()
	{
		return m_isInLv;	
	}
	//增加掉落信息
	void addDropItem(UINT32 itemID,UINT32 num);
	//获得某个关卡扫荡次数
	UINT16 getRushNum(UINT32 levelID);
	//进入竞技场关卡
	void enterArenaLv(UINT32 levelID, GameUser* pRobot);
	//进入擂台关卡
	void enterLeitaiLv(UINT8 mode);
	//进入pve关卡
	void enterLv(UINT32 levelID);
	//是否为任务副本
	bool isTaskLevel(UINT32 copyMapType);
	//加载配置
	static bool loadCfg();
	//增加章节与关卡的对应关系
	static void addLevel2Charpter(UINT32 charpterID,UINT32 levelID);
	//设置竞技场挑战名次
	void arenaChallengeRank(UINT32 rank);
	//设置开箱箱子数量
	void setOpenBoxInfo(UINT32 openBoxItem,UINT32 openBoxItemNum)
	{
		m_openBoxItemID = openBoxItem;
		m_openBoxItemNum = openBoxItemNum;
	}
	//客户端开箱子
	void openBox();
	//获得掉落数据
	void genDropInfo(std::vector<CDropMgr::stDropEntry> & dropVec,stCopyMapBase * pCopyMapBase);
	//提前发送进入关卡相关信息
	void notifyEnterLvInfo(stCopyMapBase * pCopyMapBase);
	//检查关卡刷新
	void checkDataRefresh();
	//计算刷新时间
	void calcRefreshTime();
	//是否计算星数
	bool isCalcStar(UINT8 lvType);
	//给予组队副本关卡奖励
	void giveTeamCopymapReward(UINT32 copyMapID);
	//是否pve关卡
	bool isPVELevel(UINT16 type);
	//设置擂台挑战角色ID
	void setLeitaiCharID(UINT32 charID)
	{
		m_leitaiPKInfo.m_charID = charID;
	}
	//获得擂台挑战角色ID
	UINT32 getLeitaiCharID()
	{
		return m_leitaiPKInfo.m_charID;
	}
	//设置擂台挑战机器人
	void setLeitaiRobot(GameUser* pRobot)
	{
		m_leitaiPKInfo.m_pRobot = pRobot;
	}
	//完成所有PVE关卡
	void finishAllPVELevel();
	//强迫完成某个关卡
	void forceFinishPVELv(stCopyMapBase * pBase);
	//重置所有组队副本
	void resetAllTeamCopymap();
	//获得组队副本次数
	UINT16 getTeamCopymapNum()
	{
		return m_teamCopymapNum;
	}
	//增加组队副本次数
	void addTeamCopymapNum(UINT16 addVal = 1)
	{
		m_teamCopymapNum += addVal;	
	}
	//充值组队副本次数
	void resetTeamCopymapNum()
	{
		m_teamCopymapNum = 0;
	}
	//进入拦截战斗场景
	void enterInterceptLv(UINT32 levelID);
	//进入拦截战斗场景  服务器校验模式
	void enterInterceptLv_ServerCheck(UINT32 levelID, GameUser* pRobot, UINT32 buffCnt);
	//进入翡翠梦境
	void enterEmeraldDream(UINT32 levelID);
	//获取挂机奖励的关卡进度
	UINT32 getGuajiLv();
private :
	GameUser * m_pOwner;
	std::map<UINT32,stCharpterInfoDB>  m_charpterInfos;
	bool	m_isInLv;											//是否在关卡里面
	std::map<UINT32,UINT32> m_randomDropItems;					//随机出来的道具
	static std::map<UINT32,stCharpterInfo> s_charptersCnt;		//章节的配置信息
	static std::map<UINT32,UINT32>  s_levelID2CharpterID;		//关卡ID与章节ID对应关系
	stArenaPKInfo m_arenaPKInfo;
	stLeitaiPKInfo m_leitaiPKInfo;
	UINT32 m_openBoxItemID;										//关卡开箱子道具ID
	UINT32 m_openBoxItemNum;									//开箱子获得道具数量
	bool	m_canOpenBox;										//是否可以开箱子
	UINT32  m_lastRefreshTime;									//上次刷新时间
	UINT16  m_teamCopymapNum;									//组队副本次数
	UINT16 m_emeraldDreamNum;									//翡翠梦境次数
};
#endif
