#include "TypesDef.h"
#include "CommonMicro.h"
#include "GateServer.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "GateUser.h"
#include "GateTimerCB.h"
#include "GateUserManager.h"
#include "PacketProcessor.h"
#include "GlobalPacketProcessor.h"
#include "GamePacketProcessor.h"
#include "GlobalSvrClientMgr.h"
#include "GameSvrClientMgr.h"
#include "GateUserConnMgr.h"
#include "GameVersionCfg.h"
#include <getopt.h>

class GateServerAdapter : public IPacketHandler
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

class GlobalPacketHandler : public IPacketHandler
{
	public :

		bool handlePacket(IOComponent * pConn, Packet *packet)
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
};

class GamePacketHandler : public IPacketHandler
{
	public :
		bool handlePacket(IOComponent * pConn, Packet *packet)
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
};

GateServer::GateServer() : m_netConnMgr(&GateUserConnMgr::getSingleton())
{
	m_maxGateUser = 0;
	m_isLoadSvrList = false;
	m_innerVerify = false;
	m_statNetProto = false;
	m_needVerifyToken = false;
	m_banLogin = false;
}

GateServer::~GateServer()
{
}

ClientBinPacketSpliter * g_clientSpliter = NULL;
ClientPacketFactory * g_clientFactory = NULL;
GlobalPacketHandler   * g_globalPacketHandler = NULL;
GamePacketHandler   *   g_gamePacketHandler = NULL;
GateServerAdapter * g_serverAdapter = NULL;
ClientBinPacketSpliter * g_serverSpliter = NULL;

bool GateServer::start()
{
	if (!ServerBase::start()){
		return false;	
	}

	std::map<UINT16,SvrInfo>::iterator it = m_serverCtn.begin();
	for (;it != m_serverCtn.end() ; ++it){
		SvrInfo & svrInfo = it->second;
		if (svrInfo.m_svrType == GAMESERVER) {
			//m_zoneID2GameID[svrInfo.m_zoneID] = svrInfo.m_svrID;
			//m_gameSvrID = svrInfo.m_svrID;
			m_gameSvrIDVec.push_back(svrInfo.m_svrID);
		}
	}
		
	m_maxGateUser = (UINT32)atoi(Athena::global["GateUserNum"].c_str());
	if (!m_maxGateUser){
		m_maxGateUser = 3000;
	}

    if (!m_netConnMgr.init(m_maxGateUser)){
		Athena::logger->error("网络初始化失败,服务器启动不成功!");
		return  false;
	}
	
	char listenParam[250] = {0};
	snprintf(listenParam,sizeof(listenParam),"tcp:%s:%d",m_svrInfo.m_ip,m_svrInfo.m_port);
    IOComponent *ioc = m_netConnMgr.listen(listenParam, g_serverSpliter, g_serverAdapter);
    if (ioc == NULL) {
		Athena::logger->error("监听失败!");
		return false;
    }
	
	m_svrID = m_svrInfo.m_svrID;
	if(loadAllClient() == false) {
		return false;
	}
	if (Athena::global["VerifyToken"] == "true"){
		m_needVerifyToken = true;
	}
	if (Athena::global["forwardProtoTrace"] == "true"){
		m_forwardProtoTrace = true;
	}

	if (Athena::global["InnerVerifyMethod"] == "true"){
		m_innerVerify  = true;	
	}
	
	if (!CGameVersionCfg::getSingleton().loadCfg()){
		Athena::logger->error("加载游戏版本配置失败!");
		return false;
	}

	//这个要放最后初始化
	GateTimerCB  * cb = ATHENA_NEW GateTimerCB(0);
	m_timerMgr.addTimer(cb);

	GateTimerCB10Sec * cb10Sec = ATHENA_NEW GateTimerCB10Sec();
	m_timerMgr.addTimer(cb10Sec);

	GateTimerCB30Sec * cb20Sec = ATHENA_NEW GateTimerCB30Sec();
	m_timerMgr.addTimer(cb20Sec);
	if (Athena::global["statProto"] == "true"){
		m_statNetProto = true;
	}

	
#ifdef _Memmory_Leak_
	initMemoryLeakCheckSys();
	Athena::logger->trace("[服务器启动]初始化内存检测系统");
#endif
	printf("网关服务器启动成功\n");
	return true;	
}

void GateServer::stop()
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("进入函数%s",__PRETTY_FUNCTION__);
#endif
	m_netConnMgr.stop();
	m_timerMgr.stop();
	GateUserManager::destroyMe();
	memoryLeaksDump();
}

