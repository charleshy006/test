#include "M78.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"

bool CDrawInfoCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/draw.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[打怪兽]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[打怪兽]抽奖配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	TiXmlElement * pM78DrawNode = pRoot->FirstChildElement("M78Draw");
	if(pM78DrawNode){
		TiXmlElement * pM78DrawIDNode = pM78DrawNode->FirstChildElement("M78DrawID");	
		while(pM78DrawIDNode){

			stDrawInfo drawInfo; 
			
			if (pM78DrawIDNode->Attribute("id",&iVal)){  
				drawInfo.m_id = iVal;
			}
			
			TiXmlElement * pDrawitemNode = pM78DrawIDNode->FirstChildElement("drawitem");
			while(pDrawitemNode){
				stDrawItem drawItem;

				if (pDrawitemNode->Attribute("ID",&iVal)){
					drawItem.m_id = iVal;
				}

				if (pDrawitemNode->Attribute("drawnum",&iVal)){
					drawItem.m_drawnum = iVal;
				}
				
				if (pDrawitemNode->Attribute("drawRP",&iVal)){
					drawItem.m_drawRP = iVal;
				}

				drawInfo.m_items.push_back(drawItem);

				pDrawitemNode = pDrawitemNode->NextSiblingElement("drawitem");
			}
			
			m_items[drawInfo.m_id] = drawInfo;

			pM78DrawIDNode = pM78DrawIDNode->NextSiblingElement("M78DrawID");
		}
	}

	TiXmlElement * pM78DeadNode = pRoot->FirstChildElement("M78Dead");
	if(pM78DeadNode){
		TiXmlElement * pM78DeadIDNode = pM78DeadNode->FirstChildElement("M78DeadID");
		while(pM78DeadIDNode){
			stDrawInfo drawInfo; 

			if (pM78DeadIDNode->Attribute("id",&iVal)){
				drawInfo.m_id = iVal;                  
			}
			
			TiXmlElement * pDrawitemNode = pM78DeadIDNode->FirstChildElement("drawitem");
			while(pDrawitemNode){

				stDrawItem drawItem;                            
				                                                
				if (pDrawitemNode->Attribute("ID",&iVal)){
					drawItem.m_id = iVal;
				}
				                                                
				if (pDrawitemNode->Attribute("drawnum",&iVal)){
					drawItem.m_drawnum = iVal;
				}                                               
				                                                
				if (pDrawitemNode->Attribute("drawRP",&iVal)){
					drawItem.m_drawRP = iVal;
				}                                               
				                                                
				drawInfo.m_items.push_back(drawItem);           
				pDrawitemNode = pDrawitemNode->NextSiblingElement("drawitem");
			}

			m_deadItems[drawInfo.m_id] = drawInfo;

			pM78DeadIDNode = pM78DeadIDNode->NextSiblingElement("M78DeadID");
		}
	}

	TiXmlElement * pM78EXNode = pRoot->FirstChildElement("M78EXDraw");
	if(pM78DeadNode){
		TiXmlElement * pM78DrawIDNode = pM78EXNode->FirstChildElement("M78EXDrawID");
		while(pM78DrawIDNode){
			stDrawInfo drawInfo; 

			if (pM78DrawIDNode->Attribute("id",&iVal)){
				drawInfo.m_id = iVal;                  
			}
			
			TiXmlElement * pDrawitemNode = pM78DrawIDNode->FirstChildElement("drawitem");
			while(pDrawitemNode){

				stDrawItem drawItem;                            
				                                                
				if (pDrawitemNode->Attribute("ID",&iVal)){
					drawItem.m_id = iVal;
				}
				                                                
				if (pDrawitemNode->Attribute("drawnum",&iVal)){
					drawItem.m_drawnum = iVal;
				}                                               
				                                                
				if (pDrawitemNode->Attribute("drawRP",&iVal)){
					drawItem.m_drawRP = iVal;
				}                                               
				                                                
				drawInfo.m_items.push_back(drawItem);           
				pDrawitemNode = pDrawitemNode->NextSiblingElement("drawitem");
			}

			m_exDrawItems[drawInfo.m_id] = drawInfo;

			pM78DrawIDNode = pM78DrawIDNode->NextSiblingElement("M78EXDrawID");
		}
	}


