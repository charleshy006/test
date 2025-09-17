#ifndef __TBL_ROBOTBASE_H__
#define __TBL_ROBOTBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

#define ROBOT_COEF 100

struct RobotRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}
	
	UINT32  m_field_1;						//机器人ID
	UINT32  m_field_2;						//机器人等级
	UINT32  m_field_3;						//机器人职业
	UINT32  m_field_4;						//机器人性别
	char	m_field_5[50];					//头部装备-LV
	char    m_field_6[50];					//上身装备-LV
	char	m_field_7[50];					//武器装备-LV
	char	m_field_8[50];					//下身装备-LV
	char	m_field_9[50];					//项链装备-LV
	char	m_field_10[50];					//披风装备-LV
	char	m_field_11[50];					//戒指装备-LV
	char	m_field_12[50];					//翅膀装备-LV
	char	m_field_13[50];					//携带宠物1-LV
	char	m_field_14[33];					//机器人名字
	char    m_field_15[50];					//携带宠物2-LV
	UINT32  m_field_16;						//所在关卡
	UINT32  m_field_17;						//机器人序列
	UINT32  m_field_18;						//宠物1进化
	UINT32  m_field_19;						//宠物2进化
	char    m_field_20[75];					//头部宝石
	char    m_field_21[75];					//衣服宝石
	char	m_field_22[75];					//武器宝石
	char	m_field_23[75];					//裤子宝石
	char	m_field_24[75];					//项链宝石
	char	m_field_25[75];					//披风宝石
	char	m_field_26[75];					//戒子宝石
}__attribute__((packed));

#define INIT_ROBOT_EQUIP(field) do { \
		strVec.clear(); \
		g_stringtok(strVec,data.field,"-"); \
		if (strVec.size() >= 2){\
			UINT32 itemID = (UINT32)atoi(strVec[0].c_str());\
			UINT16 lv  = (UINT16)atoi(strVec[1].c_str());\
			m_equips.push_back(std::make_pair(itemID,lv));\
		}\
	}while (false)

#define INIT_ROBOT_PET(field) do { \
		strVec.clear(); \
		g_stringtok(strVec,data.field,"-"); \
		if (strVec.size() >= 2){\
			UINT32 petID = (UINT32)atoi(strVec[0].c_str());\
			UINT16 lv  = (UINT16)atoi(strVec[1].c_str());\
			m_pets.push_back(std::make_pair(petID,lv));\
		}\
	}while (false)

#define INIT_ROBOT_FUWEN(field,fuWenVec) do {\
	strVec.clear();\
	g_stringtok(strVec,field,";");\
	std::vector<std::string> subStrVec;	\
	for (UINT16 i = 0;i < strVec.size();++i){\
		subStrVec.clear();\
		g_stringtok(subStrVec,strVec[i],"-");\
		if (subStrVec.size() >= 2){\
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());\
			UINT16 lv  = (UINT16)atoi(subStrVec[1].c_str());\
			fuWenVec.push_back(std::make_pair(itemID,lv));\
		}\
	}\
}while(false);

struct stRobotBase
{
	UINT32 m_id;											//机器人ID
	UINT16 m_lv;											//机器人等级
	UINT8  m_job;											//机器人职业
	UINT8  m_sex;											//机器人性别
	
	std::vector<std::pair<UINT32,UINT16> > m_equips; 		//装备信息
	std::vector<std::pair<UINT32,UINT16> > m_pets;			//宠物
	char  m_robotName[33];									//机器人名字
	UINT32 m_levelID;										//关卡ID
	UINT8  m_groupID;										//组ID
	UINT16 m_pet1EvelotionLv;								//宠物1的进化等级
	UINT16 m_pet2EvelotionLv;								//宠物2的进化等级
	std::vector<std::pair<UINT32,UINT16> > m_headFuWens;		//头部符文
	std::vector<std::pair<UINT32,UINT16> > m_bodyFuWens;		//衣服符文
	std::vector<std::pair<UINT32,UINT16> > m_weaponFuWens;	//武器符文
	std::vector<std::pair<UINT32,UINT16> > m_shoeFuWens;		//裤子符文
	std::vector<std::pair<UINT32,UINT16> > m_necklaceFuWens;	//项链符文
	std::vector<std::pair<UINT32,UINT16> > m_cloakFuWens;  	//披风符文
	std::vector<std::pair<UINT32,UINT16> > m_ringFuWens;	  	//项链符文

	void fill(const RobotRawData & data)
	{
		m_id = data.m_field_1;
		m_lv = data.m_field_2;
		m_job = data.m_field_3;
		m_sex = data.m_field_4;

		std::vector<std::string> strVec;

		INIT_ROBOT_EQUIP(m_field_5);	
		INIT_ROBOT_EQUIP(m_field_6);	
		INIT_ROBOT_EQUIP(m_field_7);	
		INIT_ROBOT_EQUIP(m_field_8);	
		INIT_ROBOT_EQUIP(m_field_9);	
		INIT_ROBOT_EQUIP(m_field_10);	
		INIT_ROBOT_EQUIP(m_field_11);	
		INIT_ROBOT_EQUIP(m_field_12);

		INIT_ROBOT_PET(m_field_13);
		strncpy(m_robotName,data.m_field_14,sizeof(m_robotName));
		INIT_ROBOT_PET(m_field_15);
		m_levelID = data.m_field_16;
		m_groupID = data.m_field_17;
		m_pet1EvelotionLv = data.m_field_18;
		m_pet2EvelotionLv = data.m_field_19;
	
		INIT_ROBOT_FUWEN(data.m_field_20,m_headFuWens);
		INIT_ROBOT_FUWEN(data.m_field_21,m_bodyFuWens);
		INIT_ROBOT_FUWEN(data.m_field_22,m_weaponFuWens);
		INIT_ROBOT_FUWEN(data.m_field_23,m_shoeFuWens);
		INIT_ROBOT_FUWEN(data.m_field_24,m_necklaceFuWens);
		INIT_ROBOT_FUWEN(data.m_field_25,m_cloakFuWens);
		INIT_ROBOT_FUWEN(data.m_field_26,m_ringFuWens);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("机器人配置id=%d,lv=%d,携带的宠物有:%d,levelID:%d,groupID:%d",m_id,m_lv,m_pets.size(),m_levelID,m_groupID);
#endif
	}
};

#endif
