#ifndef __H_HERO_DB_DATA_H__
#define __H_HERO_DB_DATA_H__
#include "TypesDef.h"
#pragma pack(1)
#define MAX_SKILL_NUM  4
#define MAX_HERO_RUNE_NUM  6

struct stHeroSkillInfo
{
	stHeroSkillInfo()
	{
		m_skillID = 0;
		m_skillLv = 0;
	}

	UINT32 m_skillID;			//技能ID
	UINT16 m_skillLv;			//技能等级
};

struct stHeroDBData
{
	UINT32 m_id;				//武将ID
	UINT16 m_level;				//等级
	UINT8  m_state;				//状态
	UINT8  m_pos;				//出战的位置
	UINT16 m_evolutionLv;		//进化等级
	UINT32 m_studySkillID;		//领悟技能的ID
	UINT16 m_trainLv;			//调教等级
	UINT32 m_trainExp;			//调教经验
	stHeroSkillInfo m_skillVec[MAX_SKILL_NUM];	//主动技能

	stHeroDBData()
	{
		m_id = 0;
		m_level = 0;
		m_state = 0;
		m_pos = 0;
		m_evolutionLv = 0;
		m_studySkillID = 0;
		m_trainLv = 0;
		m_trainExp = 0;
	}
};

struct stHeroRuneDBData
{
    UINT16 m_runeVec[MAX_HERO_RUNE_NUM]; //装备符文数量,存储在随从符文背包的slot Index
    stHeroRuneDBData () {
        for (size_t i = 0; i < MAX_HERO_RUNE_NUM; i++) {
            m_runeVec[i] = (UINT16)-1;
        }
    }
};

struct stHeroSaveInfo
{
	UINT32 m_version;		//版本
	UINT32 m_skillExp;		//技能经验
	UINT16 m_num;			//个数
	stHeroDBData m_data[0];

	stHeroSaveInfo()
	{
		m_version = 0;
		m_skillExp = 0;
		m_num = 0;
	}
};
#pragma pack()
#endif
