#include "HeroLeague.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GameServer.h"
#include "GameUser.h"
#include "SortList.pb.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "ArenaConfig.h"
#include <tinyxml/tinyxml.h>
#include "SysLanguages.h"
#include "GameUserManager.h"
#include "Global2GameModHandle.h"
#include "Map.h"
#include <set>
#include "RobotMgr.h"
#include "CopyMap.h"
#include "MapMgr.h"   
#include "Friend.pb.h"
#include "HeroLeagueCfg.h"
#include <math.h>

extern GameServer * g_gameServer;


UINT32 getNowTime()
{
	struct tm _tm; 
	RealTime::getLocalTime(_tm,g_gameServer->getSec());
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

	/* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
	UINT16 wday = _tm.tm_wday;
	//secNum = secNum + wday * 86400 ;
	return secNum + wday * 86400;
}

bool vecCmp(const stRecord & v1, const stRecord & v2)
{
	return v1.m_score > v2.m_score;
}

void CHeroLeagueMgr::scoreSort(UINT8 num)
{
	std::vector<stRecord> vec;
	std::map<UINT32, stRecord>::iterator it = m_recvrdMap.begin();
	for(;it != m_recvrdMap.end(); ++it){
		vec.push_back(it->second);
	}

	m_recvrdMap.clear();

	sort(vec.begin(), vec.end(), vecCmp);
	if(vec.begin() + num <= vec.end()){
		vec.erase(vec.begin() + num, vec.end());
	}

	std::vector<stRecord>::iterator vecIt = vec.begin();
	for(;vecIt != vec.end(); ++vecIt){
		m_recvrdMap.insert(std::make_pair((*vecIt).m_pubCharID, (*vecIt)));
	}

	m_teamVec.clear();

	UINT8 count = vec.size() - 1;
	for(UINT8 i = 0; i < vec.size() / 2; ++i, --count){
		stTeam team;
		team.m_charID1 = vec[i].m_pubCharID;
		team.m_charID2 = vec[count].m_pubCharID;
		team.m_time = g_gameServer->getSec();
		m_teamVec.push_back(team);
	}
}

bool isEqual(UINT8 num)           
{  
	for(UINT8 i = 0; i <= 4; ++i){
		if(num == pow(2,i)){
			return true;
		}
	}
	return false;                 
}                                 

void CHeroLeagueMgr::timer(UINT32 cur)
{
	if (m_lastCheckTime + s_CHECK_MATCH_INTERVAL < g_gameServer->getSec()){
		m_lastCheckTime = g_gameServer->getSec();
		checkMatchNew();
	}

	CHeroLeagueCfg cfg = CHeroLeagueCfg::getSingleton();
	if(cfg.isPreOpen(getNowTime()) || cfg.isRemOpen(getNowTime())){
		preEnterMap(cur);
	}else if(getNowTime() == cfg.getIntermediary().m_openTime - 60){//复赛开始前1分钟检查数据是否存在
		Athena::logger->trace("[英雄联赛 timer] 复赛同步");
		m_teamVec.clear();
		m_recvrdMap.clear(); 

		PubGame::stSynPubRoleList retCmd;
		retCmd.set_tag(1);
		g_gameServer->getConnMgr().broadcastByType(GAMESERVER,retCmd);
	}else if(cfg.isFinalBeginRestTime(getNowTime() - 5)){//决赛每一轮前同步
		Athena::logger->trace("[英雄联赛 timer] 决赛同步");
		m_teamVec.clear();
		m_recvrdMap.clear(); 

		PubGame::stSynPubRoleList retCmd;
		retCmd.set_tag(1);
		g_gameServer->getConnMgr().broadcastByType(GAMESERVER,retCmd);
	}else if(canEnterMap(getNowTime())){
		enterMap();
	}
}

bool CHeroLeagueMgr::canEnterMap(UINT32 cur)
{
	stFinals finalConfig = CHeroLeagueCfg::getSingleton().getFinals();
	std::map<UINT8, stFinalElement>::iterator it = finalConfig. m_finalConfig.begin();
	for(;it != finalConfig. m_finalConfig.end();++it){
		stFinalElement & element = it->second;
		if(cur == element.m_beginBattletime){
			return true;
		}
	}
	return false;
}

void CHeroLeagueMgr::updateWaitCharIdsMap(PubGame::stSynPubRoleListResult *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	Athena::logger->trace("[英雄联赛]开始同步名单！");

	if(recvCmd->isfinal()){
		//m_teamVec.clear(); 
		for(UINT16 i = 0; i < recvCmd->team_size(); ++i){
			bool exist = false;
			std::vector<stTeam>::iterator it =  m_teamVec.begin();
			for(;it != m_teamVec.end(); ++it){
				stTeam & one = *it;
				if(one.m_charID1 == recvCmd->team(i).user1().pubcharid()
						|| one.m_charID1 == recvCmd->team(i).user2().pubcharid()
						|| one.m_charID2 == recvCmd->team(i).user1().pubcharid()
						|| one.m_charID2 == recvCmd->team(i).user2().pubcharid()){
					exist = true;
					break;
				}
			}

			if(exist){
				continue;
			}

			stUserInfo teamInfo1;
			teamInfo1.m_charID = recvCmd->team(i).user1().charid();
			teamInfo1.m_pubCharID = recvCmd->team(i).user1().pubcharid();
			teamInfo1.m_areaCode = recvCmd->team(i).user1().areacode();
			strncpy(teamInfo1.m_name, recvCmd->team(i).user1().name().c_str(), sizeof(teamInfo1.m_name));
			teamInfo1.m_level = recvCmd->team(i).user1().level();
			teamInfo1.m_vipLv = recvCmd->team(i).user1().viplv();
			teamInfo1.m_job = recvCmd->team(i).user1().job();
			teamInfo1.m_sex = recvCmd->team(i).user1().sex();
			teamInfo1.m_battlePower = recvCmd->team(i).user1().battlepower();
			teamInfo1.m_photoID = recvCmd->team(i).user1().photoid();
			m_teamInfoMap.insert(std::make_pair(teamInfo1.m_pubCharID, teamInfo1));

#ifdef _HDZ_DEBUG 
			Athena::logger->trace("[英雄联赛 1] charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d", recvCmd->team(i).user1().charid(),recvCmd->team(i).user1().pubcharid(), recvCmd->team(i).user1().areacode(), recvCmd->team(i).user1().name().c_str(), recvCmd->team(i).user1().level(), recvCmd->team(i).user1().viplv(), recvCmd->team(i).user1().job(), recvCmd->team(i).user1().sex(), recvCmd->team(i).user1().battlepower());
#endif

			stUserInfo teamInfo2;
			teamInfo2.m_charID = recvCmd->team(i).user2().charid(); 
			teamInfo2.m_pubCharID = recvCmd->team(i).user2().pubcharid();
			teamInfo2.m_areaCode = recvCmd->team(i).user2().areacode();
			strncpy(teamInfo2.m_name, recvCmd->team(i).user2().name().c_str(), sizeof(teamInfo2.m_name));
			teamInfo2.m_level = recvCmd->team(i).user2().level();
			teamInfo2.m_vipLv = recvCmd->team(i).user2().viplv();
			teamInfo2.m_job = recvCmd->team(i).user2().job();
			teamInfo2.m_sex = recvCmd->team(i).user2().sex();
			teamInfo2.m_battlePower = recvCmd->team(i).user2().battlepower();
			teamInfo2.m_photoID = recvCmd->team(i).user2().photoid();
			m_teamInfoMap.insert(std::make_pair(teamInfo2.m_pubCharID, teamInfo2));

#ifdef _HDZ_DEBUG            
			Athena::logger->trace("[英雄联赛 2] charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d", recvCmd->team(i).user2().charid(),recvCmd->team(i).user2().pubcharid(), recvCmd->team(i).user2().areacode(), recvCmd->team(i).user2().name().c_str(), recvCmd->team(i).user2().level(), recvCmd->team(i).user2().viplv(), recvCmd->team(i).user2().job(), recvCmd->team(i).user2().sex(), recvCmd->team(i).user2().battlepower());
#endif

			stTeam	 team;
			team.m_charID1 = recvCmd->team(i).user1().pubcharid(); 
			team.m_charID2 = recvCmd->team(i).user2().pubcharid(); 
			team.m_time = g_gameServer->getSec();
			m_teamVec.push_back(team);

			Athena::logger->trace("[英雄联赛 决赛]charId1:%d, areaCode1:%d, pubcharid1:%d，name1:%s <--> charId2:%d, areaCode2:%d, pubcharid2:%d，name2:%s", teamInfo1.m_charID, teamInfo2.m_areaCode, teamInfo1.m_pubCharID, teamInfo1.m_name,  teamInfo2.m_charID, teamInfo2.m_areaCode,teamInfo2.m_pubCharID, teamInfo2.m_name);
		}
	}else{
		//m_recvrdMap.clear();
		for(UINT16 i = 0; i < recvCmd->charids_size(); ++i){
			std::map<UINT32, stRecord>::iterator it =  m_recvrdMap.find(recvCmd->charids(i).pubcharid());
			if(it != m_recvrdMap.end())
			{
				continue;
			}

			stRecord record;
			record.m_charID = recvCmd->charids(i).charid();
			record.m_pubCharID = recvCmd->charids(i).pubcharid();
			record.m_areaCode = recvCmd->charids(i).areacode();
			strncpy(record.m_name, recvCmd->charids(i).name().c_str(),MAX_NAMESIZE);
			record.m_level = recvCmd->charids(i).level();
			record.m_vipLv = recvCmd->charids(i).viplv();
			record.m_job = recvCmd->charids(i).job();
			record.m_sex = recvCmd->charids(i).sex();
			record.m_battlePower = recvCmd->charids(i).battlepower();
			record.m_pubServerIndex = recvCmd->charids(i).pubserverindex();
			record.m_photoID = recvCmd->charids(i).photoid();
			record.m_score = recvCmd->charids(i).score();

			m_recvrdMap.insert(std::make_pair(record.m_pubCharID, record));
		
			Athena::logger->trace("[英雄联赛 复赛]charId:%d, areaCode:%d, pubCharID:%d, name:%s", record.m_charID, record.m_areaCode, record.m_pubCharID, record.m_name);
		}
	}

	Athena::logger->trace("[英雄联赛]m_teamVec:size:%d", m_teamVec.size());
	Athena::logger->trace("[英雄联赛]m_recvrdMap:size:%d", m_recvrdMap.size());
	Athena::logger->trace("[英雄联赛]m_teamInfoMap:size:%d",m_teamInfoMap.size()); 
}

stRecord * CHeroLeagueMgr::isInRecvrd(UINT32 charID)
{
	std::map<UINT32, stRecord>::iterator it = m_recvrdMap.find(charID);
	if(it != m_recvrdMap.end()){
		return &it->second;
	}
	return NULL;
}

void CHeroLeagueMgr::updateRecvrd(stRecord& record)
{
	m_recvrdMap[record.m_pubCharID] = record;
}

void CHeroLeagueMgr::addMatchUser(GameUser * pUser)
{
	if (!pUser){
		return ;
	}	

	SortProto::stHeroLeagueMatchResult retCmd;
	
	CHeroLeagueCfg cfg = CHeroLeagueCfg::getSingleton();
	
	UINT8 limit =  0;
	if (cfg.isPreOpen(getNowTime()) || cfg.isRemOpen(getNowTime())){
		limit = cfg.getPreConfiguration().m_limitnum;
		if(cfg.isRemOpen(getNowTime())){

			limit = cfg.getIntermediary().m_limitnum;
			std::map<UINT32, stRecord>::iterator it = m_recvrdMap.find(pUser->getID());
			if(it == m_recvrdMap.end()){
				retCmd.set_result(SortProto::stHeroLeagueMatchResult::eNo_Qualification);
				pUser->sendProto2Me(retCmd); 
				Athena::logger->error("[英雄联赛匹配]匹配失败，没有资格(name=%s,id=%d)",pUser->getName(),pUser->getID());
				return;
			}
		}

		if (!pUser->m_openFuncMgr.checkFunOpen(OpenFunID_HeroLeague)){
			retCmd.set_result(SortProto::stHeroLeagueMatchResult::eLv_Limit);
			pUser->sendProto2Me(retCmd);
			Athena::logger->error("[英雄联赛匹配]匹配失败，等级限制(name=%s,id=%d)",pUser->getName(),pUser->getID());
			return ;
		}

		if(pUser->m_countMgr.getDayRefreshValue(ECOUNT_HERO_LEAGUE_LIMIT_NUM) > limit){
			retCmd.set_result(SortProto::stHeroLeagueMatchResult::eRegister_Num_Lack);
			pUser->sendProto2Me(retCmd);
			Athena::logger->error("[英雄联赛匹配]匹配失败，次数不足(name=%s,id=%d)", pUser->getName(), pUser->getID());
			return;

		}
		if (pUser->m_levelMgr.isInLv() || (pUser->getMap() && pUser->getMap()->isCopyMap())){
			Athena::logger->error("[英雄联赛匹配]匹配失败，玩家已经在副本(name=%s,id=%d)",pUser->getName(),pUser->getID());
			return;
		}
/*
		std::map<UINT32, UINT32>::iterator it = m_waitCharIDs.find(pUser->getID());
		if(it != m_waitCharIDs.end()){
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[英雄联赛匹配]匹配失败，已存在(name=%s,id=%d)", pUser->getName(), pUser->getID());
#endif 
			return;
		}
*/

		m_waitCharIDs.insert(std::make_pair(pUser->getID(), pUser->getID()));

		//checkMatchNew();

		retCmd.set_result(SortProto::stHeroLeagueMatchResult::eWait);
		pUser->sendProto2Me(retCmd);
		
	}else if(/*决赛时间*/CHeroLeagueCfg::getSingleton().isPreOpen(getNowTime())){
	
	}else{
		retCmd.set_result(SortProto::stHeroLeagueMatchResult::eNot_In_Time);
		pUser->sendProto2Me(retCmd); 
#ifdef _HDZ_DEBUG
		Athena::logger->error("[英雄联赛匹配]匹配失败，时间未开放(name=%s,id=%d)",pUser->getName(),pUser->getID());
#endif 
		return;
	}
}

