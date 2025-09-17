#include "LimitTimePetCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"

LimitTimePetCfg::LimitTimePetCfg()
{
	m_minScore = 0;
}

LimitTimePetCfg::~LimitTimePetCfg()
{}

bool LimitTimePetCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string guildFile = Athena::global["confDir"] + "/limitpet.xml";
	if (!doc.LoadFile(guildFile.c_str())){
		Athena::logger->error("[限时武神配置]打开配置文件%s错误",guildFile.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("conf");
	if (!pRoot){
		Athena::logger->error("[限时武神配置]读取配置文件找不到节点<conf>");
		return false;
	}
	
	INT32 iVal = 0;
	TiXmlElement * pActivityNode = pRoot->FirstChildElement("Activity");
	while (pActivityNode){
		stActivity one;
		if (pActivityNode->Attribute("Activityid",&iVal)){
			one.m_id = iVal;
		}
		
		if (pActivityNode->Attribute("petid",&iVal)){
			one.m_heroID = iVal;
		}
		
		TiXmlElement * pDrawNode = pActivityNode->FirstChildElement("drawid");
		if (pDrawNode){
			TiXmlElement * pEntryNode = pDrawNode->FirstChildElement("entry");
			if (pEntryNode){
				if (pEntryNode->Attribute("limittimedrawid",&iVal)){
					one.m_drawID = iVal;
				}	
			}
		}
		
		TiXmlElement * pPointRewardNode = pActivityNode->FirstChildElement("pointreward");
		if (pPointRewardNode){
			TiXmlElement * pEntryNode = pPointRewardNode->FirstChildElement("entry");
			while (pEntryNode){
				stScoreReward subOne;
				if (pEntryNode->Attribute("id",&iVal)){
					subOne.m_id = iVal;
				}
				if (pEntryNode->Attribute("pointnum",&iVal)){
					subOne.m_score = iVal;
				}
				if (pEntryNode->Attribute("boxid",&iVal)){
					subOne.m_boxID = iVal;
				}
				one.m_scoreRewards.insert(std::make_pair(subOne.m_id,subOne));
				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}
		}
		
		TiXmlElement *  pRankRewardNode = pActivityNode->FirstChildElement("rankreward");
		if (pRankRewardNode){
			TiXmlElement * pEntryNode = pRankRewardNode->FirstChildElement("entry");
			while (pEntryNode){
				stRankReward subOne;
				if (pEntryNode->Attribute("Startrank",&iVal)){
					subOne.m_beginRank = iVal;
				}
				
				if (pEntryNode->Attribute("EndRank",&iVal)){
					subOne.m_endRank = iVal;
				}
				
				if (pEntryNode->Attribute("boxid",&iVal)){
					subOne.m_boxID = iVal;
				}
				one.m_rankRewards.push_back(subOne);
				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}
		}
		m_allActivitys.insert(std::make_pair(one.m_id,one));
		pActivityNode = pActivityNode->NextSiblingElement("Activity"); 
	}
	
	const char * szVal = NULL;
	TiXmlElement * pActivityOnNode = pRoot->FirstChildElement("ActivityOn");
	if (pActivityOnNode){
		TiXmlElement * pEntryNode = pActivityOnNode->FirstChildElement("entry");
		while (pEntryNode){
			stActivityOpenInfo one;
			if (pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}
			if (pEntryNode->Attribute("Activityid",&iVal)){
				one.m_activityID = iVal;
			}
			if ((szVal = pEntryNode->Attribute("Starttime"))){
				struct tm _tm;
				if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
					 one.m_beginTime = mktime(&_tm) ;
				}   
			}
			if ((szVal = pEntryNode->Attribute("Endtime"))){
				struct tm _tm;
				if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
					one.m_endTime = mktime(&_tm) ;
				} 
			}
			m_openInfos.insert(std::make_pair(one.m_id,one));
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

	TiXmlElement * pLimitNode = pRoot->FirstChildElement("limit");
	if(pLimitNode){
		TiXmlElement * pEntryNode = pLimitNode->FirstChildElement("entry");

		if(pEntryNode){
			if (pEntryNode->Attribute("limitpoint",&iVal)){
				m_minScore = iVal;
			}
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印限时武神活动内容:");
	{
		std::map<UINT32,stActivity>::iterator it = m_allActivitys.begin();
		for (;it != m_allActivitys.end();++it){
			stActivity & one = it->second;
			Athena::logger->trace("id:%d,heroID:%d,drawID:%d",one.m_id,one.m_heroID,one.m_drawID);

			Athena::logger->trace("积分奖励:");
			std::map<UINT16,stScoreReward>::iterator subIt = one.m_scoreRewards.begin();
			for (;subIt != one.m_scoreRewards.end();++subIt){
				stScoreReward & subOne = subIt->second;
				Athena::logger->trace("id=%d,boxID:%d,score:%d",subOne.m_id,subOne.m_boxID,subOne.m_score);	
			}

			Athena::logger->trace("排名奖励:");
			for (UINT16 i = 0;i < one.m_rankRewards.size();++i){
				stRankReward & rankReward = one.m_rankRewards[i];
				Athena::logger->trace("beginRank:%d,endRank:%d,boxID:%d",rankReward.m_beginRank,rankReward.m_endRank,rankReward.m_boxID);
			}	
		}
	}

	Athena::logger->trace("打印限时武神开放时间:");
	{
		std::map<UINT32,stActivityOpenInfo>::iterator it = m_openInfos.begin();
		for (;it != m_openInfos.end();++it){
			stActivityOpenInfo & openInfo = it->second;
			Athena::logger->trace("id:%d,activityID:%d,beginTime:%d,endTime:%d",openInfo.m_id,openInfo.m_activityID,openInfo.m_beginTime,openInfo.m_endTime);
		}
	}

	Athena::logger->trace("打印限时武神最低积分:%d",m_minScore);
#endif
	return true;
}

LimitTimePetCfg::stActivityOpenInfo * LimitTimePetCfg::getOpenInfo(UINT32 cur)
{
	std::map<UINT32,stActivityOpenInfo>::iterator it = m_openInfos.begin();	
	for (;it != m_openInfos.end();++it){
		stActivityOpenInfo & one = it->second;
		if ((cur >= one.m_beginTime) && (cur <= one.m_endTime)){
			return &it->second;	
		}
	}
	return NULL;
}

UINT32 LimitTimePetCfg::getHeroID(UINT32 id)
{
	std::map<UINT32,stActivity>::iterator it = m_allActivitys.find(id);	
	if (it != m_allActivitys.end()){
		return it->second.m_heroID;
	}
	return 0;
}

LimitTimePetCfg::stScoreReward * LimitTimePetCfg::getScoreRewardInfo(UINT32 activityID,UINT16 scoreID)
{
	std::map<UINT32,stActivity>::iterator it = m_allActivitys.find(activityID);
	if (it != m_allActivitys.end()){
		std::map<UINT16,stScoreReward>::iterator subIt = it->second.m_scoreRewards.find(scoreID);
		if (subIt != it->second.m_scoreRewards.end()){
			return &subIt->second;	
		}
	}
	return NULL;
}

UINT32 LimitTimePetCfg::getBoxIDByRank(UINT32 activityID,UINT16 rank)
{
	std::map<UINT32,stActivity>::iterator it = m_allActivitys.find(activityID);
	if (it != m_allActivitys.end()){
		for (UINT16 i = 0;i < it->second.m_rankRewards.size();++i){
			stRankReward & rankReward = it->second.m_rankRewards[i];
			if ((rank >= rankReward.m_beginRank) && (rank <= rankReward.m_endRank)){
				return rankReward.m_boxID;
			}
		}
	}
	return 0;
}

LimitTimePetCfg::stActivity * LimitTimePetCfg::getActivity(UINT32 activityID)
{
	std::map<UINT32,stActivity>::iterator it = m_allActivitys.find(activityID);
	if (it != m_allActivitys.end()){
		return &it->second;
	}
	return NULL;
}

void LimitTimePetCfg::setOpenInfo(std::map<UINT32, stActivityOpenInfo> & petMap) {
    m_openInfos = petMap;
}

void LimitTimePetCfg::getOpenInfo(std::map<UINT32, stActivityOpenInfo> & petMap) {
    petMap = m_openInfos;
}
