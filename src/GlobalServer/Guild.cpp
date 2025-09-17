#include "Guild.h"
#include "Log4cxxHelp.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "PacketProcessor.h"
#include "GuildConfig.h"
#include "RoleInfoManager.h"
#include "RankMgr.h"
#include "Team.h"
#include "GlobalVar.h"
#include "TblDataMgr.h"
#include "GuildConfig.h"
#include "SysLanguages.h"

extern GlobalServer * g_globalServer;

CTblField g_guildMemberTblDef[] = {
	{ "CHARID",      	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "NAME",   	 	CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "JOB",      	 	CTblField::DB_TYPE_UINT8,  sizeof(UINT8) },
	{ "SEX",      	 	CTblField::DB_TYPE_UINT8,  sizeof(UINT8) },
	{ "TITLE",       	CTblField::DB_TYPE_UINT8,  sizeof(UINT8) },
	{ "CONTRIBUTE",  	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "GUILDID", 	 	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "JOININTIME",  	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "LASTLOGINTIME",	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "ACTIVITY_DEGREE",CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "CHALLENGE_NUM",	CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ "ASSIST_NUM",		CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ "DAMAGE1",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "DAMAGE2",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "DAMAGE3",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ NULL, 0, 0 },
};

CTblField g_guildTblDef[] = {
	{ "ID",      	 CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "NAME",   	 CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "LEADERID",  	 CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "CONTRIBUTE",  CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "HALLLV", 	 CTblField::DB_TYPE_UINT8,   sizeof(UINT8) },
	{ "PRAYBUILDINGLV", CTblField::DB_TYPE_UINT8,   sizeof(UINT8) },
	{ "SHOPLV", 	 CTblField::DB_TYPE_UINT8,   sizeof(UINT8) },
	{ "FIRETIME", 	 CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "SIGN",        CTblField::DB_TYPE_STRING,  GUILD_SIGN_LEN },
	{ "NOTICE",      CTblField::DB_TYPE_STRING,  GUILD_SIGN_LEN },
	{ "BADGE",       CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ "ACTIVITY_DEGREE",CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "BLACKMARKETLV",  CTblField::DB_TYPE_UINT8,   sizeof(UINT8) },
	{ NULL, 0, 0 },
};

