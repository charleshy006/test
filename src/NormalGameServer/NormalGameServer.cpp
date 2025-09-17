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
#include "PubSvrClient.h"
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
#include "NormalGameServer.h"
#include "Pub2NormalPacketProcessor.h"

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


bool Pub2NormalPacketHandler::handlePacket(IOComponent * pConn, Packet *packet)
{
	if (!packet){
		return false;
	}

	if (ControlPacket::isControlPacket(packet)){
		return false;
	}
	Pub2NormalPacketProcessor::getSingleton().addPacket((ClientPacket *)packet);	
	return true;	 	
}

NormalGameServer::NormalGameServer()
{
}

NormalGameServer::~NormalGameServer()
{
}

ClientPacketFactory * g_factory = NULL;
ClientBinPacketSpliter  * g_spliter = NULL;
GlobalPacketHandler    * g_globalPacketHandler = NULL;
Pub2NormalPacketHandler * g_pub2NormalPacketHandler = NULL;
GameServerAdapter     * g_serverAdapter = NULL;

// bool NormalGameServer::start()
// {
// 	if (!GameServer::start()){
// 		return false;
// 	}

//     if (!m_netConnMgr.init()){
// 		Athena::logger->error("服务器初始化网络失败,启动服务器不成功");
// 		return false;
// 	}
	
// 	if (Athena::global["statProto"] == "true"){
// 		m_traceProto = true;
// 	}
// 	else {
// 		m_traceProto = false;
// 	}
	
// 	if (Athena::global["CreateTestAccount"] == "true"){
// 		m_bCreateTestRole = true;	
// 	}
// 	else {
// 		m_bCreateTestRole = false;
// 	}

// 	char listenParam[250] = {0};
// 	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%d",m_svrInfo.m_ip,m_svrInfo.m_port);
//     IOComponent *ioc = m_netConnMgr.listen(listenParam,g_spliter,g_serverAdapter);
//     if (ioc == NULL) {
// 		Athena::logger->error("监听失败!");
// 		return false;
//     }
// 	m_svrID = m_svrInfo.m_svrID;

// 	if(loadAllClient() == false) {
// 		return false;
// 	}

// #ifdef _USE_BEHAVIAC_AI
// 	if (!CBehaviacAIMgr::getSingleton().init()){
// 		Athena::logger->error("加载behaviac AI失败!");
// 		return false;
// 	}
// #endif	
	
// 	if (g_loadAllTbl()){
// 		Athena::logger->trace("[加载表格]加载成功!");
// 	}
// 	else {
// 		Athena::logger->error("[加载表格]失败!");
// 		return false;
// 	}
	
// 	if (!SevenDayActivityCfg::getSingleton().loadCfg()){
// 		Athena::logger->error("加载七天活动配置失败!");
// 		return false;
// 	}

// 	if (!CMapMgr::getSingleton().init(Athena::global["mapDir"].c_str())){
// 		Athena::logger->error("初始化地图失败!");
// 		return false;
// 	}
	
// 	if (!CSkillRangeCfg::getSingleton().loadCfg()) {
// 		Athena::logger->error("初始化技能范围失败!");
// 		return false;
// 	}
	
// 	if (!CShopMgr::loadCfg()){
// 		Athena::logger->error("加载商店配置失败!");
// 		return false;
// 	}
	
// 	if (!ItemBoxCfg::getSingleton().loadCfg()){
// 		Athena::logger->error("加载箱子配置文件失败!");
// 		return false;
// 	}
	
// 	if (!CAllRewardMgr::getSingleton().loadCfg()){
// 		Athena::logger->error("加载奖励相关配置失败!");
// 		return false;	
// 	}
	
// 	if (!CMiscCfgMgr::getSingleton().loadCfg()){
// 		Athena::logger->error("加载杂项配置失败!");
// 		return false;
// 	}
	
// 	if (!CDrawMgr::loadCfg()){
// 		Athena::logger->error("加载抽奖失败!");
// 		return false;
// 	}
	
// 	if (!CLevelMgr::loadCfg()){
// 		Athena::logger->error("加载关卡配置失败");
// 		return false;
// 	}
	
