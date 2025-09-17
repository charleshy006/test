#include "InviteFriendMgr.h"

#include "Log4cxxHelp.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "Misc.pb.h"


#include <sstream>

extern GlobalServer  * g_globalServer;

CTblField g_inviteFriendTblDef[] = { 
	{ "ID",				    CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ "ACCOUNT",	        CTblField::DB_TYPE_STRING,			MAX_ACCNAMESIZE + 1 },
	{ "INVITE_CODE",	    CTblField::DB_TYPE_STRING,			INVITE_CODE_LEN     },
	{ "INVITE_COUNT",	    CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ "INVITE_REWARD",	    CTblField::DB_TYPE_STRING,			INVITE_REWARD_LEN   },
	{ "BE_INVITED",	        CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ NULL, 0, 0 },
};

void InviteFriend::fillDBData(stInviteFriendInfoDB & data) {
    data.id = m_id;
    memcpy(data.account, m_account.c_str(), m_account.length());
    memcpy(data.code, m_inviteCode.c_str(), m_inviteCode.length());
    data.inviteCount = m_inviteCount;
    std::map<UINT8, bool>::iterator it = m_inviteRewardMap.begin();
    std::stringstream ss;
    if (it != m_inviteRewardMap.end()) {
        ss << (UINT32)it->first << ","
            << (UINT32)it->second;
        for (it++; it != m_inviteRewardMap.end(); it++) {
            ss << ";" << (UINT32)it->first << ","
                << (UINT32)it->second;
        }
    }
    std::string rewardStr = ss.str();
    memcpy(data.inviteReward, rewardStr.c_str(), rewardStr.length());
    data.beInvited = (UINT32)m_beInvited;
}

void InviteFriend::initFromDBData(const stInviteFriendInfoDB & data) {
    m_id = data.id;
    m_account = data.account;
    m_inviteCode = data.code;
    m_inviteCount = data.inviteCount;
    if (data.inviteReward) {
        std::vector<std::string> strVec, rewardVec;
        g_stringtok(strVec, data.inviteReward, ";");//param 2 seems should not be null
        for (size_t i = 0; i < strVec.size(); i++) {
            rewardVec.clear();
            g_stringtok(rewardVec, strVec[i], ",");
            if (rewardVec.size() != 2) {
                Athena::logger->error("InviteFriend::initFromDBData reward vec size is not 2");
                continue ;
            }
            UINT8 id = atoi(rewardVec[0].c_str());
            bool drawReward = (bool)atoi(rewardVec[1].c_str());
            m_inviteRewardMap.insert(std::make_pair(id, drawReward));
        }
    }
    m_beInvited = (bool)data.beInvited;
#ifdef _HDZ_DEBUG
    Athena::logger->trace("InviteFriend::initFromDBData id is %d, account is %s, code is %s, inviteCount is %d, beInvited is %d",
            m_id, m_account.c_str(), m_inviteCode.c_str(), m_inviteCount, (UINT32)m_beInvited);
#endif
}

void InviteFriend::sendData(Player * pUser) {
    MiscProto::stRetInviteFriend cmd;
    cmd.set_code(m_inviteCode);
    cmd.set_num(m_inviteCount);
    cmd.set_useinvitecode(m_beInvited);
    MiscProto::InviteFriendReward * pReward = NULL;
    for (std::map<UINT8, bool>::iterator it = m_inviteRewardMap.begin();
            it != m_inviteRewardMap.end(); it++) {
        pReward = cmd.add_reward();
        if (pReward) {
            pReward->set_id(it->first);
            pReward->set_status((UINT32)it->second);
        }
    }
	pUser->sendProtoToMe(cmd);
}

bool InviteFriend::isDrawReward(UINT8 id) {
    std::map<UINT8, bool>::iterator it = m_inviteRewardMap.find(id);
    if (it != m_inviteRewardMap.end()) {
        return it->second;
    }
    return false;
}

void InviteFriend::drawReward(UINT8 id) {
    std::map<UINT8, bool>::iterator it = m_inviteRewardMap.find(id);
    if (it != m_inviteRewardMap.end()) {
        it->second = true;
    } else {
        m_inviteRewardMap.insert(std::make_pair(id, true));
    }
}

CInviteFriendMgr::~CInviteFriendMgr() {
    for (std::map<std::string, InviteFriend* >::iterator it = m_inviteMap.begin();
            it != m_inviteMap.end(); it++) {
        SAFE_DELETE(it->second);
    }
}

