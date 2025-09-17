#include "UpdateCache.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "HttpParser.h"
#include "json/reader.h"
#include "md5.h"
#include "UtilFunction.h"
#include "HttpClient.h"
#include "UpdateServer.h"


std::map<std::string, std::map<std::string, std::string> > UpdateCacheMgr::m_cacheMap;

int UpdateCacheMgr::getUpdate(const std::string & partnerId, const std::string & version, bool totalUpdate,
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

void UpdateCacheMgr::addRecord(const std::string & partnerId, const std::string & version, bool totalUpdate,
                                std::map<std::string, std::string> & valueMap) {
    std::stringstream ss;
    ss << partnerId << '&' << version << '&' << (UINT32)totalUpdate;
    std::string key = ss.str();

	m_mutex.lock();
    std::map<std::string, std::map<std::string, std::string> >::iterator it =
            m_cacheMap.find(key);
    if (it != m_cacheMap.end()) {
        it->second = valueMap;
    } else {
        m_cacheMap.insert(std::make_pair(key, valueMap));
    }
	m_mutex.unlock();
}

void UpdateCacheMgr::getRecord(const std::string & partnerId, const std::string & version, bool totalUpdate,
                                std::map<std::string, std::string> & valueMap) {
    std::stringstream ss;
    ss << partnerId << '&' << version << '&' << (UINT32)totalUpdate;
    std::string key = ss.str();

	m_mutex.lock();
    std::map<std::string, std::map<std::string, std::string> >::iterator it =
            m_cacheMap.find(key);
    if (it != m_cacheMap.end()) {
        valueMap = it->second;
    }
	m_mutex.unlock();
}

void UpdateCacheMgr::timer(UINT32 cur) {
#ifdef _HDZ_DEBUG
    Athena::logger->trace("timer : now is %d", cur);
#endif
    bool totalUpdate = false;
    int retCode = 0;
    std::vector<std::string> strVec;
    std::map<std::string, std::string> valueMap;
	m_mutex.lock();
    for (std::map<std::string, std::map<std::string, std::string> >::iterator it =
            m_cacheMap.begin(); it != m_cacheMap.end(); it++) {
        strVec.clear();
        g_stringtok(strVec, it->first, "&");
        if (strVec.size() != 3) {
			Athena::logger->trace("invalid param %s, size is %d", it->first.c_str(), strVec.size());
            continue;
        }
        totalUpdate = bool(atoi(strVec[2].c_str()));
        valueMap.clear();
        retCode = getUpdate(strVec[0], strVec[1], totalUpdate, valueMap);
        if (retCode == 1) {
            it->second = valueMap;
        } else {
            Athena::logger->trace("req %s failed, return msg is %s", (totalUpdate ? "total update" :  "inner update"), valueMap["return_msg"].c_str());
        }
    }
	m_mutex.unlock();
}
