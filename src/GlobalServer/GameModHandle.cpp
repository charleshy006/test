#include "GameModHandle.h"
#include "TblDataMgr.h"
#include "GuildConfig.h"
#include "SvrInfo.h"
#include "GlobalServer.h"
#include "Player.h"
#include "PlayerManager.h"
#include "AthenaNew.h"
#include "Callback.h"
#include "SqlOperation.h"
#include "MysqlDatabase.h"
#include "Log4cxxHelp.h"
#include "GlobalServer.h"
#include "RoleInfoManager.h"
#include "Global.pb.h"
#include "RechargeMgr.h"
#include "Mail.h"
#include "Misc.pb.h"
#include "Guild.h"
#include "Role.pb.h"
#include "RankMgr.h"
#include "Team.pb.h"
#include "Team.h"
#include "CaptureTreasure.h"
#include "RedisOpHelp.h"
#include "Arena.h"
#include "One2OnePVP.h"
#include "NoticeMgr.h"
#include "DrawHeroScoreMgr.h"
#include "FirstKillMgr.h"
#include "WorldBoss.h"
#include "WorldBossCfg.h"
#include "DrawHeroScoreMgr.h"
#include "AdvEventReport.h"
#include "CompensationSys.h"
#include "ArenaConfig.h"
#include "ItemBoxCfg.h"
#include "QuestionMgr.h"
#include "GuardMgr.h"
#include "CloseBetaWelfareMgr.h"
#include "GloryfashionMgr.h"
#include "Expedition.h"
#include "SysLanguages.h"
#include "EmeraldDreamMgr.h"
#include "OperateActivity.h"
#include "OperateActivityCfg.h"
#include "EmeraldDreamCfg.h"
#include "DiscoverMgr.h"
#include "InviteFriendMgr.h"
#include "HeroLeagueMgr.h"

#include "json/reader.h"
#include "json/writer.h"
#include "HttpClient.h"
#include "HttpParser.h"
#include "md5.h"


extern GlobalServer * g_globalServer;

/*
unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else ;
	return y;
}

std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
				(str[i] == '-') ||
				(str[i] == '_') ||
				(str[i] == '.') ||
				(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			if(i + 2 < length){
				unsigned char high = FromHex((unsigned char)str[++i]);
				unsigned char low = FromHex((unsigned char)str[++i]);
				strTemp += high * 16 + low;
			}
		}
		else strTemp += str[i];
	}
	return strTemp;
}
*/

GameModHandle::GameModHandle()
{
	registerHandle(&GameModHandle::userLoginGameSuccess);
	registerHandle(&GameModHandle::saveRoleData);
	registerHandle(&GameModHandle::processGM);
	registerHandle(&GameModHandle::createGuild);
	registerHandle(&GameModHandle::joinGuild);
	registerHandle(&GameModHandle::broadcastGuildChat);
	registerHandle(&GameModHandle::addGuildContribute);
	registerHandle(&GameModHandle::guildDonateSuccess);
	registerHandle(&GameModHandle::synUserLv);
	registerHandle(&GameModHandle::synCopymapStarNum);
	registerHandle(&GameModHandle::synUserBattlePower);
	registerHandle(&GameModHandle::teamLeaveCopymap);
	registerHandle(&GameModHandle::broadcastTeamChat);
	registerHandle(&GameModHandle::treasureCombine);
	registerHandle(&GameModHandle::buyPeaceModuleSuccess);
	registerHandle(&GameModHandle::reqChangeSvr);
	registerHandle(&GameModHandle::addTreasure);
	registerHandle(&GameModHandle::challengeLeitaiUser);
	registerHandle(&GameModHandle::arenaPKResult);
	registerHandle(&GameModHandle::updateLeitaiUseTime);
	registerHandle(&GameModHandle::captureTreasure);
	registerHandle(&GameModHandle::synGuildContribute);
	registerHandle(&GameModHandle::one2OneFinish);
	registerHandle(&GameModHandle::synUserDrawHeroScore);
	registerHandle(&GameModHandle::equipInfoChg);
	registerHandle(&GameModHandle::teamCopyMapFinish);
	registerHandle(&GameModHandle::setWorldBossCDValue);
	registerHandle(&GameModHandle::setWorldBossBuyNum);
	registerHandle(&GameModHandle::userDecSP);
	registerHandle(&GameModHandle::userDecLilian);
	registerHandle(&GameModHandle::guildTeamCopymapFail);
	registerHandle(&GameModHandle::guildTeamCopymapSuccess);
	registerHandle(&GameModHandle::worldBossFail);
	registerHandle(&GameModHandle::killWorldBoss);
	registerHandle(&GameModHandle::leaveWorldBossMap);
	registerHandle(&GameModHandle::chgBadge);
	registerHandle(&GameModHandle::reqScoreReward);
	registerHandle(&GameModHandle::investgateResult);
	registerHandle(&GameModHandle::synVipLv);
	registerHandle(&GameModHandle::clearArenaCD);
	registerHandle(&GameModHandle::resetArenaChallengeNum);
	registerHandle(&GameModHandle::setGuardHighQuality);
	registerHandle(&GameModHandle::clearOne2OneScoreSuccess);
	registerHandle(&GameModHandle::inspiredSuccess);
	registerHandle(&GameModHandle::quickFinishDecDiamond);
	registerHandle(&GameModHandle::resetInterceptNum);
	registerHandle(&GameModHandle::finishManorFight);
	registerHandle(&GameModHandle::synName);
	registerHandle(&GameModHandle::synUserGloryfashion);
	registerHandle(&GameModHandle::addExpedition);
	registerHandle(&GameModHandle::delExpedition);
	registerHandle(&GameModHandle::fastExpedition);
	registerHandle(&GameModHandle::cancelExpedition);
	registerHandle(&GameModHandle::updateDevilReward);
	registerHandle(&GameModHandle::updateDevilDrawNum);
	registerHandle(&GameModHandle::updateEmeraldDreamDamage);
	registerHandle(&GameModHandle::updateOperateActivity);
	registerHandle(&GameModHandle::OperateActivityRewardCb);
	registerHandle(&GameModHandle::OperateActivityLotteryRewardCb);
	registerHandle(&GameModHandle::reqUserRankInfo);
	registerHandle(&GameModHandle::reqCopyMapInfo);
	registerHandle(&GameModHandle::reqOnPubLeaveTeam);
	registerHandle(&GameModHandle::reqIntegralReward);
	registerHandle(&GameModHandle::reqRefreshNum);
	registerHandle(&GameModHandle::reqMissionCheck);
	registerHandle(&GameModHandle::reqTaskCheck);
	registerHandle(&GameModHandle::reqDisbandTeam);
	registerHandle(&GameModHandle::ReqDrawInviteRewardCb);
	registerHandle(&GameModHandle::updateRegisterInfo);
	registerHandle(&GameModHandle::reqSynRoleList);
	registerHandle(&GameModHandle::synChampionInfo);
	registerHandle(&GameModHandle::reqWorskip);
	registerHandle(&GameModHandle::reqHeroLeagueRedPoint);
	registerHandle(&GameModHandle::notifyInterceptResult);
	registerHandle(&GameModHandle::notifyCoupleMarry);
	registerHandle(&GameModHandle::notifyMarryInfoCb);
	registerHandle(&GameModHandle::notifyForceDivorce);
	registerHandle(&GameModHandle::notifyRejectDivorce);
	registerHandle(&GameModHandle::resetMarryRecord);
	registerHandle(&GameModHandle::reqGuildBlackMarketLv);
	registerHandle(&GameModHandle::reqsynCoupleInfo);
	registerHandle(&GameModHandle::reqSynPhotoID);
	registerHandle(&GameModHandle::reqUseLvlBox);
	registerHandle(&GameModHandle::reqRechangeStatistics);
	registerHandle(&GameModHandle::queryNftResult);
	registerHandle(&GameModHandle::changeNftItemResult);
	registerHandle(&GameModHandle::sendNftVipMail);
}

GameModHandle::~GameModHandle()
{

}

bool GameModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<GameModHandle>::handle(pCom,pData,cmdLen,funID);
}

