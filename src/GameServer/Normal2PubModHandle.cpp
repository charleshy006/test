#include "Normal2PubModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "MapModHandle.h"
#include "Map.pb.h"
#include "RoleModHandle.h"
#include "MiscModHandle.h"
#include "GuildModHandle.h"
#include "TaskModHandle.h"
#include "SortListModHandle.h"
#include "Map.h"
#include "MapMgr.h"
#include "Team.h"
#include "TeamModHandle.h"
#include "HeroLeague.h"

extern GameServer * g_gameServer;

Normal2PubModHandle::Normal2PubModHandle()
{
	m_protoModHandles[Proto::MAP_MOD] =  ATHENA_NEW MapModHandle();
	m_protoModHandles[Proto::ROLE_MOD] = ATHENA_NEW RoleModHandle();
	m_protoModHandles[Proto::MISC_MOD]  = ATHENA_NEW MiscModHandle();
	m_protoModHandles[Proto::MISC2_MOD]  = ATHENA_NEW MiscModHandle();
	m_protoModHandles[Proto::GUILD_MOD] = ATHENA_NEW GuildModHandle();
	m_protoModHandles[Proto::TASK_MOD] = ATHENA_NEW TaskModHandle();
	m_protoModHandles[Proto::SORTLIST_MOD] = ATHENA_NEW SortListModHandle();
	m_protoModHandles[Proto::TEAM_MOD] = ATHENA_NEW TeamModHandle();

	registerHandle(&Normal2PubModHandle::loginPubGame);
	registerHandle(&Normal2PubModHandle::normalForwardPub);
	registerHandle(&Normal2PubModHandle::getRoleInfo);
	registerHandle(&Normal2PubModHandle::logout);
	registerHandle(&Normal2PubModHandle::realTimeTeamMatch);

	registerHandle(&Normal2PubModHandle::reqReqMenberInfo);
	registerHandle(&Normal2PubModHandle::reqQualifiedList);
	registerHandle(&Normal2PubModHandle::reqHeroLeagueMatch);
	registerHandle(&Normal2PubModHandle::reqcancelMatch);
}

Normal2PubModHandle::~Normal2PubModHandle()
{

}

bool Normal2PubModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<Normal2PubModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void Normal2PubModHandle::loginPubGame(TCPComponent * pCom,PubGame::stLoginPubGame * loginCmd)
{
	Athena::logger->trace("NormalGame登录校验开始 svrIndex[%u] ip[%s] pCom[%p]",loginCmd->svrid(),loginCmd->ip().c_str(),pCom);
	if (g_gameServer->getPubServerKey() != loginCmd->key())
	{
		Athena::logger->trace("NormalGame登录校验不通过 ip[%s] key[%s]",loginCmd->ip().c_str(),loginCmd->key().c_str());
		pCom->close();
		return;
	}
	SvrInfo info;
	info.m_svrID = loginCmd->svrid();
	info.m_svrType = loginCmd->svrtype();
	info.m_port = loginCmd->port();
	strcpy(info.m_ip, loginCmd->ip().c_str()); 
	pCom->setSvrInfo(info);

	if (g_gameServer->getConnMgr().verifyComponent(pCom, info))
	{
		Athena::logger->trace("NormalGame登录校验通过 svrIndex[%u] ip[%s]",loginCmd->svrid(),loginCmd->ip().c_str());
	}
	else
	{
		Athena::logger->trace("NormalGame登录校验未通过 svrIndex[%u] ip[%s]",loginCmd->svrid(),loginCmd->ip().c_str());
		return;
	}
}

