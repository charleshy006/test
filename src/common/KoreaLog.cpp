#include "KoreaLog.h"

CKoreaLogMgr::CKoreaLogMgr()
{
	m_pLogger = NULL;
}

CKoreaLogMgr::~CKoreaLogMgr()
{

}

bool CKoreaLogMgr::init(std::string & fileName)
{
	m_pLogger = new Log4cxxHelp(fileName);
	m_pLogger->removeConsoleLog();
	m_pLogger->addLocalFileLog(fileName);
	return true;
}

void CKoreaLogMgr::createRole(std::string account,UINT32 charID,std::string ip,UINT32 op_id,std::string roleName,UINT32 curTime,std::string deviceType)
{
	if (NULL == m_pLogger){
		return ;
	}		
	m_pLogger->trace("[1001][%d][%s][%d][%s][%s][%s][%s][%d]",curTime,account.c_str(),charID,ip.c_str(),deviceType.c_str(),roleName.c_str(),Athena::global["gameVersion"].c_str(),op_id);
}

void CKoreaLogMgr::loginOn(UINT32 curTime,std::string account,UINT32 charID,std::string ip,UINT16 level)
{
	if (NULL == m_pLogger){
		return ;
	}	

	m_pLogger->trace("[1002][%d][%s][%d][%s][%s][%d]",curTime,account.c_str(),charID,ip.c_str(),Athena::global["gameVersion"].c_str(),level);
}

void CKoreaLogMgr::loginOff(UINT32 curTime,std::string account,UINT32 charID,UINT16 level)
{
	if (NULL == m_pLogger){
		return ;
	}
	m_pLogger->trace("[1003][%d][%s][%d][%d]",curTime,account.c_str(),charID,level);
}
void CKoreaLogMgr::userLevelup(UINT32 curTime,std::string account,UINT32 charID,UINT16 level)
{
	if (NULL == m_pLogger){
		return ;
	}
	
	m_pLogger->trace("[1010][%d][%s][%d][%d]",curTime,account.c_str(),charID,level);
}

void CKoreaLogMgr::userGuide(UINT32 curTime,std::string account,UINT32 charID,UINT16 level,UINT16 stepID)
{
	if (NULL == m_pLogger){
		return ;
	}
	m_pLogger->trace("[1011][%d][%s][%d][%d][%d]",curTime,account.c_str(),charID,level,stepID);
}

void CKoreaLogMgr::recharge(UINT32 curTime,std::string account,UINT32 charID,UINT16 level,std::string payID,std::string orderID,UINT32 amount,UINT32 coin,UINT32 totalRecharge)
{
	if (NULL == m_pLogger){
		return ;
	}
	Athena::logger->trace("[1020][%d][%s][%d][%d][%s][%s][%d][%d][%d]",curTime,account.c_str(),charID,level,payID.c_str(),orderID.c_str(),amount,coin,totalRecharge);
}

void CKoreaLogMgr::consume(UINT32 curTime,std::string account,UINT32 charID,UINT16 level,UINT16 type,UINT32 coin,UINT32 currentCoin)
{
	if (NULL == m_pLogger){
		return ;
	}
	
	Athena::logger->trace("[1021][%d][%s][%d][%d][%d][%d][%d]",curTime,account.c_str(),charID,level,type,coin,currentCoin);
}