void GameModHandle::userLoginGameSuccess(TCPComponent * pCom,Global::stUserEnterMapSuccess  * recvCmd)
{
	if (!recvCmd || !pCom) {
		return;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->m_gmType = recvCmd->gmtype();
	pUser->m_level = recvCmd->level();
	pUser->m_sex = recvCmd->sex();
	pUser->m_job = recvCmd->job();
	pUser->m_fetchLilianNum = recvCmd->fetchliliannum();
	pUser->m_serverID = recvCmd->serverid();
	pUser->m_vipLv = recvCmd->viplv();
	pUser->m_headID = recvCmd->headid();
	pUser->m_bodyID = recvCmd->bodyid();
	pUser->m_cloackID = recvCmd->cloackid();
	pUser->m_weaponID = recvCmd->weaponid();
	pUser->m_shoeID = recvCmd->shoeid();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("玩家:%s,shoeID:%d",pUser->getName(),pUser->m_shoeID);
#endif
	pUser->m_wingID = recvCmd->wingid();
	pUser->m_fashionBodyID = recvCmd->fashionbody();
	pUser->m_fashionDecorationID = recvCmd->fashiondecoration();
	pUser->m_fashionFaceID = recvCmd->fashionface();
	pUser->m_fashionHairID = recvCmd->fashionhair();
	pUser->m_fashionShoeID = recvCmd->fashionshoe();
	pUser->m_fashionWeaponID = recvCmd->fashionweapon();
	pUser->m_compensationID = recvCmd->compensationid();
	pUser->m_effectID = recvCmd->effectid();
	pUser->m_wingBreakLv = recvCmd->wingbreaklv();
	pUser->m_pubServerIndex = recvCmd->pubserverindex();
	pUser->m_pubCharID = recvCmd->pubcharid();
	pUser->m_areaCode = recvCmd->areacode();
	pUser->m_photoID = recvCmd->photoid();

	CRechargeMgr::getSingleton().userOnline(pUser);
	MailManager::getSingleton().userOnline(pUser);
	pUser->m_friendMgr.userOnline();
	CGuildMgr::getSingleton().userOnline(pUser);
	ArenaMgr::getSingleton().userOnline(pUser);
	COne2OnePVPMgr::getSingleton().userOnline(pUser);
	CFirstKillMgr::getSingleton().userOnline(pUser);
	CCompensationMgr::getSingleton().userOnline(pUser);
	CGuardMgr::getSingleton().userOnline(pUser);
	CCloseBetaWelfareMgr::getSingleton().userOnline(pUser);

	// AdvEventReport::getSingleton().reportLogin(g_globalServer->getSec(),129,(UINT32)atoi(Athena::global["serverID"].c_str()),pUser->m_deviceID,Athena::global["bundleID"],Athena::global["gameVersion"],pUser->getAccount(),pUser->getName());
}

void GameModHandle::saveRoleData(TCPComponent * pCom,Global::stSaveRoleInfo * recvCmd)
{
	if (!pCom || !recvCmd) {
		return ;
	}

	RoleInfoForSave saveData;
	if (recvCmd->datasize() < sizeof(saveData.m_roleInfo) + sizeof(BinaryStamp)) {
		Athena::logger->error("charID=%d请求保存角色数据失败,发过来的数据太小",recvCmd->charid());
		return ;
	}

	if (recvCmd->datasize() > sizeof(saveData)) {
		Athena::logger->error("charID=%d请求保存角色数据失败,发过来的数据太大",recvCmd->charid());
		return ;
	}

	bcopy(recvCmd->data().data(),&saveData.m_roleInfo,sizeof(saveData.m_roleInfo));
	BinaryStamp* stamp = (BinaryStamp*)(saveData.m_binData);
	RealTime cur;
	stamp->m_time = cur.msecs();
	stamp->m_totalSize = recvCmd->datasize() - sizeof(saveData.m_roleInfo);
	UINT32 bufferCapacity = stamp->m_totalSize;
	if(stamp->m_totalSize > sizeof(saveData.m_binData) - sizeof(BinaryStamp)){
		bufferCapacity = sizeof(saveData.m_binData) - sizeof(BinaryStamp);
		Athena::logger->error("[二进制变量]%s(%d) 存档时 数据过大%d, 超过buffer容量，截断数据进行存储！！实际存储数据%d", recvCmd->account().c_str(), recvCmd->charid(),sizeof(saveData.m_binData) - sizeof(BinaryStamp), bufferCapacity);
	}
	const char * sBin = recvCmd->data().data()  + sizeof(saveData.m_roleInfo);
	byte * dBin = (saveData.m_binData + sizeof(BinaryStamp));
	bcopy(sBin,dBin,bufferCapacity);
	RoleInfoManager::getSingleton().saveRoleInfo(saveData);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("保存数据charID=%d,成功",recvCmd->charid());
#endif

	if (Global::SAVE_CHGSVR == recvCmd->type()){//换服
		Global::stRetRoleInfo  cmd;
		cmd.set_eloaddatatype(Global::eLoadData_ChgSvr);
		Player * pUser = PlayerManager::getSingleton().getUserByID(saveData.m_roleInfo.m_charID);
		if (pUser) {
			cmd.set_account(recvCmd->account());
			cmd.set_account(saveData.m_roleInfo.m_account);
			cmd.set_charid(saveData.m_roleInfo.m_charID);
			cmd.set_ip(pUser->getIP());
			byte* sour = saveData.m_binData;
			BinaryStamp* stamp = (BinaryStamp*)sour;
			cmd.set_datasize(stamp->m_totalSize + sizeof(RoleInfo));
			std::string binData;
			binData.append((char *)&saveData.m_roleInfo,sizeof(RoleInfo));
			binData.append((char *)saveData.m_binData + sizeof(BinaryStamp),stamp->m_totalSize);
			cmd.set_data(binData);
			cmd.set_gateid(pUser->getGateID());
			pUser->setGameID(pUser->m_toGameSvrID);
			g_globalServer->getConnMgr().broadcastByID(pUser->m_toGameSvrID,cmd);
			Athena::logger->trace("玩家name=%s,account=%s,id=%d开始挑战到服务器svrID=%d",pUser->getName(),pUser->getAccount().c_str(),pUser->getID(),pUser->m_toGameSvrID);
			pUser->m_toGameSvrID = 0;
		}
	}
}

void g_testSendMailCB(UINT64 autoID)
{
}

void GameModHandle::processGM(TCPComponent * pCom,Global::stProcessGM *recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		Athena::logger->trace("玩家name=%s,charID=%d发送gm指令失败!",pUser->getName(),pUser->getID());
		return ;
	}

	switch (recvCmd->type()){
		case Global::GMType_SendMail :
			{
				stMailDBInfo mailInfo;
				mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
				std::string title;
				std::string content;
				if (recvCmd->strparam_size() >= 2){
					title = recvCmd->strparam(0);
					content = recvCmd->strparam(1);
				}
				mailInfo.m_receiverID = pUser->getID();
				strncpy(mailInfo.m_receiverName,pUser->getName(),sizeof(mailInfo.m_receiverName));
				mailInfo.m_senderID = 0;
				strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
				if (((recvCmd->intparam_size() % 2) == 0) && (recvCmd->intparam_size() > 0)){
					for (UINT16 i = 0; i < recvCmd->intparam_size(); i += 2){
						if (0 == i){
							mailInfo.m_attachID1 = recvCmd->intparam(i);
							mailInfo.m_attachNum1 = recvCmd->intparam(i + 1);
						}

						if (2 == i){
							mailInfo.m_attachID2 = recvCmd->intparam(i);
							mailInfo.m_attachNum2 = recvCmd->intparam(i + 1);
						}

						if (4 == i){
							mailInfo.m_attachID3 = recvCmd->intparam(i);
							mailInfo.m_attachNum3 = recvCmd->intparam(i + 1);
						}
					}
					mailInfo.m_isFetchAttach = 0;
				}
				strncpy(mailInfo.m_title,title.c_str(),sizeof(mailInfo.m_title));
				strncpy(mailInfo.m_content,content.c_str(),sizeof(mailInfo.m_content));
				mailInfo.m_sendTime = g_globalServer->getSec();
				MailManager::getSingleton().sendMail(mailInfo,g_testSendMailCB);
			}
			break;
		case Global::GMType_OpRedis :
			{
				std::string key;
				std::string op;

				if (recvCmd->strparam_size() >= 2){
					key = recvCmd->strparam(0);
					op = recvCmd->strparam(1);
				}

				if (op == "set"){
					CRedisOpHelp::getSingleton().testGet(key);
				}
				else if( op == "get") {

				}
			}
			break;
		case Global::GMType_SendNotice :
			{
				UINT32 delay = 0;
				UINT32 num = 0;
				UINT32 interval = 0;
				std::string content;
				if (recvCmd->intparam_size() >= 3){
					delay = recvCmd->intparam(0);
					num = recvCmd->intparam(1);
					interval = recvCmd->intparam(2);
				}

				if (recvCmd->strparam_size() > 0){
					content = recvCmd->strparam(0);
				}

				CNoticeMgr::getSingleton().addNotice(1,g_globalServer->getSec(),delay,num,interval,content.c_str());
			}
			break;
		case Global::GMType_BatchSendMail :
			{
				stBatchSendMailTask task;
				task.m_title = "测试发送批量邮件";
				task.m_content = "测试发送批量邮件";
				for (UINT16 i = 0;i < 1000;++i){
					task.m_receiveIDs.insert(i + 1);
				}
				CBatchSendMailMgr::getSingleton().addTask(task);
				Athena::logger->trace("输入测试批量发送邮件指令!");
			}
			break;
		case Global::GMType_ADD_GUILD_ACTIVITY_DEGREE :
			{
				CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
				if (!pGuild){
					Athena::logger->error("还没有公会不能给自己加活跃度!");
					return ;
				}
				UINT32 addNum = 0;
				if (recvCmd->intparam_size() > 0){
					addNum = recvCmd->intparam(0);
				}
				pGuild->addActivityDegree(addNum,"GM指令添加活跃度");
				Athena::logger->trace("测试增加活跃度!");
			}
			break;
	}
}

extern CTblField g_guildTblDef[];

void GameModHandle::createGuild(TCPComponent * pCom,Global::stCreateGuild * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return;
	}

	GuildProto::stCreateGuildError errorRet;
	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByName(const_cast<char *>(recvCmd->name().c_str()));
	if (pGuild){
		errorRet.set_result(GuildProto::CREATE_GUILD_ERROR_NAME_EXIST);
		pUser->sendProtoToMe(errorRet);
		return;
	}

	pGuild = CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if (pGuild){
		errorRet.set_result(GuildProto::CREATE_GUILD_ERROR_HAS_GUILD);
		pUser->sendProtoToMe(errorRet);
		return;
	}

	UINT32 zoneID = g_globalServer->getZoneID();
	UINT32 maxGuildID = CGuildMgr::getSingleton().getCurMaxGuildID();
	stGuildDB data;
	data.m_id = (zoneID << 22)|(++maxGuildID);
	CGuildMgr::getSingleton().incCurMaxGuildID();
	data.m_leaderID = recvCmd->charid();
	data.m_hallLv = 1;
	data.m_prayBuildingLv = 1;
	data.m_shopLv = 1;
	data.m_blackMarketLv = 1;
	data.m_badge =  recvCmd->badge();
	strncpy(data.m_name,recvCmd->name().c_str(),sizeof(data.m_name));

	Athena::CUDSqlCallBack<CGuildMgr,stGuildDB> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CGuildMgr,stGuildDB>(&CGuildMgr::getSingleton(),&CGuildMgr::createGuildCB,false,0,data);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_guildTblDef,(const char *)&data,"GUILD",autoID,pDelaySql);
}

