#include "OperationMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "WebServer.h"
#include "md5.h"

CTblField g_gameTblNameTblDef[] = {
	    { "TABLE_NAME",      CTblField::DB_TYPE_STRING,    100 },
		{ NULL, 0, 0 },
};

CTblField g_loginIPTypeTblDef[] = {
		{"IP_TYPE",	 CTblField::DB_TYPE_UINT16,	  sizeof(UINT16)	},
	    { "IP",      CTblField::DB_TYPE_STRING,   50  				},
		{ NULL, 0, 0 },
};

void COperationMgr::clearAllTbl()
{
	CTblQueryResult * pResult = NULL;
	pResult = g_databaseEngine.listTable(g_gameTblNameTblDef,"%");

	if (pResult && pResult->getData()){
		stGameTblNameInfo * pTblName = (stGameTblNameInfo *)pResult->getData();
		for (UINT16 i = 0;i < pResult->getRowCount();++i){
			if (strncmp(pTblName[i].m_name,"ACTIVITY_CONFIG",sizeof(pTblName[i].m_name)) == 0){
				continue;
			}
			
			if (strncmp(pTblName[i].m_name,"SERVER_CONFIG",sizeof(pTblName[i].m_name)) == 0){
				continue;
			}
			
			if (strncmp(pTblName[i].m_name,"GIFT_BAG",sizeof(pTblName[i].m_name)) == 0){
				continue;
			}

			char sqlDef[200] = {0};
			snprintf(sqlDef,sizeof(sqlDef)," DELETE FROM %s ",pTblName[i].m_name);
			g_databaseEngine.directExecute(sqlDef,strlen(sqlDef),sqlDef);
		}
	}
	SAFE_DELETE(pResult);
}

void COperationMgr::load()
{
	
}

void COperationMgr::clearAllIP()
{
	char sqlDef[200] = {0};
	snprintf(sqlDef,sizeof(sqlDef)," DELETE FROM LOGIN_IP_TYPE ");
	g_databaseEngine.directExecute(sqlDef,strlen(sqlDef),sqlDef);
}

void COperationMgr::addOneIPType(const char * szIP,eLoginIPType type)
{
	if (!szIP){
		return ;
	}
	
	stLoginIPTypeDB data;
	data.m_type = type;
	strncpy(data.m_ip,szIP,sizeof(data.m_ip));
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_loginIPTypeTblDef,(const char*)&data," LOGIN_IP_TYPE ",autoID);
}

bool COperationMgr::verifySign(std::vector<std::string> &vec,std::string &sign)
{
	char tokenBuf[1024] = { 0 };
	int  bufLen = 0;

	std::ostringstream os ;
	for (UINT16 i = 0;i < vec.size();++i){
		os << vec[i];
	}

    if (Athena::global["isNotUuzu"] == std::string("true")) {
        snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s%s",os.str().c_str(), Athena::global["signKey"].c_str());
    } else {
        snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s%s",os.str().c_str(),OPERATION_SERVER_KEY);
    }
	bufLen = strlen(tokenBuf);
	char digest[64] = {0};
	utils::Md5(tokenBuf,bufLen,digest);
	utils::HexString::bin2asc(digest,16,digest,64,bufLen);
	digest[bufLen] = 0;
	
	std::string subDigest(&digest[3],24);

	if (sign == subDigest){
		return true;
	}
	else {
		return false;
	}
}

