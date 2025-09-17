#include "FriendMgr.h"
#include "Player.h"
#include "MysqlDatabase.h"
#include "Log4cxxHelp.h"
#include "GlobalServer.h"
#include "PlayerManager.h"
#include "KVPair.h"
#include "RankMgr.h"
#include "GuardMgr.h"
#include "Misc.pb.h"
#include "GuardCfg.h"

extern GlobalServer * g_globalServer;

CTblField  g_friendTblDef[] = {
	{ "CHARID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "OTHERID",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "OTHERNAME",   CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "JOB",         CTblField::DB_TYPE_UINT8,   sizeof(UINT8)	},
	{ "SEX",   		 CTblField::DB_TYPE_UINT8,   sizeof(UINT8)  },
	{ "USERPHOTOID", CTblField::DB_TYPE_UINT16,  sizeof(UINT16) },
	{ NULL, 0, 0 },
};

void CFriendInfo::fillDBData(stFriendDBInfo & data)
{
	data.m_sex = m_sex;
	data.m_job   = m_job;
	data.m_otherID = m_charID;
	data.m_charID = m_ownerID;
	data.m_photoID = m_photoID;
	strncpy(data.m_otherName,m_name,sizeof(data.m_otherName));
}

void CFriendInfo::initFromDBData(const stFriendDBInfo & data)
{
	m_ownerID = data.m_charID;
	m_sex = data.m_sex;
	m_job  = data.m_job;
	m_charID = data.m_otherID;
	strncpy(m_name,data.m_otherName,sizeof(m_name));
	setName(data.m_otherName);
	setID(data.m_otherID);
	m_photoID = data.m_photoID;
}

void CFriendInfo::fillSynData(FriendProto::stSynFriendInfo & data)
{
	data.set_charid(m_charID);
	data.set_job(m_job);
	data.set_level(CRankMgr::getSingleton().getLevelByCharID(m_charID));
	data.set_name(m_name);
	data.set_sex(m_sex);
	data.set_photoid(m_photoID);
	//data.set_lastlogintime(CFriendMgr::getLastLoginTime(m_charID));
	data.set_lastlogintime(CUserLastLoginTimeMgr::getSingleton().getLastTime(m_charID).c_str());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("Friend: id:%d, time:%s",m_charID, CUserLastLoginTimeMgr::getSingleton().getLastTime(m_charID).c_str());
#endif
	Player * pUser = PlayerManager::getSingleton().getUserByID(m_charID);
	if (pUser) {
		data.set_online(true);
		data.set_battlepower(pUser->m_battlePower);
	}
	else {
		data.set_online(false);
		data.set_battlepower(CRankMgr::getSingleton().getBattlePowerByCharID(m_charID));
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送好友的战力name=%s,battlepower=%d",m_name,data.battlepower());
#endif
}

CFriendMgr::CFriendMgr(Player * pOwner):ElementMgr(false,true,true,false),m_pOwner(pOwner)
{

}

CFriendMgr::~CFriendMgr()
{

}

bool CFriendMgr::addFriend(CFriendInfo * pFriend)
{
	if (!pFriend){
		return false;
	}
	return addElement(pFriend);
}

bool CFriendMgr::removeFriend(CFriendInfo * pFriend)
{
	if (!pFriend){
		return false;
	}
	removeElement(pFriend);
	return true;
}

void CFriendMgr::addNewFriend(const stFriendDBInfo & dbData)
{
	CFriendInfo  * pFriend = ATHENA_NEW CFriendInfo();
	pFriend->initFromDBData(dbData);
	if (!addFriend(pFriend)){
		Athena::logger->error("account=%s,name=%s,charID=%d 把玩家name=%s,charID=%d添加到容器失败!",m_pOwner->getAccount().c_str(),m_pOwner->getName(),m_pOwner->getID(),dbData.m_otherName,dbData.m_otherID);
		SAFE_DELETE(pFriend);
		return;
	}
	CFriendDBOp::getSingleton().addFriendRecord(dbData);
	FriendProto::stNotifyAddOneFriend cmd;
	FriendProto::stSynFriendInfo * pSynInfo = cmd.mutable_data();
	pFriend->fillSynData(*pSynInfo);
	m_pOwner->sendProtoToMe(cmd);

	CFriendDBOp::getSingleton().delApplyInfo(dbData.m_charID,dbData.m_otherID);

	FriendProto::stAddFriendTip  retCmd;
	retCmd.set_charid(m_pOwner->getID());
	retCmd.set_name(m_pOwner->getName());

	stFriendDBInfo dbData1;
	dbData1.m_charID = dbData.m_otherID;
	dbData1.m_otherID = dbData.m_charID;
	dbData1.m_job = m_pOwner->m_job;
	dbData1.m_sex = m_pOwner->m_sex;
	strncpy(dbData1.m_otherName,m_pOwner->getName(),sizeof(dbData1.m_otherName));
	dbData1.m_photoID = m_pOwner->m_photoID;

	Player * pOther = PlayerManager::getSingleton().getUserByID(dbData.m_otherID);
	if (pOther){//在线情况
		pOther->sendProtoToMe(retCmd);
		CFriendInfo  * pFriend1 = ATHENA_NEW CFriendInfo();
		pFriend1->initFromDBData(dbData1);
		if (!pOther->m_friendMgr.addFriend(pFriend1)){
			SAFE_DELETE(pFriend1);
		}
		else {
			CFriendDBOp::getSingleton().addFriendRecord(dbData1);	
			FriendProto::stNotifyAddOneFriend cmd1;
			FriendProto::stSynFriendInfo * pSynInfo1 = cmd1.mutable_data();
			pFriend1->fillSynData(*pSynInfo1);
			pOther->sendProtoToMe(cmd1);
		}
	}
	else {//离线情况
		CFriendDBOp::getSingleton().addFriendRecord(dbData1);	
	}
}

CTblField g_roleLastLoginTimeInfoDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  sizeof(UINT32)  },
	{ "LASTLOGINTIME",  CTblField::DB_TYPE_STRING,  20              },
	{ NULL, 0, 0 },
};

void CUserLastLoginTimeMgr::load()
{
	CTblQueryResult * result = g_databaseEngine.select(g_roleLastLoginTimeInfoDef,"ROLEINFO",NULL,NULL,0,NULL);
	if (result && result->getData()){                                                                          
		stLastLoginTime * pData = (stLastLoginTime * )result->getData();                                       
		for(UINT16 i = 0; i < result->getRowCount();++i){                                                      
			m_lastTimeMap[pData[i].m_charID] = pData[i].m_lastLoginTime;                                     
#ifdef _HDZ_DEBUG                                                                                              
			Athena::logger->trace("[Role]Role Info:charID:%d, lastLoginTime:%s",pData[i].m_charID,pData[i].    
					m_lastLoginTime);                                                                                              
#endif                                                                                                         
		}                                                                                                      
	}                                                                                                          
	SAFE_DELETE(result);
}

std::string CUserLastLoginTimeMgr::getLastTime(UINT32 id)
{
	std::map<UINT32, std::string>::iterator it = m_lastTimeMap.find(id);
	if(it == m_lastTimeMap.end()){
		std::string s = g_utc2StringDate(g_globalServer->getSec(),"%Y-%m-%d %H:%M:%S");
		Athena::logger->error("查找了不存在的key：%d,当前时间：%s",id, s.c_str());
		return s;
	}
	Athena::logger->trace("[最后上线时间]UserID:%d, lastTime:%s",id,it->second.c_str());
	return it->second;
}

