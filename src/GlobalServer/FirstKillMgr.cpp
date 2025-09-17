#include "FirstKillMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Team.pb.h"

CTblField g_firstKillInfoTblDef[] = {
	{ "COPYMAPID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CHARID1",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "JOB1",            CTblField::DB_TYPE_UINT8,  sizeof(UINT8)	 },
	{ "SEX1",            CTblField::DB_TYPE_UINT8,  sizeof(UINT8)	 },
	{ "NAME1",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1},
	{ "LEVEL1",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "BATTLEPOWER1",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "CHARID2",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "JOB2",            CTblField::DB_TYPE_UINT8,  sizeof(UINT8)	 },
	{ "SEX2",            CTblField::DB_TYPE_UINT8,  sizeof(UINT8)	 },
	{ "NAME2",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1},
	{ "LEVEL2",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "BATTLEPOWER2",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "CHARID3",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },  
	{ "JOB3",            CTblField::DB_TYPE_UINT8,  sizeof(UINT8)	 },
	{ "SEX3",            CTblField::DB_TYPE_UINT8,  sizeof(UINT8)	 },
	{ "NAME3",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1},
	{ "LEVEL3",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "BATTLEPOWER3",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "KILLTIME",    	 CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "USERPHOTOID1",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "USERPHOTOID2",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ "USERPHOTOID3",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ NULL, 0, 0 },
};

void stFirstKillInfo::initFromDB(const stFirstKillInfoDB & dbData)
{
	m_copyMapID = dbData.m_copyMapID;
	m_charID1 = dbData.m_charID1;
	m_job1 = dbData.m_job1;
	m_sex1 = dbData.m_sex1;
	strncpy(m_name1,dbData.m_name1,sizeof(m_name1));
	m_level1 = dbData.m_level1;
	m_battlePower1 = dbData.m_battlePower1;
	m_photoID1 = dbData.m_photoID1;

	m_charID2 = dbData.m_charID2;
	m_job2 = dbData.m_job2;
	m_sex2 = dbData.m_sex2;
	strncpy(m_name2,dbData.m_name2,sizeof(m_name2));
	m_level2 = dbData.m_level2;
	m_battlePower2 = dbData.m_battlePower2;
	m_photoID2 = dbData.m_photoID2;

	m_charID3 = dbData.m_charID3;
	m_job3 = dbData.m_job3;
	m_sex3 = dbData.m_sex3;
	strncpy(m_name3,dbData.m_name3,sizeof(m_name3));
	m_level3 = dbData.m_level3;
	m_battlePower3 = dbData.m_battlePower3;
	m_killTime = dbData.m_killTime;
	m_photoID3 = dbData.m_photoID3;
}

void stFirstKillInfo::fillDBData(stFirstKillInfoDB & dbData)
{
	dbData.m_copyMapID = m_copyMapID;
	dbData.m_charID1 = m_charID1;
	dbData.m_job1 = m_job1;
	dbData.m_sex1 = m_sex1;
	strncpy(dbData.m_name1,m_name1,sizeof(dbData.m_name1));
	dbData.m_level1 = m_level1;
	dbData.m_battlePower1 = m_battlePower1;
	dbData.m_photoID1 = m_photoID1;

	dbData.m_charID2 = m_charID2;
	dbData.m_job2 = m_job2;
	dbData.m_sex2 = m_sex2;
	strncpy(dbData.m_name2,m_name2,sizeof(dbData.m_name2));
	dbData.m_level2 = m_level2;
	dbData.m_battlePower2 = m_battlePower2;
	dbData.m_photoID2 = m_photoID2;
	
	dbData.m_charID3 = m_charID3;
	dbData.m_job3 = m_job3;
	dbData.m_sex3 = m_sex3;
	strncpy(dbData.m_name3,m_name3,sizeof(dbData.m_name3));
	dbData.m_level3 = m_level3;
	dbData.m_battlePower3 = m_battlePower3;
	dbData.m_killTime = m_killTime;
	dbData.m_photoID3 = m_photoID3;
}

CFirstKillMgr::~CFirstKillMgr()
{

}

