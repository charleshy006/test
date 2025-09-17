#include "Team.h"
#include "PlayerManager.h"
#include "Log4cxxHelp.h"
#include "GlobalServer.h"
#include "Global.pb.h"
#include "PacketProcessor.h"
#include "TblDataMgr.h"
#include "TblCopyMapBase.h"
#include "Guild.h"
#include "ArenaConfig.h"

UINT32  CTeamMgr::s_teamIDGenID;

extern GlobalServer * g_globalServer;

void stTeamMember::fillSynInfo(TeamProto::stSynMemberInfo  & data)
{
	data.set_charid(m_charID);
	data.set_job(m_job);
	data.set_level(m_level);
	data.set_name(m_name);
	data.set_sex(m_sex);
	Player * pUser = PlayerManager::getSingleton().getUserByID(m_charID);

	if (pUser){
		data.set_battlepower(pUser->m_battlePower);
		data.set_headid(pUser->m_headID);
		data.set_bodyid(pUser->m_bodyID);
		data.set_cloackid(pUser->m_cloackID);
		data.set_shoeid(pUser->m_shoeID);
		data.set_weaponid(pUser->m_weaponID);
		data.set_wingid(pUser->m_wingID);
		data.set_fashionhair(pUser->m_fashionHairID);
		data.set_fashionface(pUser->m_fashionFaceID);
		data.set_fashionbody(pUser->m_fashionBodyID);
		data.set_fashiondecoration(pUser->m_fashionDecorationID);
		data.set_fashionshoe(pUser->m_fashionShoeID);
		data.set_fashionweapon(pUser->m_fashionWeaponID);
		data.set_effectid(pUser->m_effectID);
		data.set_wingbreaklv(pUser->m_wingBreakLv);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("组队发送了玩家:charID=%d,wingID:%d,shoeID:%d,fashionShoe:%d",pUser->getID(),pUser->m_wingID,pUser->m_shoeID,pUser->m_fashionShoeID);
#endif
	}
}

void stTeamMember::fillSynInfo(Global::stSynMemberInfo  & data)
{
	data.set_charid(m_charID);
	data.set_job(m_job);
	data.set_level(m_level);
	data.set_name(m_name);
	data.set_sex(m_sex);
	Player * pUser = PlayerManager::getSingleton().getUserByID(m_charID);

	if (pUser){
		data.set_battlepower(pUser->m_battlePower);
		data.set_headid(pUser->m_headID);
		data.set_bodyid(pUser->m_bodyID);
		data.set_cloackid(pUser->m_cloackID);
		data.set_shoeid(pUser->m_shoeID);
		data.set_weaponid(pUser->m_weaponID);
		data.set_wingid(pUser->m_wingID);
		data.set_fashionhair(pUser->m_fashionHairID);
		data.set_fashionface(pUser->m_fashionFaceID);
		data.set_fashionbody(pUser->m_fashionBodyID);
		data.set_fashiondecoration(pUser->m_fashionDecorationID);
		data.set_fashionshoe(pUser->m_fashionShoeID);
		data.set_fashionweapon(pUser->m_fashionWeaponID);
		data.set_effectid(pUser->m_effectID);
		data.set_wingbreaklv(pUser->m_wingBreakLv);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("组队发送了玩家:charID=%d,wingID:%d,shoeID:%d,fashionShoe:%d",pUser->getID(),pUser->m_wingID,pUser->m_shoeID,pUser->m_fashionShoeID);
#endif
	}
}

