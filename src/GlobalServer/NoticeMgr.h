#ifndef __H_NOTICE_MGR_H__
#define __H_NOTICE_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"

#include <map>

struct stNotice
{
	UINT32 m_nextSendTime;	//下次发送时间
	UINT16 m_num;			//次数
	UINT32 m_interval;		//间隔
	std::string m_content;  //内容

	stNotice()
	{
		m_nextSendTime = 0;
		m_num = 0;
		m_interval = 0;
	}
};

class CNoticeMgr
{
	USE_SINGLETON_NEW(CNoticeMgr);
private :
	//构造函数
	CNoticeMgr();
public:
	//析构函数
	~CNoticeMgr();
	//返回公告管理函数
	static CNoticeMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//增加一条公告
	void addNotice(UINT32 noticeID,UINT32 startTime,UINT32 delay,UINT16 num,UINT32 interval,const char * szContent);
	//修改一条公告
	void updateNotice(UINT32 noticeID,UINT32 startTime,UINT32 delay,UINT32 num,UINT32 interval,const char * szContent);
	//发送公告
	void sendNotify(const stNotice & notice);
	//定时回调
	void timer(UINT32 cur);
	//删除吊公告
	void delNotice(UINT32 noticeID);
private :
	std::map<UINT32,stNotice>  m_allNotices;		//所有公告
	static UINT32 s_noticeGenID;				//公告ID
};
#endif
