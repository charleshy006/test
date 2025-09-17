#include "CloseBetaWelfareMgr.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "GlobalServer.h"
#include "TblCloseBetaWelfare.h"
#include "TblDataMgr.h"
#include "Role.pb.h"
#include "Player.h"
#include "RoleInfoManager.h"

extern GlobalServer * g_globalServer;

CTblField g_closeBetaReward[] = {
	{ "ACCOUNT",           CTblField::DB_TYPE_STRING,  50 },
	{ NULL, 0, 0 },
};

void stCloseBetaRewardInfo::initFromDB(const stCloseBetaRewardDB & data)
{
	m_account = data.m_account;	
}

void stCloseBetaRewardInfo::fillDBData(stCloseBetaRewardDB & data)
{
	strncpy(data.m_account,m_account.c_str(),sizeof(data.m_account));
}

void stCloseBetaRewardInfo::print()
{
	Athena::logger->trace("已经领取奖励的账号:%s",m_account.c_str());
}

CCloseBetaWelfareMgr::CCloseBetaWelfareMgr():CTemplateDB<stCloseBetaRewardDB,stCloseBetaRewardInfo>("CLOSEBETAREWARD",g_closeBetaReward) 
{
}

CCloseBetaWelfareMgr::~CCloseBetaWelfareMgr()
{

}

struct stCloseBetaRewardParseCB : public TblDatabaseCB<stCloseBetaWelfareBase>
{
	stCloseBetaRewardParseCB()
	{}

	~stCloseBetaRewardParseCB()
	{}

	virtual bool doit(stCloseBetaWelfareBase * pBase)
	{
		if (!pBase){
			return true;
		}
		CloseBetaRechargeInfo * pInfo = ATHENA_NEW CloseBetaRechargeInfo();
		pInfo->m_account = pBase->m_account;
		pInfo->m_boxID = pBase->m_boxID;
		pInfo->m_giveDiamond  = pBase->m_giveDiamond;
		pInfo->m_giveRechargeNum   = pBase->m_giveRechargeNum;
		pInfo->m_serverID    = pBase->m_serverID;
		CCloseBetaWelfareMgr::getSingleton().addRechargeAccount(pInfo);
		return true;
	}
};

void CCloseBetaWelfareMgr::loadTblInfo()
{
	stCloseBetaRewardParseCB cb;
	g_closeBetaWelfareBaseMgr.execEveryOne(cb);
	Athena::logger->trace("加载内侧账号数量:%d",m_rechargeAccount.size());
}

void CCloseBetaWelfareMgr::userOnline(Player * pUser)
{
	if (!pUser){
		return;
	}
	
	RechargeAccount_It it = m_rechargeAccount.find(pUser->getAccount());

	if ((it != m_rechargeAccount.end())
			&& (m_rewardAccount.find(pUser->getAccount()) == m_rewardAccount.end())){
		UINT32 serverID = (UINT32)atoi(Athena::global["serverID"].c_str());
		if (serverID != it->second->m_serverID){
			return ;
		}
		
		UINT32 firstCharID = RoleInfoManager::getSingleton().getFirstCreateCharID(pUser->getAccount());
		if (firstCharID != pUser->getID()){//不是第一个账号
			return ;
		}

		Role::stHasCloseBetaReward retCmd;
		retCmd.set_boxid(it->second->m_boxID);
		retCmd.set_givediamond(it->second->m_giveDiamond);
		retCmd.set_giverecharge(it->second->m_giveRechargeNum);
		pUser->sendProtoToMe(retCmd);
	}
}

void CCloseBetaWelfareMgr::add2Container(stCloseBetaRewardInfo & info)
{
	m_rewardAccount.insert(info.m_account);
}

void CCloseBetaWelfareMgr::addRechargeAccount(CloseBetaRechargeInfo * pRechargeInfo)
{
	if (!pRechargeInfo){
		return ;
	}
	m_rechargeAccount[pRechargeInfo->m_account] = pRechargeInfo;
}

void CCloseBetaWelfareMgr::giveReward(Player *pUser)
{
	if (!pUser){
		return;
	}

	Role::stGotCloseBetaRewardResult retCmd;

	RechargeAccount_It it = m_rechargeAccount.find(pUser->getAccount());
	if (it == m_rechargeAccount.end()){
		retCmd.set_result(Role::stGotCloseBetaRewardResult::eHas_Not_Reward);
		pUser->sendProtoToMe(retCmd);
		return;
	}

	RewardAccount_It subIt = m_rewardAccount.find(pUser->getAccount());

	if (subIt != m_rewardAccount.end()){
		retCmd.set_result(Role::stGotCloseBetaRewardResult::eHas_Got_Reward);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	
	UINT32 serverID = (UINT32)atoi(Athena::global["serverID"].c_str());
	if (serverID != it->second->m_serverID){
		Athena::logger->error("不是自己获得奖励的服不能领取");
		return ;
	}

	stCloseBetaRewardInfo info;
	info.m_account = pUser->getAccount();
	stCloseBetaRewardDB dbData;
	info.fillDBData(dbData);
	addRecord(dbData);
	m_rewardAccount.insert(pUser->getAccount());

	Global::stNotifyCloseBetaReward cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_boxid(it->second->m_boxID);
	cmd.set_givediamond(it->second->m_giveDiamond);
	cmd.set_giverecharge(it->second->m_giveRechargeNum);
	pUser->sendProto2Game(cmd);
}
