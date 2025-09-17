#include "TypesDef.h"
#include "CommonMicro.h"
#include "GlobalServer.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "MysqlDatabase.h"
#include "SqlDelayThreadPool.h"
#include "PlayerManager.h"
#include "PacketProcessor.h"
#include "RechargeMgr.h"
#include "HttpClient.h"
#include "httppacketspliter.h"
#include "RankMgr.h"
#include "Guild.h"
#include "GuildConfig.h"
#include "CaptureTreasure.h"
#include "Arena.h"
#include "One2OnePVP.h"
#include "xRedisClient.h"
#include "RedisDelayThreadPool.h"
#include "CaptureTreasure.h"
#include "BillPacketProcessor.h"
#include "BillSvrClient.h"
#include "TblDataMgr.h"
#include "FirstKillMgr.h"
#include "ArenaConfig.h"
#include "DrawHeroScoreMgr.h"
#include "GlobalVar.h"
#include "ActivityConfigMgr.h"
#include "HistoryRankMgr.h"
#include "GiftBag.h"
#include "BlackWhiteMgr.h"
#include "ServerConfig.h"
#include "CompensationSys.h"
#include "WorldBossCfg.h"
#include "WorldBoss.h"
#include "LimitTimePetCfg.h"
#include "OpenServerInfo.h"
#include "ItemBoxCfg.h"
#include "MiscCfgMgr.h"
#include "CaptureCfg.h"
#include "RoleInfoManager.h"
#include "QuestionCfg.h"
#include "QuestionMgr.h"
#include "CommonRefreshMgr.h"
#include "GuardCfg.h"
#include "OpenSvrPowerRankCfg.h"
#include "GuardMgr.h"
#include "WelfareCenterCfg.h"
#include "TxGiftBagMgr.h"
#include "CloseBetaWelfareMgr.h"
#include <getopt.h>
#include "GloryfashionMgr.h"
#include "GloryfashionCfg.h"
#include "Expedition.h"
#include "DevilCallCfg.h"
#include "SysLanguages.h"
#include "EmeraldDreamMgr.h"
#include "EmeraldDreamCfg.h"
#include "OperateActivity.h"
#include "OperateActivityCfg.h"
#include "FriendMgr.h"
#include "DiscoverCfg.h"
#include "DiscoverMgr.h"
#include "InviteCfg.h"
#include "InviteFriendMgr.h"
#include "HeroLeagueMgr.h"
#include "HeroLeagueCfg.h"
#include "LimitPetMgr.h"

xRedisClient g_redisEngine;

class GlobalServerAdapter : public IPacketHandler
{
public:
    bool handlePacket(IOComponent *pConn, Packet *packet)
	{
		if (ControlPacket::isControlPacket(packet)){
			return false; 
		}

		((ClientPacket *)packet)->m_pComponent = (TCPComponent *)pConn; 
		PacketProcessor::getSingleton().addPacket((ClientPacket *)packet);
        return true;
    }
};

bool BillPacketHandler::handlePacket(IOComponent * pConn, Packet *packet)
{
	if (!packet){
		return false;
	}

	if (ControlPacket::isControlPacket(packet)){
		return false;
	}

	BillPacketProcessor::getSingleton().addPacket((ClientPacket *)packet);	
	return true;	 	
}

GlobalServer::GlobalServer() : m_netConnMgr(&m_connMgr)
{
}

GlobalServer::~GlobalServer()
{
}

ClientPacketFactory * g_factory = NULL;
ClientBinPacketSpliter * g_spliter = NULL;
GlobalServerAdapter * g_serverAdapter = NULL;
ClientStringPacketSpliter * g_platFormSpliter = NULL;
HttpPacketSpliter  * g_httpPacketSpliter  = NULL;
BillPacketHandler * g_billPacketHandler = NULL;

