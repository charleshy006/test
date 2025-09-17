#include "TypesDef.h"
#include "CommonMicro.h"
#include "WebServer.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "MysqlDatabase.h"
#include "SqlDelayThreadPool.h"
#include "PacketProcessor.h"
#include "httppacketspliter.h"
#include "GlobalSvrClient.h"
#include "GlobalPacketProcessor.h"
#include "GameSvrClientMgr.h"
#include "GamePacketProcessor.h"
#include "UserLogMgr.h"
#include "TblDataMgr.h"
#include "WebUserConnMgr.h"
#include "ZoneMgr.h"
#include "MysqlTblQueryResult.h"
#include "ActivityConfigMgr.h"
#include "MiscCfgMgr.h"
#include "GameStaticsMgr.h"
#include "RMBShopCfg.h"
#include "LimitPetMgr.h"
#include <getopt.h>
#include "NftConfig.h"


class WebServerAdapter : public IPacketHandler
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

bool GamePacketHandler::handlePacket(IOComponent * pConn, Packet *packet)
{
	if (!packet){
		return false;
	}

	if (ControlPacket::isControlPacket(packet)){
		return false;
	}
	GamePacketProcessor::getSingleton().addPacket((ClientPacket *)packet);	
	return true;	 	
}

WebServer::WebServer() : m_netConnMgr(&WebUserConnMgr::getSingleton())
{
}

WebServer::~WebServer()
{
}

ClientPacketFactory * g_factory = NULL;
ClientBinPacketSpliter * g_spliter = NULL;
WebServerAdapter * g_serverAdapter = NULL;
HttpPacketSpliter  * g_httpPacketSpliter  = NULL;
GlobalPacketHandler    * g_globalPacketHandler = NULL;
GamePacketHandler * g_gamePacketHandler = NULL;

bool WebServer::start()
{
	
	if (!ServerBase::start()){
		return false;
	}	
	
    if (!m_netConnMgr.init()){
		Athena::logger->error("网络库初始化失败,启动服务器不成功!");
		return false;
	}
	    
	char listenParam[250] = {0};
	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%d",m_svrInfo.m_ip,m_svrInfo.m_port);

    IOComponent *pIoc = m_netConnMgr.listen(listenParam,g_httpPacketSpliter, g_serverAdapter);
    if (pIoc == NULL) {
		Athena::logger->error("监听%d失败!",m_svrInfo.m_port);
		return false;
    }
	if (g_databaseEngine.initialize(Athena::global["mysqlurl"].c_str())){
		Athena::logger->trace("[服务器启动]初始化%s成功",Athena::global["mysqlurl"].c_str());
	}
	else {
		Athena::logger->error("[服务器启动]初始化数据库连接失败!");
		return false;
	}
	
	if (!g_webLoadAllTbl()){
		Athena::logger->error("加载表格信息错误!");
		return false;
	}
	
	if (!CZoneMgr::getSingleton().load()){
		Athena::logger->error("加载区信息错误!");
		return false;
	}
	if(loadAllClient() == false) {
		return false;
	}  
	CGameStaticsMgr::getSingleton().load();	
	CMiscCfgMgr::getSingleton().loadCfg();
	CRMBShopCfg::getSingleton().loadCfg();
	/*char def[1024] = {
		"create table testTbl (`ID` int(10) unsigned NOT NULL AUTO_INCREMENT,PRIMARY KEY (`ID`))ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;"	
	};

	g_databaseEngine.directExecute(def,strlen(def),"");*/

	SqlDelayThreadPool::getSingleton().init(&g_databaseEngine);
	
	UserLogMgr::getSingleton().init();
	CActivityConfigMgr::getSingleton().load();
	UserLogMgr::getSingleton().loadEnterSelectPage();
	CLimitPetMgr::getSingleton().load();
	
	if (Athena::global["nftServer"] == "true")
	{
		NftConfig::getSingleton().loadCfg();
	} 
	
	WebServerTimerCB * timerCB = ATHENA_NEW WebServerTimerCB(0);
	m_timerMgr.addTimer(timerCB);

	WebServer1SecTimerCB * oneSecTimerCB = ATHENA_NEW WebServer1SecTimerCB();
	m_timerMgr.addTimer(oneSecTimerCB);

	WebServer1HourTimerCB * oneHourTimerCB = ATHENA_NEW WebServer1HourTimerCB();
	m_timerMgr.addTimer(oneHourTimerCB);
#ifdef _Memmory_Leak_
	initMemoryLeakCheckSys();
	Athena::logger->trace("[服务器启动]初始化内存检测系统");
#endif
	printf("webserver启动完毕!");
	return true;
}

