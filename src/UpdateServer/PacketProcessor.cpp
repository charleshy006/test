#include "PacketProcessor.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "UpdateServer.h"
#include "HttpParser.h"
#include "json/reader.h"
#include "md5.h"
#include "UtilFunction.h"
#include "UpdateInfoMgr.h"
#include "json/writer.h"
#include "NoticeMgr.h"
#include "OpIDMgr.h"
#include "UidMgr.h"
#include "HttpClient.h"
#include "UpdateCache.h"

static void getOsServers(std::map<std::string, OsServerInfo> & idMap) {
    UINT32 now = g_updateServer->getSec();
    std::stringstream ss;

    std::string url = Athena::global["ApiURL"];
    ss << "columns=ip|is_active|is_recommend|server_url|server_port|socket_domain|socket_port|type|first_opentime&"
        << "gameId=129&"
        << "gameOpId=7000&"
        << "opId=7000&"
        << "isAll=1&"
        << "isGzip=1&"
        << "serverId=0&"
        << "userid=&"
        << "time=" << now << "&"
        << "sign=" << now;

    CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
    pRequest->setRequestType(CHttpRequest::kHttpPost);
    char buf[1024] = { 0 };
    snprintf(buf, sizeof(buf) - 1, "%sserverlist?%s", url.c_str(), ss.str().c_str());
    pRequest->setUrl(buf);
    std::string configData;
    CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
    if (pRes && pRes->isSucceed()){
        std::vector<char> &retVec = *pRes->getResponseData();
        if (retVec.size() > 0){
            configData.assign(&retVec[0],retVec.size());
        }
    }
    SAFE_DELETE(pRes);
    Json::Value configJson;
    Json::Reader reader;

    if (reader.parse(configData, configJson)) {
        if (configJson["data"].isArray()){
            Json::Value allStep = configJson["data"]; 	
            Json::Value::iterator it = allStep.begin();
            for (;it != allStep.end();++it){
                Json::Value & step = *it;
                OsServerInfo item;
                if (step["server_id"].isString()){
                    item.id = step["server_id"].asString();
                }
                if (step["name"].isString()){
                    item.name = step["name"].asString();
                }/*
                if (step["socket_domain"].isString()){
                    item.domain = step["socket_domain"].asString();
                }*/
                if (step["ip"].isString()){
                    item.domain = step["ip"].asString();
                }
                idMap.insert(std::make_pair(item.id, item));
            }
        }
    }
#ifdef _HDZ_DEBUG
    for (std::map<std::string, OsServerInfo>::iterator it = idMap.begin();
            it != idMap.end(); it++) {
        Athena::logger->trace("id is %s, name is %s, domain is %s, port is %s",
                it->second.id.c_str(), it->second.name.c_str(), it->second.domain.c_str(), it->second.port.c_str());
    }
#endif
}