bool GlobalServer::start()
{
	
	if (!ServerBase::start()){
		return false;
	}	
	
    if (!m_netConnMgr.init()){
		Athena::logger->error("网络库初始化失败,启动服务器不成功!");
		return false;
	}
	
	m_pubServerIndex    =   atoi(Athena::global["pubServerIndex"].c_str()); 

	m_areaCode          =   atoi(Athena::global["gameID"].c_str());

	char listenParam[250] = {0};
	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%d",m_svrInfo.m_ip,m_svrInfo.m_port);

    IOComponent *pIoc = m_netConnMgr.listen(listenParam, g_spliter, g_serverAdapter);
    if (pIoc == NULL) {
		Athena::logger->error("监听%d失败!",m_svrInfo.m_port);
		return false;
    }
	
	/*bzero(listenParam,sizeof(listenParam));
	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%s",m_svrInfo.m_ip,Athena::global["platfromPort"].c_str());
	pIoc = m_netConnMgr.listen(listenParam, g_platFormSpliter, g_serverAdapter);
	if (!pIoc) {
		Athena::logger->error("监听%s失败!",Athena::global["platfromPort"].c_str());
		return false;
	}
	
	bzero(listenParam,sizeof(listenParam));
	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%s",m_svrInfo.m_ip,Athena::global["httpPort"].c_str());
	pIoc = m_netConnMgr.listen(listenParam, g_httpPacketSpliter, g_serverAdapter);
	if (!pIoc) {
		Athena::logger->error("监听%s失败!",Athena::global["httpPort"].c_str());
		return false;
	}*/

	if (g_databaseEngine.initialize(Athena::global["mysqlurl"].c_str())){
		Athena::logger->trace("[服务器启动]初始化%s成功",Athena::global["mysqlurl"].c_str());
	}
	else {
		Athena::logger->error("[服务器启动]初始化数据库连接失败!");
		return false;
	}

	if(loadAllClient() == false) {
		return false;
	}   
	
	if (!g_globalLoadTbl()){
		Athena::logger->error("加载表格错误!");
		return false;
	}
	COpenFunction::loadCfg();

	CGlobalVarMgr::getSingleton().load();

	ArenaMgr::getSingleton().loadAll();

	CRankMgr::getSingleton().load();

	CRechargeMgr::getSingleton().load();
	
	CHttpClient::getSingleton().init();
	
	CGuildConfig::getSingleton().loadCfg();

	CGuildMgr::getSingleton().loadAllGuild();
	
	CCaptureTreasureMgr::getSingleton().loadData();
	
	COne2OnePVPMgr::getSingleton().loadCfg();
	COne2OnePVPMgr::getSingleton().load();
	
	CCaptureTreasureMgr::getSingleton().loadConfig();
	
	PlayerManager::getSingleton().load();
	
	CFirstKillMgr::getSingleton().load();

	SqlDelayThreadPool::getSingleton().init(&g_databaseEngine);
	
	ArenaConfig::getSingleton().loadCfg();

	CEmeraldDreamMgr::getSingleton().load();
	
	CUserLastLoginTimeMgr::getSingleton().load();

	/*{
		RedisNode RedisList1[3]=
		{       
			{0,"127.0.0.1", 6379, "", 2, 5},
		}; 
		g_redisEngine.Init();
		g_redisEngine.ConnectRedisCache(RedisList1,1, 1);
		CRedisDelayThreadPool::getSingleton().init(&g_redisEngine);
	}*/
	
	CDrawHeroScoreMgr::getSingleton().load();
	
	//CGlobalVarMgr::getSingleton().load();
	
	CActivityConfigMgr::getSingleton().load();
	
	CHistoryRankMgr::getSingleton().load();
	CGiftBagMgr::getSingleton().load();
	CBlackWhiteMgr::getSingleton().load();
	CServerConfigMgr::getSingleton().load();
	CCompensationMgr::getSingleton().load();
	CWorldBossCfg::getSingleton().loadCfg();
	CWorldBossMgr::getSingleton().load();
	LimitTimePetCfg::getSingleton().loadCfg();
    CLimitPetMgr::getSingleton().load();
    CLimitPetMgr::getSingleton().synInfo();
	CDrawHeroRankMgr::getSingleton().load();
	COpenServerMgr::getSingleton().load();
	ItemBoxCfg::getSingleton().loadCfg();
	CMiscCfgMgr::getSingleton().loadCfg();
	CCaptureCfg::getSingleton().loadCfg();
	RoleInfoManager::getSingleton().loadMaxCharID();
	CQuestionCfg::getSingleton().loadCfg();
	QuestionMgr::getSingleton().load();
	GuardCfg::getSingleton().loadCfg();	
	OpenSvrPowerRankCfg::getSingleton().loadCfg();
	CGuardMgr::getSingleton().load();
	WelfareCenterCfg::getSingleton().loadCfg();
	CTxGiftBagMgr::getSingleton().load();
	CCloseBetaWelfareMgr::getSingleton().load();
	CCloseBetaWelfareMgr::getSingleton().loadTblInfo();
	CManorStateMgr::getSingleton().load();
	CManorStateMgr::getSingleton().initState();
	CManorInfoMgr::getSingleton().load();
	CManorInfoMgr::getSingleton().initManorInfo();
	CManorApplyMgr::getSingleton().load();
	
	if(!CGloryfashionCfg::getSingleton().loadCfg()){
		Athena::logger->trace("加载荣耀时装商店失败!");
		return false;
	}

	//if(Athena::global["CountryVersion"] == "Korea"){
        if (!CDevilCallCfg::getSingleton().loadCfg()){
            Athena::logger->trace("加载魔王召唤出错!");
            return false;
        }
	//}
    if (!COperateActivityCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载新加活动出错!");
        return false;
    }
    if (!CInviteCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载分享好友出错!");
        return false;
    }

	CGloryfashionRankMgr::getSingleton().load();
    ExpeditionMgr::getSingleton().load();
    DevilCallMgr::getSingleton().load();
    OperateActivityMgr::getSingleton().load();

	CSysLanguages::getSingleton().load();
	
	CEmeraldDreamCfg::getSingleton().loadCfg();

	if (!CDiscoverCfg::getSingleton().loadCfg()) {
        Athena::logger->trace("加载走格子出错!");
        return false;
    }

	CDiscoverMgr::getSingleton().load();

	CInviteFriendMgr::getSingleton().load();

	CHeroLeagueCfg::getSingleton().loadCfg();
	CHeroLeagueMgr::getSingleton().load();
	CMarryMgr::getSingleton().load();


	UINT32 freshTime = CGlobalVarMgr::getSingleton().getValue(eGlobalVar_Common_RefreshTime);
	if (freshTime > 0){
		CCommonRefreshMgr::getSingleton().setRefreshTime(freshTime);
	}

	if (!CGlobalVarMgr::getSingleton().verifyDataBase()){
		Athena::logger->error("数据库没有启动成功,服务器启动失败!");
		return false;
	}
	/*std::string mysqlRet = s_exeShellCommand("mysql -uhdz -p123456 -h10.0.3.13  GameDB -e \"select ACCOUNT,NAME,CHARID from ROLEINFO\"");
	Athena::logger->trace("调用shell查询的值:");
	Athena::logger->trace(mysqlRet.c_str());*/

	GlobalServerTimerCB * pTimerCB = ATHENA_NEW GlobalServerTimerCB(0);
	m_timerMgr.addTimer(pTimerCB);

	GlobalServer1SecTimerCB * pOneSecTimerCB = ATHENA_NEW GlobalServer1SecTimerCB();
	m_timerMgr.addTimer(pOneSecTimerCB);

	GlobalServer5MinTimerCB * pMinTimerCB = ATHENA_NEW GlobalServer5MinTimerCB();
	m_timerMgr.addTimer(pMinTimerCB);
#ifdef _Memmory_Leak_
	initMemoryLeakCheckSys();
	Athena::logger->trace("[服务器启动]初始化内存检测系统");
#endif
	printf("global服务器启动完毕!");
	return true;
}

