#include "DiscoverMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Player.h"
#include "DiscoverCfg.h"
#include "Misc.pb.h"
#include "PlayerManager.h"

extern GlobalServer  * g_globalServer;

void stDiscoverInfo::initFromDB(stDiscoverDB & data)
{
	m_charID		=	data.m_charID;
	m_mapID			=	data.m_mapID;
	m_place			=	data.m_place;
	m_taskID		=	data.m_taskID;
	m_state			=	data.m_state;
	m_battleNum		=	data.m_battleNum;
	m_integral		=	data.m_integral;
	m_refreshNum	=	data.m_refreshNum;
	m_playDiceNum	=	data.m_playDiceNum;
	m_canUseNum		=	data.m_canUseNum;
	m_hasReceivedID	=	data.m_hasReceivedID;
}

void stDiscoverInfo::fillDBData(stDiscoverDB & data)
{
	data.m_charID			=	m_charID;
	data.m_mapID			=	m_mapID;
	data.m_place			=	m_place;
	data.m_taskID			=	m_taskID;
	data.m_state			=	m_state;
	data.m_battleNum		=	m_battleNum;
	data.m_integral			=	m_integral;
	data.m_refreshNum		=	m_refreshNum;
	data.m_playDiceNum		=	m_playDiceNum;
	data.m_canUseNum		=	m_canUseNum;
	data.m_hasReceivedID	=	m_hasReceivedID;
}

CTblField g_discoverInfoTblDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "MAPID",      	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "PLACE",        	CTblField::DB_TYPE_UINT8,   sizeof(UINT8) 	},
	{ "TASKID",         CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "STATE",        	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)	},
	{ "BATTLENUM",		CTblField::DB_TYPE_UINT8,   sizeof(UINT8)	},
	{ "INTEGRAL",       CTblField::DB_TYPE_UINT32,  sizeof(UINT32)	},
	{ "REFRESHNUM",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "PLAYDICENUM",	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "CANUSENUM",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "HASRECEIVEDID",  CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ NULL, 0, 0 },
};

CDiscoverMgr::CDiscoverMgr()
{
	m_zeroRefresh = false;
}

void CDiscoverMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_discoverInfoTblDef,"DISCOVER_INFO",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stDiscoverDB * pData = (stDiscoverDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stDiscoverInfo & discoverInfo = m_allDiscoverInfos[pData[i].m_charID];
			discoverInfo.initFromDB(pData[i]);
		}
	}

	SAFE_DELETE(result);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("秘境寻宝：size：%d",m_allDiscoverInfos.size());
	DiscoverInfos_IT it = m_allDiscoverInfos.begin();
	for(;it != m_allDiscoverInfos.end(); ++it){
		stDiscoverInfo & one = it->second;
		Athena::logger->trace("key:%d,m_charID:%d, m_mapID:%d,m_place:%d, m_state:%d,m_battleNum, m_integral:%d,m_refreshNum:%d, m_playDiceNum:%d", it->first,one.m_mapID, one.m_charID, one.m_place, one.m_state, one.m_battleNum, one.m_integral, one.m_refreshNum, one.m_playDiceNum); 
	}
#endif
}

void CDiscoverMgr::timer(UINT32 cur)
{
	//5点清空
	struct tm curTm;
	RealTime::getLocalTime(curTm,cur);
	if ((curTm.tm_hour == 5) && (curTm.tm_min == 0) && (curTm.tm_sec < 10) && (!m_zeroRefresh)){
		m_zeroRefresh  = true;
		m_allDiscoverInfos.clear();
		removeAllRecord();
	}
	else if (curTm.tm_sec > 10){
		m_zeroRefresh  = false;
	}
}

void CDiscoverMgr::addRecord(const char * pData)
{
	Athena::CUDSqlCallBack<CDiscoverMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDiscoverMgr>(this,&CDiscoverMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_discoverInfoTblDef, pData, "DISCOVER_INFO", autoID, pDelaySql);
}

void CDiscoverMgr::addRecordCB(bool opResult,UINT64 autoID)
{}

