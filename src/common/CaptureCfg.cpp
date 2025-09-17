#include "CaptureCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include <vector>

CCaptureCfg::CCaptureCfg()
{
	m_beginTime = 0;
	m_endTime = 0;
}

CCaptureCfg::~CCaptureCfg()
{
}


bool CCaptureCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/Indiana.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[夺宝配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[夺宝配置]读取配置文件找不到节点<root>");
		return false;
	}  

	INT32 iVal = 0;
	TiXmlElement * pEludeNode = pRoot->FirstChildElement("Elude");
	if (pEludeNode){
		TiXmlElement * pTimeNode = pEludeNode->FirstChildElement("Time");
		while (pTimeNode){
			stPeaceModulePrice one;

			if (pTimeNode->Attribute("hours",&iVal)){
				one.m_hour = iVal;	
			}
			
			if (pTimeNode->Attribute("itemid",&iVal)){
				one.m_itemID = iVal;
			}
			
			if (pTimeNode->Attribute("costResType",&iVal)){
				one.m_costType = iVal;
			}
			
			if (pTimeNode->Attribute("costResNum",&iVal)){
				one.m_costNum = iVal;
			}

			if (1 == one.m_hour){
				m_1HourPrice = one;
			}
			
			if (3 == one.m_hour){
				m_3HourPrice = one;
			}
			
			if (12 == one.m_hour){
				m_12HourPrice = one;
			}

			pTimeNode = pTimeNode->NextSiblingElement("Time");
		}
	}
	
	const char * szVal = NULL;
	TiXmlElement * pOpeningtimeNode = pRoot->FirstChildElement("openingtime");
	if (pOpeningtimeNode){
		TiXmlElement * pEntryNode = pOpeningtimeNode->FirstChildElement("entry");
		if (pEntryNode){
			if ((szVal = pEntryNode->Attribute("opentime"))){
				std::vector<std::string> strVec;
				g_stringtok(strVec,szVal,"-");

				if (strVec.size() >= 2){
					m_beginTime = (UINT32)atoi(strVec[0].c_str());
					m_endTime = (UINT32)atoi(strVec[1].c_str());
				}
			}
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("夺宝价格配置:hour:%d,itemID:%d,costType:%d,costNum:%d",m_1HourPrice.m_hour,m_1HourPrice.m_itemID,m_1HourPrice.m_costType,m_1HourPrice.m_costNum);
	Athena::logger->trace("夺宝价格配置:hour:%d,itemID:%d,costType:%d,costNum:%d",m_3HourPrice.m_hour,m_3HourPrice.m_itemID,m_3HourPrice.m_costType,m_3HourPrice.m_costNum);
	Athena::logger->trace("夺宝价格配置:hour:%d,itemID:%d,costType:%d,costNum:%d",m_12HourPrice.m_hour,m_12HourPrice.m_itemID,m_12HourPrice.m_costType,m_12HourPrice.m_costNum);
	Athena::logger->trace("夺宝开始时间:%d,结束时间:%d",m_beginTime,m_endTime);
#endif
	return true;
}
