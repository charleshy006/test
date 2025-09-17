#include "GameUser.h"
#include "Log4cxxHelp.h"
#include <stdarg.h>
#include "UtilFunction.h"
#include "GameUserManager.h"
#include "zlib.h"
#include "Map.h"
#include "PacketProcessor.h"
#include "GlobalPacketProcessor.h"
#include "TblDataMgr.h"
#include "GameServer.h"
#include "GlobalSvrClient.h"
#include "Role.pb.h"
#include "MapMgr.h"
#include "CopyMap.h"
#include "MiscCfgMgr.h"
#include "Login.pb.h"
#include "datacenter_log.h"
#include "log_singleton.h"
#include "Game.pb.h"
#include "KVPair.h"
#include "KoreaLog.h"
#include "ArenaConfig.h"
#include "LimitFashionCfg.h"
#include "PubSvrClient.h"
#include "PubGame.pb.h"
#include "HeroMgr.h"
#include "MarryMgr.h"
#include "LimitTimePetCfg.h"
#include "LateWelfareCfg.h"
#include "ResBackCfg.h"
#include "NftConfig.h"

CGameUserCountMgr::CGameUserCountMgr(GameUser * pOwner) : m_pOwner(pOwner)
{}

CGameUserCountMgr::~CGameUserCountMgr()
{}

UINT32  CGameUserCountMgr::curTime()
{
	return g_gameServer->getSec();	
}

void  CGameUserCountMgr::resetValueCB(UINT32 id)
{
	/*if (ECOUNT_DAY_CONSUME == id){
		m_pOwner->m_activityDataMgr.resetActivity(eActivity_Daily_Consume);
		Athena::logger->trace("活动id:%d被重置了",eActivity_Daily_Consume);
	}
	else if (ECOUNT_WEEK_CONSUME == id){
		m_pOwner->m_activityDataMgr.resetActivity(eActivity_Weekly_Consume);
		Athena::logger->trace("活动id:%d被重置了",eActivity_Weekly_Consume);
	}*/
}

CGameOpenFunction::CGameOpenFunction(GameUser * pOwner) : m_pOwner(pOwner)
{}

CGameOpenFunction::~CGameOpenFunction()
{}

UINT16 CGameOpenFunction::getLevel()
{
	return m_pOwner->getLevel();	
}


UINT64   GameUser::USER_TEMPID_GEN = 0;									//玩家连是ID生成器
UINT32   GameUser::USER_GROUPID_GEN = 0;								//玩家的组ID生成器
std::vector<LuckyDrawRecord> GameUser::m_luckyDrawRecordVec;


GameUser::GameUser(const char * account,UINT16 gateID,const char * ip):m_gateID(gateID),m_saveTimer(600,g_randBetween(1,600)),m_oneSec(1),m_10Sec(10),m_roleExtBinMgr(this),m_bagMgr(this),m_taskMgr(this),m_countMgr(this),m_heroMgr(this),m_shopMgr(this),m_boxMgr(this),m_rewardSys(this),m_titleMgr(this),m_spriteSysMgr(this),m_drawMgr(this),m_levelMgr(this),m_leitaiMgr(this),m_rmbShop(this),m_activityDataMgr(this),m_sevenDayActivityMgr(this),m_rechargeActivityMgr(this),m_questInvestigateMgr(this),m_luckShopMgr(this),m_limitFashionDrawMgr(this),m_openFuncMgr(this), m_expeditionMgr(this),m_guildBlackMarketMgr(this),m_monsterMgr(this),m_mountMgr(this),m_photoMgr(this), m_wflfareMgr(this)
{
	if (account){
		strncpy(m_account,account,sizeof(m_account));
	}
	bzero(&m_roleInfo,sizeof(m_roleInfo));
	++USER_TEMPID_GEN;
	setTempID(USER_TEMPID_GEN);
	if (ip){
		strncpy(m_ip,ip,sizeof(m_ip));
	}
	m_startTime = 0;
	m_gmType = 0;
	m_groupID = ++USER_GROUPID_GEN;
	m_zeroRefresh  = true;
	m_iBroadcastIdx = 0;
	m_onlineTime = 0;
	m_guildID = 0;
	m_guildLv = 0;
	m_joinInTime = 0;
	m_blackMacketLv = 0;
	m_eCreateGuildRes = 0;
	m_vipLv = 0;
	m_buyInvestPlan = 0;
	m_buySummonPlan = 0;
	m_buySummonNum = 0;
	m_buyLimitSummonNum = 0;
	m_buyLimitSummonPlan = 0;
	m_recoverSPLilianTime = g_gameServer->getSec();
	m_currentTaskID = 0;
	m_currentTaskStage = 0;
	m_loginTime = 0;
	m_firstLogin = 0;
	m_arenaHistoryRank = (UINT32)-1;
	m_lastHitByNormalTime = 0;
	m_batiEmptyTime = 0;
	m_systemSetting = 0;
	m_roleInfo.m_newBieGuideID = 1;
	m_lastIntoMapID = 0;
	m_beginCollectTime = 0;
	m_deathTime = 0;
	m_compensationID = 0;
	m_extraDamagePer = 0;
	m_createRoleDay = 0;
	m_bRobot = false;
	m_yestodayFirstLoginTime = 0;  //昨天第一次登录时间
	m_cumulativeLogin  = 0;        //累积登录
	m_arenaRank = (UINT32)-1;
	m_lastSpeakTime = 0;
	m_wingBreakLv = 0;
	m_changeNameNum = 1;
    m_bIsOnPubGameServer = false;
	m_normalServerIndex = g_gameServer->getPubServerIndex();
	m_areaCode = g_gameServer->getAreaCode();
	bzero(m_uniqueName, sizeof(m_uniqueName));
    m_inMarryScene = false;
    m_isSetDivorceExpireTime = false;
	bzero(m_netIP, sizeof(m_netIP));
}

GameUser::~GameUser()
{
}

void GameUser::unreg(Global::SaveType saveType)
{
	this->save(saveType);
	GameUserManager::getSingleton().removeUser(this);
	if (getMap()){
		CMap * pMap = getMap();
		getMap()->remove(this);

		if (pMap->isCopyMap() && (pMap->getPlayerCount() == 0)){//是副本,且没有人
			pMap->unLoadMap();	
		}
	}
	NotifyPubLogout();
	Athena::logger->trace("注销%s(%ld)", this->getName(), this->getID());
}


INT32 GameUser::compressSaveData(byte* buf, const UINT32 size)
{
	byte cache[MAX_UZLIB_CHAR];
	bzero(cache, sizeof(cache));
	byte* p = cache;
	int totalSize = 0;
	
	//保存武将
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_heroMgr.saveAllHero(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}

	//保存道具
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_bagMgr.saveAllItem(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}
	
	//保存技能
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_skillMgr.save(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}

	//计数器保存
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_countMgr.save(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}
	
	//保存商店数据
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_shopMgr.saveAllShop(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);	
	}		
	
	//保存了称号数据
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_titleMgr.saveAllTitle(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);	
	}
	
	//保存精灵系统
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_spriteSysMgr.saveData(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize); 
	}
	
	//保存关卡系统
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_levelMgr.save(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize); 
	}
	
	//保存抽奖数据
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_drawMgr.save(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}
	
	//保存任务数据
	if (MAX_UZLIB_CHAR > 1 + totalSize)
	{	
		totalSize += this->m_taskMgr.saveAllTask(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}
	
	//保存了奖励系统数据
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_rewardSys.saveData(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);	
	}
	
	//保存人民币商店数据
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_rmbShop.save(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}
	
	//擂台相关数据
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_leitaiMgr.save(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}
	
	//保存活动相关
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_activityDataMgr.saveAll(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}
	
	//保存充值活动相关数据
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_rechargeActivityMgr.save(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}
	
	//保存问卷调查相关
	if (MAX_UZLIB_CHAR > 1 + totalSize){
		totalSize += this->m_questInvestigateMgr.saveAll(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}

	//保存远征副本数据
	if (MAX_UZLIB_CHAR > 1 + totalSize){	
		totalSize += this->m_expeditionMgr.saveAll(p + totalSize, MAX_UZLIB_CHAR - 1 - totalSize);
	}

	//保存扩充二进制
	if (MAX_UZLIB_CHAR > 1 + totalSize){	
		totalSize += this->m_roleExtBinMgr.saveAll(p + totalSize,MAX_UZLIB_CHAR - 1 - totalSize);
	}

	uLongf zsize = size;
	int ret = compress(buf, &zsize, cache, (uLongf)totalSize);


	if (ret != Z_OK) {
		Athena::logger->error("[二进制变量](id=%d,name=%s )压缩出错 return code: %d",this->getID(),this->getName(),ret);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("%s保存的数据大小为%d",getName(),zsize);
#endif
	return zsize;
}

bool GameUser::uncompressSaveData(const byte * data, const UINT32 size)
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("%s的解压数据大小为%d",getName(),size);
#endif
	byte buffer[MAX_UZLIB_CHAR];
	bzero(buffer, sizeof(buffer));
	uLongf destLong = MAX_UZLIB_CHAR;

	int ret = uncompress(buffer, &destLong, data, size);
#ifdef _HDZ_DEBUG
	switch (ret){
		case Z_MEM_ERROR :
			{
				Athena::logger->error("没有足够的内存");
			};
			break;
		case Z_BUF_ERROR :
			{
				Athena::logger->error("缓存不够大");
			};
			break;
		case Z_DATA_ERROR :
			{
				Athena::logger->error("数据错误");
			}
			break;
	}
	Athena::logger->trace("%s解压出来的大小为:%d",getName(),destLong);
#endif
	if(ret != Z_OK)
	{
		Athena::logger->debug("[二进制变量](id=%d,name=%s)解压出现错误 return code: %d", getID(),getName(),ret);
		return false;
	}
	UINT32 totalSize = 0;
	//加载武将 
	totalSize += this->m_heroMgr.loadAllHero(buffer + totalSize);
	//加载包裹道具
	totalSize += this->m_bagMgr.loadAllItem(buffer + totalSize);
	//加载技能
	totalSize += this->m_skillMgr.load(buffer + totalSize);
	//计数器
	totalSize += this->m_countMgr.load(buffer + totalSize);
	//加载商店数据
	totalSize += this->m_shopMgr.loadAllShop(buffer + totalSize);
	//加载了称号数据
	totalSize += this->m_titleMgr.loadAllTitle(buffer + totalSize);
	//加载精灵相关数据
	totalSize += this->m_spriteSysMgr.loadData(buffer + totalSize);
	//加载关卡系统
	totalSize += this->m_levelMgr.load(buffer + totalSize);
	//加载抽奖系统
	totalSize += this->m_drawMgr.load(buffer + totalSize);
	//加载任务
	totalSize += this->m_taskMgr.loadAllTask(buffer + totalSize);
	//加载奖励相关数据
	totalSize += this->m_rewardSys.loadData(buffer + totalSize);
	//加载人民币商店相关数据
	totalSize += this->m_rmbShop.load(buffer + totalSize);
	//加载擂台相关数据
	totalSize += this->m_leitaiMgr.load(buffer + totalSize);
	//加载活动相关数据
	totalSize += this->m_activityDataMgr.loadAll(buffer + totalSize);
	//加载充值活动数据
	totalSize += this->m_rechargeActivityMgr.load(buffer + totalSize);
	//加载问卷调查相关
	totalSize += this->m_questInvestigateMgr.loadAll(buffer + totalSize);
	//加载远征副本相关数据
	totalSize += this->m_expeditionMgr.loadAll(buffer + totalSize);
	//加载2进制相关数据	
	if (!this->m_roleExtBinMgr.loadAll(buffer + totalSize)){
		return false;
	}
	
	return true;
}

bool GameUser::save(const Global::SaveType savetype)
{
	if (m_bRobot){//是机器人不保存
		return true;
	}
	//跨服不保存 
	if (g_gameServer->isPubGameServer())
	{
		return true;
	}

	if (savetype == Global::SAVE_LOGOUT){
		if (getMap()) {
			m_roleInfo.m_mapID = getMap()->getID();
            if (m_roleInfo.m_mapID > 3) {
                m_roleInfo.m_mapID = 1;
                MarryMgr::getSingleton().delUserToRoom(getID());
            }
			m_roleInfo.m_posX =  getPos().m_x;
			m_roleInfo.m_posY = getPos().m_y;
			m_roleInfo.m_posZ = getPos().m_z;
		}

		/*--------------- 打生态日志 ----------------*/
		uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
		uuzu::logsdk::UserInfo userInfo;
		userInfo.level = getLevel();
		userInfo.passport = getAccount(); //必须填写
		userInfo.uid = g_int2string(getID()); 
		userInfo.clientip = getIP();
		userInfo.exp = m_roleInfo.m_exp;
		userInfo.blackcoin = m_roleInfo.m_giveDiamond;
		userInfo.moneycoin =  m_roleInfo.m_diamond;
		userInfo.gamecoin = m_roleInfo.m_goldCoin;
		userInfo.device = m_deviceID;
		userInfo.viplevel =getVipLv();  
		userInfo.firstintime = m_firstLogin;
		userInfo.version = Athena::global["gameVersion"];
		std::string onlineNum = g_int2string(g_gameServer->getSec() - m_loginTime);
		std::string opID = g_int2string(m_roleInfo.m_opID);
		logger.setPID(opID.c_str());  
		logger.sendDataLog(userInfo,"", "off", onlineNum.c_str(),"","","","","","login");
		////////////////////////////////////////////////

		/*--------------打印韩国日志--------------*/
		CKoreaLogMgr::getSingleton().loginOff(g_gameServer->getSec(),m_roleInfo.m_account,getID(),getLevel());	
		/*--------------打印韩国日志---------------*/
	}
	//保存角色档案信息
	Global::stSaveRoleInfo  saveChar ;
	saveChar.set_account(m_account);
	saveChar.set_charid(getID());
	saveChar.set_type(savetype);
	saveChar.set_gameid(g_gameServer->m_svrInfo.m_svrID);
    saveEnchant();
    m_luckyDrawMgr.save(m_roleInfo.m_luckDrawData);
   	m_guildBlackMarketMgr.saveAllShop(m_roleInfo.m_guildBlackMarketData);
	std::string dailyInviteStr = m_dailyInvite.toString();
    std::string dragonNestStr = m_dragonNest.toString();
	std::string stionCopyMapStr = m_stoneCopyMap.toString();  
	m_monsterMgr.saveData(m_roleInfo.m_monsterInfo);
	m_mountMgr.saveData(m_roleInfo.m_mountInfo);
	m_roleInfo.m_userPhotoID = m_photoMgr.getUsePhotoID();
	m_photoMgr.saveData(m_roleInfo.m_photoInfo);
	std::string walfareStr = m_lateWalfareMgr.toString();    
	std::string resBackStr = m_resBackMgr.toString();   
	std::string levelWflfareTime = m_wflfareMgr.saveTimeData();
	std::string levelWflfare = m_wflfareMgr.saveData();
	bzero(&m_roleInfo.m_dailyInviteInfo,sizeof(m_roleInfo.m_dailyInviteInfo));
    memcpy(m_roleInfo.m_dailyInviteInfo, dailyInviteStr.c_str(), dailyInviteStr.length());
	bzero(&m_roleInfo.m_dragonNestInfo,sizeof(m_roleInfo.m_dragonNestInfo));
    memcpy(m_roleInfo.m_dragonNestInfo, dragonNestStr.c_str(), dragonNestStr.length());
	bzero(&m_roleInfo.m_stoneCopyMapInfo,sizeof(m_roleInfo.m_stoneCopyMapInfo));
	memcpy(m_roleInfo.m_stoneCopyMapInfo, stionCopyMapStr.c_str(), stionCopyMapStr.length());
	bzero(&m_roleInfo.m_latewalfareInfo,sizeof(m_roleInfo.m_latewalfareInfo));
	memcpy(m_roleInfo.m_latewalfareInfo, walfareStr.c_str(), walfareStr.length());
    Athena::logger->trace("save m_roleInfo.m_resBackInfo , length %d", resBackStr.length());
	bzero(&m_roleInfo.m_resBackInfo,sizeof(m_roleInfo.m_resBackInfo));
	memcpy(m_roleInfo.m_resBackInfo, resBackStr.c_str(), resBackStr.length());
	bzero(&m_roleInfo.m_levelWflfareTime,sizeof(m_roleInfo.m_levelWflfareTime));
	memcpy(m_roleInfo.m_levelWflfareTime, levelWflfareTime.c_str(),levelWflfareTime.length());
	bzero(&m_roleInfo.m_levelWflfare,sizeof(m_roleInfo.m_levelWflfare));
	memcpy(m_roleInfo.m_levelWflfare, levelWflfare.c_str(),levelWflfare.length());
	RoleInfoForSave  forSave;
	bzero(&forSave,sizeof(forSave));
	bcopy(&m_roleInfo,&forSave.m_roleInfo,sizeof(m_roleInfo));
	UINT32 binSize = compressSaveData(forSave.m_binData);
	saveChar.set_datasize(sizeof(m_roleInfo) + binSize);
	saveChar.set_data(&forSave,saveChar.datasize());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(saveChar);
	return true;
}

bool GameUser::heartBeat(UINT64 uTime)
{
	CBeing::heartBeat(uTime);
	//目前跨服只检查霸体恢复及实时pvp逻辑，其他不处理
	if (g_gameServer->isPubGameServer())
	{
		if (m_oneSec(g_gameServer->getRealTime())){//一秒
			if (getMap() && getMap()->isCopyMap() && (!((CCopyMap *)getMap())->isGuildHall())){
				checkBati();
				checkCollect();
				checkRelive();
			}
		}
		return true;
	}

	if (m_saveTimer(g_gameServer->getRealTime())){//定时保存
		save(Global::SAVE_TIMETICK);
	}
	
	if (m_oneSec(g_gameServer->getRealTime())){//一秒
		m_onlineTime += 1;
		m_rewardSys.exeEverySec();

		if (m_10Sec(g_gameServer->getRealTime())){//10秒
			if (m_recoverSPLilianTime + s_RECOVER_SP_LILIAN_INTERVAL < g_gameServer->getSec()) {
				m_recoverSPLilianTime = g_gameServer->getSec();
				if (m_roleInfo.m_sp < m_roleState.m_maxSP){//最大体力
					m_roleInfo.m_sp += 1;	

					Role::stSynResource retCmd;
					retCmd.set_restype(eResource_Sp);
					retCmd.set_num(m_roleInfo.m_sp);
					this->sendProto2Me(retCmd);
				}	

				if (m_roleInfo.m_lilian < m_roleState.m_maxLilian){//最大历练

					m_roleInfo.m_lilian += 1;

					Role::stSynResource retCmd;
					retCmd.set_restype(eResource_Lilian);
					retCmd.set_num(m_roleInfo.m_lilian);
					this->sendProto2Me(retCmd);
				}

				if (m_roleInfo.m_trainSp < m_roleState.m_maxTrainSP){//最大调教体力
					m_roleInfo.m_trainSp += 1;	

					Role::stSynResource retCmd;
					retCmd.set_restype(eResource_TrainSp);
					retCmd.set_num(m_roleInfo.m_trainSp);
					this->sendProto2Me(retCmd);
				}	
#ifdef _HDZ_DEBUG
				Athena::logger->trace("玩家name=%s,id=%d恢复了1点体力和1点历练",getName(),getID());
#endif
			}

			if (m_taskMgr.needRefreshDailyTask()){//是否需要刷新任务
				m_taskMgr.refreshDailyTask();	
				m_taskMgr.setNeedRefreshDailyTask(true);
			}
		}

		if (getMap() && getMap()->isCopyMap() && (!((CCopyMap *)getMap())->isGuildHall())){
			if (!isOnPub()) //角色不在跨服才监测
			{
				checkBati();
				checkCollect();
				checkRelive();
			}
			
		}
		CheckGuajiStartClear();
	}
	
	m_monsterMgr.timer(g_gameServer->getSec());

	//零点清零
	struct tm curTm;
	RealTime::getLocalTime(curTm,g_gameServer->getSec());
	if ((curTm.tm_hour == 0)  
			&&(curTm.tm_min == 0)
			&&(curTm.tm_sec < 10)
			&&(!m_zeroRefresh)){
		m_zeroRefresh  = true;
		m_onlineTime = 0;
		m_rewardSys.zeroRefresh();
		calcCreateRoleDay();
		checkNftVipGift();
	}
	else if (curTm.tm_sec > 10){
		m_zeroRefresh  = false;
	}
	return true;
}
//检测霸体恢复
void GameUser::checkBati()
{
	if(m_lastHitByNormalTime + 4 < g_gameServer->getSec()){//4秒内不收到攻击要回复
		if (m_batiEmptyTime && (m_batiEmptyTime + 8 > g_gameServer->getSec())){//霸体变空了,8秒内不加霸体

		}
		else {
			this->addBati(5);
		}
	}

	if (m_batiEmptyTime && (m_batiEmptyTime + 8 < g_gameServer->getSec())){//霸体回复
		m_batiEmptyTime = 0;
		this->addBati(this->getMaxBati());
	}

}

void GameUser::createNormalSkill()
{
	stRoleBase * pRoleBase = g_roleBaseMgr.get(getRoleID());	
	if (pRoleBase){
		CSkill * pSkill = CSkillMgr::createSkill(pRoleBase->m_normalSkill);
		if (pSkill){
			if (!this->m_skillMgr.addSkill(pSkill)){
				SAFE_DELETE(pSkill);
			}	
		}
	}

}

