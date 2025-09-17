#include "Pub2NormalModHandle.h"
#include "SvrInfo.h"
#include "GameServer.h"
#include "GameUserManager.h"
#include "MapModHandle.h"
#include "PubGame.pb.h"
#include "RoleModHandle.h"
#include "MiscModHandle.h"
#include "GuildModHandle.h"
#include "TaskModHandle.h"
#include "SortListModHandle.h"
#include "GlobalSvrClient.h"
#include "OperateActivityCfg.h"
#include "ArenaConfig.h"
#include "GameEventNotify.h"
#include "MapMgr.h"
#include "Map.h"
#include "CopyMap.h"

extern GameServer * g_gameServer;

Pub2NormalModHandle::Pub2NormalModHandle()
{
	registerHandle(&Pub2NormalModHandle::forwardMe);
	registerHandle(&Pub2NormalModHandle::notifyOnPub);
	registerHandle(&Pub2NormalModHandle::notifyOne2OneResult);
	registerHandle(&Pub2NormalModHandle::notifyRealTimeResult);
	registerHandle(&Pub2NormalModHandle::notifyAddRegisterNum);
	registerHandle(&Pub2NormalModHandle::notifyAttendPreUser);
	registerHandle(&Pub2NormalModHandle::notifySynRoleInfo);
	registerHandle(&Pub2NormalModHandle::synChampionInfo);
}

Pub2NormalModHandle::~Pub2NormalModHandle()
{

}

bool Pub2NormalModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<Pub2NormalModHandle>::handle(pCom,pData,cmdLen,funID); 
}

void Pub2NormalModHandle::forwardMe(TCPComponent * pCom,PubGame::stPubForwardGate * recvCmd)
{
	if (!recvCmd) {
		return ;
	}
// #ifdef _HDZ_DEBUG
// 		Athena::logger->trace("pubgame forwardGate发过来的协议是modID=%d,funID=%d len=%d",recvCmd->modid(),recvCmd->funid(),recvCmd->cmdlen());
// #endif
	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser){
		Game::stGameForwardGate  forwardMe;
		forwardMe.set_charid(pUser->getID());
		forwardMe.set_modid(recvCmd->modid());
		forwardMe.set_funid(recvCmd->funid());
		forwardMe.set_cmdlen(recvCmd->cmdlen());
		forwardMe.set_data(recvCmd->data());
		g_gameServer->getConnMgr().broadcastByID(pUser->getGateID(),forwardMe);
	}else{
		Athena::logger->error("[未找到玩家]pubgame forwardGate发过来的协议是modID=%d,funID=%d",recvCmd->modid(),recvCmd->funid());
	}
}

void Pub2NormalModHandle::notifyOnPub(TCPComponent * pCom,PubGame::stNotifyUserOnPub * recvCmd)
{
	Athena::logger->trace("notifyOnPub  charID=%d onpub=%s", recvCmd->charid(), recvCmd->onpub() ? "true" : "false");

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser)
	{
		pUser->setOnPub(recvCmd->onpub());
		if (recvCmd->onpub() == false)
		{
			pUser->NotifyPubLogout();

			//如果不在任何地图中，加入到地图
			if (pUser->getMap() == NULL)
			{
				UINT32 toMapID = pUser->m_lastIntoMapID;
				CMap *pNewMap = CMapMgr::getSingleton().getMapByID(toMapID);
				if (!pNewMap){
					toMapID = NEW_PERSON_MAPID;
				}
				else if (pNewMap && pNewMap->isCopyMap() &&(!((CCopyMap *)pNewMap)->isGuildHall())){//请求离开的不是公会
					toMapID = NEW_PERSON_MAPID;
				}

				pNewMap = CMapMgr::getSingleton().getMapByID(toMapID);
				if (pNewMap){
						//pNewMap->sendMapInfo(pUser);
					if (pNewMap->add(pUser,pUser->getPos())){
						Athena::logger->trace("name=%s,id=%d切换到新的(name=%s,id=%d)地图成功,当前坐标:(x:%d,y:%d,z:%d)",pUser->getName(),pUser->getID(),pUser->getName(),pUser->getID(),pUser->getPos().m_x,pUser->getPos().m_y,pUser->getPos().m_z);
						if (pNewMap->isShowHero()){
							pUser->m_heroMgr.summonHero();
						}
					}
					else {
						Athena::logger->trace("name=%s,id=%d切换到新的(name=%s,id=%d)地图失败,往新地图添加失败!",pUser->getName(),pUser->getID(),pNewMap->getName(),pNewMap->getID());
						return ;
					}
				}
				else {
					Athena::logger->error("(name:%s,id:%d,从跨服里出来时候,找不到要去的关卡:%d)",pUser->getName(),pUser->getID(),toMapID);
					return;
				}
			}
		}
		else
		{
			Global::stOnPubLeaveTeam leaveTimeCmd;
			leaveTimeCmd.set_charid(pUser->getID());
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(leaveTimeCmd);

			//已在跨服，本服如果在副本，移出地图
			if (pUser->getMap()->isCopyMap())
			{
				pUser->getMap()->remove(pUser);
			}
		}

	}else{
		Athena::logger->error("[未找到玩家]pubgame notifyOnPub charid=%d",recvCmd->charid());
	}
}

