#include "AdvEventReport.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "HttpClient.h"
#include "OpIDMgr.h"

const std::string  AdvEventReport::s_IOS_REPORT_URL = "http://api.m.uuzu.com/?device=ios";
const std::string  AdvEventReport::s_ANDRIOD_REPORT_URL = "http://api.m.uuzu.com/?device=android";


AdvEventReport::AdvEventReport()
{
	m_bIOS = false;	
}

AdvEventReport::~AdvEventReport()
{

}

bool AdvEventReport::init(UINT32 platformID,UINT16 level)
{
	if (COpIDMgr::ePLATFORM_APPSTORE == platformID){
		m_bIOS = true;
	}
	else {
		m_bIOS = false;
	}
	m_level = level;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("该服务器是:%s",m_bIOS ? "appstore" : "android");
#endif
	return true;
}

static void g_reportCreateRoleCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){ 
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("上报创建角色获得的http响应信息%s",ret.c_str());
		}  
	}
	else {
		Athena::logger->trace("上报创建角色失败");
	}
}

void AdvEventReport::reportCreateRole(UINT32 cur,UINT32 opID,std::string devicID,std::string ip,UINT32 gameID,UINT32 serverID,std::string  bundleID,std::string gameVersion,std::string  account,std::string name)
{
	/*if ((!m_bIOS) && ){
		return ;
	}*/

	CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
	if (m_bIOS){
		pRequest->setUrl(s_IOS_REPORT_URL.c_str());
	}
	else {
		pRequest->setUrl(s_ANDRIOD_REPORT_URL.c_str());
	}
	pRequest->setRequestType(CHttpRequest::kHttpPost);
	pRequest->setTag("reportCreateRole");
	char szBuff[512] = { 0 };	
	snprintf(szBuff,sizeof(szBuff),"event=active&logtime=%d&ad_id=%d&device_id=%s&ip=%s&game_id=%d&server_id=%u&bundle_id=%s&game_version=%s&account=%s&rolename=%s",cur,opID,devicID.c_str(),ip.c_str(),gameID,serverID,bundleID.c_str(),gameVersion.c_str(),account.c_str(),name.c_str());
	pRequest->setRequestData(szBuff,strlen(szBuff));
	pRequest->setResponseCallback(g_reportCreateRoleCB);
	CHttpClient::getSingleton().asynSend(pRequest);
	//Athena::logger->trace("请求参数:%s",szBuff);
}

static void g_reportLoginCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){ 
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("上报登录获得的http相应信息%s",ret.c_str());
		}  
	}
	else {
		Athena::logger->trace("上报登录失败");
	}
}

void AdvEventReport::reportLogin(UINT32 cur,UINT32 gameID,UINT32 serverID,std::string deviceID,std::string bundleID,std::string gameVersion,std::string account,std::string name)
{
	CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
	if (m_bIOS){
		pRequest->setUrl(s_IOS_REPORT_URL.c_str());
	}
	else {
		pRequest->setUrl(s_ANDRIOD_REPORT_URL.c_str());
	}
	pRequest->setRequestType(CHttpRequest::kHttpPost);
	pRequest->setTag("reportLogin");
	char szBuff[512] = { 0 };	
	snprintf(szBuff,sizeof(szBuff),"event=login&logtime=%d&game_id=%d&server_id=%u&device_id=%s&bundle_id=%s&game_version=%s&account=%s&rolename=%s",cur,gameID,serverID,deviceID.c_str(),bundleID.c_str(),gameVersion.c_str(),account.c_str(),name.c_str());
	pRequest->setRequestData(szBuff,strlen(szBuff));
	pRequest->setResponseCallback(g_reportLoginCB);
	CHttpClient::getSingleton().asynSend(pRequest);

}

static void g_reportLevelupCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){ 
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("上报升级获得的http相应信息%s",ret.c_str());
		}  
	}
	else {
		Athena::logger->trace("上报升级失败");
	}
}

void AdvEventReport::reportLevelup(UINT32 cur,UINT32 gameID,UINT32 serverID,std::string bundleID,std::string account,std::string name,UINT16 level)
{
	if (level > m_level){
		return ;
	}
	CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
	if (m_bIOS){
		pRequest->setUrl(s_IOS_REPORT_URL.c_str());
	}
	else {
		pRequest->setUrl(s_ANDRIOD_REPORT_URL.c_str());
	}
	pRequest->setRequestType(CHttpRequest::kHttpPost);
	pRequest->setTag("reportLevelup");
	char szBuff[512] = { 0 };	
	snprintf(szBuff,sizeof(szBuff),"event=level&logtime=%d&game_id=%d&server_id=%u&bundle_id=%s&account=%s&rolename=%s&level=%d",cur,gameID,serverID,bundleID.c_str(),account.c_str(),name.c_str(),level);
	pRequest->setRequestData(szBuff,strlen(szBuff));
	pRequest->setResponseCallback(g_reportLevelupCB);
	CHttpClient::getSingleton().asynSend(pRequest);
}
