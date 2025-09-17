#ifndef __H_RECASTEQIUP_CFG_H__
#define __H_RECASTEQIUP_CFG_H__ 
#include "TypesDef.h"
#include "Singleton.h"
#include <map>

class RecastEquipCfg
{
	USE_SINGLETON_NEW(RecastEquipCfg);
public :
 
public :
	struct stItem
	{
		stItem()
		{
			m_itemID = 0;
			m_num = 0; 
		}

		UINT32 m_itemID;				//道具ID
		UINT32 m_num;					//数量 
	};
 
private :
	//构造函数
	RecastEquipCfg();
public :
	//析构函数
	~RecastEquipCfg();
	//获得实例
	static RecastEquipCfg & getSingleton()
	{   
		return THE_SINGLETON::Instance();
	} 
	//加载所有配置
	bool loadCfg();
	//
	void AddRecastCfg(UINT32 pieceID, UINT32 peiceCnt, UINT32 itemID);
	//获得配置
	stItem * getRecastItem(UINT32 equipID);
public :
	std::map<UINT32, stItem*> m_Cfg;
};
#endif
