#include "TypesDef.h"
#include "CommonMicro.h"
#include "GameServer.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "TblDataMgr.h"
#include "GameTimerCB.h"
#include "MapMgr.h"
#include "Chat.h"
#include "PacketProcessor.h"
#include "GlobalPacketProcessor.h"
#include "Skill.h"
#include "GlobalSvrClient.h"
#include "Skill.h"
#include "ShopMgr.h"
#include "ItemBox.h"
#include "RewardSys.h"
#include "MiscCfgMgr.h"
#include "DrawMgr.h"
#include "LevelMgr.h"
#include "GuildConfig.h"
#include "DropMgr.h"
#include "Task.h"
#include "RobotMgr.h"
#include "AIConfigMgr.h"
#include "RMBShop.h"
#include "ArenaConfig.h"
#include "LimitTimePetCfg.h"
#include "SevenDayActivity.h"
#include "ItemBoxCfg.h"
#include "CaptureCfg.h"
#include "ExpeditionConfig.h"
#ifdef _USE_BEHAVIAC_AI
#include "BehaviacAI.h"
#endif
#include "RechargeActivityCfg.h"
#include "QuestionCfg.h"
#include "ShopActivity.h"
#include "LuckShopCfg.h"
#include "GuardCfg.h"
#include "LimitFashionCfg.h"
#include "OpenFunction.h"
#include <getopt.h>
#include "MysqlDatabase.h"
#include "SqlDelayThreadPool.h"
#include "GloryfashionCfg.h"
#include "FashionShopOpenCfg.h"
#include "EmeraldDreamCfg.h"
#include "DevilCallCfg.h"
#include "OperateActivityCfg.h"
#include "EnchantCfg.h"
#include "LuckyDrawCfg.h"
#include "InviteCfg.h"
#include "HeroLeagueCfg.h"
#include "RuneCfg.h"
#include "GemCfg.h"
#include "MarryCfg.h"
#include "DragonNest.h"
#include "StoneCopyMapCfg.h"
#include "SmeltItemCfg.h"
#include "DailyRecharge.h"
#include "LateWelfareCfg.h"
#include "ResBackCfg.h"
#include "M78.h"
#include "MountSynthesisCfg.h"
#include "ProFilePhotoCfg.h"
#include "SysLanguages.h"
#include "LevelWflfareCfg.h"
#include "HeroTrainCfg.h"
#include "NftConfig.h"
#include "RecastEquipCfg.h"
#include "HttpClient.h"

class GameServerAdapter : public IPacketHandler
{
public:
    bool handlePacket(IOComponent * pConn, Packet *packet)
	{
		if (ControlPacket::isControlPacket(packet)){
			return false;
		}   
		((ClientPacket *)packet)->m_pComponent = (TCPComponent *)pConn; 
		PacketProcessor::getSingleton().addPacket((ClientPacket *)packet);
		return true;
    }
};

bool GlobalPacketHandler::handlePacket(IOComponent * pConn, Packet *packet)
{
	if (!packet){
		return false;
	}

	if (ControlPacket::isControlPacket(packet)){
		return false;
	}
	GlobalPacketProcessor::getSingleton().addPacket((ClientPacket *)packet);	
	return true;	 	
}

GameServer::GameServer() : m_netConnMgr(&m_connMgr)
{
	m_isLoadSvrList = false;
	m_openServerDay = 0;
	m_bCreateTestRole = false;
}

GameServer::~GameServer()
{
}

// ClientPacketFactory * g_factory = NULL;
// ClientBinPacketSpliter  * g_spliter = NULL;
// GlobalPacketHandler    * g_globalPacketHandler = NULL;
// GameServerAdapter     * g_serverAdapter = NULL;