void GameUser::initTestRoleData()
{
	m_roleInfo.m_level = 50;
	m_roleInfo.m_newBieGuideID = 1000;
	m_bagMgr.addItem2Bag(202620,1,0,"创建角色");
	m_bagMgr.addItem2Bag(212620,1,0,"创建角色");
	m_bagMgr.addItem2Bag(222620,1,0,"创建角色");
	m_bagMgr.addItem2Bag(232620,1,0,"创建角色");
	m_bagMgr.addItem2Bag(242620,1,0,"创建角色");
	m_bagMgr.addItem2Bag(252620,1,0,"创建角色");
	m_bagMgr.addItem2Bag(262620,1,0,"创建角色");
	m_bagMgr.addItem2Bag(273000,1,0,"创建角色");
	m_bagMgr.addItem2Bag(102664,1,0,"创建角色");
	m_roleInfo.m_rechargeNum = 150000;

	m_roleInfo.m_goldCoin = 99999999;
	m_roleInfo.m_giveDiamond = 99999999;

	HeroInfo * pHeroInfo = m_heroMgr.createHero(811607,50);
	if (pHeroInfo){
		if (!m_heroMgr.addHeroInfo(pHeroInfo)){
			SAFE_DELETE(pHeroInfo);
		}
		else {
			m_heroMgr.addBattleHero(811607,0);
		}
	}   
	
	pHeroInfo = m_heroMgr.createHero(811617,50);
	if (pHeroInfo){
		if (!m_heroMgr.addHeroInfo(pHeroInfo)){
			SAFE_DELETE(pHeroInfo);
		} 
		else {
			m_heroMgr.addBattleHero(811617,1);
		}
	} 
	
	pHeroInfo = m_heroMgr.createHero(811618,50);
	if (pHeroInfo){
		if (!m_heroMgr.addHeroInfo(pHeroInfo)){
			SAFE_DELETE(pHeroInfo);
		} 
		else {
			m_heroMgr.addBattleHero(811618,2);
		}
	} 
	
	pHeroInfo = m_heroMgr.createHero(811615,50);
	if (pHeroInfo){
		if (!m_heroMgr.addHeroInfo(pHeroInfo)){
			SAFE_DELETE(pHeroInfo);
		} 
		else {
			m_heroMgr.addBattleHero(811615,3);
		}
	}

	m_levelMgr.finishAllPVELevel();

	m_taskMgr.forceAddTaskID(10000);
	m_taskMgr.forceAddTaskID(10001);
	m_taskMgr.forceAddTaskID(10002);
	m_taskMgr.forceAddTaskID(10003);
	m_taskMgr.forceAddTaskID(10004);
	m_taskMgr.forceAddTaskID(10011);

	this->m_systemSetting |= (1l << (Role::eSysSetting_First_Enter_GoldCoin_LV1 - 1));
	this->m_systemSetting |= (1l << (Role::eSysSetting_First_Enter_GoldCoin_LV2 - 1));
	this->m_systemSetting |= (1l << (Role::eSysSetting_First_Enter_GoldCoin_LV3 - 1));
	this->m_systemSetting |= (1l << (Role::eSysSetting_First_Enter_Survival_LV1 - 1));
	this->m_systemSetting |= (1l << (Role::eSysSetting_First_Enter_Survival_LV2 - 1));
	this->m_systemSetting |= (1l << (Role::eSysSetting_First_Enter_Survival_LV3 - 1));
	this->m_systemSetting |= (1l << (Role::eSysSetting_First_Defeated - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_GoldLevel_Introducee - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_EliteLevel_Introducee - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_SurvialLevel_Introducee - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_OpenMap - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_Task_Interrupt - 1));
	
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_Goto_Guild - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_SuperBar_End - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_Got_Title - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_RUSH_LIMIT_LV - 1));
	this->m_systemSetting |= (1l << (Role::eSysSettion_First_First_Enter_MainCity3 - 1));

}

void GameUser::firstLogin()
{
	m_bagMgr.setBagSize(Role::BAG_TYPE_MAIN1,getCountByVip(eVipFun_MainBagNum));
	createNormalSkill();
	/*stRoleBase * pRoleBase = g_roleBaseMgr.get(getRoleID());	
	if (pRoleBase){
		CSkill * pSkill = CSkillMgr::createSkill(pRoleBase->m_normalSkill);
		if (pSkill){
			if (!this->m_skillMgr.addSkill(pSkill)){
				SAFE_DELETE(pSkill);
			}	
		}
	}*/
	m_skillMgr.checkSkillUnlock();	
	m_roleInfo.m_sp = this->getCountByVip(eVipFun_MaxSp);
	m_roleInfo.m_trainSp = this->getCountByVip(eVipFun_MaxTrainSp);
	m_roleInfo.m_lilian = this->getCountByVip(eVipFun_MaxLilian);
	m_drawMgr.checkRefresh();

	m_taskMgr.acceptTask(10000,false,false);
	m_taskMgr.forceFinish(10000,false);
	m_bagMgr.addItem2Bag(200300,1,0,"创建角色");
    if (Athena::global["CountryVersion"] == std::string("YiJie")) {
        //500w 金币, 1w钻石, 1000点vip经验
        addGoldCoin(5000000, "易接创建给金币");
        addDiamond(10000, "易接创建给钻石");
        addVipExp(1000);
        Athena::logger->trace("易接登陆");
    } else {
        addGoldCoin(200000,"创建给金币");	
    }

	//机甲服, 创建号给钻石
	if (Athena::global["nftServer"] == "true") {
		UINT32 diamond = NftConfig::getSingleton().getCreateDiamond(std::string(m_roleInfo.m_account));
		if (diamond>0)
		{
			addDiamond(diamond, "机甲老号创角");
		}
		
	}
	
	if (g_gameServer->isCreateTestRole()){
	
	}
	else {
		this->m_systemSetting |= (1l << (Role::eSysSetting_ShowFashion - 1));
		this->m_systemSetting |= (1l << (Role::eSysSettion_First_Got_Title - 1));
		this->m_systemSetting |= (1l << (Role::eSysSettion_First_RUSH_LIMIT_LV - 1));
	}

	m_roleInfo.m_newBieGuideID = 1;

	//出生给装备
	CItem *pItem = NULL;

#undef CREATE_ADD_ITEM
#define CREATE_ADD_ITEM(itemID) do {\
	UINT32 realItemID = CItem::realItemIDByJobAndSex(itemID,getJob(),getSex());\
	pItem = m_bagMgr.createItem(realItemID,1,0); \
	if (pItem){ \
		pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_EQUIP; \
		pItem->m_itemInfo.m_pos.m_pos = CEquipBag::getSlotIndexByItemType(pItem->m_pItemBase->m_type);\
		if (!m_bagMgr.m_equipBag.addItem(pItem)) {\
			SAFE_DELETE(pItem);\
			Athena::logger->trace("(name=%s,id=%d)创建给道具穿到身上不成功",getName(),getID());\
		}\
	}\
}while(false)

	//CREATE_ADD_ITEM(200300);
	CREATE_ADD_ITEM(210300);
	CREATE_ADD_ITEM(220300);
	CREATE_ADD_ITEM(230300);
	CREATE_ADD_ITEM(240300);
	CREATE_ADD_ITEM(250300);
	CREATE_ADD_ITEM(260300);	

	/*---------韩国项目日志-------------*/	
	CKoreaLogMgr::getSingleton().userLevelup(g_gameServer->getSec(),m_roleInfo.m_account,getID(),getLevel());
	/*---------韩国项目日志-------------*/
	if (g_gameServer->isCreateTestRole()){
		initTestRoleData();
	}
	synMyLevel(true);
}

void GameUser::userOnline()
{
	m_heroMgr.calcAllHero();
	this->setupRoleState(true);
	this->calcVipLv();
    setBackResData();//设置最近4天的找回数据
	UINT32 t_lastLoginTime = g_stringDate2Utc(m_roleInfo.m_lastLoginTime);
	std::string timestamp = g_utc2StringFormat(g_gameServer->getSec());
	strncpy(m_roleInfo.m_lastLoginTime,timestamp.c_str(),sizeof(m_roleInfo.m_lastLoginTime));
	m_loginTime = g_gameServer->getSec();

	if (!g_isTheSameDay(g_gameServer->getSec(),m_yestodayFirstLoginTime)){//刷新累积登录时间
		m_yestodayFirstLoginTime = g_gameServer->getSec();
		m_cumulativeLogin += 1;
	} 

	struct tm _tm;
	if (strptime(m_roleInfo.m_createTime,"%Y-%m-%d %H:%M:%S",&_tm)) {
		m_firstLogin = mktime(&_tm); 
#ifdef _HDZ_DEBUG
		Athena::logger->trace("firstLoginTime:%d",m_firstLogin);
#endif
	}
	
	calcCreateRoleDay();
	//计算体力
	UINT32 recoverPassTime = g_gameServer->getSec() > this->m_recoverSPLilianTime ? g_gameServer->getSec() - this->m_recoverSPLilianTime : 0;
	UINT32 addNum = recoverPassTime / s_RECOVER_SP_LILIAN_INTERVAL;
	UINT32 recoverPassTimeMod = recoverPassTime % s_RECOVER_SP_LILIAN_INTERVAL;

	if (addNum) {
		this->m_recoverSPLilianTime = 	g_gameServer->getSec() - recoverPassTimeMod;//+ s_RECOVER_SP_LILIAN_INTERVAL * addNum;
		if (m_roleInfo.m_sp + addNum <= m_roleState.m_maxSP){
			m_roleInfo.m_sp += addNum;
		}
		else {
			if (m_roleInfo.m_sp < m_roleState.m_maxSP){
				m_roleInfo.m_sp = m_roleState.m_maxSP;
			}
		}	

		if (m_roleInfo.m_trainSp + addNum <= m_roleState.m_maxTrainSP){
			m_roleInfo.m_trainSp += addNum;
		}
		else {
			if (m_roleInfo.m_trainSp < m_roleState.m_maxTrainSP){
				m_roleInfo.m_trainSp = m_roleState.m_maxTrainSP;
			}
		}	
		
		if (m_roleInfo.m_lilian + addNum <= m_roleState.m_maxLilian){
			m_roleInfo.m_lilian += addNum;
		}
		else {
			if (m_roleInfo.m_lilian < m_roleState.m_maxLilian) {
				m_roleInfo.m_lilian = m_roleState.m_maxLilian;
			}
		}

#ifdef _HDZ_DEBUG
		Athena::logger->trace("玩家上线增加了%d点体力/历练/调教体力,现在的体力/历练/调教体力分别为%d,%d,%d",addNum,m_roleInfo.m_sp,m_roleInfo.m_lilian,m_roleInfo.m_trainSp);
#endif
	}
	
	Role::stSynRoleCreateTime synRoleTimeCmd;
	synRoleTimeCmd.set_createtime(m_firstLogin);
	this->sendProto2Me(synRoleTimeCmd);

	this->m_activityDataMgr.onUserLogin();
	m_levelMgr.checkDataRefresh();
	this->CheckGuajiStartClear(false);
	
	Role::stSendCharCreateTime createTimeCmd;
    createTimeCmd.set_createtimestr(m_roleInfo.m_createTime);
	this->sendProto2Me(createTimeCmd);
	
	sendMainData2Me();
	m_bagMgr.synAllData();
	m_heroMgr.synAllHero();
	m_titleMgr.userOnline();
	this->m_spriteSysMgr.userOnline();
	Login::stRetServerTime retCmd;
	retCmd.set_svrtime(g_gameServer->getSec());
	this->sendProto2Me(retCmd);
	m_rewardSys.userOnline();
	m_levelMgr.userOnline();
	m_skillMgr.synAllSkill();
	m_taskMgr.userOnline();
	m_taskMgr.synAllTask();
	m_mountMgr.userOnline();

	//向客户端通知跨服唯一ID
	this->NotifyPubUniqueID();

	Role::stNotifyNewBieGuideStep newBieGuideCmd;
	newBieGuideCmd.set_stepid(m_roleInfo.m_newBieGuideID);
	newBieGuideCmd.set_substepid(m_roleInfo.m_subNewBieGuideID);
	sendProto2Me(newBieGuideCmd);
	
	TaskProto::stNotifyCurrentTask curTaskCmd;
	curTaskCmd.set_curtaskid(this->m_currentTaskID);
	curTaskCmd.set_curstate(this->m_currentTaskStage);
	sendProto2Me(curTaskCmd);

	m_shopMgr.sendShopList2Me(CShopMgr::SHOP_NORMAL);
	m_rmbShop.synRMBInfo2Client2(t_lastLoginTime);

	m_rmbShop.synRMBInfo2Client();

	Role::stSynSystemSetting systemSettingCmd;
	systemSettingCmd.set_systemsetting(m_systemSetting);
	this->sendProto2Me(systemSettingCmd);
	
	this->m_sevenDayActivityMgr.addLoginDay(getCreateRoleDay());
	this->m_sevenDayActivityMgr.userOnline();
	this->m_activityDataMgr.sendAllAvaibleActivity();
	this->m_questInvestigateMgr.userOnline();
	this->m_luckShopMgr.userOnline();
    sendLimitPetActivity();

	Role::stDataInitFinish dataFinishCmd;
	dataFinishCmd.set_result(true);
	dataFinishCmd.set_ip(m_netIP);
	this->sendProto2Me(dataFinishCmd);

    checkLateWalfare();
    this->m_resBackMgr.checkBackRes(this);
    resetMarryRecord();
	
	m_wflfareMgr.sendHasGotReward();

	if (Athena::global["nftServer"] == "true")
	{
		Role::stNftVipNtf cmd;
		cmd.set_isnftvip(m_roleInfo.m_nftVip);
		this->sendProto2Me(cmd);
	}
	this->checkNftVipGift();
}

void GameUser::checkNftVipGift()
{
	if (Athena::global["nftServer"] == "true" )
	{
		if (m_roleInfo.m_nftVip > 0 && !g_isTheSameDay(g_gameServer->getSec(), m_roleInfo.m_nftVipGiftTime))
		{
			Global::stGame2GlobalNftVipGift cmd;
			cmd.set_charid(m_roleInfo.m_charID);
			cmd.set_viplv(m_roleInfo.m_nftVip);

			std::map<UINT32,UINT32> m = NftConfig::getSingleton().getVipMailItemMap(m_roleInfo.m_nftVip);
			std::map<UINT32,UINT32>::iterator it = m.begin();
			for (;it != m.end(); it++)
			{ 
				Global::stSendMailAttach * pSyn = cmd.add_attaches();
				if (pSyn){
					pSyn->set_itemid(it->first);
					pSyn->set_num(it->second);
				}
			}
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			
			m_roleInfo.m_nftVipGiftTime = g_gameServer->getSec();
		}
	}
}

void GameUser::fillMainData(Role::stRoleMainData * pRoleMainData)
{
	pRoleMainData->set_charid(m_roleInfo.m_charID);
	pRoleMainData->set_exp(m_roleInfo.m_exp);
	pRoleMainData->set_goldcoin(m_roleInfo.m_goldCoin);
	pRoleMainData->set_level(m_roleInfo.m_level);
	pRoleMainData->set_name(getName());
	pRoleMainData->set_pointcard(m_roleInfo.m_diamond + m_roleInfo.m_giveDiamond);
	pRoleMainData->set_curhp(m_maxHP);
	pRoleMainData->set_curmp(m_maxMP);
	pRoleMainData->set_exp(m_roleInfo.m_exp);
	pRoleMainData->set_reputaion(m_roleInfo.m_reputaion);
	pRoleMainData->set_honour(m_roleInfo.m_honour);
	pRoleMainData->set_braveval(m_roleInfo.m_braveVal);
	pRoleMainData->set_peakcertificate(m_roleInfo.m_peakCertificate);
	pRoleMainData->set_hufu(m_roleInfo.m_hufu);
	pRoleMainData->set_yuntie(m_roleInfo.m_yuntie);
	pRoleMainData->set_heishijiejing(m_roleInfo.m_heishijiejing);
	pRoleMainData->set_todayrecharge(m_countMgr.getDayRefreshValue(ECOUT_TODAY_RECHARGE));
	pRoleMainData->set_totalrecharge(m_roleInfo.m_rechargeNum + m_roleInfo.m_giveRechargeNum);
	pRoleMainData->set_job(m_roleInfo.m_job);
	pRoleMainData->set_sex(m_roleInfo.m_sex);
	pRoleMainData->set_sp(m_roleInfo.m_sp);
	pRoleMainData->set_lilian(m_roleInfo.m_lilian);
	pRoleMainData->set_totalconsume(m_roleInfo.m_totlaConsume);
	pRoleMainData->set_juexingexp(m_roleInfo.m_juexingExp);
	pRoleMainData->set_juexingnum(m_countMgr.getDayRefreshValue(ECOUNT_EQUIP_JUEXING));
	pRoleMainData->set_battlepower(m_roleInfo.m_battlePower);
	pRoleMainData->set_headid(m_roleInfo.m_headID);
	pRoleMainData->set_faceid(m_roleInfo.m_faceID);
	pRoleMainData->set_studyheroskillnum(m_countMgr.getDayRefreshValue(ECOUNT_HERO_STUDY_SKILL_NUM));
	pRoleMainData->set_changenamenum(m_changeNameNum);
	pRoleMainData->set_integral(m_roleInfo.m_integral);
	pRoleMainData->set_soulfire(m_roleInfo.m_soulFire);
	pRoleMainData->set_coupleid(m_roleInfo.m_coupleId);
	pRoleMainData->set_lifeessence(m_roleInfo.m_lifeEssence);
	pRoleMainData->set_guidcrystal(m_roleInfo.m_guidCrystal);
	pRoleMainData->set_photoid(m_photoMgr.getUsePhotoID());
	pRoleMainData->set_trainsp(m_roleInfo.m_trainSp);
	pRoleMainData->set_guajistart(m_roleInfo.m_guajiStart);
	pRoleMainData->set_guajiend(m_roleInfo.m_guajiEnd);
	pRoleMainData->set_guajitodaycount(m_roleInfo.m_guajiTime>0?1:0);
	if(m_roleInfo.m_charID <= MAX_ROBOT_CHARID){
		pRoleMainData->set_isrobot(true);
	}else{
		pRoleMainData->set_isrobot(false);
	}
	
	if(m_countMgr.getDayRefreshValue(ECOUNT_REWARD_RECEIVE_NUM) >= 1){
	    pRoleMainData->set_isreceive(true);
	}else{
		pRoleMainData->set_isreceive(false);
	}

	UINT32 remainTime = 0;//m_roleInfo.m_leaveGuildTime + LEAVE_GUILD_COOLDOWN > g_gameServer->getSec() ? m_roleInfo.m_leaveGuildTime + LEAVE_GUILD_COOLDOWN - g_gameServer->getSec() : 0; 
	pRoleMainData->set_joinguildremaintime(remainTime);

	for (UINT16 i = 1;i < m_roleState.m_battleProps.size(); ++i){
		Role::stBattleProp * pSynProp = pRoleMainData->add_battleprop();
		if (pSynProp){
			pSynProp->set_propid(i);
			pSynProp->set_propval(m_roleState.m_battleProps[i]);
		}
	}
	
	pRoleMainData->set_teamcopymapnum(m_levelMgr.getTeamCopymapNum());
	pRoleMainData->set_realtimepknum(m_countMgr.getDayRefreshValue(ECOUNT_REAL_TIME_NUM));
	if (m_roleInfo.m_bitmask & BITMASK_GOT_FIRST_RECHARGE_REWARD){
		pRoleMainData->set_gotfstrechargereward(true);
	}
	else {
		pRoleMainData->set_gotfstrechargereward(false);
	}

	if (m_roleInfo.m_bitmask & BITMASK_GOT_10FIRST_RECHARGE_REWARD){
		pRoleMainData->set_gotfst10rechargereward(true);
	}
	else {
		pRoleMainData->set_gotfst10rechargereward(false);
	}

	pRoleMainData->set_teamcopymapresetnum(m_countMgr.getDayRefreshValue(ECOUNT_TEAM_COPYMAP_RESETNUM));
	pRoleMainData->set_realtimeresetnum(m_countMgr.getDayRefreshValue(ECOUNT_REALTIME_RESETNUM));
}

void GameUser::sendBattleProp()
{
	Role::stSynBattleProp retCmd;
	for (UINT16 i = 1;i < m_roleState.m_battleProps.size(); ++i){
		Role::stBattleProp * pSynProp = retCmd.add_battleprop();
		if (pSynProp){
			pSynProp->set_propid(i);
			pSynProp->set_propval(m_roleState.m_battleProps[i]);
		}
	}
	retCmd.set_battlepower(m_roleInfo.m_battlePower);
	this->sendProto2Me(retCmd);
}

void GameUser::sendMainData2Me()
{
	Role::stInitMainData initCmd;
	Role::stRoleMainData * roleMainData = initCmd.mutable_data();
	fillMainData(roleMainData);
	sendProto2Me(initCmd);
}


bool GameUser::initData(const RoleInfo & roleInfo,byte * binData,UINT32 cmdLen)
{
	bcopy(&roleInfo,&m_roleInfo,sizeof(m_roleInfo));
    loadEnchant();
    m_luckyDrawMgr.load(m_roleInfo.m_luckDrawData);
    m_dailyInvite.fromString(m_roleInfo.m_dailyInviteInfo);
    m_dragonNest.fromString(m_roleInfo.m_dragonNestInfo);
	m_guildBlackMarketMgr.loadAllShop(m_roleInfo.m_guildBlackMarketData);
	m_monsterMgr.loadData(m_roleInfo.m_monsterInfo);
    m_stoneCopyMap.fromString(m_roleInfo.m_stoneCopyMapInfo);
    m_lateWalfareMgr.fromString(m_roleInfo.m_latewalfareInfo);
    m_resBackMgr.fromString(m_roleInfo.m_resBackInfo);
	m_mountMgr.loadData(m_roleInfo.m_mountInfo);
	m_photoMgr.setUsePhotoID(m_roleInfo.m_userPhotoID);
	m_photoMgr.loadData(m_roleInfo.m_photoInfo);
	m_wflfareMgr.loadTimeData(m_roleInfo.m_levelWflfareTime);
	m_wflfareMgr.loadData(m_roleInfo.m_levelWflfare);
	if (m_roleInfo.m_mapID > 3) {
        m_roleInfo.m_mapID = 1;
    }
	if (cmdLen) {
		if (!uncompressSaveData(binData,cmdLen)){
			return false;	
		}
	}
	else {//第一登录
		firstLogin();
	}
	return true;
}

UINT32 GameUser::getRoleID()
{
	if (m_roleInfo.m_job == Role::eJOB_WARRIOR){//战士
		if (m_roleInfo.m_sex == Role::eSEX_MAN){ //男性
			return eCharID_Warrior_Man;	
		}
		else {
			return eCharID_Warrior_Female;	
		}
	}
	else if (m_roleInfo.m_job == Role::eJOB_MASTER){//法师
		if (m_roleInfo.m_sex == Role::eSEX_MAN){ //男性
			return eCharID_Master_Man;	
		}
		else {
			return eCharID_Master_Female;	
		}
	}
	else {//弓手
		if (m_roleInfo.m_sex == Role::eSEX_MAN){ //男性
			return eCharID_Bowman_Man;	
		}
		else {
			return eCharID_Bowman_Female;	
		}
	}
}

