#include "PacketProcessor.h"
#include "AthenaNew.h"
#include "Callback.h"
#include "SqlOperation.h"
#include "MysqlDatabase.h"
#include "Log4cxxHelp.h"
#include "WebServer.h"
#include "Global.pb.h"
#include "HttpParser.h"
#include "json/reader.h"
#include "json/writer.h"
#include "UserLogMgr.h"
#include "RoleInfoManager.h"
#include "TblDataMgr.h"
#include "GameStaticsMgr.h"
#include "GlobalSvrClient.h"
#include "WebUserConnMgr.h"
#include "ZoneMgr.h"
#include "zlib.h"
#include "ActivityConfigMgr.h"
#include "Game.pb.h"
#include "GameSvrClientMgr.h"
#include "OperationMgr.h"
#include "md5.h"
#include "OpIDMgr.h"
#include "RMBShopCfg.h"
#include "AppConfig.h"
#include "HttpClient.h"
#include "LimitPetMgr.h"
#include "ServerListener.h"
#include "NftConfig.h"

#include <fstream>

PacketProcessor::PacketProcessor()
{
}

PacketProcessor::~PacketProcessor()
{
}

void PacketProcessor::disConnnectPacketCome(UINT16 svrID)
{
	Athena::logger->trace("收到了连接断开的协议svrID=%d",svrID);
}

struct stGetItemInfoCB : public TblDatabaseCB<stItemBase>
{
	Json::Value & m_resList;
	Json::Value & m_itemList;
	UINT32 m_itemCount;

	stGetItemInfoCB(Json::Value & resList,Json::Value &itemList) : m_resList(resList),m_itemList(itemList)
	{
		m_itemCount = 0;
	}

	~stGetItemInfoCB()
	{
		
	}

	virtual bool doit(stItemBase * pItemBase)
	{
		if (!pItemBase){
			return true;
		}
		
		/*if (m_itemCount >= 5){
			return true;
		}*/

		Json::Value info;
		info["id"] = pItemBase->m_itemID;
		//info["name"] = "test1";
		info["name"] = pItemBase->m_name;
		info["desc"] = "";

		if (pItemBase->m_itemID <= eResource_ID_Max){//是资源
			m_resList.append(info);
		}	
		else {
			m_itemCount += 1;
			m_itemList.append(info);
		}
		
		return true;
	}
};

#define unzipBufferLen(zipSize)    ((zipSize) * 120 / 100 + 12)

