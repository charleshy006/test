#include "LeitaiMgr.h"
#include "GameUser.h"
#include "SortList.pb.h"
#include "GlobalSvrClient.h"
#include "ArenaConfig.h"

CLeitaiMgr::CLeitaiMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_refreshTime = 0;
	m_playerCurHP = 0;
	m_pkMode = 0;
	bzero(m_battleHeroIDs,sizeof(m_battleHeroIDs));
	m_challengeCharID = 0;
	m_lastRefreshBuffTime = 0;
	m_buffID = 0;
	m_heroBuffID = 0;
}

CLeitaiMgr::~CLeitaiMgr()
{}

UINT32 CLeitaiMgr::save(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	//刷新时间
	*(UINT32 *)(buf + ret) = m_refreshTime;
	ret += sizeof(UINT32);
	//保存玩家的血量
	*(UINT32 *)(buf + ret) = m_playerCurHP;
	ret += sizeof(UINT32);
	//保存pk模式
	*(UINT8 *)(buf + ret) = m_pkMode;
	ret += sizeof(UINT8);
	//刷新buff时间
	*(UINT32 *)(buf + ret) = m_lastRefreshBuffTime;
	ret += sizeof(UINT32);
	//刷新出来的buff
	*(UINT16 *)(buf + ret) = m_buffID;
	ret += sizeof(UINT16);
	//刷新出来的宠物buff
	*(UINT16 *)(buf + ret) = m_heroBuffID;
	ret += sizeof(UINT16);
	//宠物的血量
	UINT16 size = m_heroHP.size();
	*(UINT16*)(buf + ret) = size;
	ret += sizeof(UINT16);

	std::map<UINT32,UINT32>::iterator it = m_heroHP.begin();
	for (;it != m_heroHP.end(); ++it){
		*(UINT32 *)(buf + ret) = it->first;
		ret += sizeof(UINT32);

		*(UINT32 *)(buf + ret) = it->second;
		ret += sizeof(UINT32);
	}
	//宠物的位置
	for (UINT8 i = 1; i <= HeroInfoMgr::s_MAX_BATTLE_HERO_NUM;++i){
		*(UINT32 *)(buf + ret) = m_battleHeroIDs[i];
		ret += sizeof(UINT32);
	}
	return ret;
}

