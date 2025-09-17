#include "Team.h"
#include "GameUserManager.h"
#include "Log4cxxHelp.h"
#include "GameServer.h"
#include "Global.pb.h"
#include "PacketProcessor.h"
#include "TblDataMgr.h"
#include "TblCopyMapBase.h"
// #include "Guild.h"
#include "ArenaConfig.h"
#include "CopyMap.h"
#include "MapMgr.h"
#include "RobotMgr.h"
#include "MiscCfgMgr.h"
#include "ResBackCfg.h"

UINT32  CTeamMgr::s_teamIDGenID;

extern GameServer * g_gameServer;

void stTeamMember::fillSynInfo(TeamProto::stSynMemberInfo  & data)
{
// 	data.set_charid(m_charID);
// 	data.set_job(m_job);
// 	data.set_level(m_level);
// 	data.set_name(m_name);
// 	data.set_sex(m_sex);
// 	GameUser * pUser = GameUserManager::getSingleton().getUserByID(m_charID);

// 	if (pUser){
// 		data.set_battlepower(pUser->m_battlePower);
// 		data.set_headid(pUser->m_headID);
// 		data.set_bodyid(pUser->m_bodyID);
// 		data.set_cloackid(pUser->m_cloackID);
// 		data.set_shoeid(pUser->m_shoeID);
// 		data.set_weaponid(pUser->m_weaponID);
// 		data.set_wingid(pUser->m_wingID);
// 		data.set_fashionhair(pUser->m_fashionHairID);
// 		data.set_fashionface(pUser->m_fashionFaceID);
// 		data.set_fashionbody(pUser->m_fashionBodyID);
// 		data.set_fashiondecoration(pUser->m_fashionDecorationID);
// 		data.set_fashionshoe(pUser->m_fashionShoeID);
// 		data.set_fashionweapon(pUser->m_fashionWeaponID);
// 		data.set_effectid(pUser->m_effectID);
// 		data.set_wingbreaklv(pUser->m_wingBreakLv);
// #ifdef _HDZ_DEBUG
// 		Athena::logger->trace("组队发送了玩家:charID=%d,wingID:%d,shoeID:%d,fashionShoe:%d",pUser->getID(),pUser->m_wingID,pUser->m_shoeID,pUser->m_fashionShoeID);
// #endif
// 	}
}

CTeam::CTeam()
{
	m_id = 0;
	m_copyMapID = 0;
	bzero(m_members,sizeof(m_members));
	m_hasEnterCopyMap = false;
	m_teamMatching = false;
}

CTeam::~CTeam()
{

}

UINT8 CTeam::getMemSize()
{
	UINT8 ret = 0;
	for (UINT8 i = 0;i < s_MAX_MEMBER;++i){
		if (m_members[i].m_charID){
			ret += 1;
		}	
	}
	return ret;
}

bool CTeam::initTeam(GameUser *pOwner,UINT32 copyMapID)
{
	if (!pOwner || !getID()){
		return false;
	}
	m_copyMapID = copyMapID;
	m_members[0].m_charID = pOwner->getID();
	m_members[0].m_job = pOwner->getJob();
	m_members[0].m_level = pOwner->getLevel();
	m_members[0].m_sex = pOwner->getSex();
	strncpy(m_members[0].m_name,pOwner->getName(),sizeof(m_members[0].m_name));
	return true;
}

void CTeam::sendTeamInfo(GameUser * pUser)
{
	if (!pUser){
		return ;
	}
	
	TeamProto::stRoomInfo retCmd;
	retCmd.set_copymapid(m_copyMapID);
	retCmd.set_teamid(getID());
	for (UINT8 i = 0;i < s_MAX_MEMBER;++i){
		if (m_members[i].m_charID){
			TeamProto::stSynMemberInfo * pSyn = retCmd.add_members();
			if (pSyn){
				m_members[i].fillSynInfo(*pSyn);
				if (0 == i){
					pSyn->set_leader(true);
				}
				else {
					pSyn->set_leader(false);
				}
				pSyn->set_pos(i);
			}
		}
	}
	pUser->sendProto2Me(retCmd);
}

void CTeam::userOnline(GameUser * pUser)
{
	
}

void CTeam::userOffline(GameUser *pUser)
{
	if (!pUser){
		return ;
	}
	leaveTeam(pUser->getID());
}


bool CTeam::joinInTeam(GameUser * pUser,UINT8 pos)
{
	if (!pUser){
		return false;
	}
	if (pos >= s_MAX_MEMBER){
		return false;
	} 

	m_members[pos].m_charID = pUser->getID();
	m_members[pos].m_job = pUser->getJob();
	m_members[pos].m_level = pUser->getLevel();
	m_members[pos].m_sex = pUser->getSex();
	strncpy(m_members[pos].m_name,pUser->getName(),sizeof(m_members[pos].m_name));

	CTeamMgr::getSingleton().addCharID2Team(pUser->getID(),getID());
	return true;
}


bool CTeam::leaveTeam(UINT32 charID)
{
	UINT8 index = (UINT8)-1;
	for (UINT8 i = 0; i< s_MAX_MEMBER;++i){
		if (m_members[i].m_charID == charID){
			index = i;
			break;
		}
	}
	
	if (index >= s_MAX_MEMBER){
		return false;
	}
	
	GameUser * pUser = GameUserManager::getSingleton().getUser(charID);
	if (pUser){
		// CTeamMgr::getSingleton().cancelMatch(pUser,getCopyMapID(),false);
	}
	
	CTeamMgr::getSingleton().removeCharID2Team(charID);

	if (getLeaderID() == charID){//如果是队长离开
		TeamProto::stNotifyFireTeam cmd;
		sendProto2All(cmd);
		for (UINT8 i = 0;i < s_MAX_MEMBER;++i){
			CTeamMgr::getSingleton().removeCharID2Team(m_members[i].m_charID);
			m_members[i].reset();
		}	
		CTeamMatchMgr::getSingleton().removeMatchTeam(getID(),Global::eRealTimePKType_KillAll);
		CTeamMatchMgr::getSingleton().removeMatchTeam(getID(),Global::eRealTimePKType_Finish);
	}
	else {
		TeamProto::stNotifyLeaveTeam cmd;
		cmd.set_charid(charID);
		sendProto2All(cmd);
		m_members[index].reset();
	}

	return true;
}

void CTeam::timer(UINT32 cur)
{
	
}

void CTeam::addInviteInfo(UINT32 charID)
{
	m_inviteList[charID] = g_gameServer->getSec();
}

bool CTeam::isInInviteList(UINT32 charID)
{
	if (m_inviteList.find(charID) != m_inviteList.end()){
		return true;
	}
	return false;
}

void CTeam::eraseCharIDFromInviteList(UINT32 charID)
{
	m_inviteList.erase(charID);
}

UINT16 CTeam::getMemberAvgLevel()
{
	UINT8 Cnt = 0;
	UINT16 Level = 0;
	for (UINT8 i = 0;i < s_MAX_MEMBER;++i){
		if (m_members[i].m_charID){
			Cnt += 1;
			Level += m_members[i].m_level;
		}	
	}
	if (Cnt == 0)
	{
		return 0;
	}
	return Level / Cnt;
}

// void CTeam::enterCopyMap()
// {
// 	if (m_hasEnterCopyMap){
// 		return ;
// 	}
	
// 	GameUser * pLeader = GameUserManager::getSingleton().getUser(getLeaderID());
// 	if (!pLeader){
// 		return ;
// 	}

// 	Global::stTeamEnterCopyMap cmd;
// 	cmd.set_copymapid(m_copyMapID);
// 	cmd.set_teamid(getID());
// 	for (UINT8 i = 0;i < s_MAX_MEMBER; ++i){
// 		Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 		if (pMemInfo){
// 			pMemInfo->set_charid(m_members[i].m_charID);
// 		}
// 	}
// 	// g_globalServer->getConnMgr().broadcastByID(pLeader->getGameID(),cmd);
// 	teamEnterCopymap(&cmd);
// 	m_hasEnterCopyMap = true;
// }

void CTeam::leaveCopyMap()
{
	m_hasEnterCopyMap = false;
}

CTeamMgr::CTeamMgr()
{
}

CTeamMgr::~CTeamMgr()
{

}

bool CTeamMgr::addTeam(CTeam * pTeam)
{
	if (!pTeam || !pTeam->getID()){
		return false;
	}
	return m_allTeam.insert(std::make_pair(pTeam->getID(),pTeam)).second;
}

bool CTeamMgr::removeTeam(CTeam * pTeam)
{
	if (!pTeam || !pTeam->getID()){
		return false;
	}
	m_allTeam.erase(pTeam->getID());
	returnTeamID(pTeam->getID());
	return true;
}

void CTeamMgr::addCharID2Team(UINT32 charID,UINT32 teamID)
{
	m_charID2TeamID[charID] = teamID;
}

void CTeamMgr::removeCharID2Team(UINT32 charID)
{
	m_charID2TeamID.erase(charID);
}

CTeam * CTeamMgr::getTeamByCharID(UINT32 charID)
{
	CharID2TeamID_IT it = m_charID2TeamID.find(charID);
	if (it == m_charID2TeamID.end()){
		return NULL;
	}

	Team_IT subIt = m_allTeam.find(it->second);
	if (subIt == m_allTeam.end()){
		return NULL;
	}
	return subIt->second;
}