// 	if (!CGuildConfig::getSingleton().loadCfg()){
// 		Athena::logger->error("加载公会配置失败!");
// 		return false;
// 	}
	
// 	if (!CDropMgr::getSingleton().loadCfg()){
// 		Athena::logger->error("加载掉落配置失败!");
// 		return false;
// 	}
	
// 	if (!CTaskTemplateMgr::getSingleton().loadTaskCfg()){
// 		Athena::logger->error("加载任务配置错误");
// 		return false;
// 	}
	
// 	if (!CRobotMgr::getSingleton().loadCfg()){
// 		Athena::logger->error("加载机器人错误");
// 		return false ;
// 	}
	
// 	if (!CAIConfigMgr::getSingleton().loadCfg()){
// 		Athena::logger->error("AI配置文件");
// 		return false;
// 	}
	
// 	if (!CRMBShop::loadCfg()){
// 		Athena::logger->error("加载人民币商店错误!");
// 		return false;
// 	}

// 	if (!CTitleMgr::afterLoadTitleTbl()){
// 		Athena::logger->error("处理称号表格失败!");
// 		return false;
// 	}
	
// 	if (!ArenaConfig::getSingleton().loadCfg()){
// 		Athena::logger->error("竞技场奖励加载配置失败!");
// 		return false;
// 	}

// 	if (!LimitTimePetCfg::getSingleton().loadCfg()){
// 		Athena::logger->error("加载限时武神奖励失败!");
// 		return false;
// 	}
	
// 	if (!CCaptureCfg::getSingleton().loadCfg()){
// 		Athena::logger->error("加载夺宝配置");
// 		return false;
// 	}
	
// 	if (!RechargeActivityCfg::getSingleton().loadCfg()){
// 		Athena::logger->error("加载充值活动奖励");
// 		return false;
// 	}
	
// 	if (!CQuestionCfg::getSingleton().loadCfg()){
// 		Athena::logger->error("问卷调查配置加载错误");
// 		return false;
// 	}
	
// 	if (!CShopActivityMgr::getSingleton().loadCfg()){
// 		Athena::logger->error("商店活动加载失败!");
// 		return false;
// 	}
	
// 	if (!LuckShopCfg::getSingleton().loadCfg()){
// 		Athena::logger->error("幸运商店配置!");
// 		return false;
// 	}
	
// 	if (!GuardCfg::getSingleton().loadCfg()){
// 		Athena::logger->trace("护送配置!");
// 		return false;
// 	}
	
// 	if (!CLimitFashionCfg::getSingleton().loadCfg()){
// 		Athena::logger->trace("加载限时时装错误!");
// 		return false;
// 	}
	
// 	if (!COpenFunction::loadCfg()){
// 		Athena::logger->trace("加载功能开放等级错误!");
// 		return false;
// 	}

// 	if(Athena::global["CountryVersion"] == "Korea"){
// 		if(!CGloryfashionCfg::getSingleton().loadCfg()){
// 			Athena::logger->trace("加载荣耀时装商店失败!");
// 			return false;
// 		}
// 	}

// 	if(Athena::global["CountryVersion"] == "Korea"){
// 		if(!CFashionShopOpenCfg::getSingleton().loadCfg()){
// 			Athena::logger->trace("加载时装商店失败!");
// 			return false;
// 		}
// 	}

// 	if (!CExpeditionCfg::getSingleton().loadCfg()){
// 		Athena::logger->trace("加载远征副本配置出错!");
// 		return false;
// 	}

// 	//new KBEngine::Navigation();
// 	//KBEngine::Navigation::getSingleton().loadNavigation("xinshoucun",false);

// 	ChatMgr::getSingleton().initGMAccountInfo();
	
