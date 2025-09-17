#include "Player.h"
#include "PlayerManager.h"
#include "Log4cxxHelp.h"
#include "Time.h"
#include "PacketProcessor.h"
#include "GlobalServer.h"
#include "Mail.h"
#include "Team.h"
#include "Guild.h"
#include "Arena.h"
#include "WorldBoss.h"
#include "TblDataMgr.h"
#include "GuardMgr.h"

GlobalOpenFunction::GlobalOpenFunction(Player * pUser) : m_pOwner(pUser)
{}

GlobalOpenFunction::~GlobalOpenFunction()
{}

UINT16 GlobalOpenFunction::getLevel()
{
	return m_pOwner->m_level;
}

UINT32 Player::m_userCount = 0;

Player::Player(UINT16 gateID,const std::string account,const char * ip) : m_friendMgr(this),m_openFuncMgr(this)
{
	m_account = account;
	if (ip){
		strncpy(m_ip,ip,sizeof(m_ip));
	}
	m_gateID = gateID;	
	m_gameID = 0;
	m_gmType = 0;
	m_level = 0;
	m_job = 0;
	m_sex = 0;
	m_loginTime = g_globalServer->getSec();
	m_maxHP = 0;
	m_battlePower = 0;
	m_fetchLilianNum = 0;
	m_toGameSvrID = 0;
	m_serverID = 0;
	m_headID = 0;						//头部ID
	m_bodyID = 0;						//身体ID
	m_weaponID = 0;						//武器ID
	m_cloackID = 0;						//披风ID
	m_wingID  = 0;						//翅膀ID
	m_shoeID = 0;						//鞋子ID
	m_fashionHairID = 0;				//时装头发
	m_fashionFaceID = 0;				//时装脸ID
	m_fashionBodyID = 0;				//时装身体
	m_fashionWeaponID = 0;				//时装武器
	m_fashionShoeID = 0;				//时装鞋子
	m_fashionDecorationID = 0;			//时装饰品
	m_opID = 0;
	m_compensationID = 0;
	m_effectID = 0;
	m_interceptID = 0;
	m_wingBreakLv = 0;
	m_changeNameNum = 0;
	m_pubServerIndex = 0;
	m_pubCharID = 0;
	m_areaCode = 0;
	m_photoID = 0;

	Athena::logger->debug("Player目前在线人数: %u", ++m_userCount);
}

Player::~Player()
{
	Athena::logger->debug("Player目前在线人数: %u", --m_userCount);
}

bool Player::init(const RoleInfo & roleInfo)
{
	return true;
}

void Player::unreg()
{
	MailManager::getSingleton().deleteHasReadMail(getID());
	m_friendMgr.userOffline();
	CTeamMgr::getSingleton().userOffline(this);
	CGuildMgr::getSingleton().userOffline(this);
	CTeamMgr::getSingleton().userOffline(this);
	CWorldBossMgr::getSingleton().userOffline(this);
	CGuardMgr::getSingleton().userOffline(this);
	CManorInfoMgr::getSingleton().userLeaveTeam(this);
}

void Player::timerUpdate(UINT32 cur)
{
	
}

void Player::setIP(const char * szIP)
{
	if (!szIP){
		return ;
	}
	strncpy(m_ip,szIP,sizeof(m_ip));
}

UINT32 Player::getCountByVip(UINT16 countType)
{
	stVipBase * pVipBase = NULL;
	if (g_globalServer->isAppVersion()){
		pVipBase = g_vipBaseMgr.get(countType * VIP_NUM_COEF + 15); 
	}    
	else {
		pVipBase = g_vipBaseMgr.get(countType * VIP_NUM_COEF + m_vipLv);
	}    

	if (!pVipBase){
		return 0;
	}    
	return pVipBase->m_num;
}
