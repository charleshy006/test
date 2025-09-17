#include "FriendModHandle.h"
#include "SvrInfo.h"
#include "FriendMgr.h"
#include "PlayerManager.h"
#include "RankMgr.h"

FriendModHandle::FriendModHandle()
{
	registerHandle(&FriendModHandle::reqAddFriend);
	registerHandle(&FriendModHandle::reqDelFriend);
	registerHandle(&FriendModHandle::findAndAddFriend);
	registerHandle(&FriendModHandle::reqApplyList);

	registerHandle(&FriendModHandle::reqRecommondFriend);
	registerHandle(&FriendModHandle::giveLilian);
	registerHandle(&FriendModHandle::reqLilianList);
	registerHandle(&FriendModHandle::fetchLilian);
	registerHandle(&FriendModHandle::add2ApplyList);
	registerHandle(&FriendModHandle::refuseAddMeFriend);
	registerHandle(&FriendModHandle::invitePK);
	registerHandle(&FriendModHandle::replyPKInvite);
	registerHandle(&FriendModHandle::reqPropose);
	registerHandle(&FriendModHandle::reqAgreeMarry);
	registerHandle(&FriendModHandle::reqCancelMarry);
}

FriendModHandle::~FriendModHandle()
{

}

bool FriendModHandle::handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<FriendModHandle,Player>::handle(pUser,pData,cmdLen,funID); 
}

void FriendModHandle::reqAddFriend(Player * pUser,FriendProto::stReqAddOneFriend * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	if (recvCmd->charid() == 0){
		std::vector<UINT32> applyList;
		CFriendDBOp::getSingleton().getApplyList(pUser,applyList);
		for (UINT16 i = 0;(i < applyList.size()) && (i < 10);++i){
			CFriendDBOp::getSingleton().reqAddFriend(pUser->getID(),applyList[i]);	
		}
	}
	else {
		CFriendDBOp::getSingleton().reqAddFriend(pUser->getID(),recvCmd->charid());
	}
}

void FriendModHandle::reqDelFriend(Player * pUser,FriendProto::stDelFriend * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CFriendDBOp::getSingleton().reqRemoveFriend(pUser,recvCmd->charid());
}

void FriendModHandle::findAndAddFriend(Player * pUser,FriendProto::stFindAndAddFriend * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CFriendDBOp::getSingleton().findAndAdd(pUser,const_cast<char *>(recvCmd->name().c_str()));
}

void FriendModHandle::reqApplyList(Player * pUser,FriendProto::stReqApplyAddFriendList * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}

	CFriendDBOp::getSingleton().reqApplyList(pUser);
}

