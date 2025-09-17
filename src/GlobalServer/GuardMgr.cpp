#include "GuardMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "Misc.pb.h"
#include "GuardCfg.h"
#include "Guild.h"
#include "RoleInfoManager.h"
#include "RankMgr.h"
#include "SysLanguages.h"

extern GlobalServer * g_globalServer;

CTblField g_guardTblDef[] = {
	{ "CHARID",   		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "QUALITY",  		CTblField::DB_TYPE_UINT8,  sizeof(UINT8)  	 },  
	{ "ENDTIME",      	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "HELPFRIEND",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "BEINTERCEPTNUM", CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "REFRESHNUM", 	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "DECRESNUM1",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "DECRESNUM2",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ NULL, 0, 0 },
};

CTblField g_guardUserInfoTblDef[] = {
	{ "CHARID",       		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "GUARDNUM",     		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },  
	{ "INTERCEPTNUM", 		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "HELPNUM",      		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },  
	{ "REFRESHFLAG",  		CTblField::DB_TYPE_UINT8,   sizeof(UINT8)   },
	{ "BUFF",    	  		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },  
	{ "RESETNUM",  	  		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },  
	{ "LASTINTERCEPTTIME",  CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ NULL, 0, 0 },
};


CTblField g_resetGuardUserInfoTblDef[] = {
	{ "GUARDNUM",     		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },  
	{ "INTERCEPTNUM", 		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "HELPNUM",      		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },  
	{ "RESETNUM",  	  		CTblField::DB_TYPE_UINT16,  sizeof(UINT16)  },  
	{ NULL, 0, 0 },
};

void stGuardInfo::initFromDB(const stGuardInfoDB & data)
{
	m_charID = data.m_charID;
	m_quality = data.m_quality;
	m_endTime = data.m_endTime;
	m_helpFriendID = data.m_helpFriendID;
	m_beInterceptNum = data.m_beInterceptNum;
	m_refreshNum  = data.m_refreshNum;
	m_decResNum1 = data.m_decResNum1;
	m_decResNum2 = data.m_decResNum2;
}

void stGuardInfo::fillDBData(stGuardInfoDB & data)
{
	data.m_charID = m_charID;
	data.m_quality = m_quality;
	data.m_endTime = m_endTime;
	data.m_helpFriendID = m_helpFriendID;
	data.m_beInterceptNum = m_beInterceptNum;
	data.m_refreshNum = m_refreshNum;
	data.m_decResNum1 = m_decResNum1;
	data.m_decResNum2 = m_decResNum2;
}

void stGuardUserInfo::initFromDB(const stGuardUserInfoDB & data)
{
	m_charID = data.m_charID;
	m_guardNum = data.m_guardNum;
	m_interceptNum = data.m_interceptNum;
	m_helpNum = data.m_helpNum;
	m_bHasRefresh = data.m_bHasRefresh;
	m_buff = data.m_buff;
	m_lastInterceptTime = data.m_lastInterceptTime;
	m_resetNum = data.m_resetNum;
}

void stGuardUserInfo::fillDBData(stGuardUserInfoDB & data)
{
	data.m_charID = m_charID;
	data.m_guardNum = m_guardNum;
	data.m_interceptNum = m_interceptNum;
	data.m_helpNum = m_helpNum;
	data.m_bHasRefresh = m_bHasRefresh;
	data.m_buff = m_buff;
	data.m_lastInterceptTime = m_lastInterceptTime;
	data.m_resetNum = m_resetNum;
}


CGuardMgr::~CGuardMgr()
{

}

bool CGuardMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_guardTblDef," GUARD_INFO ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stGuardInfoDB * pDBData = (stGuardInfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stGuardInfo & guardInfo = m_guardInfos[pDBData[i].m_charID];
			guardInfo.initFromDB(pDBData[i]);

			if (guardInfo.m_endTime > 0){
				m_guardings.insert(std::make_pair(guardInfo.m_charID,guardInfo.m_endTime));
			}
		}
	}
	SAFE_DELETE(result);

	result = g_databaseEngine.select(g_guardUserInfoTblDef," GUARD_USER_INFO ",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stGuardUserInfoDB * pDBData = (stGuardUserInfoDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stGuardUserInfo & guardUserInfo = m_guardUserInfos[pDBData[i].m_charID];
			guardUserInfo.initFromDB(pDBData[i]);
		}
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("服务器启动加载了运镖任务:%d",m_guardInfos.size());
	Athena::logger->trace("服务器启动加载了%d个玩家运镖信息",m_guardUserInfos.size());
#endif
	return true;
}
	
void CGuardMgr::addGuard(const stGuardInfoDB & data)
{
	Athena::CUDSqlCallBack<CGuardMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuardMgr>(this,&CGuardMgr::addGuardCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guardTblDef,(const char *)&data,"GUARD_INFO",autoID,pDelaySql);
}
	
void CGuardMgr::addGuardCB(bool opResult,UINT64 autoID)
{

}

void CGuardMgr::updateGuard(stGuardInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `CHARID`=%u", data.m_charID);

	Athena::CUDSqlCallBack<CGuardMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuardMgr>(this,&CGuardMgr::updateGuardCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_guardTblDef,(const char *)&data,"GUARD_INFO",where,pDelaySql);

}

