#include "One2OnePVP.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GameServer.h"
#include "GameUser.h"
#include "SortList.pb.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
// #include "GlobalVar.h"
// #include "Mail.h"
// #include "RankMgr.h"
#include "ArenaConfig.h"
// #include "PlayerManager.h"
#include <tinyxml/tinyxml.h>
#include "SysLanguages.h"
#include "GameUserManager.h"
#include "Global2GameModHandle.h"
#include "Map.h"
#include "RobotMgr.h"
#include "CopyMap.h"
#include "MapMgr.h"   
#include "Friend.pb.h"


void COne2OnePVPMgr::timer(UINT32 cur)
{
	if (m_lastCheckTime + s_CHECK_MATCH_INTERVAL < g_gameServer->getSec()){
		m_lastCheckTime = g_gameServer->getSec();
		//checkMatch();
		checkMatchNew();
	}
}

void COne2OnePVPMgr::addMatchUser(GameUser * pUser)
{
	if (!pUser){
		return ;
	}	
	
	SortProto::stOne2OnePVPMatchResult retCmd;
	struct tm _tm; 
	RealTime::getLocalTime(_tm,g_gameServer->getSec());
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

	if (!ArenaConfig::getSingleton().isOne2OneOpen(secNum)){//时间未到
		retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eNot_In_Time);
		pUser->sendProto2Me(retCmd);
		return ;
	}
	
	if (pUser->getLevel() < 18){
		retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eFail);
		pUser->sendProto2Me(retCmd);
		return ;
	}

	UINT32 myScore = pUser->getOne2OnePVPScore();
	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin();
	
	bool bMatch = false;

	for (;it != m_waitMatchUsers.end();++it){
		stMatchCfg & matchInfo = it->second;
		if ((myScore >= matchInfo.m_startPoint) && (myScore <= matchInfo.m_endPoint)){
			matchInfo.m_waitCharIDs.insert(std::make_pair(pUser->getID(),g_gameServer->getSec()));
			bMatch = true;
			break;
		}
	}
	if (!bMatch){
		Athena::logger->error("one2One匹配错误!");
		return ;
	}
	retCmd.set_result(SortProto::stOne2OnePVPMatchResult::eWait);
	pUser->sendProto2Me(retCmd);

}

void COne2OnePVPMgr::cancelMatch(GameUser * pUser)
{
	if (!pUser){
		return ;
	}

	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin();
	for (;it != m_waitMatchUsers.end();++it){
		it->second.m_waitCharIDs.erase(pUser->getID());	
	}
	SortProto::stCancelOne2OnePVPResult retCmd;
	retCmd.set_result(SortProto::stCancelOne2OnePVPResult::eSuccess);
	pUser->sendProto2Me(retCmd);

	pUser->unreg();
	SAFE_DELETE(pUser);
}


void COne2OnePVPMgr::checkMatchReset()
{
	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >::iterator it =  m_waitState1Users.begin();
	for (;it != m_waitState1Users.end();++it){
		it->second.clear();	
	}

	it = m_waitState2Users.begin();
	for (;it != m_waitState2Users.end();++it){
		it->second.clear();
	}
	
	it = m_waitState3Users.begin();
	for (;it != m_waitState3Users.end();++it){
		it->second.clear();
	}

}

