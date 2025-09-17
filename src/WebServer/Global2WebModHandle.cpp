#include "Global2WebModHandle.h"
#include "SvrInfo.h"
#include "WebServer.h"
#include "HttpParser.h"
#include "json/writer.h"
#include "WebUserConnMgr.h"
#include "KoreaHttpInf.h"
#include "UserLogMgr.h"
#include "TblDataMgr.h"
#include "LimitPetMgr.h"

extern WebServer * g_webServer;

Global2WebModHandle::Global2WebModHandle()
{
	registerHandle(&Global2WebModHandle::kickUserOutResult);
	registerHandle(&Global2WebModHandle::setGM);
	registerHandle(&Global2WebModHandle::banUser);
	registerHandle(&Global2WebModHandle::sendMailResult);
	registerHandle(&Global2WebModHandle::retSvrCfg);
	registerHandle(&Global2WebModHandle::updateOnlineNum);
	registerHandle(&Global2WebModHandle::enterSelectPage);
	registerHandle(&Global2WebModHandle::onCreateRole);
	registerHandle(&Global2WebModHandle::guildLog);
	registerHandle(&Global2WebModHandle::guildResLog);
	registerHandle(&Global2WebModHandle::moduleLog);
	registerHandle(&Global2WebModHandle::modifyUserInfo);
	registerHandle(&Global2WebModHandle::retGetTxGiftBag);
	registerHandle(&Global2WebModHandle::resourceChgLog);
	registerHandle(&Global2WebModHandle::useInviteCodeCb);
	registerHandle(&Global2WebModHandle::reqLimitPetActivityData);
	registerHandle(&Global2WebModHandle::queryNftResult);
	registerHandle(&Global2WebModHandle::changeNftItemResult);
}

Global2WebModHandle::~Global2WebModHandle()
{

}

bool Global2WebModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<Global2WebModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void Global2WebModHandle::kickUserOutResult(TCPComponent * pCom,Global::stKillUserOutResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	if (recvCmd->result()){
		root["data"] = "成功!";
	}
	else {
		root["data"] = "失败!";
	}
	std::string content = writer.write(root);
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
	Athena::logger->trace("踢人返回!");
}

void Global2WebModHandle::setGM(TCPComponent * pCom,Global::stSetUserGMTypeResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	if (recvCmd->result()){
		root["data"] = "成功!";
	}
	else {
		root["data"] = "失败!";
	}
	std::string content = writer.write(root);
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
	Athena::logger->trace("设置GM返回!");
}

void Global2WebModHandle::banUser(TCPComponent * pCom,Global::stBanUserResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	if (recvCmd->result()){
		root["data"] = "成功!";
	}
	else {
		root["data"] = "失败!";
	}
	std::string content = writer.write(root);
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
	Athena::logger->trace("封号解封返回!");
}

void Global2WebModHandle::sendMailResult(TCPComponent * pCom,Global::stSendMail2UserResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Json::FastWriter writer;
	Json::Value root;
	root["status"] = 1;
	Json::Value data;

	//data.append(1);
	//data.append(2);

	root["data"] = data;
	root["tag"] = recvCmd->tag();

	std::string content = writer.write(root);
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送邮件返回是:%s",ret.c_str());
#endif
}

void Global2WebModHandle::retSvrCfg(TCPComponent * pCom,Global::stRetSvrConfig * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Json::FastWriter writer;
	Json::Value root;
	Json::Value configData;

	root["return_code"] =  0;
	root["return_msg"] = "成功!";
	
	for (UINT16 i = 0;i < recvCmd->cfgpair_size();++i){
		const Global::stServerCfgPair & cfgPair = recvCmd->cfgpair(i);
		configData[cfgPair.key()] = cfgPair.value();
	}
	
	root["config_data"] = configData;
	std::string content = writer.write(root);
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("给玩家返回的值:%s",ret.c_str());
#endif
}

void Global2WebModHandle::updateOnlineNum(TCPComponent * pCom,Global::stNotifyWebOnlineNum * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	CKoreaHttpInfMgr::getSingleton().updateOnline(recvCmd->num());	
}
	
void Global2WebModHandle::enterSelectPage(TCPComponent * pCom,Global::stEnterSelectPage * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	stEnterSelectPageLogDB dbData;
	strncpy(dbData.m_account,recvCmd->account().c_str(),sizeof(dbData.m_account));
	strncpy(dbData.m_deviceID,recvCmd->deviceid().c_str(),sizeof(dbData.m_deviceID));
	dbData.m_opID = recvCmd->opid();
	std::string curTimeStr = g_utc2StringFormat(g_webServer->getSec());
	strncpy(dbData.m_doTime,curTimeStr.c_str(),sizeof(dbData.m_doTime));
	UserLogMgr::getSingleton().addEnterSelectPageLog(dbData);
}