void CHeroLeagueMgr::cancelMatch(GameUser * pUser)
{
	if (!pUser){
		return ;
	}

	
	m_waitCharIDs.erase(pUser->getID());

	SortProto::stCanceHeroLeagueResult retCmd;
	retCmd.set_result(SortProto::stCanceHeroLeagueResult::eSuccess);
	pUser->sendProto2Me(retCmd);

//	pUser->unreg();
//	SAFE_DELETE(pUser);
}

template<typename T>
void fillSynInfo(T & data, GameUser *pUser)
{
	if(!pUser){
		return;
	}

	data.set_charid(pUser->getID());
	data.set_job(pUser->getJob());
	data.set_level(pUser->getLevel());
	data.set_name(pUser->getName());
	data.set_sex(pUser->getSex());
	data.set_battlepower(pUser->getBattlePower());

	CItem *pHead = pUser->m_bagMgr.m_equipBag.getEquipByIndex(HEAD_SLOT_INDEX);
	if(pHead){
		data.set_headid(pHead->getID());
	}

	CItem * pCloack = pUser->m_bagMgr.m_equipBag.getEquipByIndex(CLOAK_SLOT_INDEX);
	if(pCloack){
		data.set_cloackid(pCloack->getID());
	}

	CItem * pWeapon = pUser->m_bagMgr.m_equipBag.getEquipByIndex(WEAPON_SLOT_INDEX);
	if(pWeapon){ 
		data.set_weaponid(pWeapon->getID());
	}

	CItem *pBody = pUser->m_bagMgr.m_equipBag.getEquipByIndex(BODY_SLOT_INDEX);
	if (pBody){                                                         
		data.set_bodyid(pBody->getID());                          
	}                                                                   

	CItem * pShoe = pUser->m_bagMgr.m_equipBag.getEquipByIndex(SHOE_SLOT_INDEX);            
	if (pShoe){                                                                      
		data.set_shoeid(pShoe->getID());                                       
	}                                                                                

	CItem * pWing = pUser->m_bagMgr.m_equipBag.getEquipByIndex(WING_SLOT_INDEX);
	if (pWing){
		data.set_wingid(pWing->getID());  
	}

	CItem * pFashionHead = pUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_HEAD_SLOT_INDEX);
	if(pFashionHead){
		data.set_fashionhair(pFashionHead->getID());
	}

	CItem * pFashionFace = pUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_FACE_SLOT_INDEX);
	if(pFashionFace){
		data.set_fashionface(pFashionFace->getID());
	}

	CItem * pFashionBody = pUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_BODY_SLOT_INDEX);
	if(pFashionBody){
		data.set_fashionbody(pFashionBody->getID());
	}

	CItem * pFashionDecoration = pUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_RING_SLOT_INDEX);
	if(pFashionDecoration){
		data.set_fashiondecoration(pFashionDecoration->getID());
	}

	CItem * pFashionShoe = pUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_SHOE_SLOT_INDEX);
	if(pFashionShoe){
		data.set_fashionshoe(pFashionShoe->getID());
	}

	CItem * pFashionWeapon = pUser->m_bagMgr.m_equipBag.getEquipByIndex(FASHION_WEAPON_SLOT_INDEX);
	if(pFashionWeapon){
		data.set_fashionweapon(pFashionWeapon->getID());
	}

	data.set_effectid(pUser->getEffectID());
	data.set_wingbreaklv(pUser->m_wingBreakLv);
}

