#include "AppConfig.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"

AppConfig::AppConfig(const char* filename)
{
	this->m_filename = filename;
}

AppConfig::~AppConfig()
{
}

bool AppConfig::getNodeVal(const TiXmlNode * node)
{
	if (!node){
		return false;
	}
	const TiXmlElement * pElem = node->FirstChildElement();
	while (pElem) {
		std::string key = pElem->Value();
		const char * szValue = pElem->GetText();
		std::string value;
		if (szValue){
			value = szValue;
			Athena::global.setProperty(key,value);		
		}
		pElem = pElem->NextSiblingElement();
	}

	return true;
}

bool AppConfig::loadCfg(const char* name)
{
	if (m_doc.LoadFile(m_filename.c_str())) {

		TiXmlNode * root = m_doc.FirstChild("Athena");
		if (root) {
			TiXmlNode * globalNode = root->FirstChild("Global");
			if (globalNode) {
				if (!getNodeVal(globalNode)){
					return false;
				}
			} else {
				Athena::logger->warn("配置文件中，没有找到全局配置");
			}

			TiXmlNode * svrNode = root->FirstChild(name);
			if (svrNode) {
				if (!getNodeVal(svrNode)) {
					return false;
				}
				parseCustom(svrNode);
			} else {
				Athena::logger->warn("配置文件中，没有找到 %s 配置", name);
			}
			return true;
		}
	}
	return false;
}

bool AppConfig::isFileExist(const char * fileName)
{
	if (!fileName){
		return false;
	}

	if (access(fileName,F_OK) == 0){
		return true;
	}	
	else {
		return false;
	}
}

#define LINK_XML_CHILD(parent,elementName,key) do {\
		TiXmlElement nameElem(elementName);\
		TiXmlText    valueTxt(Athena::global[key].c_str());\
		nameElem.InsertEndChild(valueTxt);\
		parent.InsertEndChild(nameElem);\
}while(false);

#define LINK_XML_CHILD_EXTEND(parent,elementName,key,value) do{\
	TiXmlElement nameElem(elementName);\
	TiXmlText    valueTxt(value);\
	nameElem.InsertEndChild(valueTxt);\
	parent.InsertEndChild(nameElem);\
}while(false);

