#ifndef __HERO_LEAGUE_H__
#define __HERO_LEAGUE_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

struct stCommonPart
{
	UINT32 m_openTime;          //开放时间
	UINT32 m_endTime;           //结束时间
	UINT32 m_clearing;          //结算时间
	UINT32 m_levelID;           //场景ID
	UINT8  m_limitnum;          //限制场次
	UINT8  m_victoryPoints;     //胜利点数

	stCommonPart()
	{
		m_openTime		=	0;
		m_endTime		=	0;
		m_clearing		=	0;
		m_levelID		=	0;
		m_limitnum  	=   0;
		m_victoryPoints	=	0;
	}
};


struct stPreliminary : stCommonPart     //初赛配置
{
	UINT32 m_participationBOX;	//参与奖励
	UINT32 m_victoryBOX;		//获胜奖励

	stPreliminary()
	{
		m_participationBOX	=	0;
		m_victoryBOX		=	0;
	}
};

struct stSemiElement
{
	UINT8	m_id;
	UINT16	m_startrank;
	UINT16	m_endrank;
	UINT32 	m_rewardid;

	stSemiElement()
	{
		m_id		=	0;
		m_startrank	=	0;
		m_endrank	=	0;
		m_rewardid	=	0;
	}
};


struct stIntermediary : stCommonPart		//复赛配置
{
	std::map<UINT8, stSemiElement> m_semiConfig;	

	stIntermediary()
	{
	}
};

struct stFinalElement
{
	UINT8  m_id;				//id
	UINT8  m_screening; 		//场次
	
	UINT32 m_beginBattletime;	//战斗开始时间
	UINT32 m_endBattletime;	 	//战斗结束时间
	
	UINT32 m_beginResttime; 	//休息开始时间
	UINT32 m_endResttime;		//休息结束时间

	UINT32 m_participationBOX;	//失败奖励
	UINT32 m_victoryBOX;		//获胜奖励

	stFinalElement()
	{
		m_id				=	0;
		m_screening			=	0;      

		m_beginBattletime	=	0;
		m_endBattletime		=	0;  

		m_beginResttime		=	0;  
		m_endResttime		=	0;
		
		m_victoryBOX		=	0;
	}

};

struct stFinals								//决赛配置
{
	UINT32 m_applyTime;	//报名时间
	UINT32 m_clearing;	//结算时间
	UINT32 m_levelid;	//场景ID

	std::map<UINT8, stFinalElement> m_finalConfig;
};


struct stWorship
{
	UINT8  m_id;				//id
	UINT32 m_differentitem;	//膜拜奖励
	UINT32 m_denticalitem;	//同服膜拜奖励

	stWorship()
	{
		m_id			=	0;
		m_differentitem	=	0;
		m_denticalitem	=	0; 
	}
};

class CHeroLeagueCfg
{
	USE_SINGLETON_NEW(CHeroLeagueCfg);
private :
	CHeroLeagueCfg();
public:
	//构造函数 
	static CHeroLeagueCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	//加载配置 
	bool loadCfg();
	//初赛开放
	bool isPreOpen(UINT32 sec);
	//复赛开放
	bool isRemOpen(UINT32 sec);
	//决赛报名开放
	bool isFinalOpen(UINT32 sec);
	//决赛战斗时间开放
	bool isFinalFightOpen(UINT32 sec);
	//获得初赛配置
	stPreliminary getPreConfiguration()
	{
		return m_preliminary;
	}
	//获得复赛配置
	stIntermediary getIntermediary()
	{
		return m_intermediary;
	}
	//复赛结算 根据排名获得奖励
	UINT32 getRewardid(UINT16 sortRank);
	//获得决赛配置
	stFinals getFinals()
	{
		return m_finals;
	}

	//根据id获得决赛场次信息
	stFinalElement * getFinalsSessionInfo(UINT8 id);

	//决赛场次战斗开始
	bool isFinalBeginBattleTime(UINT32 sec);
	//决赛场次休息开始
	bool isFinalBeginRestTime(UINT32 sec);

	stWorship getWorskipConfig()
	{
		return m_worship;
	}
	//决赛场次结算时间
	bool finalMiddleSettlement(UINT32 sec);
private:
	stPreliminary	m_preliminary;			//初赛配置
	stIntermediary	m_intermediary;			//复赛配置
	stFinals		m_finals;				//决赛配置
	stWorship		m_worship;					//膜拜配置

};

#endif