void GameModHandle::joinGuild(TCPComponent *pCom,Global::stForwardJoininGuild * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	GuildProto::stJoinInGuildResult result;
	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		result.set_result(GuildProto::REQ_JOIN_GUILD_ERROR_HAS_GROUP);
		pUser->sendProtoToMe(result);
		return;
	}

	pGuild = CGuildMgr::getSingleton().getGuildByID(recvCmd->guildid());
	if (!pGuild){
		result.set_result(GuildProto::REQ_JOIN_GUILD_ERROR_NOT_EXIST);
		pUser->sendProtoToMe(result);
		return;
	}

	if (pGuild->isFull()){//人数已经满了
		result.set_result(GuildProto::REQ_JOIN_GUILD_ERROR_FULL);
		pUser->sendProtoToMe(result);
		return;
	}

	if (pGuild->isInApplyList(pUser->getID())){
		result.set_result(GuildProto::REQ_JOIN_GUILD_ERROR_HAS_APPLY);
		pUser->sendProtoToMe(result);
		return;
	}

	pGuild->addApplier(pUser);
	result.set_guildid(pGuild->getID());
	result.set_result(GuildProto::REQ_JOIN_GUILD_SUCESS);
	pUser->sendProtoToMe(result);
}

void GameModHandle::guildDonateSuccess(TCPComponent *pCom,Global::stGuildDonateSuccess * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		pGuild->addContribute(recvCmd->addcontribute(),"公会捐赠");
		stGuildDB guildDB;
		pGuild->fillDBdata(guildDB);
		CGuildMgr::getSingleton().updateGuild(guildDB);

		CGuildMember * pMember = pGuild->getGuildMemberByID(pUser->getID());
		if (pMember){
			pMember->addTodayContribute(recvCmd->todaycontribute());

			GuildProto::stDonateResult retCmd;
			retCmd.set_result(GuildProto::stDonateResult::eSUCCESS);
			pUser->sendProtoToMe(retCmd);

			if (GuildProto::stReqDonate::eType1 == recvCmd->donatetype()){
				pGuild->addTrace(GuildProto::eTrace_Donate_Type1,pUser->getID(),pUser->getName());
			}
			else if (GuildProto::stReqDonate::eType2 == recvCmd->donatetype()){
				pGuild->addTrace(GuildProto::eTrace_Donate_Type2,pUser->getID(),pUser->getName());
			}
			else if (GuildProto::stReqDonate::eType3 == recvCmd->donatetype()){
				pGuild->addTrace(GuildProto::eTrace_Donate_Type3,pUser->getID(),pUser->getName());
			}
		}
	}

}

void GameModHandle::addGuildContribute(TCPComponent *pCom,Global::stAddGuildContribute * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		pGuild->addContribute(recvCmd->contribute(),"其它原因");
		stGuildDB guildDB;
		pGuild->fillDBdata(guildDB);
		CGuildMgr::getSingleton().updateGuild(guildDB);

		CGuildMember * pMember = pGuild->getGuildMemberByID(pUser->getID());
		if (pMember){
			pMember->addTodayContribute(recvCmd->todaydaycontribute());
		}
	}
}

void GameModHandle::synGuildContribute(TCPComponent *pCom,Global::stSynMyContribyte * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		CGuildMember * pMember = pGuild->getGuildMemberByID(pUser->getID());
		if (pMember && (pMember->getContribute() != recvCmd->mycontribute())){
			pMember->synContribute(recvCmd->mycontribute());
			stGuildMemberDB memDB;
			pMember->fillDBdata(memDB);
			CGuildMgr::getSingleton().updateGuildMem(memDB);
			GuildProto::stSynMemberContribute cmd;
			cmd.set_charid(pMember->getID());
			cmd.set_num(recvCmd->mycontribute());
			pGuild->sendProto2Mem(cmd);
		}
	}
}

void GameModHandle::broadcastGuildChat(TCPComponent *pCom,Global::stBroadcastGuildChatInfo * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(pUser->getID());
	if (pGuild){
		Role::stChatCmd retCmd;
		retCmd.set_destcharid(pGuild->getID());
		retCmd.set_srccharid(pUser->getID());
		retCmd.set_srccharname(pUser->getName());
		retCmd.set_type(Role::CHAT_TYPE_GUILD);
		retCmd.set_content(recvCmd->content());
		retCmd.set_job(pUser->m_job);
		retCmd.set_sex(pUser->m_sex);
		retCmd.set_viplv(recvCmd->viplv());
		retCmd.set_isvoice(recvCmd->isvoice());
		retCmd.set_voiceurl(recvCmd->voiceurl());
		retCmd.set_voicelenght(recvCmd->voicelenght());
		pGuild->sendProto2Mem(retCmd);
	}
}

void GameModHandle::synUserLv(TCPComponent *pCom,Global::stSynUserLevel * recvCmd)
{
	 if (!pCom || !recvCmd){
	 	return ;
	 }

	 Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	 if (!pUser){
	 	return ;
	 }

	 CRankMgr::getSingleton().updateMyLevel(pUser,recvCmd->lv());
	// if (recvCmd->levelup()){
	// 	AdvEventReport::getSingleton().reportLevelup(g_globalServer->getSec(),129,(UINT32)atoi(Athena::global["serverID"].c_str()),Athena::global["bundleID"],pUser->getAccount(),pUser->getName(),recvCmd->lv());
	// }
}

void GameModHandle::synCopymapStarNum(TCPComponent *pCom,Global::stSynCopymapStarNum * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CRankMgr::getSingleton().updateMyStarNum(pUser,recvCmd->starnum());
}

void GameModHandle::synUserBattlePower(TCPComponent *pCom,Global::stSynUserBattlePower * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->m_maxHP = recvCmd->maxhp();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了同步来最大血量:%d",pUser->m_maxHP);
#endif
	CRankMgr::getSingleton().updateMyBattlePower(pUser,recvCmd->battlepower());
}

void GameModHandle::synUserDrawHeroScore(TCPComponent *pCom,Global::stSynDrawHeroScore   * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CDrawHeroScoreMgr::getSingleton().userIncScore(recvCmd->charid(),recvCmd->score());

	UINT32 score = CDrawHeroScoreMgr::getSingleton().getScoreByID(recvCmd->charid());
	CDrawHeroRankMgr::getSingleton().updateDrawHeroScore(pUser,score);
}

void GameModHandle::broadcastTeamChat(TCPComponent * pCom,Global::stSendTeamChatInfo * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByCharID(recvCmd->charid());
	if (pTeam){
		Role::stChatCmd retCmd;
		retCmd.set_destcharid(pTeam->getID());
		retCmd.set_srccharid(pUser->getID());
		retCmd.set_srccharname(pUser->getName());
		retCmd.set_type(Role::CHAT_TYPE_TEAM);
		retCmd.set_job(pUser->m_job);
		retCmd.set_sex(pUser->m_sex);
		retCmd.set_content(recvCmd->content());
		retCmd.set_isvoice(recvCmd->isvoice());
		retCmd.set_voiceurl(recvCmd->voiceurl());
		retCmd.set_voicelenght(recvCmd->voicelenght());
		pTeam->sendProto2All(retCmd);
	}
}

void GameModHandle::teamLeaveCopymap(TCPComponent * pCom,Global::stTeamLeaveCopyMap * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByID(recvCmd->teamid());
	if (pTeam) {
		pTeam->leaveCopyMap();
	}
}

void GameModHandle::treasureCombine(TCPComponent * pCom,Global::stTreasureCombine * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CCaptureTreasureMgr::getSingleton().treasureCombine(pUser,recvCmd->combinetype());
}

void GameModHandle::buyPeaceModuleSuccess(TCPComponent * pCom,Global::stBuyPeaceModubleTime * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	CCaptureTreasureMgr::getSingleton().setPeaceEndTime(pUser,g_globalServer->getSec() + recvCmd->addtime());
}

void GameModHandle::reqChangeSvr(TCPComponent * pCom,Global::stReqChangeGameSvr * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	pUser->m_toGameSvrID = recvCmd->gamesvrid();

	Global::stConfirmChangeGameSvr retCmd;
	retCmd.set_charid(recvCmd->charid());
	retCmd.set_gamesvrid(recvCmd->gamesvrid());
	pUser->sendProto2Game(retCmd);
	Athena::logger->trace("收到了name=%s,id=%d,account=%s请求换服务器的请求,并返回了确认",pUser->getName(),pUser->getID(),pUser->getAccount().c_str());
}

void GameModHandle::addTreasure(TCPComponent * pCom,Global::stSynAddTreasure * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CCaptureTreasureMgr::getSingleton().addUserRes(pUser,recvCmd->resid(),recvCmd->num());
}