void Pub2NormalModHandle::notifyOne2OneResult(TCPComponent * pCom,PubGame::stNotifyOne2OneResult * recvCmd)
{
	Athena::logger->trace("notifyOne2OneResult server1=%d charid1=%d,score1=%d win1=%s",recvCmd->serveridx1(),recvCmd->charid1(),recvCmd->score1(),recvCmd->win1() ? "true" : "false");
	Athena::logger->trace("notifyOne2OneResult server2=%d charid2=%d,score2=%d win2=%s",recvCmd->serveridx2(),recvCmd->charid2(),recvCmd->score2(),recvCmd->win2() ? "true" : "false");

	Global::stOne2OnePVPFinish cmd;
	cmd.set_win1(recvCmd->win1());
	cmd.set_win2(recvCmd->win2());
	cmd.set_pkwithfriend(recvCmd->withfriend());

	if (recvCmd->serveridx1() == g_gameServer->getPubServerIndex() )
	{
		GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid1());
		if (pUser)
		{
			if (recvCmd->withfriend())
			{
				/* code */
			}else
			{
				if (recvCmd->win1())
				{
					pUser->addOne2OnePVPScore(recvCmd->score1());
					pUser->m_countMgr.addDayRefreshValue(ECOUNT_ONE2ONE_WIN_NUM,1);
					pUser->m_sevenDayActivityMgr.addOne2OneWinNum(1);

					Global::stOperateActivityCb globalCmd;
					globalCmd.set_charid(pUser->getID());
					globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::One_Vs_One));
					globalCmd.set_num(1);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);	
				}else{
					pUser->removeOne2OnePVPScore(recvCmd->score1());

					Global::stOperateActivityCb globalCmd;
					globalCmd.set_charid(pUser->getID());
					globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::One_Vs_One));
					globalCmd.set_num(1);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
				}

				stDianfengPkNum notify;
				notify.notify(pUser,1);
				pUser->removeResource(eResource_Lilian,recvCmd->costlilian(),"巅峰");
			}
			cmd.set_charid1(recvCmd->charid1());
			cmd.set_score1(pUser->getOne2OnePVPScore());
		}else{
			Athena::logger->error("[未找到玩家]pubgame notifyOne2OneResult charid=%d",recvCmd->charid1());
		}
	}

	if (recvCmd->serveridx2() == g_gameServer->getPubServerIndex() )
	{
		GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid2());
		if (pUser)
		{
			if (recvCmd->withfriend())
			{
				/* code */
			}else
			{
				if (recvCmd->win2())
				{
					pUser->addOne2OnePVPScore(recvCmd->score2());
					pUser->m_countMgr.addDayRefreshValue(ECOUNT_ONE2ONE_WIN_NUM,1);
					pUser->m_sevenDayActivityMgr.addOne2OneWinNum(1);

					Global::stOperateActivityCb globalCmd;
					globalCmd.set_charid(pUser->getID());
					globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::One_Vs_One));
					globalCmd.set_num(1);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);	
				}else{
					pUser->removeOne2OnePVPScore(recvCmd->score2());

					Global::stOperateActivityCb globalCmd;
					globalCmd.set_charid(pUser->getID());
					globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::One_Vs_One));
					globalCmd.set_num(1);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
				}
				stDianfengPkNum notify;
				notify.notify(pUser,1);
				pUser->removeResource(eResource_Lilian,recvCmd->costlilian(),"巅峰");
			}
			cmd.set_charid2(recvCmd->charid2());
			cmd.set_score2(pUser->getOne2OnePVPScore());
		}else{
			Athena::logger->error("[未找到玩家]pubgame notifyOne2OneResult charid=%d",recvCmd->charid2());
		}
	}

	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}