void CGuardMgr::updateGuardCB(bool opResult,UINT64 autoID)
{

}

void CGuardMgr::removeGuard(UINT32 charID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID =%d ", charID);
	Athena::CUDSqlCallBack<CGuardMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuardMgr>(this,&CGuardMgr::removeGuardCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GUARD_INFO",where,pDelaySql);
}

void CGuardMgr::removeGuardCB(bool opResult,UINT64 autoID)
{

}

void CGuardMgr::addGuardUserInfo(const stGuardUserInfoDB & data)
{
	Athena::CUDSqlCallBack<CGuardMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuardMgr>(this,&CGuardMgr::addGuardUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guardUserInfoTblDef,(const char *)&data,"GUARD_USER_INFO",autoID,pDelaySql);
}

void CGuardMgr::addGuardUserInfoCB(bool opResult,UINT64 autoID)
{

}

void CGuardMgr::updateGuardUserInfo(stGuardUserInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `CHARID`=%u", data.m_charID);

	Athena::CUDSqlCallBack<CGuardMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuardMgr>(this,&CGuardMgr::updateGuardUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_guardUserInfoTblDef,(const char *)&data,"GUARD_USER_INFO",where,pDelaySql);

}

void CGuardMgr::updateGuardUserInfoCB(bool opResult,UINT64 autoID)
{

}

void CGuardMgr::removeGuardUserInfo(UINT32 charID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " CHARID =%d ", charID);
	Athena::CUDSqlCallBack<CGuardMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuardMgr>(this,&CGuardMgr::removeGuardUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("GUARD_USER_INFO",where,pDelaySql);
}

void CGuardMgr::removeGuardUserInfoCB(bool opResult,UINT64 autoID)
{

}
	
stGuardUserInfo * CGuardMgr::getGuardUserInfo(UINT32 charID)
{
	std::map<UINT32,stGuardUserInfo>::iterator it = m_guardUserInfos.find(charID);
	if (it != m_guardUserInfos.end()){
		return &it->second;
	}
	return NULL;
}

stGuardInfo * CGuardMgr::getGuardInfo(UINT32 charID)
{
	std::map<UINT32,stGuardInfo>::iterator it = m_guardInfos.find(charID);
	if (it != m_guardInfos.end()){
		return &it->second;
	}
	return NULL;
}


void CGuardMgr::openGuardWin(Player * pUser)
{
	if (!pUser){
		return ;
	}
	
	//checkExist(pUser->getID());

	MiscProto::stGuardWinInfo retCmd;
	
	stGuardUserInfo * pUserInfo = getGuardUserInfo(pUser->getID());
	if (pUserInfo){
		retCmd.set_guardnum(pUserInfo->m_guardNum);
		retCmd.set_helpnum(pUserInfo->m_helpNum);
		retCmd.set_interceptnum(pUserInfo->m_interceptNum);
		UINT32 remainTime = 0;
		if (pUserInfo->m_lastInterceptTime + GuardCfg::getSingleton().getInterceptCD() > g_globalServer->getSec()){
			remainTime = pUserInfo->m_lastInterceptTime + GuardCfg::getSingleton().getInterceptCD() - g_globalServer->getSec();
		}
		retCmd.set_interceptcd(remainTime);

		std::list<stGuardEnemy>::iterator it = pUserInfo->m_emenies.begin();
		UINT8 enemyNum = 0;
		for (;it != pUserInfo->m_emenies.end();++it){
			stGuardEnemy & enemy = *it;
			stGuardInfo *pEnemyGuard = getGuardInfo(enemy.m_charID);
			if (pEnemyGuard && pEnemyGuard->m_endTime) {
				MiscProto::stGuardBaseInfo * pSyn = retCmd.add_enemies();
				if (pSyn){
					pSyn->set_charid(enemy.m_charID);
					pSyn->set_quality(pEnemyGuard->m_quality);
					if (pEnemyGuard->m_endTime > g_globalServer->getSec()){
						pSyn->set_remaintime(pEnemyGuard->m_endTime - g_globalServer->getSec());
					}
				}
				++enemyNum;
			}

			if (enemyNum >= s_MAX_SEND_ENEMY){
				break;
			}
		}
	}
	else {
		retCmd.set_guardnum(0);
		retCmd.set_helpnum(0);
		retCmd.set_interceptnum(0);
		retCmd.set_interceptcd(0);
	}
	
	stGuardInfo * pGuardInfo = getGuardInfo(pUser->getID());
	if (pGuardInfo){
		GuardCfg::stGuardInfo * pGuardInfoCfg = GuardCfg::getSingleton().getGuardInfo(pGuardInfo->m_quality);		
		
		UINT32 remainTime = 0;
		if (pGuardInfoCfg){
			if (pGuardInfo->m_endTime > g_globalServer->getSec()){
				remainTime = pGuardInfo->m_endTime  - g_globalServer->getSec();
			}	
		}
		retCmd.set_guardremaintime(remainTime);
	}
	else {
		retCmd.set_guardremaintime(0);
	}
	
	UINT16 guardNum = 0;
	std::map<UINT32,stGuardInfo>::iterator it = m_guardInfos.begin();
	for (;it != m_guardInfos.end();++it){
		stGuardInfo & guardInfo = it->second;
		if (guardInfo.m_endTime <= 0){//还没有开始
			continue;
		}

		MiscProto::stGuardBaseInfo * pSyn = retCmd.add_guardinfos();
		if (pSyn){
			pSyn->set_charid(guardInfo.m_charID);
			pSyn->set_quality(guardInfo.m_quality);

			if (guardInfo.m_endTime > g_globalServer->getSec()){
				pSyn->set_remaintime(guardInfo.m_endTime - g_globalServer->getSec());
			}
			else {
				pSyn->set_remaintime(0);
			}
		}
		++guardNum;
		if (guardNum >= s_MAX_SEND_GUARDINFO){
			break;
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CGuardMgr::sendGuardDetailInfo(Player * pUser,UINT32 charID)
{
	if (!pUser){
		return ;
	}

	MiscProto::stRetGuardDetailInfo retCmd;
	std::map<UINT32,stGuardInfo>::iterator it = m_guardInfos.find(charID);

	if (it != m_guardInfos.end()){
		stGuardInfo & guardInfo = it->second;
		retCmd.mutable_detailinfo()->set_charid(guardInfo.m_charID);
		retCmd.mutable_detailinfo()->set_beinterceptnum(guardInfo.m_beInterceptNum);
		
		stRoleBaseInfo *pRoleBaseInfo = RoleInfoManager::getSingleton().getRoleBaseInfo(guardInfo.m_charID);
		if (pRoleBaseInfo){
			retCmd.mutable_detailinfo()->set_name(pRoleBaseInfo->m_name);
		}	

		pRoleBaseInfo = RoleInfoManager::getSingleton().getRoleBaseInfo(guardInfo.m_helpFriendID);
		if (pRoleBaseInfo){
			retCmd.mutable_detailinfo()->set_friendname(pRoleBaseInfo->m_name);
		}

		retCmd.mutable_detailinfo()->set_quality(guardInfo.m_quality);

		stGuardUserInfo *pGuardUserInfo = getGuardUserInfo(guardInfo.m_charID);
		if (pGuardUserInfo){
			retCmd.mutable_detailinfo()->set_inspirednum(pGuardUserInfo->m_buff);
		}

		CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(guardInfo.m_charID);

		if (pGuild){
			retCmd.mutable_detailinfo()->set_guildname(pGuild->getName());
		}
		
		if ((guardInfo.m_endTime > 0) && (guardInfo.m_endTime > g_globalServer->getSec())){
			retCmd.mutable_detailinfo()->set_remaintime(guardInfo.m_endTime  - g_globalServer->getSec());
		}
		retCmd.mutable_detailinfo()->set_level(CRankMgr::getSingleton().getLevelByCharID(guardInfo.m_charID));
	}
	pUser->sendProtoToMe(retCmd);
}

void CGuardMgr::finishGuard(UINT32 charID)
{
	stGuardInfo *pGuardInfo = getGuardInfo(charID);
	if (pGuardInfo){
		GuardCfg::stGuardInfo *pGuardCfgInfo = GuardCfg::getSingleton().getGuardInfo(pGuardInfo->m_quality);
		if (pGuardCfgInfo){
			UINT32 resNum1 = pGuardCfgInfo->m_resNum1 > pGuardInfo->m_decResNum1 ? pGuardCfgInfo->m_resNum1 - pGuardInfo->m_decResNum1 : 0;
			UINT32 resNum2 = pGuardCfgInfo->m_resNum2 > pGuardInfo->m_decResNum2 ? pGuardCfgInfo->m_resNum2 - pGuardInfo->m_decResNum2 : 0;

			std::vector<std::pair<UINT32,UINT32> > selfAttaches;
			std::vector<std::pair<UINT32,UINT32> > friendAttaches;
			if (resNum1 > 0){
				selfAttaches.push_back(std::make_pair(pGuardCfgInfo->m_resID1,resNum1));
				friendAttaches.push_back(std::make_pair(pGuardCfgInfo->m_resID1,resNum1));
			}

			if (resNum2 > 0){
				selfAttaches.push_back(std::make_pair(pGuardCfgInfo->m_resID2,resNum2));
				friendAttaches.push_back(std::make_pair(pGuardCfgInfo->m_resID2,resNum2));
			}

			selfAttaches.push_back(std::make_pair(pGuardCfgInfo->m_resID3,pGuardCfgInfo->m_resNum3));
			MailManager::getSingleton().giveItemByMail(CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_ESCORT_REWARD),CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_ESCORT_OBTAIN_REWARD),selfAttaches,pGuardInfo->m_charID);

			MailManager::getSingleton().giveItemByMail(CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_ESCORT_REWARD),CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_ESCORT_OBTAIN_REWARD),friendAttaches,pGuardInfo->m_helpFriendID);

			Player * pUser = PlayerManager::getSingleton().getUserByID(pGuardInfo->m_charID);
			if (pUser){
				MiscProto::stNotifyGuardFinish retCmd;
				retCmd.set_quality(pGuardInfo->m_quality);
				retCmd.set_beinterceptnum(pGuardInfo->m_beInterceptNum);
				retCmd.set_resid1(pGuardCfgInfo->m_resID1);
				retCmd.set_resnum1(resNum1);
				retCmd.set_resid2(pGuardCfgInfo->m_resID2);
				retCmd.set_resnum2(resNum2);
				retCmd.set_resid3(pGuardCfgInfo->m_resID3);
				retCmd.set_resnum3(pGuardCfgInfo->m_resNum3);
				retCmd.set_guardid(pGuardInfo->m_charID);

				stRoleBaseInfo *pRoleBaseInfo = RoleInfoManager::getSingleton().getRoleBaseInfo(pGuardInfo->m_helpFriendID);
				if (pRoleBaseInfo){
					retCmd.set_friendname(pRoleBaseInfo->m_name);
				}	
				pUser->sendProtoToMe(retCmd);
			}

			Player * pOther = PlayerManager::getSingleton().getUserByID(pGuardInfo->m_helpFriendID);
			if (pOther){
				MiscProto::stNotifyFriendGuardFinish retCmd;	
				retCmd.set_quality(pGuardInfo->m_quality);
				retCmd.set_beinterceptnum(pGuardInfo->m_beInterceptNum);
				retCmd.set_resid1(pGuardCfgInfo->m_resID1);
				retCmd.set_resnum1(resNum1);
				retCmd.set_resid2(pGuardCfgInfo->m_resID2);
				retCmd.set_resnum2(resNum2);
				retCmd.set_guardid(pGuardInfo->m_charID);
				stRoleBaseInfo *pRoleBaseInfo = RoleInfoManager::getSingleton().getRoleBaseInfo(pGuardInfo->m_charID);
				if (pRoleBaseInfo){
					retCmd.set_friendname(pRoleBaseInfo->m_name);
				}	
				pOther->sendProtoToMe(retCmd);
			}
		}
		
		stGuardUserInfo * pGuardUserInfo = getGuardUserInfo(pGuardInfo->m_helpFriendID);
		if (pGuardUserInfo){
			pGuardUserInfo->m_bHelpGuarding = false;
			//pGuardUserInfo->m_helpNum++;
			//stGuardUserInfoDB dbData1;
			//pGuardUserInfo->fillDBData(dbData1);
			//updateGuardUserInfo(dbData1);
		}
		
		pGuardUserInfo = getGuardUserInfo(pGuardInfo->m_charID);
		if (pGuardUserInfo){
			pGuardUserInfo->m_bHasRefresh = 0;
			pGuardUserInfo->m_buff = 0;
			stGuardUserInfoDB dbData1;
			pGuardUserInfo->fillDBData(dbData1);
			updateGuardUserInfo(dbData1);
		}

		pGuardInfo->m_beInterceptNum = 0;
		pGuardInfo->m_endTime = 0;
		pGuardInfo->m_helpFriendID = 0;
		pGuardInfo->m_quality = eGuard_Qaulity_1;
		pGuardInfo->m_refreshNum = 0;
		pGuardInfo->m_decResNum1 = 0;
		pGuardInfo->m_decResNum2 = 0;
		pGuardInfo->m_beIntercepting = false;
	

		stGuardInfoDB dbData;
		pGuardInfo->fillDBData(dbData);
		updateGuard(dbData);
	}

}

