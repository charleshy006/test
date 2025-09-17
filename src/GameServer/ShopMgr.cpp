#include "ShopMgr.h"
#include "GameUser.h"
#include "GameEventNotify.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include <tinyxml/tinyxml.h>
#include "MiscCfgMgr.h"
#include "FashionShopOpenCfg.h"
#include "OperateActivityCfg.h"
#include "GlobalSvrClient.h"
#include "ResBackCfg.h"

std::map<UINT32,CShopMgr::stShop> CShopMgr::m_shopCfg;

CShopMgr::CShopMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	
}

CShopMgr::~CShopMgr()
{

}

bool CShopMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/Shop.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[商店配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("Root");
	if (!pRoot){
		Athena::logger->error("[商店配置]商店配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	const char * szVal = NULL;
	TiXmlElement * pShopNode = pRoot->FirstChildElement("Shop");
	while (pShopNode){
		stShop shop;	
		if (pShopNode->Attribute("id",&iVal)){
			shop.m_shopID = iVal;	
		}

		if ((szVal = pShopNode->Attribute("name"))){
			shop.m_shopName = szVal;
		}
		
		if (pShopNode->Attribute("refreshTime",&iVal)){
			shop.m_refreshTime = iVal;
		}
		
		if (pShopNode->Attribute("costResType",&iVal)){
			shop.m_costResType = iVal;
		}
		
		if (pShopNode->Attribute("costResNum",&iVal)){
			shop.m_costResNum = iVal;	
		}
		
		if (pShopNode->Attribute("sellNum",&iVal)){
			shop.m_sellNum = iVal;
		}
		
		if (pShopNode->Attribute("Openclass",&iVal)){
			shop.m_openLv = iVal;	
		}
		
		if (pShopNode->Attribute("random",&iVal)){
			if (iVal){
				shop.m_isRandom = true;
			}
			else {
				shop.m_isRandom = false;
			}
		}

		if (pShopNode->Attribute("refreshType",&iVal)){
			shop.m_refreshType = iVal;		
		}

		TiXmlElement * pShopItem = pShopNode->FirstChildElement("item");
		while (pShopItem){
			stShopItem item;	
			
			if (pShopItem->Attribute("indexID",&iVal)){
				item.m_uniqueID = iVal;
			}
		
			if (pShopItem->Attribute("id",&iVal)){
				item.m_itemID = iVal;
			}
			
			if (pShopItem->Attribute("rate",&iVal)){
				item.m_rate = iVal;
			}

			if (pShopItem->Attribute("maxBuyNum",&iVal)){
				item.m_maxBuyNum = iVal;
			}

			if (pShopItem->Attribute("costResType",&iVal)){
				item.m_costResType = iVal;
			}
		
			if (pShopItem->Attribute("curPrice",&iVal)){
				item.m_curPrice = iVal;
			}
			
			if (pShopItem->Attribute("oriPrice",&iVal)){
				item.m_oriPrice = iVal;
			}
			
			if (pShopItem->Attribute("buyNumOneTime",&iVal)){
				item.m_buyNumOneTime = iVal;	
			}
			
			if (pShopItem->Attribute("Buypermissions",&iVal)){
				item.m_buyLimitLv = iVal;		
			}
			
			if (pShopItem->Attribute("vipPrivilege",&iVal)){
				item.m_vipNumID = iVal;
			}

			if (pShopItem->Attribute("priceID",&iVal)){
				item.m_priceID = iVal;
			}
			
			if (pShopItem->Attribute("retainBuyNum",&iVal)){
				if (iVal){
					item.m_retainBuyNum = true;	
				}	
			}

			shop.m_itemVec.insert(std::make_pair(item.m_uniqueID,item));
			pShopItem = pShopItem->NextSiblingElement("item");
		}

		m_shopCfg[shop.m_shopID] = shop;	
		pShopNode = pShopNode->NextSiblingElement("Shop");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了商店的配置");
	std::map<UINT32,stShop>::iterator it = m_shopCfg.begin();
	for (;it != m_shopCfg.end();++it){
		stShop & shop = it->second;
		Athena::logger->trace("shopID=%d,name=%s,m_refreshTime=%d,m_costResType=%d,m_costResNum=%d,openLv=%d,random=%s,refreshType:%d",shop.m_shopID,shop.m_shopName.c_str(),shop.m_refreshTime,shop.m_costResType,shop.m_costResNum,shop.m_openLv,shop.m_isRandom ? "true" : "false",shop.m_refreshType);
		std::map<UINT32,stShopItem>::iterator subIt = shop.m_itemVec.begin();	
		for (;subIt != shop.m_itemVec.end(); ++subIt){
			stShopItem  & item = subIt->second;
			Athena::logger->trace("该商店出售的道具有:m_indexID=%d,m_itemID=%d,m_rate=%d,m_maxBuyNum=%d,m_costResType=%d,m_curPrice=%d,m_oriPrice=%d,buyLimitLv=%d,vipPrivilege=%d,priceID=%d,buyNumOneTime=%d",item.m_uniqueID,item.m_itemID,item.m_rate,item.m_maxBuyNum,item.m_costResType,item.m_curPrice,item.m_oriPrice,item.m_buyLimitLv,item.m_vipNumID,item.m_priceID,item.m_buyNumOneTime);
		}
	}
#endif
	return true;
}

UINT32 CShopMgr::saveAllShop(byte * buf,UINT32 needSpace)
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
	Athena::logger->trace("保存了name=%s,id=%d的商店数据num=%d",m_pOwner->getName(),m_pOwner->getID(),m_shopDB.size());
#endif
	return ret;
}


UINT32 CShopMgr::loadAllShop(byte * buf)
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

			// Athena::logger->trace("加载了商店info,shopID:%d", shop.m_shopID);
		}
	} 
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了name=%s,id=%d的商店数据num=%d",m_pOwner->getName(),m_pOwner->getID(),m_shopDB.size());
#endif
	return ret;
}
void CShopMgr::ramdomShopList(stShopDB & shopDB,const stShop & shopCfg)
{

	std::map<UINT32,UINT32> itemOddsPair;			// uniqueID ---> odds
	std::map<UINT32,UINT32> uniqueIDItemIDPair;		// uniqueID ---> itemID
	//shopDB.m_lastRefreshTime = g_gameServer->getSec();
	shopDB.m_itemVec.clear();
	UINT32 sumVal = 0;
	std::map<UINT32,stShopItem>::const_iterator it = shopCfg.m_itemVec.begin();
	for (;it != shopCfg.m_itemVec.end(); ++it){
		itemOddsPair[it->first] = it->second.m_rate;	
		uniqueIDItemIDPair[it->first] =  it->second.m_itemID;
		sumVal += it->second.m_rate;
	}

	for (UINT16 i = 0;i < shopCfg.m_sellNum;++i){
		UINT32 uniqueID = g_selectByMap(itemOddsPair,sumVal);
		if (uniqueID != (UINT32)-1){
			stShopItemDB item;
			item.m_uniqueID = uniqueID;
			item.m_itemID = uniqueIDItemIDPair[uniqueID];//CItem::realItemIDByJobAndSex(uniqueIDItemIDPair[uniqueID],m_pOwner->getJob(),m_pOwner->getSex());
			item.m_buyNum = 0;
			std::map<UINT32,stShopItem>::const_iterator it = shopCfg.m_itemVec.find(item.m_uniqueID);
			if (it != shopCfg.m_itemVec.end()){
				item.m_buyNumPerTime = g_randBetween(1,it->second.m_buyNumOneTime);
			}
			shopDB.m_itemVec.push_back(item);
		}
	}

}

