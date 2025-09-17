#include "Mail.h"
#include "Log4cxxHelp.h"
#include "tinyxml/tinyxml.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "PlayerManager.h"
#include "Role.pb.h"
#include "SysLanguages.h"

CTblField g_mailTblDef[] = {
	{ "ID",				CTblField::DB_TYPE_UINT32,			sizeof(UINT32)	 },
	{ "SENDERID",		CTblField::DB_TYPE_UINT32,			sizeof(UINT32)	 },
	{ "SENDERNAME",		CTblField::DB_TYPE_STRING,			MAX_NAMESIZE + 1 },
	{ "RECEIVERID",		CTblField::DB_TYPE_UINT32,			sizeof(UINT32)	 },
	{ "RECEIVERNAME",	CTblField::DB_TYPE_STRING,			MAX_NAMESIZE + 1 },
	{ "TITILE",			CTblField::DB_TYPE_STRING, 	    	MAIL_TILE_LEN },
	{ "CONTENT",		CTblField::DB_TYPE_STRING, 	    	MAIL_CONTENT_LEN },
	{ "ISREAD",         CTblField::DB_TYPE_UINT8,           sizeof(byte)},
	{ "ISFETCHATTACH",  CTblField::DB_TYPE_UINT8,           sizeof(byte)},
	{ "MAILTYPE",		CTblField::DB_TYPE_UINT8,			sizeof(byte)},
	{ "ATTACHID1",      CTblField::DB_TYPE_UINT32,          sizeof(UINT32)   },
	{ "ATTACHNUM1",     CTblField::DB_TYPE_UINT32,          sizeof(UINT32)   },
	{ "ATTACHID2",      CTblField::DB_TYPE_UINT32,          sizeof(UINT32)   },
	{ "ATTACHNUM2",     CTblField::DB_TYPE_UINT32,          sizeof(UINT32)   },
	{ "ATTACHID3",      CTblField::DB_TYPE_UINT32,          sizeof(UINT32)   },
	{ "ATTACHNUM3",     CTblField::DB_TYPE_UINT32,          sizeof(UINT32)   },
	{ "SENDTIME",       CTblField::DB_TYPE_UINT32,          sizeof(UINT32)   },
	{ NULL, 0, 0 },
};

void MailManager::fillSynSnapMailInfo(MiscProto::stSynMainSnalInfo & synInfo,const stMailDBInfo & mailInfo)
{
	synInfo.set_id(mailInfo.m_id);
	synInfo.set_isfetchattach(mailInfo.m_isFetchAttach ? true : false);
	synInfo.set_isread(mailInfo.m_isRead ? true : false);
	synInfo.set_mailtype((MiscProto::eMailType)mailInfo.m_mailType);
	synInfo.set_sendtime(mailInfo.m_sendTime);
	synInfo.set_sendername(mailInfo.m_senderName);
	synInfo.set_title(mailInfo.m_title);
	if ((mailInfo.m_attachID1 > 0 ) || (mailInfo.m_attachID2 > 0) || (mailInfo.m_attachID3 > 0)){
		synInfo.set_hasattach(true);
	}
	else {
		synInfo.set_hasattach(false);
	}
}

void MailManager::fillSynDetailMailInfo(MiscProto::stSynDetailMailInfo & synInfo,const stMailDBInfo & dbInfo)
{
	synInfo.set_id(dbInfo.m_id);
	synInfo.set_sendername(dbInfo.m_senderName);
	synInfo.set_title(dbInfo.m_title);
	synInfo.set_content(dbInfo.m_content);
	synInfo.set_mailtype((MiscProto::eMailType)dbInfo.m_mailType);
	synInfo.set_isfetchattach(dbInfo.m_isFetchAttach? true : false);
	synInfo.set_isread(dbInfo.m_isRead ? true : false);

	if (dbInfo.m_attachID1){
		MiscProto::stSynMailAttach * pAttach = synInfo.add_attachs();
		if (pAttach){
			pAttach->set_itemid(dbInfo.m_attachID1);
			pAttach->set_num(dbInfo.m_attachNum1);
		}
	}
	
	if (dbInfo.m_attachID2){
		MiscProto::stSynMailAttach * pAttach = synInfo.add_attachs();
		if (pAttach){
			pAttach->set_itemid(dbInfo.m_attachID2);
			pAttach->set_num(dbInfo.m_attachNum2);
		}
	}
	
	if (dbInfo.m_attachID3){
		MiscProto::stSynMailAttach * pAttach = synInfo.add_attachs();
		if (pAttach){
			pAttach->set_itemid(dbInfo.m_attachID3);
			pAttach->set_num(dbInfo.m_attachNum3);
		}
	}
}