void GameModHandle::challengeLeitaiUser(TCPComponent * pCom,Global::stUserLeitaiChallenge * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	UINT32 charID = 0;
	if (recvCmd->challengecharid() <= 0){
		UINT32 myRank = CRankMgr::getSingleton().getMyBattlePowerRank(pUser->getID());
		UINT16 percent = ArenaConfig::getSingleton().getPercentCoef(recvCmd->mode(),recvCmd->round());

		ArenaConfig::stLeitaiMatch * pMatchInfo = ArenaConfig::getSingleton().getMatchInfo(recvCmd->mode(),myRank);
		if (!pMatchInfo){
			Athena::logger->trace("擂台挑战失败,没有找到匹配配置");
			return ;
		}

		UINT16 challengeRank = (UINT16)((pMatchInfo->m_range * percent / 100.0f) + g_randBetween(pMatchInfo->m_randomMin,pMatchInfo->m_randomMax));
#ifdef _HDZ_DEBUG
		Athena::logger->trace("擂台挑战玩家,挑战模式:%d ,自己的名次:%d,计算出来的对方名次:%d,percent:%d,range:%d,minRand:%d,maxRand:%d,",recvCmd->mode(),myRank,challengeRank,percent,pMatchInfo->m_range,pMatchInfo->m_randomMin,pMatchInfo->m_randomMax);
#endif
		charID = CRankMgr::getSingleton().getBattlePowerCharIDByRank(challengeRank);
	}
	else {
		charID = recvCmd->challengecharid();
	}

	Player * pOther = PlayerManager::getSingleton().getUserByID(charID);
	if (pOther){
		Global::stBeginPKWithOther cmd;
		cmd.set_othercharid(charID);
		cmd.set_mycharid(pUser->getID());
		cmd.set_datasize(0);
		cmd.set_type(Global::stBeginPKWithOther::eLeitai_PK);
		pUser->sendProto2Game(cmd);
	}
	else {
		RoleInfoManager::getSingleton().loadOfflineUserChallege(pUser->getID(),charID,Global::stBeginPKWithOther::eLeitai_PK,0);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("给擂台发送了玩家:%d",charID);
#endif
}

void GameModHandle::arenaPKResult(TCPComponent * pCom,Global::stArenaPKResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	Global::stSendModuleLog webCmd;
	webCmd.set_account(pUser->getAccount());
	webCmd.set_charid(pUser->getID());
	webCmd.set_copymapid(0);
	webCmd.set_level(pUser->m_level);
	webCmd.set_opid(pUser->m_opID);
	webCmd.set_viplv(pUser->m_vipLv);
	UINT16 oldRank = ArenaMgr::getSingleton().getMyRank(pUser->getID());
	if (recvCmd->win()){//胜利
		ArenaMgr::getSingleton().replaceRank(pUser,recvCmd->rank(),recvCmd->historyrank());
		webCmd.set_result(true);
		webCmd.set_oldrank(oldRank);
		webCmd.set_newrank(recvCmd->rank());
	}

	ArenaMgr::getSingleton().changeUserArenaInfo(pUser);
	webCmd.set_module("天梯排行");
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void GameModHandle::resetArenaChallengeNum(TCPComponent * pCom,Global::stNotfiyResetChallengeNum * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	ArenaMgr::getSingleton().resetChallengeNum(pUser);
}

void GameModHandle::updateLeitaiUseTime(TCPComponent * pCom,Global::stUpLeitaiUseTime * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CRankMgr::getSingleton().updateLeitaiUseTime(pUser,recvCmd->usetime());
}

void GameModHandle::captureTreasure(TCPComponent * pCom,Global::stForwardCapture * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CCaptureTreasureMgr::getSingleton().captureUserTreasure(pUser,recvCmd->otherid(),recvCmd->combineid(),recvCmd->restype(),recvCmd->num(),recvCmd->multicapture());
}

void GameModHandle::one2OneFinish(TCPComponent * pCom,Global::stOne2OnePVPFinish * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Global::stSendModuleLog webCmd;
	if (recvCmd->pkwithfriend()){
		webCmd.set_module("切磋");
	}
	else {
		webCmd.set_module("王战");
	}
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid1());
	if (pUser && (pUser->getID() > MAX_ROBOT_CHARID)){
		UINT16 oldRank = COne2OnePVPMgr::getSingleton().getMyRank(pUser->getID());
		if (!recvCmd->pkwithfriend()){
			COne2OnePVPMgr::getSingleton().updateScore(pUser,recvCmd->score1());
		}
		UINT16 newRank = COne2OnePVPMgr::getSingleton().getMyRank(pUser->getID());
		webCmd.set_account(pUser->getAccount());
		webCmd.set_charid(pUser->getID());
		webCmd.set_copymapid(0);
		webCmd.set_level(pUser->m_level);
		webCmd.set_oldrank(oldRank);
		webCmd.set_newrank(newRank);
		webCmd.set_opid(pUser->m_opID);
		webCmd.set_result(recvCmd->win1());
		webCmd.set_viplv(pUser->m_vipLv);
		char szOtherCharID[20] = { 0 };
		snprintf(szOtherCharID,sizeof(szOtherCharID),"%d",recvCmd->charid2());
		g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	}

	pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid2());
	if (pUser && ((pUser->getID() > MAX_ROBOT_CHARID))){
		UINT16 oldRank = COne2OnePVPMgr::getSingleton().getMyRank(pUser->getID());
		if (!recvCmd->pkwithfriend()){
			COne2OnePVPMgr::getSingleton().updateScore(pUser,recvCmd->score2());
		}
		UINT16 newRank = COne2OnePVPMgr::getSingleton().getMyRank(pUser->getID());
		webCmd.set_account(pUser->getAccount());
		webCmd.set_charid(pUser->getID());
		webCmd.set_copymapid(0);
		webCmd.set_level(pUser->m_level);
		webCmd.set_oldrank(oldRank);
		webCmd.set_newrank(newRank);
		webCmd.set_opid(pUser->m_opID);
		webCmd.set_result(recvCmd->win2());
		webCmd.set_viplv(pUser->m_vipLv);
		char szOtherCharID[20] = { 0 };
		snprintf(szOtherCharID,sizeof(szOtherCharID),"%d",recvCmd->charid1());
		g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	}

}

void GameModHandle::equipInfoChg(TCPComponent * pCom,Global::stNotifyCurEquipInfo * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->m_effectID = recvCmd->effectid();
	switch (recvCmd->index()){
		case HEAD_SLOT_INDEX :                //头部
			{
				pUser->m_headID = recvCmd->itemid();
			}
			break;
		case BODY_SLOT_INDEX :                //衣服
			{
				pUser->m_bodyID = recvCmd->itemid();
			}
			break;
		case WEAPON_SLOT_INDEX :              //武器
			{
				pUser->m_weaponID = recvCmd->itemid();
			}
			break;
		case SHOE_SLOT_INDEX :                //鞋子
			{
				pUser->m_shoeID =  recvCmd->itemid();
			}
			break;
		case NECKLACE_SLOT_INDEX :            //项链
			{
			}
			break;
		case CLOAK_SLOT_INDEX :               //披风
			{
				pUser->m_cloackID = recvCmd->itemid();
			}
			break;
		case RING_SLOT_INDEX :                //戒子
			{
			}
			break;
		case WING_SLOT_INDEX  :                //翅膀
			{
				pUser->m_wingID = recvCmd->itemid();
			}
			break;
		case FASHION_HEAD_SLOT_INDEX :
			{
				pUser->m_fashionHairID = recvCmd->itemid();
			}
			break;
		case FASHION_BODY_SLOT_INDEX :
			{
				pUser->m_fashionBodyID = recvCmd->itemid();
			}
			break;
		case FASHION_WEAPON_SLOT_INDEX :
			{
				pUser->m_fashionWeaponID = recvCmd->itemid();
			}
			break;
		case FASHION_SHOE_SLOT_INDEX :       //裤子
			{
				pUser->m_fashionShoeID = recvCmd->itemid();
			}
			break;
		case FASHION_FACE_SLOT_INDEX :
			{
				pUser->m_fashionFaceID = recvCmd->itemid();
			}
			break;
		case FASHION_CLOAK_SLOT_INDEX  :
			{

			}
			break;
		case FASHION_RING_SLOT_INDEX :       //脸饰品
			{
				pUser->m_fashionDecorationID = recvCmd->itemid();
			}
			break;
	}
}

void GameModHandle::teamCopyMapFinish(TCPComponent * pCom,Global::stTeamCopyMapFinish * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	CFirstKillMgr::getSingleton().teamCopymapFinish(recvCmd);
}

void GameModHandle::setWorldBossCDValue(TCPComponent * pCom,Global::stSetClearWorldBossTime * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CWorldBossMgr::getSingleton().clearCD(recvCmd->charid());
}

void GameModHandle::setWorldBossBuyNum(TCPComponent * pCom,Global::stSetWorldBossBuyNum * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CWorldBossMgr::getSingleton().setBuyNum(recvCmd->charid(),recvCmd->buynum());
}

void GameModHandle::userDecSP(TCPComponent * pCom,Global::stUserDecSP * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if (pGuild){
		CGuildMember * pMember = pGuild->getGuildMemberByID(recvCmd->charid());
		if (pMember){
			pMember->onDecSp(recvCmd->sp());
		}
	}
}

void GameModHandle::userDecLilian(TCPComponent * pCom,Global::stUsreDecLilian * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if (pGuild){
		CGuildMember * pMember = pGuild->getGuildMemberByID(recvCmd->charid());
		if (pMember){
			pMember->onDecLilian(recvCmd->lilian());
		}
	}
}

void GameModHandle::guildTeamCopymapFail(TCPComponent * pCom,Global::stGuildCopymapFail * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	if (recvCmd->damageinfo_size() <= 0){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(recvCmd->damageinfo(0).charid());
	if (!pGuild){
		return;
	}

	UINT32 totalDamage = 0;
	for (UINT16 i = 0;i < recvCmd->damageinfo_size();++i){
		const Global::stUserDamagePair & damageInfo = recvCmd->damageinfo(i);
		pGuild->updateMemberDamage(damageInfo.charid(),damageInfo.damage());
		totalDamage += damageInfo.damage();
	}
	pGuild->updateGuildBossHP(recvCmd->copymapid(),totalDamage);
	UINT32 score = 0;
	UINT32 bossID = CGuildConfig::getSingleton().getBossIDByLevelID(recvCmd->copymapid());
	UINT16 stepID = 1;
	stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(bossID,stepID));
	if (pMonsterBase){
		score = (UINT32)(pMonsterBase->m_hp > 0 ? ((totalDamage * 1.0f)/(pMonsterBase->m_hp)) * 10000 : 0);
	}
	CGuildMgr::getSingleton().updateCopymapScore(pGuild->getID(),recvCmd->copymapid(),score);
}