void CDiscoverMgr::updateRecord(UINT32 key,const char * pData)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",key);

	Athena::CUDSqlCallBack<CDiscoverMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDiscoverMgr>(this,&CDiscoverMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_discoverInfoTblDef, pData, "DISCOVER_INFO", where, pDelaySql);
}

void CDiscoverMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void CDiscoverMgr::removeRecord(UINT32 key)
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere,sizeof(szWhere),"CHARID=%u",key);
	Athena::CUDSqlCallBack<CDiscoverMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDiscoverMgr>(this,&CDiscoverMgr::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("DISCOVER_INFO",szWhere,pDelaySql);
}

void CDiscoverMgr::removeRecordCB(bool opResult,UINT64 autoID)
{}

void CDiscoverMgr::removeAllRecord()
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere,sizeof(szWhere),"CHARID>0");
	Athena::CUDSqlCallBack<CDiscoverMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CDiscoverMgr>(this,&CDiscoverMgr::removeAllRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("DISCOVER_INFO",szWhere,pDelaySql);
}

void CDiscoverMgr::removeAllRecordCB(bool opResult,UINT64 autoID)
{}

void CDiscoverMgr::sendActivityInfo(Player * pUser)
{
	if(!pUser){
		return ;
	}	

	MiscProto::stOpenDiscoverWindowResult retCmd;
	stDiscoverInfo *info = getUserInfo(pUser->getID());
	if(!info){
		UINT8 mapID = CDiscoverCfg::getSingleton().randMapID();
		retCmd.set_mapid(mapID);
		retCmd.set_place(0);
		retCmd.set_taskid(0);
		retCmd.set_state(TASK_STATE_GIVEUP);
		retCmd.set_battlenum(0);
		retCmd.set_integral(0);
		retCmd.set_refreshnum(0);
		retCmd.set_playdicenum(0);
		retCmd.set_canusenum(CDiscoverCfg::getSingleton().getFreeNum());
		retCmd.set_hasreceivedidentify(0);

		stDiscoverDB dbData;
		stDiscoverInfo & discoverInfo = m_allDiscoverInfos[pUser->getID()];
		discoverInfo.m_charID = pUser->getID();
		discoverInfo.m_mapID = mapID;
		discoverInfo.m_state = TASK_STATE_GIVEUP;
		discoverInfo.m_canUseNum = CDiscoverCfg::getSingleton().getFreeNum();

		discoverInfo.fillDBData(dbData);
		addRecord((const char *)&dbData);

	}else{
		retCmd.set_mapid(info->m_mapID);
		retCmd.set_place(info->m_place);
		retCmd.set_taskid(info->m_taskID);
		retCmd.set_state(info->m_state);
		retCmd.set_battlenum(info->m_battleNum);
		retCmd.set_integral(info->m_integral);
		retCmd.set_refreshnum(info->m_refreshNum);
		retCmd.set_playdicenum(info->m_playDiceNum);
		retCmd.set_canusenum(info->m_canUseNum);
		retCmd.set_hasreceivedidentify(info->m_hasReceivedID);
	}
	pUser->sendProtoToMe(retCmd);
}

stDiscoverInfo * CDiscoverMgr::getUserInfo(UINT32 charID){
	DiscoverInfos_IT it = m_allDiscoverInfos.find(charID);
	if(it == m_allDiscoverInfos.end()){
		return NULL;
	}
	return &it->second;
}

void CDiscoverMgr::playDice(Player *pUser)
{
	if(!pUser){
		return ;
	}

	UINT32 point = rand() % 6 + 1;//1~6

	UINT32 indexPot = 0;//记录点数
	UINT32 indexPos = 0;//记录当前位置

	MiscProto::stPlayDiceResult retCmd;

	stDiscoverInfo * info = getUserInfo(pUser->getID());

	if(!info){
		retCmd.set_result(MiscProto::stPlayDiceResult::eInner_Error);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)掷骰子失败!",pUser->getID(), pUser->getName());
#endif
		return;
	}

	stDiscoverInfo & discoverInfo = m_allDiscoverInfos[pUser->getID()];

	if(discoverInfo.m_playDiceNum >= discoverInfo.m_canUseNum){
		retCmd.set_result(MiscProto::stPlayDiceResult::eNot_Dice_NUM);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)掷骰子失败，次数不足",pUser->getID(), pUser->getName());
