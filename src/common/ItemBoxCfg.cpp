#include "ItemBoxCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include <tinyxml/tinyxml.h>

ItemBoxCfg::ItemBoxCfg()
{
	
}

ItemBoxCfg::~ItemBoxCfg()
{

}

bool ItemBoxCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/box.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[宝箱配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[宝箱配置]宝箱配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	TiXmlElement * pBoxNode = pRoot->FirstChildElement("box");
	if (!pBoxNode){
		Athena::logger->error("[宝箱配置]宝箱配置加载错误,找不到box节点!");
		return false;
	}
	

	TiXmlElement * pEntryNode = pBoxNode->FirstChildElement("entry");
	while (pEntryNode){
		stItemBox box;
		if (pEntryNode->Attribute("id",&iVal)){
			box.m_id = iVal;	
		}
		
		if (pEntryNode->Attribute("random",&iVal)){
			box.m_randomType = iVal;
		}
		
		if (pEntryNode->Attribute("bagnum",&iVal)){
			box.m_needSlot = iVal;	
		}

		TiXmlElement * pItemNode = pEntryNode->FirstChildElement("item");
		while (pItemNode){
			stItem item;
			if (pItemNode->Attribute("id",&iVal)){
				item.m_itemID = iVal;
			}	

			if (pItemNode->Attribute("itemnum",&iVal)){
				item.m_num = iVal;
			}

			if (pItemNode->Attribute("randomnum",&iVal)){
				item.m_rate = iVal;
			}
			
			if (pItemNode->Attribute("viplv",&iVal)){
				item.m_vipLv = iVal;	
			}

			if (pItemNode->Attribute("vipAddition",&iVal)){
				item.m_vipAddition = iVal;	
			}
			
			if (pItemNode->Attribute("randomnumlimit",&iVal)){
				item.m_randMin = iVal;	
			}

			if (pItemNode->Attribute("randomnumtoplimit",&iVal)){
				item.m_randMax = iVal;	
			}
			box.m_itemVec.push_back(item);
			pItemNode = pItemNode->NextSiblingElement("item");
		}
		m_boxCfg.insert(std::make_pair(box.m_id,box));
		pEntryNode = pEntryNode->NextSiblingElement("entry");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了箱子的配置");
	std::map<UINT32,stItemBox>::iterator it = m_boxCfg.begin();
	for (;it != m_boxCfg.end();++it){
		stItemBox	& box = it->second;
		Athena::logger->trace("id=%d",box.m_id);
		for (UINT16 i = 0;i < box.m_itemVec.size(); ++i){
			stItem  & item = box.m_itemVec[i];
			Athena::logger->trace("该箱子的道具有:m_itemID=%d,m_num=%d,min:%d,max:%d",item.m_itemID,item.m_num,item.m_randMin,item.m_randMax);
		}
	}
#endif
	return true;
}

ItemBoxCfg::stItemBox * ItemBoxCfg::getItemBox(UINT32 boxID)
{
	std::map<UINT32,stItemBox>::iterator it = m_boxCfg.find(boxID);
	if (it != m_boxCfg.end()){
		return &it->second;
	}
	return NULL;
}
