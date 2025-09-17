#include "SysLanguages.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"

bool CSysLanguages::load()
{
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/syslanguages.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[杂项配置]打开配置文件%s错误",file.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[杂项配置]找不到节点root");
		return false;
	}

	TiXmlElement * pConstantNode = pRoot->FirstChildElement("syslanguages");
	if (!pConstantNode){
		Athena::logger->error("[杂项配置]找不到节点syslanguages");
		    return false;
	}

	INT32 iVal = 0;
	const char * szVal = NULL;

	TiXmlElement * pEntryNode = pConstantNode->FirstChildElement("entry");
	while (pEntryNode){
		UINT32 id = 0;
		if (pEntryNode->Attribute("id",&iVal)){
			id = iVal;
		}

		if((szVal = pEntryNode->Attribute("languages")))
		{
			m_languageMap[id] = szVal;
		}
		pEntryNode = pEntryNode->NextSiblingElement("entry");
	}

#ifdef _HDZ_DEBUG
	std::map<UINT8, std::string>::iterator it = m_languageMap.begin();
	for(; it != m_languageMap.end(); ++it)
	{
		Athena::logger->trace("加载了系统语言: id=%d, language = %s",it->first,it->second.c_str());
	}
#endif
	return true;
}

const char * CSysLanguages::getLanguage(eTitleID id)
{
	std::map<UINT8, std::string>::iterator it = m_languageMap.begin();
	for(; it != m_languageMap.end(); ++it)
	{
		if(id == it->first)
		{
			return it->second.c_str();
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->error("加载了系统语言，访问了不存在的key：%d",id);
#endif
	return " ";
}