static int getWebPort(const std::string & domain) {
    std::string signKey = Athena::global["signKey"];
    UINT32 now = g_updateServer->getSec();
    std::stringstream ss;

    char szQuery[125] = { 0 };
    char tokenBuf[1024] = { 0 };
    int  bufLen = 0, webPort = 0;
    snprintf(tokenBuf, sizeof(tokenBuf) - 1, "%d%s%s", now, domain.c_str(), signKey.c_str());
    bufLen = strlen(tokenBuf);
    char digest[64] = {0};
    utils::Md5(tokenBuf,bufLen,digest);
    utils::HexString::bin2asc(digest,16,digest,64,bufLen);
    digest[bufLen] = 0;
    std::string subDigest(&digest[3],24);
    std::string url = Athena::global["ApiURL"];
    snprintf(szQuery,sizeof(szQuery),"%sgetServerConfig?ip=%s&timestamp=%d&sign=%s", url.c_str(), domain.c_str(), now, subDigest.c_str());

    CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
    pRequest->setRequestType(CHttpRequest::kHttpGet);
    pRequest->setUrl(szQuery);
    std::string configData;
    CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
    if (pRes && pRes->isSucceed()){
        std::vector<char> &retVec = *pRes->getResponseData();
        if (retVec.size() > 0){
            configData.assign(&retVec[0],retVec.size());
        }
    }
    SAFE_DELETE(pRes);
    Json::Value configJson;
    Json::Reader reader;

    if (reader.parse(configData, configJson)) {
        Json::Value & configVal = configJson["config_data"];
        if (configVal["WebServer_Port"].isString()){
            webPort = (int)atoi(configVal["WebServer_Port"].asString().c_str());
        }
    }
    return webPort;
}
/*
static int getUpdate(const std::string & partnerId, const std::string & version, bool totalUpdate,
                        std::map<std::string, std::string> & valueMap) {
    std::string signKey = Athena::global["signKey"];
    UINT32 now = g_updateServer->getSec();
    std::stringstream ss;
    if (totalUpdate) {
        //强更
        ss << partnerId << version;
    } else {
        //热更
        ss << version << partnerId;
    }
    ss << now << signKey;

    char tokenBuf[1024] = {0};
    int  bufLen = 0;
    snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",ss.str().c_str());
    bufLen = strlen(tokenBuf);
    char digest[64] = {0};
    utils::Md5(tokenBuf,bufLen,digest);
    utils::HexString::bin2asc(digest,16,digest,64,bufLen);
    digest[bufLen] = 0;
    std::string subDigest(&digest[3], 24);

    char szQuery[512] = { 0 };
    std::string url = Athena::global["ApiURL"];
    if (totalUpdate) {
        snprintf(szQuery,sizeof(szQuery),"%spackage_update?partner_id=%s&version=%s&timestamp=%d&sign=%s",
                url.c_str(), partnerId.c_str(), version.c_str(), now, subDigest.c_str());
    } else {
        snprintf(szQuery,sizeof(szQuery),"%spackage_list?version=%s&partner_id=%s&timestamp=%d&sign=%s",
                url.c_str(), version.c_str(), partnerId.c_str(), now, subDigest.c_str());
    }
#ifdef _HDZ_DEBUG
	Athena::logger->trace("szQuery is %s", szQuery);
#endif

    CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
    pRequest->setRequestType(CHttpRequest::kHttpGet);
    pRequest->setUrl(szQuery);
    std::string configData;
    CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
    if (pRes && pRes->isSucceed()){
        std::vector<char> &retVec = *pRes->getResponseData();
        if (retVec.size() > 0){
            configData.assign(&retVec[0],retVec.size());
        }
    }
    SAFE_DELETE(pRes);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("configData is %s", configData.c_str());
#endif
    Json::Value configJson;
    Json::Reader reader;

    int retCode = 0;
    if (reader.parse(configData, configJson)) {
        if (configJson["return_code"].isInt()) {
            retCode = configJson["return_code"].asInt();
        }
        if (configJson["return_msg"].isString()) {
            valueMap.insert(std::make_pair(std::string("return_msg"),
                        configJson["return_msg"].asString()));
        }
        if (totalUpdate) {//强更
            if (configJson["app_review"].isString()) {
                valueMap.insert(std::make_pair(std::string("app_review"),
                            configJson["app_review"].asString()));
            }
            if (configJson["update_url"].isString()) {
                valueMap.insert(std::make_pair(std::string("update_url"),
                            configJson["update_url"].asString()));
            }
            if (configJson["update_flag"].isString()) {
                valueMap.insert(std::make_pair(std::string("update_flag"),
                            configJson["update_flag"].asString()));
            }
        } else {//热更
            if (configJson["max_version"].isString()) {
                valueMap.insert(std::make_pair(std::string("max_version"),
                            configJson["max_version"].asString()));
            }
            if (configJson["source_md5"].isString()) {
                valueMap.insert(std::make_pair(std::string("source_md5"),
                            configJson["source_md5"].asString()));
            }
            if (configJson["source_url"].isString()) {
                valueMap.insert(std::make_pair(std::string("source_url"),
                            configJson["source_url"].asString()));
            }
        }
    }

    return retCode;
}
*/

