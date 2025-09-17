#include "HeroLeagueCfg.h"
#include "KVPair.h"      
#include "Log4cxxHelp.h" 
#include <string>        
#include "UtilFunction.h"
#include <sstream>

CHeroLeagueCfg::CHeroLeagueCfg()
{

}

bool CHeroLeagueCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string heroLeagueFile = Athena::global["confDir"] + "/HeroLeague.xml";
	if (!doc.LoadFile(heroLeagueFile.c_str())){
		Athena::logger->error("[英雄联赛配置]打开配置文件%s错误",heroLeagueFile.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[英雄联赛配置]读取配置文件找不到节点<root>");
		return false;
	}

	INT32 iVal = 0;
	const char* szVal = NULL;
	
	TiXmlElement * pPreliminaryNode = pRoot->FirstChildElement("Preliminary");
	if(pPreliminaryNode){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[英雄联赛配置]加载<Preliminary>节点!");
#endif
		//stPreliminary one;
		if((szVal = pPreliminaryNode->Attribute("opentime"))){
			std::vector<std::string> strVec;
			g_stringtok(strVec, szVal, "-");

			if(strVec.size() >= 2){
				m_preliminary.m_openTime = (UINT32)atoi(strVec[0].c_str());
				m_preliminary.m_endTime =  (UINT32)atoi(strVec[1].c_str());
			}
		}

		if(pPreliminaryNode->Attribute("Clearing",&iVal)){
			m_preliminary.m_clearing = iVal;
		}

		if(pPreliminaryNode->Attribute("Levelid",&iVal)){
			m_preliminary.m_levelID = iVal;
		}

		if(pPreliminaryNode->Attribute("Limitnum",&iVal)){
			m_preliminary.m_limitnum = iVal;
		}

		if(pPreliminaryNode->Attribute("VictoryPoints",&iVal)){
			m_preliminary.m_victoryPoints = iVal;
		}

		if(pPreliminaryNode->Attribute("participationBOX",&iVal)){
			m_preliminary.m_participationBOX = iVal;
		}
		
		if(pPreliminaryNode->Attribute("victoryBOX",&iVal)){
			m_preliminary.m_victoryBOX = iVal;
		}
	}
	
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("opentime:%d-%d, Clearing:%d, Levelid:%d, Limitnum:%d, VictoryPoints:%d, participationBOX:%d, victoryBOX:%d", m_preliminary.m_openTime, m_preliminary.m_endTime, m_preliminary.m_clearing, m_preliminary.m_levelID, m_preliminary.m_limitnum, m_preliminary.m_victoryPoints, m_preliminary.m_participationBOX, m_preliminary.m_victoryBOX);
	}