void Pub2NormalModHandle::notifyRealTimeResult(TCPComponent * pCom,PubGame::stNotifyRealTimeResult * recvCmd)
{
	Athena::logger->trace("pubgame notifyRealTimeResult charid=%d win=%s",recvCmd->charid(), recvCmd->win() ? "true" : "false");

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());
	if (pUser)
	{
		stRealTimeCopymapNum notify;
		ArenaConfig::stRealTimeReward *pReward;
		if (recvCmd->win())
		{
			pReward =  ArenaConfig::getSingleton().getRealTimeReward(true);
		}
		else
		{
			pReward =  ArenaConfig::getSingleton().getRealTimeReward(false);
		}

		if (pReward){
			pUser->addResource((eResource)pReward->m_itemID,pReward->m_itemNum,"积分战场");
		}
		pUser->addReamTimePKNum(1);
		notify.notify(pUser,1);
		pUser->removeResource(eResource_Lilian,recvCmd->costlilian(),"积分战场");
		Global::stOperateActivityCb globalCmd;
		globalCmd.set_charid(pUser->getID());
		globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Three_Vs_Three));
		globalCmd.set_num(1);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
	}else{
		Athena::logger->error("[未找到玩家]pubgame notifyRealTimeResult charid=%d",recvCmd->charid());
	}
}

void Pub2NormalModHandle::notifyAddRegisterNum(TCPComponent * pCom,PubGame::stAddRegisterNum * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	GameUser * pUser = GameUserManager::getSingleton().getUser(recvCmd->charid());  
	if(!pUser){
		return;
	}

	pUser->m_countMgr.addDayRefreshValue(ECOUNT_HERO_LEAGUE_LIMIT_NUM,1);
}

