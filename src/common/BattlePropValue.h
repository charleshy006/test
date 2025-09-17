#ifndef __H_BATTLE_PROP_VALUE_H__
#define __H_BATTLE_PROP_VALUE_H__
#include "TypesDef.h"

enum eFuwenActivePropType
{
	ActivePropType_Raise = 1,		//强化等级
	ActivePropType_Refine = 2,		//精炼等级
};

struct stBattlePropValue
{
	UINT8  m_propID;
	UINT32 m_val1;
	UINT32 m_val2;
	UINT32 m_val3;
	UINT32 m_val4;

	stBattlePropValue()
	{   
		m_propID = 0;
		m_val1 = 0;
		m_val2 = 0;
		m_val3 = 0;
		m_val4 = 0;
	} 

	stBattlePropValue &  operator += (const stBattlePropValue & one);
	void init(std::string str);
};

#endif
