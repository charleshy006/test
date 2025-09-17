#include "QuestionMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "QuestionCfg.h"

CTblField g_questResultTblDef[] = {
	{ "ID",      		  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "QUESTION_ID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "PROBLEM_ID",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "ANSWER_A",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ANSWER_B",    	  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "ANSWER_C",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ANSWER_D",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "ANSWER_E",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ANSWER_F",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ANSWER_G",    	  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "ANSWER_H",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "ANSWER_I",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "ANSWER_J",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ NULL, 0, 0 },
};

void stProblemInfo::initFromDB(const stProblemInfoDB & data)
{
	m_id = data.m_id;
	m_questID = data.m_questID;
	m_problemID = data.m_problemID;
	m_answerA = data.m_answerA;
	m_answerB = data.m_answerB;
	m_answerC = data.m_answerC;
	m_answerD = data.m_answerD;
	m_answerE = data.m_answerE;
	m_answerF = data.m_answerF;
	m_answerG = data.m_answerG;
	m_answerH = data.m_answerH;
	m_answerI = data.m_answerI;
	m_answerJ = data.m_answerJ;
}

void stProblemInfo::fillDBData(stProblemInfoDB & data)
{
	data.m_id = m_id;
	data.m_questID = m_questID;
	data.m_problemID = m_problemID;
	data.m_answerA = m_answerA;
	data.m_answerB = m_answerB;
	data.m_answerC = m_answerC;
	data.m_answerD = m_answerD;
	data.m_answerE = m_answerE;
	data.m_answerF = m_answerF; 
	data.m_answerG = m_answerG;
	data.m_answerH = m_answerH;
	data.m_answerI = m_answerI;
	data.m_answerJ = m_answerJ;
}

QuestionMgr::~QuestionMgr()
{

}

bool QuestionMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_questResultTblDef," QUESTION_RESULT ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stProblemInfoDB * pDBData = (stProblemInfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stProblemInfo problemInfo;
			problemInfo.initFromDB(pDBData[i]);
			m_problemInfos[problemInfo.getKey()] = problemInfo;
		}
	}
	
	std::set<std::pair<UINT32,UINT32> >  problems = CQuestionCfg::getSingleton().getAllProblems();
	std::set<std::pair<UINT32,UINT32> >::iterator it = problems.begin();

	for (;it != problems.end();++it){
		std::map<UINT32,stProblemInfo>::iterator subIt =  m_problemInfos.find((it->second << 16) | it->first);
		
		if (subIt == m_problemInfos.end()){//数据库里面还没有
			stProblemInfoDB data;
			data.m_questID = it->second;
			data.m_problemID = it->first;
			UINT64 autoID = 0;
			g_databaseEngine.insertRecord(g_questResultTblDef,(const char *)&data,"QUESTION_RESULT",autoID,NULL);	
			data.m_id = autoID;
			stProblemInfo problemInfo;
			problemInfo.initFromDB(data);
			m_problemInfos[problemInfo.getKey()] = problemInfo;
		}
	}
#ifdef _HDZ_DEBUG
	std::map<UINT32,stProblemInfo>::iterator thdIt = m_problemInfos.begin();
	for (;thdIt != m_problemInfos.end();++thdIt){
		stProblemInfo & problem = thdIt->second;
		Athena::logger->trace("已经设置的全局变量是:id:%d,questID:%d,problemID:%d,A:%d,B:%d,C:%d,D:%d,E:%d,F:%d,G:%d,H:%d,I:%d,J:%d",problem.m_id,problem.m_questID,problem.m_problemID,problem.m_answerA,problem.m_answerB,problem.m_answerC,problem.m_answerD,problem.m_answerE,problem.m_answerF,problem.m_answerG,problem.m_answerH,problem.m_answerI,problem.m_answerJ);
	}
#endif
	return true;
}

void QuestionMgr::addRecord(const stProblemInfoDB & data)
{
	Athena::CUDSqlCallBack<QuestionMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<QuestionMgr>(this,&QuestionMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_questResultTblDef,(const char *)&data,"QUESTION_RESULT",autoID,pDelaySql);
}

void QuestionMgr::addRecordCB(bool opResult,UINT64 autoID)
{
	
}

void QuestionMgr::updateRecord(const stProblemInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `ID`=%u", data.m_id);

	Athena::CUDSqlCallBack<QuestionMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<QuestionMgr>(this,&QuestionMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_questResultTblDef,(const char *)&data,"QUESTION_RESULT",where,pDelaySql);
}

void QuestionMgr::updateRecordCB(bool opResult,UINT64 autoID)
{
}

void QuestionMgr::updateProblemInfo(UINT32 questID,bool a,bool b,bool c,bool d,bool e ,bool f,bool g,bool h,bool i,bool j)
{
	std::map<UINT32,stProblemInfo>::iterator it = m_problemInfos.find(questID);	
	if (it == m_problemInfos.end()){
		return ;
	}
	
	stProblemInfo & info = it->second;
	if (a){
		info.m_answerA += 1;
	}

	if (b){
		info.m_answerB += 1;
	}

	if (c){
		info.m_answerC += 1;
	}
	
	if (d){
		info.m_answerD += 1;
	}

	if (e){
		info.m_answerE += 1;
	}
	
	if (f){
		info.m_answerF += 1;
	}

	if (g){
		info.m_answerG += 1;
	}

	if (h){
		info.m_answerH += 1;
	}
	
	if (i){
		info.m_answerI += 1;
	}

	if (j){
		info.m_answerJ += 1;
	}

	stProblemInfoDB dbData;
	info.fillDBData(dbData);
	updateRecord(dbData);
}
