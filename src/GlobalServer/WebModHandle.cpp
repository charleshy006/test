#include "WebModHandle.h"
#include "SvrInfo.h"
#include "GlobalServer.h"
#include "Player.h"
#include "PlayerManager.h"
#include "AthenaNew.h"
#include "RoleInfoManager.h"
#include "Mail.h"
#include "NoticeMgr.h"
#include "RechargeMgr.h"
#include "GiftBag.h"
#include "BlackWhiteMgr.h"
#include "ServerConfig.h"
#include "KVPair.h"
#include "CompensationSys.h"
#include "WelfareCenterCfg.h"
#include "TxGiftBagMgr.h"
#include "SysLanguages.h"
#include "InviteFriendMgr.h"
#include "LimitTimePetCfg.h"
#include "ShutDownMgr.h"
#include "json/reader.h"
#include "json/writer.h"
#include "HttpClient.h"
#include "HttpParser.h"

extern GlobalServer * g_globalServer;

WebModHandle::WebModHandle()
{
	registerHandle(&WebModHandle::killUserOut);
	registerHandle(&WebModHandle::setGM);
	registerHandle(&WebModHandle::banUser);
	registerHandle(&WebModHandle::sendMail);
	registerHandle(&WebModHandle::modifyUserInfo);
	registerHandle(&WebModHandle::opNotice);
	registerHandle(&WebModHandle::delNotice);
	registerHandle(&WebModHandle::notifyRecharge);
	registerHandle(&WebModHandle::addGiftBagConfig);
	registerHandle(&WebModHandle::removeGiftBagConfig);
	registerHandle(&WebModHandle::addBlackWhite);
	registerHandle(&WebModHandle::setMaintanenceTime);
	registerHandle(&WebModHandle::getSvrCfg);
	registerHandle(&WebModHandle::updateSvrCfg);
	registerHandle(&WebModHandle::reqGetTxGiftBag);
	registerHandle(&WebModHandle::updateSvrOpenTime);
	registerHandle(&WebModHandle::reqUseInviteCode);
	registerHandle(&WebModHandle::updateLimitPetActivityData);
	registerHandle(&WebModHandle::sendShutDown);
	registerHandle(&WebModHandle::queryNft);
	registerHandle(&WebModHandle::changeNftItem);
}

WebModHandle::~WebModHandle()
{

}

bool WebModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<WebModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void WebModHandle::killUserOut(TCPComponent * pCom,Global::stReqKillUserOut * recvCmd)
{
	if (!recvCmd){
		return;
	}
	Athena::logger->trace("玩家(charid:%d,webconnid:%d) 收到踢人协议!",recvCmd->charid(),recvCmd->webconnid());	
	Global::stKillUserOutResult cmd;
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
			Athena::logger->trace("踢人 ！没找到玩家!");

		cmd.set_result(false);
		cmd.set_webconnid(recvCmd->webconnid());
	}
	else {
		Athena::logger->trace("踢人 ！找到了玩家！！！玩家!");

		cmd.set_result(true);
		cmd.set_webconnid(recvCmd->webconnid());
		Global::stKillUserOut  gateCmd;
		gateCmd.set_charid(pUser->getID());
		pUser->sendProto2Gate(gateCmd);
	}
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
	Athena::logger->trace("收到踢人协议!");
}

void WebModHandle::setGM(TCPComponent * pCom,Global::stReqSetUserGMType * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Global::stBanUserResult cmd;
	cmd.set_webconnid(recvCmd->webconnid());
	cmd.set_result(true);
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pUser){//在线情况
		Global::stOnlineUserGMType	cmd1;
		cmd1.set_charid(recvCmd->charid());
		cmd1.set_gmvalue(recvCmd->gmvalue());
		pUser->sendProto2Game(cmd1);
	}
	else {//离线情况
		RoleInfoManager::getSingleton().setUserGMValue(recvCmd->charid(),recvCmd->gmvalue());	
	}
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
	Athena::logger->trace("收到设置GM协议!");
}

