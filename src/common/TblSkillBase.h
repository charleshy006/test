#ifndef __TBL_SKILLBASE_H__
#define __TBL_SKILLBASE_H__

#include <TypesDef.h>
#include <string>
#include "CommonMicro.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "BattlePropValue.h"

struct BufferRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}
	UINT32 m_field_1;			//buffID
	char   m_field_2[33];		//名字
	UINT32 m_field_3;			//减益buff标志
	UINT32 m_field_4;			//不可消除标志
	UINT32 m_field_5;			//逻辑效果类型
	UINT32 m_field_6;			//参数1
	UINT32 m_field_7;			//参数2
	UINT32 m_field_8;			//参数3
	UINT32 m_field_9;			//参数4
	UINT32 m_field_10;			//参数5
	UINT32 m_field_11;			//参数6
	UINT32 m_field_12;			//目标对象
}__attribute__((packed));

enum eSkillTarget
{
	eSkill_Target_Self = 1,		//自身
	eSkill_Target_Master = 2,	//主人
	eSkill_Target_Friend = 4,	//友友
	eSkill_Target_Enemy = 8,	//敌方
};

struct BufferBase
{
	UINT32 m_id;						//buffID
	char   m_name[MAX_NAMESIZE + 1];	//buf名字
	bool   m_bDebuff;					//是否减益buf
	bool   m_bCanClear;					//不可消除
	UINT32 m_opID;						//操作效果ID
	std::vector<UINT32> m_paramsVec;	//参数类型
	UINT32 m_target;					//目标对象

	void fill(const BufferRawData & data)
	{
		m_id = data.m_field_1;
		strncpy(m_name,data.m_field_2,sizeof(m_name));
		if (data.m_field_3){
			m_bDebuff = true;
		}
		else {
			m_bDebuff = false;
		}

		if (!data.m_field_4){
			m_bCanClear = false;
		}
		else {
			m_bCanClear = true;
		}

		m_opID = data.m_field_5;
		
		m_paramsVec.push_back(data.m_field_6);
		m_paramsVec.push_back(data.m_field_7);
		m_paramsVec.push_back(data.m_field_8);
		m_paramsVec.push_back(data.m_field_9);
		m_paramsVec.push_back(data.m_field_10);
		m_paramsVec.push_back(data.m_field_11);
		m_target = data.m_field_12;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("id=%d,name=%s,是否增益buf:%s,是否不可清除:%s,opID:%d,param1:%d,param2:%d,param3:%d,param4:%dparam5:%d,param6:%d,",m_id,m_name,m_bDebuff ? "是":"否",m_bCanClear?"是":"否",m_opID,data.m_field_6,data.m_field_7,data.m_field_8,data.m_field_9,data.m_field_10,data.m_field_11);
#endif
	}
};

struct SkillTblRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}

	UINT32  m_field_1;						//技能ID
	char    m_field_2[33];					//技能名字
	UINT32  m_field_3;						//技能类型
	UINT32  m_field_4;						//针对目标
	UINT32  m_field_5;						//成长类型
	char    m_field_6[100];                 //buff
	UINT32  m_field_7;						//开放等级限制
	UINT32  m_field_8;						//破防值1
	UINT32  m_field_9;						//破防值2
	UINT32  m_field_10;						//破防值3
	UINT32  m_field_11;						//破防值4
	UINT32	m_field_12;						//是否普通攻击
	UINT32  m_field_13;						//是否计算伤害
	UINT32  m_field_14;						//技能品质
}__attribute__((packed));

enum
{
	eSkill_Type_Passitive = 1,		//被动
	eSkill_Type_Active = 2,			//主动
};

enum
{
	TARGET_SELF  = 1,			//自己
	TARGET_FRIEND  = 2,			//友方
	TARGET_FRIEND_AND_SELF = 3, //友方和自己
	TARGET_EMENY = 4,			//敌方
};

enum 
{
	SKILL_QUALITY_GREEN = 1,	//绿色
	SKILL_QUALITY_BLUE  = 2,	//蓝色
	SKILL_QUALITY_PURPLE = 3,	//紫色
	SKILL_QUALITY_ORANGE = 4,	//橙色
};

struct stSkillOp
{
	UINT16 m_opID;						//操作效果ID
	std::vector<UINT32> m_paraVec;		//参数列表

	stSkillOp()
	{
		m_opID  = 0;
	}
};

struct stSkillBase
{
	UINT32 m_skillID;						//技能ID
	char   m_name[MAX_NAMESIZE + 1];		//名字
	UINT8  m_skillType;						//技能类型
	UINT8  m_target;						//目标
	UINT8  m_grownType;						//成长类型
	//std::vector<stSkillOp> m_buffVec;		//buff
	std::vector<UINT32>	m_buffVec;
	UINT16 m_openLv;						//开放等级
	UINT16 m_poFang1;						//破防1
	UINT16 m_poFang2;						//破防2
	UINT16 m_poFang3;						//破防3
	UINT16 m_poFang4;						//破防4
	bool   m_isNormal;						//是否是普通攻击
	bool   m_isCalcDam;						//是否计算伤害
	UINT16 m_quality;						//技能品质

