#ifndef __TBL_MOUNT_BASE_H__
#define __TBL_MOUNT_BASE_H__

#include <TypesDef.h>
#include <string>
#include "CommonMicro.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "BattlePropValue.h"

struct stMountgrowRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}

	UINT32 m_field_1;           //等级
	UINT32 m_field_2;			//显示阶级
	UINT32 m_field_3;			//显示星级
	UINT32 m_field_4;			//升级所需经验
	char   m_field_5[200];      //获得属性
	char   m_field_6[200];      //升级消耗材料
	char   m_field_7[200];		//升阶消耗材料
}__attribute__((packed));

struct stMountgrowBase
{
	UINT32 m_level;											//等级
	UINT32 m_class;											//显示阶级
	UINT32 m_star;											//显示星级
	UINT32 m_needExp;										//升级所需经验
	std::map<UINT8, stBattlePropValue>	m_addProp;			//获得属性
	std::vector<std::pair<UINT8, UINT32> > m_costResVec;    //升级花费资源
	std::map<UINT32, UINT32> m_costItemMap;  				//升级花费道具
	std::vector<std::pair<UINT8, UINT32> > m_orderResVec;	//升阶花费资源
	std::vector<std::pair<UINT32, UINT32> > m_orderItemVec;	//升阶话费道具

	void fill(const stMountgrowRawData & data)
	{
	 	m_level		=	data.m_field_1;
		m_class		=	data.m_field_2;
		m_star		=	data.m_field_3;
		m_needExp	=	data.m_field_4;

		std::vector<std::string> strVec;
		g_stringtok(strVec, data.m_field_5, ";");
		for(UINT8 i = 0; i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(), "-");
			
			stBattlePropValue propVal;
			UINT8 propID = 0;
			
			if(subStrVec.size() >= 1){
				propVal.m_propID = propID = (UINT32)atoi(subStrVec[0].c_str());
			}

			if(subStrVec.size() >= 2)
			{
				propVal.m_val1 = (UINT32)atoi(subStrVec[1].c_str());
			}

			if(subStrVec.size() >= 3)
			{
				propVal.m_val2 = (UINT32)atoi(subStrVec[2].c_str());
			}

			if(subStrVec.size() >= 4)
			{
				propVal.m_val3 = (UINT32)atoi(subStrVec[3].c_str());
			}

			if(subStrVec.size() >= 5)
			{
			    propVal.m_val4 = (UINT32)atoi(subStrVec[4].c_str());
			}

			m_addProp[propID] = propVal;
		}

		strVec.clear();
		g_stringtok(strVec, data.m_field_6, ";");
		for(UINT8 i = 0; i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec, strVec[i], "-");

			if(subStrVec.size() >= 2){
				UINT32 assertID = (UINT32)atoi(subStrVec[0].c_str());
				UINT32 num = (UINT32)atoi(subStrVec[1].c_str());

				if(assertID < eResource_ID_Max){
					m_costResVec.push_back(std::make_pair(assertID,num));
				}else{
					m_costItemMap.insert(std::make_pair(assertID,num));
				}
			}
		}

		strVec.clear();
		g_stringtok(strVec, data.m_field_7, ";");
		for(UINT8 i = 0; i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec, strVec[i], "-");

			if(subStrVec.size() >= 2){
				UINT32 assertID = (UINT32)atoi(subStrVec[0].c_str());
				UINT32 num = (UINT32)atoi(subStrVec[1].c_str());

				if(assertID < eResource_ID_Max){
					m_orderResVec.push_back(std::make_pair(assertID,num));
				}else{
					m_orderItemVec.push_back(std::make_pair(assertID,num));
				}
			}
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("坐骑成长配置 level=%d, class=%d, star=%d, needExp:%d, m_addPropSize:%d, m_costResVec.size:%d, m_costItemMap.size:%d, m_orderResVec.size:%d, m_orderItemVec.size:%d",m_level, m_class, m_star, m_needExp, m_addProp.size(), m_costResVec.size(), m_costItemMap.size(), m_orderResVec.size(), m_orderItemVec.size());
#endif
	}
};