CTeam * CTeamMgr::getTeamByID(UINT32 teamID)
{
	Team_IT it = m_allTeam.find(teamID);
	if (it !=m_allTeam.end()){
		return it->second;
	}
	return NULL;
}

void CTeamMgr::userOnline(GameUser * pUser)
{
	if (!pUser){
		return ;
	}
	CTeam * pTeam = getTeamByCharID(pUser->getID());
	if (!pTeam){
		return ;
	}
	pTeam->userOnline(pUser);
}

void CTeamMgr::userOffline(GameUser * pUser)
{
	if (!pUser){
		return ;
	}
	std::map<UINT32, stWaitMatchInfo >::iterator it =  m_waitMatchQueue.begin();
	for (;it != m_waitMatchQueue.end();++it){
		std::set<UINT32>::iterator subIt = it->second.m_waitIDs.find(pUser->getID());
		if (subIt != it->second.m_waitIDs.end()){
			it->second.m_waitIDs.erase(subIt);
			break;
		}
	}

	CTeam * pTeam = getTeamByCharID(pUser->getID());
	if (!pTeam){
		return ;
	}
	UINT32 oldAvgLevel = pTeam->getMemberAvgLevel();
	UINT32 teamID = pTeam->getID();
	pTeam->userOffline(pUser);

	CTeamMatchMgr::getSingleton().OnTeamMemberLeave(teamID, oldAvgLevel);

	if (!pTeam->getMemSize()) {//没有人了
		if (removeTeam(pTeam)){
			it = m_waitMatchTeamQueue.begin();
			for (;it != m_waitMatchTeamQueue.end();++it){
				it->second.m_waitTeamIDs.erase(pTeam->getID());	
			}
			SAFE_DELETE(pTeam);
			return ;
		}
	}
}

bool  CTeamMgr::genTeamID(UINT32 &id)
{
	id = ++s_teamIDGenID;

	return true;
}

void CTeamMgr::returnTeamID(UINT32 id)
{
	//m_teamIDGen.putID(id);	
}

void CTeamMgr::createTeam(GameUser * pUser,UINT32 copyMapID)
{
	if (!pUser){
		return ;
	}

	TeamProto::stCreateTeamResult ret;
	if (getTeamByCharID(pUser->getID())){
		ret.set_result(TeamProto::stCreateTeamResult::eHAS_JOIN_IN_TEAM);
		pUser->sendProto2Me(ret);
#ifdef _HDZ_DEBUG
		Athena::logger->error("已经加入队伍了,不能再创建队伍!");
#endif
		return;
	}

	CTeam * pTeam   = ATHENA_NEW CTeam();
	if (!pTeam){
		ret.set_result(TeamProto::stCreateTeamResult::eINNER_ERROR);
		pUser->sendProto2Me(ret);
		return;
	}

	UINT32 teamID = 0;
	if (!genTeamID(teamID)){
		ret.set_result(TeamProto::stCreateTeamResult::eHAS_JOIN_IN_TEAM);
		pUser->sendProto2Me(ret);
#ifdef _HDZ_DEBUG
		Athena::logger->error("创建房间不成功了,已经超过创建的房间上限了");
#endif
		return;
	}

	pTeam->setID(teamID);			
	if (!pTeam->initTeam(pUser,copyMapID)){
		ret.set_result(TeamProto::stCreateTeamResult::eHAS_JOIN_IN_TEAM);
		pUser->sendProto2Me(ret);
		return;
	}
	addCharID2Team(pUser->getID(),pTeam->getID());
	ret.set_result(TeamProto::stCreateTeamResult::eSUCCESS);
	pUser->sendProto2Me(ret);
	pTeam->sendTeamInfo(pUser);
	m_allTeam[pTeam->getID()] = pTeam;
}

void CTeamMgr::enterTeam(GameUser * pUser,UINT32 teamID)
{
	CTeam * pTeam = getTeamByID(teamID);
	TeamProto::stEnterTeamResult cmd;
	if (!pTeam){
		cmd.set_result(TeamProto::stEnterTeamResult::eTeam_Not_Exist);
		pUser->sendProto2Me(cmd);
		return;
	}
	
	std::map<UINT32, stWaitMatchInfo >::iterator waitMatchIt =  m_waitMatchTeamQueue.find(pTeam->getCopyMapID());
	if (waitMatchIt != m_waitMatchTeamQueue.end()){
		if (waitMatchIt->second.m_waitTeamIDs.find(pTeam->getID()) != waitMatchIt->second.m_waitTeamIDs.end()){
			cmd.set_result(TeamProto::stEnterTeamResult::eTeam_Is_Auto_Match);
			pUser->sendProto2Me(cmd);
			return;
		}
	}

	stCopyMapBase * pCopyMapBase =  g_copyMapBaseMgr.get(pTeam->getCopyMapID());
	if (pCopyMapBase){
		// if (eLevelType_Guild_Copymap == pCopyMapBase->m_type){
		// 	CGuild * pGuild1 = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
		// 	CGuild * pGuild2 = CGuildMgr::getSingleton().getGuildByCharID(pTeam->getLeaderID());
		// 	if (pGuild1 != pGuild2){
		// 		cmd.set_result(TeamProto::stEnterTeamResult::eTeam_Not_Exist);
		// 		pUser->sendProto2Me(cmd);
		// 		return;
		// 	}
		// }
	}

	if (!pTeam->joinInTeam(pUser,(UINT8)-1)){
		cmd.set_result(TeamProto::stEnterTeamResult::eHas_In_Team);
		pUser->sendProto2Me(cmd);
		return;	
	}
	pTeam->sendTeamInfo(pUser);
}

void CTeamMgr::leaveTeam(GameUser * pUser)
{
	if (!pUser){
		return ;
	}

	CTeam * pTeam = getTeamByCharID(pUser->getID());
	if (!pTeam){
		return;
	}

	if (!pTeam->leaveTeam(pUser->getID())){
		return;
	}

	if (!pTeam->getLeaderID()){//队长离开了房间,要换队长

	}

	if (!pTeam->getMemSize()) {//没有人了
		if (removeTeam(pTeam)){
			SAFE_DELETE(pTeam);
			return;
		}
	}
}

// void CTeamMgr::fireTeam(GameUser * pUser,bool bNotify)
// {
// 	if (!pUser){
// 		return ;
// 	}

// 	CTeam * pTeam = getTeamByCharID(pUser->getID());
// 	if (!pTeam){
// 		return;
// 	}

// 	if (pUser->getID() != pTeam->getLeaderID()){//不是队长不可以解散
// 		return;
// 	}

// 	if (removeTeam(pTeam)){
// 		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER;++i){
// 			CTeamMgr::getSingleton().removeCharID2Team(pTeam->getCharID(i));
// 		}
// 		if (bNotify){
// 			TeamProto::stNotifyFireTeam cmd;
// 			pTeam->sendProto2All(cmd);
// 		}
// 		SAFE_DELETE(pTeam);
// 		return;
// 	}
// }

// void CTeamMgr::inviteJoinIn(GameUser * pUser,UINT32 charID,UINT8 pos,TeamProto::eTeamType eType)
// {
// 	if (!pUser){
// 		return ;
// 	}

// 	CTeam * pTeam = getTeamByCharID(pUser->getID());
// 	if (!pTeam){
// 		return;
// 	}
	
// 	TeamProto::stInviteJoinInTeamResult ret;
// 	if (pTeam->getLeaderID() != pUser->getID()){
// 		ret.set_result(TeamProto::stInviteJoinInTeamResult::NOT_LEADER);
// 		pUser->sendProto2Me(ret);
// 		return;
// 	}

// 	GameUser * pOther = GameUserManager::getSingleton().getUser(charID);
// 	if (!pOther){
// 		ret.set_result(TeamProto::stInviteJoinInTeamResult::USER_OFFLINE);
// 		pUser->sendProto2Me(ret);
// 		return;
// 	}
	
// 	TeamProto::stAskJoinInTeam askCmd;
// 	askCmd.set_pos(pos);
// 	askCmd.set_invitename(pUser->getName());
// 	askCmd.set_teamid(pTeam->getID());
// 	askCmd.set_etype(eType);
// 	askCmd.set_copymapid(pTeam->getCopyMapID());
// 	pOther->sendProto2Me(askCmd);
// 	pTeam->addInviteInfo(pOther->getID());
// }

// void CTeamMgr::replyInvite(GameUser * pUser,UINT32 teamID,bool agree,UINT8 pos,TeamProto::eTeamType eType,bool timeUseOut)
// {
// 	if (!pUser){
// 		return ;
// 	}	
// 	TeamProto::stInviteJoinInTeamResult ret;
// 	ret.set_etype(eType);
// 	CTeam * pTeam = getTeamByID(teamID);
// 	if (!pTeam){
// 		ret.set_result(TeamProto::stInviteJoinInTeamResult::TEAM_NOT_EXSIT);
// 		pUser->sendProto2Me(ret);
// 		return;
// 	}
	
// 	std::map<UINT32, stWaitMatchInfo >::iterator waitMatchIt =  m_waitMatchTeamQueue.find(pTeam->getCopyMapID());

