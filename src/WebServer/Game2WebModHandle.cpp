#include "Game2WebModHandle.h"
#include "SvrInfo.h"
#include "WebServer.h"
#include "UserLogMgr.h"
#include "TblDataMgr.h"
#include "HttpParser.h"
#include "json/writer.h"
#include "WebUserConnMgr.h"
#include "RoleInfoManager.h"
#include "GameSvrClientMgr.h"

extern WebServer * g_webServer;

Game2WebModHandle::Game2WebModHandle()
{
	registerHandle(&Game2WebModHandle::userLevelupLog);
	registerHandle(&Game2WebModHandle::userLoginLog);
	registerHandle(&Game2WebModHandle::vipLevelupLog);
	registerHandle(&Game2WebModHandle::honourChgLog);
	registerHandle(&Game2WebModHandle::itemChgLog);
	registerHandle(&Game2WebModHandle::resourceChgLog);
	registerHandle(&Game2WebModHandle::dailyTaskLog);
	registerHandle(&Game2WebModHandle::publishActivityResult);
	registerHandle(&Game2WebModHandle::unloadActivityResult);
	registerHandle(&Game2WebModHandle::recharegeLog);
	registerHandle(&Game2WebModHandle::mallBuyLog);
	registerHandle(&Game2WebModHandle::heroPropChgLog);
	registerHandle(&Game2WebModHandle::guildLog);
	registerHandle(&Game2WebModHandle::guildResLog);
	registerHandle(&Game2WebModHandle::moduleLog);
	registerHandle(&Game2WebModHandle::skillChgLog);
	registerHandle(&Game2WebModHandle::spriteLog);
	registerHandle(&Game2WebModHandle::copymapLog);
	registerHandle(&Game2WebModHandle::equipLog);
	registerHandle(&Game2WebModHandle::activityLog);
	registerHandle(&Game2WebModHandle::userCancelResult);
	registerHandle(&Game2WebModHandle::reqPayType);
}

Game2WebModHandle::~Game2WebModHandle()
{

}

bool Game2WebModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<Game2WebModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void Game2WebModHandle::userLevelupLog(TCPComponent * pCom,Game::stUserLevelupLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	stUserLevelupLogDB data;
	data.m_charID = recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	data.m_newExp = recvCmd->newexp();
	data.m_oldExp = recvCmd->oldexp();
	data.m_newLv = recvCmd->newlv();
	data.m_oldLv = recvCmd->oldlv();
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_levelupTime,timestamp.c_str(),sizeof(data.m_levelupTime));
	UserLogMgr::getSingleton().addUserLevelupLog(data);
}


void Game2WebModHandle::userLoginLog(TCPComponent * pCom,Game::stUserLoginLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stUserLoginLogDB data;
	data.m_charID = recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	strncpy(data.m_loginIP,recvCmd->ip().c_str(),sizeof(data.m_loginIP));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_loginTime,timestamp.c_str(),sizeof(data.m_loginTime));
	data.m_adUser = recvCmd->isadvuser();
	data.m_opID = recvCmd->opid();
	strncpy(data.m_createTime,recvCmd->createtime().c_str(),sizeof(data.m_createTime));
	strncpy(data.m_deviceID,recvCmd->deviceid().c_str(),sizeof(data.m_deviceID));
	UserLogMgr::getSingleton().addLoginLog(data);
	
	if (recvCmd->isenterselectlog()) {
		stEnterSelectPageLogDB dbData;
		strncpy(dbData.m_account,recvCmd->account().c_str(),sizeof(dbData.m_account));
		strncpy(dbData.m_deviceID,recvCmd->deviceid().c_str(),sizeof(dbData.m_deviceID));
		dbData.m_opID = recvCmd->opid();
		strncpy(dbData.m_doTime,timestamp.c_str(),sizeof(dbData.m_doTime));
		UserLogMgr::getSingleton().addEnterSelectPageLog(dbData);
	}
}

