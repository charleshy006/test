#include "RMBShop.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "GameUser.h"
#include "GameEventNotify.h"
#include <tinyxml/tinyxml.h>
#include "GlobalSvrClient.h"

std::map<UINT32,CRMBShop::stRMBShopItem> CRMBShop::s_shopItemCfgs;		//物品ID配置
std::map<UINT32, UINT32> CRMBShop::s_id2MoneyCfgs;                      //itemId对应国家货币的配置
std::map<UINT32,UINT32> CRMBShop::s_money2ItemID;
std::map<UINT32, std::set<UINT32> > CRMBShop::m_id2MoneyMap;   //id和所有渠道金额的映射
std::map<UINT32, std::map<UINT32, UINT32> > CRMBShop::m_opid2MoneyMap;   //id和所有渠道金额的映射 itemid-opid-money

UINT32  CRMBShop::s_firstRechargeBoxID = 0;
UINT32	CRMBShop::s_first10RechargeBoxID = 0;


UINT32 CRMBShop::getItemMoney(UINT32 itemId) {
    std::map<UINT32,stRMBShopItem>::iterator it =
        s_shopItemCfgs.find(itemId);
    if (it != s_shopItemCfgs.end()) {
        return it->second.m_needRMB;
    }
    return 0;
}

bool CRMBShop::loadMoneyCfg() {
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/rechargefoross.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[商店货币]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[商店货币]商店配置加载错误,找不到root节点!");
		return false;
	}

	TiXmlElement * pCost = pRoot->FirstChildElement("cost");
	if (!pCost){
		Athena::logger->error("[商店货币]商店配置加载错误,找不到cost节点!");
		return false;
	}
    int id = 0, num = 0;
    while (pCost) {
        if (!pCost->Attribute("id", &id)) {
            Athena::logger->error("[商店货币]商店配置加载错误,找不到id节点!");
            return true;
        }
        if (!pCost->Attribute("rechaegenum", &num)) {
            Athena::logger->error("[商店货币]商店配置加载错误,找不到rechaegenum点!");
            return true;
        }
        s_id2MoneyCfgs.insert(std::make_pair(id, num));
        pCost = pCost->NextSiblingElement("cost");
    }
#ifdef _HDZ_DEBUG
    for (std::map<UINT32, UINT32>::iterator it =
            s_id2MoneyCfgs.begin(); it != s_id2MoneyCfgs.end(); it++) {
        Athena::logger->trace("[商店货币配置]id is %d, money is %d",
                it->first, it->second);
    }
#endif
    return true;
}

