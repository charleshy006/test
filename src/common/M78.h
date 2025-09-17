#ifndef _M_78_H__
#define _M_78_H__

#include "TypesDef.h"
#include "Singleton.h"
        
#include <map>
#include <vector>
#include <tinyxml/tinyxml.h>

struct stDrawItem
{
	UINT32 m_id;		//抽奖物品ID
	UINT32 m_drawnum;	//抽奖出的数量
	UINT32 m_drawRP;	//抽奖概率

	stDrawItem()
	{
		m_id		=	0;
		m_drawnum	=	0;
		m_drawRP	=	0;
	}
};

struct stDrawInfo
{
	UINT32 m_id;
	std::vector<stDrawItem> m_items;

	stDrawInfo()
	{
		m_id = 0;
	}
};

class CDrawInfoCfg
{
	USE_SINGLETON_NEW(CDrawInfoCfg);
public:
	//构造函数
	CDrawInfoCfg() {}
	//析构函数
	~CDrawInfoCfg() {}
	//获得实例                              
	static CDrawInfoCfg & getSingleton() {  
		return THE_SINGLETON::Instance();   
	}    
	//加载配置
	bool loadCfg();
	//根据id和抽奖次数获取攻击奖励
	void getAttackRewardById(UINT32 id, UINT32 drawNum, bool isAkey, std::vector<std::pair<UINT32,UINT32> > & vec);

	//根据id和抽奖次数获取死亡奖励
	void getDeadRewardById(UINT32 id, UINT32 drawNum, std::vector<std::pair<UINT32,UINT32> > & vec);

	//根据id和概率和抽奖次数获取奖励
	void getAttackRewardByIdAndRP(UINT32 id, UINT32 rp, UINT32 drawNum, std::vector<std::pair<UINT32,UINT32> > & vec);
private:
	std::map<UINT32, stDrawInfo> m_items;

	std::map<UINT32, stDrawInfo> m_deadItems;

	std::map<UINT32, stDrawInfo> m_exDrawItems;
};


class CM78Cfg
{
	USE_SINGLETON_NEW(CM78Cfg);
public:
	struct stBoss
	{
		UINT32 m_id;        //id
		UINT32 m_bossLv;    //boss等级
		UINT32 m_hp;        //boss生命值
		UINT32 m_drawID;    //抽奖池
		UINT32 m_deadID;    //死亡奖励
		UINT32 m_countNum;  //每次抽奖次数
		UINT32 m_exDraw;	//额外抽奖池
		UINT32 m_exrp;		//额外获得几率%

		stBoss()
		{
			m_id        =   0;
			m_bossLv    =   0;
			m_hp        =   0;
			m_drawID    =   0;
			m_deadID    =   0;
			m_countNum  =   0;
			m_exDraw	=	0;
			m_exrp		=	0;
		}
	}; 

	struct stBossInfo
	{
		UINT32 m_maxCount;	//最大次数
		UINT32 m_time;		//回复时间

		std::map<UINT32, stBoss> m_bossMap;

		stBossInfo()
		{
			m_maxCount = 0;
			m_time     = 0;
		}
	};


	struct stBuyInfo
	{
		UINT32 m_mixBuyNum; //最小购买次数
		UINT32 m_maxBuyNum;	//最大购买次数
		UINT32 m_cost;		//购买所需钻石

		stBuyInfo()
		{
			m_mixBuyNum = 0;
			m_maxBuyNum = 0;
			m_cost		= 0;     
		}	
	};

	struct stBuyCount
	{
		UINT32 m_buyCount;//购买获得次数
		UINT32 m_costItemID;//购买花费道具ID
		std::vector<stBuyInfo> m_buyInfo;

		stBuyCount()
		{
			m_buyCount = 0;
			m_costItemID = 0;
		}
	};

public:
	//构造函数
	CM78Cfg(){}
	//析构函数
	~CM78Cfg(){}
	//获得实例 
	static CM78Cfg & getSingleton() { 
		return THE_SINGLETON::Instance();
	}

	//加载配置 
	bool loadCfg();

	//获得攻击次数
	UINT32 getMaxCount()
	{
		return m_bossInfo.m_maxCount;
	}
	
	//根据等级获取boss信息
	stBoss * getBossByLv(UINT32 lv);
	//根据购买次数获得需要钻石
	UINT32 getNeedDiamondByNum(UINT32 num);
	//获得购买次数
	UINT32 getBuyCount()
	{
		return m_buyCount.m_buyCount;
	}
	//获得购买次数
	UINT32 getCostItemID()
	{
		return m_buyCount.m_costItemID;
	}
	//获得恢复时间
	UINT32 getRecoveryTime(){
		return m_bossInfo.m_time;
	}
private:
	stBossInfo m_bossInfo;
	stBuyCount m_buyCount;
};

#endif
