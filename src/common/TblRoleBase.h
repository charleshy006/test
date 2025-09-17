#ifndef __TBL_ROLEBASE_H__
#define __TBL_ROLEBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include <set>

enum 
{
	eCharID_Warrior_Man = 100000,		//男战士
	eCharID_Warrior_Female = 100001,	//女战士
	eCharID_Bowman_Man = 100002,		//男弓手
	eCharID_Bowman_Female= 100003,		//女弓手
	eCharID_Master_Man = 100004,		//男法师
	eCharID_Master_Female = 100005,		//女法师
};

struct stRoleRawData
{
	INT32 getID() const
	{
		return m_field_1 ;
	}
	
	UINT32 m_field_1;					//角色ID
	char   m_field_2[33];				//角色名字
	UINT32 m_field_3;					//角色类型
	UINT32 m_field_4;					//角色品质
	UINT32 m_field_5;					//角色品阶
	UINT32 m_field_6;					//种族类型
	UINT32 m_field_7;					//普通攻击
	UINT32 m_field_8;					//技能1
	UINT32 m_field_9;					//技能2
	UINT32 m_field_10;					//技能3
	UINT32 m_field_11;					//技能4
	UINT32 m_field_12;					//技能5
	UINT32 m_field_13;					//技能6
	UINT32 m_field_14;					//技能7
	UINT32 m_field_15;					//技能8
	UINT32 m_field_16;					//技能1解锁等级
	UINT32 m_field_17;					//技能2解锁等级
	UINT32 m_field_18;					//技能3解锁等级
	UINT32 m_field_19;					//技能4解锁等级
	UINT32 m_field_20;					//技能5解锁等级
	UINT32 m_field_21;					//技能6解锁等级
	UINT32 m_field_22;					//技能7解锁等级
	UINT32 m_field_23;					//技能8解锁等级
	UINT32 m_field_24;					//输出防御类型
	UINT32 m_field_25;					//近战远战类型
}__attribute__((packed));

enum eRoleType
{
	eRole_Char = 1,		//角色
	eRole_Monster = 2,  //怪
	eRole_Pet = 3,		//Pet
	eRole_Npc = 4,		//Npc
	eRole_Trap = 5,		//陷阱
};

enum eOutPutType
{
    eOut_Put_Attack = 1,    //进攻
    eOut_Put_Defense = 2,    //防御
    eOut_Put_Assist = 3,    //辅助
};

enum eBattleType
{
    eBattle_Near = 1,    //近程
    eBattle_Romote = 2,  //远程
};

struct stRoleBase
{
	UINT32 m_id;						//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//名字
	eRoleType  m_type;					//角色类型
	UINT8   m_quality;					//品质
	UINT8   m_classType;				//品阶
	UINT8   m_race;						//种群
	
	UINT32  m_normalSkill;				//普通攻击
	UINT32  m_skill1;					//技能1
	UINT32  m_skill2;					//技能2
	UINT32  m_skill3;					//技能3
	UINT32  m_skill4;					//技能4
	UINT32  m_skill5;					//技能5
	UINT32  m_skill6;					//技能6
	UINT32  m_skill7;					//技能7
	UINT32  m_skill8;					//技能8
	
	UINT32  m_skill1UnlockLv;			//技能1解锁等级
	UINT32  m_skill2UnlockLv;			//技能2解锁等级
	UINT32  m_skill3UnlockLv;			//技能3解锁等级
	UINT32  m_skill4UnlockLv;			//技能4解锁等级
	UINT32  m_skill5UnlockLv;			//技能5解锁等级
	UINT32  m_skill6UnlockLv;			//技能6解锁等级
	UINT32  m_skill7UnlockLv;			//技能7解锁等级
	UINT32  m_skill8UnlockLv;			//技能8解锁等级

    eOutPutType m_outPutType;           //输出防御类型
    eBattleType m_battleType;           //远近战类型

	void fill(const stRoleRawData & data)
	{
		m_id = data.m_field_1;
		strncpy(m_name,data.m_field_2,sizeof(m_name));
		m_type = (eRoleType)data.m_field_3;
		m_quality = data.m_field_4;
		m_classType = data.m_field_5;
		m_race = data.m_field_6;
		m_normalSkill = data.m_field_7;
		m_skill1 = data.m_field_8;
		m_skill2 = data.m_field_9;
		m_skill3 = data.m_field_10;
		m_skill4 = data.m_field_11;
		m_skill5 = data.m_field_12;
		m_skill6 = data.m_field_13;
		m_skill7 = data.m_field_14;
		m_skill8 = data.m_field_15;
		m_skill1UnlockLv = data.m_field_16;
		m_skill2UnlockLv = data.m_field_17;
		m_skill3UnlockLv = data.m_field_18;
		m_skill4UnlockLv = data.m_field_19;
		m_skill5UnlockLv = data.m_field_20;
		m_skill6UnlockLv = data.m_field_21;
		m_skill7UnlockLv = data.m_field_22;
		m_skill8UnlockLv = data.m_field_23;
		m_outPutType = (eOutPutType)data.m_field_24;
		m_battleType = (eBattleType)data.m_field_25;

#ifdef _HDZ_DEBUG
		Athena::logger->trace("rolebase 数据 id=%d,name=%s,m_skill1UnlockLv=%d,m_skill2UnlockLv=%d,m_skill3UnlockLv=%d,m_skill4UnlockLv=%d,",m_id,m_name,m_skill1UnlockLv,m_skill2UnlockLv,m_skill3UnlockLv,m_skill4UnlockLv);
#endif
	}
};

