#ifndef __TBL_ITEMBASE_H__
#define __TBL_ITEMBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "CommonMicro.h"
#include "Role.pb.h"
#include "BattlePropValue.h"
#include <set>

struct ItemTblRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}

	UINT32  m_field_1;						//道具ID
	char    m_field_2[33];					//道具名字
	UINT32  m_field_3;						//道具类型
	UINT32  m_field_4;						//品质
	UINT32  m_field_5;						//品阶
	UINT32  m_field_6;						//配置部位
	UINT32  m_field_7;						//效果
	UINT32  m_field_8;						//是否需要媒介
	UINT32  m_field_9;						//媒介物品ID
	UINT32  m_field_10;						//是否可以出售
	UINT32  m_field_11;						//出售价格
	UINT32  m_field_12;						//叠加类型
	UINT32  m_field_13;						//成长编号
	UINT32  m_field_14;						//boxID
	UINT32  m_field_15;						//熔炼值
	char	m_field_16[50];					//装备类物品附加技能
	UINT32  m_field_17;						//排序ID
	UINT32  m_field_18;						//是否需要广播
	UINT32  m_field_19;						//时效
}__attribute__((packed));

struct stItemBase
{
	UINT32 	m_itemID;						//道具ID
	char   	m_name[MAX_NAMESIZE + 1];       //名字
	UINT16 	m_type;							//道具类型
	UINT16  m_quality;						//品质
	UINT16  m_classType;					//品阶
	UINT16  m_equipPart;					//装备部位
	UINT32  m_effect;						//效果
	bool	m_needMedia;					//是否需要媒介
	UINT32  m_mediaID;						//媒介物品ID
	bool 	m_canSell;						//是否可以出售
	UINT32  m_sellPrice;					//出售价格		
	UINT32  m_overlapNum;					//叠加类型
	UINT32 	m_grownID;						//成长ID
	UINT32  m_boxID;						//箱子ID
	UINT32  m_smeltVal;						//熔炼值
	std::pair<UINT8,stBattlePropValue> m_bpValue;//附加的属性
	UINT32  m_sortID;						//排序ID
	bool	m_bNeedNotify;					//是否需要广播
	UINT32  m_expireTime;					//失效时间

	void fill(const ItemTblRawData & data)
	{
		m_itemID = data.m_field_1;
		strncpy(m_name,data.m_field_2,sizeof(m_name));
		m_type = data.m_field_3;
		m_quality = data.m_field_4;
		m_classType = data.m_field_5;
		m_equipPart = data.m_field_6;
		m_effect = data.m_field_7;
		if (data.m_field_8){
			m_needMedia = true;
		}
		else {
			m_needMedia = false;
		}
		m_mediaID = data.m_field_9;

		if (data.m_field_10){
			m_canSell = true;
		}
		else {
			m_canSell = false;
		}

		m_sellPrice = data.m_field_11;
		m_overlapNum = data.m_field_12;
		m_grownID = data.m_field_13;
		m_boxID = data.m_field_14;
		m_smeltVal = data.m_field_15;

		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_16,"-");
		
		UINT8 propID = 0;
		stBattlePropValue bpValue;

		if (strVec.size() >= 1){
			bpValue.m_propID = propID = (UINT8)atoi(strVec[0].c_str());
		}

		if (strVec.size() >= 2){
			bpValue.m_val1 = (UINT32)atoi(strVec[1].c_str());	
		}
		
		if (strVec.size() >= 3){
			bpValue.m_val2 = (UINT32)atoi(strVec[2].c_str());	
		}
		
		if (strVec.size() >= 4){
			bpValue.m_val3 = (UINT32)atoi(strVec[3].c_str());	
		}
		
		if (strVec.size() >= 5){
			bpValue.m_val4 = (UINT32)atoi(strVec[4].c_str());	
		}

		m_bpValue.first = propID;
		m_bpValue.second = bpValue;
		m_sortID = data.m_field_17;
		if (data.m_field_18){
			m_bNeedNotify = true;
		}
		else {
			m_bNeedNotify = false;
		}
		m_expireTime = data.m_field_19;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("加载了道具id=%d,name=%s,type=%d,quality=%d,classType=%d",m_itemID,m_name,m_type,m_quality,m_classType);
#endif
	}
};

#define ITEM_TYPE_COEF  1000

//道具升级表
struct ItemLevelupTblRawData
{
	INT32 getID() const
	{
		return m_field_1 * ITEM_TYPE_COEF + m_field_2;
	}

