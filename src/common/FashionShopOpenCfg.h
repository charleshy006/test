#ifndef __FASHION_SHOP_H__
#define __FASHION_SHOP_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

class CFashionShopOpenCfg
{
	USE_SINGLETON_NEW(CFashionShopOpenCfg);
private :
	CFashionShopOpenCfg() {}
public:
	struct stFashinoShopOpenInfo
	{
		UINT16 m_ID;
		UINT16 m_shopID;
		UINT32 m_startTime;
		UINT32 m_endTime;

		stFashinoShopOpenInfo()
		{
			m_ID = 0;
			m_shopID = 0;
			m_startTime = 0;
			m_endTime = 0;
		}
	};
public:

	//构造函数 
	static CFashionShopOpenCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	//加载配置 
	bool loadCfg();

	stFashinoShopOpenInfo * getFashionShopOpenInfo(UINT32 cur);

private:
	std::map<UINT16, stFashinoShopOpenInfo> m_allFashionShop;
};

#endif