void GameModHandle::guildTeamCopymapSuccess(TCPComponent * pCom,Global::stGuildCopymapKillBoss * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	if (recvCmd->damageinfo_size() <= 0){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(recvCmd->damageinfo(0).charid());
	if (!pGuild){
		return;
	}


	std::vector<std::pair<UINT32,UINT32> > attaches;

	UINT32 totalDamage = 0;
	for (UINT16 i = 0;i < recvCmd->damageinfo_size();++i){
		const Global::stUserDamagePair & damageInfo = recvCmd->damageinfo(i);
		pGuild->updateMemberDamage(damageInfo.charid(),damageInfo.damage());
		totalDamage += damageInfo.damage();

		/*if (damageInfo.charid()){
			attaches.clear();
			UINT32 boxID = CGuildConfig::getSingleton().getCompleteBoxID(recvCmd->copymapid());
			attaches.push_back(std::make_pair(boxID,1));
			MailManager::getSingleton().giveItemByMail("公会副本完成奖励","公会副本完成奖励",attaches,damageInfo.charid());
		}*/
	}

	UINT32 score = 0;
	UINT32 bossID = CGuildConfig::getSingleton().getBossIDByLevelID(recvCmd->copymapid());
	UINT16 stepID = pGuild->getGuildBossStepID(recvCmd->copymapid());
	stMonsterBase * pMonsterBase =  g_monsterBaseMgr.get(stMonsterBase::getFindID(bossID,stepID));
	if (pMonsterBase){
		score = (UINT32)(pMonsterBase->m_hp > 0 ? ((totalDamage * 1.0f)/(pMonsterBase->m_hp)) * 10000 : 0);
	}
	CGuildMgr::getSingleton().updateCopymapScore(pGuild->getID(),recvCmd->copymapid(),score);

	if (pGuild->getGuildBossHP(recvCmd->copymapid()) <= 0){
		return ;
	}

	UINT32 boxID = CGuildConfig::getSingleton().getCompleteBoxID(recvCmd->copymapid());
	stBatchSendMailTask task;
	char szMailContent[50] = { 0 };
	task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_GUILD_COPYMAP_COMPLETE_REWARD);
	snprintf(szMailContent,sizeof(szMailContent),"MailWnd_22;%d",recvCmd->copymapid());
	task.m_content = szMailContent;
	task.m_attaches.push_back(std::make_pair(boxID,1));
	std::vector<UINT32> allMembers;
	pGuild->getAllMember(allMembers);
	for (UINT16 j = 0;j < allMembers.size();++j){
		task.m_receiveIDs.insert(allMembers[j]);
	}

	CBatchSendMailMgr::getSingleton().addTask(task);

	pGuild->killGuildBoss(recvCmd->copymapid(),recvCmd->killcharid());

	if (recvCmd->killcharid()){
		attaches.clear();
		UINT32 boxID = CGuildConfig::getSingleton().getKillBoxID(recvCmd->copymapid());
		attaches.push_back(std::make_pair(boxID,1));
		MailManager::getSingleton().giveItemByMail(CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_GUILD_COPYMAP_KILL_REWARD),"MailWnd_16",attaches,recvCmd->killcharid());

		Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->killcharid());
		if (pUser){
			Athena::logger->trace("name:%s,charID:%d,account:%s,完成了公会副本:%d,击杀了boss",pUser->getName(),pUser->getID(),pUser->getAccount().c_str(),recvCmd->copymapid());
		}
	}
}

void GameModHandle::worldBossFail(TCPComponent * pCom,Global::stWorldBossCopymapFail * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	if (!CWorldBossMgr::getSingleton().isActivityStart()){
		return ;
	}

	if (CWorldBossMgr::getSingleton().getWorldBossHP() <= 0){//已经死亡了不再算
		Athena::logger->trace("世界boss已经死亡了");
		return ;
	}

	UINT32 totalDamage = 0;
	for (UINT16 i = 0;i < recvCmd->damageinfo_size();++i){
		const Global::stUserDamagePair 	& damageInfo = recvCmd->damageinfo(i);
		Player * pUser = PlayerManager::getSingleton().getUserByID(damageInfo.charid());
		if (pUser){
			CWorldBossMgr::getSingleton().updateMyDamage(pUser,damageInfo.damage());
			totalDamage += damageInfo.damage();
		}
	}
	CWorldBossMgr::getSingleton().updateBossHP(totalDamage);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("更新了世界boss血量:%d",recvCmd->bosshp());
#endif
}

void g_killWorldBossSendMailCB(UINT64 autoID)
{}

void GameModHandle::killWorldBoss(TCPComponent * pCom,Global::stKillWorldBoss * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	if (!CWorldBossMgr::getSingleton().isActivityStart()){
		return ;
	}

	if (CWorldBossMgr::getSingleton().getWorldBossHP() <= 0){//已经死亡了不再算
		Athena::logger->trace("世界boss已经死亡了");
		return ;
	}

	for (UINT16 i = 0;i < recvCmd->damageinfo_size();++i){
		const Global::stUserDamagePair 	& damageInfo = recvCmd->damageinfo(i);
		Player * pUser = PlayerManager::getSingleton().getUserByID(damageInfo.charid());
		if (pUser){
			CWorldBossMgr::getSingleton().updateMyDamage(pUser,damageInfo.damage());
		}
	}

	CWorldBossMgr::getSingleton().incBossStep();

	Role::stChatCmd retCmd;
	retCmd.set_type(Role::CHAT_TYPE_NOTICE);
#ifdef _THAILAND_VER
	char buf[400] = { 0 };
#else
	char buf[200] = { 0 };
#endif
	Player * pKiller = PlayerManager::getSingleton().getUserByID(recvCmd->killcharid());
	if (pKiller){
		snprintf(buf,sizeof(buf),"[key=system_notice_3][player=%s][boss=%d]",pKiller->getName(),recvCmd->bossid());
		CWorldBossMgr::getSingleton().setKillerName(pKiller->getName());
		Athena::logger->trace("玩家(name:%s,id:%d,account:%s)完成世界boss,完成了首杀",pKiller->getName(),pKiller->getID(),pKiller->getAccount().c_str());
	}
	retCmd.set_content(buf);
	retCmd.set_srccharid(0);
	retCmd.set_srccharname("系统");
	PlayerManager::getSingleton().sendProto2AllZone(retCmd);

	//发击杀奖励
	stMailDBInfo mailInfo;
	mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM;
	mailInfo.m_receiverID = recvCmd->killcharid();
	mailInfo.m_senderID = 0;
	strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
	mailInfo.m_attachID1 = CWorldBossCfg::getSingleton().getKillReward();
	mailInfo.m_attachNum1 = 1;
	mailInfo.m_isFetchAttach = 0;
	strncpy(mailInfo.m_title,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_WORLD_BOSS_KILL_REWARD),sizeof(mailInfo.m_title));
	strncpy(mailInfo.m_content,"MailWnd_14",sizeof(mailInfo.m_content));
	mailInfo.m_sendTime = g_globalServer->getSec();
	MailManager::getSingleton().sendMail(mailInfo,g_killWorldBossSendMailCB);
}

void GameModHandle::leaveWorldBossMap(TCPComponent * pCom,Global::stUserLeaveWorldBoss * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CWorldBossMgr::getSingleton().setLastLeaveTime(recvCmd->charid(),g_globalServer->getSec());
}