void COne2OnePVPMgr::checkMatchNew()
{
	checkMatchReset();
	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin();
	for (;it != m_waitMatchUsers.end();++it){
		stMatchCfg & matchInfo = it->second;
		
		if (eMatch_Type1 ==matchInfo.m_matchType){//类型一的通通匹配机器人
			std::map<UINT32,UINT32>::iterator subIt = matchInfo.m_waitCharIDs.begin();
			UINT16 matchNum = 0;
			for (;subIt != matchInfo.m_waitCharIDs.end();){
				std::map<UINT32,UINT32>::iterator subItTmp = subIt;
				++subIt;
				GameUser* pUser =GameUserManager::getSingleton().getUser(subItTmp->first);
				if (pUser == NULL)
				{
					matchInfo.m_waitCharIDs.erase(subItTmp);
					continue;
				}
				Global::stOne2OnePkWithRobot cmd;
				cmd.set_charid(subItTmp->first);
				// UINT16 userLv = CRankMgr::getSingleton().getLevelByCharID(subItTmp->first);
				cmd.set_robotgroup(getRobotGroupID(eMatch_Type1,pUser->getLevel()));
				// g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
				
				PkWithRobot(&cmd);

				matchInfo.m_waitCharIDs.erase(subItTmp);
				++matchNum;
				if (matchNum >= 10){
					break;
				}
			}		
			continue;
		}
		
		//同一阶段的都匹配玩家先
		std::map<UINT32,UINT32>::iterator subIt1,subIt2;
		while (matchInfo.m_waitCharIDs.size() >= 2){
			UINT32 charID1 = 0, charID2 = 0;
			subIt1 = matchInfo.m_waitCharIDs.begin();
			if (subIt1 != matchInfo.m_waitCharIDs.end()){
				charID1 = subIt1->first;
			}
			++subIt1;
			if (subIt1 != matchInfo.m_waitCharIDs.end()){
				charID2 = subIt1->first;
			}

			if ((charID1 > 0) && (charID2 > 0 )){
				Global::stOne2OnePVPEnterCopyMap cmd;
				cmd.add_memberids(charID1);
				cmd.add_memberids(charID2);
				matchInfo.m_waitCharIDs.erase(charID1);
				matchInfo.m_waitCharIDs.erase(charID2);
				// g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
				PkWithOther(&cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("one2One 匹配到玩家了1(%d,%d)!",charID1,charID2);
#endif
			}
		}
		
		//放到各个阶段
		std::map<UINT32,UINT32>::iterator subIt = matchInfo.m_waitCharIDs.begin();
		for (;subIt != matchInfo.m_waitCharIDs.end();++subIt){
			if (subIt->second + matchInfo.m_matchTimeState1 > g_gameServer->getSec()){//第一阶段
				m_waitState1Users[matchInfo.m_matchType].push_back(std::make_pair(subIt->first,matchInfo.m_id));
			}
			else if (subIt->second + matchInfo.m_matchTimeState2 > g_gameServer->getSec()){//第二阶段
				m_waitState2Users[matchInfo.m_matchType].push_back(std::make_pair(subIt->first,matchInfo.m_id));
			}
			else {
				m_waitState3Users[matchInfo.m_matchType].push_back(std::make_pair(subIt->first,matchInfo.m_id));
			}
		}
	}
	
	//处于第一阶段的怎么处理
	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >::iterator thdIt =  m_waitState1Users.begin();
	/*for (;thdIt != m_waitState1Users.end();++thdIt){
		for (UINT16 i = 0;i < thdIt->second.size();++i){
			
		}
	}*/
	
	
	//处于第二阶段的玩家
	thdIt =  m_waitState2Users.begin();
	for (;thdIt != m_waitState2Users.end();++thdIt){
		for (UINT16 i = 0;i < thdIt->second.size();++i){
			if (eMatch_Type1 == thdIt->first){//类型一不进行任何处理
			
			}	
			else if (eMatch_Type2 == thdIt->first){//类型2不进行任何处理,开始匹配机器人了
				GameUser* pUser =GameUserManager::getSingleton().getUser(thdIt->second[i].first);
				if (pUser == NULL)
				{
					it = m_waitMatchUsers.find(thdIt->second[i].second);
					if (it != m_waitMatchUsers.end()){
						it->second.m_waitCharIDs.erase(thdIt->second[i].first);
					}
					continue;
				}
				Global::stOne2OnePkWithRobot cmd;
				cmd.set_charid(thdIt->second[i].first);
				cmd.set_robotgroup(getRobotGroupID(eMatch_Type1,pUser->getLevel()));

				PkWithRobot(&cmd);  

				it = m_waitMatchUsers.find(thdIt->second[i].second);
				if (it != m_waitMatchUsers.end()){
					it->second.m_waitCharIDs.erase(thdIt->second[i].first);
				}
			}
			else {//其它两种,到上下两级匹配人
				UINT16 id = thdIt->second[i].second;
				UINT16 from = id > 2 ? id - 2 : (id>1?id-1:id);
				UINT16 to = id + 2;

				if (from < to) {
					for (;from <= to; ++from){
						it = m_waitMatchUsers.find(from);
						if (it != m_waitMatchUsers.end()){
							if (it->second.m_waitCharIDs.size() > 0){
								std::map<UINT32,UINT32>::iterator subIt = it->second.m_waitCharIDs.begin();
								if ((subIt != it->second.m_waitCharIDs.end()) && (thdIt->second[i].first != subIt->first)){
									Global::stOne2OnePVPEnterCopyMap cmd; 
									cmd.add_memberids(thdIt->second[i].first);
									cmd.add_memberids(subIt->first);
									it->second.m_waitCharIDs.erase(subIt);

									it = m_waitMatchUsers.find(id);
									if (it != m_waitMatchUsers.end()){
										it->second.m_waitCharIDs.erase(thdIt->second[i].first);
									}
									// g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
									PkWithOther(&cmd);
#ifdef _HDZ_DEBUG
									Athena::logger->trace("one2One 匹配到玩家了2!");
#endif
								}
							}	
						}
					}
				}
			}
		}
	}

	//第三阶段一律放机器人
	thdIt =  m_waitState3Users.begin();
	for (;thdIt != m_waitState3Users.end();++thdIt){
		for (UINT16 i = 0;i < thdIt->second.size();++i){
			GameUser* pUser =GameUserManager::getSingleton().getUser(thdIt->second[i].first);
			if (pUser == NULL)
			{
				it = m_waitMatchUsers.find(thdIt->second[i].second);
				if (it != m_waitMatchUsers.end()){
					it->second.m_waitCharIDs.erase(thdIt->second[i].first);
				}
				continue;
			}
			Global::stOne2OnePkWithRobot cmd;
			cmd.set_charid(thdIt->second[i].first);
			cmd.set_robotgroup(getRobotGroupID(eMatch_Type1,pUser->getLevel()));

			PkWithRobot(&cmd);  

			it = m_waitMatchUsers.find(thdIt->second[i].second);
			if (it != m_waitMatchUsers.end()){
				it->second.m_waitCharIDs.erase(thdIt->second[i].first);
			}
		}
	}
}

UINT16 COne2OnePVPMgr::getRobotGroupID(UINT16 matchType,UINT16 lv)
{
	std::map<UINT16,std::vector<stMatchRobotGroup> >::iterator it = m_matchRobotInfos.find(matchType);
	if (it == m_matchRobotInfos.end()){
		return 0;
	}

	std::vector<stMatchRobotGroup> & vec = it->second;

	for (UINT16 i = 0;i < vec.size();++i){
		stMatchRobotGroup & groupInfo = vec[i];
		if ((lv >= groupInfo.m_minLv) && (lv <= groupInfo.m_maxLv)){
			return groupInfo.m_groupID;
		}
	}
	return 0;
}

bool COne2OnePVPMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/arenamatch.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[巅峰对决]加载文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[巅峰对决]巅峰配置加载错误,找不到Root节点!");
		return false;
	}

	INT32 iVal = 0;
	TiXmlElement * pArenamatchNode = pRoot->FirstChildElement("Arenamatch");
	if (pArenamatchNode){
		TiXmlElement * pMatch = pArenamatchNode->FirstChildElement("match");
		while (pMatch){
			stMatchCfg match;
			if (pMatch->Attribute("id",&iVal)){
				match.m_id = iVal;	
			}		
			if (pMatch->Attribute("startPoint",&iVal)){
				match.m_startPoint = iVal;
			}
			if (pMatch->Attribute("endPoint",&iVal)){
				match.m_endPoint = iVal; 
			}
			if (pMatch->Attribute("matchType",&iVal)){
				match.m_matchType = iVal; 
			}
			if (pMatch->Attribute("matchTimeStage1",&iVal)){
				match.m_matchTimeState1 = iVal; 
			}
			if (pMatch->Attribute("matchTimeStage2",&iVal)){
				match.m_matchTimeState2 = iVal; 
			}
			if (pMatch->Attribute("matchRobot",&iVal)){
				match.m_matchRangeID = iVal; 
			}

			m_waitMatchUsers[match.m_id] = match;
			pMatch = pMatch->NextSiblingElement("match");
		}
	}
	
	TiXmlElement *pArenarobotNode = pRoot->FirstChildElement("Arenarobot");
	if (pArenarobotNode){
		TiXmlElement * pRobotRangeNode = pArenarobotNode->FirstChildElement("robotRange");
		while (pRobotRangeNode){
			UINT16 matchType = 0;
			if (pRobotRangeNode->Attribute("id",&iVal)){
				matchType = iVal;
			}
			TiXmlElement *  pRobotNode = pRobotRangeNode->FirstChildElement("robot");
			while (pRobotNode){
				stMatchRobotGroup groupInfo;
				if (pRobotNode->Attribute("startLevel",&iVal)){
					groupInfo.m_minLv = iVal;	
				}
				if (pRobotNode->Attribute("endLevel",&iVal)){
					groupInfo.m_maxLv = iVal;
				}
				if (pRobotNode->Attribute("group",&iVal)){
					groupInfo.m_groupID = iVal;
				}
				m_matchRobotInfos[matchType].push_back(groupInfo);
				pRobotNode = pRobotNode->NextSiblingElement("robot");
			}
			pRobotRangeNode = pRobotRangeNode->NextSiblingElement("robotRange"); 
		}
	}

	TiXmlElement * pArenaRewardNode = pRoot->FirstChildElement("ArenaReward");
	if (pArenaRewardNode){
		TiXmlElement * pRewardNode = pArenaRewardNode->FirstChildElement("reward");
		while (pRewardNode){
			stEverydayReward reward;
			if (pRewardNode->Attribute("Startrank",&iVal)){
				reward.m_beginRank = iVal;	
			}
			if (pRewardNode->Attribute("Endrank",&iVal)){
				reward.m_endRank = iVal;
			}
			if (pRewardNode->Attribute("Fristrewardid",&iVal)){
				reward.m_itemID = iVal;
			}
			if (pRewardNode->Attribute("Fristrewardnum",&iVal)){
				reward.m_num = iVal;	
			}
			m_everydayRewards.push_back(reward);
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
	}

	TiXmlElement * pTimeRewardNode = pRoot->FirstChildElement("TimeReward");
	if (pTimeRewardNode){
		TiXmlElement * pRewardNode = pTimeRewardNode->FirstChildElement("reward");
		while (pRewardNode){
			stWeekReward reward;
			if (pRewardNode->Attribute("Startrank",&iVal)){
				reward.m_beginRank = iVal;
			}

			if (pRewardNode->Attribute("Endrank",&iVal)){
				reward.m_endRank = iVal;	
			}

			if (pRewardNode->Attribute("Fristrewardid",&iVal)){
				reward.m_itemID1 = iVal;
			}

			if (pRewardNode->Attribute("Fristrewardnum",&iVal)){
				reward.m_num1 = iVal;
			}

			if (pRewardNode->Attribute("Secondrewardid",&iVal)){
				reward.m_itemID2 = iVal;
			}

			if (pRewardNode->Attribute("Secondrewardnum",&iVal)){
				reward.m_num2 = iVal;
			}
			
			if (pRewardNode->Attribute("Thirdrewardid",&iVal)){
				reward.m_itemID3 = iVal;
			}

			if (pRewardNode->Attribute("Thirdrewardnum",&iVal)){
				reward.m_num3 = iVal;
			}

			m_weekRewards.push_back(reward);
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
		pTimeRewardNode = pTimeRewardNode->NextSiblingElement("TimeReward");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("巅峰匹配配置:");
	std::map<UINT16,stMatchCfg>::iterator it = m_waitMatchUsers.begin(); 
	for (;it != m_waitMatchUsers.end();++it){
		stMatchCfg & match = it->second;
		Athena::logger->trace("id=%d,startPoint=%d,endPoint=%d,matchType=%d,matchTimeState1=%d,matchTimeState2=%d,matchRangeID:%d",match.m_id,match.m_startPoint,match.m_endPoint,match.m_matchType,match.m_matchTimeState1,match.m_matchTimeState2,match.m_matchRangeID);
	}
	
	Athena::logger->trace("每天奖励配置:");
	for (UINT16 i = 0;i < m_everydayRewards.size();++i){
		stEverydayReward & reward = m_everydayRewards[i];
		Athena::logger->trace("beginRank=%d,endRank=%d,itemID1=%d,num1=%d",reward.m_beginRank,reward.m_endRank,reward.m_itemID,reward.m_num);
	}

	Athena::logger->trace("周奖励配置:");
	for (UINT16 i = 0;i < m_weekRewards.size();++i){
		stWeekReward & reward = m_weekRewards[i];
		Athena::logger->trace("beginRank=%d,endRank=%d,itemID1=%d,num1=%d,itemID2=%d,num2=%d",reward.m_beginRank,reward.m_endRank,reward.m_itemID1,reward.m_num1,reward.m_itemID2,reward.m_num2);	
	}

	Athena::logger->trace("打印机器人匹配信息:");
	std::map<UINT16,std::vector<stMatchRobotGroup> >::iterator subIt = m_matchRobotInfos.begin();
	for (;subIt != m_matchRobotInfos.end();++subIt){
		Athena::logger->trace("匹配的类型为:%d",subIt->first);
	
		std::vector<stMatchRobotGroup> & vec = subIt->second;
		for (UINT16 i = 0;i < vec.size();++i){
			stMatchRobotGroup & groupInfo = vec[i];
			Athena::logger->trace("startLv:%d,endLv:%d,group:%d",groupInfo.m_minLv,groupInfo.m_maxLv,groupInfo.m_groupID);
		}
	}
#endif
	return true;
}


bool PkWithRobot(Global::stOne2OnePkWithRobot * recvCmd)
{
	stCopyMapBase * pCopyMapBase = NULL;
	pCopyMapBase = g_copyMapBaseMgr.get(ONE2ONE_PVP_MAPID);
	if (!pCopyMapBase){
		Athena::logger->error("[和机器人巅峰对决]找不到关卡配置!");
		return false;
	}

	GameUser * pUser1 = GameUserManager::getSingleton().getUser(recvCmd->charid());
	
	if (!pUser1){
		Athena::logger->error("[和机器人巅峰对决]有一方已经不在线了,进入失败!");
		return false;
	}
	
	if (!pUser1->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
		Athena::logger->error("[和机器人巅峰对决]玩家(name:%s,id:%d)历练不足进入1v1失败!",pUser1->getName(),pUser1->getID());
		return false;
	}

	if (pUser1->getMap() && pUser1->getMap()->isCopyMap() && (!((CCopyMap *)pUser1->getMap())->isGuildHall())){
		Athena::logger->error("[和机器人巅峰对决]被邀请一方已经在副本了");
		return false;
	}
	
	pUser1->setCampID(Map::eCamp_Red);
	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser1->getUniqueName(),pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[和机器人巅峰对决]进入副本失败,创建副本不成功!");
		return false;
	}
	
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);
	
	pCopyMap->joinCamp(Map::eCamp_Red,pUser1->getID());

	pUser1->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	pUser1->resetBattleProp();
	CMapMgr::getSingleton().changeMap(pUser1,pCopyMap,1);

	stRobotInfo * pRobot = CRobotMgr::getSingleton().getOne2OneRobot(ONE2ONE_PVP_MAPID,recvCmd->robotgroup());
	if (pRobot && pRobot->m_pUser) {
		pRobot->m_pUser->setCampID(Map::eCamp_Black);
		//pCopyMap->joinCamp(Map::eCamp_Red,pRobot->m_pUser->getID());
		pRobot->m_pUser->resetBattleProp();
		CRobotMgr::getSingleton().useRobot(pRobot->m_pUser->getID(),pRobot);
		pCopyMap->joinCamp(Map::eCamp_Black,pRobot->m_pUser->getID());
		CMapMgr::getSingleton().robotChangeMap(pRobot->m_pUser,pCopyMap,2);
	}
	else {
		Athena::logger->trace("1v1 放机器人时候,没有机器人了!");
	}
	pUser1->NotifyNormalOnPub(true);
	return true;
}

