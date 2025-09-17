#include "ShopActivity.h"
#include "GameUser.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include <tinyxml/tinyxml.h>


CShopActivityMgr::CShopActivityMgr()
{
	
}

CShopActivityMgr::~CShopActivityMgr()
{

}

bool CShopActivityMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/flashshop.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[限时商店]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("RewardConf");
	if (!pRoot){
		Athena::logger->error("[限时商店]配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	const char * szVal = NULL;
	TiXmlElement * pActivityNode = pRoot->FirstChildElement("activity");
	while (pActivityNode){
		stShopActivity info;

		if (pActivityNode->Attribute("id",&iVal)){
			info.m_id = iVal;		
		}	
		
		if ((szVal = pActivityNode->Attribute("StartTime"))){
			struct tm _tm;
			if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
				info.m_startTime = mktime(&_tm) ;
			}   
			else {
				return false;
			}   
		}	
	
		if ((szVal = pActivityNode->Attribute("EndTime"))){
			struct tm _tm;
			if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
				info.m_endTime = mktime(&_tm) ;
			}   
			else {
				return false;
			}  
		}
		
		if (pActivityNode->Attribute("ShopID",&iVal)){
			info.m_shopID = iVal;
		}

		m_activities[info.m_id] = info;
		pActivityNode = pActivityNode->NextSiblingElement("activity");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了商店活动的配置:");

	std::map<UINT32,stShopActivity>::iterator it = m_activities.begin();

	for (;it != m_activities.end();++it){
		stShopActivity & activity = it->second;
		Athena::logger->trace("活动ID:%d,开始时间:%s,结束时间:%s,shopID:%d",activity.m_id,g_utc2StringDate(activity.m_startTime,"%Y-%m-%d %H:%M:%S").c_str(),g_utc2StringDate(activity.m_endTime,"%Y-%m-%d %H:%M:%S").c_str(),activity.m_shopID);
	}
#endif
	return true;
}

bool CShopActivityMgr::isShopOpen(UINT32 shopID)
{
	std::map<UINT32,stShopActivity>::iterator it = m_activities.begin();
	for (;it != m_activities.end();++it){
		stShopActivity & activity = it->second;
		if ((shopID == activity.m_shopID) && (g_gameServer->getSec() >= activity.m_startTime) && (g_gameServer->getSec() <= activity.m_endTime)){
			return true;	
		}
	}
	return false;
}

void CShopActivityMgr::sendAllActivity(GameUser * pUser)
{
	if (!pUser){
		return ;
	}
	MiscProto::stActivityShopList retCmd;

	std::map<UINT32,stShopActivity>::iterator it = m_activities.begin();
	for (;it != m_activities.end();++it){
		stShopActivity & activity = it->second;
		if ((g_gameServer->getSec() >= activity.m_startTime) && (g_gameServer->getSec() <= activity.m_endTime)){
			retCmd.add_activityids(it->first);
		}
	}

	pUser->sendProto2Me(retCmd);
}
