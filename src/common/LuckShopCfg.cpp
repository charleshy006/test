#include "LuckShopCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include <tinyxml/tinyxml.h>

LuckShopCfg::LuckShopCfg()
{
	m_closeBeginTime = 0;
	m_closeEndTime = 0;
	m_freshTime = 0;
}

LuckShopCfg::~LuckShopCfg()
{

}

bool LuckShopCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/luckshop.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[幸运商店配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("activity");
	if (!pRoot){
		Athena::logger->error("[幸运商店配置]配置加载错误,找不到activity节点!");
		return false;
	}
	
	const char * szVal = NULL;
	INT32 iVal = 0;

	if ((szVal = pRoot->Attribute("OffTime"))){
		std::vector<std::string> strVec;
		g_stringtok(strVec,szVal,"-");

		if (strVec.size() >= 2){
			m_closeBeginTime = (UINT32)atoi(strVec[0].c_str());
			m_closeEndTime = (UINT32)atoi(strVec[1].c_str());
		}
		else {
			Athena::logger->error("[幸运商店配置]找不到 activity关闭时间");
			return false;
		}
	}
	
	if (pRoot->Attribute("RefreshTime",&iVal)){
		m_freshTime = iVal;
	}

	TiXmlElement * pScreenNode = pRoot->FirstChildElement("screen");
	while (pScreenNode){
		stScreenInfo screenInfo;
		if (pScreenNode->Attribute("VIP",&iVal)){
			screenInfo.m_vipLv = iVal;
		}

		TiXmlElement * pEntryNode = pScreenNode->FirstChildElement("entry");
		while (pEntryNode){
			stRewardItem  rewardItem;
			if (pEntryNode->Attribute("id",&iVal)){
				rewardItem.m_id = iVal;
			}
			
			if (pEntryNode->Attribute("RP",&iVal)){
				rewardItem.m_rate = iVal;
			}
			
			if (pEntryNode->Attribute("AginRecharge",&iVal)){
				rewardItem.m_needRecharge = iVal;
			}
			
			if (pEntryNode->Attribute("boxid",&iVal)){
				rewardItem.m_boxID = iVal;
			}
			
			screenInfo.m_rewards.insert(std::make_pair(rewardItem.m_id,rewardItem));
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
		m_screenInfos.insert(std::make_pair(screenInfo.m_vipLv,screenInfo));
		pScreenNode = pScreenNode->NextSiblingElement("screen");
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("幸运商店相关配置!");
	Athena::logger->trace("m_closeBeginTime : %d, m_closeEndTime : %d,m_freshTime:%d",m_closeBeginTime,m_closeEndTime,m_freshTime);

	std::map<UINT16,stScreenInfo>::iterator it = m_screenInfos.begin();
	for (;it != m_screenInfos.end();++it){
		stScreenInfo & screenInfo = it->second;
		Athena::logger->trace("vip等级:%d,可能获得的奖励是:",screenInfo.m_vipLv);
		
		std::map<UINT16,stRewardItem>::iterator subIt = screenInfo.m_rewards.begin();

		for (;subIt != screenInfo.m_rewards.end();++subIt){
			stRewardItem & rewardItem = subIt->second;
			Athena::logger->trace("id:%d,rate:%d,needRecharge:%d,boxID:%d",rewardItem.m_id,rewardItem.m_rate,rewardItem.m_needRecharge,rewardItem.m_boxID);
		}
	}
#endif
	return true;
}

LuckShopCfg::stRewardItem  * LuckShopCfg::getRewardItem(UINT16 vipLv,UINT16 id)
{
	std::map<UINT16,stScreenInfo>::iterator it = m_screenInfos.find(vipLv);	
	if (it == m_screenInfos.end()){
		return NULL;
	}

	std::map<UINT16,stRewardItem>::iterator subIt = it->second.m_rewards.find(id);
	if (subIt == it->second.m_rewards.end()){
		return NULL;
	}
	return &subIt->second;
}

UINT16 LuckShopCfg::randVipRewardID(UINT16 vipLv)
{
	std::map<UINT16,stScreenInfo>::iterator it = m_screenInfos.find(vipLv);
	if (it == m_screenInfos.end()){
		return 0;
	}
	
	std::map<UINT32,UINT32> id2Rates;
	UINT32 totalRate = 0;

	std::map<UINT16,stRewardItem>::iterator subIt = it->second.m_rewards.begin();
	for (;subIt != it->second.m_rewards.end();++subIt){
		stRewardItem & rewardItem = subIt->second;
		totalRate += rewardItem.m_rate;
		id2Rates.insert(std::make_pair(rewardItem.m_id,rewardItem.m_rate));
	}

	INT32 id = g_selectByMap(id2Rates,totalRate);

	if (-1 != id){
		return id;
	}
	else {
		return 0;
	}
}
