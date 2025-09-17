#include "ExpeditionConfig.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool CExpeditionCfg::loadCfg() {
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/Expedition.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[远征副本]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[远征副本]找不到节点root");
		return false;
	}

	TiXmlElement * pExpedition = pRoot->FirstChildElement("Expedition");
    if (!pExpedition) {
        Athena::logger->error("[远征副本]找不到节点Expedition");
        return false;
    }
    TiXmlElement * pEntry = pExpedition->FirstChildElement("entry");
    if (!pEntry) {
        Athena::logger->error("[远征副本]找不到节点entry");
        return false;
    }

    bool lvlSet = false;
    INT32 maxEntryTimes = 0;
    std::string lvlStr;
	const char * szVal = NULL;
    std::vector<std::string> strVec;
    stExpeditionConfig expedtion;
    while (pEntry) {
        if (!(szVal = pEntry->Attribute("Lv"))) {
            Athena::logger->error("[远征副本]找不到节点Lv");
            return false;
        }
        strVec.clear();
        g_stringtok(strVec, szVal, "-");
        if (strVec.empty()) {
            Athena::logger->error("[远征副本]Lv节点数据分隔符错误");
            return false;
        }
        if (strVec.size() != 2) {
            Athena::logger->error("[远征副本]Lv节点数据不等于2");
            return false;
        }
        expedtion.m_minLvl = (UINT32)atoi(strVec[0].c_str());
        expedtion.m_maxLvl = (UINT32)atoi(strVec[1].c_str());
        if (!pEntry->Attribute("Dayurv", &maxEntryTimes)) {
            Athena::logger->error("[远征副本]Dayurv节点错误");
            return false;
        }
        if (lvlSet) {
            if (m_minLvl > expedtion.m_minLvl) {
                m_minLvl = expedtion.m_minLvl;
            }
            if (m_maxLvl < expedtion.m_maxLvl) {
                m_maxLvl = expedtion.m_maxLvl;
            }
        } else {
            lvlSet = true;
            m_minLvl = expedtion.m_minLvl;
            m_maxLvl = expedtion.m_maxLvl;
        }
        expedtion.m_maxTimes = maxEntryTimes;
        m_expeditionConfigVec.push_back(expedtion);
        pEntry = pEntry->NextSiblingElement("entry");
    }

	TiXmlElement * pQuickExpe = pRoot->FirstChildElement("quickExpedition");
    if (!pQuickExpe) {
        Athena::logger->error("[远征副本]找不到节点quickExpedition");
        return false;
    }
	TiXmlElement * pQuickEntry = pQuickExpe->FirstChildElement("entry");
    if (!pQuickEntry) {
        Athena::logger->error("[远征副本]quickExpedition下面找不到节点entry");
        return false;
    }
    INT32 baseTime = 1, baseDiamond = 1;
    if (!pQuickEntry->Attribute("BaseTime", &baseTime)) {
        Athena::logger->error("[远征副本]entry节点找不到BaseTime");
        return false;
    }
    if (!pQuickEntry->Attribute("Consume", &baseDiamond)) {
        Athena::logger->error("[远征副本]entry节点找不到Consume");
        return false;
    }
    m_baseTime = baseTime;
    m_baseDiamond = baseDiamond;

	TiXmlElement * pExpeditionTask = pRoot->FirstChildElement("Expeditiontask");
    if (!pExpeditionTask) {
        Athena::logger->error("[远征副本]找不到节点Expeditiontask");
        return false;
    }
	TiXmlElement * pTask = pExpeditionTask->FirstChildElement("task");
    if (!pTask) {
        Athena::logger->error("[远征副本]找不到节点task");
        return false;
    }
    INT32 taskId = 0, needTime = 0, typeId = 0;
    INT32 heroNum = 0, basePower = 0, extraNeedType = 0, extraNeedValue = 0;
    std::vector<UINT32> rewardIdVec, rewardNumVec;
    while (pTask) {
        stTask task;
        if (!pTask->Attribute("id", &taskId)) {
            Athena::logger->error("[远征副本]Dayurv节点错误");
            return false;
        }
        task.m_id = taskId;
        if (!(szVal = pTask->Attribute("Openlv"))) {
            Athena::logger->error("[远征副本]找不到节点Openlv");
            return false;
        }
        strVec.clear();
        g_stringtok(strVec, szVal, "-");
        if (strVec.empty()) {
            Athena::logger->error("[远征副本]OpenLv节点数据分隔符错误");
            return false;
        }
        if (strVec.size() != 2) {
            Athena::logger->error("[远征副本]OpenLv节点数据不等于2");
            return false;
        }
        task.m_minLvl = (UINT32)atoi(strVec[0].c_str());
        task.m_maxLvl = (UINT32)atoi(strVec[1].c_str());
        if (!pTask->Attribute("Time", &needTime)) {
            Athena::logger->error("[远征副本]找不到节点Time");
            return false;
        }
        task.m_needTime = needTime;
        if (!pTask->Attribute("Neednum", &heroNum)) {
            Athena::logger->error("[远征副本]找不到节点Neednum");
            return false;
        }
        task.m_heroNum = heroNum;
        if (!pTask->Attribute("BaseFightingCapacity", &basePower)) {
            Athena::logger->error("[远征副本]找不到节点BaseFightingCapacity");
            return false;
        }
        task.m_basePower = basePower;
        if (!(szVal = pTask->Attribute("BaseRewardID"))) {
            Athena::logger->error("[远征副本]找不到节点BaseRewardID");
            return false;
        }
        strVec.clear();
        g_stringtok(strVec, szVal, ";");
        if (strVec.empty()) {
            Athena::logger->error("[远征副本]BaseRewardID节点数据分隔符错误");
            return false;
        }
        rewardIdVec.clear();
        for (size_t i = 0; i < strVec.size(); i++) {
            rewardIdVec.push_back((UINT32)atoi(strVec[i].c_str()));
        }
        if (!(szVal = pTask->Attribute("BaseRewardNum"))) {
            Athena::logger->error("[远征副本]找不到节点BaseRewardNum");
            return false;
        }
        strVec.clear();
        g_stringtok(strVec, szVal, ";");
        if (strVec.empty()) {
            Athena::logger->error("[远征副本]BaseRewardNum节点数据分隔符错误");
            return false;
        }
        rewardNumVec.clear();
        for (size_t i = 0; i < strVec.size(); i++) {
            rewardNumVec.push_back((UINT32)atoi(strVec[i].c_str()));
        }
        if (rewardIdVec.size() != rewardNumVec.size() ||
                !rewardIdVec.size()) {
            Athena::logger->error("[远征副本]task reward id and num not match");
            return false;
        }
        for (size_t i = 0; i < rewardIdVec.size(); i++) {
            task.m_baseRewardMap.insert(std::make_pair(rewardIdVec[i], rewardNumVec[i]));
        }
        if (!(szVal = pTask->Attribute("ExtraRewardID"))) {
            Athena::logger->error("[远征副本]找不到节点ExtraRewardID");
            return false;
        }
        strVec.clear();
        g_stringtok(strVec, szVal, ";");
        if (strVec.empty()) {
            Athena::logger->error("[远征副本]ExtraRewardID节点数据分隔符错误");
            return false;
        }
        rewardIdVec.clear();
        for (size_t i = 0; i < strVec.size(); i++) {
            rewardIdVec.push_back((UINT32)atoi(strVec[i].c_str()));
        }
        if (!(szVal = pTask->Attribute("ExtraRewardNum"))) {
            Athena::logger->error("[远征副本]找不到节点ExtraRewardNum");
            return false;
        }
        strVec.clear();
        g_stringtok(strVec, szVal, ";");
        if (strVec.empty()) {
            Athena::logger->error("[远征副本]ExtraRewardNum节点数据分隔符错误");
            return false;
        }
        rewardNumVec.clear();
        for (size_t i = 0; i < strVec.size(); i++) {
            rewardNumVec.push_back((UINT32)atoi(strVec[i].c_str()));
        }
        if (rewardIdVec.size() != rewardNumVec.size() ||
                !rewardIdVec.size()) {
            Athena::logger->error("[远征副本]task extra reward id and num not match");
            return false;
        }
        for (size_t i = 0; i < rewardIdVec.size(); i++) {
            task.m_extraRewardMap.insert(std::make_pair(rewardIdVec[i], rewardNumVec[i]));
        }
        if (!pTask->Attribute("typeID", &typeId)) {
            Athena::logger->error("[远征副本]找不到节点typeId");
            return false;
        }
        task.m_type = typeId;
        TiXmlElement * pExtra = pTask->FirstChildElement("Extra");
        while (pExtra) {
            if (!pExtra->Attribute("Needtype", &extraNeedType)) {
                Athena::logger->error("[远征副本]找不到Needtype");
                return false;
            }
            if (!pExtra->Attribute("ExtraNeedvalue", &extraNeedValue)) {
                Athena::logger->error("[远征副本]找不到ExtraNeedvalue");
                return false;
            }
            task.m_extraConditionMap.insert(std::make_pair(extraNeedType, extraNeedValue));
            pExtra = pExtra->NextSiblingElement("Extra");
        }
        m_taskMap.insert(std::make_pair(taskId, task));
        pTask = pTask->NextSiblingElement("task");
    }
    return true;
}

CExpeditionCfg::~CExpeditionCfg() {

}

UINT32 CExpeditionCfg::getMaxTime(UINT32 level) {
    for (std::vector<stExpeditionConfig>::iterator it =
            m_expeditionConfigVec.begin(); it != m_expeditionConfigVec.end(); it++) {
        if (level >= (*it).m_minLvl && level <= (*it).m_maxLvl) {
            return (*it).m_maxTimes;
        }
    }
    return 0;
}

const CExpeditionCfg::stTask * CExpeditionCfg::getTask(UINT32 id) {
    std::map<UINT32, stTask>::iterator it = m_taskMap.find(id);
    if (it != m_taskMap.end()) {
        return &it->second;
    }
    return NULL;
}

bool CExpeditionCfg::hasSameTask(UINT32 targetType, std::vector<UINT16> & idVec) {
    for (size_t i = 0; i < idVec.size(); i++) {
        std::map<UINT32, stTask>::iterator it = m_taskMap.find(idVec[i]);
        if (it == m_taskMap.end()) {
            continue ;
        }
        if (targetType == it->second.m_type) {
            return true;
        }
    }
    return false;
}
