#include "QuestInvestigateMgr.h"
#include "GameUser.h"
#include "QuestionCfg.h"


CQuestInvestigateMgr::CQuestInvestigateMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
}

CQuestInvestigateMgr::~CQuestInvestigateMgr()
{
	
}
	
UINT32 CQuestInvestigateMgr::saveAll(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	//保存问卷调查数量
	*(UINT16 *)(buf + ret) = m_questInvestigateInfos.size();
	ret += sizeof(UINT16);

	std::map<UINT16,std::set<UINT16> >::iterator it =  m_questInvestigateInfos.begin();
	for (;it != m_questInvestigateInfos.end();++it){
		//保存questID
		*(UINT16 *)(buf + ret) = it->first;
		ret += sizeof(UINT16);
		
		//等级数量
		*(UINT16 *)(buf + ret) = it->second.size();
		ret += sizeof(UINT16);
		std::set<UINT16>::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			*(UINT16 *)(buf + ret) = *subIt;
			ret += sizeof(UINT16);
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("保存(name=%s,id=%d)的问卷,大小:%d",m_pOwner->getName(),m_pOwner->getID(),ret);
#endif
	return ret;
}

UINT32 CQuestInvestigateMgr::loadAll(byte * buf)
{
	UINT32 ret = 0;
	//问卷个数
	UINT16 size = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);

	for (UINT16 i = 0;i < size;++i){
		//问卷ID
		UINT16 questID = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);
		//等级数量
		UINT16 subSize = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);
		
		for (UINT16 j = 0;j < subSize; ++j){
			m_questInvestigateInfos[questID].insert(*(UINT16 *)(buf + ret));
			ret += sizeof(UINT16);
		}
	}	
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载(name=%s,id=%d)的问卷,大小:%d",m_pOwner->getName(),m_pOwner->getID(),ret);
#endif
	return ret;
}

void CQuestInvestigateMgr::userOnline()
{
	MiscProto::stSynAllInvestigation retCmd;
	
	std::map<UINT16,std::set<UINT16> >::iterator it =  m_questInvestigateInfos.begin();
	
	for (;it != m_questInvestigateInfos.end();++it){
		MiscProto::stSynInvestigation *pSyn = retCmd.add_infos();
		if (pSyn){
			pSyn->set_questid(it->first);
			std::set<UINT16>::iterator subIt = it->second.begin();
			for (;subIt != it->second.end();++subIt){
				pSyn->add_levels(*subIt);
			}
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}

bool CQuestInvestigateMgr::giveReward(UINT16 questID,UINT16 level)
{

	MiscProto::stInvestgationRewardResult retCmd;
	std::map<UINT16,std::set<UINT16> >::iterator it = m_questInvestigateInfos.find(questID);
	if (it != m_questInvestigateInfos.end()){
		std::set<UINT16>::iterator subIt = it->second.find(level);
		if (subIt != it->second.end()){
			retCmd.set_result(MiscProto::stInvestgationRewardResult::eHas_Got_Reward);
			m_pOwner->sendProto2Me(retCmd);
			return false;
		}
	}
	
	UINT32 boxID = CQuestionCfg::getSingleton().getRewardBoxID(questID,level);
	if (!boxID){
		retCmd.set_result(MiscProto::stInvestgationRewardResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return false;	
	}

	if (m_pOwner->m_boxMgr.needSlotNum(boxID) > m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1)){
		retCmd.set_result(MiscProto::stInvestgationRewardResult::eBag_Full);
		m_pOwner->sendProto2Me(retCmd);
		return false;	
	}
	
	m_questInvestigateInfos[questID].insert(level);
	m_pOwner->m_boxMgr.openBox(boxID);
	retCmd.set_result(MiscProto::stInvestgationRewardResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
	return true;
}