#endif
		return;
	}

	if(!isFinish(pUser)){
		retCmd.set_result(MiscProto::stPlayDiceResult::eTask_Not_Finish); 
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)掷骰子失败，任务还未完成，小子可能开挂了",pUser->getID(), pUser->getName());
#endif
		return ;
	}

	UINT8 maxSquareNum = CDiscoverCfg::getSingleton().getMapSquareNum(discoverInfo.m_mapID); 

	if(discoverInfo.m_place >= maxSquareNum){
		retCmd.set_result(MiscProto::stPlayDiceResult::eThe_End);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)掷骰子失败,已经走完了!",pUser->getID(), pUser->getName());
#endif
		return;
	}

	if(discoverInfo.m_place + point > maxSquareNum){

		indexPot = maxSquareNum - discoverInfo.m_place;
		indexPos = maxSquareNum;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)掷骰子点数:%d，当前在第%d格",pUser->getID(), pUser->getName(), indexPot, indexPos);
#endif
	}else{
		indexPot = point;
		indexPos = discoverInfo.m_place + point;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)掷骰子点数:%d，当前在第%d格",pUser->getID(), pUser->getName(), indexPot, indexPos);
#endif
	}

	stSquareType * type = CDiscoverCfg::getSingleton().randSquareTask(discoverInfo.m_mapID, indexPos);
	if(!type){
		retCmd.set_result(MiscProto::stPlayDiceResult::eInner_Error);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)掷骰子失败,找不到配置!,mapid:%d, indexPos:%d, level:%d", pUser->getID(), pUser->getName(),discoverInfo.m_mapID, indexPos, pUser->m_level); 
#endif
		return ;
	}

	std::pair<UINT8,UINT8> taskType = CDiscoverCfg::getSingleton().getSquareType(discoverInfo.m_mapID,indexPos);
	if(taskType.first == eSQUARE_TYPE_DIAMOND
			|| taskType.first == eSQUARE_TYPE_ITEM_BOX
			|| taskType.first == eSQUARE_TYPE_AFTER_BOX
			|| taskType.first == eSQUARE_TYPE_END
			|| taskType.first == eSQUARE_TYPE_BLANK){
		retCmd.set_state(TASK_STATE_FINISH);
		discoverInfo.m_state = TASK_STATE_FINISH;
	}else{
		retCmd.set_state(TASK_STATE_NOT_RECEIVE); 
		discoverInfo.m_state = TASK_STATE_NOT_RECEIVE;
	}

	discoverInfo.m_place = indexPos;
	discoverInfo.m_playDiceNum += 1;
	discoverInfo.m_taskID = type->m_id;

	retCmd.set_point(indexPot);               
	retCmd.set_taskid(discoverInfo.m_taskID); 
	retCmd.set_mapid(discoverInfo.m_mapID);   

	stDiscoverDB dbData;                                 
	discoverInfo.fillDBData(dbData);                     
	updateRecord(discoverInfo.m_charID, (const char *)&dbData); 

	pUser->sendProtoToMe(retCmd);

    Global::stDiscoverConsume broadCmd;
    broadCmd.set_charid(pUser->getID());
    g_globalServer->getConnMgr().broadcastByType(GAMESERVER, broadCmd);
}

void CDiscoverMgr::refreshNum(Player *pUser)
{
	if(!pUser){
		return ;
	}

	MiscProto::stRefreshNumResult retCmd;

	stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID());
	if(!pDiscoverInfo){
		retCmd.set_result(MiscProto::stRefreshNumResult::eInner_Error);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)重置失败,找不到配置!",pUser->getID(), pUser->getName());
#endif
		return ;
	}

	if(pDiscoverInfo->m_playDiceNum < pDiscoverInfo->m_canUseNum){
		retCmd.set_result(MiscProto::stRefreshNumResult::eNum_Useless_Over);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)重置失败,次数还没用完!",pUser->getID(),pUser->getName());
