#ifndef __DISCOVER_CFG_H__
#define __DISCOVER_CFG_H__

#include "TypesDef.h"
#include "Singleton.h"
#include <map>

struct stResetNum
{
	UINT8	m_resetNum;		//购买次数
	UINT16	m_resetPrice;	//价格

	stResetNum()
	{
		m_resetNum		= 	0;
		m_resetPrice	= 	0;
	}
};

struct stDiscoverBounds                       
{                                     
	UINT8   m_id;           //ID      
	UINT32  m_bounds;       //积分    
	UINT32  m_boundsBox;    //积分宝箱

	stDiscoverBounds()
	{
		m_id		=	0;
		m_bounds	= 	0;   
		m_boundsBox	= 	0;
	}
};                                    


struct stDiscoverMap
{                                     
	UINT8   m_id;       //ID          
	UINT32  m_mapID;    //地图ID      
	UINT32  m_mapRP;    //地图概率

	stDiscoverMap()
	{
		m_id	=	0;   
		m_mapID	=	0;
		m_mapRP	=	0;
	}
};

struct stLevelID
{
	UINT32 m_level_id_1;	//关卡id 1
	UINT32 m_level_id_2;	//关卡id 2

	stLevelID()
	{
		m_level_id_1 = 0;
		m_level_id_2 = 0;
	}
};

struct stPlayLevelRange
{
	std::pair<UINT16, UINT16> m_vec_1;	//最低等级、最高等级
	std::pair<UINT16, UINT16> m_vec_2;	//最低等级、最高等级
};

struct stLevelIDAndPlayLevel
{
	std::vector<UINT32> m_levelIDVer;		//关卡ID
	std::vector<std::pair<UINT16, UINT16> > m_playLevelVer; //角色等级
};


struct stSquareType
{
	UINT8	m_id;			//id
	UINT32	m_rewardBox;	//宝箱
	UINT8	m_battleType;	//战斗类型（1：完成普通关卡 2：完成精英关卡 3：完成多人副本 4：完成天梯排行)
	UINT8	m_battleNum;	//战斗次数
	UINT8	m_rewardBounds;	//奖励积分
	UINT32	m_battleRP;		//概率

	stLevelIDAndPlayLevel m_level; 

	stSquareType()
	{
		m_id			=	0;
		m_rewardBox		=	0;
		m_battleType	=	0;
		m_battleNum		=	0;
		m_rewardBounds	=	0;
		m_battleRP		=	0;
	}
};

enum eSquareType
{
	eSQUARE_TYPE_START = 0,	//起点
	eSQUARE_TYPE_DIAMOND,	//砖石
	eSQUARE_TYPE_GOLD,		//金币
	eSQUARE_TYPE_DRAGON,	//龙晶石
	eSQUARE_TYPE_BOSS,		//BOSS
	eSQUARE_TYPE_FIGHT_TASK,//战斗任务
	eSQUARE_TYPE_ORDINARY_TASK,//普通任务
	eSQUARE_TYPE_ITEM_BOX,	//道具宝箱
	eSQUARE_TYPE_AFTER_BOX,	//随从宝箱
	eSQUARE_TYPE_END,		//终点
	eSQUARE_TYPE_BLANK,		//空白格
};

class CDiscoverCfg
{
	USE_SINGLETON_NEW(CDiscoverCfg);
private :
	//构造函数
	CDiscoverCfg();
public :
	//析构函数
	~CDiscoverCfg();
	//获得实例
	static CDiscoverCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载所有配置
	bool loadCfg();

public:
	//获得免费次数
	UINT8 getFreeNum(){
		return m_freeNum;
	}
	//通过刷新次数获得需要扣除的砖石数量
	UINT32 getNeedDiamondNum(UINT32 num);
	//通过Id获取积分宝箱
	stDiscoverBounds * getBoundsBoxByid(UINT8 id);
	//获得积分宝箱数量
	UINT8 getBoundsBoxNum(){
		return m_boundsBox.size();
	}
	//随机一张地图
	UINT32 randMapID();
	//获得地图最大格子数  (mapID:地图ID)
	UINT8 getMapSquareNum(UINT8 mapID);
	//获得某张地图某个格子的任务类型 (mapID:地图ID、square：第几个格子)
	std::pair<UINT8,UINT8> getSquareType(UINT8 mapID, UINT8 square);
	//随机某个格子的任务 (mapID:地图ID、square：第几个格子)
	stSquareType * randSquareTask(UINT8 mapID, UINT8 square);
	//获得某个格子的信息
	stSquareType * getSquareInfo(UINT8 mapID, UINT8 square, UINT8 taskID);
private:
	//免费次数
	UINT8 m_freeNum;
	//购买次数及价格
	std::map<UINT32, UINT32> m_buyNum;
	//积分宝箱
	std::map<UINT8, stDiscoverBounds> m_boundsBox;
	//地图
	std::map<UINT8, stDiscoverMap> m_map;
	//格子 <Map id, square ID, squareType.first, squareType.second>
	std::map<UINT8, std::map<UINT8, std::pair<UINT8, UINT8> > > m_square;
	//任务类型
	std::map<UINT8, std::map<UINT8, stSquareType> > m_squareType;
};

#endif 