void Game2WebModHandle::vipLevelupLog(TCPComponent * pCom,Game::stVipLevelupLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	stUserVipLevelupLogDB  data;
	data.m_charID = recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	data.m_addDiamond = recvCmd->adddiamond();
	data.m_newExp = recvCmd->newexp();
	data.m_oldExp = recvCmd->oldexp();
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_levelupTime,timestamp.c_str(),sizeof(data.m_levelupTime));
	UserLogMgr::getSingleton().addVipLevelupLog(data);

}

void Game2WebModHandle::honourChgLog(TCPComponent * pCom,Game::stHonourChgLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stHonourChgLogDB  data;
	data.m_charID = recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	data.m_newHonour = recvCmd->newhonour();
	data.m_oldHonour = recvCmd->oldhonour();
	strncpy(data.m_reason,recvCmd->reason().c_str(),sizeof(data.m_reason));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_chgTime,timestamp.c_str(),sizeof(data.m_chgTime));
	UserLogMgr::getSingleton().addHonourChgLog(data);
}

void Game2WebModHandle::itemChgLog(TCPComponent * pCom,Game::stItemChgLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	stItemChgLogDB  data;
	data.m_charID = recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	stItemBase * pItemBase = g_itemBaseMgr.get(recvCmd->itemid());
	if (pItemBase) {
		strncpy(data.m_itemName,pItemBase->m_name,sizeof(data.m_itemName));
		data.m_quality = pItemBase->m_quality;
		data.m_type = pItemBase->m_type;
	}
	data.m_num = recvCmd->num();
	if (recvCmd->isadd()){
		data.m_isAdd = 1;
	}
	else {
		data.m_isAdd = 0;
	}
	data.m_raiseLv = recvCmd->raiselv();
	data.m_refineLv = recvCmd->refinelv();
	data.m_userLv = recvCmd->userlv();
	data.m_vipLv = recvCmd->viplv();
	strncpy(data.m_reason,recvCmd->reason().c_str(),sizeof(data.m_reason));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_chgTime,timestamp.c_str(),sizeof(data.m_chgTime));
	UserLogMgr::getSingleton().addItemChgLog(data);
}

void Game2WebModHandle::resourceChgLog(TCPComponent * pCom,Game::stResourceChgLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	stResChgLogDB  data;
	data.m_charID = recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	data.m_addNum = recvCmd->addnum();
	data.m_newValue = recvCmd->newvalue();
	data.m_oldValue = recvCmd->oldvalue();
	strncpy(data.m_reason,recvCmd->reason().c_str(),sizeof(data.m_reason));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_chgTime,timestamp.c_str(),sizeof(data.m_chgTime));
	stItemBase * pItemBase =  g_itemBaseMgr.get(recvCmd->resid());
	if (pItemBase){
		strncpy(data.m_resName,pItemBase->m_name,sizeof(data.m_resName));
	}
	UserLogMgr::getSingleton().addResChgLog(data);
	
	if (recvCmd->oldvalue() > recvCmd->newvalue()) {
		if ((recvCmd->resid() == eResource_GoldCoin) || (recvCmd->resid() == eResource_Diamond)){
			stExchangeListDB data1;
			data1.m_charID = recvCmd->charid();
			strncpy(data1.m_account,recvCmd->account().c_str(),sizeof(data1.m_account));
			strncpy(data1.m_name,recvCmd->name().c_str(),sizeof(data1.m_name));
			data1.m_flag = recvCmd->resid();
			data1.m_num = recvCmd->oldvalue() - recvCmd->newvalue();
			strncpy(data1.m_doTime,timestamp.c_str(),sizeof(data1.m_doTime));
			strncpy(data1.m_moduleName,recvCmd->reason().c_str(),sizeof(data1.m_moduleName));
			UserLogMgr::getSingleton().addExchangeLog(data1);
		}
	}
}