void MailManager::sendMail(const stMailDBInfo & mailDBInfo,SendMailCB cb)
{
	Athena::CUDSqlCallBack<MailManager,stMailDBInfo,SendMailCB> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<MailManager,stMailDBInfo,SendMailCB>(&MailManager::getSingleton(),&MailManager::sendMailCB,false,0,mailDBInfo,cb);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_mailTblDef,(const char *)&mailDBInfo,"MAIL",autoID,pDelaySql);
}

void MailManager::sendMailCB(bool opResult,UINT64 autoID,stMailDBInfo mailInfo,SendMailCB cb)
{
	if (opResult && cb) {
		cb(autoID);
		MiscProto::stReceiveMail retCmd;
		mailInfo.m_id = autoID;
		fillSynSnapMailInfo(*retCmd.mutable_mailinfo(),mailInfo);
		Player * pUser = PlayerManager::getSingleton().getUserByID(mailInfo.m_receiverID);
		if (pUser){
			pUser->sendProtoToMe(retCmd);	
		}
	}
	else {
		Athena::logger->error("发送邮件失败charID=%d",mailInfo.m_senderID);
	}
}

void g_giveItemByMailCB(UINT64 autoID)
{

}

void MailManager::giveItemByMail(const char * szTitle,const char * szContent,std::vector<std::pair<UINT32,UINT32> > & attaches,UINT32 receiverID)
{
	if (!szTitle || !szContent){
		return ;
	}

	stMailDBInfo mailInfo;
	mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
	mailInfo.m_receiverID = receiverID;
	mailInfo.m_senderID = 0;
	strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
	
	for (UINT8 i = 0;i < attaches.size();++i){
		const std::pair<UINT32,UINT32> & reward = attaches[i];

		if (mailInfo.m_attachID1 <= 0){
			mailInfo.m_attachID1 = reward.first;
			mailInfo.m_attachNum1 = reward.second;
			continue;
		}	
		
		if (mailInfo.m_attachID2 <= 0){
			mailInfo.m_attachID2 = reward.first;
			mailInfo.m_attachNum2 = reward.second;
			continue;
		}	
	
		if (mailInfo.m_attachID3 <= 0){
			mailInfo.m_attachID3 = reward.first;
			mailInfo.m_attachNum3 = reward.second;
			continue;
		}	
	}

	mailInfo.m_isFetchAttach = 0;
	strncpy(mailInfo.m_title,szTitle,sizeof(mailInfo.m_title));
	strncpy(mailInfo.m_content,szContent,sizeof(mailInfo.m_content));
	mailInfo.m_sendTime = g_globalServer->getSec();
	MailManager::getSingleton().sendMail(mailInfo,g_giveItemByMailCB); 
}

void MailManager::userOnline(Player * pUser)
{
	if (!pUser){
		return ;
	}

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " RECEIVERID=%d ", pUser->getID());
	Athena::QueryCallback<MailManager,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<MailManager,UINT32>(&MailManager::getSingleton(),&MailManager::userOnlineCB,NULL,pUser->getID());
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_mailTblDef,"MAIL",where,NULL,0,pDelaySql);
}

void MailManager::userOnlineCB(CTblQueryResult* result,UINT32 charID)
{
	if (!result) {
		return ;
	}	
	
	result->setFieldDef(g_mailTblDef);
	UINT32 recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	
	stMailDBInfo * p = NULL;
	MiscProto::stSynAllMail retCmd;
	if (result->getData()) {
		for (UINT16 i = 0;i < result->getRowCount() ; ++i) {
			p = (stMailDBInfo *)(result->getData() + sizeof(stMailDBInfo) * i);
			MiscProto::stSynMainSnalInfo * pSynInfo = retCmd.add_mailinfos();
			fillSynSnapMailInfo(*pSynInfo,*p);
		}	
	}
	
	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (pUser) {
		pUser->sendProtoToMe(retCmd);
	}
}