struct stMountIllusionRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}

	UINT32 m_field_1;           //角色ID
	UINT32 m_field_2;			//幻化后角色ID
	char   m_field_3[50];		//幻化解锁条件
	char   m_field_4[200];		//进阶或解锁获得属性
	char   m_field_5[200];      //进阶或解锁消耗材料
}__attribute__((packed));


struct stUnloackCondition
{
	UINT8	m_type;				//条件类型 1-代表解锁阶级 2-代表解锁所需使用物品
	UINT32	m_cond;			//条件
	UINT32	m_num;				//数量

	stUnloackCondition()
	{
		m_type	=	0;
		m_cond	=	0;
		m_num	=	0;
	}
};


struct stMountIllusionBase
{
	UINT32 m_id;											//角色ID
	UINT32 m_unrealID;										//幻化后角色ID
	std::vector<stUnloackCondition> m_unlock;				//解锁条件
	std::map<UINT8, stBattlePropValue>  m_addProp;  		//进阶或解锁获得属性
	std::vector<std::pair<UINT8, UINT32> > m_costResVec;	//花费资源
	std::vector<std::pair<UINT32, UINT32> > m_costItemVec;  //花费道具

	void fill(const stMountIllusionRawData & data)
	{
		m_id		=	data.m_field_1;
		m_unrealID	=	data.m_field_2;

		std::vector<std::string> strVec;
		g_stringtok(strVec, data.m_field_3, ";");
		for(UINT8 i = 0; i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec, strVec[i], "-");
			if(subStrVec.size() >= 3){
				stUnloackCondition cound;
				cound.m_type = (UINT8)atoi(subStrVec[0].c_str());
				cound.m_cond = (UINT32)atoi(subStrVec[1].c_str());
				cound.m_num = (UINT32)atoi(subStrVec[2].c_str());
				m_unlock.push_back(cound);
			}
		}

		strVec.clear();
		g_stringtok(strVec, data.m_field_4, ";");
		for(UINT8 i = 0; i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec, strVec[i], "-");

			 stBattlePropValue propVal;
			 UINT8 propID = 0;

			 if(subStrVec.size() >= 1)
			 {
				 propVal.m_propID = propID = (UINT32)atoi(subStrVec[0].c_str());
			 }

			 if(subStrVec.size() >= 2)
			 {
				 propVal.m_val1 = (UINT32)atoi(subStrVec[1].c_str());
			 }

			 if(subStrVec.size() >= 3)
			 {
				 propVal.m_val2 = (UINT32)atoi(subStrVec[2].c_str());
			 }

			 if(subStrVec.size() >= 4)
			 {
				 propVal.m_val3 = (UINT32)atoi(subStrVec[3].c_str());
			 }

			 if(subStrVec.size() >= 5)
			 {
				 propVal.m_val4 = (UINT32)atoi(subStrVec[4].c_str());
			 }

			 m_addProp[propID] = propVal;
		}

		strVec.clear();
		g_stringtok(strVec, data.m_field_5, ";");
		for(UINT8 i = 0; i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec, strVec[i], "-");
			
			if(subStrVec.size() >= 2){
				UINT32 assertID = (UINT32)atoi(subStrVec[0].c_str());
				UINT32 num = (UINT32)atoi(subStrVec[1].c_str());

				if(assertID < eResource_ID_Max){
					m_costResVec.push_back(std::make_pair(assertID,num));
				}else{
					m_costItemVec.push_back(std::make_pair(assertID,num));
				}
			}
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("坐骑进阶配置id=%d,m_unrealID=%d, m_addProp:%d ,costResNum:%d,m_costItemVec:%d",m_id, m_unrealID, m_addProp.size(), m_costResVec.size(), m_costItemVec.size());
#endif
	}
};
#endif
