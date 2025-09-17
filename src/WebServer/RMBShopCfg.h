#ifndef __H_RMBSHOP_CFG_H__
#define __H_RMBSHOP_CFG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <map>
#include <set>

class CRMBShopCfg
{
	USE_SINGLETON_NEW(CRMBShopCfg);
private :
	//构造函数
	CRMBShopCfg();
public :
	//析构函数
	~CRMBShopCfg();
	//获得实例
	static CRMBShopCfg & getSingleton()
	{   
		return THE_SINGLETON::Instance();
	} 
	//加载所有配置
	bool loadCfg();
	//获得商品ID
	UINT32 getItemID(UINT32 opID,std::string opProductID);
    //获得商铺ID
	UINT32 getItemId(std::string & productId);
    //id是否合法
    bool validItem(UINT32 itemId);
private :
    std::set<UINT32> m_itemSet;
    std::map<std::string, UINT32> m_product2ItemMap;    //productId itemId
	std::map<UINT32,std::map<std::string ,UINT32> > m_opID2OpProductID2ItemID;	//<opID,<OpProductID,itemID>> 
};
#endif
