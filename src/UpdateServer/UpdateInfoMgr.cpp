#include "UpdateInfoMgr.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "TblQueryResult.h"
#include "UpdateServer.h"
#include "MysqlDatabase.h"
#include <tinyxml/tinyxml.h>

CTblField g_innerUpdateInfoDef[] = { 
	{ "GAME_ID",            CTblField::DB_TYPE_UINT32,   sizeof(UINT32)   },  
	{ "GAME_OP_ID",         CTblField::DB_TYPE_UINT32,   sizeof(UINT32)   },  
	{ "VERSION",     		CTblField::DB_TYPE_STRING,   25               },  
	{ "UPGRADE_URL",     	CTblField::DB_TYPE_STRING,   250              },  
	{ NULL, 0, 0 },
};

void stUpdateInfo::initFromDB(const stInnerUpdateInfoDB & data)
{
	m_gameID = data.m_gameID;
	m_opGameID = data.m_opGameID;
	m_version = data.m_version;
	m_upgradeURL = data.m_upgradeURL;
}

void stUpdateInfo::fillDBData(stInnerUpdateInfoDB & data)
{
	data.m_gameID = m_gameID;
	data.m_opGameID = m_opGameID;
	strncpy(data.m_version,m_version.c_str(),sizeof(data.m_version));
	strncpy(data.m_upgradeURL,m_upgradeURL.c_str(),sizeof(data.m_upgradeURL));
}

UpdateInfoMgr::~UpdateInfoMgr()
{
	
}

UpdateInfoMgr::UpdateInfoMgr()
{
	
}

bool UpdateInfoMgr::load()
{
	clearAll();

	{
		CTblQueryResult * result = g_databaseEngine.select(g_innerUpdateInfoDef," INNER_UPDATE_INFO ",NULL,NULL,0,NULL);
		if (result && result->getData()){
			stInnerUpdateInfoDB * pDBData = (stInnerUpdateInfoDB * )result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stUpdateInfo *pInfo = ATHENA_NEW stUpdateInfo();
				pInfo->initFromDB(pDBData[i]);
				m_updateInfos[pInfo->m_opGameID][pInfo->m_version] = pInfo;
			}   
		}  

		SAFE_DELETE(result);
	}

	TiXmlDocument doc;
	std::string fileName = "../../config/UpdateInfo.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[更新信息配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("Root");
	if (!pRoot){
		Athena::logger->error("[更新信息配置]区配置加载错误,找不到Root节点!");
		return false;
	} 

	INT32 iVal = 0;
	const char * szVal = NULL;
	
	if ((szVal = pRoot->Attribute("openAppVersion"))){
		m_openAppVersion = szVal;	
	}
	
	if ((szVal = pRoot->Attribute("sanbox"))){
		m_sanbox = szVal;	
	}
	
	if ((szVal = pRoot->Attribute("directBuy"))){
		m_directBuy = szVal;	
	}

	TiXmlElement * pInnerUpdateNode = pRoot->FirstChildElement("InnerUpdate");
	if (pInnerUpdateNode) {
		TiXmlElement * pUpdateInfoNode = pInnerUpdateNode->FirstChildElement("UpdateInfo");
		while (pUpdateInfoNode){
			stUpdateInfo * pUpdateInfo = ATHENA_NEW stUpdateInfo();
			if (pUpdateInfoNode->Attribute("gameID",&iVal)){
				pUpdateInfo->m_gameID = iVal;
			}		

			if (pUpdateInfoNode->Attribute("op_gameID",&iVal)){
				pUpdateInfo->m_opGameID = iVal;
			}

			if ( (szVal = pUpdateInfoNode->Attribute("version"))){
				pUpdateInfo->m_version = szVal;
			}

			if ( (szVal = pUpdateInfoNode->Attribute("upgradeURL"))){
				pUpdateInfo->m_upgradeURL = szVal;
			}
			m_updateInfos[pUpdateInfo->m_opGameID][pUpdateInfo->m_version] = pUpdateInfo;
			pUpdateInfoNode = pUpdateInfoNode->NextSiblingElement("UpdateInfo");
		}
	}

	TiXmlElement * pTotalUpdateNode = pRoot->FirstChildElement("TotalUpdate");
	if (pTotalUpdateNode){
		TiXmlElement * pOpNode = pTotalUpdateNode->FirstChildElement("Op");
		while (pOpNode){
			stTotalUpdateInfo * pTotalUpdateInfo = ATHENA_NEW stTotalUpdateInfo();
			UINT32 opID = 0;
			if (pOpNode->Attribute("opID",&iVal)){
				opID = iVal;
			}
			
			if ((szVal = pOpNode->Attribute("appReviewVer"))){
				pTotalUpdateInfo->m_appReviewVer = szVal;
			}
			
			if ( (szVal = pOpNode->Attribute("curVer"))){
				pTotalUpdateInfo->m_curVer = szVal;
			}
			
			if ((szVal = pOpNode->Attribute("appstoreUrl"))){
				pTotalUpdateInfo->m_appUrl = szVal;
				pTotalUpdateInfo->m_appUrl = HttpUtility::URLDecode(pTotalUpdateInfo->m_appUrl);
			}
			m_totalUpdateInfos[opID] = pTotalUpdateInfo;
			pOpNode = pOpNode->NextSiblingElement("Op"); 
		}
	}