#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载配置攻击抽奖配置信息:");
	std::map<UINT32, stDrawInfo>::iterator it = m_items.begin();
	for(;it != m_items.end(); ++it){
		stDrawInfo & one = it->second;
		Athena::logger->trace("[打怪兽]DrawID:%d", it->first);
		 std::vector<stDrawItem>::iterator subIt = one.m_items.begin();
		 for(;subIt != one.m_items.end(); ++subIt){
		 	stDrawItem & item = *subIt;
			Athena::logger->trace("    [打怪兽]id:%d, drawnum:%d, drawRP:%d", item.m_id, item.m_drawnum, item.m_drawRP);
		 }
	}

	Athena::logger->trace("加载配置死亡抽奖配置信息:");
	std::map<UINT32, stDrawInfo>::iterator iit = m_deadItems.begin();
	for(;iit != m_deadItems.end(); ++iit){
		stDrawInfo & one = iit->second;
		Athena::logger->trace("[打怪兽]DeadID:%d", iit->first);
		std::vector<stDrawItem>::iterator subIt = one.m_items.begin();
		for(;subIt != one.m_items.end(); ++subIt){
			stDrawItem & item = *subIt;
			Athena::logger->trace("    [打怪兽]id:%d, drawnum:%d, drawRP:%d", item.m_id, item.m_drawnum, item.m_drawRP);
		 }
	}

	Athena::logger->trace("加载配置额外抽奖配置信息:");
	std::map<UINT32, stDrawInfo>::iterator iiit = m_exDrawItems.begin();
	for(;iiit != m_exDrawItems.end(); ++iiit){
		stDrawInfo & one = iiit->second;
		Athena::logger->trace("[打怪兽]EXDrawID:%d", iiit->first);
		std::vector<stDrawItem>::iterator subIt = one.m_items.begin();
		for(;subIt != one.m_items.end(); ++subIt){
			stDrawItem & item = *subIt;
			Athena::logger->trace("    [打怪兽]id:%d, drawnum:%d, drawRP:%d", item.m_id, item.m_drawnum, item.m_drawRP);
		 }
	}

#endif

	return true;
}

void CDrawInfoCfg::getAttackRewardById(UINT32 id, UINT32 drawNum, bool isAKey, std::vector<std::pair<UINT32,UINT32> > & vec)
{
	std::map<UINT32, stDrawInfo>::iterator it = m_items.find(id);
	if(it == m_items.end()){
		Athena::logger->error("[打怪兽]获取攻击奖励失败，id:%d 不存在！",id); 
		return;
	}

	std::map<UINT32, UINT32> itemID2Odds;
	UINT32 sumVal = 0;   

	stDrawInfo & info = it->second;

	for (UINT16 i = 0;i < info.m_items.size(); ++i){
		stDrawItem & item = info.m_items[i];
		itemID2Odds[i] = item.m_drawRP;
		sumVal += item.m_drawRP;
	}

	for (UINT32 i = 0;i < drawNum ; ++i){
		INT32 index = g_selectByMap(itemID2Odds,sumVal);
		if ((-1 != index) && (index < (INT32)info.m_items.size())){
			stDrawItem & item = info.m_items[index];
			vec.push_back(std::make_pair(item.m_id,item.m_drawnum));
			if(!isAKey){
				sumVal -= item.m_drawRP;
				itemID2Odds.erase(i);
			}
		}
	} 
}

void CDrawInfoCfg::getDeadRewardById(UINT32 id, UINT32 drawNum,  std::vector<std::pair<UINT32,UINT32> > & vec)
{
	std::map<UINT32, stDrawInfo>::iterator it = m_deadItems.find(id);
	if(it == m_items.end()){
		Athena::logger->error("[打怪兽]获取死亡奖励失败，id:%d 不存在！",id); 
		return;
	}

	std::map<UINT32, UINT32> itemID2Odds;
	UINT32 sumVal = 0;   

	stDrawInfo & info = it->second;

	for (UINT16 i = 0;i < info.m_items.size(); ++i){
		stDrawItem & item = info.m_items[i];
		itemID2Odds[i] = item.m_drawRP;
		sumVal += item.m_drawRP;
	}

	for (UINT32 i = 0;i < drawNum ; ++i){
		INT32 index = g_selectByMap(itemID2Odds,sumVal);
		if ((-1 != index) && (index < (INT32)info.m_items.size())){
			stDrawItem & item = info.m_items[index];
			vec.push_back(std::make_pair(item.m_id,item.m_drawnum));
			sumVal -= item.m_drawRP;
			itemID2Odds.erase(i);
		}                                                          
	}
}

void CDrawInfoCfg::getAttackRewardByIdAndRP(UINT32 id, UINT32 rp, UINT32 drawNum, std::vector<std::pair<UINT32,UINT32> > &vec)
{
	std::map<UINT32, stDrawInfo>::iterator it = m_exDrawItems.find(id);
	if(it == m_items.end()){
		Athena::logger->error("[打怪兽]获取额外奖励失败，id:%d 不存在！",id); 
		return;
	}

	std::map<UINT32, UINT32> itemID2Odds;
	UINT32 sumVal = 0;   

	stDrawInfo & info = it->second;

	for (UINT16 i = 0;i < info.m_items.size(); ++i){
		stDrawItem & item = info.m_items[i];
		itemID2Odds[i] = item.m_drawRP;
		sumVal += item.m_drawRP;
	}

	for (UINT32 i = 0;i < drawNum ; ++i){ 
		if(g_selectByPercent(rp)){
			INT32 index = g_selectByMap(itemID2Odds,sumVal);
			if ((-1 != index) && (index < (INT32)info.m_items.size())){
				stDrawItem & item = info.m_items[index];
				vec.push_back(std::make_pair(item.m_id,item.m_drawnum));
			}
		}
	}
}

