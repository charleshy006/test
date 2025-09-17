#include "CaptureTreasure.h"
#include "Log4cxxHelp.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "TblQueryResult.h"
#include "PlayerManager.h"
#include "RankMgr.h"
#include "KVPair.h"
#include "RankMgr.h"
#include "Mail.h"
#include "Role.pb.h"
#include "CaptureCfg.h"
#include "SysLanguages.h"
#include "DiscoverMgr.h"

CTblField g_treasureTblDef[] = {
	{ "CHARID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",        CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "JOB",         CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",         CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "PEACEENDTIME",CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "DATA",        CTblField::DB_TYPE_STRING, TREASURE_DATA_LEN },
	{ "USERPHOTOID", CTblField::DB_TYPE_UINT16,   sizeof(UINT16)  },
	{ NULL, 0, 0 }, 
};

bool UserTreasureInfo::isValidRes(UINT32 resType)
{
	switch (resType) {
		case MiscProto::eTreasure_Yuntie1000_1 :			//陨铁1000碎片1
		case MiscProto::eTreasure_Yuntie1000_2 :			//陨铁1000碎片2
		case MiscProto::eTreasure_Yuntie3000_1 :			//陨铁3000碎片1
		case MiscProto::eTreasure_Yuntie3000_2 :			//陨铁3000碎片2
		case MiscProto::eTreasure_Yuntie3000_3 :			//陨铁3000碎片3
		case MiscProto::eTreasure_Yuntie5000_1 :			//陨铁5000碎片1
		case MiscProto::eTreasure_Yuntie5000_2 :			//陨铁5000碎片2
		case MiscProto::eTreasure_Yuntie5000_3 :			//陨铁5000碎片3
		case MiscProto::eTreasure_Yuntie5000_4 :			//陨铁5000碎片4
		case MiscProto::eTreasure_Jinglian_1  :				//精炼石碎片1
		case MiscProto::eTreasure_Jinglian_2  :				//精炼石碎片2
		case MiscProto::eTreasure_Jinglian_3  :				//精炼石碎片3
		case MiscProto::eTreasure_Small_Jinglian_Mine1 :	//小精炼矿碎片1
		case MiscProto::eTreasure_Small_Jinglian_Mine2 :   	//小精炼矿碎片2
		case MiscProto::eTreasure_Small_Jinglian_Mine3 :	//小精炼矿碎片3
		case MiscProto::eTreasure_Mid_Jinglian_Mine1  :		//中精炼石碎片1
		case MiscProto::eTreasure_Mid_Jinglian_Mine2  :  	//中精炼石碎片2
		case MiscProto::eTreasure_Mid_Jinglian_Mine3  :		//中精炼石碎片3
		case MiscProto::eTreasure_Mid_Jinglian_Mine4  :		//中精炼石碎片4
		case MiscProto::eTreasure_Big_Jinglian_Mine1  :		//大精炼矿碎片1
		case MiscProto::eTreasure_Big_Jinglian_Mine2  :		//大精炼矿碎片2
		case MiscProto::eTreasure_Big_Jinglian_Mine3  :		//大精炼矿碎片3
		case MiscProto::eTreasure_Big_Jinglian_Mine4  :		//大精炼矿碎片4
		case MiscProto::eTreasure_Big_Jinglian_Mine5  :		//大精炼矿碎片5
		case MiscProto::eTreasure_Fuwen_2Class_1 	  :		//2阶符文碎片1
		case MiscProto::eTreasure_Fuwen_2Class_2      :		//2阶符文碎片2
		case MiscProto::eTreasure_Fuwen_2Class_3      :		//2阶符文碎片3
		case MiscProto::eTreasure_Fuwen_10Class_1	  :		//10阶符文碎片1
		case MiscProto::eTreasure_Fuwen_10Class_2     :     //10阶符文碎片2
		case MiscProto::eTreasure_Fuwen_10Class_3     :     //10阶符文碎片3
		case MiscProto::eTreasure_Fuwen_10Class_4     :     //10阶符文碎片4
		case MiscProto::eTreasure_Fuwen_18Class_1     :		//18阶符文碎片1
		case MiscProto::eTreasure_Fuwen_18Class_2     :     //18阶符文碎片2
		case MiscProto::eTreasure_Fuwen_18Class_3     :     //18阶符文碎片3
		case MiscProto::eTreasure_Fuwen_18Class_4     :     //18阶符文碎片4
		case MiscProto::eTreasure_Fuwen_18Class_5     :     //18阶符文碎片5
		case MiscProto::eTreasure_Fuwen_20Class_1	  :     //20阶符文碎片1
		case MiscProto::eTreasure_Fuwen_20Class_2     :     //20阶符文碎片2
		case MiscProto::eTreasure_Fuwen_20Class_3     :     //20阶符文碎片3
		case MiscProto::eTreasure_Fuwen_20Class_4     :     //20阶符文碎片4
		case MiscProto::eTreasure_Fuwen_20Class_5     :     //20阶符文碎片5
		case MiscProto::eTreasure_Fuwen_26Class_1	  :		//26阶符文碎片1
		case MiscProto::eTreasure_Fuwen_26Class_2     :     //26阶符文碎片2
		case MiscProto::eTreasure_Fuwen_26Class_3     :     //26阶符文碎片3
		case MiscProto::eTreasure_Fuwen_26Class_4     :     //26阶符文碎片4
		case MiscProto::eTreasure_Fuwen_26Class_5     :     //26阶符文碎片5
		case MiscProto::eTreasure_Fuwen_26Class_6     :     //26阶符文碎片6
		
		case MiscProto::eTreasure_Fuwen_28Class_1_1	  :		//28阶符文碎片1
		case MiscProto::eTreasure_Fuwen_28Class_1_2     :     //28阶符文碎片2
		case MiscProto::eTreasure_Fuwen_28Class_1_3     :     //28阶符文碎片3
		case MiscProto::eTreasure_Fuwen_28Class_1_4     :     //28阶符文碎片4
		case MiscProto::eTreasure_Fuwen_28Class_1_5     :     //28阶符文碎片5
		case MiscProto::eTreasure_Fuwen_28Class_1_6     :     //28阶符文碎片6
		case MiscProto::eTreasure_Fuwen_28Class_2_1	  :		//28阶符文碎片1
		case MiscProto::eTreasure_Fuwen_28Class_2_2     :     //28阶符文碎片2
		case MiscProto::eTreasure_Fuwen_28Class_2_3     :     //28阶符文碎片3
		case MiscProto::eTreasure_Fuwen_28Class_2_4     :     //28阶符文碎片4
		case MiscProto::eTreasure_Fuwen_28Class_2_5     :     //28阶符文碎片5
		case MiscProto::eTreasure_Fuwen_28Class_2_6     :     //28阶符文碎片6
		case MiscProto::eTreasure_Fuwen_28Class_3_1	  :		//28阶符文碎片1
		case MiscProto::eTreasure_Fuwen_28Class_3_2     :     //28阶符文碎片2
		case MiscProto::eTreasure_Fuwen_28Class_3_3     :     //28阶符文碎片3
		case MiscProto::eTreasure_Fuwen_28Class_3_4     :     //28阶符文碎片4
		case MiscProto::eTreasure_Fuwen_28Class_3_5     :     //28阶符文碎片5
		case MiscProto::eTreasure_Fuwen_28Class_3_6     :     //28阶符文碎片6


		case MiscProto::eTreasure_Fuwen_30Class_1	  :		//30阶符文碎片1
		case MiscProto::eTreasure_Fuwen_30Class_2     :     //30阶符文碎片2
		case MiscProto::eTreasure_Fuwen_30Class_3     :     //30阶符文碎片3
		case MiscProto::eTreasure_Fuwen_30Class_4     :     //30阶符文碎片4
		case MiscProto::eTreasure_Fuwen_30Class_5     :     //30阶符文碎片5
		case MiscProto::eTreasure_Fuwen_30Class_6     :     //30阶符文碎片6


		case MiscProto::eTreasure_Small_Exp_Fushi_1   :		//小经验符石碎片1
		case MiscProto::eTreasure_Small_Exp_Fushi_2   :		//小经验符石碎片2
		case MiscProto::eTreasure_Small_Exp_Fushi_3   : 	//小经验符石碎片3
		case MiscProto::eTreasure_Mid_Exp_Fushi_1	   : 	//中经验符石碎片1
		case MiscProto::eTreasure_Mid_Exp_Fushi_2	   : 	//中经验符石碎片2
		case MiscProto::eTreasure_Mid_Exp_Fushi_3	   : 	//中经验符石碎片3
		case MiscProto::eTreasure_Mid_Exp_Fushi_4	   :	//中经验符石碎片4
		case MiscProto::eTreasure_Big_Exp_Fushi_1	   :	//大经验符石碎片1
		case MiscProto::eTreasure_Big_Exp_Fushi_2	   : 	//大经验符石碎片2
		case MiscProto::eTreasure_Big_Exp_Fushi_3	   : 	//大经验符石碎片3
		case MiscProto::eTreasure_Big_Exp_Fushi_4	   :	//大经验符石碎片4
		case MiscProto::eTreasure_Big_Exp_Fushi_5	   :	//大经验符石碎片5
		
		case MiscProto::eTreasure_GoldCoins_Card2_1  :    //2W金币卡碎片1
		case MiscProto::eTreasure_GoldCoins_Card2_2  :    //2W金币卡碎片2
		case MiscProto::eTreasure_GoldCoins_Card2_3  :    //2W金币卡碎片3

		case MiscProto::eTreasure_GoldCoins_Card5_1  :    //5W金币卡碎片1
		case MiscProto::eTreasure_GoldCoins_Card5_2  :    //5W金币卡碎片2
		case MiscProto::eTreasure_GoldCoins_Card5_3  :    //5W金币卡碎片3
		case MiscProto::eTreasure_GoldCoins_Card5_4  :    //5W金币卡碎片4

		case MiscProto::eTreasure_GoldCoins_Card10_1  :   //10W金币卡碎片1
		case MiscProto::eTreasure_GoldCoins_Card10_2  :   //10W金币卡碎片2
		case MiscProto::eTreasure_GoldCoins_Card10_3  :   //10W金币卡碎片3
		case MiscProto::eTreasure_GoldCoins_Card10_4  :   //10W金币卡碎片4
		case MiscProto::eTreasure_GoldCoins_Card10_5  :   //10W金币卡碎片5

		case MiscProto::eTreasure_Diamond_Card10_1    :   //10钻石卡碎片1
		case MiscProto::eTreasure_Diamond_Card10_2    :   //10钻石卡碎片2
		case MiscProto::eTreasure_Diamond_Card10_3    :   //10钻石卡碎片3

		case MiscProto::eTreasure_Diamond_Card20_1    :   //20钻石卡碎片1
		case MiscProto::eTreasure_Diamond_Card20_2    :   //20钻石卡碎片2
		case MiscProto::eTreasure_Diamond_Card20_3    :   //20钻石卡碎片3
		case MiscProto::eTreasure_Diamond_Card20_4    :   //20钻石卡碎片4

		case MiscProto::eTreasure_Diamond_Card50_1    :   //50钻石卡碎片1
		case MiscProto::eTreasure_Diamond_Card50_2    :   //50钻石卡碎片2
		case MiscProto::eTreasure_Diamond_Card50_3    :   //50钻石卡碎片3
		case MiscProto::eTreasure_Diamond_Card50_4    :   //50钻石卡碎片4
		case MiscProto::eTreasure_Diamond_Card50_5    :   //50钻石卡碎片4
			{
				return true;
			}
	}
	return false;
}

bool UserTreasureInfo::checkRes(UINT32 resType,UINT32 num)
{
	if (!isValidRes(resType)){
		return false;
	}

	std::map<UINT32,UINT32>::iterator it = m_res2Num.find(resType);
	if (it == m_res2Num.end()){
		return false;
	}
	return num <= it->second ;
}

bool UserTreasureInfo::addRes(UINT32 resType,UINT32 num)
{
	if (!isValidRes(resType)){
		return false;
	}

	m_res2Num[resType] += num;

	MiscProto::stSynTreasureNum retCmd;
	retCmd.set_restype((MiscProto::eTreasureType)resType);
	retCmd.set_num(m_res2Num[resType]);

	Player * pUser = PlayerManager::getSingleton().getUserByID(m_charID);
	if (pUser){
		pUser->sendProtoToMe(retCmd);
	}
	stUserTreasureDB dbData;
	fillDBData(dbData);
	CCaptureTreasureMgr::getSingleton().updateRecord(dbData);

	CCaptureTreasureMgr::getSingleton().addRes2StaticsCnt(resType,m_charID,CRankMgr::getSingleton().getLevelByCharID(m_charID));
	return true;
}

bool UserTreasureInfo::removeRes(UINT32 resType,UINT32 num)
{
	if (!isValidRes(resType)){
		return false;
	}
	
	std::map<UINT32,UINT32>::iterator it = m_res2Num.find(resType);
	if (it == m_res2Num.end()){
		return false;
	}
	
	if (num > it->second){
		return false;
	}
	it->second -= num;

	MiscProto::stSynTreasureNum retCmd;
	retCmd.set_restype((MiscProto::eTreasureType)resType);
	retCmd.set_num(it->second);
	Player * pUser = PlayerManager::getSingleton().getUserByID(m_charID);
	if (pUser){
		pUser->sendProtoToMe(retCmd);
	}
	
	if (it->second <= 0){
		m_res2Num.erase(it);
		CCaptureTreasureMgr::getSingleton().delRes2StaticsCnt(resType,m_charID,CRankMgr::getSingleton().getLevelByCharID(m_charID));
	}
	
	stUserTreasureDB dbData;
	fillDBData(dbData);
	CCaptureTreasureMgr::getSingleton().updateRecord(dbData);

	return true;
}

void UserTreasureInfo::fillDBData(stUserTreasureDB & dbData)
{
	std::ostringstream os;

	std::map<UINT32,UINT32>::iterator it = m_res2Num.begin();
	for (;it != m_res2Num.end();++it){
		os << it->first << "-" << it->second << ";";		
	}
	
	dbData.m_charID = m_charID;
	dbData.m_job = m_job;
	dbData.m_sex = m_sex;
	dbData.m_photoID = m_photoID; 
	strncpy(dbData.m_data,os.str().c_str(),sizeof(dbData.m_data));
	strncpy(dbData.m_name,m_name,sizeof(dbData.m_name));
	dbData.m_peaceEndTime = m_peaceEndTime;
}

void UserTreasureInfo::initFromDBData(stUserTreasureDB & dbData)
{
	m_charID = dbData.m_charID;
	m_job = dbData.m_job;
	m_sex = dbData.m_sex;
	m_photoID = dbData.m_photoID;
	strncpy(m_name,dbData.m_name,sizeof(m_name));
	m_peaceEndTime =  dbData.m_peaceEndTime;

	std::vector<std::string> strVec;

	g_stringtok(strVec,dbData.m_data,";");

	for (UINT8 i = 0;i < strVec.size();++i){
		std::vector<std::string> subStrVec;
		g_stringtok(subStrVec,strVec[i],"-");
		if (subStrVec.size() >= 2){
			UINT32 resType = (UINT32)atoi(subStrVec[0].c_str());	
			UINT32 num = (UINT32)atoi(subStrVec[1].c_str());
			if (num <= 0){
				continue;
			}
			m_res2Num.insert(std::make_pair(resType,num));
			CCaptureTreasureMgr::getSingleton().addRes2StaticsCnt(resType,m_charID,CRankMgr::getSingleton().getLevelByCharID(m_charID));
		}
	}
}

CCaptureTreasureMgr::CCaptureTreasureMgr()
{
}

CCaptureTreasureMgr::~CCaptureTreasureMgr()
{
	
}

bool CCaptureTreasureMgr::loadData()
{
	CTblQueryResult* result = g_databaseEngine.select(g_treasureTblDef,"CAPTURETRESURE",NULL,NULL,0,NULL); 
	if (!result){
		return false;
	}

	if (result->getData()){
		stUserTreasureDB * pTreasureDB = (stUserTreasureDB *)result->getData();     
		for (UINT32 i = 0; i < result->getRowCount();++i){
			UserTreasureInfo info;
			info.initFromDBData(pTreasureDB[i]);
			m_charID2ResInfo[info.getCharID()] = info;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("角色charID=%d,的宝藏数据=%s",pTreasureDB->m_charID,pTreasureDB->m_data);
#endif
		}
	}
	return true;
}

void CCaptureTreasureMgr::addRecord(const stUserTreasureDB & dbData)
{
	Athena::CUDSqlCallBack<CCaptureTreasureMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CCaptureTreasureMgr>(this,&CCaptureTreasureMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0; 
	g_databaseEngine.insertRecord(g_treasureTblDef,(const char *)&dbData,"CAPTURETRESURE",autoID,pDelaySql);
}

void CCaptureTreasureMgr::addRecordCB(bool opResult,UINT64 autoID)
{

}

void CCaptureTreasureMgr::updateRecord(const stUserTreasureDB & dbData)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",dbData.m_charID);
	Athena::CUDSqlCallBack<CCaptureTreasureMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CCaptureTreasureMgr>(this,&CCaptureTreasureMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_treasureTblDef,(const char *)&dbData,"CAPTURETRESURE",where,pDelaySql);
}
	
void CCaptureTreasureMgr::updateRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CCaptureTreasureMgr::delRecord(UINT32 charID)
{
	char where[65] = { 0 }; 
	bzero(where, 65); 
	snprintf(where, 64, "CHARID=%d",charID);
	Athena::CUDSqlCallBack<CCaptureTreasureMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CCaptureTreasureMgr>(this,&CCaptureTreasureMgr::delRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("CAPTURETRESURE",where,pDelaySql);
}

void CCaptureTreasureMgr::delRecordCB(bool opResult,UINT64 autoID)
{
	
}

void CCaptureTreasureMgr::addRes2StaticsCnt(UINT32 resType,UINT32 charID,UINT16 level)
{
	m_resStatics[resType][level].insert(charID);	
}

void CCaptureTreasureMgr::delRes2StaticsCnt(UINT32 resType,UINT32 charID,UINT16 level)
{
	m_resStatics[resType][level].erase(charID);	
}

void CCaptureTreasureMgr::sendUserTreasure(Player * pUser)
{
	if (!pUser){
		return ;
	}

	MiscProto::stRetCaptureTreasure retCmd;
	std::map<UINT32,UserTreasureInfo>::iterator it = m_charID2ResInfo.find(pUser->getID());	
	if (it == m_charID2ResInfo.end()){
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	UserTreasureInfo & info = it->second;
	
	std::map<UINT32,UINT32>::iterator subIt = info.m_res2Num.begin();
	for (;subIt != info.m_res2Num.end();++subIt){
		MiscProto::stSynCaptureTreasureInfo * pSyn = retCmd.add_tresures();
		if (pSyn){
			pSyn->set_restype(subIt->first);
			pSyn->set_resnum(subIt->second);
		}
	}
	UINT32 endTime = info.getPeaceEndTime();
	UINT32 remainTime = endTime > g_globalServer->getSec() ? endTime - g_globalServer->getSec() : 0;
	retCmd.set_remaintime(remainTime);
	pUser->sendProtoToMe(retCmd);
}

void CCaptureTreasureMgr::treasureCombine(Player * pUser,UINT32 combineType)
{
	if (!pUser){
		return ;
	}	
	
	MiscProto::stCombineTreasureResult retCmd;

	std::map<UINT32,UserTreasureInfo>::iterator it = m_charID2ResInfo.find(pUser->getID());	
	if (it == m_charID2ResInfo.end()){
		retCmd.set_result(MiscProto::stCombineTreasureResult::eTREASURE_NOT_ENOUGH);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	UserTreasureInfo & treasureInfo = it->second;

	std::map<UINT32,stTreasureCombineCfg>::iterator cfgIt = m_combineCfg.find(combineType);	
	if (cfgIt == m_combineCfg.end()){
		retCmd.set_result(MiscProto::stCombineTreasureResult::eINNER_ERROR);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	stTreasureCombineCfg & cfg = cfgIt->second;
	
	for (UINT8 i = 0;i < cfg.m_needTreasures.size(); ++i){
		if (!treasureInfo.checkRes(cfg.m_needTreasures[i],1)){
			retCmd.set_result(MiscProto::stCombineTreasureResult::eTREASURE_NOT_ENOUGH);
			pUser->sendProtoToMe(retCmd);
			return ;
		}	
	}

	for (UINT8 i = 0;i < cfg.m_needTreasures.size(); ++i){
		treasureInfo.removeRes(cfg.m_needTreasures[i],1);	
	}

	retCmd.set_result(MiscProto::stCombineTreasureResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);

	Global::stTreasureCombineSuccess cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_itemid(cfg.m_combineItemID);
	pUser->sendProto2Game(cmd);
}

void g_sendCaptureTreasureMailCB(UINT64 autoID)
{

}

void CCaptureTreasureMgr::captureUserTreasure(Player * pUser,UINT32 charID,UINT32 combineID,UINT32 resType,UINT8 num,bool multiCapture)
{
	if (!pUser){
		return ;
	}
	MiscProto::stCaptureUserTreasureResult retCmd;
	struct tm _tm;
	RealTime::getLocalTime(_tm,g_globalServer->getSec());
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;
	if (charID != 0){//非机器人的情况下
		if (!CCaptureCfg::getSingleton().canCapture(secNum)){
			retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eNOT_IN_OPEN_TIME);
			pUser->sendProtoToMe(retCmd);
			return ;
		}
	}

	Global::stSendModuleLog  webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_charid(pUser->getID());
	webCmd.set_level(pUser->m_level);
	webCmd.set_module("夺宝");
	webCmd.set_opid(pUser->m_opID);
	webCmd.set_viplv(pUser->m_vipLv);
	webCmd.set_othercharid(g_int2string(charID));

	std::map<UINT32,stTreasureCombineCfg>::iterator cfgIt = m_combineCfg.find(combineID);
	if (cfgIt == m_combineCfg.end()){
		retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eINNER_ERROR);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (0 != charID){
		setPeaceEndTime(pUser,g_globalServer->getSec());
	}

	bool win = false;		//是否胜利

	if (0 == charID){//0表示机器人
		win = true;
	}
	else {
		UINT32 otherBattlePower = CRankMgr::getSingleton().getBattlePowerByCharID(charID);
		otherBattlePower = otherBattlePower == 0 ? 1 : otherBattlePower;
		UINT32 bpCoef =(UINT32)(((pUser->m_battlePower * 1.0f )/ otherBattlePower)  * 1000);
		
		if (bpCoef >= 1025){//必定胜利
			win = true;
		}
		else if (bpCoef >= 975){
			if (g_selectByPercent(50)){//胜利
				win = true;
			}
		}
		else {
			win = false;
		}
	}

	CDiscoverMgr::getSingleton().checkFightType(pUser->getID(),6 ,4);

	if (!win){//抢夺失败
		retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eFAIL);
		pUser->sendProtoToMe(retCmd);
		Global::stCaptureTreasureFail failCmd;
		//failCmd.set_charid(charID);
		failCmd.set_charid(pUser->getID());
		pUser->sendProto2Game(failCmd);

		webCmd.set_result(false);
		g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
		return ;
	}
	
	std::map<UINT32,UserTreasureInfo>::iterator it = m_charID2ResInfo.end();
	if (0 != charID){
		it = m_charID2ResInfo.find(charID);	
		if (it == m_charID2ResInfo.end()){
			retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eFAIL);
			pUser->sendProtoToMe(retCmd);
			Global::stCaptureTreasureFail failCmd;
			//failCmd.set_charid(charID);
			failCmd.set_charid(pUser->getID());
			pUser->sendProto2Game(failCmd);
			return ;
		}

		if (it->second.getPeaceEndTime() > g_globalServer->getSec()){
			Athena::logger->error("抢夺失败,改玩家处于和平模式,客户端发送协议有问题,可能使用了挂!");
			return  ;
		}
	}
	
	Global::stCaptureReward cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_dropid(cfgIt->second.m_dropID);
	cmd.set_expcoef(cfgIt->second.m_expCoef);
	cmd.set_moneycoef(cfgIt->second.m_moneyCoef);
	cmd.set_multicapture(multiCapture);

	bool getPiece = false;
	for (UINT16 i =0 ;i < num; ++i){
		//在胜利的情况下
		if (0 == charID){//如果是机器人
			if (g_selectByTenThound(cfgIt->second.m_robotRate)){
				getPiece = true;
			}		
		}
		else {
			if (g_selectByTenThound(cfgIt->second.m_playerRate)){
				getPiece = true;
			}		
		}
		cmd.set_num(i + 1);
		if (getPiece){//抢到就结束
			break;
		}
	}

	retCmd.set_obtainpiece(getPiece);
	cmd.set_obtainpiece(getPiece);

	if (getPiece && (0 != charID) && (it != m_charID2ResInfo.end())) {//不是机器人要扣除他的资源
		if (!it->second.removeRes(resType,1)){
			retCmd.set_result(MiscProto::stCaptureUserTreasureResult::eFAIL);
			pUser->sendProtoToMe(retCmd);
			return ;
		}
		//发送邮件
		stMailDBInfo mailInfo;
		mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
		mailInfo.m_receiverID = charID;
		strncpy(mailInfo.m_receiverName,it->second.getName(),sizeof(mailInfo.m_receiverName));
		strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
		strncpy(mailInfo.m_title,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SNATCH_SYSTEM),sizeof(mailInfo.m_title));
#ifdef _THAILAND_VER
		char mailCnt[400] = { 0 };
#else
		char mailCnt[250] = { 0 };
#endif
		snprintf(mailCnt,sizeof(mailCnt),"MailWnd_5;%s;%d",pUser->getName(),resType);
		strncpy(mailInfo.m_content,mailCnt,sizeof(mailInfo.m_content));
		mailInfo.m_sendTime = g_globalServer->getSec();
		MailManager::getSingleton().sendMail(mailInfo,g_sendCaptureTreasureMailCB);
	}

	if (getPiece){//获得碎片
		it = m_charID2ResInfo.find(pUser->getID());
		if (it == m_charID2ResInfo.end()){
			stUserTreasureDB dbData;
			dbData.m_charID = pUser->getID();
			dbData.m_job = pUser->m_job;
			dbData.m_sex = pUser->m_sex;
			dbData.m_photoID = pUser->m_photoID;
			snprintf(dbData.m_data,sizeof(dbData.m_data),"%d-1",resType);
			strncpy(dbData.m_name,pUser->getName(),sizeof(dbData.m_name));
			CCaptureTreasureMgr::getSingleton().addRecord(dbData);
			m_charID2ResInfo[pUser->getID()].initFromDBData(dbData);
			MiscProto::stSynTreasureNum synResCmd;
			synResCmd.set_restype((MiscProto::eTreasureType)resType);
			synResCmd.set_num(1);
			pUser->sendProtoToMe(synResCmd);
		}
		else {
			UserTreasureInfo & resInfo = m_charID2ResInfo[pUser->getID()];
			resInfo.addRes(resType,1);
		}
		
		if (pUser){
			Athena::logger->trace("玩家(name:%s,id:%d)在夺宝中获得了碎片:%d",pUser->getName(),pUser->getID(),resType);
		}
	}
	pUser->sendProto2Game(cmd);
	webCmd.set_result(true);
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

bool CCaptureTreasureMgr::loadConfig()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/Indiana.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[宝藏配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[宝藏配置]宝箱配置加载错误,找不到Root节点!");
		return false;
	}  

	INT32 iVal = 0;
	TiXmlElement * pIndianaNode = pRoot->FirstChildElement("Indiana");
	if (pIndianaNode){
		TiXmlElement * pIndianagroupNode = pIndianaNode->FirstChildElement("Indianagroup");
		while (pIndianagroupNode){
			stTreasureCombineCfg one;	
			if (pIndianagroupNode->Attribute("Synthesisitem",&iVal)){
				one.m_combineItemID = iVal;
			}
			
			if (pIndianagroupNode->Attribute("Robotprobability",&iVal)){
				one.m_robotRate = iVal;
			}
			
			if (pIndianagroupNode->Attribute("Playerprobability",&iVal)){
				one.m_playerRate = iVal;
			}
			
			if (pIndianagroupNode->Attribute("DropID",&iVal)){
				one.m_dropID = iVal;
			}

			if (pIndianagroupNode->Attribute("exe",&iVal)){
				one.m_expCoef = iVal;
			}

			if (pIndianagroupNode->Attribute("money",&iVal)){
				one.m_moneyCoef = iVal;
			}

			
			TiXmlElement * pEntryNode = pIndianagroupNode->FirstChildElement("entry");
			while (pEntryNode){
				if (pEntryNode->Attribute("Fragmentitem",&iVal)){
					one.m_needTreasures.push_back(iVal);
				}
				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}
			m_combineCfg[one.m_combineItemID] = one;
			pIndianagroupNode = pIndianagroupNode->NextSiblingElement("Indianagroup");
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印夺宝配置:");
	std::map<UINT32,stTreasureCombineCfg>::iterator it = m_combineCfg.begin();
	for (;it != m_combineCfg.end();++it){
		Athena::logger->trace("合成itemID=%d robotRate=%d,userRate=%d,dropID=%d,expCoef=%d,moneyCoef=%d,需要的碎片ID:",it->first,it->second.m_robotRate,it->second.m_playerRate,it->second.m_dropID,it->second.m_expCoef,it->second.m_moneyCoef);
		for (UINT16 i = 0;i < it->second.m_needTreasures.size();++i){
			Athena::logger->trace("碎片ID=%d",it->second.m_needTreasures[i]);		
		}
	}
#endif
	return true;	
}

void CCaptureTreasureMgr::addUserRes(Player * pUser,UINT32 resID,UINT32 num)
{
	if (!pUser){
		return ;
	}

	if (m_charID2ResInfo.find(pUser->getID()) == m_charID2ResInfo.end()){
		stUserTreasureDB dbData;
		dbData.m_charID = pUser->getID();
		dbData.m_job = pUser->m_job;
		dbData.m_sex = pUser->m_sex;
		dbData.m_photoID = pUser->m_photoID;
		snprintf(dbData.m_data,sizeof(dbData.m_data),"%d-%d",resID,num);
		strncpy(dbData.m_name,pUser->getName(),sizeof(dbData.m_name));
		CCaptureTreasureMgr::getSingleton().addRecord(dbData);
		m_charID2ResInfo[pUser->getID()].initFromDBData(dbData);
	}
	else {
		m_charID2ResInfo[pUser->getID()].addRes(resID,num);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("玩家charID=%d,resID=%d,num=%d",pUser->getID(),resID,num);
#endif
}

void CCaptureTreasureMgr::sendChallegeList(Player * pUser,UINT32 resID)
{
	if (!pUser){
		return ;
	}	
	
	struct tm _tm;
	RealTime::getLocalTime(_tm,g_globalServer->getSec());
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;
	bool bAllRobot = false;
	if (!CCaptureCfg::getSingleton().canCapture(secNum)){
		bAllRobot = true;
	}

	MiscProto::stRetChallengeUserList retCmd;
	if (!bAllRobot){
		UINT16 beginLv = 0;
		if (pUser->m_level > 2){
			beginLv = pUser->m_level - 2;	
		}
		else {
			beginLv = 1;
		}

		UINT16 endLv = pUser->m_level + 5;

		Statics_IT it = m_resStatics.find(resID);
		if (it != m_resStatics.end()){//资源ID对应的玩家
			for (UINT16 lv = beginLv; lv <= endLv ; ++lv){//等级对应的玩家
				if (retCmd.userinfo_size() >= (s_MAX_CHALLENGE_LIST / 2)){
					break;
				}

				Level2Users_IT subIt =  it->second.find(lv);
				if (subIt != it->second.end()){
					std::set<UINT32>::iterator thdIt = subIt->second.begin();
					for (;thdIt != subIt->second.end();++thdIt){
						std::map<UINT32,UserTreasureInfo>::iterator userIt =  m_charID2ResInfo.find(*thdIt);
						if ((userIt != m_charID2ResInfo.end()) && (userIt->second.m_charID != pUser->getID())){
							UserTreasureInfo & userInfo = userIt->second;
							if (userInfo.getPeaceEndTime() > g_globalServer->getSec()){
								continue;
							}
							MiscProto::stCaptureTreasureUserInfo * pSyn = retCmd.add_userinfo();
							if (pSyn){
								pSyn->set_charid(userInfo.getCharID());
								pSyn->set_job(userInfo.getJob());
								pSyn->set_name(userInfo.getName());
								pSyn->set_sex(userInfo.getSex());
								pSyn->set_photoid(userInfo.getPhotoID());
								pSyn->set_level(CRankMgr::getSingleton().getLevelByCharID(userInfo.getCharID()));
							}
						}
						if (retCmd.userinfo_size() >= (s_MAX_CHALLENGE_LIST / 2)){
							break;
						}
					}
				}
			}
		}
	}

	if (retCmd.userinfo_size() < s_MAX_CHALLENGE_LIST){//还小于这个数,放机器人
		UINT16 robotNum = s_MAX_CHALLENGE_LIST - retCmd.userinfo_size();
		for (UINT16 i = 0; i < robotNum; ++i){
			MiscProto::stCaptureTreasureUserInfo * pSyn = retCmd.add_userinfo();
			if (pSyn){
				pSyn->set_charid(0);
				pSyn->set_sex(g_randBetween(Role::eSEX_MAN,Role::eSEX_FEMALE));
				pSyn->set_job(g_randBetween(Role::eJOB_WARRIOR,Role::eJOB_MASTER));
				pSyn->set_level(pUser->m_level);
				pSyn->set_photoid(1);
			}
		}
	}

	pUser->sendProtoToMe(retCmd);
}

void CCaptureTreasureMgr::setPeaceEndTime(Player * pUser,UINT32 endTime)
{
	if (!pUser){
		return ;
	}

	std::map<UINT32,UserTreasureInfo>::iterator it = m_charID2ResInfo.find(pUser->getID());
	if (it == m_charID2ResInfo.end()){
		stUserTreasureDB dbData;
		dbData.m_charID = pUser->getID();
		dbData.m_job = pUser->m_job;
		dbData.m_sex = pUser->m_sex;
		strncpy(dbData.m_name,pUser->getName(),sizeof(dbData.m_name));
		dbData.m_peaceEndTime = endTime;
		dbData.m_photoID = pUser->m_photoID;
		addRecord(dbData);
		return ;
	}
	it->second.setPeaceEndTime(endTime);
	stUserTreasureDB dbData;
	it->second.fillDBData(dbData);
	updateRecord(dbData);
}

void CCaptureTreasureMgr::onUserLvChange(Player * pUser,UINT16 oldLv,UINT16 newLv)
{
	if (!pUser){
		return ;
	}

	std::map<UINT32,UserTreasureInfo>::iterator it = m_charID2ResInfo.find(pUser->getID());
	if (it == m_charID2ResInfo.end()){
		return ;
	}
	UserTreasureInfo & treasureInfo = it->second;
	
	std::map<UINT32,UINT32>::iterator subIt = treasureInfo.m_res2Num.begin();
	for (;subIt != treasureInfo.m_res2Num.end();++subIt){
		if (subIt->second > 0 ){
			m_resStatics[subIt->first][oldLv].erase(pUser->getID());
			m_resStatics[subIt->first][newLv].insert(pUser->getID());
		}
	}
}

void CCaptureTreasureMgr::synRecord(Player * pUser)
{
	if(!pUser){
		return;
	}
	
	std::map<UINT32,UserTreasureInfo>::iterator it = m_charID2ResInfo.find(pUser->getID());
	if (it == m_charID2ResInfo.end()){                                                     
		return;
	}

	stUserTreasureDB dbData;
	strncpy(it->second.m_name, pUser->getName(), sizeof(it->second.m_name));
	it->second.m_photoID = pUser->m_photoID;
	it->second.fillDBData(dbData);
	
	updateRecord(dbData);
}
