#include "GameVersionCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include <tinyxml/tinyxml.h>

CGameVersionCfg::CGameVersionCfg()
{
	m_minGameVersion = 0;
}

CGameVersionCfg::~CGameVersionCfg()
{

}

bool CGameVersionCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/Config.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[游戏版本配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("config");
	if (!pRoot){
		Athena::logger->error("[游戏版本配置]配置加载错误,找不到config节点!");
		return false;
	}
	
	INT32 iVal = 0;
	TiXmlElement * pMinVersionNode = pRoot->FirstChildElement("minVersion");
	if (pMinVersionNode){
		if (pMinVersionNode->Attribute("value",&iVal)){
			m_minGameVersion = iVal;	
		}
	}
	Athena::logger->trace("登录需求版本:%d",m_minGameVersion);
	return true;
}
