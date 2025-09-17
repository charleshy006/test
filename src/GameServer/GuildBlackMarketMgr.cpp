#include "GuildBlackMarketMgr.h"
#include "GameEventNotify.h"  
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "Role.pb.h"
#include "KVPair.h"
#include "GuildConfig.h"
#include "GlobalSvrClient.h"
#include "Global.pb.h"

CGuildBlackMarketMgr::CGuildBlackMarketMgr(GameUser * pOwner) : CShopMgr(pOwner)
{}

CGuildBlackMarketMgr::~CGuildBlackMarketMgr()
{}

UINT32 CGuildBlackMarketMgr::loadAllShop(const char * buf)
{ 
	UINT32 ret = 0;                                                                            
	UINT32 version = *(UINT32 *)(buf + ret);                                                   
	ret += sizeof(UINT32);                                                                     

	if (version == s_SHOP_DB_VERSION){                                                         
		UINT16 shopNum = *(UINT16 *)(buf + ret);                                               
		ret += sizeof(UINT16);                                                                 

		for (UINT16 i = 0;i < shopNum ; ++i){                                                  
			stShopDB shop;                                                                     
			shop.m_shopID = *(UINT32 *)(buf + ret);                                            
			ret += sizeof(UINT32);                                                             
			shop.m_lastRefreshTime = *(UINT32 *)(buf + ret);                                   
			ret += sizeof(UINT32);                                                             

			UINT16 itemNum = *(UINT16 *)(buf + ret);                                           
			ret += sizeof(UINT16);                                                             

			for (UINT16 j = 0;j < itemNum ; ++j){                                              
				stShopItemDB item;                                                             
				item.m_uniqueID = *(UINT32 *)(buf + ret);                                      
				ret += sizeof(UINT32);                                                         
				item.m_itemID = *(UINT32 *)(buf + ret);                                        
				ret += sizeof(UINT32);                                                         
				item.m_buyNum = *(UINT32 *)(buf + ret);                                        
				ret += sizeof(UINT32);                                                         
				item.m_buyNumPerTime = *(UINT16 *)(buf + ret);                                 
				ret += sizeof(UINT16);                                                         
				shop.m_itemVec.push_back(item);                                             
			}                                                                                  

			m_shopDB.insert(make_pair(shop.m_shopID,shop));                                    

			Athena::logger->trace("加载了公会黑市info,shopID:%d", shop.m_shopID);                  
		}                                                                                      
	}                                                                                          
#ifdef _HDZ_DEBUG                                                                              
	Athena::logger->trace("加载了name=%s,id=%d的公会黑市数据num=%d",m_pOwner->getName(),m_pOwner->getID(),m_shopDB.size());
#endif                                                                                         
	return ret;                                                                                
}

UINT32 CGuildBlackMarketMgr::saveAllShop(char * buf)
{
	UINT32 ret = 0;                                                                          
	*(UINT32*)(buf + ret) = s_SHOP_DB_VERSION;                                               
	ret += sizeof(UINT32);                                                                   

	*(UINT16*)(buf + ret) = m_shopDB.size();                                                 
	ret += sizeof(UINT16);                                                                   
	std::map<UINT32,stShopDB>::iterator it = m_shopDB.begin();                               
	for (;it != m_shopDB.end();++it){                                                        
		stShopDB & shop = it->second;                                                        
		*(UINT32*)(buf + ret) = shop.m_shopID;                                               
		ret += sizeof(UINT32);                                                               
		*(UINT32*)(buf + ret) = shop.m_lastRefreshTime;                                      
		ret += sizeof(UINT32);                                                               

		*(UINT16 *)(buf + ret) = shop.m_itemVec.size();                                      
		ret += sizeof(UINT16);                                                               
		for (UINT16 i = 0;i < shop.m_itemVec.size(); ++i){                                   
			stShopItemDB &item = shop.m_itemVec[i];
			*(UINT32 *)(buf + ret) = item.m_uniqueID;                                        
			ret += sizeof(UINT32);                                                           
			*(UINT32 *)(buf + ret) = item.m_itemID;                                          
			ret += sizeof(UINT32);                                                           
			*(UINT32 *)(buf + ret) = item.m_buyNum;                                          
			ret += sizeof(UINT32);
			*(UINT16 *)(buf + ret) = item.m_buyNumPerTime;
			ret += sizeof(UINT16);                                                           
		}                                                                                    
	}                                                                                        
#ifdef _HDZ_DEBUG                                                                            
	Athena::logger->trace("保存了name=%s,id=%d的公会黑市数据num=%d",m_pOwner->getName(),m_pOwner->getID(),m_shopDB.size());
#endif 
	return ret;                                                                              
}