CTblField g_guildTraceDef[] = {
	{ "ID",     	 CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "TRACEID",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "CHARID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "NAME",   	 CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "DOTIME",    	 CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "GUILDID",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_guildSearchTblDef[] = {
	{ "ID",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_guildCopyMapDef[] = {
	{ "COPYMAP_ID",	 CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "OPENTIME",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "BOSSHP",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "STEPID",      CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ "GUILD_ID",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "KILL_CHARID", CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_guildMemberResetTblDef[] = {
	{ "CHALLENGE_NUM",	CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ "ASSIST_NUM",		CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ "DAMAGE1",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "DAMAGE2",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "DAMAGE3",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CTblField g_guildCopymapScoreRankTblDef[] = {
	{ "COPYMAP_ID",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "GUILD_ID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "SCORE",			CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CGuildMember::CGuildMember()
{
	//m_level = 0;
	m_job = 0;
	m_sex = 0;
	m_title = 0;
	m_contribute = 0;
	m_guildID = 0;
	m_joinInTime = 0;
	m_lastLoginTime = 0;
	m_todayContribute = 0;
	m_lastAddCbTime = 0;
	m_activityDegree = 0;
	m_challengeNum = 0;
	m_assitNum = 0;
	m_damage1 = 0;
	m_damage2 = 0;
	m_damage3 = 0;
	m_decLilian = 0;
	m_photoID = 0;
}

CGuildMember::~CGuildMember()
{

}

void CGuildMember::initFromDB(const stGuildMemberDB & data)
{
	setID(data.m_charID);
	setName(data.m_name);
	//m_level = data.m_level;
	m_job = data.m_job;
	m_title = data.m_title;
	m_sex = data.m_sex;
	m_contribute = data.m_contribute;
	m_guildID = data.m_guildID;
	m_joinInTime  = data.m_joinInTime;
	m_lastLoginTime = data.m_lastLoginTime;
	m_activityDegree = data.m_activityDegree;
	m_challengeNum = data.m_challengeNum;
	m_assitNum = data.m_assitNum;
	m_damage1 = data.m_damage1;
	m_damage2 = data.m_damage2;
	m_damage3 = data.m_damage3;
	m_photoID = data.m_photoID;
}

void CGuildMember::fillDBdata(stGuildMemberDB & data)
{
	data.m_charID = getID();
	strncpy(data.m_name,getName(),sizeof(data.m_name));
	//data.m_level = m_level;
	data.m_job = m_job;
	data.m_sex = m_sex;
	data.m_title = m_title;
	data.m_contribute = m_contribute;
	data.m_guildID = m_guildID;
	data.m_joinInTime = m_joinInTime;
	data.m_lastLoginTime = m_lastLoginTime;
	data.m_activityDegree = m_activityDegree;
	data.m_challengeNum = m_challengeNum;
	data.m_assitNum = m_assitNum;
	data.m_damage1 = m_damage1;
	data.m_damage2 = m_damage2;
	data.m_damage3 = m_damage3;
	data.m_photoID = m_photoID;
}

void CGuildMember::fillSynData(GuildProto::stSynGuildMemInfo & data)
{
	data.set_charid(getID());
	data.set_contribute(m_contribute);
	data.set_job(m_job);
	data.set_sex(m_sex);
	//data.set_level(m_level);
	data.set_level(CRankMgr::getSingleton().getLevelByCharID(getID()));
	data.set_title(m_title);
	data.set_name(getName());
	if (PlayerManager::getSingleton().getUserByID(data.charid())){
		data.set_online(true);
	}
	else {
		data.set_online(false);
	}
	data.set_battlepower(CRankMgr::getSingleton().getBattlePowerByCharID(getID()));
	data.set_todaycontribute(m_todayContribute);
	data.set_challengenum(m_challengeNum);
	data.set_assistnum(m_assitNum);
	data.set_activityval(m_activityDegree);
	data.set_lastlogintime(m_lastLoginTime);
	data.set_photoid(m_photoID);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[工会] lastLoginTime：%d",m_lastLoginTime);
#endif
}

bool CGuildMember::synContribute(UINT32 num)
{
	m_contribute = num;
	m_lastAddCbTime = g_globalServer->getSec();

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByID(m_guildID);
	if (pGuild){

		GuildProto::stNotifyGuildMemContribute notifyCmd;
		notifyCmd.set_charid(getID());
		notifyCmd.set_curval(m_contribute);
		pGuild->sendProto2Mem(notifyCmd);
	}
	return true;
}

bool CGuildMember::addChallengeNum(UINT16 num)
{
	m_challengeNum += num;
	GuildProto::stSynMemberChallengeNum retCmd;
	retCmd.set_charid(getID());
	retCmd.set_challengenum(m_challengeNum);

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByID(m_guildID);
	if (pGuild){
		pGuild->sendProto2Mem(retCmd);
	}
	Athena::logger->trace("公会成员(id:%d,name:%s)增加了:%d次挑战次数",getID(),getName(),num);
	return true;
}

bool CGuildMember::addAssistNum(UINT16 num)
{
	m_assitNum += num;
	GuildProto::stSynMemberAssistNum retCmd;
	retCmd.set_charid(getID());
	retCmd.set_assistnum(m_assitNum);

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByID(m_guildID);
	if (pGuild){
		pGuild->sendProto2Mem(retCmd);
	}
	Athena::logger->trace("公会成员(id:%d,name:%s)增加了%d次协助次数",getID(),getName(),num);
	return true;
}


void CGuildMember::sendDetailInfo(Player * pUser)
{
	GuildProto::stRetGuildMemInfo cmd;
	cmd.set_allcontribute(m_contribute);
	cmd.set_charid(getID());
	if (m_lastAddCbTime){
		cmd.set_contributetime( g_globalServer->getSec() - m_lastAddCbTime);
	}
	else {
		cmd.set_allcontribute(0);
	}
	cmd.set_joinintime(g_globalServer->getSec() - m_joinInTime);
	cmd.set_lastlogintime(g_globalServer->getSec() - m_lastLoginTime);
	cmd.set_todaycontribute(m_todayContribute);
	pUser->sendProtoToMe(cmd);
}

void CGuildMember::onDecSp(UINT16 sp)
{
	UINT16 add = sp / s_SP_2_ACTIVITY_RATE;
	UINT16 oldVal = m_activityDegree;
	m_activityDegree += add;

	if (m_activityDegree > CGuildConfig::getSingleton().getMemberActivityLimit()){
		m_activityDegree = CGuildConfig::getSingleton().getMemberActivityLimit(); 
	}
	add = m_activityDegree > oldVal ? m_activityDegree - oldVal : 0 ; 

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByID(m_guildID);
	if (pGuild){
		pGuild->addActivityDegree(add,"消耗体力");

		GuildProto::stNotifyGuildMemActivityDegree notifyCmd;
		notifyCmd.set_charid(getID());
		notifyCmd.set_curval(m_activityDegree);
		pGuild->sendProto2Mem(notifyCmd);
	}
	Athena::logger->trace("公会成员(id:%d,name:%s)消耗体力增加了:%d活跃度,现在活跃度是:%d",getID(),getName(),add,m_activityDegree);
}

void CGuildMember::onDecLilian(UINT16 lilian)
{
	m_decLilian += lilian;
	UINT16 add = m_decLilian / s_LILIAN_2_ACTIVITY_RATE;	
	UINT16 oldVal = m_activityDegree;
	m_activityDegree += add;
	m_decLilian = m_decLilian % s_LILIAN_2_ACTIVITY_RATE;

	if (m_activityDegree > CGuildConfig::getSingleton().getMemberActivityLimit()){
		m_activityDegree = CGuildConfig::getSingleton().getMemberActivityLimit();		
	}
	add = m_activityDegree > oldVal ? m_activityDegree - oldVal : 0;

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByID(m_guildID);
	if (pGuild){
		pGuild->addActivityDegree(add,"消耗历练");

		GuildProto::stNotifyGuildMemActivityDegree notifyCmd;
		notifyCmd.set_charid(getID());
		notifyCmd.set_curval(m_activityDegree);
		pGuild->sendProto2Mem(notifyCmd);
	}
	Athena::logger->trace("公会成员(id:%d,name:%s)消耗历练增加了:%d活跃度,现在活跃度是:%d",getID(),getName(),add,m_activityDegree);
}

CGuild::CGuild() : ElementMgr(false,true,true,false)
{
	m_exp = 0;
	m_leaderID = 0;
	m_contribute = 0;
	bzero(m_sign,sizeof(m_sign));
	m_hallLv = 0;
	m_prayBuildingLv = 0;
	m_shopLv = 0;
	m_refresh = true;
	m_fireTime =  0;
	m_payNum = 0;
	bzero(m_notice,sizeof(m_notice));
	m_badge = 0;
	m_activityDegree = 0;
	m_zeroRefresh = false;
	m_blackMarketLv = 0;
}

CGuild::~CGuild()
{
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CGuildMember * pGuildMem = (CGuildMember *)it->second;
		SAFE_DELETE(pGuildMem);
	}
}

void CGuild::initFromDB(const stGuildDB & data)
{
	setID(data.m_id);
	setName(data.m_name);
	m_leaderID = data.m_leaderID;
	m_contribute  = data.m_contribute;
	m_hallLv = data.m_hallLv;						//公会大营等级
	m_prayBuildingLv = data.m_prayBuildingLv;						//兵粮大营等级	
	m_shopLv = data.m_shopLv;						//特训营等级	
	m_fireTime  = data.m_fireTime;							//解散时间
	strncpy(m_sign,data.m_sign,sizeof(m_sign));
	strncpy(m_notice,data.m_notice,sizeof(m_notice));
	m_badge = data.m_badge;	
	m_activityDegree = data.m_activityDegree;
	m_blackMarketLv = data.m_blackMarketLv;
}

void CGuild::fillDBdata(stGuildDB & data)
{
	data.m_id = getID();
	strncpy(data.m_name,getName(),sizeof(data.m_name));
	data.m_leaderID = m_leaderID;
	data.m_contribute  = m_contribute;
	strncpy(data.m_sign,m_sign,sizeof(data.m_sign));
	data.m_hallLv = m_hallLv;											//公会大营等级
	data.m_prayBuildingLv = m_prayBuildingLv;							//祈祷大厅等级	
	data.m_shopLv = m_shopLv;											//商店大厅等级	
	data.m_fireTime = m_fireTime;										//解散时间
	strncpy(data.m_notice,m_notice,sizeof(m_notice));
	data.m_badge = m_badge;
	data.m_activityDegree = m_activityDegree;
	data.m_blackMarketLv = m_blackMarketLv;
}

void CGuild::fillGuildListInfo(GuildProto::stGuildListInfo & data)
{
	data.set_guildid(getID());
	data.set_guildname(getName());
	CGuildMember *pMember =  getGuildMemberByID(m_leaderID);
	if (pMember) {
		data.set_leadername(pMember->getName());
		data.set_leaderlv(CRankMgr::getSingleton().getLevelByCharID(m_leaderID));
	}
	data.set_halllv(m_hallLv);
	data.set_memnum(ElementMgr::size());
	data.set_maxnum(getMaxMember());
	data.set_sign(m_sign);
	data.set_badge(m_badge);
}

void CGuild::fillDetailInfo(GuildProto::stRetGuildDetailInfo * data)
{
	data->set_guildid(getID());
	data->set_guildname(getName());
	data->set_exp(m_exp);
	data->set_halllv(m_hallLv);
	data->set_maxnum(getMaxMember());
	data->set_sign(m_sign);
	data->set_memnum(0);
	data->set_badge(m_badge);
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		GuildProto::stSynGuildMemInfo * pSynGroupMem = data->add_meminfo();
		CGuildMember * pGuildMem = (CGuildMember *)it->second;
		pGuildMem->fillSynData(*pSynGroupMem);
	}
}

void CGuild::sendGuildInfo(Player * pUser)
{
	if (!pUser){
		return ;
	}

	GuildProto::stSendMyGuild  cmd ;
	cmd.set_contribute(m_contribute);
	cmd.set_exp(m_exp);
	cmd.set_guildid(getID());
	cmd.set_leaderid(m_leaderID);
	cmd.set_halllv(m_hallLv);
	if (m_fireTime){
		cmd.set_fireremaintime(m_fireTime - g_globalServer->getSec());
	}
	cmd.set_guildname(getName());
	cmd.set_sign(m_sign);
	cmd.set_maxnum(getMaxMember());
	cmd.set_halllv(m_hallLv);
	cmd.set_praybuildinglv(m_prayBuildingLv);
	cmd.set_shoplv(m_shopLv);
	cmd.set_blackmarket(m_blackMarketLv);
	cmd.set_notice(m_notice);
	cmd.set_badge(m_badge);
	cmd.set_rank(CGuildMgr::getSingleton().getGuildRank(getID()));
	cmd.set_activitydegree(m_activityDegree);

	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		GuildProto::stSynGuildMemInfo * pSynGroupMem = cmd.add_data();
		CGuildMember * pGuildMem = (CGuildMember *)it->second;
		pGuildMem->fillSynData(*pSynGroupMem);
	}
	pUser->sendProtoToMe(cmd);
}

void CGuild::addApplier(Player * pUser)
{
	if (!pUser){
		return ;
	}

	stApplyJoinInInfo one;
	one.m_applyTime = g_globalServer->getSec();
	one.m_charID = pUser->getID();
	one.m_job = pUser->m_job;
	one.m_level = pUser->m_level;
	one.m_sex = pUser->m_sex;
	strncpy(one.m_name,pUser->getName(),sizeof(one.m_name));
	one.m_photoID = pUser->m_photoID;
	m_applyList.push_back(one);

	if (m_applyList.size() > s_MAX_APPLY_NUM){
		m_applyList.pop_front();	
	} 
}

bool CGuild::isInApplyList(UINT32 charID)
{
	std::list<stApplyJoinInInfo>::iterator it = m_applyList.begin();
	for (;it != m_applyList.end();++it){
		if (charID == it->m_charID){
			return true;
		}
	}
	return false;
}

void CGuild::delFromApplyList(UINT32 charID)
{
	std::list<stApplyJoinInInfo>::iterator it = m_applyList.begin();
	for (;it != m_applyList.end();++it){
		if (charID == it->m_charID){
			m_applyList.erase(it);
			return ;
		}
	}

}

void CGuild::sendApplyList(Player * pUser)
{
	if (!pUser){
		return ;
	}
	GuildProto::stRetApplyJoinInList  cmd;
	std::list<stApplyJoinInInfo>::iterator it = m_applyList.begin();
	for (;it != m_applyList.end();++it){
		GuildProto::stSynApplierInfo * pSynInfo = cmd.add_data();
		pSynInfo->set_charid(it->m_charID);
		pSynInfo->set_level(it->m_level);
		pSynInfo->set_job(it->m_job);
		pSynInfo->set_sex(it->m_sex);
		pSynInfo->set_passtime(it->m_applyTime);
		pSynInfo->set_name(it->m_name);
		pSynInfo->set_photoid(it->m_photoID);
		pSynInfo->set_battlepower(CRankMgr::getSingleton().getBattlePowerByCharID(it->m_charID));
	}
	pUser->sendProtoToMe(cmd);
}

UINT16 CGuild::getMaxMember()
{
	CGuildConfig::stHallInfo * pHallInfo = CGuildConfig::getSingleton().getHallInfo(m_hallLv);
	if (pHallInfo){
		return pHallInfo->m_maxMem;	
	}
	else {
		return 20;
	}
}


void CGuild::leaveGuild(Player * pUser)
{
	if (!pUser){
		return ;
	}

	CGuildMember * pGuildMem = getGuildMemberByID(pUser->getID());
	if (!pGuildMem){
		return ;
	}

	GuildProto::stMemLeaveGuild leaveCmd;
	leaveCmd.set_charid(pUser->getID());
	sendProto2Mem(leaveCmd);

	if (!removeMember(pGuildMem)){
		return ;
	}
	SAFE_DELETE(pGuildMem);
	CGuildMgr::getSingleton().deleteGuildMem(pUser->getID());
	CGuildMgr::getSingleton().removeCharID2GuildID(pUser->getID());
	addTrace(GuildProto::eTrace_Leave_Guild,pUser->getID(),pUser->getName());

	sendGuildLog(pUser,"离开公会","");

	Global::stUserLeaveGuild cmd;
	cmd.add_charids(pUser->getID());
	pUser->sendProto2Game(cmd);
	Athena::logger->trace("公会成员(id:%d,name:%s)离开了公会:%s",pUser->getID(),pUser->getName(),getName());
}

void CGuild::assignTitle(Player * pLeader,UINT32 charID,GuildProto::eGuildTile eTitle)
{
	if (!pLeader){
		return ;
	}

	if (eTitle >= GuildProto::GUILD_TITLE_LEADER){
		return ;
	}

	CGuildMember * pGuildMem = getGuildMemberByID(charID);
	CGuildMember * pLeaderMem = getGuildMemberByID(pLeader->getID());
	if (pGuildMem == pLeaderMem){//不能任命自己
		Athena::logger->error("不能任命自己!");
		return ;	
	}

	GuildProto::stAssignTitleError errorCmd;
	if (!pGuildMem || !pLeaderMem){
		errorCmd.set_result(GuildProto::ASSIGN_TITLE_ERROR_NOT_MEMBER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}

	if (pGuildMem->getTitle() >= pLeaderMem->getTitle()){
		errorCmd.set_result(GuildProto::ASSIGN_TITLE_ERROR_NO_POWER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}

	pGuildMem->m_title = eTitle;
	GuildProto::stAssignGuildTitle assignTitleCmd;
	assignTitleCmd.set_charid(charID);
	assignTitleCmd.set_etitle(eTitle);
	sendProto2Mem(assignTitleCmd);
	stGuildMemberDB dataDB;
	pGuildMem->fillDBdata(dataDB);
	CGuildMgr::getSingleton().updateGuildMem(dataDB);

	if (GuildProto::GUILD_TILE_VICELEADER == eTitle) { 
		addTrace(GuildProto::eTrace_Assign_Vice_Leader,pGuildMem->getID(),pGuildMem->getName());
	}
	else if (GuildProto::GUILD_TITLE_ELITE == eTitle){
		addTrace(GuildProto::eTrace_Assign_Elite,pGuildMem->getID(),pGuildMem->getName());
	}
	char szObjID[20] = { 0 };
	snprintf(szObjID,sizeof(szObjID),"%d",eTitle);
	sendGuildLog(pLeader,"授予职位",szObjID);

	Athena::logger->trace("玩家(id:%d,name:%s)授予了玩家(id:%d,name:%s)头衔:%d",pLeaderMem->getID(),pLeaderMem->getName(),pGuildMem->getID(),pGuildMem->getName(),eTitle);
}

void CGuild::updateMemberInfo(Player * pUser)
{
	if(!pUser){
		return ;
	}

	CGuildMember * pGuildMem = getGuildMemberByID(pUser->getID());
	if(!pGuildMem){
		return;
	}

	pGuildMem->setName(pUser->getName());
	pGuildMem->setPhotoID(pUser->m_photoID);
	stGuildMemberDB dataDB;
	pGuildMem->fillDBdata(dataDB);
	CGuildMgr::getSingleton().updateGuildMem(dataDB);
}

void CGuild::cancelTitle(Player * pLeader,UINT32 charID)
{
	if (!pLeader){
		return ;
	}
	CGuildMember * pGuildMem = getGuildMemberByID(charID);
	CGuildMember * pLeaderMem = getGuildMemberByID(pLeader->getID());

	if (pGuildMem == pLeaderMem){//不能踢自己
		Athena::logger->error("不能踢自己,踢玩家出公会失败!");
		return ;	
	}

	GuildProto::stCancelTitleError errorCmd;
	if (!pGuildMem || !pLeaderMem){
		errorCmd.set_result(GuildProto::CANCEL_TITLE_ERROR_NOT_MEMBER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}

	if (pGuildMem->getTitle() >= pLeaderMem->getTitle()){
		errorCmd.set_result(GuildProto::CANCEL_TITLE_ERROR_NO_POWER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}

	pGuildMem->m_title = GuildProto::GUILD_TITLE_NORMAL;
	GuildProto::stCancelTitle cancelTitleCmd;
	cancelTitleCmd.set_charid(pGuildMem->getID());
	sendProto2Mem(cancelTitleCmd);
	stGuildMemberDB dataDB;
	pGuildMem->fillDBdata(dataDB);
	CGuildMgr::getSingleton().updateGuildMem(dataDB);

	sendGuildLog(pLeader,"免职","0");
	Athena::logger->trace("玩家(id:%d,name:%s)取消了玩家(id:%d,name:%s)头衔",pLeaderMem->getID(),pLeaderMem->getName(),pGuildMem->getID(),pGuildMem->getName());
}

void CGuild::giveLeader(Player * pLeader,UINT32 charID)
{
	if (!pLeader){
		return ;
	}

	CGuildMember * pGuildMem = getGuildMemberByID(charID);
	GuildProto::stGiveLeaderError  errorCmd;
	if (!pGuildMem){
		errorCmd.set_result(GuildProto::GIVE_LEADER_ERROR_NOT_MEMBER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}

	CGuildMember * pLeaderMem = getGuildMemberByID(pLeader->getID());
	if (!pLeaderMem){
		errorCmd.set_result(GuildProto::GIVE_LEADER_ERROR_NOT_MEMBER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}

	if (m_leaderID != pLeader->getID()){
		errorCmd.set_result(GuildProto::GIVE_LEADER_ERROR_NO_POWER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}

	m_leaderID = charID;
	pLeaderMem->m_title = GuildProto::GUILD_TITLE_NORMAL;
	pGuildMem->m_title = GuildProto::GUILD_TITLE_LEADER;
	GuildProto::stGiveLeader2Other giveLeaderCmd;
	giveLeaderCmd.set_charid(charID);
	sendProto2Mem(giveLeaderCmd);
	stGuildMemberDB dataDB;
	pGuildMem->fillDBdata(dataDB);
	CGuildMgr::getSingleton().updateGuildMem(dataDB);
	pLeaderMem->fillDBdata(dataDB);
	CGuildMgr::getSingleton().updateGuildMem(dataDB);

	stGuildDB guildData;
	this->fillDBdata(guildData);
	CGuildMgr::getSingleton().updateGuild(guildData);

	Athena::logger->trace("公会成员(id:%d,name:%s)把会长转让给了玩家:%d",getID(),getName(),charID);
}

void CGuild::killOutMember(Player * pLeader,UINT32 charID)
{
	if (!pLeader){
		return ;
	}	

	GuildProto::stKillOutGroupMemberError  errorCmd;

	CGuildMember *pMember =  getGuildMemberByID(pLeader->getID());
	if (!pMember){
		Athena::logger->error("踢成员,不是公会成员!");
		errorCmd.set_result(GuildProto::KILL_OUT_ERROR_NO_POWER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}    

	if (!CGuildConfig::getSingleton().hasRight(pMember->getTitle(),CGuildConfig::eRight_Dislodge)){//没有权限 
		Athena::logger->error("踢成员不成功,没有权限!");
		errorCmd.set_result(GuildProto::KILL_OUT_ERROR_NO_POWER);
		pLeader->sendProtoToMe(errorCmd);
		return ;
	}    

	CGuildMember * pGuildMem = getGuildMemberByID(charID);
	if (!pGuildMem){
		errorCmd.set_result(GuildProto::KILL_OUT_ERROR_NOT_MEMBER);
		pLeader->sendProtoToMe(errorCmd);
		return ;		
	}

	Athena::logger->trace("玩家(id:%d,name:%s)把成员(id:%d,name:%s)踢出了公会(id:%d,name:%s)",pLeader->getID(),pLeader->getName(),pGuildMem->getID(),pGuildMem->getName(),getID(),getName());

	GuildProto::stKillOutGroupMember killOutCmd;
	killOutCmd.set_charid(charID);
	this->sendProto2Mem(killOutCmd);

	if (!removeMember(pGuildMem)){
		return ;
	}

	CGuildMgr::getSingleton().deleteGuildMem(pGuildMem->getID());
	GuildProto::stKillOutTip  tipCmd;
	Player * pOther = PlayerManager::getSingleton().getUserByID(pGuildMem->getID());
	if (pOther){
		pOther->sendProtoToMe(tipCmd);
		Global::stUserLeaveGuild leaveCmd;
		leaveCmd.add_charids(pOther->getID());
		pOther->sendProto2Game(leaveCmd);

		sendGuildLog(pOther,"踢出公会","");
	}
	else {
		RoleInfoManager::getSingleton().updateLeaveGuildTime(pGuildMem->getID(),g_globalServer->getSec());
	}

	CGuildMgr::getSingleton().removeCharID2GuildID(pGuildMem->getID());
	addTrace(GuildProto::eTrace_Kill_Out_Member,pGuildMem->getID(),pGuildMem->getName());
	SAFE_DELETE(pGuildMem);
}

void CGuild::addContribute(UINT32 num,const char * desc)
{
	if (!desc){
		return ;
	}

	UINT32 oldVal = m_contribute;
	m_contribute += num;
	Athena::logger->trace("公会(id:%d,name:%s),因为:%s,获得了%d贡献度",getID(),getName(),desc,num);
	synContribute();

	Global::sstSendGuildResLog webCmd;
	webCmd.set_guildid(getID());
	webCmd.set_reason(desc);
	webCmd.set_res("贡献度");
	webCmd.set_newval(m_contribute);
	webCmd.set_oldval(oldVal);
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

bool CGuild::removeContribute(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_contribute < num){
		return false;
	}
	UINT32 oldVal = m_contribute;
	m_contribute -= num;
	Athena::logger->trace("公会(id:%d,name:%s),因为:%s,扣除了%d贡献度",getID(),getName(),desc,num);
	synContribute();

	Global::sstSendGuildResLog webCmd;
	webCmd.set_guildid(getID());
	webCmd.set_reason(desc);
	webCmd.set_res("贡献度");
	webCmd.set_newval(m_contribute);
	webCmd.set_oldval(oldVal);
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	return true;
}

void CGuild::synContribute()
{
	GuildProto::stSynGuildContribute synCmd;
	synCmd.set_num(m_contribute);
	sendProto2Mem(synCmd);
}


void CGuild::chgSign(Player * pUser,const char * sign)
{
	if (!pUser || !sign){
		return ;
	}

	CGuildMember *pMember =  getGuildMemberByID(pUser->getID());
	if (!pMember){
		Athena::logger->error("修改签名失败,不是公会成员!");
		return ;
	}

	if (!CGuildConfig::getSingleton().hasRight(pMember->m_title,CGuildConfig::eRight_Manifesto)){//没有权限 
		Athena::logger->error("试图修改公会的签名不成功,没有权限!");
		return ;
	}

	strncpy(m_sign,sign,sizeof(m_sign));
	GuildProto::stChgGuildSign cmd;
	cmd.set_sign(m_sign);
	sendProto2Mem(cmd);

	Athena::logger->trace("公会成员(id:%d,name:%s)修改了签名",pUser->getID(),pUser->getName());
	stGuildDB data;
	this->fillDBdata(data);
	CGuildMgr::getSingleton().updateGuild(data);
}

void CGuild::chgNotice(Player * pUser,const char * notice)
{
	if (!pUser || !notice){
		return ;
	}	

	CGuildMember *pMember =  getGuildMemberByID(pUser->getID());
	if (!pMember){
		Athena::logger->error("修改公告失败,不是公会成员!");
		return ;
	}

	if (!CGuildConfig::getSingleton().hasRight(pMember->m_title,CGuildConfig::eRight_Nofity)){//没有权限 
		Athena::logger->error("试图修改公会的公告不成功,没有权限!");
		return ;
	}

	strncpy(m_notice,notice,sizeof(m_notice));
	GuildProto::stChgNotice cmd;
	cmd.set_notice(m_notice);
	sendProto2Mem(cmd);

	Athena::logger->trace("公会成员(id:%d,name:%s)修改了公告",pUser->getID(),pUser->getName());

	stGuildDB data;
	this->fillDBdata(data);
	CGuildMgr::getSingleton().updateGuild(data);
}

void CGuild::changeBadge(Player * pUser,UINT16 badgeID)
{
	if (!pUser){
		return ;
	}	

	if (m_leaderID != pUser->getID()){//没有权限 
		return ;
	}

	m_badge = badgeID;
	GuildProto::stChangeBadge cmd;
	cmd.set_result(GuildProto::stChangeBadge::eSuccess);
	cmd.set_badgeid(badgeID);
	sendProto2Mem(cmd);

	char szObj[20] = { 0 };
	snprintf(szObj,sizeof(szObj),"%d",badgeID);
	sendGuildLog(pUser,"更改会标",szObj);
	Athena::logger->trace("公会成员(id:%d,name:%s)修改了会标",pUser->getID(),pUser->getName());
}

void CGuild::setAllMemberLeaveTime()
{
	Global::stUserLeaveGuild cmd;
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CGuildMember * pGuildMem = (CGuildMember *)it->second;
		if (pGuildMem){
			cmd.add_charids(pGuildMem->getID());
			CGuildMgr::getSingleton().removeCharID2GuildID(pGuildMem->getID());
		}
	}
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

void CGuild::userLevelup(UINT32 charID,UINT16 level)
{
	CGuildMember * pGuildMem = getGuildMemberByID(charID);
	if (!pGuildMem){
		return ;
	}
	//pGuildMem->setLevel(level);
	//stGuildMemberDB dataDB;
	//pGuildMem->fillDBdata(dataDB);
	//CGuildMgr::getSingleton().updateGuildMem(dataDB);
}

void CGuild::levelupHall(Player * pUser)
{
	if (!pUser){
		return ;
	}

	GuildProto::stLevelUpHallLvResult retCmd;

	if (!checkMemberRight(pUser->getID(),CGuildConfig::eRight_Lvup)){
		retCmd.set_result(GuildProto::stLevelUpHallLvResult::eHAS_NOT_RIGHT);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	CGuildConfig::stHallInfo * pHallInfo = CGuildConfig::getSingleton().getHallInfo(m_hallLv);

	if (!pHallInfo){
		retCmd.set_result(GuildProto::stLevelUpHallLvResult::eINNER_ERROR);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (!removeContribute(pHallInfo->m_contribute,"升级大厅")){
		retCmd.set_result(GuildProto::stLevelUpHallLvResult::eCONTRIBUTE_NOT_ENOGUH);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	m_hallLv += 1;

	GuildProto::stSynHallLv synCmd;
	synCmd.set_lv(m_hallLv);
	//pUser->sendProtoToMe(synCmd);
	sendProto2Mem(synCmd);

	retCmd.set_result(GuildProto::stLevelUpHallLvResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);

	addTrace(GuildProto::eTrace_Hall_Levelup,pUser->getID(),pUser->getName());

	CGuildMgr::getSingleton().guildLevelChg(getID(),getLevel());

	stGuildDB data;
	this->fillDBdata(data);
	CGuildMgr::getSingleton().updateGuild(data);

	sendGuildLog(pUser,"升级建筑","大厅");

	Global::stSynGuildLv2AllMember cmd;
	cmd.set_guildlv(getLevel());
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CGuildMember * pGuildMem = (CGuildMember *)it->second;
		if (pGuildMem){
			cmd.add_members(pGuildMem->getID());
		}
	}
	pUser->sendProto2Game(cmd);
	Athena::logger->trace("公会成员(id:%d,name:%s)把公会(id:%d,name:%s)大厅等级升级到了:%d",pUser->getID(),pUser->getName(),getID(),getName(),m_hallLv);
}

void CGuild::levelupPrayBuilding(Player * pUser)
{
	if (!pUser){
		return ;
	}

	GuildProto::stPrayBuildingLvResult retCmd;
	if (!checkMemberRight(pUser->getID(),CGuildConfig::eRight_Lvup)){
		retCmd.set_result(GuildProto::stPrayBuildingLvResult::eHAS_NOT_RIGHT);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	CGuildConfig::stPrayInfo * pPrayInfo = CGuildConfig::getSingleton().getPrayInfo(m_prayBuildingLv);

	if (!pPrayInfo){
		retCmd.set_result(GuildProto::stPrayBuildingLvResult::eINNER_ERROR);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (m_prayBuildingLv >= m_hallLv){//等级不能高于大厅等级
		retCmd.set_result(GuildProto::stPrayBuildingLvResult::eHALL_LV_TOO_LOW);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (!removeContribute(pPrayInfo->m_contribute,"升级祈福殿")){
		retCmd.set_result(GuildProto::stPrayBuildingLvResult::eCONTRIBUTE_NOT_ENOGUH);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	m_prayBuildingLv += 1;

	GuildProto::stSynPrayBuildingLv synCmd;
	synCmd.set_lv(m_prayBuildingLv);
	//pUser->sendProtoToMe(synCmd);
	sendProto2Mem(synCmd);

	retCmd.set_result(GuildProto::stPrayBuildingLvResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);

	addTrace(GuildProto::eTrace_Pray_BuildingLevelup,pUser->getID(),pUser->getName());

	stGuildDB data;
	this->fillDBdata(data);
	CGuildMgr::getSingleton().updateGuild(data);
	sendGuildLog(pUser,"升级建筑","祈福殿");

	Athena::logger->trace("公会成员(id:%d,name:%s)升级了公会(id:%d,name:%s)祈福殿的等级到:%d",pUser->getID(),pUser->getName(),getID(),getName(),m_prayBuildingLv);
}

void CGuild::levelupShop(Player * pUser)
{
	if (!pUser){
		return ;
	}

	GuildProto::stLevelUpShopBuildingLvResult retCmd;
	if (!checkMemberRight(pUser->getID(),CGuildConfig::eRight_Lvup)){
		retCmd.set_result(GuildProto::stLevelUpShopBuildingLvResult::eHAS_NO_RIGHT);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	CGuildConfig::stShopInfo * pShopInfo = CGuildConfig::getSingleton().getShopInfo(m_shopLv);

	if (!pShopInfo){
		retCmd.set_result(GuildProto::stLevelUpShopBuildingLvResult::eINNER_ERROR);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (m_shopLv >= m_hallLv){//等级不能高于大厅等级
		retCmd.set_result(GuildProto::stLevelUpShopBuildingLvResult::eHALL_LV_TOO_LOW);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (!removeContribute(pShopInfo->m_contribute,"升级商店")){
		retCmd.set_result(GuildProto::stLevelUpShopBuildingLvResult::eCONTRIBUTE_NOT_ENOUGH);
		pUser->sendProtoToMe(retCmd);
		return ;
	}


	m_shopLv += 1;

	GuildProto::stSynShopBuilding synCmd;
	synCmd.set_lv(m_shopLv);
	//pUser->sendProtoToMe(synCmd);
	sendProto2Mem(synCmd);

	retCmd.set_result(GuildProto::stLevelUpShopBuildingLvResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);

	addTrace(GuildProto::eTrace_Shop_Levelup,pUser->getID(),pUser->getName());

	stGuildDB data;
	this->fillDBdata(data);
	CGuildMgr::getSingleton().updateGuild(data);
	sendGuildLog(pUser,"升级建筑","商店建筑");
	Athena::logger->trace("公会成员(id:%d,name:%s)升级了公会(id:%d,name:%s)商店建筑的等级到了:%d",pUser->getID(),pUser->getName(),getID(),getName(),m_shopLv);
}

void CGuild::levelupBlackMarket(Player * pUser)
{
	if (!pUser){
		return ;
	}

	GuildProto::stLevelUpBlackMarketLvResult retCmd;
	if (!checkMemberRight(pUser->getID(),CGuildConfig::eRight_Lvup)){
		retCmd.set_result(GuildProto::stLevelUpBlackMarketLvResult::eHAS_NO_RIGHT);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	CGuildConfig::stBlackMarketInfo * pBlackMarketInfo = CGuildConfig::getSingleton().getBlackMarketInfo(m_blackMarketLv);

	if (!pBlackMarketInfo){
		retCmd.set_result(GuildProto::stLevelUpBlackMarketLvResult::eINNER_ERROR);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (m_blackMarketLv >= m_hallLv){//等级不能高于大厅等级
		retCmd.set_result(GuildProto::stLevelUpBlackMarketLvResult::eHALL_LV_TOO_LOW);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (!removeContribute(pBlackMarketInfo->m_construction,"升级黑市")){
		retCmd.set_result(GuildProto::stLevelUpBlackMarketLvResult::eCONTRIBUTE_NOT_ENOUGH);
		pUser->sendProtoToMe(retCmd);
		return ;
	}


	m_blackMarketLv += 1;

	GuildProto::stSynBlackMarketBuildingLv synCmd;
	synCmd.set_lv(m_blackMarketLv);
	//pUser->sendProtoToMe(synCmd);
	sendProto2Mem(synCmd);

	retCmd.set_result(GuildProto::stLevelUpBlackMarketLvResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);

	addTrace(GuildProto::eTrace_BlackMarket_Levelup,pUser->getID(),pUser->getName());

	stGuildDB data;
	this->fillDBdata(data);
	CGuildMgr::getSingleton().updateGuild(data);
	sendGuildLog(pUser,"升级建筑","黑市建筑");
	Athena::logger->trace("公会成员(id:%d,name:%s)升级了公会(id:%d,name:%s)黑市建筑的等级到了:%d",pUser->getID(),pUser->getName(),getID(),getName(),m_blackMarketLv);
	
}

void CGuild::addTrace(GuildProto::eTraceID eID,UINT32 charID,std::string charName)
{
	stTraceInfo traceInfo;
	traceInfo.m_charID = charID;
	traceInfo.m_eID = eID;
	traceInfo.m_charName = charName;
	traceInfo.m_doTime = g_globalServer->getSec();

	m_traces.push_back(traceInfo);

	if (m_traces.size() > s_MAX_TRACE_LEN){
		m_traces.erase(m_traces.begin());
	}

	GuildProto::stAddNewGuildTrace retCmd;
	retCmd.mutable_traceinfo()->set_etraceid(eID);
	retCmd.mutable_traceinfo()->set_dotime(traceInfo.m_doTime);
	retCmd.mutable_traceinfo()->set_name(charName);
	this->sendProto2Mem(retCmd,0);

	stTraceInfoDB dbData;
	dbData.m_doTime = traceInfo.m_doTime;
	dbData.m_guildID = getID();
	dbData.m_traceID = eID;
	strncpy(dbData.m_charName,charName.c_str(),sizeof(dbData.m_charName));
	CGuildMgr::getSingleton().addTraceInfoRecord(dbData);
}

void CGuild::sendTraceList(Player * pUser)
{
	if (!pUser){
		return ;
	}

    UINT32 traceNum = 0;
	GuildProto::stRetGuildTrace retCmd;
	std::list<stTraceInfo>::reverse_iterator it = m_traces.rbegin();
	for (;it != m_traces.rend();++it){
		stTraceInfo & traceInfo = *it;
		GuildProto::stGuildTraceInfo * pSyn = retCmd.add_traces();
		if (pSyn){
            traceNum++;
			pSyn->set_charid(traceInfo.m_charID);
			pSyn->set_etraceid(traceInfo.m_eID);
			pSyn->set_dotime(traceInfo.m_doTime);
			pSyn->set_name(traceInfo.m_charName);
		}
        if (traceNum >= s_MAX_TRACE_LEN) {
            Athena::logger->trace("[sendTraceList] len more than 100, break");
            break;
        }
	}
	pUser->sendProtoToMe(retCmd);
}

void CGuild::pray(Player * pUser)
{
	if (!pUser){
		return ;
	}

	GuildProto::stPrayResult retCmd;
	CGuildConfig::stPrayInfo *pPrayInfo = CGuildConfig::getSingleton().getPrayInfo(m_prayBuildingLv);
	if (!pPrayInfo){
		retCmd.set_result(GuildProto::stPrayResult::eCANNT_PRAY);
		pUser->sendProtoToMe(retCmd);
		return ;
	}	

	if (m_payNum >= pPrayInfo->m_maxPrayNum){
		retCmd.set_result(GuildProto::stPrayResult::ePRAY_NUM_OUT);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	Global::stGuildPray cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_buildinglv(m_prayBuildingLv);
	cmd.set_extraboxid(CManorInfoMgr::getSingleton().getOwnerPrayBoxID(getID()));
	pUser->sendProto2Game(cmd);

	sendGuildLog(pUser,"祈祷","");

	Athena::logger->trace("公会成员(id:%d,name:%s)在公会(id:%d,name:%s)进行了祈福",pUser->getID(),pUser->getName(),getID(),getName());
}

void CGuild::timer(UINT32 cur)
{
	bool needSave = false;
	if (needSave){
		stGuildDB dbData;
		fillDBdata(dbData);
		CGuildMgr::getSingleton().updateGuild(dbData);
	}

	struct tm curTm;
	RealTime::getLocalTime(curTm,cur);
	if ((curTm.tm_hour == 0)  
			&&(curTm.tm_min == 0)
			&&(curTm.tm_sec < 5) ){
		if (!m_refresh){
			m_refresh = true;	
			TIDMap_IT it = m_idMap.begin();
			for (;it != m_idMap.end();++it){
				CGuildMember * pGuildMem = (CGuildMember *)it->second;
				pGuildMem->reset();
			}
		}
	}
	else {
		m_refresh = false;
	}

	transferGuild();

	//零点清零
	struct tm _curTm;
	RealTime::getLocalTime(_curTm,g_globalServer->getSec());
	/*tm_wday  星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
	if(_curTm.tm_wday == 1){
		if ((_curTm.tm_hour == 0) && (_curTm.tm_min == 0) && (_curTm.tm_sec < 10) && (!m_zeroRefresh)){
			m_zeroRefresh  = true;
			m_guildCopyMapInfos.clear();
			CGuildCopyMapMgr::getSingleton().removeRecord();
		}
		else if (_curTm.tm_sec > 10){
			m_zeroRefresh  = false;
		}
	}
}

bool SortVecByTitle(const CGuildMember* first, const CGuildMember* second)
{
	return first->getTitle() > second->getTitle();
}

bool SortVecByContribute(const CGuildMember* first, const CGuildMember* second)
{
	return first->getContribute() > second->getContribute();
}

void CGuild::transferGuild()
{
	std::vector<UINT32>  charIDVec;
	//获取工会所有成员ID
	getAllMember(charIDVec);
	//工会只有一人，不能转让
	if(1 >= charIDVec.size())
	{
		return;
	}

	//将除会长以外且7日内上过线的成员添加到列表，以便条件查找
	std::vector<CGuildMember*> guildMemberVec;

	std::vector<UINT32>::iterator it = charIDVec.begin();
	for(; it != charIDVec.end(); ++it){
		//根据ID获得成员
		CGuildMember * pGuildMember = getGuildMemberByID(*it);
		if(!pGuildMember){
			return ;
		}
		//7天604800秒
		if(isLeader(*it)){
			//小于7天不处理
			if(g_globalServer->getSec() - pGuildMember->getLastLoginTime() < 604800){
				return;
			}
			continue;
		}
		if(g_globalServer->getSec() - pGuildMember->getLastLoginTime() < 604800)
		{
			guildMemberVec.push_back(pGuildMember);
		}
	}

	if(0 == guildMemberVec.size())
	{
		return;
	}

	//按头衔从大到小排序
	std::sort(guildMemberVec.begin(), guildMemberVec.end(), SortVecByTitle);

	std::vector<CGuildMember*>::iterator iter = guildMemberVec.begin();
	//存放头衔最高的成员
	std::vector<CGuildMember*> titleMemberVec;

	UINT8  title  = (*iter)->getTitle();
	for(; iter != guildMemberVec.end(); ++iter){
		if(title == (*iter)->getTitle()){
			titleMemberVec.push_back(*iter);
		}
	}

	//将头衔最高成员按贡献度排序
	std::sort(titleMemberVec.begin(), titleMemberVec.end(), SortVecByContribute);

	iter =  titleMemberVec.begin();

	UINT32 contribute = (*iter)->getContribute();

	guildMemberVec.clear();
	//添加头衔最高，且贡献度相同的成员
	for(; iter != titleMemberVec.end(); ++iter){
		if(contribute == (*iter)->getContribute()){
			guildMemberVec.push_back(*iter);
		}
	}
	//头衔最高，贡献相同，取随机数
	UINT32 vecSize = guildMemberVec.size();
	UINT32 randNum = rand()%vecSize;
	UINT32 charID = guildMemberVec[randNum]->getID();

	CGuildMember * pLeaderMem = getGuildMemberByID(m_leaderID);
	CGuildMember * pGuildMem = getGuildMemberByID(charID);
	m_leaderID = charID;
	
	if(!pLeaderMem || !pGuildMem){
		return;
	}	
	pLeaderMem->m_title = GuildProto::GUILD_TITLE_NORMAL;
	pGuildMem->m_title = GuildProto::GUILD_TITLE_LEADER;
	GuildProto::stGiveLeader2Other giveLeaderCmd;
	giveLeaderCmd.set_charid(charID);
	sendProto2Mem(giveLeaderCmd);
	stGuildMemberDB dataDB;
	pGuildMem->fillDBdata(dataDB);
	CGuildMgr::getSingleton().updateGuildMem(dataDB);
	pLeaderMem->fillDBdata(dataDB);
	CGuildMgr::getSingleton().updateGuildMem(dataDB);

	stGuildDB guildData;
	this->fillDBdata(guildData);
	CGuildMgr::getSingleton().updateGuild(guildData);


	//工会内邮件通知
	stBatchSendMailTask task;   
	char szMailContent[50] = { 0 };
	task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_LEADER);
	snprintf(szMailContent,sizeof(szMailContent),"MailWnd_23;%s",pGuildMem->getName());
	task.m_content = szMailContent;
	for (UINT16 i = 0; i < charIDVec.size(); ++i){
		task.m_receiveIDs.insert(charIDVec[i]);
	}
	CBatchSendMailMgr::getSingleton().addTask(task);

	Athena::logger->trace("公会会长(id:%d,name:%s,lastLoginTime:%d)自动把转让给了玩家:%d",getID(),getName(),pLeaderMem->getLastLoginTime(),charID);
}

void CGuild::synAllMenber()
{
	std::vector<UINT32> charIDVec;
	getAllMember(charIDVec);
	std::vector<UINT32>::iterator it = charIDVec.begin();
	for(; it != charIDVec.end(); ++it){
		Player * pUser = PlayerManager::getSingleton().getUserByID(*it);  
		if(pUser){
			this->sendGuildInfo(pUser);
		}
	}
}

void CGuild::collectDamageRewardChar(std::map<UINT16,stBatchSendMailTask> & task)
{
	std::vector<stMemberDamageInfo> vec;
	UINT16 num = m_damageSortList.getListNum();
	m_damageSortList.getFromN12N2(vec,1,num);
	for (UINT16 i = 0;i < num;++i){
		UINT16 rank = i + 1;
		stMemberDamageInfo & info = vec[i];
		std::map<UINT16,stBatchSendMailTask>::iterator it = task.find(rank);
		if (it == task.end()){
			stBatchSendMailTask & one = task[rank];
			char szMailContent[50] = { 0 };
			snprintf(szMailContent,sizeof(szMailContent),"MailWnd_12;%d",i + 1);
			one.m_content = szMailContent;
			one.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_GUILD_COPYMAP_SORTLIST);
			//one.m_attaches.push_back(std::make_pair());
			one.m_receiveIDs.insert(info.m_charID);
		}
		else {
			it->second.m_receiveIDs.insert(info.m_charID);
		}
	}
	m_damageSortList.clear();				
}

void CGuild::addMember2BatchSendTask(stBatchSendMailTask & task)
{
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CGuildMember * pGuildMem = (CGuildMember *)it->second;
		task.m_receiveIDs.insert(pGuildMem->getID());
	}
}

void CGuild::getAllMember(std::vector<UINT32> & charIDVec)
{
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CGuildMember * pGuildMem = (CGuildMember *)it->second;
		charIDVec.push_back(pGuildMem->getID());
	}
}

void CGuild::sendGuildLog(Player * pUser,const char * szAction,const char * szOpObj)
{	
	if (!pUser || !szAction || !szOpObj){
		return ;
	}

	Global::stSendGuildLog webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_name(pUser->getName());
	webCmd.set_charid(pUser->getID());
	webCmd.set_action(szAction);
	webCmd.set_opobjid(szOpObj);
	webCmd.set_guildid(getID());
	webCmd.set_level(pUser->m_level);
	webCmd.set_viplv(pUser->m_vipLv);
	webCmd.set_opid(pUser->m_opID);
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void CGuild::replyApplier(Player * pUser,UINT32 otherID,bool agree)
{
	if (!pUser)	{
		return ;
	}

	CGuildMember * pReplyMember = getGuildMemberByID(pUser->getID());
	if (!pReplyMember){
		return ;
	}

	GuildProto::stJoinInGuildTip tipCmd;
	GuildProto::stReplyApplerError errorRet;

	if (!CGuildConfig::getSingleton().hasRight(pReplyMember->m_title,CGuildConfig::eRight_Audit)){//没有权限 
		Athena::logger->error("试图审核成员不成功,没有权限!");
		errorRet.set_result(GuildProto::REPLY_ERROR_NO_POWER);
		pUser->sendProtoToMe(errorRet);
		return ;
	}

	if (0 == otherID){//所有人都拒绝
		m_applyList.clear();
		tipCmd.set_accept(false);
		pUser->sendProtoToMe(tipCmd);
		return ;
	}

	bool find = false;
	std::list<stApplyJoinInInfo>::iterator it =  m_applyList.begin();
	for (;it != m_applyList.end(); ++it){//查找是否在列表中
		if ((*it).m_charID == otherID){
			find = true;
			break;
		}	
	}

	if (!find){//找不到
		errorRet.set_result(GuildProto::REPLY_ERROR_NO_IN_LIST);
		pUser->sendProtoToMe(errorRet);
		return ;
	}

	if (size() >= getMaxMember()){
		errorRet.set_result(GuildProto::REPLY_ERROR_FULL);
		pUser->sendProtoToMe(errorRet);
		m_applyList.clear();
		return ;
	}

	if (it == m_applyList.end()){
		return ;
	}

	tipCmd.set_othername((*it).m_name);
	if (!agree){
		tipCmd.set_accept(false);
		pUser->sendProtoToMe(tipCmd);
		m_applyList.erase(it);
		return ;
	}

	m_applyList.erase(it);

	CGuildMember * pMember = ATHENA_NEW CGuildMember();
	stGuildMemberDB dataDB;
	dataDB.m_guildID = this->getID();
	dataDB.m_charID = otherID;
	dataDB.m_contribute = 0;
	dataDB.m_job = (*it).m_job;
	//dataDB.m_level = (*it).m_level;
	dataDB.m_sex = (*it).m_sex;
	dataDB.m_photoID = (*it).m_photoID;
	strncpy(dataDB.m_name,(*it).m_name,sizeof(dataDB.m_name));
	dataDB.m_title = GuildProto::GUILD_TITLE_NORMAL;
	dataDB.m_joinInTime = g_globalServer->getSec();
	dataDB.m_lastLoginTime  = g_globalServer->getSec();

	pMember->initFromDB(dataDB);
	if (!addMember(pMember)){
		Athena::logger->trace("公会%s同意%s,%d加入时候,往容器里添加成员时候失败!",getName(),pMember->getName(),pMember->getID());
		SAFE_DELETE(pMember);
		return ;
	}
	tipCmd.set_accept(true);
	pUser->sendProtoToMe(tipCmd);

	Player * pOther = PlayerManager::getSingleton().getUserByID(pMember->getID());
	if (pOther){
		pOther->sendProtoToMe(tipCmd);
		this->sendGuildInfo(pOther);
		sendGuildLog(pOther,"加入公会","");
	}

	CGuildMgr::getSingleton().createGuildMem(dataDB);
	GuildProto::stMemJoinInGuild joinInCmd;
	GuildProto::stSynGuildMemInfo *pSynMemInfo = joinInCmd.mutable_data();
	pMember->fillSynData(*pSynMemInfo);
	sendProto2Mem(joinInCmd,pMember->getID());
	CGuildMgr::getSingleton().addCharID2GuildID(pMember->getID(),getID());

	addTrace(GuildProto::eTrace_Enter_Guild,dataDB.m_charID,dataDB.m_name);
}

bool CGuild::addMember(CGuildMember * pMember)
{
	if (!pMember || !pMember->getID() || !pMember->getName()){
		return false;
	}
	return addElement(pMember);
}

bool CGuild::removeMember(CGuildMember * pMember)
{
	if (!pMember){
		return false;
	}
	removeElement(pMember);
	return true;
}

void CGuild::updateMemberDamage(UINT32 charID,UINT32 damage)
{

	CGuildMember *pMember = getGuildMemberByID(charID);
	if (!pMember){
		return ;
	}

	//先冒泡排下序
	static UINT32 s_damage[3] = { 0 };
	bzero(s_damage,sizeof(s_damage));
	s_damage[0] = pMember->getDamage1();
	s_damage[1] = pMember->getDamage2();
	s_damage[2] = pMember->getDamage3();

	if ((damage <= s_damage[0]) && (damage <= s_damage[1]) && (damage <= s_damage[2])){//伤害血量最小
		return ;
	}

	if (s_damage[0] < s_damage[1]){
		std::swap(s_damage[0],s_damage[1]);
	}

	if (s_damage[1] < s_damage[2]){
		std::swap(s_damage[1],s_damage[2]);
	}

	if (s_damage[0] < s_damage[1]){
		std::swap(s_damage[0],s_damage[1]);
	}

	if (s_damage[2] > damage){//比最小的还小不能进入队列
		return ;
	}
	s_damage[2] = damage;
	pMember->setDamage1(s_damage[0]);
	pMember->setDamage2(s_damage[1]);
	pMember->setDamage3(s_damage[2]);
	UINT32 totalDamage = s_damage[0] + s_damage[1] + s_damage[2];

	if (m_damageSortList.isInSortList(charID)){
		stMemberDamageInfo info = m_damageSortList.getSortInfo(charID); 
		if (info.m_damage < totalDamage){
			info.m_damage = totalDamage;
			m_damageSortList.updateSortInfo(info);
		}   
	}   
	else {
		stMemberDamageInfo info;
		info.m_charID = charID;
		info.m_damage = damage;
		m_damageSortList.insertSortInfo(info);
	}

	stGuildMemberDB dataDB;
	pMember->fillDBdata(dataDB);
	CGuildMgr::getSingleton().updateGuildMem(dataDB);
}

void CGuild::addActivityDegree(UINT32 degree,const char * desc)
{
	if (!desc){
		return;
	}
	UINT32 oldVal = m_activityDegree;
	m_activityDegree += degree;
	if (m_activityDegree > CGuildConfig::getSingleton().getGuildActivtyLimit()){
		m_activityDegree = CGuildConfig::getSingleton().getGuildActivtyLimit();
	}

	GuildProto::stSynGuildActivityDegree retCmd;
	retCmd.set_value(m_activityDegree);
	sendProto2Mem(retCmd);

	stGuildDB guildData;
	this->fillDBdata(guildData);
	CGuildMgr::getSingleton().updateGuild(guildData);

	Global::sstSendGuildResLog webCmd;
	webCmd.set_guildid(getID());
	webCmd.set_reason(desc);
	webCmd.set_res("活跃度");
	webCmd.set_newval(m_activityDegree);
	webCmd.set_oldval(oldVal);
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

	Athena::logger->trace("公会(id:%d,name:%s)因为%s增加了%d活跃度",getID(),getName(),desc,degree);
}

bool CGuild::removeActivityDegree(UINT32 degree,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_activityDegree < degree){
		return false;
	}
	UINT32 oldVal = m_activityDegree;
	m_activityDegree -= degree;
	GuildProto::stSynGuildActivityDegree retCmd;
	retCmd.set_value(m_activityDegree);
	sendProto2Mem(retCmd);

	stGuildDB guildData;
	this->fillDBdata(guildData);
	CGuildMgr::getSingleton().updateGuild(guildData);

	Global::sstSendGuildResLog webCmd;
	webCmd.set_guildid(getID());
	webCmd.set_reason(desc);
	webCmd.set_res("活跃度");
	webCmd.set_newval(m_activityDegree);
	webCmd.set_oldval(oldVal);
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

	Athena::logger->trace("公会(%d,name:%s)因为%s扣除了%d活跃度",getID(),getName(),desc,degree);
	return true;
}

void CGuild::sendDamageSortlist(Player * pUser,UINT16 beginRank)
{
	if (!pUser){
		return ;
	}	
	GuildProto::stRetDamageSortlist retCmd;
	retCmd.set_beginrank(beginRank);
	std::vector<stMemberDamageInfo> vec;
	m_damageSortList.getFromN12N2(vec,beginRank,beginRank + 10);
	retCmd.set_myrank(m_damageSortList.getMySortRank(pUser->getID()));
	for (UINT16 i = 0;i < vec.size();++i){
		stMemberDamageInfo & info = vec[i];
		GuildProto::stDamageSortItemInfo * pSyn = retCmd.add_sortitem();
		if (pSyn){
			pSyn->set_charid(info.m_charID);
			pSyn->set_damage(info.m_damage);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CGuild::sendCopymapWin(Player * pUser)
{
	if (!pUser){
		return ;
	}

	CGuildMember *pMember =  getGuildMemberByID(pUser->getID());
	if (!pMember){
		return ;
	}

	GuildProto::stRetGuildCopymapInfo retCmd;
	retCmd.set_challengenum(pMember->getChallengeNum());
	retCmd.set_assistnum(pMember->getAssistNum());

	std::map<UINT32,stGuildCopyMapInfo>::iterator it =  m_guildCopyMapInfos.begin();
	for (;it != m_guildCopyMapInfos.end();++it){
		stGuildCopyMapInfo & info = it->second;
		GuildProto::stCopymapOpenInfo * pSyn = retCmd.add_openinfo();
		if (pSyn){
			pSyn->set_copymapid(info.m_copyMapID);
			pSyn->set_opentime(info.m_openTime);
			pSyn->set_stepid(info.m_stepID);
			if (info.m_openTime > 0 ){
				pSyn->set_isstarted(true);
			}
			else {
				pSyn->set_isstarted(false);
			}
			pSyn->set_killerid(info.m_killCharID);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CGuild::startCopyMap(Player * pUser,UINT32 copymapID)
{
	if (!pUser){
		return ;
	}

	CGuildMember *pMember =  getGuildMemberByID(pUser->getID());
	if (!pMember){
		return ;
	}

	GuildProto::stStartGuildCopymapResult retCmd;
	retCmd.set_copymapid(copymapID);

	if ((pMember->m_title == GuildProto::GUILD_TITLE_LEADER) || (pMember->m_title == GuildProto::GUILD_TILE_VICELEADER)){
		std::map<UINT32,stGuildCopyMapInfo>::iterator it =  m_guildCopyMapInfos.find(copymapID);
		bool bInsert = false;
		UINT16 oldStep  = 0;
		if (it != m_guildCopyMapInfos.end()){
			if (it->second.m_openTime > 0){//已经开启了
				retCmd.set_result(GuildProto::stStartGuildCopymapResult::eHas_Open);
				pUser->sendProtoToMe(retCmd);
				return ;
			}	
			oldStep = it->second.m_stepID;
		}
		else {
			bInsert = true;
		}

		if (m_hallLv < CGuildConfig::getSingleton().getCopyMapOpenLv(copymapID)){//检查开启等级
			retCmd.set_result(GuildProto::stStartGuildCopymapResult::eLevel_Not_Meet);
			pUser->sendProtoToMe(retCmd);
			return ;
		}
		UINT32 needActivity =  CGuildConfig::getSingleton().getStartCopymapCost(copymapID + oldStep);

		if (m_activityDegree < needActivity){//检查活跃度
			retCmd.set_result(GuildProto::stStartGuildCopymapResult::eNot_Enough_Activity);
			pUser->sendProtoToMe(retCmd);
			return ;
		}

		this->removeActivityDegree(needActivity,"开启副本");

		stGuildCopyMapInfo & info = m_guildCopyMapInfos[copymapID];	
		info.m_copyMapID = copymapID;
		info.m_openTime = g_globalServer->getSec();
		info.m_stepID = ++info.m_stepID;
		info.m_guildID  = getID();
		UINT32 bossID = CGuildConfig::getSingleton().getBossIDByLevelID(copymapID + info.m_stepID - 1);
		stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(bossID,info.m_stepID));
		if (pMonsterBase){
			info.m_bossHP = pMonsterBase->m_hp;
		}
		retCmd.set_result(GuildProto::stStartGuildCopymapResult::eSuccess);
		retCmd.set_starttime(info.m_openTime);
		pUser->sendProtoToMe(retCmd);

		stGuildCopyMapDB dbData;
		info.fillDBInfo(dbData);
		if (bInsert){
			CGuildCopyMapMgr::getSingleton().addRecord(dbData);
		}
		else {
			CGuildCopyMapMgr::getSingleton().updateRecord(dbData);	
		}
		char szObjID[20] = { 0 };
		snprintf(szObjID,sizeof(szObjID),"%d",copymapID);
		sendGuildLog(pUser,"开启副本",szObjID);
	}
	else {
		retCmd.set_result(GuildProto::stStartGuildCopymapResult::eHas_No_Right);
		pUser->sendProtoToMe(retCmd);
	}
}

void CGuild::enterCopyMap(Player * pUser,UINT32 copymapID)
{
	GuildProto::stRetEnterGuildCopymapResult retCmd;
	std::map<UINT32,stGuildCopyMapInfo>::iterator it =  m_guildCopyMapInfos.find(copymapID);
	if ((it == m_guildCopyMapInfos.end()) || it->second.m_openTime <= 0){
		retCmd.set_result(GuildProto::stRetEnterGuildCopymapResult::eHas_Not_Start);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByCharID(pUser->getID());
	if (!pTeam){//还没有组队
		retCmd.set_result(GuildProto::stRetEnterGuildCopymapResult::eHas_No_Team);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (pTeam->getLeaderID() != pUser->getID()){
		retCmd.set_result(GuildProto::stRetEnterGuildCopymapResult::eNot_Leader);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	Global::stTeamEnterCopyMap cmd;
	cmd.set_copymapid(copymapID + it->second.m_stepID - 1);
	cmd.set_teamid(pTeam->getID());
	cmd.set_bosshp(it->second.m_bossHP);
	cmd.set_bosslv(it->second.m_stepID);

	char szObjID[20] = { 0 };
	snprintf(szObjID,sizeof(szObjID),"%d",cmd.copymapid());


	UINT32 charID = pTeam->getCharID(0);
	if (charID > 0){
		CGuildMember *pMember =  getGuildMemberByID(charID);
		if (pMember){
			if (pMember->getChallengeNum() < CGuildConfig::getSingleton().getMaxChallengeNum()){
				Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
				if (pMemInfo){
					pMemInfo->set_charid(charID);
				}
				pMember->addChallengeNum(1);

				Player * pLogUser = PlayerManager::getSingleton().getUserByID(charID);
				if (pLogUser){
					sendGuildLog(pLogUser,"进入组队副本",szObjID);
				}
			}
		}
	}

	charID = pTeam->getCharID(1);
	if (charID > 0){
		CGuildMember *pMember =  getGuildMemberByID(charID);
		if (pMember){
			bool bCanEnter = false;
			if (pMember->getChallengeNum() < CGuildConfig::getSingleton().getMaxChallengeNum()){
				pMember->addChallengeNum(1);
				bCanEnter = true;

			}
			else if (pMember->getAssistNum() < CGuildConfig::getSingleton().getMaxAssistNum()){
				pMember->addAssistNum(1);
				bCanEnter = true;
			}

			if (bCanEnter){
				Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
				if (pMemInfo){
					pMemInfo->set_charid(charID);
				}

				Player * pLogUser = PlayerManager::getSingleton().getUserByID(charID);
				if (pLogUser){
					sendGuildLog(pLogUser,"进入组队副本",szObjID);
				}
			}
		}
	}

	charID = pTeam->getCharID(2);
	if (charID > 0){
		CGuildMember *pMember =  getGuildMemberByID(charID);
		if (pMember){
			bool bCanEnter = false;
			if (pMember->getChallengeNum() < CGuildConfig::getSingleton().getMaxChallengeNum()){
				pMember->addChallengeNum(1);
				bCanEnter = true;

			}
			else if (pMember->getAssistNum() < CGuildConfig::getSingleton().getMaxAssistNum()){
				pMember->addAssistNum(1);
				bCanEnter = true;
			}

			if (bCanEnter){
				Global::stEnterCopymapMemInfo * pMemInfo = cmd.add_memerinfo();
				if (pMemInfo){
					pMemInfo->set_charid(charID);
				}

				Player * pLogUser = PlayerManager::getSingleton().getUserByID(charID);
				if (pLogUser){
					sendGuildLog(pLogUser,"进入组队副本",szObjID);
				}
			}
		}
	}

	if (cmd.memerinfo_size() > 0){
		g_globalServer->getConnMgr().broadcastByID(pUser->getGameID(),cmd);
	}
	else {
		Athena::logger->trace("[进入组队公会副本]失败,所有成员的挑战次数都完了");
	}
	CTeamMgr::getSingleton().fireTeam(pUser);
}

UINT32 CGuild::getGuildBossHP(UINT32 copymapID)
{
	UINT32 realCopymapID = getRealCopyMapID(copymapID);
	std::map<UINT32,stGuildCopyMapInfo>::iterator it =  m_guildCopyMapInfos.find(realCopymapID);	
	if (it != m_guildCopyMapInfos.end()){
		return it->second.m_bossHP;
	}
	return 0;
}

void CGuild::killGuildBoss(UINT32 copymapID,UINT32 killerID)
{
	UINT32 realCopymapID = getRealCopyMapID(copymapID);
	std::map<UINT32,stGuildCopyMapInfo>::iterator it =  m_guildCopyMapInfos.find(realCopymapID);	
	if (it != m_guildCopyMapInfos.end()){
		it->second.m_openTime = 0;
		it->second.m_killCharID = killerID;
		it->second.m_bossHP = 0;

		/*UINT32 bossID = CGuildConfig::getSingleton().getBossIDByLevelID(realCopymapID + it->second.m_stepID - 1);
		  stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(bossID,it->second.m_stepID));
		  if (pMonsterBase){
		  it->second.m_bossHP = pMonsterBase->m_hp;
		  }*/

		stGuildCopyMapDB dbData;
		it->second.fillDBInfo(dbData);
		CGuildCopyMapMgr::getSingleton().updateRecord(dbData);	
	}
}

UINT16	 CGuild::getGuildBossStepID(UINT32 copymapID)
{
	UINT32 realCopymapID = getRealCopyMapID(copymapID);
	std::map<UINT32,stGuildCopyMapInfo>::iterator it =  m_guildCopyMapInfos.find(realCopymapID);	
	if (it != m_guildCopyMapInfos.end()){
		return it->second.m_stepID;
	}
	return 1;
}

void CGuild::updateGuildBossHP(UINT32 copymapID,UINT32 curHP)
{
	UINT32 realCopymapID = getRealCopyMapID(copymapID);
	std::map<UINT32,stGuildCopyMapInfo>::iterator it =  m_guildCopyMapInfos.find(realCopymapID);	
	if (it != m_guildCopyMapInfos.end()){

		if (it->second.m_bossHP > curHP){
			it->second.m_bossHP -= curHP;
		}
		else {
			it->second.m_bossHP = 0;
		}

		if (it->second.m_bossHP <= 0){
			UINT32 bossID = CGuildConfig::getSingleton().getBossIDByLevelID(it->second.m_copyMapID + it->second.m_stepID - 1);
			stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(bossID,it->second.m_stepID));
			if (pMonsterBase){
				it->second.m_bossHP = (UINT32)(pMonsterBase->m_hp * 0.01);
			}
		}

		stGuildCopyMapDB dbData;
		it->second.fillDBInfo(dbData);
		CGuildCopyMapMgr::getSingleton().updateRecord(dbData);	
	}
}

void CGuild::loadMember()
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "GUILDID=%d", getID());
	CTblQueryResult* result = g_databaseEngine.select(g_guildMemberTblDef,"GUILD_MEMBER",where,NULL,0,NULL);	
	if (!result){
		return ;
	}

	if (result->getData()){
		stGuildMemberDB  * pMemberDB = (stGuildMemberDB  *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			CGuildMember * pMember = ATHENA_NEW CGuildMember();
			pMember->initFromDB(pMemberDB[i]);
			if (!addMember(pMember)){
				Athena::logger->error("公会name=%s加载成员charID=%d,name=%s,添加到容器失败!",getName(),pMember->getID(),pMember->getName());
				SAFE_DELETE(pMember);
				continue;
			}
			UINT32 damage = pMemberDB[i].m_damage1 + pMemberDB[i].m_damage2 + pMemberDB[i].m_damage3;
			if (damage > 0){
				stMemberDamageInfo sortInfo;
				sortInfo.m_charID = pMemberDB[i].m_charID;
				sortInfo.m_damage = damage;
				m_damageSortList.insertSortInfo(sortInfo);
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("加载了公会%s的成员id=%d,name=%s,joinInTime=%d,lastLoginTime=%d",getName(),pMember->getID(),pMember->getName(),pMember->m_joinInTime,pMember->m_lastLoginTime);
#endif
			CGuildMgr::getSingleton().addCharID2GuildID(pMember->getID(),getID());
#ifdef _HDZ_DEBUG
			Athena::logger->trace("公会(name:%s,charID:%d)的排名信息有:%d条",getName(),getID(),m_damageSortList.getListNum());
#endif
		}
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了:%d个伤害列表",m_damageSortList.getListNum());
#endif
#ifdef _HDZ_DEBUG
	Athena::logger->trace("公会name=%s加载了%d个成员",getName(),size());
#endif

	bzero(where, 65);
	snprintf(where, 64, "GUILDID=%d", getID());
	char orderBy[65] = {0};
	snprintf(orderBy,64," DOTIME ASC ");
	result = g_databaseEngine.select(g_guildTraceDef,"GUILD_TRACE",where,NULL,0,NULL);	
	if (result){
		if (result->getData()){
			stTraceInfoDB * pTraceDB = (stTraceInfoDB *)result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stTraceInfo traceInfo;
				traceInfo.m_id = pTraceDB[i].m_id;
				traceInfo.m_eID = (GuildProto::eTraceID)pTraceDB[i].m_traceID;
				traceInfo.m_doTime = pTraceDB[i].m_doTime;
				traceInfo.m_charName = pTraceDB[i].m_charName;
				traceInfo.m_charID = pTraceDB[i].m_charID; 
				m_traces.push_back(traceInfo);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("公会name=%s 的跟踪记录 id=%d,charName=%s,doTime=%d",getName(),pTraceDB[i].m_traceID,pTraceDB[i].m_charName,pTraceDB[i].m_doTime);
#endif
			}
		}
		SAFE_DELETE(result);
	}

	bzero(where, 65);
	snprintf(where, 64, "GUILD_ID=%d", getID());
	result = g_databaseEngine.select(g_guildCopyMapDef,"GUILD_COPYMAP",where,NULL,0,NULL);	
	if (result){
		if (result->getData()){
			stGuildCopyMapDB * pCopyMapDB = (stGuildCopyMapDB *)result->getData();		
			for (UINT32 i = 0; i < result->getRowCount();++i){
				stGuildCopyMapInfo info;
				info.initFromDB(pCopyMapDB[i]);
				m_guildCopyMapInfos[info.m_copyMapID] = info;
			}
		}
		SAFE_DELETE(result);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印了公会副本信息:");
	std::map<UINT32,stGuildCopyMapInfo>::iterator it =  m_guildCopyMapInfos.begin();
	for (;it != m_guildCopyMapInfos.end();++it){
		stGuildCopyMapInfo & info = it->second;
		Athena::logger->trace("copyMapID:%d,openTime:%d,stepID:%d",info.m_copyMapID,info.m_openTime,info.m_stepID);	
	}
#endif

}

bool CGuild::checkMemberRight(UINT32 memberID,UINT32 right)
{
	CGuildMember *pMember =  getGuildMemberByID(memberID);	
	if (!pMember){
		return false;
	}
	return CGuildConfig::getSingleton().hasRight(pMember->getTitle(),right);
}

CGuildMgr::CGuildMgr() : ElementMgr(false,true,true,false)
{
	m_curMaxGuildID = 0;	
}

CGuildMgr::~CGuildMgr()
{

}

void CGuildMgr::loadAllGuild()
{
	CTblQueryResult* result = g_databaseEngine.select(g_guildTblDef,"GUILD",NULL,NULL,0,NULL);	
	if (!result){
		return ;
	}

	if (result->getData()){
		stGuildDB * pGuildDB = (stGuildDB *)result->getData();		
		for (UINT32 i = 0; i < result->getRowCount();++i){
			CGuild  * pGuild = ATHENA_NEW CGuild();
			pGuild->initFromDB(pGuildDB[i]);
			pGuild->loadMember();
			if (!addGuild(pGuild)){
				Athena::logger->error("加载公会时候往容器放id=%d,name=%s时候出现错误",pGuild->getID(),pGuild->getName());
				SAFE_DELETE(pGuild);
				continue;
			}
			stGuildRankInfo rankInfo;
			rankInfo.m_guildID = pGuild->getID();
			rankInfo.m_level = pGuild->getLevel(); 
			m_guildSortList.insertSortInfo(rankInfo);
			if (pGuild->getID() > m_curMaxGuildID){
				m_curMaxGuildID = pGuild->getID();
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("加载了公会id=%d,name=%s",pGuild->getID(),pGuild->getName());
#endif
		}
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("服务器启动一共加载了%d个公会,公会最大ID:%d",guildNum(),m_curMaxGuildID);
#endif

	result = g_databaseEngine.select(g_guildCopymapScoreRankTblDef,"GUILD_COPYMAP_SCORE_RANK",NULL,NULL,0,NULL);
	if (result){
		stGuildCopyMapRankDB * pDBData = (stGuildCopyMapRankDB *)result->getData();
		for (UINT32 i = 0; i < result->getRowCount();++i){
			stGuildCopymapScoreInfo info;
			info.m_guildID = pDBData[i].m_guildID;
			info.m_score = pDBData[i].m_score;
			m_copymapScoreList[pDBData[i].m_copyMapID].insertSortInfo(info);
		}
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印副本公会积分:");
	std::map<UINT32,GuildCopymapScoreList>::iterator it = m_copymapScoreList.begin();
	for (;it != m_copymapScoreList.end();++it){
		Athena::logger->trace("副本ID:%d,的积分列表数量:%d",it->first,it->second.getListNum());
	}
#endif
}

UINT32 CGuildMgr::guildNum()
{
	return ElementMgr::size();	
}

bool CGuildMgr::addGuild(CGuild * pGuild)
{
	if (!pGuild || !pGuild->getID() || !pGuild->getName()){
		return false;
	}	
	return addElement(pGuild);
}

bool CGuildMgr::removeGuild(CGuild * pGuild)
{
	if (!pGuild){
		return false;
	}
	removeElement(pGuild);
	return true;
}

CGuild * CGuildMgr::getGuildByID(UINT32 guildID)
{
	return (CGuild *)getElementByID(guildID);	
}

CGuild * CGuildMgr::getGuildByName(char * guildName)
{
	if (!guildName){
		return NULL;
	}
	return (CGuild *) getElementByName(guildName);
}

CGuild * CGuildMgr::getGuildByCharID(UINT32 charID)
{
	CharID2GuildID_IT it = m_charID2GuildID.find(charID);
	if (it != m_charID2GuildID.end()){
		return getGuildByID(m_charID2GuildID[charID]);	
	}
	return NULL;
}

bool CGuildMgr::addCharID2GuildID(UINT32 charID,UINT32 guildID)
{
	m_charID2GuildID[charID] = guildID;
	return true;
}

bool CGuildMgr::removeCharID2GuildID(UINT32 charID)
{
	m_charID2GuildID.erase(charID);
	return true;
}

void CGuildMgr::userOnline(Player * pUser)
{
	if (!pUser){
		return ;
	}	
	CGuild * pGuild = getGuildByCharID(pUser->getID());
	if (pGuild){
		pGuild->sendGuildInfo(pUser);
		Global::stSynGuildID   synGuildIDCmd;
		CGuildMember * pMember = pGuild->getGuildMemberByID(pUser->getID());
		if(pMember){
			pMember->m_lastLoginTime = g_globalServer->getSec();
			synGuildIDCmd.set_joinintime(pMember->getJoinInTime());
		}
		
		synGuildIDCmd.set_blackmarketlv(pGuild->getBlackMarketLv());
		synGuildIDCmd.set_charid(pUser->getID());
		synGuildIDCmd.set_guildid(pGuild->getID());
		synGuildIDCmd.set_guildlv(pGuild->getLevel());
		pUser->sendProto2Game(synGuildIDCmd);
		GuildProto::stGuildMemOnOffLine onlineCmd;
		onlineCmd.set_charid(pUser->getID());
		onlineCmd.set_online(true);
		pGuild->sendProto2Mem(onlineCmd);
	}
}

void CGuildMgr::userOffline(Player * pUser)
{
	if (!pUser){
		return ;
	}
	CGuild * pGuild = getGuildByCharID(pUser->getID());
	if (pGuild){
		GuildProto::stGuildMemOnOffLine onlineCmd;
		onlineCmd.set_charid(pUser->getID());
		onlineCmd.set_online(false);
		pGuild->sendProto2Mem(onlineCmd);
	}
}

void CGuildMgr::createGuildCB(bool opResult,UINT64 autoID,stGuildDB data)
{
	if (!opResult){
		Athena::logger->error("charID=%d创建公会时候插入数据库数据失败!",data.m_leaderID);
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(data.m_leaderID);
	if (!pUser){
		Athena::logger->error("charID=%d创建公会时候已经下线了!",data.m_leaderID);
		return ;
	}

	CGuild * pGuild = ATHENA_NEW CGuild();
	if (!pGuild){
		return ;	
	}
	pGuild->setID(autoID);
	pGuild->setName(data.m_name);
	data.m_id = autoID;
	pGuild->initFromDB(data);

	if (!addGuild(pGuild)){
		Athena::logger->error("id=%d,name=%s创建公会失败,容器里添加时候不成功!",pUser->getID(),pUser->getName());
		return ;
	}

	stGuildMemberDB memData;
	memData.m_guildID = pGuild->getID();
	memData.m_charID = pUser->getID();
	memData.m_contribute = 0;
	memData.m_job = pUser->m_job;
	memData.m_sex = pUser->m_sex;
	//memData.m_level = pUser->m_level;
	memData.m_title = GuildProto::GUILD_TITLE_LEADER;
	memData.m_joinInTime = g_globalServer->getSec();
	memData.m_lastLoginTime  = g_globalServer->getSec();
	strncpy(memData.m_name,pUser->getName(),sizeof(memData.m_name));
	memData.m_photoID = pUser->m_photoID;

	CGuildMember * pMember = ATHENA_NEW CGuildMember();
	pMember->initFromDB(memData);
	if (!pGuild->addMember(pMember)){
		Athena::logger->error("id=%d,name=%s创建公会失败,添加自己为公会成员时候不成功!",pUser->getID(),pUser->getName());
		SAFE_DELETE(pMember);
		return ;
	}

	pGuild->sendGuildInfo(pUser);
	createGuildMem(memData);
	addCharID2GuildID(pUser->getID(),pGuild->getID());

	stGuildRankInfo rankInfo;
	rankInfo.m_guildID = pGuild->getID();
	rankInfo.m_level = pGuild->getLevel();
	m_guildSortList.insertSortInfo(rankInfo);

	Global::stSynGuildID   synGuildIDCmd;
	synGuildIDCmd.set_charid(pUser->getID());
	synGuildIDCmd.set_guildid(pGuild->getID());
	synGuildIDCmd.set_guildlv(pGuild->getLevel());
	synGuildIDCmd.set_blackmarketlv(pGuild->getBlackMarketLv());
	if(pMember){
		synGuildIDCmd.set_joinintime(pMember->getJoinInTime());
	}

	pUser->sendProto2Game(synGuildIDCmd);

	Global::stCreateGuildDecMoney decMoneyCmd;
	decMoneyCmd.set_charid(pUser->getID());
	pUser->sendProto2Game(decMoneyCmd);

	pGuild->sendGuildLog(pUser,"创建公会","");
}

void CGuildMgr::createGuildMem(const stGuildMemberDB & data)
{
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::createGuildMemCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guildMemberTblDef,(const char *)&data,"GUILD_MEMBER",autoID,pDelaySql);

	Player * pUser = PlayerManager::getSingleton().getUserByID(data.m_charID);
	if (pUser) {
		Global::stSynGuildID   synGuildIDCmd;
		synGuildIDCmd.set_charid(pUser->getID());
		synGuildIDCmd.set_guildid(data.m_guildID);
		CGuild * pGuild = getGuildByID(data.m_guildID);
		if (pGuild) {
			synGuildIDCmd.set_guildlv(pGuild->getLevel());
			synGuildIDCmd.set_blackmarketlv(pGuild->getBlackMarketLv());
		}
		synGuildIDCmd.set_joinintime(data.m_joinInTime);
		pUser->sendProto2Game(synGuildIDCmd);
	}
}

void CGuildMgr::createGuildMemCB(bool opResult,UINT64 autoID)
{

}

void CGuildMgr::deleteGuildMem(UINT32 charID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%d",charID);
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::deleteGuildMemCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GUILD_MEMBER",where,pDelaySql);
}

void CGuildMgr::deleteGuildMemCB(bool opResult,UINT64 autoID)
{

}

void CGuildMgr::updateGuildMem(const stGuildMemberDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",data.m_charID);
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::updateGuildMemCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_guildMemberTblDef,(const char *)&data,"GUILD_MEMBER",where,pDelaySql);
}

void CGuildMgr::updateGuildMemCB(bool opResult,UINT64 autoID)
{}

void CGuildMgr::deleteGuild(UINT32 guildID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%d",guildID);
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::deleteGuildCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GUILD",where,pDelaySql);

	bzero(where, 65);
	snprintf(where, 64, "GUILDID=%u",guildID);
	pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::deleteGuildMemCB,false,0);
	pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GUILD_MEMBER",where,pDelaySql);
}

void CGuildMgr::deleteGuildCB(bool opResult,UINT64 autoID)
{}

void CGuildMgr::updateGuild(const stGuildDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%u",data.m_id);
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::updateGuildCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_guildTblDef,(const char *)&data,"GUILD",where,pDelaySql);
}

void CGuildMgr::updateGuildCB(bool opResult,UINT64 autoID)
{}

void CGuildMgr::fireGuild(UINT32 guildID)
{
	CGuild * pGuild = getGuildByID(guildID); 
	if (!pGuild){
		return ;
	}

	pGuild->setAllMemberLeaveTime();	

	if (!removeGuild(pGuild)){
		return;
	}

	GuildProto::stFireGuild fireCmd;
	pGuild->sendProto2Mem(fireCmd);
	deleteGuild(pGuild->getID());
	m_guildSortList.removeSortInfo(guildID);
	SAFE_DELETE(pGuild);
}

void CGuildMgr::timer(UINT32 cur)
{
	std::vector<UINT32> delVec;
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CGuild * pGuild = (CGuild *)it->second;
		pGuild->timer(cur);
		if ((pGuild->m_fireTime < cur) && pGuild->m_fireTime){
			delVec.push_back(pGuild->getID());
		}
	}

	for (UINT32 i = 0;i < delVec.size(); ++i){
		fireGuild(delVec[i]);	
	}

	struct tm curTm;
	RealTime::getLocalTime(curTm,g_globalServer->getSec());
	//重置
	UINT32 lastRefreshTime = CGlobalVarMgr::getSingleton().getValue(eGlobalVar_Refresh_Guild_Zero);
	if (!g_isTheSameDay(lastRefreshTime,g_globalServer->getSec())) {
		CGlobalVarMgr::getSingleton().setValue(eGlobalVar_Refresh_Guild_Zero,g_globalServer->getSec());	
		char where[65] = { 0 };
		bzero(where, 65);
		snprintf(where, 64, " `ID` > 0");
		stGuildMemberResetDB dbData;
		Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::resetMemberDBCB,false,0);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		g_databaseEngine.updateRecord(g_guildMemberResetTblDef,(const char *)&dbData,"GUILD_MEMBER",where,pDelaySql);

		//每日发伤害排行奖励
		std::map<UINT16,stBatchSendMailTask>  batchSendMail;
		it = m_idMap.begin();
		for (;it != m_idMap.end();++it){
			CGuild * pGuild = (CGuild *)it->second;
			pGuild->collectDamageRewardChar(batchSendMail);
		}
		std::map<UINT16,stBatchSendMailTask>::iterator subIt =  batchSendMail.begin();
		for (;subIt != batchSendMail.end();++subIt){
			UINT32 boxID = CGuildConfig::getSingleton().getRankReward(subIt->first);
			subIt->second.m_attaches.push_back(std::make_pair(boxID,1));
			CBatchSendMailMgr::getSingleton().addTask(subIt->second);
		}
	}

	lastRefreshTime = CGlobalVarMgr::getSingleton().getValue(eGlobalVar_Refresh_Guild_Score);
	if ((1 == curTm.tm_hour) &&
			((!g_isTheSameWeek(lastRefreshTime,cur)) && (curTm.tm_wday == 0))){//是周日零点
		CGlobalVarMgr::getSingleton().setValue(eGlobalVar_Refresh_Guild_Score,g_globalServer->getSec());
		std::map<UINT32,GuildCopymapScoreList>::iterator it =m_copymapScoreList.begin();
		for (;it != m_copymapScoreList.end();++it){
			std::map<UINT16,stBatchSendMailTask>  batchSendMail;
			UINT16 num = it->second.getListNum();
			std::vector<stGuildCopymapScoreInfo> vec;
			it->second.getFromN12N2(vec,1,num);

			for (UINT16 i = 0;i < vec.size();++i){
				stGuildCopymapScoreInfo & one = vec[i];
				UINT32 boxID = CGuildConfig::getSingleton().getScoreRankBoxID(it->first,i + 1);
				if (0 == boxID){
					continue;
				}

				CGuild * pGuild = getGuildByID(one.m_guildID); 
				if (pGuild){
					stBatchSendMailTask & task = batchSendMail[i + 1];
					task.m_content = "MailWnd_11";
					task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_GUILD_INTEGRAL_REWARD);
					task.m_attaches.push_back(std::make_pair(boxID,1));
					pGuild->addMember2BatchSendTask(task);
				}
			}
			std::map<UINT16,stBatchSendMailTask>::iterator subIt =  batchSendMail.begin();
			for (;subIt != batchSendMail.end();++subIt){
				CBatchSendMailMgr::getSingleton().addTask(subIt->second);
			}
		}
		deleteScoreRank();
		m_copymapScoreList.clear();
	}

	CManorStateMgr::getSingleton().timer(cur);
}


void CGuildMgr::resetMemberDBCB(bool opResult,UINT64 autoID)
{}

void CGuildMgr::sendGuildList(Player * pUser,UINT16 rank)
{
	if (!pUser){
		return ;
	}

	GuildProto::stRetGuild retCmd;
	retCmd.set_maxnum(m_guildSortList.getListNum());
	std::vector<stGuildRankInfo> vec;
	m_guildSortList.getFromN12N2(vec,rank,rank + 10);
	for (UINT16 i = 0;i < vec.size();++i){
		CGuild * pGuildMem = getGuildByID(vec[i].m_guildID);
		if (pGuildMem){
			GuildProto::stGuildListInfo * pSynInfo = retCmd.add_data();
			if (pSynInfo) {
				pGuildMem->fillGuildListInfo(*pSynInfo);
				pSynInfo->set_rank(rank + i);
				pSynInfo->set_hasapplied(pGuildMem->isInApplyList(pUser->getID()));
			}
		}

	}
	pUser->sendProtoToMe(retCmd);
}

void CGuildMgr::reqGuildDetail(Player * pUser,UINT32 guildID)
{
	if (!pUser){
		return ;
	}	
	CGuild * pGuildMem = getGuildByID(guildID);
	if (pGuildMem){
		GuildProto::stRetGuildDetailInfo retCmd;
		pGuildMem->fillDetailInfo(&retCmd);
		pUser->sendProtoToMe(retCmd);
	}
}


void CGuildMgr::addTraceInfoRecord(stTraceInfoDB & dbData)
{
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::addTraceInfoRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guildTraceDef,(const char *)&dbData,"GUILD_TRACE",autoID,pDelaySql);
}

void CGuildMgr::addTraceInfoRecordCB(bool opResult,UINT64 autoID)
{

}

void CGuildMgr::delTraceInfo(UINT32 id)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " ID=%d ",id);
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::delTraceInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GUILD_TRACE",where,pDelaySql);
}

void CGuildMgr::delTraceInfoCB(bool opResult,UINT64 autoID)
{

}

UINT16 CGuildMgr::getGuildRank(UINT32 guildID)
{
	return m_guildSortList.getMySortRank(guildID);
}

void   CGuildMgr::guildLevelChg(UINT32 guildID,UINT8 lv)
{
	stGuildRankInfo rankInfo;
	rankInfo.m_guildID = guildID;
	rankInfo.m_level = lv;
	m_guildSortList.updateSortInfo(rankInfo);
}

void   CGuildMgr::searchGuild(Player * pUser,const char * pGuildName)
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere, 64, " NAME like '%%%s%%' ", pGuildName);
	Athena::QueryCallback<CGuildMgr,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<CGuildMgr,UINT32>(this,&CGuildMgr::searchGuildCB,NULL,pUser->getID());
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_guildSearchTblDef," GUILD ",szWhere,NULL,0,pDelaySql);
}

void  CGuildMgr::searchGuildCB(CTblQueryResult* result,UINT32 charID)
{
	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (!pUser){
		return ;	
	}

	GuildProto::stRetSearchGuildResult retCmd;
	if (!result){
		pUser->sendProtoToMe(retCmd);
		return ;
	}


	result->setFieldDef(g_guildSearchTblDef);
	UINT32 recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	if (result->getData ())
	{
		UINT32 *pGuildArray = (UINT32 *) result->getData ();
		for (UINT16 i = 0; (i < result->getRowCount ()) && (i < 50); ++i)
		{
			CGuild *pGuild = getGuildByID (pGuildArray[i]);
			if (pGuild)
			{
				GuildProto::stGuildListInfo * pSyn = retCmd.add_listinfo ();
				if (pSyn)
				{
					pGuild->fillGuildListInfo (*pSyn);
				}
			}
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void  CGuildMgr::updateCopymapScore(UINT32 guildID,UINT32 copymapID,UINT32 score)
{
	UINT32 realCopyMapID = CGuild::getRealCopyMapID(copymapID);
	GuildCopymapScoreList & copymapScoreList = m_copymapScoreList[realCopyMapID];
	if (copymapScoreList.isInSortList(guildID)){
		stGuildCopymapScoreInfo info = copymapScoreList.getSortInfo(guildID);
		info.m_score += score;
		copymapScoreList.updateSortInfo(info);
		stGuildCopyMapRankDB dbData;
		dbData.m_copyMapID = realCopyMapID;
		dbData.m_guildID = guildID;
		dbData.m_score = info.m_score;
		updateScoreRankRecord(dbData);
	}   
	else {
		stGuildCopymapScoreInfo info;
		info.m_guildID = guildID;
		info.m_score = score;
		copymapScoreList.insertSortInfo(info);
		stGuildCopyMapRankDB dbData;
		dbData.m_copyMapID = realCopyMapID;
		dbData.m_guildID = guildID;
		dbData.m_score = score;
		addScoreRankRecord(dbData);
	}
}

void CGuildMgr::addScoreRankRecord(stGuildCopyMapRankDB & dbData)
{
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::addScoreRankRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guildCopymapScoreRankTblDef,(const char *)&dbData,"GUILD_COPYMAP_SCORE_RANK",autoID,pDelaySql);

}

void CGuildMgr::addScoreRankRecordCB(bool opResult,UINT64 autoID)
{

}

void CGuildMgr::updateScoreRankRecord(stGuildCopyMapRankDB & dbData)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " COPYMAP_ID=%u AND GUILD_ID=%d ",dbData.m_copyMapID,dbData.m_guildID);
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::updateScoreRankRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_guildCopymapScoreRankTblDef,(const char *)&dbData,"GUILD_COPYMAP_SCORE_RANK",where,pDelaySql);

}

void CGuildMgr::deleteScoreRank()
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " ID > 0");
	Athena::CUDSqlCallBack<CGuildMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr>(this,&CGuildMgr::deleteScoreRankCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GUILD_COPYMAP_SCORE_RANK",where,pDelaySql);
}

void CGuildMgr::deleteScoreRankCB(bool opResult,UINT64 autoID)
{}

void CGuildMgr::updateScoreRankRecordCB(bool opResult,UINT64 autoID)
{

}

void CGuildMgr::sendCopymapSortList(Player * pUser,UINT32 copymapID,UINT16 beginRank)
{
	if (!pUser){
		return ;
	}

	GuildCopymapScoreList & copymapScoreList = m_copymapScoreList[copymapID];
	GuildProto::stRetGuildCopymapScoreSortlist retCmd;
	retCmd.set_copymapid(copymapID);
	std::vector<stGuildCopymapScoreInfo> vec;
	copymapScoreList.getFromN12N2(vec,beginRank,beginRank + 10);
	retCmd.set_myrank(copymapScoreList.getMySortRank(pUser->getID()));
	retCmd.set_beginrank(beginRank);

	for (UINT16 i = 0;i < vec.size();++i){
		stGuildCopymapScoreInfo & info = vec[i];
		CGuild *pGuild =  getGuildByID(info.m_guildID);	
		if (pGuild) {
			GuildProto::stCopymapScoreSortItem * pSyn = retCmd.add_sortitem();
			if (pSyn){
				pSyn->set_guildid(info.m_guildID);
				pSyn->set_name(pGuild->getName());
				pSyn->set_score(info.m_score);
			}
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void stGuildCopyMapInfo::initFromDB(const stGuildCopyMapDB & data)
{
	m_copyMapID = data.m_copyMapID;	
	m_openTime  = data.m_openTime;
	m_bossHP = data.m_bossHP;
	m_stepID    = data.m_stepID;
	m_guildID = data.m_guildID;
	m_killCharID = data.m_killCharID;
}

void stGuildCopyMapInfo::fillDBInfo(stGuildCopyMapDB & data)
{
	data.m_copyMapID = m_copyMapID;
	data.m_openTime  = m_openTime;
	data.m_bossHP = m_bossHP;
	data.m_stepID = m_stepID;
	data.m_guildID = m_guildID;
	data.m_killCharID = m_killCharID;
}

void CGuildCopyMapMgr::addRecord(stGuildCopyMapDB & data)
{
	Athena::CUDSqlCallBack<CGuildCopyMapMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildCopyMapMgr>(this,&CGuildCopyMapMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guildCopyMapDef,(const char *)&data,"GUILD_COPYMAP",autoID,pDelaySql);
}

void CGuildCopyMapMgr::addRecordCB(bool opResult,UINT64 autoID)
{

}

void CGuildCopyMapMgr::updateRecord(stGuildCopyMapDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " COPYMAP_ID=%u AND GUILD_ID=%d ",data.m_copyMapID,data.m_guildID);
	Athena::CUDSqlCallBack<CGuildCopyMapMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildCopyMapMgr>(this,&CGuildCopyMapMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_guildCopyMapDef,(const char *)&data,"GUILD_COPYMAP",where,pDelaySql);
}

void CGuildCopyMapMgr::updateRecordCB(bool opResult,UINT64 autoID)
{

}

void CGuildCopyMapMgr::removeRecord()
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere,sizeof(szWhere),"GUILD_ID>0");
	Athena::CUDSqlCallBack<CGuildCopyMapMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildCopyMapMgr>(this,&CGuildCopyMapMgr::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GUILD_COPYMAP",szWhere,pDelaySql);
}

void CGuildCopyMapMgr::removeRecordCB(bool opResult,UINT64 autoID)
{}


void stManorState::initFromDB(const stManorStateDB & data)
{
	m_id = data.m_id;
	m_state = data.m_state;
	m_bGiveReward = data.m_bGiveReward;
}

void stManorState::fillDBData(stManorStateDB & data)
{
	data.m_id = m_id;
	data.m_state = m_state;
	data.m_bGiveReward = m_bGiveReward;
}

void stManorState::print()
{
	Athena::logger->trace("领地状态:state:%d,giveReward:%d",m_state,m_bGiveReward);	
}

stManorFormation::stManorFormation(GuildProto::eFormation formation) : m_eFormation(formation)
{
	reset();
}

void stManorFightTeam::reset()
{
	m_bEnter = false;
	m_bWin = false;
	for (UINT16 j = 0; j < eManoFightTeamMaxMem;++j){
		m_userInfo[j].reset();
	}
}

void stManorFormation::reset()
{
	m_bFinish = false;
	m_bSideAWin = false;
	m_bSideBWin = false;
	for (UINT16 i = 0;i < eManorFightMaxTeam;++i){
		m_teamA[i].reset();
	}
}

void stManorFormation::calcBattleFinish(UINT32 manorID)
{
	if (m_bFinish){
		return ;
	}
	bool bSideAAllDie = true;
	bool bSideBAllDie = true;
	bool bOldFinishState = m_bFinish;

	for (UINT16 i = 0;i < eManorFightMaxTeam;++i){
		if (!m_teamA[i].isAllDie()){
			bSideAAllDie = false;	
		}

		if (!m_teamB[i].isAllDie()){
			bSideBAllDie = false;	
		}
	}

	if (bSideAAllDie || bSideBAllDie){
		m_bFinish = true;
		if (bSideAAllDie && (!bSideBAllDie)){
			m_bSideBWin = true;
			m_bSideAWin = false;
		}

		if ((!bSideAAllDie) && bSideBAllDie){
			m_bSideAWin = true;	
			m_bSideBWin = false;
		}
	}
	else {
		m_bFinish = false;
	}

	if ((!bOldFinishState) && m_bFinish && (m_bSideAWin || m_bSideBWin)){
		GuildProto::stManorFightFormationResult cmd;
		cmd.set_manorid(manorID);
		cmd.set_formation(m_eFormation);
		//sendProto2TeamA(cmd);
		//sendProto2TeamB(cmd);
		stManorInfo *pManorInfo =  CManorInfoMgr::getSingleton().getManorInfo(manorID);
		if (pManorInfo){
			if (m_bSideAWin){
				cmd.set_winguildid(pManorInfo->m_pkSideA);
			}

			if (m_bSideBWin){
				cmd.set_winguildid(pManorInfo->m_pkSideB);
			}

			CGuild * pGuildA = CGuildMgr::getSingleton().getGuildByID(pManorInfo->m_pkSideA);
			if (pGuildA){
				pGuildA->sendProto2Mem(cmd);
			}
			CGuild * pGuildB = CGuildMgr::getSingleton().getGuildByID(pManorInfo->m_pkSideB);
			if (pGuildB){
				pGuildB->sendProto2Mem(cmd);
			}
		}
	}
}


UINT8 stManorFormation::getTeamMemNum(bool bTeamA,UINT8 teamID)
{
	if (teamID < eManorFightMaxTeam){
		UINT8 ret = 0;
		if (bTeamA){
			if (m_teamA[teamID].m_userInfo[0].m_id){
				ret += 1;
			}

			if (m_teamA[teamID].m_userInfo[1].m_id){
				ret += 1;
			}

			if (m_teamA[teamID].m_userInfo[2].m_id){
				ret += 1;
			}
		}	
		else {
			if (m_teamB[teamID].m_userInfo[0].m_id){
				ret += 1;
			}

			if (m_teamB[teamID].m_userInfo[1].m_id){
				ret += 1;
			}

			if (m_teamB[teamID].m_userInfo[2].m_id){
				ret += 1;
			}
		}
		return ret;
	}
	else {
		return 0;
	}

}

void stManorInfo::initFromDB(const stManorInfoDB & data)
{
	m_manorID = data.m_manorID;
	m_ownerGuildID = data.m_ownerGuildID;
	m_pkSideA = data.m_pkSideA;
	m_pkSideB = data.m_pkSideB;
	m_loserGuildID = data.m_loserGuildID;
	m_bFinish = data.m_bFinish;
}

void stManorInfo::fillDBData(stManorInfoDB & data)
{
	data.m_manorID = m_manorID;
	data.m_ownerGuildID = m_ownerGuildID;
	data.m_pkSideA = m_pkSideA;
	data.m_pkSideB = m_pkSideB;
	data.m_loserGuildID = m_loserGuildID;
	data.m_bFinish = m_bFinish;
}

void stManorInfo::reset()
{
	m_ownerGuildID = 0;
	m_pkSideA = 0;
	m_pkSideB = 0;
	//m_winGuildID = 0;
	m_loserGuildID = 0;
	m_bFinish = false;
	m_drawGuildID1 = 0;
	m_drawGuildID2 = 0;

	m_main.reset();
	m_left.reset();
	m_right.reset();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("领地信息被重置了manorID:%d",m_manorID);
#endif
}

void stManorInfo::print()
{
	Athena::logger->trace("领地信息:manroID:%d,ownerID:%d,m_pkSideA:%d,m_pkSideB",m_manorID,m_ownerGuildID,m_pkSideA,m_pkSideB);
}

void stManorApplyInfo::initFromDB(const stManorApplyDB & data)
{
	m_guildID = data.m_guildID;
	m_manorID = data.m_manorID;
	m_contribute = data.m_contribute;
	m_createTime = data.m_createTime;
}

void stManorApplyInfo::fillDBData(stManorApplyDB & data)
{
	data.m_guildID = m_guildID;
	data.m_manorID = m_manorID;
	data.m_contribute = m_contribute;
	data.m_createTime = m_createTime;
}

void stManorApplyInfo::print()
{

}

CTblField g_manorStateTblDef[] = {
	{ "ID",     		 CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "STATE",     		 CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ "GIVEREWARD",      CTblField::DB_TYPE_UINT8,  sizeof(UINT8)   },
	{ NULL, 0, 0 },
};


CTblField g_manorInfoTblDef[] = {
	{ "MANORID",     		CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ "OWNER_GUILDID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "PK_SIDEA",      		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "PK_SIDEB",      		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "LOSER_GUILDID", 		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "IS_FINISH",          CTblField::DB_TYPE_UINT8,  sizeof(UINT8)   },
	{ NULL, 0, 0 },
};

CTblField g_manorApplyTblDef[] = {
	{ "MANORID",     		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "GUILDID",      		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "CONTRIBUTE",      	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "CREATETIME",      	CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

CManorStateMgr::CManorStateMgr() :CTemplateDB<stManorStateDB,stManorState>("MANORSTATE",g_manorStateTblDef)
{
	m_bInit = false;
}

CManorStateMgr::~CManorStateMgr()
{

}

void CManorStateMgr::updateManatorState()
{
	stManorStateDB dbData;
	m_manorState.fillDBData(dbData);
	char szWhere[50] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," ID=%d ",m_manorState.m_id);
	updateRecord(dbData,szWhere);
}

void CManorStateMgr::giveRewardCB()
{
	if (m_manorState.m_bGiveReward){//已经给过奖励不能给了
		return ;
	}
	m_manorState.m_bGiveReward  = 1;
	updateManatorState();
	CManorInfoMgr::getSingleton().giveReward();
}

void CManorStateMgr::add2Container(stManorState & info)
{
	m_manorState = info;
	m_bInit = true;
	Athena::logger->trace("领地战的状态是:%d",m_manorState.m_state);
}

void CManorStateMgr::initState()
{
	if (m_bInit){
		return ;
	}

	UINT64 autoID = 0;
	stManorStateDB dbData;
	g_databaseEngine.insertRecord(g_manorStateTblDef,(const char *)&dbData,"MANORSTATE",autoID,NULL);
	m_manorState.m_id = autoID;
}

UINT32 CManorStateMgr::calcSecOfWeek(UINT32 cur)
{
	struct tm _tm;
	RealTime::getLocalTime(_tm,cur);

	UINT16 wday = _tm.tm_wday;
	if (0 == _tm.tm_wday){
		wday = 7;
	}
	return (wday - 1) * 86400 + _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;
}

void CManorStateMgr::notifyStateChg(UINT16 oldState,UINT16 newState)
{
	m_manorState.m_state = newState;

	if((oldState != eState_Apply) && (newState == eState_Apply)){
		char szWhere[65] = { 0 };
		snprintf(szWhere,sizeof(szWhere)," ID > 0 ");
		CManorApplyMgr::getSingleton().removeRecord(szWhere);
		CManorApplyMgr::getSingleton().clearApplyInfo();
		CManorStateMgr::getSingleton().resetRewardState();
	}

	if ((eState_Break1 == oldState) && (eState_FIGHT1 == newState)){//第1轮开始
		CManorInfoMgr::getSingleton().beginBattle(1);
	}

	if ((eState_Break2 == oldState) && (eState_FIGHT2 == newState)){//第2轮开始
		CManorInfoMgr::getSingleton().beginBattle(2);
	}

	if ((eState_Break3 == oldState) && (eState_FIGHT3 == newState)){//第3轮开始
		CManorInfoMgr::getSingleton().beginBattle(3);
	}

	if ((eState_Break4 == oldState) && (eState_FIGHT4 == newState)){//第4轮开始
		CManorInfoMgr::getSingleton().beginBattle(4);
	}

	if ((eState_Break5 == oldState) && (eState_FIGHT5 == newState)){//第5轮开始
		CManorInfoMgr::getSingleton().beginBattle(5);
	}

	if ((eState_ApplyOver == oldState) && (eState_Ready == newState)){
		CManorInfoMgr::getSingleton().enterReady();
		CManorApplyMgr::getSingleton().endApply();
	}

	if (eState_Break1 == newState){
		CManorInfoMgr::getSingleton().enterBreak1();	
	}

	if (eState_FIGHT5 == oldState){//战斗结束,结算
		CManorInfoMgr::getSingleton().battleEnd();
	}

	if (eState_GiveReward == newState){
		giveRewardCB();	
	}
	updateManatorState();
	Athena::logger->trace("公会领地战状态切换:(oldState:%d,%s,----> curState:%d,%s)",oldState,getStateName(oldState),newState,getStateName(newState));
}

void CManorStateMgr::timer(UINT32 cur)
{
	UINT16 oldState = m_manorState.m_state;
	UINT32 secOfWeek = calcSecOfWeek(cur);

	std::map<UINT16,CGuildConfig::stManorFightTime> &fightTimeCnt =  CGuildConfig::getSingleton().getManorFightTime();
	std::map<UINT16,CGuildConfig::stManorFightTime>::iterator it = fightTimeCnt.begin();

	UINT32 curState = eState_None;
	for (;it != fightTimeCnt.end();++it) {
		CGuildConfig::stManorFightTime & fightTime = it->second;
		if ((secOfWeek >= fightTime.m_applyBeginTime) && (secOfWeek <= fightTime.m_applyEndTime)){
			curState = eState_Apply;	
			break;
		}
		else if((secOfWeek >= fightTime.m_applyOverBeginTime) && (secOfWeek <= fightTime.m_applyOverEndTime)){
			curState = eState_ApplyOver;
			break;
		}
		else if ((secOfWeek >= fightTime.m_readyBeginTime) && (secOfWeek <= fightTime.m_readyEndTime)){
			curState = eState_Ready;	
			break;
		}
		else if ((secOfWeek >= fightTime.m_giveRewardBeginTime) && (secOfWeek <= fightTime.m_giveRewardEndTime)){
			curState = eState_GiveReward; 
			break;
		}

		bool bBreak = false;
		std::map<UINT16,CGuildConfig::stManorFightStepTime>::iterator subIt = it->second.m_steps.begin();
		for (;subIt != it->second.m_steps.end();++subIt){
			CGuildConfig::stManorFightStepTime & step = subIt->second;

			if ((secOfWeek >= step.m_breakBeginTime) && (secOfWeek <= step.m_breakEndTime)){
				if (step.m_id == 1){
					curState = eState_Break1;
				}

				if (step.m_id == 2){
					curState = eState_Break2;
				}

				if (step.m_id == 3){
					curState = eState_Break3;
				}

				if (step.m_id == 4){
					curState = eState_Break4;
				}

				if (step.m_id == 5){
					curState = eState_Break5;
				}
				break;
			}
			else if ((secOfWeek >= step.m_fightBeginTime) && (secOfWeek <= step.m_fightEndTime)){
				if (step.m_id == 1){
					curState = eState_FIGHT1; 
				}

				if (step.m_id == 2){
					curState = eState_FIGHT2; 
				}

				if (step.m_id == 3){
					curState = eState_FIGHT3; 
				}

				if (step.m_id == 4){
					curState = eState_FIGHT4; 
				}

				if (step.m_id == 5){
					curState = eState_FIGHT5; 
				}
				break;
			}
		}

		if (bBreak){
			break;
		}
	}

	if (oldState != curState){
		curState = getNextState(oldState);
		notifyStateChg(oldState,curState);
	}
}

UINT16 CManorStateMgr::getNextState(UINT16 state)
{
	return ((state + 1)% eState_MAX);	
}

void CManorStateMgr::resetRewardState()
{
	stManorStateDB dbData;
	m_manorState.m_bGiveReward = false;
	updateManatorState();
}

const char * CManorStateMgr::getStateName(UINT16 state)
{
	switch (state){
		case eState_None :
			{
				return "空";
			}
			break;
		case eState_Apply ://申请状态
			{
				return "申请状态";
			}
			break;
		case eState_ApplyOver ://申请结束
			{
				return "申请结束";
			}
			break;
		case eState_Ready : //准备
			{
				return "准备";
			}
			break;
		case eState_Break1 : //调整
			{
				return "调整1";
			}
			break;
		case eState_FIGHT1 ://战斗
			{
				return "战斗1";
			}
			break;
		case eState_Break2  ://调整
			{
				return "调整2";
			}
			break;
		case eState_FIGHT2  : //战斗
			{
				return "战斗2";
			}
			break;
		case eState_Break3  : //调整
			{
				return "调整3";
			}
			break;
		case eState_FIGHT3 ://战斗
			{
				return "战斗3";
			}
			break;
		case eState_Break4 ://调整
			{
				return "调整4";
			}
			break;
		case eState_FIGHT4 ://战斗
			{
				return "战斗4";
			}
			break;
		case eState_Break5  ://调整
			{
				return "调整5";
			}
			break;
		case eState_FIGHT5 : //战斗
			{
				return "战斗5";
			}
			break;
		case eState_GiveReward : //给奖励
			{
				return "给奖励";
			}
			break;
	}
	return "不存在的状态";
}

CManorInfoMgr::CManorInfoMgr() : CTemplateDB<stManorInfoDB,stManorInfo>("MANORINFO",g_manorInfoTblDef)
{

}

CManorInfoMgr::~CManorInfoMgr()
{

}

void CManorInfoMgr::add2Container(stManorInfo & info)
{
	m_manorInfos.insert(std::make_pair(info.m_manorID,info));
}

void CManorInfoMgr::initManorInfo()
{
	std::map<UINT16,CGuildConfig::stManorInfo> &manorCnt =  CGuildConfig::getSingleton().getManorInfo();
	std::map<UINT16,CGuildConfig::stManorInfo>::iterator it = manorCnt.begin();


	for (;it != manorCnt.end();++it){
		CGuildConfig::stManorInfo & manorInfo = it->second;
		if (m_manorInfos.find(manorInfo.m_id) == m_manorInfos.end()){
			stManorInfoDB dbData;
			dbData.m_manorID = manorInfo.m_id;
			dbData.m_ownerGuildID = 0;
			dbData.m_pkSideA = 0;
			dbData.m_pkSideB = 0;
			stManorInfo info;
			info.initFromDB(dbData);
			UINT64 autoID = 0;
			g_databaseEngine.insertRecord(g_manorInfoTblDef,(const char *)&dbData,"MANORINFO",autoID,NULL);	
			m_manorInfos[manorInfo.m_id]  = info;
		}
	}
}

void CManorInfoMgr::sendManorFightWinInfo(Player * pUser)
{
	if (!pUser){
		return ;
	}

	GuildProto::stRetGuildManorFightInfo  retCmd;
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.begin();

	for (;it != m_manorInfos.end();++it){
		stManorInfo & manorInfo = it->second;
		GuildProto::stSynManorInfo * pSyn = retCmd.add_manorinfos();
		if (pSyn){
			pSyn->set_manorid(manorInfo.m_manorID);
			pSyn->set_ownerguildid(manorInfo.m_ownerGuildID);
			pSyn->set_sideaguildid(manorInfo.m_pkSideA);
			pSyn->set_sidebguildid(manorInfo.m_pkSideB);

			CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_ownerGuildID);
			if (pGuild){
				pSyn->set_ownerguildname(pGuild->getName());
			}
		}
	}
	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		retCmd.set_applymanorid(CManorApplyMgr::getSingleton().getApplyManorID(pGuild->getID()));
	}
	pUser->sendProtoToMe(retCmd);
}

void CManorInfoMgr::teamEnterBattle(stManorInfo & manorInfo,GuildProto::eFormation formation,UINT8 teamA,UINT8 teamB)
{
	if ((teamA >= eManorFightMaxTeam) || (teamB >= eManorFightMaxTeam)){
		return ;
	}

	stManorFormation * pFormation = NULL;

	if (GuildProto::eFormation_Main == formation){
		pFormation = &manorInfo.m_main;
	}
	else if (GuildProto::eFormation_Left == formation){
		pFormation = &manorInfo.m_left;
	}
	else {
		pFormation = &manorInfo.m_right;
	}

	if (!pFormation){
		return ;
	}

	if (pFormation->m_teamA[teamA].m_bEnter || pFormation->m_teamB[teamB].m_bEnter){
		return;
	}

	Global::stEnterManorFightMap cmd;
	cmd.set_manorid(manorInfo.m_manorID);
	cmd.set_fomation(formation);
	cmd.set_teamaid(teamA);
	cmd.set_teambid(teamB);

	for (UINT16 i = 0;i < eManoFightTeamMaxMem;++i){
		if ((pFormation->m_teamA[teamA].m_userInfo[i].m_id > 0) && (pFormation->m_teamA[teamA].m_userInfo[i].m_curHP > 0)){
			Global::stEnterManorFightMapUserInfo * pSyn = cmd.add_guildamem();
			if (pSyn){
				pSyn->set_charid(pFormation->m_teamA[teamA].m_userInfo[i].m_id);
				pSyn->set_hp(pFormation->m_teamA[teamA].m_userInfo[i].m_curHP);
			}
		}	

		if ((pFormation->m_teamB[teamB].m_userInfo[i].m_id > 0) && (pFormation->m_teamB[teamB].m_userInfo[i].m_curHP > 0)){
			Global::stEnterManorFightMapUserInfo * pSyn = cmd.add_guildbmem();
			if (pSyn){
				pSyn->set_charid(pFormation->m_teamB[teamB].m_userInfo[i].m_id);
				pSyn->set_hp(pFormation->m_teamB[teamB].m_userInfo[i].m_curHP);
			}
		}
	}

	if ((cmd.guildamem_size() > 0) && (cmd.guildbmem_size() > 0)){//有人才进入队伍
		g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
		pFormation->m_teamA[teamA].m_bEnter = true;
		pFormation->m_teamB[teamB].m_bEnter = true;			
	}
#ifdef _HDZ_DEBUG
	if ((cmd.guildamem_size() > 0) && (cmd.guildbmem_size() > 0)){
		Athena::logger->trace("manorID:%d 阵容:%d,队伍A:%d,队伍B:%d,进入场景",manorInfo.m_manorID,formation,teamA,teamB);
	}
#endif
}

void CManorInfoMgr::calcResult1(stManorInfo & manorInfo)
{
	if (! manorInfo.m_pkSideA || !manorInfo.m_pkSideB){
		return;
	}

	UINT8 sideAWinNum = 0;
	UINT8 sideBWinNum = 0;

	if (manorInfo.m_left.m_bSideAWin){
		sideAWinNum += 1;
	}

	if (manorInfo.m_main.m_bSideAWin){
		sideAWinNum += 1;
	}

	if (manorInfo.m_right.m_bSideAWin){
		sideAWinNum += 1;	
	}

	if (manorInfo.m_left.m_bSideBWin){
		sideBWinNum += 1;
	}

	if (manorInfo.m_main.m_bSideBWin){
		sideBWinNum += 1;
	}

	if (manorInfo.m_right.m_bSideBWin){
		sideBWinNum += 1;
	}

	if (sideAWinNum > sideBWinNum){
		//manorInfo.m_winGuildID = manorInfo.m_pkSideA;
		manorInfo.m_loserGuildID = manorInfo.m_pkSideB;
		manorInfo.m_ownerGuildID = manorInfo.m_pkSideA;
	}
	else if (sideAWinNum < sideBWinNum){
		//manorInfo.m_winGuildID = manorInfo.m_pkSideB;
		manorInfo.m_loserGuildID = manorInfo.m_pkSideA;
		manorInfo.m_ownerGuildID = manorInfo.m_pkSideB;
	}
	else {
		manorInfo.m_ownerGuildID = 0;
		//manorInfo.m_winGuildID = 0;
		manorInfo.m_loserGuildID = 0;
		manorInfo.m_drawGuildID1 = manorInfo.m_pkSideA;
		manorInfo.m_drawGuildID2 = manorInfo.m_pkSideB;
	}
	manorInfo.m_pkSideA = 0;
	manorInfo.m_pkSideB = 0;

	stManorInfoDB dbData;
	manorInfo.fillDBData(dbData);
	char szWhere[65] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," MANORID=%d ",manorInfo.m_manorID);
	updateRecord(dbData,szWhere);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("公会领地:%d,结算了winID:%d,loseID:%d",manorInfo.m_manorID,manorInfo.m_ownerGuildID,manorInfo.m_loserGuildID);
#endif

}

void CManorInfoMgr::calcResult(stManorInfo & manorInfo)
{
	if (manorInfo.m_pkSideA && manorInfo.m_pkSideB){
		UINT8 sideAWinNum = 0;
		UINT8 sideBWinNum = 0;

		bool sideALeftAlive = false;
		bool sideBLeftAlive = false;
		bool sideARightAlive = false;
		bool sideBRightAlive = false;
		bool sideAMainAlive = false;
		bool sideBMainAlive = false;

		for (UINT8 i = 0;i < eManorFightMaxTeam; ++i){//是否有人
			if (!manorInfo.m_left.m_teamA[i].isAllDie()){//还有人活着
				sideALeftAlive = true;
			}

			if (!manorInfo.m_left.m_teamB[i].isAllDie()){//还有人活着
				sideBLeftAlive = true;
			}

			if (!manorInfo.m_right.m_teamA[i].isAllDie()){//还有人活着
				sideARightAlive = true;
			}

			if (!manorInfo.m_right.m_teamB[i].isAllDie()){//还有人活着
				sideBRightAlive = true;
			}

			if (!manorInfo.m_main.m_teamA[i].isAllDie()){//还有人活着
				sideAMainAlive = true;
			}

			if (!manorInfo.m_main.m_teamB[i].isAllDie()){//还有人活着
				sideBMainAlive = true;
			}
		}

		if ((sideALeftAlive  == true) && (sideBLeftAlive == false)){
			sideAWinNum += 1;
		}
		else if ((sideALeftAlive  == false) && (sideBLeftAlive == true)){
			sideBWinNum += 1;
		}

		if ((sideARightAlive  == true) && (sideBRightAlive == false)){
			sideAWinNum += 1;
		}
		else if ((sideARightAlive  == false) && (sideBRightAlive == true)){
			sideBWinNum += 1;
		}

		if ((sideAMainAlive  == true) && (sideBMainAlive == false)){
			sideAWinNum += 1;
		}
		else if ((sideAMainAlive  == false) && (sideBMainAlive == true)){
			sideBWinNum += 1;
		}

		if (sideAWinNum > sideBWinNum){
			//manorInfo.m_winGuildID = manorInfo.m_pkSideA;
			manorInfo.m_loserGuildID = manorInfo.m_pkSideB;
			manorInfo.m_ownerGuildID = manorInfo.m_pkSideA;
		}
		else if (sideAWinNum < sideBWinNum){
			//manorInfo.m_winGuildID = manorInfo.m_pkSideB;
			manorInfo.m_loserGuildID = manorInfo.m_pkSideA;
			manorInfo.m_ownerGuildID = manorInfo.m_pkSideB;
		}
		else {
			manorInfo.m_ownerGuildID = 0;
			//manorInfo.m_winGuildID = 0;
			manorInfo.m_loserGuildID = 0;
		}
		manorInfo.m_pkSideA = 0;
		manorInfo.m_pkSideB = 0;

		stManorInfoDB dbData;
		manorInfo.fillDBData(dbData);
		char szWhere[65] = { 0 };
		snprintf(szWhere,sizeof(szWhere)," MANORID=%d ",manorInfo.m_manorID);
		updateRecord(dbData,szWhere);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("公会领地:%d,结算了winID:%d,loseID:%d",manorInfo.m_manorID,manorInfo.m_ownerGuildID,manorInfo.m_loserGuildID);
#endif
	}
}

void CManorInfoMgr::enterReady()
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.begin();
	for (;it != m_manorInfos.end();++it){
		stManorInfo & manorInfo = it->second;
		manorInfo.m_bFinish = false;
		manorInfo.m_left.reset();
		manorInfo.m_right.reset();
		manorInfo.m_main.reset();
	}
}

void CManorInfoMgr::enterBreak1()
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.begin();
	for (;it != m_manorInfos.end();++it){
		stManorInfo & manorInfo = it->second;
		if (manorInfo.m_bFinish){

		}
		else {
			manorInfo.m_ownerGuildID = 0;
		}
		manorInfo.m_loserGuildID = 0;
		manorInfo.m_drawGuildID1 = 0;
		manorInfo.m_drawGuildID2 = 0;
		//manorInfo.m_winGuildID = 0;
		//manorInfo.m_left.reset();
		//manorInfo.m_right.reset();
		//manorInfo.m_right.reset();

		stManorInfoDB dbData;
		manorInfo.fillDBData(dbData);
		char szWhere[60] = { 0 };
		snprintf(szWhere,sizeof(szWhere),"MANORID=%d",manorInfo.m_manorID);
		updateRecord(dbData,szWhere);
		checkBattleFinish(manorInfo);
	}
}

void CManorInfoMgr::battleEnd()
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.begin();
	for (;it != m_manorInfos.end();++it){
		stManorInfo & manorInfo = it->second;
		//calcResult(manorInfo);
		calcResult1(manorInfo);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[领地战一轮战斗结束] ");
#endif
}

void CManorInfoMgr::beginBattle(UINT16 round)
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.begin();
	for (;it != m_manorInfos.end();++it){
		stManorInfo & manorInfo = it->second;
		if (manorInfo.m_pkSideA && manorInfo.m_pkSideB){
			for (UINT8 i = 0;i < eManorFightMaxTeam; ++i){
				if (!manorInfo.m_left.m_teamA[i].isAllDie()) {
					for (UINT8 j = 0;j < eManorFightMaxTeam; ++j){
						if (!manorInfo.m_left.m_teamB[j].isAllDie()){
							teamEnterBattle(manorInfo,GuildProto::eFormation_Left,i,j);	
						}
					}
				}

				if (!manorInfo.m_right.m_teamA[i].isAllDie()){
					for (UINT8 j = 0;j < eManorFightMaxTeam; ++j){
						if (!manorInfo.m_right.m_teamB[j].isAllDie()){
							teamEnterBattle(manorInfo,GuildProto::eFormation_Right,i,j); 		
						}
					}
				}

				if (!manorInfo.m_main.m_teamA[i].isAllDie()){
					for (UINT8 j = 0;j < eManorFightMaxTeam; ++j){
						if (!manorInfo.m_main.m_teamB[j].isAllDie()){
							teamEnterBattle(manorInfo,GuildProto::eFormation_Main,i,j);
						}
					}
				}
			}

			//给没有进入的人发送协议
			GuildProto::stManorFightHasNotMatch leaveCmd;
			leaveCmd.set_manorid(manorInfo.m_manorID);
			for (UINT8 i = 0;i < eManorFightMaxTeam;++i){
				if (!manorInfo.m_left.m_teamA[i].m_bEnter){
					manorInfo.m_left.m_teamA[i].sendProto2Mem(leaveCmd);	
				}

				if (!manorInfo.m_left.m_teamB[i].m_bEnter){
					manorInfo.m_left.m_teamB[i].sendProto2Mem(leaveCmd);	
				}

				if (!manorInfo.m_main.m_teamA[i].m_bEnter){
					manorInfo.m_main.m_teamA[i].sendProto2Mem(leaveCmd);	
				}

				if (!manorInfo.m_main.m_teamB[i].m_bEnter){
					manorInfo.m_main.m_teamB[i].sendProto2Mem(leaveCmd);	
				}

				if (!manorInfo.m_right.m_teamA[i].m_bEnter){
					manorInfo.m_right.m_teamA[i].sendProto2Mem(leaveCmd);	
				}

				if (!manorInfo.m_right.m_teamB[i].m_bEnter){
					manorInfo.m_right.m_teamB[i].sendProto2Mem(leaveCmd);	
				}
			}
		}
	}
}

bool CManorInfoMgr::isManorExist(UINT32 manorID)
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(manorID);

	if (it != m_manorInfos.end()){
		return true;
	}
	else {
		return false;
	}
}

void CManorInfoMgr::setOwner(UINT32 manorID,UINT32 guildID)
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(manorID);
	if (it == m_manorInfos.end()){
		return ;
	}

	it->second.m_ownerGuildID = guildID;
	it->second.m_pkSideA = 0;
	it->second.m_pkSideB = 0;
	//it->second.m_winGuildID = guildID;
	it->second.m_loserGuildID = 0;
	it->second.m_bFinish = true;

	stManorInfoDB dbData;
	it->second.fillDBData(dbData);
	char szWhere[65] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," MANORID=%d ",manorID);
	updateRecord(dbData,szWhere);
}

void CManorInfoMgr::setPKGuildID(UINT32 manorID,UINT32 sideAID,UINT32 sideBID)
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(manorID);
	if (it == m_manorInfos.end()){
		return ;
	}

	it->second.m_ownerGuildID = 0;
	it->second.m_pkSideA = sideAID;
	it->second.m_pkSideB = sideBID;

	stManorInfoDB dbData;
	it->second.fillDBData(dbData);
	char szWhere[65] = { 0 };
	snprintf(szWhere,sizeof(szWhere)," MANORID=%d ",manorID);
	updateRecord(dbData,szWhere);
}

void CManorInfoMgr::sendFormation(Player * pUser,UINT32 manorID,GuildProto::eFormation eFormat)
{
	if (!pUser){
		return ;
	}

	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(manorID);
	if (it == m_manorInfos.end()){
		Athena::logger->error("发送阵容队列信息时候错误,发送过来的manorID:%d不正确!",manorID);
		return ;
	}

	stManorFormation * pFormation = NULL;

	if (GuildProto::eFormation_Main == eFormat){
		pFormation = &it->second.m_main;
	}
	else if (GuildProto::eFormation_Left == eFormat){
		pFormation = &it->second.m_left;
	}
	else {
		pFormation = &it->second.m_right;
	}

	if (!pFormation){
		return ;
	}

	GuildProto::stRetManorFightFormation retCmd;
	retCmd.set_sideaguildid(it->second.m_pkSideA);
	retCmd.set_sidebguildid(it->second.m_pkSideB);
	retCmd.set_formation(eFormat);
	retCmd.set_manorid(manorID);

	for (UINT8 i = 0;i < eManorFightMaxTeam; ++i){
		for (UINT8 j = 0;j < eManoFightTeamMaxMem;++j){
			if (pFormation->m_teamA[i].m_userInfo[j].m_id){
				GuildProto::stSynManorBattleMem * pSyn = NULL;
				pSyn = retCmd.add_sideateam();
				if (pSyn){
					pSyn->set_charid(pFormation->m_teamA[i].m_userInfo[j].m_id);
					pSyn->set_job(pFormation->m_teamA[i].m_userInfo[j].m_job);
					pSyn->set_sex(pFormation->m_teamA[i].m_userInfo[j].m_sex);
					pSyn->set_level(pFormation->m_teamA[i].m_userInfo[j].m_level);
					pSyn->set_battlepower(pFormation->m_teamA[i].m_userInfo[j].m_battlePower);
					pSyn->set_curhp(pFormation->m_teamA[i].m_userInfo[j].m_curHP);
					pSyn->set_maxhp(pFormation->m_teamA[i].m_userInfo[j].m_maxHP);
					pSyn->set_name(pFormation->m_teamA[i].m_userInfo[j].m_name);
					pSyn->set_pos(j);
					pSyn->set_teamid(i);
					pSyn->set_photoid(pFormation->m_teamA[i].m_userInfo[j].m_photoID);
				}
			}

			if (pFormation->m_teamB[i].m_userInfo[j].m_id){
				GuildProto::stSynManorBattleMem * pSyn = NULL;
				pSyn = retCmd.add_sidebteam();

				if (pSyn){
					pSyn->set_charid(pFormation->m_teamB[i].m_userInfo[j].m_id);
					pSyn->set_job(pFormation->m_teamB[i].m_userInfo[j].m_job);
					pSyn->set_sex(pFormation->m_teamB[i].m_userInfo[j].m_sex);
					pSyn->set_level(pFormation->m_teamB[i].m_userInfo[j].m_level);
					pSyn->set_battlepower(pFormation->m_teamB[i].m_userInfo[j].m_battlePower);
					pSyn->set_curhp(pFormation->m_teamB[i].m_userInfo[j].m_curHP);
					pSyn->set_maxhp(pFormation->m_teamB[i].m_userInfo[j].m_maxHP);
					pSyn->set_name(pFormation->m_teamB[i].m_userInfo[j].m_name);
					pSyn->set_pos(j);
					pSyn->set_teamid(i);
					pSyn->set_photoid(pFormation->m_teamB[i].m_userInfo[j].m_photoID);
				}
			}
		}
	}

	pUser->sendProtoToMe(retCmd);
}

void CManorInfoMgr::enterManorFightTeam(Player * pUser,UINT32 manorID,GuildProto::eFormation formation,UINT8 teamID,UINT8 pos)
{
	if (!pUser){
		return ;
	}

	if (teamID >= eManorFightMaxTeam){
		Athena::logger->error("进入公会领地队伍失败,队伍不ID正确");
		return ;
	}

	if (pos >= eManoFightTeamMaxMem){
		Athena::logger->error("进入公会领地失败,队伍位置不正确");
		return ;
	}

	std::map<UINT32,stUserManorFightTeamInfo>::iterator subIt = m_userTeamInfos.find(pUser->getID());
	if (subIt != m_userTeamInfos.end()){
		Athena::logger->error("已经在队伍里还尝试进入队伍");
		return ;
	}

	GuildProto::stEnterManorFightTeamResult retCmd;
	retCmd.set_manorid(manorID);
	retCmd.set_formation(formation);
	retCmd.set_teamid(teamID);
	retCmd.set_pos(pos);

	if (CManorStateMgr::eState_Ready != CManorStateMgr::getSingleton().getState()){
		retCmd.set_result(GuildProto::stEnterManorFightTeamResult::eNot_In_Time);	
		pUser->sendProtoToMe(retCmd);
		return;
	}

	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(manorID);
	if (it == m_manorInfos.end()){
		Athena::logger->error("领地不存在,进入公会领地战斗队伍失败!");
		return ;
	}

	stManorInfo & manorInfo = it->second;
	stManorFormation * pFormation = NULL;
	retCmd.set_sideaguildid(manorInfo.m_pkSideA);
	retCmd.set_sidebguildid(manorInfo.m_pkSideB);

	if (GuildProto::eFormation_Main == formation){
		pFormation = &it->second.m_main;
	}
	else if (GuildProto::eFormation_Left == formation){
		pFormation = &it->second.m_left;
	}
	else {
		pFormation = &it->second.m_right;
	}

	if (!pFormation){
		return ;
	}

	stManorFightTeam * pTeam = NULL;

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild && (pGuild->getID() == manorInfo.m_pkSideA)){
		pTeam = &pFormation->m_teamA[teamID];	
	}

	if (pGuild && (pGuild->getID() == manorInfo.m_pkSideB)){
		pTeam = &pFormation->m_teamB[teamID];	
	}

	if (!pTeam){
		retCmd.set_result(GuildProto::stEnterManorFightTeamResult::eNot_ChallengeGuildID);	
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (pTeam->m_userInfo[pos].m_id > 0){
		retCmd.set_result(GuildProto::stEnterManorFightTeamResult::ePos_Not_Empty);	
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	pTeam->m_userInfo[pos].m_id = pUser->getID();
	pTeam->m_userInfo[pos].m_job = pUser->m_job;
	pTeam->m_userInfo[pos].m_sex = pUser->m_sex;
	pTeam->m_userInfo[pos].m_level = CRankMgr::getSingleton().getLevelByCharID(pUser->getID());
	pTeam->m_userInfo[pos].m_battlePower = CRankMgr::getSingleton().getBattlePowerByCharID(pUser->getID());
	pTeam->m_userInfo[pos].m_curHP = pUser->m_maxHP;
	pTeam->m_userInfo[pos].m_maxHP = pUser->m_maxHP;
	pTeam->m_userInfo[pos].m_name = pUser->getName();
	pTeam->m_userInfo[pos].m_photoID = pUser->m_photoID;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("阵容:%d,队伍:%d,进入了玩家:%s,charID:%d,curHP:%d,maxHP:%d",formation,pos,pTeam->m_userInfo[pos].m_name.c_str(),pTeam->m_userInfo[pos].m_id,pTeam->m_userInfo[pos].m_curHP,pTeam->m_userInfo[pos].m_maxHP);
#endif
	retCmd.set_result(GuildProto::stEnterManorFightTeamResult::eSuccess);	
	pUser->sendProtoToMe(retCmd);

	GuildProto::stEnterManorFightTeam retCmd1;
	retCmd1.set_manorid(manorID);
	retCmd1.set_formation(formation);
	retCmd1.set_teamid(teamID);
	retCmd1.set_pos(pos);
	retCmd1.set_sideaguildid(manorInfo.m_pkSideA);
	retCmd1.set_sidebguildid(manorInfo.m_pkSideB);
	retCmd1.set_charid(pUser->getID());
	retCmd1.set_job(pUser->m_job);
	retCmd1.set_sex(pUser->m_sex);
	retCmd1.set_level(pUser->m_level);
	retCmd1.set_battlepower(pUser->m_battlePower);
	retCmd1.set_maxhp(pUser->m_maxHP);
	retCmd1.set_name(pUser->getName());
	retCmd1.set_photoid(pUser->m_photoID);

	if (pGuild){
		retCmd1.set_guildid(pGuild->getID());
	}

	pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_pkSideA);
	if (pGuild){
		pGuild->sendProto2Mem(retCmd1,pUser->getID());
	}

	pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_pkSideB);
	if (pGuild){
		pGuild->sendProto2Mem(retCmd1,pUser->getID());
	}

	stUserManorFightTeamInfo & teamInfo =  m_userTeamInfos[pUser->getID()];
	teamInfo.m_formation = formation;
	teamInfo.m_manorID = manorID;
	teamInfo.m_pos = pos;
	teamInfo.m_teamID = teamID;
}

stManorInfo * CManorInfoMgr::getManorInfo(UINT32 manorID)
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(manorID);
	if (it == m_manorInfos.end()){
		return NULL;
	}
	return &it->second;
}

UINT32 CManorInfoMgr::getOwnerPrayBoxID(UINT32 guildID)
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.begin();
	for (;it != m_manorInfos.end();++it){
		stManorInfo & manorInfo = it->second;
		if (guildID == manorInfo.m_ownerGuildID){
			return CGuildConfig::getSingleton().getOwnerPrayBoxID(it->first);
		}
	}
	return 0;
}

void CManorInfoMgr::chgTeamPos(Player * pUser,UINT8 destTeamID,UINT8 destPos)
{
	if (!pUser){
		return ;
	}

	if (destTeamID >= eManorFightMaxTeam){
		Athena::logger->error("切换公会领地战队伍失败,队伍不ID正确");
		return ;
	}

	if (destPos >= eManoFightTeamMaxMem){
		Athena::logger->error("切换公会领地战队伍失败,队伍位置不正确");
		return ;
	}

	GuildProto::stChgManorFightTeamPosResult retCmd;

	std::map<UINT32,stUserManorFightTeamInfo>::iterator subIt = m_userTeamInfos.find(pUser->getID());
	if (subIt == m_userTeamInfos.end()){
		retCmd.set_result(GuildProto::stChgManorFightTeamPosResult::eHas_Not_Enter);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	stUserManorFightTeamInfo & teamInfo = subIt->second;

	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(teamInfo.m_manorID);
	if (it == m_manorInfos.end()){
		return ;
	}

	stManorFormation * pFormation = NULL;
	if (GuildProto::eFormation_Main == teamInfo.m_formation){
		pFormation = &it->second.m_main;
	}
	else if (GuildProto::eFormation_Left == teamInfo.m_formation){
		pFormation = &it->second.m_left;
	}
	else {
		pFormation = &it->second.m_right;
	}

	if (!pFormation){
		Athena::logger->error("切换公会领地战队伍失败,原来的阵容不对!");
		return ;
	}

	UINT8 srcTeamID = teamInfo.m_teamID;
	UINT32 srcPos = teamInfo.m_pos;

	if (srcTeamID >= eManorFightMaxTeam){
		Athena::logger->error("切换公会领地战队伍失败,源队伍不ID正确");
		return ;
	}

	if (srcPos >= eManoFightTeamMaxMem){
		Athena::logger->error("切换公会领地战队伍失败,源队伍位置不正确");
		return ;
	}

	if (pFormation->m_teamA[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos].m_id == pUser->getID()){
		if (pFormation->m_teamA[destTeamID].m_userInfo[destPos].m_id > 0){
			retCmd.set_result(GuildProto::stChgManorFightTeamPosResult::eDest_Pos_Was_Occupied);
			pUser->sendProtoToMe(retCmd);
			return ;
		}
		pFormation->m_teamA[destTeamID].m_userInfo[destPos] = pFormation->m_teamA[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos];
		pFormation->m_teamA[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos].reset();
		teamInfo.m_teamID = destTeamID;
		teamInfo.m_pos = destPos;
	}

	if (pFormation->m_teamB[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos].m_id == pUser->getID()){
		if (pFormation->m_teamB[destTeamID].m_userInfo[destPos].m_id > 0){
			retCmd.set_result(GuildProto::stChgManorFightTeamPosResult::eDest_Pos_Was_Occupied);
			pUser->sendProtoToMe(retCmd);
			return ;
		}
		pFormation->m_teamB[destTeamID].m_userInfo[destPos] = pFormation->m_teamB[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos];
		pFormation->m_teamB[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos].reset();
		teamInfo.m_teamID = destTeamID;
		teamInfo.m_pos = destPos;
	}
	retCmd.set_teamid(teamInfo.m_teamID);
	retCmd.set_pos(teamInfo.m_pos);
	retCmd.set_result(GuildProto::stChgManorFightTeamPosResult::eSuccess);
	pUser->sendProtoToMe(retCmd);

	GuildProto::stNotifyChgManorFightTeamPos retCmd1;
	retCmd1.set_charid(pUser->getID());
	retCmd1.set_srcpos(srcPos);
	retCmd1.set_srcteamid(srcTeamID);
	retCmd1.set_dstteamid(teamInfo.m_teamID);
	retCmd1.set_formation(teamInfo.m_formation);
	retCmd1.set_dstpos(teamInfo.m_pos);

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(it->second.m_pkSideA);
	if (pGuild){
		pGuild->sendProto2Mem(retCmd1,pUser->getID());
	}

	pGuild = CGuildMgr::getSingleton().getGuildByID(it->second.m_pkSideB);
	if (pGuild){
		pGuild->sendProto2Mem(retCmd1,pUser->getID());
	}
}

void CManorInfoMgr::sendManorFightInfo(Player * pUser,UINT32 manorID)
{
	if (!pUser){
		return ;
	}

	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(manorID);

	if (it == m_manorInfos.end()){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());	
	if (!pGuild){
		return ;
	}

	stManorInfo & manorInfo = it->second;

	GuildProto::stRetManorFightWinInfo retCmd;
	UINT16 mainTeamNum = 0;
	UINT16 leftTeamNum = 0;
	UINT16 rightTeamNum = 0;
	retCmd.set_manorid(manorID);
	if (manorInfo.m_left.m_bSideAWin){
		retCmd.set_leftwinguildid(manorInfo.m_pkSideA);	
	}
	else if (manorInfo.m_left.m_bSideBWin){
		retCmd.set_leftwinguildid(manorInfo.m_pkSideB);
	}

	if (manorInfo.m_main.m_bSideAWin){
		retCmd.set_mainwinguildid(manorInfo.m_pkSideA);
	}
	else if (manorInfo.m_main.m_bSideBWin){
		retCmd.set_mainwinguildid(manorInfo.m_pkSideB);
	}

	if (manorInfo.m_right.m_bSideAWin){
		retCmd.set_rightwinguildid(manorInfo.m_pkSideA);
	}
	else if (manorInfo.m_right.m_bSideBWin){
		retCmd.set_rightwinguildid(manorInfo.m_pkSideB);
	}

	bool bTeamA = false;
	if (pGuild->getID() == manorInfo.m_pkSideA){
		bTeamA = true;
	}	

	for (UINT8 i = eManorFightTeam1;i < eManorFightMaxTeam;++i) {
		mainTeamNum = manorInfo.m_main.getTeamMemNum(bTeamA,i);
		if (mainTeamNum > 0){
			GuildProto::stSynFormationTeamInfo * pSyn = retCmd.add_mainjoininnum();
			if (pSyn){
				pSyn->set_teamid(i);
				pSyn->set_num(mainTeamNum);
			}
		}

		leftTeamNum = manorInfo.m_left.getTeamMemNum(bTeamA,i);
		if (leftTeamNum > 0){
			GuildProto::stSynFormationTeamInfo * pSyn = retCmd.add_leftjoininnum();
			if (pSyn){
				pSyn->set_teamid(i);
				pSyn->set_num(leftTeamNum);
			}
		}

		rightTeamNum = manorInfo.m_right.getTeamMemNum(bTeamA,i);
		if (rightTeamNum > 0){
			GuildProto::stSynFormationTeamInfo * pSyn = retCmd.add_rightjoininnum();
			if (pSyn){
				pSyn->set_teamid(i);
				pSyn->set_num(rightTeamNum);
			}
		}
	}

	pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_pkSideA);
	if (pGuild){
		retCmd.set_guildaname(pGuild->getName());
	}

	pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_pkSideB);
	if (pGuild){
		retCmd.set_guildbname(pGuild->getName());
	}
	pUser->sendProtoToMe(retCmd);	
}

void CManorInfoMgr::userLeaveTeam(Player * pUser)
{
	if (!pUser)	{
		return ;
	}

	std::map<UINT32,stUserManorFightTeamInfo>::iterator it = m_userTeamInfos.find(pUser->getID());
	if (it == m_userTeamInfos.end()){
		return ;
	}

	stUserManorFightTeamInfo & teamInfo = it->second;
	std::map<UINT16,stManorInfo>::iterator subIt = m_manorInfos.find(teamInfo.m_manorID);
	if (subIt != m_manorInfos.end()){
		bool bNeedCheckFinish = false;
		if (teamInfo.m_teamID < eManorFightMaxTeam){
			if (teamInfo.m_pos < eManoFightTeamMaxMem){
				stManorFormation * pFormation = NULL;
				if (GuildProto::eFormation_Main == teamInfo.m_formation){
					pFormation = &subIt->second.m_main;
				}
				else if (GuildProto::eFormation_Left == teamInfo.m_formation){
					pFormation = &subIt->second.m_left;
				}
				else {
					pFormation = &subIt->second.m_right;
				}

				if (pFormation){
					if (pFormation->m_teamA[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos].m_id == pUser->getID()){
						pFormation->m_teamA[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos].m_id = 0;
#ifdef _HDZ_DEBUG
						Athena::logger->trace("玩家(id:%d,name:%s)退出了领地战队伍",pUser->getID(),pUser->getName());
#endif
						bNeedCheckFinish = true;
					}

					if (pFormation->m_teamB[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos].m_id == pUser->getID()){
						pFormation->m_teamB[teamInfo.m_teamID].m_userInfo[teamInfo.m_pos].m_id = 0;
#ifdef _HDZ_DEBUG
						Athena::logger->trace("玩家(id:%d,name:%s)退出了领地战队伍",pUser->getID(),pUser->getName());
#endif
						bNeedCheckFinish = true;
					}
				}
			}
		}	

		if (bNeedCheckFinish && CManorStateMgr::getSingleton().canCheckFinish()){
			checkBattleFinish(subIt->second);	
		}
	}
	UINT8 formation = teamInfo.m_formation;
	UINT8 teamID = teamInfo.m_teamID;
	m_userTeamInfos.erase(it);

	CGuild *pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		GuildProto::stNotifyLeaveManorFightWin retCmd;
		retCmd.set_charid(pUser->getID());
		retCmd.set_guildid(pGuild->getID());
		retCmd.set_formation(formation);
		retCmd.set_teamid(teamID);

		if (subIt != m_manorInfos.end()){
			CGuild * pGuildA = CGuildMgr::getSingleton().getGuildByID(subIt->second.m_pkSideA);
			if (pGuildA){
				pGuildA->sendProto2Mem(retCmd);
			}
			CGuild * pGuildB = CGuildMgr::getSingleton().getGuildByID(subIt->second.m_pkSideB);
			if (pGuildB){
				pGuildB->sendProto2Mem(retCmd);
			}
		}
	}
}

void CManorInfoMgr::giveReward()
{
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.begin();	

	for (;it != m_manorInfos.end();++it){
		stManorInfo & manorInfo = it->second;

		CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.getWinGuild());
		if (pGuild){
			std::vector<UINT32> charIDs;
			pGuild->getAllMember(charIDs);
			stBatchSendMailTask task;
			char szMailContent[50] = { 0 };
			task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_GUILD_MANOR_WAR);
			snprintf(szMailContent,sizeof(szMailContent),"MailWnd_20");
			task.m_content = szMailContent;
			task.m_attaches.push_back(std::make_pair(CGuildConfig::getSingleton().getManorWinBoxID(),1));	
			for (UINT16 j = 0;j < charIDs.size();++j){
				task.m_receiveIDs.insert(charIDs[j]);
			}
			CBatchSendMailMgr::getSingleton().addTask(task);
		}

		pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.getLoserGuild());
		if (pGuild){
			std::vector<UINT32> charIDs;
			pGuild->getAllMember(charIDs);
			stBatchSendMailTask task;
			char szMailContent[50] = { 0 };
			task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_GUILD_MANOR_WAR);
			snprintf(szMailContent,sizeof(szMailContent),"MailWnd_21");
			task.m_content = szMailContent;
			task.m_attaches.push_back(std::make_pair(CGuildConfig::getSingleton().getManorLoserBoxID(),1));	
			for (UINT16 j = 0;j < charIDs.size();++j){
				task.m_receiveIDs.insert(charIDs[j]);
			}
			CBatchSendMailMgr::getSingleton().addTask(task);
		}
	}
}

