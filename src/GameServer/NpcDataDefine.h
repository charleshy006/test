#ifndef __H_NPC_DATA_DEFINE_
#define __H_NPC_DATA_DEFINE_
#include <string.h>
#include <string>
#include "RectArea.h"
#include "tinyxml/tinyxml.h"


//对应地图中NPC附加定义
struct NpcDefine
{
	UINT32 m_id;					//NPC的道具ID
	UINT16 m_lv;					//npc等级
	UINT32 m_x;						//x坐标 
	UINT32 m_z;						//z坐标
	UINT32 m_y;						//y坐标
	UINT16  m_isBoss;				//是否是boss
	UINT32 m_aminID;				//动画ID
	UINT16 m_dirAngle;				//方向角度,单位是度
	UINT16 m_actionGroupID;			//动作组ID
	UINT32 m_lastTime;				//持续时间

	NpcDefine()
	{
		m_id = 0;
		m_lv = 0;
		m_x = 0;
		m_z = 0;
		m_y = 0;
		m_isBoss = 0;
		m_aminID = 0;
		m_dirAngle = 0;
		m_actionGroupID = 0;
		m_lastTime = 0;
	}

	NpcDefine(const NpcDefine& define)
	{
		operator=(define);
	}

	NpcDefine& operator=(const NpcDefine& define)
	{
		m_id = define.m_id;
		m_lv = define.m_lv;
		m_x = define.m_x;
		m_z = define.m_z;
		m_y = define.m_y;
		m_isBoss = define.m_isBoss;
		m_aminID = define.m_aminID;
		m_dirAngle = define.m_dirAngle;
		m_actionGroupID = define.m_actionGroupID;
		m_lastTime = define.m_lastTime;
		return *this;
	}
};

typedef std::vector<NpcDefine> NpcDefineVec ;

#endif