//参数1：charid1，参数2：charid1是否参加战斗，参数3：charid2，参数4：charid2是否参加战斗
void WinningProcessing(UINT32 charID1, bool isFight1, UINT32 charID2, bool isFight2,  CHeroLeagueMgr *pmgr)
{
	UINT32 secNum = getNowTime(); 
	CHeroLeagueCfg cfg = CHeroLeagueCfg::getSingleton();
	PubGame::stNotifyHeroLeagueResult notifyCmd;

	if(cfg.isPreOpen(secNum)|| cfg.isRemOpen(secNum)){
		stPreliminary preliminary = cfg.getPreConfiguration(); 
		if(isFight1){
			PubGame::stPubRoleInfo *info = notifyCmd.mutable_user1();
			GameUser * pUser = GameUserManager::getSingleton().getUser(charID1);
			if(pUser && info){
				info->set_charid(pUser->getNormalCharID());          
				info->set_pubcharid(pUser->getID());                 
				info->set_areacode(pUser->getAreaCode());            
				info->set_name(pUser->getName());                    
				info->set_level(pUser->getLevel());                  
				info->set_viplv(pUser->getVipLv());                  
				info->set_job(pUser->getJob());                      
				info->set_sex(pUser->getSex());                      
				info->set_battlepower(pUser->getBattlePower());      
				info->set_pubserverindex(pUser->getNormalServerID());
	
				UINT8 points = 0;
				if(cfg.isRemOpen(secNum)){                           
					stIntermediary inter = cfg.getIntermediary();    
					points = inter.m_victoryPoints;
				}else{                             
					points = preliminary.m_victoryPoints;
				}
				info->set_score(points);
#ifdef _HDZ_DEBUG
				 Athena::logger->trace("[英雄联赛 在线]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d, score:%d",pUser->getNormalCharID(), pUser->getID(), pUser->getAreaCode(), pUser->getName(), pUser->getLevel(),pUser->getVipLv(), pUser->getJob(), pUser->getSex(), pUser->getBattlePower(), pUser->getNormalServerID(), points);
#endif

			}
		}else{
			PubGame::stPubRoleInfo *info = notifyCmd.mutable_user1();
			if(pmgr && info){
				std::map<UINT32,stUserInfo>::iterator it = pmgr->getInfoMap().find(charID1);
				if(it != pmgr->getInfoMap().end()){                                        
					stUserInfo & userInfo = it->second;                                    
					info->set_charid(userInfo.m_charID);                                   
					info->set_pubcharid(userInfo.m_pubCharID);                             
					info->set_areacode(userInfo.m_areaCode);                               
					info->set_name(userInfo.m_name);                                       
					info->set_level(userInfo.m_level);                                     
					info->set_viplv(userInfo.m_vipLv);                                     
					info->set_job(userInfo.m_job);                                         
					info->set_sex(userInfo.m_sex);                                         
					info->set_battlepower(userInfo.m_battlePower); 
					info->set_photoid(userInfo.m_photoID);
					info->set_score(0);
#ifdef _HDZ_DEBUG
				 Athena::logger->trace("[英雄联赛 不在线]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, score:%d",userInfo.m_charID,userInfo.m_pubCharID,userInfo.m_areaCode,userInfo.m_name,userInfo.m_level,userInfo.m_vipLv,userInfo.m_job,userInfo.m_sex,userInfo.m_battlePower,0); 
#endif
				}
			}
		}

		if(isFight2){
			PubGame::stPubRoleInfo *info = notifyCmd.mutable_user2();
			GameUser * pUser = GameUserManager::getSingleton().getUser(charID2);        
			if(pUser && info){  
				info->set_charid(pUser->getNormalCharID());                            
				info->set_pubcharid(pUser->getID());                                   
				info->set_areacode(pUser->getAreaCode());                              
				info->set_name(pUser->getName());                                      
				info->set_level(pUser->getLevel());                                    
				info->set_viplv(pUser->getVipLv());                                    
				info->set_job(pUser->getJob());                                        
				info->set_sex(pUser->getSex());                                        
				info->set_battlepower(pUser->getBattlePower());                        
				info->set_pubserverindex(pUser->getNormalServerID());                 
				info->set_photoid(pUser->m_photoMgr.getUsePhotoID());

				UINT8 points = 0;
				if(cfg.isRemOpen(secNum)){                                              
					stIntermediary inter = cfg.getIntermediary();                       
					points = inter.m_victoryPoints;
				}else{                                                                  
					points =preliminary.m_victoryPoints;
				} 
				info->set_score(points);
#ifdef _HDZ_DEBUG
				 Athena::logger->trace("[英雄联赛 在线]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d, score:%d",pUser->getNormalCharID(), pUser->getID(), pUser->getAreaCode(), pUser->getName(), pUser->getLevel(),pUser->getVipLv(), pUser->getJob(), pUser->getSex(), pUser->getBattlePower(), pUser->getNormalServerID(), points);     
#endif
			}                                                                           
		}else{
			PubGame::stPubRoleInfo *info = notifyCmd.mutable_user2();
			if(pmgr && info){
				std::map<UINT32,stUserInfo>::iterator it = pmgr->getInfoMap().find(charID2);
				if(it != pmgr->getInfoMap().end()){                                         
					stUserInfo & userInfo = it->second;                                     
					info->set_charid(userInfo.m_charID);                                    
					info->set_pubcharid(userInfo.m_pubCharID);                              
					info->set_areacode(userInfo.m_areaCode);                                
					info->set_name(userInfo.m_name);                                        
					info->set_level(userInfo.m_level);                                      
					info->set_viplv(userInfo.m_vipLv);                                      
					info->set_job(userInfo.m_job);                                          
					info->set_sex(userInfo.m_sex);                                          
					info->set_battlepower(userInfo.m_battlePower);        
					info->set_photoid(userInfo.m_photoID);
					info->set_score(0);                                                     
#ifdef _HDZ_DEBUG
				 Athena::logger->trace("[英雄联赛 不在线]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, score:%d",userInfo.m_charID,userInfo.m_pubCharID,userInfo.m_areaCode,userInfo.m_name,userInfo.m_level,userInfo.m_vipLv,userInfo.m_job,userInfo.m_sex,userInfo.m_battlePower,0);     
#endif
				}        
			}
		}                                                                               
	}

	if(CHeroLeagueCfg::getSingleton().isFinalFightOpen(secNum)){ 
		if(isFight1){ 
			PubGame::stPubRoleInfo *info = notifyCmd.mutable_user1();                      
			GameUser * pUser = GameUserManager::getSingleton().getUser(charID1);        
			if(pUser && info){                                                                  
				info->set_charid(pUser->getNormalCharID());                            
				info->set_pubcharid(pUser->getID());                                   
				info->set_areacode(pUser->getAreaCode());                              
				info->set_name(pUser->getName());                                      
				info->set_level(pUser->getLevel());                                    
				info->set_viplv(pUser->getVipLv());                                    
				info->set_job(pUser->getJob());                                        
				info->set_sex(pUser->getSex());                                        
				info->set_battlepower(pUser->getBattlePower());                        
				info->set_pubserverindex(pUser->getNormalServerID());                  
				info->set_winorloss(true);

				if(pmgr->getUserNum() == 2){                   
					PubGame::stReqSynChampionInfo synCmd;
					PubGame::stSynChampionInfo * data = synCmd.mutable_info();
					if(data){
						fillSynInfo(*data, pUser); 
						g_gameServer->getConnMgr().broadcastByType(GAMESERVER,synCmd);
					}
				}                                        
#ifdef _HDZ_DEBUG
				 Athena::logger->trace("[英雄联赛 在线]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d",pUser->getNormalCharID(), pUser->getID(), pUser->getAreaCode(), pUser->getName(), pUser->getLevel(),pUser->getVipLv(), pUser->getJob(), pUser->getSex(), pUser->getBattlePower(), pUser->getNormalServerID());     
#endif
			}                                                                           
		}else{ 
			PubGame::stPubRoleInfo *info = notifyCmd.mutable_user1();
			if(pmgr && info){
				std::map<UINT32,stUserInfo>::iterator it = pmgr->getInfoMap().find(charID1);
				if(it != pmgr->getInfoMap().end()){                                         
					stUserInfo & userInfo = it->second;                                     
					info->set_charid(userInfo.m_charID);                                    
					info->set_pubcharid(userInfo.m_pubCharID);                              
					info->set_areacode(userInfo.m_areaCode);                                
					info->set_name(userInfo.m_name);                                        
					info->set_level(userInfo.m_level);                                      
					info->set_viplv(userInfo.m_vipLv);                                      
					info->set_job(userInfo.m_job);                                          
					info->set_sex(userInfo.m_sex);                                          
					info->set_battlepower(userInfo.m_battlePower);                          
					info->set_winorloss(false);   
					info->set_photoid(userInfo.m_photoID);
#ifdef _HDZ_DEBUG
				 Athena::logger->trace("[英雄联赛 不在线]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d",userInfo.m_charID,userInfo.m_pubCharID,userInfo.m_areaCode,userInfo.m_name,userInfo.m_level,userInfo.m_vipLv,userInfo.m_job,userInfo.m_sex,userInfo.m_battlePower);     
#endif
				}                                                                           
			}            
		}

		if(isFight2){                                                                   
			PubGame::stPubRoleInfo *info = notifyCmd.mutable_user2();
			GameUser * pUser = GameUserManager::getSingleton().getUser(charID2);        
			if(pUser && info){                                                                  
				info->set_charid(pUser->getNormalCharID());                            
				info->set_pubcharid(pUser->getID());                                   
				info->set_areacode(pUser->getAreaCode());                              
				info->set_name(pUser->getName());                                      
				info->set_level(pUser->getLevel());                                    
				info->set_viplv(pUser->getVipLv());                                    
				info->set_job(pUser->getJob());                                        
				info->set_sex(pUser->getSex());                                        
				info->set_battlepower(pUser->getBattlePower());                        
				info->set_pubserverindex(pUser->getNormalServerID());                  
				info->set_winorloss(true);
				info->set_photoid(pUser->m_photoMgr.getUsePhotoID());

				if(pmgr->getUserNum() == 2){                   
					PubGame::stReqSynChampionInfo synCmd;
					PubGame::stSynChampionInfo *data = synCmd.mutable_info();
					if(data){
						fillSynInfo(*data, pUser);
						g_gameServer->getConnMgr().broadcastByType(GAMESERVER,synCmd);
					}
				}                                        
#ifdef _HDZ_DEBUG
				 Athena::logger->trace("[英雄联赛 在线]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d",pUser->getNormalCharID(), pUser->getID(), pUser->getAreaCode(), pUser->getName(), pUser->getLevel(),pUser->getVipLv(), pUser->getJob(), pUser->getSex(), pUser->getBattlePower(), pUser->getNormalServerID());     
#endif
			} 
		}else{
			PubGame::stPubRoleInfo *info = notifyCmd.mutable_user2();
			if(pmgr && info){
				std::map<UINT32,stUserInfo>::iterator it = pmgr->getInfoMap().find(charID2);
				if(it != pmgr->getInfoMap().end()){                                         
					stUserInfo & userInfo = it->second;                                     
					info->set_charid(userInfo.m_charID);                                    
					info->set_pubcharid(userInfo.m_pubCharID);                              
					info->set_areacode(userInfo.m_areaCode);      
					info->set_name(userInfo.m_name);              
					info->set_level(userInfo.m_level);            
					info->set_viplv(userInfo.m_vipLv);            
					info->set_job(userInfo.m_job);                
					info->set_sex(userInfo.m_sex);                
					info->set_battlepower(userInfo.m_battlePower);
					info->set_winorloss(false);
					info->set_photoid(userInfo.m_photoID);
#ifdef _HDZ_DEBUG
				 Athena::logger->trace("[英雄联赛 不在线]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d",userInfo.m_charID,userInfo.m_pubCharID,userInfo.m_areaCode,userInfo.m_name,userInfo.m_level,userInfo.m_vipLv,userInfo.m_job,userInfo.m_sex,userInfo.m_battlePower);     
#endif
				}
			}
		}
	}
	g_gameServer->getConnMgr().broadcastByType(GAMESERVER,notifyCmd);
}