void WebModHandle::banUser(TCPComponent * pCom,Global::stReqBanUser * recvCmd)
{
	if (!recvCmd){
		return ;
	}
 	
	Global::stBanUserResult retCmd;
	retCmd.set_webconnid(recvCmd->webconnid());
	retCmd.set_result(true);
	
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pUser){//在线情况
		if (recvCmd->ban()){//封号,在线的玩家解封无意义
			Global::stForbidOnlineUser gameCmd;
			gameCmd.set_charid(recvCmd->charid());
			pUser->sendProto2Game(gameCmd);
			
			Global::stKillUserOut  gateCmd;
			gateCmd.set_charid(recvCmd->charid());
			pUser->sendProto2Gate(gateCmd);
		}
	}
	else {//离线情况
		RoleInfoManager::getSingleton().selectRoleBitMaskForModify(recvCmd->charid(),recvCmd->ban());	
	}
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,retCmd);
	Athena::logger->trace("收到封进解封玩家协议!");
}

void g_webSendMailCB(UINT64 autoID)
{}

void WebModHandle::sendMail(TCPComponent * pCom,Global::stSendMail2User * recvCmd)
{
	if (!recvCmd){
		return ;	
	}
	
	if (recvCmd->ball()){
		stCompensationInfoDB dbData;
		strncpy(dbData.m_title,recvCmd->title().c_str(),sizeof(dbData.m_title));
		strncpy(dbData.m_content,recvCmd->content().c_str(),sizeof(dbData.m_content));
		dbData.m_createTime = g_globalServer->getSec();
		std::ostringstream os;
		for (UINT16 j = 0;j < recvCmd->attaches_size();++j){
			const Global::stSendMailAttach & pAttach = recvCmd->attaches(j);
			os << pAttach.itemid() << "-" << pAttach.num() << ";";
		}
		strncpy(dbData.m_rewards,os.str().c_str(),sizeof(dbData.m_rewards));
		CCompensationMgr::getSingleton().addRecord(dbData);
	}
	else {
		for (UINT16 i = 0;i < recvCmd->userlist_size();++i){
			stMailDBInfo mailInfo;
			mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
			mailInfo.m_receiverID = recvCmd->userlist(i);
			mailInfo.m_senderID = 0;
			strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
			strncpy(mailInfo.m_title,recvCmd->title().c_str(),sizeof(mailInfo.m_title));
			strncpy(mailInfo.m_content,recvCmd->content().c_str(),sizeof(mailInfo.m_content));
			for (UINT16 j = 0;j < recvCmd->attaches_size();++j){
				const Global::stSendMailAttach & pAttach = recvCmd->attaches(j);
				if (j >= 3){
					break;
				}
				if (0 == j){
					mailInfo.m_attachID1 = pAttach.itemid();
					mailInfo.m_attachNum1 = pAttach.num();
				}

				if (1 == j){
					mailInfo.m_attachID2 = pAttach.itemid();
					mailInfo.m_attachNum2 = pAttach.num();
				}

				if (2 == j){
					mailInfo.m_attachID3 = pAttach.itemid();
					mailInfo.m_attachNum3 = pAttach.num();
				}

			}

			mailInfo.m_sendTime = g_globalServer->getSec();
			MailManager::getSingleton().sendMail(mailInfo,g_webSendMailCB);   
		}
	}
	Global::stSendMail2UserResult cmd;
	cmd.set_result(true);
	cmd.set_webconnid(recvCmd->webconnid());
	cmd.set_tag(recvCmd->tag());
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
}

void WebModHandle::modifyUserInfo(TCPComponent * pCom,Global::stWebMotifyUserInfo * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pUser){//在线情况
		Global::stGMModifyUserInfo cmd;
		cmd.set_charid(recvCmd->charid());
		cmd.set_newbieguideid(recvCmd->newbieguideid());
		cmd.set_goldcoin(recvCmd->goldcoin());
		cmd.set_diamond(recvCmd->diamond());
		cmd.set_vippoint(recvCmd->vippoint());
		pUser->sendProto2Game(cmd);
	}
	else {//离线情况
		stGMModifyUserInfoDB dbData;
		dbData.m_newbieGuideID = recvCmd->newbieguideid();
        dbData.m_goldCoin = recvCmd->goldcoin();
        dbData.m_diamond = recvCmd->diamond();
        dbData.m_vipPoint = recvCmd->vippoint();
		RoleInfoManager::getSingleton().gmModifyOfflineUserInfo(recvCmd->charid(),dbData);	
	}

	Global::stWebMotifyUserInfoRet  webCmd;
	webCmd.set_charid(recvCmd->charid());
	webCmd.set_result(true);
	webCmd.set_webconnid(recvCmd->webconnid());
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void WebModHandle::opNotice(TCPComponent * pCom,Global::stAddOrUpdateNotice * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	if (Global::eInsert_Notice == recvCmd->op()){//插入公告
		if (3 == recvCmd->noticeposition()){
			MiscProto::stOpenWindowTip retCmd;
			retCmd.set_action(MiscProto::stOpenWindowTip::eNotify_Download_New_Version);
			retCmd.set_content(recvCmd->content());
			PlayerManager::getSingleton().sendProto2AllZone(retCmd);
		}
		else {
			CNoticeMgr::getSingleton().addNotice(recvCmd->noticeid(),recvCmd->begintime(),0,recvCmd->num(),recvCmd->interval(),recvCmd->content().c_str());	
		}
	}
	else if (Global::eUpdate_Notice == recvCmd->op()){//更新公告
		CNoticeMgr::getSingleton().updateNotice(recvCmd->noticeid(),recvCmd->begintime(),0,recvCmd->num(),recvCmd->interval(),recvCmd->content().c_str());	
	}
}

