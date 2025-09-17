#include "ServerListener.h"
#include "HttpClient.h"
#include "ServerBase.h"
#include "WebServer.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "md5.h"

#include <cstring>
#include <ifaddrs.h>
#include <arpa/inet.h>

extern WebServer * g_webServer;

static std::string getIp(const std::string &ifaName) {
    std::string ip;
    struct sockaddr_in *sin = NULL;
    struct ifaddrs *ifa = NULL, *ifList;

    if (getifaddrs(&ifList) < 0) {
        return ip;
    }
    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next) {
        if(ifa->ifa_addr->sa_family == AF_INET && !strncmp(ifa->ifa_name, ifaName.c_str(), ifaName.length())) {
            sin = (struct sockaddr_in *)ifa->ifa_addr;
            ip = inet_ntoa(sin->sin_addr);
            break;
        }
    }
    freeifaddrs(ifList);
    return ip;
}

ServerListener::ServerListener() {
    m_ip = getIp("eth0");
}

void ServerListener::notifySelf() {
	SvrInfo *pSvrInfo = g_webServer->getSvrInfoByType(WEBSERVER);
	if (!pSvrInfo){
		return ;
	}

	std::ostringstream os;
    os << "ip=" << m_ip << "&webPort=" << pSvrInfo->m_port << "&gameId=" << Athena::global["gameID"];

	CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
    //这里的ip要用域名代替,端口号随机4个中的一个
	pRequest->setUrl("http://audit.ly.tjsanguo.com:30000/reportSelf");
	pRequest->setRequestType(CHttpRequest::kHttpPost);
	pRequest->setRequestData(os.str().c_str(),os.str().size());

    std::string configData;
    CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
    if (pRes && pRes->isSucceed()){
        std::vector<char> &retVec = *pRes->getResponseData();
        if (retVec.size() > 0){
            configData.assign(&retVec[0],retVec.size());
        }
    }
    SAFE_DELETE(pRes);
    Athena::logger->trace("reportSelf end, configData is %s", configData.c_str());
}

void ServerListener::timer(UINT32 cur) {
    notifySelf();
}

bool ServerListener::verifySign(std::vector<std::string> &vec,std::string &sign) {
	char tokenBuf[1024] = { 0 };
	int  bufLen = 0;

	std::ostringstream os ;
	for (UINT16 i = 0;i < vec.size();++i){
		os << vec[i];
	}
    
    std::string key("rsba0dcn4erDsdf3h3kVsdfsyewlrsdf6S5xvsde");
    snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s%s",os.str().c_str(), key.c_str());
	bufLen = strlen(tokenBuf);
	char digest[64] = {0};
	utils::Md5(tokenBuf,bufLen,digest);
	utils::HexString::bin2asc(digest,16,digest,64,bufLen);
	digest[bufLen] = 0;
	
	std::string subDigest(&digest[3],24);

	if (sign == subDigest){
		return true;
	}
	else {
		return false;
	}
}