void CFriendMgr::userOnline()
{
	/*
	CTblQueryResult * result = g_databaseEngine.select(g_roleLastLoginTimeInfoDef,"ROLEINFO",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stLastLoginTime * pData = (stLastLoginTime * )result->getData();
		for(UINT16 i = 0; i < result->getRowCount();++i){
			m_lastLoginTime[pData[i].m_charID] = pData[i].m_lastLoginTime;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[Role]Role Info:charID:%d, lastLoginTime:%s",pData[i].m_charID,pData[i].m_lastLoginTime);
#endif
		}
	}

	SAFE_DELETE(result);
*/

	std::string s = g_utc2StringDate(g_globalServer->getSec(),"%Y-%m-%d %H:%M:%S");
	CUserLastLoginTimeMgr::getSingleton().setLastTime(m_pOwner->getID(),s.c_str());
	Athena::logger->trace("[添加最后上线时间]UserID:%d, lastTime:%s",m_pOwner->getID(),s.c_str());          

	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere, 64, " CHARID=%d ", m_pOwner->getID());
	Athena::QueryCallback<CFriendDBOp,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<CFriendDBOp,UINT32>(&CFriendDBOp::getSingleton(),&CFriendDBOp::loadAllFriendCB,NULL,m_pOwner->getID());
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_friendTblDef,"FRIEND",szWhere,NULL,0,pDelaySql);

	//通知别人自己上线了
	CFriendDBOp::getSingleton().notifyAllFriend(m_pOwner->getID(),FriendProto::NOTIFY_ONLINE);
}

void CFriendMgr::synAllFriend()
{
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CFriendInfo	* info = (CFriendInfo*)it->second;
		if(info){
			char szWhere[65] = { 0 };
			bzero(szWhere, 65);
			snprintf(szWhere, 64, " CHARID=%d ", info->m_charID);
			Athena::QueryCallback<CFriendDBOp,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<CFriendDBOp,UINT32>(&CFriendDBOp::getSingleton(),&CFriendDBOp::synAllFriendCB,NULL,info->m_charID);
			CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
			g_databaseEngine.select(g_friendTblDef,"FRIEND",szWhere,NULL,0,pDelaySql);
		}
	}
}

void CFriendMgr::userOffline()
{
	CFriendDBOp::getSingleton().notifyAllFriend(m_pOwner->getID(),FriendProto::NOTIFY_OFFLINE);
}