void Global2WebModHandle::onCreateRole(TCPComponent * pCom,Global::stNotifyWebCreateRole * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	UserLogMgr::getSingleton().updateCreateRoleTime(recvCmd->account());
	stCreateRoleLogDB dbData;
	strncpy(dbData.m_account,recvCmd->account().c_str(),sizeof(dbData.m_account));
	strncpy(dbData.m_deviceID,recvCmd->account().c_str(),sizeof(dbData.m_deviceID));
	std::string curTimeStr = g_utc2StringFormat(g_webServer->getSec());
	strncpy(dbData.m_doTime,curTimeStr.c_str(),sizeof(dbData.m_doTime));
	dbData.m_opID = recvCmd->opid();
	UserLogMgr::getSingleton().addCreateRoleLog(dbData);
}
	
void Global2WebModHandle::guildLog(TCPComponent * pCom,Global::stSendGuildLog * recvCmd)
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
	strncpy(dbData.m_name,recvCmd->name().c_str(),sizeof(dbData.m_name));
	strncpy(dbData.m_opObjID,recvCmd->opobjid().c_str(),sizeof(dbData.m_opObjID));
	UserLogMgr::getSingleton().addGuildLog(dbData);
}

void Global2WebModHandle::guildResLog(TCPComponent * pCom,Global::sstSendGuildResLog * recvCmd)
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

void Global2WebModHandle::moduleLog(TCPComponent * pCom,Global::stSendModuleLog * recvCmd)
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

void Global2WebModHandle::modifyUserInfo(TCPComponent * pCom,Global::stWebMotifyUserInfoRet * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	Json::FastWriter writer;
	Json::Value root;
	Json::Value configData;

	root["status"] =  1;
	root["data"] = "修改成功!";
	std::string content = writer.write(root);
	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();
	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("Global2WebModHandle::modifyUserInfo 给玩家返回的值:%s",ret.c_str());
#endif
}

void Global2WebModHandle::retGetTxGiftBag(TCPComponent * pCom,Global::stRetGetTxGiftBag * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Json::FastWriter writer;
	Json::Value root;
	
	root["ret"] = recvCmd->result();
	root["msg"] = recvCmd->msg();

	std::string content = writer.write(root);

	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();

	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
}

void Global2WebModHandle::resourceChgLog(TCPComponent * pCom, Global::stResourceChgLog * recvCmd) {
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

void Global2WebModHandle::useInviteCodeCb(TCPComponent * pCom, Global::stUseInviteCodeCb * recvCmd) {
	if (!recvCmd){
		return ;
	}
	Json::FastWriter writer;
	Json::Value root;
	
    if (recvCmd->msg() == std::string("ok")) {
        root["return_code"] = std::string("0");
    } else {
        root["return_code"] = std::string("1");
    }
	root["return_msg"] = recvCmd->msg();
    root["user_account"] = recvCmd->account();
    root["user_id"] = recvCmd->userid();

	std::string content = writer.write(root);

	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();

	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("转发stUseInviteCodeCb返回, content is %s!", content.c_str());
#endif
}

void Global2WebModHandle::reqLimitPetActivityData(TCPComponent * pCom, Global::stReqLimitPetActivityData * recvCmd) {
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("收到stReqLimitPetActivityData,");
    //CLimitPetMgr::getSingleton().synGlobal();
    CLimitPetMgr::getSingleton().synGame();
}
//查询nft结果
void Global2WebModHandle::queryNftResult(TCPComponent * pCom, Global::stGlobal2WebQueryUserNftResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("stGlobal2WebQueryUserNftResult");

	Json::FastWriter writer;
	Json::Value root;
	
	root["code"] = recvCmd->result();
	root["message"] = "";
	root["vip"] = recvCmd->vip();
	
	Json::Value data;
	Json::Value d;

	for (UINT16 i = 0;i <  recvCmd->data_size(); ++i){    
		Global::stNftItem item = recvCmd->data(i);
		d["itemid"] = item.itemid(); 
		d["itemname"] = item.itemname(); 
		d["count"] = item.itemcnt(); 
		d["url"] = item.url(); 
		data.append(d);
	}
	
	root["item"] = data;

	std::string content = writer.write(root);

	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();

	WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());
#ifdef _HDZ_DEBUG
	Athena::logger->trace("转发queryNftResult返回, content is %s!", content.c_str());
#endif
}

//修改nftItem结果
void Global2WebModHandle::changeNftItemResult(TCPComponent * pCom, Global::stGlobal2WebChangeNftItemResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("stGlobal2WebChangeNftItemResult");

	Json::FastWriter writer;
	Json::Value root;
	
	root["code"] = recvCmd->result();
	root["message"] = "";
	
	Json::Value data;
	Json::Value d;
  
	std::string content = writer.write(root);

	CHttpParser parser;
	parser.setStatus(true);
	parser.setBody(content.c_str(),content.size());
	parser.setHeader("Content-Type","text/html; charset=UTF-8");
	std::string ret = parser.buildResponse();

	bool result = WebUserConnMgr::getSingleton().sendRawData(recvCmd->webconnid(),ret.c_str(),ret.size());
	WebUserConnMgr::getSingleton().putbackUniqueID(recvCmd->webconnid());

	Athena::logger->trace("转发changeNftItemResult返回,result:%s, content is %s!", content.c_str(), result?"ok":"fail");

}