void Pub2NormalModHandle::notifyAttendPreUser(TCPComponent * pCom,PubGame::stNotifyHeroLeagueResult *recvCmd)
{
	if(!recvCmd){
		return ;
	}
	
	Global::stHeroLeagueResult retCmd;
	Global::stPubRoleInfo *info1 = retCmd.mutable_user1();
	if(info1){
		info1->set_charid(recvCmd->user1().charid());
		info1->set_pubcharid(recvCmd->user1().pubcharid());
		info1->set_areacode(recvCmd->user1().areacode());
		info1->set_name(recvCmd->user1().name().c_str());
		info1->set_level(recvCmd->user1().level());
		info1->set_viplv(recvCmd->user1().viplv());
		info1->set_job(recvCmd->user1().job());
		info1->set_sex(recvCmd->user1().sex());
		info1->set_battlepower(recvCmd->user1().battlepower());
		info1->set_pubserverindex(recvCmd->user1().pubserverindex());
		info1->set_score(recvCmd->user1().score());
		info1->set_winorloss(recvCmd->user1().winorloss());
		info1->set_photoid(recvCmd->user1().photoid());
#ifdef HDZ_DEBUG
		Athena::logger->trace("[英雄联赛 Pub2Normal info1]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d, score:%d, winorloss:%d"，recvCmd->user1().charid(),recvCmd->user1().pubcharid(),recvCmd->user1().areacode(),recvCmd->user1().name().c_str(),recvCmd->user1().level(),recvCmd->user1().viplv(),recvCmd->user1().job(),recvCmd->user1().sex(),recvCmd->user1().battlepower(),recvCmd->user1().pubserverindex(),recvCmd->user1().score(),recvCmd->user1().winorloss());
#endif
	}
	Global::stPubRoleInfo *info2 = retCmd.mutable_user2();
	if(info2){
		info2->set_charid(recvCmd->user2().charid());
		info2->set_pubcharid(recvCmd->user2().pubcharid());
		info2->set_areacode(recvCmd->user2().areacode());
		info2->set_name(recvCmd->user2().name().c_str());
		info2->set_level(recvCmd->user2().level());
		info2->set_viplv(recvCmd->user2().viplv());
		info2->set_job(recvCmd->user2().job());
		info2->set_sex(recvCmd->user2().sex());
		info2->set_battlepower(recvCmd->user2().battlepower());
		info2->set_pubserverindex(recvCmd->user2().pubserverindex());
		info2->set_score(recvCmd->user2().score());
		info2->set_winorloss(recvCmd->user2().winorloss());
		info2->set_photoid(recvCmd->user2().photoid());
#ifdef HDZ_DEBUG
		Athena::logger->trace("[英雄联赛 Pub2Normal info2]charid:%d, pubcharid:%d, areacode:%d, name:%s,level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubserverindex:%d, score:%d, winorloss:%d"，recvCmd->user2().charid(),recvCmd->user2().pubcharid(),recvCmd->user2().areacode(),recvCmd->user2().name().c_str(),recvCmd->user2().level(),recvCmd->user2().viplv(),recvCmd->user2().job(),recvCmd->user2().sex(),recvCmd->user2().battlepower(),recvCmd->user2().pubserverindex(),recvCmd->user2().score(),recvCmd->user2().winorloss());
#endif
	}
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);	
}

void Pub2NormalModHandle::notifySynRoleInfo(TCPComponent * pCom,PubGame::stSynPubRoleList *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	Global::stSynRoleList retCmd;
	retCmd.set_tag(1);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
}

void Pub2NormalModHandle::synChampionInfo(TCPComponent * pCom,PubGame::stReqSynChampionInfo *recvCmd)
{
	if(!recvCmd){
		return ;
	}
	
	Global::stReqSynChampionInfo retCmd;
	Global::stSynChampionInfo *info = retCmd.mutable_info();
	if(info){
		info->set_charid(recvCmd->info().charid());                      
		info->set_name(recvCmd->info().name().c_str());                  
		info->set_areacode(recvCmd->info().areacode());                  
		info->set_level(recvCmd->info().level());                        
		info->set_job(recvCmd->info().job());                            
		info->set_sex(recvCmd->info().sex());                            
		info->set_leader(recvCmd->info().leader());                      
		info->set_battlepower(recvCmd->info().battlepower());            
		info->set_pos(recvCmd->info().pos());                            
		info->set_effectid(recvCmd->info().effectid());                  
		info->set_wingbreaklv(recvCmd->info().wingbreaklv());            
		info->set_headid(recvCmd->info().headid());                      
		info->set_bodyid(recvCmd->info().bodyid());                      
		info->set_weaponid(recvCmd->info().weaponid());                  
		info->set_cloackid(recvCmd->info().cloackid());                  
		info->set_wingid(recvCmd->info().wingid());                      
		info->set_shoeid(recvCmd->info().shoeid());                      
		info->set_fashionhair(recvCmd->info().fashionhair());            
		info->set_fashionface(recvCmd->info().fashionface());            
		info->set_fashionbody(recvCmd->info().fashionbody());            
		info->set_fashionweapon(recvCmd->info().fashionweapon());        
		info->set_fashionshoe(recvCmd->info().fashionshoe());            
		info->set_fashiondecoration(recvCmd->info().fashiondecoration());
		info->set_photoid(recvCmd->info().photoid());
	}
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(retCmd);
}