bool GameServer::start()
{
	if (!ServerBase::start()){
		return false;
	}
	m_pubServerKey		=	Athena::global["pubServerKey"].c_str();
	m_pubServerIndex	=	atoi(Athena::global["pubServerIndex"].c_str());
	m_areaCode			=	atoi(Athena::global["gameID"].c_str());

    if (!m_netConnMgr.init()){
		Athena::logger->error("服务器初始化网络失败,启动服务器不成功");
		return false;
	}
	
	if (Athena::global["statProto"] == "true"){
		m_traceProto = true;
	}
	else {
		m_traceProto = false;
	}
	
	if (Athena::global["CreateTestAccount"] == "true"){
		m_bCreateTestRole = true;	
	}
	else {
		m_bCreateTestRole = false;
	}

	if (!startListen())
	{
		return false;
	}

	m_svrID = m_svrInfo.m_svrID;

	if(loadAllClient() == false) {
		return false;
	}

#ifdef _USE_BEHAVIAC_AI
	if (!CBehaviacAIMgr::getSingleton().init()){
		Athena::logger->error("加载behaviac AI失败!");
		return false;
	}
#endif	
	
	if (g_loadAllTbl()){
		Athena::logger->trace("[加载表格]加载成功!");
	}
	else {
		Athena::logger->error("[加载表格]失败!");
		return false;
	}
	
	if (!SevenDayActivityCfg::getSingleton().loadCfg()){
		Athena::logger->error("加载七天活动配置失败!");
		return false;
	}

	if (!CMapMgr::getSingleton().init(Athena::global["mapDir"].c_str())){
		Athena::logger->error("初始化地图失败!");
		return false;
	}
	
	if (!CSkillRangeCfg::getSingleton().loadCfg()) {
		Athena::logger->error("初始化技能范围失败!");
		return false;
	}
	
	if (!CShopMgr::loadCfg()){
		Athena::logger->error("加载商店配置失败!");
		return false;
	}
	
	if (!ItemBoxCfg::getSingleton().loadCfg()){
		Athena::logger->error("加载箱子配置文件失败!");
		return false;
	}
	
	if (!CAllRewardMgr::getSingleton().loadCfg()){
		Athena::logger->error("加载奖励相关配置失败!");
		return false;	
	}
	
	if (!CMiscCfgMgr::getSingleton().loadCfg()){
		Athena::logger->error("加载杂项配置失败!");
		return false;
	}

	if (!CHeroTrainCfg::getSingleton().loadCfg()){
		Athena::logger->error("加载随从调教配置失败!");
		return false;
	}
	
	if (!CDrawMgr::loadCfg()){
		Athena::logger->error("加载抽奖失败!");
		return false;
	}
	
	if (!CLevelMgr::loadCfg()){
		Athena::logger->error("加载关卡配置失败");
		return false;
	}
	
	if (!CGuildConfig::getSingleton().loadCfg()){
		Athena::logger->error("加载公会配置失败!");
		return false;
	}
	
	if (!CDropMgr::getSingleton().loadCfg()){
		Athena::logger->error("加载掉落配置失败!");
		return false;
	}
	
	if (!CTaskTemplateMgr::getSingleton().loadTaskCfg()){
		Athena::logger->error("加载任务配置错误");
		return false;
	}
	
	if (!CRobotMgr::getSingleton().loadCfg()){
		Athena::logger->error("加载机器人错误");
		return false ;
	}
	
	if (!CAIConfigMgr::getSingleton().loadCfg()){
		Athena::logger->error("AI配置文件");
		return false;
	}
	
	if (!CRMBShop::loadCfg()){
		Athena::logger->error("加载人民币商店错误!");
		return false;
	}

	if (!CTitleMgr::afterLoadTitleTbl()){
		Athena::logger->error("处理称号表格失败!");
		return false;
	}
	
	if (!ArenaConfig::getSingleton().loadCfg()){
		Athena::logger->error("竞技场奖励加载配置失败!");
		return false;
	}

	if (!LimitTimePetCfg::getSingleton().loadCfg()){
		Athena::logger->error("加载限时武神奖励失败!");
		return false;
	}
	
	if (!CCaptureCfg::getSingleton().loadCfg()){
		Athena::logger->error("加载夺宝配置");
		return false;
	}
	
	if (!RechargeActivityCfg::getSingleton().loadCfg()){
		Athena::logger->error("加载充值活动奖励");
		return false;
	}
	
	if (!CQuestionCfg::getSingleton().loadCfg()){
		Athena::logger->error("问卷调查配置加载错误");
		return false;
	}
	
	if (!CShopActivityMgr::getSingleton().loadCfg()){
		Athena::logger->error("商店活动加载失败!");
		return false;
	}
	
	if (!LuckShopCfg::getSingleton().loadCfg()){
		Athena::logger->error("幸运商店配置!");
		return false;
	}
	
	if (!GuardCfg::getSingleton().loadCfg()){
		Athena::logger->trace("护送配置!");
		return false;
	}
	
	if (!CLimitFashionCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载限时时装错误!");
		return false;
	}
	
	if (!COpenFunction::loadCfg()){
		Athena::logger->trace("加载功能开放等级错误!");
		return false;
	}

	//if(Athena::global["CountryVersion"] == "Korea"){
		if(!CGloryfashionCfg::getSingleton().loadCfg()){
			Athena::logger->trace("加载荣耀时装商店失败!");
			return false;
		}
	//}

    if(!CFashionShopOpenCfg::getSingleton().loadCfg()){
        Athena::logger->trace("加载时装商店失败!");
        return false;
    }
    if (!CLuckyDrawCfg::getSingleton().loadCfg()){
        Athena::logger->trace("加载幸运抽奖出错!");
        return false;
    }

    if (!CDevilCallCfg::getSingleton().loadCfg()){
        Athena::logger->trace("加载魔王召唤出错!");
        return false;
    }
	if (!CExpeditionCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载远征副本配置出错!");
		return false;
	}

	CSysLanguages::getSingleton().load();

	if(!CEmeraldDreamCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载翡翠梦境配置出错!");
		return false;
	}

    if (!COperateActivityCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载新加活动出错!");
        return false;
    }

    if (!CEnchantCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载时装附魔出错!");
        return false;
    }
    if (!CInviteCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载分享码出错!");
        return false;
    }

	if (!CHeroLeagueCfg::getSingleton().loadCfg()) {
	 	Athena::logger->trace("加载英雄联赛出错!");
		return false;
	}
    
	if (!CRuneCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载符文出错!");
        return false;
    }

	if (!LoadGemCfg()) {
        Athena::logger->trace("加载Gemsynthesis.xml出错!");
        return false;
	}

	if (!CDragonNestCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载巨龙巢穴出错!");
        return false;
	}

	if (!CMarryCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载结婚场景出错!");
        return false;
	}

	if(!CStoneCopyMapCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载宝石副本出错!");
		return false;
	}

	if(Athena::global["isLYServer"] == "true"){
		if(!CSmeltItemCfg::getSingleton().loadCfg()) {
			Athena::logger->trace("加载黑市熔炼出错!");
			return false;
		}
	}

	if(!CDailyRecharge::getSingleton().loadCfg()){
		Athena::logger->trace("加载每日首冲奖励出错!");
		return false;
	}

	if(!CLateWelfareCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载限时福利出错!");
		return false;
	}

	if(!CResBackCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载资源找回出错!");
		return false;
	}

	if(!CDrawInfoCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载打怪兽奖励出错!");
		return false;
	}

	if(!CM78Cfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载打怪兽配置出错!");
		return false;
	}

	if(!CMountSynthesisCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载坐骑装备配置出错!");
		return false;
	}

	if(!CProFilePhotoCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载头像配置出错!");
		return false;
	}

	if(!CLevelWflfareCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载冲级好礼配置出错!");
		return false;
	}

	if(!RecastEquipCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载熔铸配置出错!");
		return false;
	}

	if (Athena::global["nftServer"] == "true")
	{
		NftConfig::getSingleton().loadCfg();
	} 

	//new KBEngine::Navigation();
	//KBEngine::Navigation::getSingleton().loadNavigation("xinshoucun",false);

	ChatMgr::getSingleton().initGMAccountInfo();
	CHttpClient::getSingleton().init();
	
	if (!extraLoad())
	{
		return false;
	}
	createTimer();
#ifdef _Memmory_Leak_
	initMemoryLeakCheckSys();
	Athena::logger->trace("[服务器启动]初始化内存检测系统");
#endif
	printf("游戏服务器启动成功, 名字最大长度%d\n", MAX_NAMESIZE);

#ifdef _YUNDING_TONGJI
	printf("已加入云顶充值统计\n");
#endif
	return true;
}

bool GameServer::startListen()
{
	return true;
}
void GameServer::createTimer()
{

}
bool GameServer::extraLoad()
{
	return true;
}

void GameServer::stop()
{
	m_netConnMgr.stop();
	m_timerMgr.stop();
	g_unloadAllTbl();
	memoryLeaksDump();
}

bool GameServer::loadAllClient()
{
	return true;
}

GameServer *g_gameServer;

