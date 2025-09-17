#include "RMBShopCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include <tinyxml/tinyxml.h>

CRMBShopCfg::CRMBShopCfg()
{
	
}

CRMBShopCfg::~CRMBShopCfg()
{

}

bool CRMBShopCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/recharge.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[人民币商店配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[人民币商店配置]加载错误,找不到root节点!");
		return false;
	}

	INT32 iVal = 0;
	const char *szVal = NULL;
	TiXmlElement * pPayNode = pRoot->FirstChildElement("pay");
	while (pPayNode){
		UINT32 itemID = 0;
		UINT32 opID = 0;
		std::string opProductID, projectId;
		
		TiXmlElement * pCostNode = pPayNode->FirstChildElement("cost");
		while (pCostNode) {
			if (pCostNode->Attribute("id",&iVal)){
				itemID = iVal;
			}
            m_itemSet.insert(itemID);
			if ((szVal = pCostNode->Attribute("projectid"))) {
                projectId = szVal;
                m_product2ItemMap.insert(std::make_pair(projectId, itemID));
			}

			TiXmlElement * pSpecialNode = pCostNode->FirstChildElement("special");
			while (pSpecialNode){
				if (pSpecialNode->Attribute("opID",&iVal)){
					opID = iVal;		
				}

				if ((szVal = pSpecialNode->Attribute("chargeID"))){
					opProductID = szVal;	
				}
				m_opID2OpProductID2ItemID[opID][opProductID] = itemID;
                m_product2ItemMap.insert(std::make_pair(opProductID, itemID));
				pSpecialNode = pSpecialNode->NextSiblingElement("special");
			}
			pCostNode = pCostNode->NextSiblingElement("cost");
		}
		pPayNode = pPayNode->NextSiblingElement("special");
	}
	Athena::logger->trace("打印特殊渠道的商品:");
	std::map<UINT32,std::map<std::string ,UINT32> >::iterator it = m_opID2OpProductID2ItemID.begin();

	for (;it != m_opID2OpProductID2ItemID.end();++it){
		Athena::logger->trace("渠道:%d 对应商品:",it->first);

		std::map<std::string ,UINT32>::iterator subIt = it->second.begin();

		for (;subIt != it->second.end();++subIt){
			Athena::logger->trace("渠道的商品ID:%s,自己定义的商品ID:%d",subIt->first.c_str(),subIt->second);	
		}
	}
    for (std::map<std::string ,UINT32>::iterator it = m_product2ItemMap.begin(); it !=
            m_product2ItemMap.end(); it++) {
        Athena::logger->trace("project ID : %s, item ID : %d", it->first.c_str(), it->second);
    }
    for (std::set<UINT32>::iterator it = m_itemSet.begin();
            it != m_itemSet.end(); it++) {
        Athena::logger->trace("item ID : %d", *it);
    }
	return true;
}

UINT32 CRMBShopCfg::getItemID(UINT32 opID,std::string opProductID)
{
	std::map<UINT32,std::map<std::string ,UINT32> >::iterator it =  m_opID2OpProductID2ItemID.find(opID);
	if (it == m_opID2OpProductID2ItemID.end()){
		return 0;
	}

	std::map<std::string ,UINT32>::iterator subIt = it->second.find(opProductID);
	if (subIt == it->second.end()){
		return 0;
	}
	return subIt->second;
}

UINT32 CRMBShopCfg::getItemId(std::string & productId) {
    std::map<std::string, UINT32>::iterator it =
            m_product2ItemMap.find(productId);
    if (it != m_product2ItemMap.end()) {
        return it->second;
    }
    return 0;
}

bool CRMBShopCfg::validItem(UINT32 itemId) {
    return m_itemSet.find(itemId) != m_itemSet.end();
}