void CHeroLeagueMgr::checkMatchNew()
{
	while(m_waitCharIDs.size() >= 2){
		UINT32 charID1 = 0, charID2 = 0;
		std::map<UINT32, UINT32>::iterator it = m_waitCharIDs.begin(); 
		if (it != m_waitCharIDs.end()){
			charID1 = it->second;
		}
		++it;
		if (it != m_waitCharIDs.end()){
				charID2 = it->second;
			}

			if ((charID1 > 0) && (charID2 > 0 )){

				GameUser * pUser1 = GameUserManager::getSingleton().getUser(charID1);
				GameUser * pUser2 = GameUserManager::getSingleton().getUser(charID2);
				if(!pUser1 || !pUser2){
					return ;
				}

				SortProto::stMemberinfo retCmd1;
				SortProto::stSynChampionInfo *data1 = retCmd1.add_member();  
				fillSynInfo(*data1, pUser1);
				Athena::logger->trace("###"); 
				for(UINT8 i = 0; i < retCmd1.member_size(); ++i){
					SortProto::stSynChampionInfo info = retCmd1.member(i);
					Athena::logger->trace("charid:%d,name:%s,areacode:%d, level%d, job:%d, sex:%d, battlepower:%d, headID:%d, bodyID:%d, weaponID:%d, cloackID:%d, wingID:%d, ShoeID:%d, fashionHair:%d, fashionFace:%d, fashionBody:%d, fashionweapon:%d, fashionShoe:%d, fashionDecoration:%d, effectID:%d, wingBackLv:%d",info.charid(), info.name().c_str(), info.areacode(),info.level(),info.job(),info.sex(),info.battlepower(),info.headid(),info.bodyid(),info.weaponid(), info.cloackid(),info.wingid(),info.shoeid(),info.fashionhair(),info.fashionface(),info.fashionbody(),info.fashionweapon(),info.fashionshoe(),info.fashiondecoration(),info.effectid(),info.wingbreaklv());
				}
				pUser2->sendProto2Me(retCmd1);

				SortProto::stMemberinfo retCmd2;
				SortProto::stSynChampionInfo *data2 = retCmd2.add_member();  
				fillSynInfo(*data2, pUser2);
				Athena::logger->trace("###"); 
				for(UINT8 i = 0; i < retCmd2.member_size(); ++i){
					SortProto::stSynChampionInfo info = retCmd2.member(i);
					Athena::logger->trace("charid:%d,name:%s,areacode:%d, level%d, job:%d, sex:%d, battlepower:%d, headID:%d, bodyID:%d, weaponID:%d, cloackID:%d, wingID:%d, ShoeID:%d, fashionHair:%d, fashionFace:%d, fashionBody:%d, fashionweapon:%d, fashionShoe:%d, fashionDecoration:%d, effectID:%d, wingBackLv:%d",info.charid(), info.name().c_str(), info.areacode(),info.level(),info.job(),info.sex(),info.battlepower(),info.headid(),info.bodyid(),info.weaponid(), info.cloackid(),info.wingid(),info.shoeid(),info.fashionhair(),info.fashionface(),info.fashionbody(),info.fashionweapon(),info.fashionshoe(),info.fashiondecoration(),info.effectid(),info.wingbreaklv());
				}
				pUser1->sendProto2Me(retCmd2);

				stTeam team;
				team.m_charID1 = charID1;
				team.m_charID2 = charID2;
				team.m_time = g_gameServer->getSec();
				m_teamVec.push_back(team);

				//报名成功次数加1
				PubGame::stAddRegisterNum notifyCmd;    
				notifyCmd.set_charid(pUser1->getNormalCharID());   
				pUser1->sendProto2NormalGame(notifyCmd); 

				notifyCmd.set_charid(pUser2->getNormalCharID());
				pUser2->sendProto2NormalGame(notifyCmd); 

				SortProto::stHeroLeagueMatchResult cmd;                     
				cmd.set_result(SortProto::stHeroLeagueMatchResult::eSucess);
				pUser1->sendProto2Me(cmd);                                  
				pUser2->sendProto2Me(cmd);                                  

				m_waitCharIDs.erase(charID1);
				m_waitCharIDs.erase(charID2);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("英雄联赛 匹配到玩家了1(%d,%d)!",charID1,charID2);
#endif
			}
		}
	//}
}


