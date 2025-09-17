#include "LevelWflfareMgr.h"
#include "AthenaNew.h"      
#include "Log4cxxHelp.h"    
#include "KVPair.h"         
#include "UtilFunction.h"   
#include "GameServer.h"     
#include "GameUser.h"       
#include "TblDataMgr.h"
#include "Misc.pb.h"
#include "LevelWflfareCfg.h"

extern GameServer  * g_gameServer;

CLevelWflfareMgr::CLevelWflfareMgr(GameUser *pUser): pOwner(pUser)
{
}

CLevelWflfareMgr::~CLevelWflfareMgr()
{
}

void CLevelWflfareMgr::loadTimeData(const char * buf)
{
	if (buf && *buf != '\0') {
		std::vector<std::string> strVec;
		g_stringtok(strVec, buf, ";");
		if(strVec.size() >= 2){	
			UINT32 openTime = (UINT32)atoi(strVec[0].c_str());
			m_activityTime.m_openTime = openTime;
			UINT32 endTime  = (UINT32)atoi(strVec[1].c_str());
			m_activityTime.m_endTime = endTime;
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("load CLevelWflfareMgr openTime:%d, endTime:%d",m_activityTime.m_openTime, m_activityTime.m_endTime);
#endif
}

std::string CLevelWflfareMgr::saveTimeData()
{
	std::stringstream ss;
	if (!m_activityTime.m_openTime || !m_activityTime.m_endTime) {
		return ss.str();
	}

	ss<<m_activityTime.m_openTime<<";"<<m_activityTime.m_endTime;

	Athena::logger->trace("save CLevelWflfareMgr time  tostring size is %d, data is %s", ss.str().length(), ss.str().c_str());

	return ss.str();
}

void CLevelWflfareMgr::loadData(const char * buf)
{                                                      
	if(buf && *buf != '\0'){
		std::vector<std::string> strVec;
		g_stringtok(strVec, buf, ";");
		for(UINT16 i = 0; i < strVec.size(); ++i){
			std::vector<std::string> strSubVec;
			g_stringtok(strSubVec, strVec[i].c_str(), "-");
			if(strSubVec.size() >= 2){
				UINT16	level = (UINT16)atoi(strSubVec[0].c_str());
				UINT16	type  = (UINT16)atoi(strSubVec[1].c_str());
				addInfoToMap(level, type);	
			}
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("load CLevelWflfareMgr data:");
		std::map<UINT16, std::map<UINT16,UINT16> >::iterator it = m_hasGotReward.begin();
		for(;it != m_hasGotReward.end(); ++it){
			std::map<UINT16,UINT16> & tmpMap = it->second;
			std::map<UINT16,UINT16>::iterator subIt = tmpMap.begin();
			for(;subIt != tmpMap.end(); ++subIt){
				Athena::logger->trace("level=%d,type=%d",it->first, subIt->first);
			}
		}
#endif
	}
}                                                      

std::string CLevelWflfareMgr::saveData()      
{                                                      
	std::stringstream ss;
	if (m_hasGotReward.empty()) {
		return ss.str();
	}

	std::map<UINT16, std::map<UINT16,UINT16> >::iterator it = m_hasGotReward.begin();
	bool first = true;
	for(;it != m_hasGotReward.end(); ++it){
		std::map<UINT16,UINT16> & tmpMap = it->second;
		std::map<UINT16,UINT16>::iterator subIt = tmpMap.begin();
		for(;subIt != tmpMap.end(); ++subIt){
			if(first){
				ss<<it->first<<"-"<<subIt->first;
				first = false;
			}else{
				ss<<";"<<it->first<<"-"<<subIt->first;
			}
		}
	}

	Athena::logger->trace("save CLevelWflfareMgr data  tostring size is %d, data is %s", ss.str().length(), ss.str().c_str());
	return ss.str();
}


void CLevelWflfareMgr::sendHasGotReward()
{
	MiscProto::stHasGotRewardWflfareInfo retCmd;

	if(isOpen()){
		std::map<UINT16, std::map<UINT16,UINT16> >::iterator it = m_hasGotReward.begin();
		for(;it != m_hasGotReward.end(); ++it){
			std::map<UINT16,UINT16> & subMap = it->second;
			std::map<UINT16,UINT16>::iterator subIt = subMap.begin();
			for(;subIt != subMap.end(); ++subIt){
				MiscProto::stLevelWflfareInfo* cmd = retCmd.add_info();
				if(cmd){
					cmd->set_level(it->first);
					cmd->set_type(subIt->first);
				}
			}
		}

		bool isOpen = false;

		UINT16 total = 0;
		std::map<UINT16, std::map<UINT16,UINT16> >::iterator hIt = m_hasGotReward.begin();
		for(;hIt != m_hasGotReward.end(); ++hIt){\
		 	isOpen += hIt->second.size();
		}

		UINT16 size = CLevelWflfareCfg::getSingleton().getWflfareSize();
		if(total == size){
			isOpen = false;
		}else{
			isOpen = true;
		}

		if(isOpen){
			retCmd.set_isopen(true);
		}else{
			retCmd.set_isopen(false);
		}
	}else{
		retCmd.set_isopen(false);
	}
	pOwner->sendProto2Me(retCmd);
}

void CLevelWflfareMgr::addInfoToMap(UINT16 level, UINT16 type)
{
	std::map<UINT16, std::map<UINT16,UINT16> >::iterator it = m_hasGotReward.find(level);
	if(it == m_hasGotReward.end()){
		std::map<UINT16,UINT16> tmpMap;
		tmpMap.insert(std::make_pair(type,type));
		m_hasGotReward.insert(std::make_pair(level, tmpMap));
	}else{
		std::map<UINT16,UINT16> & tmpMap = it->second;
		tmpMap.insert(std::make_pair(type,type));
	}
}

void CLevelWflfareMgr::addOpenTime()
{
	UINT32 openTime = g_gameServer->getSec();
	UINT32 endTime = openTime + 7 * 86400;//7天后
	m_activityTime.m_openTime = openTime;
	m_activityTime.m_endTime = endTime;
}

bool CLevelWflfareMgr::isOpen()
{

	UINT32 openTime = g_gameServer->getSec();
	if(openTime>= m_activityTime.m_openTime && openTime <= m_activityTime.m_endTime){
		return true;
	}
	return false;
}

bool CLevelWflfareMgr::checkHasGotReward(UINT16 level, UINT16 type)
{
	std::map<UINT16, std::map<UINT16,UINT16> >::iterator it = m_hasGotReward.find(level);
	if(it == m_hasGotReward.end()){
		return false;
	}
	std::map<UINT16,UINT16> & tmpMap = it->second;
	std::map<UINT16,UINT16>::iterator subIt = tmpMap.find(type);
	if(subIt == tmpMap.end()){
		return false;
	}
	return true;
}