void WebModHandle::delNotice(TCPComponent * pCom,Global::stDelNotice * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	CNoticeMgr::getSingleton().delNotice(recvCmd->noticeid());
}


static void g_HttpRequestCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("获得的http相应信息%s",ret.c_str());
		}
	}
	else {
		Athena::logger->error("获得的http相应失败!");
	}
}

void WebModHandle::notifyRecharge(TCPComponent * pCom,Global::stWebNotifyRechargeInfo * recvCmd)
{
	if (!recvCmd){
		return ;
	}

    if (recvCmd->tid() != std::string("sandbox")) {
        if (CRechargeMgr::getSingleton().checkTid(recvCmd->tid())){
            Athena::logger->trace("[人民币商店购买]流水id=%s已经充值过了,还试图发送过来",recvCmd->tid().c_str());
            return;
        }
    } else {
		Athena::logger->trace("[人民币商店购买]沙盒环境,不检测订单号");
    }

	CRechargeMgr::getSingleton().addTid(recvCmd->tid());

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());

#ifdef _YUNDING_TONGJI
	if (pUser){//在线的情况
		CRechargeMgr::getSingleton().onlineRecharge(pUser,recvCmd->money(),recvCmd->itemid(),recvCmd->tid(), recvCmd->isextend(), recvCmd->data());
		Athena::logger->trace("[人民币商店购买]给在线玩家account=%s,id=%d,name=%s,opID=%d,冲了%d人民币,流水号是%s, 扩展%s",recvCmd->account().c_str(),pUser->getID(),pUser->getName(),recvCmd->opid(),recvCmd->money(),recvCmd->tid().c_str(), recvCmd->data().c_str());
	}
	else {//离线的情况
		CRechargeMgr::getSingleton().offlineRecharge(recvCmd->charid(),recvCmd->money(),recvCmd->itemid(),recvCmd->tid(), recvCmd->isextend(), recvCmd->data());
		Athena::logger->trace("[人民币商店购买]给离线玩家account=%s,id=%d,name=%s,opID:%d,冲了%d人民币,流水号是%s, 扩展%s",recvCmd->account().c_str(),recvCmd->charid(),"",recvCmd->opid(),recvCmd->money(),recvCmd->tid().c_str(), recvCmd->data().c_str());
	}
#else
	if (pUser){//在线的情况
		CRechargeMgr::getSingleton().onlineRecharge(pUser,recvCmd->money(),recvCmd->itemid(),recvCmd->tid());
		Athena::logger->trace("[人民币商店购买]给在线玩家account=%s,id=%d,name=%s,opID=%d,冲了%d人民币,流水号是%s",recvCmd->account().c_str(),pUser->getID(),pUser->getName(),recvCmd->opid(),recvCmd->money(),recvCmd->tid().c_str());
	}
	else {//离线的情况
		CRechargeMgr::getSingleton().offlineRecharge(recvCmd->charid(),recvCmd->money(),recvCmd->itemid(),recvCmd->tid());
		Athena::logger->trace("[人民币商店购买]给离线玩家account=%s,id=%d,name=%s,opID:%d,冲了%d人民币,流水号是%s",recvCmd->account().c_str(),recvCmd->charid(),"",recvCmd->opid(),recvCmd->money(),recvCmd->tid().c_str());
	}