void Game2WebModHandle::dailyTaskLog(TCPComponent * pCom,Game::stDailyTaskLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stDailyTaskLogDB  data;
	data.m_charID = recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	data.m_taskID = recvCmd->taskid();
	strncpy(data.m_taskName,recvCmd->taskname().c_str(),sizeof(data.m_taskName));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_doTime,timestamp.c_str(),sizeof(data.m_doTime));
	UserLogMgr::getSingleton().addDailyTaskLog(data);
}

void Game2WebModHandle::recharegeLog(TCPComponent * pCom,Game::stRechargeLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stUserRechargeLogDB data;
	data.m_charID = recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	data.m_nowDiamond = recvCmd->diamondnum();
	data.m_rechargeNum = recvCmd->rechargenum();
	strncpy(data.m_tid,recvCmd->tid().c_str(),sizeof(data.m_tid));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_rechargeTime,timestamp.c_str(),sizeof(data.m_rechargeTime));
	if (recvCmd->newrecharge()){
		data.m_newRecharge = 1;
	}
	else {
		data.m_newRecharge = 0;
	}
	data.m_opID = recvCmd->opid();
	data.m_addDiamond = recvCmd->adddiamond();
	data.m_itemID = recvCmd->itemid();
	data.m_viLv = recvCmd->itemid();
	UserLogMgr::getSingleton().addRechargeLog(data);
}

void Game2WebModHandle::mallBuyLog(TCPComponent * pCom,Game::stMallBuyLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stMallBuyLogDB data;
	data.m_charID =recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_doTime,timestamp.c_str(),sizeof(data.m_doTime));
	data.m_itemID = recvCmd->itemid();
	data.m_itemNum = recvCmd->itemnum();
	data.m_level = recvCmd->level();
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	data.m_resID = recvCmd->resid();
	data.m_resNum = recvCmd->resnum();
	data.m_shopID = recvCmd->shopid();
	data.m_vipLv = recvCmd->viplv();
	UserLogMgr::getSingleton().addMallBuyLog(data);
}

void Game2WebModHandle::heroPropChgLog(TCPComponent * pCom,Game::stHeroPropChgLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stHeroPropChgLogDB data;
	data.m_charID =recvCmd->charid();
	strncpy(data.m_account,recvCmd->account().c_str(),sizeof(data.m_account));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_doTime,timestamp.c_str(),sizeof(data.m_doTime));
	data.m_level = recvCmd->level();
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));
	data.m_heroID = recvCmd->heroid();
	if (1 == recvCmd->action()){
		strncpy(data.m_action,"升级",sizeof(data.m_action));
	}
	else if (2 == recvCmd->action()){
		strncpy(data.m_action,"进化",sizeof(data.m_action));
	}
	else if (3 == recvCmd->action()){
		strncpy(data.m_action,"领悟",sizeof(data.m_action));
	}
	else if (4 == recvCmd->action()){
		strncpy(data.m_action,"10连领悟",sizeof(data.m_action));
	}
	data.m_oldLv = recvCmd->oldlv();
	data.m_newLv = recvCmd->newlv();
	data.m_vipLv = recvCmd->viplv();
	data.m_evoNewLv = recvCmd->viplv();
	data.m_evoOldLv = recvCmd->oldevolv();
	data.m_evoNewLv = recvCmd->newevolv();
	UserLogMgr::getSingleton().addHeroPropChgLog(data);

}

void Game2WebModHandle::skillChgLog(TCPComponent * pCom,Game::stSkillChgLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stSkillChgLogDB data;
	data.m_skillID = recvCmd->skillid();
	data.m_decGoldcoin = recvCmd->decgoldcoind();
	if (1 == recvCmd->action()){
		strncpy(data.m_action,"升级玩家技能",sizeof(data.m_action));
	}
	else if (2 == recvCmd->action()){
		strncpy(data.m_action,"升级领悟技能",sizeof(data.m_action));
	}
	else if (3 == recvCmd->action()){
		strncpy(data.m_action,"升级稀有技能",sizeof(data.m_action));
	}
	else if (4 == recvCmd->action()){
		strncpy(data.m_action,"升级觉醒技能",sizeof(data.m_action));
	}
	data.m_newLv = recvCmd->newlv();
	data.m_oldLv = recvCmd->oldlv();
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_doTime,timestamp.c_str(),sizeof(data.m_doTime));
	data.m_pos = recvCmd->pos();
	UserLogMgr::getSingleton().addSkillChgLog(data);
}