void CHeroLeagueMgr::preEnterMap(UINT32 sec)
{
	std::vector<stTeam>::iterator it = m_teamVec.begin();
	for(;it != m_teamVec.end();){
		if(sec >= (*it).m_time + s_PRE_WAIT_TIME){
			UINT32 charID1 = (*it).m_charID1;
			UINT32 charID2 = (*it).m_charID2;
#ifdef _HDZ_DEBUG                                  
			Athena::logger->trace("英雄联赛 进入地图 charID1:%d, charID2:%d", charID1,charID2); 
#endif
			Global::stHeroLeagueEnterCopyMap cmd;
			cmd.add_memberids(charID1);
			cmd.add_memberids(charID2);
			PkWithOther(&cmd,this);

			m_teamVec.erase(it);
		}
		else{
			++it;
		}
	}
}

void CHeroLeagueMgr::enterMap()
{

#ifdef _HDZ_DEBUG                                 
	Athena::logger->trace("英雄联赛 enterMap() ");
#endif                                            
	std::vector<stTeam>::iterator it = m_teamVec.begin();        
//	UINT8 userNum = 0;
	m_finalUserNum = m_teamVec.size();
	for(;it != m_teamVec.end();){
//		++userNum;

		UINT32 charID1 = (*it).m_charID1;                                    
		UINT32 charID2 = (*it).m_charID2;                                    
#ifdef _HDZ_DEBUG                                                                               
		Athena::logger->trace("英雄联赛 决赛进入地图 charID1:%d, charID2:%d", charID1,charID2);
#endif
		Global::stHeroLeagueEnterCopyMap cmd;                                
		cmd.add_memberids(charID1);                                          
		cmd.add_memberids(charID2);                                          
		PkWithOther(&cmd,this); 

		m_teamVec.erase(it);                                                 
	}                           
//	m_finalUserNum = userNum;

	m_waitCharIDs.clear();
	m_recvrdMap.clear();
	m_teamVec.clear();
}