void CGuardMgr::timer(UINT32 cur)
{
	std::map<UINT32,UINT32>::iterator it =  m_guardings.begin();
	std::vector<UINT32> finishVec;

	for (;it != m_guardings.end();++it){
		if (it->second < g_globalServer->getSec()){
			finishVec.push_back(it->first);
		}	

		if (finishVec.size() >= 8){//每次只处理8条记录
			break;
		}
	}

	for (UINT16 i = 0;i < finishVec.size(); ++i){
		m_guardings.erase(finishVec[i]);
		finishGuard(finishVec[i]);
	}

}
	
void CGuardMgr::clearInterceptCD(Player * pUser)
{
	if (!pUser){
		return ;
	}

	stGuardUserInfo *pUserInfo = getGuardUserInfo(pUser->getID());
	if (!pUserInfo){
		return;
	}

	pUserInfo->m_lastInterceptTime = 0;
	stGuardUserInfoDB dbData;
	pUserInfo->fillDBData(dbData);
	updateGuardUserInfo(dbData);
}

void CGuardMgr::resetInterceptNum(Player * pUser)
{
	if (!pUser){
		return ;
	}	

	stGuardUserInfo *pUserInfo = getGuardUserInfo(pUser->getID());
	if (!pUserInfo){
		return ;
	}
	
	pUserInfo->m_resetNum += 1;
	pUserInfo->m_interceptNum = 0;
	stGuardUserInfoDB dbData;
	pUserInfo->fillDBData(dbData);
	updateGuardUserInfo(dbData);
}

