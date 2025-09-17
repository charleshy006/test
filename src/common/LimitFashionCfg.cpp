#include "LimitFashionCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool CLimitFashionCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string guildFile = Athena::global["confDir"] + "/limitdraw.xml";
	if (!doc.LoadFile(guildFile.c_str())){
		Athena::logger->error("[限时时装抽奖]打开配置文件%s错误",guildFile.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("Conf");
	if (!pRoot){
		Athena::logger->error("[限时时装抽奖]读取配置文件找不到节点<conf>");
		return false;
	}
	
	INT32 iVal = 0;
	const char * szVal = NULL;
	TiXmlElement * pActivityNode = pRoot->FirstChildElement("Activity");
	while (pActivityNode){
		if (pActivityNode->Attribute("Activityid",&iVal)){
			m_cfg.m_id = iVal;	
		}
		
		if ((szVal = pActivityNode->Attribute("StartTime"))){
			struct tm _tm;
			if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
				m_cfg.m_beginTime =  mktime(&_tm);	
			}	
		}
		
		if ((szVal = pActivityNode->Attribute("EndTime"))){
			struct tm _tm;
			if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
				m_cfg.m_endTime =  mktime(&_tm);	
			}	
		}

		TiXmlElement * pItemNode = pActivityNode->FirstChildElement("item");
		while (pItemNode){
			stItem item;
			if (pItemNode->Attribute("id",&iVal)){
				item.m_id = iVal;	
			}
			if (pItemNode->Attribute("Itemid",&iVal)){
				item.m_itemID = iVal;
			}
			if (pItemNode->Attribute("ItemNum",&iVal)){
				item.m_itemNum = iVal;
			}
			if (pItemNode->Attribute("DrawProbability",&iVal)){
				item.m_rate = iVal;
			}
			m_cfg.m_randItems.insert(std::make_pair(item.m_id,item));
			pItemNode = pItemNode->NextSiblingElement("item");
		}
		pActivityNode = pActivityNode->NextSiblingElement("Activity"); 
	}

	TiXmlElement * pDrawLevelNode = pRoot->FirstChildElement("DrawLevel");
	if (pDrawLevelNode){
		TiXmlElement *pDrawNode = pDrawLevelNode->FirstChildElement("draw");
		while (pDrawNode){
			UINT16 level = 0;
			UINT16 num = 0;
			if (pDrawNode->Attribute("Level",&iVal)){
				level = iVal;	
			}

			if (pDrawNode->Attribute("DrawNum",&iVal)){
				num = iVal;
			}

			m_lv2Nums[level] = num;
			pDrawNode = pDrawNode->NextSiblingElement("draw");
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印限时时装抽奖活动内容:id:%d,beginTime:%s,endTime:%s",m_cfg.m_id,g_utc2StringDate(m_cfg.m_beginTime,"%Y-%m-%d %H:%M:%S").c_str(),g_utc2StringDate(m_cfg.m_endTime,"%Y-%m-%d %H:%M:%S").c_str());
	Athena::logger->trace("可以抽到的道具有:");
	
	std::map<UINT16,stItem>::iterator it = m_cfg.m_randItems.begin();
	for (;it != m_cfg.m_randItems.end();++it){
		stItem & item = it->second;
		Athena::logger->trace("id:%d,itemID:%d,itemNum:%d,rate:%d",item.m_id,item.m_itemID,item.m_itemNum,item.m_rate);
	}

	std::map<UINT16,UINT16>::iterator subIt = m_lv2Nums.begin();

	for (;subIt != m_lv2Nums.end();++subIt){
		Athena::logger->trace("到达等级 %d 获得积分 %d",subIt->first,subIt->second);	
	}
#endif
	return true;
}

bool CLimitFashionCfg::isLvExist(UINT16 lv)
{
	 if (m_lv2Nums.find(lv) != m_lv2Nums.end()){
	 	return true;
	 }
	 else {
	 	return false;
	 }
}

