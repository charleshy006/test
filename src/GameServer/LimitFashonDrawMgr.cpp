#include "LimitFashonDrawMgr.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "GameServer.h"
#include "LimitFashionCfg.h"
#include "GameUser.h"


extern GameServer * g_gameServer;

UINT32 CLimitFashonDrawMgr::save(byte * buf)
{
	UINT32 ret = 0;
	//保存版本
	*(UINT32 *)(buf + ret) = s_SAVE_VERSION_INFO;
	ret += sizeof(UINT32);
	//可以抽奖次数	
	*(UINT16 *)(buf + ret)  = m_drawNum;
	ret += sizeof(UINT16);
	//已经抽的东西
	*(UINT16 *)(buf + ret) = m_drawSets.size(); 
	ret += sizeof(UINT16);
	std::set<UINT16>::iterator it = m_drawSets.begin();
	for (;it != m_drawSets.end();++it){
		*(UINT16 *)(buf + ret) = *it;
		ret += sizeof(UINT16);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("CLimitFashonDrawMgr::save %d",ret);
#endif
	return ret;
}

UINT32 CLimitFashonDrawMgr::load(byte * buf)
{
	UINT32 ret = 0;
	//读版本
	UINT32 version = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	if (version == s_SAVE_VERSION_INFO){
		//已经抽次数
		m_drawNum = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);
		
		//次数
		UINT16 num = *(UINT16 *)(buf + ret);
		ret += sizeof(UINT16);
		for (UINT16 i = 0;i < num;++i){
			UINT16 id = *(UINT16 *)(buf + ret);
			ret += sizeof(UINT16);
			m_drawSets.insert(id);
		}
	}
	else {
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("CLimitFashonDrawMgr::load %d",ret);
#endif
	return ret;
}

UINT32 CLimitFashonDrawMgr::getSaveSize()
{
	UINT32 ret = 0;
	ret = sizeof(UINT32) + sizeof(UINT16) + sizeof(UINT16) +sizeof(UINT16) * m_drawSets.size(); 
	return ret;
}


CLimitFashonDrawMgr::CLimitFashonDrawMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_drawNum = 0;
}

CLimitFashonDrawMgr::~CLimitFashonDrawMgr()
{

}
	
void CLimitFashonDrawMgr::openDrawWin()
{
	MiscProto::stRetLimitFashionDrawWinInfo retCmd;
	retCmd.set_candrawnum(m_drawNum);

	std::set<UINT16>::iterator it = m_drawSets.begin();
	for (;it != m_drawSets.end();++it){
		retCmd.add_hasdrawedid(*it);	
	}
	m_pOwner->sendProto2Me(retCmd);
}

void CLimitFashonDrawMgr::draw()
{
	MiscProto::stDrawLimitFashionResult retCmd;
	
	if (m_drawNum <= 0){
		retCmd.set_result(MiscProto::stDrawLimitFashionResult::eHas_Not_Draw_Num);
		m_pOwner->sendProto2Me(retCmd);
		return;
	}	

	if (!CLimitFashionCfg::getSingleton().isActivityOpen(g_gameServer->getSec())){
		retCmd.set_result(MiscProto::stDrawLimitFashionResult::eActivity_Not_Open);
		m_pOwner->sendProto2Me(retCmd);
		return;
	}
	

	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) < 1){
		retCmd.set_result(MiscProto::stDrawLimitFashionResult::eActivity_Not_Open);
		m_pOwner->sendProto2Me(retCmd);
		return;
	}
	
	CLimitFashionCfg::stCfg & cfg = CLimitFashionCfg::getSingleton().getCfg();
	if (cfg.m_randItems.size() <= m_drawSets.size()){
		retCmd.set_result(MiscProto::stDrawLimitFashionResult::eDraw_Out);
		m_pOwner->sendProto2Me(retCmd);
		return;
	}

	retCmd.set_result(MiscProto::stDrawLimitFashionResult::eSuccess);


	std::map<UINT32,UINT32> id2Rates;
	UINT32 totalRate = 0;

	std::map<UINT16,CLimitFashionCfg::stItem>::iterator it = cfg.m_randItems.begin();
	for (;it != cfg.m_randItems.end();++it){
		CLimitFashionCfg::stItem & item = it->second;
		if (m_drawSets.find(item.m_id) == m_drawSets.end()){
			id2Rates.insert(std::make_pair(item.m_id,item.m_rate));
			totalRate += item.m_rate;
		}
	}

	UINT16 id = g_selectByMap(id2Rates,totalRate);
	retCmd.set_id(id);

	it = cfg.m_randItems.find(id);
	if (it != cfg.m_randItems.end()){
		m_pOwner->giveAssert(it->second.m_itemID,it->second.m_itemNum,0,"限制时装抽奖");	
	}
	m_drawNum -= 1;
	m_drawSets.insert(id);
	m_pOwner->sendProto2Me(retCmd);	
}