void GameUser::calcBattlePower()
{
	float battlePower = 0.0f;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_HP] * BP_HP_COEF;
	battlePower += std::max((INT32)(m_roleState.m_battleProps[eBATTLE_PROP_MP] - BP_MP_BASE),0) * BP_MP_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_DAMAGE] * BP_DAMAGE_COEF;
	battlePower += std::max((INT32)(m_roleState.m_battleProps[eBATTLE_PROP_HITRATE] - BP_HITRATE_BASE),0) * BP_HIT_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_DODGERATE] * BP_DODGE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_BANGRATE] * BP_BANG_COEF;
	battlePower += std::max((INT32)(m_roleState.m_battleProps[eBATTLE_PROP_BANGDAMAGE] - BP_BANGDAMAGE_BASE), 0) * BP_BANGDAMAGE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_ANTIBANGRATE] * BP_ANTI_BANG_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_ANTIBANGDAMAGE] * BP_ANTI_BANGDAMAGE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_FINALDAMAGE] * BP_FINAL_DAMAGE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_RECOVERHP] * BP_RECOVERHP_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_ABSORBHP] * BP_ABSORBHP_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_REFLECTDAMAGE] * BP_DAMAGE_REFLECT_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_FIREDAMAGE] * BP_FIRE_DAMAGE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_ICEDAMAGE] * BP_ICE_DAMAGE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_THUNDERDAMAGE] * BP_THUNDER_DAMAGE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_FIRERATE] * BP_FIRE_RATE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_ICERATE] * BP_ICE_RATE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_THUNDERRATE] * BP_THUNDER_RATE_COEF;
	battlePower += m_roleState.m_battleProps[eBATTLE_PROP_DAMAGEREDUCE] * BP_DAMAGEREDUCE_COEF;

	m_roleInfo.m_battlePower += (UINT32)battlePower;
}

void GameUser::setupRoleState(bool login)
{	
	//bzero(&m_roleState,sizeof(m_roleState));
	m_roleState.resetBattleProps();
	m_roleInfo.m_battlePower = 0;
	stPlayGrownBase * pGrownBase = g_playGrownBaseMgr.get(stPlayGrownBase::getFindID(getRoleID(),m_roleInfo.m_level));
	if (pGrownBase){
		m_roleState.m_nextLvExp = pGrownBase->m_needExp;
		m_roleState.m_battleProps[eBATTLE_PROP_HP] += pGrownBase->m_maxHP;
		m_roleState.m_battleProps[eBATTLE_PROP_DAMAGE] += pGrownBase->m_damage;
		m_roleState.m_battleProps[eBATTLE_PROP_HITRATE] += pGrownBase->m_hitRate;
		m_roleState.m_battleProps[eBATTLE_PROP_DODGERATE] += pGrownBase->m_dodgeRate;
		m_roleState.m_battleProps[eBATTLE_PROP_BANGRATE] += pGrownBase->m_bangRate;
		m_roleState.m_battleProps[eBATTLE_PROP_BANGDAMAGE] += pGrownBase->m_bangDamage;
		m_roleState.m_battleProps[eBATTLE_PROP_ANTIBANGRATE] += pGrownBase->m_antiBangRate;
		m_roleState.m_battleProps[eBATTLE_PROP_ANTIBANGDAMAGE] += pGrownBase->m_antiBangDamge;
		m_roleState.m_battleProps[eBATTLE_PROP_FINALDAMAGE] += pGrownBase->m_finalDamage;
		m_roleState.m_battleProps[eBATTLE_PROP_RECOVERHP] += pGrownBase->m_recoverHP;
		m_roleState.m_battleProps[eBATTLE_PROP_RECOVERMP] += pGrownBase->m_recoverMP;
		m_roleState.m_battleProps[eBATTLE_PROP_ABSORBHP] += pGrownBase->m_absorbHP;
		m_roleState.m_battleProps[eBATTLE_PROP_REFLECTDAMAGE] += pGrownBase->m_reflectDamage;
		m_roleState.m_battleProps[eBATTLE_PROP_FIREDAMAGE] += pGrownBase->m_fireDamage;
		m_roleState.m_battleProps[eBATTLE_PROP_ICEDAMAGE] += pGrownBase->m_iceDamage;
		m_roleState.m_battleProps[eBATTLE_PROP_THUNDERDAMAGE] += pGrownBase->m_thunderDamage;
		m_roleState.m_battleProps[eBATTLE_PROP_FIRERATE] += pGrownBase->m_fireRate;
		m_roleState.m_battleProps[eBATTLE_PROP_ICERATE] += pGrownBase->m_iceRate;
		m_roleState.m_battleProps[eBATTLE_PROP_THUNDERRATE] += pGrownBase->m_thunderRate;
		m_roleState.m_battleProps[eBATTLE_PROP_REF_FIRE_RATE] += pGrownBase->m_antiFireRate;
		m_roleState.m_battleProps[eBATTLE_PROP_REF_ICE_RATE] += pGrownBase->m_antiIceRate;
		m_roleState.m_battleProps[eBATTLE_PROP_REF_THUNDER_RATE] += pGrownBase->m_antiThunderRate;
		m_roleState.m_battleProps[eBATTLE_PROP_DAMAGEREDUCE] += pGrownBase->m_damageReduce;
		m_roleState.m_battleProps[eBATTLE_PROP_MP] += pGrownBase->m_maxMP;
		m_roleState.m_battleProps[eBATTLE_PROP_BATI] += pGrownBase->m_baTi;
		
		m_bagMgr.m_equipBag.calcEquipData();
		const stEquipData & equipData = m_bagMgr.m_equipBag.getEquipData();
		for (UINT16 i = 0;i < eMAX_BATTLE_PROP;++i){
			m_roleState.m_battleProps[i] += equipData.m_battleProps[i];	
		}
		//强化技能属性
		m_raiseSkillProp.clear();
		for (UINT16 i = 0; i < equipData.m_raiseSkillBPVec.size(); ++i)
		{
			const stBattlePropValue & prop = equipData.m_raiseSkillBPVec[i];
			std::map<UINT32, std::map<UINT32, UINT32> >::iterator it = m_raiseSkillProp.find(prop.m_val1);
			if (it == m_raiseSkillProp.end())
			{
				std::map<UINT32, UINT32>  propMap;
				propMap[prop.m_val2] = prop.m_val3;
				m_raiseSkillProp[prop.m_val1] = propMap;
			}
			else
			{
				if (it->second.find(prop.m_val2) == it->second.end())
				{
					it->second.insert(make_pair<UINT32,UINT32>(prop.m_val2,prop.m_val3));
				}
				else
				{
					it->second[prop.m_val2] += prop.m_val3;
				}
			}
		}
#ifdef _HDZ_DEBUG
		std::map<UINT32, std::map<UINT32, UINT32> >::iterator it = m_raiseSkillProp.begin();
		for (; it != m_raiseSkillProp.end(); ++it)
		{
			std::map<UINT32, UINT32>::iterator it1 = it->second.begin();
			for (; it1 != it->second.end(); ++it1)
			{
				Athena::logger->trace("技能强化属性 skillID[%d],强化类型[%d],强化数值[%d]", it->first,it1->first,it1->second);
			}
		}
#endif
        for (std::map<UINT64, std::vector<EnchantItem> >::iterator it =
                m_enchantMap.begin(); it != m_enchantMap.end(); it++) {
            if (!m_bagMgr.m_equipBag.getItemByInstaceID(it->first)) {
                continue;
            }
            std::vector<EnchantItem> & itemVec = it->second;
            for (size_t i = 0; i < itemVec.size(); i++) {
                if (itemVec[i].propType < eMAX_BATTLE_PROP) {
                    m_roleState.m_battleProps[itemVec[i].propType] += itemVec[i].propVal;
                }
            }
        }
		
		//加称号加成
		m_titleMgr.addProp();
		//打星加成
		m_spriteSysMgr.addProp();
		
		//坐骑加成
		m_mountMgr.addProp();
		
		//宠物加成
		m_heroMgr.addProp2Owner();

		calcBattlePower();	
		
		m_skillMgr.calcBP();
		m_roleInfo.m_battlePower += m_skillMgr.getAddBP();
#ifdef _HDZ_DEBUG
		Athena::logger->trace("计算出来任务自身的战力:%d,武将提供的战力:%d,装备提供的战力:%d,技能提供的战力:%d",m_roleInfo.m_battlePower,m_heroMgr.getBattlePower(),equipData.m_battlePower,m_skillMgr.getAddBP());
#endif
		// m_roleInfo.m_battlePower += m_heroMgr.getBattlePower();
		//m_roleInfo.m_hp = m_roleState.m_battleProps[eBATTLE_PROP_HP];
		//m_roleInfo.m_mp = m_roleState.m_battleProps[eBATTLE_PROP_DAMAGE];	
		
		m_maxHP = m_roleState.m_battleProps[eBATTLE_PROP_HP];
		m_maxMP = m_roleState.m_battleProps[eBATTLE_PROP_MP];
		m_maxBati = m_roleState.m_battleProps[eBATTLE_PROP_BATI];

		if (!login) {
			synMyLevel();
			synMyBattlePower();
			synMyCopymapStarNum();
		}

		m_roleInfo.m_equipAveRaiseLv = equipData.m_equipAveRaiseLv;
		m_roleInfo.m_equipAveRefineLv = equipData.m_equipAveRefineLv;
		m_roleInfo.m_wingRefineLv = equipData.m_wingAveRefineLv;
		m_roleInfo.m_fuwenAveRaiseLv = equipData.m_equipAveRaiseLv;
		m_roleInfo.m_fuwenAveQuality = equipData.m_fuwenAveQuality;
		m_equipAveBreakLv = equipData.m_equipAveBreakLv;
		m_wingBreakLv = equipData.m_wingAveBreakLv;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("计算出来的战力是:%d",m_roleInfo.m_battlePower);
#endif
	}
	else {
		m_roleState.m_nextLvExp = (UINT32)-1;
	}

	// calcVipLv();
}

UINT32 GameUser::addExp(UINT32 exp)
{
	if (m_roleInfo.m_level >= CMiscCfgMgr::getSingleton().getMaxRoleLvl()){//超过了最大等级
		return 0;
	}	

	m_roleInfo.m_exp += exp;
	
	Role::stNotifyRoleAddExp addCmd;
	addCmd.set_addexp(exp);
	this->sendProto2Me(addCmd);

	UINT16 loopNum = 0;

	while (m_roleInfo.m_exp >= m_roleState.m_nextLvExp){
		m_roleInfo.m_exp -= m_roleState.m_nextLvExp;
		levelup();
		loopNum += 1;
		if (loopNum >= CMiscCfgMgr::getSingleton().getMaxRoleLvl()){
			break;
		}

		if (m_roleInfo.m_level >= CMiscCfgMgr::getSingleton().getMaxRoleLvl()){
			break;
		}
	}
	
#ifdef _HDZ_DEBUG
	Athena::logger->trace("玩家account=%s,name=%s获得了%d经验",getAccount(),getName(),exp);
#endif
	return exp;
}

void GameUser::levelup()
{
	if (m_roleInfo.m_level >= CMiscCfgMgr::getSingleton().getMaxRoleLvl()){
		return ;
	}

	m_roleInfo.m_level += 1;
	if(m_roleInfo.m_level == CMiscCfgMgr::getSingleton().getLevelWflfare()){
		 m_wflfareMgr.addOpenTime();
		 m_wflfareMgr.sendHasGotReward();
	}
	this->setupRoleState();
	this->calcVipLv();
	Athena::logger->trace("account=%s,name=%s,charID=%d 升级到了 %d",this->getAccount(),this->getName(),this->getID(),m_roleInfo.m_level);
	Role::stSynRoleLevel  synCmd;
	synCmd.set_curexp(m_roleInfo.m_exp);
	synCmd.set_curlevel(m_roleInfo.m_level);
	this->sendProto2Me(synCmd);
	//this->sendMainData2Me();
	this->sendBattleProp();
	m_skillMgr.checkSkillUnlock();
	synMyLevel(true);
	
	if (CLimitFashionCfg::getSingleton().isLvExist(getLevel())){
		m_limitFashionDrawMgr.addDrawNum(1);	
	}

	Game::stUserLevelupLog webCmd;
	webCmd.set_account(m_roleInfo.m_account);
	webCmd.set_charid(getID());
	webCmd.set_name(getName());
	webCmd.set_newexp(m_roleInfo.m_exp);
	webCmd.set_oldlv(getLevel() - 1);
	webCmd.set_newlv(getLevel());
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

	m_taskMgr.checkDailyTask();

	/*----------韩国项目日志--------------*/
	CKoreaLogMgr::getSingleton().userLevelup(g_gameServer->getSec(),m_roleInfo.m_account,getID(),getLevel());
	/*----------韩国项目日志--------------*/

	Game::stNotifyLevelUp  gateCmd;
	gateCmd.set_charid(getID());
	gateCmd.set_level(getLevel());
	this->sendProto2Gate(gateCmd);
}

bool  GameUser::checkGoldCoin(UINT32 num)
{
	if (m_roleInfo.m_goldCoin < num){
		return false;
	}	
	return true;
}

bool  GameUser::addGoldCoin(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_goldCoin += num;
	/*Role::stSynGoldCoin  notifyCmd;
	notifyCmd.set_num(m_roleInfo.m_goldCoin);
	sendProto2Me(notifyCmd);*/
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_GoldCoin);
	retCmd.set_num(m_roleInfo.m_goldCoin);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account=%s,name=%s,id=%d 通过%s获得了金币%d",getAccount(),getName(),getID(),desc,num);
	return true;
}
	
bool  GameUser::removeGoldCoin(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_roleInfo.m_goldCoin < num){
		return false;
	}

	m_roleInfo.m_goldCoin -= num;
	/*Role::stSynGoldCoin  notifyCmd;
	notifyCmd.set_num(m_roleInfo.m_goldCoin);
	sendProto2Me(notifyCmd);*/
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_GoldCoin);
	retCmd.set_num(m_roleInfo.m_goldCoin);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s扣除了金币%d",getAccount(),getName(),getID(),desc,num);
	return true;
}

bool GameUser::checkDiamond(UINT32 num)
{
	if (m_roleInfo.m_diamond + m_roleInfo.m_giveDiamond< num){
		return false;	
	}
	return true;
}

void GameUser::recharge(UINT32 money,UINT32 addDiamondNum,UINT32 itemID,const std::string & tid,UINT32 giveDiamond)
{
	m_roleInfo.m_diamond += addDiamondNum;
	m_roleInfo.m_giveDiamond += giveDiamond;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Diamond);
	retCmd.set_num(m_roleInfo.m_diamond + m_roleInfo.m_giveDiamond);
	this->sendProto2Me(retCmd);
	
	bool bFirstRecharge = m_roleInfo.m_rechargeNum  == 0 ? true : false;
	m_roleInfo.m_rechargeNum +=  addDiamondNum;
	m_countMgr.addDayRefreshValue(ECOUT_TODAY_RECHARGE,addDiamondNum);
	this->m_rechargeActivityMgr.onRecharge(addDiamondNum);
	this->m_luckShopMgr.onRecharge(addDiamondNum);
	this->m_activityDataMgr.onRecharge(addDiamondNum);

	calcVipLv();
	synTotalRecharge();
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了钻石%d,tid:%s",getAccount(),getName(),getID(),"充值",addDiamondNum + giveDiamond,tid.c_str());
	/*--- 打印生态日志 ---*/

	uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
	uuzu::logsdk::UserInfo userInfo;
	userInfo.level = m_roleInfo.m_level;
	userInfo.passport = m_roleInfo.m_account; //必须填写
	char szCharID[15] = { 0 };
	snprintf(szCharID,sizeof(szCharID),"%d",getID());
	userInfo.uid = szCharID;
	userInfo.clientip = m_ip;
	char szNum[15] = { 0 };
    UINT32 realMoney = CRMBShop::getMoney(itemID);
    if (realMoney) {
        userInfo.moneycoin = realMoney;
        snprintf(szNum,sizeof(szNum),"%d", realMoney);
    } else {
        userInfo.moneycoin = money;
        snprintf(szNum,sizeof(szNum),"%d", money);
    }
#ifdef  _HDZ_DEBUG
	Athena::logger->trace("[CountryVersion] %s, szNum is %s, addDiamondNum is %d, realMoney is %d",
            Athena::global["CountryVersion"].c_str(), szNum, addDiamondNum, realMoney);
#endif
	userInfo.clientip = getIP();
	userInfo.exp = m_roleInfo.m_exp;
	userInfo.blackcoin = m_roleInfo.m_giveDiamond;
	//userInfo.moneycoin =  m_roleInfo.m_diamond;
	userInfo.gamecoin = m_roleInfo.m_goldCoin;
	userInfo.device = m_deviceID;
	userInfo.viplevel =getVipLv();  
	userInfo.firstintime = m_firstLogin;
	userInfo.version = Athena::global["gameVersion"];
	std::string opID = g_int2string(m_roleInfo.m_opID);
	logger.setPID(opID.c_str()); 
	logger.sendDataLog(userInfo, "2","",szNum,tid.c_str(),this->m_channelID.c_str(),"","","","pay");
	/*----- 打印生态日志 -----*/
	
	Game::stRechargeLog webCmd;
	webCmd.set_account(m_account);
	webCmd.set_charid(getID());
	webCmd.set_name(getName());
	webCmd.set_diamondnum(m_roleInfo.m_diamond + m_roleInfo.m_giveDiamond);
	webCmd.set_rechargenum(money);
	webCmd.set_tid(tid);
	webCmd.set_opid(m_roleInfo.m_opID);
	webCmd.set_newrecharge(bFirstRecharge);
	webCmd.set_adddiamond(addDiamondNum + giveDiamond);
	webCmd.set_viplv(this->getVipLv());
	webCmd.set_itemid(itemID);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

bool GameUser::addDiamond(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}
	m_roleInfo.m_giveDiamond += num;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Diamond);
	retCmd.set_num(m_roleInfo.m_diamond + m_roleInfo.m_giveDiamond);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了钻石%d",getAccount(),getName(),getID(),desc,num);

	/*--- 打印生态日志 ---*/

	uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
	uuzu::logsdk::UserInfo userInfo;
	userInfo.level = m_roleInfo.m_level;
	userInfo.passport = m_roleInfo.m_account; //必须填写
	char szCharID[15] = { 0 };
	snprintf(szCharID,sizeof(szCharID),"%d",getID());
	userInfo.uid = szCharID;
	userInfo.clientip = m_ip;
	char szNum[15] = { 0 };
	snprintf(szNum,sizeof(szNum),"%d",num);
	userInfo.clientip = getIP();
	userInfo.exp = m_roleInfo.m_exp;
	userInfo.blackcoin = m_roleInfo.m_giveDiamond;
	userInfo.moneycoin =  m_roleInfo.m_diamond;
	userInfo.gamecoin = m_roleInfo.m_goldCoin;
	userInfo.device = m_deviceID;
	userInfo.viplevel =getVipLv();  
	userInfo.firstintime = m_firstLogin;
	userInfo.version = Athena::global["gameVersion"];
	std::string opID = g_int2string(m_roleInfo.m_opID);
	logger.setPID(opID.c_str()); 
	logger.sendDataLog(userInfo,desc, "",szNum, "", "",szNum,"","","curr");
	/*----- 打印生态日志 -----*/
	return true;
}

bool GameUser::removeDiamond(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_roleInfo.m_diamond  + m_roleInfo.m_giveDiamond < num){
		return false;
	}
	
	UINT32 toDec = num;
	UINT32 decGiveDiamond = 0;
	UINT32 decDiamond = 0;

	if (m_roleInfo.m_giveDiamond >= num){
		m_roleInfo.m_giveDiamond -= num;
		decGiveDiamond = num;
	}
	else {
		toDec = toDec - m_roleInfo.m_giveDiamond;
		m_roleInfo.m_giveDiamond = 0;
		m_roleInfo.m_diamond -= toDec;
		decDiamond = toDec;
		decGiveDiamond = num - toDec;
	}

	m_roleInfo.m_totlaConsume += num;
	m_countMgr.addDayRefreshValue(ECOUNT_DAY_CONSUME,num);
    struct tm _tm;
    UINT32 createTime = 0;
    if (strptime(m_roleInfo.m_createTime, "%Y-%m-%d %H:%M:%S", &_tm)){
        createTime = mktime(&_tm);
        createTime += 864000;//10 days expire
    }
    if (createTime >= g_gameServer->getSec()) {
        m_roleInfo.m_sevenDayConsumeDiamond += num;
    }
	this->m_activityDataMgr.onUseDiamond(num);

	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Diamond);
	retCmd.set_num(m_roleInfo.m_diamond + m_roleInfo.m_giveDiamond);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s减少了钻石%d",getAccount(),getName(),getID(),desc,num);
    sendDiamondDataLog(num, decGiveDiamond, decDiamond, desc);
	return true;
}
	
void GameUser::sendDiamondDataLog(UINT32 num, UINT32 decGiveDiamond, UINT32 decDiamond, const char * desc) {
	/*--- 打印生态日志 ---*/

	uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
	uuzu::logsdk::UserInfo userInfo;
	userInfo.level = m_roleInfo.m_level;
	userInfo.passport = m_roleInfo.m_account; //必须填写
	char szCharID[15] = { 0 };
	snprintf(szCharID,sizeof(szCharID),"%d",getID());
	userInfo.uid = szCharID;
	userInfo.clientip = m_ip;
	char szNum[15] = { 0 };
	snprintf(szNum,sizeof(szNum),"%d",num);
	char szGiveDiamondNum[15] = { 0 };
	char szDiamondNum[15] = { 0 };
	snprintf(szGiveDiamondNum,sizeof(szGiveDiamondNum),"%d",decGiveDiamond);
	snprintf(szDiamondNum,sizeof(szDiamondNum),"%d",decDiamond);
	userInfo.clientip = getIP();
	userInfo.exp = m_roleInfo.m_exp;
	userInfo.blackcoin = m_roleInfo.m_giveDiamond;
	userInfo.moneycoin =  m_roleInfo.m_diamond;
	userInfo.gamecoin = m_roleInfo.m_goldCoin;
	userInfo.device = m_deviceID;
	userInfo.viplevel =getVipLv();  
	userInfo.firstintime = m_firstLogin;
	userInfo.version = Athena::global["gameVersion"];
	std::string opID = g_int2string(m_roleInfo.m_opID);
	logger.setPID(opID.c_str()); 
	logger.sendDataLog(userInfo, desc, "", szNum,szGiveDiamondNum, szDiamondNum , "","","","prop",1);
	/*----- 打印生态日志 -----*/

}

bool GameUser::addHeishijiejing(UINT32 num, const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_heishijiejing += num;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Heishijiejing);
	retCmd.set_num(m_roleInfo.m_heishijiejing);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了黑市结晶%d",getAccount(),getName(),getID(),desc,num);
	return true;
}

