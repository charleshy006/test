#include "TypesDef.h"
#include "CommonMicro.h"
#include "LoginServer.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "LoginTimerCB.h"
#include "AthenaNew.h"
#include "MysqlDatabase.h"
#include "SqlDelayThreadPool.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "LoginInfo.h"
#include "UserManager.h"
#include "md5.h"
#include <getopt.h>
#include "net.h"
#include "Login.pb.h"
#include "ModID.pb.h"
#include "LoginModHandle.h"
#include "UserManager.h"

LoginServerAdapter::LoginServerAdapter()
{
	m_protoModHandles[Proto::LOGIN_MOD] = ATHENA_NEW LoginModHandle();	
}

bool LoginServerAdapter::handlePacket(IOComponent * pConn, Packet *packet)
{
	if (ControlPacket::isControlPacket(packet)){
		return true;
	}
	
	PacketHeader * header = packet->getPacketHeader();

	ProtoMsgHandleBase * pModHandle = m_protoModHandles[header->m_modID];
	 if (pModHandle) {
		 if (pModHandle->handle((TCPComponent *)pConn,(char *)((ClientPacket *)packet)->getData(),header->m_dataLen,header->m_funID)){
			 packet->free();
			 return true;
		 }
	 }
	packet->free();
	return true;
}

CConnListner g_connListner;
void CConnListner::onAddComponent(IOComponent *ioc)
{}

void CConnListner::onRemoveComponent(IOComponent *ioc)
{}

LoginServer::LoginServer() : m_netConnMgr(&g_connListner) 
{
}

LoginServer::~LoginServer()
{
}

ClientPacketFactory * g_factory = NULL;
ClientBinPacketSpliter * g_spliter = NULL;
LoginServerAdapter * g_serverAdapter = NULL;

bool LoginServer::start()
{
	if (!ServerBase::start()){
		return false;
	}

    if (!m_netConnMgr.init()){
		Athena::logger->error("初始化网络库失败,服务器启动失败!");
		return false;
	}
	    
	char listenParam[250] = {0};
	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%d",m_svrInfo.m_ip,m_svrInfo.m_port);
    IOComponent *ioc = m_netConnMgr.listen(listenParam,g_spliter,g_serverAdapter);
    if (ioc == NULL) {
        return false;
    }

	if (g_databaseEngine.initialize(Athena::global["mysqlurl"].c_str())){
		Athena::logger->trace("[服务器启动]初始化%s成功",Athena::global["mysqlurl"].c_str());
	}
	UserManager::getSingleton().load();

	SqlDelayThreadPool::getSingleton().init(&g_databaseEngine);
#ifdef _Memmory_Leak_
	initMemoryLeakCheckSys();
	Athena::logger->trace("[服务器启动]初始化内存检测系统");
#endif
	return true;	
}

void LoginServer::stop()
{
    m_netConnMgr.stop();
	m_timerMgr.stop();
	g_databaseEngine.destroyMe();
	SqlDelayThreadPool::getSingleton().destroyMe();
	memoryLeaksDump();
}

LoginServer *g_loginServer;

void g_singalHandler(int sig)
{
    if (SIGTERM == sig) {
		g_loginServer->stopTimer();
    }
	else if (SIGINT == sig) {
		g_loginServer->stopTimer();
    } 
	else if (SIGQUIT == sig){
        g_loginServer->stopTimer();
    }
	else if (SIGABRT == sig){
		g_loginServer->stopTimer();	
	}
	else if (SIGHUP == sig){
	}

}

int main(int argc, char *argv[])
{
	Athena::global["ServerName"] = "LoginServer";
	//解析命令行
	g_parseCmdLine(argc, argv);
	//初始化公共
	g_initCommon(argc,argv);
    LoginServer loginServer;
	//初始化信号量
	signal(SIGTERM, g_singalHandler);
    signal(SIGINT,  g_singalHandler);
    signal(SIGQUIT, g_singalHandler);
	signal(SIGABRT, g_singalHandler);
	signal(SIGHUP, g_singalHandler);

	ClientPacketFactory factory;
    ClientBinPacketSpliter spliter(&factory);

	spliter.setZipFlag(false);
	spliter.setEncDec(true);
	spliter.setEncDecMethod(ENCDEC_ECBDES);
	unsigned char key[8]={'!','Q','}','+', '2', 'w', '[', '-'};
	spliter.setEncDecKey((char *)key);

    LoginServerAdapter serverAdapter;
	g_factory = &factory;
	g_spliter = &spliter;
	g_serverAdapter = &serverAdapter;

    g_loginServer = &loginServer;
    loginServer.run();
    return EXIT_SUCCESS;
}
