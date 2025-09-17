#include "TypesDef.h"
#include "CommonMicro.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "AppConfig.h"
#include "HttpClient.h"
#include "json/reader.h"
#include "UtilFunction.h"
#include "md5.h"
#include <signal.h>
#include <getopt.h>
#include <stdio.h>      
#include <ifaddrs.h>
#include <string.h> 
#include <arpa/inet.h>

#define VERIFY_SIGN_KEY "6EPin6wUV,7mYkbM"

#define GET_VALUE_FROM_JSON(key) do{\
	if (configJson[key].isString()){\
		std::string value =  configJson[key].asString();\
		Athena::global[key] =  value;\
	}\
}while(false);

#define GM_SERVER_DOMAIN "http://gm.mmzqlqq.youzu.com/api/getServerConfig"

int main(int argc, char *argv[])
{
	UINT32 timestamp = time(NULL);
	std::string myIp;
	if (argc >= 2){
		myIp = argv[1];
	}
	std::vector<std::string> ipList;
	struct ifaddrs * ifAddrStruct=NULL;
	void * tmpAddrPtr=NULL;
	getifaddrs(&ifAddrStruct);
	
	while (ifAddrStruct!=NULL) {
		if (ifAddrStruct->ifa_addr->sa_family == AF_INET) {// check it is IP4
			tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN] = { 0 };
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			if (strncmp(addressBuffer,"127.0.0.1",sizeof(addressBuffer)) == 0){
				ifAddrStruct=ifAddrStruct->ifa_next;
				continue;
			}
			ipList.push_back(addressBuffer);
			//printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer); 
		}
		ifAddrStruct=ifAddrStruct->ifa_next;
	}
	
	std::vector<std::string> headers;
	std::string configData;
	for (UINT16 i = 0;i < ipList.size();++i) {
		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
		pRequest->setRequestType(CHttpRequest::kHttpGet);
		char szQuery[125] = { 0 };
		char tokenBuf[1024] = { 0 };
		int  bufLen = 0;
		if (myIp.size() > 0){
			snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%d%s%s",timestamp,myIp.c_str(),VERIFY_SIGN_KEY);
		}
		else{ 
			snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%d%s%s",timestamp,ipList[i].c_str(),VERIFY_SIGN_KEY);
		}
		bufLen = strlen(tokenBuf);
		char digest[64] = {0};
		utils::Md5(tokenBuf,bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string subDigest(&digest[3],24);
		
		if (myIp.size() > 0){
			snprintf(szQuery,sizeof(szQuery),"%s?ip=%s&timestamp=%d&sign=%s",GM_SERVER_DOMAIN,myIp.c_str(),timestamp,subDigest.c_str());
		}
		else {
			snprintf(szQuery,sizeof(szQuery),"%s?ip=%s&timestamp=%d&sign=%s",GM_SERVER_DOMAIN,ipList[i].c_str(),timestamp,subDigest.c_str());
		}
		pRequest->setUrl(szQuery);
		printf("设置请求参数:%s\n",szQuery);
		CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
		if (pRes && pRes->isSucceed()){
			std::vector<char> &retVec = *pRes->getResponseData();
			printf("请求成功,返回内容长度为:%d\n",(UINT32)retVec.size());
			if (retVec.size() > 0){
				configData.assign(&retVec[0],retVec.size());
				if (myIp.size() <= 0){
					myIp = ipList[i];
				}
				break;
			}
		}
		SAFE_DELETE(pRes);
	}
	
	printf("获得的配置内容为:%s\n",configData.c_str());

	if (configData.size() > 0){//成功
		Json::Reader reader;
		Json::Value  rootJson;
		if (!reader.parse(configData,rootJson)){
			printf("解析json失败!\n");
			return EXIT_SUCCESS;
		}   
		
		int retCode = -1;
		if (rootJson["return_code"].isInt()){
			retCode = (int)(rootJson["return_code"].asInt());
		}
	
		if (0 != retCode){
			printf("获得服务器配置失败\n");
			return EXIT_SUCCESS; 
		}

		Json::Value configJson = rootJson["config_data"];

		GET_VALUE_FROM_JSON("GlobalServer_IP");
		GET_VALUE_FROM_JSON("GlobalServer_Port");
		GET_VALUE_FROM_JSON("GameServer_IP");
		GET_VALUE_FROM_JSON("GameServer_Port");
		//GET_VALUE_FROM_JSON("WebServer_IP");
		Athena::global["WebServer_IP"] = myIp;
		GET_VALUE_FROM_JSON("WebServer_Port");
		GET_VALUE_FROM_JSON("GateServer_IP");
		GET_VALUE_FROM_JSON("GateServer_Port");
		GET_VALUE_FROM_JSON("gameVersion");
		Athena::global["statNetFlow"] = "false";
		Athena::global["statProto"] = "false";
		Athena::global["loadStat"] = "true";
		Athena::global["tblResDir"] = "./tblres";
		Athena::global["confDir"] = "./conf";
		Athena::global["isApp"] = "false";
		Athena::global["sanbox"] = "false";
		Athena::global["ZoneID"] = "1";
		Athena::global["bundleID"] = "com.uuzu.DHLong";

		GET_VALUE_FROM_JSON("gameID");
		GET_VALUE_FROM_JSON("platformID");
		GET_VALUE_FROM_JSON("serverID");
		GET_VALUE_FROM_JSON("serverKey");
		GET_VALUE_FROM_JSON("billURL");
		GET_VALUE_FROM_JSON("OpenServer_Time");
	
		std::string serverID = Athena::global["serverID"];
		std::string platformID;
		if (serverID.size() >= 4){ 
			platformID = serverID.substr(0,4);
		}   
		Athena::global["platformID"] = platformID;

		GET_VALUE_FROM_JSON("GlobalServer_Log");
		GET_VALUE_FROM_JSON("GlobalServer_DBUrl");
		GET_VALUE_FROM_JSON("GlobalServer_DBPort");
		GET_VALUE_FROM_JSON("GlobalServer_DBUser");
		GET_VALUE_FROM_JSON("GlobalServer_DBPassword");
		GET_VALUE_FROM_JSON("GlobalServer_DBName");
		GET_VALUE_FROM_JSON("GameServer_Log");
		GET_VALUE_FROM_JSON("GateServer_Log");
		GET_VALUE_FROM_JSON("WebServer_Log");
		GET_VALUE_FROM_JSON("WebServer_DBUrl");
		GET_VALUE_FROM_JSON("WebServer_DBPort");
		GET_VALUE_FROM_JSON("WebServer_DBUser");
		GET_VALUE_FROM_JSON("WebServer_DBPassword");
		GET_VALUE_FROM_JSON("WebServer_DBName");
		GET_VALUE_FROM_JSON("BillServer_Log");
		GET_VALUE_FROM_JSON("BillServer_DBUrl");
		GET_VALUE_FROM_JSON("UpdateServer_Log");
		GET_VALUE_FROM_JSON("UpdateServer_DBUrl");
		Athena::global["VerifyToken"] = "true";
		Athena::global["InnerVerifyMethod"] = "false";
		Athena::global["GateUserNum"] = "20000";
	}
	else {
		printf("获取配置失败\n");
		return EXIT_SUCCESS;
	}
	AppConfig app;
	app.saveAppConfig("../../config/appconfig.xml");
	printf("保存服务器配置成功!");
    return EXIT_SUCCESS;
}
