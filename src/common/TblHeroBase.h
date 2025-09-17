#ifndef __TBL_HEROBASE_H__
#define __TBL_HEROBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include <set>


#define HERO_LV_PROP_COEF 1000

struct stHeroLvPropTblRawData
{
	INT32 getID() const
	{
		return m_field_1 * HERO_LV_PROP_COEF  + m_field_2;
	}
	
	UINT32  m_field_1;						//武将ID
	UINT32  m_field_2;						//武将等级
	UINT32  m_field_3;						//成长类型,金币或者突破
	UINT32  m_field_4;						//消耗突破丹
	UINT32  m_field_5;						//消耗突破丹数量
	UINT32  m_field_6;						//消耗金币数量
	UINT32  m_field_7;						//战力加成
	UINT32 	m_field_8;						//生命
	UINT32 	m_field_9;						//法力
	UINT32 	m_field_10;						//伤害
	UINT32 	m_field_11;						//命中率
	UINT32 	m_field_12;						//闪避率
	UINT32 	m_field_13;						//暴击概率	
	UINT32 	m_field_14;						//暴击伤害
	UINT32 	m_field_15;						//抗暴击
	UINT32 	m_field_16;						//暴击免伤
	UINT32 	m_field_17;						//最终伤害
	UINT32 	m_field_18;						//伤害减免
	UINT32 	m_field_19;						//普攻回血
	UINT32 	m_field_20;						//普攻吸血
	UINT32 	m_field_21;						//伤害反弹
	UINT32 	m_field_22;						//火强伤害
	UINT32 	m_field_23;						//冰冻伤害
	UINT32 	m_field_24;						//雷电伤害
	UINT32 	m_field_25;						//火强概率
	UINT32 	m_field_26;						//冰冻概率
	UINT32 	m_field_27;						//雷电概率
	UINT32  m_field_28;						//火强免伤
	UINT32  m_field_29;						//冰冻免伤
	UINT32  m_field_30;						//雷电免伤
	UINT32  m_field_31;						//等级限制
	UINT32  m_field_32;						//火强免疫
	UINT32  m_field_33;						//冰冻免疫
	UINT32  m_field_34;						//雷电免疫
	char    m_field_35[500];                //重铸返还资源
	char    m_field_36[50];                 //高级突破所需灵魂石
}__attribute__((packed));

enum
{
	HERO_GROWN_TYPE_UPGRADE = 1,	//升级
	HERO_GROWN_TYPE_BREAK  = 2,		//突破
	HERO_GROWN_TYPE_HIGH_BREAK = 3, //高级突破
};

struct stHeroLvPropBase 
{

	static UINT32 genFindID(UINT32 heroID,UINT16 lv)
	{
		return heroID  * HERO_LV_PROP_COEF  + lv;
	}

	UINT32 m_heroID;								//武将ID
	UINT16 m_level;									//武将等级
	UINT8  m_lvType;								//成长类型
	UINT32 m_levelItemID;							//突破丹ID
	UINT32 m_levelItemNum;							//突破丹数量
	UINT32 m_costGoldCoin;							//消耗金币
	UINT32 m_battlePower;							//战力
	UINT32 m_maxHP;			 						//最大血量
	UINT32 m_maxMP;			 						//最大魔法值
	UINT32 m_damage;		 						//伤害,攻击力
	UINT32 m_hitRate;        						//命中率 %
	UINT32 m_dodgeRate;      						//闪避率 %
	UINT32 m_bangRate;       						//暴击概率 %
	UINT32 m_bangDamage;     						//暴击伤害 %
	UINT32 m_antiBangRate;   						//抗暴击概率 %
	UINT32 m_antiBangDamge;	 						//暴击免伤
	UINT32 m_finalDamage;    						//最终伤害
	UINT32 m_damageReduce;	 						//伤害减免
	UINT32 m_recoverHP;      						//生命回复
	UINT32 m_absorbHP;       						//普攻吸血 %
	UINT32 m_reflectDamage;  						//伤害反弹 %
	UINT32 m_fireDamage;     						//火强伤害
	UINT32 m_iceDamage;      						//冰墙伤害
	UINT32 m_thunderDamage;  						//雷电伤害
	UINT32 m_fireRate;       						//火强概率
	UINT32 m_iceRate;        						//冰墙概率
	UINT32 m_thunderRate;    						//雷电概率
	UINT32 m_antiFireDamage;						//火强反伤
	UINT32 m_antiIceDamage;							//冰冻反伤
	UINT32 m_antiThunderDamage;						//雷电反伤

	UINT32 m_roleLvLimit;							//玩家等级限制
	UINT32 m_antiFireRate;  						//火强概率免疫
	UINT32 m_antiIceRate;							//冰冻概率免疫
	UINT32 m_antiThunderRate;						//雷电概率免疫
	
	std::vector<std::pair<UINT32, UINT32> > m_returnAssert;	//重铸返还资源
	std::vector<std::pair<UINT32, UINT32> > m_costStoneVec;	//高级突破所需灵魂石

