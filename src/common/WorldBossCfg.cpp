#include "WorldBossCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include <vector>

CWorldBossCfg::CWorldBossCfg()
{
	m_killReward = 0;
}

CWorldBossCfg::~CWorldBossCfg()
{
}


bool CWorldBossCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/allchange.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[世界boss配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[世界boss配置]读取配置文件找不到节点<root>");
		return false;
	}  

	INT32 iVal = 0;
	const char *szVal = 0;
	TiXmlElement * pActivitychangeNode = pRoot->FirstChildElement("activitychange");
	if (pActivitychangeNode){
		TiXmlElement * pOpenNode = pActivitychangeNode->FirstChildElement("open");
		while (pOpenNode){
			stActivityOpenInfo one;
			if (pOpenNode->Attribute("id",&iVal)){
				one.m_id = iVal;	
			}
			
			if (pOpenNode->Attribute("openlevel",&iVal)){
				one.m_copymapID = iVal;	
			}
			
			if (pOpenNode->Attribute("bossid",&iVal)){
				one.m_bossID = iVal;	
			}

			if ((szVal = pOpenNode->Attribute("opentime"))){
				std::vector<std::string> strVec;
				g_stringtok(strVec,szVal,"-");
				if (strVec.size() >= 2){
					one.m_beginTime = (UINT32)atoi(strVec[0].c_str());
					one.m_endTime = (UINT32)atoi(strVec[1].c_str());
				}
			}
			m_openTimes[one.m_id] = one;
			pOpenNode = pOpenNode->NextSiblingElement("open");
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("活动开启时间");
	{
		std::map<UINT32,stActivityOpenInfo>::iterator it = m_openTimes.begin();
		for (;it != m_openTimes.end();++it){
			stActivityOpenInfo & info = it->second;
			Athena::logger->trace("id:%d,beginTime:%d,endTime:%d,openLevel:%d,bossID:%d",info.m_id,info.m_beginTime,info.m_endTime,info.m_copymapID,info.m_bossID);
		}
	}
#endif

	TiXmlElement * pActivityrewardNode = pRoot->FirstChildElement("activityreward");
	if (pActivityrewardNode){
		TiXmlElement *  pRewardNode = pActivityrewardNode->FirstChildElement("reward");
		while (pRewardNode){
			stRewardInfo info;
			if (pRewardNode->Attribute("startrank",&iVal)){
				info.m_beginRank = 	iVal;
			}
			
			if (pRewardNode->Attribute("endrank",&iVal)){
				info.m_endRank =  iVal;
			}
			
			if (pRewardNode->Attribute("rewarditem",&iVal)){
				info.m_itemID =  iVal;
			}
			m_rewardVec.push_back(info);
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("世界boss奖励:");
		for (UINT16 i = 0;i < m_rewardVec.size();++i){
			stRewardInfo & info	= m_rewardVec[i];
			Athena::logger->trace("startrank:%d,endrank:%d,boxid:%d",info.m_beginRank,info.m_endRank,info.m_itemID);
		}
	}
#endif

	TiXmlElement * pKillrewardNode = pRoot->FirstChildElement("killreward");
	if (pKillrewardNode){
		TiXmlElement * pRewardNode = pKillrewardNode->FirstChildElement("reward");
		if (pRewardNode){
			if (pRewardNode->Attribute("rewarditem",&iVal)){
				m_killReward = iVal;	
			}	
		}
	}
	return true;
}

bool CWorldBossCfg::isInOpenTime(UINT32 curSec)
{
	std::map<UINT32,stActivityOpenInfo>::iterator it = m_openTimes.begin();	

	for (;it != m_openTimes.end();++it){
		stActivityOpenInfo & info = it->second;
		if ((curSec >= info.m_beginTime) && (curSec <= info.m_endTime)){
			return true;	
		}
	}
	return false;
}

CWorldBossCfg::stActivityOpenInfo * CWorldBossCfg::getCurActivityInfo(UINT32 curSec)
{
	std::map<UINT32,stActivityOpenInfo>::iterator it = m_openTimes.begin();	
	for (;it != m_openTimes.end();++it){
		stActivityOpenInfo & info = it->second;
		if ((curSec >= info.m_beginTime) && (curSec < info.m_endTime)){
			return &it->second;	
		}
	}
	return NULL;
}

CWorldBossCfg::stActivityOpenInfo * CWorldBossCfg::getNextActivityInfo(UINT32 curSec)
{
	std::map<UINT32,stActivityOpenInfo>::iterator it = m_openTimes.begin();	

	for (;it != m_openTimes.end();++it){
		stActivityOpenInfo & info = it->second;
		if (curSec <= info.m_beginTime){//第一个小于开始时间，就是要找的
			return &it->second;	
		}
	}
	if (m_openTimes.size() > 0){
		return &(m_openTimes.begin()->second);
	}
	else {
		return NULL;	
	}
}