void CGuardMgr::sendGuardWin(Player * pUser)
{
	    	Athena::logger->trace("reqIntoGuardWin 2");

	MiscProto::stRetGuardWin retCmd;
	stGuardInfo *pGuardInfo = getGuardInfo(pUser->getID());
	stGuardUserInfo *pGuardUserInfo =  getGuardUserInfo(pUser->getID());
	
	// if (!pGuardInfo || !pGuardUserInfo){
	// 		    	Athena::logger->trace("reqIntoGuardWin 3");

	// 	return ;
	// }
	    if (!pGuardInfo ){
                    Athena::logger->trace("pGuardInfo 为空 ");

        return ;
    }
        if (!pGuardUserInfo){
                    Athena::logger->trace("pGuardUserInfo 为空 ");

        return ;
    }
	retCmd.set_buff(pGuardUserInfo->m_buff);
	retCmd.set_canfreerefresh(pGuardUserInfo->m_bHasRefresh ? true : false);
	retCmd.set_quality(pGuardInfo->m_quality);
	retCmd.set_refreshnum(pGuardInfo->m_refreshNum);
				    	Athena::logger->trace("reqIntoGuardWin 4");

	CFriendInfo * pFriendInfo = pUser->m_friendMgr.getFriend(pGuardInfo->m_helpFriendID);
	if (pFriendInfo){
					    	Athena::logger->trace("reqIntoGuardWin 5");

		retCmd.set_friendname(pFriendInfo->m_name);
	}
						    	Athena::logger->trace("reqIntoGuardWin 6");

	pUser->sendProtoToMe(retCmd);
}

