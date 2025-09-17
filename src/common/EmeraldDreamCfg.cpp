#include "EmeraldDreamCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include <string>
#include "UtilFunction.h"

CEmeraldDreamCfg::CEmeraldDreamCfg():m_dailyNum(0)
{}

bool CEmeraldDreamCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string emeraldFile = Athena::global["confDir"] + "/emeralddream.xml";
	if (!doc.LoadFile(emeraldFile.c_str())){
		Athena::logger->error("[翡翠梦境配置]打开配置文件%s错误",emeraldFile.c_str());
		return false;
	}
	

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[翡翠梦境配置]读取配置文件找不到节点<root>");
		return false;
	}

	INT32 iVal = 0;

	TiXmlElement * pRankAwardOnNode = pRoot->FirstChildElement("rankaward");
	if(pRankAwardOnNode){
		TiXmlElement * pEntryNode = pRankAwardOnNode->FirstChildElement("Reward");
		while(pEntryNode){
			stRankAward one;

			if (pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}

			if (pEntryNode->Attribute("Startrank",&iVal)){
				one.m_startRank = iVal;
			}

			if (pEntryNode->Attribute("Endrank",&iVal)){
				one.m_endRank = iVal;
			}

			if (pEntryNode->Attribute("ItemID",&iVal)){
				one.m_itemID = iVal;
			}

			if (pEntryNode->Attribute("Boxid",&iVal)){
				one.m_boxID = iVal;
			}
			m_rankAwardMap[one.m_id] = one;
			pEntryNode = pEntryNode->NextSiblingElement("Reward");
		}
	}

#ifdef _HDZ_DEBUG
	{
		std::map<UINT32, stRankAward>::iterator it = m_rankAwardMap.begin();
		for(;it != m_rankAwardMap.end(); ++it){
			const stRankAward & award = it->second;
			Athena::logger->trace("[翡翠梦境]排名区域奖励:id=%d, Startrank=%d， Endrank=%d, ItemID=%d, Boxid=%d",it->first,award.m_startRank,award.m_endRank,award.m_itemID,award.m_boxID);
		}
	}
#endif

	TiXmlElement * pResetOnNode = pRoot->FirstChildElement("reset");
	if(pResetOnNode){
		TiXmlElement * pEntryNode = pResetOnNode->FirstChildElement("time");
		while(pEntryNode){
			stReset one;

			if(pEntryNode->Attribute("num",&iVal)){
				one.m_num = iVal;
			}
			
			if(pEntryNode->Attribute("diamondmun",&iVal)){ 
				one.m_diamondNum = iVal;
			}
			m_resetMap[one.m_num] = one.m_diamondNum ;
			pEntryNode = pEntryNode->NextSiblingElement("time"); 
		}
	}
#ifdef _HDZ_DEBUG
	{
		std::map<UINT8, UINT32>::iterator it = m_resetMap.begin();
		for(;it != m_resetMap.end(); ++it){
			Athena::logger->trace("[翡翠梦境]重置次数消耗钻石:num=%d, diamondNum=%d",it->first,it->second);
		}
	}
#endif

	TiXmlElement * pChangeIdOnNode = pRoot->FirstChildElement("changeid");
	if(pChangeIdOnNode){
		TiXmlElement * pEntryNode = pChangeIdOnNode->FirstChildElement("open");
		while(pEntryNode){
			stChangeId one;

			if(pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}

			if(pEntryNode->Attribute("Lvevlid",&iVal)){
				one.m_levelId = iVal;
			}

			if(pEntryNode->Attribute("opentime",&iVal)){
				one.m_openTime = iVal;
			}

			m_changeIdMap[one.m_id] = one;
			pEntryNode = pEntryNode->NextSiblingElement("open"); 
		}
	}
#ifdef _HDZ_DEBUG
	std::map<UINT8, stChangeId>::iterator it = m_changeIdMap.begin();
	for(;it != m_changeIdMap.end(); ++it){
		const stChangeId & changeid = it->second;
		Athena::logger->trace("[翡翠梦境]副本开启时间:id=%d, levelId=%d, openTime=%d",changeid.m_id,changeid.m_levelId,changeid.m_openTime);
	}
#endif

	TiXmlElement * pDailyNumOnNode = pRoot->FirstChildElement("frequency"); 
	if(pDailyNumOnNode){
		TiXmlElement * pEntryNode = pDailyNumOnNode->FirstChildElement("Day");
		if(pEntryNode){
			if(pEntryNode->Attribute("num",&iVal)){
				m_dailyNum = iVal;
			}
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[翡翠梦境]每日次数:num=%d",m_dailyNum);
#endif
	return true;
}

stRankAward * CEmeraldDreamCfg::getRankAward(UINT32 rank)
{
	std::map<UINT32, stRankAward>::iterator it =  m_rankAwardMap.begin();
	for(; it != m_rankAwardMap.end(); ++it){
		stRankAward & award = it->second;
		if(rank >= award.m_startRank && rank <= award.m_endRank){
			return &award;
		}
	}
	return NULL;
}

UINT32 CEmeraldDreamCfg::getReset(UINT32 num)
{
	std::map<UINT8, UINT32>::iterator it = m_resetMap.find(num);
	if(it != m_resetMap.end()){
		return it->second;
	}
	return (UINT32)-1;
}

stChangeId * CEmeraldDreamCfg::getChangeId(UINT8 id)
{
	std::map<UINT8, stChangeId>::iterator it = m_changeIdMap.find(id);
	if(it != m_changeIdMap.end()){
		return &it->second;
	}
	return NULL;
}