bool CM78Cfg::loadCfg()
{
	TiXmlDocument doc; 
	std::string fileName = Athena::global["confDir"] + "/M78.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[打怪兽]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[打怪兽]抽奖配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	const char * szVal = NULL;

	TiXmlElement * pM78Node = pRoot->FirstChildElement("M78");
	if(pM78Node){

		if (pM78Node->Attribute("MaxCount", &iVal)) {                              
			m_bossInfo.m_maxCount = iVal;
		}

		if (pM78Node->Attribute("time", &iVal)) {
			m_bossInfo.m_time = iVal;
		}                                             

		TiXmlElement * pEntryNode = pM78Node->FirstChildElement("entry");
		while(pEntryNode){
			stBoss boss;

			if (pEntryNode->Attribute("id", &iVal)) {
				boss.m_id = iVal;
			}

			if (pEntryNode->Attribute("Bosslv", &iVal)) {
				boss.m_bossLv = iVal;
			}

			if (pEntryNode->Attribute("HP", &iVal)) {
				boss.m_hp = iVal;
			} 

			if (pEntryNode->Attribute("DrawID", &iVal)) {
				boss.m_drawID = iVal;
			}

			if (pEntryNode->Attribute("DeadID", &iVal)) {
				boss.m_deadID = iVal;
			}

			if (pEntryNode->Attribute("CountNum", &iVal)) {
				boss.m_countNum = iVal;
			}

			if (pEntryNode->Attribute("EXDraw", &iVal)) {
			    boss.m_exDraw = iVal;                    
			}

			if (pEntryNode->Attribute("EXRP", &iVal)) {
				boss.m_exrp = iVal;                    
			}
			
			m_bossInfo.m_bossMap[boss.m_bossLv] = boss;

			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

	TiXmlElement * pBuyNode = pRoot->FirstChildElement("Buy");
	if(pBuyNode){

		if (pBuyNode->Attribute("BuyCount", &iVal)) {                              
			m_buyCount.m_buyCount = iVal;
		}
		if (pBuyNode->Attribute("CostItemID", &iVal)) {                              
			m_buyCount.m_costItemID = iVal;
		}else{
			m_buyCount.m_costItemID = (UINT32)eResource_Diamond;
		}

		TiXmlElement * pEntryNode =  pBuyNode->FirstChildElement("entry");
		while(pEntryNode){
			 stBuyInfo info;
			if ((szVal = pEntryNode->Attribute("BuyNum"))) {
				std::vector<std::string> strVec;
				g_stringtok(strVec, szVal, "-");
				if(strVec.size() >= 2){
					info.m_mixBuyNum = (UINT32)atoi(strVec[0].c_str());
					info.m_maxBuyNum = (UINT32)atoi(strVec[1].c_str());
				}
			}

			if (pEntryNode->Attribute("Cost", &iVal)) {
				info.m_cost = iVal;                    
			}
			m_buyCount.m_buyInfo.push_back(info);

			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}	
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载配置boss配置信息:");
	Athena::logger->trace("[打怪兽]MaxCount:%d, time:%d",m_bossInfo.m_maxCount, m_bossInfo.m_time);
	std::map<UINT32, stBoss>::iterator it = m_bossInfo.m_bossMap.begin();
	for(;it != m_bossInfo.m_bossMap.end(); ++it){
		stBoss & boss = it->second;
		Athena::logger->trace("[打怪兽]id:%d, Bosslv:%d, HP:%d, DrawID:%d, DeadID:%d, CountNum:%d",boss.m_id, boss.m_bossLv, boss.m_hp, boss.m_drawID, boss.m_deadID, boss.m_countNum);
	}

	Athena::logger->trace("加载配置购买配置信息:");

	Athena::logger->trace("[打怪兽]BuyCount:%d", m_buyCount.m_buyCount);
	std::vector<stBuyInfo>::iterator iit = m_buyCount.m_buyInfo.begin();
	for(;iit != m_buyCount.m_buyInfo.end();++iit){
		stBuyInfo & one = *iit;
		Athena::logger->trace("[打怪兽]BuyNum:%d-%d, Cost:%d", one.m_mixBuyNum, one.m_maxBuyNum, one.m_cost);
	}
#endif
	return true;
}

CM78Cfg::stBoss * CM78Cfg::getBossByLv(UINT32 lv)
{
	std::map<UINT32, stBoss>::iterator it = m_bossInfo.m_bossMap.find(lv);
	if(it == m_bossInfo.m_bossMap.end()){
		return NULL;
	}

	return &it->second;
}

UINT32 CM78Cfg::getNeedDiamondByNum(UINT32 num)
{
	++num;
	std::vector<stBuyInfo>::iterator it = m_buyCount.m_buyInfo.begin();
	for(;it != m_buyCount.m_buyInfo.end(); ++it){
		stBuyInfo & one = *it;
		if(one.m_mixBuyNum <= num && one.m_maxBuyNum >= num){
			return one.m_cost;
		}
	}
	return (UINT32)-1;
}