void CFriendMgr::sendCanHelpGuardFriend(Player * pUser)
{
	if (!pUser){
		return ;
	}

	MiscProto::stRetCanHelpGuardFriend retCmd;

	TIDMap_IT it = m_idMap.begin();

	for (;it != m_idMap.end();++it){
		CFriendInfo * pFriend = (CFriendInfo *)it->second;		
		Player * pUser = PlayerManager::getSingleton().getUserByID(pFriend->m_charID);
		if (pUser && (pUser->m_level >= CGuardMgr::s_NEED_LEVEL)){
			stGuardUserInfo *pGaurdUserInfo = CGuardMgr::getSingleton().getGuardUserInfo(pFriend->m_charID);
			if (pGaurdUserInfo && (pGaurdUserInfo->m_helpNum < GuardCfg::getSingleton().getHelpNum())){
				MiscProto::stHelpGaurdFriendInfo * pSyn = retCmd.add_infos();
				if (pSyn){
					pSyn->set_charid(pFriend->m_charID);
					pSyn->set_name(pFriend->m_name);
					pSyn->set_remainhelpnum(GuardCfg::getSingleton().getHelpNum() - pGaurdUserInfo->m_helpNum);
					pSyn->set_level(CRankMgr::getSingleton().getLevelByCharID(pFriend->m_charID));
				}
			}
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void stApplyAddFriend::fillSynInfo(FriendProto::stSynApplyerInfo * pSyn)
{
	if (!pSyn){
		return ;
	}
	pSyn->set_charid(m_charID);
	pSyn->set_name(m_name);
	pSyn->set_level(m_level);
	pSyn->set_job(m_job);
	pSyn->set_sex(m_sex);
	pSyn->set_battlepower(CRankMgr::getSingleton().getBattlePowerByCharID(m_charID));
	pSyn->set_photoid(m_photoID);
}

CFriendDBOp::CFriendDBOp()
{
	m_zeroRefresh = false;	
}

CFriendDBOp::~CFriendDBOp()
{

}

bool CFriendDBOp::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/Friend.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->trace("[好友配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}

	TiXmlElement * pRoot = doc.FirstChildElement("FriendConf");
	if (!pRoot){
		Athena::logger->trace("[好友配置]加载杂项时候找不到FriendConf节点!");
		return false;
	}
	return true;
}

void CFriendDBOp::loadAllFriendCB(CTblQueryResult* result,UINT32 charID)
{
	if (!result){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (!pUser){
		return ;
	}

	FriendProto::stSynAllFriend cmd;
	cmd.set_fetchliliannum(pUser->m_fetchLilianNum);

	std::map<UINT32,std::set<UINT32> > &giveInfos = CLilianMgr::getSingleton().getGiveLilianInfos();
	std::map<UINT32,std::set<UINT32> >::iterator it = giveInfos.find(charID);

	if (it != giveInfos.end()){
		std::set<UINT32>::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			cmd.add_hasgiveliliancharid(*subIt);	
			if (cmd.hasgiveliliancharid_size() >= 100){
				break;
			}
		}
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_friendTblDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	byte * p = NULL;
	if (result->getData()){
		for (UINT16 i = 0;i < result->getRowCount(); ++i){
			p = result->getData() + sizeof(stFriendDBInfo) * i;
			stFriendDBInfo & one = *(stFriendDBInfo *)p;
			CFriendInfo  * pFriend = ATHENA_NEW CFriendInfo();
			pFriend->initFromDBData(one);

			if (!pUser->m_friendMgr.addFriend(pFriend)){
				Athena::logger->error("玩家(account=%s,name=%s,id=%d)上线加载好友(name=%s,id=%d, photoID=%d),添加到容器失败",pUser->getAccount().c_str(),pUser->getName(),pUser->getID(),pFriend->m_name,pFriend->m_charID, pFriend->m_photoID);
				SAFE_DELETE(pFriend);
				continue;
			}

			FriendProto::stSynFriendInfo *pSynInfo = cmd.add_allfriend();
			pFriend->fillSynData(*pSynInfo);
#ifdef _HDZ_DEBUG
			Athena::logger->error("玩家(account=%s,name=%s,id=%d)上线加载好友(name=%s,id=%d, photoID=%d)",pUser->getAccount().c_str(),pUser->getName(),pUser->getID(),pFriend->m_name,pFriend->m_charID, pFriend->m_photoID);
#endif
		}
	}
	pUser->sendProtoToMe(cmd);	
}

void CFriendDBOp::synAllFriendCB(CTblQueryResult* result,UINT32 charID)
{
	if (!result){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (!pUser){
		return ;
	}

	FriendProto::stSynAllFriend cmd;
	cmd.set_fetchliliannum(pUser->m_fetchLilianNum);

	std::map<UINT32,std::set<UINT32> > &giveInfos = CLilianMgr::getSingleton().getGiveLilianInfos();
	std::map<UINT32,std::set<UINT32> >::iterator it = giveInfos.find(charID);

	if (it != giveInfos.end()){
		std::set<UINT32>::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			cmd.add_hasgiveliliancharid(*subIt);	
			if (cmd.hasgiveliliancharid_size() >= 100){
				break;
			}
		}
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_friendTblDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	byte * p = NULL;
	if (result->getData()){
		for (UINT16 i = 0;i < result->getRowCount(); ++i){
			p = result->getData() + sizeof(stFriendDBInfo) * i;
			stFriendDBInfo & one = *(stFriendDBInfo *)p;
			CFriendInfo  * pFriend = ATHENA_NEW CFriendInfo();
			pFriend->initFromDBData(one);
			/*
			if (!pUser->m_friendMgr.addFriend(pFriend)){
				Athena::logger->error("玩家(account=%s,name=%s,id=%d)上线加载好友(name=%s,id=%d),添加到容器失败",pUser->getAccount().c_str(),pUser->getName(),pUser->getID(),pFriend->m_name,pFriend->m_charID);
				SAFE_DELETE(pFriend);
				continue;
			}
*/
			FriendProto::stSynFriendInfo *pSynInfo = cmd.add_allfriend();
			pFriend->fillSynData(*pSynInfo);
#ifdef _HDZ_DEBUG
			Athena::logger->error("玩家(account=%s,name=%s,id=%d)上线加载好友(name=%s,id=%d)",pUser->getAccount().c_str(),pUser->getName(),pUser->getID(),pFriend->m_name,pFriend->m_charID);
#endif
		}
	}
	pUser->sendProtoToMe(cmd);
}

void CFriendDBOp::addFriendRecord(const stFriendDBInfo & data)
{
	Athena::CUDSqlCallBack<CFriendDBOp> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CFriendDBOp>(this,&CFriendDBOp::addFriendRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_friendTblDef,(const char *)&data,"FRIEND",autoID,pDelaySql);
}

void CFriendDBOp::addFriendRecordCB(bool opResult,UINT64 autoID)
{

}

void CFriendDBOp::updateOneInfo(Player *pUser) 
{
	if(!pUser){
		return;
	}

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "OTHERID=%d", pUser->getID());
	CTblQueryResult* result = g_databaseEngine.select(g_friendTblDef,"FRIEND",where,NULL,0,NULL);

	if (result && result->getData()){
		stFriendDBInfo * pMemberDB = (stFriendDBInfo *)result->getData();                        
		for (UINT16 i = 0;i < result->getRowCount();++i){ 
			stFriendDBInfo dbData;
			dbData.m_charID = pMemberDB[i].m_charID;
			dbData.m_otherID = pMemberDB[i].m_otherID;
			strncpy(dbData.m_otherName, pUser->getName(), sizeof(dbData.m_otherName));
			dbData.m_job = pMemberDB[i].m_job;
			dbData.m_sex = pMemberDB[i].m_sex;
			dbData.m_photoID = pUser->m_photoID;
			updateRecord(dbData);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("更新莫个人的信息 otherName:%s, otherID:%d, photoID:%d, ",pUser->getName(), pMemberDB[i].m_otherID, pUser->m_photoID); 
#endif
		}
	}
	SAFE_DELETE(result);
}

void CFriendDBOp::updateRecord(const stFriendDBInfo& data) 
{
	char where[65] = { 0 }; 
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u AND OTHERID=%u",data.m_charID, data.m_otherID);
	Athena::CUDSqlCallBack<CFriendDBOp> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CFriendDBOp>(this,&CFriendDBOp::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_friendTblDef,(const char *)&data,"FRIEND",where,pDelaySql);
}

void CFriendDBOp::updateRecordCB(bool opResult,UINT64 autoID)
{}

void CFriendDBOp::delFriendRecord(UINT32 charID,UINT32 otherID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%d AND OTHERID=%d",charID,otherID);
	Athena::CUDSqlCallBack<CFriendDBOp> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CFriendDBOp>(this,&CFriendDBOp::delFriendRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("FRIEND",where,pDelaySql);
}

void CFriendDBOp::delFriendRecordCB(bool opResult,UINT64 autoID)
{}

CTblField  g_friendNotifyTblDef[] = {
	{ "CHARID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ NULL, 0, 0 },
};


void CFriendDBOp::notifyAllFriend(UINT32 charID,UINT8 type)
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere, 64, " OTHERID=%d ", charID);
	Athena::QueryCallback<CFriendDBOp,UINT32,UINT8 > * pCallBack = ATHENA_NEW Athena::QueryCallback<CFriendDBOp,UINT32,UINT8>(&CFriendDBOp::getSingleton(),&CFriendDBOp::notifyAllFriendCB,NULL,charID,type);
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_friendNotifyTblDef,"FRIEND",szWhere,NULL,0,pDelaySql);

}

void CFriendDBOp::notifyAllFriendCB(CTblQueryResult* result,UINT32 charID,UINT8 type)
{
	if (!result){
		return ;
	}

	/*Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	  if (!pUser){
	  return ;
	  }*/

	UINT32 recordLen = 0;
	result->setFieldDef(g_friendNotifyTblDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	FriendProto::stNotifyFriend cmd;
	cmd.set_charid(charID);
	cmd.set_type((FriendProto::NotifyFriendType)type);
	if (result->getData()){
		for (UINT16 i = 0;i < result->getRowCount();++i){
			UINT32 notifyID = *(UINT32 *)(result->getData() + sizeof(UINT32) * i);
			Player * pOther = PlayerManager::getSingleton().getUserByID(notifyID);
			if (pOther){
				pOther->sendProtoToMe(cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("好友上线通知%s,%d上线了",pOther->getName(),charID);
#endif
			}
		}	
	}
}

#pragma pack(1)
struct stAddFriendRoleInfo
{
	UINT32 m_otherID;						//对方的角色ID
	char   m_otherName[MAX_NAMESIZE + 1];	//对方的角色名字
	UINT16 m_level;							//等级
	UINT8  m_job;							//职业
	UINT8  m_sex;							//性别
	UINT16 m_photoID;						//头像ID

	stAddFriendRoleInfo()
	{
		m_otherID = 0;
		bzero(m_otherName,sizeof(m_otherName));
		m_level = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};
#pragma pack()

CTblField g_RoleFriendInfoDef[] = {
	{ "CHARID",         CTblField::DB_TYPE_UINT32,  		sizeof(UINT32)   },
	{ "NAME",           CTblField::DB_TYPE_STRING,          MAX_NAMESIZE + 1 },
	{ "LEVEL",          CTblField::DB_TYPE_UINT16,          sizeof(UINT16)   },
	{ "JOB",            CTblField::DB_TYPE_UINT8,           sizeof(byte)     },
	{ "SEX",            CTblField::DB_TYPE_UINT8,           sizeof(byte)     },
	{ "USERPHOTOID",    CTblField::DB_TYPE_UINT16,          sizeof(UINT16)   },
	{ NULL, 0, 0 },
};

void CFriendDBOp::selectFriendInfoCB(CTblQueryResult* result,UINT32 charID)
{
	if (!result){
		return ;
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_RoleFriendInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);

	if (result->getData()){
		stAddFriendRoleInfo & roleInfo = *(stAddFriendRoleInfo *)result->getData();
		stFriendDBInfo dbData;
		dbData.m_charID = charID;
		dbData.m_sex = roleInfo.m_sex;
		dbData.m_job = roleInfo.m_job;
		dbData.m_otherID = roleInfo.m_otherID;
		dbData.m_photoID = roleInfo.m_photoID;
		strncpy(dbData.m_otherName,roleInfo.m_otherName,sizeof(dbData.m_otherName));
		CFriendDBOp::getSingleton().delApplyInfo(dbData.m_charID,dbData.m_otherID);
		Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
		if (pUser) {
			pUser->m_friendMgr.addNewFriend(dbData);
			FriendProto::stAddFriendTip  retCmd;
			retCmd.set_charid(roleInfo.m_otherID);
			retCmd.set_name(dbData.m_otherName);
			pUser->sendProtoToMe(retCmd);
		}
		else {
			/*CFriendDBOp::getSingleton().addFriendRecord(dbData);
			  stApplyAddFriend applyInfo;
			  applyInfo.m_charID = dbData.m_otherID;
			  strncpy(applyInfo.m_name,dbData.m_otherName,sizeof(applyInfo.m_name));
			  applyInfo.m_level = CRankMgr::getSingleton().getLevelByCharID(dbData.m_otherID);//dbData.m_level;
			  applyInfo.m_job = dbData.m_job;
			  applyInfo.m_sex = dbData.m_sex;
			  applyInfo.m_applyTime = g_globalServer->getSec();
			  CFriendDBOp::getSingleton().addApplyInfo2List(dbData.m_charID,applyInfo);

			  FriendProto::stAddFriendTip  retCmd;
			  retCmd.set_charid(dbData.m_charID);

			  Player * pOther = PlayerManager::getSingleton().getUserByID(dbData.m_otherID);
			  if (pOther){
			  pOther->sendProtoToMe(retCmd);
			  }*/
		}
	}
}

void CFriendDBOp::findRoleInfoCB(CTblQueryResult* result,UINT32 charID)
{

	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (!pUser){
		return ;
	}
	FriendProto::stFindAndAddFriendResult retCmd;

	if (!result){
		retCmd.set_result(0);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	UINT32 recordLen = 0;
	result->setFieldDef(g_RoleFriendInfoDef);
	recordLen = result->cacRecordLen();
	result->fillData(recordLen);

	if (result->getData()){
		stAddFriendRoleInfo & roleInfo = *(stAddFriendRoleInfo *)result->getData();
		/*stFriendDBInfo dbData;
		  dbData.m_charID = pUser->getID();
		  dbData.m_sex = roleInfo.m_sex;
		  dbData.m_job = roleInfo.m_job;
		//dbData.m_level = roleInfo.m_level;
		dbData.m_otherID = roleInfo.m_otherID;
		strncpy(dbData.m_otherName,roleInfo.m_otherName,sizeof(dbData.m_otherName));
		pUser->m_friendMgr.addNewFriend(dbData);
		retCmd.set_name(roleInfo.m_otherName);
		retCmd.set_result(1);
		pUser->sendProtoToMe(retCmd);*/
		stApplyAddFriend applyInfo;
		applyInfo.m_charID = pUser->getID();
		strncpy(applyInfo.m_name,pUser->getName(),sizeof(applyInfo.m_name));
		applyInfo.m_level = pUser->m_level;
		applyInfo.m_job = pUser->m_job;
		applyInfo.m_sex = pUser->m_sex;
		applyInfo.m_applyTime = g_globalServer->getSec();
		applyInfo.m_photoID = pUser->m_photoID;
		CFriendDBOp::getSingleton().addApplyInfo2List(roleInfo.m_otherID,applyInfo);

		FriendProto::stAdd2ApplyListResult retCmd;
		retCmd.set_result(FriendProto::stAdd2ApplyListResult::eSUCCESS);
		pUser->sendProtoToMe(retCmd);
	}
}

void CFriendDBOp::addApplyInfo2List(UINT32 charID,stApplyAddFriend & one)
{
	/*std::list<stApplyAddFriend> & cnt = m_applyList[charID];
	  std::list<stApplyAddFriend>::iterator it = cnt.begin();
	  for (;it != cnt.end(); ++it){
	  if (it->m_charID == one.m_charID){
	  return ;
	  }
	  }
	  cnt.push_back(one);*/
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere, 64, " CHARID=%d  AND OTHERID=%d ",one.m_charID,charID);
	Athena::QueryCallback<CFriendDBOp,UINT32,stApplyAddFriend> * pCallBack = ATHENA_NEW Athena::QueryCallback<CFriendDBOp,UINT32,stApplyAddFriend>(&CFriendDBOp::getSingleton(),&CFriendDBOp::addApplyInfo2ListCB,NULL,charID,one);
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_friendTblDef,"FRIEND",szWhere,NULL,0,pDelaySql);

}

void CFriendDBOp::addApplyInfo2ListCB(CTblQueryResult* result,UINT32 charID,stApplyAddFriend one)
{
	if (!result){
		std::list<stApplyAddFriend> & cnt = m_applyList[charID];
		std::list<stApplyAddFriend>::iterator it = cnt.begin();
		for (;it != cnt.end(); ++it){
			if (it->m_charID == one.m_charID){
				return ;
			}
		}
		cnt.push_back(one);
		Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
		if (pUser){
			FriendProto::stSomeOneApplyAddFriend retCmd;
			one.fillSynInfo(retCmd.mutable_one());
			pUser->sendProtoToMe(retCmd);
		}
	}
}

void CFriendDBOp::delApplyInfo(UINT32 charID,UINT32 otherID)
{
	if (m_applyList.find(charID) == m_applyList.end()){
		return ;
	}

	std::list<stApplyAddFriend> & cnt = m_applyList[charID];
	std::list<stApplyAddFriend>::iterator it = cnt.begin();
	for (;it != cnt.end(); ++it){
		if (it->m_charID == otherID){
			cnt.erase(it);
			return ;
		}
	}
}

bool CFriendDBOp::isInApplyList(UINT32 charID,UINT32 otherID)
{
	if (m_applyList.find(charID) == m_applyList.end()){
		return false;
	}

	std::list<stApplyAddFriend> & cnt = m_applyList[charID];
	std::list<stApplyAddFriend>::iterator it = cnt.begin();
	for (;it != cnt.end(); ++it){
		if (it->m_charID == otherID){
			return true;
		}
	}
	return false;
}


void CFriendDBOp::timer(UINT32 cur)
{
	struct tm curTm;
	RealTime::getLocalTime(curTm,cur);
	if ((curTm.tm_hour == 0)  
			&&(curTm.tm_min == 0)
			&&(curTm.tm_sec < 10)){
		if (!m_zeroRefresh){
			m_zeroRefresh = true;
		}
	}
	else {
		m_zeroRefresh = false;
	}
}

void CFriendDBOp::reqAddFriend(UINT32 charID,UINT32 otherID)
{
	if (otherID == charID){
		Athena::logger->error("自己不能加自己为好友!");
		return ;	
	}

	if (!isInApplyList(charID,otherID)){//不在申请列表中
		return ;	
	}


	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	Player * pOther = PlayerManager::getSingleton().getUserByID(otherID);

	if (!pUser){//被添加的不应该不在线
		return ;
	}

	if (pUser->m_friendMgr.getFriendNum() >= CFriendMgr::s_MAX_FRIEND_NUM){
		return;
	}

	if (!pOther){//不在线情况
		char szWhere[65] = { 0 };
		bzero(szWhere, 65);
		snprintf(szWhere, 64, " CHARID=%d ",otherID);
		Athena::QueryCallback<CFriendDBOp,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<CFriendDBOp,UINT32>(this,&CFriendDBOp::selectFriendInfoCB,NULL,charID);
		CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
		g_databaseEngine.select(g_RoleFriendInfoDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);
	}
	else {//在线情况
		stFriendDBInfo dbData;
		dbData.m_charID = pUser->getID();
		dbData.m_sex = pOther->m_sex;
		dbData.m_job = pOther->m_job;
		//dbData.m_level = pOther->m_level;
		dbData.m_otherID = pOther->getID();
		strncpy(dbData.m_otherName,pOther->getName(),sizeof(dbData.m_otherName));
		dbData.m_photoID = pOther->m_photoID;
		pUser->m_friendMgr.addNewFriend(dbData);

		FriendProto::stAddFriendTip  retCmd;
		retCmd.set_charid(pOther->getID());
		retCmd.set_name(pOther->getName());
		pUser->sendProtoToMe(retCmd);

	}
}

void CFriendDBOp::reqRemoveFriend(Player * pUser,UINT32 charID)
{
	CFriendInfo * pFriend =  pUser->m_friendMgr.getFriend(charID);
	if (!pFriend){
		Athena::logger->error("account=%s,name=%s,charID=%d请求删除好友%d时候找不到好友信息",pUser->getAccount().c_str(),pUser->getName(),pUser->getID(),charID);
		return ;
	}
	if (pUser->m_friendMgr.removeFriend(pFriend)){
		FriendProto::stDelFriend retCmd;
		retCmd.set_charid(charID);
		pUser->sendProtoToMe(retCmd);
		delFriendRecord(pUser->getID(),charID);
		SAFE_DELETE(pFriend);

		Player * pOther = PlayerManager::getSingleton().getUserByID(charID);
		if (pOther){
			CFriendInfo * pOtherFriend = pOther->m_friendMgr.getFriend(pUser->getID());
			if (pOtherFriend && pOther->m_friendMgr.removeFriend(pOtherFriend)){
				FriendProto::stDelFriend  retCmd1;
				retCmd1.set_charid(pUser->getID());
				pOther->sendProtoToMe(retCmd1);
				SAFE_DELETE(pOtherFriend);
			}
		}
		delFriendRecord(charID,pUser->getID());
		CLilianMgr::getSingleton().removeLilian(pUser->getID(),charID);
		return ;
	}
}

void CFriendDBOp::findAndAdd(Player * pUser,char * name)
{
	if (pUser->m_friendMgr.getFriend(name)) {//已经是好友了
		return;
	}
	Player * pOther = PlayerManager::getSingleton().getUserByName(name);
	if (!pOther){//不存在或者不在线 
#ifdef _THAILAND_VER
		char szWhere[200] = { 0 };
#else
		char szWhere[65] = { 0 };
#endif
		bzero(szWhere, sizeof(szWhere));
		snprintf(szWhere, sizeof(szWhere) - 1, " NAME='%s' ",name);
		Athena::QueryCallback<CFriendDBOp,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<CFriendDBOp,UINT32>(this,&CFriendDBOp::findRoleInfoCB,NULL,pUser->getID());
		CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
		g_databaseEngine.select(g_RoleFriendInfoDef,"ROLEINFO",szWhere,NULL,0,pDelaySql);
	}
	else {
		if (pUser == pOther){
			return ;
		}

		stApplyAddFriend applyInfo;
		applyInfo.m_charID = pUser->getID();
		strncpy(applyInfo.m_name,pUser->getName(),sizeof(applyInfo.m_name));
		applyInfo.m_level = pUser->m_level;
		applyInfo.m_job = pUser->m_job;
		applyInfo.m_sex = pUser->m_sex;
		applyInfo.m_applyTime = g_globalServer->getSec(); 
		applyInfo.m_photoID = pUser->m_photoID;
		CFriendDBOp::getSingleton().addApplyInfo2List(pOther->getID(),applyInfo);

		FriendProto::stAdd2ApplyListResult retCmd;
		retCmd.set_result(FriendProto::stAdd2ApplyListResult::eSUCCESS);
		pUser->sendProtoToMe(retCmd);
		/*stFriendDBInfo dbData;
		  dbData.m_charID = pUser->getID();
		  dbData.m_sex = pOther->m_sex;
		  dbData.m_job = pOther->m_job;
		  dbData.m_otherID = pOther->getID();
		  strncpy(dbData.m_otherName,pOther->getName(),sizeof(dbData.m_otherName));
		  pUser->m_friendMgr.addNewFriend(dbData);
		  FriendProto::stFindAndAddFriendResult resultCmd;
		  resultCmd.set_result(1);
		  resultCmd.set_name(name);
		  pUser->sendProtoToMe(resultCmd);*/
	}
}

void CFriendDBOp::getApplyList(Player * pUser,std::vector<UINT32> & applyList)
{
	if (!pUser){
		return ;
	}

	ApplyList_IT  it = m_applyList.find(pUser->getID());
	if (it == m_applyList.end()){
		return;
	}

	std::list<stApplyAddFriend>::iterator subIt = it->second.begin();
	for (;subIt != it->second.end();){
		stApplyAddFriend & info = *subIt;
		if (pUser->m_friendMgr.getFriend(info.m_charID)){
			std::list<stApplyAddFriend>::iterator tmpIt = subIt;
			++subIt;
			it->second.erase(tmpIt);
			continue;
		}
		applyList.push_back(info.m_charID);
		++subIt;
	}	
}

void CFriendDBOp::reqApplyList(Player * pUser)
{
	if (!pUser){
		return ;
	}

	FriendProto::stRetApplyAddFriendList retCmd;
	ApplyList_IT  it = m_applyList.find(pUser->getID());
	if (it == m_applyList.end()){
		pUser->sendProtoToMe(retCmd);
		return;
	}

	std::list<stApplyAddFriend>::iterator subIt = it->second.begin();
	for (;subIt != it->second.end();){
		stApplyAddFriend & info = *subIt;
		if (pUser->m_friendMgr.getFriend(info.m_charID)){
			std::list<stApplyAddFriend>::iterator tmpIt = subIt;
			++subIt;
			it->second.erase(tmpIt);
			continue;
		}
		++subIt;
		FriendProto::stSynApplyerInfo * pSyn = retCmd.add_applyerlist();
		if (pSyn){
			info.fillSynInfo(pSyn);
		}
	}	
	pUser->sendProtoToMe(retCmd);
}

CLilianMgr::CLilianMgr()
{
	m_zeroRefresh = false;
}

CLilianMgr::~CLilianMgr()
{
}

void CLilianMgr::giveLilian2Friend(Player * pUser,UINT32 charID)
{
	if (!pUser){
		return ;
	}

	FriendProto::stGiveLilianResult retCmd;
	retCmd.set_charid(charID);

	if (!pUser->m_friendMgr.getFriend(charID)){
		retCmd.set_result(FriendProto::stGiveLilianResult::eFAIL);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[赠送历练]name=%s,charID=%d赠送不成功,toID=%d不是好友",pUser->getName(),pUser->getID(),charID);
#endif
		return ;
	}

	UINT32 myID = charID;
	UINT32 otherID = pUser->getID();

	if ((m_recLilianInfos[myID].find(otherID) != m_recLilianInfos[myID].end())
			||(m_fetchLilianInfos[myID].find(otherID) != m_fetchLilianInfos[myID].end())){
		retCmd.set_result(FriendProto::stGiveLilianResult::eFAIL);
		pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("[赠送历练]name=%s,charID=%d赠送不成功,已经给toID=%d赠送过了历练!",pUser->getName(),pUser->getID(),charID);
#endif
		return ;	
	}
	m_recLilianInfos[myID].insert(otherID);
	m_giveLilianInfos[otherID].insert(myID);
	retCmd.set_result(FriendProto::stGiveLilianResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);

	Global::stGiveLilianDayTaskCb cmd;
	cmd.set_charid(pUser->getID());
	pUser->sendProto2Game(cmd);
}

void CLilianMgr::removeLilian(UINT32 myID,UINT32 otherID)
{
	std::map<UINT32,std::set<UINT32> >::iterator it = m_recLilianInfos.find(myID);	

	if (it != m_recLilianInfos.end()){
		it->second.erase(otherID);
	}
}

void CLilianMgr::getLilianList(Player * pUser)
{
	if (!pUser){
		return ;
	}

	FriendProto::stReceiveLilianList retCmd;
	UINT32 myID = pUser->getID();
	if (m_recLilianInfos.find(myID) == m_recLilianInfos.end()){
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	std::set<UINT32>::iterator it = m_recLilianInfos[myID].begin();
	for (;it != m_recLilianInfos[myID].end();++it){
		retCmd.add_charids(*it);	
	}
	pUser->sendProtoToMe(retCmd);
}

void CLilianMgr::fetchLilian(Player * pUser,UINT32 charID)
{
	if (!pUser){
		return ;
	}

	FriendProto::stFetchLilianResult retCmd;
	retCmd.set_charid(charID);
	retCmd.set_type(FriendProto::FETCH_ONE);
	if (pUser->m_fetchLilianNum >= s_MAX_FETCH_LILIAN_NUM){
		retCmd.set_result(FriendProto::stFetchLilianResult::eNUM_USE_OUT);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	UINT32 myID = pUser->getID();
	if (m_recLilianInfos[myID].find(charID) == m_recLilianInfos[myID].end()){
		retCmd.set_result(FriendProto::stFetchLilianResult::eNOT_GIVE_LILIAN);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	pUser->m_fetchLilianNum += 1;
	m_recLilianInfos[myID].erase(charID);
	m_fetchLilianInfos[myID].insert(charID);

	retCmd.set_result(FriendProto::stFetchLilianResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);

	Global::stNotifyAddLilian cmd;
	cmd.set_charid(myID);
	cmd.set_num(GIVE_LILIAN_NUM_PER_TIME);
	pUser->sendProto2Game(cmd);
	return ;
}

void CLilianMgr::fetchAllLilian(Player * pUser)
{
	if (!pUser){
		return  ;
	}	

	FriendProto::stFetchLilianResult retCmd;
	retCmd.set_charid(0);
	retCmd.set_type(FriendProto::FETCH_ALL);
	UINT32 myID = pUser->getID();

	if (pUser->m_fetchLilianNum >= s_MAX_FETCH_LILIAN_NUM){
		retCmd.set_result(FriendProto::stFetchLilianResult::eNUM_USE_OUT);
		pUser->sendProtoToMe(retCmd);
		m_recLilianInfos[myID].clear();
		return ;
	}

	UINT32 remainNum = s_MAX_FETCH_LILIAN_NUM - pUser->m_fetchLilianNum;
	UINT32 canFetchNum = m_recLilianInfos[myID].size() >= remainNum ? remainNum : m_recLilianInfos[myID].size(); 

	std::set<UINT32>::iterator it = m_recLilianInfos[myID].begin();
	std::set<UINT32>::iterator end = m_recLilianInfos[myID].end();
	std::set<UINT32> & fetchLilianInfos = m_fetchLilianInfos[myID];
	std::set<UINT32> & giveLilianInfos  = m_giveLilianInfos[myID];

	for (;it != end;++it){
		fetchLilianInfos.insert(*it);
		if (giveLilianInfos.find(*it) == giveLilianInfos.end()){//没有赠送过的才可以赠送
			m_recLilianInfos[*it].insert(myID);
			giveLilianInfos.insert(*it);
		}
	}

	pUser->m_fetchLilianNum += canFetchNum;
	m_recLilianInfos.erase(myID);
	retCmd.set_result(FriendProto::stFetchLilianResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);

	Global::stNotifyAddLilian cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_num(GIVE_LILIAN_NUM_PER_TIME * canFetchNum);
	pUser->sendProto2Game(cmd);
}

void CLilianMgr::timer(UINT32 cur)
{
	struct tm curTm;
	RealTime::getLocalTime(curTm,cur);
	if ((curTm.tm_hour == 0)  
			&&(curTm.tm_min == 0)
			&&(curTm.tm_sec < 10)){
		if (!m_zeroRefresh){
			m_zeroRefresh = true;
			m_recLilianInfos.clear();
			m_fetchLilianInfos.clear();
			m_giveLilianInfos.clear();
		}   
	}   
	else {
		m_zeroRefresh = false;
	}   
}

CTblField g_marryTblDef[] = { 
	{ "ID",				    CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ "HUSBAND",	        CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ "WIFE",	            CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ "HUS_RING",	        CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ "WIFE_RING",	        CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ "WEDDING_ID",	        CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ "EXPIRE_TIME",	        CTblField::DB_TYPE_UINT32,			sizeof(UINT32)      },
	{ NULL, 0, 0 },
};

void CMarryMgr::load() {
	CTblQueryResult* result = g_databaseEngine.select(g_marryTblDef, "MARRY_INFO",NULL,NULL,0,NULL); 
	if (result && result->getData()) {
        stMarryInfoDB * db = (stMarryInfoDB*) result->getData();
        MarryInfo marryInfo;
        for (UINT16 i = 0;i < result->getRowCount();++i){
            marryInfo.load(db[i]);
            m_coupleMap.insert(std::make_pair(marryInfo.id, marryInfo));
			m_coupleToRoomMap.insert(std::make_pair(marryInfo.husband, marryInfo.id));
			m_coupleToRoomMap.insert(std::make_pair(marryInfo.wife, marryInfo.id));
        }
    }
#ifdef _HDZ_DEBUG
    for (std::map<UINT32, MarryInfo >::iterator it =
            m_coupleMap.begin(); it != m_coupleMap.end(); it++) {
        Athena::logger->trace("[结婚房间信息]id is %d, husband is %d, wife is %d, husRing %d, wifeRing %d, weddingId %d, expireTime %d",
                    it->first, it->second.husband, it->second.wife, it->second.husRing, it->second.wifeRing, it->second.weddingId, it->second.expireTime);
    }
#endif
	SAFE_DELETE(result);
}

void CMarryMgr::insertRecord(const stMarryInfoDB & data) {
	Athena::CUDSqlCallBack<CMarryMgr, stMarryInfoDB> * pDelayCB =
        ATHENA_NEW Athena::CUDSqlCallBack<CMarryMgr, stMarryInfoDB>(this,&CMarryMgr::insertRecordCB,false,0, data);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_marryTblDef,(const char *)&data,"MARRY_INFO",autoID,pDelaySql);
}

void CMarryMgr::insertRecordCB(bool opResult,UINT64 autoID, stMarryInfoDB data) {
    if (opResult) {
        data.id = autoID;
#ifdef _HDZ_DEBUG
        Athena::logger->trace("CMarryMgr::insertRecordCB id is %d", data.id);
#endif
        MarryInfo marryInfo;
        marryInfo.load(data);
        bool res = m_coupleMap.insert(std::make_pair(marryInfo.id, marryInfo)).second;
        bool hRes = m_coupleToRoomMap.insert(std::make_pair(marryInfo.husband, marryInfo.id)).second;
        bool wRes = m_coupleToRoomMap.insert(std::make_pair(marryInfo.wife, marryInfo.id)).second;
        addFriendToRoom(marryInfo.husband, marryInfo.id);
        addFriendToRoom(marryInfo.wife, marryInfo.id);
        Athena::logger->trace("CMarryMgr::insertRecordCB m_coupleMap.insert is %d, hRes is %d, wRes is %d",
                res, hRes, wRes);
        Global::stNotifyCoupleMarryCb cmd;
        cmd.set_husband(marryInfo.husband);
        cmd.set_wife(marryInfo.wife);
        cmd.set_roomid(marryInfo.id);
        g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
    }
}

void CMarryMgr::coupleMarry(UINT32 husband, UINT32 wife, UINT32 id, const std::string & husName, const std::string & wifeName) {
    stMarryInfoDB data;
    data.husband = husband;
    data.wife = wife;
    data.weddingId = id;
    insertRecord(data);

    stRoleCoupleInfoDB husDb, wifeDb;
    husDb.charId = husband;
    husDb.coupleId = wife;
    strncpy(husDb.coupleName, husName.c_str(), sizeof(husDb.coupleName));
    updateCoupleRecord(husDb);

    wifeDb.charId = wife;
    wifeDb.coupleId = husband;
    strncpy(wifeDb.coupleName, wifeName.c_str(), sizeof(wifeDb.coupleName));
    updateCoupleRecord(wifeDb);
}

void CMarryMgr::delRecord(UINT32 roomId) {
	char where[65] = { 0 };
	bzero(where, 65);	
	snprintf(where, 64, " ID=%d ", roomId);
	Athena::CUDSqlCallBack<CMarryMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CMarryMgr>(this,&CMarryMgr::delRecordCB,false,0);			
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
    g_databaseEngine.deleteRecord("MARRY_INFO",where,pDelaySql);
}

void CMarryMgr::delRecordCB(bool opResult,UINT64 autoID)
{ }

CTblField g_coupleTblDef[] = {
	{ "CHARID",      CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "COUPLE_ID",   CTblField::DB_TYPE_UINT32,  sizeof(UINT32) },
	{ "COUPLE_NAME", CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ NULL, 0, 0 },
};

void CMarryMgr::updateCoupleRecord(const stRoleCoupleInfoDB & data) {
	char where[65] = { 0 };
	bzero(where, 65);	
	snprintf(where, 64, " CHARID=%d ", data.charId);

	Athena::CUDSqlCallBack<CMarryMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CMarryMgr>(this,&CMarryMgr::updateCoupleRecordCB,false,0);			
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_coupleTblDef,(const char *)&data,"ROLEINFO",where,pDelaySql);
}

void CMarryMgr::updateCoupleRecordCB(bool opResult,UINT64 autoID)
{ }

void CMarryMgr::updateRecord(const stMarryInfoDB & data) {
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%u",data.id);
	
	Athena::CUDSqlCallBack<CMarryMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CMarryMgr>(this,&CMarryMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_marryTblDef,(const char *)&data,"MARRY_INFO",where,pDelaySql);
}

void CMarryMgr::updateRecordCB(bool opResult,UINT64 autoID) {

}

UINT32 CMarryMgr::getRoomId(UINT32 coupleId) {
    std::map<UINT32, UINT32>::iterator it = m_coupleToRoomMap.find(coupleId);
    if (it != m_coupleToRoomMap.end()) {
        Athena::logger->trace("CMarryMgr::getRoomId find roomId %d, coupleId is %d",
                it->second, coupleId);
        return it->second;
    }
    return 0;
}

bool CMarryMgr::roomExists(UINT32 roomId) {
	return m_coupleMap.find(roomId) != m_coupleMap.end();
}

bool CMarryMgr::coupleDivorce(UINT32 friendId) {
    UINT32 roomId = getRoomId(friendId);
    if (!roomId) {
		Athena::logger->trace("CMarryMgr::coupleDivorce not find roomId, friend id is %d", friendId);
		return false;
	}
	std::map<UINT32, MarryInfo>::iterator tmpIt = m_coupleMap.find(roomId);
	if (tmpIt == m_coupleMap.end()) {
		Athena::logger->trace("CMarryMgr::coupleDivorce not find couple, friend id is %d", friendId);
		return false;
	}
	m_coupleToRoomMap.erase(tmpIt->second.husband);
	m_coupleToRoomMap.erase(tmpIt->second.wife);
	std::map<UINT32, std::set<UINT32> >::iterator it = m_roomToInviteMap.find(roomId);
	std::set<UINT32> s;
	if (it != m_roomToInviteMap.end()) {
		s = it->second;
	}
	m_roomToInviteMap.erase(roomId);
	for (std::set<UINT32>::iterator it = s.begin(); it != s.end(); it++) {
		m_inviteToRoomMap.erase(*it);
	}
	m_coupleMap.erase(roomId);
	delRecord(roomId);
	return true;
}

bool CMarryMgr::inviteFriend(UINT32 roomId, UINT32 friendId) {
    std::map<UINT32, std::set<UINT32> >::iterator secIt = m_roomToInviteMap.find(roomId);
    if (secIt != m_roomToInviteMap.end()) {
        if (secIt->second.insert(friendId).second) {
			m_inviteToRoomMap.insert(std::make_pair(friendId, roomId));
			Athena::logger->trace("CMarryMgr::inviteFriend insert other record");
			return true;
		}
    } else {
        std::set<UINT32> s;
        s.insert(friendId);
        if (m_roomToInviteMap.insert(std::make_pair(roomId, s)).second) {
			m_inviteToRoomMap.insert(std::make_pair(friendId, roomId));
			Athena::logger->trace("CMarryMgr::inviteFriend insert first record");
			return true;
		}
    }
	return false;
}

bool CMarryMgr::acceptInvite(UINT32 inviteId, UINT32 friendId) {
    UINT32 roomId = getRoomId(inviteId);
    if (!roomId) {
        Athena::logger->trace("CMarryMgr::acceptInvite not find room Id");
        return false;
    }
    std::map<UINT32, std::set<UINT32> >::iterator secIt = m_roomToInviteMap.find(roomId);
    if (secIt == m_roomToInviteMap.end()) {
        Athena::logger->trace("CMarryMgr::acceptInvite not find roomId in m_roomToInviteMap");
        return false;
    }
    if ((bool)secIt->second.erase(friendId)) {
		m_inviteToRoomMap.erase(friendId);
		return true;
	}
	return false;
}

void CMarryMgr::addFriendToRoom(UINT32 friendId, UINT32 roomId) {
    if (m_userToRoomMap.insert(std::make_pair(friendId, roomId)).second) {
        std::map<UINT32, std::set<UINT32> >::iterator it = m_inRoomUserMap.find(roomId);
        if (it != m_inRoomUserMap.end()) {
            it->second.insert(friendId);
        } else {
            std::set<UINT32> s;
            s.insert(friendId);
            m_inRoomUserMap.insert(std::make_pair(roomId, s));
        }
        Athena::logger->trace("CMarryMgr::addFriendToRoom success, friendId is %d, roomId is %d",
                friendId, roomId);
    } else {
        Athena::logger->trace("CMarryMgr::addFriendToRoom failed, friendId is %d, roomId is %d",
                friendId, roomId);
    }
}

bool CMarryMgr::friendInRoom(UINT32 friendId) {
    return m_userToRoomMap.find(friendId) != m_userToRoomMap.end();
}

UINT32 CMarryMgr::getRoomIdByInviteId(UINT32 friendId) {
	std::map<UINT32, UINT32>::iterator it = m_inviteToRoomMap.find(friendId);
	if (it != m_inviteToRoomMap.end()) {
		return it->second;
	}
	return 0;
}

bool CMarryMgr::isInvited(UINT32 friendId, UINT32 roomId) {
	std::map<UINT32, UINT32>::iterator it = m_inviteToRoomMap.find(friendId);
	if (it != m_inviteToRoomMap.end()) {
		return it->second == roomId;
	}
	return false;
}

void CMarryMgr::getRoomFriends(UINT32 roomId, std::set<UINT32> & s) {
	std::map<UINT32, std::set<UINT32> >::iterator it =
			m_inRoomUserMap.find(roomId);
	if (it != m_inRoomUserMap.end()) {
		s = it->second;
	}
}

void CMarryMgr::getCouple(UINT32 roomId, UINT32 & husband, UINT32 & wife) {
	std::map<UINT32, MarryInfo >::iterator it =
			m_coupleMap.find(roomId);
	if (it == m_coupleMap.end()) {
		return ;
	}
	husband = it->second.husband;
	wife = it->second.wife;
}

UINT32 CMarryMgr::getRoomIdByUser(UINT32 friendId) {
	std::map<UINT32, UINT32>::iterator it = m_userToRoomMap.find(friendId);
	if (it != m_userToRoomMap.end()) {
		return it->second;
	}
	return 0;
}

void CMarryMgr::quitRoom(UINT32 friendId) {
	std::map<UINT32, UINT32>::iterator it = m_userToRoomMap.find(friendId);
	if (it == m_userToRoomMap.end()) {
		return ;
	}
	UINT32 roomId = it->second;
    m_userToRoomMap.erase(it);
	std::map<UINT32, std::set<UINT32> >::iterator tmpIt = m_inRoomUserMap.find(roomId);
	if (tmpIt == m_inRoomUserMap.end()) {
		return ;
	}
	tmpIt->second.erase(friendId);
}

bool CMarryMgr::getMarryInfo(UINT32 roomId, MarryInfo & marryInfo) {
    std::map<UINT32, MarryInfo>::iterator it = m_coupleMap.find(roomId);
    if (it != m_coupleMap.end()) {
        marryInfo = it->second;
        return true;
    }
    return false;
}

void CMarryMgr::updateMarry(UINT32 roomId, UINT32 charId, UINT32 ringId) {
    std::map<UINT32, MarryInfo>::iterator it = m_coupleMap.find(roomId);
    if (it == m_coupleMap.end()) {
        return ;
    }
    if (charId == it->second.husband) {
        it->second.husRing = ringId;
    } else if (charId == it->second.wife) {
        it->second.wifeRing = ringId;
    } else {
        return ;
    }
    stMarryInfoDB data;
    data.id = it->second.id;
    data.husband = it->second.husband;
    data.wife = it->second.wife;
    data.husRing = it->second.husRing;
    data.wifeRing = it->second.wifeRing;
    data.weddingId = it->second.weddingId;
    updateRecord(data);
}

void CMarryMgr::forceDivorce(UINT32 coupleA, UINT32 coupleB, bool coupleOnline) {
    //房间所有人踢出
    UINT32 roomId = 0;
    std::map<UINT32, UINT32>::iterator it = m_coupleToRoomMap.find(coupleA);
    if (it == m_coupleToRoomMap.end()) {
        it = m_coupleToRoomMap.find(coupleB);
        if (it == m_coupleToRoomMap.end()) {
            return ;
        }
    }
    roomId = it->second;
    std::set<UINT32> inviteSet;
    std::map<UINT32, std::set<UINT32> >::iterator tmpIt = m_roomToInviteMap.find(roomId);
    if (tmpIt != m_roomToInviteMap.end()) {
        inviteSet = tmpIt->second;
    }
    for (std::set<UINT32>::iterator it = inviteSet.begin();
            it != inviteSet.end(); it++) {
        m_inviteToRoomMap.erase(*it);
    }
    std::set<UINT32> inRoomUserSet;
    tmpIt = m_inRoomUserMap.find(roomId);
    if (tmpIt != m_inRoomUserMap.end()) {
        inRoomUserSet = tmpIt->second;
    }
    for (std::set<UINT32>::iterator it = inRoomUserSet.begin();
            it != inRoomUserSet.end(); it++) {
        m_userToRoomMap.erase(*it);
    }
    m_coupleMap.erase(roomId);
    m_coupleToRoomMap.erase(coupleA);
    m_coupleToRoomMap.erase(coupleB);

	Global::stRetForceDivorceCb cmd;
    for (std::set<UINT32>::iterator it = inRoomUserSet.begin();
            it != inRoomUserSet.end(); it++) {
        cmd.add_id(*it);
    }
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);

    delRecord(roomId);

    stRoleCoupleInfoDB data;
    data.charId = coupleA;
    data.coupleId = coupleB;
    updateCoupleRecord(data);

    data.charId = coupleB;
    data.coupleId = coupleA;
    updateCoupleRecord(data);
}

void CMarryMgr::setDivorceTime(UINT32 coupleId, UINT32 time) {
    std::map<UINT32, UINT32>::iterator it = m_coupleToRoomMap.find(coupleId);
    if (it == m_coupleToRoomMap.end()) {
        Athena::logger->trace("setDivorceTime not find roomId for coupleId %d", coupleId);
        return ;
    }
    std::map<UINT32, MarryInfo>::iterator tmpIt = m_coupleMap.find(it->second);
    if (tmpIt == m_coupleMap.end()) {
        Athena::logger->trace("setDivorceTime not find roomId for roomId %d", it->second);
        return ;
    }
    tmpIt->second.expireTime = time;
    stMarryInfoDB data;
    data.id = tmpIt->second.id;
    data.husband = tmpIt->second.husband;
    data.wife = tmpIt->second.wife;
    data.husRing = tmpIt->second.husRing;
    data.wifeRing = tmpIt->second.wifeRing;
    data.weddingId = tmpIt->second.weddingId;
    data.expireTime = tmpIt->second.expireTime;
    updateRecord(data);
}

void CMarryMgr::timer(UINT32 cur) {
    UINT32 roomId = 0, husband = 0, wife = 0;
    UINT32 now = g_globalServer->getSec(), expireTime = 0;
    for (std::map<UINT32, MarryInfo>::iterator it = m_coupleMap.begin();
            it != m_coupleMap.end(); ) {
        expireTime = it->second.expireTime;
        if (!expireTime || expireTime > now) {
            it++;
            continue;
        }
        roomId = it->second.id;
        std::map<UINT32, std::set<UINT32> >::iterator tmpIt = m_roomToInviteMap.find(roomId);
        if (tmpIt != m_roomToInviteMap.end()) {
            std::set<UINT32> & inviteSet = tmpIt->second;
            for (std::set<UINT32>::iterator it = inviteSet.begin();
                    it != inviteSet.end(); it++) {
                m_inviteToRoomMap.erase(*it);
            }
        }
        Global::stRetForceDivorceCb cmd;
        tmpIt = m_inRoomUserMap.find(roomId);
        if (tmpIt != m_inRoomUserMap.end()) {
            std::set<UINT32> & inRoomUserSet = tmpIt->second;
            for (std::set<UINT32>::iterator it = inRoomUserSet.begin();
                    it != inRoomUserSet.end(); it++) {
                cmd.add_id(*it);
                m_userToRoomMap.erase(*it);
            }
        }
        husband = it->second.husband;
        wife = it->second.wife;
        m_coupleToRoomMap.erase(husband);
        m_coupleToRoomMap.erase(wife);
        m_coupleMap.erase(it++);

        delRecord(roomId);
        Player * pUser = PlayerManager::getSingleton().getUserByID(husband);
        if (!pUser) {
            stRoleCoupleInfoDB data;
            data.charId = husband;
            updateCoupleRecord(data);
        } else {
            cmd.add_coupleid(husband);
        }
        stRoleCoupleInfoDB data;
        data.charId = husband;
        updateCoupleRecord(data);
        data.charId = wife;
        updateCoupleRecord(data);
        pUser = PlayerManager::getSingleton().getUserByID(wife);
        if (pUser) {
            cmd.add_coupleid(wife);
        }
        g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
    }
}

void CMarryMgr::delInviteRecord(UINT32 charId, UINT32 roomId) {
    m_inviteToRoomMap.erase(charId);
    std::map<UINT32, std::set<UINT32> >::iterator it =
            m_roomToInviteMap.find(roomId);
    if (it != m_roomToInviteMap.end()) {
        it->second.erase(charId);
        if (it->second.empty()) {
            m_roomToInviteMap.erase(it);
        }
    }
}

void CMarryMgr::synRecord(UINT32 charID, UINT32 coupleId, const std::string & coupleName)
{
	stRoleCoupleInfoDB data;
	data.charId = coupleId;
	data.coupleId = charID;
	strncpy(data.coupleName,coupleName.c_str(), sizeof(data.coupleName));
	updateCoupleRecord(data);
}
