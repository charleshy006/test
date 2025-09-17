#include "CommonRefreshMgr.h"
#include "GlobalServer.h"
#include "GlobalVar.h"
#include "Arena.h"
#include "GuardMgr.h"

extern GlobalServer * g_globalServer;

UINT32 CCommonRefreshMgr::curTime()
{
	return g_globalServer->getSec();
}

void CCommonRefreshMgr::doRefreshData()
{
	CGlobalVarMgr::getSingleton().setValue(eGlobalVar_Common_RefreshTime,m_lastRefreshTime);
	//m_lastRefreshTime = curTime();
	ArenaMgr::getSingleton().resetUserArenaInfo();
	CGuardMgr::getSingleton().resetGuardUserInfo();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("进行了通用模块的刷新!");
#endif
}
