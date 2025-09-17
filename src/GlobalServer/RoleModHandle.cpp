#include "RoleModHandle.h"
#include "PlayerManager.h"
#include "RoleInfoManager.h"
#include "GiftBag.h"
#include "Arena.h"
#include "Guild.h"
#include "CloseBetaWelfareMgr.h"

RoleModHandle::RoleModHandle()
{
	registerHandle(&RoleModHandle::reqUserDetailInfo);
	registerHandle(&RoleModHandle::reqGiftBag);
	registerHandle(&RoleModHandle::reqCloseBetaReward);
	registerHandle(&RoleModHandle::reqChangeName);

	registerHandle(&RoleModHandle::reqInviteToRoom);
	registerHandle(&RoleModHandle::reqAgreeEnterWedding);
	registerHandle(&RoleModHandle::reqUseFirework);
	registerHandle(&RoleModHandle::reqTakeCandy);
	registerHandle(&RoleModHandle::reqBuyFirework);
	registerHandle(&RoleModHandle::reqEnterWeddingAlone);
	registerHandle(&RoleModHandle::reqQuitWeddingScene);
	registerHandle(&RoleModHandle::reqDrawWeddingRing);
}

RoleModHandle::~RoleModHandle()
{

}

bool RoleModHandle::handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<RoleModHandle,Player>::handle(pUser,pData,cmdLen,funID); 
}

void RoleModHandle::reqUserDetailInfo(Player * pUser,Role::stReqUserDetailInfo * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	Global::stRetOtherUserData2Me cmd;
	cmd.set_mycharid(pUser->getID());
	cmd.set_othercharid(recvCmd->charid());
	
	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if (pGuild){
		cmd.set_guildname(pGuild->getName());			
	}

	if (recvCmd->charid() <= MAX_ROBOT_CHARID){//是机器人
		cmd.set_isrobot(true);
		pUser->sendProto2Game(cmd);	
		return ;
	}

	Player * pOther = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pOther){//在线情况
		pUser->sendProto2Game(cmd);		
	}
	else {//离线情况
		RoleInfoManager::getSingleton().loadOneUserDetail(recvCmd->charid(),pUser->getID());	
	}
}

void RoleModHandle::reqGiftBag(Player * pUser,Role::stReqGiftBag * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CGiftBagMgr::getSingleton().reqGiftBagReward(pUser,recvCmd->code());
}

void RoleModHandle::reqCloseBetaReward(Player *pUser,Role::stReqGotCloseBetaReward * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CCloseBetaWelfareMgr::getSingleton().giveReward(pUser);
}

void RoleModHandle::reqChangeName(Player *pUser,Role::stReqChangeName * recvCmd)
{
	if (!pUser || !recvCmd){
	    return ;
	}
	
	if (strlen(recvCmd->name().c_str()) <= 0){
	    Athena::logger->error("修改角色名的时候名字没有输入任何内容");
	    return ;
	}

	RoleInfoManager::getSingleton().existNameInTbl(recvCmd->name(), recvCmd->charid());
}

void RoleModHandle::reqInviteToRoom(Player *pUser,Role::stReqInviteToRoom * recvCmd) {
	if (!pUser || !recvCmd){
	    return ;
	}
	UINT32 roomId = CMarryMgr::getSingleton().getRoomId(pUser->getID());
	if (!roomId) {
		Athena::logger->trace("stReqInviteToRoom not find room Id");
		return;
	}
	CFriendInfo *pInfo = pUser->m_friendMgr.getFriend(recvCmd->id());
	if (!pInfo) {
		Athena::logger->trace("stReqInviteToRoom not find friend Id");
		return;
	}
	Player * pFriendUser = PlayerManager::getSingleton().getUserByID(recvCmd->id());
	if (!pFriendUser) {
		Athena::logger->trace("stReqInviteToRoom friend is not online");
		return;
	}
	CMarryMgr::getSingleton().inviteFriend(roomId, recvCmd->id());
	Role::NotifyEnterWedding retCmd;
	retCmd.set_id(pUser->getID());
	pFriendUser->sendProtoToMe(retCmd);
}