bool CRMBShop::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/recharge.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[人民币商店]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[人民币商店]商店配置加载错误,找不到root节点!");
		return false;
	}   
	
	INT32 iVal = 0, money = 0, opid = 0;
	const char * szVal = NULL;
	TiXmlElement * pPayNode = pRoot->FirstChildElement("pay");
	if (pPayNode){
		TiXmlElement * pCostNode = pPayNode->FirstChildElement("cost");
		while (pCostNode){
			stRMBShopItem item;
			if (pCostNode->Attribute("id",&iVal)){
				item.m_id = iVal;		
			}
			
			if ((szVal = pCostNode->Attribute("projectid"))){
				item.m_productID = szVal;
			}

			if (pCostNode->Attribute("type",&iVal)){
				item.m_type = iVal;
			}

			if (pCostNode->Attribute("firstreward",&iVal)){
				item.m_hasFirstReward = iVal;
			}

			if (pCostNode->Attribute("money",&iVal)){
				item.m_needRMB = iVal;
			}
			
			if (pCostNode->Attribute("getdiamond",&iVal)){
				item.m_obtainDiamond = iVal;
			}

			if (pCostNode->Attribute("firstextradiamond",&iVal)){
				item.m_firstBuyExtraDiamond = iVal;
			}
			
			if (pCostNode->Attribute("extradiamond",&iVal)){
				item.m_extraDiamond = iVal;
			}



			// UINT32 tOpenServerTime = (UINT32)atoi(Athena::global["OpenServer_Time"].c_str());
			// Athena::logger->trace("开服时间utc:%d,%s",tOpenServerTime,g_utc2StringFormat(tOpenServerTime).c_str());

			if (item.m_type == eType_Summon) {
				if (pCostNode->Attribute("refreshType",&iVal)){
					item.m_refreshType = iVal;
				}
				if (pCostNode->Attribute("refreshTime",&iVal)){
					item.m_refreshTime = iVal;
				}				
			}
			
			if (item.m_type == eType_Treasure_Box) {
				if (pCostNode->Attribute("buyCount",&iVal)){
					item.m_buyCount = iVal;
				}
				if (pCostNode->Attribute("boxId",&iVal)){
					item.m_boxID = iVal;
				}
				if (pCostNode->Attribute("boxNum",&iVal)){
					item.m_boxNum = iVal;
				}	
				if (pCostNode->Attribute("refreshType",&iVal)){
					item.m_refreshType = iVal;
				}
				if (pCostNode->Attribute("refreshTime",&iVal)){
					item.m_refreshTime = iVal;
				}				
			}

			s_money2ItemID[item.m_needRMB] = item.m_id;
			s_shopItemCfgs.insert(std::make_pair(item.m_id,item));

            std::map<UINT32, std::set<UINT32> >::iterator it = 
                    m_id2MoneyMap.find(item.m_id);
            if (it == m_id2MoneyMap.end()) {
                std::set<UINT32> moneySet;
                moneySet.insert(item.m_needRMB);
                m_id2MoneyMap.insert(std::make_pair(item.m_id, moneySet));
            } else {
                it->second.insert(money);
            }
            TiXmlElement * pSpecial = pCostNode->FirstChildElement("special");
            while (pSpecial) {
                if (pSpecial->Attribute("money", &iVal)){
                    money = iVal;
                }
				if (pSpecial->Attribute("opID", &iVal)){
                    opid = iVal;
                }
                std::map<UINT32, std::set<UINT32> >::iterator it = 
                        m_id2MoneyMap.find(item.m_id);
                if (it == m_id2MoneyMap.end()) {
                    std::set<UINT32> moneySet;
                    moneySet.insert(money);
                    m_id2MoneyMap.insert(std::make_pair(item.m_id, moneySet));
                } else {
                    it->second.insert(money);
                }
				//opid处理
				std::map<UINT32, std::map<UINT32, UINT32> >::iterator itM = 
                        m_opid2MoneyMap.find(item.m_id);
                if (itM == m_opid2MoneyMap.end()) {
                    std::map<UINT32, UINT32> moneyMap;
                    moneyMap.insert(std::make_pair(opid,money));
                    m_opid2MoneyMap.insert(std::make_pair(item.m_id, moneyMap));
                } else {
                    itM->second.insert(std::make_pair(opid,money));
                }

                pSpecial = pSpecial->NextSiblingElement("special");
            }
			pCostNode = pCostNode->NextSiblingElement("cost");
		}
	}
	
	TiXmlElement * pFirstPayNode = pRoot->FirstChildElement("firstPay");
	if (pFirstPayNode){
		TiXmlElement * pRewardNode = pFirstPayNode->FirstChildElement("reward");
		while (pRewardNode){
			UINT16 id = 0;
			UINT32 boxID = 0;
			if (pRewardNode->Attribute("id",&iVal)){
				id = iVal;
			}
			
			if (pRewardNode->Attribute("boxid",&iVal)){
				boxID = iVal;
			}

			if (1 == id){
				s_firstRechargeBoxID = boxID;	
			}
			else if (2 == id){
				s_first10RechargeBoxID = boxID;	
			}
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印人民币商店内容:");
	std::map<UINT32,stRMBShopItem>::iterator it = s_shopItemCfgs.begin();
	for (;it != s_shopItemCfgs.end();++it){
		stRMBShopItem & item = it->second;
		Athena::logger->trace("id=%d,type=%d,firstreward=%d,money=%d,getdiamond=%d,firstextradiamond=%d,extradiamond=%d,productID:%s",item.m_id,item.m_type,item.m_hasFirstReward,item.m_needRMB,item.m_obtainDiamond,item.m_firstBuyExtraDiamond,item.m_extraDiamond,item.m_productID.c_str());
	}

	std::map<UINT32,UINT32>::iterator subIt =  s_money2ItemID.begin();
	for (;subIt != s_money2ItemID.end();++subIt){
		Athena::logger->trace("充值的人民币与商品ID的对应关系:money:%d,itemID:%d",subIt->first,subIt->second);	
	}
    for (std::map<UINT32, std::set<UINT32> >::iterator it =
                m_id2MoneyMap.begin(); it != m_id2MoneyMap.end(); it++) {
        for (std::set<UINT32>::iterator setIt = it->second.begin();
                setIt != it->second.end(); setIt++) {
            Athena::logger->trace("Item id is %d, money is %d", it->first, *setIt);
        }
    }
	 for (std::map<UINT32, std::map<UINT32, UINT32> >::iterator it =
                m_opid2MoneyMap.begin(); it != m_opid2MoneyMap.end(); it++) {
        for (std::map<UINT32, UINT32>::iterator mapIt = it->second.begin();
                mapIt != it->second.end(); mapIt++) {
            Athena::logger->trace("Item id is %d, opid is%d, money is %d", it->first, mapIt->first, mapIt->second);
        }
    }
#endif
	return loadMoneyCfg();
}

UINT32 CRMBShop::save(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	//保存版本
	*(UINT32 *)(buf + ret) = s_SAVE_VERSION_INFO;
	ret += sizeof(UINT32);

	//保存数量
	*(UINT16 *)(buf + ret) = m_buyShopInfo.size();
	ret += sizeof(UINT16);
	
	//保存数据
	std::map<UINT16,stShopItem>::iterator it = m_buyShopInfo.begin();
	for (;it != m_buyShopInfo.end();++it){
		stShopItem & item = it->second;
		*(UINT16 *)(buf + ret) = item.m_id;
		ret += sizeof(UINT16);
		*(UINT32 *)(buf + ret) = item.m_endTime;
		ret += sizeof(UINT32);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("人民币商店:玩家  name :%s, id:%d 下线保存了 id:%d,endTime:%d",m_pOwner->getName(),m_pOwner->getID(),item.m_id,item.m_endTime);
#endif
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("人民币商店:玩家 name :%s, id:%d下线保存了%d字节",m_pOwner->getName(),m_pOwner->getID(),ret);
#endif
	return ret;
}

UINT32 CRMBShop::load(byte * buf)
{
	UINT32 ret = 0;
	//读版本
	UINT32 version = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	if (version == s_SAVE_VERSION_INFO){
		//商品数量
		UINT16 size = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);

		for (UINT16 i = 0;i < size;++i){
			stShopItem item;
			item.m_id = *(UINT16 *)(buf + ret);
			ret += sizeof(UINT16);
			item.m_endTime = *(UINT32 *)(buf + ret);
			ret += sizeof(UINT32);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("人民币商店:玩家  name :%s, id:%d 上线保存了 id:%d,endTime:%d",m_pOwner->getName(),m_pOwner->getID(),item.m_id,item.m_endTime);
#endif
			m_buyShopInfo.insert(std::make_pair(item.m_id,item));
		}
	}
	else {
		UINT16 size = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);
		ret += (6 * size);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("人民币商店:玩家 name :%s, id:%d上线加载了%d字节",m_pOwner->getName(),m_pOwner->getID(),ret);
#endif
	return ret;
}

// CRMBShopMgr::CRMBShopMgr(GameUser * pOwner) : m_rmb_shop(pOwner)
// {
// 	m_rmb_shop = ATHENA_NEW CRMBShop(pOwner);  
// }

// CRMBShopMgr::~CRMBShopMgr()
// {

// }

// void CRMBShopMgr::RechargeCheck(UINT32 rechargeId)
// {
// 	MiscProto::stRechargeCheckResult retCmd;
	
// 	std::map<UINT32,UINT32>::iterator it = m_pOwner->m_buyTreasureCount.find(rechargeId);

// 	if (it != m_pOwner->m_buyTreasureCount.end()) {//已经买过累加
// 		std::map<UINT32,stRMBShopItem>::iterator cfgIt = m_rmb_shop->s_shopItemCfgs.find(rechargeId);	
// 		if (cfgIt == s_shopItemCfgs.end()){
// 			retCmd.set_result(MiscProto::stRechargeCheckResult::eOther_Error);
// 			pUser->sendProto2Me(retCmd);
// 			Athena::logger->error("没有该配置 请检查表格");

// 			return ;
// 		}

// 		UINT32 tempcount = 0;
// 		tempcount = it->second + 1;

// 		Athena::logger->trace("tempcount ： %d,cpmBuyCount ：%d", tempcount, cfgIt->second.m_buyCount);

// 		if (tempcount > cfgIt->second.m_buyCount) {//tempcount 大于配置 false
// 			retCmd.set_result(MiscProto::stRechargeCheckResult::eNo_Times);
// 			pUser->sendProto2Me(retCmd);
// 			return ;
// 		}
// 	}

// 	// 检查背包剩余格子数量
//     if (pUser->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 1) {
//         retCmd.set_result(MiscProto::stRechargeCheckResult::eBag_Full);
//         pUser->sendProto2Me(retCmd);
//         return ;
//     }

// 	retCmd.set_result(MiscProto::stRechargeCheckResult::eSuccess);

// 	pUser->sendProto2Me(retCmd);
// }

CRMBShop::CRMBShop(GameUser * pOwner) : m_pOwner(pOwner)
{
}

CRMBShop::~CRMBShop()
{

}

//for type 5, 6
void CRMBShop::synRMBInfo2Client2(UINT32 lastLoginTime)
{
	Role::stSynRMBShopInfo2 retCmd;
	Athena::logger->trace("[上次登陆]时间 utc:%d,%s",lastLoginTime,g_utc2StringFormat(lastLoginTime).c_str());

	UINT32 tOpenServerTime = (UINT32)atoi(Athena::global["OpenServer_Time"].c_str());
	Athena::logger->trace("[开服时间] utc:%d,%s",tOpenServerTime,g_utc2StringFormat(tOpenServerTime).c_str());

	UINT32 tCurrentTime = g_gameServer->getSec();
	Athena::logger->trace("[当前时间] utc:%d,%s",tCurrentTime,g_utc2StringFormat(tCurrentTime).c_str());

	std::map<UINT32,stRMBShopItem>::iterator it = s_shopItemCfgs.begin();
	for (;it != s_shopItemCfgs.end();++it){
		//持久数据处理
		stRMBShopItem & item = it->second;

		if (item.m_type == eType_Summon || item.m_type == eType_Treasure_Box) {
			Athena::logger->trace("id=%d, type=%d",item.m_id, item.m_type);
			Role::stSynRMPShopItem2 * pItem =  retCmd.add_items();
			pItem->set_id(item.m_id);

			// # 截止时间 todo pItem->set_delaytime();
			if (eType_Treasure_Box_Type1 == item.m_refreshType) {//1表示
				UINT32 tCurrentTime1 = g_gameServer->timestamp_zerohour_time(tCurrentTime + item.m_refreshTime);
				Athena::logger->trace("[宝箱截止时间1  ] utc:%d,%s",tCurrentTime1,g_utc2StringFormat(tCurrentTime1).c_str());
				pItem->set_delaytime(tCurrentTime1);
				if (item.m_refreshTime <= 86400) {//每日刷新 0点 
					//重置逻辑 todo
					if (item.m_type == eType_Treasure_Box) {
						//如果最后登陆时间 和截止日期不是同一天
						if (g_isTheSameDay(lastLoginTime, tCurrentTime1-1)) {
							Athena::logger->trace("type1  86400 是同一天 不需要重置");

						} else{
							Athena::logger->trace("type1 86400 不是同一天  重置～～！！！");
							m_pOwner->m_buyTreasureCount.clear();
						}
					}	
				} else if (item.m_refreshTime > 86400 && item.m_refreshTime <= 604800) {//每周刷新 0点 
					//重置逻辑 todo
					if (item.m_type == eType_Treasure_Box) {
						//如果最后登陆时间 和截止日期不是同一天
						if (g_isTheSameWeek(lastLoginTime, tCurrentTime1-1)) {
							Athena::logger->trace("type1 604800 是同一周 不需要重置");

						} else{
							Athena::logger->trace("type1 604800 不是同一周  重置～～！！！");
							m_pOwner->m_buyTreasureCount.clear();
						}
					}	
				}
	
			} else if (eType_Treasure_Box_Type2 == item.m_refreshType) {//2表示间隔时间刷新 开服时间 
				UINT32 const_endtime = 1893427200; //1893427200 (2030/1/1 00:00:00) 防止死循环
				UINT32 t_index = 1; // 期数
				UINT32 t_StartTime = 0; // 开始时间
				UINT32 t_EndTime = tOpenServerTime; //结束时间
				UINT32 t_RefreshTime = item.m_refreshTime; //刷新间隔时间

				if (tOpenServerTime > tCurrentTime) {//如果开服时间大于今天 写死方便测试
					Athena::logger->trace("开服时间大于今天 内测版本");
					t_StartTime = t_EndTime;
					t_EndTime = tOpenServerTime + (t_RefreshTime * t_index);

					Athena::logger->trace("【内测版本】当前时间距离开服时间 计算出 当前战令期数为 %d", t_index);
					Athena::logger->trace(" 【内测版本】计算出 当前战令期 开始时间  %d,%s", t_StartTime, g_utc2StringFormat(t_StartTime).c_str());
					Athena::logger->trace(" 【内测版本】计算出 当前战令期 结束时间  %d,%s", t_EndTime, g_utc2StringFormat(t_EndTime).c_str());
				
					pItem->set_delaytime(t_EndTime);
					pItem->set_starttime(t_StartTime);
					pItem->set_plannumber(t_index);
					
					if (eType_Summon_Equip == item.m_id){//51
						m_pOwner->m_buySummonPlan = 0;
						m_pOwner->m_buySummonNum = 0;					
						m_pOwner->m_freeSummonSet.clear();
						m_pOwner->m_rechargeSummonSet.clear();
					} else if (eType_Summon_Hero == item.m_id) {//52
						m_pOwner->m_buyLimitSummonPlan = 0;
						m_pOwner->m_buyLimitSummonNum = 0;	
						m_pOwner->m_freeLimitSummonSet.clear();
						m_pOwner->m_rechargeLimitSummonSet.clear();
					}
				} else {

					Athena::logger->trace("[间隔时间] utc:%d,%s",t_RefreshTime,g_utc2StringFormat(t_RefreshTime).c_str());
					while (t_EndTime < const_endtime) { //防止死循环
						t_StartTime = t_EndTime;
						t_EndTime = tOpenServerTime + (t_RefreshTime * t_index);

						if (t_StartTime < tCurrentTime && tCurrentTime < t_EndTime) {//当前时间大于 开始时间 且 小于结束时间
							Athena::logger->trace("ccc 符合 跳出 当前时间%d,%s", tCurrentTime, g_utc2StringFormat(tCurrentTime).c_str());
							Athena::logger->trace("当前时间距离开服时间 计算出 当前战令期数为 %d", t_index);
							Athena::logger->trace(" 计算出 当前战令期 开始时间  %d,%s", t_StartTime, g_utc2StringFormat(t_StartTime).c_str());
							Athena::logger->trace(" 计算出 当前战令期 结束时间  %d,%s", t_EndTime, g_utc2StringFormat(t_EndTime).c_str());
							break;
						} else {
							t_index ++;
						}
					}
					pItem->set_delaytime(t_EndTime);
					pItem->set_starttime(t_StartTime);
					pItem->set_plannumber(t_index);

					if (t_StartTime < lastLoginTime && lastLoginTime < t_EndTime) {
						Athena::logger->trace("上次登陆 在本期战令时间内 不需要重置 。");
					} else {
						Athena::logger->trace("上次登陆 在本期战令时间内  重置战令状态 ！！！");
						if (eType_Summon_Equip == item.m_id){//51
							m_pOwner->m_buySummonPlan = 0;
							m_pOwner->m_buySummonNum = 0;					
							m_pOwner->m_freeSummonSet.clear();
							m_pOwner->m_rechargeSummonSet.clear();
						} else if (eType_Summon_Hero == item.m_id) {//52
							m_pOwner->m_buyLimitSummonPlan = 0;
							m_pOwner->m_buyLimitSummonNum = 0;	
							m_pOwner->m_freeLimitSummonSet.clear();
							m_pOwner->m_rechargeLimitSummonSet.clear();
						}
					}
				}
			} else if (eType_Treasure_Box_Type3 == item.m_refreshType) {//3表示不刷新 永久 
				pItem->set_delaytime(0);
			} else if (eType_Treasure_Box_Type4 == item.m_refreshType) {//4表示每周星期一凌晨 0点
				//暂时不处理 传0
				pItem->set_delaytime(0);
			}


			if (item.m_type == eType_Summon) {//5 战令
				if (eType_Summon_Equip == item.m_id){//51
					// @ 剩余购买次数
					pItem->set_buycount(m_pOwner->m_buySummonPlan);
				} else if (eType_Summon_Hero == item.m_id) {//52
					// @ 剩余购买次数
					pItem->set_buycount(m_pOwner->m_buyLimitSummonPlan);
				}
			} else if (item.m_type == eType_Treasure_Box) {//6 宝箱购买
				std::map<UINT32,UINT32>::iterator buyit = m_pOwner->m_buyTreasureCount.find(item.m_id);
				// @ 剩余购买次数
				UINT32 dtimes = 0;
				if (buyit != m_pOwner->m_buyTreasureCount.end()) {//已经买过累加
					dtimes = item.m_buyCount - buyit->second;
				} else { 
					dtimes = item.m_buyCount;					
				}
				pItem->set_buycount(dtimes);
				Athena::logger->trace("宝箱  %d  已经购买剩余次数  %d ",item.m_id, dtimes);
			}

			
		}
	}

	m_pOwner->sendProto2Me(retCmd);
}

void CRMBShop::synRMBInfo2Client()
{
	Role::stSynRMBShopInfo retCmd;
	
	std::map<UINT16,stShopItem>::iterator it = m_buyShopInfo.begin();

	
	for (;it != m_buyShopInfo.end();++it){
		stShopItem & item = it->second;
		Role::stSynRMPShopItem * pItem =  retCmd.add_items();
		pItem->set_id(item.m_id);
		pItem->set_hasbuy(true);
		if ((UINT32)(-1) != item.m_endTime){
			UINT32 remainTime = item.m_endTime > g_gameServer->getSec() ? item.m_endTime - g_gameServer->getSec() : 0;
			pItem->set_remaintime(remainTime);
		}

		std::map<UINT32,stRMBShopItem>::iterator synCfgIt = s_shopItemCfgs.find(item.m_id);	
		if (synCfgIt == s_shopItemCfgs.end()){
			Athena::logger->error("[人民币商店购买](name=%s,id=%d)找不到商品配置,购买失败",m_pOwner->getName(),m_pOwner->getID());
			return ;
		}

		if (synCfgIt->second.m_type == eType_Treasure_Box)
		{

			std::map<UINT32,UINT32>::iterator it = m_pOwner->m_buyTreasureCount.find(item.m_id);
			if (it != m_pOwner->m_buyTreasureCount.end()) {//已经买过累加
				Athena::logger->trace("已经买过商品 %d,数量是%d", item.m_id, it->second);

				pItem->set_buycount(it->second);
			} else { 
				Athena::logger->error("没有购买过该物品");
				// return ;
				pItem->set_buycount(0);
			}
		}
		else
		{
			pItem->set_buycount(0);
		}
	}


	m_pOwner->sendProto2Me(retCmd);
}

void CRMBShop::buy(UINT32 money,UINT16 id,std::string  tid, const std::string & data, bool isExtend)
{
		Athena::logger->trace("[CHharles ]找到money %d id = %d, name = %s  商品配置 %d", money,
                id, m_pOwner->getName(), m_pOwner->getID());

    std::map<UINT32, std::set<UINT32> >::iterator it = 
            m_id2MoneyMap.find(id);
    if (it == m_id2MoneyMap.end()) {
		Athena::logger->error("[人民币商店购买](id=%d, name=%s)找不到商品配置 %d, 购买失败",
                id, m_pOwner->getName(), m_pOwner->getID());
        return ;
    }
    bool bFindMoney = false;
    if (it->second.find(money) != it->second.end()) {
        bFindMoney = true;
		Athena::logger->trace("[人民币商店购买]找到money %d", money);
    }
	std::map<UINT32,stRMBShopItem>::iterator cfgIt = s_shopItemCfgs.find(id);	
	if (cfgIt == s_shopItemCfgs.end()){
		Athena::logger->error("[人民币商店购买](name=%s,id=%d)找不到商品配置,购买失败",m_pOwner->getName(),m_pOwner->getID());
		return ;
	}
	
	UINT32 obtainDiamond = 0;
	UINT32 giveDiamond = 0;
	if (money != cfgIt->second.m_needRMB && !bFindMoney){
		UINT32 newID = 0;
		UINT32 newMoney = 0;
		std::map<UINT32,UINT32>::iterator subIt =  s_money2ItemID.begin();
		for (;subIt != s_money2ItemID.end();++subIt){
			if (money >= subIt->first){
				newID = subIt->second;
				newMoney = subIt->first;
			}
			else {
				break;
			}
		}
		if (!newID){
			giveDiamond = money * 10;
			m_pOwner->addResource(eResource_Diamond,giveDiamond,"充值数量不足最低档!");
			return ;
		}
		id = newID;
		cfgIt = s_shopItemCfgs.find(id);
		if (cfgIt == s_shopItemCfgs.end()){
			Athena::logger->error("[人民币商店购买](name=%s,id=%d)新的ID:%d找不到商品配置,购买失败",m_pOwner->getName(),m_pOwner->getID(),newID);
			return ;
		}
		giveDiamond = money > newMoney ? (money - newMoney) * 10: 0;
		Athena::logger->error("[人民币商店购买](name=%s,id=%d)花了人民币跟要购买的商品需要的人民币不符:money:%d,needMoney:%d,现在改为itemID:%d,赠送钻石:%d",m_pOwner->getName(),m_pOwner->getID(),money,cfgIt->second.m_needRMB,newID,giveDiamond);
	}

	bool bFirst = false;
	if (m_buyShopInfo.find(id) == m_buyShopInfo.end()){
		bFirst = true;	
	}
	
	if (g_gameServer->isAppVersion()){

	}
	else {
		if ((!bFirst) && (eType_Forever == cfgIt->second.m_type)){
			Athena::logger->error("(id:%d)该类型商品只能购买一次",id);
			return ;
		}

		if ((!bFirst) && (eType_Grown == cfgIt->second.m_type)){
			Athena::logger->error("(id:%d)该类型商品只能购买一次",id);
			return ;
		}
	}

	Role::stBuyRMBItemSuccess retCmd;
	retCmd.mutable_iteminfo()->set_id(id);
	retCmd.mutable_iteminfo()->set_hasbuy(true);

	stShopItem & shopItemDB = m_buyShopInfo[id];
	shopItemDB.m_id = id;	
	if ((cfgIt->second.m_hasFirstReward) && bFirst){
		giveDiamond += cfgIt->second.m_firstBuyExtraDiamond;
	}
	else {
		giveDiamond += cfgIt->second.m_extraDiamond;
	}

	if (eType_Time_Year == cfgIt->second.m_type){//卡季卡年卡等有时间限制的卡
		UINT32 addTime = 0;
		if (eItem_Month_Card == id){
			addTime = 31 * 3600 * 24;
		}
		else if (eItem_Quarter_Card == id){
			addTime = 3 *31 * 3600 * 24;	
		}
		else if (eItem_Year_Card == id){
			addTime = 365 * 3600 * 24;
		}
		shopItemDB.m_endTime = g_gameServer->getSec() + addTime;
		obtainDiamond += cfgIt->second.m_obtainDiamond;
		retCmd.mutable_iteminfo()->set_remaintime(addTime);
	}
	else if (eType_Forever == cfgIt->second.m_type){//永久生效的福利卡
		obtainDiamond += cfgIt->second.m_obtainDiamond;	
	}
	else if (eType_Normal == cfgIt->second.m_type){//普通卡
		obtainDiamond += cfgIt->second.m_obtainDiamond; 	
	}
	else if (eType_Grown == cfgIt->second.m_type){//购买了成长计划
		obtainDiamond += cfgIt->second.m_obtainDiamond;
		MiscProto::stBuyInvestPlanResult buyPlanCmd;
		buyPlanCmd.set_result(MiscProto::stBuyInvestPlanResult::eSUCCESS);
		m_pOwner->sendProto2Me(buyPlanCmd);
		m_pOwner->m_buyInvestPlan = 1;
	}
	else if (eType_Summon == cfgIt->second.m_type){//购买了召唤计划
		if (eType_Summon_Equip == id){
			if (m_pOwner->m_buySummonPlan) {
				MiscProto::stBuySummonPlanResult buyPlanCmd;
				buyPlanCmd.set_result(MiscProto::stBuySummonPlanResult::eHAS_BUY);
				buyPlanCmd.set_index("1");
				m_pOwner->sendProto2Me(buyPlanCmd);
				Athena::logger->trace("【装备】战令已经购买");
				return ;
			} else {
				obtainDiamond += cfgIt->second.m_obtainDiamond;
				MiscProto::stBuySummonPlanResult buyPlanCmd;
				buyPlanCmd.set_result(MiscProto::stBuySummonPlanResult::eSUCCESS);				
				buyPlanCmd.set_index("1");
				m_pOwner->sendProto2Me(buyPlanCmd);
				m_pOwner->m_buySummonPlan = 1;
				Athena::logger->trace("【装备】战令 购买成功！！！");
			}
		}
		else if (eType_Summon_Hero == id) {
			if (m_pOwner->m_buyLimitSummonPlan) {
				MiscProto::stBuySummonPlanResult buyPlanCmd;
				buyPlanCmd.set_result(MiscProto::stBuySummonPlanResult::eHAS_BUY);
				buyPlanCmd.set_index("2");
				m_pOwner->sendProto2Me(buyPlanCmd);
				Athena::logger->trace("【英雄】战令已经购买");
				return ;
			} else {
				obtainDiamond += cfgIt->second.m_obtainDiamond;
				MiscProto::stBuySummonPlanResult buyPlanCmd;
				buyPlanCmd.set_result(MiscProto::stBuySummonPlanResult::eSUCCESS);
				buyPlanCmd.set_index("2");
				m_pOwner->sendProto2Me(buyPlanCmd);
				m_pOwner->m_buyLimitSummonPlan = 1;
				Athena::logger->trace("【英雄】战令 购买成功！！！");
			}
		}

	}
	else if (eType_Treasure_Box == cfgIt->second.m_type){//购买了宝箱
		UINT32 tempcount = 0;

		std::map<UINT32,UINT32>::iterator it = m_pOwner->m_buyTreasureCount.find(id);
		if (it != m_pOwner->m_buyTreasureCount.end()) {//已经买过累加
			tempcount = it->second + 1;
		}
		else {//没买过
			tempcount = 1; //初始化
		}

		if (tempcount > cfgIt->second.m_buyCount) {//tempcount 大于配置 false
			Athena::logger->error("超过购买次数");
			return ;
		} else {
			m_pOwner->m_buyTreasureCount.erase(id);

			std::map<UINT32,UINT32>::iterator it3 = m_pOwner->m_buyTreasureCount.find(id);
			if (it3 != m_pOwner->m_buyTreasureCount.end()) {//已经买过累加
				Athena::logger->error("删除失败！");
			} else { 
				m_pOwner->m_buyTreasureCount.insert(std::make_pair(id, tempcount));
			}

			std::map<UINT32,UINT32>::iterator it2 = m_pOwner->m_buyTreasureCount.find(id);
			retCmd.mutable_iteminfo()->set_buycount(tempcount);

			//发奖 购买宝箱奖励 
			Role::stObtainItemTip tipCmd;
			Role::stSynTipItem * pSyn = tipCmd.add_getitems();
			if (pSyn){
				pSyn->set_itemid(cfgIt->second.m_boxID);
				pSyn->set_num(cfgIt->second.m_boxNum);
				if (cfgIt->second.m_boxID < 30) {
					m_pOwner->addResource((eResource)cfgIt->second.m_boxID, cfgIt->second.m_boxNum, "购买宝箱奖励的 资源");
				} else {
					m_pOwner->giveAssert(cfgIt->second.m_boxID, cfgIt->second.m_boxNum, 0, "购买宝箱奖励的 道具");
				}
			}

			m_pOwner->sendProto2Me(tipCmd);
		}
	}

	m_pOwner->recharge(cfgIt->second.m_needRMB,obtainDiamond,id,tid,giveDiamond);
	m_pOwner->m_taskMgr.clearCardTask();
	m_pOwner->m_taskMgr.checkDailyTask();

#ifdef _YUNDING_TONGJI
	if(isExtend){
		Global::stRechangeStatistics cmd;
		cmd.set_charid(m_pOwner->getID());
		cmd.set_obtaindiamond(obtainDiamond);
		cmd.set_givediamond(giveDiamond);
		cmd.set_data(data);
		cmd.set_money(money);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);	
	}
#endif


	m_pOwner->sendProto2Me(retCmd);
	stGetWelfareCard webfareNotify;
	webfareNotify.notify(m_pOwner);
}

