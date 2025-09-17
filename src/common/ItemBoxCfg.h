#ifndef __H_ITEMBOX_CFG_H__
#define __H_ITEMBOX_CFG_H__ 
#include "TypesDef.h"
#include "Singleton.h"
#include <map>

class ItemBoxCfg
{
	USE_SINGLETON_NEW(ItemBoxCfg);
public :
	enum 
	{
		eRandom_Type_1  = 1,	//随机类型1
		eRandom_Type_2  = 2,	//随机类型2
		eRandom_Type_3  = 3,	//随机类型3
	};
public :
	struct stItem
	{
		stItem()
		{
			m_itemID = 0;
			m_num = 0;
			m_rate = 0;
			m_vipLv = 0;
			m_vipAddition = 0;
			m_randMin = 0;
			m_randMax = 0;
		}

		UINT32 m_itemID;				//道具ID
		UINT32 m_num;					//数量
		UINT32 m_rate;					//概率
		UINT8  m_vipLv;					//vip等级
		UINT32 m_vipAddition;			//vip对应
		UINT32 m_randMin;				//随机最小值
		UINT32 m_randMax;				//随机最大值
	};

	struct stItemBox
	{
		stItemBox()
		{
			m_id = 0;
			m_randomType = 0;
			m_needSlot = 0;
		}

		UINT32 m_id ;				    //箱子ID
		UINT8  m_randomType;			//随机类型
		UINT16 m_needSlot;				//需要格子数
		std::vector<stItem> m_itemVec;  //箱子中的物品
	};
private :
	//构造函数
	ItemBoxCfg();
public :
	//析构函数
	~ItemBoxCfg();
	//获得实例
	static ItemBoxCfg & getSingleton()
	{   
		return THE_SINGLETON::Instance();
	} 
	//加载所有配置
	bool loadCfg();
	//获得箱子配置
	stItemBox * getItemBox(UINT32 boxID);
public :
	std::map<UINT32,stItemBox> m_boxCfg;
};
#endif