void FriendModHandle::reqRecommondFriend(Player * pUser,FriendProto::stReqRecommondFriend * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	std::vector<Player *> vec;
	PlayerManager::getSingleton().getRecommondList(vec,pUser->m_serverID);
	FriendProto::stRecommondFriendList retCmd;
	for (UINT8 i = 0;i < vec.size();++i){
		Player * pOther = vec[i];
		if (pOther != pUser){
			if (pUser->m_friendMgr.getFriend(pOther->getID())){
				continue;	
			}
			
			if (CFriendDBOp::getSingleton().isInApplyList(pUser->getID(),pOther->getID())){
				continue;
			}

			FriendProto::stSynRecommondChar * pSyn = retCmd.add_charlist();
			Player * pOther = vec[i];
			pSyn->set_charid(pOther->getID());
			pSyn->set_name(pOther->getName());
			pSyn->set_job(pOther->m_job);
			pSyn->set_sex(pOther->m_sex);
			pSyn->set_level(pOther->m_level);
			pSyn->set_battlepower(CRankMgr::getSingleton().getBattlePowerByCharID(pOther->getID()));
			pSyn->set_photoid(pOther->m_photoID);
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void FriendModHandle::giveLilian(Player * pUser,FriendProto::stGiveLilian2Friend * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CLilianMgr::getSingleton().giveLilian2Friend(pUser,recvCmd->charid());
}

void FriendModHandle::reqLilianList(Player * pUser,FriendProto::stReqReceiveLilian * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	CLilianMgr::getSingleton().getLilianList(pUser);
}

void FriendModHandle::fetchLilian(Player * pUser,FriendProto::stFetchLilian * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	if (FriendProto::FETCH_ONE == recvCmd->type()) {
		CLilianMgr::getSingleton().fetchLilian(pUser,recvCmd->charid());	
	}
	else {
		CLilianMgr::getSingleton().fetchAllLilian(pUser);	
	}
}

void FriendModHandle::add2ApplyList(Player * pUser,FriendProto::stReqAdd2ApplyList * recvCmd)
{
	if (!pUser || !recvCmd){
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
	CFriendDBOp::getSingleton().addApplyInfo2List(recvCmd->otherid(),applyInfo);

	FriendProto::stAdd2ApplyListResult retCmd;
	retCmd.set_result(FriendProto::stAdd2ApplyListResult::eSUCCESS);
	pUser->sendProtoToMe(retCmd);
}

void FriendModHandle::refuseAddMeFriend(Player * pUser,FriendProto::stRefuseAddFriend * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	CFriendDBOp::getSingleton().delApplyInfo(pUser->getID(),recvCmd->charid());
}

void FriendModHandle::invitePK(Player * pUser,FriendProto::stInvitePKWithFriend * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	
	FriendProto::stInvitePKResult retCmd;

	/*CFriendInfo * pInfo = pUser->m_friendMgr.getFriend(recvCmd->charid());

	if (!pInfo){
		retCmd.set_result(FriendProto::stInvitePKResult::eNotYourFriend);
		pUser->sendProtoToMe(retCmd);
		return ;
	}*/

	Player * pOther = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pOther){
		retCmd.set_result(FriendProto::stInvitePKResult::eNotOnline);
		pUser->sendProtoToMe(retCmd);
		return ;	
	}

	retCmd.set_result(FriendProto::stInvitePKResult::eInviteSuccess);
	pUser->sendProtoToMe(retCmd);

	FriendProto::stAskAgreePKWithFriend askCmd;
	askCmd.set_charid(pUser->getID());
	askCmd.set_name(pUser->getName());
	pOther->sendProtoToMe(askCmd);
}

void FriendModHandle::replyPKInvite(Player * pUser,FriendProto::stReplyFriendPKInvite * recvCmd)
{
	if (!pUser || !recvCmd){
		return ;
	}
	Player * pOther = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	FriendProto::stInvitePKResult retCmd;	
	if (!pOther){
		retCmd.set_result(FriendProto::stInvitePKResult::eNotOnline);
		pUser->sendProtoToMe(retCmd);
		return ;
	}

	if (!recvCmd->agree()){
		retCmd.set_result(FriendProto::stInvitePKResult::eRefuse);
		pOther->sendProtoToMe(retCmd);
		return ;	
	}

	retCmd.set_result(FriendProto::stInvitePKResult::eAgree);
	pOther->sendProtoToMe(retCmd);

	Global::stOne2OnePVPEnterCopyMap cmd;
	cmd.add_memberids(pUser->getID());
	cmd.add_memberids(recvCmd->charid());
	cmd.set_pkwithfriend(true);
	g_globalServer->getConnMgr().broadcastByID(pUser->getGameID(),cmd);
}

void FriendModHandle::reqPropose(Player *pUser, FriendProto::stReqPropose *recvCmd)
{
#undef REJECT_RETURN
#define REJECT_RETURN(msg) do {	\
	packet.set_result(FriendProto::RejectMarry::RESULT_ERROR_##msg);	\
	pUser->sendProtoToMe(packet);	\
	return;	\
} while(0)

#undef MARRY_SET_INFO
#define MARRY_SET_INFO(name) do {	\
	if (recvCmd->info().name()) {	\
		info->set_##name(recvCmd->info().name());	\
	} else {	\
		info->set_##name(0);	\
	}	\
} while(0)

	if (pUser && recvCmd) {
		FriendProto::RejectMarry packet;
		UINT32 inviter_id = pUser->getID();
		UINT32 invitee_id = recvCmd->id();
		CFriendInfo *pInfo = pUser->m_friendMgr.getFriend(invitee_id);
		if (!pInfo) {
			REJECT_RETURN(FRIEND);
		}
		if (!(PlayerManager::getSingleton().getUserByID(invitee_id))) {
			REJECT_RETURN(ONLINE);
		}
		if (pUser->m_sex == pInfo->m_sex) {
			REJECT_RETURN(SEX);
		}
		if (CMarryMgr::getSingleton().getRoomId(pUser->getID())) {
			REJECT_RETURN(MARRIED);
		}
		if (CMarryMgr::getSingleton().getRoomId(recvCmd->id())) {
			REJECT_RETURN(MARRIED);
		}

		Global::stReqPropose innerpacket;
		innerpacket.set_id1(inviter_id);
		innerpacket.set_id2(invitee_id);
		Global::MarryObjInfo *info = innerpacket.mutable_info();
		MARRY_SET_INFO(charid);
		info->set_name(recvCmd->info().name());
		MARRY_SET_INFO(level);
		MARRY_SET_INFO(job);
		MARRY_SET_INFO(sex);
		MARRY_SET_INFO(battlepower);
		MARRY_SET_INFO(headid);
		MARRY_SET_INFO(bodyid);
		MARRY_SET_INFO(weaponid);
		MARRY_SET_INFO(cloackid);
		MARRY_SET_INFO(wingid);
		MARRY_SET_INFO(shoeid);
		MARRY_SET_INFO(fashionhair);
		MARRY_SET_INFO(fashionface);
		MARRY_SET_INFO(fashionbody);
		MARRY_SET_INFO(fashionweapon);
		MARRY_SET_INFO(fashionshoe);
		MARRY_SET_INFO(fashiondecoration);
		MARRY_SET_INFO(effectid);
		MARRY_SET_INFO(wingbreaklv);
		MARRY_SET_INFO(fashionwingid);

		pUser->sendProto2Game(innerpacket);
	}
#undef MARRY_SET_INFO
#undef REJECT_RETURN
}

void FriendModHandle::reqAgreeMarry(Player *pUser, FriendProto::stReqAgreeMarry *recvCmd)
{
#undef MARRY_SET_INFO
#define MARRY_SET_INFO(name, value) do {	\
	if (pUser->value) {	\
		info->set_##name(pUser->value);	\
	} else {	\
		info->set_##name(0);	\
	}	\
} while(0)

	if (pUser && recvCmd) {
		FriendProto::ProposeResult packet;
		UINT32 id = recvCmd->id();
		Player *p = PlayerManager::getSingleton().getUserByID(id);
		if (p) {
			if (((UINT32)recvCmd->result()) == ((UINT32)(FriendProto::stReqAgreeMarry::RESULT_AGREE))) {
				Global::stReqAgreeMarry innerpacket;
				innerpacket.set_id1(id);
				innerpacket.set_id2(pUser->getID());
				Global::MarryObjInfo *info = innerpacket.mutable_info();
				MARRY_SET_INFO(charid, getID());
				info->set_name(pUser->getName());
				MARRY_SET_INFO(level, m_level);
				MARRY_SET_INFO(job, m_job);
				MARRY_SET_INFO(sex, m_sex);
				MARRY_SET_INFO(battlepower, m_battlePower);
				MARRY_SET_INFO(headid, m_headID);
				MARRY_SET_INFO(bodyid, m_bodyID);
				MARRY_SET_INFO(weaponid, m_weaponID);
				MARRY_SET_INFO(cloackid, m_cloackID);
				MARRY_SET_INFO(wingid, m_wingID);
				MARRY_SET_INFO(shoeid, m_shoeID);
				MARRY_SET_INFO(fashionhair, m_fashionHairID);
				MARRY_SET_INFO(fashionface, m_fashionFaceID);
				MARRY_SET_INFO(fashionbody, m_fashionBodyID);
				MARRY_SET_INFO(fashionweapon, m_fashionWeaponID);
				MARRY_SET_INFO(fashionshoe, m_fashionShoeID);
				MARRY_SET_INFO(fashiondecoration, m_fashionDecorationID);
				MARRY_SET_INFO(effectid, m_effectID);
				MARRY_SET_INFO(wingbreaklv, m_wingBreakLv);
				
				pUser->sendProto2Game(innerpacket);

                Global::stRetMarryInfoToCouple cmd;
                cmd.set_husband(id);
                cmd.set_wife(pUser->getID());
                g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
			} else {
				packet.set_result(FriendProto::ProposeResult::RESULT_REFUSE);
				pUser->sendProtoToMe(packet);
			}
		}
	}
#undef MARRY_SET_INFO
}

void FriendModHandle::reqCancelMarry(Player *pUser, FriendProto::stReqCancelMarry *recvCmd)
{
	if (pUser && recvCmd) {
		FriendProto::NotifyCancelMarry packet;
		UINT32 id1 = pUser->getID();
		UINT32 id2 = recvCmd->id();
		Player * pOther = PlayerManager::getSingleton().getUserByID(id2);
		if (pOther) {
			packet.set_id(id1);
			pOther->sendProtoToMe(packet);
		}
        packet.set_id(id2);
        pUser->sendProtoToMe(packet);
	}
}


