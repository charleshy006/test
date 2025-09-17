#include "ZoneMgr.h"
#include "json/writer.h"
#include "json/reader.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "OpIDMgr.h"
#include <tinyxml/tinyxml.h>

CZoneMgr::~CZoneMgr()
{
	
}

CZoneMgr::CZoneMgr()
{
	
}

bool CZoneMgr::load()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/ZoneList.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[区配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("Root");
	if (!pRoot){
		Athena::logger->error("[区配置]区配置加载错误,找不到Root节点!");
		return false;
	}  
	
	const char * szVal = NULL;

	TiXmlElement * pZonesNode = pRoot->FirstChildElement("Zones");
	if (pZonesNode){
		TiXmlElement * pZoneNode = pZonesNode->FirstChildElement("Zone");
		while (pZoneNode){
			stZoneInfo zone;

			if ((szVal = pZoneNode->Attribute("name"))){
				zone.m_name = szVal;	
			}

			if ((szVal = pZoneNode->Attribute("status"))){
				zone.m_status = szVal;	
			}		
			
			if ((szVal = pZoneNode->Attribute("server_id"))){
				zone.m_serverID = szVal;
			}

			if ((szVal = pZoneNode->Attribute("is_active"))){
				zone.m_isActive = szVal;
			}

			if ((szVal = pZoneNode->Attribute("is_recommend"))){
				zone.m_isRecommend = szVal;
			}	
			
			if ((szVal = pZoneNode->Attribute("socket_domain"))){
				zone.m_ip = szVal;	
			}

			if ((szVal = pZoneNode->Attribute("socket_port"))){
				zone.m_port = szVal;
			}		
			m_zoneInfos.push_back(zone);
			pZoneNode = pZoneNode->NextSiblingElement("Zone");
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载区配置信息:");
	for (UINT16 i = 0;i < m_zoneInfos.size(); ++i){
		stZoneInfo & zone =  m_zoneInfos[i];
		Athena::logger->trace("name=%s,status=%s,serverID=%s,isActive=%s,isRecommond=%s,ip=%s,port=%s",zone.m_name.c_str(),zone.m_status.c_str(),zone.m_serverID.c_str(),zone.m_isActive.c_str(),zone.m_isRecommend.c_str(),zone.m_ip.c_str(),zone.m_port.c_str());
	}
#endif
#ifdef _HDZ_DEBUG
	m_appServerInfo = readFile("./appserver.txt");
	m_txServerInfo  = readFile("./txserver.txt");
	m_gcServerInfo  = readFile("./gcserver.txt");

	Athena::logger->trace("appstore 服务器列表:%s",m_appServerInfo.c_str());
	Athena::logger->trace("tx 服务器列表:%s",m_txServerInfo.c_str());
	Athena::logger->trace("gc 服务器列表:%s",m_gcServerInfo.c_str());
#endif
	return true;
}

std::string CZoneMgr::readFile(const char * szPath)
{
	if (!szPath){
		return "";
	}

	FILE* fp = fopen(szPath, "r");
	if (fp){
		fseek(fp,0,SEEK_END);
		UINT32 fileLen = ftell(fp);
		if (fileLen <= 0){
			fclose(fp);
			return "";
		}
		char *pData = ATHENA_NEW char[fileLen];
		bzero(pData,fileLen);
		fseek(fp,0,SEEK_SET);
		fread(pData,sizeof(char),fileLen,fp);
		fclose(fp);
		std::string ret = pData;
		SAFE_DELETE(pData);
		return ret;
	}
	else {
		return "";
	}
}

std::string CZoneMgr::getZoneInfo()
{
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = "1";
	root["msg"] = "返回成功";
	Json::Value data;
	Json::Value zone;

	for (UINT16 i = 0;i < m_zoneInfos.size(); ++i){    
		stZoneInfo & zoneInfo =  m_zoneInfos[i];
		zone["name"] = zoneInfo.m_name;
		zone["status"] = atoi(zoneInfo.m_status.c_str());
		zone["server_id"] = zoneInfo.m_serverID;
		zone["is_active"] = zoneInfo.m_isActive;
		zone["is_recommend"] = zoneInfo.m_isRecommend;
		zone["socket_domain"] = zoneInfo.m_ip;
		zone["socket_port"] = zoneInfo.m_port;
		data.append(zone);
	}
	
	root["data"] = data;

	std::string json_file = writer.write(root);
	return json_file;
}

std::string CZoneMgr::getZoneInfoByPlatfromID(UINT32 platformID)
{
	if (COpIDMgr::ePLATFORM_HUNFU == platformID){
		return m_gcServerInfo; 
	}	
	else if (COpIDMgr::ePLATFORM_APPSTORE == platformID){
		return m_appServerInfo;	
	}
	else if (COpIDMgr::ePLATFORM_YYB == platformID){
		return m_txServerInfo;	
	}
	else {
		return getZoneInfo();	
	}
}

