#include "OpenFunction.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include <tinyxml/tinyxml.h>

std::map<UINT16,UINT16> COpenFunction::m_funID2Lvs;		//功能号与等级
std::map<UINT16,UINT8> COpenFunction::m_funID2Day;     //功能号与延时天数

COpenFunction::COpenFunction()
{
}

COpenFunction::~COpenFunction()
{

}

bool COpenFunction::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/openfunction.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[开放功能等级配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[开放功能等级配置]配置加载错误,找不到root节点!");
		return false;
	}
	
	INT32 iVal = 0;
	TiXmlElement * pOpenLevelNode = pRoot->FirstChildElement("Openlevel");
	if (pOpenLevelNode){
		TiXmlElement * pEntryNode = pOpenLevelNode->FirstChildElement("entry");
		while (pEntryNode) {
			UINT16 level = 0;
			UINT16 funID = 0;
			UINT8 openDay = 0;
			if (pEntryNode->Attribute("level",&iVal)){
				level = iVal;
			}

			if (pEntryNode->Attribute("openfunction",&iVal)){
				funID = iVal;
			}

			if(pEntryNode->Attribute("openday",&iVal)){
				openDay = iVal;
			}

			m_funID2Lvs[funID] = level;
			m_funID2Day[funID] = openDay;
			
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}
#ifdef _HDZ_DEBUG
	std::map<UINT16,UINT16>::iterator it = m_funID2Lvs.begin();
	for (;it != m_funID2Lvs.end();++it){
		Athena::logger->trace("功能号:%d,等级:%d",it->first,it->second);	
	}

	std::map<UINT16,UINT8>::iterator iter = m_funID2Day.begin();
	for (;iter != m_funID2Day.end();++iter){
		Athena::logger->trace("功能号:%d,延时天数:%d",iter->first,iter->second);
	}
#endif
	return true;
}

bool COpenFunction::checkFunOpen(eOpenFunID funID)
{
	std::map<UINT16,UINT16>::iterator it = m_funID2Lvs.find(funID);	

	if (it == m_funID2Lvs.end()){
		return true;
	}
	return getLevel() >= it->second; 
}

UINT8 COpenFunction::getOpenDay(eOpenFunID funID)
{
	std::map<UINT16,UINT8>::iterator it = m_funID2Day.find(funID);

	if (it == m_funID2Day.end()){
		    return 0;
	}
	return it->second;
}