#ifdef  _HDZ_DEBUG
	Athena::logger->trace("更新信息:openAppVersion:%s,sanbox:%s,dirctBuy:%s",m_openAppVersion.c_str(),m_sanbox.c_str(),m_directBuy.c_str());
	UpdateInfoMap_It it = m_updateInfos.begin();
	
	for (;it != m_updateInfos.end();++it){
		Athena::logger->trace("平台 gameOpID:%d,所拥有版本:",it->first);	
		__gnu_cxx::hash_map<std::string,stUpdateInfo *, my_hash<std::string> >::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			Athena::logger->trace("version:%s,url:%s",subIt->first.c_str(),subIt->second->m_upgradeURL.c_str());	
		}
	}

	Athena::logger->trace("整包更新:");
	TotalUpdateInfoMap_It subIt = m_totalUpdateInfos.begin();
	for (;subIt != m_totalUpdateInfos.end();++subIt){
		//stTotalUpdateInfo * pUpdateInfo = subIt->second;
		Athena::logger->trace("opID:%d,appReviewVer:%s,curVer:%s,appstoreUrl:%s",subIt->first,subIt->second->m_appReviewVer.c_str(),subIt->second->m_curVer.c_str(),subIt->second->m_appUrl.c_str());
	}
#endif
	return true;
}

void UpdateInfoMgr::clearAll()
{
	UpdateInfoMap_It it = m_updateInfos.begin();
	
	for (;it != m_updateInfos.end();++it){
		Athena::logger->trace("平台 gameOpID:%d,所拥有版本:",it->first);	
		__gnu_cxx::hash_map<std::string,stUpdateInfo *, my_hash<std::string> >::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			SAFE_DELETE(subIt->second);
		}
	}
	m_updateInfos.clear();
}

std::string UpdateInfoMgr::getUpgradeURL(UINT32 gameID,UINT32 opGameID,std::string version)
{
	UpdateInfoMap_It it = m_updateInfos.find(opGameID);
	if (it == m_updateInfos.end()){
		return "";
	}
	
	__gnu_cxx::hash_map<std::string,stUpdateInfo *, my_hash<std::string> >::iterator subIt = it->second.find(version);	
	if (subIt == it->second.end()){
		return "";
	}
	return subIt->second->m_upgradeURL;
}

bool UpdateInfoMgr::getTotalUpdateInfo(UINT32 opID,std::string & appReviewVer, std::string & curVer,std::string & appUrl)
{
	TotalUpdateInfoMap_It it  = m_totalUpdateInfos.find(opID);
	if (it != m_totalUpdateInfos.end()){
		appReviewVer = it->second->m_appReviewVer;
		curVer = it->second->m_curVer;
		appUrl = it->second->m_appUrl;
		return true;
	}
	return false;
}