void CShopMgr::sendShopList2Me(stShopDB & shopDB,const stShop & shopCfg)
{
	Role::stRetShopItemList retCmd;
	retCmd.set_shopid(shopDB.m_shopID);
	UINT32 remainTime = getRemainTime(shopDB,shopCfg);
	retCmd.set_refreshremaintime(remainTime);
	
	if (shopDB.m_shopID == SHOP_HEISHI){
		retCmd.set_refreshnum(m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_REFRESH_HEISHI));	
	}else if(shopDB.m_shopID == SHOP_HEISHI2){
		retCmd.set_refreshnum(m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_PET_COUNT));	
	}

	for (UINT16 i = 0;i < shopDB.m_itemVec.size() ; ++i){
		stShopItemDB & item = shopDB.m_itemVec[i];
		Role::stShopItem *pSynItem = retCmd.add_shoplist();
		if (pSynItem){
			pSynItem->set_index(i);
			pSynItem->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
			pSynItem->set_uniqueid(item.m_uniqueID);
			pSynItem->set_buynum(item.m_buyNum);
			pSynItem->set_buynumpertime(item.m_buyNumPerTime);
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}

void CShopMgr::sendShopList2Me(UINT32 shopID)
{
	std::map<UINT32,stShop>::iterator cfgIt =  m_shopCfg.find(shopID);
	if (cfgIt == m_shopCfg.end()){
#ifdef _HDZ_DEBUG
		Athena::logger->error("[发送商店列表]请求的商店列表信息不存在!");
#endif
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

void CShopMgr::autoRefreshShopList(stShopDB & shopDB,const stShop & shopCfg)
{
	if (shopCfg.m_isRandom) {
		shopDB.m_itemVec.clear();
		ramdomShopList(shopDB,shopCfg);
	}
	else {
		std::map<UINT32,UINT32> itemID2Num;
		for (UINT32 i = 0;i < shopDB.m_itemVec.size();++i){
			stShopItemDB & shopDBItem = shopDB.m_itemVec[i];
			itemID2Num[shopDBItem.m_itemID] = shopDBItem.m_buyNum;
		}
		shopDB.m_itemVec.clear();
		std::map<UINT32,stShopItem>::const_iterator itemIt = shopCfg.m_itemVec.begin();
		for (;itemIt != shopCfg.m_itemVec.end();++itemIt){
			const stShopItem & shopItem = itemIt->second;
			stShopItemDB item;
			item.m_uniqueID = shopItem.m_uniqueID;
			item.m_itemID = shopItem.m_itemID ;//CItem::realItemIDByJobAndSex(shopItem.m_itemID,m_pOwner->getJob(),m_pOwner->getSex());
			if (shopItem.m_retainBuyNum){
				item.m_buyNum = itemID2Num[item.m_itemID]; 	
			}
			else {
				item.m_buyNum = 0;
			}
			item.m_buyNumPerTime = g_randBetween(1,shopItem.m_buyNumOneTime);
			shopDB.m_itemVec.push_back(item);
		}
	}
}

void CShopMgr::buyItem(UINT32 shopID,UINT32 index,UINT32 num, bool isFashion)
{
	Role::stBuyItemResult retCmd;

	if(isFashion){
		UINT32 time = g_gameServer->getSec();
		CFashionShopOpenCfg::stFashinoShopOpenInfo * info = CFashionShopOpenCfg::getSingleton().getFashionShopOpenInfo(time);
		if(!info){
			retCmd.set_retcode(Role::stBuyItemResult::NOT_OPEN_TIME);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,找不到配置!",m_pOwner->getName(),m_pOwner->getID());
#endif      
			return;
		}

		if(info->m_shopID != shopID)
		{
			retCmd.set_retcode(Role::stBuyItemResult::NOT_OPEN_TIME);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,已过购买时间，无法购买!",m_pOwner->getName(),m_pOwner->getID());
#endif  
			return;
		}
	}

	std::map<UINT32,stShop>::iterator cfgIt =  m_shopCfg.find(shopID);
	if (cfgIt == m_shopCfg.end()){
		retCmd.set_retcode(Role::stBuyItemResult::ITEM_NOT_EXIST);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,要购买的道具配置不存在!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	std::map<UINT32,stShopDB>::iterator it = m_shopDB.find(shopID);
	if (it == m_shopDB.end()){
		retCmd.set_retcode(Role::stBuyItemResult::ITEM_NOT_EXIST);
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
		retCmd.set_retcode(Role::stBuyItemResult::ITEM_NOT_EXIST);
		m_pOwner->sendProto2Me(retCmd);
		Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,发送过来的索引不正确!,indxID = %d,itemID=%d",m_pOwner->getName(),m_pOwner->getID(),index,it->second.m_itemVec[index].m_itemID);
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
		UINT32 maxBuyNum = 0;
		
		if (shopItem.m_vipNumID <= 0){
			maxBuyNum = shopItem.m_maxBuyNum;
		}
		else {
			maxBuyNum = m_pOwner->getCountByVip(shopItem.m_vipNumID);
		}

		if (hasBuyNum + num> maxBuyNum){
			retCmd.set_retcode(Role::stBuyItemResult::BUY_OUT);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,已经超过购买数量,indexID=%d, itemID = %d",m_pOwner->getName(),m_pOwner->getID(),uniqueID,itemID);
#endif
			return ;	
		}
#ifdef _HDZ_DEBUG
	if (101201 == itemID){
		Athena::logger->trace("(name:%s,id:%d)购买了体力丹,已经购买次数:%d,最大购买次数:%d",m_pOwner->getName(),m_pOwner->getID(),hasBuyNum,maxBuyNum);	
	}
#endif

		if (!g_gameServer->isAppVersion()) {
			if (shopItem.m_buyLimitLv){//够购买等级限制
				if (SHOP_NORMAL == shopID || SHOP_WELFARE == shopID || SHOP_VIPWEEK == shopID){//普通商店和福利商店
					if (m_pOwner->getVipLv() < shopItem.m_buyLimitLv){
						retCmd.set_retcode(Role::stBuyItemResult::LV_LIMIT);
						m_pOwner->sendProto2Me(retCmd);
						return ;
					}	
				}
				else if (SHOP_GUILD == shopID){
					if (m_pOwner->m_guildLv < shopItem.m_buyLimitLv){
						retCmd.set_retcode(Role::stBuyItemResult::GUILD_LV_LIMIT);
						m_pOwner->sendProto2Me(retCmd);
						return ;
					}	
				}
				else if (SHOP_ARENARANK == shopID){
					if (m_pOwner->m_arenaRank > shopItem.m_buyLimitLv){
						retCmd.set_retcode(Role::stBuyItemResult::ARENA_RANK_LIMIT);
						m_pOwner->sendProto2Me(retCmd);
						return ;
					}	
				}
			}
		}	
		priceID = shopItem.m_priceID;
	}
	else {
		return ;
	}

	UINT8 bagType = Role::BAG_TYPE_MAIN1;
	if(isFashion){
		bagType = Role::BAG_TYPE_FASHION;
	}

	if (m_pOwner->m_bagMgr.getFreeSlotNum(bagType) < 2){
		retCmd.set_retcode(Role::stBuyItemResult::BUY_FULL);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,包裹空格不足!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;	
	}

	UINT32 price = 0;
	if (priceID){
		stPriceBase * pPriceBase1 = g_priceBaseMgr.get(stPriceBase::getFindID(priceID,hasBuyNum));	
		stPriceBase * pPriceBase2 = g_priceBaseMgr.get(stPriceBase::getFindID(priceID,hasBuyNum + num));
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
		price = cfgItemIt->second.m_curPrice * num * buyNumOneTime;	
	}
	
	char szDesc[25] = { 0 };
	snprintf(szDesc,sizeof(szDesc),"购买道具shopID=%d",shopID);

	if (!m_pOwner->removeResource((eResource)cfgItemIt->second.m_costResType,price,szDesc)){
		retCmd.set_retcode(Role::stBuyItemResult::RESOURCE_NOT_ENOUGH);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[购买道具](name=%s,id=%d)购买不成功,所需资源不足!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	Athena::logger->trace("[购买道具]道具购买成功，shopID=%d,indexID=%d,itemID=%d",shopID,uniqueID,itemID);
	
	it->second.m_itemVec[index].m_buyNum += num;
	m_pOwner->giveAssert(itemID,num * buyNumOneTime,0,"购买道具");
	retCmd.set_retcode(Role::stBuyItemResult::BUY_SUCCESS);
	retCmd.set_shopid(shopID);
	stShopItemDB & item = it->second.m_itemVec[index]; 
	retCmd.mutable_shopitem()->set_index(index);
	retCmd.mutable_shopitem()->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
	retCmd.mutable_shopitem()->set_uniqueid(item.m_uniqueID);
	retCmd.mutable_shopitem()->set_buynum(item.m_buyNum);
	retCmd.mutable_shopitem()->set_buynumpertime(item.m_buyNumPerTime);
	m_pOwner->sendProto2Me(retCmd);

    if (SHOP_NORMAL == shopID) {
        stBuyShopItemNum buyShopItemNumNotify;
        buyShopItemNumNotify.notify(m_pOwner, 1, itemID);
        if (101201 == itemID) {
            m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Big_Sp, num, m_pOwner->getVipLv());
        } else if (101202 == itemID) {
            m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Big_Exp, num, m_pOwner->getVipLv());
        }
    } else if (51 == shopID) {
        if (172513 == itemID) {
            m_pOwner->m_resBackMgr.addBackRes(CResBackCfg::Sp_Box, num, m_pOwner->getVipLv());
        }
    }

	Game::stMallBuyLog webCmd;
	webCmd.set_account(m_pOwner->getAccount());
	webCmd.set_charid(m_pOwner->getID());
	webCmd.set_name(m_pOwner->getName());
	webCmd.set_level(m_pOwner->getLevel());
	webCmd.set_viplv(m_pOwner->getVipLv());
	webCmd.set_resid(cfgItemIt->second.m_costResType);
	webCmd.set_resnum(price);
	webCmd.set_itemid(itemID);
	webCmd.set_itemnum(num * buyNumOneTime);
	webCmd.set_shopid(shopID);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void CShopMgr::reqRefreshShopList(UINT32 shopID)
{
	Role::stRefreshShoplistResult retCmd;
	retCmd.set_shopid(shopID);
	std::map<UINT32,stShop>::iterator cfgIt =  m_shopCfg.find(shopID);
	if (cfgIt == m_shopCfg.end()){
		retCmd.set_result(Role::stRefreshShoplistResult::SHOP_NOT_EXIST);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[刷新商店列表](name=%s,id=%d)需要刷新的商店ID不存在!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	if (!m_pOwner->removeAssert(cfgIt->second.m_costResType,cfgIt->second.m_costResNum,0,"刷商店列表")){//没有资源的情况
		bool resEnough = false;
		if (shopID == SHOP_HEISHI || shopID == SHOP_HEISHI2){//如果是黑市的情况,还要检查钻石
			if (m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_REFRESH_HEISHI) >= m_pOwner->getCountByVip(eVipFun_Heishi_Refresh)){
				retCmd.set_result(Role::stRefreshShoplistResult::REFRESH_NUM_USE_OUT);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[刷新商店列表](name=%s,id=%d)钻石刷新次数已经用完,刷新商店失败!",m_pOwner->getName(),m_pOwner->getID());
#endif
				return ;	
			}

			if (!m_pOwner->removeResource(eResource_Diamond,s_DIAMOND_REFRESH_COST,"刷新商店")){
				retCmd.set_result(Role::stRefreshShoplistResult::RESOURCE_NOT_ENOUGH);
				m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
				Athena::logger->error("[刷新商店列表](name=%s,id=%d)需要的资源不足,刷新商店失败!",m_pOwner->getName(),m_pOwner->getID());
#endif
				return ;	
			}
			resEnough = true;
			if(shopID == SHOP_HEISHI){
				m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_REFRESH_HEISHI,1);
			}else if(shopID == SHOP_HEISHI2){
				m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_PET_COUNT,1);
			}
		}
		if (!resEnough) {
			retCmd.set_result(Role::stRefreshShoplistResult::RESOURCE_NOT_ENOUGH);
			m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[刷新商店列表](name=%s,id=%d)需要的资源不足,刷新商店失败!",m_pOwner->getName(),m_pOwner->getID());
#endif
			return ;	
		}
	}
	
	if (shopID == SHOP_HEISHI || shopID == SHOP_HEISHI2){
		m_pOwner->m_sevenDayActivityMgr.addHeishiRefreshNum(1);
	}

	std::map<UINT32,stShopDB>::iterator it = m_shopDB.find(shopID);
	if (it == m_shopDB.end()){
		stShopDB shopDB;
		autoRefreshShopList(shopDB,cfgIt->second);
		for (UINT16 i = 0;i < shopDB.m_itemVec.size();++i){
			stShopItemDB & item = shopDB.m_itemVec[i];
			Role::stShopItem * pSynItem = retCmd.add_shoplist();
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
			Role::stShopItem * pSynItem = retCmd.add_shoplist();
			if (pSynItem){
				pSynItem->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
				pSynItem->set_buynum(item.m_buyNum);
				pSynItem->set_index(i);
				pSynItem->set_uniqueid(item.m_uniqueID);
				pSynItem->set_buynumpertime(item.m_buyNumPerTime);
			}
		}
	}
	retCmd.set_result(Role::stRefreshShoplistResult::SUCCESS);
	m_pOwner->sendProto2Me(retCmd);
}

bool CShopMgr::canRefresh(stShopDB & shopDB,const stShop shopCfg)
{
	if (REFRESH_TYPE_EVERYDAY == shopCfg.m_refreshType){//每日刷新
		if (!g_isTheSameDay(shopDB.m_lastRefreshTime,g_gameServer->getSec())){
			return true;	
		}	
	}
	else if (REFRESH_TYPE_INTERVAL == shopCfg.m_refreshType){//刷新间隔
		if (g_gameServer->getSec() > (shopDB.m_lastRefreshTime + 7200)){
			return true;
		}		
	}
	else if (REFRESH_TYPE_INFINITE == shopCfg.m_refreshType){//永久不刷新
	
	}
	else if (REFRESH_TYPE_WEEK == shopCfg.m_refreshType){//每周刷新
		if (!g_isTheSameWeek(shopDB.m_lastRefreshTime,g_gameServer->getSec())){
			return true;
		}	
	}
	return false;
}

void CShopMgr::setLastRefreshTime(stShopDB & shopDB,const stShop shopCfg)
{
	if (REFRESH_TYPE_EVERYDAY == shopCfg.m_refreshType){//每日刷新
		shopDB.m_lastRefreshTime = g_gameServer->getSec();
	}
	else if (REFRESH_TYPE_INTERVAL == shopCfg.m_refreshType){//刷新间隔
		struct tm _tm;
		RealTime::getLocalTime(_tm,g_gameServer->getSec());
		UINT16 modHour = _tm.tm_hour % 2;
		shopDB.m_lastRefreshTime = g_gameServer->getSec() - (modHour * 3600 + _tm.tm_min * 60 + _tm.tm_sec);	
	}
	else if (REFRESH_TYPE_INFINITE == shopCfg.m_refreshType){//永久不刷新
	
	}
	else if (REFRESH_TYPE_WEEK == shopCfg.m_refreshType){//每周刷新
		shopDB.m_lastRefreshTime = g_gameServer->getSec();	
	}
	Athena::logger->trace("玩家(name:%s,%d)又进行了商店:%d刷新类型(%d):",m_pOwner->getName(),m_pOwner->getID(),shopCfg.m_shopID,shopCfg.m_refreshType);
}

UINT32 CShopMgr::getRemainTime(stShopDB & shopDB,const stShop shopCfg)
{
	if (REFRESH_TYPE_EVERYDAY == shopCfg.m_refreshType){//每日刷新
		struct tm _tm;
		RealTime::getLocalTime(_tm,g_gameServer->getSec());
		return  86400 - (_tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec);
	}
	else if (REFRESH_TYPE_INTERVAL == shopCfg.m_refreshType){//刷新间隔
		struct tm _tm;
		RealTime::getLocalTime(_tm,g_gameServer->getSec());
		UINT16 modHour = _tm.tm_hour % 2;
		return 7200 - (modHour * 3600 + _tm.tm_min * 60 + _tm.tm_sec);	
	}
	else if (REFRESH_TYPE_INFINITE == shopCfg.m_refreshType){//永久不刷新
		return (UINT32)-1;	
	}
	else if (REFRESH_TYPE_WEEK == shopCfg.m_refreshType){ //每周刷新
		return (UINT32)-1;	
	}
	return (UINT32)-1;
}

void CShopMgr::diamondBuyGold()
{
	Role::stBuyGoldResult retCmd;

	UINT32 countByVip = m_pOwner->getCountByVip(eVipFun_GoldcoinBag);

	UINT32 buyCount = m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_DIAMOND_BUY_GOLD_NUM) + 1;
	//UINT32  = m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_DIAMOND_BUY_GOLD_NUM,1);

	if(buyCount > countByVip)
	{
		retCmd.set_result(Role::stBuyGoldResult::eRole_Vip_Limit);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[宝石购买金币](name=%s,id=%d)VIP等级限制!",m_pOwner->getName(),m_pOwner->getID());
#endif
		 m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	CMiscCfgMgr::stBuyGoldInfo *info =  CMiscCfgMgr::getSingleton().getBuyGoldNeedDiamond(buyCount);
	if(!info)
	{
		retCmd.set_result(Role::stBuyGoldResult::eInner_Cfg_Error);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[宝石购买金币](name=%s,id=%d)找不到配置!",m_pOwner->getName(),m_pOwner->getID());
#endif  
		m_pOwner->sendProto2Me(retCmd);        
		return ;                              
	}

	if (!m_pOwner->removeResource(eResource_Diamond,info->m_needDiamond,"宝石购买金币")){
		retCmd.set_result(Role::stBuyGoldResult::eGoldCoin_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[宝石购买金币](name=%s,id=%d)钻石不足!",m_pOwner->getName(),m_pOwner->getID());
#endif
		return ;
	}

	m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_DIAMOND_BUY_GOLD_NUM,1);
	m_pOwner->addResource(eResource_GoldCoin,info->m_gainsGoldNum,"宝石购买金币");
	retCmd.set_result(Role::stBuyGoldResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);

	Athena::logger->trace("[宝石购买金币](name=%s,id=%d)该vip可购买次数:%d, 现购买次数:%d, 花费宝石:%d, 得到金币:%d", m_pOwner->getName(), m_pOwner->getID(), countByVip, buyCount, info->m_needDiamond, info->m_gainsGoldNum);

    Global::stOperateActivityCb globalCmd;
    globalCmd.set_charid(m_pOwner->getID());
    globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Buy_Gold));
    globalCmd.set_num(1);
    GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
}
