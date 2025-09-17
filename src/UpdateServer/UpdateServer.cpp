#include "TypesDef.h"
#include "CommonMicro.h"
#include "UpdateServer.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "PacketProcessor.h"
#include "httppacketspliter.h"
#include "UpdateInfoMgr.h"
#include "MysqlDatabase.h"
#include "SqlDelayThreadPool.h"
#include "NoticeMgr.h"
#include <getopt.h>


class UpdateServerAdapter : public IPacketHandler
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

UpdateServer::UpdateServer() : m_netConnMgr(&m_connMgr)
{
	m_bReloadCfg = false;
}

UpdateServer::~UpdateServer()
{
}

ClientPacketFactory * g_factory = NULL;
UpdateServerAdapter * g_serverAdapter = NULL;
HttpPacketSpliter  * g_httpPacketSpliter  = NULL;

bool UpdateServer::start()
{
	
	if (!ServerBase::start()){
		return false;
	}	
	UINT32 maxConnNum = (UINT32)atoi(Athena::global["maxConn"].c_str());
	if (maxConnNum <= 0){
		maxConnNum = 20000;
	}
    if (!m_netConnMgr.init(maxConnNum)){
		Athena::logger->error("网络库初始化失败,启动服务器不成功,初始化连接数");
		return false;
	}
	Athena::logger->trace("初始化了网络库最大连接数量为:%d",maxConnNum);
	    
	char listenParam[250] = {0};
	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%d",m_svrInfo.m_ip,m_svrInfo.m_port);

    IOComponent *pIoc = m_netConnMgr.listen(listenParam, g_httpPacketSpliter, g_serverAdapter);
    if (pIoc == NULL) {
		Athena::logger->error("监听%d失败!",m_svrInfo.m_port);
		return false;
    }

	if (g_databaseEngine.initialize(Athena::global["mysqlurl"].c_str())){
		Athena::logger->trace("[服务器启动]初始化%s成功",Athena::global["mysqlurl"].c_str());
	}  

	UpdateInfoMgr::getSingleton().load();
	CNoticeMgr::getSingleton().load();

	SqlDelayThreadPool::getSingleton().init(&g_databaseEngine);

	UpdateTimerCB * pTimerCB = ATHENA_NEW UpdateTimerCB(0);
	m_timerMgr.addTimer(pTimerCB);

	Update1SecTimerCB * oneSecTimerCB = ATHENA_NEW Update1SecTimerCB();
	m_timerMgr.addTimer(oneSecTimerCB);

	Update5MinTimerCB * fiveMinTimerCB = ATHENA_NEW Update5MinTimerCB();
	m_timerMgr.addTimer(fiveMinTimerCB);
#ifdef _Memmory_Leak_
	initMemoryLeakCheckSys();
	Athena::logger->trace("[服务器启动]初始化内存检测系统");
#endif
	printf("UpdateServer启动完毕!");
	return true;
}

void UpdateServer::stop()
{
	m_netConnMgr.stop();
	m_timerMgr.stop();
	Athena::logger->error("quit");
	memoryLeaksDump();
}

UpdateServer *g_updateServer;
void g_singalHandler(int sig)
{
    if (SIGTERM == sig) {
		g_updateServer->stopTimer();
    }
	else if (SIGINT == sig) {
		g_updateServer->stopTimer();
    } 
	else if (SIGQUIT == sig){
        g_updateServer->stopTimer();
    }
	else if (SIGABRT == sig){
		g_updateServer->stopTimer();	
	}
	else if (SIGHUP == sig){
		if (g_updateServer){
			g_updateServer->reloadSvrCfg();
			g_updateServer->setReloadCfg(true);
		}
	}

}

int main(int argc, char *argv[])
{
	Athena::global["ServerName"] = "UpdateServer";
	//解析命令行
	g_parseCmdLine(argc,argv);
	//初始化
	g_initCommon(argc,argv);
    UpdateServer updateServer;
    signal(SIGTERM, g_singalHandler);
    signal(SIGINT,  g_singalHandler);
    signal(SIGQUIT, g_singalHandler);
	signal(SIGABRT, g_singalHandler);
	signal(SIGHUP, g_singalHandler);
	ClientPacketFactory factory;
    UpdateServerAdapter serverAdapter;
	HttpPacketSpliter  httpPacketSpliter(&factory);
	httpPacketSpliter.setProtoHeader(5,0);

	g_factory = &factory;
	g_serverAdapter = &serverAdapter;
    g_updateServer = &updateServer;
	g_httpPacketSpliter = &httpPacketSpliter;
    updateServer.run();
    return EXIT_SUCCESS;
}