bool CRMBShop::isBuyItem(UINT16 itemID)
{
	if (m_buyShopInfo.find(itemID) != m_buyShopInfo.end()){
		return true;
	}	
	return false;
}

bool CRMBShop::cancelCard(UINT16 type) {
    for (std::map<UINT16,stShopItem>::iterator it = m_buyShopInfo.begin();
            it != m_buyShopInfo.end(); ) {
		std::map<UINT32,stRMBShopItem>::iterator subIt = s_shopItemCfgs.find(it->second.m_id);
		if (type == subIt->second.m_type){
            m_buyShopInfo.erase(it++);
        } else {
            it++;
        }
    }
    return true;
}

bool CRMBShop::existsItem(UINT32 itemId) {
    std::map<UINT32, std::set<UINT32> >::iterator it = 
            m_id2MoneyMap.find(itemId);
    if (it != m_id2MoneyMap.end()) {
        return true;
    }
    return false;
}

//check opid对应的item的money
bool CRMBShop::checkOpItemMoney(UINT32 itemID, UINT32 opID, UINT32 money)
{
	std::map<UINT32, std::map<UINT32,UINT32> >::iterator it = 
            m_opid2MoneyMap.find(itemID);
    if (it == m_opid2MoneyMap.end()) {
        return false;
    }

	std::map<UINT32,UINT32>::iterator itSub = it->second.find(opID);
	if (itSub == it->second.end()) {
        return false;
    }
	if (itSub->second != money)
	{
		return false;
	}
	return true;
}

UINT32 CRMBShop::getItemEndTime(UINT16 itemID)
{
	std::map<UINT16,stShopItem>::iterator it = m_buyShopInfo.find(itemID);
	if (it != m_buyShopInfo.end()){
		return it->second.m_endTime;
	}
	return 0;
}


bool  CRMBShop::isBuyForeverItem()
{
	std::map<UINT16,stShopItem>::iterator it = m_buyShopInfo.begin();
	for (;it != m_buyShopInfo.end();++it){
		std::map<UINT32,stRMBShopItem>::iterator subIt = s_shopItemCfgs.find(it->second.m_id);
		if (eType_Forever == subIt->second.m_type){
			return true;
		}
	}
	return false;
}

UINT32 CRMBShop::getMoney(UINT32 itemId) {
    std::map<UINT32, UINT32>::iterator it =
        s_id2MoneyCfgs.find(itemId);
    if (it != s_id2MoneyCfgs.end()) {
        return it->second;
    }
    return 0;
}