#endif
		return ;
	}

	//通知game 扣钱
	Global::stDeductMoney cmd;
	cmd.set_num(CDiscoverCfg::getSingleton().getNeedDiamondNum(pDiscoverInfo->m_refreshNum + 1));
	cmd.set_charid(pUser->getID());
	pUser->sendProto2Game(cmd);
}

void CDiscoverMgr::finishRefresh(Global::stDeductMoneyResult *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	MiscProto::stRefreshNumResult retCmd;
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!pUser){
		return;
	}
	if(recvCmd->result() == Global::stDeductMoneyResult::eSuccess){
		stDiscoverDB dbData;                                          
		stDiscoverInfo &info = m_allDiscoverInfos[pUser->getID()];    
		UINT8 count = 5;//购买次数
		info.m_refreshNum += 1;                                       
		info.m_canUseNum += count;
		info.fillDBData(dbData);                                      
		updateRecord(pUser->getID(), (const char *)&dbData);
#ifdef _HDZ_DEBUG 
		Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)增加%d次刷新次数共%d次！",pUser->getID(), pUser->getName(), CDiscoverCfg::getSingleton().getFreeNum(),info.m_canUseNum);
#endif
		retCmd.set_count(count);
		retCmd.set_result(MiscProto::stRefreshNumResult::eSuccess); 
		pUser->sendProtoToMe(retCmd);
	}else{
		retCmd.set_result(MiscProto::stRefreshNumResult::eDIAMOND_NOT_ENOUGH);
		pUser->sendProtoToMe(retCmd);
	}
}

bool CDiscoverMgr::isFinish(Player * pUser)
{
	stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID());
	if(!pDiscoverInfo){
		return false;
	}

	if(pDiscoverInfo->m_state < TASK_STATE_FINISH){
		return false;
	}

	return true;
}

void CDiscoverMgr::getTask(Player * pUser)
{
	if(!pUser){
		return ;
	}
	MiscProto::stGetTaskResult retCmd;

	stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID());
	if(!pDiscoverInfo){
		retCmd.set_result(MiscProto::stGetTaskResult::eInner_Error);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取任务失败!",pUser->getID(),pUser->getName());
#endif
		return ;
	}

	if(pDiscoverInfo->m_state != TASK_STATE_NOT_RECEIVE){
		retCmd.set_result(MiscProto::stGetTaskResult::eTask_Not_Finish);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取任务失败，任务还未完成",pUser->getID(),pUser->getName());
#endif
		return ;
	}

	std::pair<UINT8,UINT8> taskType = CDiscoverCfg::getSingleton().getSquareType(pDiscoverInfo->m_mapID, pDiscoverInfo->m_place);
	if(0 == taskType.first){
		retCmd.set_result(MiscProto::stGetTaskResult::eInner_Error);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取任务失败，内部错误，找不到配置",pUser->getID(),pUser->getName());
#endif
	}

	stDiscoverDB dbData;

	stDiscoverInfo &info = m_allDiscoverInfos[pUser->getID()];
	info.m_taskID = info.m_taskID;
	//需要进2/3/4/类型关卡的状态设为1，以便挑战失败还能进入
	if(taskType.first == eSQUARE_TYPE_GOLD
			|| taskType.first == eSQUARE_TYPE_DRAGON
			|| taskType.first == eSQUARE_TYPE_BOSS){
		info.m_state = TASK_STATE_NOT_RECEIVE;
	}else{
		info.m_state = TASK_STATE_UNFINISHED;
	}
	info.fillDBData(dbData);
	updateRecord(pUser->getID(), (const char *)&dbData);

	retCmd.set_tasktype(taskType.first);
	retCmd.set_taskid(info.m_taskID);
	retCmd.set_result(MiscProto::stGetTaskResult::eSuccess);
	pUser->sendProtoToMe(retCmd);

	if(taskType.first == eSQUARE_TYPE_GOLD 
			|| taskType.first == eSQUARE_TYPE_DRAGON
			|| taskType.first == eSQUARE_TYPE_BOSS){
		Global::stReqEnterTaskLv cmd;
		stSquareType * type = CDiscoverCfg::getSingleton().getSquareInfo(info.m_mapID, info.m_place, info.m_taskID);

		if(!type){
			return;
		}

		cmd.set_charid(pUser->getID());
		UINT32 levelID = type->m_level.m_levelIDVer[0];

		for(UINT16 i = 0; i < type->m_level.m_playLevelVer.size(); ++i){
			if(pUser->m_level >= type->m_level.m_playLevelVer[i].first
					&& pUser->m_level <= type->m_level.m_playLevelVer[i].second){
				levelID = type->m_level.m_levelIDVer[i];

			}
		}


		cmd.set_levelid(levelID);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)任务关卡levelID:%d",pUser->getID(),pUser->getName(), levelID);
