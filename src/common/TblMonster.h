#ifndef __TBL_MONSTERBASE_H__
#define __TBL_MONSTERBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

#define MONSTER_LEVEL_COEF 2301

struct stMonsterRawData
{
	INT32 getID() const
	{
		return m_field_1 * MONSTER_LEVEL_COEF + m_field_26;
	}
	UINT32 m_field_1;				//编号
	UINT32 m_field_2;				//生命
	UINT32 m_field_3;				//伤害
	UINT32 m_field_4;				//法力
	UINT32 m_field_5;				//命中概率
	UINT32 m_field_6;				//闪避率
	UINT32 m_field_7;				//暴击概率
	UINT32 m_field_8;				//暴击伤害
	UINT32 m_field_9;				//抗暴击
	UINT32 m_field_10;				//暴击免伤
	UINT32 m_field_11;				//伤害减免
	UINT32 m_field_12;				//最终伤害
	UINT32 m_field_13;				//普攻回血
	UINT32 m_field_14;				//普攻吸血
	UINT32 m_field_15;				//伤害反弹
	UINT32 m_field_16;				//火强伤害
	UINT32 m_field_17;				//冰冻伤害
	UINT32 m_field_18;				//雷电伤害
	UINT32 m_field_19;				//火强触发概率
	UINT32 m_field_20;				//冰冻触发概率
	UINT32 m_field_21;				//雷电触发概率
	UINT32 m_field_22;				//霸体值
	UINT32 m_field_23;				//火强免伤
	UINT32 m_field_24;				//冰冻免伤
	UINT32 m_field_25;				//雷电免伤
	UINT32 m_field_26;				//等级
	UINT32 m_field_27;				//火强触发免疫
	UINT32 m_field_28;				//冰冻触发免疫
	UINT32 m_field_29;				//雷电触发免疫
}__attribute__((packed));


struct stMonsterBase
{
	UINT32 m_id;							//关卡ID
	UINT32 m_hp;							//血量成长
	UINT32 m_damage;						//伤害成长
	UINT32 m_mp;							//魔法成长
	UINT32 m_hitRate;        				//命中率 %
	UINT32 m_dodgeRate;      				//闪避率 %
	UINT32 m_bangRate;       				//暴击概率 %
	UINT32 m_bangDamage;     				//暴击伤害 %
	UINT32 m_antiBangRate;   				//抗暴击概率 %
	UINT32 m_antiBangDamge;	 				//暴击免伤
	UINT32 m_damageReduce;   				//伤害减免
	UINT32 m_finalDamage;    				//最终伤害
	UINT32 m_recoverHP;      				//生命回复
	UINT32 m_absorbHP;       				//普攻吸血 %
	UINT32 m_reflectDamage;  				//伤害反弹 %
	UINT32 m_fireDamage;     				//火强伤害
	UINT32 m_iceDamage;      				//冰墙伤害
	UINT32 m_thunderDamage;  				//雷电伤害
	UINT32 m_fireRate;       				//火强概率
	UINT32 m_iceRate;        				//冰墙概率
	UINT32 m_thunderRate;    				//雷电概率
	UINT32 m_recoverMP;      				//魔法回复
	UINT32 m_baTi;							//霸体成长
	UINT32 m_antiFireDamage;				//火强免伤
	UINT32 m_antiIceDamage;					//冰冻免伤
	UINT32 m_antiThunderDamage;				//雷电免伤
	UINT16 m_level;							//等级
	UINT32 m_antiFireRate;  				//火强概率免疫
	UINT32 m_antiIceRate;					//冰冻概率免疫
	UINT32 m_antiThunderRate;				//雷电概率免疫

	static UINT32 getFindID(UINT32 id,UINT16 lv)
	{
		return id * MONSTER_LEVEL_COEF + lv;
	}

