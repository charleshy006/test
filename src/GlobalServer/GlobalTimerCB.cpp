#include <iostream>
#include <string>
#include "GlobalTimerCB.h"
#include "SqlDelayThreadPool.h"
#include "PacketProcessor.h"
#include "HttpClient.h"
#include "Guild.h"
#include "One2OnePVP.h"
#include "RedisDelayThreadPool.h"
#include "BillPacketProcessor.h"
#include "FriendMgr.h"
#include "json/writer.h"
#include "datacenter_log.h" 
#include "log_singleton.h"
#include "KVPair.h"
#include "NoticeMgr.h"
#include "HistoryRankMgr.h"
#include "ShutDownMgr.h"
#include "Arena.h"
#include "WorldBoss.h"
#include "Mail.h"
#include "DrawHeroScoreMgr.h"
#include "CompensationSys.h"
#include "OpenServerInfo.h"
#include "Team.h"
#include "Mail.h"
#include "CommonRefreshMgr.h"
#include "GuardMgr.h"
#include "GloryfashionMgr.h"
#include "RankMgr.h"
#include "EmeraldDreamMgr.h"
#include "DiscoverMgr.h"
#include "Guild.h"
#include "HeroLeagueMgr.h"

bool GlobalServerTimerCB::exec(RealTime & curTime)
{
	PacketProcessor::getSingleton().processAllPacket();
	SqlDelayThreadPool::getSingleton().processAllCB();
	CRedisDelayThreadPool::getSingleton().processAllCB();
	BillPacketProcessor::getSingleton().processAllPacket();
	CHttpClient::getSingleton().processResponse(curTime.sec());
	return true;
}

bool GlobalServer1SecTimerCB::exec(RealTime & curTime)
{
	CGuildMgr::getSingleton().timer(curTime.sec());
	COne2OnePVPMgr::getSingleton().timer(curTime.sec());
	CLilianMgr::getSingleton().timer(curTime.sec());
	CNoticeMgr::getSingleton().timer(curTime.sec());
	CHistoryRankMgr::getSingleton().timer(curTime.sec());
	CShutDownMgr::getSingleton().timer(curTime.sec());
	ArenaMgr::getSingleton().timer(curTime.sec());
	CWorldBossMgr::getSingleton().timer(curTime.sec());
	CBatchSendMailMgr::getSingleton().timer(curTime.sec());
	CDrawHeroRankMgr::getSingleton().timer(curTime.sec());
	COpenServerMgr::getSingleton().timer(curTime.sec());
	CTeamMgr::getSingleton().timer(curTime.sec());
	CCommonRefreshMgr::getSingleton().checkDataRefresh();
	CGuardMgr::getSingleton().timer(curTime.sec());
	CManorApplyMgr::getSingleton().returnContribute();
	CGloryfashionRankMgr::getSingleton().timer(curTime.sec());
	CRankMgr::getSingleton().timer(curTime.sec());
	CEmeraldDreamMgr::getSingleton().timer(curTime.sec());
	CDiscoverMgr::getSingleton().timer(curTime.sec());
	CHeroLeagueMgr::getSingleton().timer(curTime.sec());
	CMarryMgr::getSingleton().timer(curTime.sec());
	return true;
}

static void g_reportOnlineNumCB(CHttpClient* client, CHttpResponse* response)
{
	if (response && response->isSucceed()){
		std::vector<char> &retVec = *response->getResponseData();
		if (retVec.size() > 0){
			std::string ret;
			ret.assign(&retVec[0],retVec.size());
			Athena::logger->trace("获得的http响应信息%s",ret.c_str());
		}
	}
	else {
		Athena::logger->error("获得的http响应失败!");
	}
}

bool GlobalServer5MinTimerCB::exec(RealTime & curTime)
{
	MailManager::getSingleton().deleteOutdateMail();
	CCompensationMgr::getSingleton().timer(curTime.sec());

	if(Athena::global["tapdbid"] != ""){
// 		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
// 		// pRequest->setUrl("http://monitor.uuzuonline.com/onlineReport");
// 		pRequest->setUrl("https://se.tapdb.net/tapdb/online");
// 		pRequest->setRequestType(CHttpRequest::kHttpPost);
// 		// pRequest->setTag("reportOnline");

// 		std::vector<std::string> headerVec;
// 		headerVec.push_back("Content-Type: application/json");
// 		pRequest->setHeaders(headerVec);

// 		Json::FastWriter writer;
// 		// Json::Value root;
// 		Json::Value content;	
		
// 		content["appid"] = Athena::global["tapdbid"].c_str();//"9subbbcjkudb1mkn"; 

// 		Json::Value valueList;
// 		Json::Value value;	
		
// 		value["server"] = Athena::global["serverID"].c_str();
// 		value["online"] = PlayerManager::getSingleton().userNum(); 
// 		value["timestamp"] =  curTime.sec();

// 		valueList.append(value);
// 		content["onlines"] = valueList;
// 		// root.append(content);
// 		std::string json_file = writer.write(content);
// 		pRequest->setRequestData(json_file.c_str(),json_file.size());
// #ifdef _HDZ_DEBUG
// 		Athena::logger->trace("上报的文本是%s",json_file.c_str());
// #endif
// 		pRequest->setResponseCallback(g_reportOnlineNumCB);
// 		CHttpClient::getSingleton().asynSend(pRequest);
	}

	/*--- 打印生态日志 ---*/
	 uuzu::logsdk::DatacenterLog& logger = uuzu::logsdk::Singleton<uuzu::logsdk::DatacenterLog>::getInstance();
	 //uuzu::logsdk::UserInfo userInfo;
	 char szTimeStamp[20] = { 0 };
	 snprintf(szTimeStamp,sizeof(szTimeStamp),"%d",curTime.sec());
	 char szOnlineNumStamp[20] = { 0 };
	 snprintf(szOnlineNumStamp,sizeof(szOnlineNumStamp),"%d",PlayerManager::getSingleton().userNum());
	 logger.sendServerLog("PCU",szTimeStamp,szOnlineNumStamp);
	/*----- 打印生态日志 -----*/

	 PlayerManager::getSingleton().updateOnlineNum(curTime.sec());

	 Global::stNotifyWebOnlineNum webCmd;
	 webCmd.set_num(PlayerManager::getSingleton().userNum());
	 g_globalServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
	return true;
}