#endif
		pUser->sendProto2Game(cmd);
	}
}

void CDiscoverMgr::giveUpTask(Player *pUser)
{
	if(!pUser){
		return ;
	}

	MiscProto::stGiveUpTaskResult retCmd;

	stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID());
	if(!pDiscoverInfo){
		retCmd.set_result(MiscProto::stGiveUpTaskResult::eInner_Error);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)放弃任务失败,找不到配置!",pUser->getID(), pUser->getName());
#endif
		return ;
	}

	stDiscoverDB dbData; 
	stDiscoverInfo &info = m_allDiscoverInfos[pUser->getID()];
	info.m_state = TASK_STATE_GIVEUP;

	info.fillDBData(dbData);
	updateRecord(pUser->getID(), (const char *)&dbData);

	retCmd.set_result(MiscProto::stGiveUpTaskResult::eSuccess);
	pUser->sendProtoToMe(retCmd);
}

/*                                       
 * @Brief calculateIdentify 计算领取后的标识
 *
 * @Param identify 是否领取过积分奖的励识
 * @Param id 领取的奖励id
 *
 * @Returns  领取后的标识
 */

UINT8 calculateIdentify(UINT8 identify, UINT8 id){
	if(id > 0 && id <= CDiscoverCfg::getSingleton().getBoundsBoxNum()){
		return	identify | (1 << ( id - 1 ));
	}
	return 0;
}

/*
 * @Brief  hasReceived 查询奖励是否已领取
 *
 * @Param identify 是否领取过积分奖的励识
 * @Param id  查询的奖励id
 *
 * @Returns   true-已领取  false-未领取
 */
bool hasReceived(UINT8 identify, UINT8 id){
	if(id > 0 && id <= CDiscoverCfg::getSingleton().getBoundsBoxNum()){
		return identify & (1 << ( id - 1 ));
	}
	return true;
}

void CDiscoverMgr::getIntegralReward(Player * pUser, UINT8 id)
{
	if(!pUser){
		return ;
	}

	MiscProto::stGetIntegralRewardResult retCmd;

	stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID());
	if(!pDiscoverInfo){
		retCmd.set_result(MiscProto::stGetIntegralRewardResult::eInner_Error);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取积分奖励失败，找不到配置!",pUser->getID(), pUser->getName());
#endif
		return ;                                                                                          
	}

	if(hasReceived(pDiscoverInfo->m_hasReceivedID, id)){
		retCmd.set_result(MiscProto::stGetIntegralRewardResult::eHas_Received);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG 
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取积分奖励失败，已经领取过了！",pUser->getID(),pUser->getName());
#endif
		return;
	}

	stDiscoverBounds * bounds = CDiscoverCfg::getSingleton().getBoundsBoxByid(id);
	if(!bounds){
		retCmd.set_result(MiscProto::stGetIntegralRewardResult::eNum_Useless_Over);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取积分奖励失败，积分不足!",pUser->getID(),pUser->getName());
#endif
		return ;
	}

	//通知game 发奖励
	Global::stGiveReward cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_boxid(bounds->m_boundsBox);
	cmd.set_rewardid(id);
	cmd.set_isintegralreward(true);
	pUser->sendProto2Game(cmd);
}