bool AppConfig::saveAppConfig(const char * fileName)
{
	if (!fileName){
		return false;
	}

	TiXmlDocument doc;
	TiXmlElement  athenaElement("Athena");

	TiXmlElement  globalElement("Global");
	TiXmlElement  serverListElement("ServerList");
	TiXmlElement  globalServerElement("GlobalServer");
	TiXmlElement  gameServerElement("GameServer");
	TiXmlElement  gateServerElement("GateServer");
	TiXmlElement  webServerElement("WebServer");
	TiXmlElement  billServerElement("BillServer");
	TiXmlElement  updateServerElement("UpdateServer");
	TiXmlElement  loginServerElement("LoginServer");
	
	//生成serverList节点
	{
		TiXmlElement globalElem("Server");
		TiXmlElement gameElem("Server");
		TiXmlElement gateElem("Server");
		TiXmlElement webElem("Server");
		
		globalElem.SetAttribute("svrID","1");
		globalElem.SetAttribute("name","全局服务器");
		globalElem.SetAttribute("svrType","1");
		globalElem.SetAttribute("ip",Athena::global["GlobalServer_IP"].c_str());
		globalElem.SetAttribute("port",Athena::global["GlobalServer_Port"].c_str());
		
		gameElem.SetAttribute("svrID","2");
		gameElem.SetAttribute("name","游戏服务器");
		gameElem.SetAttribute("svrType","2");
		gameElem.SetAttribute("ip",Athena::global["GameServer_IP"].c_str());
		gameElem.SetAttribute("port",Athena::global["GameServer_Port"].c_str());
		
		webElem.SetAttribute("svrID","3");
		webElem.SetAttribute("name","web服务器");
		webElem.SetAttribute("svrType","5");
		webElem.SetAttribute("ip",Athena::global["WebServer_IP"].c_str());
		webElem.SetAttribute("port",Athena::global["WebServer_Port"].c_str());
		
		gateElem.SetAttribute("svrID","4");
		gateElem.SetAttribute("name","网关服务器");
		gateElem.SetAttribute("svrType","3");
		gateElem.SetAttribute("ip",Athena::global["GateServer_IP"].c_str());
		gateElem.SetAttribute("port",Athena::global["GateServer_Port"].c_str());

		serverListElement.InsertEndChild(globalElem);
		serverListElement.InsertEndChild(gameElem);
		serverListElement.InsertEndChild(gateElem);
		serverListElement.InsertEndChild(webElem);
	}

	//生成global节点的信息
	{
		LINK_XML_CHILD(globalElement,"gameVersion","gameVersion");
		LINK_XML_CHILD(globalElement,"statNetFlow","statNetFlow");
		LINK_XML_CHILD(globalElement,"statProto","statProto");
		LINK_XML_CHILD(globalElement,"loadStat","loadStat");
		LINK_XML_CHILD(globalElement,"tblResDir","tblResDir");
		LINK_XML_CHILD(globalElement,"confDir","confDir");
		LINK_XML_CHILD(globalElement,"gameID","gameID");
		LINK_XML_CHILD(globalElement,"platformID","platformID");
		LINK_XML_CHILD(globalElement,"serverID","serverID");
		LINK_XML_CHILD(globalElement,"serverKey","serverKey");
		LINK_XML_CHILD(globalElement,"billURL","billURL");
		LINK_XML_CHILD(globalElement,"isApp","isApp");
		LINK_XML_CHILD(globalElement,"sanbox","sanbox");
		LINK_XML_CHILD(globalElement,"ZoneID","ZoneID");
		LINK_XML_CHILD(globalElement,"OpenServer_Time","OpenServer_Time");
		LINK_XML_CHILD(globalElement,"bundleID","bundleID");
	}
	
	//生成globalServer
	{
		LINK_XML_CHILD_EXTEND(globalServerElement,"logfilename","GlobalServer_Log",Athena::global["GlobalServer_Log"].c_str());
#ifdef _HDZ_DEBUG
		printf("GlobalServer_Log:%s\n",Athena::global["GlobalServer_Log"].c_str());
#endif
		std::ostringstream os ;
		os << Athena::global["GlobalServer_DBUrl"] 
			<<";"
			<< Athena::global["GlobalServer_DBPort"] 
			<< ";"
			<< Athena::global["GlobalServer_DBUser"]
			<< ";"
			<< Athena::global["GlobalServer_DBPassword"]
			<< ";"
			<<  Athena::global["GlobalServer_DBName"];
#ifdef _HDZ_DEBUG
		printf("globalserver url:%s",os.str().c_str());
#endif
		LINK_XML_CHILD_EXTEND(globalServerElement,"mysqlurl","mysqlurl",os.str().c_str());
	}
	//生成gameServer
	{
		LINK_XML_CHILD_EXTEND(gameServerElement,"logfilename","logfilename",Athena::global["GameServer_Log"].c_str());
	}
	//生成gateServer
	{
		LINK_XML_CHILD_EXTEND(gateServerElement,"logfilename","logfilename",Athena::global["GateServer_Log"].c_str());
		LINK_XML_CHILD(gateServerElement,"VerifyToken","VerifyToken");
		LINK_XML_CHILD(gateServerElement,"InnerVerifyMethod","InnerVerifyMethod");
		LINK_XML_CHILD(gateServerElement,"GateUserNum","GateUserNum");
	}
	//生成webServer
	{
		LINK_XML_CHILD_EXTEND(webServerElement,"logfilename","logfilename",Athena::global["WebServer_Log"].c_str());
		std::ostringstream os ;		
		os << Athena::global["WebServer_DBUrl"] 
			<<";"
			<< Athena::global["WebServer_DBPort"] 
			<< ";"
			<< Athena::global["WebServer_DBUser"]
			<< ";"
			<< Athena::global["WebServer_DBPassword"]
			<< ";"
			<<  Athena::global["WebServer_DBName"];
		LINK_XML_CHILD_EXTEND(webServerElement,"mysqlurl","mysqlurl",os.str().c_str());
	}
	//生成billServer
	{
		LINK_XML_CHILD_EXTEND(billServerElement,"logfilename","logfilename",Athena::global["BillServer_Log"].c_str());
		LINK_XML_CHILD_EXTEND(billServerElement,"mysqlurl","mysqlurl",Athena::global["BillServer_DBUrl"].c_str());
	}
	//生成updateServer
	{
		LINK_XML_CHILD_EXTEND(updateServerElement,"logfilename","logfilename",Athena::global["UpdateServer_Log"].c_str());
		LINK_XML_CHILD_EXTEND(updateServerElement,"mysqlurl","mysqlurl",Athena::global["UpdateServer_DBUrl"].c_str());
	}
	//生成loginServer
	{
		//LINK_XML_CHILD(loginServerElement,"logfilename",Athena::global["WebServer_Log"].c_str());
		//LINK_XML_CHILD(loginServerElement,"mysqlurl",Athena::global["WebServer_Log"].c_str());
	}
	athenaElement.InsertEndChild(globalElement);
	athenaElement.InsertEndChild(serverListElement);
	athenaElement.InsertEndChild(globalServerElement);
	athenaElement.InsertEndChild(gameServerElement);
	athenaElement.InsertEndChild(gateServerElement);
	athenaElement.InsertEndChild(webServerElement);
	athenaElement.InsertEndChild(billServerElement);
	athenaElement.InsertEndChild(updateServerElement);
	athenaElement.InsertEndChild(loginServerElement);

	doc.InsertEndChild(athenaElement);
	doc.SaveFile(fileName);
	return true;
}