void GameModHandle::chgBadge(TCPComponent * pCom,Global::stForwardChgBadge * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	CGuild *pGuild =  CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if (!pGuild){
		return;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pGuild->changeBadge(pUser,recvCmd->badgeid());
}

void GameModHandle::reqScoreReward(TCPComponent * pCom,Global::stForwardDrawHeroScoreReward * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CDrawHeroScoreMgr::getSingleton().scoreReward(pUser,recvCmd->scoreid());
}

void GameModHandle::investgateResult(TCPComponent * pCom,Global::ForwardInvestgateResult * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	std::ostringstream os;
	for (UINT16 i = 0;i < recvCmd->infos_size();++i){
		os.str("");
		os << "选择的答案:";
		const Global::stForwardInvestgateAnswer & answer =  recvCmd->infos(i);
		QuestionMgr::getSingleton().updateProblemInfo((recvCmd->questid() << 16) | answer.problemid(),answer.answera(),answer.answerb(),answer.answerc(),answer.answerd(),answer.answere(),answer.answerf(),answer.answerg(),answer.answerh(),answer.answeri(),answer.answerj());
		bool bFirst = true;
		if (answer.answera()){
			if (bFirst){
				os << "A";
				bFirst = false;
			}
			else {
				os << "-A";
			}
		}

		if (answer.answerb()){
			if (bFirst){
				os << "B";
				bFirst = false;
			}
			else {
				os << "-B";
			}
		}

		if (answer.answerc()){
			if (bFirst){
				os << "C";
				bFirst = false;
			}
			else {
				os << "-C";
			}
		}

		if (answer.answerd()){
			if (bFirst){
				os << "D";
				bFirst = false;
			}
			else {
				os << "-D";
			}
		}

		if (answer.answere()){
			if (bFirst){
				os << "E";
				bFirst = false;
			}
			else {
				os << "-E";
			}
		}

		if (answer.answerf()){
			if (bFirst){
				os << "F";
				bFirst = false;
			}
			else {
				os << "-F";
			}
		}

		if (answer.answerg()){
			if (bFirst){
				os << "G";
				bFirst = false;
			}
			else {
				os << "-G";
			}
		}

		if (answer.answerh()){
			if (bFirst){
				os << "H";
				bFirst = false;
			}
			else {
				os << "-H";
			}
		}

		if (answer.answeri()){
			if (bFirst){
				os << "I";
				bFirst = false;
			}
			else {
				os << "-I";
			}
		}

		if (answer.answerj()){
			if (bFirst){
				os << "J";
				bFirst = false;
			}
			else {
				os << "-J";
			}
		}

		Athena::logger->trace("[问卷调查结果] name:%s,charID:%d,account:%s,ip:%s,useTime:%d,questID:%d,problemID:%d,%s",pUser->getName(),pUser->getID(),pUser->getAccount().c_str(),pUser->getIP(),recvCmd->usetime(),recvCmd->questid(),answer.problemid(),os.str().c_str());
	}
}

void GameModHandle::synVipLv(TCPComponent * pCom,Global::stSynVipLv * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	pUser->m_vipLv = recvCmd->viplv();
}

void GameModHandle::clearArenaCD(TCPComponent * pCom,Global::stNotifyClearArenaCD *recvCmd)
{
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	ArenaMgr::getSingleton().clearCD(pUser);
}

void GameModHandle::clearInterceptCD(TCPComponent * pCom,Global::stNotifyClearInterceptCD * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CGuardMgr::getSingleton().clearInterceptCD(pUser);
}

void GameModHandle::resetInterceptNum(TCPComponent * pCom,Global::stNotifyResetInterceptNum * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	CGuardMgr::getSingleton().resetInterceptNum(pUser);
}

void GameModHandle::setGuardHighQuality(TCPComponent * pCom,Global::stNotifyGuardQuality * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
	CGuardMgr::getSingleton().summonHighQualityGuard(pUser,recvCmd->quality());
}

void GameModHandle::inspiredSuccess(TCPComponent * pCom,Global::stNotifyGuardInspiredSuccess * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	CGuardMgr::getSingleton().inspired(pUser);
}

void GameModHandle::clearOne2OneScoreSuccess(TCPComponent * pCom,Global::stClearOne2OneScoreSuccess * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	RoleInfoManager::getSingleton().clearOne2OneScore();
}

void GameModHandle::quickFinishDecDiamond(TCPComponent * pCom,Global::stQuickFinishDiamondSuccess * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	CGuardMgr::getSingleton().finishGuard(recvCmd->charid());
	CGuardMgr::getSingleton().delGuarding(recvCmd->charid());
	MiscProto::stQuickFinishGuardResult retCmd;
	retCmd.set_result(MiscProto::stQuickFinishGuardResult::eSuccess);
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pUser){
		pUser->sendProtoToMe(retCmd);
	}
}

void GameModHandle::finishManorFight(TCPComponent * pCom,Global::stFinishManorFight * recvCmd)
{
	if (!recvCmd){
		return;
	}

	CManorInfoMgr::getSingleton().finishFight(recvCmd);
}

void GameModHandle::synName(TCPComponent * pCom,Global::stSynName * recvCmd)
{
	if (!pCom || !recvCmd){
	    return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}

	pUser->setName(recvCmd->name().c_str());

	RoleInfoManager::getSingleton().synRecord(pUser);
	//同步好友信息
	CFriendDBOp::getSingleton().updateOneInfo(pUser);
	pUser->m_friendMgr.synAllFriend();
	//同步工会信息
	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if(pGuild){
		pGuild->updateMemberInfo(pUser);
		pGuild->synAllMenber();
	}

	//同步多人副本
	CFirstKillMgr::getSingleton().synAllRecords(pUser);
	//同步巅峰 1v1
	COne2OnePVPMgr::getSingleton().synAllsRecord(pUser);
	//同步天梯竞技场
	ArenaMgr::getSingleton().synRecord(pUser);
	//同步限时武神
	CDrawHeroRankMgr::getSingleton().synRecord(pUser);
	//同步擂台
	CRankMgr::getSingleton().synRecord(pUser);
	//同步WorldBoss
	CWorldBossMgr::getSingleton().synRecord(pUser);
	//同步英雄联赛
	CHeroLeagueMgr::getSingleton().synRecord(pUser);
	//同步荣耀时装
	CGloryfashionRankMgr::getSingleton().synRecord(pUser);
	//同步翡翠梦境
	CEmeraldDreamMgr::getSingleton().synRecord(pUser);
	//同步夺宝
	CCaptureTreasureMgr::getSingleton().synRecord(pUser); 

}

void GameModHandle::synUserGloryfashion(TCPComponent *pCom,Global::stSynGloryfashionCharID * recvCmd)
{
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}
	CGloryfashionRankMgr::getSingleton().updateGloryfashionTime(pUser,recvCmd->buytime());
}

void GameModHandle::addExpedition(TCPComponent *pCom,Global::stExpeditionRet * recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}

	ExpeditionMgr::getSingleton().addExpedition(pUser->getID(), recvCmd);
}

void GameModHandle::delExpedition(TCPComponent *pCom,Global::stExpeditionRewardRet* recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}

	ExpeditionMgr::getSingleton().delExpedition(pUser->getID(), recvCmd);
}

void GameModHandle::fastExpedition(TCPComponent *pCom,Global::stFastExpeditionRet* recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}

	ExpeditionMgr::getSingleton().fastExpedition(pUser->getID(), recvCmd);
}

void GameModHandle::cancelExpedition(TCPComponent *pCom,Global::stCancelExpeditionRet* recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}

	ExpeditionMgr::getSingleton().cancelExpedition(pUser->getID(), recvCmd);
}

void GameModHandle::updateDevilReward(TCPComponent *pCom,Global::stDevilCallRewardRet* recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}

	DevilCallMgr::getSingleton().updateDevilCallReward(pUser->getID(), recvCmd);
}

void GameModHandle::updateDevilDrawNum(TCPComponent *pCom,Global::stDevilCallRet* recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}

	DevilCallMgr::getSingleton().updateDevilCallDrawNum(pUser->getID(), recvCmd);
}

/*
void GameModHandle::updateEmeraldDreamDamage(TCPComponent *pCom,Global::stUpdateDamageRank* recvCmd)
{
	if(!pCom || !recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!pUser){
		return ;
	}

	CEmeraldDreamMgr::getSingleton().updateEmeraldDreamInfo(pUser, recvCmd->damage());
}
*/

void GameModHandle::updateEmeraldDreamDamage(TCPComponent *pCom,Global::stKillEmeraldDreamBoss * recvCmd)
{
	if(!pCom || !recvCmd){
		return ;
	}
/*
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!pUser){
		return ;
	}
*/
	for (UINT16 i = 0;i < recvCmd->damageinfo_size();++i){
		const Global::stUserDamagePair  & damageInfo = recvCmd->damageinfo(i);
		Player * pUser = PlayerManager::getSingleton().getUserByID(damageInfo.charid());
		if (pUser){
		//	CWorldBossMgr::getSingleton().updateMyDamage(pUser,damageInfo.damage());
			CEmeraldDreamMgr::getSingleton().updateEmeraldDreamInfo(pUser, damageInfo.damage());
		}
	}

//	CEmeraldDreamMgr::getSingleton().updateEmeraldDreamInfo(pUser, recvCmd->damage());
}

void GameModHandle::updateOperateActivity(TCPComponent *pCom,Global::stOperateActivityCb* recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}
	OperateActivityMgr::getSingleton().checkOperateActivity(recvCmd->activityid(),
            recvCmd->charid(), recvCmd->num());
}

void GameModHandle::OperateActivityRewardCb(TCPComponent *pCom,Global::stOperateActivityRewardCb* recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}
    OperateActivity* pActivity = OperateActivityMgr::getSingleton().getOperateActivity(recvCmd->charid());
    if (!pActivity) {
        Athena::logger->error("[MiscModHandle] new error");
        return ;
    }
    pActivity->updateItem(recvCmd->activityid(), recvCmd->itemid(), recvCmd->status() + 1);
    OperateActivityMgr::getSingleton().updateActivity(recvCmd->charid());
}

void GameModHandle::OperateActivityLotteryRewardCb(TCPComponent *pCom,Global::stOperateActivityLotteryRewardCb* recvCmd) {
	if (!pCom || !recvCmd){
		return ;
	}
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}
    OperateActivity* pActivity = OperateActivityMgr::getSingleton().getOperateActivity(recvCmd->charid());
    if (!pActivity) {
        Athena::logger->error("[MiscModHandle] new error");
        return ;
    }
    pActivity->updateActivity(COperateActivityCfg::getSingleton().findIdByType(
                COperateActivityCfg::Diamond_Lottery), 1);
    pActivity->updateItem(COperateActivityCfg::getSingleton().findIdByType(
                COperateActivityCfg::Diamond_Lottery), recvCmd->itemid(), 1);
    OperateActivityMgr::getSingleton().updateActivity(recvCmd->charid());
}

void GameModHandle::reqUserRankInfo(TCPComponent *pCom,Global::stReqUserRankInfo* recvCmd)
{
	if(!pCom || !recvCmd){
		return ;
	}

	CEmeraldDreamMgr::getSingleton().getRankInfo(recvCmd->charid());
}

void GameModHandle::reqCopyMapInfo(TCPComponent *pCom,Global::stCopyMapInfo *recvCmd)
{
	Global::stCopyMapInfoResult retCmd;
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	stEmeraldDreamOpenTimeInfo info = CEmeraldDreamMgr::getSingleton().getCopyMapInfo();
	stChangeId * changeId = CEmeraldDreamCfg::getSingleton().getChangeId(info.m_activityID);
	if(changeId){
		retCmd.set_charid(recvCmd->charid());
		retCmd.set_levelid(changeId->m_levelId);
	}
	pUser->sendProto2Game(retCmd);
}

