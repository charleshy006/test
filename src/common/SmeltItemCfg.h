#ifndef __SMELT_ITEM_H__
#define __SMELT_ITEM_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <list>

struct stItemPRGruop
{
	UINT8	m_id;				//ID
	UINT8	m_equipNum;//同品阶的数量
	UINT32	m_getPR;	//权重
	UINT16	m_gruopID;	//获得掉落组ID
	
	stItemPRGruop()
	{
		m_id		=	0;
		m_equipNum	=	0;
		m_getPR		=	0;
		m_gruopID	=	0;
	}
};

struct stItemOrder
{
	UINT8	m_id;			//ID
	UINT16	m_productOrder;	//熔炼物品品阶
	UINT32	m_totalPR;		//总权重

	std::map<UINT8, stItemPRGruop> m_grop;

	stItemOrder()
	{
		m_id			=	0;
		m_productOrder	=	0;
		m_totalPR		=	0;
	}
};


struct stItemInfo
{
	UINT8	m_id;		//ID
	UINT32	m_itemID;	//itemID
	UINT32	m_pr;		//概率

	stItemInfo()
	{
		m_id		=	0;
		m_itemID	=	0;
		m_pr		=	0;
	}
};


struct stGroup
{
	UINT16 m_groupID;								//获得掉落组ID
	std::vector<std::pair<UINT8, UINT32> > m_num;	//道具数量  key:可获得数量,second:概率
	std::map<UINT32, stItemInfo> m_items;			//key:itemID, 

	stGroup()
	{
		m_groupID = 0;
	}
};


class CSmeltItemCfg
{
	USE_SINGLETON_NEW(CSmeltItemCfg);
public:
	//获得实例
	static CSmeltItemCfg & getSingleton() {
    	return THE_SINGLETON::Instance();
    }

	//加载所有配置
	bool loadCfg(); 
	
	//通过物品品阶和数量获得掉落ID
	UINT16 getGroupIdByOrderAndNum(UINT16 order, UINT8 num);

	//通过掉落组ID获得itemIDs (可能有2个)
	void  getGroupByGroupID(UINT16 id, std::vector<UINT32> & vec);
private:
	std::map<UINT16, stItemOrder> m_itemOrder;//key:items品阶
	std::map<UINT16, stGroup> m_groups;			//key:掉落组ID
};


#endif