// 	if (waitMatchIt != m_waitMatchTeamQueue.end()){
// 		if (waitMatchIt->second.m_waitTeamIDs.find(pTeam->getID()) != waitMatchIt->second.m_waitTeamIDs.end()){
// 			ret.set_result(TeamProto::stInviteJoinInTeamResult::TEAM_IS_AUTO_MATCH);
// 			pUser->sendProto2Me(ret);
// 			return;
// 		}
// 	}

// 	if (!pTeam->isInInviteList(pUser->getID())){
// 		Athena::logger->error("name=%s,account=%s,id=%d答复进入队伍邀请时候不在邀请列表",pUser->getAccount(),pUser->getName(),pUser->getID());
// 		ret.set_result(TeamProto::stInviteJoinInTeamResult::NOT_IN_APPLY_LIST);
// 		pUser->sendProto2Me(ret);
// 		return;
// 	}
// 	pTeam->eraseCharIDFromInviteList(pUser->getID());	
// 	if (!agree){
// 		if (timeUseOut){
// 			ret.set_result(TeamProto::stInviteJoinInTeamResult::COPYMAP_NUM_USE_OUT);
// 			pUser->sendProto2Me(ret);
// 		}
// 		else {
// 			ret.set_result(TeamProto::stInviteJoinInTeamResult::DISAGREE);
// 		}
// 	}
// 	else {
// 		if (pTeam->joinInTeam(pUser,pos)){
// 			pTeam->sendTeamInfo(pUser);
// 		}
// 		else {
// 			ret.set_result(TeamProto::stInviteJoinInTeamResult::OTHER);
// 			pUser->sendProto2Me(ret);
// 		}
// 	}
// }

// void CTeamMgr::enterLv(GameUser * pUser)
// {
// 	if (!pUser){
// 		return ;
// 	}

// 	CTeam * pTeam = getTeamByCharID(pUser->getID());
// 	if (!pTeam){
// 		return;
// 	}

// 	if (pTeam->getLeaderID() != pUser->getID()){
// 		Athena::logger->error("account=%s,name=%s,charID=%d,进入请求进入副本不成功,不是队长!",pUser->getAccount(),pUser->getName(),pUser->getID());
// 		return;
// 	}
// 	pTeam->enterCopyMap();
// 	fireTeam(pUser,false);
// }

// void CTeamMgr::timer(UINT32 cur)
// {
// 	std::map<UINT32, stWaitMatchInfo >::iterator it =  m_waitMatchQueue.begin();	

// 	for (;it != m_waitMatchQueue.end();++it){
// 		stWaitMatchInfo & info = it->second;
// 		if (cur > info.m_beginWaitTime + s_MATCH_MAX_WAIT_TIME){
// 			if (info.m_waitIDs.size() <= 0){
// 				continue;
// 			}
// 			info.m_beginWaitTime = g_gameServer->getSec();
// 			Global::stTeamEnterCopyMap cmd;
// 			cmd.set_copymapid(it->first);
// 			cmd.set_teamid(0);
// 			std::set<UINT32>::iterator subIt = it->second.m_waitIDs.begin();
// 			for (;subIt != it->second.m_waitIDs.end();){
// 				std::set<UINT32>::iterator tmpSubIt = subIt;
// 				++subIt;
// 				Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 				if (pMemInfo){
// 					pMemInfo->set_charid(*tmpSubIt);
// 				}
// 				it->second.m_waitIDs.erase(tmpSubIt);
				
// 				if (cmd.memerinfo_size() >= CTeam::s_MAX_MEMBER){
// 					break;
// 				}
// 			}
// 			// g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
// 			teamEnterCopymap(&cmd);
// 		}
// 	}

// 	it = m_waitMatchTeamQueue.begin();
// 	for (;it != m_waitMatchTeamQueue.end();++it){
// 		std::map<UINT32,UINT32>::iterator subIt = it->second.m_waitTeamIDs.begin();

// 		for (;subIt != it->second.m_waitTeamIDs.end();){
// 			std::map<UINT32,UINT32>::iterator tmpSubIt = subIt;
// 			++subIt;
// 			if (tmpSubIt->second + s_MATCH_MAX_WAIT_TIME < g_gameServer->getSec()){
// 				CTeam * pWaitTeam = getTeamByID(tmpSubIt->first);
// 				if (pWaitTeam){
// 					Global::stTeamEnterCopyMap cmd;
// 					cmd.set_copymapid(it->first);
// 					cmd.set_teamid(0);

// 					UINT32 memCharID = pWaitTeam->getCharID(0);
// 					if (memCharID > 0){
// 						Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 						if (pMemInfo){
// 							pMemInfo->set_charid(memCharID);
// 						}
// 					}

// 					memCharID = pWaitTeam->getCharID(1);
// 					if (memCharID > 0){
// 						Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 						if (pMemInfo){
// 							pMemInfo->set_charid(memCharID);
// 						}
// 					}

// 					memCharID = pWaitTeam->getCharID(2);
// 					if (memCharID > 0){
// 						Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 						if (pMemInfo){
// 							pMemInfo->set_charid(memCharID);
// 						}
// 					}

// 					if (CTeamMgr::getSingleton().removeTeam(pWaitTeam)){
// 						SAFE_DELETE(pWaitTeam);
// 					}
// 					// g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
// 					teamEnterCopymap(&cmd);
// 					it->second.m_waitTeamIDs.erase(tmpSubIt);
// 				}
// 			}
// 		}
// 	}
// }

// void CTeamMgr::autoMatch(GameUser * pUser,UINT32 copyMapID)
// {
// 	if (!pUser || copyMapID <= 0){
// 		return ;
// 	}

// 	std::map<UINT32, stWaitMatchInfo>::iterator it =  m_waitMatchQueue.begin();
// 	for (;it != m_waitMatchQueue.end();++it){
// 		if (it->second.m_waitIDs.find(pUser->getID()) != it->second.m_waitIDs.end()){
// 			Athena::logger->error("请求自动请求匹配错误,已经组队错误!");
// 			return ;
// 		}
// 	}

// 	CTeam * pMyTeam = getTeamByCharID(pUser->getID());	
	
// 	if (pMyTeam){
// 		it = m_waitMatchTeamQueue.begin();
// 		for (;it != m_waitMatchTeamQueue.end();++it){
// 			if (it->second.m_waitTeamIDs.find(pMyTeam->getID()) != it->second.m_waitTeamIDs.end()){
// 				Athena::logger->error("请求自动请求匹配错误,已经组队错误!");
// 				return ;	
// 			}	
// 		}
// 	}
	
// 	stWaitMatchInfo & waitMatchQueue = m_waitMatchQueue[copyMapID];
// 	stWaitMatchInfo & waitMatchTeamQueue = m_waitMatchTeamQueue[copyMapID];

// 	if (pMyTeam && (pMyTeam->getMemSize() == 1)){//1个人一个队伍情况
// 		if (waitMatchQueue.m_waitIDs.size() <= 0){
// 			waitMatchQueue.m_beginWaitTime = g_gameServer->getSec();
// 		}
// 		waitMatchQueue.m_waitIDs.insert(pUser->getID());	
// 		if (CTeamMgr::getSingleton().removeTeam(pMyTeam)){
// 			SAFE_DELETE(pMyTeam);
// 		}
// 	}
// 	else if (pMyTeam && (pMyTeam->getMemSize() == 2)){//2个人一个队伍情况
// 		waitMatchTeamQueue.m_waitTeamIDs.insert(std::make_pair(pMyTeam->getID(),g_gameServer->getSec()));	
// 	}
// 	else if (pMyTeam){//三个人一个队伍不应该发送匹配协议
// 		Athena::logger->error("三个人一个队伍不应该发送匹配协议");
// 		return ;
// 	}
// 	else {
// 		if (waitMatchQueue.m_waitIDs.size() <= 0){
// 			waitMatchQueue.m_beginWaitTime = g_gameServer->getSec();
// 		}
// 		waitMatchQueue.m_waitIDs.insert(pUser->getID());
// 	}

// 	if (waitMatchQueue.m_waitIDs.size() < CTeam::s_MAX_MEMBER){//人数不足一个队伍的情况
// 		TeamProto::stWaitMatchTeam waitCmd;
// 		if (waitMatchTeamQueue.m_waitIDs.size() <= 0){
// 			pUser->sendProto2Me(waitCmd);
// 		}

// 		if ((waitMatchQueue.m_waitIDs.size() > 0) && (waitMatchTeamQueue.m_waitTeamIDs.size() > 0)){
// 			UINT32 teamID = waitMatchTeamQueue.m_waitTeamIDs.begin()->first; 
// 			CTeam * pWaitTeam = getTeamByID(teamID);
// 			if (pWaitTeam){
// 				waitMatchTeamQueue.m_waitTeamIDs.erase(teamID);
// 				Global::stTeamEnterCopyMap cmd;
// 				cmd.set_copymapid(copyMapID);
// 				cmd.set_teamid(0);
// 				std::set<UINT32>::iterator subIt = waitMatchQueue.m_waitIDs.begin();
// 				if (subIt != waitMatchQueue.m_waitIDs.end()){
// 					Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 					if (pMemInfo){
// 						pMemInfo->set_charid(*subIt);
// 						waitMatchQueue.m_waitIDs.erase(subIt);
// 					}
// 				}
				
