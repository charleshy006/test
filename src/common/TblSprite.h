#ifndef __TBL_SPRITE_H__
#define __TBL_SPRITE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

#define SPRITE_COEF  1000
#define ILLUSIONISM_COEF  500

struct SpriteRawData
{
	INT32 getID() const
	{
		return m_field_1 * SPRITE_COEF + m_field_2;
	}
	
	UINT32  m_field_1;						//精灵ID
	UINT32  m_field_2;						//培养等级
	UINT32  m_field_3;						//是否可以进阶
	char	m_field_4[50];					//培养消耗
	char	m_field_5[150];					//总属性加成
	char	m_field_6[50];					//解锁条件
	UINT32  m_field_7;						//玩家等级
}__attribute__((packed));

enum 
{
	SPRITE_UNLOCKED_ROLE_LV  = 1,
	SPRITE_UNLOCKED_VIP_LV = 2,
};

struct stSpriteBase
{
	UINT32 m_id;												//精灵ID
	UINT16 m_level;												//精灵等级
	bool   m_bAdvance;											//是否可以进阶
	std::vector<std::pair<UINT32,UINT32> > m_consumesVec;		//消耗
	std::vector<std::pair<UINT8,UINT32> > m_addPropsVec;		//增加的属性
	std::vector<std::pair<UINT8,UINT32> > m_unlockedsVec;		//解锁条件
	UINT16 m_needLv;											//需要玩家等级
	
	static UINT32 getFindID(UINT32 id,UINT16 level)
	{
		return SPRITE_COEF * id + level;
	}

	void fill(const SpriteRawData & data)
	{
		m_id = data.m_field_1;
		m_level = data.m_field_2;
		if (data.m_field_3){
			m_bAdvance = true;
		}
		else {
			m_bAdvance = false;
		}
		
		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_4,";");
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i],"-");
			if (subStrVec.size() >= 2){
				UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());	
				UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
				m_consumesVec.push_back(std::make_pair(itemID,num));		
			}
		}

		strVec.clear();
		g_stringtok(strVec,data.m_field_5,";");
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i],"-");
			if (subStrVec.size() >= 2){
				UINT8 type = (UINT8)atoi(subStrVec[0].c_str());	
				UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
				m_addPropsVec.push_back(std::make_pair(type,num));
			}
		}
		
		strVec.clear();
		g_stringtok(strVec,data.m_field_6,";");
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i],"-");
			if (subStrVec.size() >= 2){
				UINT8 type = (UINT8)atoi(subStrVec[0].c_str());
				UINT32 num = (UINT8)atoi(subStrVec[1].c_str());
				m_unlockedsVec.push_back(std::make_pair(type,num));
			}
		}
		m_needLv = data.m_field_7;

#ifdef _HDZ_DEBUG
		Athena::logger->trace("加载了精灵配置,id:%d,level:%d,needLv:%d",m_id,m_level,m_needLv);
#endif
	}
};

struct IllusionismRawData
{
	INT32 getID() const
	{
		return m_field_1 * ILLUSIONISM_COEF + m_field_2;//给每行记录生成一个唯一id
	}
	
	UINT32  m_field_1;						//精灵ID
	UINT32  m_field_2;						//等级
	UINT32  m_field_3;						//升级经验
	UINT32  m_field_4;						//获得天赋点
	char	m_field_5[100];					//获取双倍经验的道具id
	UINT32  m_field_6;						//当前等级到满级所需经验
}__attribute__((packed));

struct stIllusionismBase
{
    UINT32 m_id;                                //精灵id
    UINT32 m_level;                             //精灵等级
    UINT32 m_exp;                               //精灵到下一级所需经验
    UINT32 m_talentNum;                         //精灵天赋点
    std::vector<UINT32> m_doubleExpItemsVec;    //获得双倍经验道具
    UINT32 m_toMaxLvlExp;                       //当前等级到满级所需经验d


	static UINT32 getFindID(UINT32 id,UINT16 level)
	{
		return ILLUSIONISM_COEF * id + level;
	}
	void fill(const IllusionismRawData & data)
	{
		m_id = data.m_field_1;
		m_level = data.m_field_2;
		m_exp = data.m_field_3;
		m_talentNum = data.m_field_4;

		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_5,";");
		for (size_t i = 0;i < strVec.size();++i){
            m_doubleExpItemsVec.push_back((UINT32)atoi(strVec[i].c_str()));
		}
		m_toMaxLvlExp = data.m_field_6;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("加载了强化精灵配置,id:%d,level:%d,exp:%d,talentNum:%d,toMaxLvlExp is %d",
                m_id, m_level, m_exp, m_talentNum, m_toMaxLvlExp);
#endif
    }
};
#endif
