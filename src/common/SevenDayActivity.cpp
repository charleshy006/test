#include "SevenDayActivity.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"

bool SevenDayActivityCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/carnival.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[7天活动配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("conf");
	if (!pRoot){
		Athena::logger->error("[7天活动配置]读取配置文件找不到节点<conf>");
		return false;
	}
	
	INT32 iVal = 0;
	TiXmlElement * pDaysNode = pRoot->FirstChildElement("days");
	while (pDaysNode){
		stSDActivityInfo day;
		if (pDaysNode->Attribute("id",&iVal)){
			day.m_day = iVal;	
		}
		TiXmlElement * pActivityNode = pDaysNode->FirstChildElement("activity");
		while (pActivityNode){
			stSDGoalTypeInfo goalType;
			if (pActivityNode->Attribute("activityType",&iVal)){
				goalType.m_goalType = iVal;
			}

			TiXmlElement *pEntryNode = pActivityNode->FirstChildElement("entry");
			while (pEntryNode){
				stSDGoalInfo goalInfo;
				if (pEntryNode->Attribute("id",&iVal)){
					goalInfo.m_id = iVal;	
				}
				
				if (pEntryNode->Attribute("activityCondition",&iVal)){
					goalInfo.m_cond = iVal;
				}
				
				if (pEntryNode->Attribute("conditionValue",&iVal)){
					goalInfo.m_value = iVal;
				}
				
				if (pEntryNode->Attribute("rewardBoxid",&iVal)){
					goalInfo.m_boxID = iVal;
				}
				
				if (pEntryNode->Attribute("extraTarget",&iVal)){
					goalInfo.m_extraVal = iVal;
				}

				goalType.m_goals[goalInfo.m_id] = goalInfo;
				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}
			day.m_goalTypes[goalType.m_goalType] = goalType;
			pActivityNode = pActivityNode->NextSiblingElement("activity");
		}
		m_7Days[day.m_day] = day;
		pDaysNode = pDaysNode->NextSiblingElement("days");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("7天活动奖励信息:");
	std::map<UINT8,stSDActivityInfo>::iterator it = m_7Days.begin();
	for (;it != m_7Days.end();++it){
		stSDActivityInfo & info = it->second;
		Athena::logger->trace("day:%d,活动类型有",it->first);
		
		std::map<UINT8,stSDGoalTypeInfo>::iterator subIt = info.m_goalTypes.begin();
		for (;subIt != info.m_goalTypes.end();++subIt){
			stSDGoalTypeInfo & goalType = subIt->second;
			Athena::logger->trace("目标类型:%d的目标有:",goalType.m_goalType);
			
			std::map<UINT16,stSDGoalInfo>::iterator thdIt = goalType.m_goals.begin();
			for (;thdIt != goalType.m_goals.end();++thdIt){
				stSDGoalInfo & goalInfo = thdIt->second;
				Athena::logger->trace("目标:id=%d,cond=%d,value=%d,boxID=%d",goalInfo.m_id,goalInfo.m_cond,goalInfo.m_value,goalInfo.m_boxID);
			}
		}
	}
#endif
	return true;
}

stSDActivityInfo * SevenDayActivityCfg::getDayInfo(UINT8 day)
{
	std::map<UINT8,stSDActivityInfo>::iterator it = m_7Days.find(day);

	if (it != m_7Days.end()){
		return &it->second;
	}
	return NULL;
}

stSDGoalInfo * SevenDayActivityCfg::getGoalInfo(UINT8 day,UINT8 goalType,UINT16 id)
{
	std::map<UINT8,stSDActivityInfo>::iterator it = m_7Days.find(day);
	if (it == m_7Days.end()){
		return NULL;
	}

	std::map<UINT8,stSDGoalTypeInfo>::iterator subIt = it->second.m_goalTypes.find(goalType);
	if (subIt == it->second.m_goalTypes.end()){
		return NULL;
	}

	std::map<UINT16,stSDGoalInfo>::iterator thdIt = subIt->second.m_goals.find(id);
	if (thdIt == subIt->second.m_goals.end()){
		return NULL;
	}
	
	return &thdIt->second;
}

stSDGoalInfo * SevenDayActivityCfg::getGoalInfoByGoalID(UINT8 day,UINT8 goalType,UINT16 goalID)
{
	std::map<UINT8,stSDActivityInfo>::iterator it = m_7Days.find(day);
	if (it == m_7Days.end()){
		return NULL;
	}

	std::map<UINT8,stSDGoalTypeInfo>::iterator subIt = it->second.m_goalTypes.find(goalType);
	if (subIt == it->second.m_goalTypes.end()){
		return NULL;
	}

	std::map<UINT16,stSDGoalInfo>::iterator thdIt = subIt->second.m_goals.begin();
	for (;thdIt != subIt->second.m_goals.end();++thdIt){
		stSDGoalInfo & goalInfo = thdIt->second;
		
		if (goalInfo.m_cond == goalID){
			return &thdIt->second;	
		}
	}
	return NULL;
}

stSDGoalInfo * SevenDayActivityCfg::getGoalInfoByGoalIDAndExtraVal(UINT8 day,UINT8 goalType,UINT16 goalID,UINT16 extralVal)
{
	std::map<UINT8,stSDActivityInfo>::iterator it = m_7Days.find(day);
	if (it == m_7Days.end()){
		return NULL;
	}

	std::map<UINT8,stSDGoalTypeInfo>::iterator subIt = it->second.m_goalTypes.find(goalType);
	if (subIt == it->second.m_goalTypes.end()){
		return NULL;
	}

	std::map<UINT16,stSDGoalInfo>::iterator thdIt = subIt->second.m_goals.begin();
	for (;thdIt != subIt->second.m_goals.end();++thdIt){
		stSDGoalInfo & goalInfo = thdIt->second;
		
		if ((goalInfo.m_cond == goalID) && (extralVal == goalInfo.m_extraVal)){
			return &thdIt->second;	
		}
	}
	return NULL;
}