void CGuardMgr::checkExist(UINT32 charID)
{
	stGuardUserInfo *pUserInfo = getGuardUserInfo(charID);
	if (!pUserInfo){
		stGuardUserInfo userInfo;
		userInfo.m_charID = charID;
		
		stGuardUserInfoDB dbData;
		userInfo.fillDBData(dbData);
		addGuardUserInfo(dbData);

		m_guardUserInfos[charID] = userInfo;
	}
	
	stGuardInfo *pGuardInfo = getGuardInfo(charID);
	if (!pGuardInfo){
		stGuardInfo guardInfo;
		guardInfo.m_charID = charID;
		guardInfo.m_quality = eGuard_Qaulity_1;
		
		stGuardInfoDB dbData;
		guardInfo.fillDBData(dbData);
		addGuard(dbData);

		m_guardInfos[charID] = guardInfo;
	}
}

void CGuardMgr::summonHighQualityGuard(Player * pUser,UINT8 quality)
{
	stGuardInfo *pGuardInfo = getGuardInfo(pUser->getID());
	if (!pGuardInfo){
		return ;
	}

	pGuardInfo->m_quality = quality;
	stGuardInfoDB dbData;
	pGuardInfo->fillDBData(dbData);
	updateGuard(dbData);
}

void CGuardMgr::inspired(Player *pUser)
{
	if (!pUser){
		return ;
	}

	stGuardUserInfo *pUserInfo = getGuardUserInfo(pUser->getID());
	if (!pUserInfo){
		return ;
	}
	
	if (pUserInfo->m_buff >= s_MAX_INSPIRED_NUM){
		return ;
	}
	
	pUserInfo->m_buff += 1;
	stGuardUserInfoDB dbData;
	pUserInfo->fillDBData(dbData);
	updateGuardUserInfo(dbData);

	MiscProto::stGuardInspiredResult retCmd;
	retCmd.set_result(MiscProto::stGuardInspiredResult::eSuccess);
	retCmd.set_num(pUserInfo->m_buff);
	pUser->sendProtoToMe(retCmd);
}

