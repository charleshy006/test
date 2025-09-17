#ifndef __H_MAIL_H__
#define __H_MAIL_H__ 
#include "CommonMicro.h"
#include "Player.h"
#include "Misc.pb.h"
#include <map>
#include <list>
#include <vector>
#include <set>

#define MAIL_TILE_LEN 100
#define MAIL_CONTENT_LEN 250

enum eFetchFlag
{
	eFetchFlag_Has_Got = 2,		//已经领取
};

#pragma pack(1)

struct stMailDBInfo
{
	UINT32 m_id;
	UINT32 m_senderID;							//发送者ID
	char   m_senderName[MAX_NAMESIZE + 1];		//发送者名字
	UINT32 m_receiverID;						//接收者ID
	char   m_receiverName[MAX_NAMESIZE + 1];	//接收者名字
	char   m_title[MAIL_TILE_LEN];				//标题
	char   m_content[MAIL_CONTENT_LEN];			//内容
	byte   m_isRead;							//是否已读
	byte   m_isFetchAttach;						//是否已经领取附件
	byte   m_mailType;							//邮件类型
	UINT32 m_attachID1;							//附件1ID
	UINT32 m_attachNum1;						//附件1数量
	UINT32 m_attachID2;							//附件2ID
	UINT32 m_attachNum2;						//附件2数量
	UINT32 m_attachID3;							//附件3ID
	UINT32 m_attachNum3;						//附件3数量
	UINT32 m_sendTime;							//发送时间

	stMailDBInfo()
	{
		m_id = 0;
		m_senderID = 0;
		bzero(m_senderName,sizeof(m_senderName));
		m_receiverID = 0;
		bzero(m_receiverName,sizeof(m_receiverName));
		bzero(m_title,sizeof(m_title));
		bzero(m_content,sizeof(m_content));
		m_isRead = 0;
		m_isFetchAttach = 0;
		m_mailType = 0;
		m_attachID1 = 0;
		m_attachNum1 = 0;
		m_attachID2 = 0;
		m_attachNum2 = 0;
		m_attachID3 = 0;
		m_attachNum3 = 0;
		m_sendTime = 0;
	}
};
#pragma pack()

class CTblQueryResult;
//发送邮件回调
typedef void (*SendMailCB)(UINT64 autoID) ; 

class MailManager 
{
	USE_SINGLETON_NEW(MailManager);
public :
	struct stAttachInfo
	{
		UINT32 m_charID;
		UINT32 m_attachID1;
		UINT32 m_attachNum1;
		UINT32 m_attachID2;
		UINT32 m_attachNum2;
		UINT32 m_attachID3;
		UINT32 m_attachNum3;
		
		stAttachInfo()
		{
			m_charID = 0;
			m_attachID1 = 0;
			m_attachNum1 = 0;
			m_attachID2 = 0;
			m_attachNum2 = 0;
			m_attachID3 = 0;
			m_attachNum3 = 0;
		}
	};

	static const UINT16 MAX_MAIL_NUM = 10;
public:
	static MailManager & getSingleton()
	{
		 return THE_SINGLETON::Instance();
	}
	
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	//构造
	MailManager() {}
	//析构
	~MailManager() {}
	//填充邮件信息
	void fillSynSnapMailInfo(MiscProto::stSynMainSnalInfo & synInfo,const stMailDBInfo & dbInfo);
	//填充邮件详细信息
	void fillSynDetailMailInfo(MiscProto::stSynDetailMailInfo & synInfo,const stMailDBInfo & dbInfo);
	//发送邮件
	void sendMail(const stMailDBInfo & mailDBInfo,SendMailCB cb);
	//发送邮件回调
	void sendMailCB(bool opResult,UINT64 autoID,stMailDBInfo mailInfo,SendMailCB cb);
	//通过邮件给奖励
	void giveItemByMail(const char * szTitle,const char * szContent,std::vector<std::pair<UINT32,UINT32> > & attaches,UINT32 receiver);
	//阅读邮件
	void readMail(Player *pUser,UINT32 id);
	//阅读邮件回调
	void readMailCB(CTblQueryResult* result,UINT32 charID);
	//更新读字段回调返回
	void updateReadFlagCB(bool opResult,UINT64 autoID);
	//用户上线
	void userOnline(Player * pUser);
	//用户上线回调
	void userOnlineCB(CTblQueryResult* result,UINT32 charID);
	//获取附件
	void fetchAttach(Player * pUser,UINT32 id);
	//提取附件回调
	void fetchAttachCB(CTblQueryResult* result,UINT32 charID);
	//一键提取附件
	void autofetchAttach(Player * pUser,UINT32 id);
	//一键自动提取邮件回调
	void autofetchAttachCB(CTblQueryResult* result,UINT32 charID);
	//更新附件字段后回调
	void updateAttachFlagCB(bool opResult,UINT64 autoID,stAttachInfo attachInfo);
	//清空过期日志
	void deleteOutdateMail();
	//删除选定的邮件
	//删除已经读邮件
	void deleteHasReadMail(UINT32 charID);
	//删除已读邮件回调
	void selectHasReadMailCB(CTblQueryResult* result,UINT32 charID);
	//删除已读邮件回
	void deleteHasReadMailCB(bool opResult,UINT64 autoID);
	//删除选择的邮件
	void deleteSelMail(const char *szWhere);
	//删除选择邮件回调
	void deleteSelMailCB(bool opResult,UINT64 autoID);
};

struct stBatchSendMailTask
{
	UINT32 m_senderID;							//发送者ID
	std::string m_senderName;					//发送者名字
	std::string m_title;						//标题
	std::string m_content;						//内容
	std::set<UINT32> m_receiveIDs; 				//收件人列表
	std::vector<std::pair<UINT32,UINT32> > m_attaches;	//附件
	UINT32 m_lastOpTime;						//最近一次操作时间

	stBatchSendMailTask()
	{
		m_senderID = 0;	
		m_lastOpTime = 0;
	}
};

class CBatchSendMailMgr
{
	USE_SINGLETON_NEW(CBatchSendMailMgr);
public :
	static const UINT32 s_SENDMAIL_NUM_PER_TIME = 8;	//每秒发送邮件数量
public :
	//析构函数
	~CBatchSendMailMgr();
private :
	//构造函数
	CBatchSendMailMgr();
public :
	static CBatchSendMailMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();		
	}
	void addTask(const stBatchSendMailTask & task);
	//发送邮件
	void sendMail(const stBatchSendMailTask & task,UINT32 receiverID);
	//定时器
	void timer(UINT32 cur);
private :
	std::list<stBatchSendMailTask> m_allTasks;			//所有任务	
};
#endif