void CManorInfoMgr::checkBattleFinish(stManorInfo & manorInfo)
{
	manorInfo.m_left.calcBattleFinish(manorInfo.m_manorID);
	manorInfo.m_main.calcBattleFinish(manorInfo.m_manorID);
	manorInfo.m_right.calcBattleFinish(manorInfo.m_manorID);

	if (manorInfo.m_left.isBattleFinish() && manorInfo.m_main.isBattleFinish() && manorInfo.m_right.isBattleFinish() && (!manorInfo.m_bFinish)){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("检查到领地战结束");
#endif
		manorInfo.m_bFinish = true;
		calcResult1(manorInfo);	

		if (manorInfo.m_ownerGuildID > 0){//有胜利者
			CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_ownerGuildID);
			if (pGuild){
				GuildProto::stNotifyWinerAfterApplyEnd cmd;
				cmd.set_guildid(manorInfo.m_ownerGuildID);
				cmd.set_manorid(manorInfo.m_manorID);
				pGuild->sendProto2Mem(cmd);
			}

			pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_loserGuildID);
			if (pGuild){
				GuildProto::stNotifyManorFightFail cmd;
				cmd.set_manorid(manorInfo.m_manorID);
				pGuild->sendProto2Mem(cmd);
			}
		}
		else {//没有胜利者
			GuildProto::stManorFightFinish cmd;
			cmd.set_manorid(manorInfo.m_manorID);

			CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_drawGuildID1);
			if (pGuild){
				pGuild->sendProto2Mem(cmd);
			}

			pGuild = CGuildMgr::getSingleton().getGuildByID(manorInfo.m_drawGuildID2);
			if (pGuild){
				pGuild->sendProto2Mem(cmd);
			}
		}
	}
	else {
#ifdef _HDZ_DEBUG
		Athena::logger->trace("检查到领地战未结束!");
#endif
	}

}