void MailManager::readMail(Player *pUser,UINT32 id)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " ID=%d  AND RECEIVERID=%d ", id,pUser->getID());
	Athena::QueryCallback<MailManager,UINT32> * pCallBack = ATHENA_NEW Athena::QueryCallback<MailManager,UINT32>(&MailManager::getSingleton(),&MailManager::readMailCB,NULL,pUser->getID());
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_mailTblDef,"MAIL",where,NULL,0,pDelaySql);
}

CTblField g_readFlagTblDef[] = {
	{ "ISREAD",         CTblField::DB_TYPE_UINT8,           sizeof(byte)},
	{ NULL, 0, 0 },
};



void MailManager::readMailCB(CTblQueryResult* result,UINT32 charID)
{
	MiscProto::stRetMailInfo retCmd;
	if (!result){
		retCmd.mutable_mailinfo()->set_id(0);
		Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
		if (pUser){
			pUser->sendProtoToMe(retCmd);
		}
		return ;
	}

	result->setFieldDef(g_mailTblDef);
	UINT32 recordLen = result->cacRecordLen();
	result->fillData(recordLen);

	stMailDBInfo  * p = NULL;
	if (result->getData()) {
		p = (stMailDBInfo *)result->getData();
		fillSynDetailMailInfo(*retCmd.mutable_mailinfo(),*p);
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
	if (pUser) {
		pUser->sendProtoToMe(retCmd);
	}

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID=%u", retCmd.mailinfo().id());
	Athena::CUDSqlCallBack<MailManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<MailManager>(&MailManager::getSingleton(),&MailManager::updateReadFlagCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT8 readFlag = 1;
	g_databaseEngine.updateRecord(g_readFlagTblDef,(const char *)&readFlag,"MAIL",where,pDelaySql);
}

void MailManager::updateReadFlagCB(bool opResult,UINT64 autoID)
{
	
}

void MailManager::fetchAttach(Player * pUser,UINT32 id)
{
	if (!pUser){
		return ;
	}

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " ID=%d AND RECEIVERID=%d AND ISFETCHATTACH != %d ", id,pUser->getID(),eFetchFlag_Has_Got);
	Athena::QueryCallback<MailManager,UINT32> * pCallBack = NULL;
	pCallBack = ATHENA_NEW Athena::QueryCallback<MailManager,UINT32>(&MailManager::getSingleton(),&MailManager::fetchAttachCB,NULL,pUser->getID());
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_mailTblDef,"MAIL",where,NULL,0,pDelaySql);
}

CTblField g_attachFlagTblDef[] = {
	{ "ISFETCHATTACH",         CTblField::DB_TYPE_UINT8,           sizeof(byte)},
	{ NULL, 0, 0 },
};