bool PkWithRobot(Global::stHeroLeaguePkWithRobot * recvCmd)
{
	return true;
}

bool PkWithOther(Global::stHeroLeagueEnterCopyMap * recvCmd, CHeroLeagueMgr *pmgr)
{
	if (!recvCmd || !pmgr){
		return false;
	}

	stCopyMapBase * pCopyMapBase = NULL;

	pCopyMapBase = g_copyMapBaseMgr.get(CHeroLeagueCfg::getSingleton().getPreConfiguration().m_levelID); 

	if (!pCopyMapBase){
		Athena::logger->error("[英雄联赛]找不到关卡配置!");
		return false;
	}

	if (recvCmd->memberids_size() < 2){
		Athena::logger->error("[英雄联赛]人数不足两人,进入失败!");
		return false;
	}

	GameUser * pUser1 = GameUserManager::getSingleton().getUser(recvCmd->memberids(0));
	GameUser * pUser2 = GameUserManager::getSingleton().getUser(recvCmd->memberids(1));

	if (!pUser1 || !pUser2){
		if(pUser1 && !pUser2){
			WinningProcessing(recvCmd->memberids(0), true, recvCmd->memberids(1), false, pmgr);
#ifdef _HDZ_DEBUG                      
			Athena::logger->trace("英雄联赛charID:%d不在，charid:%d直接获胜！",recvCmd->memberids(1),recvCmd->memberids(0));
#endif                                 
		}
		if(!pUser1 && pUser2){
			WinningProcessing(recvCmd->memberids(0), false, recvCmd->memberids(1), true, pmgr); 
#ifdef _HDZ_DEBUG
			Athena::logger->trace("英雄联赛charID:%d不在，charid:%d直接获胜！",recvCmd->memberids(0),recvCmd->memberids(1));
#endif
		}

		if(!pUser1 && !pUser2){
			WinningProcessing(recvCmd->memberids(0), false, recvCmd->memberids(1), false, pmgr);
		}
		return false;
	}

	if (pUser1 == pUser2){
		Athena::logger->error("[英雄联赛]是同一个玩家!");
		return false;
	}

	if (pUser1->getMap() && pUser1->getMap()->isCopyMap() && (!((CCopyMap *)pUser1->getMap())->isGuildHall())){
		Athena::logger->error("[英雄联赛]被邀请一方已经在副本了");
		return false;
	}

	if (pUser2->m_levelMgr.isInLv()){
		Athena::logger->error("[英雄联赛]发起邀请一方已经pve场景");
		FriendProto::stReplyFriendPKInviteResult retCmd;
		retCmd.set_result(FriendProto::stReplyFriendPKInviteResult::eNot_Invited_Has_Enter);
		pUser1->sendProto2Me(retCmd);
		return false;
	}

	if (pUser2->getMap() && pUser2->getMap()->isCopyMap() && (!((CCopyMap *)pUser2->getMap())->isGuildHall())){
		Athena::logger->error("[英雄联赛]发起邀请一方已经在副本了");
		FriendProto::stReplyFriendPKInviteResult retCmd;
		retCmd.set_result(FriendProto::stReplyFriendPKInviteResult::eNot_Invited_Has_Enter);
		pUser1->sendProto2Me(retCmd);
		return false;
	}

	pUser1->setCampID(Map::eCamp_Red);
	pUser2->setCampID(Map::eCamp_Black);

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser1->getUniqueName(),pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[英雄联赛]进入副本失败,创建副本不成功, CopyMapID:%d!",pCopyMapBase->m_id);
		return false;
	}

	
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);

	pCopyMap->joinCamp(Map::eCamp_Red,pUser1->getID());
	pCopyMap->joinCamp(Map::eCamp_Black,pUser2->getID());

	pUser1->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	pUser1->resetBattleProp();
	
	pUser2->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	pUser2->resetBattleProp();
	
	CMapMgr::getSingleton().changeMap(pUser1,pCopyMap,1);
	CMapMgr::getSingleton().changeMap(pUser2,pCopyMap,2);

	pUser1->NotifyNormalOnPub(true);
	pUser2->NotifyNormalOnPub(true);
	return true;
}

