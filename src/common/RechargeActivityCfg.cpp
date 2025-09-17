#include "RechargeActivityCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include <vector>

RechargeActivityCfg::RechargeActivityCfg()
{
}

RechargeActivityCfg::~RechargeActivityCfg()
{
}


bool RechargeActivityCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/payreward.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[充值活动配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("RewardConf");
	if (!pRoot){
		Athena::logger->error("[充值活动配置]读取配置文件找不到节点<RewardConf>");
		return false;
	}  

	INT32 iVal = 0;
	const char * szVal = NULL;
	TiXmlElement * pActivityNode = pRoot->FirstChildElement("activity");
	while (pActivityNode){
		stActivityInfo activity;
		
		if (pActivityNode->Attribute("id",&iVal)){
			activity.m_id = iVal;	
		}
		
		if (pActivityNode->Attribute("RefreshTime",&iVal)){
			activity.m_interval = iVal;	
		}

		if ((szVal = pActivityNode->Attribute("StartTime"))){
			struct tm _tm;
			if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
				activity.m_beginTime = mktime(&_tm) ;//- 8 * 3600;
			}   
			else {
				return false;
			}   
		}
		
		if ((szVal = pActivityNode->Attribute("EndTime"))){
			struct tm _tm;
			if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
				activity.m_endTime = mktime(&_tm) ;//- 8 * 3600;
			}   
			else {
				return false;
			}   
		}

		TiXmlElement * pCostNode = pActivityNode->FirstChildElement("cost");
		while (pCostNode){
			stActivityStepInfo stepInfo;
			if (pCostNode->Attribute("id",&iVal)){
				stepInfo.m_id = iVal;	
			}
		
			if (pCostNode->Attribute("RechargeNum",&iVal)){
				stepInfo.m_rechargeNum = iVal;	
			}
			
			if (pCostNode->Attribute("Boxid",&iVal)){
				stepInfo.m_boxID = iVal;	
			}
			activity.m_steps.insert(std::make_pair(stepInfo.m_id,stepInfo));
			pCostNode = pCostNode->NextSiblingElement("cost");
		}
		m_activities.insert(std::make_pair(activity.m_id,activity));
		pActivityNode = pActivityNode->NextSiblingElement("activity");
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("开始打印充值活动:");
	std::map<UINT32,stActivityInfo>::iterator it = m_activities.begin();
	for (;it != m_activities.end();++it){
		stActivityInfo & info = it->second;
		Athena::logger->trace("id:%d,interval:%d,beginTime:%s,endTime:%s",info.m_id,info.m_interval,g_utc2StringDate(info.m_beginTime,"%Y-%m-%d %H:%M:%S").c_str(),g_utc2StringDate(info.m_endTime,"%Y-%m-%d %H:%M:%S").c_str());
		Athena::logger->trace("每个阶段为:");
		
		std::map<UINT8,stActivityStepInfo>::iterator subIt = info.m_steps.begin();
		for (;subIt != info.m_steps.end();++subIt){
			stActivityStepInfo & step = subIt->second;
			Athena::logger->trace("id:%d,rechargeNum:%d,boxID:%d",step.m_id,step.m_rechargeNum,step.m_boxID);
		}
	}
#endif
	return true;
}

RechargeActivityCfg::stActivityInfo * RechargeActivityCfg::getActivity(UINT16 activityID)
{
	std::map<UINT32,stActivityInfo>::iterator it = m_activities.find(activityID);

	if (it != m_activities.end()){
		return &it->second;
	}
	return NULL;
}
