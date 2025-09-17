#include "RankMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "SortList.pb.h"
#include "CaptureTreasure.h"
#include "Mail.h"
#include "GlobalVar.h"
#include "OpenSvrPowerRankCfg.h"
#include "RoleInfoManager.h"
#include "OpenServerInfo.h"
#include "GuardMgr.h"
#include "SysLanguages.h"

CTblField g_levelRankTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "LEVEL",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "JOB",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

CTblField g_battlePowerRankTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "BATTLEPOWER",  	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)	 },
	{ "JOB",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

CTblField g_copymapRankTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "STARNUM",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)	 },
	{ "JOB",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

CTblField g_charLvTblDef [] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "LEVEL",          CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },
	{ NULL, 0, 0 },
};

CTblField g_charBattlePowerTblDef [] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BATTLEPOWER",    CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ NULL, 0, 0 },
};



CTblField g_leitaiTblDef [] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "USETIME",       	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)	 },
	{ "JOB",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",         	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

CTblField g_openSvrPowerRankTblDef [] = {
	{ "ID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "REWARD",    	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "BINARYDATA", CTblField::DB_TYPE_BLOB,    1024 			 },
	{ NULL, 0, 0 },
};

#pragma pack(1)
struct stCharIDAndLv
{
	UINT32 m_charID;			//角色ID
	UINT16 m_level;				//等级

	stCharIDAndLv()
	{
		m_charID = 0;
		m_level = 0;
	}
};

struct stCharIDAndBattlePower
{
	UINT32 m_charID;
	UINT32 m_battlePower;

	stCharIDAndBattlePower()
	{
		m_charID = 0;
		m_battlePower = 0;
	}
};
#pragma pack()

void stOpenSvrPowerRank::fillDataDB(stOpenSvrPowerRankDB & data)
{
	data.m_id = m_id;
	data.m_hasReward = m_hasReward;
	
	std::map<UINT16,stOpenSvrBPRankItem>::iterator it = m_rank2CharIDs.begin();
	
	UINT16 i = 0;
	for (;it != m_rank2CharIDs.end();++it){
		data.data.data[i].m_charID = it->second.m_charID;
		data.data.data[i].m_bpVal = it->second.m_bpVal; 
		data.data.data[i].m_rank = it->first;
		++i;
		if (i >= OPEN_SVR_POWER_RANK){
			break;		
		}
	}
}

void stOpenSvrPowerRank::initFromDB(stOpenSvrPowerRankDB & data)
{
	m_id = data.m_id;
	m_hasReward = data.m_hasReward;
	
	if (!m_hasReward){
		return ;
	}

	for (UINT16 i = 0;i < OPEN_SVR_POWER_RANK;++i){
		stOpenSvrBPRankItem & info = m_rank2CharIDs[data.data.data[i].m_rank];
		info.m_charID = data.data.data[i].m_charID;	
		info.m_bpVal = data.data.data[i].m_bpVal;
	}
}

void CRankMgr::load()
{
	char szOderBy[65] = { 0 };	
	CTblQueryResult* result = NULL;

	snprintf(szOderBy,64, " LEVEL DESC ");
	result = g_databaseEngine.select(g_levelRankTblDef,"ROLEINFO",NULL,szOderBy,500,NULL);	
	if (result){
		if (result->getData()){
			stLevelRankDB *  levelRankDB = (stLevelRankDB *) result->getData();
			for (UINT16 i = 0;i < result->getRowCount();++i){
				stLevelRankInfo levelRankInfo;
				levelRankInfo.m_charID = levelRankDB[i].m_charID;
				levelRankInfo.m_level = levelRankDB[i].m_level;
				strncpy(levelRankInfo.m_name,levelRankDB[i].m_name,sizeof(levelRankInfo.m_name));
				levelRankInfo.m_job = levelRankDB[i].m_job;
				levelRankInfo.m_sex = levelRankDB[i].m_sex;
				levelRankInfo.m_photoID = levelRankDB[i].m_photoID;
				m_levelSortList.insertSortInfo(levelRankInfo);
			}
		}
	}
	SAFE_DELETE(result);
	
	result = g_databaseEngine.select(g_charLvTblDef," ROLEINFO ",NULL,NULL,0,NULL);
	if (result){
		if (result->getData()){
			stCharIDAndLv * pDBData = (stCharIDAndLv *)result->getData();
			for (UINT32 i = 0;i < result->getRowCount();++i){
				m_charID2LevelMap.insert(std::make_pair(pDBData[i].m_charID, pDBData[i].m_level));	
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("加载所有角色的等级与角色ID对应关系,记录数 %d",m_charID2LevelMap.size());
#endif
		}
	}
	SAFE_DELETE(result);
	
	result = g_databaseEngine.select(g_charBattlePowerTblDef," ROLEINFO ",NULL,NULL,0,NULL);
	if (result){
		if (result->getData()){
			stCharIDAndBattlePower * pDBData = (stCharIDAndBattlePower *)result->getData();
			for (UINT32 i = 0;i < result->getRowCount();++i){
				m_charID2BattlePowerMap.insert(std::make_pair(pDBData[i].m_charID, pDBData[i].m_battlePower));
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("加载所有角色的战力与角色ID对应关系,记录数 %d",m_charID2LevelMap.size());
#endif
		}
	}
	SAFE_DELETE(result);

	snprintf(szOderBy,64, " BATTLEPOWER DESC ");
	result = g_databaseEngine.select(g_battlePowerRankTblDef,"ROLEINFO",NULL,szOderBy,0,NULL,2000);	
	if (result){
		stBattlePowerRankDB * pAbilityRank  = (stBattlePowerRankDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stBattlePowerRankInfo  abilityInfo;
			abilityInfo.m_charID = pAbilityRank[i].m_charID;
			abilityInfo.m_battlePower = pAbilityRank[i].m_battlePower;
			strncpy(abilityInfo.m_name,pAbilityRank[i].m_name,sizeof(abilityInfo.m_name));
			abilityInfo.m_job = pAbilityRank[i].m_job;
			abilityInfo.m_sex = pAbilityRank[i].m_sex;
			abilityInfo.m_photoID = pAbilityRank[i].m_photoID;
			m_battlePowerSortList.insertSortInfo(abilityInfo);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("charID=%d,name=%s,battlePower=%d",abilityInfo.m_charID,abilityInfo.m_name,abilityInfo.m_battlePower);
#endif
		}
	}
	SAFE_DELETE(result);
	
	snprintf(szOderBy,64, " STARNUM DESC ");
	result = g_databaseEngine.select(g_copymapRankTblDef,"ROLEINFO",NULL,szOderBy,0,NULL);
	if (result){
		stCopyMapRankDB * pCopymapRank = (stCopyMapRankDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stCopyMapRankInfo copymapRank;
			copymapRank.m_charID = pCopymapRank[i].m_charID;
			copymapRank.m_starNum = pCopymapRank[i].m_starNum;
			copymapRank.m_job = pCopymapRank[i].m_job;
			copymapRank.m_sex = pCopymapRank[i].m_sex;
			copymapRank.m_photoID = pCopymapRank[i].m_photoID;
			strncpy(copymapRank.m_name,pCopymapRank[i].m_name,sizeof(copymapRank.m_name));
			m_copymapStarSortlList.insertSortInfo(copymapRank);
		}	
	}
	SAFE_DELETE(result);

	
	result = g_databaseEngine.select(g_leitaiTblDef,"LEITAI_RANK",NULL,NULL,0,NULL);

	if (result){
		stLeitaiRankDB * pLeitaiInfo = (stLeitaiRankDB *)result->getData();	
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stLeitaiRankInfo info;
			info.m_charID = pLeitaiInfo[i].m_charID;
			info.m_job = pLeitaiInfo[i].m_job;
			strncpy(info.m_name,pLeitaiInfo[i].m_name,sizeof(info.m_name));
			info.m_sex = pLeitaiInfo[i].m_sex;
			info.m_useTime = pLeitaiInfo[i].m_useTime;
			info.m_photoID = pLeitaiInfo[i].m_photoID;
			m_leitaiSortList.insertSortInfo(info);
		}
	}
	SAFE_DELETE(result);

	result = g_databaseEngine.select(g_openSvrPowerRankTblDef,"OPEN_SVR_POWER_RANK",NULL,NULL,0,NULL);
	if (result){
		stOpenSvrPowerRankDB * pPowerRankDB = (stOpenSvrPowerRankDB *)result->getData();	
		m_openSvrPowerRank.initFromDB(*pPowerRankDB);
	}
	else {//还没有数据
		stOpenSvrPowerRankDB dbData;
		dbData.m_hasReward = 0;
		UINT64 autoID = 0;
		g_databaseEngine.insertRecord(g_openSvrPowerRankTblDef,(const char *)&dbData,"OPEN_SVR_POWER_RANK",autoID,NULL);
		dbData.m_id = autoID;
		m_openSvrPowerRank.initFromDB(dbData);
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("等级排行榜记录数为%d",m_levelSortList.getListNum());

	Athena::logger->trace("战力排行榜记录数为%d",m_battlePowerSortList.getListNum());

	Athena::logger->trace("副本排行榜记录数为%d",m_copymapStarSortlList.getListNum());
	
	Athena::logger->trace("擂台排行榜数据为%d",m_leitaiSortList.getListNum());	
#endif
}

void CRankMgr::addRecord(const char * pData,UINT8 rankType)
{
	Athena::CUDSqlCallBack<CRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRankMgr>(this,&CRankMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	if (rankType == LEVEL_RANK ) {
		//g_databaseEngine.insertRecord(g_levelRankTblDef,pData,"LEVEL_RANK",autoID,pDelaySql);
	}
	else if (rankType == BATTLEPOWER_RANK){
		//g_databaseEngine.insertRecord(g_battlePowerRankTblDef,pData,"BATTLE_POWER_RANK",autoID,pDelaySql);
	}
	else if (rankType == COPYMAP_RANK){
		//g_databaseEngine.insertRecord(g_copymapRankTblDef,pData,"COPYMAP_RANK",autoID,pDelaySql);
	}
	else if (rankType == LETAI_RANK){
		g_databaseEngine.insertRecord(g_leitaiTblDef,pData,"LEITAI_RANK",autoID,pDelaySql);
	}
}

void CRankMgr::addRecordCB(bool opResult,UINT64 autoID)
{}

void CRankMgr::updateRecord(UINT32 key,const char * pData,UINT8 rankType)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",key);
	
	Athena::CUDSqlCallBack<CRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRankMgr>(this,&CRankMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	if (rankType == LEVEL_RANK ) {
		//g_databaseEngine.updateRecord(g_levelRankTblDef,pData,"LEVEL_RANK",where,pDelaySql);
	}
	else if (rankType == BATTLEPOWER_RANK){
		//g_databaseEngine.updateRecord(g_battlePowerRankTblDef,pData,"BATTLE_POWER_RANK",where,pDelaySql);
	}
	else if (rankType == COPYMAP_RANK){
		//g_databaseEngine.updateRecord(g_copymapRankTblDef,pData,"COPYMAP_RANK",where,pDelaySql);
	}
	else if (rankType == LETAI_RANK){
		g_databaseEngine.updateRecord(g_leitaiTblDef,pData,"LEITAI_RANK",where,pDelaySql);
	}
}

void CRankMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void CRankMgr::removeRecord(UINT32 key,UINT8 rankType)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%d",key);
	Athena::CUDSqlCallBack<CRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRankMgr>(this,&CRankMgr::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	
	if (rankType == LEVEL_RANK ) {
		//g_databaseEngine.deleteRecord("LEVEL_RANK",where,pDelaySql);
	}
	else if (rankType == BATTLEPOWER_RANK){
		//g_databaseEngine.deleteRecord("BATTLE_POWER_RANK",where,pDelaySql);
	}
	else if (rankType == COPYMAP_RANK){
		//g_databaseEngine.deleteRecord("COPYMAP_RANK",where,pDelaySql);
	}
	else if (rankType == LETAI_RANK){
		g_databaseEngine.deleteRecord("LEITAI_RANK",where,pDelaySql);
	}
}

void CRankMgr::removeRecordCB(bool opResult,UINT64 autoID)
{}

void CRankMgr::updateMyLevel(Player * pUser,UINT16 level)
{
	if (!pUser){
		return ;
	}
	pUser->m_level = level;	
	UINT16 oldLv = m_charID2LevelMap[pUser->getID()];
	m_charID2LevelMap[pUser->getID()] = level;
	
	if (oldLv != level){
		CCaptureTreasureMgr::getSingleton().onUserLvChange(pUser,oldLv,level);
		if ((oldLv < CGuardMgr::s_NEED_LEVEL) && (level >= CGuardMgr::s_NEED_LEVEL)) {
			CGuardMgr::getSingleton().checkExist(pUser->getID());
		}
	}

	if ((0 == pUser->m_job) || (0 == pUser->m_sex)){//有可能出现这个情况
		return ;
	}

	if (m_levelSortList.isInSortList(pUser->getID())){
		stLevelRankInfo info = m_levelSortList.getSortInfo(pUser->getID());
		/*
		if(0 != strcmp(info.m_name, pUser->getName()))          
		{                                                       
	    	strncpy(info.m_name,pUser->getName(),MAX_NAMESIZE); 
		    m_levelSortList.updateSortInfo(info);        
		} 
		*/
		if (info.m_level < level){
			info.m_level = level;
			m_levelSortList.updateSortInfo(info);
		}
	}
	else {
		stLevelRankInfo info;
		info.m_charID = pUser->getID();
		info.m_level = pUser->m_level;
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_job = pUser->m_job;
		info.m_sex = pUser->m_sex;
		info.m_photoID = pUser->m_photoID;
		m_levelSortList.insertSortInfo(info);
	}
}

void CRankMgr::updateMyStarNum(Player * pUser,UINT16 starNum)
{
	if (!pUser){
		return ;
	}
	
	if (m_copymapStarSortlList.isInSortList(pUser->getID())){
		stCopyMapRankInfo info = m_copymapStarSortlList.getSortInfo(pUser->getID());
/*		if(0 != strcmp(info.m_name, pUser->getName()))
		{
			strncpy(info.m_name,pUser->getName(),MAX_NAMESIZE);
			m_copymapStarSortlList.updateSortInfo(info);
		}
		*/
		if (info.m_starNum < starNum){
			info.m_starNum = starNum;
			m_copymapStarSortlList.updateSortInfo(info);
		}
	}
	else {
		stCopyMapRankInfo info;
		info.m_charID = pUser->getID();
		info.m_starNum = starNum;
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_job = pUser->m_job;
		info.m_sex = pUser->m_sex;
		info.m_photoID = pUser->m_photoID;
		m_copymapStarSortlList.insertSortInfo(info);
	}
}

void CRankMgr::updateMyBattlePower(Player * pUser,UINT32 battlePower)
{
	if (!pUser){
		return ;
	}
	pUser->m_battlePower = battlePower;	
	m_charID2BattlePowerMap[pUser->getID()] = battlePower;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("更新了我的战力为:%d",pUser->m_battlePower);
#endif
	if (m_battlePowerSortList.isInSortList(pUser->getID())){
		stBattlePowerRankInfo info = m_battlePowerSortList.getSortInfo(pUser->getID());	
	/*	if(0 != strcmp(info.m_name, pUser->getName()))
		{
			strncpy(info.m_name,pUser->getName(),MAX_NAMESIZE);
			m_battlePowerSortList.updateSortInfo(info);
		}
		*/
		if (info.m_battlePower < battlePower){
			info.m_battlePower = battlePower;
			m_battlePowerSortList.updateSortInfo(info);
		}
	}
	else {
		stBattlePowerRankInfo info;
		info.m_charID = pUser->getID();
		info.m_battlePower = battlePower;
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_job = pUser->m_job;
		info.m_sex = pUser->m_sex;
		info.m_photoID = pUser->m_photoID;
		m_battlePowerSortList.insertSortInfo(info);
	}
}


void CRankMgr::updateLeitaiUseTime(Player * pUser,UINT32 useTime)
{
	if (!pUser){
		return ;
	}

	if (m_leitaiSortList.isInSortList(pUser->getID())){
		stLeitaiRankInfo info = m_leitaiSortList.getSortInfo(pUser->getID());
		/*if(0 != strcmp(info.m_name, pUser->getName()))
		{
			strncpy(info.m_name,pUser->getName(),MAX_NAMESIZE);
			m_leitaiSortList.updateSortInfo(info);
			stLeitaiRankDB dbData;
			info.fillDBData(dbData);
			updateRecord(info.getKey(),(const char *)&dbData,LETAI_RANK);
		}            
		*/
		if (info.m_useTime > useTime){
			info.m_useTime = useTime;
			m_leitaiSortList.updateSortInfo(info);
			stLeitaiRankDB dbData;
			info.fillDBData(dbData);
			updateRecord(info.getKey(),(const char *)&dbData,LETAI_RANK);
		}
	}
	else {
		UINT32 lastOneKey = m_leitaiSortList.getLastOneKey();
		stLeitaiRankInfo	info;
		info.m_charID = pUser->getID();
		info.m_useTime = useTime;
		strncpy(info.m_name,pUser->getName(),sizeof(info.m_name));
		info.m_job = pUser->m_job;
		info.m_sex = pUser->m_sex;
		info.m_photoID = pUser->m_photoID;
		m_leitaiSortList.insertSortInfo(info);

		stLeitaiRankDB dbData;
		info.fillDBData(dbData);
		addRecord((const char *)&dbData,LETAI_RANK);
		
		if (!m_leitaiSortList.isInSortList(lastOneKey)){//已经不在排行帮了
			removeRecord(lastOneKey,LETAI_RANK);		
		}
	}

}


void CRankMgr::sendLevelSortList(Player * pUser,UINT32 beginRank)
{
	if (!pUser){
		return ;
	}
	SortProto::stRetLevelSortList retCmd;
	std::vector<stLevelRankInfo> vec;
	m_levelSortList.getFromN12N2(vec,beginRank,beginRank + s_SEND_SORTITEM_MAX);
	retCmd.set_beginrank(beginRank);
	retCmd.set_myrank(m_levelSortList.getMySortRank(pUser->getID()));
	if (g_globalServer->isAppVersion()){
		vec.clear();
	}

	for (UINT8 i = 0;i < vec.size();++i){
		SortProto::stSynLevelSortItem * pSortItem = retCmd.add_sortitem();
		if (pSortItem){
			pSortItem->set_charid(vec[i].m_charID);
			pSortItem->set_name(vec[i].m_name);
			pSortItem->set_level(vec[i].m_level);
			pSortItem->set_job(2);
			pSortItem->set_sex(1);
			pSortItem->set_photoid(vec[i].m_photoID);
		}
	}
	pUser->sendProtoToMe(retCmd);

}

void CRankMgr::sendBattlePowerSortList(Player * pUser,UINT32 beginRank)
{
	if (!pUser){
		return ;
	}

	SortProto::stRetBattlePowerSortList retCmd;
	retCmd.set_beginrank(beginRank);
	retCmd.set_myrank(m_battlePowerSortList.getMySortRank(pUser->getID()));
	std::vector<stBattlePowerRankInfo> vec;
	m_battlePowerSortList.getFromN12N2(vec,beginRank,beginRank + s_SEND_SORTITEM_MAX);
	if (g_globalServer->isAppVersion()){
		vec.clear();
	}
	for (UINT8 i = 0;i < vec.size();++i){
		SortProto::stSynBattlePowerSortItem * pSortItem = retCmd.add_sortitem();
		if (pSortItem){
			pSortItem->set_charid(vec[i].m_charID);
			pSortItem->set_name(vec[i].m_name);
			pSortItem->set_battlepower(vec[i].m_battlePower);
			pSortItem->set_job(2);
			pSortItem->set_sex(1);
			pSortItem->set_level(getLevelByCharID(vec[i].m_charID));
			pSortItem->set_photoid(vec[i].m_photoID);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CRankMgr::sendCopymapSortList(Player * pUser,UINT32 beginRank)
{
	if (!pUser){
		return ;
	}

	SortProto::stRetCopyMapSortList retCmd;
	retCmd.set_beginrank(beginRank);

	std::vector<stCopyMapRankInfo> vec;
	m_copymapStarSortlList.getFromN12N2(vec,beginRank,beginRank + s_SEND_SORTITEM_MAX);	
	retCmd.set_myrank(m_copymapStarSortlList.getMySortRank(pUser->getID()));
	if (g_globalServer->isAppVersion()){
		vec.clear();
	}

	for (UINT8 i = 0;i < vec.size();++i){
		SortProto::stCopyMapSortItem * pSortItem = retCmd.add_sortitem();
		if (pSortItem){
			pSortItem->set_charid(vec[i].m_charID);
			pSortItem->set_name(vec[i].m_name);
			pSortItem->set_starnum(vec[i].m_starNum);
			pSortItem->set_job(2);
			pSortItem->set_sex(1);
			pSortItem->set_level(getLevelByCharID(vec[i].m_charID));
			pSortItem->set_photoid(vec[i].m_photoID);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CRankMgr::sendLeitaiSortList(Player * pUser,UINT32 beginRank)
{
	if (!pUser){
		return ;
	}	

	SortProto::stRetLeitaiSortList retCmd;
	retCmd.set_beginrank(beginRank);
	
	std::vector<stLeitaiRankInfo> vec;
	m_leitaiSortList.getFromN12N2(vec,beginRank,beginRank + s_SEND_SORTITEM_MAX);
	retCmd.set_myrank(m_leitaiSortList.getMySortRank(pUser->getID()));
	if (g_globalServer->isAppVersion()){
		vec.clear();
	}
	for (UINT8 i = 0;i < vec.size(); ++i){
		SortProto::stLeitaiSortListItem * pSortItem = retCmd.add_sortlistitem();
		if (pSortItem){
			pSortItem->set_charid(vec[i].m_charID);
			pSortItem->set_name(vec[i].m_name);
			pSortItem->set_sex(1);
			pSortItem->set_job(2);
			pSortItem->set_usetime(vec[i].m_useTime);
			pSortItem->set_level(getLevelByCharID(vec[i].m_charID));
			pSortItem->set_photoid(vec[i].m_photoID);
		}
	}

	pUser->sendProtoToMe(retCmd);
}

void CRankMgr::timer(UINT32 cur)
{
	UINT32 weekRefreshTime = CGlobalVarMgr::getSingleton().getValue(eGlobalVar_Refresh_Week_LeiTaiRank);

	struct tm curTm;
	RealTime::getLocalTime(curTm,cur);
	if (curTm.tm_hour == 1){
		if ((!g_isTheSameWeek(weekRefreshTime,cur)) && (curTm.tm_wday == 0)){//不同周,先检查不周
			CGlobalVarMgr::getSingleton().setValue(eGlobalVar_Refresh_Week_LeiTaiRank,cur);
			resetUserLeitaiInfo();
		}
	}
}

void CRankMgr::resetUserLeitaiInfo()
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere,sizeof(szWhere),"CHARID>0");
	Athena::CUDSqlCallBack<CRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRankMgr>(this,&CRankMgr::deleteLeitaiUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("LEITAI_RANK",szWhere,pDelaySql);
}

void CRankMgr::deleteLeitaiUserInfoCB(bool opResult,UINT64 autoID)
{}

UINT16 CRankMgr::getLevelByCharID(UINT32 charID)
{
	CharID2LevelMap_IT it = m_charID2LevelMap.find(charID);
	if (it != m_charID2LevelMap.end()){
		return it->second;
	}
	return 0;
}

UINT32 CRankMgr::getBattlePowerCharIDByRank(UINT32 rank)
{
	if (rank > m_battlePowerSortList.getListNum()) {
		rank = m_battlePowerSortList.getListNum();
	}

	std::vector<stBattlePowerRankInfo> infoVec;
	m_battlePowerSortList.getFromN12N2(infoVec,rank,rank);
	if (infoVec.size()){
		return infoVec[0].m_charID;
	}
	else {
		return 0;
	}
}

UINT32 CRankMgr::getBattlePowerByCharID(UINT32 charID)
{
	CharID2BattlePower_IT it = m_charID2BattlePowerMap.find(charID);
	if (it != m_charID2BattlePowerMap.end()){
		return it->second;
	}
	else {
		return 0;
	}
}

UINT16 CRankMgr::getMyCopyMapRank(UINT32 charID)
{
	return m_copymapStarSortlList.getMySortRank(charID);
}

UINT16 CRankMgr::getMyBattlePowerRank(UINT32 charID)
{
	return m_battlePowerSortList.getMySortRank(charID);
}

void CRankMgr::getLevelSortListTop3(stHistoryRankInfo & info)
{
	std::vector<stLevelRankInfo> vec;
	m_levelSortList.getTopN(vec,3);

	for (UINT16 i = 0;i < vec.size();++i){
		stLevelRankInfo & data = vec[i];
		info.m_charID2Rank.insert(std::make_pair(data.m_charID, i + 1));
	}
}

void CRankMgr::getBattlePowerSortListTop3(stHistoryRankInfo & info)
{
	std::vector<stBattlePowerRankInfo> vec;
	m_battlePowerSortList.getTopN(vec,3);

	for (UINT16 i = 0;i < vec.size();++i){
		stBattlePowerRankInfo & data = vec[i];
		info.m_charID2Rank.insert(std::make_pair(data.m_charID, i + 1));
	}

}

void CRankMgr::getCopymapSortListTop3(stHistoryRankInfo & info)
{
	std::vector<stCopyMapRankInfo> vec;
	m_copymapStarSortlList.getTopN(vec,3);

	for (UINT16 i = 0;i < vec.size();++i){
		stCopyMapRankInfo & data = vec[i];
		info.m_charID2Rank.insert(std::make_pair(data.m_charID, i + 1));
	}
}

void CRankMgr::getLeitaiSortListTop3(stHistoryRankInfo & info)
{
	std::vector<stLeitaiRankInfo> vec;
	m_leitaiSortList.getTopN(vec,3);

	for (UINT16 i = 0;i < vec.size();++i){
		stLeitaiRankInfo & data = vec[i];
		info.m_charID2Rank.insert(std::make_pair(data.m_charID, i + 1));
	}
}

void CRankMgr::checkOpenSvrPowerRank()
{
	if (m_openSvrPowerRank.m_hasReward){//已经给过奖励了
		return ;
	}

	m_openSvrPowerRank.m_hasReward = 1;
	std::vector<stBattlePowerRankInfo> vec;
	m_battlePowerSortList.getTopN(vec,OPEN_SVR_POWER_RANK);

	for (UINT16 i = 0;i < vec.size();++i){
		stBattlePowerRankInfo & data = vec[i];
		stOpenSvrBPRankItem & rankItem = m_openSvrPowerRank.m_rank2CharIDs[i + 1];
		rankItem.m_charID = data.m_charID;
		rankItem.m_bpVal = data.m_battlePower;

		stBatchSendMailTask task;
		char szMailContent[50] = { 0 };
		task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_OPEN_SERVER_POWER_SORTLIST);
		snprintf(szMailContent,sizeof(szMailContent),"MailWnd_36;%d",i + 1);
		task.m_content = szMailContent;
		task.m_receiveIDs.insert(data.m_charID);
		
		OpenSvrPowerRankCfg::stReward *pReward = OpenSvrPowerRankCfg::getSingleton().getOpenSvrRankReward(i + 1);
		if (pReward){
			task.m_attaches.push_back(std::make_pair(pReward->m_boxID,1));   
		}
		CBatchSendMailMgr::getSingleton().addTask(task);
	}

	stOpenSvrPowerRankDB dbData;
	m_openSvrPowerRank.fillDataDB(dbData);
	updateOpenSvrPowerRank(dbData);
}

void CRankMgr::updateOpenSvrPowerRank(const stOpenSvrPowerRankDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%u",m_openSvrPowerRank.m_id);
	
	Athena::CUDSqlCallBack<CRankMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CRankMgr>(this,&CRankMgr::updateOpenSvrPowerRankCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_openSvrPowerRankTblDef,(const char *)&data,"OPEN_SVR_POWER_RANK",where,pDelaySql);
}

void CRankMgr::updateOpenSvrPowerRankCB(bool opResult,UINT64 autoID)
{

}

void CRankMgr::sendOpenSvrBPRank(Player * pUser)
{
	if (!pUser){
		return ;
	}

	SortProto::stRetOpenSvrBPRank retCmd;
	if (m_openSvrPowerRank.m_hasReward){//已经有奖励的情况下
		for (UINT8 i = 1;i <= 3;++i){
			std::map<UINT16,stOpenSvrBPRankItem>::iterator it = m_openSvrPowerRank.m_rank2CharIDs.find(i);
			if (it != m_openSvrPowerRank.m_rank2CharIDs.end()){//第一名
				stRoleBaseInfo *pRoleBaseInfo = RoleInfoManager::getSingleton().getRoleBaseInfo(it->second.m_charID);
				if (pRoleBaseInfo){
					SortProto::stBPRankItemInfo * pSyn = retCmd.add_items();
					if (pSyn){
						pSyn->set_charid(pRoleBaseInfo->m_charID);
						pSyn->set_name(pRoleBaseInfo->m_name);
						pSyn->set_level(getLevelByCharID(it->second.m_charID));
						pSyn->set_job(pRoleBaseInfo->m_job);
						pSyn->set_sex(pRoleBaseInfo->m_sex);
						pSyn->set_bpval(it->second.m_bpVal);
					}
				}
			}
		}
	}	
	else {
		std::vector<stBattlePowerRankInfo> vec;
		m_battlePowerSortList.getTopN(vec,3);
		
		for (UINT8 i = 0;i < vec.size();++i){
			stBattlePowerRankInfo & info = vec[i];
			SortProto::stBPRankItemInfo * pSyn = retCmd.add_items();

			if (pSyn){
				pSyn->set_charid(info.m_charID);
				pSyn->set_name(info.m_name);
				pSyn->set_job(info.m_job);
				pSyn->set_sex(info.m_sex);
				pSyn->set_bpval(info.m_battlePower);
			}
		}
	}
	retCmd.set_opensvrtime(COpenServerMgr::getSingleton().getOpenSvrTime());
	pUser->sendProtoToMe(retCmd);
}

void CRankMgr::synRecord(Player * pUser)
{
	if(!pUser){
		return;
	}

	if (m_levelSortList.isInSortList(pUser->getID())){                     
		stLevelRankInfo info = m_levelSortList.getSortInfo(pUser->getID());
		strncpy(info.m_name,pUser->getName(),MAX_NAMESIZE);
		info.m_photoID = pUser->m_photoID;
		m_levelSortList.updateSortInfo(info);                          
	}
	
	if (m_copymapStarSortlList.isInSortList(pUser->getID())){   
		stCopyMapRankInfo info = m_copymapStarSortlList.getSortInfo(pUser->getID());
		strncpy(info.m_name,pUser->getName(),MAX_NAMESIZE);                     
		info.m_photoID = info.m_photoID;
		m_copymapStarSortlList.updateSortInfo(info);                            
	}                                                                               

	if (m_battlePowerSortList.isInSortList(pUser->getID())){                            
		stBattlePowerRankInfo info = m_battlePowerSortList.getSortInfo(pUser->getID()); 
		strncpy(info.m_name,pUser->getName(),MAX_NAMESIZE);         
		info.m_photoID = pUser->m_photoID;
		m_battlePowerSortList.updateSortInfo(info);                                 
	}                                                                                   

	if (m_leitaiSortList.isInSortList(pUser->getID())){
		stLeitaiRankInfo info = m_leitaiSortList.getSortInfo(pUser->getID());
		strncpy(info.m_name, pUser->getName(), MAX_NAMESIZE);
		info.m_photoID = pUser->m_photoID;
		m_leitaiSortList.updateSortInfo(info);
		stLeitaiRankDB dbData;
		info.fillDBData(dbData);
		updateRecord(info.getKey(),(const char *)&dbData,LETAI_RANK);
	} 
}

UINT32 CRankMgr::getMaxLevelId() {
    std::vector<stLevelRankInfo> vec;
    m_levelSortList.getTopN(vec,1);
    if (vec.size()) {
        return vec[0].m_level;
    }
    return 0;
}
