#include "TitleMgr.h"
#include "GameUser.h"
#include "Misc.pb.h"
#include "HistoryRankMgr.h"

std::map<UINT32,UINT32>  CTitleMgr::m_conditions;	//获取条件

CTitleMgr::CTitleMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_activeTitleID = 0;
}

CTitleMgr::~CTitleMgr()
{
	
}
	
UINT32 CTitleMgr::saveAllTitle(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	*(UINT16*)(buf + ret) = m_hasGotTitles.size();
	ret += sizeof(UINT16);
	std::set<UINT32>::iterator it =  m_hasGotTitles.begin();
	for (;it != m_hasGotTitles.end();++it){
		*(UINT32 *)(buf + ret) = *it;
		ret += sizeof(UINT32);
	}

	*(UINT32 *)(buf + ret) = m_activeTitleID;
	ret += sizeof(UINT32);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("保存(name=%s,id=%d)的称号",m_pOwner->getName(),m_pOwner->getID());
#endif
	return ret;
}

UINT32 CTitleMgr::loadAllTitle(byte * buf)
{
	UINT32 ret = 0;
	UINT16 num = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);
	for (UINT16 i = 0;i < num; ++i){
		m_hasGotTitles.insert(*(UINT32*)(buf + ret));
		ret += sizeof(UINT32);
	}

	m_activeTitleID = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载(name=%s,id=%d)的称号",m_pOwner->getName(),m_pOwner->getID());
#endif
	return ret;
}