void Game2WebModHandle::spriteLog(TCPComponent * pCom,Game::stSpriteLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stSpriteLogDB data;
	data.m_consume = recvCmd->consume();
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_doTime,timestamp.c_str(),sizeof(data.m_doTime));
	data.m_newLv = recvCmd->newlv();
	data.m_oldLv = recvCmd->oldlv();
	data.m_spriteID = recvCmd->spriteid();
	UserLogMgr::getSingleton().addSpriteLog(data);
}

void Game2WebModHandle::copymapLog(TCPComponent * pCom,Game::stCopymapLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stCopymapLogDB data;
	strncpy(data.m_action,recvCmd->action().c_str(),sizeof(data.m_action));
	data.m_copymapID = recvCmd->copymapid();
	data.m_win = recvCmd->win();
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_doTime,timestamp.c_str(),sizeof(data.m_doTime));
	UserLogMgr::getSingleton().addCopymapLog(data);
}

void Game2WebModHandle::equipLog(TCPComponent * pCom,Game::stEquipChgLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stEquipChgLogDB data;
	strncpy(data.m_action,recvCmd->action().c_str(),sizeof(data.m_action));
	data.m_addGoldcoin = recvCmd->addgoldcoin();
	data.m_addItemID = recvCmd->additemid();
	data.m_addYuntie = recvCmd->addyuntie();
	data.m_decGoldcoin = recvCmd->decgoldcoin();
	data.m_decItemID = recvCmd->decitemid();
	data.m_decYuntie = recvCmd->decyuntie();
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(data.m_doTime,timestamp.c_str(),sizeof(data.m_doTime));
	data.m_itemID = recvCmd->itemid();
	data.m_newRaiseLv = recvCmd->newraiselv();
	data.m_newRefineLv = recvCmd->newrefinelv();
	data.m_oldRaiseLv = recvCmd->oldraiselv();
	data.m_oldRefineLv = recvCmd->oldrefinelv();
	UserLogMgr::getSingleton().addEquipChgLog(data);
}

void Game2WebModHandle::publishActivityResult(TCPComponent * pCom,Game::stPublishActivityResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Json::FastWriter writer;
	Json::Value root;
	if (recvCmd->result()){
		root["status"] = 1;
		root["data"] = "发布成功";
	}
	else {
		root["status"] = 0;
		root["data"] = "发布失败";
	}
	
	std::string content = writer.write(root); 
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了发布任务任务的返回:%s",ret.c_str());
#endif
}

void Game2WebModHandle::unloadActivityResult(TCPComponent * pCom,Game::stUnloadActivityResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	Json::FastWriter writer;
	Json::Value root;
	if (recvCmd->result()){
		root["status"] = 1;
		root["data"] = "卸载活动成功";
	}
	else {
		root["status"] = 0;
		root["data"] = "卸载活动失败";
	}
	
	std::string content = writer.write(root); 
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
}

void Game2WebModHandle::guildLog(TCPComponent * pCom,Game::stSendGuildLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stGuildLogDB dbData;
	strncpy(dbData.m_account,recvCmd->account().c_str(),sizeof(dbData.m_account));
	strncpy(dbData.m_action,recvCmd->action().c_str(),sizeof(dbData.m_action));
	dbData.m_charID = recvCmd->charid();
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(dbData.m_doTime,timestamp.c_str(),sizeof(dbData.m_doTime));
	dbData.m_guildID = recvCmd->guildid();
	dbData.m_level  =  recvCmd->level();
	dbData.m_opID = recvCmd->opid();
	strncpy(dbData.m_opObjID,recvCmd->opobjid().c_str(),sizeof(dbData.m_opObjID));
	UserLogMgr::getSingleton().addGuildLog(dbData);
}