// 				UINT32 memCharID = pWaitTeam->getCharID(0);
// 				if (memCharID > 0){
// 					Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 					if (pMemInfo){
// 						pMemInfo->set_charid(memCharID);
// 					}
// 				}
				
// 				memCharID = pWaitTeam->getCharID(1);
// 				if (memCharID > 0){
// 					Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 					if (pMemInfo){
// 						pMemInfo->set_charid(memCharID);
// 					}
// 				}
				
// 				memCharID = pWaitTeam->getCharID(2);
// 				if (memCharID > 0){
// 					Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 					if (pMemInfo){
// 						pMemInfo->set_charid(memCharID);
// 					}
// 				}

// 				if (CTeamMgr::getSingleton().removeTeam(pWaitTeam)){
// 					SAFE_DELETE(pWaitTeam);
// 				}
				
// 				// g_globalServer->getConnMgr().broadcastByID(pUser->getGameID(),cmd);
// 				teamEnterCopymap(&cmd);
// 			}
// 		}
// 		else if (pMyTeam){
// 			pMyTeam->sendProto2All(waitCmd);
// 		}
// 		else {
// 			pUser->sendProto2Me(waitCmd);
// 		}
// 	}
// 	else {
// 		TeamProto::stMathcTeamSuccess retCmd;
// 		pUser->sendProto2Me(retCmd);
// 		Global::stTeamEnterCopyMap cmd;
// 		cmd.set_copymapid(copyMapID);
// 		cmd.set_teamid(0);
// 		std::set<UINT32>::iterator subIt = waitMatchQueue.m_waitIDs.begin();
// 		for (;subIt != waitMatchQueue.m_waitIDs.end();){
// 			std::set<UINT32>::iterator tmpSubIt = subIt;
// 			++subIt;
// 			Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
// 			if (pMemInfo){
// 				pMemInfo->set_charid(*tmpSubIt);
// 			}
// 			waitMatchQueue.m_waitIDs.erase(tmpSubIt);

// 			if (cmd.memerinfo_size() >= CTeam::s_MAX_MEMBER)
// 			{
// 				break;
// 			}
// 		}
// 		// g_globalServer->getConnMgr().broadcastByID(pUser->getGameID(),cmd);
// 		teamEnterCopymap(&cmd);
// 	}
// }

// void CTeamMgr::cancelMatch(GameUser * pUser,UINT32 copyMapID,bool bNotify)
// {
// 	if (!pUser){
// 		return ;
// 	}
	
// 	if (0 == copyMapID){
// 		Athena::logger->error("玩家(name:%s,id:%d)取消组队副本时候出现错误,副本ID为0",pUser->getName(),pUser->getID());
// 	}

// 	std::map<UINT32, stWaitMatchInfo >::iterator it =  m_waitMatchQueue.begin();
// 	for (;it != m_waitMatchQueue.end();++it){
// 		if (it->first  == copyMapID){
// 			it->second.m_waitIDs.erase(pUser->getID());
// 		}
// 	}

// 	CTeam * pMyTeam = getTeamByCharID(pUser->getID());	
// 	TeamProto::stCancelAutoMatchTeamResult retCmd;
// 	retCmd.set_result(TeamProto::stCancelAutoMatchTeamResult::eSUCCESS);	

// 	if (pMyTeam){
// 		it =  m_waitMatchTeamQueue.begin();
// 		for (;it != m_waitMatchTeamQueue.end();++it){
// 			if (it->first == copyMapID){
// 				it->second.m_waitTeamIDs.erase(pMyTeam->getID());
// 			}
// 		}
// 		if (bNotify){
// 			pMyTeam->sendProto2All(retCmd);
// 		}
// 	}
// 	else {
// 		if (bNotify){
// 			pUser->sendProto2Me(retCmd);
// 		}
// 	}
// }

CTeamMatchMgr::CTeamMatchMgr()
{
	m_lastCheckTime = 0;
}

CTeamMatchMgr::~CTeamMatchMgr()
{
	
}

void CTeamMatchMgr::addMatchTeam(GameUser *pUser,CTeam *pTeam,Global::eRealTimePKType type)
{
	if (!pUser || !pTeam){
		return ;
	}
	
	if (!isOpen(type)){
		TeamProto::stRealTimePKNotOpen failCmd;
		if (type == Global::eRealTimePKType_KillAll){
			failCmd.set_etype(TeamProto::eRealTimePKType_KillAll);
		}
		else if (type == Global::eRealTimePKType_Finish){
			failCmd.set_etype(TeamProto::eRealTimePKType_Finish);
		}
		pUser->sendProto2Me(failCmd);
		return ;
	}

	std::map<UINT16, stPvpWaitMatchInfo > * pWaitMatchTeam = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchTeam = &m_waitMatchTeams[0]; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchTeam = &m_waitMatchTeams[1];
	}

	if (!pWaitMatchTeam){
		return ;
	}

	UINT16 avgLevel = pTeam->getMemberAvgLevel();
	if (pWaitMatchTeam->find(avgLevel) == pWaitMatchTeam->end())
	{
		stPvpWaitMatchInfo	stInfo;
		stInfo.m_Level = avgLevel;
		stInfo.m_waitIDs[pTeam->getID()] = g_gameServer->getSec();
		(*pWaitMatchTeam)[avgLevel] = stInfo;
	}
	else
	{
		(*pWaitMatchTeam)[avgLevel].m_waitIDs[pTeam->getID()] = g_gameServer->getSec();
	}
	
	
	TeamProto::stRealTimeTeamMatchWait retCmd;
	if (type == Global::eRealTimePKType_KillAll){
		retCmd.set_etype(TeamProto::eRealTimePKType_KillAll);
	}
	else if (type == Global::eRealTimePKType_Finish){
		retCmd.set_etype(TeamProto::eRealTimePKType_Finish);
	}
	
	pTeam->sendProto2All(retCmd);

	
	return ;
}

bool CTeamMatchMgr::removeMatchTeam(UINT32 teamID,Global::eRealTimePKType type)
{
	std::map<UINT16, stPvpWaitMatchInfo > * pWaitMatchTeam = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchTeam = &m_waitMatchTeams[0]; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchTeam = &m_waitMatchTeams[1];
	}
	
	if (!pWaitMatchTeam){
		return false;
	}
	
	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(teamID);
	if (!pTeam){
		return false;
	}

	TeamProto::stCancelRealTimeTeamMatchResult retCmd;
	retCmd.set_success(true);
	pTeam->sendProto2All(retCmd);

	UINT16 avgLevel = pTeam->getMemberAvgLevel();
	if (pWaitMatchTeam->find(avgLevel) != pWaitMatchTeam->end()
		&& (*pWaitMatchTeam)[avgLevel].m_waitIDs.find(pTeam->getID()) != (*pWaitMatchTeam)[avgLevel].m_waitIDs.end())
	{
		(*pWaitMatchTeam)[avgLevel].m_waitIDs.erase(pTeam->getID());
	}
	else
	{
		//对应等级的等待队列中没有找到该队伍，可能等级变化了，全表搜索，确保删除
		std::map<UINT16, stPvpWaitMatchInfo >::iterator itr = pWaitMatchTeam->begin();
		for (; itr != pWaitMatchTeam->end(); ++itr)
		{
			itr->second.m_waitIDs.erase(pTeam->getID());
		}
	}
	
	return true;
}

bool CTeamMatchMgr::isOpen(Global::eRealTimePKType type)
{
	struct tm _tm; 
	RealTime::getLocalTime(_tm,g_gameServer->getSec());
	
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

	//if(Athena::global["CountryVersion"] == "Korea"){
		//星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
		UINT16 wday = _tm.tm_wday;
		secNum = secNum + wday * 86400 ;
	//}
	if (type == Global::eRealTimePKType_KillAll){
		return ArenaConfig::getSingleton().isRealTimeOpen(secNum);
	}
	else if (type == Global::eRealTimePKType_Finish){
		return ArenaConfig::getSingleton().isMobaOpen(secNum);
	}
	return false;
}

void CTeamMatchMgr::addMatchUser(GameUser * pUser,Global::eRealTimePKType type)
{
	if (!pUser){
		return ;
	}

	if (!isOpen(type)){
		TeamProto::stRealTimePKNotOpen failCmd;
		if (type == Global::eRealTimePKType_KillAll){
			failCmd.set_etype(TeamProto::eRealTimePKType_KillAll);
		}
		else if (type == Global::eRealTimePKType_Finish){
			failCmd.set_etype(TeamProto::eRealTimePKType_Finish);
		}
		pUser->sendProto2Me(failCmd);
		return ;
	}

	std::map<UINT16, stPvpWaitMatchInfo > * pWaitMatchUser = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchUser = &m_waitMatchUser[0]; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchUser = &m_waitMatchUser[1];
	}

	if (!pWaitMatchUser){
		return ;
	}

	if (pWaitMatchUser->find(pUser->getLevel()) == pWaitMatchUser->end())
	{
		stPvpWaitMatchInfo	stInfo;
		stInfo.m_Level = pUser->getLevel();
		stInfo.m_waitIDs[pUser->getID()] = g_gameServer->getSec();
		(*pWaitMatchUser)[pUser->getLevel()] = stInfo;
	}
	else
	{
		(*pWaitMatchUser)[pUser->getLevel()].m_waitIDs[pUser->getID()] = g_gameServer->getSec();
	}

	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByCharID(pUser->getID());
	if (pTeam){
		CTeamMgr::getSingleton().removeCharID2Team(pUser->getID());
		if (CTeamMgr::getSingleton().removeTeam(pTeam)){
			SAFE_DELETE(pTeam);
		} 
	}

	TeamProto::stRealTimeSingleMatchWait retCmd;
	if (type == Global::eRealTimePKType_KillAll){
		retCmd.set_etype(TeamProto::eRealTimePKType_KillAll);
	}
	else if (type == Global::eRealTimePKType_Finish){
		retCmd.set_etype(TeamProto::eRealTimePKType_Finish);
	}
	pUser->sendProto2Me(retCmd);

	return ;	
}