bool CFirstKillMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_firstKillInfoTblDef," FIRST_KILL ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stFirstKillInfoDB * pDBData = (stFirstKillInfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stFirstKillInfo killInfo;
			killInfo.initFromDB(pDBData[i]);
			m_firstKillInfos[killInfo.m_copyMapID] = killInfo;
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("首杀记录数:%d",m_firstKillInfos.size());
	std::map<UINT32,stFirstKillInfo>::iterator it = m_firstKillInfos.begin();

	for (;it != m_firstKillInfos.end();++it){
		stFirstKillInfo & killInfo = it->second;
		Athena::logger->trace("copymapID:%d,charID1=%d,job1=%d,sex1=%d,name1=%s,level1=%d,bp1=%d,charID2=%d,job2=%d,sex2=%d,name2=%s,level2=%d,bp2=%d,charID3=%d,job3=%d,sex3=%d,name3=%s,level3=%d,bp3=%d",killInfo.m_copyMapID,killInfo.m_charID1,killInfo.m_job1,killInfo.m_sex1,killInfo.m_name1,killInfo.m_level1,killInfo.m_battlePower1,killInfo.m_charID2,killInfo.m_job2,killInfo.m_sex2,killInfo.m_name2,killInfo.m_level2,killInfo.m_battlePower2,killInfo.m_charID3,killInfo.m_job3,killInfo.m_sex3,killInfo.m_name3,killInfo.m_level3,killInfo.m_battlePower3);
	}
#endif
	return true;
}

void CFirstKillMgr::sendFirstKillInfo(Player * pUser,UINT32 copymapID)
{
	if (!pUser){
		return ;
	}
	
	std::map<UINT32,stFirstKillInfo>::iterator it = m_firstKillInfos.find(copymapID);
	if (it == m_firstKillInfos.end()){
		TeamProto::stRetFirstKillInfo cmd;
		cmd.set_copymapid(copymapID);
		pUser->sendProtoToMe(cmd);
		return ;
	}
	stFirstKillInfo & killInfo = it->second;

	TeamProto::stRetFirstKillInfo cmd;
	cmd.set_copymapid(copymapID);
	cmd.set_charid1(killInfo.m_charID1);
	cmd.set_job1(killInfo.m_job1);
	cmd.set_sex1(killInfo.m_sex1);
	cmd.set_name1(killInfo.m_name1);
	cmd.set_level1(killInfo.m_level1);	
	cmd.set_battlepower1(killInfo.m_battlePower1);
	cmd.set_photoid1(killInfo.m_photoID1);
	
	cmd.set_charid2(killInfo.m_charID2);
	cmd.set_job2(killInfo.m_job2);
	cmd.set_sex2(killInfo.m_sex2);
	cmd.set_name2(killInfo.m_name2);
	cmd.set_level2(killInfo.m_level2);	
	cmd.set_battlepower2(killInfo.m_battlePower2);
	cmd.set_photoid2(killInfo.m_photoID2);
	
	cmd.set_charid3(killInfo.m_charID3);
	cmd.set_job3(killInfo.m_job3);
	cmd.set_sex3(killInfo.m_sex3);
	cmd.set_name3(killInfo.m_name3);
	cmd.set_level3(killInfo.m_level3);	
	cmd.set_battlepower3(killInfo.m_battlePower3);
	cmd.set_killtime(killInfo.m_killTime);
	cmd.set_photoid3(killInfo.m_photoID3);

	pUser->sendProtoToMe(cmd);
}