#endif

	TiXmlElement * pItdNode = pRoot->FirstChildElement("intermediary");
	if(pItdNode){
#ifdef _HDZ_DEBUG                                                 
		Athena::logger->trace("[英雄联赛配置]加载<intermediary>节点!"); 
#endif                                                            
		if((szVal = pItdNode->Attribute("opentime"))){
			std::vector<std::string> strVec;
			g_stringtok(strVec, szVal, "-");
			if(strVec.size() >= 2){
				m_intermediary.m_openTime =(UINT32)atoi(strVec[0].c_str());
				m_intermediary.m_endTime = (UINT32)atoi(strVec[1].c_str());  
			}
		}

		if(pItdNode->Attribute("Clearing",&iVal)){
			m_intermediary.m_clearing = iVal;
		}

		if(pItdNode->Attribute("Levelid",&iVal)){ 
			m_intermediary.m_levelID = iVal;
		}

		if(pItdNode->Attribute("Limitnum",&iVal)){      
			m_intermediary.m_limitnum = iVal;                    
		}                                                       

		if(pPreliminaryNode->Attribute("VictoryPoints",&iVal)){ 
			m_intermediary.m_victoryPoints = iVal;               
		}                                                       

		TiXmlElement *pEntryNode = pItdNode->FirstChildElement("entry");
		while(pEntryNode){
			stSemiElement one;
			if(pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}
			
			if(pEntryNode->Attribute("Startrank",&iVal)){
				one.m_startrank = iVal;
			}

			if(pEntryNode->Attribute("Endrank",&iVal)){
				one.m_endrank = iVal;
			}

			if(pEntryNode->Attribute("rewardid",&iVal)){
				one.m_rewardid = iVal;
			}

			m_intermediary.m_semiConfig[one.m_id] = one;

			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("openTime:%d-%d, Clearing:%d, Levelid:%d, Limitnum:%d, VictoryPoints:%d", m_intermediary.m_openTime,m_intermediary.m_endTime, m_intermediary.m_clearing, m_intermediary.m_levelID,m_intermediary.m_limitnum, m_intermediary.m_victoryPoints);
	
		std::map<UINT8, stSemiElement>::iterator it = m_intermediary.m_semiConfig.begin();
		for(; it != m_intermediary.m_semiConfig.end(); ++it){
			stSemiElement & one = it->second;
			Athena::logger->trace("    id:%d, Startrank:%d, Endrank:%d, rewardid:%d", it->first, one.m_startrank, one.m_endrank, one.m_rewardid);
		}
	}
#endif


	TiXmlElement * pFinalsNode = pRoot->FirstChildElement("Finals");
	if(pFinalsNode){
#ifdef _HDZ_DEBUG                                                 
		Athena::logger->trace("[英雄联赛配置]加载<Finals>节点!"); 
#endif                                                            
		if(pFinalsNode->Attribute("Applytime",&iVal)){
			m_finals.m_applyTime = iVal;
		}

		if(pFinalsNode->Attribute("Clearing",&iVal)){
			m_finals.m_clearing = iVal;
		}

		if(pFinalsNode->Attribute("Levelid",&iVal)){ 
			m_finals.m_levelid = iVal;
		}


		TiXmlElement *pEntryNode = pFinalsNode->FirstChildElement("entry");
		while(pEntryNode){
			stFinalElement one;
			if(pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}

			if(pEntryNode->Attribute("Screening",&iVal)){ 
				one.m_screening = iVal;
			}

			if((szVal = pEntryNode->Attribute("Battletime"))){
				std::vector<std::string> strVec; 
				g_stringtok(strVec, szVal, "-"); 

				if(strVec.size() >= 2){
					one.m_beginBattletime = (UINT32)atoi(strVec[0].c_str());
					one.m_endBattletime = (UINT32)atoi(strVec[1].c_str());
				}
			}

			if((szVal = pEntryNode->Attribute("Resttime"))){ 
				std::vector<std::string> strVec;
				g_stringtok(strVec, szVal, "-");              
						                                                  
				if(strVec.size() >= 2){                       
					one.m_beginResttime = (UINT32)atoi(strVec[0].c_str());
					one.m_endResttime = (UINT32)atoi(strVec[1].c_str());
				}
			}

			if(pEntryNode->Attribute("participationBOX",&iVal)){
				one.m_participationBOX = iVal;
			}

			if(pEntryNode->Attribute("victoryBOX",&iVal)){
				one.m_victoryBOX = iVal;
			}

			m_finals.m_finalConfig[one.m_id] = one;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("Applytime:%d, Clearing:%d, Levelid:%d", m_finals.m_applyTime, m_finals.m_clearing, m_finals.m_levelid);
	
		std::map<UINT8, stFinalElement>::iterator it = m_finals.m_finalConfig.begin();
		for(;it != m_finals.m_finalConfig.end(); ++it){
			stFinalElement & one = it->second;
			Athena::logger->trace("    id:%d, Screening:%d, Battletime:%d-%d, Resttime:%d-%d, participationBOX:%d ,victoryBOX:%d", it->first, one.m_screening, one.m_beginBattletime, one.m_endBattletime, one.m_beginResttime, one.m_endResttime, one.m_participationBOX, one.m_victoryBOX);
		}
	}
#endif

	TiXmlElement * pWorshipNode = pRoot->FirstChildElement("worship");
	if(pWorshipNode){
#ifdef _HDZ_DEBUG                                                
		Athena::logger->trace("[英雄联赛配置]加载<worship>节点!");
#endif                                                           
		if(pWorshipNode->Attribute("id",&iVal)){
			m_worship.m_id = iVal;
		}

		if(pWorshipNode->Attribute("differentitem",&iVal)){
			m_worship.m_differentitem = iVal;
		}

		if(pWorshipNode->Attribute("denticalitem",&iVal)){
			m_worship.m_denticalitem = iVal;
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("id:%d, differentitem:%d, denticalitem:%d", m_worship.m_id, m_worship.m_differentitem, m_worship.m_denticalitem);
#endif

	return true;
}

bool CHeroLeagueCfg::isPreOpen(UINT32 sec)
{
	if((sec >= m_preliminary.m_openTime) && (sec <= m_preliminary.m_endTime)){
		return true;
	}
	return false;
}

bool CHeroLeagueCfg::isRemOpen(UINT32 sec)
{
	if(sec >= m_intermediary.m_openTime && sec <= m_intermediary.m_endTime){
		return true;
	}
	return false;
}

bool CHeroLeagueCfg::isFinalOpen(UINT32 sec)
{
	if(sec >= m_finals.m_applyTime && sec <= m_finals.m_applyTime + 600){
		return true;
	}
	return false;
}

bool CHeroLeagueCfg::isFinalFightOpen(UINT32 sec)
{
	std::map<UINT8, stFinalElement>::iterator it = m_finals.m_finalConfig.begin(); 
	for(;it != m_finals.m_finalConfig.end(); ++it){ 
		stFinalElement &one = it->second;
		if(sec >= one.m_beginBattletime && sec <= one.m_endBattletime){
			return true;
		}
	}
	return false;
}

UINT32 CHeroLeagueCfg::getRewardid(UINT16 sortRank)
{
	std::map<UINT8, stSemiElement>::iterator it = m_intermediary.m_semiConfig.begin();
	for(;it != m_intermediary.m_semiConfig.end(); ++it){
		stSemiElement & info = it->second;
		if(sortRank >= info.m_startrank && sortRank <= info.m_endrank){
			return info.m_rewardid;
		}
	}
	return (UINT32)-1;
}

stFinalElement * CHeroLeagueCfg::getFinalsSessionInfo(UINT8 id)
{
	std::map<UINT8, stFinalElement>::iterator it = m_finals.m_finalConfig.find(id);
	if(it != m_finals.m_finalConfig.end()){
		return &it->second;
	}
	return NULL;
}

bool CHeroLeagueCfg::isFinalBeginBattleTime(UINT32 sec)
{
	std::map<UINT8, stFinalElement>::iterator it = m_finals.m_finalConfig.begin();
	for(;it != m_finals.m_finalConfig.end();++it){
		stFinalElement & element = it->second;
		if(sec == element.m_beginBattletime){
			return true;
		}
	}
	return false;
}

bool CHeroLeagueCfg::isFinalBeginRestTime(UINT32 sec)
{
	std::map<UINT8, stFinalElement>::iterator it = m_finals.m_finalConfig.begin();
	for(;it != m_finals.m_finalConfig.end();++it){                                
		stFinalElement & element = it->second;                                    
		if(sec == element.m_beginResttime){                                     
			return true;                                                          
		}                                                                         
	}                                                                             
	return false;                                                                 
}

bool CHeroLeagueCfg::finalMiddleSettlement(UINT32 sec)
{
	stFinalElement * pInfo1 = getFinalsSessionInfo(2); 
	stFinalElement * pInfo2 = getFinalsSessionInfo(3);  
	stFinalElement * pInfo3 = getFinalsSessionInfo(4);
	if(!pInfo1 || !pInfo2 || !pInfo3){
		return false;
	}


	if((sec == pInfo1->m_beginResttime) || (sec == pInfo2->m_beginResttime) || (sec == pInfo3->m_beginResttime)){
		return true;
	}

	return false;
}
