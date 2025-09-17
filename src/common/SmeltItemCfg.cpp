#include "SmeltItemCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"


bool CSmeltItemCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/smelt.xml"; 
	if (!doc.LoadFile(file.c_str())){                                      
		Athena::logger->error("[黑市熔炼]打开配置文件%s错误",file.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[黑市熔炼]找不到节点root");
		return false;
	}

	INT32 val = 0;
	const char * szVal = NULL;

	TiXmlElement * pSmeltqualityNode = pRoot->FirstChildElement("smeltquality");
	if(pSmeltqualityNode){
		TiXmlElement * pQualityNode = pSmeltqualityNode->FirstChildElement("quality");
		while(pQualityNode){
			
			stItemOrder itemOrder;
			
			if (pQualityNode->Attribute("id", &val)) {
				itemOrder.m_id = val;
			}

			if (pQualityNode->Attribute("ProductOrder", &val)) {
				itemOrder.m_productOrder = val;
			}

			if (pQualityNode->Attribute("TotalPR", &val)) { 
				itemOrder.m_totalPR = val;	
			}

			TiXmlElement * pEntry = pQualityNode->FirstChildElement("entry");
			while(pEntry){

				stItemPRGruop gruop;
				
				if (pEntry->Attribute("id", &val)) {
					gruop.m_id = val;			
				}

				if (pEntry->Attribute("EquipNum", &val)) {
					gruop.m_equipNum = val;
				}

				if (pEntry->Attribute("GetPR", &val)) {
					gruop.m_getPR = val;
				}
				
				if (pEntry->Attribute("PRGruop", &val)) {
					gruop.m_gruopID = val;
				}

				itemOrder.m_grop.insert(std::make_pair(gruop.m_id, gruop));
				pEntry = pEntry->NextSiblingElement("entry");
			}

			m_itemOrder.insert(std::make_pair(itemOrder.m_productOrder, itemOrder));
			pQualityNode = pQualityNode->NextSiblingElement("quality");
		}
	}

	TiXmlElement * pRPgroupNode = pRoot->FirstChildElement("RPgroup");
	if(pRPgroupNode)
	{
		TiXmlElement * pGroupNode = pRPgroupNode->FirstChildElement("group");
		
		while(pGroupNode){
			
			stGroup group;
			
			if (pGroupNode->Attribute("GroupID", &val)) {
				group.m_groupID = val;
			}

			if ((szVal = pGroupNode->Attribute("Num"))) {
				std::vector<std::string> strVec;
				g_stringtok(strVec, szVal, ":");
				for(UINT8 i = 0; i < strVec.size(); ++i){
					std::vector<std::string> subVec;
					g_stringtok(subVec, strVec[i], "-");
					std::pair<UINT8, UINT32> p;
					if(subVec.size() >= 2){
						p.first = (UINT8)atoi(subVec[0].c_str());
						p.second = (UINT32)atoi(subVec[1].c_str());
						group.m_num.push_back(p);
					}
				}
			}

			TiXmlElement * pEntryNode = pGroupNode->FirstChildElement("entry");
			while(pEntryNode){
				
				stItemInfo info;

				if (pEntryNode->Attribute("id", &val)) {
					info.m_id = val;
				}

				if (pEntryNode->Attribute("itemID", &val)) {
					info.m_itemID = val;
				}

				if (pEntryNode->Attribute("RP", &val)) {
					info.m_pr = val;
				}
			
				group.m_items.insert(std::make_pair(info.m_itemID, info));

				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}
			
			m_groups.insert(std::make_pair(group.m_groupID, group));

			pGroupNode = pGroupNode->NextSiblingElement("group");
		}
	}

#ifdef _HDZ_DEBUG
	{
		std::map<UINT16, stItemOrder>::iterator it = m_itemOrder.begin();
		for(;it != m_itemOrder.end(); ++it){
			stItemOrder & one = it->second;
			Athena::logger->trace("[黑市熔炼] id:%d, ProductOrder:%d, TotalPR:%d", one.m_id,one.m_productOrder, one.m_totalPR);

			std::map<UINT8, stItemPRGruop>::iterator subIt = one.m_grop.begin();
			for(; subIt != one.m_grop.end(); ++subIt){
				stItemPRGruop & gruop = subIt->second;
				Athena::logger->trace("[黑市熔炼] id:%d, EquipNum:%d, GetPR:%d, PRGruop:%d", gruop.m_id, gruop.m_equipNum, gruop.m_getPR, gruop.m_gruopID);
			}
		}
	}

	{
		std::map<UINT16, stGroup>::iterator it = m_groups.begin();
		for(;it != m_groups.end(); ++it){
			stGroup & group = it->second;
			std::stringstream  str;
			std::vector<std::pair<UINT8, UINT32> >::iterator vecIt = group.m_num.begin();
			for(;vecIt != group.m_num.end(); ++vecIt){
				str<<(UINT32)(*vecIt).first<<":"<<(*vecIt).second<<"-"; 
			}

			Athena::logger->trace("[黑市熔炼] GroupID:%d, Num:%s", group.m_groupID, str.str().c_str());

			std::map<UINT32, stItemInfo>::iterator subIt = group.m_items.begin();
			for(;subIt != group.m_items.end(); ++subIt){
				stItemInfo & info = subIt->second;
				Athena::logger->trace("[黑市熔炼] id:%d, itemID:%d, RP:%d", info.m_id, info.m_itemID, info.m_pr);

			}
		}
	}
#endif

	return true;
}