	void fill(const stMonsterRawData & data)
	{
		m_id = data.m_field_1;
		m_hp = data.m_field_2;
		m_damage = data.m_field_3;
		m_mp  = data.m_field_4;
		m_hitRate = data.m_field_5;
		m_dodgeRate = data.m_field_6;
		m_bangRate = data.m_field_7;
		m_bangDamage = data.m_field_8;
		m_antiBangRate = data.m_field_9;
		m_antiBangDamge = data.m_field_10;
		m_damageReduce = data.m_field_11;
		m_finalDamage = data.m_field_12;
		m_recoverHP = data.m_field_13;
		m_absorbHP = data.m_field_14;
		m_fireDamage = data.m_field_15;
		m_iceDamage = data.m_field_16;
		m_thunderDamage = data.m_field_17;
		m_fireRate = data.m_field_18;
		m_iceRate = data.m_field_19;
		m_thunderRate = data.m_field_20;
		m_recoverMP = data.m_field_21;
		m_baTi = data.m_field_22;
		m_antiFireDamage = data.m_field_23;
		m_antiIceDamage = data.m_field_24;
		m_antiThunderDamage = data.m_field_25;
		m_level = data.m_field_26;
		m_antiFireRate = data.m_field_27;  				//火强概率免疫
		m_antiIceRate = data.m_field_28;				//冰冻概率免疫
		m_antiThunderRate = data.m_field_29;			//雷电概率免疫

#ifdef _HDZ_DEBUG
		Athena::logger->trace("怪物配置配置id=%d,hp=%u,mp=%d",m_id,m_hp,m_mp);
#endif
	}
};

struct NpcAIRawData
{
	INT32 getID() const 
	{
		return m_field_1;
	}
	UINT32 m_field_1;				//AI id
	UINT16 m_field_2;				//攻击范围
	UINT8  m_field_3;				//兴趣类型
	char   m_field_4[50];			//监视条件
	char   m_field_5[50];			//触发事件
}__attribute__((packed));

enum
{
	AI_TRIGER_COND_HP = 1 ,			//HP触发
	AI_TRIGER_COND_DIS = 2,			//距离触发
};
enum
{
	AI_TRIGER_EVENT_USESKILL = 1, 	//使用技能
	AI_TRIGER_EVENT_SPEAK = 2,	  	//说话
};

struct NpcAIBase 
{
	typedef std::pair<UINT8,UINT32>  TTrigerCond;
	UINT32 m_id;												//AI id
	UINT16 m_attRange;											//攻击范围
	UINT8  m_targetType;										//目标类型 1:仇恨值最高的,0 否
	std::vector<TTrigerCond> m_trigerCond;  					//触发条件
	std::vector<UINT8> m_trigerEvent;							//触发事件

	void fill(const NpcAIRawData & data)
	{
		m_id = data.m_field_1;
		m_attRange = data.m_field_2;	

		std::vector<std::string> strVec;
		m_targetType = data.m_field_3;
		m_trigerCond.clear();
		g_stringtok(strVec,data.m_field_4,";");	//触发条件

		for (UINT8 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i],"-");
			if (subStrVec.size() >= 2){
				TTrigerCond trigerCond;
				trigerCond.first = (UINT8)atoi(subStrVec[0].c_str());
				trigerCond.second = (UINT32)atoi(subStrVec[1].c_str());
				m_trigerCond.push_back(trigerCond);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[AI属性的触发事件]%d,%d",trigerCond.first,trigerCond.second);
#endif
			}
		}

		m_trigerEvent.clear();
		strVec.clear();
		g_stringtok(strVec,data.m_field_5,",");   //触发事件
		for (UINT8 i = 0;i < strVec.size();++i){
			m_trigerEvent.push_back(atoi(strVec[i].c_str()));
		}

#ifdef _HDZ_DEBUG
		Athena::logger->trace("[AI属性数据]id=%d,attRange=%d,targetType=%d,cond=%d,event=%d",m_id,m_attRange,m_targetType,m_trigerCond.size(),m_trigerEvent.size());
#endif
	}
};

#endif
