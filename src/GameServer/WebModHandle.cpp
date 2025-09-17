#include "WebModHandle.h"
#include "SvrInfo.h"
#include "AthenaNew.h"
#include "GameServer.h"
#include "Activity.h"
#include "RMBShop.h"
#include "GameUser.h"
#include "GameUserManager.h"
#include "LimitTimePetCfg.h"

extern GameServer * g_gameServer;

WebModHandle::WebModHandle()
{
	registerHandle(&WebModHandle::publishActivity);
	registerHandle(&WebModHandle::unloadActivity);
	registerHandle(&WebModHandle::userCancel);
	registerHandle(&WebModHandle::retPayType);
	registerHandle(&WebModHandle::updateSvrOpenTime);
	registerHandle(&WebModHandle::updateLimitPetActivityData);
}

WebModHandle::~WebModHandle()
{

}

bool WebModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<WebModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void WebModHandle::publishActivity(TCPComponent * pCom,Game::stPublishActivity * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到的活动配置为:%s",recvCmd->config().c_str());
#endif
	
	ActivityCfgMgr::getSingleton().init(recvCmd->config());

	Game::stPublishActivityResult cmd;
	cmd.set_result(true);
	cmd.set_webconnid(recvCmd->webconnid());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
}

void WebModHandle::unloadActivity(TCPComponent * pCom,Game::stUnloadActivity * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	ActivityCfgMgr::getSingleton().delActivity(recvCmd->id());

	Game::stUnloadActivityResult cmd;
	cmd.set_result(true);
	cmd.set_webconnid(recvCmd->webconnid());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
}

void WebModHandle::userCancel(TCPComponent * pCom, Game::stUserCancel * recvCmd) {
	if (!recvCmd){
		return ;
	}
	
#ifdef _HDZ_DEBUG
	Athena::logger->trace("[玩家删除接口] : web2game recv role id is %d, type is %d",
                recvCmd->charid(), recvCmd->type());
#endif
    Game::stUserCancelRes cmd;
	cmd.set_webconnid(recvCmd->webconnid());
    GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
    if (!pUser) {//玩家不一定在线,可能只是取到的在线玩家,这个要注意
        Athena::logger->trace("[玩家删除接口] : not find role id");
        cmd.set_res("not_find_role_id");
        g_gameServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
        return ;
    }
    UINT16 type = 0;
    if (recvCmd->type() == Game::stUserCancel::Month_Card) {
        type = CRMBShop::eType_Time_Year;
    } else if (recvCmd->type() == Game::stUserCancel::Forever_Card) {
        type = CRMBShop::eType_Forever;
    } else if (recvCmd->type() == Game::stUserCancel::Grown_Card) {
        type = CRMBShop::eType_Grown;
        pUser->m_buyInvestPlan = 0;
        pUser->m_rewardSys.clearInvestRecords();
        Athena::logger->trace("年卡删除成功");
    }
    if (type) {
        pUser->m_rmbShop.cancelCard(type);
        pUser->m_rmbShop.synRMBInfo2Client();
    } else {
        cmd.set_res("not_find_that_type");
    }
    g_gameServer->getConnMgr().broadcastByType(WEBSERVER,cmd);
}

void WebModHandle::retPayType(TCPComponent * pCom, Game::stReqPayTypeCb * recvCmd) {
    GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
    if (!pUser) {
        Athena::logger->trace("[retPayType] : not find role id");
        return ;
    }
    MiscProto::stRetPayType retCmd;
    retCmd.set_itemid(recvCmd->itemid());
    retCmd.set_paytype(recvCmd->paytype());
    retCmd.set_orderid(recvCmd->orderid());
    if (recvCmd->res() == true) {
        retCmd.set_res(MiscProto::stRetPayType::eSuccess);
    } else {
        retCmd.set_res(MiscProto::stRetPayType::eSame_Order_Id);
    }
#ifdef _HDZ_DEBUG
	Athena::logger->trace("下发充值渠道配置: item is %d, paytype is %s order %s result %d",
			recvCmd->itemid(), recvCmd->paytype().c_str(),recvCmd->orderid().c_str(), retCmd.res());
#endif
    pUser->sendProto2Me(retCmd);
}

void WebModHandle::updateSvrOpenTime(TCPComponent * pCom, Game::stUpdateSvrOpenTime * recvCmd) {
	if (!recvCmd){
		return ;
	}
    g_gameServer->setOpenServerTime(recvCmd->time());
    std::stringstream ss;
    ss << recvCmd->time();
    Athena::global["OpenServer_Time"] = ss.str();
}

void WebModHandle::updateLimitPetActivityData(TCPComponent * pCom, Game::stSendLimitPetActivityData * recvCmd) {
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
    //广播给玩家
    MiscProto::stSendAllAcitivityLimitPet retCmd;
    MiscProto::stAcitivityLimitPetItem * pItem = NULL;
    std::map<UINT32, LimitTimePetCfg::stActivityOpenInfo> petMap;
    LimitTimePetCfg::getSingleton().getOpenInfo(petMap);

    for (std::map<UINT32, LimitTimePetCfg::stActivityOpenInfo>::iterator it = petMap.begin();
            it != petMap.end(); it++) {
        pItem = retCmd.add_item();
        if (pItem) {
            pItem->set_id(it->second.m_id);
            pItem->set_activiyid(it->second.m_activityID);
            pItem->set_starttime(it->second.m_beginTime);
            pItem->set_endtime(it->second.m_endTime);
        }
    }
    Athena::logger->trace("广播限时随从活动");
    GameUserManager::getSingleton().sendProto2World(retCmd);
}
