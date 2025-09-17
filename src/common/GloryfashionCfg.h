#ifndef __GLORY_GASHION_CFG_H__
#define __GLORY_GASHION_CFG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

class CGloryfashionCfg
{
	USE_SINGLETON_NEW(CGloryfashionCfg);
private :
	CGloryfashionCfg() {}
public:
	struct stActivityInfo
	{
		UINT16			m_id;			//活动ID
		UINT32			m_starttime;	//开启时间
		UINT32			m_endtime;		//结束时间
		UINT16			m_costResId;	//花费资源ID
		UINT32			m_costResNum;	//花费资源数量
		UINT32			m_fashionitemid;//时装包裹ID

		stActivityInfo()
		{
			m_id = 0;
			m_starttime = 0;
			m_endtime = 0;
			m_costResId = 0;
			m_costResNum = 0;
			m_fashionitemid = 0;
		}
	};

public:
	//构造函数 
	static CGloryfashionCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	//加载配置 
	bool loadCfg();

	//获得活动信息
	stActivityInfo * getActicityInfo(UINT32 cur);

private:

	std::map<UINT32,stActivityInfo> m_allActivitys;
};


#endif