	UINT32 m_field_1;						//道具id
	UINT32 m_field_2;						//道具等级
	UINT32 m_field_3;						//强化所需经验
	UINT32 m_field_4;						//物品本身经验
	UINT32 m_field_5;						//重铸返还金钱
	UINT32 m_field_6;						//重铸返还陨铁的数量
	UINT32 m_field_7;						//伤害
	UINT32 m_field_8;						//生命
	UINT32 m_field_9;						//强化等级要求角色等级
	UINT32 m_field_10;						//强化提升战力
	UINT32 m_field_11;						//出售价格
	char   m_field_12[100];					//符文额外属性
	char   m_field_13[100];					//符文专用主属性
}__attribute__((packed));

struct stItemLevelupBase
{
	UINT32 m_itemID;			//道具ID
	UINT16 m_raiseLv;			//强化等级
	UINT32 m_needExp;			//强化需要经验
	UINT32 m_exp;				//物品本身经验
	UINT32 m_returnGoldCoin;	//重铸返回经验
	UINT32 m_returnStone;		//重铸返还陨铁的数量
	UINT32 m_damage;			//伤害
	UINT32 m_hp;				//生命值
	UINT16 m_roleLv;			//角色等级
	UINT32 m_addBattlePower;	//增加战力
	UINT32 m_sellPrice;			//出售价格
	std::vector<stBattlePropValue> m_addProps;	//额外增加的属性
	std::vector<stBattlePropValue> m_fuWenProps;//符文专用主属性

	static INT32 genFindID(UINT32 itemID,UINT16 raiseLv)
	{
		return  itemID * ITEM_TYPE_COEF  + raiseLv;
	}

	void fill(const ItemLevelupTblRawData & data)
	{
		m_itemID = data.m_field_1;
		m_raiseLv = data.m_field_2;
		m_needExp = data.m_field_3;
		m_exp = data.m_field_4;
		m_returnGoldCoin = data.m_field_5;
		m_returnStone = data.m_field_6;
		m_damage = data.m_field_7;
		m_hp = data.m_field_8;
		m_roleLv = data.m_field_9;
		m_addBattlePower = data.m_field_10;
		m_sellPrice = data.m_field_11;
		
		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_12,";");
		for (UINT16 i = 0;i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			stBattlePropValue propVal;
			if (subStrVec.size() >= 1){
				propVal.m_propID = (UINT8)atoi(subStrVec[0].c_str());
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
			m_addProps.push_back(propVal);
		}

        strVec.clear();
		g_stringtok(strVec,data.m_field_13,";");
        for (UINT16 i = 0; i < strVec.size(); ++i) {
			std::vector<std::string> subStrVec;
            g_stringtok(subStrVec,strVec[i].c_str(),"-");
			stBattlePropValue propVal;
			
			if (subStrVec.size() >= 1){
				propVal.m_propID = (UINT8)atoi(subStrVec[0].c_str());
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
			m_fuWenProps.push_back(propVal);

        }
#ifdef _HDZ_DEBUG
		Athena::logger->trace("加载了道具升级表 itemID=%d,raiseLv=%d,needExp=%d,m_damage=%d",m_itemID,m_raiseLv,m_needExp,m_damage);

		Athena::logger->trace("额外增加的属性是:%s, 符文主属性是:%s",data.m_field_12, data.m_field_13);
#endif
	}
};

//道具合成表
struct ItemCombineRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}

	UINT32 m_field_1;						//碎片物品
	UINT32 m_field_2;						//合成物品类型
	UINT32 m_field_3;						//合成物品
	UINT32 m_field_4;						//合成数量
}__attribute__((packed));

enum
{
	COMBINE_TYPE_ITEM = 2,	//道具合成
	COMBINE_TYPE_HERO = 1,	//武将合成
};

struct ItemCombineBase
{
	UINT32 m_id ; 			//碎片ID
	UINT8  m_combineType;	//宠物合成或道具合成
	UINT32 m_combineItemID;	//合成道具ID
	UINT32 m_pieceNum;		//碎片数量

	void fill(const ItemCombineRawData & data)
	{
		m_id = data.m_field_1;
		m_combineType = data.m_field_2;
		m_combineItemID = data.m_field_3;
		m_pieceNum = data.m_field_4;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("合成表 id=%d,m_combineType=%d,num=%d,pieceID=%d",m_combineItemID,m_combineType,m_pieceNum,m_id);
#endif
	}
};

#define SOULINK_SET_COEF 100

struct stSoullinkRawData
{
	INT32 getID() const
	{
		return m_field_1 * SOULINK_SET_COEF + m_field_2;
	}
	