#endif
	if(Athena::global["tapdbid"] != ""){
// 		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
// 		pRequest->setUrl("https://e.tapdb.net/event");
// 		pRequest->setRequestType(CHttpRequest::kHttpPost); 

// 		Json::FastWriter writer; 
// 		Json::Value content;	
		
// 		content["module"] = "GameAnalysis"; 
// 		content["name"] = "charge"; 
// 		if (recvCmd->paychannel() == PayLaosiji){
// 			content["index"] =  "p8gbp05xt29463y1"; 
// 		}else{
// 			content["index"] =  Athena::global["tapdbid"].c_str();//"9subbbcjkudb1mkn"; 
// 		}
		
// 		content["identify"] = recvCmd->account();
// 		content["type"] = "track"; 
// 		content["user_id"] = recvCmd->account();
	
// 		Json::Value value;	
		
// 		value["order_id"] = recvCmd->tid();
// 		value["amount"] =  recvCmd->money()*100;
// 		value["virtual_currency_amount"] = 0;
// 		if (recvCmd->paychannel() == PayLaosiji){
// 			value["currency_type"] =  "TWD";
// 		}else{
// 			value["currency_type"] =  "USD";
// 		}
// 		std::ostringstream os;
// 		os <<  recvCmd->itemid();
// 		value["product"] = os.str();
	
// 		content["properties"] = value;
// 		// root.append(content);
// 		std::string json_file = writer.write(content);
// #ifdef _HDZ_DEBUG
// 		Athena::logger->trace("上报内容文本是%s",json_file.c_str());
// #endif
// 		std::string code = HttpUtility::URLEncode(json_file);
// #ifdef _HDZ_DEBUG
// 		Athena::logger->trace("URLEncode后,文本是%s",code.c_str());
// #endif
// 		pRequest->setRequestData(code.c_str(),code.size());
// 		pRequest->setResponseCallback(g_HttpRequestCB);
// 		CHttpClient::getSingleton().asynSend(pRequest);
	}
}
	
void WebModHandle::addGiftBagConfig(TCPComponent * pCom,Global::stAddGiftBagCfg * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CGiftBagMgr::getSingleton().addGiftBagCfg(recvCmd->id(),recvCmd->content());
}

void WebModHandle::removeGiftBagConfig(TCPComponent * pCom,Global::stDelGiftBagCfg * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CGiftBagMgr::getSingleton().removeGiftBagCfg(recvCmd->id());
}

void WebModHandle::addBlackWhite(TCPComponent * pCom,Global::stAddBlackAndWhite * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	CBlackWhiteMgr::getSingleton().clear();

	for (UINT16 i = 0;i < recvCmd->blacklist_size();++i){
		CBlackWhiteMgr::getSingleton().addBlackName(recvCmd->blacklist(i));	
#ifdef _HDZ_DEBUG
		Athena::logger->trace("添加了黑名单:%s",recvCmd->blacklist(i).c_str());
#endif
	}

	for (UINT16 i = 0;i < recvCmd->whitelist_size();++i){
		CBlackWhiteMgr::getSingleton().addWhiteName(recvCmd->whitelist(i));
#ifdef _HDZ_DEBUG
		Athena::logger->trace("添加了白名单:%s",recvCmd->whitelist(i).c_str());
#endif
	}
}

void WebModHandle::setMaintanenceTime(TCPComponent * pCom,Global::stSetServerMaintenanceTime * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	char szTime[120] = { 0 };
	snprintf(szTime,sizeof(szTime),"%s",recvCmd->optime().c_str());
	
	UINT32 startTime = 0 ;

	struct tm _tm;
	if (!strptime(szTime,"%Y-%m-%d %H:%M:%S",&_tm)){
		return ;
	}
	startTime = mktime(&_tm) /*- 8 * 3600*/;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了维护时间:start:%d,lasttime:%d",startTime,recvCmd->opduring() * 60);
#endif
	CBlackWhiteMgr::getSingleton().setMaintanenceTime(startTime,recvCmd->opduring() * 60);	
}

