#include "UidMgr.h"

#include "UpdateServer.h"
#include "UtilFunction.h"
#include "md5.h"

UidMgr::UidMgr() {
    char dict[27];
    for (int i = 0; i < 26; i++) {
        dict[i] = 'a' + i;
    }
    dict[26] = '\0';
    m_dictStr = dict;
}

std::string UidMgr::generateUid() {
    std::stringstream ss;
    ss << g_updateServer->getSec();
    std::string key = ss.str();
    int keyLen = key.length();
    int charLen = 26;
    char digest[64] = {0};
    utils::Md5(key.c_str(), keyLen, digest);
    utils::HexString::bin2asc(digest,16,digest,64,keyLen);
    digest[keyLen] = '\0';
    key = digest;
#ifdef _HDZ_DEBUG
    Athena::logger->trace("[generateUid]first key is %s", key.c_str());
#endif
    char buf[27];
    for (int i = 0; i < charLen; i++) {
        buf[i] = m_dictStr.at(rand() % charLen);
    }
    buf[charLen] = '\0';
#ifdef _HDZ_DEBUG
    Athena::logger->trace("[generateUid]buf is %s", buf);
#endif
    memset(digest, '\0', 64);
    utils::Md5(buf, charLen, digest);
    utils::HexString::bin2asc(digest,16,digest,64,charLen);
    digest[charLen] = '\0';
    key += digest;
	m_mutex.lock();
    bool res = m_uidSet.insert(key).second;
	m_mutex.unlock();
#ifdef _HDZ_DEBUG
    Athena::logger->trace("[generateUid]%s uid %s", (res ? "insert " : "not insert"), key.c_str());
#endif
    return key;
}

bool UidMgr::existUid(const std::string & uid) {
    bool res = false;
	m_mutex.lock();
    if (m_uidSet.find(uid) != m_uidSet.end()) {
        res = true;
    }
	m_mutex.unlock();
#ifdef _HDZ_DEBUG
    Athena::logger->trace("[existUid]%s uid %s", (res ? "find " : "not find"), uid.c_str());
#endif
    return res;
}

bool UidMgr::delUid(const std::string & uid) {
	m_mutex.lock();
    size_t count = m_uidSet.erase(uid);
    m_mutex.unlock();
#ifdef _HDZ_DEBUG
    Athena::logger->trace("[delUid]%s uid %s", (count ? "del " : "not find"), uid.c_str());
#endif
    return (bool)count;
}