	UINT32 m_field_1;				//灵魂锁链ID
	UINT32 m_field_2;				//套装编号ID
	UINT32 m_field_3;				//需要数量
	char   m_field_4[500];			//所需套装部件
	char   m_field_5[30];			//属性强度
}__attribute__((packed));

struct stSoulinkBase
{
	UINT32 m_id;									//锁链ID
	UINT32 m_setID;									//套装ID
	UINT8  m_needNum;								//套装数量
	std::set<UINT32> m_needEquipIDs[6];				//需要的装备ID	
	std::pair<UINT8,UINT32>  m_prop;				//属性
	std::pair<UINT8,UINT32>  m_prop1;

	static INT32 getFindID(UINT32 growID,UINT8 setID){
		return growID * SOULINK_SET_COEF + setID;
	}
	
	std::set<UINT32> & getNeedEquiID(UINT8 job,UINT8 sex)
	{
		if (job == Role::eJOB_WARRIOR){
			if (sex == Role::eSEX_MAN){
				return m_needEquipIDs[0];  
			}    
			else if (sex == Role::eSEX_FEMALE){
				return m_needEquipIDs[1]; 
			}    
		}    
		else if (job == Role::eJOB_BOWMAN){
			if (sex == Role::eSEX_MAN){
				return m_needEquipIDs[2]; 
			}    
			else if (sex == Role::eSEX_FEMALE){
				return m_needEquipIDs[3]; 
			}    
		}    
		else if (job == Role::eJOB_MASTER){
			if (sex == Role::eSEX_MAN){
				return m_needEquipIDs[4]; 
			}    
			else if (sex == Role::eSEX_FEMALE){
				return m_needEquipIDs[5];
			}
		}
		return m_needEquipIDs[0];			
	}

	void fill(const stSoullinkRawData & data)
	{
		m_id = data.m_field_1;
		m_setID = data.m_field_2;
		m_needNum = data.m_field_3;
		
		std::vector<std::string > strVec;
		g_stringtok(strVec,data.m_field_4,";");
		for (UINT16 i = 0;i < strVec.size();++i){
			UINT32 itemID = (UINT32)atoi(strVec[i].c_str());
			for (UINT8 j = 0;j < 6;++j){//扩充所有职业
				m_needEquipIDs[j].insert(itemID + j);	
			}
		}
		
		
		strVec.clear();
		g_stringtok(strVec,data.m_field_5,";");
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string > subStrVec;
			g_stringtok(subStrVec,strVec[i],"-");
			if (subStrVec.size() >= 2){
				if (0 == i){
					m_prop.first = (UINT8)atoi(subStrVec[0].c_str());
					m_prop.second = (UINT32)atoi(subStrVec[1].c_str());
				}
				else {
					m_prop1.first = (UINT8)atoi(subStrVec[0].c_str());
					m_prop1.second = (UINT32)atoi(subStrVec[1].c_str());
				}
			}
		}

#ifdef _HDZ_DEBUG
		Athena::logger->trace("灵魂锁链套装 id=%d,setID=%d,m_needNum=%d,propID:%d,num:%d,prop1ID:%d,num:%d",m_id,m_setID,m_needNum,m_prop.first,m_prop.second,m_prop1.first,m_prop1.second);
#endif
	}
};

#define REFINE_EFFECT_COEF 1000

struct stRefineEffectRawData
{
	INT32 getID() const
	{
		INT32 id = m_field_1 * REFINE_EFFECT_COEF + m_field_2;
		return id;
	}

	UINT32 m_field_1;				//职业
	UINT32 m_field_2;				//需要精炼等级
	UINT32 m_field_3;				//特效编号
	UINT32 m_field_4;				//id
}__attribute__((packed));

struct stRefineEffectBase
{
	UINT8 	m_job;					//职业ID
	UINT8   m_level;				//等级
	UINT8   m_effectID;				//特效ID
	UINT16  m_id;					//id

	static UINT32 getFindID(UINT8 job,UINT8 lv)
	{
		UINT32 ret = job * REFINE_EFFECT_COEF + lv;
		return ret;
	}

	void fill(const stRefineEffectRawData & data)
	{
		m_job = data.m_field_1;
		m_level = data.m_field_2;
		m_effectID = data.m_field_3;
		m_id = data.m_field_4;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[精炼特效]job:%d,level:%d,effectID:%d,id:%d",m_job,m_level,m_effectID,m_id);
#endif
	}
};
#endif
