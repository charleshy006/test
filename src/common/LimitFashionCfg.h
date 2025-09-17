#ifndef __H_LITMIT_FASHION_H__
#define __H_LITMIT_FASHION_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

class CLimitFashionCfg
{
	USE_SINGLETON_NEW(CLimitFashionCfg);
public :
	struct stItem
	{
		UINT16 m_id;
		UINT32 m_itemID;
		UINT32 m_itemNum;
		UINT32 m_rate;

		stItem()
		{
			m_id = 0;
			m_itemID = 0;
			m_itemNum = 0;
			m_rate = 0;
		}
	};

	struct stCfg
	{
		UINT32 m_id;					//获得ID
		UINT32 m_beginTime;				//开始时间
		UINT32 m_endTime;				//结束时间
		std::map<UINT16,stItem>  m_randItems;

		stCfg()
		{
			m_id = 0;
			m_beginTime = 0;
			m_endTime = 0;
		}
	};
public :
	//构造函数 
	static CLimitFashionCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置 
	bool loadCfg();
	//是否存在等级
	bool isLvExist(UINT16 lv);
	//获得配置
	stCfg & getCfg()
	{
		return m_cfg;
	}
	//是否在时间内
	bool isActivityOpen(UINT32 cur)
	{
		return ((cur >= m_cfg.m_beginTime) && (cur <= m_cfg.m_endTime));
	}
private :
	stCfg m_cfg;
	std::map<UINT16,UINT16> m_lv2Nums;
};
#endif