CTeam::CTeam()
{
	m_id = 0;
	m_copyMapID = 0;
	bzero(m_members,sizeof(m_members));
	m_hasEnterCopyMap = false;
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

bool CTeam::initTeam(Player *pOwner,UINT32 copyMapID)
{
	if (!pOwner || !getID()){
		return false;
	}
	m_copyMapID = copyMapID;
	m_members[0].m_charID = pOwner->getID();
	m_members[0].m_job = pOwner->m_job;
	m_members[0].m_level = pOwner->m_level;
	m_members[0].m_sex = pOwner->m_sex;
	strncpy(m_members[0].m_name,pOwner->getName(),sizeof(m_members[0].m_name));
	return true;
}

void CTeam::sendTeamInfo(Player * pUser)
{
	if (!pUser){
		return ;
	}

    /*
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
	pUser->sendProtoToMe(retCmd);
    */
	Global::stRetRoomInfo retCmd;
	retCmd.set_charid(pUser->getID());
	retCmd.set_copymapid(m_copyMapID);
	retCmd.set_teamid(getID());
	for (UINT8 i = 0;i < s_MAX_MEMBER;++i){
		if (m_members[i].m_charID){
			Global::stSynMemberInfo * pSyn = retCmd.add_members();
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
	pUser->sendProto2Game(retCmd);
}

void CTeam::userOnline(Player * pUser)
{
	
}

void CTeam::userOffline(Player *pUser)
{
	if (!pUser){
		return ;
	}
	leaveTeam(pUser->getID());
}


bool CTeam::joinInTeam(Player * pUser,UINT8 pos)
{
	if (!pUser){
		return false;
	}
	
	if (CTeamMgr::getSingleton().getTeamByCharID(pUser->getID())){
		return false;
	}

	for (UINT8 i = 0; i< s_MAX_MEMBER;++i){
		if (m_members[i].m_charID == pUser->getID()){
			return false;
		}
	}
	
	TeamProto::stEnterTeamResult ret;

	if (m_hasEnterCopyMap) {
		ret.set_result(TeamProto::stEnterTeamResult::eTeam_Leader_Busy);
		pUser->sendProtoToMe(ret);
		return false;
	}

	if (getMemSize() >= s_MAX_MEMBER){
		ret.set_result(TeamProto::stEnterTeamResult::eTeam_Full);
		pUser->sendProtoToMe(ret);
		return false;
	}
	
	UINT8 index = (UINT8)-1;
	if (pos < s_MAX_MEMBER){
		index = pos;	
	}
	else {
		for (UINT8 i = 0; i< s_MAX_MEMBER;++i){
			if (!m_members[i].m_charID){
				index = i;
				break;
			}	
		}
	}

	if (index >= s_MAX_MEMBER){
		ret.set_result(TeamProto::stEnterTeamResult::eTeam_Full);
		pUser->sendProtoToMe(ret);
		return false;
	} 
	
	if (m_members[index].m_charID){
		ret.set_result(TeamProto::stEnterTeamResult::ePos_Has_Member);
		pUser->sendProtoToMe(ret);
		return false;
	}

	m_members[index].m_charID = pUser->getID();
	m_members[index].m_job = pUser->m_job;
	m_members[index].m_level = pUser->m_level;
	m_members[index].m_sex = pUser->m_sex;
	
	strncpy(m_members[index].m_name,pUser->getName(),sizeof(m_members[index].m_name));
	sendTeamInfo(pUser);
    /*
	TeamProto::stAddTeamMember cmd;
	cmd.set_pos(index);	
	m_members[index].fillSynInfo(*cmd.mutable_memberinfo());
	cmd.mutable_memberinfo()->set_pos(index);
	if (index == 0){
		cmd.mutable_memberinfo()->set_leader(true);
	}
	else {
		cmd.mutable_memberinfo()->set_leader(false);
	}
	sendProto2All(cmd,pUser->getID());
    */
    Global::stRetAddTeamMember cmd;
	for (UINT8 i = 0; i< s_MAX_MEMBER;++i){
		if (m_members[i].m_charID){
            cmd.add_charid(m_members[i].m_charID);
        }
    }
    cmd.set_exceptid(pUser->getID());
	cmd.set_pos(index);	
	m_members[index].fillSynInfo(*cmd.mutable_memberinfo());
	cmd.mutable_memberinfo()->set_pos(index);
	if (index == 0){
		cmd.mutable_memberinfo()->set_leader(true);
	}
	else {
		cmd.mutable_memberinfo()->set_leader(false);
	}
	pUser->sendProto2Game(cmd);

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
	
	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (pUser){
		CTeamMgr::getSingleton().cancelMatch(pUser,getCopyMapID(),false);
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
	m_inviteList[charID] = g_globalServer->getSec();
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

void CTeam::enterCopyMap()
{
	if (m_hasEnterCopyMap){
		return ;
	}
	
	Player * pLeader = PlayerManager::getSingleton().getUserByID(getLeaderID());
	if (!pLeader){
		return ;
	}

	Global::stTeamEnterCopyMap cmd;
	cmd.set_copymapid(m_copyMapID);
	cmd.set_teamid(getID());
	for (UINT8 i = 0;i < s_MAX_MEMBER; ++i){
		Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
		if (pMemInfo){
			pMemInfo->set_charid(m_members[i].m_charID);
		}
	}
	g_globalServer->getConnMgr().broadcastByID(pLeader->getGameID(),cmd);
	m_hasEnterCopyMap = true;
}

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

void CTeamMgr::userOnline(Player * pUser)
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

void CTeamMgr::userOffline(Player * pUser)
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
	pTeam->userOffline(pUser);
	/*if (!pTeam->getLeaderID()){//队长离开了房间要解散
			
	}*/

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
	/*id = m_teamIDGen.genID();
	if (m_teamIDGen.isValidID(id)){
		return true;
	}
	else {
		return false;
	}*/
	return true;
}

void CTeamMgr::returnTeamID(UINT32 id)
{
	//m_teamIDGen.putID(id);	
}

void CTeamMgr::createTeam(Player * pUser,UINT32 copyMapID)
{
	if (!pUser){
		return ;
	}

	TeamProto::stCreateTeamResult ret;
	if (getTeamByCharID(pUser->getID())){
		ret.set_result(TeamProto::stCreateTeamResult::eHAS_JOIN_IN_TEAM);
		pUser->sendProtoToMe(ret);
#ifdef _HDZ_DEBUG
		Athena::logger->error("已经加入队伍了,不能再创建队伍!");
#endif
		return;
	}

	CTeam * pTeam   = ATHENA_NEW CTeam();
	if (!pTeam){
		ret.set_result(TeamProto::stCreateTeamResult::eINNER_ERROR);
		pUser->sendProtoToMe(ret);
		return;
	}

	UINT32 teamID = 0;
	if (!genTeamID(teamID)){
		ret.set_result(TeamProto::stCreateTeamResult::eHAS_JOIN_IN_TEAM);
		pUser->sendProtoToMe(ret);
#ifdef _HDZ_DEBUG
		Athena::logger->error("创建房间不成功了,已经超过创建的房间上限了");
#endif
		return;
	}

	pTeam->setID(teamID);			
	if (!pTeam->initTeam(pUser,copyMapID)){
		ret.set_result(TeamProto::stCreateTeamResult::eHAS_JOIN_IN_TEAM);
		pUser->sendProtoToMe(ret);
		return;
	}
	addCharID2Team(pUser->getID(),pTeam->getID());
	ret.set_result(TeamProto::stCreateTeamResult::eSUCCESS);
	pUser->sendProtoToMe(ret);
	pTeam->sendTeamInfo(pUser);
	m_allTeam[pTeam->getID()] = pTeam;
}

void CTeamMgr::enterTeam(Player * pUser,UINT32 teamID)
{
	CTeam * pTeam = getTeamByID(teamID);
	TeamProto::stEnterTeamResult cmd;
	if (!pTeam){
		cmd.set_result(TeamProto::stEnterTeamResult::eTeam_Not_Exist);
		pUser->sendProtoToMe(cmd);
		return;
	}
	
	std::map<UINT32, stWaitMatchInfo >::iterator waitMatchIt =  m_waitMatchTeamQueue.find(pTeam->getCopyMapID());
	if (waitMatchIt != m_waitMatchTeamQueue.end()){
		if (waitMatchIt->second.m_waitTeamIDs.find(pTeam->getID()) != waitMatchIt->second.m_waitTeamIDs.end()){
			cmd.set_result(TeamProto::stEnterTeamResult::eTeam_Is_Auto_Match);
			pUser->sendProtoToMe(cmd);
			return;
		}
	}

	stCopyMapBase * pCopyMapBase =  g_copyMapBaseMgr.get(pTeam->getCopyMapID());
	if (pCopyMapBase){
		if (eLevelType_Guild_Copymap == pCopyMapBase->m_type){
			CGuild * pGuild1 = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
			CGuild * pGuild2 = CGuildMgr::getSingleton().getGuildByCharID(pTeam->getLeaderID());
			if (pGuild1 != pGuild2){
				cmd.set_result(TeamProto::stEnterTeamResult::eTeam_Not_Exist);
				pUser->sendProtoToMe(cmd);
				return;
			}
		}
	}

	if (!pTeam->joinInTeam(pUser,(UINT8)-1)){
		cmd.set_result(TeamProto::stEnterTeamResult::eHas_In_Team);
		pUser->sendProtoToMe(cmd);
		return;	
	}
	pTeam->sendTeamInfo(pUser);
}

void CTeamMgr::leaveTeam(Player * pUser)
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
		//pTeam->chgLeader();
	}

	if (!pTeam->getMemSize()) {//没有人了
		if (removeTeam(pTeam)){
			SAFE_DELETE(pTeam);
			return;
		}
	}
}

void CTeamMgr::fireTeam(Player * pUser,bool bNotify)
{
	if (!pUser){
		return ;
	}

	CTeam * pTeam = getTeamByCharID(pUser->getID());
	if (!pTeam){
		return;
	}

	if (pUser->getID() != pTeam->getLeaderID()){//不是队长不可以解散
		return;
	}

	if (removeTeam(pTeam)){
		for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER;++i){
			CTeamMgr::getSingleton().removeCharID2Team(pTeam->getCharID(i));
		}
		if (bNotify){
			TeamProto::stNotifyFireTeam cmd;
			pTeam->sendProto2All(cmd);
		}
		SAFE_DELETE(pTeam);
		return;
	}
}

void CTeamMgr::inviteJoinIn(Player * pUser,UINT32 charID,UINT8 pos,TeamProto::eTeamType eType)
{
	if (!pUser){
		return ;
	}

	CTeam * pTeam = getTeamByCharID(pUser->getID());
	if (!pTeam){
		return;
	}
	
	TeamProto::stInviteJoinInTeamResult ret;
	if (pTeam->getLeaderID() != pUser->getID()){
		ret.set_result(TeamProto::stInviteJoinInTeamResult::NOT_LEADER);
		pUser->sendProtoToMe(ret);
		return;
	}

	Player * pOther = PlayerManager::getSingleton().getUserByID(charID);
	if (!pOther){
		ret.set_result(TeamProto::stInviteJoinInTeamResult::USER_OFFLINE);
		pUser->sendProtoToMe(ret);
		return;
	}
	
	TeamProto::stAskJoinInTeam askCmd;
	askCmd.set_pos(pos);
	askCmd.set_invitename(pUser->getName());
	askCmd.set_teamid(pTeam->getID());
	askCmd.set_etype(eType);
	askCmd.set_copymapid(pTeam->getCopyMapID());
	pOther->sendProtoToMe(askCmd);
	pTeam->addInviteInfo(pOther->getID());
}

void CTeamMgr::replyInvite(Player * pUser,UINT32 teamID,bool agree,UINT8 pos,TeamProto::eTeamType eType,bool timeUseOut)
{
	if (!pUser){
		return ;
	}	
	TeamProto::stInviteJoinInTeamResult ret;
	ret.set_etype(eType);
	CTeam * pTeam = getTeamByID(teamID);
	if (!pTeam){
		ret.set_result(TeamProto::stInviteJoinInTeamResult::TEAM_NOT_EXSIT);
		pUser->sendProtoToMe(ret);
		return;
	}
	
	std::map<UINT32, stWaitMatchInfo >::iterator waitMatchIt =  m_waitMatchTeamQueue.find(pTeam->getCopyMapID());

	if (waitMatchIt != m_waitMatchTeamQueue.end()){
		if (waitMatchIt->second.m_waitTeamIDs.find(pTeam->getID()) != waitMatchIt->second.m_waitTeamIDs.end()){
			ret.set_result(TeamProto::stInviteJoinInTeamResult::TEAM_IS_AUTO_MATCH);
			pUser->sendProtoToMe(ret);
			return;
		}
	}

	if (!pTeam->isInInviteList(pUser->getID())){
		Athena::logger->error("name=%s,account=%s,id=%d答复进入队伍邀请时候不在邀请列表",pUser->getAccount().c_str(),pUser->getName(),pUser->getID());
		ret.set_result(TeamProto::stInviteJoinInTeamResult::NOT_IN_APPLY_LIST);
		pUser->sendProtoToMe(ret);
		return;
	}
	pTeam->eraseCharIDFromInviteList(pUser->getID());	
	if (!agree){
		if (timeUseOut){
			ret.set_result(TeamProto::stInviteJoinInTeamResult::COPYMAP_NUM_USE_OUT);
			pUser->sendProtoToMe(ret);
		}
		else {
			ret.set_result(TeamProto::stInviteJoinInTeamResult::DISAGREE);
		}
	}
	else {
		if (pTeam->joinInTeam(pUser,pos)){
			pTeam->sendTeamInfo(pUser);
		}
		else {
			ret.set_result(TeamProto::stInviteJoinInTeamResult::OTHER);
			pUser->sendProtoToMe(ret);
		}
	}
}

void CTeamMgr::enterLv(Player * pUser)
{
	if (!pUser){
		return ;
	}

	CTeam * pTeam = getTeamByCharID(pUser->getID());
	if (!pTeam){
		return;
	}

	if (pTeam->getLeaderID() != pUser->getID()){
		Athena::logger->error("account=%s,name=%s,charID=%d,进入请求进入副本不成功,不是队长!",pUser->getAccount().c_str(),pUser->getName(),pUser->getID());
		return;
	}
	pTeam->enterCopyMap();
	fireTeam(pUser,false);
}

void CTeamMgr::timer(UINT32 cur)
{
	std::map<UINT32, stWaitMatchInfo >::iterator it =  m_waitMatchQueue.begin();	

	for (;it != m_waitMatchQueue.end();++it){
		stWaitMatchInfo & info = it->second;
		if (cur > info.m_beginWaitTime + s_MATCH_MAX_WAIT_TIME){
			if (info.m_waitIDs.size() <= 0){
				continue;
			}
			info.m_beginWaitTime = g_globalServer->getSec();
			Global::stTeamEnterCopyMap cmd;
			cmd.set_copymapid(it->first);
			cmd.set_teamid(0);
			std::set<UINT32>::iterator subIt = it->second.m_waitIDs.begin();
			for (;subIt != it->second.m_waitIDs.end();){
				std::set<UINT32>::iterator tmpSubIt = subIt;
				++subIt;
				Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
				if (pMemInfo){
					pMemInfo->set_charid(*tmpSubIt);
				}
				it->second.m_waitIDs.erase(tmpSubIt);
				
				if (cmd.memerinfo_size() >= CTeam::s_MAX_MEMBER){
					break;
				}
			}
			g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
		}
	}

	it = m_waitMatchTeamQueue.begin();
	for (;it != m_waitMatchTeamQueue.end();++it){
		std::map<UINT32,UINT32>::iterator subIt = it->second.m_waitTeamIDs.begin();

		for (;subIt != it->second.m_waitTeamIDs.end();){
			std::map<UINT32,UINT32>::iterator tmpSubIt = subIt;
			++subIt;
			if (tmpSubIt->second + s_MATCH_MAX_WAIT_TIME < g_globalServer->getSec()){
				CTeam * pWaitTeam = getTeamByID(tmpSubIt->first);
				if (pWaitTeam){
					Global::stTeamEnterCopyMap cmd;
					cmd.set_copymapid(it->first);
					cmd.set_teamid(0);

					UINT32 memCharID = pWaitTeam->getCharID(0);
					if (memCharID > 0){
						Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
						if (pMemInfo){
							pMemInfo->set_charid(memCharID);
						}
					}

					memCharID = pWaitTeam->getCharID(1);
					if (memCharID > 0){
						Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
						if (pMemInfo){
							pMemInfo->set_charid(memCharID);
						}
					}

					memCharID = pWaitTeam->getCharID(2);
					if (memCharID > 0){
						Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
						if (pMemInfo){
							pMemInfo->set_charid(memCharID);
						}
					}

					if (CTeamMgr::getSingleton().removeTeam(pWaitTeam)){
						SAFE_DELETE(pWaitTeam);
					}
					g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
					it->second.m_waitTeamIDs.erase(tmpSubIt);
				}
			}
		}
	}
}

void CTeamMgr::autoMatch(Player * pUser,UINT32 copyMapID)
{
	if (!pUser || copyMapID <= 0){
		return ;
	}

	std::map<UINT32, stWaitMatchInfo>::iterator it =  m_waitMatchQueue.begin();
	for (;it != m_waitMatchQueue.end();++it){
		if (it->second.m_waitIDs.find(pUser->getID()) != it->second.m_waitIDs.end()){
			Athena::logger->error("请求自动请求匹配错误,已经组队错误!");
			return ;
		}
	}

	CTeam * pMyTeam = getTeamByCharID(pUser->getID());	
	
	if (pMyTeam){
		it = m_waitMatchTeamQueue.begin();
		for (;it != m_waitMatchTeamQueue.end();++it){
			if (it->second.m_waitTeamIDs.find(pMyTeam->getID()) != it->second.m_waitTeamIDs.end()){
				Athena::logger->error("请求自动请求匹配错误,已经组队错误!");
				return ;	
			}	
		}
	}
	
	stWaitMatchInfo & waitMatchQueue = m_waitMatchQueue[copyMapID];
	stWaitMatchInfo & waitMatchTeamQueue = m_waitMatchTeamQueue[copyMapID];

	/*if (!waitMatchQueue || !waitMatchTeamQueue){
		return ;
	}*/
	
	if (pMyTeam && (pMyTeam->getMemSize() == 1)){//1个人一个队伍情况
		if (waitMatchQueue.m_waitIDs.size() <= 0){
			waitMatchQueue.m_beginWaitTime = g_globalServer->getSec();
		}
		waitMatchQueue.m_waitIDs.insert(pUser->getID());	
		if (CTeamMgr::getSingleton().removeTeam(pMyTeam)){
			SAFE_DELETE(pMyTeam);
		}
	}
	else if (pMyTeam && (pMyTeam->getMemSize() == 2)){//2个人一个队伍情况
		//waitMatchQueue.m_beginWaitTime = g_globalServer->getSec();
		waitMatchTeamQueue.m_waitTeamIDs.insert(std::make_pair(pMyTeam->getID(),g_globalServer->getSec()));	
	}
	else if (pMyTeam){//三个人一个队伍不应该发送匹配协议
		Athena::logger->error("三个人一个队伍不应该发送匹配协议");
		return ;
	}
	else {
		if (waitMatchQueue.m_waitIDs.size() <= 0){
			waitMatchQueue.m_beginWaitTime = g_globalServer->getSec();
		}
		waitMatchQueue.m_waitIDs.insert(pUser->getID());
	}

	if (waitMatchQueue.m_waitIDs.size() < CTeam::s_MAX_MEMBER){//人数不足一个队伍的情况
		TeamProto::stWaitMatchTeam waitCmd;
		if (waitMatchTeamQueue.m_waitIDs.size() <= 0){
			pUser->sendProtoToMe(waitCmd);
		}

		if ((waitMatchQueue.m_waitIDs.size() > 0) && (waitMatchTeamQueue.m_waitTeamIDs.size() > 0)){
			UINT32 teamID = waitMatchTeamQueue.m_waitTeamIDs.begin()->first; 
			CTeam * pWaitTeam = getTeamByID(teamID);
			if (pWaitTeam){
				waitMatchTeamQueue.m_waitTeamIDs.erase(teamID);
				Global::stTeamEnterCopyMap cmd;
				cmd.set_copymapid(copyMapID);
				cmd.set_teamid(0);
				std::set<UINT32>::iterator subIt = waitMatchQueue.m_waitIDs.begin();
				if (subIt != waitMatchQueue.m_waitIDs.end()){
					Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
					if (pMemInfo){
						pMemInfo->set_charid(*subIt);
						waitMatchQueue.m_waitIDs.erase(subIt);
					}
				}
				
				UINT32 memCharID = pWaitTeam->getCharID(0);
				if (memCharID > 0){
					Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
					if (pMemInfo){
						pMemInfo->set_charid(memCharID);
					}
				}
				
				memCharID = pWaitTeam->getCharID(1);
				if (memCharID > 0){
					Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
					if (pMemInfo){
						pMemInfo->set_charid(memCharID);
					}
				}
				
				memCharID = pWaitTeam->getCharID(2);
				if (memCharID > 0){
					Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
					if (pMemInfo){
						pMemInfo->set_charid(memCharID);
					}
				}

				if (CTeamMgr::getSingleton().removeTeam(pWaitTeam)){
					SAFE_DELETE(pWaitTeam);
				}
				
				g_globalServer->getConnMgr().broadcastByID(pUser->getGameID(),cmd);
			}
		}
		else if (pMyTeam){
			pMyTeam->sendProto2All(waitCmd);
		}
		else {
			pUser->sendProtoToMe(waitCmd);
		}
	}
	else {
		TeamProto::stMathcTeamSuccess retCmd;
		pUser->sendProtoToMe(retCmd);
		Global::stTeamEnterCopyMap cmd;
		cmd.set_copymapid(copyMapID);
		cmd.set_teamid(0);
		std::set<UINT32>::iterator subIt = waitMatchQueue.m_waitIDs.begin();
		for (;subIt != waitMatchQueue.m_waitIDs.end();){
			std::set<UINT32>::iterator tmpSubIt = subIt;
			++subIt;
			Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
			if (pMemInfo){
				pMemInfo->set_charid(*tmpSubIt);
			}
			waitMatchQueue.m_waitIDs.erase(tmpSubIt);

			if (cmd.memerinfo_size() >= CTeam::s_MAX_MEMBER)
			{
				break;
			}
		}
		g_globalServer->getConnMgr().broadcastByID(pUser->getGameID(),cmd);
	}
}

void CTeamMgr::cancelMatch(Player * pUser,UINT32 copyMapID,bool bNotify)
{
	if (!pUser){
		return ;
	}
	
	if (0 == copyMapID){
		Athena::logger->error("玩家(name:%s,id:%d)取消组队副本时候出现错误,副本ID为0",pUser->getName(),pUser->getID());
	}

	std::map<UINT32, stWaitMatchInfo >::iterator it =  m_waitMatchQueue.begin();
	for (;it != m_waitMatchQueue.end();++it){
		if (it->first  == copyMapID){
			it->second.m_waitIDs.erase(pUser->getID());
		}
	}

	CTeam * pMyTeam = getTeamByCharID(pUser->getID());	
	TeamProto::stCancelAutoMatchTeamResult retCmd;
	retCmd.set_result(TeamProto::stCancelAutoMatchTeamResult::eSUCCESS);	

	if (pMyTeam){
		it =  m_waitMatchTeamQueue.begin();
		for (;it != m_waitMatchTeamQueue.end();++it){
			if (it->first == copyMapID){
				it->second.m_waitTeamIDs.erase(pMyTeam->getID());
			}
		}
		if (bNotify){
			pMyTeam->sendProto2All(retCmd);
		}
	}
	else {
		if (bNotify){
			pUser->sendProtoToMe(retCmd);
		}
	}
}

CTeamMatchMgr::CTeamMatchMgr()
{
	
}

CTeamMatchMgr::~CTeamMatchMgr()
{
	
}

void CTeamMatchMgr::addMatchTeam(Player *pUser,UINT32 teamID,Global::eRealTimePKType type)
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
		pUser->sendProtoToMe(failCmd);
		return ;
	}

	std::set<UINT32> * pWaitMatchTeam = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchTeam = &m_typeAWaitMatchTeams; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchTeam = &m_typeBWaitMatchTeams;
	}
	
	std::set<UINT32> * pWaitMatchUser = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchUser = &m_typeAWaitMatchUser; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchUser = &m_typeBWaitMatchUser;
	}

	if (!pWaitMatchTeam || !pWaitMatchUser){
		return ;
	}

	pWaitMatchTeam->insert(teamID);
	
	if (checkCanEnterCopyMap(pUser->getGameID(),type)){
		
	}
	else {
		TeamProto::stRealTimeTeamMatchWait retCmd;
		if (type == Global::eRealTimePKType_KillAll){
			retCmd.set_etype(TeamProto::eRealTimePKType_KillAll);
		}
		else if (type == Global::eRealTimePKType_Finish){
			retCmd.set_etype(TeamProto::eRealTimePKType_Finish);
		}
		
		CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(teamID);
		if (pTeam){
			pTeam->sendProto2All(retCmd);
		} 

	}
	
	return ;
}