bool WebServer::loadAllClient()
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
	}
	::sleep(3);

	it =   m_serverCtn.begin();
	for (;it != m_serverCtn.end();++it){
		if (it->second.m_svrType == GAMESERVER) {
			char addr[200] = {0};
			snprintf(addr,sizeof(addr),"tcp:%s:%d",it->second.m_ip,it->second.m_port);
			IOComponent * pConn = m_netConnMgr.connect(addr,g_spliter, true,&GameSvrClientMgr::getSingleton());
			if (!pConn){
				Athena::logger->error("连接game失败!");
				 return false;
			}
			((TCPComponent *)pConn)->getTCPNetIO()->setDefaultPacketHandler(g_gamePacketHandler);
			GameSvrClientMgr::getSingleton().addConnectSvrInfo((TCPComponent *)pConn,it->second);
		}
	}
	return true;
}

void WebServer::stop()
{
	m_timerMgr.stop();
	Athena::logger->error("quit");
	//m_netConnMgr.stop();
	//SqlDelayThreadPool::getSingleton().destroyMe();
	//g_databaseEngine.destroyMe();
	memoryLeaksDump();
}

WebServer * g_webServer;
void g_singalHandler(int sig)
{
    if (SIGTERM == sig) {
		g_webServer->stopTimer();
    }
	else if (SIGINT == sig) {
		g_webServer->stopTimer();
    } 
	else if (SIGQUIT == sig){
        g_webServer->stopTimer();
    }
	else if (SIGABRT == sig){
		g_webServer->stopTimer();	
	}
	else if (SIGHUP == sig){
		if (g_webServer){
			g_webServer->reloadSvrCfg();
		}
	}

}

int main(int argc, char *argv[])
{
	Athena::global["ServerName"] = "WebServer";
	//解析命令行
	g_parseCmdLine(argc,argv);
	//初始化
	g_initCommon(argc,argv);
    WebServer webServer;
    signal(SIGTERM, g_singalHandler);
    signal(SIGINT,  g_singalHandler);
    signal(SIGQUIT, g_singalHandler);
	signal(SIGABRT, g_singalHandler);
	signal(SIGHUP, g_singalHandler);
	ClientPacketFactory factory;
    ClientBinPacketSpliter spliter(&factory);
    WebServerAdapter serverAdapter;
	HttpPacketSpliter  httpPacketSpliter(&factory);
	httpPacketSpliter.setProtoHeader(5,0);
	GlobalPacketHandler  globalPacketHandler;
	GamePacketHandler    gamePacketHandler;

	g_factory = &factory;
	g_spliter = &spliter;
	g_serverAdapter = &serverAdapter;
    g_webServer = &webServer;
	g_httpPacketSpliter = &httpPacketSpliter;
	g_globalPacketHandler = &globalPacketHandler;
	g_gamePacketHandler =  &gamePacketHandler;
	if (Athena::global["deleteLogTbl"] == "true"){
		UserLogMgr::getSingleton().dropAllLogTbl();
		exit(0);
		return EXIT_SUCCESS;
	}

    webServer.run();
    return EXIT_SUCCESS;
}