void Normal2PubModHandle::getRoleInfo(TCPComponent * pCom,PubGame::stRoleInfo * recvCmd)
{
	if (!recvCmd) {
		return ;
	}

	RoleInfoForSave	 forSave;
	if (recvCmd->datasize() < sizeof(RoleInfo)){
		Athena::logger->error("玩家account=%s RoleInfoForSave长度异常",recvCmd->account().c_str());
		return ;
	}

	bcopy(recvCmd->data().data(),&forSave.m_roleInfo,sizeof(RoleInfo));

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		pUser->unreg();
		SAFE_DELETE(pUser);
	}
	pUser = ATHENA_NEW GameUser(recvCmd->account().c_str(),0,recvCmd->ip().c_str());
	pUser->setID(recvCmd->charid());
	pUser->setName(forSave.m_roleInfo.m_name);

	INT32 binSize = recvCmd->datasize() - sizeof(RoleInfo) ;
	if (binSize > 0) {
		bcopy(recvCmd->data().data() + sizeof(RoleInfo),forSave.m_binData,binSize);
	}
	else {
		binSize = 0;
	}

	if (!pUser->initData(forSave.m_roleInfo,forSave.m_binData,binSize)){
		Athena::logger->error("初始化玩家数据失败name=%s,account=%s,id=%d",pUser->getName(),pUser->m_account,pUser->getID());
		return;	
	}
	pUser->m_titleMgr.parse(recvCmd->titles());
	//上阵随从羁绊生效
	for(UINT8 i = 0; i < pUser->m_heroMgr.s_MAX_BATTLE_HERO_NUM; ++i){
		pUser->m_heroMgr.refreshImpactHero(pUser->m_heroMgr.getBattleHeroID(i), true, true);
	}
	//用uniquePubID代替charid
	pUser->m_roleInfo.m_charID = recvCmd->charid();
	pUser->setNormalServerID(recvCmd->normalserverid());
	pUser->setNormalCharID(recvCmd->normalcharid());
	pUser->setAreaCode(recvCmd->areacode());
    UINT32 mapId = 0;
    if (recvCmd->mapid() <= 5) {
        pUser->m_lastIntoMapID = recvCmd->mapid();
        mapId = recvCmd->mapid();
    } else {
        mapId = NEW_PERSON_MAPID;
    }

	pUser->setupRoleState(true);

	if (!GameUserManager::getSingleton().addUser(pUser)){
#ifdef _THAILAND_VER
		char temp[200];
#else
		char temp[60];
#endif
		bzero(temp,sizeof(temp));
		snprintf(temp,sizeof(temp),"添加到玩家容器里失败(id=%d,name=%s)",pUser->getID(),pUser->getName());
		Athena::logger->trace("%s",temp);
		SAFE_DELETE(pUser);
		return ;
	}
	CMap* newMap = CMapMgr::getSingleton().getMapByID(mapId);//NEW_PERSON_MAPID);
	if (!pUser->intoNewMap(newMap,true,false)){
		Athena::logger->error("(account=%s,name=%s,charID=%d)进入地图失败!",pUser->getAccount(),pUser->getName(),recvCmd->normalcharid());
		GameUserManager::getSingleton().removeUser(pUser);
		SAFE_DELETE(pUser);
		return;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("AllUserCnt : %d",GameUserManager::getSingleton().userNum());
#endif
	Athena::logger->trace("同步玩家数据成功 charid[%u] name[%s] normalcharid[%u] normalserverid[%u]",recvCmd->charid(),pUser->getName(),pUser->getNormalCharID(),pUser->getNormalServerID());
}

//logout
void Normal2PubModHandle::logout(TCPComponent * pCom,PubGame::stRoleLogout * recvCmd)
{
	if (!pCom || !recvCmd) {
		return ;
	}
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		CTeamMgr::getSingleton().userOffline(pUser);
		CTeamMatchMgr::getSingleton().userOffline(pUser);
		
		Athena::logger->trace("玩家离线 charid[%d] normalserverid[%u]",pUser->getNormalCharID(),pUser->getNormalServerID());
	
		pUser->unreg();
		SAFE_DELETE(pUser);
	}

	//英雄联赛
	CHeroLeagueMgr::getSingleton().removeOtherModel(recvCmd->charid()); 
}