void Game2WebModHandle::guildResLog(TCPComponent * pCom,Game::sstSendGuildResLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stGuildResLogDB dbData;
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(dbData.m_doTime,timestamp.c_str(),sizeof(dbData.m_doTime));
	dbData.m_guildID = recvCmd->guildid();
	dbData.m_newVal = recvCmd->newval();
	dbData.m_oldVal = recvCmd->oldval();
	strncpy(dbData.m_reason,recvCmd->reason().c_str(),sizeof(dbData.m_reason));
	strncpy(dbData.m_res,recvCmd->res().c_str(),sizeof(dbData.m_res));
	UserLogMgr::getSingleton().addGuildResLog(dbData);
}

void Game2WebModHandle::moduleLog(TCPComponent * pCom,Game::stSendModuleLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	stModuleLogDB dbData;
	strncpy(dbData.m_account,recvCmd->account().c_str(),sizeof(dbData.m_account));
	dbData.m_charID = recvCmd->charid();
	dbData.m_copymapID = recvCmd->copymapid();
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(dbData.m_doTime,timestamp.c_str(),sizeof(dbData.m_doTime));
	dbData.m_level = recvCmd->level();
	strncpy(dbData.m_module,recvCmd->module().c_str(),sizeof(dbData.m_module));
	dbData.m_newRank = recvCmd->newrank();
	dbData.m_oldRank = recvCmd->oldrank();
	dbData.m_opID = recvCmd->opid();
	if (recvCmd->result()){
		dbData.m_result = 1;
	}
	dbData.m_vipLv = recvCmd->viplv();
	strncpy(dbData.m_otherCharID,recvCmd->othercharid().c_str(),sizeof(dbData.m_otherCharID));
	UserLogMgr::getSingleton().addModuleLog(dbData);
}

void Game2WebModHandle::activityLog(TCPComponent * pCom,Game::stActivityLog * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stActivityLogDB dbData;
	dbData.m_charID = recvCmd->charid();
	dbData.m_activityID = recvCmd->activityid();
	dbData.m_stepID = recvCmd->stepid();
	strncpy(dbData.m_name,recvCmd->activityname().c_str(),sizeof(dbData.m_name));
	strncpy(dbData.m_desc,recvCmd->desc().c_str(),sizeof(dbData.m_desc));
	std::string timestamp = g_utc2StringFormat(g_webServer->getSec());
	strncpy(dbData.m_doTime,timestamp.c_str(),sizeof(dbData.m_doTime));
	UserLogMgr::getSingleton().addActivityLog(dbData);
}

void Game2WebModHandle::userCancelResult(TCPComponent * pCom, Game::stUserCancelRes * recvCmd) {
	if (!recvCmd){
		return ;
	}

	Json::FastWriter writer;
	Json::Value root;
	if (recvCmd->res().empty()){
		root["return_code"] = 1;
		root["return_msg"] = std::string("");
	}
	else {
		root["return_code"] = 0;
		root["return_msg"] = recvCmd->res();
	}
	
	std::string content = writer.write(root); 
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了玩家删除的返回:%s",ret.c_str());
#endif
}

void Game2WebModHandle::reqPayType(TCPComponent * pCom, Game::stReqPayType * recvCmd) {
	if (!recvCmd){
		return ;
	}
    stOrderInfo orderInfo;
    orderInfo.isExtend = recvCmd->isextend();
    orderInfo.data = recvCmd->data();
    orderInfo.order = recvCmd->orderid();
    orderInfo.itemId = recvCmd->itemid();
    bool res = RoleInfoManager::getSingleton().addOrderId(
            recvCmd->charid(), orderInfo);
    Game::stReqPayTypeCb cmd;
    cmd.set_charid(recvCmd->charid());
    cmd.set_itemid(recvCmd->itemid());
    cmd.set_orderid(recvCmd->orderid());
    cmd.set_paytype(recvCmd->paytype());
    cmd.set_res(res);
    GameSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}