UINT16 CSmeltItemCfg::getGroupIdByOrderAndNum(UINT16 order, UINT8 num)
{
	std::map<UINT16, stItemOrder>::iterator it = m_itemOrder.find(order);
	if(it == m_itemOrder.end()){
		Athena::logger->error("[黑市熔炼] 获取失败，不存在该品阶:%d", order);
		return (UINT16)-1;
	}
	
	stItemOrder & orderGroup = it->second;
	std::map<UINT8, stItemPRGruop>::iterator iit = orderGroup.m_grop.find(num);
	
	if(iit == orderGroup.m_grop.end()){
		Athena::logger->error("[黑市熔炼] 获取失败，数量不正确:%d", num);
		return (UINT16)-1;
	}
	
	stItemPRGruop & gruop = iit->second;
	
	if(g_selectByTenThound(gruop.m_getPR)){
		return gruop.m_gruopID;	
	}

	return (UINT16)-1;
}

void CSmeltItemCfg::getGroupByGroupID(UINT16 id, std::vector<UINT32> & vec)
{
	vec.clear();

	std::map<UINT16, stGroup>::iterator it = m_groups.find(id);
	if(it == m_groups.end()){
		Athena::logger->error("[黑市熔炼] 获取失败，不存在该ID的掉落组:%d",id);
		return ;
	}
	
	stGroup & group = it->second;
	
	std::map<UINT32, UINT32> percentMap;
	UINT32 total = 0;

	std::vector<std::pair<UINT8, UINT32> >::iterator iit = group.m_num.begin();
	for(;iit != group.m_num.end(); ++iit){
		std::pair<UINT8, UINT32> & p = *iit;
		total += p.second;
		percentMap.insert(std::make_pair(p.first, p.second));
	}

	INT32 num = g_selectByMap(percentMap, total);

	total = 0;
	percentMap.clear();

	if(-1 != num){
	
		std::map<UINT32, stItemInfo>::iterator iiit = group.m_items.begin();

		for(;iiit != group.m_items.end(); ++iiit){
			stItemInfo & info = iiit->second;	
			total += info.m_pr;
			percentMap.insert(std::make_pair(info.m_itemID, info.m_pr));
		}

		for(UINT8 i = 0; i < num; ++i){
		
			INT32 num = g_selectByMap(percentMap, total);
			if(-1 != num){
				vec.push_back(num);
			}
		}
	}
}
