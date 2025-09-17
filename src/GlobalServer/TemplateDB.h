#ifndef __H_TEMPALTE_DB_H__
#define __H_TEMPALTE_DB_H__
#include "Singleton.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
template <class InfoDB,class Info>
class CTemplateDB 
{
public :
	//构造函数
	CTemplateDB(const char * szTblName,CTblField * tblColDef) : m_tblName(szTblName),m_tblColDef(tblColDef)
	{}
	//析构函数
	virtual ~CTemplateDB() {}
	//加载
	void load();
	//增加一个记录
	void addRecord(const InfoDB & dbData);
	//增加记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一条记录
	void updateRecord(const InfoDB & dbData,const char * szWhere);
	//更新一条记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除一条记录
	void removeRecord(const char * szWhere);
	//删除一条记录回调
	void removeRecordCB(bool opResult,UINT64 autoID);
	//添加一个对象到容器
	virtual void add2Container(Info & info) {}
protected :
	std::string m_tblName;
	CTblField * m_tblColDef;
};

template <class InfoDB,class Info>
void CTemplateDB<InfoDB,Info>::addRecord(const InfoDB & dbData)
{
	Athena::CUDSqlCallBack<CTemplateDB> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CTemplateDB>(this,&CTemplateDB::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(m_tblColDef,(const char *)&dbData,m_tblName.c_str(),autoID,pDelaySql);	
}

template <class InfoDB,class Info>
void CTemplateDB<InfoDB,Info>::addRecordCB(bool opResult,UINT64 autoID)
{}

template <class InfoDB,class Info>
void CTemplateDB<InfoDB,Info>::updateRecord(const InfoDB & dbData,const char * szWhere)
{
	Athena::CUDSqlCallBack<CTemplateDB> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CTemplateDB>(this,&CTemplateDB::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(m_tblColDef,(const char *)&dbData,m_tblName.c_str(),szWhere,pDelaySql);
}

template <class InfoDB,class Info>
void CTemplateDB<InfoDB,Info>::updateRecordCB(bool opResult,UINT64 autoID)
{}

template <class InfoDB,class Info>
void CTemplateDB<InfoDB,Info>::removeRecord(const char * szWhere)
{
	Athena::CUDSqlCallBack<CTemplateDB> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CTemplateDB>(this,&CTemplateDB::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord(m_tblName.c_str(),szWhere,pDelaySql);
}

template <class InfoDB,class Info>
void CTemplateDB<InfoDB,Info>::removeRecordCB(bool opResult,UINT64 autoID)
{}

template <class InfoDB,class Info>
void CTemplateDB<InfoDB,Info>::load()
{
	CTblQueryResult* result = NULL;
	result = g_databaseEngine.select(m_tblColDef,m_tblName.c_str(),NULL,NULL,0,NULL);
	if (result){
		InfoDB * pDataDB = (InfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			Info info ;
			info.initFromDB(pDataDB[i]);
#ifdef _HDZ_DEBUG
			info.print();
#endif
			add2Container(info);
		}
	}
	SAFE_DELETE(result);
}
#endif
