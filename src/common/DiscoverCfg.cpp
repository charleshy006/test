#include "DiscoverCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include <tinyxml/tinyxml.h>

CDiscoverCfg::CDiscoverCfg()
{
	m_freeNum = 0;
}

CDiscoverCfg::~CDiscoverCfg()
{}

bool CDiscoverCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/discover.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[秘境寻宝配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if(!pRoot){
		Athena::logger->error("[秘境寻宝配置]配置加载错误，找不到root节点!"); 
		return false;                                                               
	}

	INT32 iVal = 0;
	const char * szVal = NULL;

	TiXmlElement * pDiscoverNode = pRoot->FirstChildElement("Discover");
	if(pDiscoverNode){
		if(pDiscoverNode->Attribute("discoverNum",&iVal)){
			m_freeNum = iVal;
		}

		TiXmlElement * pEntryNode = pDiscoverNode->FirstChildElement("entry");
		while(pEntryNode){
			//stResetNum one;
			UINT32 resetNum = 0;
			UINT32 resetPrice = 0;
			if(pEntryNode->Attribute("resetNum",&iVal)){
				resetNum = iVal;
			}

			if(pEntryNode->Attribute("resetPrice",&iVal)){
				resetPrice = iVal;
			}

			m_buyNum.insert(std::make_pair(resetNum, resetPrice));

			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}	
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[秘境寻宝配置]免费次数：%d", m_freeNum);

	Athena::logger->trace("[秘境寻宝配置]加载Discover节点!"); 
	{
		std::map<UINT32, UINT32>::iterator it =  m_buyNum.begin();
		for(;it != m_buyNum.end(); ++it){
			Athena::logger->trace("resetNum:%d, resetPrice:%d",it->first, it->second);
		}
	}
#endif

	TiXmlElement * pDiscoverBoundsNode = pRoot->FirstChildElement("discoverBounds");
	if(pDiscoverBoundsNode){
		TiXmlElement * pEntryNode = pDiscoverBoundsNode->FirstChildElement("entry");
		while(pEntryNode){
			stDiscoverBounds one;
			if(pEntryNode->Attribute("id", &iVal)){
				one.m_id = iVal;
			}

			if(pEntryNode->Attribute("bounds", &iVal)){
				one.m_bounds = iVal;
			}

			if(pEntryNode->Attribute("boundsBox", &iVal)){
				one.m_boundsBox = iVal;
			}

			m_boundsBox.insert(std::make_pair(one.m_id, one));

			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[秘境寻宝配置]加载discoverBounds节点!");
	{
		std::map<UINT8, stDiscoverBounds>::iterator it = m_boundsBox.begin();
		for(;it != m_boundsBox.end(); ++it){
			stDiscoverBounds & one = it->second;
			Athena::logger->trace("id:%d, bounds:%d, boundsBox:%d", it->first, one.m_bounds, one.m_boundsBox);
		}
	}
#endif

	TiXmlElement * pDiscoverMapNode = pRoot->FirstChildElement("discoverMap");
	if(pDiscoverMapNode){
		TiXmlElement * pEntryNode = pDiscoverMapNode->FirstChildElement("entry");
		while(pEntryNode){
			stDiscoverMap one;
			if(pEntryNode->Attribute("id", &iVal)){
				one.m_id = iVal;
			}

			if(pEntryNode->Attribute("mapID", &iVal)){
				one.m_mapID = iVal;
			}

			if(pEntryNode->Attribute("mapRP", &iVal)){
				one.m_mapRP = iVal;
			}

			m_map.insert(std::make_pair(one.m_id, one));

			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[秘境寻宝配置]加载discoverMap节点!");
	{
		std::map<UINT8, stDiscoverMap>::iterator it = m_map.begin();
		for(;it != m_map.end(); ++it){
			stDiscoverMap & one = it->second;
			Athena::logger->trace("id:%d, mapID:%d, mapRP:%d",it->first, one.m_mapID, one.m_mapRP);
		}
	}
#endif

	TiXmlElement * pMapNode = pRoot->FirstChildElement("Map");
	if(pMapNode){
		while(pMapNode){
			UINT8 mapId			=	 0;
			UINT8 squareID		=	 0;
			UINT8 squareType	=	 0;
			UINT8 levelID		=	 0;

			std::pair<UINT8, UINT8> squareInfo;
			std::map<UINT8, std::pair<UINT8, UINT8> > squareTypeMap;

			if(pMapNode->Attribute("id",&iVal)){
				mapId = iVal;
			}

			TiXmlElement * pEntryNode = pMapNode->FirstChildElement("square");
			while(pEntryNode){
				if(pEntryNode->Attribute("ID", &iVal)){
					squareID = iVal;
				}

				if((szVal = pEntryNode->Attribute("squareType"))){
					std::vector<std::string> strVec;
					g_stringtok(strVec, szVal, "-");

					squareType = (UINT8)atoi(strVec[0].c_str());
					levelID = 0;
                    if (strVec.size() > 1) {
                        levelID = (UINT8)atoi(strVec[1].c_str());
                    }
				}

				squareInfo = std::make_pair(squareType, levelID);
				squareTypeMap.insert(std::make_pair(squareID, squareInfo));
				pEntryNode = pEntryNode->NextSiblingElement("square");
			}

			m_square.insert(std::make_pair(mapId, squareTypeMap));

			pMapNode = pMapNode->NextSiblingElement("Map"); 
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[秘境寻宝配置]加载Map节点!");
	{
		std::map<UINT8, std::map<UINT8, std::pair<UINT8, UINT8> > >::iterator it = m_square.begin();
		for(;it != m_square.end(); ++it){
			Athena::logger->trace("Map id:%d",it->first);
			std::map<UINT8, std::pair<UINT8, UINT8> >::iterator iit = it->second.begin();
			for(;iit != it->second.end(); ++iit){
				Athena::logger->trace("id:%d, squareType:%d,levelId:%d",iit->first, iit->second.first,iit->second.second);
			}
		}
	}
#endif
	TiXmlElement * pSquareTypeNode = pRoot->FirstChildElement("SquareType");
	while(pSquareTypeNode){
		UINT8 key = 0;
		std::map<UINT8, stSquareType> typeMap;

		if(pSquareTypeNode->Attribute("id", &iVal)){
			key = iVal;
		}

		TiXmlElement * pEntryNode = pSquareTypeNode->FirstChildElement("entry");
		while(pEntryNode){
			stSquareType one;
			if(pEntryNode->Attribute("ID", &iVal)){
				one.m_id = iVal;
			}

			if(pEntryNode->Attribute("rewardBox", &iVal)){ 
				one.m_rewardBox = iVal;
			}

			if(pEntryNode->Attribute("rewardBounds", &iVal)){ 
				one.m_rewardBounds = iVal;
			}

			if(pEntryNode->Attribute("battleRP", &iVal)){
				one.m_battleRP = iVal;
			}

			if(pEntryNode->Attribute("battleType", &iVal)){
				one.m_battleType = iVal;
			}

			if(pEntryNode->Attribute("battleNum", &iVal)){
				one.m_battleNum = iVal;
			}
			
			if((szVal = pEntryNode->Attribute("levelID"))){ 
				std::vector<std::string> strVec;
				g_stringtok(strVec, szVal, ";");
				for(UINT16 i = 0; i < strVec.size(); ++i){
					one.m_level.m_levelIDVer.push_back((UINT32)atoi(strVec[i].c_str()));
				}
			}

			if((szVal = pEntryNode->Attribute("playLevelRange"))){
				std::vector<std::string> strVec;
				g_stringtok(strVec, szVal, ";");
				for(UINT16 i = 0; i < strVec.size(); ++i){
					std::vector<std::string> strVec_1;
					g_stringtok(strVec_1, strVec[i], "-");
                    if (strVec_1.size() != 2) {
                        Athena::logger->trace("discover.xml playLevelRange size is not 2");
                        return false;
                    }

					one.m_level.m_playLevelVer.push_back(std::make_pair((UINT16)atoi(strVec_1[0].c_str()), (UINT16)atoi(strVec_1[1].c_str())));
				}
			}
            if (one.m_id == eSQUARE_TYPE_FIGHT_TASK && one.m_level.m_levelIDVer.size() != one.m_level.m_playLevelVer.size()) {
                Athena::logger->trace("levelID size is not equal playLevel");
                return false;
            }
			typeMap.insert(std::make_pair(one.m_id, one));
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
		m_squareType.insert(std::make_pair(key,typeMap));

		pSquareTypeNode = pSquareTypeNode->NextSiblingElement("SquareType");
	}

#ifdef _HDZ_DEBUG
{
	std::map<UINT8, std::map<UINT8, stSquareType> >::iterator it = m_squareType.begin();
	for(;it != m_squareType.end(); ++it){
		Athena::logger->trace("SquareType id :%d", it->first);
		std::map<UINT8, stSquareType>::iterator subIt = it->second.begin(); 
		for(;subIt != it->second.end(); ++subIt){
			stSquareType & one = subIt->second;
			Athena::logger->trace("    ID:%d, rewardBox:%d, rewardBounds:%d, battleRP:%d, battleType:%d, battleNum:%d", one.m_id, one.m_rewardBox, one.m_rewardBounds, one.m_battleRP, one.m_battleType, one.m_battleNum);
		
			for(UINT16 i = 0; i < one.m_level.m_levelIDVer.size(); ++i){
				Athena::logger->trace("      levelID:%d",one.m_level.m_levelIDVer[i]);
			}

			for(UINT16 i = 0; i < one.m_level.m_playLevelVer.size(); ++i){
				Athena::logger->trace("      playLevelRange:%d-%d",one.m_level.m_playLevelVer[i].first, one.m_level.m_playLevelVer[i].second);
			}
		}
	}
}
#endif

	return true;

}

UINT32 CDiscoverCfg::getNeedDiamondNum(UINT32 num)
{
	if(num > m_buyNum.size()){
		num = m_buyNum.size();
	}

	std::map<UINT32, UINT32>::iterator it = m_buyNum.find(num);
	if(it == m_buyNum.end()){
		return 0;
	}
	return it->second;
}

stDiscoverBounds * CDiscoverCfg::getBoundsBoxByid(UINT8 id)
{
	std::map<UINT8, stDiscoverBounds>::iterator it = m_boundsBox.find(id);
	if(it ==  m_boundsBox.end()){
		return NULL;
	}

	return &it->second;
}

UINT32 CDiscoverCfg::randMapID()
{
	std::map<UINT32,UINT32> id2Rates;
	UINT32 totalRate = 0;

	std::map<UINT8, stDiscoverMap>::iterator subIt = m_map.begin();
	for (;subIt != m_map.end();++subIt){
		stDiscoverMap & discoverMap = subIt->second;
		totalRate += discoverMap.m_mapRP;
		id2Rates.insert(std::make_pair(discoverMap.m_id,discoverMap.m_mapRP));
	}

	INT32 id = g_selectByMap(id2Rates,totalRate);

	if (id > 0){
		return id;
	}
	else {
		return 1;
	}
}

UINT8 CDiscoverCfg::getMapSquareNum(UINT8 mapID)
{
	std::map<UINT8, std::map<UINT8, std::pair<UINT8, UINT8> > >::iterator it = m_square.find(mapID);
	if(it == m_square.end()){
		return 0;
	}
	return it->second.size() - 1;//从0开始
}


std::pair<UINT8,UINT8> CDiscoverCfg::getSquareType(UINT8 mapID, UINT8 square)
{
	std::map<UINT8, std::map<UINT8, std::pair<UINT8, UINT8> > >::iterator it = m_square.find(mapID);
	if(it == m_square.end()){
		return std::make_pair(0,0);
	}

	std::map<UINT8, std::pair<UINT8, UINT8> > squareMap = it->second;
	std::map<UINT8, std::pair<UINT8, UINT8> >::iterator squareIt = squareMap.find(square);
	if(squareIt == squareMap.end()){
		return std::make_pair(0,0);
	}
	return squareIt->second;
}


stSquareType * CDiscoverCfg::randSquareTask(UINT8 mapID, UINT8 square)
{
	std::pair<UINT8,UINT8> taskType = getSquareType(mapID, square);
	if(0 == taskType.first)
	{
		Athena::logger->error("[秘境寻宝]随机某个格子的任务错误,类型不存在！mapID:%d, square:%d",mapID,square);
		return NULL;
	}

	std::map<UINT8, std::map<UINT8, stSquareType> >::iterator it =  m_squareType.find(taskType.first);
	if(it == m_squareType.end()){
		Athena::logger->error("[秘境寻宝]随机某个格子的任务错误,找不到此类型的任务,type:%d",taskType.first);
		return NULL;
	}
	std::map<UINT8, stSquareType> typeMap = it->second;

	//boss关卡不随机
	if(taskType.first == eSQUARE_TYPE_BOSS){
		std::map<UINT8, stSquareType>::iterator iit = typeMap.find(taskType.second);
		if(iit == typeMap.end()){
			Athena::logger->error("[秘境寻宝]随机某个格子的任务错误,boss关卡错误squareType:%d,taskID:%d",taskType.first, taskType.second);
			return NULL;
		}

		return &iit->second;
	}

	//只有一个，不需要随机
	if(taskType.first == eSQUARE_TYPE_END 
			|| taskType.first == eSQUARE_TYPE_BLANK){
		if(typeMap.size() > 0){
			return &typeMap[1];
		}
		return NULL;
	}

	std::map<UINT32,UINT32> id2Rates;
	UINT32 totalRate = 0;

	std::map<UINT8, stSquareType>::iterator subIt = typeMap.begin();
	for (;subIt != typeMap.end(); ++subIt){
		stSquareType & squareType = subIt->second;
		totalRate += squareType.m_battleRP;
		id2Rates.insert(std::make_pair(squareType.m_id,squareType.m_battleRP));
	}

	INT32 id = g_selectByMap(id2Rates,totalRate);

	if(-1 == id){
		Athena::logger->error("[秘境寻宝]随机某个格子的任务错误，随机错误");
		return NULL;
	}

	subIt = typeMap.find(id);
	if(subIt == typeMap.end()){
		 Athena::logger->error("[秘境寻宝]随机某个格子的任务错误，任务不存在");
		return NULL;
	}
	return &subIt->second;
}

stSquareType * CDiscoverCfg::getSquareInfo(UINT8 mapID, UINT8 square, UINT8 taskID)
{
	std::pair<UINT8,UINT8> type = getSquareType(mapID,square);
	if(0 == type.first){
		return NULL;
	}

	std::map<UINT8, std::map<UINT8, stSquareType> >::iterator it = m_squareType.find(type.first);
	if(it == m_squareType.end()){
		return NULL;
	}

	std::map<UINT8, stSquareType>::iterator subIt = it->second.find(taskID);
	if(subIt == it->second.end()){
		return NULL;
	}
	return &subIt->second;

}