void CGuildBlackMarketMgr::openWindow()
{
	Global::stReqSynGuildBlackMarketLv cmd;
	cmd.set_charid(m_pOwner->getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

	Role::stOpenWindowResult retCmd;
	if(g_isTheSameDay(m_pOwner->m_joinInTime,g_gameServer->getSec()))
	{
#ifdef _HDZ_DEBUG
		Athena::logger->error("[打开工会黑市](name:%s, charID:%d)加入时间太短，打开失败！", m_pOwner->getName(), m_pOwner->getID());
#endif
		retCmd.set_result(Role::stOpenWindowResult::TIME_LIMIT);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	retCmd.set_result(Role::stOpenWindowResult::OPEN_SUCCESS);

	retCmd.set_lv(m_pOwner->m_blackMacketLv);

	m_pOwner->sendProto2Me(retCmd);
}

void CGuildBlackMarketMgr::buyItem(UINT32 index)
{
	Role::stMarketBuyItemResult retCmd;  
	                                                                                 
	CGuildConfig::stBlackMarketInfo * pMarketInfo = CGuildConfig::getSingleton().getBlackMarketInfo(m_pOwner->m_blackMacketLv);
	if(!pMarketInfo)                                                                          
	{                                                                                         
		retCmd.set_retcode(Role::stMarketBuyItemResult::INNER_ERROR);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[购买商品](name:%s,charID:%d)该黑市等级(lv:%d)商店配置不存在，拉取列表失败!",m_pOwner->getName(), m_pOwner->getID(), m_pOwner->m_blackMacketLv);
#endif
		m_pOwner->sendProto2Me(retCmd); 
		return ;
	}

	if(m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_BLACK_MARKET_BUY_NUM) >= pMarketInfo->m_purchasequantity){
#ifdef _HDZ_DEBUG 
		Athena::logger->error("[购买商品](name:%s,charID:%d)购买次数用完了，购买失败!",m_pOwner->getName(), m_pOwner->getID());
#endif
		retCmd.set_retcode(Role::stMarketBuyItemResult::TIME_USE_OUT);
		m_pOwner->sendProto2Me(retCmd);
		return;
	}

	UINT32 shopID = pMarketInfo->m_shopID;

	std::map<UINT32,stShop>::iterator cfgIt =  m_shopCfg.find(shopID);                            
	if (cfgIt == m_shopCfg.end()){                                                                
		retCmd.set_retcode(Role::stMarketBuyItemResult::ITEM_NOT_EXIST);
		m_pOwner->sendProto2Me(retCmd);                                                           
#ifdef _HDZ_DEBUG                                                                                 
		Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,要购买的道具配置不存在!",m_pOwner->getName(),m_pOwner->getID());
#endif                                                                                            
		return ;                                                                                  
	}

	std::map<UINT32,stShopDB>::iterator it = m_shopDB.find(shopID);
	if (it == m_shopDB.end()){                                                                    
		retCmd.set_retcode(Role::stMarketBuyItemResult::ITEM_NOT_EXIST);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,要购买的商店不存在,商店ID：%d",m_pOwner->getName(),m_pOwner->getID(), shopID);
#endif                                                                                            
		return ;                                                                                  
	}                                                                                             

	if (canRefresh(it->second,cfgIt->second)){
		autoRefreshShopList(it->second,cfgIt->second);
		setLastRefreshTime(it->second,cfgIt->second);
		return ;                                                                                  
	}                                                

	UINT32 buyNumOneTime = 1; 
	UINT32 hasBuyNum = 0;     

	if (index >= it->second.m_itemVec.size()){                                                   
		retCmd.set_retcode(Role::stMarketBuyItemResult::ITEM_NOT_EXIST);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,发送过来的索引不正确!,indxID = %d,itemID=%d",m_pOwner->getName(),m_pOwner->getID(),index,it->second.m_itemVec[index].m_itemID); 
#endif
		return ;
	}                                                                                            

	hasBuyNum = it->second.m_itemVec[index].m_buyNum;
	UINT32 itemID = it->second.m_itemVec[index].m_itemID;
	UINT32 uniqueID = it->second.m_itemVec[index].m_uniqueID;
	buyNumOneTime = it->second.m_itemVec[index].m_buyNumPerTime;
	UINT32 priceID = 0;

	std::map<UINT32,stShopItem>::iterator cfgItemIt = cfgIt->second.m_itemVec.find(uniqueID); 
	if (cfgItemIt != cfgIt->second.m_itemVec.end()){                                          
		stShopItem & shopItem = cfgItemIt->second;                                            
	//	UINT32 maxBuyNum = 0;
		if (hasBuyNum >= buyNumOneTime){
			retCmd.set_retcode(Role::stMarketBuyItemResult::BUY_OUT);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG                                                                             
			Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,已经超过购买数量,indexID=%d, itemID = %d",m_pOwner->getName(),m_pOwner->getID(),uniqueID,itemID);
#endif 
				return ;                                                                          
		}
/*
#ifdef _HDZ_DEBUG
		if (101201 == itemID){                                                                    
			Athena::logger->trace("(name:%s,id:%d)购买了体力丹,已经购买次数:%d,最大购买次数:%d",m_pOwner->getName(),m_pOwner->getID(),hasBuyNum,maxBuyNum);
		}                                                                                         
#endif   
*/
		UINT8 bagType = Role::BAG_TYPE_MAIN1;  

		if (m_pOwner->m_bagMgr.getFreeSlotNum(bagType) < 2){
			retCmd.set_retcode(Role::stMarketBuyItemResult::BUY_FULL);
			m_pOwner->sendProto2Me(retCmd);                                                      
#ifdef _HDZ_DEBUG                                                                            
			Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,包裹空格不足!",m_pOwner->getName(),m_pOwner->getID());
#endif 
			return ;
		} 


		priceID = shopItem.m_priceID; 
	}
	else {
		return ; 
	}      

	UINT32 price = 0;
	if (priceID){                                                                                
		stPriceBase * pPriceBase1 = g_priceBaseMgr.get(stPriceBase::getFindID(priceID,hasBuyNum)); 
		stPriceBase * pPriceBase2 = g_priceBaseMgr.get(stPriceBase::getFindID(priceID,hasBuyNum));
		UINT32 price1 = 0;
		UINT32 price2 = 0;
		if (pPriceBase1){
			price1 = pPriceBase1->m_price;
		}                                                                                        

		if (pPriceBase2){                                                                        
			price2 = pPriceBase2->m_price;                                                       
		}                                                                                        

		price = price2 - price1;                                                                 
	}                                                                                            
	else {
		price = cfgItemIt->second.m_curPrice * buyNumOneTime;                              
	}                                                                                            

	char szDesc[25] = { 0 };                                                                       
	snprintf(szDesc,sizeof(szDesc),"购买公会黑市道具shopID=%d",shopID);

	if (!m_pOwner->removeResource((eResource)cfgItemIt->second.m_costResType,price,szDesc)){       
		retCmd.set_retcode(Role::stMarketBuyItemResult::RESOURCE_NOT_ENOUGH); 
		m_pOwner->sendProto2Me(retCmd);                                                            
#ifdef _HDZ_DEBUG                                                                                  
		Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,所需资源不足!",m_pOwner->getName(),m_pOwner->getID());
#endif                                                                                             
		return ; 
	}                                                                                              

	Athena::logger->trace("[购买公会黑市道具]道具购买成功，shopID=%d,indexID=%d,itemID=%d",shopID,uniqueID,itemID);

	it->second.m_itemVec[index].m_buyNum += 1;
	m_pOwner->giveAssert(itemID,buyNumOneTime,0,"购买道具");
	retCmd.set_retcode(Role::stMarketBuyItemResult::BUY_SUCCESS);
	stShopItemDB & item = it->second.m_itemVec[index];
	retCmd.mutable_shopitem()->set_index(index);
	retCmd.mutable_shopitem()->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
	retCmd.mutable_shopitem()->set_uniqueid(item.m_uniqueID);
	retCmd.mutable_shopitem()->set_buynum(item.m_buyNum);
	retCmd.mutable_shopitem()->set_buynumpertime(item.m_buyNumPerTime);
	m_pOwner->sendProto2Me(retCmd);


	m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_BLACK_MARKET_BUY_NUM, 1);

	if (SHOP_NORMAL == shopID) {
		stBuyShopItemNum buyShopItemNumNotify;
		buyShopItemNumNotify.notify(m_pOwner, 1, itemID);                                          
	}
}