void MailManager::fetchAttachCB(CTblQueryResult* result,UINT32 charID)
{
	MiscProto::stFetchAttachResult retCmd;
	if (!result){
		retCmd.set_result(MiscProto::stFetchAttachResult::ATTACH_NOT_EXIST);
		Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
		if (pUser) {
			pUser->sendProtoToMe(retCmd);
		}
		return ;
	}

	result->setFieldDef(g_mailTblDef);
	UINT32 recordLen = result->cacRecordLen();
	result->fillData(recordLen);

	stMailDBInfo  * p = NULL;
	if (result->getData()) { 
		p = (stMailDBInfo *)result->getData();
	}	

	if (p) {
		stAttachInfo attachInfo;
		attachInfo.m_charID = p->m_receiverID;
		attachInfo.m_attachID1 = p->m_attachID1;
		attachInfo.m_attachNum1 = p->m_attachNum1;
		attachInfo.m_attachID2 = p->m_attachID2;
		attachInfo.m_attachNum2 = p->m_attachNum2;
		attachInfo.m_attachID3 = p->m_attachID3;
		attachInfo.m_attachNum3 = p->m_attachNum3;

		char where[65] = { 0 };
		bzero(where, 65);
		snprintf(where, 64, "ID=%u", p->m_id);
		Athena::CUDSqlCallBack<MailManager,stAttachInfo> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<MailManager,stAttachInfo>(&MailManager::getSingleton(),&MailManager::updateAttachFlagCB,false,0,attachInfo);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		UINT8 fetchAttachFlag = eFetchFlag_Has_Got;
		g_databaseEngine.updateRecord(g_attachFlagTblDef,(const char *)&fetchAttachFlag,"MAIL",where,pDelaySql);
		Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
		if (pUser){

			Role::stObtainItemTip itemTipCmd;

			Global::stObtainItemFromMail gameCmd;
			gameCmd.set_charid(pUser->getID());
			if (p->m_attachID1) {
				Global::stMailAttach * pAttach = gameCmd.add_attachs();
				if (pAttach){
					pAttach->set_itemid(p->m_attachID1);
					pAttach->set_num(p->m_attachNum1);
					Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
					if(pSyn)
					{
						pSyn->set_itemid(p->m_attachID1);
						pSyn->set_num(p->m_attachNum1);
					}
				}
			}

			if (p->m_attachID2) {
				Global::stMailAttach * pAttach = gameCmd.add_attachs();
				if (pAttach){
					pAttach->set_itemid(p->m_attachID2);
					pAttach->set_num(p->m_attachNum2);
					Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
					if(pSyn)                              
					{
						pSyn->set_itemid(p->m_attachID2);
						pSyn->set_num(p->m_attachNum2);
					}
				}
			}

			if (p->m_attachID3) {
				Global::stMailAttach * pAttach = gameCmd.add_attachs();
				if (pAttach){
					pAttach->set_itemid(p->m_attachID3);
					pAttach->set_num(p->m_attachNum3);
					Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
					if(pSyn)                              
					{
						pSyn->set_itemid(p->m_attachID3);
						pSyn->set_num(p->m_attachNum3);
					}
				}
			}
			retCmd.set_result(MiscProto::stFetchAttachResult::SUCCESS);
			pUser->sendProtoToMe(retCmd);
			pUser->sendProto2Game(gameCmd);
			pUser->sendProtoToMe(itemTipCmd);
			Athena::logger->trace("[提取附件]玩家(name:%s,id:%d)提取了邮件(id:%d,title:%s,content:%s) 附件有:itemID1:%d,num1:%d,itemID2:%d,num2:%d,itemID3:%d,num3:%d",pUser->getName(),pUser->getID(),p->m_id,p->m_title,p->m_content,p->m_attachID1,p->m_attachNum1,p->m_attachID2,p->m_attachNum2,p->m_attachID3,p->m_attachNum3);
		}
	}
	else {
		Athena::logger->trace("charID=%d提取邮件附件失败,找不到邮",charID);
		retCmd.set_result(MiscProto::stFetchAttachResult::ATTACH_NOT_EXIST);
		Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
		if (pUser) {
			pUser->sendProtoToMe(retCmd);
		}
	}
}

void MailManager::autofetchAttach(Player * pUser,UINT32 id)
{
	if (!pUser){
		return ;
	}

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " ID=%d AND RECEIVERID=%d AND ISFETCHATTACH != %d ", id,pUser->getID(),eFetchFlag_Has_Got);
	Athena::QueryCallback<MailManager,UINT32> * pCallBack = NULL;

	pCallBack = ATHENA_NEW Athena::QueryCallback<MailManager,UINT32 >(&MailManager::getSingleton(),&MailManager::autofetchAttachCB,NULL, pUser->getID());
	CQuerySqlOperation * pDelaySql = ATHENA_NEW CQuerySqlOperation(pCallBack);
	g_databaseEngine.select(g_mailTblDef,"MAIL",where,NULL,0,pDelaySql);
}


