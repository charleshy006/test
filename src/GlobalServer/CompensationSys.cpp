#include "CompensationSys.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Misc.pb.h"

CTblField g_compensationInfoTblDef[] = {
	{ "ID",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "COMPENSATION_TYPE",      CTblField::DB_TYPE_UINT8,  sizeof(UINT8)   },
 	{ "TITLE",           		CTblField::DB_TYPE_STRING,  25			   },
  	{ "CONTENT",           		CTblField::DB_TYPE_STRING,  150			   },
   	{ "REWARD",           		CTblField::DB_TYPE_STRING,  150			   },
	{ "CREATETIME",      		CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};

void stCompensationInfo::initFromDB(const stCompensationInfoDB & data)
{
	m_id = data.m_id;
	m_type  = data.m_type;
	m_title = data.m_title;
	m_content = data.m_content;
	
	std::vector<std::string> strVec;
	g_stringtok(strVec,data.m_rewards,";");
	std::vector<std::string> subStrVec;

	for (UINT16 i = 0;i < strVec.size();++i){
		subStrVec.clear();
		g_stringtok(subStrVec,strVec[i],"-");
		if (subStrVec.size() >= 2){
			UINT32 itemID = (UINT32)atoi(subStrVec[0].c_str());
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			m_rewards.push_back(std::make_pair(itemID,num));	
		}
	}

	m_createTime = data.m_createTime;
}

void stCompensationInfo::fillDBData(stCompensationInfoDB & data)
{
	data.m_id = m_id;
	data.m_type = m_type;
	strncpy(data.m_title,m_title.c_str(),sizeof(data.m_title));
	strncpy(data.m_content,m_content.c_str(),sizeof(data.m_content));

	std::ostringstream os;
	bool bFirst = false;

	for (UINT16 i = 0;i < m_rewards.size();++i){
		if (bFirst){
			os << m_rewards[i].first << "-" << m_rewards[i].second; 
			bFirst = true;
		}
		else {
			os <<";"<<m_rewards[i].first << "-" << m_rewards[i].second;	
		}
	}

	strncpy(data.m_rewards,os.str().c_str(),sizeof(data.m_rewards));
	data.m_createTime = m_createTime;
}


CCompensationMgr::~CCompensationMgr()
{

}

bool CCompensationMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_compensationInfoTblDef," COMPENSATION ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stCompensationInfoDB * pDBData = (stCompensationInfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stCompensationInfo info;
			info.initFromDB(pDBData[i]);
			m_allCompensations[info.m_id] = info;
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了%d个补偿",m_allCompensations.size());
#endif
	return true;
}

void CCompensationMgr::addRecord(stCompensationInfoDB & data)
{
	Athena::CUDSqlCallBack<CCompensationMgr,stCompensationInfoDB> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CCompensationMgr,stCompensationInfoDB>(this,&CCompensationMgr::addRecordCB,false,0,data);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_compensationInfoTblDef,(const char *)&data,"COMPENSATION",autoID,pDelaySql);
}

void CCompensationMgr::addRecordCB(bool opResult,UINT64 autoID,stCompensationInfoDB data)
{
	if (opResult){//成功
		stCompensationInfo & info = m_allCompensations[(UINT32)autoID]; 
		info.initFromDB(data);
	}   
}

void CCompensationMgr::updateRecord(stCompensationInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `ID`=%u", data.m_id);

	Athena::CUDSqlCallBack<CCompensationMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CCompensationMgr>(this,&CCompensationMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_compensationInfoTblDef,(const char *)&data,"COMPENSATION",where,pDelaySql);
}

void CCompensationMgr::updateRecordCB(bool opResult,UINT64 autoID)
{
}

void CCompensationMgr::userOnline(Player * pUser)
{
	if (!pUser){
		return ;
	}	
}

void CCompensationMgr::sendList2Me(Player * pUser)
{
	if (!pUser){
		return ;
	}

	MiscProto::stRetCompensationList retCmd;
	std::map<UINT32,stCompensationInfo>::iterator it = m_allCompensations.begin();
	for (;it != m_allCompensations.end();++it){
		stCompensationInfo & one = it->second;
		if (one.m_id > pUser->m_compensationID){
			MiscProto::stCompensationInfo * pSyn = retCmd.add_info();
			if (pSyn){
				pSyn->set_id(one.m_id);
				pSyn->set_desc(one.m_content);

				for (UINT16 i = 0;i < one.m_rewards.size();++i){
					std::pair<UINT32,UINT32> & subOne = one.m_rewards[i];
					MiscProto::stCompensationItem * pSynItem = pSyn->add_items();
					if (pSynItem){
						pSynItem->set_itemid(subOne.first);
						pSynItem->set_num(subOne.second);
					}
				}
			}
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CCompensationMgr::gotCompensation(Player * pUser,UINT32 id)
{
	if (!pUser){
		return ;
	}
	MiscProto::stGotCompensationResult retCmd;

	if (pUser->m_compensationID >= id){
		retCmd.set_result(MiscProto::stGotCompensationResult::eHas_Got);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	UINT32 beginID = pUser->m_compensationID;
	pUser->m_compensationID = id;
	retCmd.set_result(MiscProto::stGotCompensationResult::eSuccess);
	pUser->sendProtoToMe(retCmd);

	Global::stSetCompensationID cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_compensationid(pUser->m_compensationID);
	Compensation_IT it = m_allCompensations.begin();
	for (;it != m_allCompensations.end();++it){
		stCompensationInfo & info = it->second;
		if (info.m_id > beginID){
			for (UINT8 i = 0;i < info.m_rewards.size();++i){
				std::pair<UINT32,UINT32> & one = info.m_rewards[i];
				Global::stCompensantionItem * pSyn = cmd.add_items();
				if (pSyn){
					pSyn->set_itemid(one.first);
					pSyn->set_num(one.second);
				}
			}	
		}
	}
	pUser->sendProto2Game(cmd);
}

void CCompensationMgr::timer(UINT32 cur)
{
	std::vector<UINT32>  delID;
	Compensation_IT it = m_allCompensations.begin();
	for (;it != m_allCompensations.end();++it){
		stCompensationInfo & info = it->second;
		if (eSynCompensation_Type_Day_Refresh == info.m_type) {
			if (!g_isTheSameDay(cur,info.m_createTime)){
				delID.push_back(info.m_id);
			}	
		}
	}

	for (UINT16 i = 0;i < delID.size();++i){
		m_allCompensations.erase(delID[i]);
		removeRecord(delID[i]);
	}

}

void CCompensationMgr::removeRecord(UINT32 id)
{
	char where[65] = { 0 };
	bzero(where, 65);   
	snprintf(where, 64, " ID=%d ", id);
	Athena::CUDSqlCallBack<CCompensationMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CCompensationMgr>(this,&CCompensationMgr::removeRecordCB,false,0);    
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("COMPENSATION",where,pDelaySql);
}

void CCompensationMgr::removeRecordCB(bool opResult,UINT64 autoID)
{

}