void CDiscoverMgr::getRewardCB(Global::stGiveRewardResult * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!pUser){
		return ;
	}

	//领取积分奖励
	if(recvCmd->isintegralreward()){
		MiscProto::stGetIntegralRewardResult retCmd; 
		stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID());
		if(!pDiscoverInfo){
			retCmd.set_result(MiscProto::stGetIntegralRewardResult::eInner_Error);
			pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取积分奖励失败，找不到配置!",pUser->getID(),pUser->getName());
#endif
			return ;
		}

		if(recvCmd->result() == Global::stGiveRewardResult::eBAG_SLOT_NOT_ENOUGH){
			retCmd.set_result(MiscProto::stGetIntegralRewardResult::eBAG_SLOT_NOT_ENOUGH);
			pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG 
			Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取积分奖励失败，背包满了!",pUser->getID(),pUser->getName());
#endif
			return;
		}

		UINT8 received = calculateIdentify(pDiscoverInfo->m_hasReceivedID, recvCmd->rewardid());

		stDiscoverDB dbData;
		stDiscoverInfo &info = m_allDiscoverInfos[pUser->getID()];
		info.m_hasReceivedID = received;

		info.fillDBData(dbData);
		updateRecord(pUser->getID(), (const char *)&dbData);

		retCmd.set_value(received);
		retCmd.set_result(MiscProto::stGetIntegralRewardResult::eSuccess);
		pUser->sendProtoToMe(retCmd);

	}else{//领取格子奖励
		MiscProto::stGiveRewardResult retCmd;

		stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID()); 
		if(!pDiscoverInfo){                                                                                    
			retCmd.set_result(MiscProto::stGiveRewardResult::eInner_Error);                             
			pUser->sendProtoToMe(retCmd);                                                                      
#ifdef _HDZ_DEBUG                                                                                              
			Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取格子奖励失败，找不到配置!",pUser->getID(),pUser->getName());
#endif
			return ;
		}

		if(recvCmd->result() == Global::stGiveRewardResult::eBAG_SLOT_NOT_ENOUGH){
			retCmd.set_result(MiscProto::stGiveRewardResult::eBAG_SLOT_NOT_ENOUGH);
			pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
			Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取格子奖励失败，背包满了!",pUser->getID(),pUser->getName());
#endif
			return;
		}

		stSquareType * squareInfo = CDiscoverCfg::getSingleton().getSquareInfo(pDiscoverInfo->m_mapID, pDiscoverInfo->m_place, pDiscoverInfo->m_taskID);                                                                         

		if(!squareInfo){                                                                                    
			return ;                                                                                        
		}                                                                                                   

		stDiscoverInfo &info = m_allDiscoverInfos[pUser->getID()]; 
		if(squareInfo->m_rewardBounds != 0){
			info.m_integral += squareInfo->m_rewardBounds;
			info.m_state =  TASK_STATE_GIVEUP;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)增加%d点积分，共%d点",pUser->getID(),pUser->getName(), squareInfo->m_rewardBounds, info.m_integral);
#endif
			if(info.m_battleNum >= squareInfo->m_battleNum){
				//	info.m_state = 3;                           
				info.m_battleNum = 0;                       
			}

			stDiscoverDB dbData;
			info.fillDBData(dbData);
			updateRecord(pUser->getID(), (const char *)&dbData);

			retCmd.set_integral(squareInfo->m_rewardBounds);
			retCmd.set_result(MiscProto::stGiveRewardResult::eSuccess);
			pUser->sendProtoToMe(retCmd);
		} else{
			info.m_state = TASK_STATE_GIVEUP;
			retCmd.set_result(MiscProto::stGiveRewardResult::eSuccess);
			pUser->sendProtoToMe(retCmd);

			stDiscoverDB dbData;                                 
			info.fillDBData(dbData);                             
			updateRecord(pUser->getID(), (const char *)&dbData);
		}

		std::pair<UINT8,UINT8> taskType = CDiscoverCfg::getSingleton().getSquareType(info.m_mapID, info.m_place);
		//到最后一格重置到第一格去                              
		if(taskType.first == eSQUARE_TYPE_END){
			info.m_mapID = CDiscoverCfg::getSingleton().randMapID();
			info.m_place = 0;
			info.m_state = TASK_STATE_GIVEUP;
			stDiscoverDB dbData;                                
			info.fillDBData(dbData);                            
			updateRecord(pUser->getID(), (const char *)&dbData);
			sendActivityInfo(pUser);                            
		}                                                       
	}
}