bool GameUser::removeHeishijiejing(UINT32 num,const char * desc)
{
	if (m_roleInfo.m_heishijiejing < num){
		return false;
	}

	m_roleInfo.m_heishijiejing -= num;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Heishijiejing);
	retCmd.set_num(m_roleInfo.m_heishijiejing);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s减少了黑市结晶%d",getAccount(),getName(),getID(),desc,num);
	return true;
}

bool GameUser::checkHeishijiejing(UINT32 num)
{
	if (m_roleInfo.m_heishijiejing < num){
		return false;
	}	
	return true;
}

bool GameUser::addReputation(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_reputaion += num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了声望%d",getAccount(),getName(),getID(),desc,num);

	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Reputation);
	retCmd.set_num(m_roleInfo.m_reputaion);
	this->sendProto2Me(retCmd);
	return true;

}

bool GameUser::removeReputaion(UINT32 num,const char * desc)
{
	if (m_roleInfo.m_reputaion < num){
		return false;
	}

	m_roleInfo.m_reputaion -= num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s减少了声望%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Reputation);
	retCmd.set_num(m_roleInfo.m_reputaion);
	this->sendProto2Me(retCmd);
	return true;

}

bool GameUser::checkReputation(UINT32 num)
{
	if (m_roleInfo.m_reputaion < num){
		return false;
	}	
	else {
		return true;
	}
}
	
bool GameUser::addHonour(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}
	UINT32 oldHonour = m_roleInfo.m_honour;
	m_roleInfo.m_honour += num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了荣誉%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Honour);
	retCmd.set_num(m_roleInfo.m_honour);
	this->sendProto2Me(retCmd);
	
	Game::stHonourChgLog webCmd;
	webCmd.set_account(m_roleInfo.m_account);
	webCmd.set_charid(getID());
	webCmd.set_name(getName());
	webCmd.set_oldhonour(oldHonour);
	webCmd.set_newhonour(m_roleInfo.m_honour);
	webCmd.set_reason(desc);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	return true;
}

bool GameUser::removeHonour(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_roleInfo.m_honour < num){
		return false;
	}
	
	UINT32 oldHonour = m_roleInfo.m_honour;

	m_roleInfo.m_honour -= num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s减少了荣誉%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Honour);
	retCmd.set_num(m_roleInfo.m_honour);
	this->sendProto2Me(retCmd);
	
	Game::stHonourChgLog webCmd;
	webCmd.set_account(m_roleInfo.m_account);
	webCmd.set_charid(getID());
	webCmd.set_name(getName());
	webCmd.set_oldhonour(oldHonour);
	webCmd.set_newhonour(m_roleInfo.m_honour);
	webCmd.set_reason(desc);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	return true;
}

bool GameUser::checkHonour(UINT32 num)
{
	if (m_roleInfo.m_honour < num){
		return false;
	}	
	return true;
}

bool GameUser::addBraveVal(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_braveVal += num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了勇敢值%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Brave);
	retCmd.set_num(m_roleInfo.m_braveVal);
	this->sendProto2Me(retCmd);
	return true;

}

bool GameUser::removeBraveVal(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_roleInfo.m_braveVal < num){
		return false;
	}

	m_roleInfo.m_braveVal -= num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s减少了勇敢值%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Brave);
	retCmd.set_num(m_roleInfo.m_braveVal);
	this->sendProto2Me(retCmd);
	return true;

}

bool GameUser::checkBraveVal(UINT32 num)
{
	if (m_roleInfo.m_braveVal < num){
		return false;
	}	
	return true;
}
	
bool GameUser::addPeakCertificate(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_peakCertificate += num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了巅峰证%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_PeakCertificate);
	retCmd.set_num(m_roleInfo.m_peakCertificate);
	this->sendProto2Me(retCmd);
	return true;

}

bool GameUser::removePeakCertificate(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_roleInfo.m_peakCertificate < num){
		return false;
	}

	m_roleInfo.m_peakCertificate -= num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s减少了巅峰证%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_PeakCertificate);
	retCmd.set_num(m_roleInfo.m_peakCertificate);
	this->sendProto2Me(retCmd);
	return true;
}

bool GameUser::checkPeakCertificate(UINT32 num)
{
	if (m_roleInfo.m_peakCertificate < num){
		return false;
	}	
	else {
		return true;
	}
}

bool GameUser::addHufu(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_hufu += num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了虎符%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Hufu);
	retCmd.set_num(m_roleInfo.m_hufu);
	this->sendProto2Me(retCmd);
	return true;

}

bool GameUser::removeHufu(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_roleInfo.m_hufu < num){
		return false;
	}

	m_roleInfo.m_hufu -= num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s减少了虎符%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Hufu);
	retCmd.set_num(m_roleInfo.m_hufu);
	this->sendProto2Me(retCmd);
	return true;
}
	
bool GameUser::checkHufu(UINT32 num)
{
	if (m_roleInfo.m_hufu < num){
		return false;
	}
	else {
		return true;
	}
}

bool GameUser::addYuntie(UINT32 num ,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_yuntie += num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了陨铁%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Yuntie);
	retCmd.set_num(m_roleInfo.m_yuntie);
	this->sendProto2Me(retCmd);
	return true;
}

bool GameUser::removeYuntie(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_roleInfo.m_yuntie < num){
		return false;
	}

	m_roleInfo.m_yuntie -= num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s减少了陨铁%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Yuntie);
	retCmd.set_num(m_roleInfo.m_yuntie);
	this->sendProto2Me(retCmd);
	return true;

}

bool GameUser::checkYuntie(UINT32 num)
{
	if (m_roleInfo.m_yuntie  < num){
		return false;
	}
	else {
		return true;
	}
}

bool GameUser::checkJuexingExp(UINT32 num)
{
	if (m_roleInfo.m_juexingExp < num){
		return false;
	}
	return true;
}

bool GameUser::addJuxingExp(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_juexingExp += num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加了觉醒经验%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Juexing_Exp);
	retCmd.set_num(m_roleInfo.m_juexingExp);
	this->sendProto2Me(retCmd);
	return true;
}

bool GameUser::removeJuxingExp(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	if (m_roleInfo.m_juexingExp < num){
		return false;
	}

	m_roleInfo.m_juexingExp -= num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s扣除了觉醒经验%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Juexing_Exp);
	retCmd.set_num(m_roleInfo.m_juexingExp);
	this->sendProto2Me(retCmd);
	return true;
}

//检查积分
bool GameUser::checkIntegral(UINT32 num)
{
	if(m_roleInfo.m_integral < num){
		return false;
	}
	return true;
}
//增加积分
bool GameUser::addIntegral(UINT32 num,const char * desc)
{
	if(!desc){
		return false;
	}

	m_roleInfo.m_integral += num;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Integral);
	retCmd.set_num(m_roleInfo.m_integral); 
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account=%s,name=%s,id=%d 通过%s获得了积分%d",getAccount(),getName(),getID(),desc,num);
	return true;
}
//减少积分
bool GameUser::removeIntegral(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}
	
	if (m_roleInfo.m_integral < num){
		return false;
	}
	
	m_roleInfo.m_integral -= num;
	
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Integral);
	retCmd.set_num(m_roleInfo.m_integral);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s扣除了积分%d",getAccount(),getName(),getID(),desc,	num);
	return true;
}

bool GameUser::addContribute(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_contribute += num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s增加公会贡献度%d",getAccount(),getName(),getID(),desc,num);
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Guild_Contri);
	retCmd.set_num(m_roleInfo.m_contribute);
	this->sendProto2Me(retCmd);
	synContribyte2Global();
	return true;
}

bool GameUser::removeContribute(UINT32 num,const char * desc)
{
	if (!desc){
		return false;
	}	

	if (m_roleInfo.m_contribute < num){
		return false;
	}

	m_roleInfo.m_contribute -= num;
	Athena::logger->trace("account=%s,name=%s,id=%d 因为%s扣除公会贡献度%d",getAccount(),getName(),getID(),desc,num);

	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Guild_Contri);
	retCmd.set_num(m_roleInfo.m_contribute);
	this->sendProto2Me(retCmd);
	synContribyte2Global();
	return true;
}

bool GameUser::checkContribute(UINT32 num)
{
	if (m_roleInfo.m_contribute >= num){
		return true;
	}	
	else {
		return false;
	}
}

bool GameUser::addResource(eResource eRes,UINT32 num,const char * desc)
{
	if (num <= 0){
		return true;
	}

	UINT32 oldVal = 0;
	UINT32 newVal = 0;
	bool ret = false;

	if (g_gameServer->isAppVersion()){
		
	}

	switch (eRes){
		case eResource_GoldCoin :
			{
				oldVal = this->getGoldCoin(); 
				ret = this->addGoldCoin(num,desc);
				newVal = this->getGoldCoin();
			}
			break;
		case eResource_Diamond :
			{
				oldVal = this->getDiamond();
				ret =  this->addDiamond(num,desc);
				newVal = this->getDiamond();
			}
			break;
		case eResource_Heishijiejing :
			{
				oldVal = this->getHeishijiejing();
				ret = this->addHeishijiejing(num,desc);	
				newVal = this->getHeishijiejing();
			}
			break;
		case eResource_Reputation :
			{
				oldVal = this->getReputation();
				ret = this->addReputation(num,desc);
				newVal = this->getReputation();
			}
			break;
		case eResource_Honour :
			{
				oldVal = this->getHonour();
				ret = this->addHonour(num,desc);
				newVal = this->getHonour();
			}
			break;
		case eResource_Brave :
			{
				oldVal = this->getBraveVal();
				ret = this->addBraveVal(num,desc);
				newVal = this->getBraveVal();
			}
			break;
		case eResource_PeakCertificate :
			{
				oldVal = this->getPeakCertificate();
				ret = this->addPeakCertificate(num,desc);
				newVal = this->getPeakCertificate();
			}
			break;
		case eResource_Hufu :
			{
				oldVal = this->getHufu();
				ret = this->addHufu(num,desc);
				newVal = this->getHufu();
			}
			break;
		case eResource_Yuntie :
			{
				oldVal = this->getYunTie();
				ret = this->addYuntie(num,desc);
				newVal = this->getYunTie();
			}
			break;
		case eResource_Sp :
			{
				oldVal = this->getSP(); 
				ret = this->addSP(num,desc);
				newVal = this->getSP();
			}
			break;
		case eResource_Lilian :
			{
				oldVal = this->getLilian();
				ret = this->addLilian(num,desc);
				newVal = this->getLilian(); 
			}
			break;
		case eResource_Guild_Contri :
			{
				oldVal = this->getContribute();
				ret = this->addContribute(num,desc);
				newVal = this->getContribute();
			}
			break;
		case eResource_Juexing_Exp :
			{
				oldVal = this->getJuxingExp();
				ret =  this->addJuxingExp(num,desc);
				newVal = this->getJuxingExp();
			}
			break;
		case eResource_Lingwu_Exp :
			{
				oldVal = this->m_heroMgr.getSkillExp();
				ret = this->m_heroMgr.addSkillExp(num,desc);
				newVal = this->m_heroMgr.getSkillExp();
			}
			break;
		case eResource_Integral :
			{
				oldVal = this->getIntegral();
				ret = this->addIntegral(num,desc);
				newVal = this->getIntegral();
			}
			break;
		case eResource_Soul_Fire :
			{
				oldVal = this->getSoulFire();
				ret = this->addSoulFire(num,desc);
				newVal = this->getSoulFire();
			}
			break;
		case eResource_Life_Essence :
			{
				oldVal = this->getLifeEssence();
				ret = this->addLifeEssence(num,desc);
				newVal = this->getLifeEssence();
			}
			break;
			
		case eResource_Guild_Crystal :
			{
				
				oldVal = this->getGuildCrystal();
				ret = this->addGuildCrystal(num,desc);
				newVal = this->getGuildCrystal();
			}                                   
			break;  
		case eResource_TrainSp:
			{
				oldVal = this->getTrainSP(); 
				ret = this->addTrainSP(num,desc);
				newVal = this->getTrainSP();
			}                                  
			break;  
	}

	if (oldVal != newVal) {
		Game::stResourceChgLog webCmd;
		webCmd.set_account(m_roleInfo.m_account);
		webCmd.set_charid(getID());
		webCmd.set_name(getName());
		webCmd.set_oldvalue(oldVal);
		webCmd.set_newvalue(newVal);
		if (newVal > oldVal){
			webCmd.set_addnum(newVal - oldVal);
		}
		webCmd.set_resid(eRes);
		webCmd.set_reason(desc);
		g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	}
	return ret;
}

bool GameUser::removeResource(eResource eRes,UINT32 num,const char * desc)
{
	if (num <= 0){
		return true;
	}

	UINT32 oldVal = 0;
	UINT32 newVal = 0;
	bool ret = false;

	switch (eRes){
		case eResource_GoldCoin :
			{
				oldVal = this->getGoldCoin();
				ret = this->removeGoldCoin(num,desc);
				newVal = this->getGoldCoin();
			}
			break;
		case eResource_Diamond :
			{
				oldVal = this->getDiamond();
				ret =  this->removeDiamond(num,desc);
				newVal = this->getDiamond();
			}
			break;
		case eResource_Heishijiejing :
			{
				oldVal = this->getHeishijiejing();
				ret =  this->removeHeishijiejing(num,desc);
				newVal = this->getHeishijiejing();
			}
			break;
		case eResource_Reputation :
			{
				oldVal = this->getReputation();
				ret = this->removeReputaion(num,desc);
				newVal = this->getReputation();
			}
			break;
		case eResource_Honour :
			{
				oldVal = this->getHonour();
				ret =  this->removeHonour(num,desc);
				newVal = this->getHonour();
			}
			break;
		case eResource_Brave :
			{
				oldVal = this->getBraveVal();
				ret = this->removeBraveVal(num,desc);
				newVal = this->getBraveVal();
			}
			break;
		case eResource_PeakCertificate :
			{
				oldVal = this->getPeakCertificate();
				ret =  this->removePeakCertificate(num,desc);
				newVal = this->getPeakCertificate();
			}
			break;
		case eResource_Hufu :
			{
				oldVal = this->getHufu();
				ret = this->removeHufu(num,desc);
				newVal = this->getHufu();
			}
			break;
		case eResource_Yuntie :
			{
				oldVal = this->getYunTie();
				ret =  this->removeYuntie(num,desc);
				newVal = this->getYunTie();
			}
			break;
		case eResource_Sp :
			{
				oldVal = this->getSP();
				ret = this->removeSP(num,desc);
				newVal = this->getSP();
			}
			break;
		case eResource_Lilian :
			{
				oldVal = this->getLilian();
				ret = this->removeLilian(num,desc);
				newVal = this->getLilian();
			}
			break;
		case eResource_Guild_Contri :
			{
				oldVal = this->getContribute();
				ret =  this->removeContribute(num,desc);	
				newVal = this->getContribute();
			}
			break;
		case eResource_Juexing_Exp :
			{
				oldVal = this->getJuxingExp();
				ret = this->removeJuxingExp(num,desc);
				newVal = this->getJuxingExp();
			}
			break;
		case eResource_Lingwu_Exp :
			{
				oldVal = this->m_heroMgr.getSkillExp();
				ret = this->m_heroMgr.removeSkillExp(num,desc);
				newVal = this->m_heroMgr.getSkillExp();
			}
			break;
		case eResource_Integral :
			{
				oldVal = this->getIntegral();
				ret = this->removeIntegral(num,desc);
				newVal = this->getIntegral();
			}
			break;
		case eResource_Soul_Fire :
			{
				oldVal = this->getSoulFire();
				ret = this->removeSoulFire(num,desc);
				newVal = this->getSoulFire();
			}
			break;
		case eResource_Life_Essence :                 
			{                                         
				oldVal = this->getLifeEssence();      
				ret = this->removeLifeEssence(num,desc); 
				newVal = this->getLifeEssence();      
			}                                         
			break;                                    
		case eResource_Guild_Crystal :                 
			{  
				
				oldVal = this->getGuildCrystal();     
				ret = this->removeGuildCrystal(num,desc);
				newVal = this->getGuildCrystal();     
				
			}                                         
			break;      
		case eResource_TrainSp :
			{
				oldVal = this->getTrainSP();
				ret = this->removeTrainSP(num,desc);
				newVal = this->getTrainSP();
			}
			break;                              
	}

	if (oldVal != newVal) {
		Game::stResourceChgLog webCmd;
		webCmd.set_account(m_roleInfo.m_account);
		webCmd.set_charid(getID());
		webCmd.set_name(getName());
		webCmd.set_oldvalue(oldVal);
		webCmd.set_newvalue(newVal);
		webCmd.set_resid(eRes);
		webCmd.set_reason(desc);
		g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	}
	return ret;
}

bool GameUser::checkResource(eResource eRes,UINT32 num)
{
	switch (eRes){
		case eResource_GoldCoin :
			{
				return this->checkGoldCoin(num);
			}
			break;
		case eResource_Diamond :
			{
				return this->checkDiamond(num);
			}
			break;
		case eResource_Heishijiejing :
			{
				return this->checkHeishijiejing(num);
			}
			break;
		case eResource_Reputation :
			{
				return this->checkReputation(num);
			}
			break;
		case eResource_Honour :
			{
				return this->checkHonour(num);
			}
			break;
		case eResource_Brave :
			{
				return this->checkBraveVal(num);
			}
			break;
		case eResource_PeakCertificate :
			{
				return this->checkPeakCertificate(num);
			}
			break;
		case eResource_Hufu :
			{
				return this->checkHufu(num);
			}
			break;
		case eResource_Yuntie :
			{
				return this->checkYuntie(num);
			}
			break;
		case eResource_Sp :
			{
				return this->checkSP(num);
			}
			break;
		case eResource_Lilian :
			{
				return this->checkLilian(num);
			}
			break;
		case eResource_Guild_Contri :
			{
				return this->checkContribute(num);
			}
			break;
		case eResource_Juexing_Exp :
			{}
			break;
		case eResource_Lingwu_Exp :
			{}
			break;
		case eResource_Integral :
			{
				return this->checkIntegral(num);
			}
			break;
		case eResource_Soul_Fire :
			{
				return this->checkSoulFire(num);
			}
			break;
		case eResource_Life_Essence :
			{
				return this->checkLifeEssence(num);
			}
			break;
		case eResource_Guild_Crystal :
			{
				return this->checkGuildCrystal(num);
			}
			break;
		case eResource_TrainSp :
			{
				return this->checkTrainSP(num);
			}
			break;
	}
	return false;
}


UINT32 GameUser::getPropVal(UINT16 propID,UINT32 propParam)
{
	switch (propID) {
		case TaskProto::ET_RAISE_EQUIP_TO_LV :     //强化装备到某个级别
			{
				stRaiseEquipNumCB cb1(propParam);
				stRaiseEquipNumCB cb2(propParam);
				
				m_bagMgr.m_mainBag.exeEveryOne(cb1);
				m_bagMgr.m_equipBag.exeEveryOne(cb2);

				return  cb1.m_num + cb2.m_num;
			}
			break;
		case TaskProto::ET_REFINE_EQUIP_TO_LV :    //精炼装备到某个级别
			{
				stRefineEquipNumCB cb1(propParam);
				stRefineEquipNumCB cb2(propParam);
				
				m_bagMgr.m_mainBag.exeEveryOne(cb1);
				m_bagMgr.m_equipBag.exeEveryOne(cb2);

				return cb1.m_num + cb2.m_num;
			}
			break;
		case TaskProto::ET_HERO_BREAK_INTO_LV :     //武将突破到某个级别
			{
				stHeroInfoCBGetLv cb(propParam);
				this->m_heroMgr.exeEveryOne(cb);
				return cb.m_num;
			}
			break;
		case TaskProto::ET_HERO_LEVELUP_TO_LV :      //武将进化到某个级别
			{
				stHeroInfoCBEvolution cb(propParam);
				m_heroMgr.exeEveryOne(cb);
				return cb.m_num;
			}
			break;
		case TaskProto::ET_RAISE_SKILL_TO_LV :       //技能突破到某个级别
			{
				stGetSkillRaiseLvCB cb(propParam);
				m_skillMgr.exeEveryOne(cb);
				return cb.m_num;
			}
			break;
		case TaskProto::ET_EQUIP_FUWEN_NUM :         //装备5个符文的次数
			{
				return m_bagMgr.m_equipBag.getWearFullFuwenEquip();	
			}
			break;
		case TaskProto::ET_RAISE_FUWEN_TO_LV :       //符文强化到某个级别
			{
				stRaiseFuwenNumCB cb1(propParam);
				stRaiseFuwenNumCB cb2(propParam);
				m_bagMgr.m_mainBag.exeEveryOne(cb1);
				m_bagMgr.m_equipBag.exeEveryOne(cb2);
				return cb1.m_num + cb2.m_num;
			}
			break;
		case TaskProto::ET_COLLECT_FASHION_NUM :
			{
				stTotalFashionNumCB cb;
				m_bagMgr.m_fashionBag.exeEveryOne(cb);
				return cb.m_num + m_bagMgr.m_equipBag.getFashionNum();
			}
			break;
		/*case TaskProto::ET_RAISE_HERO_NUM :          //强化武神次数
			{
				stHeroInfoCBGetLv cb(1);
				this->m_heroMgr.exeEveryOne(cb);
				return cb.m_num;
			}
			break;
		case TaskProto::ET_RAISE_SKILL_NUM :     	 //强化技能次数
			{
				stGetSkillRaiseLvCB cb(1);
				m_skillMgr.exeEveryOne(cb);
				return cb.m_num;
			}
			break;
		case TaskProto::ET_RAISE_EQUIP_NUM :        //强化物品次数
			{
				stRaiseEquipNumCB cb1(1);
				stRaiseEquipNumCB cb2(1);
				
				m_bagMgr.m_mainBag.exeEveryOne(cb1);
				m_bagMgr.m_equipBag.exeEveryOne(cb2);
				return  cb1.m_num + cb2.m_num;

			}
			break;
		case TaskProto::ET_RAISE_FUWEN_NUM :        //强化符文次数
			{
				stRaiseFuwenNumCB cb1(1);
				stRaiseFuwenNumCB cb2(1);
				m_bagMgr.m_mainBag.exeEveryOne(cb1);
				m_bagMgr.m_equipBag.exeEveryOne(cb2);
				return cb1.m_num + cb2.m_num;
			}
			break;*/
		/*case TaskProto::ET_DRAW_NUM :				//抽奖次数
			{
				return m_countMgr.getDayRefreshValue(ECOUNT_DRAW_NUM);	
			}
			break;*/
		case TaskProto::ET_GET_WELFARE_CARD :        //领取福利卡
			{
				if (m_rmbShop.isBuyForeverItem()){
					return 1;
				}
				else {
					return 0;
				}
			}
			break;
		case TaskProto::ET_GET_MONTH_CARD :          //领取月卡
			{
				if (m_rmbShop.isBuyItem(CRMBShop::eItem_Year_Card)){
					return 0;
				}
				
				if (m_rmbShop.isBuyItem(CRMBShop::eItem_Quarter_Card)){
					return 0;
				}
				
				if (m_rmbShop.getItemEndTime(CRMBShop::eItem_Month_Card) > g_gameServer->getSec()){
					return 1;
				}
				else {
					return 0;
				}
			}
			break;
		case TaskProto::ET_GET_QUARTER_CARD :        //领取季度卡
			{
				if (m_rmbShop.isBuyItem(CRMBShop::eItem_Year_Card)){
					return 0;
				}
				
				if (m_rmbShop.getItemEndTime(CRMBShop::eItem_Quarter_Card) > g_gameServer->getSec()){
					return 1;
				}
				else {
					return 0;
				}

			}
			break;
		case TaskProto::ET_GET_YEAR_CARD :           //领取年卡
			{
				if (m_rmbShop.getItemEndTime(CRMBShop::eItem_Year_Card) > g_gameServer->getSec()){
					return 1;
				}
				else {
					return 0;
				}
			}
			break;
		case PT_USER_LEVEL:
			{
				return m_roleInfo.m_level;
			}
			break;
		case PT_USER_PRE_TASK :
			{
				if (m_taskMgr.isTaskReward(propParam)){
					return 1;
				}
				else {
					return 0;
				}
			}
			break;
		case PT_USER_FINISH_LV :
			{
				if (m_levelMgr.isLevelFinish(propParam)){
					return 1;
				}
				else {
					return 0;
				}
			}
			break;		
	}
	return 0;
}

