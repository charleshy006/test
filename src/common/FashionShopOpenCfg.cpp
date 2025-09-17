#include "FashionShopOpenCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include <string>
#include "UtilFunction.h"

bool CFashionShopOpenCfg:: loadCfg()
{
	TiXmlDocument doc;
	std::string fashionFile = Athena::global["confDir"] + "/fashionshop.xml";
	if (!doc.LoadFile(fashionFile.c_str())){
		Athena::logger->error("[时装商店配置]打开配置文件%s错误",fashionFile.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[时装商店配置]读取配置文件找不到节点<root>");
		return false;
	}
	
	INT32 iVal = 0;
	const char * szVal = NULL;

	TiXmlElement * pfashionShopOnNode = pRoot->FirstChildElement("shop");  
	if (pfashionShopOnNode){
		TiXmlElement * pEntryNode = pfashionShopOnNode->FirstChildElement("activity");
		while (pEntryNode){
			stFashinoShopOpenInfo one;
			if (pEntryNode->Attribute("id",&iVal)){
				one.m_ID = iVal;
			}

			if (pEntryNode->Attribute("ShopID",&iVal)){
				    one.m_shopID = iVal;                   
			}

			if ((szVal = pEntryNode->Attribute("StartTime"))){
				struct tm _tm;                                  
				if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
					one.m_startTime = mktime(&_tm);
				}
			}

			if ((szVal = pEntryNode->Attribute("EndTime"))){
				struct tm _tm;
				if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
					one.m_endTime = mktime(&_tm);
				}
			}
			m_allFashionShop.insert(std::make_pair(one.m_ID, one));
			pEntryNode = pEntryNode->NextSiblingElement("activity");
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了时装商店的配置,size = %d", m_allFashionShop.size());
	std::map<UINT16,stFashinoShopOpenInfo>::iterator it = m_allFashionShop.begin();
	for(; it != m_allFashionShop.end(); ++it)
	{
		stFashinoShopOpenInfo & one = it->second;
		Athena::logger->trace("m_id = %d, m_shopid = %d, m_startTime = %d, m_endTime = %d",one.m_ID, one.m_shopID, one.m_startTime, one.m_endTime);	
	}
#endif
	return true;
}

CFashionShopOpenCfg::stFashinoShopOpenInfo *  CFashionShopOpenCfg::getFashionShopOpenInfo(UINT32 cur)
{
	std::map<UINT16,stFashinoShopOpenInfo>::iterator it = m_allFashionShop.begin();
	
	for(;it != m_allFashionShop.end(); ++it)
	{
		stFashinoShopOpenInfo & one = it->second;
		if(cur >= one.m_startTime && cur <= one.m_endTime)
		{
			return &it->second;
		}
	}
	return NULL;
}