void GameModHandle::reqOnPubLeaveTeam(TCPComponent *pCom,Global::stOnPubLeaveTeam *recvCmd)
{
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pUser)
	{
		CTeamMgr::getSingleton().leaveTeam(pUser);
	}
}

void GameModHandle::reqRefreshNum(TCPComponent *pCom,Global::stDeductMoneyResult *recvCmd)
{
	if(!pCom || !recvCmd){
		return ;
	}

	//Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());

	CDiscoverMgr::getSingleton().finishRefresh(recvCmd);
}

void GameModHandle::reqIntegralReward(TCPComponent *pCom,Global::stGiveRewardResult * recvCmd)
{
	if(!recvCmd){
		return;
	}

	CDiscoverMgr::getSingleton().getRewardCB(recvCmd);
}

void GameModHandle::reqMissionCheck(TCPComponent *pCom,Global::stReqCheckMission * recvCmd)
{
	if(!recvCmd){
		return ;
	}

	CDiscoverMgr::getSingleton().checkTask(recvCmd->charid(), recvCmd->levelid());
}

void GameModHandle::reqTaskCheck(TCPComponent *pCom,Global::stReqCheckTask * recvCmd)
{
	if(!recvCmd){
		return;
	}

	CDiscoverMgr::getSingleton().checkFightType(recvCmd->charid(),recvCmd->squaretype() ,recvCmd->battletype());
}

void GameModHandle::reqDisbandTeam(TCPComponent *pCom, Global::stReqDisbandTeam *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	CTeam * pTeam = CTeamMgr::getSingleton().getTeamByCharID(recvCmd->charid());
	if(pTeam){
		if (CTeamMgr::getSingleton().removeTeam(pTeam)){
			    SAFE_DELETE(pTeam);
		}
	}

}

void GameModHandle::ReqDrawInviteRewardCb(TCPComponent *pCom, Global::stReqDrawInviteRewardCb *recvCmd) {
	if(!recvCmd){
		return ;
	}
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}
    MiscProto::stRetDrawInviteFriend retCmd;
    retCmd.set_id(recvCmd->id());
    if (recvCmd->res()) {
        retCmd.set_result(MiscProto::stRetDrawInviteFriend::eSuccess);
        InviteFriend * pFriend = CInviteFriendMgr::getSingleton().getInviteFriend(recvCmd->account());
        if (pFriend) {
            pFriend->drawReward(recvCmd->id());
            stInviteFriendInfoDB selfDb;
            pFriend->fillDBData(selfDb);
            CInviteFriendMgr::getSingleton().updateRecord(selfDb);
        }
    } else {
        retCmd.set_result(MiscProto::stRetDrawInviteFriend::eBag_Full);
    }
    pUser->sendProtoToMe(retCmd);
#ifdef _HDZ_DEBUG
    Athena::logger->trace("收到stReqDrawInviteRewardCb");
#endif
}

void GameModHandle::updateRegisterInfo(TCPComponent *pCom, Global::stHeroLeagueResult *recvCmd)
{
	if(!recvCmd){
		return ;
	}

	CHeroLeagueMgr::getSingleton().updateAllRecord(recvCmd);
}

void GameModHandle::reqSynRoleList(TCPComponent *pCom, Global::stSynRoleList *recvCmd)
{
	if(!recvCmd){
		return ;
	}
	CHeroLeagueMgr::getSingleton().sendQualifiedListToPub();

}

void GameModHandle::synChampionInfo(TCPComponent *pCom, Global::stReqSynChampionInfo *recvCmd)
{
	if(!recvCmd){
		return;
	}


	CHeroLeagueMgr::getSingleton().updateChampionInfo(recvCmd);
}

void GameModHandle::reqWorskip(TCPComponent *pCom, Global::stReqWorskip *recvCmd)
{
	if(!recvCmd){
		return;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!pUser){
		return ;
	}

	Global::stWorskipResult retCmd;

	retCmd.set_charid(recvCmd->charid());

	bool exist = CHeroLeagueMgr::getSingleton().getChampionNum();
	if(exist){
		bool same = CHeroLeagueMgr::getSingleton().isSameAreaCode(pUser);
		retCmd.set_issameareacode(same);
	}else{
		retCmd.set_result(Global::stWorskipResult::eNoBody);
	}
	pUser->sendProto2Game(retCmd);
}

void GameModHandle::reqHeroLeagueRedPoint(TCPComponent *pCom, Global::stReqRedPoint *recvCmd)
{
	if(!recvCmd){
		return;
	}

	CHeroLeagueMgr::getSingleton().reqRedPoint(recvCmd);
}

void GameModHandle::notifyInterceptResult(TCPComponent *pCom,Global::stInterceptResultToGlobal * recvCmd)
{
	if(!recvCmd){
		return;
	}

	if (Athena::global["checkPvP"] != "true")
	{
		return;
	}
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (pUser)
	{
		CGuardMgr::getSingleton().interceptResult(pUser,recvCmd->win());
	}
}

void GameModHandle::notifyCoupleMarry(TCPComponent *pCom,Global::stNotifyCoupleMarry * recvCmd) {
	if(!recvCmd){
		return;
	}
    CMarryMgr::getSingleton().coupleMarry(recvCmd->husband(), recvCmd->wife(), recvCmd->id(), recvCmd->husname(), recvCmd->wifename());
}

void GameModHandle::notifyMarryInfoCb(TCPComponent *pCom,Global::stMarryInfoCb * recvCmd) {
	if(!recvCmd){
		return;
	}
    CMarryMgr::getSingleton().updateMarry(recvCmd->roomid(), recvCmd->charid(), recvCmd->id());
}

void GameModHandle::notifyForceDivorce(TCPComponent *pCom,Global::stRetForceDivorce * recvCmd) {
	if(!recvCmd){
		return;
	}
    CMarryMgr::getSingleton().forceDivorce(recvCmd->couplea(), recvCmd->coupleb(), recvCmd->coupleonline());
}

void GameModHandle::notifyRejectDivorce(TCPComponent *pCom,Global::stRetRejectDivorce * recvCmd) {
	if(!recvCmd){
		return;
	}
    CMarryMgr::getSingleton().setDivorceTime(recvCmd->coupleid(), recvCmd->time());
}

void GameModHandle::resetMarryRecord(TCPComponent *pCom,Global::stRetDelUserOnlineMarryInfo * recvCmd) {
	if(!recvCmd){
		return;
	}
	UINT32 roomId = CMarryMgr::getSingleton().getRoomIdByUser(recvCmd->id());
	if (!roomId) {
		Athena::logger->trace("resetMarryRecord not in room, but quit yet");
		return;
	}
	UINT32 husband = 0, wife = 0;
	Role::stNotifyFriendAction retCmd3;
	retCmd3.set_in(false);
	retCmd3.set_charid(recvCmd->id());
	CMarryMgr::getSingleton().getCouple(roomId, husband, wife);
	Player * pHus = PlayerManager::getSingleton().getUserByID(husband);
	if (pHus){
		pHus->sendProtoToMe(retCmd3);
	}
	Player * pWife = PlayerManager::getSingleton().getUserByID(wife);
	if (pWife){
		pWife->sendProtoToMe(retCmd3);
	}

	CMarryMgr::getSingleton().quitRoom(recvCmd->id());

	std::set<UINT32> s;
	Global::stReqQuitWedding cmd;
	CMarryMgr::getSingleton().getRoomFriends(roomId, s);
    for (std::set<UINT32>::iterator it = s.begin();
            it != s.end(); it++) {
        cmd.add_id(*it);
    }
	cmd.set_charid(recvCmd->id());
	cmd.set_levelid(1);
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

void GameModHandle::reqGuildBlackMarketLv(TCPComponent *pCom,Global::stReqSynGuildBlackMarketLv *recvCmd)
{
	if(!recvCmd){
		return;
	}
	Global::stSynGuildBlackMarketLvResult retCmd;

	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if (!pGuild){
		Athena::logger->error("还没有公会!");
		return ;
	}
	retCmd.set_charid(recvCmd->charid());
	retCmd.set_lv(pGuild->getBlackMarketLv());

	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,retCmd);
}

void GameModHandle::reqsynCoupleInfo(TCPComponent *pCom,Global::stSynCoupleInfo *recvCmd)
{
	if(!recvCmd){
		return;
	}

	CMarryMgr::getSingleton().synRecord(recvCmd->charid(),recvCmd->coupleid(),recvCmd->mynewname());
}