void CFirstKillMgr::teamCopymapFinish(Global::stTeamCopyMapFinish * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	std::map<UINT32,stFirstKillInfo>::iterator it = m_firstKillInfos.find(recvCmd->copymapid());
	if (it != m_firstKillInfos.end()){//已经有首杀了
		return ;
	}
	
	stFirstKillInfo & killInfo = m_firstKillInfos[recvCmd->copymapid()];
	killInfo.m_copyMapID = recvCmd->copymapid();
	killInfo.m_killTime = g_globalServer->getSec();
	for (UINT8 i = 0;i < recvCmd->userinfo_size();++i){
		const Global::stFirstKillBaseInfo & info = recvCmd->userinfo(i);
		if (0 == i){
			killInfo.m_charID1 = info.charid();
			killInfo.m_battlePower1 = info.battlepower();
			killInfo.m_job1 = info.job();
			killInfo.m_sex1 = info.sex();
			killInfo.m_level1 = info.level();
			strncpy(killInfo.m_name1,info.name().c_str(),sizeof(killInfo.m_name1));
			killInfo.m_photoID1 = info.photoid();
		}
		else if (1 == i){
			killInfo.m_charID2 = info.charid();
			killInfo.m_battlePower2 = info.battlepower();
			killInfo.m_job2 = info.job();
			killInfo.m_sex2 = info.sex();
			killInfo.m_level2 = info.level();
			strncpy(killInfo.m_name2,info.name().c_str(),sizeof(killInfo.m_name2));
			killInfo.m_photoID2 = info.photoid();
		}
		else if (2 == i){
			killInfo.m_charID3 = info.charid();
			killInfo.m_battlePower3 = info.battlepower();
			killInfo.m_job3 = info.job();
			killInfo.m_sex3 = info.sex();
			killInfo.m_level3 = info.level();
			strncpy(killInfo.m_name3,info.name().c_str(),sizeof(killInfo.m_name3));
			killInfo.m_photoID3 = info.photoid();
		}
	}

	stFirstKillInfoDB data;
	killInfo.fillDBData(data);
	addRecord(data);

	Global::stTeamFinishFirstKill cmd;
	cmd.set_copymapid(killInfo.m_copyMapID);
	cmd.set_charid1(killInfo.m_charID1);
	cmd.set_charid2(killInfo.m_charID2);
	cmd.set_charid3(killInfo.m_charID3);
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

void CFirstKillMgr::userOnline(Player * pUser)
{
	if (!pUser){
		return ;
	}
	Global::stUserOnlineSendFirstKillInfo cmd;
	cmd.set_charid(pUser->getID());
	std::map<UINT32,stFirstKillInfo>::iterator it = m_firstKillInfos.begin();		
	for (;it != m_firstKillInfos.end();++it){
		stFirstKillInfo & info = it->second;
		
		if ((info.m_charID1 == pUser->getID())
				||(info.m_charID2 == pUser->getID())
				||(info.m_charID3 == pUser->getID())){
			cmd.add_copymapid(it->first);	
		}
	}
	pUser->sendProto2Game(cmd);
}

void CFirstKillMgr::addRecord(stFirstKillInfoDB & data)
{
	Athena::CUDSqlCallBack<CFirstKillMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CFirstKillMgr>(this,&CFirstKillMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_firstKillInfoTblDef,(const char *)&data,"FIRST_KILL",autoID,pDelaySql);
}

void CFirstKillMgr::addRecordCB(bool opResult,UINT64 autoID)
{}

bool CFirstKillMgr::gotPass(UINT32 mapId) {
    std::map<UINT32, stFirstKillInfo>::iterator it =
            m_firstKillInfos.find(mapId);
    if (it != m_firstKillInfos.end()) {
        return true;
    }
    return false;
}

bool CFirstKillMgr::getRecord(UINT32 mapId, CDevilCallCfg::stDevilCallInfo & dc) {
    std::map<UINT32, stFirstKillInfo>::iterator it =
            m_firstKillInfos.find(mapId);
    if (it != m_firstKillInfos.end()) {
        dc.m_copyMapID = it->second.m_copyMapID;
        dc.name1 = std::string(it->second.m_name1);
        dc.m_charID1 = it->second.m_charID1;
        dc.name2 = std::string(it->second.m_name2);
        dc.m_charID2 = it->second.m_charID2;
        dc.name3 = std::string(it->second.m_name3);
        dc.m_charID3 = it->second.m_charID3;
        return true;
    }
    return false;
}

void CFirstKillMgr::updateRecord(stFirstKillInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "COPYMAPID=%u",data.m_copyMapID);
	Athena::CUDSqlCallBack<CFirstKillMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CFirstKillMgr>(this,&CFirstKillMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_firstKillInfoTblDef, (const char *)&data, "FIRST_KILL", where, pDelaySql);
}

void CFirstKillMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void CFirstKillMgr::synAllRecords(Player * pUser)
{
	if (!pUser){
		return ;
	}

	std::map<UINT32,stFirstKillInfo>::iterator it = m_firstKillInfos.begin();
	for (;it != m_firstKillInfos.end(); ++it){//已经有首杀了
		stFirstKillInfo & killInfo = it->second;
		
		bool isChange = false;

		if(killInfo.m_charID1 == pUser->getID()){
			strncpy(killInfo.m_name1,pUser->getName(),sizeof(killInfo.m_name1));
			killInfo.m_photoID1 = pUser->m_photoID;
			isChange = true;
		}

		if(killInfo.m_charID2 == pUser->getID()){
			strncpy(killInfo.m_name2,pUser->getName(),sizeof(killInfo.m_name2)); 
			killInfo.m_photoID2 = pUser->m_photoID;
			isChange = true;
		}

		if(killInfo.m_charID3 == pUser->getID()){
			strncpy(killInfo.m_name3,pUser->getName(),sizeof(killInfo.m_name3)); 
			killInfo.m_photoID3 = pUser->m_photoID;
			isChange = true;
		}

		if(isChange){
			stFirstKillInfoDB data;
			killInfo.fillDBData(data);
			updateRecord(data);
		}
	}
}