void CTitleMgr::addTitle(UINT32 id)
{
	std::set<UINT32>::iterator it = m_hasGotTitles.find(id);
	if (it != m_hasGotTitles.end()){//已经存在的称号
		return ;
	}
	m_hasGotTitles.insert(id);

	m_pOwner->setupRoleState();
	m_pOwner->sendBattleProp();

	MiscProto::stObtainOneTitle retCmd;
	retCmd.set_titleid(id);
	m_pOwner->sendProto2Me(retCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name=%s,id=%d获得了一个称号id=%d",m_pOwner->getName(),m_pOwner->getID(),id);
#endif
}

void CTitleMgr::activeTitle(UINT32 id)
{
	MiscProto::stActiveTitleResult retCmd;
	std::set<UINT32>::iterator it = m_hasGotTitles.find(id);
	if (it == m_hasGotTitles.end()){		
		it = m_hasGotTitlesNotSaved.find(id);
		if (it == m_hasGotTitlesNotSaved.end()){
			retCmd.set_result(MiscProto::stActiveTitleResult::TITLE_NOT_EXIST);
			m_pOwner->sendProto2Me(retCmd);
			return ;
		}
	}
	
	m_activeTitleID = id;
	retCmd.set_result(MiscProto::stActiveTitleResult::SUCCESS);
	retCmd.set_titleid(id);
	m_pOwner->sendProto2Me(retCmd);
	m_pOwner->setupRoleState();
	//m_pOwner->sendMainData2Me();
	m_pOwner->sendBattleProp();
}

void CTitleMgr::cancelActiveTitle()
{
	m_activeTitleID = 0;
	MiscProto::stCancelActiveTitleResult retCmd;
	retCmd.set_success(true);
	m_pOwner->sendProto2Me(retCmd);
	m_pOwner->setupRoleState();
	//m_pOwner->sendMainData2Me();
	m_pOwner->sendBattleProp();
}

void CTitleMgr::addProp()
{
	stRoleState & roleState = m_pOwner->m_roleState;
	//设计修改, 由只有激活称号加属性,改为:全部获取的称号都加属性
	std::set<UINT32>::iterator it = m_hasGotTitles.begin();
	for (;it != m_hasGotTitles.end(); ++it){
		stTitleBase * pTitleBase = g_titleBaseMgr.get(*it);
		if (pTitleBase){
			for (UINT16 i = 0;i < pTitleBase->m_addProps.size(); ++i) {
				stTitleAddProp & prop = pTitleBase->m_addProps[i];
				if (prop.m_propID < eMAX_BATTLE_PROP){
					roleState.m_battleProps[prop.m_propID] += prop.m_propVal;	
				}
			}	
		}
	}
	it = m_hasGotTitlesNotSaved.begin();
	for (;it != m_hasGotTitlesNotSaved.end();++it){
		stTitleBase * pTitleBase = g_titleBaseMgr.get(*it);
		if (pTitleBase){
			for (UINT16 i = 0;i < pTitleBase->m_addProps.size(); ++i) {
				stTitleAddProp & prop = pTitleBase->m_addProps[i];
				if (prop.m_propID < eMAX_BATTLE_PROP){
					roleState.m_battleProps[prop.m_propID] += prop.m_propVal;	
				}
			}	
		}
	}
}

void CTitleMgr::userOnline()
{
	CHistoryRankMgr::getSingleton().checkGotTitle(m_pOwner);

	MiscProto::stSynMyTitle retCmd;
	std::set<UINT32>::iterator it = m_hasGotTitles.begin();
	for (;it != m_hasGotTitles.end(); ++it){
		retCmd.add_alltitle(*it);		
	}

	it = m_hasGotTitlesNotSaved.begin();
	for (;it != m_hasGotTitlesNotSaved.end();++it){
		retCmd.add_alltitle(*it);	
	}
	
	if ((m_hasGotTitles.find(m_activeTitleID) == m_hasGotTitles.end()) 
			&& (m_hasGotTitlesNotSaved.find(m_activeTitleID) == m_hasGotTitlesNotSaved.end())){
		m_activeTitleID = 0;
	}

	retCmd.set_activetitle(m_activeTitleID);
	m_pOwner->sendProto2Me(retCmd);
}

void CTitleMgr::checkObtainTitle(UINT16 type,UINT32 id,bool saved,bool bNotify)
{
	UINT32 findID = type * Title_Index_Coef + id; 
	std::map<UINT32,UINT32>::iterator it =  m_conditions.find(findID);	
	if (it != m_conditions.end()){
		if (saved){
			m_hasGotTitles.insert(it->second);	 
		}
		else {
			m_hasGotTitlesNotSaved.insert(it->second);
		}

		if( !g_gameServer->isPubGameServer()){
			m_pOwner->setupRoleState();
			m_pOwner->sendBattleProp();
		}

		if (bNotify) {
			MiscProto::stObtainOneTitle retCmd;
			retCmd.set_titleid(it->second);
			m_pOwner->sendProto2Me(retCmd);
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("玩家(name=%s,id=%d)获得了称号:%d",m_pOwner->getName(),m_pOwner->getID(),it->second);
#endif
	}
}

void CTitleMgr::delTitle(UINT32 titleID)
{
	if (titleID == m_activeTitleID)	{
		m_activeTitleID = 0;
		// m_pOwner->setupRoleState();
		// m_pOwner->sendBattleProp();
		//m_pOwner->sendMainData2Me();
	}

	m_hasGotTitles.erase(titleID);
	m_hasGotTitlesNotSaved.erase(titleID);

	m_pOwner->setupRoleState();
	m_pOwner->sendBattleProp();

	MiscProto::stNotifyDelTitle retCmd;
	retCmd.set_titleid(titleID);
	m_pOwner->sendProto2Me(retCmd);
}
	
UINT32 CTitleMgr::getTitleIDByGotCond(UINT16 type,UINT32 id)
{
	UINT32 findID = type * Title_Index_Coef + id; 
	std::map<UINT32,UINT32>::iterator it =  m_conditions.find(findID);	
	if (it != m_conditions.end()){
		return it->second;
	}
	return 0;
}

struct stTitleBaseParseCB : public TblDatabaseCB<stTitleBase>
{
	virtual bool doit(stTitleBase * pBase)
	{
		if (!pBase){
			return true;	
		}
		
		if (pBase->m_getConds.size() > 0){
			stGetTitleCond & cond = pBase->m_getConds[0];
			CTitleMgr::addGotTitleIndex(cond.m_cond,cond.m_value,pBase->m_titleID);
		}
		return true;
	}
};

void CTitleMgr::addGotTitleIndex(UINT16 type,UINT32 condID,UINT32 titleID)
{
	m_conditions[type * Title_Index_Coef + condID] = titleID;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("增加了一个称号 type=%d,condID=%d,titleID=%d",type,condID,titleID);
#endif
}

bool CTitleMgr::afterLoadTitleTbl()
{
	stTitleBaseParseCB cb;
	g_titleBaseMgr.execEveryOne(cb);
	return true;
}


void CTitleMgr::pack(PubGame::stTitleToPub* st) {
	st->set_activeid(this->m_activeTitleID);
    
	std::set<UINT32>::iterator it =  m_hasGotTitles.begin();
	for (;it != m_hasGotTitles.end();++it){
		st->add_gotsave(*it);
	}

	it = m_hasGotTitlesNotSaved.begin();
	for (;it != m_hasGotTitlesNotSaved.end();++it){
		st->add_notsave(*it);
	}
}

void CTitleMgr::parse(const PubGame::stTitleToPub& st) {

	m_activeTitleID = st.activeid();

	int saveCnt = st.gotsave_size();
	for (int i = 0; i < saveCnt; i++)
	{
		m_hasGotTitles.insert(st.gotsave(i));
	}

	m_hasGotTitlesNotSaved.clear();
	for (int i = 0; i < st.notsave_size(); i++)
	{
		m_hasGotTitlesNotSaved.insert(st.notsave(i));
	}

}
