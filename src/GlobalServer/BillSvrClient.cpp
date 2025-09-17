#include "BillSvrClient.h"
#include "GlobalServer.h"
#include "Bill.pb.h"

extern GlobalServer * g_globalServer;

BillSvrClientMgr::BillSvrClientMgr()
{
	
}

BillSvrClientMgr::~BillSvrClientMgr()
{
}

bool BillSvrClientMgr::sendMyInfo(TCPComponent * pConn)
{
	Bill::stLoginBill  cmd;
	cmd.set_svrid(g_globalServer->m_svrInfo.m_svrID);
	cmd.set_svrtype(GLOBALSERVER);
	cmd.set_port(g_globalServer->m_svrInfo.m_port);
	cmd.set_ip(g_globalServer->m_svrInfo.m_ip);
	cmd.set_zoneid(1);
	SEND_PROTO_2_CLIENT(pConn,cmd);
	return true;
}