void  GameUser::taskStatusUpdate(NOTIFY_TYPE type,stTaskDBInfo *pTaskInfo)
{
	if (NT_ADD == type){//增加一个任务
		TaskProto::stNotifyAddTask addCmd;
		TaskProto::stSynTaskInfo * pSynTaskInfo =  addCmd.mutable_taskinfo(); 
		pTaskInfo->fillSynInfo(*pSynTaskInfo);
		this->sendProto2Me(addCmd);
	}
	else if (NT_UPDATE == type){
		TaskProto::stRefreshTaskInfo refreshCmd;
		TaskProto::stSynTaskInfo * pSynTaskInfo =  refreshCmd.mutable_taskinfo();
		pTaskInfo->fillSynInfo(*pSynTaskInfo);
		this->sendProto2Me(refreshCmd);		
	}
	else if (NT_CANCEL == type){
		TaskProto::stCancelTask cancelCmd;
		cancelCmd.set_taskid(pTaskInfo->m_taskID);
		this->sendProto2Me(cancelCmd);
	}
	else if (NT_REWARD == type){
		TaskProto::stNotifyTaskFinish  finishCmd;
		finishCmd.set_taskid(pTaskInfo->m_taskID);
		this->sendProto2Me(finishCmd);
		stTaskTemplate * pTemplate =  CTaskTemplateMgr::getSingleton().getTemplateByID(pTaskInfo->m_taskID);
		if (pTemplate){
			for (UINT8 i = 0;i < pTemplate->m_reward.size(); ++i){
				stTaskReward & reward = pTemplate->m_reward[i];
				switch (reward.m_type){
					case TR_GOLDCOIN :
						{
							this->addGoldCoin(reward.m_num,"任务奖励");
						}
						break;
					case TR_POINTCARD :
						{
							this->addDiamond(reward.m_num,"任务奖励");
						}
						break;
					case TR_EXP :
						{
							this->addExp(reward.m_num);
						}
						break;
					case TR_ITEM :
						{
							this->giveAssert(reward.m_id,reward.m_num,0,"任务奖励");
#ifdef  _HDZ_DEBUG
							Athena::logger->trace("任务taskID=%d获得了道具奖励item=%d",pTemplate->m_taskID,reward.m_id);
#endif
						}
						break;
				}
			}
			
			if (DAILY_TASK == pTemplate->m_taskType){//任务类型
				Game::stDailyTaskLog webCmd;
				webCmd.set_account(this->getAccount());
				webCmd.set_charid(getID());
				webCmd.set_name(getName());
				webCmd.set_taskid(pTaskInfo->m_taskID);
				webCmd.set_taskname(pTemplate->m_name);
				g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
					
				m_taskMgr.addDailyTaskScore(pTemplate->m_daiyTaskScore);
			}
		}
	}
}

bool GameUser::intoNewMap(CMap * pNewMap,bool needInit, bool send2Client )
{
	if (!pNewMap){
		Athena::logger->error("(account=%s,name=%s,charID=%d)进入地图失败,新地图指针为NULL",getAccount(),getName(),getID());
		return false;
	}
	
	CPos pos;
	//bool firstLogin = false;
	if (m_roleInfo.m_bitmask & BITMASK_HAS_LOGON){//已经登录过了
		if (needInit) {
			pos.m_x = m_roleInfo.m_posX;
			pos.m_y = m_roleInfo.m_posY;
			pos.m_z = m_roleInfo.m_posZ;
			if (!pNewMap->isValidPos(pos)) {
				if (!pNewMap->getOnBirthPos(pos)){
					pos.m_x = 8000;
					pos.m_y = 203;
					pos.m_z = 8362;
				}
			}
		}
		else if (!pNewMap->getOnBirthPos(pos)){
				pos.m_x = 8000;
				pos.m_y = 203;
				pos.m_z = 8362;
		}
	}
	else if (!pNewMap->getOnBirthPos(pos)){//获得出生点失败默认进入下面的点
			pos.m_x = 8000;
			pos.m_y = 203;
			pos.m_z = 8362;
			//firstLogin = true;
	}
	else {
		//firstLogin = true;
	}

	if (!(m_roleInfo.m_bitmask & BITMASK_HAS_LOGON)){
		m_roleInfo.m_bitmask = m_roleInfo.m_bitmask | BITMASK_HAS_LOGON;
	}
	
	//发送进入地图的协议
	if (1 == m_roleInfo.m_newBieGuideID){//第一次登录进入开场秀
		//m_levelMgr.enterLv(OPEN_SHOW_MAPID);
	}
	else {
		//pNewMap->sendMapInfo(this);
	}
	if (send2Client)
	{
		pNewMap->sendMapInfo(this);
	}
	
	if (pNewMap->add(this,pos)) {//放置到场景中
		Athena::logger->trace("(account=%s,name=%s,charID=%d,x=%d,y=%d,pMap=%p)放置到地图场景里成功",getAccount(),getName(),getID(),pos.m_x,pos.m_y,getMap());
	}
	return true;
}

#define STATICS_EQUIP_REFINE_LV_ARRAY_NUM 4

void GameUser::getAppear(Map::stPlayerData & data)
{
	data.set_moid(getUniqueID());
	data.set_rolename(getName());
	data.set_job(m_roleInfo.m_job);
	data.set_sex(m_roleInfo.m_sex);
	data.set_level(m_roleInfo.m_level);
	data.set_x(getPos().m_x);
	data.set_y(getPos().m_y);
	data.set_z(getPos().m_z);
	data.set_dir(getPos().m_dir);
	data.set_campid(m_campID);
	data.set_hp(m_hp);
	data.set_maxhp(m_maxHP);
	data.set_titleid(m_titleMgr.getActiveTitleID());
	if(m_roleInfo.m_charID <= MAX_ROBOT_CHARID || m_bRobot){
		data.set_isrobot(true);
	}else{
		data.set_isrobot(false);
	}
	
	UINT16 refineEquipLV[STATICS_EQUIP_REFINE_LV_ARRAY_NUM] = { 0 };
	
	CItem *pHead = m_bagMgr.m_equipBag.getEquipByIndex(HEAD_SLOT_INDEX);
	if (pHead){
		data.set_headid(pHead->getID());

		if (pHead->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pHead->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}

	CItem *pBody = m_bagMgr.m_equipBag.getEquipByIndex(BODY_SLOT_INDEX);
	if (pBody){
		data.set_bodyid(pBody->getID());

		if (pBody->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pBody->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}

	CItem * pWeapon = m_bagMgr.m_equipBag.getEquipByIndex(WEAPON_SLOT_INDEX);
	if (pWeapon){
		data.set_weaponid(pWeapon->getID());

		if (pWeapon->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pWeapon->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}

	CItem * pCloack = m_bagMgr.m_equipBag.getEquipByIndex(CLOAK_SLOT_INDEX);
	if (pCloack){
		data.set_cloackid(pCloack->getID());

		if (pCloack->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pCloack->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}

	CItem * pWing = m_bagMgr.m_equipBag.getEquipByIndex(WING_SLOT_INDEX);
	if (pWing){
		data.set_wingid(pWing->getID());
		data.set_wingbreaklv(m_wingBreakLv);
	}

	CItem * pShoe = m_bagMgr.m_equipBag.getEquipByIndex(SHOE_SLOT_INDEX);
	if (pShoe){
		data.set_shoeid(pShoe->getID());

		if (pShoe->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pShoe->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}
	
	CItem * pRing  = m_bagMgr.m_equipBag.getEquipByIndex(RING_SLOT_INDEX);
	if (pRing){
		if (pRing->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pRing->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}	
	}
	
	CItem * pNecklace = m_bagMgr.m_equipBag.getEquipByIndex(NECKLACE_SLOT_INDEX);
	if (pNecklace){
		if (pNecklace->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pNecklace->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}	
	}

	for (INT16 i = STATICS_EQUIP_REFINE_LV_ARRAY_NUM - 1; i >= 0 ; --i){
		UINT16 totalEquipNum = 0;
		for (UINT16 j = i;j < STATICS_EQUIP_REFINE_LV_ARRAY_NUM;++j){
			totalEquipNum += refineEquipLV[j];	
		}

		if (totalEquipNum >= 7) {
			stRefineEffectBase * pEffectBase = g_refineEffectBaseMgr.get(stRefineEffectBase::getFindID(m_roleInfo.m_job,i + 5));
			if (pEffectBase){
				data.set_effectid(pEffectBase->m_id);
				break;	
			}
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("玩家(name:%s,id:%d)身上装备精炼等级是:",getName(),getID());
		for (UINT16 i = 0;i <STATICS_EQUIP_REFINE_LV_ARRAY_NUM ;++i){
			Athena::logger->trace("lv:%d,数量:%d",i+5,refineEquipLV[i]);
		}
	}
#endif
	if (this->m_systemSetting & Role::eSysSetting_ShowFashion) {
		CItem * pFashionHead = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_HEAD_SLOT_INDEX);
		if (pFashionHead){
			data.set_fashionhair(pFashionHead->getID());
		}

		CItem * pFashionFace = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_FACE_SLOT_INDEX);
		if (pFashionFace){
			data.set_fashionface(pFashionFace->getID());
		}

		CItem * pFashionBody = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_BODY_SLOT_INDEX);
		if (pFashionBody){
			data.set_fashionbody(pFashionBody->getID());
		}

		CItem * pFashionWeapon = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_WEAPON_SLOT_INDEX);
		if (pFashionWeapon){
			data.set_weaponid(pFashionWeapon->getID());
		}

		CItem * pFashionShoe = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_SHOE_SLOT_INDEX);
		if (pFashionShoe){
			data.set_fashionshoe(pFashionShoe->getID());
		}

		CItem * pFashionDecoration = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_RING_SLOT_INDEX);
		if (pFashionDecoration){
			data.set_fashiondecoration(pFashionDecoration->getID());
		}
	}

	data.set_bati(m_bati);
	data.set_maxbati(m_maxBati);
	data.set_mp(m_mp);
	data.set_maxmp(m_maxMP);
    UINT32 followSpriteId = m_spriteSysMgr.getFollowSprite();
    data.set_followspriteid(followSpriteId);
    data.set_followspritelvl(m_spriteSysMgr.getSpriteLvl(followSpriteId));
#ifdef _HDZ_DEBUG
	//Athena::logger->trace("玩家出现charID=%d,x=%d,y=%d,z=%d",getID(),data.x(),data.y(),data.z());
#endif
}

UINT32 GameUser::getCountByVip(UINT16 countType)
{
	stVipBase * pVipBase = NULL;
	if (g_gameServer->isAppVersion()){
		pVipBase = g_vipBaseMgr.get(countType * VIP_NUM_COEF + 15);
	}
	else {
		pVipBase = g_vipBaseMgr.get(countType * VIP_NUM_COEF + m_vipLv);
	}
	
	if (!pVipBase){
		return 0;
	}
	return pVipBase->m_num;
}

bool GameUser::addSP(UINT32 sp,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_sp += sp;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Sp);
	retCmd.set_num(m_roleInfo.m_sp);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s增加了体力%d",getName(),getID(),desc,sp);
	return true;
}

bool GameUser::removeSP(UINT32 sp,const char * desc)
{
	if (sp > m_roleInfo.m_sp){
		return false;	
	}	

	m_roleInfo.m_sp -= sp;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Sp);
	retCmd.set_num(m_roleInfo.m_sp);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s扣除了体力%d",getName(),getID(),desc,sp);

	Global::stUserDecSP  cmd;
	cmd.set_charid(getID());
	cmd.set_sp(sp);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	return true;
}

bool GameUser::checkSP(UINT32 sp)
{
	if (m_roleInfo.m_sp >= sp){
		return true;
	}	
	else {
		return false;
	}
}


bool GameUser::addTrainSP(UINT32 sp,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_trainSp += sp;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_TrainSp);
	retCmd.set_num(m_roleInfo.m_trainSp);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s增加了调教体力%d",getName(),getID(),desc,sp);
	return true;
}

bool GameUser::removeTrainSP(UINT32 sp,const char * desc)
{
	if (sp > m_roleInfo.m_trainSp){
		return false;	
	}	

	m_roleInfo.m_trainSp -= sp;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_TrainSp);
	retCmd.set_num(m_roleInfo.m_trainSp);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s扣除了调教体力%d",getName(),getID(),desc,sp);

	return true;
}

bool GameUser::checkTrainSP(UINT32 sp)
{
	if (m_roleInfo.m_trainSp >= sp){
		return true;
	}	
	else {
		return false;
	}
}

bool GameUser::addLilian(UINT32 lilian,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_lilian += lilian;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Lilian);
	retCmd.set_num(m_roleInfo.m_lilian);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s增加了历练%d",getName(),getID(),desc,lilian);
	return true;
}

bool GameUser::removeLilian(UINT32 lilian,const char * desc)
{
	if (m_roleInfo.m_lilian < lilian){
		return false;
	}	

	m_roleInfo.m_lilian -= lilian;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Lilian);
	retCmd.set_num(m_roleInfo.m_lilian);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s扣除了历练%d",getName(),getID(),desc,lilian);
	
	Global::stUsreDecLilian cmd;
	cmd.set_charid(getID());
	cmd.set_lilian(lilian);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	return true;
}

bool GameUser::checkLilian(UINT32 lilian)
{
	if (m_roleInfo.m_lilian >= lilian){
		return true;
	}	
	else {
		return false;
	}
}

bool GameUser::addSoulFire(UINT32 soulFire,const char * desc)
{
	if (!desc){
		return false;
	}

	m_roleInfo.m_soulFire += soulFire;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Soul_Fire);
	retCmd.set_num(m_roleInfo.m_soulFire);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s增加了魂火%d",getName(),getID(),desc,soulFire);
	return true;
}

bool GameUser::removeSoulFire(UINT32 soulFire,const char * desc)
{
	if (m_roleInfo.m_soulFire < soulFire){
		return false;
	}	

	m_roleInfo.m_soulFire -= soulFire;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Soul_Fire);
	retCmd.set_num(m_roleInfo.m_soulFire);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s扣除了魂火%d",getName(),getID(),desc,soulFire);
	
	return true;
}

bool GameUser::checkSoulFire(UINT32 soulFire)
{
	if (m_roleInfo.m_soulFire >= soulFire){
		return true;
	}	
	else {
		return false;
	}
}

bool GameUser::addLifeEssence(UINT32 fileEssence,const char * desc)
{
	if(!desc){
		return false;
	}	

	m_roleInfo.m_lifeEssence += fileEssence;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Life_Essence);
	retCmd.set_num(m_roleInfo.m_lifeEssence);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s增加了生命精华%d",getName(),getID(),desc,fileEssence);

	return true;
}

bool GameUser::removeLifeEssence(UINT32 fileEssence,const char * desc)
{
	if(m_roleInfo.m_lifeEssence < fileEssence){
		return false;
	}

	m_roleInfo.m_lifeEssence -= fileEssence;
	Role::stSynResource retCmd;
	retCmd.set_restype(eResource_Life_Essence);
	retCmd.set_num(m_roleInfo.m_lifeEssence);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("name=%s,id=%d通过%s扣除了生命精华%d",getName(),getID(),desc,fileEssence); 

	return true;
}

bool GameUser::checkLifeEssence(UINT32 fileEssence)
{
	if(m_roleInfo.m_lifeEssence >= fileEssence){
		return true;
	}else{
		return false;
	}
}

bool GameUser::addGuildCrystal(UINT32 guildCrystal,const char * desc) 
{
	if (!desc){                                                                              
		return false;                                                                        
	}

	m_roleInfo.m_guidCrystal += guildCrystal;
	Role::stSynResource retCmd;                                                              
	retCmd.set_restype(eResource_Guild_Crystal);                                                 
	retCmd.set_num(m_roleInfo.m_guidCrystal);                                                   
	this->sendProto2Me(retCmd);                                                              
	Athena::logger->trace("name=%s,id=%d通过%s增加了公会黑市结晶%d",getName(),getID(),desc,guildCrystal);

	return true;                                                                             
}

bool GameUser::removeGuildCrystal(UINT32 guildCrystal,const char * desc)
{
	if (m_roleInfo.m_guidCrystal < guildCrystal){
	    return false;                                                                         
	}                                                                                         
                                                                                          
	m_roleInfo.m_guidCrystal -= guildCrystal;
	Role::stSynResource retCmd;                                                               
	retCmd.set_restype(eResource_Guild_Crystal);
	retCmd.set_num(m_roleInfo.m_guidCrystal);
	this->sendProto2Me(retCmd);                                                               
	Athena::logger->trace("name=%s,id=%d通过%s扣除了公会黑市结晶%d",getName(),getID(),desc,guildCrystal); 
                                                                                          
	return true;                                                                              
}

bool GameUser::checkGuildCrystal(UINT32 guildCrystal)
{
	if(m_roleInfo.m_guidCrystal >= guildCrystal){
		return true;         
	}else{
		return false;        
	} 
}

void GameUser::calcVipLv()
{
	UINT16 oldVip = m_vipLv;
	m_vipLv = CMiscCfgMgr::getSingleton().getVipLv(m_roleInfo.m_rechargeNum + m_roleInfo.m_giveRechargeNum);
	m_roleState.m_maxSP = this->getCountByVip(eVipFun_MaxSp);
	m_roleState.m_maxLilian = this->getCountByVip(eVipFun_MaxLilian);
	m_roleState.m_maxTrainSP = this->getCountByVip(eVipFun_MaxTrainSp);
	m_bagMgr.setBagSize(Role::BAG_TYPE_MAIN1,getCountByVip(eVipFun_MainBagNum));
	m_bagMgr.setBagSize(Role::BAG_TYPE_FASHION,getCountByVip(eVipFun_FashionBagNum));

	for (;oldVip <= m_vipLv;++oldVip){
		m_titleMgr.checkObtainTitle(eTitleGotType_VipLevelup,oldVip);	
	}

	/*if (oldVip != m_vipLv){
		Global::stSynVipLv cmd;
		cmd.set_charid(getID());
		cmd.set_viplv(m_vipLv);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
	}*/
	m_roleInfo.m_vipLv = m_vipLv;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("name=%s,charID=%d 当前viplv=%d,maxSP=%d,maxLilian=%d",getName(),getID(),m_vipLv,m_roleState.m_maxSP,m_roleState.m_maxLilian);
#endif
    synBackResVip(oldVip, m_vipLv);
}

void GameUser::synTotalRecharge()
{
	Role::stSynTotoalRecharge retCmd;
	retCmd.set_totalrecharge(m_roleInfo.m_rechargeNum + m_roleInfo.m_giveRechargeNum);
	retCmd.set_todayrecharge(m_countMgr.getDayRefreshValue(ECOUT_TODAY_RECHARGE));
	this->sendProto2Me(retCmd);
}