bool PacketProcessor::processHttpMsg(TCPComponent *pConn,const PacketHeader * header,const void * pNull)
{
	if (!pConn || !header || !pNull) {
		return false;
	}
	if (Global::HTTP_MOD == header->m_modID){
#ifdef _HDZ_DEBUG
		Athena::logger->trace("收到了来自http协议的包:%s,cmdLen=%d",(const char *)pNull,header->m_dataLen);
#endif
		CHttpParser parser;
		parser.parseHttpRequest((const char *)pNull,header->m_dataLen);

		/*std::string waitContinue = parser.findHeader("Expect");

		if ((waitContinue == "100-continue")){//该包是等待包
			parser.clearHeader();
			parser.setContinue(true);
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
			Athena::logger->trace("发送了一个等待包------------");
#endif
			return true;
		}*/
#ifdef _HDZ_DEBUG
		parser.printAllHeader();
#endif
		std::string actTop = parser.getReqPath();
		if (actTop == "/TestGetServerList"){//测试获得服务器信息
			std::string content = CZoneMgr::getSingleton().getZoneInfo();
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
		}
		else if (actTop == "/TXGetGiftBag"){
			if (!COpIDMgr::getSingleton().isYYB()){
				return true;
			}

			std::string openid = parser.getReqParamVal("openid");
			std::string taskid  = parser.getReqParamVal("taskid");
			std::string awardid = parser.getReqParamVal("rewardid");
			std::string billno  = parser.getReqParamVal("billno");
			std::string action = parser.getReqParamVal("action");
			std::string charID = parser.getReqParamVal("charID");

			UINT16 connID = 0;
			if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
				return true;
			}
			WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);
			
			Global::stReqGetTxGiftBag cmd;
			cmd.set_action(action);
			cmd.set_arewardid((UINT32)atoi(awardid.c_str()));
			cmd.set_taskid((UINT32)atoi(taskid.c_str()));
			cmd.set_openid(openid);
			cmd.set_billno(billno);
			cmd.set_webconnid(connID);
			cmd.set_charid((UINT32)atoi(charID.c_str()));
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到了来自txgiftserver的通知");
#endif
		}
		else if (actTop == "/shutdown"){//关闭服务器
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到了来自来自master的shutdown通知");
#endif
			std::vector<std::string> paramVec;
			std::string timestamp = parser.getReqParamVal("timestamp");
            paramVec.push_back(timestamp);
			std::string sign = parser.getReqParamVal("sign");
            std::string content;
            if (ServerListener::getSingleton().verifySign(paramVec, sign)) {
                content = std::string("ok");
            } else {
                content = std::string("failed");
            }
            Athena::logger->trace("content is %s", content.c_str());

			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
            
            if (content == std::string("ok")) {
                Global::stSendShutDown cmd;
                cmd.set_msg("shutdown");
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
            }
        }
		else if (actTop == "/get_config"){//获得服务器的对应配置信息
			std::vector<std::string> params;
			std::string timestamp = parser.getReqParamVal("timestamp");
			if (timestamp != ""){
				params.push_back(timestamp);
			}

			Json::FastWriter writer;
			Json::Value root;
			Json::Value configData;

			std::string sign = parser.getReqParamVal("sign");
			if (!COperationMgr::getSingleton().verifySign(params,sign)){
				root["return_code"] =  1;
				root["return_msg"] = "校验不成功!";
			}
			else {
				UINT16 connID = 0;
				if (WebUserConnMgr::getSingleton().genUniqueID(connID)){
					root["return_code"] =  0;
					root["return_msg"] = "成功!";
					Global::stGetSvrConfig cmd;
					cmd.set_webconnid(connID);
					WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);
					GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
					return true;
				}
				else {
					root["return_code"] =  1;
					root["return_msg"] = "连接太多,稍后再访问!";
				}
			}
			parser.clearHeader();
			std::string content = writer.write(root);
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
		}
		else if (actTop == "/post_config"){//修改服务器的对应配置信息
			std::vector<std::string> params;
			parser.changePostConent2ReqParm();
			std::string timestamp = parser.getReqParamVal("timestamp");
			if (timestamp != ""){
				params.push_back(timestamp);
			}

			Json::FastWriter writer;
			Json::Value root;
			Json::Value configData;

			std::string sign = parser.getReqParamVal("sign");
			if (!COperationMgr::getSingleton().verifySign(params,sign)){
				root["return_code"] =  1;
				root["return_msg"] = "校验不成功!";
			}
			else {
				root["return_code"] =  0;
				root["return_msg"] = "成功!";
				
				std::string configData = parser.getReqParamVal("config_data");
				//configData = HttpUtility::URLDecode(configData); 
				Athena::logger->trace("configData:%s",configData.c_str());
				Json::Reader reader;
				Json::Value  configJson;
				if (!reader.parse(configData,configJson)){
					Athena::logger->error("json解析失败!");
					return true;
				}
				
				Global::stUpdateSvrCfg cmd;
				std::string value;
				if (configJson["statProto"].isString()){
					value = configJson["statProto"].asString();	
					Athena::global["statProto"] = value;
					Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
					if (pSyn){
						pSyn->set_key("statProto");
						pSyn->set_value(value);
					}
				}
				
				if (configJson["gameVersion"].isString()){
					value = configJson["gameVersion"].asString();	
					Athena::global["gameVersion"] = value;
					Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
					if (pSyn){
						pSyn->set_key("gameVersion");
						pSyn->set_value(value);
					}
				}
				
				if (configJson["gameID"].isString()){
					value = configJson["gameID"].asString();	
					Athena::global["gameID"] = value;
					Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
					if (pSyn){
						pSyn->set_key("gameID");
						pSyn->set_value(value);
					}
				}
				
				if (configJson["opID"].isString()){
					value = configJson["opID"].asString();	
					Athena::global["platformID"] = value;
					
					Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
					if (pSyn){
						pSyn->set_key("platformID");
						pSyn->set_value(value);
					}
				}
				
				if (configJson["serverID"].isString()){
					value = configJson["serverID"].asString();	
					Athena::global["serverID"] = value;
					
					Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
					if (pSyn){
						pSyn->set_key("serverID");
						pSyn->set_value(value);
					}
				}
				
				if (configJson["serverKey"].isString()){
					value = configJson["serverKey"].asString();	
					Athena::global["serverKey"] = value;
					
					Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
					if (pSyn){
						pSyn->set_key("serverKey");
						pSyn->set_value(value);
					}
				}
				
				if (configJson["billURL"].isString()){
					value = configJson["billURL"].asString();	
					Athena::global["billURL"] = value;
					
					Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
					if (pSyn){
						pSyn->set_key("billURL");
						pSyn->set_value(value);
					}
				}
				
				if (configJson["payType"].isString()){
					value = configJson["payType"].asString();	
					Athena::global["payType"] = value;
					
					Global::stServerCfgPair * pSyn = cmd.add_cfgpair();
					if (pSyn){
						pSyn->set_key("payType");
						pSyn->set_value(value);
					}
				}
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
				////todo
			}
			parser.clearHeader();
			std::string content = writer.write(root);
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
		}
		else if (actTop == "/post_whitelist"){//修改服务器的黑白名单配置
			parser.changePostConent2ReqParm();
			std::vector<std::string> params;
			std::string timestamp = parser.getReqParamVal("timestamp");
			if (timestamp != ""){
				params.push_back(timestamp);
			}
			
			std::string black_list = parser.getReqParamVal("black_list");
			if (black_list != ""){
				//black_list = HttpUtility::URLDecode(black_list);
				params.push_back(black_list);
			}
			
			std::string white_list = parser.getReqParamVal("white_list");
			if (white_list != ""){
				//white_list = HttpUtility::URLDecode(white_list);
				params.push_back(white_list);
			}
			

			Json::FastWriter writer;
			Json::Value root;
			Json::Value configData;

			std::string sign = parser.getReqParamVal("sign");
			if (!COperationMgr::getSingleton().verifySign(params,sign)){
				root["return_code"] =  1;
				root["return_msg"] = "校验不成功!";
			}
			else {
				root["return_code"] =  0;
				root["return_msg"] = "成功!";

				////todo
				std::vector<std::string> strVec;
				g_stringtok(strVec,black_list,",");
				
				COperationMgr::getSingleton().clearAllIP();

				Global::stAddBlackAndWhite cmd;

				for (UINT16 i = 0;i < strVec.size();++i){
					COperationMgr::getSingleton().addOneIPType(strVec[i].c_str(),eBlack_Login_IP);	
					cmd.add_blacklist(strVec[i]);
				}

				strVec.clear();
				g_stringtok(strVec,white_list,",");
				for (UINT16 i = 0;i < strVec.size();++i){
					cmd.add_whitelist(strVec[i]);
					COperationMgr::getSingleton().addOneIPType(strVec[i].c_str(),eWhite_Login_IP);	
				}
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			parser.clearHeader();
			std::string content = writer.write(root);
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
		}
		else if (actTop == "/post_clear"){//清档
			parser.changePostConent2ReqParm();
			std::vector<std::string> params;
			std::string header = parser.getReqParamVal("timestamp");
			if (header != ""){
				params.push_back(header);
			}
			
			Json::FastWriter writer;
			Json::Value root;
			Json::Value configData;

			std::string sign = parser.getReqParamVal("sign");
			if (!COperationMgr::getSingleton().verifySign(params,sign)){
				root["return_code"] =  1;
				root["return_msg"] = "校验不成功!";
			}
			else {
				if (g_webServer->getSec() >= g_webServer->getOpenServerTime()){//已经开服了
					root["return_code"] =  1;
					root["return_msg"] = "已经开服了不能删档!";
				}
				else if (RoleInfoManager::getSingleton().getRecordCount("ROLEINFO","") >= 100){
					root["return_code"] =  1;
					root["return_msg"] = "已经有100条记录了不能清档!";
				}
				else {
					root["return_code"] =  0;
					root["return_msg"] = "成功!";
					COperationMgr::getSingleton().clearAllTbl();
				}
				////todo
			}

			parser.clearHeader();
			std::string content = writer.write(root);
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
		}
		else if (actTop == "/post_maintenance"){//对指定服务器发维护请求
			parser.changePostConent2ReqParm();
			std::vector<std::string> params;
			std::string header = parser.getReqParamVal("timestamp");
			if (header != ""){
				params.push_back(header);
			}
			
			std::string opTime = parser.getReqParamVal("op_time");
			if (opTime != ""){
				opTime = HttpUtility::URLDecode(opTime);
				params.push_back(opTime);	
			}
			
			std::string opDurning = parser.getReqParamVal("op_durning");
			if (opDurning != ""){
				//params.push_back(opDurning);	
			}

			Json::FastWriter writer;
			Json::Value root;
			Json::Value configData;

			std::string sign = parser.getReqParamVal("sign");
			if (!COperationMgr::getSingleton().verifySign(params,sign)){
				root["return_code"] =  1;
				root["return_msg"] = "校验不成功!";
			}
			else {
				root["return_code"] =  0;
				root["return_msg"] = "成功!";
				////todo
				Global::stSetServerMaintenanceTime cmd;
				cmd.set_optime(opTime);
				cmd.set_opduring((UINT32)atoi(opDurning.c_str()));
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}

			parser.clearHeader();
			std::string content = writer.write(root);
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
		}
		else if (actTop == "/post_open_server_time"){//修改开服时间
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到修改开服时间请求:%s",parser.getPostContent().c_str());		
#endif
            std::string emptyStr;
			std::vector<std::string> params;
			std::string openServerTime = HttpUtility::URLDecode(parser.getReqParamVal("open_time"));
            if (openServerTime != emptyStr) {
                params.push_back(openServerTime);
            }
			std::string timeStamp = parser.getReqParamVal("timestamp");
            if (timeStamp != emptyStr) {
                params.push_back(timeStamp);
            }
			std::string sign = parser.getReqParamVal("sign");

			Json::FastWriter writer;
			Json::Value root;
			Json::Value configData;

			if (!COperationMgr::getSingleton().verifySign(params, sign)){
				root["return_code"] = -1;
				root["return_msg"] = "校验不成功!";
			} else {
                struct tm _tm;
                if (strptime(openServerTime.c_str(),"%Y-%m-%d %H:%M:%S",&_tm)){
                    std::fstream app("../../config/appconfig.xml");
                    char buf[10240];
                    app.readsome(buf, sizeof(buf));
                    app.close();
                    app.open("../../config/appconfig.xml", std::fstream::trunc | std::fstream::out);
                    if (app.good()) {
                        root["return_code"] = 1;
                        root["return_msg"] = "";
                        UINT32 time = mktime(&_tm);
                        g_webServer->setOpenServerTime(time);
                        Global::stUpdateSvrOpenTime globalCmd;;
                        globalCmd.set_time(time);
                        Game::stUpdateSvrOpenTime gameCmd;;
                        gameCmd.set_time(time);
                        GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);
                        GameSvrClientMgr::getSingleton().broadcastProto2All(gameCmd);

                        std::string data(buf);
                        std::string key = Athena::global["OpenServer_Time"];
                        std::stringstream ss;
                        ss << time;
                        std::string key2 = ss.str();

                        size_t index = data.find(key.c_str());
                        std::string leftStr = data.substr(0, index);
                        std::string rightStr = data.substr(index + key.length(), data.length() - key.length());
                        app << leftStr << key2 << rightStr;
                        Athena::global["OpenServer_Time"] = key2;
                    } else {
                        Athena::logger->error("打开../../config/appconfig.xml出错");
                        root["return_code"] = -3;
                        root["return_msg"] = "打开配置文件出错";
                    }
                    app.close();
                } else {
                    root["return_code"] = -2;
                    root["return_msg"] = "时间格式错误";
                }
            }
			parser.clearHeader();
			std::string content = writer.write(root);
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
			Athena::logger->trace("修改开服时间返回:%s", content.c_str());
#endif
        }
		else if (actTop == "/Recharge"){//如果是充值
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到充值请求:%s",parser.getPostContent().c_str());		
#endif
			std::vector<std::string> strVec;
			g_stringtok(strVec,parser.getPostContent(),"&");
			std::string sign;
			UINT32 serverID = 0;
			UINT32 roleID = 0;
			std::string tid;
			UINT32 itemID = 0;
			UINT32 money = 0;
			std::string account;
			UINT32 opID = 0;
			std::string opProductID;

			std::map<std::string,std::string> kvSortMap;

			for (UINT16 i = 0;i < strVec.size();++i){
				std::vector<std::string> subStrVec;
				g_stringtok(subStrVec,strVec[i],"=");
				if (subStrVec.size() >= 2){
					if (subStrVec[0] == "sign"){
						sign = subStrVec[1];
						continue;	
					}
					else {
						kvSortMap[subStrVec[0]] = subStrVec[1];
						if (subStrVec[0] == "server_id"){
							serverID = (UINT32)atoi(subStrVec[1].c_str());
							continue;
						}
						else if (subStrVec[0] == "coo_order_id"){
							tid = subStrVec[1];
							continue;   
						} 
						else if (subStrVec[0] == "game_role_id"){
							roleID = (UINT32)atoi(subStrVec[1].c_str());
							continue;
						}
						else if (subStrVec[0] == "product_id"){
							opProductID = subStrVec[1];
							/*std::vector<std::string> thdStrVec;
							g_stringtok(thdStrVec,subStrVec[1],".");
							if (thdStrVec.size() > 0){
								itemID = (UINT32)atoi(thdStrVec[thdStrVec.size() - 1].c_str());
							}
							else {
								itemID = 0;
							}*/
							continue;
						}
						else if (subStrVec[0] == "amount"){
							money = (UINT32)atoi(subStrVec[1].c_str());
							continue;
						}
						else if (subStrVec[0] == "osdk_user_id"){
							account = subStrVec[1];
							continue;
						}
						else if (subStrVec[0] == "op_id"){
							opID = (UINT32)atoi(subStrVec[1].c_str());
							continue;
						}
					}
				}
				else if (subStrVec.size() >= 1){
					kvSortMap[subStrVec[0]] = "";
				}
			}

            if (Athena::global["isNotUuzu"] == std::string("true")) {
                itemID = CRMBShopCfg::getSingleton().getItemId(opProductID);
            } else {
                itemID = CRMBShopCfg::getSingleton().getItemID(opID,opProductID);
                if (itemID <= 0){
                    std::vector<std::string> thdStrVec;
                    g_stringtok(thdStrVec,opProductID,".");
                    if (thdStrVec.size() > 0){
                        itemID = (UINT32)atoi(thdStrVec[thdStrVec.size() - 1].c_str());
                    }
                    else {
                        itemID = 0;
                    }
                }
            }

			std::map<std::string,std::string>::iterator it = kvSortMap.begin();
			std::ostringstream os ;
			bool first = true;

			for (;it != kvSortMap.end();++it){
				if (first){
					os << it->first <<"="<< HttpUtility::URLDecode(it->second);
					first = false;
				}    
				else {
					os <<"&"<<it->first<<"="<<HttpUtility::URLDecode(it->second);
				}   
			} 
			//os << SERVER_SECRET;
			//os << "dtyPN2MDVaY5fL8U";
			//os << "JJa0MnrBolr60Inv";
			//os << CZoneMgr::getSingleton().getServerKey(serverID);
            if (Athena::global["isNotUuzu"] == std::string("true")) {
                os << Athena::global["BillKey"];
            } else {
                os << Athena::global["serverKey"];
            }
			Athena::logger->trace("[人民币商店购买]计算签名时候的值:%s",os.str().c_str());
			char tokenBuf[1024] = {0};
			int  bufLen = 0;
			snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",os.str().c_str());
			bufLen = strlen(tokenBuf);
			char digest[64] = {0};
			utils::Md5(tokenBuf,bufLen,digest);
			utils::HexString::bin2asc(digest,16,digest,64,bufLen);
			digest[bufLen] = 0;
			Athena::logger->trace("[人民币商店购买](%s)计算出来的签名是:%s",os.str().c_str(),digest);
			bool bVerifySuccess = false;
			if (sign == digest){//验证相等
				Athena::logger->trace("[人民币商店购买](%s)计算签名相等:",os.str().c_str());
				bVerifySuccess = true;
			}
			else {
				Athena::logger->error("[人民币商店购买](%s)计算出来的签名不相等",os.str().c_str());
				bVerifySuccess = false;
			}
			parser.clearHeader();
			parser.setStatus(true);
			parser.setBody("ok",strlen("ok"));
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
			
			if (bVerifySuccess){
				Global::stWebNotifyRechargeInfo cmd;
				cmd.set_charid(roleID);
				cmd.set_itemid(itemID);
				cmd.set_money(money);
				cmd.set_tid(tid);
				cmd.set_account(account);
				cmd.set_opid(opID);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
				Athena::logger->trace("[人民币商店购买]向global转发了充值信息(account:%s,charID:%d,opID:%d,itemID:%d,money:%d,tid:%s)",account.c_str(),roleID,opID,itemID,money,tid.c_str());
			}
		}
		else if (actTop == "/exchange"){//国内充值接口
            std::vector<std::string> dataVec;
            std::string orderId = parser.getReqParamVal("order");
            dataVec.push_back(orderId);
            std::string tid = parser.getReqParamVal("transaction");
            dataVec.push_back(tid);
            std::string charid = parser.getReqParamVal("character");
            dataVec.push_back(charid);
            std::string serverId = parser.getReqParamVal("server");
            dataVec.push_back(serverId);
            std::string productId = parser.getReqParamVal("product");
            dataVec.push_back(productId);
            std::string money = parser.getReqParamVal("amount");
            dataVec.push_back(money);
            std::string time = parser.getReqParamVal("timestamp");
            dataVec.push_back(time);
            std::string account = parser.getReqParamVal("account");
            dataVec.push_back(account);
            std::string sign = parser.getReqParamVal("sign");
            std::string paychannel = parser.getReqParamVal("paychannel");
			UINT32 paychannelID = (UINT32)atoi(paychannel.c_str());

            UINT32 itemId = CRMBShopCfg::getSingleton().getItemId(productId);
            if (itemId <= 0) {
				std::vector<std::string> thdStrVec;
				g_stringtok(thdStrVec, productId, ".");
				if (thdStrVec.size() > 0){
					itemId = (UINT32)atoi(thdStrVec[thdStrVec.size() - 1].c_str());
				}
				else {
					itemId = 0;
				}
            }
            stOrderInfo orderInfo;//订单详细
            if (itemId <= 0) {
                RoleInfoManager::getSingleton().getOrderInfo((UINT32)atoi(charid.c_str()), orderId, orderInfo);
                itemId = orderInfo.itemId;
            }
            bool validItem = CRMBShopCfg::getSingleton().validItem(itemId);
			std::ostringstream os;
            for (size_t i = 0; i < dataVec.size(); i++) {
                os << dataVec[i];
            }
            os << Athena::global["serverKey"];
			char tokenBuf[1024] = {0};
			int  bufLen = 0;
			snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",os.str().c_str());
			bufLen = strlen(tokenBuf);
			char digest[64] = {0};
			utils::Md5(tokenBuf,bufLen,digest);
			utils::HexString::bin2asc(digest,16,digest,64,bufLen);
			digest[bufLen] = 0;
            std::string subDigest(&digest[3], 28);
			bool bVerifySuccess = false;
            std::stringstream retData;
            Json::FastWriter writer;
            Json::Value root;

			Athena::logger->trace("[CHARLES!!!!productId](%s)",productId.c_str());
			Athena::logger->trace("[CHARLES!!!!itemId](%d)",itemId);
			Athena::logger->trace("[CHARLES!!!!sign](%s)",sign.c_str());
			Athena::logger->trace("[CHARLES!!!!subDigest](%s)",subDigest.c_str());
			Athena::logger->trace("[CHARLES!!!!validItem](%d)",validItem);
			bVerifySuccess = true;
			root["return_code"] = 1;
			root["return_msg"] = std::string("");
			root["extend"] = std::string("");
	/*

		if (sign == subDigest && validItem){//验证相等
				Athena::logger->trace("[人民币商店购买](%s)计算签名相等:",os.str().c_str());
				bVerifySuccess = true;
                root["return_code"] = 1;
                root["return_msg"] = std::string("");
                root["extend"] = std::string("");
                //retData << "{" << '"' << "return_code" << '"' << ":" << (int)1 << "," << '"' << "return_msg" << '"'
                    //<< ":" << '"' << errorInfo << '"' << ',' << '"' << "extend" << '"' << ":" << '"' << '"' << "}";
			}
			else {
				Athena::logger->error("[人民币商店购买](%s)计算出来的签名不相等",os.str().c_str());
				bVerifySuccess = false;
                root["return_code"] = -1;
                if (validItem) {
                    root["return_msg"] = std::string("sign_not_equal");
                } else {
                    root["return_msg"] = std::string("invalid_item_id");
                    Athena::logger->error("[人民币商店购买]invalid_item id is %d", itemId);
                }
                root["extend"] = std::string("");
                //retData << "{" << '"' << "return_code" << '"' << ":" << (int)-1 << "," << '"' << "return_msg" << '"'
                    //<< ":" << '"' << errorInfo << '"' << ',' << '"' << "extend" << '"' << ":" << '"' << '"' << "}";
			}
	*/	
		parser.clearHeader();
			parser.setStatus(true);
            std::string content = writer.write(root); 
            parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","application/json; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
			
			if (bVerifySuccess){
				Global::stWebNotifyRechargeInfo cmd;
				cmd.set_charid((UINT32)atoi(charid.c_str()));
				cmd.set_itemid(itemId);
				cmd.set_money((UINT32)atoi(money.c_str()));
				cmd.set_tid(tid);
				cmd.set_account(account);
				cmd.set_opid(999);//just fill	
				cmd.set_paychannel(paychannelID);
			
#ifdef _YUNDING_TONGJI
				if(orderInfo.isExtend){
					cmd.set_isextend(orderInfo.isExtend);
						
					Json::Value value;
					Json::Reader reader;
					if (reader.parse(orderInfo.data, value)) {

						Json::FastWriter writer;
						value["orderId"] = tid;
						value["serverId"] = serverId;
						std::string content = writer.write(value);
						cmd.set_data(content);
					 	Athena::logger->trace("[支付扩展] content:%s", content.c_str());
					}
				}
#endif
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}

            if (orderInfo.isExtend) {
				/*
                CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
                char buf[1024] = { 0 };
                snprintf(buf, sizeof(buf) - 1, "extend=%s&out_trade_one=%s", orderId.c_str(), tid.c_str());
                pRequest->setUrl("http://game.baozou.com/sdk.php/BaoZou/getPayType");
                pRequest->setRequestData(buf, strlen(buf));
                pRequest->setRequestType(CHttpRequest::kHttpPost);
                std::string configData;
                CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
                if (pRes && pRes->isSucceed()){
                    std::vector<char> &retVec = *pRes->getResponseData();
                    if (retVec.size() > 0){
                        configData.assign(&retVec[0],retVec.size());
                    }
                }
                SAFE_DELETE(pRes);
                Json::Value configJson;
                Json::Reader reader;

                int status = 0;
                std::string payType, sdkType;
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[暴走支付]config data is %s", configData.c_str());
#endif
                if (reader.parse(configData, configJson)) {
                    if (configJson["status"].isInt()){
                        status = configJson["status"].asInt();
                        //1:成功,并返回后面2个字段
                        //-2:失败,并返回msg字段
                        //-3:失败,并返回msg字段
                    }
                    if (configJson["pay_type"].isString()){
                        payType = configJson["pay_type"].asString();
                    }
                    if (configJson["sdk_type"].isString()){
                        sdkType = configJson["sdk_type"].asString();
                    }
                    if (status != 1) {
                        std::string msg;
                        if (configJson["msg"].isString()){
                            msg = configJson["msg"].asString();
                        }
                        Athena::logger->trace("[暴走支付]status is %d, msg is %s", status, msg.c_str());
                    }
                }
                if (status == 1) {
                    std::string opId;
                    std::map<std::string, std::string> contexMap;
                    Json::Value configJson;
                    Json::Reader reader;
                    if (reader.parse(orderInfo.data, configJson)) {
                        if (configJson["deviceid"].isString()){
                            contexMap.insert(std::make_pair("deviceid", configJson["deviceid"].asString()));
                        }
                        if (configJson["idfa"].isString()){
                            contexMap.insert(std::make_pair("idfa", configJson["idfa"].asString()));
                        }
                        if (configJson["idfv"].isString()){
                            contexMap.insert(std::make_pair("idfv", configJson["idfv"].asString()));
                        }
                        if (configJson["imei"].isString()){
                            contexMap.insert(std::make_pair("imei", configJson["imei"].asString()));
                        }
                        if (configJson["opID"].isString()){
                            opId = configJson["opID"].asString();
                        }
                    }
                    //暴走萌将Android/ (4200)
                    //AppKey:725e430b637ca476848b74738431069a
                    //暴走萌将 苹果 (5200)
                    //appkey :410858c7f80e934231ff733a2be4a8e8
                    //暴走萌将3diOS/ (5210)
                    //AppKey:e02cf43a7f508c7f08846357497b4e51
                    std::string appKey;
                    if (opId == "4200") {
                        appKey = "725e430b637ca476848b74738431069a";
                    } else if (opId == "5200") {
                        appKey = "410858c7f80e934231ff733a2be4a8e8";
                    } else if (opId == "5210") {
                        appKey = "e02cf43a7f508c7f08846357497b4e51";
                    }

                    contexMap.insert(std::make_pair("paymenttype", payType));
                    contexMap.insert(std::make_pair("transactionid", orderId));
                    contexMap.insert(std::make_pair("channelid", "_default_"));
                    contexMap.insert(std::make_pair("currencytype", "CNY"));
                    contexMap.insert(std::make_pair("currencyamount", money));
                    for (std::map<std::string, std::string>::iterator it =
                            contexMap.begin(); it != contexMap.end(); it++) {
                        if (it->second.empty()) {
                            it->second = std::string("unknown");
                        }
#ifdef _HDZ_DEBUG
                        Athena::logger->trace("[热云统计]first is %s, second is %s", it->first.c_str(), it->second.c_str());
#endif
                    }

                    Json::FastWriter writer;
                    Json::Value root, contextValue;
                    root["appid"] = appKey;
                    root["who"] = account;
                    for (std::map<std::string, std::string>::iterator it = contexMap.begin();
                            it != contexMap.end(); it++) {
                        contextValue[it->first] = it->second;
                    }
                    root["context"] = contextValue;
                    std::string content = writer.write(root);

                    CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
                    pRequest->setUrl("http://log.reyun.com/receive/track/payment");
                    pRequest->setRequestType(CHttpRequest::kHttpPost);
                    pRequest->setRequestData(content.c_str(),content.length());
                    std::string configData;
                    CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
                    if (pRes && pRes->isSucceed()){
                        std::vector<char> &retVec = *pRes->getResponseData();
                        if (retVec.size() > 0){
                            configData.assign(&retVec[0],retVec.size());
                        }
                    }
                    SAFE_DELETE(pRes);

                    std::string status;
#ifdef _HDZ_DEBUG
                    Athena::logger->trace("[热云统计]config data is %s", configData.c_str());
#endif
                    if (reader.parse(configData, configJson)) {
                        int status = 1;
                        if (configJson["status"].isInt()){
                            status = configJson["status"].asInt();
                        }
                        if (status != 0) {
                            std::string res;
                            if (configJson["result"].isString()){
                                res = configJson["result"].asString();
                            }
                            Athena::logger->error("[热云统计]返回错误信息 %s", res.c_str());
                        } else {
                            Athena::logger->trace("[热云统计]返回成功");
                        }
                    }
                }
			*/
            }
        }
		else if (actTop == "/queryitem")
		{
			UINT32 itemid = (UINT32)atoi(parser.getReqParamVal("itemid").c_str());
			
			stNftItem* itemCfg = NftConfig::getSingleton().getItemCfg(itemid);

			Json::FastWriter writer;
			Json::Value root;

			if (!itemCfg)
			{
				root["code"] = 4;
				root["message"] = "itemid invalid";
			}else{
				root["code"] = 0;
				root["message"] = "";
				root["itemid"] = itemCfg->m_id;
				stItemBase * pItemBase = g_itemBaseMgr.get(itemCfg->m_id);
				if (pItemBase) { 
					root["itemname"] = pItemBase->m_name;
				}
				root["atk"] = itemCfg->m_atk;
				root["hp"] = itemCfg->m_hp;
				root["url"] = itemCfg->m_url;
			}
			
			std::string content = writer.write(root);

			CHttpParser parser;
			parser.setStatus(true);
			parser.setBody(content.c_str(),content.size());
			parser.setHeader("Content-Type","text/html; charset=UTF-8");
			std::string ret = parser.buildResponse();
			WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size()); 
		} 
		else if (actTop == "/querynft"){//查询NFT 
			std::string account = parser.getReqParamVal("userid");
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到请求解析后account is %s", account.c_str());
#endif

            UINT32 charId = 0;
            std::string name; 
            RoleInfoManager::getSingleton().getRole(account, name, charId);

			UINT16 connID = 0;
			if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
				return true;
			}
			WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);

			Global::stWeb2GlobalQueryUserNft cmd;
			cmd.set_charid(charId);
			cmd.set_webconnid(connID);


			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		}
		else if (actTop == "/change_nftitem"){//装备上链or提现
			std::string account = parser.getReqParamVal("userid");
			UINT32 charId = 0;
            std::string name; 
            RoleInfoManager::getSingleton().getRole(account, name, charId);
			UINT32 itemid = (UINT32)atoi(parser.getReqParamVal("itemid").c_str());
			UINT32 count = (UINT32)atoi(parser.getReqParamVal("count").c_str());
			
			UINT16 connID = 0;
			if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
				return true;
			}
			WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);

			Global::stWeb2GlobalChangeNftItem cmd;
			cmd.set_charid(charId);
			cmd.set_webconnid(connID);
			cmd.set_itemid(itemid);
			cmd.set_count(count);


			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		}
		else if (actTop == "/user_cancel"){//回收接口
            std::vector<std::string> dataVec;
            dataVec.push_back(parser.getReqParamVal("player_id"));
            UINT32 roleId = (UINT32)atoi(dataVec.back().c_str());
            dataVec.push_back(parser.getReqParamVal("type"));
            UINT32 type = (UINT32)atoi(dataVec.back().c_str());
            dataVec.push_back(parser.getReqParamVal("timestamp"));
            UINT32 time = (UINT32)atoi(dataVec.back().c_str());
            std::string sign = parser.getReqParamVal("sign");

			Athena::logger->trace("收到请求解析后role id = %d, type = %d, timestamp = %d, sign = %s", roleId, type, time, sign.c_str());

			UINT16 connID = 0;
			if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
				return true;
			}
			WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);

			std::ostringstream os;
            for (size_t i = 0; i < dataVec.size(); i++) {
                os << dataVec[i];
            }
            os << Athena::global["signKey"];
			char tokenBuf[1024] = {0};
			int  bufLen = 0;
			snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",os.str().c_str());
			bufLen = strlen(tokenBuf);
			char digest[64] = {0};
			utils::Md5(tokenBuf,bufLen,digest);
			utils::HexString::bin2asc(digest,16,digest,64,bufLen);
			digest[bufLen] = 0;
            std::string subDigest(&digest[3], 24);
            std::stringstream retData;
			bool bVerifySuccess = false;
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[玩家删除接口]收到的sign = %s, 计算出来的sign = %s, 加密之前的data = %s",
                    sign.c_str(), digest, tokenBuf);