bool CTeamMatchMgr::removeMatchTeam(UINT32 teamID,Global::eRealTimePKType type)
{
	std::set<UINT32> * pWaitMatchTeam = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchTeam = &m_typeAWaitMatchTeams; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchTeam = &m_typeBWaitMatchTeams;
	}
	
	if (!pWaitMatchTeam){
		return false;
	}
	pWaitMatchTeam->erase(teamID);
	
	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(teamID);
	if (pTeam){
		TeamProto::stCancelRealTimeTeamMatchResult retCmd;
		retCmd.set_success(true);
		pTeam->sendProto2All(retCmd);
	}
	return true;
}

bool CTeamMatchMgr::isOpen(Global::eRealTimePKType type)
{
	struct tm _tm; 
	RealTime::getLocalTime(_tm,g_globalServer->getSec());
	
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;

//	if(Athena::global["CountryVersion"] == "Korea"){
		//星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
		UINT16 wday = _tm.tm_wday;
		secNum = secNum + wday * 86400 ;
//	}
	if (type == Global::eRealTimePKType_KillAll){
		return ArenaConfig::getSingleton().isRealTimeOpen(secNum);
	}
	else if (type == Global::eRealTimePKType_Finish){
		return ArenaConfig::getSingleton().isMobaOpen(secNum);
	}
	return false;
}