UINT32 CLeitaiMgr::load(byte * buf)
{
	UINT32 ret = 0;
	//刷新时间
	m_refreshTime = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);
	//玩家的血量
	m_playerCurHP = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);
	//pk模式
	m_pkMode = *(UINT8 *)(buf + ret);
	ret += sizeof(UINT8);
	//刷新buff时间
	m_lastRefreshBuffTime = *(UINT32*)(buf + ret);
	ret += sizeof(UINT32);
	//刷新出来的buff
	m_buffID = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);
	//刷新出来的宠物buff
	m_heroBuffID = *(UINT16*)(buf + ret);
	ret += sizeof(UINT16);
	
	//宠物的血量
	UINT16 size = *(UINT16*)(buf + ret);
	ret += sizeof(UINT16);

	for (UINT16 i = 0;i < size ; ++i){
		UINT32 first = *(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
		
		UINT32 second = *(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
		m_heroHP[first] = second;
	}

	//宠物的位置
	for (UINT8 i = 1; i <= HeroInfoMgr::s_MAX_BATTLE_HERO_NUM;++i){
		m_battleHeroIDs[i] = *(UINT32 *)(buf + ret);
		ret += sizeof(UINT32);
	}
	return ret;
}

void CLeitaiMgr::checkRefresh()
{
	if (!g_isTheSameDay(m_refreshTime,g_gameServer->getSec())){
		m_refreshTime = g_gameServer->getSec();
		m_playerCurHP = m_pOwner->getMaxHP();
		m_pkMode = 0;
		m_heroHP.clear();
		m_challengeCharID = 0;
	}	
}

void CLeitaiMgr::sendLeitaiInfo()
{
	checkRefresh();

	if (0 == m_pkMode){//还没有选择模式
		SortProto::stNotifySelectLeitaiMode notifyCmd;
		m_pOwner->sendProto2Me(notifyCmd);
		return ;	
	}

	UINT32 pkRound = m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_ROUND);
	SortProto::stRetLeitaiInfo retCmd;
	retCmd.set_challengenum(pkRound);
	retCmd.set_myhp(m_playerCurHP);
	
	std::map<UINT32,UINT32>::iterator it = m_heroHP.begin();
	for (;it != m_heroHP.end();++it){
		SortProto::stSynHeroHP * pSyn = retCmd.add_herohps();
		if (pSyn){
			HeroInfo *info = m_pOwner->m_heroMgr.getHeroInfoByID(it->first); 
			if(info){
				pSyn->set_maxhp(info->m_maxHP);
			}
			pSyn->set_heroid(it->first);
			pSyn->set_curhp(it->second);
		}
	}

	if (m_battleHeroIDs[1]){
		retCmd.set_pos1id(m_battleHeroIDs[1]);
	}
	
	if (m_battleHeroIDs[2]){
		retCmd.set_pos2id(m_battleHeroIDs[2]);
	}
	
	if (m_battleHeroIDs[3]){
		retCmd.set_pos3id(m_battleHeroIDs[3]);
	}
	
	if (m_battleHeroIDs[4]){
		retCmd.set_pos4id(m_battleHeroIDs[4]);
	}
	
	checkRefreshBuffTime();
	retCmd.set_buffid(m_buffID);
	retCmd.set_mode((SortProto::eLeitaiMode)m_pkMode);
	UINT32 remainTime = m_lastRefreshBuffTime + s_REFRESH_BUFF_INTERVAL > g_gameServer->getSec() ? m_lastRefreshBuffTime + s_REFRESH_BUFF_INTERVAL - g_gameServer->getSec() : 0; 
	retCmd.set_refreshbuffremaintime(remainTime);	

	m_pOwner->sendProto2Me(retCmd);

	if (pkRound >= CLeitaiMgr::s_MAX_ROUND_NUM){//已经打完了
		return ;
	}   

	Global::stUserLeitaiChallenge cmd;
	cmd.set_round(pkRound + 1);
	cmd.set_charid(m_pOwner->getID());
	cmd.set_mode(m_pkMode);
	cmd.set_challengecharid(m_challengeCharID);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void CLeitaiMgr::setHeroHP(UINT32 heroID,UINT32 curHP)
{
	std::map<UINT32,UINT32>::iterator it = m_heroHP.find(heroID);	
	if (it != m_heroHP.end()){
		if (curHP > it->second){//血量比原来还低不合理
			return ;
		}
		it->second = curHP;
	}

	m_heroHP[heroID] = curHP;
}

INT32 CLeitaiMgr::getHeroHP(UINT32 heroID)
{
	std::map<UINT32,UINT32>::iterator it = m_heroHP.find(heroID);	
	if (it != m_heroHP.end()){
		return it->second;
	}
	return -1;
}

void CLeitaiMgr::resetInfo()
{
	SortProto::stResetLeitaiInfoResult retCmd;

	if (m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_RESETNUM) >= m_pOwner->getCountByVip(eVipFun_Bazhu_Reset)){
		retCmd.set_result(SortProto::stResetLeitaiInfoResult::eNot_Time_Use_Out);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

    /*
	if (!m_pOwner->removeResource(eResource_Diamond,s_RESET_USE_DIAMOND,"擂台重置")){
		retCmd.set_result(SortProto::stResetLeitaiInfoResult::eNot_Enough_Diamond);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
    */

	retCmd.set_result(SortProto::stResetLeitaiInfoResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
	
	m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_LEITAI_RESETNUM,1);
	m_pOwner->m_countMgr.resetValue(ECOUNT_LEITAI_ROUND);
	m_pOwner->m_countMgr.resetValue(ECOUNT_LEITAI_USETIME);
	m_pkMode = 0;
	m_playerCurHP = m_pOwner->getMaxHP();
	m_heroHP.clear();
	bzero(m_battleHeroIDs,sizeof(m_battleHeroIDs));
	m_challengeCharID = 0;
}

UINT8 CLeitaiMgr::getHeroSrcPos(UINT32 heroID)
{
	for (UINT8 i = 1;i <= HeroInfoMgr::s_MAX_BATTLE_HERO_NUM;++i){
		if (m_battleHeroIDs[i]== heroID){
			return i;
		}	
	}
	return (UINT8)-1;
}

void CLeitaiMgr::moveHeroPos(UINT32 heroID,UINT8 dstPos)
{
	SortProto::stLeitaiMoveHero2BattlePosError errorCmd;
	if (dstPos > HeroInfoMgr::s_MAX_BATTLE_HERO_NUM || dstPos <= 0){
		errorCmd.set_result(SortProto::stLeitaiMoveHero2BattlePosError::eSend_Msg_Param_Error);
		m_pOwner->sendProto2Me(errorCmd);
		return ;
	}
	
	UINT8 srcPos = getHeroSrcPos(heroID);
	UINT32 dstHeroID = m_battleHeroIDs[dstPos];

	if (dstPos == srcPos){//相同位置
		errorCmd.set_result(SortProto::stLeitaiMoveHero2BattlePosError::ePos_The_Same);
		m_pOwner->sendProto2Me(errorCmd);
		return ;
	}
	
	SortProto::stLeitaiMoveHero2BattlePos retCmd;
	retCmd.set_heroid(heroID);
	retCmd.set_pos(dstPos);
	m_pOwner->sendProto2Me(retCmd);

	m_battleHeroIDs[dstPos] = heroID;		
	

	if ((UINT8)-1 != srcPos){//原来就出战了
		if (dstHeroID != 0){
			m_battleHeroIDs[srcPos] = dstHeroID;
		}	
		else {
			m_battleHeroIDs[srcPos] = 0;
		}
	}
}

void CLeitaiMgr::cancelGoOut(UINT8 pos)
{
	SortProto::stLeitaiCancelGoOutForBattleResult retCmd;
	if (pos > HeroInfoMgr::s_MAX_BATTLE_HERO_NUM){
		return ;
	}
	
	m_battleHeroIDs[pos] = 0;
	retCmd.set_result(SortProto::stLeitaiCancelGoOutForBattleResult::sSUCCESS);
	m_pOwner->sendProto2Me(retCmd);
}

UINT8 CLeitaiMgr::getPKRound()
{
	return m_pOwner->m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_ROUND);
}