	void fill(const SkillTblRawData & data)
	{
		m_skillID = data.m_field_1;
		strncpy(m_name,data.m_field_2,sizeof(m_name));
		m_skillType = data.m_field_3;
		m_target = data.m_field_4;
		m_grownType = data.m_field_5;

		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_6,"|");
		for (UINT16 i = 0;i < strVec.size();++i){
			m_buffVec.push_back((UINT32)atoi(strVec[i].c_str()));
			/*std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size()){
				stSkillOp op;
				op.m_opID = (UINT16)atoi(subStrVec[0].c_str());
			}*/
		}
		m_openLv = data.m_field_7;
		m_poFang1 = data.m_field_8;
		m_poFang2 = data.m_field_9;
		m_poFang3 = data.m_field_10;
		m_poFang4 = data.m_field_11;
		if (data.m_field_12){
			m_isNormal = true;
		}
		else {
			m_isNormal = false;
		}

		if (data.m_field_13){
			m_isCalcDam = true;
		}
		else {
			m_isCalcDam = false;
		}
		m_quality = data.m_field_14;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("加载了技能id=%d,name=%s",m_skillID,m_name);
#endif
	}
};

#define SKILL_GROWN_COEF 1000

struct SkillGrownRawData
{
	INT32 getID() const
	{
		return m_field_1 * SKILL_GROWN_COEF + m_field_2;
	}

	UINT32  m_field_1;						//技能ID
	UINT32  m_field_2;						//技能等级
	UINT32  m_field_3;						//技能最大等级
	UINT32  m_field_4;						//法力消耗
	UINT32  m_field_5;						//冷却时间
	UINT32  m_field_6;						//强化等级要求
	UINT32  m_field_7;						//所需消耗类型
	UINT32  m_field_8;						//所需强化消费
	UINT32  m_field_9;						//单击威力1
	UINT32  m_field_10;						//额外威力1
	UINT32  m_field_11;						//单击威力2
	UINT32  m_field_12;						//额外威力2
	UINT32  m_field_13;						//单击威力3
	UINT32  m_field_14;						//额外威力3
	UINT32  m_field_15;						//单击威力4
	UINT32  m_field_16;						//额外威力4
	UINT32  m_field_17;						//销毁技能返还经验
	char    m_field_18[50];                 //被动技能属性
	UINT32  m_field_19;						//增加的战力
	UINT32  m_field_20;						//攻击次数1
	UINT32  m_field_21;						//攻击次数2
	UINT32  m_field_22;						//攻击次数3
	UINT32  m_field_23;						//攻击次数4
	char    m_field_24[50];                 //技能强化条件
}__attribute__((packed));

struct SkillGrownBase
{
	UINT32 m_skillID;			//技能ID
	UINT16 m_skillLv;			//技能等级
	UINT32 m_skillMaxLv;		//技能最大等级
	UINT32 m_costMP;			//法力消耗
	UINT16 m_coolDown;			//冷却时间
	UINT16 m_needLv;			//强化等级要求
	UINT8  m_costType;			//强化消耗类型
	UINT32 m_cost;				//强化消费
	UINT32 m_damage1;			//单击威力1
	UINT32 m_addDamage1;		//额外威力1
	UINT32 m_damage2;			//单击威力2
	UINT32 m_addDamage2;		//额外威力2
	UINT32 m_damage3;			//单击威力3
	UINT32 m_addDamage3;		//额外威力3
	UINT32 m_damage4;			//单击威力4
	UINT32 m_addDamage4;		//额外威力4
	UINT32 m_returnExp;			//返还经验
	stBattlePropValue m_battleProp;//战斗属性
	UINT32 m_addBP;				//增加战力
	UINT32 m_attackNum;			//攻击次数
	UINT32 m_intensifySkillCondition;			//强化技能所需对应技能id
	UINT32 m_intensifySkillVal;                 //强化技能所需对应技能等级

	static UINT32 getFindID(UINT32 skillID,UINT16 lv)
	{
		return skillID * SKILL_GROWN_COEF + lv;
	}

	void fill(const SkillGrownRawData & data)
	{
		m_skillID = data.m_field_1;
		m_skillLv = data.m_field_2;
		m_skillMaxLv = data.m_field_3;
		m_costMP = data.m_field_4;
		m_coolDown = data.m_field_5;
		m_needLv = data.m_field_6;
		m_costType = data.m_field_7;
		m_cost = data.m_field_8;
		m_damage1 = data.m_field_9;
		m_addDamage1 = data.m_field_10;
		m_damage2 = data.m_field_11;
		m_addDamage2 = data.m_field_12;
		m_damage3 = data.m_field_13;
		m_addDamage3 = data.m_field_14;
		m_damage4 = data.m_field_15;
		m_addDamage4 = data.m_field_16;
		m_returnExp = data.m_field_17;
	
		m_battleProp.init(data.m_field_18);
		m_addBP = data.m_field_19;
		m_attackNum = data.m_field_20 + data.m_field_21 + data.m_field_22 + data.m_field_23;

		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_24,"-");
        if (strVec.size() == 2) {
            m_intensifySkillCondition = (UINT32)atoi(strVec[0].c_str());
            m_intensifySkillVal = (UINT32)atoi(strVec[1].c_str());
        } else {
            m_intensifySkillCondition = 0;
            m_intensifySkillVal = 0;
        }
#ifdef _HDZ_DEBUG
		Athena::logger->trace("技能成长表 id=%d,level=%d,damage1=%d,addDamage1=%d,costMP=%d,coolDown=%d,damage1=%d,addDamge1=%d,总的攻击次数:%d",m_skillID,m_skillLv,m_damage1,m_addDamage1,m_costMP,m_coolDown,m_damage1,m_addDamage1,m_attackNum);
#endif
	}
};
#endif
