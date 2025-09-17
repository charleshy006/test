#include "ServerConfig.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "UtilFunction.h"
#include "HttpClient.h"
#include "KVPair.h"

CTblField g_serverConfigTblDef[] = {
	{ "ID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CONTENT", CTblField::DB_TYPE_STRING,  10240  		  },  
	{ NULL, 0, 0 },
};

CServerConfigMgr::~CServerConfigMgr()
{
	
}

static void g_reportBillCB(CHttpClient* client, CHttpResponse* response)
{
	Athena::logger->trace("给bill上报信息回调成功!");
}

bool CServerConfigMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_serverConfigTblDef," SERVER_CONFIG ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stServerConfigInfoDB * pDBData = (stServerConfigInfoDB * )result->getData();
		std::vector<std::string> strVec;
		g_stringtok(strVec,pDBData->m_config,";");
		m_id = pDBData->m_id;
		for (UINT16 i = 0;i < strVec.size();++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i],"=");
			
			if (subStrVec.size() >= 2){
				m_key2Values[subStrVec[0]] = subStrVec[1];
				Athena::global[subStrVec[0]] = subStrVec[1];
			}
		}
	}
	else {//数据不存在,要插入
		stServerConfigInfoDB dbData;
		UINT64 autoID = 0;
		g_databaseEngine.insertRecord(g_serverConfigTblDef,(const char*)&dbData," SERVER_CONFIG ",autoID);
		m_id = autoID;
	}

#ifdef _HDZ_DEBUG
	std::map<std::string,std::string>::iterator it = m_key2Values.begin();
	for (;it != m_key2Values.end();++it){
		Athena::logger->trace("已经设置的服务器配置是:key=%s,valuer=%s",it->first.c_str(),it->second.c_str());
	}
#endif
	//notifyBill(Athena::global["billURL"]);
	return true;
}

void CServerConfigMgr::notifyBill(const std::string & billUrl)
{
	// SvrInfo *pSvrInfo = g_globalServer->getSvrInfoByType(WEBSERVER);
	// if (!pSvrInfo){
	// 	return ;
	// }
	// CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
	// pRequest->setUrl(billUrl.c_str());
	// pRequest->setRequestType(CHttpRequest::kHttpPost);
	// pRequest->setTag("reportBill");
	

	// char szContent[300] = { 0 };
	// snprintf(szContent,sizeof(szContent),"port=%d&serverID=%s&serverKey=%s",pSvrInfo->m_port,Athena::global["serverID"].c_str(),Athena::global["serverKey"].c_str());

	// pRequest->setRequestData(szContent,strlen(szContent)); 
	// pRequest->setResponseCallback(g_reportBillCB);
	// CHttpClient::getSingleton().asynSend(pRequest);
}

std::string CServerConfigMgr::getValue(const std::string &key)
{
	std::map<std::string,std::string>::iterator it = m_key2Values.find(key);
	if (it != m_key2Values.end()){
		return it->second;
	}
	else {
		return "";
	}
}

std::string CServerConfigMgr::operator [](const std::string &key)
{
	return getValue(key);
}

void CServerConfigMgr::setValue(const std::string &key,const std::string &value,bool bSave)
{
	std::map<std::string,std::string>::iterator it = m_key2Values.find(key);	
	if (it == m_key2Values.end()){
		m_key2Values[key] = value;
	}
	else {
		m_key2Values[key] = value;
	}

	if (bSave){
		updateRecord();
	}
}

void CServerConfigMgr::updateRecord()
{
	stServerConfigInfoDB data;
	data.m_id = m_id;
	
	std::ostringstream os;
	std::map<std::string,std::string>::iterator it = m_key2Values.begin();
	bool first = true;

	for (;it != m_key2Values.end();++it){
		if (first){
			first  = false;	
			os << it->first << "="<< it->second;
		}
		else {
			os << ";" << it->first << "="<< it->second;
		}
	}
	strncpy(data.m_config,os.str().c_str(),sizeof(data.m_config));
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `ID`=%u", m_id);
	Athena::CUDSqlCallBack<CServerConfigMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CServerConfigMgr>(this,&CServerConfigMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_serverConfigTblDef,(const char *)&data,"SERVER_CONFIG",where,pDelaySql);
}

void CServerConfigMgr::updateRecordCB(bool opResult,UINT64 autoID)
{
}	

void CServerConfigMgr::notifyGate()
{
	Global::stNotifyGateSvrCfg retCmd;
	
	std::map<std::string,std::string>::iterator it = m_key2Values.begin();

	for (;it != m_key2Values.end();++it){
		Global::stServerCfgPair * pSyn = retCmd.add_cfgpair();
		if (pSyn){
			pSyn->set_key(it->first);
			pSyn->set_value(it->second);
		}
	}
	g_globalServer->getConnMgr().broadcastByType(GATESERVER,retCmd);	
}

void CServerConfigMgr::notifyGame()
{
	Global::stNotifyGameSvrCfg retCmd;
	
	std::map<std::string,std::string>::iterator it = m_key2Values.begin();
	for (;it != m_key2Values.end();++it){
		Global::stServerCfgPair * pSyn = retCmd.add_cfgpair();
		if (pSyn){
			pSyn->set_key(it->first);
			pSyn->set_value(it->second);
		}
	}
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,retCmd);
}
