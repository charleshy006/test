#include "ItemBox.h"
#include "GameUser.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "Chat.h"
#include "ItemBoxCfg.h"
#include <tinyxml/tinyxml.h>

ItemBoxMgr::ItemBoxMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	
}

ItemBoxMgr::~ItemBoxMgr()
{

}

void ItemBoxMgr::openBox(UINT32 boxID,UINT32 num,bool bNotify,UINT32 boxItemID)
{
	ItemBoxCfg::stItemBox * pItemBox = ItemBoxCfg::getSingleton().getItemBox(boxID); 
	if (!pItemBox){
		Athena::logger->trace("开箱子时候找不到配置:%d",boxID);
		return ;
	}

	Role::stObtainItemTip retCmd;
	if (ItemBoxCfg::eRandom_Type_1 == pItemBox->m_randomType){//随机的
		for (UINT16 j = 0;j < num; ++j) {
			std::map<UINT32,UINT32> itemID2Rates;
			std::map<UINT32,UINT32> itemID2Nums;
			UINT32 totalRate = 0;
			for (UINT16 i = 0; i < pItemBox->m_itemVec.size();++i){
				ItemBoxCfg::stItem & item = pItemBox->m_itemVec[i];
				totalRate += item.m_rate;
				itemID2Rates[item.m_itemID] = item.m_rate;
				if (item.m_num ==0 && item.m_randMin >0 && item.m_randMax >0)
				{
					itemID2Nums[item.m_itemID] = g_randBetween(item.m_randMin,item.m_randMax);
				}
				else
				{
					itemID2Nums[item.m_itemID] =  item.m_num;
				}
				
			}

			UINT32 itemID = g_selectByMap(itemID2Rates,totalRate);
			if ((UINT32)-1 != itemID){
				m_pOwner->giveAssert(itemID,itemID2Nums[itemID],0,"开宝箱");
				if (bNotify) {
					ChatMgr::sendObtainItemNotify(m_pOwner,"system_notice_2",itemID,itemID2Nums[itemID],boxItemID);
				}
				Role::stSynTipItem * pSyn = retCmd.add_getitems();
				if (pSyn){
					pSyn->set_itemid(CItem::realItemIDByJobAndSex(itemID,m_pOwner->getJob(),m_pOwner->getSex()));
					pSyn->set_num(itemID2Nums[itemID]);
				}
			}
		}
		m_pOwner->sendProto2Me(retCmd);
	}
	else if (ItemBoxCfg::eRandom_Type_2 == pItemBox->m_randomType){
		for (UINT16 j = 0;j < num; ++j){
			for (UINT16 i = 0; i < pItemBox->m_itemVec.size();++i){
				ItemBoxCfg::stItem & item = pItemBox->m_itemVec[i];
				UINT32 resNum = g_randBetween(item.m_randMin,item.m_randMax);
				m_pOwner->giveAssert(item.m_itemID,resNum,0,"开宝箱");	
				Role::stSynTipItem * pSyn = retCmd.add_getitems();
				if (pSyn){
					pSyn->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
					pSyn->set_num(resNum);
				}
			}
		}
		m_pOwner->sendProto2Me(retCmd);
	}
	else if (ItemBoxCfg::eRandom_Type_3 == pItemBox->m_randomType){
		if (boxItemID < pItemBox->m_itemVec.size()){
			ItemBoxCfg::stItem & item = pItemBox->m_itemVec[boxItemID];
			m_pOwner->giveAssert(item.m_itemID,item.m_num,0,"开宝箱");

			Role::stSynTipItem * pSyn = retCmd.add_getitems();
			if (pSyn){
				pSyn->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
				pSyn->set_num(item.m_num);
			}
		}
		m_pOwner->sendProto2Me(retCmd);
	}
	else {//非随机的
		for (UINT16 j = 0; j < num; ++j){
			for (UINT16 i = 0; i < pItemBox->m_itemVec.size();++i){
				ItemBoxCfg::stItem & item = pItemBox->m_itemVec[i];
				UINT32 itemNum = item.m_num;
				if (item.m_vipLv && (m_pOwner->getVipLv() >= item.m_vipLv)){
					itemNum = item.m_vipAddition;
				}
				m_pOwner->giveAssert(item.m_itemID,itemNum,0,"开宝箱");
				if (bNotify) {
					ChatMgr::sendObtainItemNotify(m_pOwner,"system_notice_2",item.m_itemID,itemNum,boxID);
				}
				Role::stSynTipItem * pSyn = retCmd.add_getitems();
				if (pSyn){
					pSyn->set_itemid(CItem::realItemIDByJobAndSex(item.m_itemID,m_pOwner->getJob(),m_pOwner->getSex()));
					pSyn->set_num(itemNum);
				}
			}
		}
		m_pOwner->sendProto2Me(retCmd);
	}
}

UINT16 ItemBoxMgr::needSlotNum(UINT32 boxID)
{
	ItemBoxCfg::stItemBox *pItemBox = ItemBoxCfg::getSingleton().getItemBox(boxID);
	if (pItemBox){
		return pItemBox->m_needSlot;
	}
	return 1;
}