void GameUser::synMyLevel(bool bLevelup)
{
	Global::stSynUserLevel cmd;
	cmd.set_charid(getID());
	cmd.set_lv(getLevel());
	cmd.set_levelup(bLevelup);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GameUser::synMyBattlePower()
{
	Global::stSynUserBattlePower cmd;
	cmd.set_charid(getID());
	cmd.set_battlepower(m_roleInfo.m_battlePower);
	cmd.set_maxhp(m_maxHP);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GameUser::synMyCopymapStarNum()
{
	Global::stSynCopymapStarNum cmd;
	cmd.set_charid(getID());
	cmd.set_starnum(m_levelMgr.getStarNum());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GameUser::synMyPhotoID()
{
	Global::stSynUserPhotoID cmd;
	cmd.set_charid(getID());
	cmd.set_photoid(m_photoMgr.getUsePhotoID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GameUser::fillDetailInfo(Role::stUserDetailInfo * pDetail)
{
	fillMainData(pDetail->mutable_maindata());
	m_bagMgr.m_equipBag.fillMyEquip(pDetail);	
	m_heroMgr.fillMyHero(pDetail);
}

bool GameUser::giveAssert(UINT32 itemID,UINT32 num,UINT16 raiseLv,const char * desc)
{
	if (!desc){
		return false;
	}
	
	if (itemID <= eResource_ID_Max){
		return this->addResource((eResource)itemID,num,desc);
	}
	return m_bagMgr.addItem2Bag(itemID,num,raiseLv,desc);
}

bool GameUser::checkAssert(UINT32 itemID,UINT32 num)
{
	if (itemID <= eResource_ID_Max){
		return this->checkResource((eResource)itemID,num);
	}
	else {
        Athena::logger->trace("[checkAssert] item id is %d, num is %d", itemID, m_bagMgr.getItemNumByID(itemID));
		return m_bagMgr.getItemNumByID(itemID) >= num;
	}
}

bool GameUser::removeAssert(UINT32 itemID,UINT32 num,UINT16 raiseLv,const char * desc)
{
	if (!desc){
		return false;
	}
	
	if (itemID <= eResource_ID_Max){
		return this->removeResource((eResource)itemID,num,desc);
	}
	return m_bagMgr.removeItemByItemID(itemID,raiseLv,num,desc);
}

void GameUser::calcPKData()
{
	m_pkData.m_damage = m_roleState.m_battleProps[eBATTLE_PROP_DAMAGE];
	m_pkData.m_antiBangDamge = m_roleState.m_battleProps[eBATTLE_PROP_ANTIBANGDAMAGE]; 
	m_pkData.m_antiBangRate =   m_roleState.m_battleProps[eBATTLE_PROP_ANTIBANGRATE];
	m_pkData.m_bangDamage = m_roleState.m_battleProps[eBATTLE_PROP_BANGDAMAGE];
	m_pkData.m_bangRate =  m_roleState.m_battleProps[eBATTLE_PROP_BANGRATE];
	m_pkData.m_damageReduce = m_roleState.m_battleProps[eBATTLE_PROP_DAMAGEREDUCE];
	m_pkData.m_dodgeRate = m_roleState.m_battleProps[eBATTLE_PROP_DODGERATE];
	m_pkData.m_finalDamage =  m_roleState.m_battleProps[eBATTLE_PROP_FINALDAMAGE];
	m_pkData.m_hitRate = m_roleState.m_battleProps[eBATTLE_PROP_HITRATE];
	m_pkData.m_fireRate = m_roleState.m_battleProps[eBATTLE_PROP_FIRERATE];
	m_pkData.m_iceRate = m_roleState.m_battleProps[eBATTLE_PROP_ICERATE];
	m_pkData.m_thunderRate = m_roleState.m_battleProps[eBATTLE_PROP_THUNDERRATE];
	m_pkData.m_fireDamage = m_roleState.m_battleProps[eBATTLE_PROP_FIREDAMAGE];
	m_pkData.m_iceDamage = m_roleState.m_battleProps[eBATTLE_PROP_ICEDAMAGE];
	m_pkData.m_thunderDamage = m_roleState.m_battleProps[eBATTLE_PROP_THUNDERDAMAGE];
	m_pkData.m_refFireDamage = m_roleState.m_battleProps[eBATTLE_PROP_ANTI_FIRE_DAMAGE];
	m_pkData.m_refIceDamage = m_roleState.m_battleProps[eBATTLE_PROP_ANTI_ICE_DAMAGE];
	m_pkData.m_refThunderDamage = m_roleState.m_battleProps[eBATTLE_PROP_ANTI_THUNDER_DAMAGE];
	m_pkData.m_refFireRate = m_roleState.m_battleProps[eBATTLE_PROP_REF_FIRE_RATE];
	m_pkData.m_refIceRate = m_roleState.m_battleProps[eBATTLE_PROP_REF_ICE_RATE];
	m_pkData.m_refThunderRate = m_roleState.m_battleProps[eBATTLE_PROP_REF_THUNDER_RATE];

	m_pkData.m_damage = (UINT32)(m_pkData.m_damage * (1 + m_extraDamagePer / 100.0f));
}

void GameUser::onLeaveMap(CMap *pMap)
{
	if (!pMap){
		return ;
	}
	m_extraDamagePer = 0;
	//m_lastIntoMapID = pMap->getTempID();
	if (pMap->isCopyMap()){//是副本,且没有人
		((CCopyMap*)pMap)->onUserLeave(this);
		if ((pMap->getPlayerCount() == 0) && (!((CCopyMap*)pMap)->isGuildHall())) {//不是公会大厅
			pMap->unLoadMap();	
		}

		if (((CCopyMap*)pMap)->isGuildHall()){
			m_lastIntoMapID = pMap->getTempID();	
		}
		else {
			m_lastIntoMapID = 0;
		}
	}
	else {
		m_lastIntoMapID = pMap->getTempID();
	}
	m_stateMgr.clearAllBuff();	
}

void GameUser::onEnterMap(CMap *pMap)
{
	if (!pMap){
		return ;
	}
	this->clearState(Map::BEING_STATE_DEATH);
	m_roleInfo.m_mapID = pMap->getTempID();
	Athena::logger->trace("on enter map : m_roleInfo.m_mapID %d", m_roleInfo.m_mapID);
	if (pMap->isCopyMap()){
		((CCopyMap*)pMap)->onUserEnter(this);
		m_lastHitByNormalTime = g_gameServer->getSec();
	}
}

void GameUser::processDeath(CBeing *pAtter)
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("开始处理玩家死亡");
#endif
	if (!this->getMap()){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("处理玩家死亡,找不到攻击者跟地图!");
#endif
		return ;
	}
	if (this->isSetState(Map::BEING_STATE_DEATH)){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("处理玩家死亡,但是还已经设置了死亡状态");
#endif
		return ;
	}
	this->setState(Map::BEING_STATE_DEATH);
	m_deathTime = g_gameServer->getSec();

	Map::stNotifyObjDie dieCmd;
	dieCmd.set_motype(this->getType());
	dieCmd.set_moid(this->getUniqueID());
	getMap()->sendProto2Map(dieCmd);

	
	if (getMap() && getMap()->isCopyMap()){
		((CCopyMap *)getMap())->onPlayerDie(pAtter,this);
	}
}

void GameUser::synContribyte2Global()
{
	Global::stSynMyContribyte cmd;
	cmd.set_charid(getID());
	cmd.set_mycontribute(m_roleInfo.m_contribute);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

UINT32 GameUser::getRecoverMP()
{
	return m_roleState.m_battleProps[eBATTLE_PROP_RECOVERMP];
}

UINT32 GameUser::getRecoverHP()
{
	return m_roleState.m_battleProps[eBATTLE_PROP_RECOVERHP];	
}

UINT32 GameUser::getAbsorbHPPer()
{
	return m_roleState.m_battleProps[eBATTLE_PROP_ABSORBHP];
}

void   GameUser::hitByNormal()
{
	m_lastHitByNormalTime = g_gameServer->getSec();	
}

void   GameUser::batiEmpty()
{
	m_batiEmptyTime = g_gameServer->getSec();	
}

void GameUser::synMyInfoToGlobal()
{
	Global::stUserEnterMapSuccess synUserCmd;
	synUserCmd.set_charid(getID());
	synUserCmd.set_gmtype(m_gmType);
	synUserCmd.set_level(getLevel());
	synUserCmd.set_job(m_roleInfo.m_job);
	synUserCmd.set_sex(m_roleInfo.m_sex);
	synUserCmd.set_serverid(m_roleInfo.m_serverID);
	synUserCmd.set_fetchliliannum(m_countMgr.getDayRefreshValue(ECOUNT_FETCH_LILIAN_NUM));
	synUserCmd.set_viplv(getVipLv());
	synUserCmd.set_compensationid(m_compensationID);
	synUserCmd.set_effectid(getEffectID());
	synUserCmd.set_wingbreaklv(m_wingBreakLv);
	synUserCmd.set_pubserverindex(m_normalServerIndex);
	synUserCmd.set_pubcharid(getUniquePubID());
	synUserCmd.set_areacode(m_areaCode);
	synUserCmd.set_photoid(m_photoMgr.getUsePhotoID());

	CItem *pHead = m_bagMgr.m_equipBag.getEquipByIndex(HEAD_SLOT_INDEX);
	if (pHead){
		synUserCmd.set_headid(pHead->getID());
	}

	CItem *pBody = m_bagMgr.m_equipBag.getEquipByIndex(BODY_SLOT_INDEX);
	if (pBody){
		synUserCmd.set_bodyid(pBody->getID());
	}

	CItem * pWeapon = m_bagMgr.m_equipBag.getEquipByIndex(WEAPON_SLOT_INDEX);
	if (pWeapon){
		synUserCmd.set_weaponid(pWeapon->getID());
	}

	CItem * pCloack = m_bagMgr.m_equipBag.getEquipByIndex(CLOAK_SLOT_INDEX);
	if (pCloack){
		synUserCmd.set_cloackid(pCloack->getID());
	}

	CItem * pWing = m_bagMgr.m_equipBag.getEquipByIndex(WING_SLOT_INDEX);
	if (pWing){
		synUserCmd.set_wingid(pWing->getID());
	}

	CItem * pShoe = m_bagMgr.m_equipBag.getEquipByIndex(SHOE_SLOT_INDEX);
	if (pShoe){
		synUserCmd.set_shoeid(pShoe->getID());
#ifdef _HDZ_DEBUG
		Athena::logger->trace("上线同步了玩家:%s的裤子:%d",getName(),pShoe->getID());
#endif
	}

	if (this->m_systemSetting & Role::eSysSetting_ShowFashion) {
		CItem * pFashionHead = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_HEAD_SLOT_INDEX);
		if (pFashionHead){
			synUserCmd.set_fashionhair(pFashionHead->getID());
		}

		CItem * pFashionFace = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_FACE_SLOT_INDEX);
		if (pFashionFace){
			synUserCmd.set_fashionface(pFashionFace->getID());
		}

		CItem * pFashionBody = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_BODY_SLOT_INDEX);
		if (pFashionBody){
			synUserCmd.set_fashionbody(pFashionBody->getID());
		}

		CItem * pFashionWeapon = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_WEAPON_SLOT_INDEX);
		if (pFashionWeapon){
			synUserCmd.set_fashionweapon(pFashionWeapon->getID());
		}

		CItem * pFashionShoe = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_SHOE_SLOT_INDEX);
		if (pFashionShoe){
			synUserCmd.set_fashionshoe(pFashionShoe->getID());
		}

		CItem * pFashionDecoration = m_bagMgr.m_equipBag.getEquipByIndex(FASHION_RING_SLOT_INDEX);
		if (pFashionDecoration){
			synUserCmd.set_fashiondecoration(pFashionDecoration->getID());
		}
	}
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(synUserCmd);
}
	
void GameUser::addTeamCopyMapNum(UINT16 num)
{
	Role::stSynTeamCopyMapNum retCmd;
	//m_countMgr.addDayRefreshValue(ECOUNT_TEAM_COPYMAP_NUM,num);
	m_levelMgr.addTeamCopymapNum(num);
	retCmd.set_num(m_levelMgr.getTeamCopymapNum());
	this->sendProto2Me(retCmd);
}

void GameUser::addReamTimePKNum(UINT16 num)
{
	Role::stSynRealTimePKNum retCmd;
	m_countMgr.addDayRefreshValue(ECOUNT_REAL_TIME_NUM,num);
	this->m_sevenDayActivityMgr.addRealTimePkNum(num);
	retCmd.set_num(m_countMgr.getDayRefreshValue(ECOUNT_REAL_TIME_NUM));
	this->sendProto2Me(retCmd);
}
void GameUser::addGiveVipExp(UINT32 num)
{
	m_roleInfo.m_giveRechargeNum +=  num;
	calcVipLv();
	synTotalRecharge();
}
	
void GameUser::addVipExp(UINT32 num)
{
	m_roleInfo.m_rechargeNum +=  num;
	calcVipLv();
	synTotalRecharge();
}

void GameUser::addOne2OnePVPScore(UINT32 score)
{
	m_roleInfo.m_one2OnePVPScore += score;
	Athena::logger->trace("玩家(account:%s,name:%s,id:%d)获得1v1的积分:%d,当前积分:%d",getAccount(),getName(),getID(),score,m_roleInfo.m_one2OnePVPScore);
}

void GameUser::removeOne2OnePVPScore(UINT32 score)
{
	if (m_roleInfo.m_one2OnePVPScore >= score){
		m_roleInfo.m_one2OnePVPScore -= score;
	}
	else {
		m_roleInfo.m_one2OnePVPScore = 0;
	}
	Athena::logger->trace("玩家(account:%s,name:%s,id:%d)扣除1v1的积分:%d,当前积分:%d",getAccount(),getName(),getID(),score,m_roleInfo.m_one2OnePVPScore);
}

struct stResetUserCollectTime : public MapObjCallBack
{
	stResetUserCollectTime()
	{}

	~stResetUserCollectTime()
	{
	
	}

	virtual bool operator()(CObj* pObj)
	{    
		if (!pObj || !pObj->theGameUser()){
			return true;
		}    
		GameUser * pUser = (GameUser *)pObj;
		if (pUser->m_beginCollectTime > 0){
			pUser->m_beginCollectTime = 0;
			Map::stOpCopyMapRes retCmd;
			retCmd.set_collect(false);
			pUser->sendProto2Me(retCmd);
		}
		return true;
	} 
};

void GameUser::checkCollect()
{
	if (!m_beginCollectTime){
		return ;
	}

	if (m_beginCollectTime + ArenaConfig::getSingleton().getMobaCollectTime() < g_gameServer->getSec()){
		m_beginCollectTime = 0;
		Map::stPlayerCollectSuccess retCmd;
		retCmd.set_score(CCopyMap::s_COLLECT_GOT_SCORE);
		this->sendProto2Me(retCmd);
		
		UINT32 resID = 0;
		if (this->getMap() && this->getMap()->isCopyMap()){
			resID = ((CCopyMap *)this->getMap())->checkCollideResZone(this->getPos());
		}

		Map::stNotifyResDisappear retCmd1;
		retCmd1.set_id(resID);
		if (this->getMap()){
			this->getMap()->sendProto2Map(retCmd1);
		}

		if (this->getMap() && this->getMap()->isCopyMap()){
			((CCopyMap *)this->getMap())->addCampScore(this->getCampID(),CCopyMap::s_COLLECT_GOT_SCORE);
			((CCopyMap *)this->getMap())->onResCollect(this,resID);
		}

		if (getMap()){
			stResetUserCollectTime cb;
			getMap()->execEveryObjInMap(cb,CObj::OBJ_TYPE_PLAYER);
		}
	}
}

void GameUser::checkRelive()
{
	if (!this->isSetState(Map::BEING_STATE_DEATH)){
		return;
	}
	
	if (!getMap() || !getMap()->isCopyMap()){
		return ;
	}
	
	if (!((CCopyMap *)getMap())->isMobaCopyMap()){
		return ;
	}

	if (m_deathTime + ArenaConfig::getSingleton().getMobaReliveTime() < g_gameServer->getSec()){
		m_deathTime = 0;
		relive();
	}
}

void GameUser::relive()
{
	this->clearState(Map::BEING_STATE_DEATH);
	resetBattleProp();
	CPos bornPos;
	if (this->getMap()){
		UINT16 bornPosID = 1;
		
		if (Map::eCamp_Red == this->getCampID()){
			bornPosID = g_randBetween(1,3);
		}
		else {
			bornPosID = g_randBetween(4,6);
		}
		this->getMap()->getOnBirthPos(bornPos,bornPosID);
	}
	this->move(bornPos,true);
	Map::stUserRelive retCmd;
	retCmd.set_charid(getID());
	retCmd.set_x(this->getPos().m_x);
	retCmd.set_y(this->getPos().m_y);
	retCmd.set_z(this->getPos().m_z);
	if (getMap()){
		getMap()->sendProto2Map(retCmd);
	}

	if (this->getMap() && this->getMap()->isCopyMap()){
		((CCopyMap*)(this->getMap()))->onUserRelive(this);
	}
	m_heroMgr.summonHero();
}

void GameUser::sendLoginLog(bool bEnterSelectLog)
{
	/////打印日志
	Game::stUserLoginLog webCmd;
	webCmd.set_account(getAccount());
	webCmd.set_name(getName());
	webCmd.set_ip(m_ip);
	webCmd.set_charid(getID());
	webCmd.set_isadvuser(m_roleInfo.m_isAdvUser);
	webCmd.set_createtime(m_roleInfo.m_createTime);
	webCmd.set_opid(m_roleInfo.m_opID);
	webCmd.set_deviceid(m_deviceID);
	webCmd.set_isenterselectlog(bEnterSelectLog);

	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

	////////////////////////////////////////////////
	/*--------------- 打生态日志 ----------------*/
	uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
	uuzu::logsdk::UserInfo userInfo;
	userInfo.level = getLevel();
	userInfo.passport = m_roleInfo.m_account; 
	userInfo.uid = g_int2string(getID());   
	userInfo.clientip = getIP();
	userInfo.exp = m_roleInfo.m_exp;
	userInfo.blackcoin = m_roleInfo.m_giveDiamond;
	userInfo.moneycoin =  m_roleInfo.m_diamond;
	userInfo.gamecoin = m_roleInfo.m_goldCoin;
	userInfo.device = m_deviceID;
	userInfo.viplevel = getVipLv();  
	userInfo.firstintime = m_firstLogin;
	std::string opID = g_int2string(m_roleInfo.m_opID);
	userInfo.version = Athena::global["gameVersion"];
	logger.setPID(opID.c_str());
	logger.sendDataLog(userInfo,this->m_channelID.c_str(), "on", "0","","","","","","login");
	////////////////////////////////////////////////

	/*-------- 韩国项目日志 ----------*/
	CKoreaLogMgr::getSingleton().loginOn(g_gameServer->getSec(),m_roleInfo.m_account,getID(),getIP(),getLevel());
	/*-------- 韩国项目日志 ----------*/
}