void CTeamMatchMgr::removeMatchUser(GameUser * pUser,Global::eRealTimePKType type)
{
	if (!pUser){
		return ;
	}
	std::map<UINT16, stPvpWaitMatchInfo >* pWaitMatchUser = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchUser = &m_waitMatchUser[0];
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchUser = &m_waitMatchUser[1];
	}

	if (!pWaitMatchUser){
		return ;
	}

	if (pWaitMatchUser->find(pUser->getLevel()) != pWaitMatchUser->end()
		&& (*pWaitMatchUser)[pUser->getLevel()].m_waitIDs.find(pUser->getID()) 
		!= (*pWaitMatchUser)[pUser->getLevel()].m_waitIDs.end())
	{
		(*pWaitMatchUser)[pUser->getLevel()].m_waitIDs.erase(pUser->getID());
	}
	else
	{
		//对应等级的等待队列中没有找到该玩家，可能等级变化了，全表搜索该玩家，确保删除
		std::map<UINT16, stPvpWaitMatchInfo >::iterator itr = pWaitMatchUser->begin();
		for (; itr != pWaitMatchUser->end(); ++itr)
		{
			itr->second.m_waitIDs.erase(pUser->getID());
		}
	}
	
}

void CTeamMatchMgr::userOffline(GameUser * pUser)
{
	if (!pUser){
		return ;
	}
	for (int i = 0; i < s_MAX_GAME_TYPE; ++i)
	{
		if (m_waitMatchUser[i].find(pUser->getLevel()) != m_waitMatchUser[i].end())
		{
			m_waitMatchUser[i][pUser->getLevel()].m_waitIDs.erase(pUser->getID());
		}
	}
}

void CTeamMatchMgr::timer(UINT32 cur)
{
	if (m_lastCheckTime + s_CHECK_MATCH_INTERVAL < g_gameServer->getSec()){
		m_lastCheckTime = g_gameServer->getSec();
		checkMatch();
	}
}

void CTeamMatchMgr::checkMatchReset()
{
	std::map<UINT16, std::map<UINT32, UINT32> >::iterator it = m_matchUserStep2.begin();
	for (;it != m_matchUserStep2.end();++it){
		it->second.clear();	
	}

	it = m_matchTeamStep2.begin();
	for (;it != m_matchTeamStep2.end();++it){
		it->second.clear();
	}
	
	m_matchUserStep3.clear();
	m_matchTeamStep3.clear();
}

void CTeamMatchMgr::checkMatch()
{
	int maxLevel = CMiscCfgMgr::getSingleton().getMaxRoleLvl();

	std::map<UINT32, UINT32> backUser;
	std::map<UINT32, UINT32> backTeam;
	std::map<UINT32, UINT32> tempUser;
	std::map<UINT32, UINT32> tempTeam;
	std::map<UINT32, UINT32> *pWaitMatchUser = NULL;
	std::map<UINT32, UINT32> *pWaitMatchTeam = NULL;

	Global::eRealTimePKType types[s_MAX_GAME_TYPE] = {Global::eRealTimePKType_KillAll, Global::eRealTimePKType_Finish};
	
	for (int typeIdx = 0; typeIdx < s_MAX_GAME_TYPE; ++typeIdx)  
	{
		checkMatchReset();
		//扫描每个等级的队列，如果可以匹配，则队列内立刻匹配
		for (int i = maxLevel; i > 0; --i)
		{
			pWaitMatchUser = NULL;
			pWaitMatchTeam = NULL;
			tempUser.clear();
			tempTeam.clear();

			//队伍队列
			if (m_waitMatchTeams[typeIdx].find(i) != m_waitMatchTeams[typeIdx].end())
			{
				pWaitMatchTeam = &(m_waitMatchTeams[typeIdx][i].m_waitIDs);
			}
			else
			{
				pWaitMatchTeam = &tempTeam;
			}

			//玩家队列
			if (m_waitMatchUser[typeIdx].find(i) != m_waitMatchUser[typeIdx].end())
			{
				pWaitMatchUser = &(m_waitMatchUser[typeIdx][i].m_waitIDs);
			}
			else
			{
				pWaitMatchUser = &tempUser;
			}

			//匹配
			while(checkCanEnterCopyMap(types[typeIdx],pWaitMatchUser,pWaitMatchTeam,&backUser,&backTeam)){
				//此时传的pWaitMatchUser和pWaitMatchTeam 为原本的map，匹配到的数据已经erase了，不必再次处理
				for (std::map<UINT32, UINT32>::iterator itBackTeam = backTeam.begin(); itBackTeam != backTeam.end(); ++itBackTeam)
				{

					Athena::logger->trace("[实时战场]1阶段 匹配到队伍 teamID[%d] ", itBackTeam->first);

					CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(itBackTeam->first);
					if (pTeam)
					{
						if (CTeamMgr::getSingleton().removeTeam(pTeam)){
							SAFE_DELETE(pTeam);
						}
					}
				}
				backTeam.clear();

				for (std::map<UINT32, UINT32>::iterator itBackUser = backUser.begin(); itBackUser != backUser.end(); ++itBackUser)
				{
					Athena::logger->trace("[实时战场]1阶段 匹配到玩家 charID[%d] ", itBackUser->first);
				}

				backUser.clear();
			}
		}

		//第一次扫描后，扫描满足进入阶段2的玩家及队伍
		//先扫描队伍
		for (std::map<UINT16, stPvpWaitMatchInfo >::reverse_iterator itTeam = m_waitMatchTeams[typeIdx].rbegin(); itTeam != m_waitMatchTeams[typeIdx].rend(); ++itTeam)
		{
			UINT16 level = itTeam->first;
			stPvpWaitMatchInfo& info = itTeam->second;
			for (std::map<UINT32, UINT32>::iterator it = info.m_waitIDs.begin(); it != info.m_waitIDs.end(); ++it)
			{
				if (it->second + s_WAIT_TIME_1 < g_gameServer->getSec()) //进入2阶段
				{
					if (m_matchTeamStep2.find(level) == m_matchTeamStep2.end());	
					{
						std::map<UINT32, UINT32>	tempMap;
						m_matchTeamStep2[level] = tempMap;
					}
					m_matchTeamStep2[level][it->first] = level;
				}
			}
		}

		//再扫描玩家
		for (std::map<UINT16, stPvpWaitMatchInfo >::reverse_iterator itUser = m_waitMatchUser[typeIdx].rbegin(); itUser != m_waitMatchUser[typeIdx].rend(); ++itUser)
		{
			UINT16 level = itUser->first;
			stPvpWaitMatchInfo& info = itUser->second;
			for (std::map<UINT32, UINT32>::iterator it = info.m_waitIDs.begin(); it != info.m_waitIDs.end(); ++it)
			{
				if (it->second + s_WAIT_TIME_1 < g_gameServer->getSec()) //进入2阶段
				{
					if (m_matchUserStep2.find(level) == m_matchUserStep2.end());	
					{
						std::map<UINT32, UINT32>	tempMap;
						m_matchUserStep2[level] = tempMap;
					}
					m_matchUserStep2[level][it->first] = level;
				}
			}
		}

		//处理2阶段匹配
		for (int i = maxLevel; i > 5; --i){
			int toLevel = i - 5;

			tempUser.clear();
			tempTeam.clear();
			//把这个等级段的Team和User合并
			for (int j = i; j >= toLevel;--j)
			{
				std::map<UINT16, std::map<UINT32, UINT32> >::iterator it1 = m_matchTeamStep2.find(j);
				if (it1 != m_matchTeamStep2.end())
				{
					std::map<UINT32, UINT32>::iterator it2 = it1->second.begin();
					for (; it2 != it1->second.end(); ++it2)
					{
						tempTeam[it2->first] = it2->second;
					}
				}
				it1 = m_matchUserStep2.find(j);
				if (it1 != m_matchUserStep2.end())
				{
					std::map<UINT32, UINT32>::iterator it2 = it1->second.begin();
					for (; it2 != it1->second.end(); ++it2)
					{
						tempUser[it2->first] = it2->second;
					}
				}
			}

			backUser.clear();
			backTeam.clear();
			while(checkCanEnterCopyMap(types[typeIdx],&tempUser,&tempTeam,&backUser,&backTeam)){
				for (std::map<UINT32, UINT32>::iterator itBackTeam = backTeam.begin(); itBackTeam != backTeam.end(); ++itBackTeam)
				{

					Athena::logger->trace("[实时战场]2阶段 匹配到队伍 teamID[%d] teamLevel[%d]", itBackTeam->first,itBackTeam->second);

					if (m_waitMatchTeams[typeIdx].find(itBackTeam->second) != m_waitMatchTeams[typeIdx].end())
					{
						m_waitMatchTeams[typeIdx][itBackTeam->second].m_waitIDs.erase(itBackTeam->first);
					}
					if (m_matchTeamStep2.find(itBackTeam->second) != m_matchTeamStep2.end())
					{
						m_matchTeamStep2[itBackTeam->second].erase(itBackTeam->first);
					}
					CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(itBackTeam->first);
					if (pTeam)
					{
						if (CTeamMgr::getSingleton().removeTeam(pTeam)){
							SAFE_DELETE(pTeam);
						}
					}
				}
				backTeam.clear();
				for (std::map<UINT32, UINT32>::iterator itBackUser = backUser.begin(); itBackUser != backUser.end(); ++itBackUser)
				{

					Athena::logger->trace("[实时战场]2阶段 匹配到玩家 charID[%d] Level[%d]", itBackUser->first,itBackUser->second);

					if (m_waitMatchUser[typeIdx].find(itBackUser->second) != m_waitMatchUser[typeIdx].end())
					{
						m_waitMatchUser[typeIdx][itBackUser->second].m_waitIDs.erase(itBackUser->first);
					}
					if (m_matchUserStep2.find(itBackUser->second) != m_matchUserStep2.end())
					{
						m_matchUserStep2[itBackUser->second].erase(itBackUser->first);
					}
				}
				backUser.clear();
			}
		}

		//阶段2处理完，扫描满足阶段3的玩家及队伍
		//先扫描队伍
		for (std::map<UINT16, stPvpWaitMatchInfo >::reverse_iterator itTeam = m_waitMatchTeams[typeIdx].rbegin(); itTeam != m_waitMatchTeams[typeIdx].rend(); ++itTeam)
		{
			UINT16 level = itTeam->first;
			stPvpWaitMatchInfo& info = itTeam->second;
			for (std::map<UINT32, UINT32>::iterator it = info.m_waitIDs.begin(); it != info.m_waitIDs.end(); ++it)
			{
				if (it->second + s_WAIT_TIME_2 < g_gameServer->getSec()) //到了3阶段
				{
					m_matchTeamStep3[it->first] = level;
				}
			}
		}

		//再扫描玩家
		for (std::map<UINT16, stPvpWaitMatchInfo >::reverse_iterator itUser = m_waitMatchUser[typeIdx].rbegin(); itUser != m_waitMatchUser[typeIdx].rend(); ++itUser)
		{
			UINT16 level = itUser->first;
			stPvpWaitMatchInfo& info = itUser->second;
			for (std::map<UINT32, UINT32>::iterator it = info.m_waitIDs.begin(); it != info.m_waitIDs.end(); ++it)
			{
				if (it->second + s_WAIT_TIME_2 < g_gameServer->getSec()) //到了3阶段
				{
					m_matchUserStep3[it->first] = level;
				}
			}
		}

		//处理3阶段匹配
		backUser.clear();
		backTeam.clear();
		while(checkCanEnterCopyMap(types[typeIdx],&m_matchUserStep3,&m_matchTeamStep3,&backUser,&backTeam, true)){
			for (std::map<UINT32, UINT32>::iterator itBackTeam = backTeam.begin(); itBackTeam != backTeam.end(); ++itBackTeam)
			{

				Athena::logger->trace("[实时战场]3阶段 匹配到队伍 teamID[%d] teamLevel[%d]", itBackTeam->first,itBackTeam->second);

				if (m_waitMatchTeams[typeIdx].find(itBackTeam->second) != m_waitMatchTeams[typeIdx].end())
				{
					m_waitMatchTeams[typeIdx][itBackTeam->second].m_waitIDs.erase(itBackTeam->first);
				}
				CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(itBackTeam->first);
				if (pTeam)
				{
					if (CTeamMgr::getSingleton().removeTeam(pTeam)){
						SAFE_DELETE(pTeam);
					}
				}
			}
			backTeam.clear();
			for (std::map<UINT32, UINT32>::iterator itBackUser = backUser.begin(); itBackUser != backUser.end(); ++itBackUser)
			{

				Athena::logger->trace("[实时战场]3阶段 匹配到玩家 charID[%d] Level[%d]", itBackUser->first,itBackUser->second);

				if (m_waitMatchUser[typeIdx].find(itBackUser->second) != m_waitMatchUser[typeIdx].end())
				{
					m_waitMatchUser[typeIdx][itBackUser->second].m_waitIDs.erase(itBackUser->first);
				}
			}
			backUser.clear();
		}
	}
}