void CTeamMatchMgr::addMatchUser(Player * pUser,Global::eRealTimePKType type)
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
		pUser->sendProtoToMe(failCmd);
		return ;
	}

	std::set<UINT32> * pWaitMatchUser = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchUser = &m_typeAWaitMatchUser; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchUser = &m_typeBWaitMatchUser;
	}
	
	std::set<UINT32> * pWaitMatchTeam = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchTeam = &m_typeAWaitMatchTeams; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchTeam = &m_typeBWaitMatchTeams;
	}

	if (!pWaitMatchUser || !pWaitMatchTeam){
		return ;
	}

	pWaitMatchUser->insert(pUser->getID());

	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByCharID(pUser->getID());
	if (pTeam){
		CTeamMgr::getSingleton().removeCharID2Team(pUser->getID());
		if (CTeamMgr::getSingleton().removeTeam(pTeam)){
			SAFE_DELETE(pTeam);
		} 
	}

	if (checkCanEnterCopyMap(pUser->getGameID(),type)){
		
	}
	else {
		TeamProto::stRealTimeSingleMatchWait retCmd;
		if (type == Global::eRealTimePKType_KillAll){
			retCmd.set_etype(TeamProto::eRealTimePKType_KillAll);
		}
		else if (type == Global::eRealTimePKType_Finish){
			retCmd.set_etype(TeamProto::eRealTimePKType_Finish);
		}
		pUser->sendProtoToMe(retCmd);
	}
	return ;	
}