void WebModHandle::getSvrCfg(TCPComponent * pCom,Global::stGetSvrConfig * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	Global::stRetSvrConfig cmd;
	cmd.set_webconnid(recvCmd->webconnid());
	Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
	if (pSyn){
		pSyn->set_key("statProto");
		pSyn->set_value(Athena::global["statProto"]);
	}
	pSyn = cmd.add_cfgpair();
	if (pSyn){
		pSyn->set_key("gameVersion");
		pSyn->set_value(Athena::global["gameVersion"]);
	}
	pSyn = cmd.add_cfgpair();	
	if (pSyn){
		pSyn->set_key("gameID");
		pSyn->set_value(Athena::global["gameID"]);
	}
	
	pSyn = cmd.add_cfgpair();
	if (pSyn){
		pSyn->set_key("opID");
		pSyn->set_value(Athena::global["platformID"]);
	}
	
	pSyn = cmd.add_cfgpair();
	if (pSyn){
		pSyn->set_key("serverID");
		pSyn->set_value(Athena::global["serverID"]);
	}

	pSyn = cmd.add_cfgpair();
	if (pSyn){
		pSyn->set_key("serverKey");
		pSyn->set_value(Athena::global["serverKey"]);
	}

	pSyn = cmd.add_cfgpair();
	if (pSyn){
		pSyn->set_key("billURL");
		pSyn->set_value(Athena::global["billURL"]);
	}

	pSyn = cmd.add_cfgpair();
	if (pSyn){
		pSyn->set_key("loadStat");
		pSyn->set_value(Athena::global["loadStat"]);
	}

	pSyn = cmd.add_cfgpair();
	if (pSyn){
		pSyn->set_key("payType");
		pSyn->set_value(Athena::global["payType"]);
	}
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
}

void WebModHandle::updateSvrCfg(TCPComponent * pCom,Global::stUpdateSvrCfg * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	for (UINT16 i = 0;i < recvCmd->cfgpair_size();++i){
		const Global::stServerCfgPair & cfgPair = recvCmd->cfgpair(i);
		CServerConfigMgr::getSingleton().setValue(cfgPair.key(),cfgPair.value());
		Athena::global[cfgPair.key()] = cfgPair.value();
	}
	CServerConfigMgr::getSingleton().updateRecord();
	CServerConfigMgr::getSingleton().notifyGame();
	CServerConfigMgr::getSingleton().notifyGate();
}

void g_txGiftBagSendMailCB(UINT64 autoID)
{}

void WebModHandle::reqGetTxGiftBag(TCPComponent * pCom,Global::stReqGetTxGiftBag * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	Global::stRetGetTxGiftBag cmd;

	WelfareCenterCfg::stCondition *pCond =  WelfareCenterCfg::getSingleton().getCondition(recvCmd->taskid(),recvCmd->arewardid());
	if (pCond){
		if (!CTxGiftBagMgr::getSingleton().hasGotGiftBag(recvCmd->charid(),recvCmd->taskid())){
			stMailDBInfo mailInfo;
			mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
			mailInfo.m_receiverID = recvCmd->charid();
			mailInfo.m_senderID = 0; 
			strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
			for (UINT16 i = 0;i < pCond->m_rewardVec.size();++i){
				WelfareCenterCfg::stRewardItem & item = pCond->m_rewardVec[i];
				if (0 == i){
					mailInfo.m_attachID1 = item.m_id;
					mailInfo.m_attachNum1 = item.m_num; 
				}
				else if (1 == i){
					mailInfo.m_attachID2 = item.m_id;
					mailInfo.m_attachNum2 = item.m_num;
				}
				else if (2 == i){
					mailInfo.m_attachID3 = item.m_id;
					mailInfo.m_attachNum3 = item.m_num;
				}
			}
			mailInfo.m_isFetchAttach = 0; 
			strncpy(mailInfo.m_content,"领取应用宝福利中心礼包",sizeof(mailInfo.m_content));
			mailInfo.m_sendTime = g_globalServer->getSec();
			MailManager::getSingleton().sendMail(mailInfo,g_txGiftBagSendMailCB);
			cmd.set_result(0);
			cmd.set_msg("领取成功!");
			CTxGiftBagMgr::getSingleton().addGotGiftBagInfo(recvCmd->charid(),recvCmd->taskid(),recvCmd->billno().c_str());
		}
		else {
			cmd.set_result(102);
			cmd.set_msg("该步骤奖励已发放过!");
		}
	}   
	else {
		cmd.set_result(2);
		cmd.set_msg("请求参数错误!");
	}   
	cmd.set_webconnid(recvCmd->webconnid());
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
}