UINT16 GameUser::getEffectID()
{
	UINT16 refineEquipLV[STATICS_EQUIP_REFINE_LV_ARRAY_NUM] = { 0 };
	
	CItem *pHead = m_bagMgr.m_equipBag.getEquipByIndex(HEAD_SLOT_INDEX);
	if (pHead){
		if (pHead->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pHead->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}

	CItem *pBody = m_bagMgr.m_equipBag.getEquipByIndex(BODY_SLOT_INDEX);
	if (pBody){
		if (pBody->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pBody->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}

	CItem * pWeapon = m_bagMgr.m_equipBag.getEquipByIndex(WEAPON_SLOT_INDEX);
	if (pWeapon){
		if (pWeapon->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pWeapon->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}

	CItem * pCloack = m_bagMgr.m_equipBag.getEquipByIndex(CLOAK_SLOT_INDEX);
	if (pCloack){
		if (pCloack->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pCloack->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}

	CItem * pShoe = m_bagMgr.m_equipBag.getEquipByIndex(SHOE_SLOT_INDEX);
	if (pShoe){
		if (pShoe->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pShoe->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}
	}
	
	CItem * pRing  = m_bagMgr.m_equipBag.getEquipByIndex(RING_SLOT_INDEX);
	if (pRing){
		if (pRing->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pRing->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}	
	}
	
	CItem * pNecklace = m_bagMgr.m_equipBag.getEquipByIndex(NECKLACE_SLOT_INDEX);
	if (pNecklace){
		if (pNecklace->m_itemInfo.m_refineLv >= 5){
			UINT8 index = pNecklace->m_itemInfo.m_refineLv - 5;
			if (index < STATICS_EQUIP_REFINE_LV_ARRAY_NUM){
				refineEquipLV[index] += 1;
			}
		}	
	}

	for (INT16 i = STATICS_EQUIP_REFINE_LV_ARRAY_NUM - 1; i >= 0 ; --i){
		UINT16 totalEquipNum = 0;
		for (UINT16 j = i;j < STATICS_EQUIP_REFINE_LV_ARRAY_NUM;++j){
			totalEquipNum += refineEquipLV[j];	
		}

		if (totalEquipNum >= 7) {
			stRefineEffectBase * pEffectBase = g_refineEffectBaseMgr.get(stRefineEffectBase::getFindID(m_roleInfo.m_job,i + 5));
			if (pEffectBase){
				return pEffectBase->m_id;
			}
		}
	}
	return 0;
}

void GameUser::calcCreateRoleDay()
{
	struct tm curTm,createTm;

	RealTime::getLocalTime(curTm,g_gameServer->getSec());
	RealTime::getLocalTime(createTm,m_firstLogin);
	m_createRoleDay = g_dayDiff(curTm.tm_year + 1900,curTm.tm_mon + 1,curTm.tm_mday,createTm.tm_year + 1900,createTm.tm_mon + 1,createTm.tm_mday) + 1;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("计算出来的创建角色天数:%d",m_createRoleDay);
#endif
}

void GameUser::closeBetaReward(UINT32 boxID,UINT32 diamond,UINT32 rechargeNum)
{
	m_boxMgr.openBox(boxID,1);
	m_roleInfo.m_giveDiamond 	+= diamond;
	m_roleInfo.m_giveRechargeNum += rechargeNum;
	Role::stGotCloseBetaRewardResult retCmd;
	retCmd.set_result(Role::stGotCloseBetaRewardResult::eSuccess);
	retCmd.set_givediamond(diamond);
	retCmd.set_giverecharge(rechargeNum);
	this->sendProto2Me(retCmd);
	Athena::logger->trace("account:%s,name:%s,charID:%d 领取了封测礼包,获得了钻石:%d,充值经验:%d!",getAccount(),getName(),getID(),diamond,rechargeNum);
}

void GameUser::changeName(const std::string & newName)
{
	strncpy(m_roleInfo.m_name, newName.c_str(), MAX_NAMESIZE);
 	
	synName(newName);
	
	synMyLevel();
	synMyBattlePower();
	synMyCopymapStarNum();
}

void GameUser::synName(const std::string & name)
{
	Global::stSynName cmd;
	cmd.set_charid(getID());
	cmd.set_name(name);
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
}

void GameUser::forceSetHP(UINT32 hp)
{
	m_hp = hp;
	m_lastHP = m_hp;

	if (m_hp > m_maxHP){
		m_hp = m_maxHP;
		m_lastHP = m_hp;
	}
}

bool GameUser::isValidVipItem(UINT32 itemId) {
    std::map<UINT32, UINT32> vipItemMap;
    vipItemMap.insert(std::make_pair(271800, 6));
    vipItemMap.insert(std::make_pair(272000, 11));
    vipItemMap.insert(std::make_pair(272500, 14));
    vipItemMap.insert(std::make_pair(273000, 15));

    vipItemMap.insert(std::make_pair(271801, 6));
    vipItemMap.insert(std::make_pair(271802, 6));
    vipItemMap.insert(std::make_pair(271803, 6));
    vipItemMap.insert(std::make_pair(271804, 6));
    vipItemMap.insert(std::make_pair(271805, 6));

    vipItemMap.insert(std::make_pair(272001, 11));
    vipItemMap.insert(std::make_pair(272002, 11));
    vipItemMap.insert(std::make_pair(272003, 11));
    vipItemMap.insert(std::make_pair(272004, 11));
    vipItemMap.insert(std::make_pair(272005, 11));

    vipItemMap.insert(std::make_pair(272501, 14));
    vipItemMap.insert(std::make_pair(272502, 14));
    vipItemMap.insert(std::make_pair(272503, 14));
    vipItemMap.insert(std::make_pair(272504, 14));
    vipItemMap.insert(std::make_pair(272505, 14));

    vipItemMap.insert(std::make_pair(273001, 15));
    vipItemMap.insert(std::make_pair(273002, 15));
    vipItemMap.insert(std::make_pair(273003, 15));
    vipItemMap.insert(std::make_pair(273004, 15));
    vipItemMap.insert(std::make_pair(273005, 15));

    std::map<UINT32, UINT32>::iterator it = vipItemMap.find(itemId);
    if (m_vipLv < it->second) {
        return false;
    }
    return true;
}

void GameUser::sendData2PubGame()
{
	PubGame::stRoleInfo cmd;
	cmd.set_charid(getUniquePubID());
	cmd.set_account(getAccount());
	cmd.set_ip(getIP());
	cmd.set_normalserverid(g_gameServer->getPubServerIndex());
	cmd.set_normalcharid(getID());
	cmd.set_areacode(g_gameServer->getAreaCode());
	cmd.set_mapid(m_roleInfo.m_mapID);

	m_monsterMgr.saveData(m_roleInfo.m_monsterInfo);
	m_mountMgr.saveData(m_roleInfo.m_mountInfo);

	PubGame::stTitleToPub* st = cmd.mutable_titles();
	m_titleMgr.pack(st);

	RoleInfoForSave  forSave;
	bzero(&forSave,sizeof(forSave));
	bcopy(&m_roleInfo,&forSave.m_roleInfo,sizeof(m_roleInfo));
	UINT32 binSize = compressSaveData(forSave.m_binData);
	cmd.set_datasize(sizeof(m_roleInfo) + binSize);
	cmd.set_data(&forSave,cmd.datasize());
	bool ret = PubSvrClientMgr::getSingleton().broadcast(cmd);
	if (!ret)
	{
		Athena::logger->error("SendData2PubGame failed");
    } else {
        if (getMap() && getMap()->isCopyMap() && ((CCopyMap *)getMap())->isGuildHall()) {
            CMap * pMap = CMapMgr::getSingleton().getMapByID(1);
            CMapMgr::getSingleton().changeMap(this, pMap);
        }
    }
}

//通知normal 用户在跨服
void GameUser::NotifyNormalOnPub(bool bOnPub)
{
	if (m_bRobot){//是机器人
		return;
	}

	PubGame::stNotifyUserOnPub cmd;
	cmd.set_charid(getNormalCharID());
	cmd.set_onpub(bOnPub);
	bool ret = g_gameServer->getConnMgr().broadcastByID(m_normalServerIndex,cmd);
	if (!ret)
	{
		Athena::logger->error("NotifyNormalOnPub failed charID=%d serverIndex=%d name=%s", getNormalCharID(),m_normalServerIndex,getName());
	}else
	{
		Athena::logger->trace("NotifyNormalOnPub ok  charID=%d", getNormalCharID());
	}
}

//通知客户端用户跨服唯一id
void GameUser::NotifyPubUniqueID()
{
	Role::stNotifyPubUniqueID cmd;
	cmd.set_uniqueid(getUniquePubID());
	sendProto2Me(cmd);
}

//通知pubserver用户离线
void GameUser::NotifyPubLogout()
{
	if (g_gameServer->isPubGameServer())
	{
		return;
	}

	PubGame::stRoleLogout	cmd;
	cmd.set_charid(getUniquePubID());

	bool ret = PubSvrClientMgr::getSingleton().broadcast(cmd);
	if (!ret)
	{
		Athena::logger->error("NotifyPubLogout failed charID[%d]",getID());
	}
}

void GameUser::loadEnchant() {
    char * buf = m_roleInfo.m_enchantData;
    UINT32 ret = 0;
    UINT8 totalNum = *(UINT8 *)(buf + ret);
    ret += sizeof(UINT8);
    EnchantItem item;
    std::vector<EnchantItem> itemVec;
    UINT8 itemNum = 0;
    UINT64 instanceId = 0;
    for (UINT8 i = 0; i < totalNum; i++) {
        instanceId = *(UINT64 *)(buf + ret);
        ret += sizeof(UINT64);
        itemNum = *(UINT8 *)(buf + ret);
        ret += sizeof(UINT8);
        itemVec.clear();
        for (UINT8 k = 0; k < itemNum; k++) {
            item.color = *(UINT8 *)(buf + ret);
            ret += sizeof(UINT8);
            item.locked = *(UINT8 *)(buf + ret);
            ret += sizeof(UINT8);
            item.propType = *(UINT8 *)(buf + ret);
            ret += sizeof(UINT8);
            item.propVal = *(UINT32 *)(buf + ret);
            ret += sizeof(UINT32);
            itemVec.push_back(item);
        }
        m_enchantMap.insert(std::make_pair(instanceId, itemVec));
    }
}

void GameUser::saveEnchant() {
    char * buf = m_roleInfo.m_enchantData;
    UINT32 ret = 0;
    *(UINT8 *)(buf + ret) = (UINT8)m_enchantMap.size();
    ret += sizeof(UINT8);
    for (std::map<UINT64, std::vector<EnchantItem> >::iterator it = m_enchantMap.begin();
            it != m_enchantMap.end(); it++) {
        *(UINT64 *)(buf + ret) = it->first;
        ret += sizeof(UINT64);
        *(UINT8 *)(buf + ret) = it->second.size();
        ret += sizeof(UINT8);
        for (size_t i = 0; i < it->second.size(); i++) {
            *(UINT8 *)(buf + ret) = it->second[i].color;
            ret += sizeof(UINT8);
            *(UINT8 *)(buf + ret) = it->second[i].locked;
            ret += sizeof(UINT8);
            *(UINT8 *)(buf + ret) = it->second[i].propType;
            ret += sizeof(UINT8);
            *(UINT32 *)(buf + ret) = it->second[i].propVal;
            ret += sizeof(UINT32);
        }
    }
}

void GameUser::synEnchant(Role::stSynItemInfo * pSynInfo) {
    if (!pSynInfo) {
        return ;
    }
    std::map<UINT64, std::vector<EnchantItem> >::iterator it =
                m_enchantMap.find(pSynInfo->instanceid());
    if (it == m_enchantMap.end()) {
        return ;
    }
    std::vector<EnchantItem> & itemVec = it->second;
    Role::stEnchantInfo * pEnchant = NULL;
    for (size_t i = 0; i < itemVec.size(); i++) {
        pEnchant = pSynInfo->add_enchant();
        if (!pEnchant) {
            Athena::logger->error("[synEnchant]new error");
            continue ;
        }
        pEnchant->set_color(itemVec[i].color);
        pEnchant->set_locked(itemVec[i].locked);
        pEnchant->set_proptype(itemVec[i].propType);
        pEnchant->set_propval(itemVec[i].propVal);
        pEnchant->set_index(i + 1);
    }
}

UINT8 GameUser::getEnchantLockNum(UINT64 id) {
    UINT8 lockNum = 0;
    std::map<UINT64, std::vector<EnchantItem> >::iterator it =
                m_enchantMap.find(id);
    if (it != m_enchantMap.end()) {
        std::vector<EnchantItem> & itemVec = it->second;
        for (size_t i = 0; i < itemVec.size(); i++) {
            if (itemVec[i].locked) {
                lockNum++;
            }
        }
    }
    return lockNum;
}

void GameUser::addEnchant(UINT64 id, EnchantItem & item) {
    std::map<UINT64, std::vector<EnchantItem> >::iterator it =
                m_enchantMap.find(id);
    if (it != m_enchantMap.end()) {
        it->second.push_back(item);
    } else {
        std::vector<EnchantItem> itemVec;
        itemVec.push_back(item);
        m_enchantMap.insert(std::make_pair(id, itemVec));
    }
}

UINT8 GameUser::getEnchantPropNum(UINT64 id) {
    std::map<UINT64, std::vector<EnchantItem> >::iterator it =
                m_enchantMap.find(id);
    if (it != m_enchantMap.end()) {
        return it->second.size();
    }
    return 0;
}

bool GameUser::getEnchantById(UINT64 id) {
    std::map<UINT64, std::vector<EnchantItem> >::iterator it =
                m_enchantMap.find(id);
    if (it != m_enchantMap.end()) {
        return true;
    }
    return false;
}

bool GameUser::lockEnchantById(UINT64 id, size_t index, bool locked) {
    std::map<UINT64, std::vector<EnchantItem> >::iterator it =
                m_enchantMap.find(id);
    if (it == m_enchantMap.end()) {
        return false;
    }
    if (index > it->second.size() || !index) {
        return false;
    }
    it->second[index - 1].locked = locked;
    return true;
}

void GameUser::removeEnchantById(UINT64 id) {
    m_enchantMap.erase(id);
}

UINT8 GameUser::replaceEnchant(UINT64 id, EnchantItem & item) {
    std::vector<UINT8> indexVec;
    std::map<UINT64, std::vector<EnchantItem> >::iterator it =
                m_enchantMap.find(id);
    if (it != m_enchantMap.end()) {
        std::vector<EnchantItem> & itemVec = it->second;
        for (size_t i = 0; i < itemVec.size(); i++) {
            if (itemVec[i].locked) {
                continue;
            }
            indexVec.push_back(i);
        }
        if (indexVec.size()) {
            UINT8 index = g_randBetween(0, indexVec.size() - 1);
            itemVec[indexVec[index]] = item;
            return indexVec[index] + 1;
        }
    }
    return (UINT8)-1;
}

UINT32 GameUser::getEnchantSize() {
    return m_enchantMap.size();
}


UINT32 GameUser::getRaiseSkillValue(UINT32 skillID, UINT32 raiseType){
	if (m_raiseSkillProp.find(skillID) == m_raiseSkillProp.end())
	{
		return 0;
	}
	if (m_raiseSkillProp[skillID].find(raiseType) == m_raiseSkillProp[skillID].end())
	{
		return 0;
	}
	return m_raiseSkillProp[skillID][raiseType];
}

void GameUser::OnInterceptEnd(bool bWin, bool bTimeOut)
{
	{
		Global::stInterceptResultToGlobal cmd;
		cmd.set_charid(getID());
		cmd.set_win(bWin);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd); 
	}
	{
		MiscProto::stNotifyInterceptServerResult cmd;
		if (bWin)
		{
			cmd.set_result(MiscProto::stNotifyInterceptServerResult::eWin);
		}
		else if (bTimeOut)
		{
			cmd.set_result(MiscProto::stNotifyInterceptServerResult::eTimeout);
		}
		else
		{
			cmd.set_result(MiscProto::stNotifyInterceptServerResult::eLose);
		}
		sendProto2Me(cmd);
	}
}

void GameUser::OnleitaiResult(bool timeout, UINT32 usetime,UINT32 userHPRate)
{
	Game::stSendModuleLog  webCmd;
	webCmd.set_account(getAccount());
	webCmd.set_charid(getID());
	webCmd.set_level(getLevel());
	webCmd.set_module("荣耀宝座");
	webCmd.set_opid(m_roleInfo.m_opID);
	webCmd.set_othercharid(g_int2string(m_levelMgr.getLeitaiCharID()));
	webCmd.set_viplv(getVipLv());

	if (timeout)
	{
		webCmd.set_result(false);
		g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

		SortProto::stLeitaiResultNotify retCmd;
		retCmd.set_result(SortProto::stLeitaiResultNotify::TIMEOUT);
		sendProto2Me(retCmd);
		return ;
	}

	if (m_hp <= 0){//失败了
		webCmd.set_result(false);
		g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

		SortProto::stLeitaiResultNotify retCmd;
		retCmd.set_result(SortProto::stLeitaiResultNotify::LOSE);
		sendProto2Me(retCmd);
		return ;
	}
	else {
		m_leitaiMgr.setUserHP(m_hp / userHPRate);

		CHeroMgr::getSingleton().clearHeroByOwerID(getID());
		for (UINT32 i = 0; i < m_heroMgr.s_MAX_BATTLE_HERO_NUM; ++i)
		{
			m_leitaiMgr.setHeroHP(m_heroMgr.getBattleHeroID(i),m_heroMgr.getBattleHeroHP(i));
		}
	}
	
	webCmd.set_result(false);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);

	if (m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_ROUND) < CLeitaiMgr::s_MAX_ROUND_NUM){
		m_levelMgr.finishLeitai(0);
        if (Athena::global["checkPvP"] == "true") {
            SortProto::stLeitaiResultNotify retCmd;
            retCmd.set_result(SortProto::stLeitaiResultNotify::WIN);
            sendProto2Me(retCmd);
        }
	}
	else {
		SortProto::stLeitaiResultNotify retCmd;
		retCmd.set_result(SortProto::stLeitaiResultNotify::WIN);
		sendProto2Me(retCmd);
		return ;
	}

	m_leitaiMgr.setBuffID(ArenaConfig::getSingleton().randomLeitaiBuffID(m_leitaiMgr.getPKMode() == SortProto::eLeitaiMode_Normal,true,1));
	m_leitaiMgr.setChallengeCharID(0);
	m_countMgr.addDayRefreshValue(ECOUNT_LEITAI_ROUND,1);
	m_countMgr.addDayRefreshValue(ECOUNT_LEITAI_USETIME,usetime);

	if (m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_ROUND) >= CLeitaiMgr::s_MAX_ROUND_NUM){//轮次打完了
		if (SortProto::eLeitaiMode_Difficult == m_leitaiMgr.getPKMode()){//困难模式,才排名
			Global::stUpLeitaiUseTime  cmd;
			cmd.set_charid(getID());
			cmd.set_usetime(m_countMgr.getDayRefreshValue(ECOUNT_LEITAI_USETIME));
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		}
	}
}

void LuckyDrawMgr::load(const char * buf) {
    UINT32 ret = 0;
    m_curId = *(UINT32 *)(buf + ret);
    ret += sizeof(UINT32);
    m_diamond = *(UINT32 *)(buf + ret);
    ret += sizeof(UINT32);
    m_lastFreeDraw = *(UINT32 *)(buf + ret);
    ret += sizeof(UINT32);
    m_drawNum = *(UINT32 *)(buf + ret);
    ret += sizeof(UINT32);
    UINT8 num = *(UINT8 *)(buf + ret);
    ret += sizeof(UINT8);
    UINT8 id;
    bool drawStatus = false;
    for (UINT8 i = 0; i < num; i++) {
        id = *(UINT8 *)(buf + ret);
        ret += sizeof(UINT8);
        drawStatus = (bool)*(UINT8 *)(buf + ret);
        ret += sizeof(UINT8);
        m_drawMap.insert(std::make_pair(id, drawStatus));
    }
}

void LuckyDrawMgr::save(char * buf) {
    UINT32 ret = 0;
    *(UINT32 *)(buf + ret) = (UINT32)m_curId;
    ret += sizeof(UINT32);
    *(UINT32 *)(buf + ret) = (UINT32)m_diamond;
    ret += sizeof(UINT32);
    *(UINT32 *)(buf + ret) = (UINT32)m_lastFreeDraw;
    ret += sizeof(UINT32);
    *(UINT32 *)(buf + ret) = (UINT32)m_drawNum;
    ret += sizeof(UINT32);
    *(UINT8 *)(buf + ret) = (UINT8)m_drawMap.size();
    ret += sizeof(UINT8);
    for (std::map<UINT8, bool>::iterator it = m_drawMap.begin();
            it != m_drawMap.end(); it++) {
        *(UINT8 *)(buf + ret) = (UINT8)it->first;
        ret += sizeof(UINT8);
        *(UINT8 *)(buf + ret) = (UINT8)it->second;
        ret += sizeof(UINT8);
    }
}

void GameUser::loadLuckyDrawRecord(std::vector<LuckyDrawRecord> & recordVec) {
    recordVec = m_luckyDrawRecordVec;
}
void GameUser::addLuckyDrawRecord(LuckyDrawRecord & record) {
    if (m_luckyDrawRecordVec.size() < 50) {
        m_luckyDrawRecordVec.push_back(record);
        return ;
    }
    std::vector<LuckyDrawRecord> recordVec;
    for (size_t i = 1; i < m_luckyDrawRecordVec.size(); i++) {
        recordVec.push_back(m_luckyDrawRecordVec[i]);
    }
    recordVec.push_back(record);
    m_luckyDrawRecordVec = recordVec;
}

void GameUser::addLuckyDrawRecord(std::vector<LuckyDrawRecord> & recordVec) {
    //record vec from left to right is old to new
    if (m_luckyDrawRecordVec.size() + recordVec.size() <= 50) {
        for (size_t i = 0; i < recordVec.size(); i++) {
            m_luckyDrawRecordVec.push_back(recordVec[i]);
        }
        return;
    }
    size_t startIndex = m_luckyDrawRecordVec.size() + recordVec.size() - 50;
    if (startIndex < m_luckyDrawRecordVec.size()) {
        std::vector<LuckyDrawRecord> tmpVec;
        for (size_t i = startIndex; i < m_luckyDrawRecordVec.size(); i++) {
            tmpVec.push_back(m_luckyDrawRecordVec[i]);
        }
        for (size_t i = 0; i < recordVec.size(); i++) {
            tmpVec.push_back(recordVec[i]);
        }
        m_luckyDrawRecordVec = tmpVec;
    } else {
        m_luckyDrawRecordVec = recordVec;
    }
}

bool LuckyDrawMgr::drawReward(UINT8 id) {
    std::map<UINT8, bool>::iterator it = m_drawMap.find(id);
    if (it != m_drawMap.end()) {
        return false;
    }
    m_drawMap.insert(std::make_pair(id, true));
    return true;
}

void DailyInviteFriend::checkSameDay() {
    UINT32 now = g_gameServer->getSec();
    if (!g_isTheSameDay(time, now)) {
        time = now;
        onceReward = None_Reward;
    }
}

std::string DailyInviteFriend::toString() {
    std::stringstream ss;
    ss << time << "," << num << "," << onceReward;
    std::set<UINT32>::iterator it = drawSet.begin();
    if (it != drawSet.end()) {
        ss << "#" << *it;
        for (it++; it != drawSet.end(); it++) {
            ss << "," << *it;
        }
    }
    return ss.str();
}

void DailyInviteFriend::fromString(const char * data) {
    if (data) {
        std::vector<std::string> strVec, dataVec;
        g_stringtok(strVec, data, "#");//param 2 seems should not be null
        for (size_t i = 0; i < strVec.size(); i++) {
            dataVec.clear();
            if (i == 0) {
                g_stringtok(dataVec, strVec[0], ",");
                if (dataVec.size() == 3) {
                    time = (UINT32)atoi(dataVec[0].c_str());
                    num = (UINT32)atoi(dataVec[1].c_str());
                    onceReward = (UINT32)atoi(dataVec[2].c_str());
                }
            } else if (i == 1) {
                g_stringtok(dataVec, strVec[1], ",");
                for (size_t k = 0; k < dataVec.size(); k++) {
                    drawSet.insert((UINT32)atoi(dataVec[k].c_str()));
                }
            }
        }
    }
}

void DragonNestInfo::checkReset() {
    const UINT32 fiveHour = 18000;
    UINT32 now = g_gameServer->getSec();
    if (!g_isTheSameDay(resetDate, now - fiveHour)) {
        usedTimes = 0;
        resetTimes = 0;
        resetDate = now - fiveHour;
    }
}

std::string DragonNestInfo::toString() {
    std::stringstream ss;
    ss << (UINT32)usedTimes << "," << (UINT32)resetTimes << "," << (UINT32)resetDate;
    return ss.str();
}

void DragonNestInfo::fromString(const char * data) {
    if (data) {
        std::vector<std::string> dataVec;
        g_stringtok(dataVec, data, ",");
        if (dataVec.size() == 3) {
            usedTimes = (UINT8)atoi(dataVec[0].c_str());
            resetTimes = (UINT32)atoi(dataVec[1].c_str());
            resetDate = (UINT32)atoi(dataVec[2].c_str());
        }
    }
}


void StoneCopyMapInfo::checkReset() {                                                  
	const UINT32 fiveHour = 18000;                                                   
	UINT32 now = g_gameServer->getSec();                                             
	if (!g_isTheSameDay(resetDate, now - fiveHour)) {                                
		usedTimes = 0;                                                               
		resetTimes = 0;                                                              
		resetDate = now - fiveHour;                                                  
	}                                                                                
}                                                                                    

std::string StoneCopyMapInfo::toString() {                                             
	std::stringstream ss;                                                            
	ss << (UINT32)usedTimes << "," << (UINT32)resetTimes << "," << (UINT32)resetDate;
	return ss.str();                                                                 
}                                                                                    

void StoneCopyMapInfo::fromString(const char * data) {                                 
	if (data) {                                                                      
		std::vector<std::string> dataVec;                                            
		g_stringtok(dataVec, data, ",");                                             
		if (dataVec.size() == 3) {                                                   
			usedTimes = (UINT8)atoi(dataVec[0].c_str());                             
			resetTimes = (UINT32)atoi(dataVec[1].c_str());                           
			resetDate = (UINT32)atoi(dataVec[2].c_str());                            
		}                                                                            
	}                                                                                
}                                                                                    

void GameUser::forceDivorce() {
	setCoupleId(0);
	setCoupleName(std::string(""));
}

void GameUser::resetMarryRecord() {
	Global::stRetDelUserOnlineMarryInfo forwardCmd;
	forwardCmd.set_id(getID());
	GlobalSvrClientMgr::getSingleton().broadcastProto2All(forwardCmd);
}

void GameUser::sendLimitPetActivity() {
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
    this->sendProto2Me(retCmd);
}

std::string LateWalfareInfo::toString() {
    std::stringstream ss;
    ss << id << "," << expireTime;
    if (rewardSet.size()) {
        std::set<UINT32>::iterator it = rewardSet.begin();
        ss << ";" << *it;
        for (++it; it != rewardSet.end(); it++) {
            ss << "," << *it;
        }
    }
	Athena::logger->trace("限时奖励字符串是%s", ss.str().c_str());
    return ss.str();
}

void LateWalfareInfo::fromString(const char * data) {
    if (data) {
        std::vector<std::string> strVec;
        g_stringtok(strVec, data, ";");
        for (size_t i = 0; i < strVec.size(); i++) {
            std::vector<std::string> dataVec;
            if (i == 0) {
                g_stringtok(dataVec, strVec[0], ",");
                id = (UINT32)atoi(dataVec[0].c_str());
                expireTime = (UINT32)atoi(dataVec[1].c_str());
            } else if (i == 1) {
                g_stringtok(dataVec, strVec[1], ",");
                for (size_t j = 0; j < dataVec.size(); j++) {
                    rewardSet.insert((UINT32)atoi(dataVec[j].c_str()));
                }
            }
        }
    }
}