void CTeamMatchMgr::removeMatchUser(Player * pUser,Global::eRealTimePKType type)
{
	if (!pUser){
		return ;
	}
	std::set<UINT32> * pWaitMatchUserVec = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchUserVec = &m_typeAWaitMatchUser;
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchUserVec = &m_typeBWaitMatchUser;
	}

	if (!pWaitMatchUserVec){
		return ;
	}
	pWaitMatchUserVec->erase(pUser->getID());
}

void CTeamMatchMgr::userOffline(Player * pUser)
{
	if (!pUser){
		return ;
	}
	m_typeAWaitMatchUser.erase(pUser->getID());
	m_typeBWaitMatchUser.erase(pUser->getID());
}

bool CTeamMatchMgr::checkCanEnterCopyMap(UINT16 gameID,Global::eRealTimePKType type)
{
	std::set<UINT32> * pWaitMatchUser = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchUser = &m_typeAWaitMatchUser; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchUser = &m_typeBWaitMatchUser;
	}
	
	std::set<UINT32> * pWaitMatchTeam = NULL;
	if (type == Global::eRealTimePKType_KillAll){
		pWaitMatchTeam = &m_typeAWaitMatchTeams; 
	}
	else if (type == Global::eRealTimePKType_Finish){
		pWaitMatchTeam = &m_typeBWaitMatchTeams;
	}

	if (!pWaitMatchUser || !pWaitMatchTeam){
		return false;
	}
	
	std::vector<UINT32> pkMembers;

	UINT32 fullTeamA = 0;
	UINT32 fullTeamB = 0;

	std::set<UINT32>::iterator teamIt = pWaitMatchTeam->begin();
	for (;teamIt != pWaitMatchTeam->end();++teamIt){
		CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(*teamIt);

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
	

	if (pWaitMatchUser->size() >= (UINT32)s_MAX_PK_NUM){//可以匹配
		std::set<UINT32>::iterator it = pWaitMatchUser->begin();
		//填充队伍A的成员
		for (;it != pWaitMatchUser->end();){
			std::set<UINT32>::iterator tmpIt = it;
			++it;
			pkMembers.push_back(*tmpIt);
			pWaitMatchUser->erase(tmpIt);
			if (pkMembers.size() >= (UINT32)s_MAX_PK_NUM){
				break;
			}
		}
	}
	else if ((fullTeamA > 0) && (fullTeamB > 0)){
		CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(fullTeamA);
		if (pTeam){
			for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
				UINT32 charID = pTeam->getCharID(i);
				if (charID){
					pkMembers.push_back(charID);
					CTeamMgr::getSingleton().removeCharID2Team(charID);
				}
			}
			if (CTeamMgr::getSingleton().removeTeam(pTeam)){
				SAFE_DELETE(pTeam);
			}
		}
		
		pWaitMatchTeam->erase(fullTeamA);

		pTeam = CTeamMgr::getSingleton().getTeamByID(fullTeamB);
		if (pTeam){
			for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
				UINT32 charID = pTeam->getCharID(i);
				if (charID){
					pkMembers.push_back(charID);
					CTeamMgr::getSingleton().removeCharID2Team(charID);
				}
			}

			if (CTeamMgr::getSingleton().removeTeam(pTeam)){
				SAFE_DELETE(pTeam);
			}
		}
		pWaitMatchTeam->erase(fullTeamB);
	}
	else if ((fullTeamA > 0) && (pWaitMatchUser->size() >= (UINT32)(s_MAX_PK_NUM / 2))){//只有一个队伍满员情况
		std::set<UINT32>::iterator it = pWaitMatchUser->begin();
		//填充队伍A的成员
		for (;it != pWaitMatchUser->end();){
			std::set<UINT32>::iterator tmpIt = it;
			++it;
			pkMembers.push_back(*tmpIt);
			pWaitMatchUser->erase(tmpIt);
			if (pkMembers.size() >= (UINT32)s_MAX_PK_NUM){
				break;
			}
		}
		
		CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(fullTeamA);
		if (pTeam){
			for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
				UINT32 charID = pTeam->getCharID(i);
				if (charID){
					pkMembers.push_back(charID);
					CTeamMgr::getSingleton().removeCharID2Team(charID);
				}
			}

			if (CTeamMgr::getSingleton().removeTeam(pTeam)){
				SAFE_DELETE(pTeam);
			}
		}
		pWaitMatchTeam->erase(fullTeamA);

	}
	else if (pWaitMatchTeam->size() >= 2){//未满队伍不足2队的情况
		if (pWaitMatchUser->size() < 2){
			return false;
		}
		std::set<UINT32>::iterator it =  pWaitMatchTeam->begin();
		UINT32 teamA = 0;
		UINT32 teamB = 0;
		if (it != pWaitMatchTeam->end()){//出现问题
			teamA = *it;
			++it ;
			if (it != pWaitMatchTeam->end()){
				teamB = *it;
			}
		}
		
		if (teamA == 0 || teamB == 0){
			return false;
		}

		CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(teamA);
		if (pTeam){
			for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
				UINT32 charID = pTeam->getCharID(i);
				if (charID){
					pkMembers.push_back(charID);
					CTeamMgr::getSingleton().removeCharID2Team(charID);
				}
			}
			if (CTeamMgr::getSingleton().removeTeam(pTeam)){
				SAFE_DELETE(pTeam);
			}
		}
		
		pWaitMatchTeam->erase(teamA);

		it = pWaitMatchUser->begin();
		if (it != pWaitMatchUser->end()){
			pkMembers.push_back(*it);
			pWaitMatchUser->erase(it);
		}


		pTeam = CTeamMgr::getSingleton().getTeamByID(teamB);
		if (pTeam){
			for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
				UINT32 charID = pTeam->getCharID(i);
				if (charID){
					pkMembers.push_back(charID);
					CTeamMgr::getSingleton().removeCharID2Team(charID);
				}
			}

			if (CTeamMgr::getSingleton().removeTeam(pTeam)){
				SAFE_DELETE(pTeam);
			}
		}

		pWaitMatchTeam->erase(teamB);
		it = pWaitMatchUser->begin();
		if (it != pWaitMatchUser->end()){
			pkMembers.push_back(*it);
			pWaitMatchUser->erase(it);
		}
	}
	else if (pWaitMatchTeam->size() >= 1){//未满队伍不足1队情况
		if (pWaitMatchUser->size() < 4){
			return false;
		}	
		
		std::set<UINT32>::iterator it =  pWaitMatchTeam->begin();
		if (it == pWaitMatchTeam->end()){
			return false;
		}

		CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(*it);
		if (pTeam){
			for (UINT8 i = 0;i < CTeam::s_MAX_MEMBER; ++i){
				UINT32 charID = pTeam->getCharID(i);
				if (charID){
					pkMembers.push_back(charID);
					CTeamMgr::getSingleton().removeCharID2Team(charID);
				}
			}

			if (CTeamMgr::getSingleton().removeTeam(pTeam)){
				SAFE_DELETE(pTeam);
			}
		}
		pWaitMatchTeam->erase(it);
		
		it = pWaitMatchUser->begin();
		for (;it != pWaitMatchUser->end();){
			std::set<UINT32>::iterator tmpIt = it;
			++it;
			pkMembers.push_back(*tmpIt);
			pWaitMatchUser->erase(tmpIt);
			if (pkMembers.size() >= (UINT32)s_MAX_PK_NUM){
				break;
			}
		}

	}
	else {
		return false;
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
	}
	g_globalServer->getConnMgr().broadcastByID(gameID,cmd);
	return true;
}
