#include "GloryfashionMgr.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include <string>
#include "UtilFunction.h"

bool CGloryfashionMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string guildFile = Athena::global["confDir"] + "/Gloryfashion.xml";
	if (!doc.LoadFile(guildFile.c_str())){
		Athena::logger->error("[荣耀时装配置]打开配置文件%s错误",guildFile.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[荣耀时装配置]读取配置文件找不到节点<root>");
		return false;
	}

	INT32 iVal = 0;
	const char * szVal = NULL;
	
	TiXmlElement * pActivityOnNode = pRoot->FirstChildElement("Gloryfashion");
	if (pActivityOnNode){
		TiXmlElement * pEntryNode = pActivityOnNode->FirstChildElement("entry");
		while (pEntryNode){
			stActivityInfo one;
			if (pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}
		 	
			if ((szVal = pEntryNode->Attribute("Starttime"))){
				struct tm _tm;
				if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
					one.m_starttime = mktime(&_tm) ;
				}
			}

			if ((szVal = pEntryNode->Attribute("Endtime"))){
				struct tm _tm;
				if (strptime(szVal,"%Y-%m-%d %H:%M:%S",&_tm)){
					one.m_endtime = mktime(&_tm) ;
				}
			}

			if ((szVal = pEntryNode->Attribute("price"))){
				 std::vector<std::string> strVec;
				 g_stringtok(strVec, szVal, "-");
				 if(strVec.size() >= 2)
				 {
					 one.m_costResId = (UINT16)atoi(strVec[0].c_str());
					 one.m_costResNum = (UINT32)atoi(strVec[1].c_str());
				 }
			}

			if(pEntryNode->Attribute("fashionitemid",&iVal)){
				one.m_fashionitemid = iVal;
			}

		 	m_allActivitys.insert(std::make_pair(one.m_id,one));
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了商店的配置");
	std::map<UINT32,stActivityInfo>::iterator it = m_allActivitys.begin();
	for(; it != m_allActivitys.end(); ++it)
	{
		stActivityInfo & one = it->second;
		Athena::logger->trace("m_id = %d, m_starttime = %d, m_endtime = %d, m_costResType = %d, m_costResNum = %d, m_fashionitemid = %d", one.m_id, one.m_starttime, one.m_endtime, one.m_costResId, one.m_costResNum, one.m_fashionitemid);
	}

#endif
	return true;
}

//获得活动信息
CGloryfashionMgr::stActivityInfo * CGloryfashionMgr::getActicityInfo(UINT32 cur)
{
	std::map<UINT32,stActivityInfo>::iterator it = m_allActivitys.begin();
	for(;it != m_allActivitys.end(); ++it)
	{
		stActivityInfo & one = it->second;
		if(cur >= one.m_starttime && cur <= one.m_endtime)
		{
			return &it->second;
		}
	}

	return NULL;
}