void RoleModHandle::reqAgreeEnterWedding(Player *pUser,Role::stReqAgreeEnterWedding * recvCmd) {
	if (!pUser || !recvCmd){
	    return ;
	}
	Role::ResultEnterWedding retCmd;
	UINT32 roomId = CMarryMgr::getSingleton().getRoomIdByInviteId(pUser->getID());
	if (!roomId) {
		Athena::logger->trace("reqAgreeEnterWedding not find room Id");
		retCmd.set_result(Role::ResultEnterWedding::RESULT_FAILURE);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	if (!CMarryMgr::getSingleton().isInvited(pUser->getID(), roomId)) {
		Athena::logger->trace("reqAgreeEnterWedding not invited");
		return;
	}
    if (recvCmd->result() == Role::stReqAgreeEnterWedding::RESULT_REFUSE) {
		Athena::logger->trace("reqAgreeEnterWedding 拒绝邀请");
        CMarryMgr::getSingleton().delInviteRecord(pUser->getID(), roomId);
        retCmd.set_result(Role::ResultEnterWedding::RESULT_FAILURE);
        Player * pInviter = PlayerManager::getSingleton().getUserByID(recvCmd->id());
        if (pInviter) {
            pInviter->sendProtoToMe(retCmd);
        }
        pUser->sendProtoToMe(retCmd);
        return;
    }
	CMarryMgr::getSingleton().addFriendToRoom(pUser->getID(), roomId);
	CMarryMgr::getSingleton().delInviteRecord(pUser->getID(), roomId);
	retCmd.set_result(Role::ResultEnterWedding::RESULT_SUCCESS);
	pUser->sendProtoToMe(retCmd);

	std::set<UINT32> s;
	CMarryMgr::getSingleton().getRoomFriends(roomId, s);
	UINT32 husband = 0, wife = 0;
	Role::MarryInviterPlace retCmd2;
	retCmd2.set_cfg1(2);
	retCmd2.set_time(2);
	CMarryMgr::getSingleton().getCouple(roomId, husband, wife);
	Player * pHus = PlayerManager::getSingleton().getUserByID(husband);
	if (pHus){
		retCmd2.set_name1(pHus->getName());
	}
	Player * pWife = PlayerManager::getSingleton().getUserByID(wife);
	if (pWife){
		retCmd2.set_name2(pWife->getName());
	}

	retCmd2.set_state(Role::MarryInviterPlace::STATE_INVITEE);
	Global::stReqAgreeInvite cmd;
	for (std::set<UINT32>::iterator it = s.begin(); 
			it != s.end(); it++) {
		retCmd2.add_id(*it);
        cmd.add_id(*it);
	}
	retCmd2.set_state(Role::MarryInviterPlace::STATE_INVITEE);
	pUser->sendProtoToMe(retCmd2);

	Role::stNotifyFriendAction retCmd3;
	retCmd3.set_in(true);
	retCmd3.set_charid(pUser->getID());
	if (pHus) {
		pHus->sendProtoToMe(retCmd3);
	}
	if (pWife) {
		pWife->sendProtoToMe(retCmd3);
	}


	cmd.set_charid(pUser->getID());
	cmd.set_roomid(roomId);
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}


void RoleModHandle::reqUseFirework(Player *pUser,Role::stReqUseFirework * recvCmd) {
	if (!pUser || !recvCmd){
	    return ;
	}
	Role::ResultUseFirework retCmd;
	UINT32 roomId = CMarryMgr::getSingleton().getRoomIdByUser(pUser->getID());
	if (!roomId) {
		Athena::logger->trace("reqUseFirework not find room Id");
        retCmd.set_result(Role::ResultUseFirework::RESULT_FAILURE);
        pUser->sendProtoToMe(retCmd);
		return ;
	}
	std::set<UINT32> s;
	CMarryMgr::getSingleton().getRoomFriends(roomId, s);
    s.erase(pUser->getID());
	Global::stUseFireWork cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_index(recvCmd->fire());
	for (std::set<UINT32>::iterator it = s.begin();
			it != s.end(); it++) {
        cmd.add_id(*it);
	}
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}


void RoleModHandle::reqTakeCandy(Player *pUser,Role::stReqTakeCandy * recvCmd) {
	if (!pUser || !recvCmd){
	    return ;
	}
	Role::ResultTakeCandy retCmd;
	retCmd.set_result(Role::ResultTakeCandy::RESULT_FAILURE);
	pUser->sendProtoToMe(retCmd);

	//Global::stReqTakeCandy cmd;
	//cmd.set_charid(pUser->getID());
	//g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}


void RoleModHandle::reqBuyFirework(Player *pUser,Role::stReqBuyFirework * recvCmd) {
	if (!pUser || !recvCmd){
	    return ;
	}
	Global::stReqBuyFireWork cmd;
	cmd.set_charid(pUser->getID());
	cmd.set_id(recvCmd->id());
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

void RoleModHandle::reqEnterWeddingAlone(Player *pUser,Role::stReqEnterWeddingAlone * recvCmd) {
	if (!pUser || !recvCmd){
	    return ;
	}
	Role::ResultEnterWeddingAlone retCmd;
	UINT32 roomId = CMarryMgr::getSingleton().getRoomId(pUser->getID());
	if (!roomId) {
		Athena::logger->trace("stReqEnterWeddingAlone not find room Id");
		retCmd.set_result(Role::ResultEnterWeddingAlone::RESULT_FAILURE);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	CMarryMgr::getSingleton().addFriendToRoom(pUser->getID(), roomId);
	retCmd.set_result(Role::ResultEnterWeddingAlone::RESULT_SUCCESS);
	pUser->sendProtoToMe(retCmd);
	
	std::set<UINT32> s;
	CMarryMgr::getSingleton().getRoomFriends(roomId, s);
	UINT32 husband = 0, wife = 0;
	Role::MarryInviterPlace retCmd2;
	CMarryMgr::getSingleton().getCouple(roomId, husband, wife);

	Role::stNotifyFriendAction retCmd3;
	retCmd3.set_in(true);
	retCmd3.set_charid(pUser->getID());

	if (husband == pUser->getID()) {
		retCmd2.set_name1(pUser->getName());
		CFriendInfo *pInfo = pUser->m_friendMgr.getFriend(wife);
		if (pInfo) {
			retCmd2.set_name2(pInfo->m_name);
		}
		Player * pWife = PlayerManager::getSingleton().getUserByID(wife);
		if (pWife){
			pWife->sendProtoToMe(retCmd3);
		}
	}
	if (wife == pUser->getID()) {
		retCmd2.set_name2(pUser->getName());
		CFriendInfo *pInfo = pUser->m_friendMgr.getFriend(husband);
		if (pInfo) {
			retCmd2.set_name1(pInfo->m_name);
		}
		Player * pHus = PlayerManager::getSingleton().getUserByID(husband);
		if (pHus){
			pHus->sendProtoToMe(retCmd3);
		}
	}
	retCmd2.set_state(Role::MarryInviterPlace::STATE_INVITER);
	retCmd2.set_cfg1(2);
    
	Global::stReqReEnterWedding cmd;
	cmd.set_charid(pUser->getID());
    cmd.set_roomid(roomId);
	for (std::set<UINT32>::iterator it = s.begin(); 
			it != s.end(); it++) {
		retCmd2.add_id(*it);
        cmd.add_id(*it);
	}
	pUser->sendProtoToMe(retCmd2);

	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

void RoleModHandle::reqQuitWeddingScene(Player *pUser,Role::stReqQuitWeddingScene * recvCmd) {
	if (!pUser || !recvCmd){
	    return ;
	}
	Role::stRetQuitWeddingScene retCmd;
	UINT32 roomId = CMarryMgr::getSingleton().getRoomIdByUser(pUser->getID());
	if (!roomId) {
		Athena::logger->trace("reqQuitWeddingScene not in room, but quit yet");
		retCmd.set_result(Role::stRetQuitWeddingScene::eSuccess);
		pUser->sendProtoToMe(retCmd);
		return;
	}
	UINT32 husband = 0, wife = 0;
	Role::stNotifyFriendAction retCmd3;
	retCmd3.set_in(false);
	retCmd3.set_charid(pUser->getID());
	CMarryMgr::getSingleton().getCouple(roomId, husband, wife);
	Player * pHus = PlayerManager::getSingleton().getUserByID(husband);
	if (pHus){
		pHus->sendProtoToMe(retCmd3);
	}
	Player * pWife = PlayerManager::getSingleton().getUserByID(wife);
	if (pWife){
		pWife->sendProtoToMe(retCmd3);
	}

	CMarryMgr::getSingleton().quitRoom(pUser->getID());

	std::set<UINT32> s;
	Global::stReqQuitWedding cmd;
	CMarryMgr::getSingleton().getRoomFriends(roomId, s);
    for (std::set<UINT32>::iterator it = s.begin();
            it != s.end(); it++) {
        cmd.add_id(*it);
    }
	cmd.set_charid(pUser->getID());
	cmd.set_levelid(recvCmd->levelid());
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

void RoleModHandle::reqDrawWeddingRing(Player *pUser,Role::stReqDrawWeddingRing * recvCmd) {
	if (!pUser || !recvCmd){
	    return ;
	}
    Role::stRetDrawWeddingRing retCmd;
	UINT32 roomId = CMarryMgr::getSingleton().getRoomId(pUser->getID());
	if (!roomId) {
		Athena::logger->trace("reqDrawWeddingRing not find room Id");
        retCmd.set_result(Role::stRetDrawWeddingRing::eNot_In_Scene);
		pUser->sendProtoToMe(retCmd);
		return;
	}
    MarryInfo marryInfo;
    if (!CMarryMgr::getSingleton().getMarryInfo(roomId, marryInfo)) {
		Athena::logger->trace("reqDrawWeddingRing not find room Id, not marryed ");
        retCmd.set_result(Role::stRetDrawWeddingRing::eNot_Marry);
		pUser->sendProtoToMe(retCmd);
		return;
    }
    if (marryInfo.husband != pUser->getID() && marryInfo.wife != pUser->getID()) {
		Athena::logger->trace("reqDrawWeddingRing not marryed, invalid user id");
        retCmd.set_result(Role::stRetDrawWeddingRing::eNot_Marry);
		pUser->sendProtoToMe(retCmd);
		return;
    }
    if (marryInfo.husband == pUser->getID() && marryInfo.husRing) {
		Athena::logger->trace("reqDrawWeddingRing husband ring aready get");
        retCmd.set_result(Role::stRetDrawWeddingRing::eAready_Draw);
		pUser->sendProtoToMe(retCmd);
		return;
    }
    if (marryInfo.wife == pUser->getID() && marryInfo.wifeRing) {
		Athena::logger->trace("reqDrawWeddingRing wife ring aready get");
        retCmd.set_result(Role::stRetDrawWeddingRing::eAready_Draw);
		pUser->sendProtoToMe(retCmd);
		return;
    }
    Global::stSendWeedingRing cmd;
    cmd.set_charid(pUser->getID());
    cmd.set_roomid(roomId);
    cmd.set_id(marryInfo.weddingId);
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}
