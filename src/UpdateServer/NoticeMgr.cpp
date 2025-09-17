#include "NoticeMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "UpdateServer.h"
#include "json/reader.h"
#include "json/writer.h"

CTblField g_noticeTblDef[] = {
	{ "NOTICEID", 		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "TITLE",     		CTblField::DB_TYPE_STRING,  50},
	{ "CONTENT",        CTblField::DB_TYPE_STRING,  4096},
	{ "ORDERID",     	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "CREATETIME",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ NULL, 0, 0 },
};

void stNoticeInfo::initFromDB(const stNoticeInfoDB & data)
{
	m_noticeID = data.m_noticeID;
	strncpy(m_title,data.m_title,sizeof(m_title));
	strncpy(m_content,data.m_content,sizeof(m_content));
	m_orderID = data.m_orderID;
	m_createTime = data.m_createTime;
}

void stNoticeInfo::fillDBData(stNoticeInfoDB & data)
{
	data.m_noticeID = m_noticeID;
	strncpy(data.m_title,m_title,sizeof(data.m_title));
	strncpy(data.m_content,m_content,sizeof(data.m_content));
	data.m_orderID = m_orderID;
	data.m_createTime = m_createTime;
}

CNoticeMgr::~CNoticeMgr()
{

}

CTblField g_logTblDef[] = {  
	{ "TABLE_NAME",      CTblField::DB_TYPE_STRING,    100 },   
	{ NULL, 0, 0 },   
};  

#pragma pack(1)
struct stLogTblInfo
{
	char m_tblName[100];

	stLogTblInfo()
	{    
		bzero(m_tblName,sizeof(m_tblName));
	}    
};
#pragma pack()

bool CNoticeMgr::load()
{
	m_tblName = "NOTICE_" +Athena::global["svrID"];
	Athena::logger->trace("notice表真实名字为:",m_tblName.c_str());
	CTblQueryResult * pResult = NULL;
	pResult = g_databaseEngine.listTable(g_logTblDef,"NOTICE%");
	bool bIsExist = false;
	if (pResult && pResult->getData()){
		stLogTblInfo * pTblNameInfo = (stLogTblInfo *)pResult->getData();
		for (UINT16 i = 0;i < pResult->getRowCount();++i){
			if (m_tblName == pTblNameInfo[i].m_tblName){
				bIsExist = true;
			}
		}
	}
	SAFE_DELETE(pResult);
	if (!bIsExist){//不存在则创建
		static char s_createSql[1024] = { 0 }; 
		snprintf(s_createSql,sizeof(s_createSql),"CREATE TABLE `%s` (\
			`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,\
			`NOTICEID`    int(10) unsigned NOT NULL DEFAULT '0',\
			`TITLE` varchar(50) NOT NULL DEFAULT '',\
			`CONTENT` varchar(4096) NOT NULL DEFAULT '',\
			`ORDERID`  smallint unsigned NOT NULL DEFAULT '0',\
			`CREATETIME`   int(10) unsigned  NOT NULL DEFAULT '0',\
			PRIMARY KEY (`ID`)\
			) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;",m_tblName.c_str());
		g_databaseEngine.directExecute(s_createSql,strlen(s_createSql),s_createSql);
		Athena::logger->trace("公告表不存在创建了公告表");
	}

	pResult = g_databaseEngine.select(g_noticeTblDef,m_tblName.c_str(),NULL,NULL,0,NULL);
	if (pResult && pResult->getData()){
		stNoticeInfoDB * pDBData = (stNoticeInfoDB * )pResult->getData();
		for (UINT16 i = 0;i < pResult->getRowCount();++i){
			stNoticeInfo info;
			info.initFromDB(pDBData[i]);
			m_notices[info.m_noticeID] = info;
		}
	}

#ifdef _HDZ_DEBUG
	std::map<UINT32,stNoticeInfo>::iterator it =  m_notices.begin();

	for (;it != m_notices.end();++it){
		stNoticeInfo & notice = it->second;
		Athena::logger->trace("加载的公告有id=%d,title:%s,content:%s",notice.m_noticeID,notice.m_title,notice.m_content);	
	}
#endif
	return true;
}


void CNoticeMgr::addRecord(const stNoticeInfoDB & data)
{
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_noticeTblDef,(const char *)&data,m_tblName.c_str(),autoID,NULL);
}

void CNoticeMgr::updateRecord(const stNoticeInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " NOTICEID=%u", data.m_noticeID);
	g_databaseEngine.updateRecord(g_noticeTblDef,(const char *)&data,m_tblName.c_str(),where,NULL);
}

void CNoticeMgr::deleteRecord(UINT32 noticeID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " NOTICEID=%u ", noticeID);
	g_databaseEngine.deleteRecord(m_tblName.c_str(),where,NULL);
}

void CNoticeMgr::deleteNotice(UINT32 noticeID)
{
	m_notices.erase(noticeID);	
	deleteRecord(noticeID);
}

void CNoticeMgr::save(UINT32 noticeID,UINT16 orderID,std::string title,std::string & content)
{
	std::map<UINT32,stNoticeInfo>::iterator it = m_notices.find(noticeID);	

	if (it == m_notices.end()){//还没有存在
		stNoticeInfo info;
		info.m_noticeID = noticeID;
		info.m_orderID = orderID;
		strncpy(info.m_title,title.c_str(),sizeof(info.m_title));
		strncpy(info.m_content,content.c_str(),sizeof(info.m_content));
		info.m_createTime = g_updateServer->getSec();
		stNoticeInfoDB data;		
		info.fillDBData(data);
		addRecord(data);
		m_notices[info.m_noticeID] = info;
	}
	else {
		stNoticeInfo & info = it->second;
		info.m_orderID = orderID;
		strncpy(info.m_title,title.c_str(),sizeof(info.m_title));
		strncpy(info.m_content,content.c_str(),sizeof(info.m_content));
		stNoticeInfoDB data;
		info.fillDBData(data);  
		updateRecord(data);
	}
}

std::string CNoticeMgr::getNotices()
{
	Json::FastWriter writer;
	Json::Value root;
	Json::Value data;
	root["status"] = "0";
	Json::Value notice;
	
	if (m_notices.size() > 0) {
		root["status"] = "1";
		std::map<UINT32,stNoticeInfo>::iterator it = m_notices.begin();
		for (;it != m_notices.end();++it){
			stNoticeInfo &info = it->second;
			Json::Value notice;
			notice["title"] = info.m_title;
			notice["content"] = info.m_content;
			notice["orderID"] = info.m_orderID;
			data.append(notice);
		}
	}
	
	root["data"] = data;
	std::string json_file = writer.write(root);
	return json_file;
}