bool PkWithOther(Global::stOne2OnePVPEnterCopyMap * recvCmd)
{
	if (!recvCmd){
		return false;
	}
	
	stCopyMapBase * pCopyMapBase = NULL;
	
	if (recvCmd->pkwithfriend()){
		pCopyMapBase = g_copyMapBaseMgr.get(PKWITH_FRIEND_MAPID);
	}
	else {
		pCopyMapBase = g_copyMapBaseMgr.get(ONE2ONE_PVP_MAPID);
	}

	if (!pCopyMapBase){
		Athena::logger->error("[巅峰对决]找不到关卡配置!");
		return false;
	}

	if (recvCmd->memberids_size() < 2){
		Athena::logger->error("[巅峰对决]人数不足两人,进入失败!");
		return false;
	}
	
	GameUser * pUser1 = GameUserManager::getSingleton().getUser(recvCmd->memberids(0));
	GameUser * pUser2 = GameUserManager::getSingleton().getUser(recvCmd->memberids(1));
	
	if (!pUser1 || !pUser2){
		Athena::logger->error("[巅峰对决]有一方已经不在线了,进入失败!");
		return false;
	}

	if (pUser1 == pUser2){
		Athena::logger->error("[巅峰约战]是同一个玩家!");
		return false;
	}
	
	if (!recvCmd->pkwithfriend()) {
		if (!pUser1->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
			Athena::logger->error("玩家(name:%s,id:%d)历练不足进入1v1失败!",pUser1->getName(),pUser1->getID());
			return false;
		}
		
		if (!pUser2->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
			Athena::logger->error("玩家(name:%s,id:%d)历练不足进入1v1失败!",pUser2->getName(),pUser2->getID());
			return false;
		}
	}

	if (pUser1->getMap() && pUser1->getMap()->isCopyMap() && (!((CCopyMap *)pUser1->getMap())->isGuildHall())){
		Athena::logger->error("[巅峰约战]被邀请一方已经在副本了");
		return false;
	}
	
	if (pUser2->m_levelMgr.isInLv()){
		Athena::logger->error("[巅峰约战]发起邀请一方已经pve场景");
		FriendProto::stReplyFriendPKInviteResult retCmd;
		retCmd.set_result(FriendProto::stReplyFriendPKInviteResult::eNot_Invited_Has_Enter);
		pUser1->sendProto2Me(retCmd);
		return false;
	}

	if (pUser2->getMap() && pUser2->getMap()->isCopyMap() && (!((CCopyMap *)pUser2->getMap())->isGuildHall())){
		Athena::logger->error("[巅峰约战]发起邀请一方已经在副本了");
		FriendProto::stReplyFriendPKInviteResult retCmd;
		retCmd.set_result(FriendProto::stReplyFriendPKInviteResult::eNot_Invited_Has_Enter);
		pUser1->sendProto2Me(retCmd);
		return false;
	}

	pUser1->setCampID(Map::eCamp_Red);
	pUser2->setCampID(Map::eCamp_Black);

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(pUser1->getUniqueName(),pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[巅峰对决]进入副本失败,创建副本不成功!");
		return false;
	}
	
	if (recvCmd->pkwithfriend()){
		pCopyMap->setPKWithFriend();
	}

	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);
	
	pCopyMap->joinCamp(Map::eCamp_Red,pUser1->getID());
	pCopyMap->joinCamp(Map::eCamp_Black,pUser2->getID());

	pUser1->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	//pUser1->resetHP();
	pUser1->resetBattleProp();
	pUser2->m_levelMgr.notifyEnterLvInfo(pCopyMapBase);
	//pUser2->resetHP();
	pUser2->resetBattleProp();
	CMapMgr::getSingleton().changeMap(pUser1,pCopyMap,1);
	CMapMgr::getSingleton().changeMap(pUser2,pCopyMap,2);

	pUser1->NotifyNormalOnPub(true);
	pUser2->NotifyNormalOnPub(true);
	return true;
}