void CDiscoverMgr::giveReward(Player *pUser,MiscProto::stReqGiveReward * recvCmd)
{
	if(!pUser || !recvCmd){
		return ;
	}

	MiscProto::stGiveRewardResult retCmd;

	stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID());
	if(!pDiscoverInfo){
		retCmd.set_result(MiscProto::stGiveRewardResult::eInner_Error);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取奖励失败，找不到配置!",pUser->getID(),pUser->getName());
#endif
		return;
	}

	if(pDiscoverInfo->m_state == TASK_STATE_GIVEUP){
		retCmd.set_result(MiscProto::stGiveRewardResult::eHas_Received);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取奖励失败，已经领过了!",pUser->getID(), pUser->getName());
#endif
		return ;
	}

	std::pair<UINT8,UINT8> taskType = CDiscoverCfg::getSingleton().getSquareType(pDiscoverInfo->m_mapID, pDiscoverInfo->m_place);
	if(0 == taskType.first){
		return ;
	}

	//if(taskType.first == eSQUARE_TYPE_GOLD 
	//		|| taskType.first == eSQUARE_TYPE_DRAGON
	//		|| taskType.first == eSQUARE_TYPE_BOSS
	//		|| is5Or6Type(taskType.first)){
	if(pDiscoverInfo->m_state != TASK_STATE_FINISH){
		retCmd.set_result(MiscProto::stGiveRewardResult::eTask_Has_Receive);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->error("[秘境寻宝](charID:%d, name:%s)领取奖励失败，任务还未完成!",pUser->getID(),pUser->getName());
#endif
		return ;
	}
	//	}

	stSquareType * squareInfo = CDiscoverCfg::getSingleton().getSquareInfo(pDiscoverInfo->m_mapID, pDiscoverInfo->m_place,pDiscoverInfo->m_taskID);

	if(!squareInfo){
		return ;
	}

	stDiscoverInfo &info = m_allDiscoverInfos[pUser->getID()];  
	//如果有box奖励需要到game检查背包是否已满
	if(squareInfo->m_rewardBox != 0){
		//通知game 发奖励                  
		Global::stGiveReward cmd;          
		cmd.set_charid(pUser->getID());    
		cmd.set_boxid(squareInfo->m_rewardBox);
		cmd.set_rewardid(0); 
		cmd.set_isintegralreward(false);   
		pUser->sendProto2Game(cmd);
	}else{
		if(squareInfo->m_rewardBounds != 0){
			info.m_integral += squareInfo->m_rewardBounds;
			info.m_state = TASK_STATE_GIVEUP;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[秘境寻宝](charID:%d, name:%s)增加%d点积分，共%d点",pUser->getID(),pUser->getName(),squareInfo->m_rewardBounds, info.m_integral);
#endif
			info.m_battleNum = 0;                        

			stDiscoverDB dbData;
			info.fillDBData(dbData);
			updateRecord(pUser->getID(), (const char *)&dbData);

			retCmd.set_integral(squareInfo->m_rewardBounds);
			retCmd.set_result(MiscProto::stGiveRewardResult::eSuccess);
			pUser->sendProtoToMe(retCmd);
		}
		//到最后一格重置到第一格去
		if(taskType.first == eSQUARE_TYPE_END){
			info.m_mapID = CDiscoverCfg::getSingleton().randMapID(); 
			info.m_place = 0;
			info.m_state = TASK_STATE_GIVEUP;
			stDiscoverDB dbData;
			info.fillDBData(dbData);
			updateRecord(pUser->getID(), (const char *)&dbData); 
			sendActivityInfo(pUser);
		}

	}
}

bool CDiscoverMgr::is5Or6Type(UINT8 type)
{
	if(type == eSQUARE_TYPE_FIGHT_TASK || type == eSQUARE_TYPE_ORDINARY_TASK){
		return true;
	}
	return false ;
}