static std::string getNewStr(const std::string & data) {
    std::stringstream ss;
    for (size_t i = 0; i < data.length(); i++) {
        if (data[i] == '{' || data[i] == '-' || data[i] == '}') {
            continue;
        }
        ss << data[i];
    }
    return ss.str();
}

PacketProcessor::PacketProcessor()
{
}

PacketProcessor::~PacketProcessor()
{
}

void PacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	Athena::logger->trace("收到了连接断开的协议svrID=%d",svrID);
}

bool PacketProcessor::processHttpMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
	if (!pConn || !header || !pNull) {
		return false;
	}
	if (5 == header->m_modID){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("收到了来自http协议的包:%s",(const char *)pNull);
#endif
		CHttpParser parser;
		parser.parseHttpRequest((const char *)pNull,header->m_dataLen);
#ifdef _HDZ_DEBUG
		parser.printAllHeader();
#endif
		std::string act = parser.getReqPath();	
		if (act == "/GetUpdateNew"){
			std::string gameID = parser.getReqParamVal("gameID");
			std::string opID  = parser.getReqParamVal("opID");
			std::string opGameID = parser.getReqParamVal("opGameID");
			std::string version = parser.getReqParamVal("version");
            //opId version to total update
            //opGameID version to inner update
			parser.clearHeader();
			Json::FastWriter writer;
			Json::Value root;
			root["status"] = 1;
			root["version"] = version;
			root["upgradeurl"] = "";
			root["needUpdate"] = "false";
			root["appstoreUrl"] = "";

			root["openAppVersion"] = "false";
			root["sanbox"] = "false";
			root["directBuy"] = "false";

            bool errorOccured = false;
            std::map<std::string, std::string> valueMap;
            UpdateCacheMgr::getSingleton().getRecord(opID, version, true, valueMap);
            if (valueMap.empty()) {
                int retCode = UpdateCacheMgr::getSingleton().getUpdate(opID, version, true, valueMap);
                if (retCode == 1) {
                    UpdateCacheMgr::getSingleton().addRecord(opID, version, true, valueMap);
                } else {
                    errorOccured = true;
                    Athena::logger->trace("req total update failed, return msg is %s", valueMap["return_msg"].c_str());
                }
            }
            if (!errorOccured) {
                std::string updateUrl = valueMap["update_url"];
                if (!updateUrl.empty()) {
                    root["needUpdate"] = "true";
                    root["appstoreUrl"] = updateUrl;
                }
                if (valueMap["app_review"] == std::string("1")) {
                    root["openAppVersion"] = "true";
                    root["sanbox"] = "true";
                    root["directBuy"] = "true";
                }
                valueMap.clear();
                UpdateCacheMgr::getSingleton().getRecord(opID, version, false, valueMap);
                if (valueMap.empty()) {
                    int retCode = UpdateCacheMgr::getSingleton().getUpdate(opID, version, false, valueMap);
                    if (retCode == 1) {
                        root["upgradeurl"] = valueMap["source_url"];
                        UpdateCacheMgr::getSingleton().addRecord(opID, version, false, valueMap);
                    } else {
                        Athena::logger->trace("req inner update failed, return msg is %s", valueMap["return_msg"].c_str());
                    }
                }
            } else {
                Athena::logger->trace("req total update failed, error Occured");
            }


/*
            bool bFinish = false;
            std::map<std::string, std::string> valueMap;
            int retCode = getUpdate(opID, version, true, valueMap);
            if (retCode == 1) {
                std::string updateUrl = valueMap["update_url"];
                if (!updateUrl.empty()) {
                    root["needUpdate"] = "true";
                    root["appstoreUrl"] = updateUrl;
                    //bFinish = true;
                }
                if (valueMap["app_review"] == std::string("1")) {
                    root["openAppVersion"] = "true";
                    root["sanbox"] = "true";
                    root["directBuy"] = "true";
                }
            } else {
                Athena::logger->trace("req total update failed, return msg is %s", valueMap["return_msg"].c_str());
            }
            if (!bFinish) {
                valueMap.clear();
                retCode = getUpdate(opID, version, false, valueMap);
                if (retCode == 1) {
                    root["upgradeurl"] = valueMap["source_url"];
                } else {
                    Athena::logger->trace("req inner update failed, return msg is %s", valueMap["return_msg"].c_str());
                }
            }
*/
			std::string content = writer.write(root);

			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
        else if (act == "/GetUpdate"){
			std::string gameID = parser.getReqParamVal("gameID");
			std::string opID  = parser.getReqParamVal("opID");
			std::string opGameID = parser.getReqParamVal("opGameID");
			std::string version = parser.getReqParamVal("version");
			UINT32 iGameID = (UINT32)atoi(gameID.c_str());
			//UINT32 iOpID = (UINT32)atoi(opID.c_str());
			UINT32 iOpGameID = (UINT32)atoi(opGameID.c_str());
			std::string upgradeURL = UpdateInfoMgr::getSingleton().getUpgradeURL(iGameID,iOpGameID,version);

			parser.clearHeader();
			Json::FastWriter writer;
			Json::Value root;
			root["status"] = 1;
			root["version"] = version;
			root["upgradeurl"] = upgradeURL;
			root["needUpdate"] = "false";
			root["appstoreUrl"] = "";

			UINT32 iOpID = (UINT32)atoi(opID.c_str());
			
			root["openAppVersion"] = "false";
			root["sanbox"] = "false";
			root["directBuy"] = "false";
			
			std::string  appReviewVer;
			std::string  curVer;
			std::string  appUrl;

			if (UpdateInfoMgr::getSingleton().getTotalUpdateInfo(iOpID,appReviewVer,curVer,appUrl)){
				if (appReviewVer == version){//等于审核版本
					if (COpIDMgr::getSingleton().isAppstore(iOpID)) {//appstore版本
						root["openAppVersion"] = UpdateInfoMgr::getSingleton().getOpenAppVersion();
						root["sanbox"] = UpdateInfoMgr::getSingleton().getSanbox();
						root["directBuy"] = UpdateInfoMgr::getSingleton().getDirectBuy();
					}
					else {//其它渠道不做任何事
					}
				}
				else {//不等于审核版本情况
					if (version < curVer){
						root["needUpdate"] = "true";
						root["appstoreUrl"] = appUrl;
					}
				}
			}
			
			std::string content = writer.write(root);

			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
		}
		else if (act == "/GetNotices"){
			std::string opID  = parser.getReqParamVal("opID");
			std::string opGameID = parser.getReqParamVal("opGameID");
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到的opID:%s,opGameID:%s",opID.c_str(),opGameID.c_str());
#endif
			parser.clearHeader();
			std::string content = CNoticeMgr::getSingleton().getNotices();
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
		}
        else if (act == "/get_recharge_sign") {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到get_recharge_sign请求:%s",parser.getPostContent().c_str());		
#endif
            std::string dataStr;
			std::vector<std::string> tmpVec, strVec;
            //std::map<std::string, std::string> kvSortMap;
			g_stringtok(tmpVec, parser.getPostContent(), "=");
            if (tmpVec.size() == 2) {
                dataStr = HttpUtility::URLDecode(tmpVec[1]);
                g_stringtok(strVec, dataStr, "&");
            } else {
                Athena::logger->error("not found paydata");
            }

            std::string appId;
            for (size_t i = 0; i < strVec.size(); i++) {
                if (strVec[i].find("appId") != std::string::npos) {
                    tmpVec.clear();
                    g_stringtok(tmpVec, strVec[i], "=");
                    if (tmpVec.size() == 2) {
                        appId = tmpVec[1];
                    }
                    break;
                }
            }
            std::string appKey;
            if (appId == std::string("1469700694430335")) {
                appKey = std::string("lxicBZ2g2g0znMo0HqnPBg0PdJgazoKV");
            } else if (appId == std::string("1476161596536869")) {
                appKey = std::string("GavMdwEZN6w33Jp5zH5mwS2aHOVgw78k");
            }
            int bufLen = appKey.length();
			char digest[64] = {0};
			utils::Md5(appKey.c_str(), bufLen, digest);
			utils::HexString::bin2asc(digest,16,digest,64,bufLen);
			digest[bufLen] = 0;
            std::stringstream os;
            os << dataStr << "&" << digest;
            std::string signStr = os.str();
            bufLen = signStr.length();
            char subDigest[64] = {0};
            utils::Md5(signStr.c_str(), bufLen, subDigest);
            utils::HexString::bin2asc(subDigest, 16, subDigest, 64, bufLen);
            subDigest[bufLen] = 0;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[get_recharge_sign]第二次md5计算签名时候的值:%s", subDigest);
#endif

			parser.clearHeader();
            os.str("");
            os << "mhtSignature=" << subDigest << "&mhtSignType=MD5";
			std::string content(os.str());
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[get_recharge_sign]return content is %s", content.c_str());
#endif
			parser.setStatus(true);
            parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
        else if (act == "/loginGetNo") {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到loginGetNo请求:%s",parser.getPostContent().c_str());		
#endif
            parser.clearHeader();
            Json::FastWriter writer;
            Json::Value root;
            root["userSession"] = UidMgr::getSingleton().generateUid();

			std::string content = writer.write(root);

			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
        else if (act == "/loginNotifyURL") {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到loginNotifyURL请求:%s",parser.getPostContent().c_str());		
#endif
			std::string loginNo = parser.getReqParamVal("loginNo");
            parser.clearHeader();
            Json::FastWriter writer;
            Json::Value root;
            bool res = UidMgr::getSingleton().existUid(loginNo);
            if (res) {
                root["isSuccess"] = "true";
                UidMgr::getSingleton().delUid(loginNo);
            } else {
                root["isSuccess"] = "false";
            }

			std::string content = writer.write(root);

			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
        else if (act == "/check_token") {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到check_token请求:%s",parser.getPostContent().c_str());		
#endif
			std::string token = parser.getReqParamVal("token");
            parser.clearHeader();

            std::string url("http://m.myappgo.com/databear/api/check_token.php?token=");
            CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
            pRequest->setRequestType(CHttpRequest::kHttpGet);
            char buf[1024] = { 0 };
            snprintf(buf, sizeof(buf) - 1, "%s%s", url.c_str(), token.c_str());
            pRequest->setUrl(buf);
            std::string configData, msg, uid;
            CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
            if (pRes && pRes->isSucceed()){
                std::vector<char> &retVec = *pRes->getResponseData();
                if (retVec.size() > 0){
                    configData.assign(&retVec[0],retVec.size());
                }
            }
            SAFE_DELETE(pRes);

			parser.setStatus(true);
			parser.setBody(configData.c_str(),configData.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
        else if (act == "/serverlist") {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到serverlist请求:%s",parser.getPostContent().c_str());		
#endif
            parser.clearHeader();
            std::map<std::string, OsServerInfo> idMap;
            getOsServers(idMap);

            std::stringstream ss;
            std::map<std::string, OsServerInfo>::iterator it = idMap.begin();
            if (it != idMap.end()) {
                ss << "[{'id':'" << it->second.id
                    << "','name':'" << it->second.name
                    << "'}";
                for (it++; it != idMap.end(); it++) {
                    ss << ",{'id':'" << it->second.id
                        << "','name':'" << it->second.name
                        << "'}";
                }
                ss << "]";
            } else {
                ss << "[]";
            }
            std::string content = ss.str();
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
        else if (act == "/role") {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到role请求:%s",parser.getPostContent().c_str());		
#endif
            parser.clearHeader();
			std::string account = parser.getReqParamVal("account");
			std::string serverId = parser.getReqParamVal("server");
#ifdef _HDZ_DEBUG
			Athena::logger->trace("account is %s, serverId is %s", account.c_str(), serverId.c_str());
#endif
            std::map<std::string, OsServerInfo> idMap;
            getOsServers(idMap);

            int webPort = 0;
            std::string content("{}");
            std::map<std::string, OsServerInfo>::iterator it = idMap.find(serverId);
            if (it != idMap.end()) {
                webPort = getWebPort(it->second.domain);
                if (webPort) {
                    char szQuery[1024] = { 0 };
                    snprintf(szQuery, sizeof(szQuery) - 1, "%s:%d/fetch_user?account=%s", it->second.domain.c_str(), webPort, account.c_str());
                    CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
                    pRequest->setRequestType(CHttpRequest::kHttpPost);
                    pRequest->setUrl(szQuery);
                    std::string configData;
                    CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
                    if (pRes && pRes->isSucceed()){
                        std::vector<char> &retVec = *pRes->getResponseData();
                        if (retVec.size() > 0){
                            configData.assign(&retVec[0],retVec.size());
                            content = configData;
                        }
                    }
                    SAFE_DELETE(pRes);
                }
            }

			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
        else if (act == "/GetPayParam") {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到GetPayParam请求:%s",parser.getPostContent().c_str());		
#endif
            parser.clearHeader();
			std::string serverId = parser.getReqParamVal("serverID");
			std::string roleId = parser.getReqParamVal("roleID");
#ifdef _HDZ_DEBUG
			Athena::logger->trace("roleId is %s, serverId is %s", roleId.c_str(), serverId.c_str());
#endif
            std::stringstream ss;
            ss << serverId << "_"
                << roleId << "_"
                << g_updateServer->getSec();
            std::string content = ss.str();
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
		else if (act == "/checkLogin"){//登陆验证
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到checkLogin");
#endif
            std::string sdk = parser.getReqParamVal("sdk");
            std::string app = parser.getReqParamVal("app");
            std::string uid = parser.getReqParamVal("uin");
            std::string sessionId = parser.getReqParamVal("sess");
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到checkLogin : sdk is %d, app is %d, uid is %s, sessionId is %d",
                    sdk.c_str(), app.c_str(), uid.c_str(), sessionId.c_str());
#endif
            parser.clearHeader();
            std::string newSdk = getNewStr(sdk);
            std::string newApp = getNewStr(app);

            std::stringstream ss;
            std::string bzUrl = std::string("http://sync.1sdk.cn/login/check.html");
            ss << bzUrl << "?sdk=" << newSdk << "&app=" << newApp << "&uin=" << HttpUtility::URLEncode(uid) << "&sess=" << HttpUtility::URLEncode(sessionId);
#ifdef _HDZ_DEBUG
            Athena::logger->trace("send to bz url is %s", ss.str().c_str());
#endif
            CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
            pRequest->setRequestType(CHttpRequest::kHttpGet);
            pRequest->setUrl(ss.str().c_str());
            std::string configData;
            CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
            if (pRes && pRes->isSucceed()){
                std::vector<char> &retVec = *pRes->getResponseData();
                if (retVec.size() > 0){
                    configData.assign(&retVec[0],retVec.size());
                }
            }
            SAFE_DELETE(pRes);
            Json::Value configJson;
            Json::Reader reader;

            ss.str("");
            if (configData == std::string("0")) {
                ss << "SUCCESS";
            } else {
                ss << "FAILED";
            }
            parser.setStatus(true);
            std::string content = ss.str();
            parser.setBody(content.c_str(),content.size());
            parser.setHeader("Content-Type","text/html; charset=UTF-8");
            std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
		else if (act == "/loginValid"){//登陆验证
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到loginValid");
#endif
            std::string accountId = parser.getReqParamVal("accountid");
            std::string sessionId = parser.getReqParamVal("sessionid");
            std::string gameId = parser.getReqParamVal("gameid");
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到 loginValid : accountId is %d, sessionId is %d, gameId is %d",
                    accountId.c_str(), sessionId.c_str(), gameId.c_str());
#endif
            parser.clearHeader();

            std::string appKey("eadd11f0246ce896");
            std::stringstream ss;
            ss << "accountid=" << accountId << "&gameid=" << gameId << "&sessionid=" << sessionId;

            char tokenBuf[1024] = { 0 };
            int  bufLen = 0;
            snprintf(tokenBuf, sizeof(tokenBuf) - 1, "%s%s", ss.str().c_str(), appKey.c_str());
            bufLen = strlen(tokenBuf);
            char digest[64] = {0};
            utils::Md5(tokenBuf,bufLen,digest);
            utils::HexString::bin2asc(digest,16,digest,64,bufLen);
            digest[bufLen] = 0;
            ss << "&sign=" << digest;

            std::string vaildUrl = std::string("http://asdk.ay99.net:8081/loginvalid.php?") + ss.str();
#ifdef _HDZ_DEBUG
            Athena::logger->trace("send to valid url is %s", vaildUrl.c_str());
#endif
            CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
            pRequest->setRequestType(CHttpRequest::kHttpGet);
            pRequest->setUrl(vaildUrl.c_str());
            std::string configData;
            CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
            if (pRes && pRes->isSucceed()){
                std::vector<char> &retVec = *pRes->getResponseData();
                if (retVec.size() > 0){
                    configData.assign(&retVec[0],retVec.size());
                }
            }
            SAFE_DELETE(pRes);
#ifdef _HDZ_DEBUG
            Athena::logger->trace("config data is %s", configData.c_str());
#endif
            Json::Value configJson;
            Json::Reader reader;

            parser.setStatus(true);
            std::string content = configData;
            parser.setBody(content.c_str(),content.size());
            parser.setHeader("Content-Type","text/html; charset=UTF-8");
            std::string ret = parser.buildResponse();
			g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
        }
		else {
			std::vector<std::string> strVec;
			g_stringtok(strVec,parser.getPostContent(),"&");
			std::string act;
			std::string data;
			std::string auth;
			std::string timeStr;
			
			for (UINT8 i = 0;i < strVec.size(); ++i){
				std::vector<std::string> subStrVec;
				g_stringtok(subStrVec,strVec[i],"=");
				if (subStrVec.size() >= 2){
					if (0 == i){
						act = subStrVec[1];
					}
					else if (1 == i){
						data = subStrVec[1];
					}
					else if (2 == i){
						auth = subStrVec[1];
					}
					else {
						timeStr = subStrVec[1];
					}
				}
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到请求解析后act=%s,data=%s,auth=%s,timeStr=%s",act.c_str(),data.c_str(),auth.c_str(),timeStr.c_str());
#endif
			
			Json::Reader reader;
			Json::Value valuePost;
			Json::Value searchCond;

			std::string unencodeStrSearch = g_decode64(HttpUtility::URLDecode(data).c_str());
#ifdef _HDZ_DEBUG
			Athena::logger->trace("decode64后data:%s",unencodeStrSearch.c_str());
#endif
			if (!reader.parse(unencodeStrSearch,searchCond)){
				Athena::logger->error("json解析失败!");
				return true;
			}	
#ifdef _HDZ_DEBUG
			Athena::logger->trace("解析后的data:%s",unencodeStrSearch.c_str());
#endif
			if (act == "publishPluginData"){
				parser.clearHeader();
				std::string pluginID;
				if (searchCond["plugin_id"].isString()){
					pluginID = searchCond["plugin_id"].asString();
				}

				if (pluginID == "noticePlugin"){//公告插件
					UINT32 noticeID = 0;
					std::string title;
					std::string content;
					UINT16 orderID = 0;

					if (searchCond["noticeID"].isString()){
						noticeID = (UINT32)atoi(searchCond["noticeID"].asString().c_str());
					}
					
					if (searchCond["noticeOrder"].isString()){
						orderID = (UINT32)atoi(searchCond["noticeOrder"].asString().c_str());
					}
					
					if (searchCond["noticeTtile"].isString()){
						title = searchCond["noticeTtile"].asString();
					}
					
					if (searchCond["noticeContent"].isString()){
						content = searchCond["noticeContent"].asString();
						content = HttpUtility::URLDecode(content);
#ifdef _HDZ_DEBUG
						Athena::logger->trace("公告的内容是:%s",content.c_str());
#endif
					}

					CNoticeMgr::getSingleton().save(noticeID,orderID,title,content);

					parser.clearHeader();
					Json::FastWriter writer;
					Json::Value root;
					root["status"] = 1;
					root["data"] = "发布公告成功!";
					std::string json_content = writer.write(root);
					parser.setStatus(true);
					parser.setBody(json_content.c_str(),json_content.size());
					parser.setHeader("Content-Type","text/html; charset=UTF-8");
					std::string ret = parser.buildResponse();
					g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
					Athena::logger->trace("处理了发布公告插件,返回值是:%s",ret.c_str());
#endif
				}
#ifdef _HDZ_DEBUG
				Athena::logger->trace("publishPluginData发布插件返回");
#endif
				return true;
			}
			else if (act == "delPluginData"){
				parser.clearHeader();
				std::string pluginID;
				if (searchCond["plugin_id"].isString()){
					pluginID = searchCond["plugin_id"].asString();
				}

				if (pluginID == "noticePlugin"){//公告插件
					UINT32 noticeID = 0;
					if (searchCond["noticeID"].isString()){
						noticeID = (UINT32)atoi(searchCond["noticeID"].asString().c_str());
					}
					CNoticeMgr::getSingleton().deleteNotice(noticeID);
					parser.clearHeader();
					Json::FastWriter writer;
					Json::Value root;
					root["status"] = 1;
					root["data"] = "发布公告成功!";
					std::string json_content = writer.write(root);
					parser.setStatus(true);
					parser.setBody(json_content.c_str(),json_content.size());
					parser.setHeader("Content-Type","text/html; charset=UTF-8");
					std::string ret = parser.buildResponse();
					g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
					Athena::logger->trace("处理了卸载公告插件,返回值是:%s",ret.c_str());
#endif
				}
#ifdef _HDZ_DEBUG
				Athena::logger->trace("delPluginDat下架插件返回");
#endif
				return true;
			}
			else {
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				root["data"] = "未响应改接口!";
				std::string json_content = writer.write(root);
				parser.setStatus(true);
				parser.setBody(json_content.c_str(),json_content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				g_updateServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("处理了接口:%s,返回值:%s",act.c_str(),ret.c_str());
#endif
			}
		}
		return true;
	}
	return false;
}


bool PacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了协议 modID=%d,funID=%d",header->m_modID,header->m_funID);
#endif

	if (PacketProcessorBase::processOneBufferMsg(pConn,header,pNull)){
		return true;
	}
	
	if (processHttpMsg(pConn,header,pNull)){
		return true;
	}

	ProtoMsgHandleBase * pModHandle = m_protoModHandles[header->m_modID];
	if (pModHandle) {
		if (pModHandle->handle(pConn,(char *)pNull,header->m_dataLen,header->m_funID)){
			return true;
		}
	}
	Athena::logger->error("%s (%u, %u, %u)", __PRETTY_FUNCTION__, header->m_modID, header->m_funID,header->m_dataLen);
	return false;
}