// #ifdef _Memmory_Leak_
// 	initMemoryLeakCheckSys();
// 	Athena::logger->trace("[服务器启动]初始化内存检测系统");
// #endif
// 	printf("游戏服务器启动成功\n");
// 	return true;
// }
bool NormalGameServer::startListen()
{
	char listenParam[250] = {0};
	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%d",m_svrInfo.m_ip,m_svrInfo.m_port);
    IOComponent *ioc = m_netConnMgr.listen(listenParam,g_spliter,g_serverAdapter);
    if (ioc == NULL) {
		Athena::logger->error("监听失败!");
		return false;
    }
	return true;
}
void NormalGameServer::createTimer()
{
	GameTimerCB  * cb = ATHENA_NEW GameTimerCB(0);
	m_timerMgr.addTimer(cb);
	GameTimer1SecCB * cb1Sec = ATHENA_NEW GameTimer1SecCB();
	m_timerMgr.addTimer(cb1Sec);
}

void NormalGameServer::stop()
{
	m_netConnMgr.stop();
	m_timerMgr.stop();
	g_unloadAllTbl();
	memoryLeaksDump();
}

bool NormalGameServer::loadAllClient()
{
	//连接Global服务器
	std::map<UINT16,SvrInfo>::iterator it =   m_serverCtn.begin();
	for (;it != m_serverCtn.end();++it){
		if (it->second.m_svrType == GLOBALSERVER) {
			char addr[200] = {0};
			snprintf(addr,sizeof(addr),"tcp:%s:%d",it->second.m_ip,it->second.m_port);
			IOComponent * pConn = m_netConnMgr.connect(addr,g_spliter, true,&GlobalSvrClientMgr::getSingleton());
			if (!pConn){
				Athena::logger->error("连接global失败!");
				 return false;
			}
			((TCPComponent *)pConn)->getTCPNetIO()->setDefaultPacketHandler(g_globalPacketHandler);
			GlobalSvrClientMgr::getSingleton().addConnectSvrInfo((TCPComponent *)pConn,it->second);
		}
		if (it->second.m_svrType == PUBGAMESERVER) {
			char addr[200] = {0};
			snprintf(addr,sizeof(addr),"tcp:%s:%d",it->second.m_ip,it->second.m_port);
			IOComponent * pConn = m_netConnMgr.connect(addr,g_spliter, true,&PubSvrClientMgr::getSingleton());
			if (!pConn){
				Athena::logger->error("连接pubgame失败!");
				 return false;
			}
			((TCPComponent *)pConn)->getTCPNetIO()->setDefaultPacketHandler(g_pub2NormalPacketHandler);
			PubSvrClientMgr::getSingleton().addConnectSvrInfo((TCPComponent *)pConn,it->second);
		}
	}
	::sleep(3);	
	return true;
}

extern GameServer *g_gameServer;

void g_singalHandler(int sig)
{
    if (SIGTERM == sig) {
		g_gameServer->stopTimer();
    }
	else if (SIGINT == sig) {
		g_gameServer->stopTimer();
    } 
	else if (SIGQUIT == sig){
        g_gameServer->stopTimer();
    }
	else if (SIGABRT == sig){
		g_gameServer->stopTimer();	
	}
	else if (SIGHUP == sig){
	}

}


int main(int argc, char *argv[])
{
	Athena::global["ServerName"] = "GameServer";
	//解析命令行
	g_parseCmdLine(argc, argv);
	//通用的初始化
	g_initCommon(argc,argv);
	
    NormalGameServer gameServer;
	//设置信号量
    signal(SIGTERM, g_singalHandler);
    signal(SIGINT,  g_singalHandler);
    signal(SIGQUIT, g_singalHandler);
	signal(SIGABRT, g_singalHandler);
	signal(SIGHUP, g_singalHandler);
    g_gameServer = &gameServer;

	ClientPacketFactory factory;
    ClientBinPacketSpliter spliter(&factory);
    GameServerAdapter serverAdapter;
  	GlobalPacketHandler  globalPacketHandler; 
  	Pub2NormalPacketHandler pub2NormalPacketHandler;
	g_factory = &factory;
	g_spliter = &spliter;
	g_globalPacketHandler = &globalPacketHandler;
	g_pub2NormalPacketHandler = &pub2NormalPacketHandler;
	g_serverAdapter = &serverAdapter;

    gameServer.run();
    return EXIT_SUCCESS;
}