void CGuardMgr::beginGuard(Player * pUser)
{
	if (!pUser){
		    	Athena::logger->trace("pUser error");

		return ;
	}


	Athena::logger->trace("beginGuard 1");

	stGuardInfo *pGuardInfo = getGuardInfo(pUser->getID());
	stGuardUserInfo *pGuardUserInfo =  getGuardUserInfo(pUser->getID());
	
	if (!pGuardInfo || !pGuardUserInfo){
		Athena::logger->error("开始护送失败,找不到相关信息");
		return ;
	}
	
	MiscProto::stBeginGuardResult retCmd;
	if (pGuardInfo->m_endTime > 0){
					    	Athena::logger->trace("m_endTime error%d", pGuardInfo->m_endTime);

		retCmd.set_result(MiscProto::stBeginGuardResult::eHas_Begin);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
Athena::logger->trace("beginGuard 2");
	if (pGuardUserInfo->m_guardNum >= GuardCfg::getSingleton().getGuardNum()){
							    	Athena::logger->trace("m_guardNum error%d",pGuardUserInfo->m_guardNum);
		retCmd.set_result(MiscProto::stBeginGuardResult::eUse_Out_Num);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	
	GuardCfg::stGuardInfo *pGuardCfgInfo = GuardCfg::getSingleton().getGuardInfo(pGuardInfo->m_quality);
	if (!pGuardCfgInfo){
									    	Athena::logger->trace("pGuardCfgInfo error");

		return ;
	}
Athena::logger->trace("beginGuard 3");
	pGuardUserInfo->m_guardNum += 1;
	pGuardInfo->m_beInterceptNum = 0;
	pGuardInfo->m_endTime = g_globalServer->getSec() + pGuardCfgInfo->m_guardTime;
	m_guardings.insert(std::make_pair(pUser->getID(),pGuardInfo->m_endTime));

	stGuardInfoDB dbData;
	pGuardInfo->fillDBData(dbData);
	updateGuard(dbData);

	stGuardUserInfoDB userDBData;
	pGuardUserInfo->fillDBData(userDBData);
	updateGuardUserInfo(userDBData);
	Athena::logger->trace("beginGuard 4");
	retCmd.set_result(MiscProto::stBeginGuardResult::eSuccess);
	pUser->sendProtoToMe(retCmd);

	Global::stNotifyUserBeginGuard cmd;
	cmd.set_charid(pUser->getID());
	pUser->sendProto2Game(cmd);
	Athena::logger->trace("beginGuard 5");
}

void CGuardMgr::sendInterceptInfos(Player * pUser)
{
	if (!pUser){
		return ;
	}	
	stGuardUserInfo *pGuardUserInfo =  getGuardUserInfo(pUser->getID());
	
	if (!pGuardUserInfo){
		Athena::logger->error("请求战报,找不到相关信息");
		return ;
	}

	MiscProto::stRetInterceptInfo retCmd;
	
	std::map<UINT32,std::string>::iterator it = pGuardUserInfo->m_interceptInfos.begin();

	for (;it != pGuardUserInfo->m_interceptInfos.end();++it){
		MiscProto::stInterceptInfo * pSyn = retCmd.add_infos();

		if (pSyn){
			pSyn->set_timestamp(it->first);
			pSyn->set_content(it->second);
		}
	}
	pGuardUserInfo->m_interceptInfos.clear();
	pUser->sendProtoToMe(retCmd);
}

void CGuardMgr::interceptGuard(Player * pUser,UINT32 charID)
{
	if (!pUser){
		return;
	}
	
	stGuardInfo *pGuardInfo = getGuardInfo(charID);
	stGuardUserInfo *pMyUserInfo =  getGuardUserInfo(pUser->getID());
	if (!pGuardInfo || !pMyUserInfo){
		Athena::logger->error("玩家(%s,%d)拦截 charID:%d 时候找不到对方记录",pUser->getName(),pUser->getID(),charID);
		return ;
	}

	MiscProto::stIntercerptResult retCmd;
	
	if (pGuardInfo->m_endTime <= 0){
		retCmd.set_result(MiscProto::stIntercerptResult::eNot_Begin_Guard);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	
	if (pUser->getID() == pGuardInfo->m_helpFriendID){
		retCmd.set_result(MiscProto::stIntercerptResult::eHelp_This_Guard);
		pUser->sendProtoToMe(retCmd);
		return;
	}

	if (pGuardInfo->m_beInterceptNum >= GuardCfg::getSingleton().getBeInterceptNum()){
		retCmd.set_result(MiscProto::stIntercerptResult::eBeIntercept_Num_Use_Out);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (pMyUserInfo->m_interceptNum >= GuardCfg::getSingleton().getMaxInterceptNum()){
		retCmd.set_result(MiscProto::stIntercerptResult::eIntercept_Num_Use_Out);
		pUser->sendProtoToMe(retCmd);
		return ;
	}
	if (pGuardInfo->m_beIntercepting){
		retCmd.set_result(MiscProto::stIntercerptResult::eBeing_Intercepted);
		pUser->sendProtoToMe(retCmd);
		return;
	}

	if (g_globalServer->getSec() + 90 > pGuardInfo->m_endTime){//小于90分钟了不能被劫持了
		retCmd.set_result(MiscProto::stIntercerptResult::eSafe_State);
		pUser->sendProtoToMe(retCmd);
		return;
	}

	pGuardInfo->m_beIntercepting = true;
	pUser->m_interceptID = pGuardInfo->m_charID;
#ifdef _HDZ_DEBUG
	if (0 == pUser->m_interceptID){
		Athena::logger->trace("设置来接玩家id:%d为0",pUser->getID());
	}
#endif
	UINT32 challengeCharID = pGuardInfo->m_helpFriendID ? pGuardInfo->m_helpFriendID : pGuardInfo->m_charID;

	stGuardUserInfo *pUserGuardInfo = getGuardUserInfo(pGuardInfo->m_charID);
	UINT32 bufCnt = 0;
	if (pUserGuardInfo)
	{
		bufCnt = pUserGuardInfo->m_buff;
	}

	Player * pOther = PlayerManager::getSingleton().getUserByID(challengeCharID);
	if (pOther){
		Global::stBeginPKWithOther cmd;
		cmd.set_othercharid(challengeCharID);
		cmd.set_mycharid(pUser->getID());
		cmd.set_datasize(0);
		cmd.set_type(Global::stBeginPKWithOther::eIntercept_PK);
		cmd.set_challengerank(bufCnt);
		pUser->sendProto2Game(cmd);
	}   
	else {
		RoleInfoManager::getSingleton().loadOfflineUserChallege(pUser->getID(),challengeCharID,Global::stBeginPKWithOther::eIntercept_PK,bufCnt);
	}

	if (pUserGuardInfo){
		bool bExist = false;
		std::list<stGuardEnemy>::iterator it = pUserGuardInfo->m_emenies.begin();
		for (;it != pUserGuardInfo->m_emenies.end();++it){
			if (it->m_charID == pUser->getID()){
				bExist = true;
			}	
		}

		if (!bExist){
			stGuardEnemy enemy;
			enemy.m_charID = pUser->getID();
			strncpy(enemy.m_name , pUser->getName(),sizeof(enemy.m_name));
			enemy.m_job = pUser->m_job;
			enemy.m_sex = pUser->m_sex;
			pUserGuardInfo->m_emenies.push_back(enemy);

			if (pUserGuardInfo->m_emenies.size() >= s_MAX_SAVE_ENEMY){
				std::list<stGuardEnemy>::iterator it = pUserGuardInfo->m_emenies.begin();
				if (it != pUserGuardInfo->m_emenies.end()){
					pUserGuardInfo->m_emenies.erase(it);
				}
			}
		}
	}
}

void CGuardMgr::interceptResult(Player * pUser,bool win)
{
	if (!pUser){
		return;
	}

	if (!pUser->m_interceptID){//并没有拦截别人
		Athena::logger->error("玩家(name:%s,id:%d)上报拦截结果不正确,该玩家没有拦截别人的记录",pUser->getName(),pUser->getID());
		return ;
	}
	
	Global::stNotifyUserInterceptResult  notifyCmd;
	notifyCmd.set_charid(pUser->getID());
	pUser->sendProto2Game(notifyCmd);

	stGuardInfo *pGuardInfo = getGuardInfo(pUser->m_interceptID);
	stGuardUserInfo *pGuardUserInfo = getGuardUserInfo(pUser->m_interceptID);
	if (!pGuardInfo || !pGuardUserInfo){
		Athena::logger->error("玩家(%s,%d)上报拦截结果时候找不到对方记录",pUser->getName(),pUser->getID());
		return ;
	}

	pUser->m_interceptID = 0;
	pGuardInfo->m_beIntercepting = false;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了:%d,设置拦截状态为false",pGuardInfo->m_charID);
#endif
#ifdef _THAILAND_VER
	char szInterceptInfo[250] = { 0 };
#else
	char szInterceptInfo[75] = { 0 };
#endif
	if (!win){//失败不需要往下处理了
		if (!pGuardInfo->m_helpFriendID){
			snprintf(szInterceptInfo,sizeof(szInterceptInfo),"escort_Report_001;%s;%d",pUser->getName(),pGuardInfo->m_quality);	
		}
		else {
			stRoleBaseInfo *pRoleBase = RoleInfoManager::getSingleton().getRoleBaseInfo(pGuardInfo->m_helpFriendID);
			if (pRoleBase){
				snprintf(szInterceptInfo,sizeof(szInterceptInfo),"escort_Report_003;%s;%d;%s",pUser->getName(),pGuardInfo->m_quality,pRoleBase->m_name.c_str());	
			}
		}
		pGuardUserInfo->m_interceptInfos.insert(std::make_pair(g_globalServer->getSec(),szInterceptInfo));	
		MiscProto::stBeInterceptTip notifyCmd;
		notifyCmd.set_win(false);
		notifyCmd.set_quality(pGuardInfo->m_quality);
		notifyCmd.set_name(pUser->getName());
		Player * pOther = PlayerManager::getSingleton().getUserByID(pGuardInfo->m_charID);
		if (pOther) {
			pOther->sendProtoToMe(notifyCmd);	
		}
		pGuardInfo->m_beIntercepting = false;
		pGuardUserInfo = getGuardUserInfo(pUser->getID());
		if (pGuardUserInfo){
			pGuardUserInfo->m_interceptNum++;
			stGuardUserInfoDB dbData;
			pGuardUserInfo->fillDBData(dbData);
			updateGuardUserInfo(dbData);
			bzero(szInterceptInfo,sizeof(szInterceptInfo));
			stRoleBaseInfo *pRoleBase = RoleInfoManager::getSingleton().getRoleBaseInfo(pGuardInfo->m_charID);
			if (pRoleBase){
				snprintf(szInterceptInfo,sizeof(szInterceptInfo),"escort_Report_008;%s;%d",pRoleBase->m_name.c_str(),pGuardInfo->m_quality);	
			}
			pGuardUserInfo->m_interceptInfos.insert(std::make_pair(g_globalServer->getSec(),szInterceptInfo));
		}
		return;
	}

	UINT16 lvDiff = 0;
	UINT16 lv = CRankMgr::getSingleton().getLevelByCharID(pGuardInfo->m_charID);
	lvDiff = pUser->m_level > lv ? pUser->m_level - lv : 0;

	GuardCfg::stInterceptReward * pReward = GuardCfg::getSingleton().getInterceptReward(lvDiff);
	GuardCfg::stGuardInfo *pGuardCfgInfo = GuardCfg::getSingleton().getGuardInfo(pGuardInfo->m_quality);

	if (!pReward || !pGuardCfgInfo){
		Athena::logger->error("玩家(name:%s,id:%d)上报拦截结果时候找不到等级差的配置",pUser->getName(),pUser->getID());
		return ;
	}
	
	UINT32 resNum1 = (UINT32)(pGuardCfgInfo->m_resNum1 * (pReward->m_rewardPer1 / 100.0f));
	UINT32 resNum2 = (UINT32)(pGuardCfgInfo->m_resNum2 * (pReward->m_rewardPer2 / 100.0f)); 
	MiscProto::stReplyNotifyIntercept retCmd;
	retCmd.set_resid1(pGuardCfgInfo->m_resID1);
	retCmd.set_resnum1(resNum1);
	retCmd.set_resid2(pGuardCfgInfo->m_resID2);
	retCmd.set_resnum2(resNum2);
	pUser->sendProtoToMe(retCmd);

	pGuardInfo->m_decResNum1 += resNum1;
	pGuardInfo->m_decResNum2 += resNum2;
	pGuardInfo->m_beInterceptNum += 1;

	stGuardInfoDB dbData;
	pGuardInfo->fillDBData(dbData);
	updateGuard(dbData);
	
	
	Global::stInterceptReward cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_resid1(pGuardCfgInfo->m_resID1);
	cmd.set_resnum1(resNum1);
	cmd.set_resid2(pGuardCfgInfo->m_resID2);
	cmd.set_resnum2(resNum2);
	pUser->sendProto2Game(cmd);
	
	if (!pGuardInfo->m_helpFriendID){
		snprintf(szInterceptInfo,sizeof(szInterceptInfo),"escort_Report_002;%s;%d;%d;%d",pUser->getName(),pGuardInfo->m_quality,resNum1,resNum2);	
	}
	else {
		stRoleBaseInfo *pRoleBase = RoleInfoManager::getSingleton().getRoleBaseInfo(pGuardInfo->m_helpFriendID);
		if (pRoleBase){
			snprintf(szInterceptInfo,sizeof(szInterceptInfo),"escort_Report_004;%s;%d;%s;%d;%d",pUser->getName(),pGuardInfo->m_quality,pRoleBase->m_name.c_str(),resNum1,resNum2);	
		}
	}
	pGuardUserInfo->m_interceptInfos.insert(std::make_pair(g_globalServer->getSec(),szInterceptInfo));	
	
	Player * pOther = PlayerManager::getSingleton().getUserByID(pGuardInfo->m_charID);
	if (pOther){
		MiscProto::stBeInterceptTip notifyCmd;
		notifyCmd.set_win(true);
		notifyCmd.set_quality(pGuardInfo->m_quality);
		notifyCmd.set_name(pUser->getName());
		pOther->sendProtoToMe(notifyCmd);	
	}

	pGuardUserInfo = getGuardUserInfo(pUser->getID());
	if (pGuardUserInfo){
		pGuardUserInfo->m_interceptNum++;
		stGuardUserInfoDB dbData;
		pGuardUserInfo->fillDBData(dbData);
		updateGuardUserInfo(dbData);

		bzero(szInterceptInfo,sizeof(szInterceptInfo));
		stRoleBaseInfo *pRoleBase = RoleInfoManager::getSingleton().getRoleBaseInfo(pGuardInfo->m_charID);
		if (pRoleBase){
			snprintf(szInterceptInfo,sizeof(szInterceptInfo),"escort_Report_007;%s;%d;%d;%d",pRoleBase->m_name.c_str(),pGuardInfo->m_quality,resNum1,resNum2);	
		}
		pGuardUserInfo->m_interceptInfos.insert(std::make_pair(g_globalServer->getSec(),szInterceptInfo));
	}
}

void CGuardMgr::quickFinish(Player * pUser)
{
	if (!pUser){
		return;
	}
	
	stGuardInfo *pGuardInfo = getGuardInfo(pUser->getID());
	if (!pGuardInfo){
		Athena::logger->error("玩家(%s,%d)请求快速完成时候找不到对方记录",pUser->getName(),pUser->getID());
		return ;
	}
	
	MiscProto::stQuickFinishGuardResult retCmd;
	if (pGuardInfo->m_endTime <= 0){
		retCmd.set_result(MiscProto::stQuickFinishGuardResult::eHas_Not_Begin);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (pGuardInfo->m_beIntercepting){
		retCmd.set_result(MiscProto::stQuickFinishGuardResult::eBeing_Intercepted);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	Global::stQuickFinishDecDiamond cmd;
	cmd.set_charid(pUser->getID());
	pUser->sendProto2Game(cmd);
}

void CGuardMgr::userOffline(Player * pUser)
{
	if (!pUser){
		return ;
	}

	stGuardInfo *pGuardInfo = getGuardInfo(pUser->m_interceptID);	

	if (!pGuardInfo){
		return ;
	}

	pGuardInfo->m_beIntercepting = false;
}

void CGuardMgr::userOnline(Player * pUser)
{
	if (!pUser){
		return;
	}
	
	std::map<UINT32,stGuardUserInfo>::iterator it = m_guardUserInfos.find(pUser->getID());
	if (it == m_guardUserInfos.end()){
		return ;
	}
	
	stGuardUserInfo & guardUserInfo = it->second;
	/*if (guardUserInfo.m_interceptInfos.size() <= 0){
		return ;
	}*/

	MiscProto::stOnlineNotifyNewInterceptInfo retCmd;
	retCmd.set_num(guardUserInfo.m_interceptInfos.size());
	retCmd.set_guardnum(guardUserInfo.m_guardNum);
	retCmd.set_intercertnum(guardUserInfo.m_interceptNum);
	pUser->sendProtoToMe(retCmd);
}

void CGuardMgr::resetGuardUserInfo()
{
	std::map<UINT32,stGuardUserInfo>::iterator it = m_guardUserInfos.begin();	
	for (;it != m_guardUserInfos.end();++it){
		stGuardUserInfo & userInfo = it->second;
		userInfo.m_guardNum = 0;
		userInfo.m_helpNum = 0;
		userInfo.m_interceptNum = 0;
		userInfo.m_resetNum = 0;
	}
	
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " `CHARID` > 0 ");
	
	stResetGuardUserInfoDB dbData;
	Athena::CUDSqlCallBack<CGuardMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuardMgr>(this,&CGuardMgr::updateGuardUserInfoCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_resetGuardUserInfoTblDef,(const char *)&dbData,"GUARD_USER_INFO",where,pDelaySql);
}

void CGuardMgr::resetGuardUserInfoCB(bool opResult,UINT64 autoID)
{
	
}