#endif
			if (sign == subDigest){//验证相等
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[玩家删除接口](%s)计算签名相等:",os.str().c_str());
#endif
				bVerifySuccess = true;
            } else {
				Athena::logger->error("[玩家删除接口](%s)计算出来的签名不相等",os.str().c_str());
				bVerifySuccess = false;
            }
			if (bVerifySuccess){
                Game::stUserCancel cmd;
                cmd.set_webconnid(connID);
                cmd.set_charid(roleId);
                cmd.set_type(type);
				GameSvrClientMgr::getSingleton().broadcastProto2All(cmd);
            } else {
                Json::FastWriter writer;
                Json::Value root;
                root["return_code"] = 0;
                root["return_msg"] = std::string("sign_not_equal");
                parser.clearHeader();
                parser.setStatus(true);
                std::string content = writer.write(root); 
                parser.setBody(content.c_str(),content.size());
                parser.setHeader("Content-Type","text/html; charset=UTF-8");
                std::string ret = parser.buildResponse();
                WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
            }
        }
		else if (actTop == "/fetch_user"){//获取指定账户信息
            std::string account = parser.getReqParamVal("account");
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到请求解析后account is %s", account.c_str());
#endif
            parser.clearHeader();

            UINT32 charId = 0;
            std::string name;
            std::stringstream ss;
            RoleInfoManager::getSingleton().getRole(account, name, charId);

            ss << "{'id':'" << charId << "','name':'" << name << "'}";
            parser.setStatus(true);
            std::string content = ss.str();
#ifdef _HDZ_DEBUG
			Athena::logger->trace("fetch_user: content is %s", content.c_str());
#endif
            parser.setBody(content.c_str(),content.size());
            parser.setHeader("Content-Type","text/html; charset=UTF-8");
            std::string ret = parser.buildResponse();
            WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
        }
		else if (actTop == "/useActiveCode"){//转发分享码给指定服务器
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到useActiveCode");
#endif
            std::string userAcc = parser.getReqParamVal("user_account");
            std::string userId = parser.getReqParamVal("user_id");
            std::string inviteCode = parser.getReqParamVal("active_code");
            std::string timeStamp = parser.getReqParamVal("timestamp");
            std::string sign = parser.getReqParamVal("sign");
            parser.clearHeader();
            UINT16 connID = 0;
            if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
                return true;
            }
            WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);

            std::stringstream ss;
            ss << userAcc
                << userId
                << inviteCode
                << timeStamp
                << Athena::global["signKey"];
            char tokenBuf[1024] = { 0 };
            int  bufLen = 0;
            snprintf(tokenBuf, sizeof(tokenBuf) - 1, "%s", ss.str().c_str());
