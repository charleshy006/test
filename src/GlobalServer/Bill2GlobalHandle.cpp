#include "Bill2GlobalHandle.h"
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
#include "RechargeMgr.h"

extern GlobalServer * g_globalServer;

Bill2GlobalHandle::Bill2GlobalHandle()
{
	registerHandle(&Bill2GlobalHandle::notifyRecharge);
}

Bill2GlobalHandle::~Bill2GlobalHandle()
{

}

bool Bill2GlobalHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<Bill2GlobalHandle>::handle(pCom,pData,cmdLen,funID); 
}

void Bill2GlobalHandle::notifyRecharge(TCPComponent * pCom,Bill::stNotifyUserRecharge * recvCmd)
{
	if (!recvCmd){
		return ;
	}

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if (!pUser){
		return ;
	}

	Athena::logger->trace("收到玩家充值的协议!");
}