void WebModHandle::updateSvrOpenTime(TCPComponent * pCom,Global::stUpdateSvrOpenTime * recvCmd) {
	if (!recvCmd){
		return ;
	}
    g_globalServer->setOpenServerTime(recvCmd->time());
    std::stringstream ss;
    ss << recvCmd->time();
    Athena::global["OpenServer_Time"] = ss.str();
}

void WebModHandle::reqUseInviteCode(TCPComponent * pCom,Global::stUseInviteCode * recvCmd) {
	if (!recvCmd){
		return ;
	}
    std::string msg, codeOwner;
    if (CInviteFriendMgr::getSingleton().getOwner(recvCmd->code(), codeOwner)) {
        InviteFriend * pOwner = CInviteFriendMgr::getSingleton().getInviteFriend(codeOwner);
        if (pOwner) {
            pOwner->addInviteCount();
            stInviteFriendInfoDB ownerDb;
            pOwner->fillDBData(ownerDb);
            CInviteFriendMgr::getSingleton().updateRecord(ownerDb);
            msg = std::string("ok");
        } else {
            msg = std::string("not_find_code_owner");
        }
    } else {
        msg = std::string("no_such_code");
    }
    Global::stUseInviteCodeCb cmd;
    cmd.set_msg(msg);
	cmd.set_webconnid(recvCmd->webconnid());
	cmd.set_account(recvCmd->account());
	cmd.set_userid(recvCmd->userid());
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
}

void WebModHandle::updateLimitPetActivityData(TCPComponent * pCom, Global::stSendLimitPetActivityData * recvCmd) {
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("收到stSendLimitPetActivityData ");
    std::map<UINT32, LimitTimePetCfg::stActivityOpenInfo> itemMap;
    for (int i = 0; i < recvCmd->item_size(); i++) {
        LimitTimePetCfg::stActivityOpenInfo item;
        item.m_id = recvCmd->item(i).id();
        item.m_activityID = recvCmd->item(i).activiyid();
        item.m_beginTime = recvCmd->item(i).starttime();
        item.m_endTime = recvCmd->item(i).endtime();
        itemMap.insert(std::make_pair(item.m_id, item));
    }
    LimitTimePetCfg::getSingleton().setOpenInfo(itemMap);
}

void WebModHandle::sendShutDown(TCPComponent * pCom, Global::stSendShutDown * recvCmd) {
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("收到stSendShutDown ");
    CShutDownMgr::getSingleton().beginStop(true);
}

void WebModHandle::queryNft(TCPComponent * pCom, Global::stWeb2GlobalQueryUserNft * recvCmd) {
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("收到stWeb2GlobalQueryUserNft charid=%d", recvCmd->charid());
	Global::stGlobal2GameQueryUserNft cmd;
	cmd.set_charid(recvCmd->charid());
	cmd.set_webconnid(recvCmd->webconnid());

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pUser){//在线情况
		pUser->sendProto2Game(cmd);
	}
	else {//离线情况
		// Global::stGlobal2WebQueryUserNftResult  webCmd;
		// webCmd.set_charid(recvCmd->charid());
		// webCmd.set_webconnid(recvCmd->webconnid());
		// webCmd.set_result(1);
		
		// g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
		RoleInfoManager::getSingleton().loadNftOfflineUser(recvCmd->charid(), recvCmd->webconnid());
	}
}
    
//修改nft道具
void WebModHandle::changeNftItem(TCPComponent * pCom, Global::stWeb2GlobalChangeNftItem * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Athena::logger->trace("收到stWeb2GlobalChangeNftItem charid=%d itemid=%d count=%d", recvCmd->charid(), recvCmd->itemid(), recvCmd->count());

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pUser){//在线情况
		Global::stGlobal2GameChangeNftItem cmd;
		cmd.set_charid(recvCmd->charid());
		cmd.set_webconnid(recvCmd->webconnid());
		cmd.set_itemid(recvCmd->itemid());
		cmd.set_count(recvCmd->count());
		pUser->sendProto2Game(cmd);
	}
	else {//离线情况
		// Global::stGlobal2WebChangeNftItemResult  webCmd;
		// webCmd.set_charid(recvCmd->charid());
		// webCmd.set_webconnid(recvCmd->webconnid());
		// webCmd.set_result(1);
		
		// g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

		RoleInfoManager::getSingleton().loadNftOfflineUserForChange(recvCmd->charid(), recvCmd->webconnid(),recvCmd->itemid(),recvCmd->count());
	}
}