void Normal2PubModHandle::normalForwardPub(TCPComponent * pCom,PubGame::stNormalForwardPub * recvCmd)
{
	if (!pCom || !recvCmd) {
		return ;
	}
// #ifdef _HDZ_DEBUG
// 		Athena::logger->trace("normalGameServer发过来的协议是modID=%d,funID=%d",recvCmd->modid(),recvCmd->funid());
// #endif
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		GameProtoMsgHandleBase  * pHandle = m_protoModHandles[recvCmd->modid()];
		if (pHandle) {
			GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
			if (pUser) {
				pHandle->handle(pUser,recvCmd->data().data(),recvCmd->cmdlen(),recvCmd->funid());			
			}
		}
	}else{
		Athena::logger->error("[未找到玩家]normalGameServer[%d]发过来的协议是modID=%d,funID=%d charid=%d",pCom->getSvrInfo().m_svrID,recvCmd->modid(),recvCmd->funid(),recvCmd->charid());
	}
}
void Normal2PubModHandle::realTimeTeamMatch(TCPComponent * pCom,PubGame::stRealTimeTeamMatch * recvCmd)
{
	if (!pCom || !recvCmd) {
		return ;
	}
	if (recvCmd->memberid_size() < 1)
	{
		return;
	}
	//取队长，建队伍 
	GameUser * pLeader = GameUserManager::getSingleton().getUser(recvCmd->memberid(0));
	if (!pLeader)
	{
		return;
	}

	CTeam * pTeam   = ATHENA_NEW CTeam();
	if (!pTeam){
		return;
	}
	UINT32 copyMapID = 0;
	pTeam->setID(recvCmd->teamid());			
	if (!pTeam->initTeam(pLeader,copyMapID)){
		SAFE_DELETE(pTeam);
		return;
	}
	CTeamMgr::getSingleton().addCharID2Team(pLeader->getID(),pTeam->getID());
	CTeamMgr::getSingleton().addTeam(pTeam);

	Athena::logger->trace("realTimeTeamMatch leader[%u] name=%s normalcharid[%u] normalserverid[%u]",pLeader->getID(), pLeader->getName(),pLeader->getNormalCharID(),pLeader->getNormalServerID());

	//处理队员
	for (int i = 1; i < recvCmd->memberid_size(); ++i)
	{
		UINT32 memberID = recvCmd->memberid(i);
		GameUser * pUser = GameUserManager::getSingleton().getUser(memberID);
		if (!pUser)
		{
			continue;
		}
		pTeam->joinInTeam(pUser,i);
		Athena::logger->trace("realTimeTeamMatch member[%u] name=%s normalcharid[%u] normalserverid[%u]",pUser->getID(), pUser->getName(),pUser->getNormalCharID(),pUser->getNormalServerID());
	}

	if (PubGame::eRealTimePKType_KillAll == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().addMatchTeam(pLeader,pTeam,Global::eRealTimePKType_KillAll);
	}
	else if (PubGame::eRealTimePKType_Finish == recvCmd->etype()){
		CTeamMatchMgr::getSingleton().addMatchTeam(pLeader,pTeam,Global::eRealTimePKType_Finish);
	}
}

void Normal2PubModHandle::reqHeroLeagueMatch(TCPComponent * pCom,PubGame::stHeroLeagueMatch * recvCmd)
{
	if(!pCom || !recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid()); 
	if(!pUser){
		return ;
	}
	Athena::logger->trace("reqHeroLeagueMatch charid[%u] name=%s normalcharid[%u] normalserverid[%u]",recvCmd->charid(), pUser->getName(),pUser->getNormalCharID(),pUser->getNormalServerID());
	CHeroLeagueMgr::getSingleton().addMatchUser(pUser);
}

void Normal2PubModHandle::reqcancelMatch(TCPComponent * pCom,PubGame::stHeroLeagueCancelMatch *recvCmd)
{
	if(!recvCmd){
		return;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if(!pUser){                                         
		return ;                                        
	}
	Athena::logger->trace("reqcancelMatch charid[%u] name=%s normalcharid[%u] normalserverid[%u]",recvCmd->charid(), pUser->getName(),pUser->getNormalCharID(),pUser->getNormalServerID());
	CHeroLeagueMgr::getSingleton().cancelMatch(pUser); 
}

void Normal2PubModHandle::reqQualifiedList(TCPComponent * pCom,PubGame::stSynPubRoleListResult *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	CHeroLeagueMgr::getSingleton().updateWaitCharIdsMap(recvCmd);
}

void Normal2PubModHandle::reqReqMenberInfo(TCPComponent * pCom,PubGame::stReqMenberInfo * recvCmd)
{
	if(!recvCmd){
		return ;
	}
	CHeroLeagueMgr::getSingleton().sendMenberInfo(recvCmd->charid());	

}
