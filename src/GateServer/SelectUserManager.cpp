#include "SelectUserManager.h"
#include "GateUserManager.h"
#include "Log4cxxHelp.h"
#include "Role.pb.h"


GateUser* SelectUserManager::getUserByAccount(std::string account)
{
	GateUser* ret = NULL;
	Acc2GateUserMap_IT  it = m_acc2GateUser.find(account);
	if (it != m_acc2GateUser.end()){
		ret = it->second;
	}
	return ret;
}

void SelectUserManager::removeUserByAccount(std::string account)
{
	Acc2GateUserMap_IT  it = m_acc2GateUser.find(account);
	if (it != m_acc2GateUser.end()){
		m_acc2GateUser.erase(it);
	}
}

bool SelectUserManager::addUserByAccount(GateUser* pUser)
{
	Acc2GateUserMap_IT  it = m_acc2GateUser.find(pUser->m_account);
	if (it != m_acc2GateUser.end()){
		return false;
	}
	m_acc2GateUser.insert(std::make_pair(pUser->m_account,pUser));
	return true;
}


bool SelectUserManager::loginGameResult(std::string account,UINT16 gameID,byte ret)
{
	Acc2GateUserMap_IT  it = m_acc2GateUser.find(account);
	if (it != m_acc2GateUser.end()){
		GateUser * pUser = it->second;
		m_acc2GateUser.erase(it);
		if (!ret){//成功
			pUser->beginGame();
			pUser->setGameID(gameID);
			if (GateUserManager::getSingleton().addUser(pUser)){
#ifdef _HDZ_DEBUG
				Athena::logger->trace("account=%s登录游戏成功!",account.c_str());
#endif
				return true;
			}
		}
		pUser->unreg();
		pUser->terminate();
		SAFE_DELETE(pUser);
	}
	Athena::logger->error("[角色进入游戏]进入场景失败!");
	return false;
}

void SelectUserManager::kickUser(std::string account)
{
	Acc2GateUserMap_IT  it = m_acc2GateUser.find(account);
	if (it != m_acc2GateUser.end()){
		GateUser * pUser = it->second;
		pUser->unreg();
		pUser->terminate();
		SAFE_DELETE(pUser);
	}
}

void SelectUserManager::refreshGateRoleInfo(std::string account,Global::stRetSnapData * recvCmd)
{
	Athena::logger->trace("去网关user找数据! account=%s", account.c_str());

	Acc2GateUserMap_IT  it = m_acc2GateUser.find(account);
	if (it != m_acc2GateUser.end()){
		GateUser * pUser = it->second;
		Role::RefreshSnapData cmd;
		cmd.set_flag(1);
		for (UINT16 i = 0;i < recvCmd->data_size();++i) {
			Role::stSynSnapData * pSyn = cmd.add_data();
			if (pSyn){
				stGateRoleInfo & roleInfo = pUser->m_gateRoleInfo[recvCmd->data(i).charid()];
				strncpy(roleInfo.m_account,recvCmd->data(i).account().c_str(),sizeof(roleInfo.m_account));
				roleInfo.m_charID = recvCmd->data(i).charid();
				strncpy(roleInfo.m_name,recvCmd->data(i).name().c_str(),sizeof(roleInfo.m_name));
				roleInfo.m_level = recvCmd->data(i).level();
				roleInfo.m_job = recvCmd->data(i).job();
				roleInfo.m_sex = recvCmd->data(i).sex();
				roleInfo.m_serverID = recvCmd->data(i).serverid();

				pSyn->set_charid(recvCmd->data(i).charid());
				pSyn->set_name(recvCmd->data(i).name());
				pSyn->set_level(recvCmd->data(i).level());
				pSyn->set_job(recvCmd->data(i).job());
				pSyn->set_sex(recvCmd->data(i).sex());
			}
		}
		std::string retStr;
		cmd.SerializeToString(&retStr);
		pUser->sendProto(retStr.data(),retStr.size(),cmd.ModID,cmd.FunID);

		Athena::logger->trace("给用户发送选择界面数据! account=%s", account.c_str());

	}
}

void SelectUserManager::checkTime(UINT32 cur)
{
	std::vector<GateUser * > delUser;
	Acc2GateUserMap_IT  it = m_acc2GateUser.begin();
	for (;it != m_acc2GateUser.end() ;++it){
		GateUser * pUser = it->second;
		if (pUser->m_enterSelectTime + 600 < cur){//超过了5分钟
			delUser.push_back(pUser);
		}
	}

	for (UINT16 i = 0 ;i < delUser.size();++i){
		removeUserByAccount(delUser[i]->m_account);
		delUser[i]->unreg();
		delUser[i]->terminate();
		SAFE_DELETE(delUser[i]);
	}
}
