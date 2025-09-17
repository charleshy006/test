#ifndef __TBL_REFINEBASE_H__
#define __TBL_REFINEBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "BattlePropValue.h"

#define REFINE_COEF 1000

struct RefineRawData
{
	INT32 getID() const
	{
		return m_field_1 * REFINE_COEF + m_field_2;
	}
	
	UINT32  m_field_1;						//物品ID
	UINT32  m_field_2;						//精炼等级
	char	m_field_3[50];					//精炼消耗货币
	char	m_field_4[50];					//可能消耗的物品
	char	m_field_5[50];					//精炼获得的属性
	char	m_field_6[50];					//额外获得的属性
	char	m_field_7[100];					//重铸获得道具
	UINT32  m_field_8;						//进化战斗力
}__attribute__((packed));


struct stRefineBase
{
	UINT32 m_id;											//关卡ID
	UINT16 m_refineLv;										//精炼等级
	UINT32 m_addBattlePower;                                //进化战斗力
	std::vector<std::pair<UINT8,UINT32> > m_costResVec;		//花费资源
	std::vector<std::pair<UINT32,UINT32> > m_costItemVec;	//花费道具
	std::map<UINT8,stBattlePropValue > m_addProp;	 		//获得属性
	std::map<UINT8,stBattlePropValue > m_extraProp;	 		//额外属性
	std::vector<std::pair<UINT32,UINT32> > m_returnAssert;  //重铸返回的资源

	static UINT32 getFindID(UINT32 id,UINT16 lv)
	{
		return id * REFINE_COEF + lv;	
	}

	void fill(const RefineRawData & data)
	{
		m_id = data.m_field_1;
		m_refineLv = data.m_field_2;
		
		std::vector<std::string > strVec;
		g_stringtok(strVec,data.m_field_3,";");
		for (UINT8 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				m_costResVec.push_back(std::make_pair((UINT8)atoi(subStrVec[0].c_str()),(UINT32)atoi(subStrVec[1].c_str())));
			}
		}
		
		strVec.clear();
		g_stringtok(strVec,data.m_field_4,";");
		for (UINT8 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				m_costItemVec.push_back(std::make_pair((UINT32)atoi(subStrVec[0].c_str()),(UINT32)atoi(subStrVec[1].c_str())));
			}
		}
		
		strVec.clear();
		g_stringtok(strVec,data.m_field_5,";");
		for (UINT8 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");

			stBattlePropValue propVal;
			UINT8 propID = 0;
			if (subStrVec.size() >= 1){ 
				propVal.m_propID = propID = (UINT32)atoi(subStrVec[0].c_str());
			}   

			if (subStrVec.size() >= 2){ 
				propVal.m_val1 = (UINT32)atoi(subStrVec[1].c_str());
			}   

			if (subStrVec.size() >= 3){ 
				propVal.m_val2 = (UINT32)atoi(subStrVec[2].c_str());
			}   

			if (subStrVec.size() >= 4){ 
				propVal.m_val3 = (UINT32)atoi(subStrVec[3].c_str());
			}   

			if (subStrVec.size() >= 5){ 
				propVal.m_val4 = (UINT32)atoi(subStrVec[4].c_str());
			}   
			m_addProp[propID] = propVal;
		}

		strVec.clear();
		g_stringtok(strVec,data.m_field_6,";");
		for (UINT8 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			
			stBattlePropValue propVal;
			UINT8 propID = 0;
			if (subStrVec.size() >= 1){ 
				propVal.m_propID = propID = (UINT32)atoi(subStrVec[0].c_str());
			}   

			if (subStrVec.size() >= 2){ 
				propVal.m_val1 = (UINT32)atoi(subStrVec[1].c_str());
			}   

			if (subStrVec.size() >= 3){ 
				propVal.m_val2 = (UINT32)atoi(subStrVec[2].c_str());
			}   

			if (subStrVec.size() >= 4){ 
				propVal.m_val3 = (UINT32)atoi(subStrVec[3].c_str());
			}   

			if (subStrVec.size() >= 5){ 
				propVal.m_val4 = (UINT32)atoi(subStrVec[4].c_str());
			}   
			m_extraProp[propID] = propVal;
		}

		strVec.clear();
		g_stringtok(strVec,data.m_field_7,";");
		for (UINT8 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
				UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
				m_returnAssert.push_back(std::make_pair(itemID,num));	
			}
		}
        m_addBattlePower = data.m_field_8;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("精炼配置id=%d,lv=%d",m_id,m_refineLv);
#endif
	}
};

#endif
