#ifndef __H_MAP_TEMPLATE_H__
#define __H_MAP_TEMPLATE_H__
#include "TypesDef.h"
#include "Cell.h"
#include "Area.h"
#include "NpcDataDefine.h"

enum eSummonTrigger
{
	eSummon_Trigger_Npc_All_Die = 1,		//Npc全部死亡触发
};

struct stSummonWave
{
	UINT8		   m_id;					//波次
	NpcDefineVec   m_npcDefines;       		//npc配置
	UINT8		   m_triggerLogic;			//触发逻辑
	UINT32 		   m_delay;					//延迟多久刷
	CPos		   m_dirPos;				//方向坐标
	UINT32 		   m_lastTime;				//持续时间

	stSummonWave()
	{
		m_id = 0;
		m_triggerLogic = 0;
		m_delay = 0;
		m_lastTime = 0;
	}
};

enum
{
	MAP_TYPE_STATIC = 1,        //静态地图
	MAP_TYPE_DYN = 2,           //动态地图
};

//区域类型
enum ZONE_TYPE
{
	ZONE_TYPE_BIRTH = 1,    //出生点
	ZONE_TYPE_RES = 2,		//资源区域
	ZONE_TYPE_BUFF = 3,		//buff区域
};

struct ZoneDefine
{
	CRectArea m_rect;
	UINT32    m_type;

	ZoneDefine()
	{   
		m_type = 0;  
	}   
};

typedef std::vector<ZoneDefine> ZoneDefineVec;


struct stMapTemplate
{
	UINT32  m_mapID;									//地图ID
	std::string	m_name;									//名字
	INT32   m_mapX, m_mapZ;                             //地图的X和Y范围,以世界坐标为单位
	UINT32  m_enterNum;									//进入次数
	CAreaIdx m_areaIdxMax;                              //区域编号的范围，闭区间
	CCellIdx m_cellIdxMax;                              //单元格编号的范围
	std::map<UINT16,CPos>  m_bornPoss;					//出生点
	std::map<UINT16,stSummonWave> m_summonWave;			//召唤配置 <wave,stSummonWave>
	UINT8    m_type;                                    //地图类型,动态还是静态
	std::vector<CRectArea>  m_walkables;				//可行走区
	UINT16   m_needLv;									//需要等级
	std::map<UINT16,ZoneDefine>  m_resZoneDefines;		//资源点
	std::map<UINT16,ZoneDefine>  m_buffZoneDefines;		//buff区域

	stMapTemplate()
	{
		m_mapID = 0;
		m_mapX = 0;
		m_mapZ = 0;
		m_enterNum = 0;
		m_type = 0;
		m_needLv = 0;
	}
};
#endif