void GameModHandle::reqSynPhotoID(TCPComponent *pCom,Global::stSynUserPhotoID * recvCmd)
{
	if(!recvCmd){
		return;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	pUser->m_photoID = recvCmd->photoid();

	RoleInfoManager::getSingleton().synRecord(pUser);

	//同步擂台
	CRankMgr::getSingleton().synRecord(pUser);

	//同步好友信息
	CFriendDBOp::getSingleton().updateOneInfo(pUser);
	pUser->m_friendMgr.synAllFriend();
	//同步工会信息
	CGuild * pGuild = CGuildMgr::getSingleton().getGuildByCharID(recvCmd->charid());
	if(pGuild){
		pGuild->updateMemberInfo(pUser);
		pGuild->synAllMenber();
	}
	//同步英雄联赛
	CHeroLeagueMgr::getSingleton().synRecord(pUser);

	//同步多人副本
	CFirstKillMgr::getSingleton().synAllRecords(pUser);

	//同步巅峰 1v1
	COne2OnePVPMgr::getSingleton().synAllsRecord(pUser);
	//同步夺宝
	CCaptureTreasureMgr::getSingleton().synRecord(pUser);
	//同步WorldBoss
	CWorldBossMgr::getSingleton().synRecord(pUser);

	//同步天梯竞技场
	ArenaMgr::getSingleton().synRecord(pUser);
/*	//同步限时武神
	CDrawHeroRankMgr::getSingleton().synRecord(pUser);
		//同步荣耀时装
	CGloryfashionRankMgr::getSingleton().synRecord(pUser);
	*/
	//同步翡翠梦境
	CEmeraldDreamMgr::getSingleton().synRecord(pUser);
}

void GameModHandle::reqUseLvlBox(TCPComponent *pCom,Global::stUseLvlBox * recvCmd) {
	if(!recvCmd){
		return;
	}
    Global::stUseLvlBoxRes cmd;
    cmd.set_charid(recvCmd->charid());
    cmd.set_bagtype(recvCmd->bagtype());
    cmd.set_instanceid(recvCmd->instanceid());
    cmd.set_maxlvl(CRankMgr::getSingleton().getMaxLevelId());
    g_globalServer->getConnMgr().broadcastByType(GAMESERVER,cmd);
}

void GameModHandle::reqRechangeStatistics(TCPComponent *pCom,Global::stRechangeStatistics * recvCmd)
{
	if(!recvCmd){
		return;
	}

	Json::Value configJson;
	Json::Reader reader;
	std::string msg;

	std::string role_level, vip_level, vip_exp = "0", currency_amount, currency_type = "1", platform_id = "0", access_type = "1", virtual_currency_amount, virtual_currency_amount_ex;
	std::string sign, app_id, app_key, name, server_id, account_id, role_id, role_name, order_id, iap_id, iap_des, ip, channel;  
		
	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
	    return ;
	}
	role_id		=	g_int2string(recvCmd->charid());
	account_id	=	pUser->getAccount();
	
	role_name	=	HttpUtility::URLEncode(pUser->getName());
	role_level	=	g_int2string(pUser->m_level);
	vip_level	=	g_int2string(pUser->m_vipLv);
	ip			=	pUser->getIP();

	virtual_currency_amount = g_int2string(recvCmd->obtaindiamond());
	virtual_currency_amount_ex = g_int2string(recvCmd->givediamond());

	if (reader.parse(recvCmd->data(), configJson)) {
		if (configJson["productID"].isString()) {
			iap_id = configJson["productID"].asString();
		}
		if (configJson["productName"].isString()) {
		 	iap_des = configJson["productName"].asString();
		}
		iap_des	= HttpUtility::URLEncode(iap_des);
		if (configJson["appid"].isString()) {
			app_id = configJson["appid"].asString();
		}
		if (configJson["appkey"].isString()) {
			app_key = configJson["appkey"].asString();
		}
		if (configJson["name"].isString()) {
			name = configJson["name"].asString();
		}
		if (configJson["channel"].isString()) {
			channel = configJson["channel"].asString();
		}
		if (configJson["platform_id"].isString()) {
			platform_id = configJson["platform_id"].asString();
		}
		if (configJson["currency_type"].isString()) {
			currency_type = configJson["currency_type"].asString();
		}
		if (configJson["currency_amount"].isString()) {
		    currency_amount = configJson["currency_amount"].asString();
		}
		if (configJson["orderId"].isString()) {
			order_id = configJson["orderId"].asString();
		}

		if (configJson["serverId"].isString()) {
			server_id = configJson["serverId"].asString();
		}
	}

	UINT32 timestamp = time(NULL);
	std::string unix_timestamp =  g_int2string(timestamp);
	std::string str = app_key + unix_timestamp;

	int  bufLen = strlen(str.c_str());
	char digest[64] = {0};
	utils::Md5(str.c_str(),bufLen,digest);
	utils::HexString::bin2asc(digest,16,digest,64,bufLen);
	digest[bufLen] = 0;
	sign = digest;

	//appid 在5000 至6000之间： http://td.go.cc/pay_server
	//appid 在8000 至9000之间： http://kd-sg.bowinggame.com/pay_server
	UINT32 appid = (UINT32)atoi(app_id.c_str());
	std::string url = "";
	if(appid >= 5000 && appid <= 6000){
		url = "http://td.go.cc/pay_server";	
	}
	if(appid > 6000 && appid <= 9000){
		url = "http://kd-sg.bowinggame.com/pay_server";
	}

	std::string content = "sign=" + sign 
		+ "&timestamp=" + unix_timestamp 
		+ "&channel=" + channel 
		+ "&app_id=" + app_id 
		+ "&server_id=" + server_id 
		+ "&account_id=" + account_id 
		+ "&role_id=" + role_id 
		+ "&role_name=" + role_name 
		+ "&role_level=" + role_level 
		+ "&vip_level=" + vip_level 
		+ "&vip_exp=" + vip_exp 
		+ "&order_id=" + order_id 
		+ "&iap_id=" + iap_id 
		+ "&iap_des=" + iap_des 
		+ "&currency_amount=" + currency_amount 
		+ "&currency_type=" + currency_type 
		+ "&platform_id=" + platform_id 
		+ "&access_type=" + access_type 
		+ "&virtual_currency_amount=" + virtual_currency_amount 
		+ "&virtual_currency_amount_ex=" + virtual_currency_amount_ex 
		+ "&ip=" + ip;

#ifdef _HDZ_DEBUG
	Athena::logger->trace("Player(charid:%d, name:%s)url:%s, content:%s", pUser->getID(), pUser->getName(), url.c_str(), content.c_str());
#endif

	CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
	pRequest->setUrl(url.c_str());
	pRequest->setRequestType(CHttpRequest::kHttpPost);
	pRequest->setRequestData(content.c_str(),content.length());
	std::string configData;
	CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
	
	if (pRes && pRes->isSucceed()){
	    std::vector<char> &retVec = *pRes->getResponseData();
		if (retVec.size() > 0){
			configData.assign(&retVec[0],retVec.size());
		}
		Json::Value configJsonRes;
		Json::Reader readerRes;

		UINT32 result = 0;
		std::string desc = "";
		std::string data = "";
		if (readerRes.parse(configData, configJsonRes)) {
		    if (configJsonRes["result"].isInt()){
		       result = configJsonRes["result"].asInt();
		    }
			if (configJsonRes["desc"].isString()){
				desc = configJsonRes["desc"].asString();
			}
			if (configJsonRes["data"].isString()){
				data = configJsonRes["data"].asString();
			}
		}
		if(0 == result){
			Athena::logger->error("Player(charid:%d, name:%s)configData:%s",pUser->getID(), pUser->getName(), configData.c_str());		
		}else{
			Athena::logger->trace("Player(charid:%d, name:%s) success",pUser->getID(), pUser->getName());
		}
	}
	SAFE_DELETE(pRes);
}

//nft查询结果
void GameModHandle::queryNftResult(TCPComponent *pCom,Global::stGame2GlobalQueryUserNftResult * recvCmd)
{
	Athena::logger->trace("收到GameModHandle::queryNftResult charid=%d", recvCmd->charid());
	Global::stGlobal2WebQueryUserNftResult  webCmd;
	webCmd.set_charid(recvCmd->charid());
	webCmd.set_webconnid(recvCmd->webconnid());
	webCmd.set_result(recvCmd->result());
	webCmd.set_vip(recvCmd->vip());
	for (UINT8 i = 0; i < recvCmd->data_size(); i++)
	{
		const Global::stNftItem  st = recvCmd->data(i);
		Global::stNftItem* p = webCmd.add_data();
		p->set_itemid(st.itemid());
		p->set_itemcnt(st.itemcnt());
		p->set_itemname(st.itemname());
		p->set_url(st.url());

	}
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

//nft道具修改结果
void GameModHandle::changeNftItemResult(TCPComponent *pCom,Global::stGame2GlobalChangeNftItemResult * recvCmd)
{
	Global::stGlobal2WebChangeNftItemResult  webCmd;
	webCmd.set_charid(recvCmd->charid());
	webCmd.set_webconnid(recvCmd->webconnid());
	webCmd.set_result(recvCmd->result());
	g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void NftVipMailCB(UINT64 autoID)
{

}

//发送nftVIP邮件
void GameModHandle::sendNftVipMail(TCPComponent *pCom,Global::stGame2GlobalNftVipGift * recvCmd)
{
	stMailDBInfo mailInfo;
	mailInfo.m_mailType = MiscProto::MAIL_TYPE_SYSTEM_ONEDAY;
	mailInfo.m_receiverID = recvCmd->charid();
	mailInfo.m_senderID = 0;
	strncpy(mailInfo.m_senderName,CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_SYSTEM),sizeof(mailInfo.m_senderName));
	
	for (UINT8 i = 0; i < recvCmd->attaches_size(); i++)
	{
		const Global::stSendMailAttach  st = recvCmd->attaches(i); 

		if (mailInfo.m_attachID1 <= 0){
			mailInfo.m_attachID1 = st.itemid();
			mailInfo.m_attachNum1 = st.num();
			continue;
		}	
		
		if (mailInfo.m_attachID2 <= 0){
			mailInfo.m_attachID2 = st.itemid();
			mailInfo.m_attachNum2 = st.num();
			continue;
		}	
	
		if (mailInfo.m_attachID3 <= 0){
			mailInfo.m_attachID3 = st.itemid();
			mailInfo.m_attachNum3 = st.num();
			continue;
		}	
	}

	mailInfo.m_isFetchAttach = 0;
	strncpy(mailInfo.m_title,CSysLanguages::getSingleton().getLanguage(eTitleID(SYS_LANGUAGE_ID_NFT_VIP_MAIL_TITLE+recvCmd->viplv()-1)),sizeof(mailInfo.m_title));
	strncpy(mailInfo.m_content,"vipemail_1",sizeof(mailInfo.m_content));
	mailInfo.m_sendTime = g_globalServer->getSec() - g_getSecOfDay(g_globalServer->getSec());
	MailManager::getSingleton().sendMail(mailInfo,NftVipMailCB); 
}