bool GateServer::connectGame(SvrInfo & svrInfo)
{
	if (GameSvrClientMgr::getSingleton().isConnected(svrInfo.m_svrID)){
		return false;
	}

	char addr[200] = {0};
	snprintf(addr,sizeof(addr),"tcp:%s:%d",svrInfo.m_ip,svrInfo.m_port);
	IOComponent * pConn = m_netConnMgr.connect(addr,g_clientSpliter, true, &GameSvrClientMgr::getSingleton());
	if (!pConn){
		Athena::logger->error("连接game ip:%s,port:%d失败!",svrInfo.m_ip,svrInfo.m_port);
		return false;
	}

	((TCPComponent *)pConn)->getTCPNetIO()->setDefaultPacketHandler(g_gamePacketHandler);
	GameSvrClientMgr::getSingleton().addConnectSvrInfo((TCPComponent *)pConn,svrInfo);
	GameSvrClientMgr::getSingleton().addConnectSvrID(svrInfo.m_svrID);
	return true;
}

bool GateServer::connectGlobal(SvrInfo & svrInfo)
{
	char addr[200] = {0};
	snprintf(addr,sizeof(addr),"tcp:%s:%d",svrInfo.m_ip,svrInfo.m_port);
	IOComponent * pConn = m_netConnMgr.connect(addr,g_clientSpliter, true,&GlobalSvrClientMgr::getSingleton());
	if (!pConn){
		Athena::logger->error("连接global ip:%s, port:%d失败!",svrInfo.m_ip,svrInfo.m_port);
		return false;
	}

	((TCPComponent *)pConn)->getTCPNetIO()->setDefaultPacketHandler(g_globalPacketHandler);
	GlobalSvrClientMgr::getSingleton().addConnectSvrInfo((TCPComponent *)pConn,svrInfo);
	return true;
}

bool GateServer::loadAllClient()
{
	//连接Global服务器
	std::map<UINT16,SvrInfo>::iterator it =   m_serverCtn.begin();
	for (;it != m_serverCtn.end();++it){
		if (it->second.m_svrType == GLOBALSERVER) {
			if (!connectGlobal(it->second)){
				continue;
			}
		}
		else if (it->second.m_svrType == GAMESERVER){
			if (!connectGame(it->second)){
				continue;
			}

		}
		//::sleep(1);
		::usleep(50000);
	}
	return true;
}

UINT16 GateServer::assignGameSvr2User(std::string account)
{
	if (m_gameSvrIDVec.size() <= 0){
		return 0;
	}
	//my_hash<std::string> hashFun;
	//UINT16 index = hashFun(account) % m_gameSvrIDVec.size();
	//return m_gameSvrIDVec[index];
	return m_gameSvrIDVec[0];
	/*std::map<UINT16,UINT16>::iterator it = m_zoneID2GameID.find(m_svrInfo.m_zoneID);	
	if (it != m_zoneID2GameID.end()){
		return it->second;
	}*/
	//return m_gameSvrID;
}


GateServer * g_gateServer = NULL;

void g_singalHandler(int sig)
{
    if (SIGTERM == sig) {
		g_gateServer->stopTimer();
    }
	else if (SIGINT == sig) {
		g_gateServer->stopTimer();
    } 
	else if (SIGQUIT == sig){
        g_gateServer->stopTimer();
    }
	else if (SIGABRT == sig){
		g_gateServer->stopTimer();	
	}
	else if (SIGHUP == sig){
	}

}

int main(int argc, char *argv[])
{
	Athena::global["ServerName"] = "GateServer";
	//解析命令行
	g_parseCmdLine(argc, argv);
	//初始化
	g_initCommon(argc,argv);
    GateServer gateServer;
	//信号量
    signal(SIGTERM, g_singalHandler);
    signal(SIGINT,  g_singalHandler);
    signal(SIGQUIT, g_singalHandler);
	signal(SIGABRT, g_singalHandler);
	signal(SIGHUP, g_singalHandler);

    GateServerAdapter serverAdapter;
  	GlobalPacketHandler globalPacketHandler;
	GamePacketHandler   gamePacketHandler;
	ClientPacketFactory  clientFactory;
	ClientBinPacketSpliter  clientSpliter(&clientFactory);
	ClientBinPacketSpliter  serverSpliter(&clientFactory);

	serverSpliter.setZipFlag(false);
	serverSpliter.setEncDec(true);
	serverSpliter.setEncDecMethod(ENCDEC_ECBDES);
	unsigned char key[8]={'!','Q','}','+', '2', 'w', '[', '-'};
	serverSpliter.setEncDecKey((char *)key);

	g_serverAdapter = &serverAdapter;
	g_globalPacketHandler = &globalPacketHandler;
	g_gamePacketHandler = &gamePacketHandler;
	g_clientSpliter = &clientSpliter;
	g_clientFactory = &clientFactory;
	g_serverSpliter = &serverSpliter;

    g_gateServer = &gateServer;
    gateServer.run();
    return EXIT_SUCCESS;
}