void CDiscoverMgr::checkTask(UINT32 charID, UINT32 levelID)
{
	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);	
	if(!pUser){
		return;
	}

	stDiscoverInfo * pDiscoverInfo = getUserInfo(charID);
	if(!pDiscoverInfo){
		return ;
	}

	stSquareType * squareInfo = CDiscoverCfg::getSingleton().getSquareInfo(pDiscoverInfo->m_mapID, pDiscoverInfo->m_place, pDiscoverInfo->m_taskID);
	if(!squareInfo){
		return ;
	}

	if(pDiscoverInfo->m_state != TASK_STATE_NOT_RECEIVE){
		return ;
	}

	stDiscoverInfo &info = m_allDiscoverInfos[charID]; 

	//if(levelID == squareInfo->m_level.m_levelID.m_level_id_1){
	//	info.m_state = TASK_STATE_FINISH;
	//}


	UINT32 level_ID = 0;
    if (squareInfo->m_level.m_levelIDVer.size()) {
        level_ID = squareInfo->m_level.m_levelIDVer[0];                       

        for(UINT16 i = 0; i < squareInfo->m_level.m_playLevelVer.size(); ++i){
            if(pUser->m_level >= squareInfo->m_level.m_playLevelVer[i].first
                    && pUser->m_level <= squareInfo->m_level.m_playLevelVer[i].second){
                level_ID = squareInfo->m_level.m_levelIDVer[i];
            }
        }

        if(levelID == level_ID){
            info.m_state = TASK_STATE_FINISH;
        }
    } else {
        info.m_state = TASK_STATE_FINISH;
    }

	stDiscoverDB dbData;                                  
	info.fillDBData(dbData);                              
	updateRecord(charID, (const char *)&dbData);  
}

void CDiscoverMgr::checkFightType(UINT32 charID,UINT8 squareType, UINT8 battleType)
{
	stDiscoverInfo * pDiscoverInfo = getUserInfo(charID);
	if(!pDiscoverInfo){
		return ;
	}

	if(pDiscoverInfo->m_state != TASK_STATE_UNFINISHED){
		return ;
	}

	std::pair<UINT8,UINT8> taskType = CDiscoverCfg::getSingleton().getSquareType(pDiscoverInfo->m_mapID, pDiscoverInfo->m_place);
	if(squareType != taskType.first){	
		return ;
	}

	stSquareType * squareInfo = CDiscoverCfg::getSingleton().getSquareInfo(pDiscoverInfo->m_mapID, pDiscoverInfo->m_place,pDiscoverInfo->m_taskID);
	if(!squareInfo){
		return ;
	}

	if(battleType != squareInfo->m_battleType){
		return ;
	}

	stDiscoverInfo &info = m_allDiscoverInfos[charID];
	info.m_battleNum += 1;

	if(info.m_battleNum >= squareInfo->m_battleNum){
		info.m_state = TASK_STATE_FINISH; 
	}

	stDiscoverDB dbData;                                
	info.fillDBData(dbData);                            
	updateRecord(charID, (const char *)&dbData);
}

void CDiscoverMgr::redPointRuls(Player * pUser)
{
	MiscProto::stRedPointRulsResult retCmd;

	stDiscoverInfo * pDiscoverInfo = getUserInfo(pUser->getID());  
	if(!pDiscoverInfo){
		retCmd.set_times(true); 
	}else{

		if((pDiscoverInfo->m_canUseNum - pDiscoverInfo->m_playDiceNum) > 0){
			retCmd.set_times(true);
		}

		if(pDiscoverInfo->m_state == TASK_STATE_FINISH){
			retCmd.set_cangetreward(true);
		}

		for(UINT8 i = 1; i < CDiscoverCfg::getSingleton().getBoundsBoxNum(); ++i){
			if(!hasReceived(pDiscoverInfo->m_hasReceivedID, i)){
				stDiscoverBounds* bounds =  CDiscoverCfg::getSingleton().getBoundsBoxByid(i);
				if(bounds){

					if(pDiscoverInfo->m_integral >= bounds->m_bounds){
						retCmd.set_canopenintegalbox(true); 
					}
				}
			}
		}
	}
	pUser->sendProtoToMe(retCmd);
}