	void fill(const stHeroLvPropTblRawData & data)
	{
		m_heroID = data.m_field_1;
		m_level  = data.m_field_2;
		m_lvType = data.m_field_3;
		m_levelItemID = data.m_field_4;
		m_levelItemNum = data.m_field_5;
		m_costGoldCoin = data.m_field_6;
		m_battlePower = data.m_field_7;
		m_maxHP = data.m_field_8;
		m_maxMP = data.m_field_9;
		m_damage = data.m_field_10;
		m_hitRate = data.m_field_11;
		m_dodgeRate = data.m_field_12;
		m_bangRate = data.m_field_13;
		m_bangDamage = data.m_field_14;
		m_antiBangRate = data.m_field_15;
		m_antiBangDamge = data.m_field_16;
		m_finalDamage = data.m_field_17;
		m_damageReduce = data.m_field_18;
		m_recoverHP = data.m_field_19;
		m_absorbHP = data.m_field_20;
		m_reflectDamage = data.m_field_21;
		m_fireDamage = data.m_field_22;
		m_iceDamage = data.m_field_23;
		m_thunderDamage = data.m_field_24;
		m_fireRate = data.m_field_25;
		m_iceRate = data.m_field_26;
		m_thunderRate = data.m_field_27;
		m_antiFireDamage = data.m_field_28;
		m_antiIceDamage =  data.m_field_29;
		m_antiThunderDamage = data.m_field_30;
		m_roleLvLimit = data.m_field_31;
		m_antiFireRate = data.m_field_32;
		m_antiIceRate = data.m_field_33;
		m_antiThunderRate = data.m_field_34;


		std::vector<std::string > strVec;
		g_stringtok(strVec,data.m_field_35,";");
		for (UINT8 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				m_returnAssert.push_back(std::make_pair((UINT32)atoi(subStrVec[0].c_str()),(UINT32)atoi(subStrVec[1].c_str())));
			}
		}

		strVec.clear();
		g_stringtok(strVec,data.m_field_36,";");
		for (UINT8 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				m_costStoneVec.push_back(std::make_pair((UINT32)atoi(subStrVec[0].c_str()),(UINT32)atoi(subStrVec[1].c_str())));
			}
		}

#ifdef _HDZ_DEBUG
		Athena::logger->trace("加载了武将等级属性表 id=%d,level=%d,m_hp=%d,m_damage=%d,antiBangRate:%d",m_heroID,m_level,m_maxHP,m_damage,m_antiBangRate);
#endif
	}
};

#define HERO_EVOLUTION_COEF 100

//进化表
struct stHeroEvolutionRawData
{
	INT32 getID() const
	{
		return m_field_1 * HERO_EVOLUTION_COEF + m_field_2;
	}

	UINT32 m_field_1;					    //武将ID
	UINT32 m_field_2;						//武将等级
	UINT32 m_field_3;						//武魂石
	UINT32 m_field_4;						//武魂石数量
	char   m_field_5[200];					//附加属性
	UINT32 m_field_6;						//战斗力
	UINT32 m_field_7;						//重铸返还武魂
}__attribute__((packed));

enum {
	eEvolution_Prop_Target_Owner = 1,	//主人
	eEvolution_Prop_Target_Hero = 2,	//宠物	
};

struct stEvolutionAddProp
{
	UINT8 	m_propID;			//属性ID
	UINT32 	m_propTarget;		//属性目标
	UINT32 	m_value;			//值

	stEvolutionAddProp()
	{
		m_propID = 0;
		m_propTarget = 0;
		m_value = 0;
	}
};

struct stHeroEvolutionBase 
{
	UINT32 m_id;							//武将ID
	UINT8  m_evolutionLv;					//进化等级
	UINT32 m_stoneID;						//武魂石ID
	UINT32 m_stoneNum;						//石头数量
	std::vector<stEvolutionAddProp> m_propVec;//增加的属性	
	UINT32 m_battlePower;					//战斗力
	UINT32 m_retStoneNum;					//重铸返还武魂
	
	static UINT32 getFindID(UINT32 id,UINT16 lv)
	{
		return id * HERO_EVOLUTION_COEF + lv;
	}

	void fill(const stHeroEvolutionRawData & data)
	{
		m_id = data.m_field_1;			//武将ID
		m_evolutionLv = data.m_field_2;
		m_stoneID = data.m_field_3;
		m_stoneNum = data.m_field_4;
		m_retStoneNum = data.m_field_7;

		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_5,";");
		
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");

			if (subStrVec.size() >= 3){
				stEvolutionAddProp one;
				one.m_propID = (UINT8)atoi(subStrVec[1].c_str());
				one.m_propTarget = (UINT32)atoi(subStrVec[0].c_str());
				one.m_value = (UINT32)atoi(subStrVec[2].c_str());
				m_propVec.push_back(one);	
			}
		}

		m_battlePower = data.m_field_6;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("武将进化表id=%d,m_evolutionLv=%d,m_stoneID=%d,m_stoneNum=%d",m_id,m_evolutionLv,m_stoneID,m_stoneNum);
#endif
	}
};

//技能领域表
struct stSkillundRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}
	UINT32 m_field_1;			//ID
	char   m_field_4[500];		//随机技能
}__attribute__((packed));


struct stSkillundBase
{
	UINT32 m_id;													//武将ID
	std::vector<std::pair<UINT32,UINT32> > m_randomSkillVec;			//随机技能

	void fill(const stSkillundRawData & data)
	{
		m_id = data.m_field_1;
		
		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_4,";");
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				UINT32 skillID = (UINT32)atoi(subStrVec[0].c_str());
				UINT32 rate  = (UINT32)atoi(subStrVec[1].c_str());
				m_randomSkillVec.push_back(std::make_pair(skillID,rate));
			}
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("武将技能领悟:id=%d",m_id);
#endif
	}
};

#endif