#define CHARID_COEF 300

struct stPlayGrownRawData
{
	INT32 getID() const
	{
		return m_field_1 * CHARID_COEF + m_field_2;
	}
	UINT32 m_field_1;		//角色编号
	UINT32 m_field_2;		//等级
	UINT32 m_field_3;		//所需经验
	UINT32 m_field_4;		//生命
	UINT32 m_field_5;		//法力
	UINT32 m_field_6;		//伤害
	UINT32 m_field_7;		//命中率
	UINT32 m_field_8;		//闪避率
	UINT32 m_field_9;		//暴击概率	
	UINT32 m_field_10;		//暴击伤害
	UINT32 m_field_11;		//抗暴击
	UINT32 m_field_12;		//暴击免伤
	UINT32 m_field_13;		//最终伤害
	UINT32 m_field_14;		//伤害减免
	UINT32 m_field_15;		//普攻回血
	UINT32 m_field_16;		//普攻吸血
	UINT32 m_field_17;		//伤害反弹
	UINT32 m_field_18;		//火强伤害
	UINT32 m_field_19;		//冰冻伤害
	UINT32 m_field_20;		//雷电伤害
	UINT32 m_field_21;		//火强概率
	UINT32 m_field_22;		//冰冻概率
	UINT32 m_field_23;		//雷电概率
	UINT32 m_field_24;		//普攻回蓝
	UINT32 m_field_25;		//霸体
	UINT32 m_field_26;		//火强触发概率免疫
	UINT32 m_field_27;		//冰冻触发概率免疫
	UINT32 m_field_28;		//雷电触发概率免疫
}__attribute__((packed));


struct stPlayGrownBase 
{
	UINT32 m_id;			 //角色ID
	UINT16 m_level;			 //角色等级
	UINT32 m_needExp;		 //需要经验
	UINT32 m_maxHP;			 //最大血量
	UINT32 m_maxMP;			 //最大魔法值
	UINT32 m_damage;		 //伤害,攻击力
	UINT32 m_hitRate;        //命中率 %
	UINT32 m_dodgeRate;      //闪避率 %
	UINT32 m_bangRate;       //暴击概率 %
	UINT32 m_bangDamage;     //暴击伤害 %
	UINT32 m_antiBangRate;   //抗暴击概率 %
	UINT32 m_antiBangDamge;	 //暴击免伤
	UINT32 m_finalDamage;    //最终伤害
	UINT32 m_damageReduce;	 //伤害减免
	UINT32 m_recoverHP;      //生命回复
	UINT32 m_absorbHP;       //普攻吸血 %
	UINT32 m_reflectDamage;  //伤害反弹 %
	UINT32 m_fireDamage;     //火强伤害
	UINT32 m_iceDamage;      //冰墙伤害
	UINT32 m_thunderDamage;  //雷电伤害
	UINT32 m_fireRate;       //火强概率
	UINT32 m_iceRate;        //冰墙概率
	UINT32 m_thunderRate;    //雷电概率
	UINT32 m_recoverMP;      //魔法回复
	UINT32 m_baTi;			//霸体
	UINT32 m_antiFireRate;  //火强概率免疫
	UINT32 m_antiIceRate;	//冰冻概率免疫
	UINT32 m_antiThunderRate;//雷电概率免疫

	static UINT32 getFindID(UINT32 charID,UINT16 level)
	{
		return charID * CHARID_COEF + level;		
	}

	void fill(const stPlayGrownRawData & data)
	{
		m_id = data.m_field_1;
		m_level = data.m_field_2;
		m_needExp = data.m_field_3;
		m_maxHP = data.m_field_4;
		m_maxMP = data.m_field_5;
		m_damage = data.m_field_6;
		m_hitRate  = data.m_field_7;
		m_dodgeRate = data.m_field_8;
		m_bangRate = data.m_field_9;
		m_bangDamage = data.m_field_10;
		m_antiBangRate = data.m_field_11;
		m_antiBangDamge = data.m_field_12;
		m_finalDamage = data.m_field_13;
		m_damageReduce = data.m_field_14;
		m_recoverHP = data.m_field_15;
		m_absorbHP = data.m_field_16;
		m_reflectDamage = data.m_field_17;
		m_fireDamage = data.m_field_18;
		m_iceDamage = data.m_field_19;
		m_thunderDamage = data.m_field_20;
		m_fireRate = data.m_field_21;
		m_iceRate = data.m_field_22;
		m_thunderRate = data.m_field_23;
		m_recoverMP = data.m_field_24;
		m_baTi = data.m_field_25;
		m_antiFireRate = data.m_field_26;
		m_antiIceRate = data.m_field_27;
		m_antiThunderRate = data.m_field_28;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("经验表charID=%d,level=%d,exp=%d,maxHP=%d,maxMP=%d,damage=%d,m_iceRate=%d,m_fireRate=%d,m_thunderRate=%d",m_id,m_level,m_needExp,m_maxHP,m_maxMP,m_damage,m_iceRate,m_fireRate,m_thunderRate);
#endif
	}
};
#endif