//匹配，如果匹配成功，队伍和用户会从pWaitMatchTeam和pWaitMatchUser中删除，匹配到的用户和队伍，会加入到back列表返回给调用者
bool CTeamMatchMgr::checkCanEnterCopyMap(Global::eRealTimePKType type,
	std::map<UINT32, UINT32> *pWaitMatchUser,
	std::map<UINT32, UINT32> *pWaitMatchTeam,
	std::map<UINT32, UINT32> *backUser,
	std::map<UINT32, UINT32> *backTeam,
	bool forceMatch)
{
	if (!pWaitMatchUser || !pWaitMatchTeam){
		return false;
	}
	if (pWaitMatchTeam->size() == 0 && pWaitMatchUser->size() == 0)
	{
		return false;
	}

	// Athena::logger->trace("checkCanEnterCopyMap  %d  %d", pWaitMatchTeam->size(),pWaitMatchUser->size());
	
	std::vector<UINT32> pkMembers;

	UINT32 fullTeamA = 0;
	UINT32 fullTeamB = 0;

	std::map<UINT32, UINT32>::iterator teamIt = pWaitMatchTeam->begin();
	for (;teamIt != pWaitMatchTeam->end();++teamIt){
		CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(teamIt->first);

		if (pTeam && pTeam->getMemSize() >= CTeam::s_MAX_MEMBER){
			if (0 == fullTeamA){
				fullTeamA = pTeam->getID();
			}	
			else if (0 == fullTeamB){
				fullTeamB = pTeam->getID();
			}
			else {
				break;
			}
		}
	}
	

	if ((fullTeamA > 0) && (fullTeamB > 0)){	// 3 3 匹配 AB两队各3人
		CTeam * pTeamA = CTeamMgr::getSingleton().getTeamByID(fullTeamA);
		CTeam * pTeamB = CTeamMgr::getSingleton().getTeamByID(fullTeamB);
		if (!pTeamA)
		{
			pWaitMatchTeam->erase(fullTeamA);
			return false;
		}
		if (!pTeamB)
		{
			pWaitMatchTeam->erase(fullTeamB);
			return false;
		}


		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
			UINT32 charID = pTeamA->getCharID(i);
			if (charID){
				pkMembers.push_back(charID);
				CTeamMgr::getSingleton().removeCharID2Team(charID);
			}
		}
		(*backTeam)[fullTeamA] = (*pWaitMatchTeam)[fullTeamA];
		// if (CTeamMgr::getSingleton().removeTeam(pTeam)){
		// 	SAFE_DELETE(pTeam);
		// }

		pWaitMatchTeam->erase(fullTeamA);

		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
			UINT32 charID = pTeamB->getCharID(i);
			if (charID){
				pkMembers.push_back(charID);
				CTeamMgr::getSingleton().removeCharID2Team(charID);
			}
		}
		(*backTeam)[fullTeamB] = (*pWaitMatchTeam)[fullTeamB];
		pWaitMatchTeam->erase(fullTeamB);
	}
	else if ((fullTeamA > 0) && pWaitMatchTeam->size() >= 2 && pWaitMatchUser->size() >=1) // 3 2 1匹配
	{
		//A队3人，B队2人，加一个散人
		CTeam * pTeamA = CTeamMgr::getSingleton().getTeamByID(fullTeamA);
		if (!pTeamA)
		{
			pWaitMatchTeam->erase(fullTeamA);
			return false;
		}
		std::map<UINT32,UINT32>::iterator it =  pWaitMatchTeam->begin();
		UINT32 teamB = 0;
		for (;it != pWaitMatchTeam->end();it++){
			if (it->first != fullTeamA){
				teamB = it->first;
				break;
			}
		}
		CTeam * pTeamB = CTeamMgr::getSingleton().getTeamByID(teamB);
		if (!pTeamB)
		{
			pWaitMatchTeam->erase(teamB);
			return false;
		}


		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
			UINT32 charID = pTeamA->getCharID(i);
			if (charID){
				pkMembers.push_back(charID);
				CTeamMgr::getSingleton().removeCharID2Team(charID);
			}
		}
		(*backTeam)[fullTeamA] = (*pWaitMatchTeam)[fullTeamA];
		pWaitMatchTeam->erase(fullTeamA);

		//teamB 2人
		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
			UINT32 charID = pTeamB->getCharID(i);
			if (charID){
				pkMembers.push_back(charID);
				CTeamMgr::getSingleton().removeCharID2Team(charID);
			}
		}
		(*backTeam)[teamB] = (*pWaitMatchTeam)[teamB];
		pWaitMatchTeam->erase(teamB);

		//散人加一个
		it = pWaitMatchUser->begin();
		if (it != pWaitMatchUser->end()){
			pkMembers.push_back(it->first);
			(*backUser)[it->first] = it->second;
			pWaitMatchUser->erase(it);
		}
	}
	else if ((fullTeamA > 0) && (pWaitMatchUser->size() >= (UINT32)(s_MAX_PK_NUM / 2))){
		//只有一个队伍满员情况 3 1 1 1 匹配
		//填充队伍A的3个成员
		CTeam * pTeamA = CTeamMgr::getSingleton().getTeamByID(fullTeamA);
		if (!pTeamA)
		{
			pWaitMatchTeam->erase(fullTeamA);
			return false;
		}
	
		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
			UINT32 charID = pTeamA->getCharID(i);
			if (charID){
				pkMembers.push_back(charID);
				CTeamMgr::getSingleton().removeCharID2Team(charID);
			}
		}
		(*backTeam)[fullTeamA] = (*pWaitMatchTeam)[fullTeamA];
		pWaitMatchTeam->erase(fullTeamA);

		//填充3个散人
		std::map<UINT32, UINT32>::iterator it = pWaitMatchUser->begin();
		for (;it != pWaitMatchUser->end();){
			std::map<UINT32, UINT32>::iterator tmpIt = it;
			++it;
			pkMembers.push_back(tmpIt->first);
			(*backUser)[tmpIt->first] = tmpIt->second;
			pWaitMatchUser->erase(tmpIt);
			if (pkMembers.size() >= (UINT32)s_MAX_PK_NUM){
				break;
			}
		}
	}
	else if (pWaitMatchTeam->size() >= 2 && pWaitMatchUser->size() >= 2){//未满队伍够2队的情况
		//2,2,1,1匹配，2队2个人，各自带一个散人
		std::map<UINT32, UINT32>::iterator it =  pWaitMatchTeam->begin();
		UINT32 teamA = 0;
		UINT32 teamB = 0;
		if (it != pWaitMatchTeam->end()){//出现问题
			teamA = it->first;
			++it ;
			if (it != pWaitMatchTeam->end()){
				teamB = it->first;
			}
		}
		
		if (teamA == 0 || teamB == 0){
			return false;
		}

		CTeam * pTeamA = CTeamMgr::getSingleton().getTeamByID(teamA);
		CTeam * pTeamB = CTeamMgr::getSingleton().getTeamByID(teamB);
		if (!pTeamA)
		{
			pWaitMatchTeam->erase(teamA);
			return false;
		}
		if (!pTeamB)
		{
			pWaitMatchTeam->erase(teamB);
			return false;
		}

		//填充A队2个人
		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
			UINT32 charID = pTeamA->getCharID(i);
			if (charID){
				pkMembers.push_back(charID);
				CTeamMgr::getSingleton().removeCharID2Team(charID);
			}
		}
		(*backTeam)[teamA] = (*pWaitMatchTeam)[teamA];
		pWaitMatchTeam->erase(teamA);

		//填充一个散人给A队
		it = pWaitMatchUser->begin();
		if (it != pWaitMatchUser->end()){
			pkMembers.push_back(it->first);
			(*backUser)[it->first] = it->second;
			pWaitMatchUser->erase(it);
		}else{
			pkMembers.push_back(0);
		}

		//填充B队2个人
		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
			UINT32 charID = pTeamB->getCharID(i);
			if (charID){
				pkMembers.push_back(charID);
				CTeamMgr::getSingleton().removeCharID2Team(charID);
			}
		}
		(*backTeam)[teamB] = (*pWaitMatchTeam)[teamB];
		pWaitMatchTeam->erase(teamB);

		//填充一个散人给B队
		it = pWaitMatchUser->begin();
		if (it != pWaitMatchUser->end()){
			pkMembers.push_back(it->first);
			(*backUser)[it->first] = it->second;
			pWaitMatchUser->erase(it);
		}else{
			pkMembers.push_back(0);
		}
	}
	else if (pWaitMatchTeam->size() >= 1 && pWaitMatchUser->size() >= 4){//未满队伍1队情况
		//2,1,1,1,1匹配，A队2人＋一个散人  B队3个散人		
		std::map<UINT32, UINT32>::iterator it =  pWaitMatchTeam->begin();
		if (it == pWaitMatchTeam->end()){
			return false;
		}

		CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(it->first);
		if (!pTeam){
			pWaitMatchTeam->erase(it->first);
			(*backTeam)[it->first] = it->second;
			return false;
		}
		//填充A队2个人
		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
			UINT32 charID = pTeam->getCharID(i);
			if (charID){
				pkMembers.push_back(charID);
				CTeamMgr::getSingleton().removeCharID2Team(charID);
			}
		}
		(*backTeam)[it->first] = it->second;
		pWaitMatchTeam->erase(it);
		
		//填充4散人
		// it = pWaitMatchUser->begin();
		// for (;it != pWaitMatchUser->end();){
		// 	std::map<UINT32, UINT32>::iterator tmpIt = it;
		// 	++it;
		// 	pkMembers.push_back(tmpIt->first);
		// 	(*backUser)[tmpIt->first] = tmpIt->second;
		// 	pWaitMatchUser->erase(tmpIt);
		// 	if (pkMembers.size() >= (UINT32)s_MAX_PK_NUM){
		// 		break;
		// 	}
		// }
		for (UINT8 i = 0; i < CTeam::s_MAX_MEMBER*2-2; i++)
		{
			it = pWaitMatchUser->begin();
			if (it != pWaitMatchUser->end()){
				pkMembers.push_back(it->first);
				(*backUser)[it->first] = it->second;
				pWaitMatchUser->erase(it);
			}else{
				pkMembers.push_back(0);
			}
		}
	}
	else if (pWaitMatchUser->size() >= (UINT32)s_MAX_PK_NUM){//全部散人匹配
		std::map<UINT32, UINT32>::iterator it = pWaitMatchUser->begin();
		//
		for (;it != pWaitMatchUser->end();){
			std::map<UINT32, UINT32>::iterator tmpIt = it;
			++it;
			pkMembers.push_back(tmpIt->first);
			(*backUser)[tmpIt->first] = tmpIt->second;
			pWaitMatchUser->erase(tmpIt);
			if (pkMembers.size() >= (UINT32)s_MAX_PK_NUM){
				break;
			}
		}
	}
	else {
		// Athena::logger->trace("[实时战场]监测当前人数 %d! 队伍数 %d",pWaitMatchUser->size(),pWaitMatchTeam->size());
		if (!forceMatch)
		{
			return false;
		}
		
		//强制匹配, 从队伍到个人, 填满为止
		std::map<UINT32, UINT32>::iterator it =  pWaitMatchTeam->begin();
		if (it != pWaitMatchTeam->end()){
			CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(it->first);
			if (!pTeam){
				pWaitMatchTeam->erase(it->first);
				(*backTeam)[it->first] = it->second;
				return false;
			}
			//填充A队人
			for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
				UINT32 charID = pTeam->getCharID(i);
				if (charID){
					pkMembers.push_back(charID);
					CTeamMgr::getSingleton().removeCharID2Team(charID);
				}
			}
			(*backTeam)[it->first] = it->second;
			pWaitMatchTeam->erase(it);
		}
		
		//个人补齐, 不够就凑机器人(charid=0)
		for (UINT32 i = pkMembers.size(); i < (UINT32)s_MAX_PK_NUM; i++)
		{
			it = pWaitMatchUser->begin();
			if (it != pWaitMatchUser->end()){
				pkMembers.push_back(it->first);
				(*backUser)[it->first] = it->second;
				pWaitMatchUser->erase(it);
			}else{
				pkMembers.push_back(0);
			}
		}
		
	}
	
	Global::stRealTimeBattleEnterCopymap cmd;
	cmd.set_pktype(type);
	cmd.set_teamaid(1);
	cmd.set_teambid(2);
	
	for (UINT16 i = 0;i < pkMembers.size();++i){
		if (cmd.teamamembers_size() < s_MAX_PK_NUM / 2){
			cmd.add_teamamembers(pkMembers[i]);
			continue;
		}

		if (cmd.teambmembers_size() < s_MAX_PK_NUM / 2){
			cmd.add_teambmembers(pkMembers[i]);
			continue;
		}

		Athena::logger->error("匹配出来的玩家[%d] idx[%d]，却发现2只队伍都满了",pkMembers[i], i);
	}
	// g_globalServer->getConnMgr().broadcastByID(gameID,cmd);
	realTimePKEnterCopymap(&cmd);
	return true;
}