#ifdef _HDZ_DEBUG
            Athena::logger->trace("token buf is %s", tokenBuf);
#endif
            bufLen = strlen(tokenBuf);
            char digest[64] = {0};
            utils::Md5(tokenBuf,bufLen,digest);
            utils::HexString::bin2asc(digest,16,digest,64,bufLen);
            digest[bufLen] = 0;
            std::string subDigest(&digest[3],24);
#ifdef _HDZ_DEBUG
            Athena::logger->trace("subDigest is %s, sign is %s", subDigest.c_str(), sign.c_str());
#endif

            if (subDigest == sign) {
				Global::stUseInviteCode cmd;
				cmd.set_webconnid(connID);
                cmd.set_code(inviteCode);
                cmd.set_account(userAcc);
                cmd.set_userid(userId);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
            } else {
                Json::FastWriter writer;
                Json::Value root;
                root["return_code"] = std::string("1");
                root["return_msg"] = std::string("sign_not_equal");
                root["user_account"] = userAcc;
                root["user_id"] = userId;
                parser.setStatus(true);
                std::string content = writer.write(root); 
                parser.setBody(content.c_str(),content.size());
                parser.setHeader("Content-Type","text/html; charset=UTF-8");
                std::string ret = parser.buildResponse();
                WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
            }
        }
		else if (actTop == "/editLimitPet"){//编辑限时随从活动
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到editLimitPet");
#endif
			std::vector<std::string> strVec;
			g_stringtok(strVec,parser.getPostContent(),"&");
            std::string data;
            UINT32 activity_id;
            std::string timestamp;
            std::string sign;

			for (UINT8 i = 0;i < strVec.size(); ++i){
				std::vector<std::string> subStrVec;
				g_stringtok(subStrVec,strVec[i],"=");
				if (subStrVec.size() != 2){
                    continue ;
				}
                if (subStrVec[0] == "data") {
                    data = HttpUtility::URLDecode(subStrVec[1]);
                } else if (subStrVec[0] == "activity_id") {
                    activity_id = (UINT32)atoi(HttpUtility::URLDecode(subStrVec[1]).c_str());
                } else if (subStrVec[0] == "timestamp") {
                    timestamp = HttpUtility::URLDecode(subStrVec[1]);
                } else if (subStrVec[0] == "sign") {
                    sign = HttpUtility::URLDecode(subStrVec[1]);
                }
			}

            parser.clearHeader();
			Athena::logger->trace("data %s, activity_id %d, timestamp %s, sign %s", 
                    data.c_str(), activity_id, timestamp.c_str(), sign.c_str());
            std::stringstream ss;
            ss << data << activity_id << timestamp << Athena::global["signKey"];
			Athena::logger->trace("data is %s", ss.str().c_str());

			char tokenBuf[1024] = {0};
			int  bufLen = 0;
			snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",ss.str().c_str());
			bufLen = strlen(tokenBuf);
			char digest[64] = {0};
			utils::Md5(tokenBuf,bufLen,digest);
			utils::HexString::bin2asc(digest,16,digest,64,bufLen);
			digest[bufLen] = 0;
            std::string subDigest(&digest[3], 24);
			Athena::logger->trace("disgest is %s, sign is %s", digest, sign.c_str());

            Json::FastWriter writer;
            Json::Value root;
            std::vector<stLimitPet> itemVec;
            if (subDigest != sign) {
                root["return_code"] = std::string("-1");
                root["return_msg"] = std::string("sign_not_equal");
            } else {
                Json::Value configJson;
                Json::Reader reader;

                if (!reader.parse(data, configJson)){
                    root["return_code"] = std::string("-2");
                    root["return_msg"] = std::string("invalid_json");
                } else if (configJson.isArray()) {
                    struct tm _tm;
                    for (UINT32 i = 0; i < configJson.size(); i++) {
                        Json::Value & step = configJson[i];
                        stLimitPet item;
                        item.m_groupId = activity_id;
                        if (step["id"].isString()) {
                            item.m_index = (UINT32)atoi(step["id"].asString().c_str());
                        }
                        if (step["pet_id"].isString()) {
                            item.m_petId = (UINT32)atoi(step["pet_id"].asString().c_str());
                        }
                        if (step["start_time"].isString()) {
                            if (strptime(step["start_time"].asString().c_str(), "%Y-%m-%d %H:%M:%S",&_tm)) {
                                item.m_start = mktime(&_tm);
                            }
                        }
                        if (step["end_time"].isString()) {
                            if (strptime(step["end_time"].asString().c_str(), "%Y-%m-%d %H:%M:%S",&_tm)) {
                                item.m_end = mktime(&_tm);
                            }
                        }
                        itemVec.push_back(item);
                    }
                    if (CLimitPetMgr::getSingleton().check(activity_id, itemVec)) {
                        if (CLimitPetMgr::getSingleton().isUpdate(activity_id)) {
                            CLimitPetMgr::getSingleton().update(activity_id, itemVec);
                        } else {
                            CLimitPetMgr::getSingleton().insert(activity_id, itemVec);
                        }
                        root["return_code"] = std::string("1");
                        root["return_msg"] = std::string("");
                    } else {
                        root["return_code"] = std::string("-3");
                        root["return_msg"] = std::string("invalid data");
                    }
                } else {
                    root["return_code"] = std::string("-2");
                    root["return_msg"] = std::string("invalid_json");
                    Athena::logger->trace("解析json失败");
                }
            }
            parser.setStatus(true);
            std::string content = writer.write(root); 
            Athena::logger->trace("返回content is %s", content.c_str());
            parser.setBody(content.c_str(),content.size());
            parser.setHeader("Content-Type","text/html; charset=UTF-8");
            std::string ret = parser.buildResponse();
            WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
            if (itemVec.size()) {
                CLimitPetMgr::getSingleton().synGlobal();
                CLimitPetMgr::getSingleton().synGame();
                Athena::logger->trace("同步限时随从活动到global和game");
            }
        }
		else if (actTop == "/delLimitPet"){//编辑限时随从活动
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到delLimitPet");
#endif
			std::vector<std::string> strVec;
			g_stringtok(strVec,parser.getPostContent(),"&");
            UINT32 activity_id;
            std::string timestamp;
            std::string sign;

			for (UINT8 i = 0;i < strVec.size(); ++i){
				std::vector<std::string> subStrVec;
				g_stringtok(subStrVec,strVec[i],"=");
				if (subStrVec.size() != 2){
                    continue ;
				}
                if (subStrVec[0] == "activity_id") {
                    activity_id = (UINT32)atoi(HttpUtility::URLDecode(subStrVec[1]).c_str());
                } else if (subStrVec[0] == "timestamp") {
                    timestamp = HttpUtility::URLDecode(subStrVec[1]);
                } else if (subStrVec[0] == "sign") {
                    sign = HttpUtility::URLDecode(subStrVec[1]);
                }
			}

            parser.clearHeader();
			Athena::logger->trace("activity_id %d, timestamp %s, sign %s", 
                    activity_id, timestamp.c_str(), sign.c_str());
            std::stringstream ss;
            ss << activity_id << timestamp << Athena::global["signKey"];
			Athena::logger->trace("data is %s", ss.str().c_str());

			char tokenBuf[1024] = {0};
			int  bufLen = 0;
			snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",ss.str().c_str());
			bufLen = strlen(tokenBuf);
			char digest[64] = {0};
			utils::Md5(tokenBuf,bufLen,digest);
			utils::HexString::bin2asc(digest,16,digest,64,bufLen);
			digest[bufLen] = 0;
            std::string subDigest(&digest[3], 24);
			Athena::logger->trace("disgest is %s, sign is %s", digest, sign.c_str());

            bool success = false;
            Json::FastWriter writer;
            Json::Value root;
            std::vector<stLimitPet> itemVec;
            if (subDigest != sign) {
                root["return_code"] = std::string("-1");
                root["return_msg"] = std::string("sign_not_equal");
            } else {
                success = true;
                CLimitPetMgr::getSingleton().del(activity_id);
                root["return_code"] = std::string("1");
                root["return_msg"] = std::string("");
            }
            parser.setStatus(true);
            std::string content = writer.write(root); 
            Athena::logger->trace("返回content is %s", content.c_str());
            parser.setBody(content.c_str(),content.size());
            parser.setHeader("Content-Type","text/html; charset=UTF-8");
            std::string ret = parser.buildResponse();
            WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
            if (success) {
                CLimitPetMgr::getSingleton().synGlobal();
                CLimitPetMgr::getSingleton().synGame();
                Athena::logger->trace("同步限时随从活动到global和game");
            }
        }
		else {
			std::vector<std::string> strVec;
			g_stringtok(strVec,parser.getPostContent(),"&");
			std::string act;
			std::string data;
			std::string auth;
			std::string timeStr;
			
			for (UINT8 i = 0;i < strVec.size(); ++i){
				std::vector<std::string> subStrVec;
				g_stringtok(subStrVec,strVec[i],"=");
				if (subStrVec.size() >= 2){
					if (0 == i){
						act = subStrVec[1];
					}
					else if (1 == i){
						data = subStrVec[1];
					}
					else if (2 == i){
						auth = subStrVec[1];
					}
					else {
						timeStr = subStrVec[1];
					}
				}
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("收到请求解析后act=%s,data=%s,auth=%s,timeStr=%s",act.c_str(),data.c_str(),auth.c_str(),timeStr.c_str());
#endif
			
			Json::Reader reader;
			Json::Value valuePost;
			Json::Value searchCond;

			std::string unencodeStrSearch = g_decode64(HttpUtility::URLDecode(data).c_str());
#ifdef _HDZ_DEBUG
			Athena::logger->trace("decode64后data:%s",unencodeStrSearch.c_str());
#endif
            std::string calcMd5Str;
            if (Athena::global["isNotUuzu"] == std::string("true")) {
                calcMd5Str = unencodeStrSearch + Athena::global["serverKey"];//上线后才有的key
            } else {
                calcMd5Str = unencodeStrSearch + "GMXGAME@#*UUZU2014";//unencodeStrSearch.substr(5,20);
            }
			//calcMd5Str += "GMXGAME@#*UUZU2014";
			char digest[64] = {0};
			int  bufLen = 0;
			utils::Md5(calcMd5Str.c_str(),calcMd5Str.size(),digest);
			utils::HexString::bin2asc(digest,16,digest,64,bufLen);
			digest[bufLen] = 0;
			std::string subDigest(&digest[5],20);
			if (auth != subDigest){
				Athena::logger->trace("验证码不正确!,发过来的是:%s,计算出来的是:%s",auth.c_str(),subDigest.c_str());
				return true;
			}

			if (!reader.parse(unencodeStrSearch,searchCond)){
				Athena::logger->error("json解析失败!");
				return true;
			}	
#ifdef _HDZ_DEBUG
			Athena::logger->trace("解析后的data:%s",unencodeStrSearch.c_str());
#endif

			if (act == "getUser"){
#ifdef _HDZ_DEBUG
				Athena::logger->trace("执行GetUser");
#endif
				parser.clearHeader();
				std::string content = RoleInfoManager::getSingleton().getOnePageUser(searchCond);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
#ifdef _HDZ_DEBUG
				Athena::logger->trace("给玩家返回了数据:%s",ret.c_str());
#endif
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
			}
			else if (act == "getUserInfo"){
				//searchCond["player_id"] = 8;
				parser.clearHeader();
				std::string content = RoleInfoManager::getSingleton().getUserDetailInfo(searchCond);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("给玩家返回了数据:%s",ret.c_str());
#endif
			}
			else if (act == "getEditUserInfo"){
				parser.clearHeader();
				std::string content = RoleInfoManager::getSingleton().getEditUserInfo(searchCond);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("给玩家返回了数据:%s",ret.c_str());
#endif
			}
			else if (act == "editUser"){
				parser.clearHeader();
				UINT16 connID = 0;
				if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
					return true;
				}
				WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);

				UINT32 charID = 0, goldCoin = 0, diamond = 0, vipPoint = 0;
				UINT16 newbieGuideID = 0;
				Json::Value playerObj;
				if (searchCond["player"].isObject()){
					playerObj = searchCond["player"];

					if (playerObj["player_id"].isString()){
						charID = (UINT32)atoi(playerObj["player_id"].asString().c_str());
					}

					if (playerObj["user_guideID"].isString()){
						newbieGuideID = (UINT16)atoi(playerObj["user_guideID"].asString().c_str());
					}

					if (playerObj["coin"].isString()){
						goldCoin = (UINT32)atoi(playerObj["coin"].asString().c_str());
					}

					if (playerObj["diamond"].isString()){
						diamond = (UINT32)atoi(playerObj["diamond"].asString().c_str());
					}

					if (playerObj["vip"].isString()){
						vipPoint = (UINT32)atoi(playerObj["vip"].asString().c_str());
					}
				}

				Global::stWebMotifyUserInfo  cmd;
				cmd.set_webconnid(connID);
				cmd.set_charid(charID);	
				cmd.set_newbieguideid(newbieGuideID);
				cmd.set_goldcoin(goldCoin);	
				cmd.set_diamond(diamond);	
				cmd.set_vippoint(vipPoint);	
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			else if (act == "getUserDetailList"){
				parser.clearHeader();
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				Json::Value data;
				Json::Value listData;
				Json::Value one;
				one["id"] = 1;
				one["title"] = "金币";
				listData.append(one);
				data["list"] = listData;
				root["data"] = data;
				std::string content = writer.write(root);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("给玩家返回了数据:%s",ret.c_str());
#endif
			}
			else if (act == "getUserDetail"){
				parser.clearHeader();
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				Json::Value data;
				Json::Value fields;
				Json::Value listData;

				data["total"] = 1;
				fields["goldCoin"] = "金币数量";
				listData["goldCoin"] = 1;
				data["field"] = fields;
				data["list"] = listData;
				root["data"] = data;
				std::string content = writer.write(root);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("给玩家返回了数据:%s",ret.c_str());
#endif
			}
			else if (act == "getUserLogList"){
				parser.clearHeader();
				std::string content = UserLogMgr::getSingleton().getUserLogLists();
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("给玩家返回了数据:%s",ret.c_str());
#endif
			}
			else if (act == "getUserLog"){
				parser.clearHeader();
				std::string content = UserLogMgr::getSingleton().getUserLogByID(searchCond);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getUserLog返回给玩家的数据:%s",ret.c_str());
#endif
			}
			else if (act == "addGoods"){//发放资源
				parser.clearHeader();
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				Json::Value data;

				data.append(1);
				data.append(2);

				root["data"] = data;
				std::string content = writer.write(root);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
			}
			else if (act == "delGoods"){//扣除资源
				parser.clearHeader();
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				Json::Value data;

				data.append(1);
				data.append(2);

				root["data"] = data;
				std::string json_file = writer.write(root);
				g_webServer->getConnMgr().broadcastByComponent<int>(pConn,json_file.c_str(),json_file.size());
			}
			else if (act == "sendMail"){//发送邮件
				parser.clearHeader();
				UINT16 connID = 0;
				if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
					return true;
				}
				WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);
				Global::stSendMail2User cmd;
				std::string title;
				std::string content;
				std::string tag;
				std::string goodsList;
				std::string itemList;
				std::string userList;
				std::string userTypeStr;

				if (searchCond["title"].isString()){
					title = searchCond["title"].asString();
				}
				
				if (searchCond["content"].isString()){
					content = searchCond["content"].asString();
				}
				
				if (searchCond["tag"].isString()){
					tag = searchCond["tag"].asString();
				}

				if (searchCond["goods_list"].isString()){
					goodsList = searchCond["goods_list"].asString();
				}
				
				if (searchCond["item_list"].isString()){
					itemList = searchCond["item_list"].asString();
				}

				if (searchCond["type"].isString()){
					userTypeStr = searchCond["type"].asString();
				}
					
				if (searchCond["user_list"].isString()){
					userList = searchCond["user_list"].asString();
				}
				

				cmd.set_title(title);
				cmd.set_content(content);
				cmd.set_webconnid(connID);
				std::vector<std::string> strVec;
				
				g_stringtok(strVec,goodsList,";");
				for (UINT32 i = 0;i < strVec.size();++i){
					std::vector<std::string> subStrVec;
					g_stringtok(subStrVec,strVec[i],":");
					if (subStrVec.size() >= 2) {
						Global::stSendMailAttach * pSyn = cmd.add_attaches();
						if (pSyn){
							pSyn->set_itemid((UINT32)atoi(subStrVec[0].c_str()));
							pSyn->set_num((UINT32)atoi(subStrVec[1].c_str()));
						}
					}
				}
				
				strVec.clear();
				g_stringtok(strVec,itemList,";");
				for (UINT32 i = 0;i < strVec.size();++i){
					std::vector<std::string> subStrVec;
					g_stringtok(subStrVec,strVec[i],":");
					if (subStrVec.size() >= 2) {
						Global::stSendMailAttach * pSyn = cmd.add_attaches();
						if (pSyn){
							pSyn->set_itemid((UINT32)atoi(subStrVec[0].c_str()));
							pSyn->set_num((UINT32)atoi(subStrVec[1].c_str()));
						}
					}
				}

				strVec.clear();
				g_stringtok(strVec,userList,",");
				for (UINT32 i = 0;i < strVec.size();++i){
					UINT32 charID = (UINT32)atoi(strVec[i].c_str());
					cmd.add_userlist(charID);
				}
				cmd.set_tag(tag);
				if (userTypeStr == "server_id"){
					cmd.set_ball(true);	
				}
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

				/*Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				Json::Value data;
				root["data"] = data;
				root["tag"] = tag;
				std::string json_file = writer.write(root);
				parser.setStatus(true);
				parser.setBody(json_file.c_str(),json_file.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				g_webServer->getConnMgr().broadcastByComponent<int>(pConn,ret.c_str(),ret.size());*/