void CGuildBlackMarketMgr::sendShopList2Me()
{
	Role::stColumnListResult retCmd;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[发送黑市列表](name:%s, charID:%d)拉取黑市列表!", m_pOwner->getName(), m_pOwner->getID());
#endif
	CGuildConfig::stBlackMarketInfo * pMarketInfo = CGuildConfig::getSingleton().getBlackMarketInfo(m_pOwner->m_blackMacketLv);

	if(!pMarketInfo)
	{
		retCmd.set_result(Role::stColumnListResult::FAILURE);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[发送黑市列表](name:%s, charID:%d)(lv:%d)该黑市等级商店配置不存在，拉取列表失败!",m_pOwner->getName(), m_pOwner->getID(), m_pOwner->m_blackMacketLv);
#endif
		m_pOwner->sendProto2Me(retCmd);
		return;
	}

	UINT32 shopID = pMarketInfo->m_shopID;

	std::map<UINT32,stShop>::iterator cfgIt = m_shopCfg.find(shopID);
	if (cfgIt == m_shopCfg.end()){
#ifdef _HDZ_DEBUG
		Athena::logger->error("[发送黑市列表]内部错误，请求的商店列表信息不存在!");
#endif
		retCmd.set_result(Role::stColumnListResult::INNER_ERROR);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	std::map<UINT32,stShopDB>::iterator it = m_shopDB.find(shopID);
	if (it == m_shopDB.end()){//还没有,随机生成
		stShopDB shopDB;
		shopDB.m_shopID = shopID;
		autoRefreshShopList(shopDB,cfgIt->second);
		setLastRefreshTime(shopDB,cfgIt->second);
		m_shopDB[shopDB.m_shopID] = shopDB;
		sendShopList2Me(shopDB,cfgIt->second);
	}
	else if (canRefresh(it->second,cfgIt->second)){//时间到需要刷新
		autoRefreshShopList(it->second,cfgIt->second);
		setLastRefreshTime(it->second,cfgIt->second);
		sendShopList2Me(it->second,cfgIt->second);
	}
	else {
		sendShopList2Me(it->second,cfgIt->second);
	}
}

void CGuildBlackMarketMgr::sendShopList2Me(stShopDB & shopDB,const stShop & shopCfg)
{
	Role::stColumnListResult retCmd;                                                              
	for (UINT16 i = 0;i < shopDB.m_itemVec.size() ; ++i){                                        
		stShopItemDB & item = shopDB.m_itemVec[i];
		Role::stShopItem *pSynItem = retCmd.add_shoplist();
		if (pSynItem){
			pSynItem->set_index(i);
			pSynItem->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
			pSynItem->set_uniqueid(item.m_uniqueID);
			pSynItem->set_buynum(item.m_buyNum);
			pSynItem->set_buynumpertime(item.m_buyNumPerTime);

#ifdef _HDZ_DEBUG
			Athena::logger->trace("公会黑市Item:index:%d, itemid:%d uniqueid:%d, buynum:%d, buynumpertime:%d",i,CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()), item.m_uniqueID, item.m_buyNum, item.m_buyNumPerTime);
#endif
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}

void CGuildBlackMarketMgr::reqRefreshShopList()
{

#ifdef _HDZ_DEBUG                                                                                
	    Athena::logger->trace("[刷新黑市列表](name:%s, charID:%d)拉取黑市列表!", m_pOwner->getName(), m_pOwner->getID());
#endif
	Role::stRefreshListResult retCmd;
	CGuildConfig::stBlackMarketInfo * pMarketInfo = CGuildConfig::getSingleton().getBlackMarketInfo(m_pOwner->m_blackMacketLv);
	if(!pMarketInfo){
		return ;
	}

	UINT32 shopID = pMarketInfo->m_shopID;
	std::map<UINT32,stShop>::iterator cfgIt =  m_shopCfg.find(shopID);
	if (cfgIt == m_shopCfg.end()){
		retCmd.set_result(Role::stRefreshListResult::SHOP_NOT_EXIST);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[刷新黑市列表](name=%s,id=%d)需要刷新的商店ID不存在!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}                                                                                             

	if(getRefreshNum() >= pMarketInfo->m_free){
		if (!m_pOwner->removeAssert(cfgIt->second.m_costResType,cfgIt->second.m_costResNum,0,"刷公会黑市列表")){//没有资源的情况,用钻石刷新
		
			if(getDiamondNum() >= pMarketInfo->m_limitfreshtime){
#ifdef _HDZ_DEBUG 
				Athena::logger->error("[刷新商店列表](name=%s,id=%d)钻石刷新次数限制，刷新黑市失败!", m_pOwner->getName(),m_pOwner->getID());
#endif
				retCmd.set_result(Role::stRefreshListResult::REFRESH_LIMIT);
				m_pOwner->sendProto2Me(retCmd);
				return;
			}

			if (!m_pOwner->removeResource(eResource_Diamond,pMarketInfo->m_freshprice,"刷新公会黑市")){
				retCmd.set_result(Role::stRefreshListResult::RESOURCE_NOT_ENOUGH);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[刷新商店列表](name=%s,id=%d)需要的资源不足,刷新黑市失败!",m_pOwner->getName(),m_pOwner->getID());
#endif
				return ;
			} 
			m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_DIAMOND_REFRESH_NUM,1);
		}
	}

	m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_REFRESH_NUM, 1);

	retCmd.set_result(Role::stRefreshListResult::SUCCESS);
	m_pOwner->sendProto2Me(retCmd);

	Role::stColumnListResult listCmd;
	std::map<UINT32,stShopDB>::iterator it = m_shopDB.find(shopID);
	if (it == m_shopDB.end()){
		stShopDB shopDB;
		autoRefreshShopList(shopDB,cfgIt->second);
		for (UINT16 i = 0;i < shopDB.m_itemVec.size();++i){
			stShopItemDB & item = shopDB.m_itemVec[i];
			Role::stShopItem * pSynItem = listCmd.add_shoplist();
			if (pSynItem){
				pSynItem->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
				pSynItem->set_buynum(item.m_buyNum);
				pSynItem->set_index(i);
				pSynItem->set_uniqueid(item.m_uniqueID);
				pSynItem->set_buynumpertime(item.m_buyNumPerTime);
			}
		}
	}
	else {
		stShopDB & shopDB = it->second; 
		autoRefreshShopList(shopDB,cfgIt->second);
		for (UINT16 i = 0;i < shopDB.m_itemVec.size();++i){
			stShopItemDB & item = shopDB.m_itemVec[i];
			Role::stShopItem * pSynItem = listCmd.add_shoplist();
			if (pSynItem){
				pSynItem->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
				pSynItem->set_buynum(item.m_buyNum);
				pSynItem->set_index(i);
				pSynItem->set_uniqueid(item.m_uniqueID);
				pSynItem->set_buynumpertime(item.m_buyNumPerTime);
			}
		}
	}
	m_pOwner->sendProto2Me(listCmd);
}

bool CGuildBlackMarketMgr::canRefresh(stShopDB & shopDB,const stShop shopCfg)
{
	if(!g_isTheSameDay(shopDB.m_lastRefreshTime,g_gameServer->getSec())){
		return true;
	}
	return false;
}

void CGuildBlackMarketMgr::setLastRefreshTime(stShopDB & shopDB,const stShop shopCfg)
{
	shopDB.m_lastRefreshTime = g_gameServer->getSec(); 
}

UINT32 CGuildBlackMarketMgr::getBuyNum()
{
	return m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_BLACK_MARKET_BUY_NUM);
}

UINT32 CGuildBlackMarketMgr::getRefreshNum()
{
	return m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_REFRESH_NUM);
}

UINT32 CGuildBlackMarketMgr::getDiamondNum()
{
	return m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_DIAMOND_REFRESH_NUM); 
}
