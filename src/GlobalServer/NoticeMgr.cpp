#include "NoticeMgr.h"
#include "Role.pb.h"
#include "PlayerManager.h"
#include "SysLanguages.h"

UINT32 CNoticeMgr::s_noticeGenID = 1;				//公告ID

CNoticeMgr::CNoticeMgr()
{
	
}

CNoticeMgr::~CNoticeMgr()
{
	
}

void CNoticeMgr::addNotice(UINT32 noticeID,UINT32 startTime,UINT32 delay,UINT16 num,UINT32 interval,const char * szContent)
{
	stNotice notice;
	notice.m_content = szContent;
	notice.m_interval = interval;
	notice.m_num = num;
	notice.m_nextSendTime = startTime + delay;
	m_allNotices[noticeID] = notice;
}

void CNoticeMgr::updateNotice(UINT32 noticeID,UINT32 startTime,UINT32 delay,UINT32 num,UINT32 interval,const char * szContent)
{
	std::map<UINT32,stNotice>::iterator it =  m_allNotices.find(noticeID);
	if (it == m_allNotices.end()){
		return ;
	}
	
	stNotice &  notice = it->second;
	notice.m_content = szContent;
	notice.m_interval = interval;
	notice.m_nextSendTime = startTime + delay;
	notice.m_num = num;
}

void CNoticeMgr::sendNotify(const stNotice & notice)
{
	Role::stChatCmd retCmd;
	retCmd.set_type(Role::CHAT_TYPE_NOTICE);
	retCmd.set_content(notice.m_content);
	retCmd.set_srccharid(0);
	retCmd.set_srccharname(CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM));
	PlayerManager::getSingleton().sendProto2AllZone(retCmd);
}

void CNoticeMgr::timer(UINT32 cur)
{
	std::vector<UINT32> delVec;
	std::map<UINT32,stNotice>::iterator it =  m_allNotices.begin();
	
	for (;it != m_allNotices.end(); ++it){
		stNotice & notice = it->second; 
		if (notice.m_nextSendTime < cur){//要发送通知
			if (notice.m_num > 0){
				notice.m_num -= 1;
				sendNotify(notice);
			}	

			if (notice.m_num <= 0){//发送次数完了
				delVec.push_back(it->first);
			}
			else {
				notice.m_nextSendTime = cur + notice.m_interval;
			}
		}
	}

	for (UINT16 i = 0;i < delVec.size();++i){
		m_allNotices.erase(delVec[i]);	
	}
}

void CNoticeMgr::delNotice(UINT32 noticeID)
{
	std::map<UINT32,stNotice>::iterator it =  m_allNotices.find(noticeID);

	if (it == m_allNotices.end()){
		return ;
	}

	m_allNotices.erase(it);
}
