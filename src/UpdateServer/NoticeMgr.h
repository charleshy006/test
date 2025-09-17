#ifndef __H_NOTICE_MGR_H__
#define __H_NOTICE_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include <map>
#pragma pack(1)
struct 	stNoticeInfoDB
{
	UINT32 m_noticeID;					//公告ID
	char   m_title[50];					//标题
	char   m_content[4096];				//内容
	UINT16 m_orderID;					//排序ID
	UINT32 m_createTime;				//创建时间

	stNoticeInfoDB()
	{
		m_noticeID = 0;
		bzero(m_title,sizeof(m_title));
		bzero(m_content,sizeof(m_content));
		m_orderID = 0;
		m_createTime = 0;
	}
};
#pragma pack()

struct stNoticeInfo
{
	UINT32 m_noticeID;					//公告ID
	char   m_title[50];					//标题
	char   m_content[4096];				//内容
	UINT16 m_orderID;                   //排序ID
	UINT32 m_createTime;				//创建时间

	stNoticeInfo()
	{
		m_noticeID = 0;
		bzero(m_title,sizeof(m_title));
		bzero(m_content,sizeof(m_content));
		m_orderID  = 0;
		m_createTime = 0;
	}

	void initFromDB(const stNoticeInfoDB & data);

	void fillDBData(stNoticeInfoDB & data);
};


class CNoticeMgr
{
	USE_SINGLETON_NEW(CNoticeMgr);
private :
	//构造函数
	CNoticeMgr()
	{}
public :
	//获得单例
	static CNoticeMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CNoticeMgr();
	//加载全部
	bool load();
	//保存公告
	void save(UINT32 noticeID,UINT16 orderID,std::string title,std::string & content);
	//新增一个全局记录
	void addRecord(const stNoticeInfoDB & data);
	//更新一个全局记录
	void updateRecord(const stNoticeInfoDB & data);
	//删除公告
	void deleteNotice(UINT32 noticeID);
	//删除一条记录
	void deleteRecord(UINT32 noticeID);
	//活动公告内容
	std::string getNotices();
	//获得表的名字
	std::string & getTblName()
	{
		return m_tblName;
	}
private :
	std::map<UINT32,stNoticeInfo> m_notices;
	std::string m_tblName;
};
#endif
