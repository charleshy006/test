#include "InviteCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool CInviteCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/invitefriend.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[分享好友]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[分享好友]找不到节点root");
		return false;
	}

	TiXmlElement * pLimitLvl = pRoot->FirstChildElement("LimitLevel");
    if (!pLimitLvl) {
        Athena::logger->error("[分享好友]找不到节点LimitLevel");
        return false;
    }
	TiXmlElement * pEntry = pLimitLvl->FirstChildElement("entry");
    if (!pEntry) {
        Athena::logger->error("[分享好友]找不到节点entry");
        return false;
    }
    int val = 0;
    if (!pEntry->Attribute("level", &val)) {
        Athena::logger->error("[分享好友]level节点找不到level");
        return false;
    }
    m_limitLevel = val;
	TiXmlElement * pInvitedReward = pRoot->FirstChildElement("InvitationAward");
    if (!pInvitedReward) {
        Athena::logger->error("[分享好友]找不到节点InvitationAward");
        return false;
    }
	TiXmlElement * pReward = pInvitedReward->FirstChildElement("reward");
    if (!pReward) {
        Athena::logger->error("[分享好友]找不到节点reward");
        return false;
    }
    if (!pReward->Attribute("RewardBoxid", &val)) {
        Athena::logger->error("[分享好友]InvitationAward节点找不到RewardBoxid");
        return false;
    }
    m_invitedRewardBox = val;
	TiXmlElement * pInviteReward = pRoot->FirstChildElement("BeInvitationRward");
    if (!pInviteReward) {
        Athena::logger->error("[分享好友]找不到节点BeInvitationRward");
        return false;
    }
	pReward = pInviteReward->FirstChildElement("reward");
    if (!pReward) {
        Athena::logger->error("[分享好友]找不到节点reward");
        return false;
    }
    while (pReward) {
        InviteReward reward;
        if (!pReward->Attribute("id", &val)) {
            Athena::logger->error("[分享好友]id节点找不到id");
            return false;
        }
        reward.id = val;
        if (!pReward->Attribute("InvitationNum", &val)) {
            Athena::logger->error("[分享好友]InvitationNum节点找不到InvitationNum");
            return false;
        }
        reward.inviteNum = val;
        if (!pReward->Attribute("RewardBoxid", &val)) {
            Athena::logger->error("[分享好友]BeInvitationRward节点找不到RewardBoxid");
            return false;
        }
        reward.rewardBox = val;
        m_inviteRewardMap.insert(std::make_pair(reward.id, reward));
        pReward = pReward->NextSiblingElement("reward");
    }
	TiXmlElement * pDailyShare = pRoot->FirstChildElement("DailyShareReward");
    if (!pDailyShare) {
        Athena::logger->error("[分享好友]找不到节点DailyShareReward");
        return false;
    }
	pReward = pDailyShare->FirstChildElement("reward");
    if (!pReward) {
        Athena::logger->error("[分享好友]找不到节点reward");
        return false;
    }
    if (!pReward->Attribute("RewardBoxid", &val)) {
        Athena::logger->error("[分享好友]DailyShareReward节点找不到RewardBoxid");
        return false;
    }
    m_dailySharedRewardBox = val;
	TiXmlElement * pTotalShare = pRoot->FirstChildElement("TotalShareReward");
    if (!pTotalShare) {
        Athena::logger->error("[分享好友]找不到节点TotalShareReward");
        return false;
    }
	pReward = pTotalShare->FirstChildElement("reward");
    if (!pReward) {
        Athena::logger->error("[分享好友]找不到节点reward");
        return false;
    }
    while (pReward) {
        ShareReward reward;
        if (!pReward->Attribute("id", &val)) {
            Athena::logger->error("[分享好友]id节点找不到id");
            return false;
        }
        reward.id = val;
        if (!pReward->Attribute("ShareTimes", &val)) {
            Athena::logger->error("[分享好友]ShareTimes节点找不到ShareTimes");
            return false;
        }
        reward.shareNum = val;
        if (!pReward->Attribute("RewardBoxid", &val)) {
            Athena::logger->error("[分享好友]TotalShareReward节点找不到RewardBoxid");
            return false;
        }
        reward.rewardBox = val;
        m_totalShareRewardMap.insert(std::make_pair(reward.id, reward));
        pReward = pReward->NextSiblingElement("reward");
    }
#ifdef _HDZ_DEBUG
    Athena::logger->trace("[分享好友]limit level is %d, invited reward box id is %d", m_limitLevel, m_invitedRewardBox);
    for (std::map<UINT8, InviteReward>::iterator it = m_inviteRewardMap.begin();
            it != m_inviteRewardMap.end(); it++) {
        Athena::logger->trace("[分享好友]id is %d, invite num is %d, reward box id is %d",
                it->second.id, it->second.inviteNum, it->second.rewardBox);
    }
    Athena::logger->trace("[分享好友]daily reward box id is %d", m_dailySharedRewardBox);
    for (std::map<UINT8, ShareReward>::iterator it = m_totalShareRewardMap.begin();
            it != m_totalShareRewardMap.end(); it++) {
        Athena::logger->trace("[分享好友]id is %d, share num is %d, reward box id is %d",
                it->second.id, it->second.shareNum, it->second.rewardBox);
    }
#endif
    return true;
}

const CInviteCfg::InviteReward * CInviteCfg::getInviteReward(UINT8 id) {
    std::map<UINT8, InviteReward>::iterator it =
            m_inviteRewardMap.find(id);
    if (it != m_inviteRewardMap.end()) {
        return &it->second;
    }
    return NULL;
}

const CInviteCfg::ShareReward * CInviteCfg::getSharedReward(UINT8 id) {
    std::map<UINT8, ShareReward>::iterator it =
            m_totalShareRewardMap.find(id);
    if (it != m_totalShareRewardMap.end()) {
        return &it->second;
    }
    return NULL;
}
