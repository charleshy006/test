#ifndef __H_PKDATA_H__
#define __H_PKDATA_H__

#include "TypesDef.h"
struct stBufferState
{
	INT32 m_damagePropPer;			//改变伤害属性百分比
	INT32 m_damagePer;				//改变伤害百分比
	INT32 m_refBang;				//改变的抗暴击
	INT32 m_hitRate;				//改变的命中率
	INT32 m_dodge;					//改变的闪避
	INT32 m_bang;				//改变的抗暴击

	void reset()
	{
		m_damagePropPer = 0;
		m_damagePer = 0;
		m_refBang = 0;
		m_hitRate = 0;
		m_dodge = 0;
		m_bang = 0;
	}

	stBufferState()
	{
		reset();
	}
};

struct stPKCalcData
{
	UINT32 m_hitRate;                       //命中率 %
	UINT32 m_dodgeRate;                     //闪避率 %
	UINT32 m_bangRate;                      //暴击概率 %
	UINT32 m_antiBangRate;                  //抗暴击概率 %
	UINT32 m_bangDamage;                    //暴击伤害 %	
	UINT32 m_antiBangDamge;                 //暴击免伤
	UINT32 m_finalDamage;                   //最终伤害
	UINT32 m_damageReduce;                  //伤害减免
	UINT32 m_damage;						//自身伤害
	UINT32 m_skillDamage;					//技能伤害%
	UINT32 m_skillAddDamage;				//技能额外伤害
	UINT32 m_fireRate;						//火强概率
	UINT32 m_iceRate;						//冰冻概率
	UINT32 m_thunderRate;					//雷电概率
	UINT32 m_refFireRate;					//火强概率免疫
	UINT32 m_refIceRate;					//冰冻概率免疫
	UINT32 m_refThunderRate;				//雷电概率免疫
	UINT32 m_fireDamage;					//火伤害
	UINT32 m_iceDamage;						//冰冻伤害
	UINT32 m_thunderDamage;					//雷电伤害
	UINT32 m_refFireDamage;					//火伤害免疫
	UINT32 m_refIceDamage;					//冰伤害免疫
	UINT32 m_refThunderDamage;				//雷电伤害免疫
	UINT32 m_attackResult;					//攻击结果

	stPKCalcData()
	{
		reset();
	}

	void reset()
	{
		m_hitRate = 0;
		m_dodgeRate = 0;
		m_bangRate = 0;
		m_antiBangRate = 0;
		m_bangDamage = 0;
		m_antiBangDamge = 0;
		m_finalDamage = 0;
		m_damageReduce = 0;
		m_damage = 0;
		m_skillDamage = 0;
		m_skillAddDamage = 0;
		m_fireRate = 0;
		m_iceRate = 0;
		m_thunderRate = 0;
		m_refFireRate = 0;
		m_refIceRate = 0;
		m_refThunderRate = 0;
		m_fireDamage = 0;
		m_iceDamage = 0;
		m_thunderDamage = 0;
		m_refFireDamage = 0;
		m_refIceDamage = 0;
		m_refThunderDamage = 0;
		m_attackResult = 0;
	}
};
#endif