#ifdef _HDZ_DEBUG
				//Athena::logger->trace("发送邮件返回了:%s",ret.c_str());
#endif
			}
			else if (act == "kickUser"){//踢玩家
				parser.clearHeader();
				UINT16 connID = 0;
				if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
					return true;
				}
				WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);
				UINT32 charID = 0;
				if (searchCond["player_id"].isString()){
					charID = (UINT32)atoi(searchCond["player_id"].asString().c_str());
				}

				Global::stReqKillUserOut cmd;
				cmd.set_charid(charID);
				cmd.set_webconnid(connID);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			else if (act == "getNoticePosition"){//获得公告位置
				parser.clearHeader();
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				Json::Value data;
				Json::Value posItemList;
				Json::Value posItem;

				posItem["id"] = "1";
				posItem["name"] = "登录界面";
				posItem["desc"] = "玩家登录时候显示的公告";
				posItemList.append(posItem);
				
				posItem["id"] = "2";
				posItem["name"] = "跑马灯";
				posItem["desc"] = "在主界面以跑马灯形式显示";
				posItemList.append(posItem);
				
				posItem["id"] = "3";
				posItem["name"] = "弹出对话框";
				posItem["desc"] = "弹出对话框在最顶层";
				posItemList.append(posItem);

				data["list"] = posItemList;

				root["data"] = data;
				std::string content = writer.write(root);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
			}
			else if (act == "notice"){//发布公告
				parser.clearHeader();
				std::string action;
				UINT32 noticeID = 0;
				UINT32 interval = 0;
				UINT32 num = 1;
				UINT16 noticePos = 0;
				UINT32 beginTime = 0;
				UINT32 endTime = 0;

				std::string content;
				
				if (searchCond["action"].isString()){
					action = searchCond["action"].asString();
				}

				if (searchCond["tid"].isString()){
					noticeID = (UINT32)atoi(searchCond["tid"].asString().c_str());
				}
	
				if (searchCond["exec_time"].isString()){
					interval = (UINT32)atoi(searchCond["exec_time"].asString().c_str()) * 60;
				}
				
				if (searchCond["text"].isString()){
					content = searchCond["text"].asString();
				}
				
				if (searchCond["notice_position"].isString()){
					noticePos = (UINT16)atoi(searchCond["notice_position"].asString().c_str());
				}
				
				if (searchCond["begin_time"].isString()){
					beginTime = (UINT32)atoi(searchCond["begin_time"].asString().c_str());
				}
				
				if (searchCond["end_time"].isString()){
					endTime = (UINT32)atoi(searchCond["end_time"].asString().c_str());
				}
				
				Json::FastWriter writer;
				Json::Value root;
				if ((endTime <= beginTime) || (interval <= 0)){
					root["status"] = 0;
					root["data"] = "配置错误不能发送!";
					std::string json_content = writer.write(root);
					parser.setStatus(true);
					parser.setBody(json_content.c_str(),json_content.size());
					parser.setHeader("Content-Type","text/html; charset=UTF-8");
					std::string ret = parser.buildResponse();
					WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
					return true;		
				}
				
				num = (endTime - beginTime) / interval;

				root["status"] = 1;
				root["data"] = "成功!";
				std::string json_content = writer.write(root);
				parser.setStatus(true);
				parser.setBody(json_content.c_str(),json_content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());

				Global::stAddOrUpdateNotice cmd;
				if (action == "insert"){
					cmd.set_op(Global::eInsert_Notice);
				}
				else {
					cmd.set_op(Global::eUpdate_Notice);
				}
			
				cmd.set_num(num);
				cmd.set_interval(interval);
				cmd.set_content(content);
				cmd.set_noticeid(noticeID);
				cmd.set_noticeposition(noticePos);
				cmd.set_begintime(beginTime);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			else if (act == "delNotice"){ //删除公告
				parser.clearHeader();
				UINT32 noticeID = 0;
				if (searchCond["notice_id"].isString()){
					noticeID = (UINT32)atoi(searchCond["notice_id"].asString().c_str());
				}

				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				Json::Value data;
				root["data"] = "成功!";
				std::string content = writer.write(root);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
				Global::stDelNotice cmd;
				cmd.set_noticeid(noticeID);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			else if (act == "banUser"){//禁用登录	
				parser.clearHeader();
				UINT16 connID = 0;
				if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
					return true;
				}
				WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);
				UINT32 charID = 0;
				UINT8  banUser = false;
				if (searchCond["player_id"].isString()){
					charID = (UINT32)atoi(searchCond["player_id"].asString().c_str());
				}
				
				if (searchCond["is_ban"].isString()){
					UINT8 iBan = (UINT8)atoi(searchCond["is_ban"].asString().c_str());
					if (iBan){
						banUser = true;
					}
				}

				Global::stReqBanUser cmd;
				cmd.set_charid(charID);
				cmd.set_webconnid(connID);
				cmd.set_ban(banUser);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			else if (act == "setGM"){//设置GM
				parser.clearHeader();
				UINT16 connID = 0;
				if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
					return true;
				}
				WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);
				UINT32 charID = 0;
				UINT8  flag = 0;
				if (searchCond["player_id"].isString()){
					charID = (UINT32)atoi(searchCond["player_id"].asString().c_str());
				}
				
				if (searchCond["flag"].isString()){
					flag = (UINT8)atoi(searchCond["flag"].asString().c_str());
				}

				Global::stReqSetUserGMType cmd;
				cmd.set_charid(charID);
				cmd.set_webconnid(connID);
				cmd.set_gmvalue(flag);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			else if (act == "getResource"){//获得资源
				parser.clearHeader();
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				Json::Value data;

				Json::Value resources;
				Json::Value items;
				Json::Value params;

				stGetItemInfoCB cb(resources,items);
				g_itemBaseMgr.execEveryOne(cb);
				data["resource"] = resources;
				data["item"] = items;

				Json::Value param;
				param["id"] = 1;
				param["name"] = "强化等级";
				param["desc"] = "道具的强化等级";
				params.append(param);
				data["params"] = params;

				root["data"] = data;
				std::string content = writer.write(root);
				//std::string contentUrl = HttpUtility::URLEncode(content);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getResource 接口返回数据长度:%d",ret.size());
				Athena::logger->trace("getResource 接口返回%s,数据长度:%d",ret.c_str(),ret.size());
#endif
			}
			else if (act == "chargeLog"){//充值日志
				parser.clearHeader();
				//Json::Value searchCond;
				std::string content = UserLogMgr::getSingleton().getRechargeLog(searchCond);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("chargLog返回%s",ret.c_str());
#endif
			}
			else if (act == "chargeRank"){//充值排名
				parser.clearHeader();
				//Json::Value searchCond;
				std::string content = RoleInfoManager::getSingleton().rechargeRank(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("chargeRank充值排名返回:%s",ret.c_str());
#endif
			}
			else if (act == "getAdInfo"){//获得广告用户
				//Json::Value searchCond;
				parser.clearHeader();
				std::string content = RoleInfoManager::getSingleton().getAdvUsers(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("获得广告用户getAdInfo返回:%s",ret.c_str());
#endif
			}
			else if (act == "mallItem"){//获得商城购买统计
				//Json::Value searchCond;
				parser.clearHeader();
				std::string content = CGameStaticsMgr::getSingleton().getMallBuyInfo(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_BUG
				Athena::logger->trace("mallItem返回:%s",ret.c_str());
#endif
			}
			else if (act == "getOnline"){//获得在线信息
				//Json::Value searchCond;
				parser.clearHeader();
				std::string content = CGameStaticsMgr::getSingleton().getOnlineInfo(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getOnline返回:%s",ret.c_str());
#endif
			}
			else if (act == "getLevelInfo"){//获得等级分布,已经测试
				//Json::Value searchCond;
				parser.clearHeader();
				std::string content = RoleInfoManager::getSingleton().getLevelInfo(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getLevelInfo返回值:%s",ret.c_str());
#endif
			}
			else if (act == "getLevelInfo2"){//获得等级分布
				parser.clearHeader();
				std::string content = RoleInfoManager::getSingleton().getLevelInfo2(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getLevelInfo2返回值:%s",ret.c_str());
#endif
			}
			else if (act == "getExchangList"){//查询资源消耗
				parser.clearHeader();
				std::string content = CGameStaticsMgr::getSingleton().getExchangeList(searchCond);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("GetExchangList返回值:%s",ret.c_str());
#endif
			}
			else if (act == "getVocations"){//查询职业分布,已经测试
				//Json::Value searchCond;
				parser.clearHeader();
				std::string content = RoleInfoManager::getSingleton().getJobStatics(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("GetVocations查询职业分布:%s",ret.c_str());
#endif
			}
			else if (act == "getHaveInfo"){//查询留存
				parser.clearHeader();
				std::string content = CGameStaticsMgr::getSingleton().getHavaInfo(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getHaveInfo返回值:%s",ret.c_str());
#endif
			}
			else if (act == "getServerInfo"){
				parser.clearHeader();
				std::string content = CGameStaticsMgr::getSingleton().getServerInfo(searchCond);
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getServerInfo返回给玩家的数据:%s",ret.c_str());
#endif
			}
			else if (act == "getPendectAll"){//获得充值总览
				parser.clearHeader();
				std::string content = CGameStaticsMgr::getSingleton().getRechargeInfo(searchCond);;
				parser.setStatus(true);
				parser.setBody(content.c_str(),content.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getPendectAll查询充值总览:%s",ret.c_str());
#endif
			}
			else if (act == "publishPluginData"){
				parser.clearHeader();
				UINT16 connID = 0;
				if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
					return true;
				}
				
				Game::stPublishActivity cmd;
				cmd.set_webconnid(connID);
				
				UINT32 id = 0;
				if (searchCond["id"].isString()){
					id = (UINT32)atoi(searchCond["id"].asString().c_str());
				}
				cmd.set_id(id);
				cmd.set_config(unencodeStrSearch);
				CActivityConfigMgr::getSingleton().save(id,unencodeStrSearch);
				WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);
				GameSvrClientMgr::getSingleton().broadcastProto2All(cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("publishPluginData发布插件返回");
#endif
			}
			else if (act == "delPluginData"){
				parser.clearHeader();
				UINT16 connID = 0;
				if (!WebUserConnMgr::getSingleton().genUniqueID(connID)){
					return true;
				}
				
				Game::stUnloadActivity cmd;
				cmd.set_webconnid(connID);
				UINT32 id = 0;
				if (searchCond["id"].isString()){
					id = (UINT32)atoi(searchCond["id"].asString().c_str());
				}
			 	cmd.set_id(id);
				WebUserConnMgr::getSingleton().addSessionComponent(connID,pConn);
				CActivityConfigMgr::getSingleton().del(id);
				GameSvrClientMgr::getSingleton().broadcastProto2All(cmd);
#ifdef _HDZ_DEBUG
				Athena::logger->trace("delPluginDat下架插件返回");
#endif
			}
			else if (act == "addGiftBag"){
				parser.clearHeader();
				std::string goodsList;
				std::string itemList;
				UINT32 id = 0;
				std::string config;

				if (searchCond["id"].isString()){
					id = (UINT32)atoi(searchCond["id"].asString().c_str());
				}

				if (searchCond["good_list"].isString()){
					goodsList = searchCond["good_list"].asString();
				}
				
				if (searchCond["item_list"].isString()){
					itemList = searchCond["item_list"].asString();
				}

				if (goodsList.size() > 0){
					config += goodsList;
				}

				if (itemList.size() > 0){
					config += ";";
					config += itemList;
				}
#ifdef _HDZ_DEBUG
				Athena::logger->trace("发送的giftBag 配置:id:%d,content:%s",id,config.c_str());
#endif
				Global::stAddGiftBagCfg cmd;
				cmd.set_id(id);
				cmd.set_content(config);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				root["data"] = "成功";
				std::string json_file = writer.write(root);
				parser.setStatus(true);
				parser.setBody(json_file.c_str(),json_file.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("addGiftBag回复的内容为:%s",ret.c_str());
#endif
			}
			else if (act == "delGiftBag"){
				parser.clearHeader();
				UINT32 id = 0;
				if (searchCond["id"].isString()){
					id = (UINT32)atoi(searchCond["id"].asString().c_str());
				}

				Global::stDelGiftBagCfg cmd;
				cmd.set_id(id);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
				
				Json::FastWriter writer;
				Json::Value root;
				root["status"] = 1;
				root["data"] = "成功";
				std::string json_file = writer.write(root);
				parser.setStatus(true);
				parser.setBody(json_file.c_str(),json_file.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("delGiftBag回复的内容为:%s",ret.c_str());
#endif
			}
			else if (act == "getRealTimeRecharge"){
				parser.clearHeader();
				string json_file = CGameStaticsMgr::getSingleton().getCommonQuery(searchCond);
				parser.setStatus(true);
				parser.setBody(json_file.c_str(),json_file.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("getRealTimeRecharge回复的内容为:%s",ret.c_str());
#endif
			}
			else if (act == "activityLog"){
				parser.clearHeader();
				string json_file = CGameStaticsMgr::getSingleton().getActivityLog(searchCond);
				parser.setStatus(true);
				parser.setBody(json_file.c_str(),json_file.size());
				parser.setHeader("Content-Type","text/html; charset=UTF-8");
				std::string ret = parser.buildResponse();
				WebUserConnMgr::getSingleton().sendRawData(pConn,ret.c_str(),ret.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("activityLog回复的内容为:%s",ret.c_str());
#endif
			}
		}
		return true;
	}
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
	
	if (processHttpMsg(pConn,header,pNull)){
		return true;
	}

	ProtoMsgHandleBase * pModHandle = m_protoModHandles[header->m_modID];
	if (pModHandle) {
		if (pModHandle->handle(pConn,(char *)pNull,header->m_dataLen,header->m_funID)){
			return true;
		}
	}
	Athena::logger->error("%s (%u, %u, %u)", __PRETTY_FUNCTION__, header->m_modID, header->m_funID,header->m_dataLen);
	return false;
}