bool GlobalServer::loadAllClient()
{
	return true;
	//连接Global服务器
	std::map<UINT16,SvrInfo>::iterator it =   m_serverCtn.begin();
	for (;it != m_serverCtn.end();++it){
		if (it->second.m_svrType == BILLSERVER) {
			char addr[200] = {0};
			snprintf(addr,sizeof(addr),"tcp:%s:%d",it->second.m_ip,it->second.m_port);
			IOComponent * pConn = m_netConnMgr.connect(addr,g_spliter, true,&BillSvrClientMgr::getSingleton());
			if (!pConn){
				Athena::logger->error("连接Bill失败!");
				 return false;
			}
			((TCPComponent *)pConn)->getTCPNetIO()->setDefaultPacketHandler(g_billPacketHandler);
			BillSvrClientMgr::getSingleton().addConnectSvrInfo((TCPComponent *)pConn,it->second);
		}
	}
	::sleep(3);	
	return true;
}

void GlobalServer::stop()
{
	//m_netConnMgr.stop();
	m_timerMgr.stop();
	Athena::logger->error("quit");
	PlayerManager::getSingleton().destroyMe();
	g_databaseEngine.destroyMe();
	SqlDelayThreadPool::getSingleton().destroyMe();
	memoryLeaksDump();
}

GlobalServer *g_globalServer;
void g_singalHandler(int sig)
{
    if (SIGTERM == sig) {
		g_globalServer->stopTimer();
    }
	else if (SIGINT == sig) {
		g_globalServer->stopTimer();
    } 
	else if (SIGQUIT == sig){
        g_globalServer->stopTimer();
    }
	else if (SIGABRT == sig){
		g_globalServer->stopTimer();	
	}
	else if (SIGHUP == sig){
		if (g_globalServer){
			g_globalServer->reloadSvrCfg();
		}
	}

}

int main(int argc, char *argv[])
{
	Athena::global["ServerName"] = "GlobalServer";
	//解析命令行
	g_parseCmdLine(argc,argv);
	//初始化
	g_initCommon(argc,argv);
    GlobalServer globalServer;
    signal(SIGTERM, g_singalHandler);
    signal(SIGINT,  g_singalHandler);
    signal(SIGQUIT, g_singalHandler);
	signal(SIGABRT, g_singalHandler);
	signal(SIGHUP, g_singalHandler);
	ClientPacketFactory factory;
    ClientBinPacketSpliter spliter(&factory);
    GlobalServerAdapter serverAdapter;
	ClientStringPacketSpliter platFormSpliter(&factory);
	BillPacketHandler  billPacketHandler;

	platFormSpliter.setProtoHeader(Global::PLATFORM_MOD,0);
	HttpPacketSpliter  httpPacketSpliter(&factory);
	httpPacketSpliter.setProtoHeader(5,0);

	g_factory = &factory;
	g_spliter = &spliter;
	g_serverAdapter = &serverAdapter;
    g_globalServer = &globalServer;
	g_platFormSpliter = &platFormSpliter;
	g_httpPacketSpliter = &httpPacketSpliter;
	g_billPacketHandler = &billPacketHandler;
    globalServer.run();
    return EXIT_SUCCESS;
}