void MailManager::autofetchAttachCB(CTblQueryResult* result,UINT32 charID)
{
	if (!result){
		Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
		if (pUser) {
		}
		return ;
	}

	result->setFieldDef(g_mailTblDef);
	UINT32 recordLen = result->cacRecordLen();
	result->fillData(recordLen);
	
	stMailDBInfo  * p = NULL;
	if (result->getData()) {
		p = (stMailDBInfo *)result->getData();
	}

	if (p) {
		Role::stObtainItemTip itemTipCmd;
		stAttachInfo attachInfo;
		attachInfo.m_charID = p->m_receiverID;
		attachInfo.m_attachID2 = p->m_attachID2;
		attachInfo.m_attachNum2 = p->m_attachNum2;
		attachInfo.m_attachID3 = p->m_attachID3;
		attachInfo.m_attachNum3 = p->m_attachNum3;

		char where[65] = { 0 };
		bzero(where, 65);
		snprintf(where, 64, "ID=%u", p->m_id);
		Athena::CUDSqlCallBack<MailManager,stAttachInfo> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<MailManager,stAttachInfo>(&MailManager::getSingleton(),&MailManager::updateAttachFlagCB,false,0,attachInfo);
		CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
		UINT8 fetchAttachFlag = eFetchFlag_Has_Got;
		g_databaseEngine.updateRecord(g_attachFlagTblDef,(const char *)&fetchAttachFlag,"MAIL",where,pDelaySql);
		Player * pUser = PlayerManager::getSingleton().getUserByID(charID);
		if (pUser){
			Global::stObtainItemFromMail gameCmd;
			gameCmd.set_charid(pUser->getID());
			if (p->m_attachID1) {
				Global::stMailAttach * pAttach = gameCmd.add_attachs();
				if (pAttach){
					pAttach->set_itemid(p->m_attachID1);
					pAttach->set_num(p->m_attachNum1);
					
					Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
					{
						pSyn->set_itemid(p->m_attachID1);
						pSyn->set_num(p->m_attachNum1);
					}
				}
			}

			if (p->m_attachID2) {
				Global::stMailAttach * pAttach = gameCmd.add_attachs();
				if (pAttach){
					pAttach->set_itemid(p->m_attachID2);
					pAttach->set_num(p->m_attachNum2);
					
					Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
					if(pSyn)
					{
						pSyn->set_itemid(p->m_attachID2);
						pSyn->set_num(p->m_attachNum2);
					}
				}
			}

			if (p->m_attachID3) {
				Global::stMailAttach * pAttach = gameCmd.add_attachs();
				if (pAttach){
					pAttach->set_itemid(p->m_attachID3);
					pAttach->set_num(p->m_attachNum3);
					
					Role::stSynTipItem * pSyn = itemTipCmd.add_getitems();
					if(pSyn)
					{
						pSyn->set_itemid(p->m_attachID3);
						pSyn->set_num(p->m_attachNum3);
					}
				}
			}
			pUser->sendProtoToMe(itemTipCmd);
			pUser->sendProto2Game(gameCmd);
			Athena::logger->trace("[一键提取附件]玩家(name:%s,id:%d)提取了邮件(id:%d,title:%s,content:%s) 附件有:itemID1:%d,num1:%d,itemID2:%d,num2:%d,itemID3:%d,num3:%d",pUser->getName(),pUser->getID(),p->m_id,p->m_title,p->m_content,p->m_attachID1,p->m_attachNum1,p->m_attachID2,p->m_attachNum2,p->m_attachID3,p->m_attachNum3);
		}
	}
	else {
		Athena::logger->trace("charID=%d,一键提取邮件附件失败,找不到邮件",charID);
	}
}

void MailManager::updateAttachFlagCB(bool opResult,UINT64 autoID,stAttachInfo attachInfo)
{
	if (opResult) {
		Player * pUser = PlayerManager::getSingleton().getUserByID(attachInfo.m_charID);
		if (pUser) {
			Athena::logger->trace("(id=%d,name=%s)领取邮件附件成功",pUser->getID(),pUser->getName());
		}
	}
}

void MailManager::deleteOutdateMail()
{
	UINT32 outdateTime = g_globalServer->getSec() - 86400 * 7;
	UINT32 readOutdateTime = g_globalServer->getSec() - 86400;
	UINT32 shortOutdateTime = g_globalServer->getSec() - 86400;
	char where[150] = { 0 };
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where), " (SENDTIME < %d) OR (ATTACHID1=0 AND ATTACHID2=0 AND ATTACHID3=0 AND SENDTIME < %d) OR (MAILTYPE = 2 AND SENDTIME < %d) ", outdateTime,readOutdateTime,shortOutdateTime);
	g_databaseEngine.deleteRecord("MAIL",where,NULL);
}