void CHeroLeagueMgr::sendMenberInfo(UINT32 charID)
{
	stFinals cfg = CHeroLeagueCfg::getSingleton().getFinals();
	UINT32 time = getNowTime();
	
	if(cfg.m_applyTime <= time && cfg.m_clearing >= time){
		GameUser * pUser = GameUserManager::getSingleton().getUser(charID);
		if(!pUser){
			return;
		}

		std::vector<stTeam>::iterator it =  m_teamVec.begin();
		for(;it != m_teamVec.end();++it){
			stTeam & team = *it;
			if(team.m_charID1 == charID){
				GameUser * pUser1 = GameUserManager::getSingleton().getUser(team.m_charID2);
				if(pUser1){
					SortProto::stMemberinfo retCmd1;
					SortProto::stSynChampionInfo *data1 = retCmd1.add_member();  
					fillSynInfo(*data1, pUser1);   
					pUser->sendProto2Me(retCmd1);  

					SortProto::stMemberinfo retCmd2;  
					SortProto::stSynChampionInfo *data2 = retCmd2.add_member();
					fillSynInfo(*data2, pUser);
					pUser1->sendProto2Me(retCmd2);
					break;
				}
			}
			if(team.m_charID2 == charID){
				GameUser * pUser1 = GameUserManager::getSingleton().getUser(team.m_charID1);
				if(pUser1){
					SortProto::stMemberinfo retCmd1;
					SortProto::stSynChampionInfo *data1 = retCmd1.add_member();
					fillSynInfo(*data1, pUser1);
					pUser->sendProto2Me(retCmd1);

					SortProto::stMemberinfo retCmd2;
					SortProto::stSynChampionInfo *data2 = retCmd2.add_member();
					fillSynInfo(*data2, pUser);
					pUser1->sendProto2Me(retCmd2);
					break;  
				}
			}
		}
	}
}

void CHeroLeagueMgr::removeOtherModel(UINT32 charID)
{
	stFinals cfg = CHeroLeagueCfg::getSingleton().getFinals();
	UINT32 time = getNowTime();
	if(cfg.m_applyTime <= time && cfg.m_clearing >= time){
		SortProto::stRemoveModel retCmd;
		std::vector<stTeam>::iterator it =  m_teamVec.begin();
		for(;it != m_teamVec.end();++it){
			stTeam & team = *it;
			if(team.m_charID1 == charID){
				retCmd.set_charid(team.m_charID2);
				GameUser * pUser = GameUserManager::getSingleton().getUser(team.m_charID2);
				if(pUser){
					pUser->sendProto2Me(retCmd);
				}
				break;
			}

			if(team.m_charID2 == charID){
				retCmd.set_charid(team.m_charID1);  
				GameUser * pUser = GameUserManager::getSingleton().getUser(team.m_charID1);
				if(pUser){
					pUser->sendProto2Me(retCmd);
				}
				break;
			}
		}
	}
}