void GameUser::checkLateWalfare() {
    if (!CLateWelfareCfg::getSingleton().isOpen()) {
        return ;
    }
    struct tm openSrvTm, createTm;
    RealTime::getLocalTime(openSrvTm, g_gameServer->getOpenServerTime());
    RealTime::getLocalTime(createTm, m_firstLogin);
	UINT32 createRoleDay =
        g_dayDiff(createTm.tm_year + 1900, createTm.tm_mon + 1, createTm.tm_mday, openSrvTm.tm_year + 1900,openSrvTm.tm_mon + 1,openSrvTm.tm_mday) + 1;
    //g_dayDiff是返回一个差值,或正或负

	Athena::logger->trace("创建角色时间距离开服时间 %d", createRoleDay);
    CLateWelfareCfg::WalfareItem item;
    if (!CLateWelfareCfg::getSingleton().getWalfare(createRoleDay, item)) {
        MiscProto::stSendLateWalfareActivity retCmd;
        retCmd.set_id(0);
        retCmd.set_expiretime(1);
        this->sendProto2Me(retCmd);
        Athena::logger->trace("开服奖励找不到数据,发送1");
        return ;
    }

    if (!m_lateWalfareMgr.getExpireTime()) {
        m_lateWalfareMgr.setId(item.id);
        m_lateWalfareMgr.setExpireTime(g_gameServer->getSec() + item.activityTime);
    }

    MiscProto::stSendLateWalfareActivity retCmd;
    retCmd.set_id(item.id);
    retCmd.set_expiretime(m_lateWalfareMgr.getExpireTime());
    std::set<UINT32> & rewardSet = m_lateWalfareMgr.rewardSet;
    for (std::set<UINT32>::iterator it = rewardSet.begin();
            it != rewardSet.end(); it++) {
        retCmd.add_rewardid(*it);
    }
    this->sendProto2Me(retCmd);

}

void GameUser::drawLateWalfare(UINT32 rewardId) {
    MiscProto::stRetDrawLateWalfare retCmd;
    retCmd.set_rewardid(rewardId);

    if (!CLateWelfareCfg::getSingleton().isOpen()) {
        retCmd.set_result(MiscProto::stRetDrawLateWalfare::eNot_Open);
        this->sendProto2Me(retCmd);
        return ;
    }
    if (m_lateWalfareMgr.rewardDrawed(rewardId)) {
        retCmd.set_result(MiscProto::stRetDrawLateWalfare::eAlread_Draw);
        this->sendProto2Me(retCmd);
        return ;
    }
    if (m_lateWalfareMgr.getExpireTime() <= g_gameServer->getSec()) {
        retCmd.set_result(MiscProto::stRetDrawLateWalfare::eActivity_Expire);
        this->sendProto2Me(retCmd);
        return ;
    }
    CLateWelfareCfg::RewardItem item;
    if (!CLateWelfareCfg::getSingleton().getRewardItem(m_lateWalfareMgr.getId(), rewardId, item)) {
        retCmd.set_result(MiscProto::stRetDrawLateWalfare::eNo_Such_Id);
        this->sendProto2Me(retCmd);
        return ;
    }
    if (!checkDiamond(item.costDiamond)) {
		retCmd.set_result(MiscProto::stRetDrawLateWalfare::eDiamond_Not_Enough);
		this->sendProto2Me(retCmd);
		return ;
	}
    removeDiamond(item.costDiamond, "领取限时奖励");
    m_lateWalfareMgr.drawReward(rewardId);

    std::vector<CLateWelfareCfg::RewardInfo> & rewardVec = item.rewardVec;
    Role::stObtainItemTip itemTipCmd;
    Role::stSynTipItem * pSyn = NULL;
    for (size_t i = 0; i < rewardVec.size(); i++) {
        pSyn = itemTipCmd.add_getitems();
        pSyn->set_itemid(rewardVec[i].itemId);
        pSyn->set_num(rewardVec[i].num);
        giveAssert(rewardVec[i].itemId, rewardVec[i].num, 0, "领取限时奖励");
    }
	this->sendProto2Me(itemTipCmd);

    retCmd.set_result(MiscProto::stRetDrawLateWalfare::eSuccess);
    this->sendProto2Me(retCmd);
}

std::string ResBackInfo::toString() {
    std::stringstream ss;
    if (m_resBackMap.empty()) {
        return ss.str();
    }
    std::map<UINT32, std::vector<ResBackItem> > resBackMap = m_resBackMap;
    std::map<UINT32, std::vector<ResBackItem> >::iterator it = resBackMap.begin();
    std::vector<ResBackItem> & itemVec = it->second;
    if (itemVec.size()) {
        ss << it->first << "," << itemVec[0].dayTime << "," << (UINT32)itemVec[0].value << "," << (UINT32)itemVec[0].vipLvl;
        itemVec.erase(itemVec.begin());
    }
    for (; it != resBackMap.end(); it++) {
        itemVec = it->second;
        for (size_t i = 0; i < itemVec.size(); i++) {
            ss << ";" << it->first << "," << itemVec[i].dayTime << "," << (UINT32)itemVec[i].value << "," << (UINT32)itemVec[i].vipLvl;
        }
    }

    // Athena::logger->trace("ResBackInfo tostring size is %d, data is %s", ss.str().length(), ss.str().c_str());
    return ss.str();
}

void ResBackInfo::fromString(const char * data) {
    if (data) {
		// Athena::logger->trace("ResBackInfo fromString  data is %s", data);
        UINT32 key = 0;
        std::vector<std::string> strVec;
        g_stringtok(strVec, data, ";");
        for (size_t i = 0; i < strVec.size(); i++) {
            std::vector<std::string> dataVec;
            g_stringtok(dataVec, strVec[i], ",");
            if (dataVec.size() == 4) {
                ResBackItem item;
                key = (UINT32)atoi(dataVec[0].c_str());
                item.dayTime = dataVec[1];
                item.value = (UINT16)atoi(dataVec[2].c_str());
                item.vipLvl = (UINT16)atoi(dataVec[3].c_str());
                std::map<UINT32, std::vector<ResBackItem> >::iterator it = m_resBackMap.find(key);
                if (it != m_resBackMap.end()) {
                    if (it->second.size() >= 4) {
                        it->second.erase(it->second.begin());
                    }
                    it->second.push_back(item);
                } else {
                    std::vector<ResBackItem> itemVec;
                    itemVec.push_back(item);
                    m_resBackMap.insert(std::make_pair(key, itemVec));
                }
            }
        }
    }
}

static std::string getStringDate(UINT32 time) {
	struct tm curTm;
    std::stringstream ss;

	RealTime::getLocalTime(curTm, time);
    ss << (curTm.tm_year + 1900);
    if ((curTm.tm_mon + 1) < 10) {
        ss << '0';
    }
    ss << (curTm.tm_mon + 1);
    if (curTm.tm_mday < 10) {
        ss << '0';
    }
    ss << curTm.tm_mday;

    return ss.str();
}

void ResBackInfo::addBackRes(UINT32 type, UINT16 num, UINT16 vipLvl) {
    ////g_utc2StringDate会导致stringstream 后续数据无法写入
    std::string dateStr = getStringDate(g_gameServer->getSec());

    ResBackItem item;
    item.dayTime = dateStr;
    item.value = num;
    item.vipLvl = vipLvl;
    std::map<UINT32, std::vector<ResBackItem> >::iterator it = m_resBackMap.find(type);
    if (it != m_resBackMap.end()) {
        bool bFind = false;
        std::vector<ResBackItem> & itemVec = it->second;
        for (size_t i = 0; i < itemVec.size(); i++) {
            if (itemVec[i].dayTime == dateStr) {
                bFind = true;
                itemVec[i].value += num;
                itemVec[i].vipLvl = vipLvl;
                UINT32 maxNum = CResBackCfg::getSingleton().getMaxNum(type, vipLvl);
                if (itemVec[i].value >= maxNum) {
                    itemVec[i].value = maxNum;
                }
            }
        }
        if (!bFind) {
            if (itemVec.size() > 3) {
                itemVec.erase(itemVec.begin());
            }
            itemVec.push_back(item);
        }
    } else {
        std::vector<ResBackItem> itemVec;
        itemVec.push_back(item);
        m_resBackMap.insert(std::make_pair(type, itemVec));
    }
}

void ResBackInfo::checkBackRes(GameUser * pUser) {
	struct tm curTm;
	RealTime::getLocalTime(curTm, g_gameServer->getSec());

    UINT32 num = 0;
    UINT32 year, month, day;
    MiscProto::stSendAllResGetBack retCmd;
    for (std::map<UINT32, std::vector<ResBackItem> >::iterator it = m_resBackMap.begin();
            it != m_resBackMap.end(); it++) {
        num = 0;
        std::vector<ResBackItem> & itemVec = it->second;
        for (size_t i = 0; i < itemVec.size(); i++) {
            year = (UINT32)atoi(itemVec[i].dayTime.substr(0,4).c_str());
            month = (UINT32)atoi(itemVec[i].dayTime.substr(4,2).c_str());
            day = (UINT32)atoi(itemVec[i].dayTime.substr(6,2).c_str());
            INT32 diffDay = g_dayDiff(curTm.tm_year + 1900, curTm.tm_mon + 1, curTm.tm_mday, year, month, day);
            if (diffDay < 1 || diffDay > 3) {
                continue ;
            }
            UINT32 maxNum = CResBackCfg::getSingleton().getMaxNum(it->first, itemVec[i].vipLvl);
            if (itemVec[i].value >= maxNum) {
                continue ;
            }
            num += maxNum - itemVec[i].value;
        }
        if (!num) {
            continue ;
        }
        MiscProto::stResGetBackItem * pItem = retCmd.add_item();
        if (pItem) {
            pItem->set_type(it->first);
            pItem->set_num(num);
        }
    }
    pUser->sendProto2Me(retCmd);
}

void GameUser::setBackResData() {
    UINT32 now = g_gameServer->getSec();
	struct tm lastTm, curTm;
    strptime(m_roleInfo.m_lastLoginTime,"%Y-%m-%d %H:%M:%S",&lastTm);
	RealTime::getLocalTime(curTm, now);
    
    int i = 0;
    std::string str("0000");
    if (strncmp(m_roleInfo.m_lastLoginTime, str.c_str(), str.length())) {
        INT32 diffDay = g_dayDiff(curTm.tm_year + 1900, curTm.tm_mon + 1, curTm.tm_mday, lastTm.tm_year + 1900, lastTm.tm_mon + 1, lastTm.tm_mday);
        Athena::logger->trace("setBackResData diff day is %d, last login time is %s", diffDay, m_roleInfo.m_lastLoginTime);
        i = diffDay > 3 ? 3 : diffDay;
    }
    for (; i >= 0; i--) {
        setDayBackRes(now - (i * 86400));
        Athena::logger->trace("setBackResData i is %d", i);
    }
}

void GameUser::setDayBackRes(UINT32 lastTime) {
    std::string dateStr = getStringDate(lastTime);

    std::vector<UINT32> & typeVec = CResBackCfg::getSingleton().getAllType();

    ResBackItem item;
    item.dayTime = dateStr;
    item.value = 0;
    item.vipLvl = getVipLv();
    std::map<UINT32, std::vector<ResBackItem> > & resBackMap = m_resBackMgr.m_resBackMap;
    std::map<UINT32, std::vector<ResBackItem> >::iterator it;
    for (size_t i = 0; i < typeVec.size(); i++) {
        it = resBackMap.find(typeVec[i]);
        if (it == resBackMap.end()) {
            std::vector<ResBackItem> itemVec;
            itemVec.push_back(item);
            resBackMap.insert(std::make_pair(typeVec[i], itemVec));
        } else {
            bool bFind = false;
            std::vector<ResBackItem> & itemVec = it->second;
            for (size_t k = 0; k < itemVec.size(); k++) {
                if (itemVec[k].dayTime == dateStr) {
                    bFind = true;
                    break;
                }
            }
            if (!bFind) {
                if (itemVec.size() > 3) {
                    itemVec.erase(itemVec.begin());
                    Athena::logger->trace("erase first item, itemVec size is %d", itemVec.size());
                }
                itemVec.push_back(item);
                Athena::logger->trace("push back item, itemVec size is %d", itemVec.size());
            }
        }
    }
}

void GameUser::synBackResVip(UINT16 oldVip, UINT16 newVip) {
    Athena::logger->trace("synBackResVip oldVip is %d, newVip is %d", oldVip, newVip);
    std::string dateStr = getStringDate(g_gameServer->getSec());

    std::map<UINT32, std::vector<ResBackItem> > & resBackMap = m_resBackMgr.m_resBackMap;
    std::map<UINT32, std::vector<ResBackItem> >::iterator it;
    for (std::map<UINT32, std::vector<ResBackItem> >::iterator it = resBackMap.begin();
            it != resBackMap.end(); it++) {
        std::vector<ResBackItem> & itemVec = it->second;
        for (size_t i = 0; i < itemVec.size(); i++) {
            if (itemVec[i].dayTime == dateStr) {
                itemVec[i].vipLvl = newVip;
                break;
            }
        }
    }
}

void GameUser::drawBackRes(UINT32 type, UINT32 drawNum) {
    MiscProto::stRetDrawResGetBack retCmd;
    retCmd.set_type(type);

    std::map<UINT32, std::vector<ResBackItem> >::iterator it = m_resBackMgr.m_resBackMap.find(type);
    if (it == m_resBackMgr.m_resBackMap.end()) {
        retCmd.set_result(MiscProto::stRetDrawResGetBack::eNo_Such_Type);
        this->sendProto2Me(retCmd);
        return ;
    }

    UINT32 totalNum = 0;
    UINT32 year, month, day;
	struct tm curTm;
	RealTime::getLocalTime(curTm, g_gameServer->getSec());
    std::vector<ResBackItem> & itemVec = it->second;

    std::vector<ResBackItem> tmpVec = itemVec;
    for (size_t i = 0; i < itemVec.size(); i++) {
        year = (UINT32)atoi(itemVec[i].dayTime.substr(0,4).c_str());
        month = (UINT32)atoi(itemVec[i].dayTime.substr(4,2).c_str());
        day = (UINT32)atoi(itemVec[i].dayTime.substr(6,2).c_str());
        INT32 diffDay = g_dayDiff(curTm.tm_year + 1900, curTm.tm_mon + 1, curTm.tm_mday, year, month, day);
        if (diffDay < 1 || diffDay > 3) {
            continue ;
        }
        UINT32 maxNum = CResBackCfg::getSingleton().getMaxNum(it->first, itemVec[i].vipLvl);
        if (itemVec[i].value >= maxNum) {
            continue ;
        }
        totalNum += maxNum - itemVec[i].value;
    }
    if (!totalNum) {
        retCmd.set_num(0);
        retCmd.set_result(MiscProto::stRetDrawResGetBack::eAlread_Draw);
        this->sendProto2Me(retCmd);
        return ;
    }
    if (totalNum < drawNum) {
        retCmd.set_num(totalNum);
        retCmd.set_result(MiscProto::stRetDrawResGetBack::eTimes_Not_Enough);
        this->sendProto2Me(retCmd);
        return ;
    }

    UINT32 tmpDrawNum = drawNum;
    for (size_t i = 0; i < tmpVec.size(); i++) {
        year = (UINT32)atoi(tmpVec[i].dayTime.substr(0,4).c_str());
        month = (UINT32)atoi(tmpVec[i].dayTime.substr(4,2).c_str());
        day = (UINT32)atoi(tmpVec[i].dayTime.substr(6,2).c_str());
        INT32 diffDay = g_dayDiff(curTm.tm_year + 1900, curTm.tm_mon + 1, curTm.tm_mday, year, month, day);
        if (diffDay < 1 || diffDay > 3) {
            continue ;
        }
        UINT32 maxNum = CResBackCfg::getSingleton().getMaxNum(it->first, tmpVec[i].vipLvl);
        if (tmpVec[i].value >= maxNum) {
            continue ;
        }
        if (maxNum - tmpVec[i].value < tmpDrawNum) {
            tmpDrawNum -= maxNum - tmpVec[i].value;
            tmpVec[i].value = maxNum;
        } else {
            tmpVec[i].value += tmpDrawNum;
            break;
        }
    }
    retCmd.set_num(totalNum - drawNum);

    CResBackCfg::RewardItem item;
    if (!CResBackCfg::getSingleton().getItem(type, item)) {
        retCmd.set_result(MiscProto::stRetDrawResGetBack::eNo_Such_Type);
        this->sendProto2Me(retCmd);
        return ;
    }
    if (!checkDiamond(item.costDiamond * drawNum)) {
		retCmd.set_result(MiscProto::stRetDrawResGetBack::eDiamond_Not_Enough);
		this->sendProto2Me(retCmd);
		return ;
	}
    removeDiamond(item.costDiamond * drawNum, "领取找回奖励");
    //更改数据
    it->second = tmpVec;

    std::vector<CResBackCfg::RewardInfo> & rewardVec = item.rewardVec;
    Role::stObtainItemTip itemTipCmd;
    Role::stSynTipItem * pSyn = NULL;
    for (size_t i = 0; i < rewardVec.size(); i++) {
        pSyn = itemTipCmd.add_getitems();
        pSyn->set_itemid(rewardVec[i].itemId);
        pSyn->set_num(rewardVec[i].num * drawNum);
        giveAssert(rewardVec[i].itemId, rewardVec[i].num * drawNum, 0, "领取找回奖励");
    }
	this->sendProto2Me(itemTipCmd);

    retCmd.set_result(MiscProto::stRetDrawResGetBack::eSuccess);
    this->sendProto2Me(retCmd);
}

void GameUser::drawAllBackRes() {
	struct tm curTm;
	RealTime::getLocalTime(curTm, g_gameServer->getSec());

    UINT32 num = 0, costDiamond = 0;
    UINT32 year, month, day;
    std::map<UINT32, UINT32> typeMap, itemMap;
    MiscProto::stRetDrawAllResGetBack retCmd;
    for (std::map<UINT32, std::vector<ResBackItem> >::iterator it = m_resBackMgr.m_resBackMap.begin();
            it != m_resBackMgr.m_resBackMap.end(); it++) {
        num = 0;
        std::vector<ResBackItem> & itemVec = it->second;
        for (size_t i = 0; i < itemVec.size(); i++) {
            year = (UINT32)atoi(itemVec[i].dayTime.substr(0,4).c_str());
            month = (UINT32)atoi(itemVec[i].dayTime.substr(4,2).c_str());
            day = (UINT32)atoi(itemVec[i].dayTime.substr(6,2).c_str());
            INT32 diffDay = g_dayDiff(curTm.tm_year + 1900, curTm.tm_mon + 1, curTm.tm_mday, year, month, day);
            if (diffDay < 1 || diffDay > 3) {
                continue ;
            }
            UINT32 maxNum = CResBackCfg::getSingleton().getMaxNum(it->first, itemVec[i].vipLvl);
            if (itemVec[i].value >= maxNum) {
                continue ;
            }
            num += maxNum - itemVec[i].value;
        }
        if (!num) {
            continue ;
        }
        typeMap.insert(std::make_pair(it->first, num));
        CResBackCfg::RewardItem item;
        if (CResBackCfg::getSingleton().getItem(it->first, item)) {
            costDiamond += item.costDiamond * num;
            std::vector<CResBackCfg::RewardInfo> & rewardVec = item.rewardVec;
            for (size_t i = 0; i < rewardVec.size(); i++) {
                std::map<UINT32, UINT32>::iterator it = itemMap.find(rewardVec[i].itemId);
                if (it != itemMap.end()) {
                    it->second += num;
                } else {
                    itemMap.insert(std::make_pair(rewardVec[i].itemId, num));
                }
            }
        }
    }
    if (!costDiamond) {
		retCmd.set_result(MiscProto::stRetDrawAllResGetBack::eNo_Res_Back);
		this->sendProto2Me(retCmd);
		return ;
    }
    if (!checkDiamond(costDiamond)) {
		retCmd.set_result(MiscProto::stRetDrawAllResGetBack::eDiamond_Not_Enough);
		this->sendProto2Me(retCmd);
		return ;
	}
    removeDiamond(costDiamond, "一键领取找回奖励");

    Role::stObtainItemTip itemTipCmd;
    Role::stSynTipItem * pSyn = NULL;
    for (std::map<UINT32, UINT32>::iterator it = itemMap.begin();
            it != itemMap.end(); it++) {
        pSyn = itemTipCmd.add_getitems();
        pSyn->set_itemid(it->first);
        pSyn->set_num(it->second);
        giveAssert(it->first, it->second, 0, "领取找回奖励");
    }
	this->sendProto2Me(itemTipCmd);

    retCmd.set_result(MiscProto::stRetDrawAllResGetBack::eSuccess);
    this->sendProto2Me(retCmd);

    std::string dateStr = getStringDate(g_gameServer->getSec());
    std::map<UINT32, std::vector<ResBackItem> > resBackMap;
    for (std::map<UINT32, std::vector<ResBackItem> >::iterator it = m_resBackMgr.m_resBackMap.begin();
            it != m_resBackMgr.m_resBackMap.end(); it++) {
        std::vector<ResBackItem> & itemVec = it->second;
        for (size_t i = 0; i < itemVec.size(); i++) {
            if (itemVec[i].dayTime == dateStr) {
                std::vector<ResBackItem> tmpVec;
                tmpVec.push_back(itemVec[i]);
                resBackMap.insert(std::make_pair(it->first, tmpVec));
            }
        }
    }
    m_resBackMgr.m_resBackMap = resBackMap;

}

void GameUser::sendGuaji()
{
	Role::stGuaJiTimeNtf ntf;
	ntf.set_guajistart(m_roleInfo.m_guajiStart);
	ntf.set_guajiend(m_roleInfo.m_guajiEnd);
	if (m_roleInfo.m_guajiTime > 0)
	{
		ntf.set_todaycount(1);
	}else{
		ntf.set_todaycount(0);
	}
	
	this->sendProto2Me(ntf);
}


void GameUser::CheckGuajiStartClear(bool send)
{
	if (m_roleInfo.m_guajiTime==0)
	{
		return;
	}
	
	if (!g_isTheSameDay(m_roleInfo.m_guajiTime-3600*5,g_gameServer->getSec()-3600*5)){
		m_roleInfo.m_guajiTime = 0;

		if (send)
		{
			sendGuaji();
		}
		
	}
}