void CManorInfoMgr::finishFight(Global::stFinishManorFight * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	std::map<UINT16,stManorInfo>::iterator it = m_manorInfos.find(recvCmd->manorid());	

	if (it == m_manorInfos.end()){//找不到
		return ;
	}

	if (recvCmd->teamaid() >= eManorFightMaxTeam){
		return ;
	}

	if (recvCmd->teambid() >= eManorFightMaxTeam){
		return ;
	}

	stManorFormation * pFormation = NULL;

	if (GuildProto::eFormation_Main == recvCmd->fomation()){
		pFormation = &it->second.m_main;
	}
	else if (GuildProto::eFormation_Left == recvCmd->fomation()){
		pFormation = &it->second.m_left;
	}
	else {
		pFormation = &it->second.m_right;
	}

	if (!pFormation){
		return ;
	}

	pFormation->m_teamA[recvCmd->teamaid()].m_bEnter = false;
	pFormation->m_teamB[recvCmd->teambid()].m_bEnter = false;

	std::map<UINT32,UINT32> charID2HP;
	for (UINT16 i = 0;i < recvCmd->userinfo_size();++i){
		const Global::stManorFightBattleProp & prop = recvCmd->userinfo(i);
		charID2HP[prop.charid()] = prop.curhp();
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("领地战结束了收到GameServer来的数据:");
#endif
	for (UINT16 i = 0;i < eManoFightTeamMaxMem;++i){
		UINT32 charIDA = pFormation->m_teamA[recvCmd->teamaid()].m_userInfo[i].m_id;
		UINT32 charIDB = pFormation->m_teamB[recvCmd->teambid()].m_userInfo[i].m_id;

		std::map<UINT32,UINT32>::iterator it = charID2HP.find(charIDA);
		if (it != charID2HP.end()){
#ifdef _HDZ_DEBUG
			UINT32 oldHP = pFormation->m_teamA[recvCmd->teamaid()].m_userInfo[i].m_curHP;
#endif
			pFormation->m_teamA[recvCmd->teamaid()].m_userInfo[i].m_curHP = it->second;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("更新了领地:manorID:%d,formation:%d,teamA:%d,userIndex:%d,userID:%d,oldHP:%d,curHP:%d",recvCmd->manorid(),recvCmd->fomation(),recvCmd->teamaid(),i,charIDA,oldHP,pFormation->m_teamA[recvCmd->teamaid()].m_userInfo[i].m_curHP);
#endif
		}

		it = charID2HP.find(charIDB);
		if (it !=  charID2HP.end()){
#ifdef _HDZ_DEBUG
			UINT32 oldHP =  pFormation->m_teamB[recvCmd->teambid()].m_userInfo[i].m_curHP;
#endif
			pFormation->m_teamB[recvCmd->teambid()].m_userInfo[i].m_curHP = it->second; 
#ifdef _HDZ_DEBUG
			Athena::logger->trace("更新了领地:manorID:%d,formation:%d,teamB:%d,userIndex:%d,userID:%d,oldHP:%d,curHP:%d",recvCmd->manorid(),recvCmd->fomation(),recvCmd->teambid(),i,charIDB,oldHP,pFormation->m_teamB[recvCmd->teambid()].m_userInfo[i].m_curHP);
#endif
		}
	}
	checkBattleFinish(it->second);
}

CManorApplyMgr::CManorApplyMgr() : CTemplateDB<stManorApplyDB,stManorApplyInfo>("MANORAPPLY",g_manorApplyTblDef)
{

}

CManorApplyMgr::~CManorApplyMgr()
{

}

void CManorApplyMgr::add2Container(stManorApplyInfo & info)
{
	m_applyInfos[info.m_manorID].insert(info);
	m_guildID2ManorIDs[info.m_guildID] = info.m_manorID;
}

void CManorApplyMgr::sendManorApplyList(Player * pUser,UINT32 manorID)
{
	if (!pUser){
		return ;
	}

	/*if (CManorStateMgr::eState_Apply != CManorStateMgr::getSingleton().getState()){
	  Athena::logger->error("[打开公会领地申请界面](name:%s,id:%d)不处于申请阶段,不能接入申请挑战",pUser->getName(),pUser->getID());
	  return ;	
	  }*/

	if (!CManorInfoMgr::getSingleton().isManorExist(manorID)){
		Athena::logger->error("(name:%s,id:%d)公会领地战请求申请列表时候出现错误,存在的领地ID:%d",pUser->getName(),pUser->getID(),manorID);
		return ;
	}

	std::set<stManorApplyInfo> manorApplySet = m_applyInfos[manorID];

	GuildProto::stApplyManorWinInfo retCmd;
	retCmd.set_manorid(manorID);
	std::set<stManorApplyInfo>::iterator subIt = manorApplySet.begin();
	for (;subIt != manorApplySet.end();++subIt){
		const stManorApplyInfo & applyInfo = *subIt;
		GuildProto::stSynManoryApplyInfo * pSyn = retCmd.add_applyinfo();

		if (pSyn){
			pSyn->set_guildid(applyInfo.m_guildID);
			CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(applyInfo.m_guildID);
			if (pGuild){
				pSyn->set_guildname(pGuild->getName());
			}
			pSyn->set_contribute(applyInfo.m_contribute);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CManorApplyMgr::refreshApplyInfo(Player * pUser,UINT32 manorID)
{
	if (!pUser){
		return ;
	}

	if (CManorStateMgr::eState_Apply != CManorStateMgr::getSingleton().getState()){
		Athena::logger->error("[打开公会领地申请界面](name:%s,id:%d)不处于申请阶段,不能接入申请挑战",pUser->getName(),pUser->getID());
		return ;	
	}

	if (!CManorInfoMgr::getSingleton().isManorExist(manorID)){
		Athena::logger->error("(name:%s,id:%d)刷新公会领地战请求申请列表时候出现错误,不存在的领地ID:%d",pUser->getName(),pUser->getID(),manorID);
		return ;
	}

	std::set<stManorApplyInfo> manorApplySet = m_applyInfos[manorID];

	GuildProto::stRetManorApplyInfo retCmd;
	retCmd.set_manorid(manorID);
	std::set<stManorApplyInfo>::iterator subIt = manorApplySet.begin();
	for (;subIt != manorApplySet.end();++subIt){
		const stManorApplyInfo & applyInfo = *subIt;
		GuildProto::stSynManoryApplyInfo * pSyn = retCmd.add_applyinfo();

		if (pSyn){
			pSyn->set_guildid(applyInfo.m_guildID);
			CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(applyInfo.m_guildID);
			if (pGuild){
				pSyn->set_guildname(pGuild->getName());
			}
			pSyn->set_contribute(applyInfo.m_contribute);
		}
	}
	pUser->sendProtoToMe(retCmd);

}

void CManorApplyMgr::reqChallengeManor(Player * pUser,UINT32 manorID,UINT32 contribute)
{
	if (!pUser){
		return ;	
	}

	std::map<UINT32, std::set<stManorApplyInfo> >::iterator it = m_applyInfos.find(manorID);

	GuildProto::stApplyManorFightRet retCmd;
	retCmd.set_manorid(manorID);
	if (it == m_applyInfos.end()){
		retCmd.set_result(GuildProto::stApplyManorFightRet::eManor_ID_Error);
		pUser->sendProtoToMe(retCmd);
		return ;	
	}

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		CGuildMember *pMember =  pGuild->getGuildMemberByID(pUser->getID());	
		if (pMember){
			if (!CGuildConfig::getSingleton().hasRight(pMember->getTitle(),CGuildConfig::eRight_Lvup)){
				retCmd.set_result(GuildProto::stApplyManorFightRet::eHas_No_Right);
				pUser->sendProtoToMe(retCmd);
				return ;	
			}	
		}
		else {
			retCmd.set_result(GuildProto::stApplyManorFightRet::eHas_No_Right);
			pUser->sendProtoToMe(retCmd);
			return ;	
		}
	}
	else {
		retCmd.set_result(GuildProto::stApplyManorFightRet::eHas_No_Right);
		pUser->sendProtoToMe(retCmd);
		return ;	
	}

	if (CManorStateMgr::eState_Apply != CManorStateMgr::getSingleton().getState()){
		retCmd.set_result(GuildProto::stApplyManorFightRet::eOut_Time);
		pUser->sendProtoToMe(retCmd);
		return ;	
	}

	std::map<UINT32, UINT32>::iterator thdIt = m_guildID2ManorIDs.find(pGuild->getID());
	if (thdIt != m_guildID2ManorIDs.end()){
		if (thdIt->second != manorID){
			retCmd.set_result(GuildProto::stApplyManorFightRet::eManor_Has_Apply);
			pUser->sendProtoToMe(retCmd);
			return ;
		}
	}

	if (!pGuild->removeContribute(contribute,"申请领地战")){
		retCmd.set_result(GuildProto::stApplyManorFightRet::eRes_Not_Enough);
		pUser->sendProtoToMe(retCmd);
		return ;	
	}


	std::set<stManorApplyInfo>::iterator subIt = it->second.begin();

	UINT32 curContribute = contribute;
	bool bUpdate = false;
	for (;subIt != it->second.end();++subIt){
		if (pGuild->getID() == subIt->m_guildID){	
			curContribute = subIt->m_contribute;
			curContribute += contribute;
			it->second.erase(subIt);
			bUpdate = true;
			break;
		}
	}

	stManorApplyInfo applyInfo;
	applyInfo.m_guildID = pGuild->getID();
	applyInfo.m_manorID = manorID;
	applyInfo.m_contribute = curContribute;
	applyInfo.m_createTime = g_globalServer->getSec();

	stManorApplyDB dbData;
	applyInfo.fillDBData(dbData);

	if (bUpdate){
		char szWhere[65] = { 0 };
		snprintf(szWhere,sizeof(szWhere)," GUILDID=%d ",pGuild->getID());
		updateRecord(dbData,szWhere);
	}
	else {
		addRecord(dbData);	
	}

	m_applyInfos[manorID].insert(applyInfo);
	m_guildID2ManorIDs[applyInfo.m_guildID] = manorID;

	retCmd.set_result(GuildProto::stApplyManorFightRet::eSuccess);
	retCmd.set_contribute(curContribute);
	retCmd.set_guildid(pGuild->getID());
	pUser->sendProtoToMe(retCmd);
}

void CManorApplyMgr::endApply()
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("申请状态结束!进入了准备状态!");
#endif
	std::map<UINT32, std::set<stManorApplyInfo> >::iterator it = m_applyInfos.begin(); 

	for (;it != m_applyInfos.end();++it){
		if (it->second.size() >= 2){//有两人申请的情况
			std::set<stManorApplyInfo>::iterator subIt = it->second.begin();
			UINT32 sideA = 0;
			UINT32 sideB = 0;
			if (subIt != it->second.end()){
				sideA = subIt->m_guildID;
			}

			++subIt;
			if (subIt != it->second.end()){
				sideB = subIt->m_guildID;
			}
			CManorInfoMgr::getSingleton().setPKGuildID(it->first,sideA,sideB);
			++subIt;
			for (;subIt != it->second.end();++subIt){
				m_retContributeGuilds.insert(make_pair(subIt->m_guildID,subIt->m_contribute));
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("公会领地由:guildID:%d,guildID:%d 争夺",sideA,sideB);
#endif
		}
		else if (it->second.size() >= 1){//有一个人申请情况
			std::set<stManorApplyInfo>::iterator subIt = it->second.begin();
			UINT32 sideA = 0;
			if (subIt != it->second.end()){
				sideA = subIt->m_guildID;
			}

			CManorInfoMgr::getSingleton().setOwner(it->first,sideA);

			CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(sideA);
			if (pGuild){
				GuildProto::stNotifyWinerAfterApplyEnd cmd;
				cmd.set_guildid(sideA);
				cmd.set_manorid(it->first);
				pGuild->sendProto2Mem(cmd);
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("公会领地:%d 由:guildID:%d,争夺,设置sideA为胜利者",it->first,sideA);
#endif
		}
		else {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("公会领地:%d没有人争夺领地",it->first);
#endif
		}
	}	
}

void CManorApplyMgr::returnContribute()
{
	if (m_retContributeGuilds.size() <= 0){
		return;
	}	

	std::map<UINT32,UINT32>::iterator it = m_retContributeGuilds.begin();
	if (it != m_retContributeGuilds.end()){
		CGuild * pGuild = CGuildMgr::getSingleton().getGuildByID(it->first);
		if (pGuild){
			pGuild->addContribute(it->second,"公会领地战返还");		
		}
		m_retContributeGuilds.erase(it);
	}
}

UINT32 CManorApplyMgr::getApplyManorID(UINT32 guildID)
{
	std::map<UINT32, UINT32>::iterator it = m_guildID2ManorIDs.find(guildID);	

	if (it != m_guildID2ManorIDs.end()){
		return it->second;
	}
	return 0;
}