void realTimePKEnterCopymap(Global::stRealTimeBattleEnterCopymap * recvCmd)
{
	if (!recvCmd){
		return ;
	}
		
	stCopyMapBase * pCopyMapBase = NULL;
	
	if (recvCmd->pktype() == Global::eRealTimePKType_KillAll) {
		pCopyMapBase = g_copyMapBaseMgr.get(REALTIME_PVP_MAPID);
	}
	else {
		pCopyMapBase = g_copyMapBaseMgr.get(REALTIME_MOBA_MAPID);
	}

	if (!pCopyMapBase){
		Athena::logger->error("[实时战场]找不到关卡配置!");
		return ;
	}
	
	std::vector<GameUser *> teamAUserVec;
	std::vector<GameUser *> teamBUserVec;

	for (UINT8 i = 0;i < recvCmd->teamamembers_size();++i){
		if (recvCmd->teamamembers(i) == 0)
		{
			teamAUserVec.push_back(NULL);
		}else{
			GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->teamamembers(i));
			if (pUser){
				teamAUserVec.push_back(pUser);			
			}
			else {
				Athena::logger->error("[实时战场]A队伍有人已经下线了,进入失败!");
				return ;
			}
		}
	}
	
	for (UINT8 i = 0;i < recvCmd->teambmembers_size();++i){
		if (recvCmd->teambmembers(i) == 0)
		{
			teamBUserVec.push_back(NULL);	
		}
		else
		{
			GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->teambmembers(i));
			if (pUser){
				teamBUserVec.push_back(pUser);			
			}
			else {
				Athena::logger->error("[实时战场]B队伍有人已经下线了,进入失败!");
				return ;
			}
		}
	}
	
	if ((teamAUserVec.size() <= 0) || !teamAUserVec[0]){
		Athena::logger->error("[实时战场]A队伍的队长不在线了,进入失败!");
		return ;
	}

	CCopyMap * pCopyMap = CMapMgr::getSingleton().createCopyMap(teamAUserVec[0]->getUniqueName(),pCopyMapBase->m_id);
	if (!pCopyMap){
		Athena::logger->error("[实时战场]进入副本失败,创建副本不成功!");
		return;
	}
	pCopyMap->createCamp(Map::eCamp_Red);
	pCopyMap->createCamp(Map::eCamp_Black);

	for (UINT8 i = 0;i < teamAUserVec.size();++i){
		if (teamAUserVec[i]) {
			if (teamAUserVec[i]->m_countMgr.getDayRefreshValue(ECOUNT_REAL_TIME_NUM) >= teamAUserVec[i]->getCountByVip(eVipFun_BattleGround)){
				Athena::logger->error("玩家(name:%s,id:%d)次数已经用尽,不能进入战场",teamAUserVec[i]->getName(),teamAUserVec[i]->getID());
				continue;
			}

			if (!teamAUserVec[i]->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
				Athena::logger->error("玩家(name:%s,id:%d),历练不足进入实时战场失败",teamAUserVec[i]->getName(),teamAUserVec[i]->getID());
				continue;
			}
			teamAUserVec[i]->setCampID(Map::eCamp_Red);
			pCopyMap->joinCamp(Map::eCamp_Red,teamAUserVec[i]->getID());
			teamAUserVec[i]->resetBattleProp();
			CMapMgr::getSingleton().changeMap(teamAUserVec[i],pCopyMap,i + 1);
			//通知normal用户在跨服
			teamAUserVec[i]->NotifyNormalOnPub(true);

			Athena::logger->trace("玩家:%s进入了实时战场加入了:campID:%d",teamAUserVec[i]->getName(),Map::eCamp_Red);

		}else{
			stRobotInfo * pRobot = CRobotMgr::getSingleton().getRobotByLevelID(pCopyMapBase->m_id, 1);
			if (pRobot && pRobot->m_pUser) {
				CRobotMgr::getSingleton().useRobot(pRobot->m_pUser->getID(),pRobot);
				pRobot->m_pUser->setCampID(Map::eCamp_Red);
				pCopyMap->joinCamp(Map::eCamp_Red,pRobot->m_pUser->getID());
				pRobot->m_pUser->resetBattleProp();
				CMapMgr::getSingleton().robotChangeMap(pRobot->m_pUser,pCopyMap, i+1);
			}
			else {
				Athena::logger->trace("放机器人时候,没有机器人了!");
			}
		}
	}
	
	for (UINT8 i = 0;i < teamBUserVec.size();++i){
		if (teamBUserVec[i]) {
			if (teamBUserVec[i]->m_countMgr.getDayRefreshValue(ECOUNT_REAL_TIME_NUM) >= teamBUserVec[i]->getCountByVip(eVipFun_BattleGround)){
				Athena::logger->error("玩家(name:%s,id:%d)次数已经用尽,不能进入战场",teamBUserVec[i]->getName(),teamBUserVec[i]->getID());
				continue;
			}

			if (!teamBUserVec[i]->checkResource(eResource_Lilian,pCopyMapBase->m_costLiLian)){
				Athena::logger->error("玩家(name:%s,id:%d)历练不足进入实时战场失败!",teamBUserVec[i]->getName(),teamBUserVec[i]->getID());
				continue;
			}
            teamBUserVec[i]->m_resBackMgr.addBackRes(CResBackCfg::Three_Vs_Three, 1, teamBUserVec[i]->getVipLv());
			teamBUserVec[i]->setCampID(Map::eCamp_Black);
			pCopyMap->joinCamp(Map::eCamp_Black,teamBUserVec[i]->getID());
			teamBUserVec[i]->resetBattleProp();
			CMapMgr::getSingleton().changeMap(teamBUserVec[i],pCopyMap,i + 4);
			//通知normal用户在跨服
			teamBUserVec[i]->NotifyNormalOnPub(true);

			Athena::logger->trace("玩家:%s进入了实时战场加入了:campID:%d",teamBUserVec[i]->getName(),Map::eCamp_Black);

		}else{
			stRobotInfo * pRobot = CRobotMgr::getSingleton().getRobotByLevelID(pCopyMapBase->m_id, 2);
			if (pRobot && pRobot->m_pUser) {
				CRobotMgr::getSingleton().useRobot(pRobot->m_pUser->getID(),pRobot);
				pRobot->m_pUser->setCampID(Map::eCamp_Black);
				pCopyMap->joinCamp(Map::eCamp_Black,pRobot->m_pUser->getID());
				pRobot->m_pUser->resetBattleProp();
				CMapMgr::getSingleton().robotChangeMap(pRobot->m_pUser,pCopyMap, i+4);
			}
			else {
				Athena::logger->trace("放机器人时候,没有机器人了!");
			}
		}
	}
	pCopyMap->notifyExector();
}

