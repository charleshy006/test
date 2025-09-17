#include "PacketProcessor.h"
#include "RoleInfoManager.h"
#include "Player.h"
#include "PlayerManager.h"
#include "AthenaNew.h"
#include "Callback.h"
#include "SqlOperation.h"
#include "MysqlDatabase.h"
#include "Log4cxxHelp.h"
#include "GlobalServer.h"
#include "Global.pb.h"
#include "GateModHandle.h"
#include "GameModHandle.h"
#include "HttpClient.h"
#include "HttpParser.h"
#include "RechargeMgr.h"
#include "WebModHandle.h"

PacketProcessor::PacketProcessor()
{
	m_protoModHandles[Global::LOGIN_MOD]  = ATHENA_NEW LoginGlobalModHandle();
	m_protoModHandles[Global::GATE_MOD]  = ATHENA_NEW  GateModHandle();
	m_protoModHandles[Global::GAME_MOD]  = ATHENA_NEW  GameModHandle();
	m_protoModHandles[Global::WEB_MOD]  = ATHENA_NEW WebModHandle();
}

PacketProcessor::~PacketProcessor()
{
}

void PacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	PlayerManager::getSingleton().removeAllUserByConn(svrID);
	Athena::logger->trace("收到了连接断开的协议svrID=%d",svrID);
}

/*static void g_testHttpCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		Athena::logger->trace("获得的http相应信息%s",&(*response->getResponseData())[0]);
	}
	else {
		Athena::logger->error("获得的http相应失败!");
	}
}*/

bool PacketProcessor::processHttpMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
	if (!pConn || !header || !pNull) {
		return false;
	}
	/*if (Global::HTTP_MOD == header->m_modID){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("收到了来自http协议的包:%s",(const char *)pNull);
#endif
		CHttpParser parser;
		parser.parseHttpRequest((const char *)pNull,header->m_dataLen);
#ifdef _HDZ_DEBUG
		parser.printAllHeader();
#endif
		std::string act = parser.getReqPath();	
		if (act == "recharge"){
			std::string accountID = parser.getReqParamVal("accountid");		//账号
			std::string tid =  parser.getReqParamVal("tid");				//流水号
			std::string pointCard = parser.getReqParamVal("money");			//点卡数量
			
			if (CRechargeMgr::getSingleton().checkTid(tid)){
				Athena::logger->trace("流水id=%s已经充值过了,还试图发送过来",tid.c_str());
				return true;
			}
			
			CRechargeMgr::getSingleton().addTid(tid);

			Player * pUser = PlayerManager::getSingleton().getUserByAccount(accountID);
			if (pUser){//玩家在线的情况
				Global::stUserRecharge retCmd;
				retCmd.set_charid(pUser->getID());
				retCmd.set_pointcard((UINT32)atoi(pointCard.c_str()));
				pUser->sendProto2Game(retCmd);
				Athena::logger->trace("给在线玩家account=%s,id=%d,name=%s,冲了%s元宝,流水号是%s",pUser->getAccount().c_str(),pUser->getID(),pUser->getName(),pointCard.c_str(),tid.c_str());
			}
			else {//离线情况
				RoleInfoManager::getSingleton().findRechargeUserInfo((UINT32)atoi(accountID.c_str()),(UINT32)atoi(pointCard.c_str()));
				Athena::logger->trace("给离线玩家account=%s,冲了%s元宝,流水号是%s",accountID.c_str(),pointCard.c_str(),tid.c_str());
			}
			parser.reset();
			parser.setStatus(true,"ok");
			parser.setBody("recharge successful",strlen("recharge successful"));
			parser.setKeepAlive(false);
			std::string ret = parser.buildResponse();
			g_globalServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());
		}
		return true;
	}*/
	return false;
}


bool PacketProcessor::processOneBufferMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了协议 modID=%d,funID=%d",header->m_modID,header->m_funID);
#endif

	if (PacketProcessorBase::processOneBufferMsg(pConn,header,pNull)){
		return true;
	}
	
	/*if (processPlatformMsg(pConn,header,pNull)) {
		return true;
	}*/
	
	/*if (processHttpMsg(pConn,header,pNull)){
		return true;
	}*/

	ProtoMsgHandleBase * pModHandle = m_protoModHandles[header->m_modID];
	if (pModHandle) {
		if (pModHandle->handle(pConn,(char *)pNull,header->m_dataLen,header->m_funID)){
			return true;
		}
	}
	Athena::logger->error("%s (%u, %u, %u)", __PRETTY_FUNCTION__, header->m_modID, header->m_funID,header->m_dataLen);
	return false;
}