void CLeitaiMgr::refreshBuff()
{
	SortProto::stResetLeitaiBuffResult retCmd;

	if (!m_pOwner->removeResource(eResource_Diamond,s_REFRESH_BUFF_NEED_DIAMOND,"刷新擂台buff")){
		retCmd.set_result(SortProto::stResetLeitaiBuffResult::eDiamond_Not_Enough);	
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	
	m_buffID = ArenaConfig::getSingleton().randomLeitaiBuffID(m_pkMode == SortProto::eLeitaiMode_Normal,false,1);

	if (!m_buffID){
		retCmd.set_result(SortProto::stResetLeitaiBuffResult::eInner_Error);	
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	retCmd.set_buffid(m_buffID);
	retCmd.set_result(SortProto::stResetLeitaiBuffResult::eSuccess);	
	m_pOwner->sendProto2Me(retCmd);
}

void CLeitaiMgr::checkRefreshBuffTime()
{
	bool bNeedRefresh = false;

	if (0 == m_lastRefreshBuffTime){
		m_lastRefreshBuffTime = g_gameServer->getSec();	
		bNeedRefresh = true;
	}
	else if (m_lastRefreshBuffTime + s_REFRESH_BUFF_INTERVAL < g_gameServer->getSec()){
		m_lastRefreshBuffTime = g_gameServer->getSec();
		bNeedRefresh = true;
	}

	if (bNeedRefresh){
		m_buffID = ArenaConfig::getSingleton().randomLeitaiBuffID(m_pkMode == SortProto::eLeitaiMode_Normal,true,1);		
	}
}

void CLeitaiMgr::sendRefreshBuffTime()
{
	SortProto::stRetBuffResetTime retCmd;
	retCmd.set_buffid(m_buffID);
	UINT32 remainTime = m_lastRefreshBuffTime + s_REFRESH_BUFF_INTERVAL > g_gameServer->getSec() ? m_lastRefreshBuffTime + s_REFRESH_BUFF_INTERVAL - g_gameServer->getSec() : 0;
	retCmd.set_remaintime(remainTime);
	m_pOwner->sendProto2Me(retCmd);
}

UINT32 CLeitaiMgr::getHeroIDByPos(UINT32 pos)
{
	if (pos >= HeroInfoMgr::s_MAX_BATTLE_HERO_NUM)
	{
		return 0;
	}
	return m_battleHeroIDs[pos + 1];
}