CTblField g_delReadMailTblDef[] = {
	{ "ID",         CTblField::DB_TYPE_UINT32,           sizeof(UINT32)},
	{ NULL, 0, 0 },
};

void MailManager::deleteHasReadMail(UINT32 charID)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, " RECEIVERID=%d AND ISREAD=1 AND ISFETCHATTACH =%d ", charID,eFetchFlag_Has_Got);
	Athena::CUDSqlCallBack<MailManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<MailManager>(this,&MailManager::deleteHasReadMailCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("MAIL",where,pDelaySql);
}
	
void MailManager::selectHasReadMailCB(CTblQueryResult* result,UINT32 charID)
{
	if (!result) {
		return ;
	}

	result->setFieldDef(g_delReadMailTblDef);
	UINT32 recordLen = result->cacRecordLen();
	result->fillData(recordLen);
}

void MailManager::deleteHasReadMailCB(bool opResult,UINT64 autoID)
{

}

void MailManager::deleteSelMail(const char *szWhere)
{
	Athena::CUDSqlCallBack<MailManager> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<MailManager>(&MailManager::getSingleton(),&MailManager::deleteSelMailCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord(" `MAIL` ",szWhere,pDelaySql);
}


void MailManager::deleteSelMailCB(bool opResult,UINT64 autoID)
{
	
}

void g_batchSendMailCB(UINT64 autoID)
{}

CBatchSendMailMgr::~CBatchSendMailMgr()
{

}

CBatchSendMailMgr::CBatchSendMailMgr()
{
	
}
void CBatchSendMailMgr::addTask(const stBatchSendMailTask & task)
{
	m_allTasks.push_back(task);
}

void CBatchSendMailMgr::sendMail(const stBatchSendMailTask & task,UINT32 receiverID)
{
	stMailDBInfo mailInfo;
	mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
	mailInfo.m_receiverID = receiverID;
	mailInfo.m_senderID = 0;
	strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
	
	for (UINT8 i = 0;i < task.m_attaches.size();++i){
		const std::pair<UINT32,UINT32> & reward = task.m_attaches[i];

		if (mailInfo.m_attachID1 <= 0){
			mailInfo.m_attachID1 = reward.first;
			mailInfo.m_attachNum1 = reward.second;
			continue;
		}	
		
		if (mailInfo.m_attachID2 <= 0){
			mailInfo.m_attachID2 = reward.first;
			mailInfo.m_attachNum2 = reward.second;
			continue;
		}	
	
		if (mailInfo.m_attachID3 <= 0){
			mailInfo.m_attachID3 = reward.first;
			mailInfo.m_attachNum3 = reward.second;
			continue;
		}	
	}

	mailInfo.m_isFetchAttach = 0;
	strncpy(mailInfo.m_title,task.m_title.c_str(),sizeof(mailInfo.m_title));
	strncpy(mailInfo.m_content,task.m_content.c_str(),sizeof(mailInfo.m_content));
	mailInfo.m_sendTime = g_globalServer->getSec();
	MailManager::getSingleton().sendMail(mailInfo,g_batchSendMailCB); 
}

void CBatchSendMailMgr::timer(UINT32 cur)
{
	if (m_allTasks.size() <= 0){
		return ;
	}

	UINT16 sendNum = 0;	
	std::list<stBatchSendMailTask>::iterator it = m_allTasks.begin();	

	for (;it != m_allTasks.end();){
		std::set<UINT32>::iterator subIt = it->m_receiveIDs.begin();
		for (;subIt != it->m_receiveIDs.end();){
			++sendNum;
			std::set<UINT32>::iterator subTempIt = subIt;
			++subIt;
			sendMail(*it,*subTempIt);
			(*it).m_receiveIDs.erase(subTempIt);
			if (sendNum >= s_SENDMAIL_NUM_PER_TIME){
				break;
			}
		}

		if (it->m_receiveIDs.size() <= 0){
			std::list<stBatchSendMailTask>::iterator tmpIt = it;
			++it;
			m_allTasks.erase(tmpIt);
			continue;
		}
		else {
			break;
		}
	}
}
