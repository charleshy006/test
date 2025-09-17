#ifndef __SKILLINFO_H__
#define __SKILLINFO_H__

#include <cstdlib>
#include "TypesDef.h"
#include "CommonMicro.h"

enum
{
	SKILL_TYPE_SKILL = 0,	//技能攻击
	SKILL_TYPE_NORMAL = 1,	//普通技能
	SKILL_TYPE_BULLET = 2,	//字段攻击
};

#pragma pack(1)
struct SkillInfo 
{
	SkillInfo()
	{
		m_skillID = 0;
		m_level = 0;
		m_useTime = 0;
	}

	UINT32	  m_skillID;					//技能ID
	UINT8     m_level;						//技能等级
	UINT32    m_useTime;					//使用时间
};
#pragma pack()
#endif


