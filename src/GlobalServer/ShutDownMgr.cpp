#include "ShutDownMgr.h"
#include "GlobalServer.h"
#include "Role.pb.h"
#include "PlayerManager.h"
#include "SysLanguages.h"
extern GlobalServer * g_globalServer;

void CShutDownMgr::timer(UINT32 cur)
{
	if (!m_bBeginStopSvr){
		return ;
	}

	if (!m_stopSvrTime){//还没开始
		return ;
	}

	if (cur >= m_stopSvrTime){//到时间了可以停服了
		g_globalServer->stop();
		Athena::logger->trace("[系统]时间到了服务器开始停机了!");
	}

	if (cur >= m_nextSendNotifyTime){
		m_nextSendNotifyTime += s_SEND_NOTIFY_INTERVAL;
		UINT32 sec = m_stopSvrTime > m_nextSendNotifyTime ? m_stopSvrTime - m_nextSendNotifyTime : 0;
		sendNotify(sec);
	}	
}

void CShutDownMgr::beginStop(bool stopNow)
{
	m_bBeginStopSvr = true;
    if (stopNow) {
        m_stopSvrTime = g_globalServer->getSec() + 5;
    } else {
        m_stopSvrTime = g_globalServer->getSec() + s_SHUTDOWN_LASTTIME;
    }
	m_nextSendNotifyTime = g_globalServer->getSec();
}

void CShutDownMgr::sendNotify(UINT32 sec)
{
	char content[150] = { 0 };
	if (sec >= 60){
		snprintf(content,sizeof(content),"服务器将在%d分%d秒后关闭,请大家及时下线,以免遭受不必要损失!",sec / 60,sec % 60);
	}
	else {
		snprintf(content,sizeof(content),"服务器将在%d秒后关闭,请大家及时下线,以免遭受不必要损失!",sec % 60);
	}
	Role::stChatCmd retCmd;
	retCmd.set_type(Role::CHAT_TYPE_NOTICE);
	retCmd.set_content(content);
	retCmd.set_srccharid(0);
	retCmd.set_srccharname(CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM));
	PlayerManager::getSingleton().sendProto2AllZone(retCmd);
}

void CShutDownMgr::reset()
{
	m_bBeginStopSvr = false;
	m_stopSvrTime = 0;
	m_nextSendNotifyTime = 0;
}