void CTeamMatchMgr::OnTeamMemberLeave(UINT32 teamID, UINT32 oldLevel)
{
	for (int typeIdx = 0; typeIdx < s_MAX_GAME_TYPE; ++typeIdx)  
	{
		std::map<UINT16, stPvpWaitMatchInfo >::iterator it = m_waitMatchTeams[typeIdx].find(oldLevel);
		if (it != m_waitMatchTeams[typeIdx].end())
		{
			std::map<UINT32, UINT32>::iterator it1 = it->second.m_waitIDs.find(teamID);
			if (it1 != it->second.m_waitIDs.end())
			{
				//该类型，该队伍参与了匹配，则删除旧有匹配信息
				it->second.m_waitIDs.erase(teamID);

				//按新等级重新加入匹配队列
				CTeam *pTeam = CTeamMgr::getSingleton().getTeamByID(teamID);
				if (!pTeam)
				{
					return;
				}

				UINT32 memCnt = pTeam->getMemSize();
				if (memCnt <= 1)  //队伍只有一个人了,把该队员加入单人匹配中
				{
					for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
						UINT32 charID = pTeam->getCharID(i);
						if (charID){
							GameUser *pUser = GameUserManager::getSingleton().getUser(charID);
							if (pUser)
							{
								if (m_waitMatchUser[typeIdx].find(pUser->getLevel()) == m_waitMatchUser[typeIdx].end())
								{
									stPvpWaitMatchInfo	stInfo;
									stInfo.m_Level = pUser->getLevel();
									stInfo.m_waitIDs[pUser->getID()] = g_gameServer->getSec();
									m_waitMatchUser[typeIdx][pUser->getLevel()] = stInfo;
								}
								else
								{
									m_waitMatchUser[typeIdx][pUser->getLevel()].m_waitIDs[pUser->getID()] = g_gameServer->getSec();
								}
#ifdef _HDZ_DEBUG
								Athena::logger->trace("[实时战场]玩家[charID=%d][name=%s]队友离队，队伍只剩一人，转入单人匹配队列",pUser->getID(),pUser->getName());
#endif
							}
						}
					}
				}
				else
				{
					//队伍还有多人，加入到新等级的匹配
					UINT16 avgLevel = pTeam->getMemberAvgLevel();
					if (m_waitMatchTeams[typeIdx].find(avgLevel) == m_waitMatchTeams[typeIdx].end())
					{
						stPvpWaitMatchInfo	stInfo;
						stInfo.m_Level = avgLevel;
						stInfo.m_waitIDs[pTeam->getID()] = g_gameServer->getSec();
						m_waitMatchTeams[typeIdx][avgLevel] = stInfo;
					}
					else
					{
						m_waitMatchTeams[typeIdx][avgLevel].m_waitIDs[pTeam->getID()] = g_gameServer->getSec();
					}
#ifdef _HDZ_DEBUG
					Athena::logger->trace("[实时战场]队伍人员减少，由原等级[%d]转入新等级[%d]队列", oldLevel, avgLevel);
#endif
				}
			}
		}
	}
}
