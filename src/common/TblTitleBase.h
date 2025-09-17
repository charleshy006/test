#ifndef __TBL_TITLEBASE_H__
#define __TBL_TITLEBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "CommonMicro.h"

struct stTitleRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}

	UINT32  m_field_1;						//称号ID
	char    m_field_2[33];					//称号名字
	char    m_field_3[100];					//显示场景
	char    m_field_4[100];					//称号效果
	char	m_field_5[100];					//获取条件
}__attribute__((packed));

//称号增加属性
struct stTitleAddProp
{
	stTitleAddProp()
	{
		m_propID = 0;
		m_propVal = 0;
	}

	UINT16 m_propID;						//属性ID
	UINT32 m_propVal;						//属性值
};

//获得关卡条件
struct stGetTitleCond
{
	stGetTitleCond()
	{
		m_cond = 0;
		m_value = 0;
	}

	UINT16 m_cond;				//条件
	UINT32 m_value;				//值
};

enum eTitleGotType
{
	eTitleGotType_Finish_Lv = 1,			//完成关卡
	eTitleGotType_LevelRank = 2,			//等级排行
	eTitleGotType_BattlePowerRank =3,		//战力排行
	eTitleGotType_One2OnePVPRank = 4,		//巅峰排行
	eTitleGotType_CopymapRank	= 5,		//副本排行
	eTitleGotType_LeitaiRank	= 6,		//擂台排行
	eTitleGotType_VipLevelup = 9,           //vip升级
	eTitleGotType_FirstKill		= 10,		//首杀
	eTitleGotType_ArenaRank		= 11,		//竞技场排名
};

struct stTitleBase
{
	UINT32 m_titleID;						//称号ID
	char   m_name[MAX_NAMESIZE + 1];		//名字
	std::set<UINT32> m_showScenes;			//显示场景
	std::vector<stTitleAddProp> m_addProps;	//增加的属性
	std::vector<stGetTitleCond> m_getConds;	//获得条件

	void fill(const stTitleRawData & data)
	{
		m_titleID = data.m_field_1;
		strncpy(m_name,data.m_field_2,sizeof(m_name));
		
		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_3,"-");
		for (UINT16 i = 0;i < strVec.size(); ++i){
			m_showScenes.insert((UINT32)atoi(strVec[i].c_str()));	
		}

		strVec.clear();
		g_stringtok(strVec,data.m_field_4,";");
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				stTitleAddProp prop;
				prop.m_propID = (UINT16)atoi(subStrVec[0].c_str());
				prop.m_propVal = (UINT32)atoi(subStrVec[1].c_str());
				m_addProps.push_back(prop);
			}
		}
		
		strVec.clear();
		g_stringtok(strVec,data.m_field_5,";");
		for (UINT16 i = 0;i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				stGetTitleCond cond;
				cond.m_cond = (UINT16)atoi(subStrVec[0].c_str());
				cond.m_value = (UINT32)atoi(subStrVec[1].c_str());
				m_getConds.push_back(cond);
			}
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("称号表 id = %d,name=%s,scenesNum=%d,addProps=%d,getCondNum=%d",m_titleID,m_name,m_showScenes.size(),m_addProps.size(),m_getConds.size());
#endif
	}
};
#endif