void CInviteFriendMgr::load() {
	CTblQueryResult* result = g_databaseEngine.select(g_inviteFriendTblDef, "INVITE_INFO",NULL,NULL,0,NULL); 
	if (result && result->getData()) {
        stInviteFriendInfoDB * db = (stInviteFriendInfoDB*) result->getData();
        for (UINT16 i = 0;i < result->getRowCount();++i){
            InviteFriend * pItem = ATHENA_NEW InviteFriend();
            pItem->initFromDBData(db[i]);
            m_inviteMap.insert(std::make_pair(db[i].account, pItem));
            m_codeMap.insert(std::make_pair(pItem->getCode(), pItem->getAccount()));
        }
    }
	SAFE_DELETE(result);
}

void CInviteFriendMgr::insertRecord(const stInviteFriendInfoDB & data, Player * pUser) {
	Athena::CUDSqlCallBack<CInviteFriendMgr, stInviteFriendInfoDB, Player *> * pDelayCB =
        ATHENA_NEW Athena::CUDSqlCallBack<CInviteFriendMgr, stInviteFriendInfoDB, Player *>(this,&CInviteFriendMgr::insertRecordCB,false,0, data, pUser);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_inviteFriendTblDef,(const char *)&data,"INVITE_INFO",autoID,pDelaySql);
}

void CInviteFriendMgr::insertRecordCB(bool opResult,UINT64 autoID, stInviteFriendInfoDB data, Player * pUser) {
    if (opResult) {
        data.id = autoID;
        UINT32 code = (1 << 18) | data.id;
        std::string codeStr;
        std::stringstream ss;
        ss << code;
        codeStr = ss.str();
        ss.str("");
#ifdef _HDZ_DEBUG
        Athena::logger->trace("code is %s", codeStr.c_str());
#endif
        for (size_t k = 0; k < codeStr.length() && k < 6; k++) {
            ss << (char)(codeStr[k] + 17 + k * 3);//convert to A...Z
        }
        ss << getGameId();
        codeStr = ss.str();
        memcpy(data.code, codeStr.c_str(), codeStr.length());
#ifdef _HDZ_DEBUG
        Athena::logger->trace("codeStr is %s, final code, beInvited is %d", codeStr.c_str(), data.beInvited);
#endif
        InviteFriend * pItem = ATHENA_NEW InviteFriend();
        if (m_inviteMap.insert(std::make_pair(data.account, pItem)).second && pItem) {
            pItem->initFromDBData(data);
            m_codeMap.insert(std::make_pair(pItem->getCode(), pItem->getAccount()));
            pItem->sendData(pUser);
        } else {
            Athena::logger->error("CInviteFriendMgr::insertRecord insert failed");
        }
        updateRecord(data);
    }
}

void CInviteFriendMgr::updateRecord(const stInviteFriendInfoDB & data) {
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%u", data.id);
	
	Athena::CUDSqlCallBack<CInviteFriendMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CInviteFriendMgr>(this,&CInviteFriendMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_inviteFriendTblDef,(const char *)&data,"INVITE_INFO",where,pDelaySql);
}

void CInviteFriendMgr::updateRecordCB(bool opResult,UINT64 autoID) {
}

InviteFriend * CInviteFriendMgr::getInviteFriend(const std::string & account) {
    std::map<std::string, InviteFriend* >::iterator it =
            m_inviteMap.find(account);
    if (it != m_inviteMap.end()) {
        return it->second;
    }
    return NULL;
}

void CInviteFriendMgr::addInviteFriend(const std::string & account, Player * pUser) {
    stInviteFriendInfoDB data;
    memcpy(data.account, account.c_str(), account.length());
    insertRecord(data, pUser);
}

std::string CInviteFriendMgr::getGameId() {
    std::string gameId = Athena::global["gameID"];
    if (gameId.length() < 4) {
        char zero[] = "0000";
        zero[4 - gameId.length()] = '\0';
        gameId = zero + gameId; 
    } else if (gameId.length() > 4) {
        gameId = gameId.substr(0, 4);
    }
    return gameId;
}

bool CInviteFriendMgr::checkCode(const std::string & code) {
    if (code.length() != 10) {
        Athena::logger->error("checkCode : code length erro");
        return false;
    }
    size_t k = 0;
    char chA = 'A';
    for (; k < 6; k++) {
        if (code[k] < (char)(chA + k * 3) ||
                code[k] > (char)(chA + k * 3 + 10)) {
            Athena::logger->error("checkCode : left code invalid");
            return false;
        }
    }
    for (k = 6; k < 10; k++) {
        if (code[k] < (char)'0' || code[k] > (char)'9') {
            Athena::logger->error("checkCode : right gameId invalid");
            return false;
        }
    }
    return true;
}

bool CInviteFriendMgr::getOwner(const std::string & code, std::string & owner) {
    std::map<std::string, std::string>::iterator it = m_codeMap.find(code);
    if (it != m_codeMap.end()) {
        owner = it->second;
        return true;
    }
    return false;
}
