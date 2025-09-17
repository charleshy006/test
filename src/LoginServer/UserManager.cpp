#include "UserManager.h"
#include "MysqlDatabase.h"
#include "LoginServer.h"
#include "TblQueryResult.h"
#include "Log4cxxHelp.h"
#include  "LoginServer.h"
#include <tinyxml/tinyxml.h>

extern LoginServer * g_loginServer;

CTblField g_selectUserInfoDef[] = {
	{ "USERID",        	 	CTblField::DB_TYPE_UINT32,     sizeof(UINT32)   },
	{ "USERNAME",       	CTblField::DB_TYPE_STRING,     48 				},
	{ "PASSWORD",       	CTblField::DB_TYPE_STRING,     33 				},
	{ "CREATETIME",			CTblField::DB_TYPE_UINT32,	   sizeof(UINT32)	},
	{ "LASTLOGINTIME",      CTblField::DB_TYPE_UINT32,     sizeof(UINT32)   },
	{ NULL, 0, 0 },
};

void UserManager::load()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/ZoneList.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[区配置]打开配置文件%s错误",fileName.c_str());
		return ;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("Root");
	if (!pRoot){
		Athena::logger->error("[区配置]区配置加载错误,找不到Root节点!");
		return ;
	} 

	TiXmlElement * pSpecialAccountNode = pRoot->FirstChildElement("SpecialAccount");
	if (pSpecialAccountNode){
		const char * szVal = NULL;
		int iVal = 0;
		TiXmlElement * pAccountNode = pSpecialAccountNode->FirstChildElement("Account");
		while (pAccountNode){
			stSpecialAccountInfo accountInfo;
			if ((szVal = pAccountNode->Attribute("name"))){
				accountInfo.m_account = szVal;
			}

			if ((szVal = pAccountNode->Attribute("ip"))){
				accountInfo.m_ip = szVal;
			}

			if ((szVal = pAccountNode->Attribute("device"))){
				accountInfo.m_device = szVal;
			}

			if (pAccountNode->Attribute("opID",&iVal)){
				accountInfo.m_opID = iVal;
			}
			m_specialAccounts[accountInfo.m_account] = accountInfo;
			pAccountNode = pAccountNode->NextSiblingElement("Account");
		}
	}
	
	{
		std::map<std::string,stSpecialAccountInfo>::iterator it =   m_specialAccounts.begin();
		for (;it != m_specialAccounts.end();++it){
			stSpecialAccountInfo & info = it->second;
			Athena::logger->trace("特殊账号:name:%s,opID:%d,device:%s,ip:%s",info.m_account.c_str(),info.m_opID,info.m_device.c_str(),info.m_ip.c_str());	
		}
	}
}

bool UserManager::loginVerify(LoginInfo & info,const char * account,const char * password)
{
	if (m_specialAccounts.find(account) != m_specialAccounts.end()){
		return true;
	}
	char where[128] = { 0 };
	bzero(where, 128);
	snprintf(where, 128, "USERNAME='%s' and PASSWORD = '%s' ", account,password);
	CTblQueryResult * result = g_databaseEngine.select(g_selectUserInfoDef,"LOGININFO",where,NULL,0,NULL);
	if (result && result->getData()){
		bcopy(result->getData(),&info,sizeof(info));
		SAFE_DELETE(result);
		return true;
	}   
	else {
		SAFE_DELETE(result);
		return false;
	}   
}

bool UserManager::findUserByAccount(LoginInfo & info,const char * account)
{
	char where[128] = { 0 };
	bzero(where, 128);
	snprintf(where, 128, "USERNAME='%s' ", account);
	CTblQueryResult * result = g_databaseEngine.select(g_selectUserInfoDef,"LOGININFO",where,NULL,0,NULL);
	if (result && result->getData()){
		bcopy(result->getData(),&info,sizeof(info));
		SAFE_DELETE(result);
		return true;
	}   
	else {
		SAFE_DELETE(result);
		return false;
	} 
}

bool UserManager::createAccount(const char * account,const char * password,const char * machineID,const char * country)
{
	if (!account || !password || !machineID || !country){
		return false;
	}
	
	LoginInfo  loginInfo;
	loginInfo.m_bind = 1;
	loginInfo.m_createTime = g_loginServer->getSec(); 
	loginInfo.m_lastLoginTime = 0;
	strncpy(loginInfo.m_account,account,sizeof(loginInfo.m_account));
	strncpy(loginInfo.m_tmpAccount,account,sizeof(loginInfo.m_tmpAccount));
	strncpy(loginInfo.m_passwd,password,sizeof(loginInfo.m_passwd));
	loginInfo.m_userID = 0;
	UINT64 autoID = 0;
	bool ret = g_databaseEngine.insertRecord(g_selectUserInfoDef,(const char *)&loginInfo,"LOGININFO",autoID,NULL);
	if (!ret){
		return false;
	}   
	Athena::logger->debug("[创建账号成功] account=%s, accountID=%u",loginInfo.m_account,autoID);
	return true;
}

bool UserManager::updateLoginInfo(LoginInfo & info)
{
	char where[128] = { 0 };
	bzero(where, 128);
	snprintf(where, 128, " USERID=%d ",info.m_userID);
	return g_databaseEngine.updateRecord(g_selectUserInfoDef,(const char *)&info,"LOGININFO",where,NULL);
}

void UserManager::addLoginConn(NetIO *pConn)
{
	if (!pConn){
		return ;
	}
	m_mutex.lock();
	m_loginConnMap.insert(std::make_pair(pConn,g_loginServer->getSec()));
	m_mutex.unlock();
}

void UserManager::removeLoginConn(NetIO *pConn)
{
	if (!pConn){
		return ;
	}
	m_mutex.lock();
	std::map<NetIO *,UINT32>::iterator it = m_loginConnMap.find(pConn);
	if (it != m_loginConnMap.end()){
		m_loginConnMap.erase(it);	
	}
	m_mutex.unlock();
}

void UserManager::timer(UINT32 cur)
{
	m_mutex.lock();
	std::vector<NetIO *> delVec;
	std::map<NetIO *,UINT32>::iterator it = m_loginConnMap.begin();
	for (;it != m_loginConnMap.end();++it){
		if (it->second + s_LOGIN_CONN_TIMEOUT < cur){
			delVec.push_back(it->first);	
		}	
	}
	
	for (UINT32 i = 0;i < delVec.size();++i){
		m_loginConnMap.erase(delVec[i]);	
	}
	m_mutex.unlock();
}
	
stSpecialAccountInfo * UserManager::getAccountInfo(std::string account)
{
	std::map<std::string,stSpecialAccountInfo>::iterator it =   m_specialAccounts.find(account);
	if (it == m_specialAccounts.end()){
		return NULL;		
	}
	return &it->second;
}
