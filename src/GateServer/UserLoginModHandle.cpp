#include "UserLoginModHandle.h"
#include "SvrInfo.h"
#include "GateServer.h"
#include "GateUserConnMgr.h"
#include "GateUserConnMgr.h"
#include "md5.h"
#include "GateUserManager.h"
#include "SelectUserManager.h"
#include "Global.pb.h"
#include "GlobalSvrClientMgr.h"
#include "UtilFunction.h"
#include "HttpParser.h"
#include "HttpClient.h"
#include "json/writer.h"
#include "json/reader.h"
#include "Role.pb.h"
#include "GameVersionCfg.h"

extern GateServer * g_gateServer;

UserLoginModHandle::UserLoginModHandle()
{
	registerHandle(&UserLoginModHandle::userReqLogin);
	registerHandle(&UserLoginModHandle::reqSvrTime);
	registerHandle(&UserLoginModHandle::autoConnectGate);
	registerHandle(&UserLoginModHandle::queryPingValue);
	registerHandle(&UserLoginModHandle::recClientTime);
}

UserLoginModHandle::~UserLoginModHandle()
{

}




bool UserLoginModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<UserLoginModHandle>::handle(pCom,pData,cmdLen,funID); 
}

bool UserLoginModHandle::formatVerifyToken(const char * loginToken,stLoginTokenInfo & one)
{
	std::string unencodeToken = g_decode64(loginToken);
	Athena::logger->trace("[登录验证]decode64后的值:%s",unencodeToken.c_str());

	//检测字符串中出现乱码直接返回,即不存在下列等必要字段
	if((unencodeToken.find("osdk_user_id") == std::string::npos)
			|| (unencodeToken.find("sign") == std::string::npos)){
		Athena::logger->error("[登录验证](loginToken:%s)解析可能出现乱码，拒绝登录", loginToken);
		return false;
	}

	Json::Reader reader;
	Json::Value value;
	std::map<std::string,std::string> kvSortMap;
	if (reader.parse(unencodeToken, value)){
		if (value["osdk_game_id"].isString()){
			kvSortMap["osdk_game_id"] = value["osdk_game_id"].asString();
		}
		
		std::string user_id;
		if (value["user_id"].isString()){
			user_id = value["user_id"].asString();
			kvSortMap["user_id"] = user_id;
		}
		else if (value["user_id"].isInt()){
			kvSortMap["user_id"] = g_int2string(value["user_id"].asInt());
		}

		if (value["account_system_id"].isString()){
			kvSortMap["account_system_id"] = value["account_system_id"].asString();
		}

		if (value["login_sdk_name"].isString()){
			kvSortMap["login_sdk_name"] = value["login_sdk_name"].asString();
		}
	
		if (value["channel_id"].isString()){
			kvSortMap["channel_id"] = value["channel_id"].asString();		
			one.m_channelID = kvSortMap["channel_id"];
		}
		else if (value["channel_id"].isInt()){
			kvSortMap["channel_id"] = g_int2string(value["channel_id"].asInt());
			one.m_channelID = kvSortMap["channel_id"];
		}
		else if (value["channel_id"].isUInt()){
			kvSortMap["channel_id"] = g_int2string(value["channel_id"].asUInt());
			one.m_channelID = kvSortMap["channel_id"];
		}
		
		if (value["extend"].isString()){
			kvSortMap["extend"] = value["extend"].asString();
		}
		
		std::string osdk_user_id;

		if (value["osdk_user_id"].isString()){
			osdk_user_id = value["osdk_user_id"].asString();
			kvSortMap["osdk_user_id"] = osdk_user_id;
		}
		
		if (value["ip"].isString()){
			kvSortMap["ip"] = value["ip"].asString();
			one.m_ip = value["ip"].asString();	
		}
		
		if (value["time"].isInt()){
			INT32 timeInt = value["time"].asInt();
			char  szTime[20] = {0};
			snprintf(szTime,sizeof(szTime),"%d",timeInt);
			kvSortMap["time"] = szTime;
		}
		
		std::string sign;
		if (value["sign"].isString()){
			sign = value["sign"].asString();
		}
		
		/*if (value.isMember("return_code") && value["return_code"].isInt()){
			INT32  retCode = value["return_code"].asInt();
			kvSortMap["return_code"] = 	g_int2string(retCode);
		}
		
		if (value.isMember("return_msg") && value["return_msg"].isString()){
			kvSortMap["return_msg"] = value["return_msg"].asString();
		}*/

		std::map<std::string,std::string>::iterator it = kvSortMap.begin();
		bool first = true;
		std::ostringstream os ;

		for (;it != kvSortMap.end();++it){
			if (first){
				os << it->first <<"="<< it->second;
				first = false;
			}		
			else {
				os <<"&"<<it->first<<"="<<it->second;
			}
		}
		//os << "aDfbx4NSRprgEKpPY7RwTC4XI7OzQRHC";
        if (Athena::global["isNotUuzu"] == std::string("true")) {
            os << Athena::global["server_secret"];
        }else{
		    os << SERVER_SECRET;
        }
		char tokenBuf[1024] = {0};
		int  bufLen = 0;
		snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",os.str().c_str());
		Athena::logger->trace("用来计算token为:%s",tokenBuf);
		bufLen = strlen(tokenBuf);
		char digest[64] = {0};
		utils::Md5(tokenBuf,bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		if (sign == digest){
			Athena::logger->trace("用新的验证算法验证成功!");
			strncpy(one.m_account,osdk_user_id.c_str(),sizeof(one.m_account));
			//one.m_accountID = (UINT32)atoi(user_id.c_str());
			strncpy(one.m_loginToken,sign.c_str(),sizeof(one.m_loginToken));
			return true;
		}
		else {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("验证token不等,计算出来的为:%s,发送过来的为:%s",digest,sign.c_str());
#endif
		}
	}
	else {
		Athena::logger->error("登录失败,解析json不成功!");
	}
	return false;
}

bool UserLoginModHandle::verifyToken(const char * loginToken,stLoginTokenInfo & one)
{
	
	std::vector<std::string> strVec;
	g_stringtok(strVec,loginToken,",");

	if (strVec.size() < 4){
		Athena::logger->trace("[登录验证]发送过来的token有误!account=%s",loginToken);
		return false;
	}

	static std::string  key = "72abf593384ba09ae41ac8ad6b435d4a";
	
	char tokenBuf[1024] = {0};
	int  bufLen = 0;
	snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s%s%s%s",strVec[0].c_str(),strVec[1].c_str(),strVec[2].c_str(),key.c_str());
	bufLen = strlen(tokenBuf);
	char digest[64] = {0};
	utils::Md5(tokenBuf,bufLen,digest);
	utils::HexString::bin2asc(digest,16,digest,64,bufLen);
	digest[bufLen] = 0;
#ifdef _HDZ_DEBUG
	Athena::logger->trace("网关计算出来的签名是%s",digest);
#endif
	if (0 != strncmp(strVec[3].c_str(),digest,sizeof(digest))){
		Athena::logger->error("[登录验证]验证token失败,account=%s",strVec[0].c_str());
		return false;
	}
	
	strncpy(one.m_account,strVec[1].c_str(),sizeof(one.m_account));
	//one.m_accountID = (UINT32)atoi(strVec[0].c_str());
	strncpy(one.m_loginToken,strVec[3].c_str(),sizeof(one.m_loginToken));
	return true;
}

bool UserLoginModHandle::verifySdk(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());
    std::string appId;
    std::string appKey;
    if (sdkId == _11Wan) {
        appId = std::string("1000014");
        appKey = std::string("ce960562cbee7217a69745907d0b3766");
    } else if (sdkId == MoreFun) {
        appId = std::string("e9gzoc");
        appKey = std::string("f8442efd43b346108f35962bc645a5d5");
    } else if (sdkId == _Easy) {
        ;
    }

	Json::Reader reader;
	Json::Value value;
	std::list<std::string> strList;
	if (reader.parse(data, value)){
		if (value["appId"].isString()){
			strList.push_back(value["appId"].asString());
		}
		if (value["channelId"].isString()){
			strList.push_back(value["channelId"].asString());
		}
        std::string sign, uid;
		if (value["uid"].isString()){
            uid = value["uid"].asString();
			strList.push_back(uid);
		}
		if (value["token"].isString()){
			strList.push_back(value["token"].asString());
		}
		if (value["timestamp"].isString()){
			strList.push_back(value["timestamp"].asString());
		}
		if (value["sign"].isString()){
            sign = value["sign"].asString();
		}
        //新增一个协议 里面包含是否是新建账号和account
        if (!appId.empty()) {
            strList.push_back(appId);
        }

		std::ostringstream os ;
        std::list<std::string>::iterator it = strList.begin();
        if (it != strList.end()) {
            os << *it;
        }
        for (it++; it != strList.end(); it++) {
            os << "&" << *it;
        }
        if (!appKey.empty()) {
            os << "&" << appKey;
        }
		char tokenBuf[1024] = {0};
		int  bufLen = 0;
		snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",os.str().c_str());
		Athena::logger->trace("用来计算token为:%s",tokenBuf);
		bufLen = strlen(tokenBuf);
		char digest[64] = {0};
		utils::Md5(tokenBuf,bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		if (sign.length() && sign == digest){
			Athena::logger->trace("sdk 用新的验证算法验证成功!");
			strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
			//one.m_accountID = (UINT32)atoi(user_id.c_str());
			strncpy(one.m_loginToken, sign.c_str(), sizeof(one.m_loginToken));
            
            Global::stAskIsNewAccount cmd;
            cmd.set_account(uid);
            GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			return true;
		}
		else {
#ifdef _HDZ_DEBUG
			Athena::logger->trace("验证token不等,计算出来的为:%s,发送过来的为:%s",digest,sign.c_str());
#endif
		}
    }
	else {
		Athena::logger->error("登录失败,解析json不成功!");
	}
	return false;
}

bool UserLoginModHandle::verifyMoeYoo(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	Json::Reader reader;
	Json::Value value;
	std::list<std::string> strList;
	if (reader.parse(data, value)){
        std::string uid, token;
		if (value["uid"].isString()){
            uid = value["uid"].asString();
		}
		if (value["token"].isString()){
            token = value["token"].asString();
		}
        if (uid.empty() || token.empty()) {
            Athena::logger->trace("魔域渠道收到用户名或token为空, 验证失败");
            return false;
        }

        CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        std::string url("https://api.moeyooo.com/api/token/");
        url += token;
        pRequest->setUrl(url.c_str());
        std::vector<std::string> headerVec;
        headerVec.push_back("Content-Type: application/json");
        pRequest->setHeaders(headerVec);
        pRequest->setRequestData(data.c_str(), data.size());
        pRequest->setRequestType(CHttpRequest::kHttpGet);
#ifdef _HDZ_DEBUG
        Athena::logger->trace("发送过去的数据是:%s",data.c_str());
#endif
        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
            SAFE_DELETE(pRes);
            Json::Value configJson;
            Json::Reader reader;

            int code = 0;
            std::string msg;
            bool suc = false, body = false;
            if (reader.parse(configData, configJson)) {
                if (configJson["suc"].isBool()){
                    suc = configJson["suc"].asBool();
                }
                if (configJson["body"].isBool()){
                    body = configJson["body"].asBool();
                }
                if (configJson["code"].isInt()){
                    code = configJson["code"].asInt();
                }
                if (configJson["msg"].isString()){
                    msg = configJson["msg"].asString();
                }
            }
            if (suc) {
                Athena::logger->trace("魔域验证通过");
                strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
                //one.m_accountID = (UINT32)atoi(user_id.c_str());
                strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(uid);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
            Athena::logger->trace("魔域验证失败, token is %s, code is %d, msg is %s",
                    (token == std::string("true") ? "valid" : "invalid"), code, msg.c_str());
        } else {
            Athena::logger->trace("魔域验证失败, http请求失败");
        }
    }
	else {
		Athena::logger->error("登录失败,解析json不成功!");
	}
	return false;
}

bool UserLoginModHandle::verifyStarpy(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());
	Json::Reader reader;
	Json::Value value;                                                        
	std::list<std::string> strList;                                           
	if (reader.parse(data, value)){
		std::string loginKey = "7EE31E3D98F020E07E7518C8B9978732";
		std::string userId, timestamp, accessToken;
		if (value["userId"].isString()){
			userId = value["userId"].asString();
		}
		if(value["timestamp"].isString()){
			timestamp = value["timestamp"].asString();
		}
		if(value["accessToken"].isString()){
			accessToken = value["accessToken"].asString();
		}
		if (loginKey.empty() || userId.empty() || timestamp.empty() || accessToken.empty())
		{
			Athena::logger->trace("星彼渠道收到用户名或参数为空, 验证失败");
			return false;
		}
		
		std::string str = loginKey + userId + timestamp;
		int  bufLen = strlen(str.c_str());
		char digest[64] = {0};
		utils::Md5(str.c_str(),bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string MD5String = digest;
		std::transform(MD5String.begin(), MD5String.end(), MD5String.begin(), ::tolower);

		if(MD5String == accessToken){
			strncpy(one.m_account, userId.c_str(), sizeof(one.m_account));
			//one.m_accountID = (UINT32)atoi(user_id.c_str());
			strncpy(one.m_loginToken, MD5String.c_str(), sizeof(one.m_loginToken));

			Global::stAskIsNewAccount cmd;
			cmd.set_account(userId);
			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

			Athena::logger->trace("星彼渠道验证成功, 登录成功!");
			return true;	
		}else{
			Athena::logger->trace("星彼渠道验证失败, 登录不成功!");
			Athena::logger->trace("__MD5String:%s",MD5String.c_str());
			Athena::logger->trace("accessToken:%s",accessToken.c_str());
			return false;
		}

	}else{
		Athena::logger->trace("星彼渠道验证失败, 登录失败!");
	}
	return false;
}

bool UserLoginModHandle::verifyTipcat(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());
	int return_code = 0;
	std::string return_msg;
	Json::Reader reader;
	Json::Value value;                                                        
	std::list<std::string> strList;                                           
	if (reader.parse(data, value)){
		std::string app, user, sessionkey, timestamp, sign, key;
		if (value["app"].isString()){
			app = value["app"].asString();
		}
		if(value["user"].isString()){
			user = value["user"].asString();
		}
		if(value["sessionkey"].isString()){
			sessionkey = value["sessionkey"].asString();
		}
		if(value["key"].isString()){
			key = value["key"].asString();
		}
		
		if (app.empty() || user.empty() || sessionkey.empty() || key.empty())
		{
			Athena::logger->trace("Tipcat渠道收到用户名或参数为空, 验证失败");
			return false;
		}
	
		UINT32 unix_timestamp = time(NULL);
		timestamp =  g_int2string(unix_timestamp);

		std::string str = app + user + sessionkey + timestamp + key;

		int  bufLen = strlen(str.c_str());
		char digest[64] = {0};
		utils::Md5(str.c_str(),bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string MD5String = digest;
		sign = MD5String.substr(2,28);

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
		pRequest->setRequestType(CHttpRequest::kHttpGet);
		
		std::string url = "http://member.sdk.i2joy.com/openid/openid_session_check?";
		url = url + "app=" + app + "&user=" + user + "&timestamp=" + timestamp + "&sign=" + sign + "&sessionkey=" + sessionkey;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("url: %s", url.c_str());
#endif
		pRequest->setUrl(url.c_str());
		std::string configData;
		CHttpResponse * pRes = CHttpClient::getSingleton().synSend(pRequest);
		if (pRes && pRes->isSucceed()){
			std::vector<char> &retVec = *pRes->getResponseData();
			if (retVec.size() > 0){
				configData.assign(&retVec[0],retVec.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("config data is %s", configData.c_str());
#endif
				if (reader.parse(configData, value)){
					if (value["return_code"].isInt()){
						return_code = value["return_code"].asInt();
					}
					
					if(1 == return_code){	
						strncpy(one.m_account, user.c_str(), sizeof(one.m_account));
						//one.m_accountID = (UINT32)atoi(user_id.c_str());
						strncpy(one.m_loginToken, sign.c_str(), sizeof(one.m_loginToken));
            
           			 	Global::stAskIsNewAccount cmd;
            			cmd.set_account(user);
            			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

						SAFE_DELETE(pRes);
						return true;
					}

					if(value["return_msg"].isString()){
						return_msg = value["return_msg"].asString();
					}
				}
			}
		}
		SAFE_DELETE(pRes);
	}
	Athena::logger->trace("TipCat验证失败 return_code:%d, return_msg:%s", return_code, return_msg.c_str());
	return false;
}

bool UserLoginModHandle::verifyXiaoMi(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());
	int errcode = 0;//状态码
	std::string errMsg;//错误信息
	Json::Reader reader;
	Json::Value value;
	std::list<std::string> strList;
	if (reader.parse(data, value)){
		std::string appId, uid, session, signature;
		if (value["appId"].isString()){
			appId = value["appId"].asString();
		}
		if(value["uid"].isString()){
			uid = value["uid"].asString();
		}
		if(value["session"].isString()){
			session = value["session"].asString();
		}
		if(value["signature"].isString()){
			signature = value["signature"].asString();
		}
		
		if (appId.empty() || uid.empty() || session.empty() || signature.empty())
		{
			Athena::logger->trace("XiaoMi渠道收到用户名或参数为空, 验证失败");
			return false;
		}
	
		std::string url = "http://mis.migc.xiaomi.com/api/biz/service/verifySession.do?";
		url = url + "appId=" + appId + "&session=" + session + "&uid=" + uid + "&signature=" + signature;

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
		pRequest->setRequestType(CHttpRequest::kHttpGet);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("url: %s", url.c_str());
#endif
		pRequest->setUrl(url.c_str());
		std::string configData;
		CHttpResponse * pRes = CHttpClient::getSingleton().synSend(pRequest);
		if (pRes && pRes->isSucceed()){
			std::vector<char> &retVec = *pRes->getResponseData();
			if (retVec.size() > 0){
				configData.assign(&retVec[0],retVec.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("config data is %s", configData.c_str());
#endif
				if (reader.parse(configData, value)){
					if (value["errcode"].isInt()){
						errcode = value["errcode"].asInt();
					}
					
					if(200 == errcode){
						strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
						strncpy(one.m_loginToken, signature.c_str(), sizeof(one.m_loginToken));
            
           			 	Global::stAskIsNewAccount cmd;
            			cmd.set_account(uid);
            			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);

						SAFE_DELETE(pRes);
						return true;
					}

					if(value["errMsg"].isString()){
						errMsg = value["errMsg"].asString();
					}
				}
			}
		}
		SAFE_DELETE(pRes);
	}
	Athena::logger->trace("XiaoMi验证失败 状态码errcode:%d, 错误信息errMsg:%s", errcode, errMsg.c_str());
	return false;
}

bool UserLoginModHandle::verifyXueGao(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());
	Json::Reader reader;
	Json::Value value;
	std::list<std::string> strList;
	if (reader.parse(data, value)){
		std::string appId, uid, token;
		if (value["appId"].isString()){
			appId = value["appId"].asString();
		}
		if(value["uid"].isString()){
			uid = value["uid"].asString();
		}
		if(value["token"].isString()){
			token = value["token"].asString();
		}
		
		if (appId.empty() || uid.empty() || token.empty() )
		{
			Athena::logger->trace("XueGao渠道收到用户名或参数为空, 验证失败");
			return false;
		}
	
		/*
		 * http://xgsdk.xuegaogame.com/ipp-web-sdk/user/checkLogin?appId=xxx&uid=xxx&token=xxx
		 */
		std::string url = "http://xgsdk.xuegaogame.com/ipp-web-sdk/user/checkLogin?";
		url = url + "appId=" + appId + "&uid=" + uid + "&token=" + token ;

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
		pRequest->setRequestType(CHttpRequest::kHttpGet);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("url: %s", url.c_str());
#endif
		pRequest->setUrl(url.c_str());
		std::string configData;
		CHttpResponse * pRes = CHttpClient::getSingleton().synSend(pRequest);
		if (pRes && pRes->isSucceed()){
			std::vector<char> &retVec = *pRes->getResponseData();
			if (retVec.size() > 0){
				configData.assign(&retVec[0],retVec.size());
#ifdef _HDZ_DEBUG
				Athena::logger->trace("config data is %s", configData.c_str());
#endif
				if(0 == strcmp(configData.c_str(),"success")){
					strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
					strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
           			Global::stAskIsNewAccount cmd;
            		cmd.set_account(uid);
            		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
					SAFE_DELETE(pRes);
					return true;
				}
			}
		}
		SAFE_DELETE(pRes);
	}
	Athena::logger->trace("XueGao验证失败");
	return false;
}

bool UserLoginModHandle::verifySagegame(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	Json::Reader reader;
	Json::Value value;
	std::list<std::string> strList;
	if (reader.parse(data, value)){
        std::string uid, token;
		if (value["uid"].isString()){
            uid = value["uid"].asString();
		}
		if (value["token"].isString()){
            token = value["token"].asString();
		}
        if (uid.empty() || token.empty()) {
            Athena::logger->trace("Sagegame渠道收到用户名或token为空, 验证失败");
            return false;
        }
		std::string appid	=	"341";
		std::string appkey	=	"87689aba48683df4c7ede95f9292e053";

 		UINT32 unix_timestamp = time(NULL);
		std::string timestamp = g_int2string(unix_timestamp);

		std::string str = appid + timestamp + token + uid;
		int  bufLen = strlen(str.c_str());
		char digest[64] = {0};
		utils::Md5(str.c_str(),bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string signStr = digest;

		memset(digest,0,64);
		signStr += appkey;
		bufLen = strlen(signStr.c_str());
		utils::Md5(signStr.c_str(),bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string sign = digest;
	
		std::string url = "http://account.6873.com/2.0/loginverify?";
		url = url + "appid=" + appid + "&timestamp=" + timestamp + "&token=" + token + "&uid=" + uid + "&sign=" + sign;
        
		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpGet);
        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
            SAFE_DELETE(pRes);
            Json::Value configJson;
            Json::Reader reader;

            int ret = 0;
            std::string msg;
            if (reader.parse(configData, configJson)) {
                if (configJson["msg"].isString()){
                    msg = configJson["msg"].asString();
                }
                if (configJson["ret"].isInt()){
                    ret = configJson["ret"].asInt();
                }
            }
            if (0 == strcmp(msg.c_str(), "success") && 1 == ret) {
                Athena::logger->trace("Sagegame验证通过");
                strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
                strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(uid);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
			Athena::logger->trace("URL:%s", url.c_str());
            Athena::logger->trace("Sagegame验证失败, token:%s, ret:%d, msg:%s", token.c_str(), ret, msg.c_str());
    	} else {
            Athena::logger->trace("Sagegame验证失败, http请求失败:%s",url.c_str());
        }
		SAFE_DELETE(pRes);
	}
	return false;
}

bool UserLoginModHandle::verify1tsdk(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
        std::string mem_id, user_token;
		if (value["uid"].isString()){
            mem_id = value["uid"].asString();
		}
		if (value["token"].isString()){
            user_token = value["token"].asString();
		}

		std::string appid, appkey;
		if(sdkId == _1tsdk){
			appid	=	"60043";
			appkey	=	"138ef85d624540c73da6e50d0d0b6fe1";
		}else{
			if(value["app_id"].isString()){
				appid = value["app_id"].asString();
			}
			if(value["app_key"].isString()){
				appkey = value["app_key"].asString();
			}
		}

		if(mem_id.empty() || user_token.empty() || appid.empty() || appkey.empty()){
			Athena::logger->error("1tsdk渠道收到的参数为空，验证失败sdkId:%d",sdkId);
			return false;
		}

		std::string str = "app_id=" + appid + "&mem_id=" + mem_id + "&user_token=" + user_token + "&app_key=" + appkey;

		int  bufLen = strlen(str.c_str());
		char digest[64] = {0};
		utils::Md5(str.c_str(),bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string signStr = digest;
	
		std::string url = "https://union.huoyx.cn/api/cp/user/check";
		std::string requestData = "app_id=" + appid + "&mem_id=" + mem_id + "&user_token=" + user_token + "&sign=" + signStr;
        
		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpPost);
		pRequest->setRequestData(requestData.c_str(), requestData.size());
        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
            SAFE_DELETE(pRes);
            Json::Value configJson;
            Json::Reader reader;

			std::string status;
            std::string msg;
            if (reader.parse(configData, configJson)) {
                if (configJson["status"].isString()){
                    status = configJson["status"].asString();
                }
                if (configJson["msg"].isString()){
                    msg = configJson["msg"].asString();
                }
            }
            if (status == "1") {
                Athena::logger->trace("1tsdk验证通过");
                strncpy(one.m_account, mem_id.c_str(), sizeof(one.m_account));
                strncpy(one.m_loginToken, user_token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(mem_id);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
			Athena::logger->trace("URL:%s", url.c_str());
            Athena::logger->trace("1tsdk验证失败, user_token:%s, status:%s, msg:%s", user_token.c_str(), status.c_str(), msg.c_str());
    	} else {
            Athena::logger->trace("1tsdk验证失败, http请求失败:%s",url.c_str());
        }
		Athena::logger->trace("requestData:%s", requestData.c_str());
		SAFE_DELETE(pRes);
	}
	return false;
}

bool UserLoginModHandle::verify3hgame(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
        std::string id, appid, username, token;
		char cid[20] = {0};
		int iid;
		if (value["id"].isInt()){
            iid = value["id"].asInt();
			snprintf(cid,20,"%d",iid);
			id = cid;
		}
		if (value["appid"].isInt()){
			char cappid[20]={0};
			snprintf(cappid,20,"%d",value["appid"].asInt());
			appid = cappid;
		}
		if (value["username"].isString()){
			username = value["username"].asString();
		}
		if (value["token"].isString()){
            token = value["token"].asString();
		}
        if (id.empty() || appid.empty() || username.empty() || token.empty()) {
            Athena::logger->trace("3hgame渠道收到参数为空, 验证失败");
            return false;
        }

		std::string url = "http://ios.3hgame.com/cpVerify.php";
        
		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpPost);
		pRequest->setRequestData(data.c_str(),data.size());
		std::vector<std::string> headerVec;
		headerVec.push_back("Content-Type: application/json");
		pRequest->setHeaders(headerVec);

        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
            SAFE_DELETE(pRes);
            Json::Value configJson;
            Json::Reader reader;

			int return_id;
			int status;
            std::string msg;
            if (reader.parse(configData, configJson)) {
                if (configJson["id"].isInt()){
					return_id = configJson["id"].asInt();
				}
				if (configJson["status"].isInt()){
                    status = configJson["status"].asInt();
                }
                if (configJson["data"].isString()){
                    msg = configJson["data"].asString();
                }
            }

            if ((status == 1) && (return_id == iid)) {
                Athena::logger->trace("3hgame验证通过");
                strncpy(one.m_account, username.c_str(), sizeof(one.m_account));
                strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(username);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
			Athena::logger->trace("URL:%s", url.c_str());
            Athena::logger->trace("3hgame验证失败, id:%d, token:%s, status:%d, msg:%s",return_id, token.c_str(), status, msg.c_str());
    	} else {
            Athena::logger->trace("3hgame验证失败, http请求失败:%s",url.c_str());
        }
		SAFE_DELETE(pRes);
	}
	return false;
}

bool UserLoginModHandle::verify01game(UINT32 sdkId, const std::string & data, stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
        std::string appid, appkey, user_id, token;
		if (value["appid"].isString()){
            appid = value["appid"].asString();
		}
		if (value["apkey"].isString()){
			appkey = value["apkey"].asString();
		}
		if (value["user_id"].isString()){
			user_id = value["user_id"].asString();
		}
		if (value["token"].isString()){
            token = value["token"].asString();
		}
        if (appid.empty() || appkey.empty() || user_id.empty() || token.empty()) {
            Athena::logger->trace("01game渠道收到参数为空, 验证失败");
            return false;
		}

		Json::FastWriter writer;
		Json::Value root;

		root["token"] = token;
		root["user_id"] = (UINT32)atoi(user_id.c_str());
		std::string content = writer.write(root);
		std::string  bodyStr = content;
		content += appkey;

		int  bufLen = content.size();
		char digest[64] = {0};
		utils::Md5(content.c_str(),bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string sign = digest;

		std::string url = "http://aoneldapi01.smartspace-game.com:8000/api/user_verify.do?";
		std::string requestData = "appid=" + appid + "&sign=" + sign;
		url += requestData;

		Athena::logger->trace("requestData:%s",requestData.c_str());
		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpPost);
		pRequest->setRequestData(bodyStr.c_str(), bodyStr.size());
		std::vector<std::string> headerVec;
		headerVec.push_back("Content-Type: application/json");
		pRequest->setHeaders(headerVec);

        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
            SAFE_DELETE(pRes);
            Json::Value configJson;
            Json::Reader reader;

			int retcode;
			std::string retmsg;
            std::string body;
            if (reader.parse(configData, configJson)) {
                if (configJson["retcode"].isInt()){
					retcode = configJson["retcode"].asInt();
				}
				if (configJson["retmsg"].isString()){
                    retmsg = configJson["retmsg"].asString();
                }
                if (configJson["body"].isString()){
                    body = configJson["body"].asString();
                }
            }

            if (0 == retcode) {
                Athena::logger->trace("01game验证通过");
                strncpy(one.m_account, user_id.c_str(), sizeof(one.m_account));
                strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(user_id);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
            Athena::logger->trace("01game验证失败, retcode:%d, token:%s, retmsg:%s, body:%s", retcode, token.c_str(), retmsg.c_str(), body.c_str());
    	} else {
            Athena::logger->trace("01game验证失败, http请求失败!");
        }
		SAFE_DELETE(pRes);
	}
	return false;
}

bool UserLoginModHandle::verifyQuicksdk(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[Quicksdk 登录验证]sdk extar data is %s", data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string uid, token;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["appid"].isString()){
		//	dataMap["appid"] = value["appid"].asString();
		}
		if (value["appkey"].isString()){
		//	dataMap["appkey"] = value["appkey"].asString();
		}
		if (value["token"].isString()){
			token = value["token"].asString();
			dataMap["token"] = token;
		}
		if (value["product_code"].isString()){
			dataMap["product_code"] = value["product_code"].asString();
		}
		if (value["channel_code"].isString()){
			dataMap["channel_code"] = value["channel_code"].asString();
		}
		if (value["uid"].isString()){
			uid = value["uid"].asString();
			dataMap["uid"] = uid;
		}

		if(dataMap.empty() || uid.empty() || token.empty()){
            Athena::logger->trace("quick渠道收到参数为空, 验证失败");
            return false;
		}

		//std::string url = "http://checkuser.sdk.quicksdk.net/v2/checkUserInfo?";
        std::string url = "http://checkuser.quickapi.net/v2/checkUserInfo?";	
        std::string bodyStr;
        bool first = true;

		std::map<std::string, std::string>::iterator itr = dataMap.begin();
		for(;itr != dataMap.end(); ++itr){
			if(first){
				bodyStr += itr->first + "=" + itr->second;
				first = false;
			}else{
				bodyStr += "&" + itr->first + "=" + itr->second;
			}
		}
		
		url += bodyStr;
        Athena::logger->trace("#1#CHARLES URL is :%s", url.c_str());

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpGet);
		std::vector<std::string> headerVec;
		headerVec.push_back("Content-Type: application/json");
		pRequest->setHeaders(headerVec);

        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            Athena::logger->trace("#2#CHARLES Succeed !!!");
            std::vector<char> &retVec = *pRes->getResponseData();
            Athena::logger->trace("#3#CHARLES retVec.size :%d", retVec.size());
            if (retVec.size() > 0){
                Athena::logger->trace("#4#CHARLES retVec.value :%s", &retVec[0]);
                configData.assign(&retVec[0],retVec.size());
                Athena::logger->trace("#5#CHARLES configData.value :%s", configData.c_str());
            }

			SAFE_DELETE(pRes);

            if ("1" == configData) {
                Athena::logger->trace("quick验证通过");
                strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
                strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(uid);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
            Athena::logger->trace("quick验证失败, retcode:%s", configData.c_str());
    	} else {
            Athena::logger->trace("quick验证失败, http请求失败!");
        }
		SAFE_DELETE(pRes);
	}
	return false;

}

bool UserLoginModHandle::verifyQuicksdkHW(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[Quicksdk 【海外版本】 登陆]sdk extar data is %s", data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string uid, token;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["appid"].isString()){
		//	dataMap["appid"] = value["appid"].asString();
		}
		if (value["appkey"].isString()){
		//	dataMap["appkey"] = value["appkey"].asString();
		}
		if (value["token"].isString()){
			token = value["token"].asString();
			dataMap["token"] = token;
		}
		if (value["product_code"].isString()){
			dataMap["product_code"] = value["product_code"].asString();
		}
		if (value["channel_code"].isString()){
			dataMap["channel_code"] = value["channel_code"].asString();
		}
		if (value["uid"].isString()){
			uid = value["uid"].asString();
			dataMap["uid"] = uid;
		}

		if(dataMap.empty() || uid.empty() || token.empty()){
            Athena::logger->trace("quick 【海外版本】 渠道收到参数为空, 验证失败");
            return false;
		}

		//std::string url = "http://checkuser.sdk.quicksdk.net/v2/checkUserInfo?";
        // std::string url = "http://checkuser.quickapi.net/v2/checkUserInfo?";
		std::string url = "https://zyy.doufcgame.com/webapi/checkUserInfo?";

        std::string bodyStr;
        bool first = true;

		std::map<std::string, std::string>::iterator itr = dataMap.begin();
		for(;itr != dataMap.end(); ++itr){
			if(first){
				bodyStr += itr->first + "=" + itr->second;
				first = false;
			}else{
				bodyStr += "&" + itr->first + "=" + itr->second;
			}
		}
		
		url += bodyStr;
        Athena::logger->trace("#1#CHARLES URL is :%s", url.c_str());

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpGet);
		std::vector<std::string> headerVec;
		headerVec.push_back("Content-Type: application/json");
		pRequest->setHeaders(headerVec);

        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            Athena::logger->trace("#2#CHARLES Succeed !!!");
            std::vector<char> &retVec = *pRes->getResponseData();
            Athena::logger->trace("#3#CHARLES retVec.size :%d", retVec.size());
            if (retVec.size() > 0){
                Athena::logger->trace("#4#CHARLES retVec.value :%s", &retVec[0]);
                configData.assign(&retVec[0],retVec.size());
                Athena::logger->trace("#5#CHARLES configData.value :%s", configData.c_str());
            }

			SAFE_DELETE(pRes);


			Json::Value configJson;
			Json::Reader reader;

			std::string retuid;

			if (reader.parse(configData, configJson)) {
				if (configJson["uid"].isString()){
					retuid = configJson["uid"].asString();
				}
			}

        	Athena::logger->trace("#####uid :%s", uid.c_str());
        	Athena::logger->trace("#####retuid :%s", retuid.c_str());

            if (uid == retuid) {
                Athena::logger->trace("result  retuid OK !!!");
                Athena::logger->trace("quick【海外版本】验证通过");
                strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
                strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(uid);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
            Athena::logger->trace("quick【海外版本】验证失败, retcode:%s", configData.c_str());
    	} else {
            Athena::logger->trace("quick【海外版本】验证失败, http请求失败!");
        }
		SAFE_DELETE(pRes);
	}
	return false;

}

bool UserLoginModHandle::verifyBRsdk(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[BR sdk 登录验证]sdk extar data is %s", data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string uid, token;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["appid"].isString()){
			// appid = value["appid"].asString();
			// dataMap["appid"] = appid;
		}
		// if (value["appkey"].isString()){
		// //	dataMap["appkey"] = value["appkey"].asString();
		// }
		if (value["token"].isString()){
			token = value["token"].asString();
			dataMap["token"] = token;
		}
		// if (value["product_code"].isString()){
		// 	dataMap["product_code"] = value["product_code"].asString();
		// }
		// if (value["channel_code"].isString()){
		// 	dataMap["channel_code"] = value["channel_code"].asString();
		// }
		if (value["uid"].isString()){
			uid = value["uid"].asString();
			dataMap["uid"] = uid;
		}

		if(dataMap.empty() || uid.empty() || token.empty()){
            Athena::logger->trace("BR渠道收到参数为空, 验证失败");
            return false;
		}
		std::string appId = "436bba23015c3ab4caeebcf689d5bb7d";
		std::string serverKey = "6cda7a9caa82a9ac193c341dbce92eef";
		dataMap["appId"] = appId;
		std::string str = appId + token + serverKey;

		Athena::logger->trace("#1#str  is :%s", str.c_str());

		int  bufLen = strlen(str.c_str());
		char digest[64] = {0};
		utils::Md5(str.c_str(),bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string sign = digest;

		dataMap["sign"] = sign;

		//std::string url = "http://checkuser.sdk.quicksdk.net/v2/checkUserInfo?";
        std::string url = "http://user.api.qijie168.com/game/verify/token?";	
        std::string bodyStr;
        bool first = true;

		std::map<std::string, std::string>::iterator itr = dataMap.begin();
		for(;itr != dataMap.end(); ++itr){
			if(first){
				bodyStr += itr->first + "=" + itr->second;
				first = false;
			}else{
				bodyStr += "&" + itr->first + "=" + itr->second;
			}
		}
		
		url += bodyStr;
        Athena::logger->trace("#1#CHARLES URL is :%s", url.c_str());

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpGet);
		std::vector<std::string> headerVec;
		headerVec.push_back("Content-Type: application/json");
		pRequest->setHeaders(headerVec);



		std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
			SAFE_DELETE(pRes);
			Json::Value configJson;
			Json::Reader reader;

			int code;
			std::string msg;
			std::string data;

			if (reader.parse(configData, configJson)) {
				if (configJson["code"].isInt()){
					code = configJson["code"].asInt();
				}
				if (configJson["msg"].isString()){
					msg = configJson["msg"].asString();
				}
				if (configJson["data"].isString()){
					data = configJson["data"].asString();
				}
			}
            Athena::logger->trace("JSON, code:%d, msg:%s, data:%s", code, msg.c_str(), msg.c_str());

			if (code == 1) {
				Athena::logger->trace("BR 验证通过");
				strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
				strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(uid);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
            Athena::logger->trace("BR验证失败, code:%d, msg:%s", code, msg.c_str());
    	} else {
            Athena::logger->trace("BR验证失败, http请求失败!");
        }
		SAFE_DELETE(pRes);










        // if (pRes && pRes->isSucceed()){


        //     std::vector<char> &retVec = *pRes->getResponseData();
        //     if (retVec.size() > 0){
        //         Athena::logger->trace("#4#CHARLES retVec.value :%s", &retVec[0]);
		// 		configData.assign(&retVec[0],retVec.size());
		// 		std::string code;
		// 		Json::Reader reader2;
		// 		Json::Value value2;
		// 		if (reader2.parse(configData, value2)){
		// 			code = value2["code"].asString();
		// 		}
		// 		SAFE_DELETE(pRes);

        //         Athena::logger->trace("#5#code code.value :%s", code.c_str());

		// 		if ("1" == code) {
		// 			Athena::logger->trace("BR 验证通过");
		// 			strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
		// 			strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
					
		// 			Global::stAskIsNewAccount cmd;
		// 			cmd.set_account(uid);
		// 			GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		// 			return true;
		// 		}


        //     }


    	// } else {
        //     Athena::logger->trace("BR 验证失败, http请求失败!");
        // }
		// SAFE_DELETE(pRes);
	}
	return false;

}


bool UserLoginModHandle::verifyXinyu(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string appid, userid, token, act;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["appid"].isString()){
			appid = value["appid"].asString();
		}
		if (value["usertoken"].isString()){
			token = value["usertoken"].asString();
		}
		if (value["act"].isString()){
			act = value["act"].asString();
		}
		if (value["userid"].isString()){
			userid = value["userid"].asString();
		}

		if(appid.empty() || token.empty() || act.empty() || userid.empty()){
            Athena::logger->trace("Xinyu渠道收到参数为空, 验证失败");
            return false;
		}

		std::string url = "http://user-api.58tty.com/ins.aspx?";
		std::string bodyStr = "act=" + act + "&userid=" + userid + "&usertoken=" + token + "&appid=" + appid;

		url += bodyStr;

	 	Athena::logger->trace("url:%s", url.c_str());

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpGet);
		//std::vector<std::string> headerVec;
		//headerVec.push_back("Content-Type: application/json");
		//pRequest->setHeaders(headerVec);

        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
			SAFE_DELETE(pRes);
			Json::Value configJson;
			Json::Reader reader;

			std::string result;
			std::string desc;
			if (reader.parse(configData, configJson)) {
				if (configJson["result"].isString()){
					result = configJson["result"].asString();
				}
				if (configJson["desc"].isString()){
					desc = configJson["desc"].asString();
				}
			}

			if ("1" == result) {
				Athena::logger->trace("xinyu验证通过");
				strncpy(one.m_account, userid.c_str(), sizeof(one.m_account));
				strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(userid);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
            Athena::logger->trace("xinyu验证失败, result:%s, desc:%s", result.c_str(), desc.c_str());
    	} else {
            Athena::logger->trace("xinyu验证失败, http请求失败!");
        }
		SAFE_DELETE(pRes);
	}
	return false;


}

bool UserLoginModHandle::verifyDongNanYa(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string appid, appkey, userid, token;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["appid"].isString()){
			appid = value["appid"].asString();
		}
		if (value["appkey"].isString()){
			appkey = value["appkey"].asString();
		}
		if (value["userid"].isString()){
			userid = value["userid"].asString();
		}
		if (value["token"].isString()){
			token = value["token"].asString();
		}

		if(appid.empty() || appkey.empty() || userid.empty() || token.empty()){
            Athena::logger->trace("东南亚渠道收到参数为空, 验证失败");
            return false;
		}


		std::string str = appid + token + appkey;
		int  bufLen = strlen(str.c_str());
		char digest[64] = {0};
		utils::Md5(str.c_str(),bufLen,digest);
		utils::HexString::bin2asc(digest,16,digest,64,bufLen);
		digest[bufLen] = 0;
		std::string sign = digest;

		std::string url = "https://ins-asia.bowinggame.com/token.php?";
		std::string bodyStr = "userid=" + userid + "&token=" + token + "&appid=" + appid + "&sign=" + sign;

		url += bodyStr;

	 	Athena::logger->trace("url:%s", url.c_str());

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpGet);

        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
			SAFE_DELETE(pRes);
			Json::Value configJson;
			Json::Reader reader;

			int  result;
			std::string desc;
			if (reader.parse(configData, configJson)) {
				if (configJson["result"].isInt()){
					result = configJson["result"].asInt();
				}
				if (configJson["desc"].isString()){
					desc = configJson["desc"].asString();
				}
			}

			if (1 == result) {
				Athena::logger->trace("东南亚验证通过");
				strncpy(one.m_account, userid.c_str(), sizeof(one.m_account));
				strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(userid);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
            Athena::logger->trace("东南亚验证失败, result:%d, desc:%s", result, desc.c_str());
    	} else {
            Athena::logger->trace("东南亚验证失败, http请求失败!");
        }
		SAFE_DELETE(pRes);
	}
	return false;
}

bool UserLoginModHandle::verifyHuaWei(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	return false;
}

bool UserLoginModHandle::verify51sfsy(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string uid, token;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["uid"].isString()){
			uid = value["uid"].asString();
		}
		if (value["token"].isString()){
			token = value["token"].asString();
		}

		if(uid.empty() || token.empty()){
            Athena::logger->trace("51sfsy渠道收到参数为空, 验证失败");
            return false;
		}

		std::string url = "http://smi.51sfsy.com/mllm/auth?";
		std::string bodyStr = "uid=" + uid + "&token=" + token;

		url += bodyStr;

	 	Athena::logger->trace("url:%s", url.c_str());

		CHttpRequest * pRequest = ATHENA_NEW CHttpRequest();
        pRequest->setUrl(url.c_str());
        pRequest->setRequestType(CHttpRequest::kHttpGet);

        std::string configData;
        CHttpResponse *  pRes = CHttpClient::getSingleton().synSend(pRequest);
        if (pRes && pRes->isSucceed()){
            std::vector<char> &retVec = *pRes->getResponseData();
            if (retVec.size() > 0){
                configData.assign(&retVec[0],retVec.size());
            }
			SAFE_DELETE(pRes);
			Json::Value configJson;
			Json::Reader reader;

			UINT32 result = 1;
			std::string	errmsg;
			if (reader.parse(configData, configJson)) {
				if (configJson["errno"].isInt()){
					result = configJson["errno"].asInt();
				}
				if (configJson["errmsg"].isString()){
					errmsg = configJson["errmsg"].asString();
				}
			}

			if (0 == result) {
				Athena::logger->trace("51sfsy验证通过");
				strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
				strncpy(one.m_loginToken, token.c_str(), sizeof(one.m_loginToken));
                
                Global::stAskIsNewAccount cmd;
                cmd.set_account(uid);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
                return true;
            }
            Athena::logger->trace("51sfsy验证失败, errno:%d, errmsg:%s", result, errmsg.c_str());
    	} else {
            Athena::logger->trace("51sfsy验证失败, http请求失败!");
        }
		SAFE_DELETE(pRes);
	}
	return false;
}

bool UserLoginModHandle::verifyCommon(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk %d extar data is %s", sdkId, data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string uid, token;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["uid"].isString()){
			uid = value["uid"].asString();
		} 

		if(uid.empty()){
            Athena::logger->trace("通用验证收到参数为空, 验证失败");
            return false;
		}

		strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
		
		Global::stAskIsNewAccount cmd;
		cmd.set_account(uid);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		return true;
	}
	return false;
}


bool UserLoginModHandle::verifyGongKou(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk %d extar data is %s", sdkId, data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string uid, token,gameuid;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["uid"].isString()){
			uid = value["uid"].asString();
		} 
		if (value["gameuid"].isString()){
			gameuid = value["gameuid"].asString();
		} 

		if(gameuid.empty()){
            Athena::logger->trace("工口验证收到参数为空, 验证失败");
            return false;
		}

		strncpy(one.m_account, gameuid.c_str(), sizeof(one.m_account));
		
		Global::stAskIsNewAccount cmd;
		cmd.set_account(gameuid);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		return true;
	}
	return false;
}



bool UserLoginModHandle::verifyLaosiji(UINT32 sdkId, const std::string & data,stLoginTokenInfo & one)
{
	Athena::logger->trace("[登录验证]sdk extar data is %s", data.c_str());

	std::map<std::string, std::string>	dataMap;
	std::string uid, token;
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(data, value)){
		if (value["uid"].isString()){
			uid = value["uid"].asString();
		} 

		if(uid.empty()){
            Athena::logger->trace("老司机渠道收到参数为空, 验证失败");
            return false;
		}

		strncpy(one.m_account, uid.c_str(), sizeof(one.m_account));
		
		Global::stAskIsNewAccount cmd;
		cmd.set_account(uid);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
		return true;
	}
	return false;
}

void UserLoginModHandle::userReqLogin(TCPComponent * pCom,Login::ReqLoginGate  * reqLogin)
{
	if (!pCom || !reqLogin) {
		return ;
	}




	stLoginTokenInfo tokenInfo; 
	Login::LoginError  retErrorCmd;

	Athena::logger->trace("用户登录： token=%s",reqLogin->token().c_str());

	if (g_gateServer->isBanLogin()){//禁止登录
		GateUserConnMgr::getSingleton().disconnectComponent(pCom);
		return ;
	}
	
	if (reqLogin->clientversion() < CGameVersionCfg::getSingleton().getMinGameVersion()){
		Athena::logger->error("客户端版本太低,登录失败,发送过来的版本号为:%d,需要的版本号为:%d",reqLogin->clientversion(),CGameVersionCfg::getSingleton().getMinGameVersion());
		retErrorCmd.set_errortype(Login::LoginError::LOGIN_VERSION_TOO_LOW);
		GateUserConnMgr::getSingleton().sendPacket2Commponet(pCom,retErrorCmd);
		GateUserConnMgr::getSingleton().writeFinishCloseComponent(pCom);
		return ;
	}

	bool verifySuccess = false;
    if ((reqLogin->sdkid() >= _11Wan && reqLogin->sdkid() <= Sdk_Max) && (YuWan != reqLogin->sdkid())) {
		Athena::logger->debug("[游戏验证帐号] sdk id %d 帐号验证", reqLogin->sdkid());
        if(reqLogin->sdkid() == Starpy){
			verifySuccess = verifyStarpy(reqLogin->sdkid(), reqLogin->extradata(), tokenInfo);
		}else if((reqLogin->sdkid() == Tipcat) || (reqLogin->sdkid() == GoldGame)){
			verifySuccess = verifyTipcat(reqLogin->sdkid(), reqLogin->extradata(), tokenInfo); 
		}else if(reqLogin->sdkid() == XiaoMi){
			verifySuccess = verifyXiaoMi(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == XueGao){
			verifySuccess = verifyXueGao(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == Sagegame){
			verifySuccess = verifySagegame(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == _1tsdk || reqLogin->sdkid() == _1tsdk_){
			verifySuccess = verify1tsdk(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);	
		}else if(reqLogin->sdkid() == _3hgame){
			verifySuccess = verify3hgame(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);	
		}else if(reqLogin->sdkid() == _01game){
			verifySuccess = verify01game(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == Quicksdk){
			verifySuccess = verifyQuicksdk(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == QuicksdkHW){
			verifySuccess = verifyQuicksdkHW(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == BR_sdk){
			verifySuccess = verifyBRsdk(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == Xinyu){
			verifySuccess = verifyXinyu(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == DongNanYa){
			verifySuccess = verifyDongNanYa(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == HuaWei){
			verifySuccess = verifyHuaWei(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == _51sfsy){
			verifySuccess = verify51sfsy(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == H365 || reqLogin->sdkid() == NftWallet){
			verifySuccess = verifyCommon(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == Laosiji){
			verifySuccess = verifyLaosiji(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		}else if(reqLogin->sdkid() == GongKou){
			verifySuccess = verifyGongKou(reqLogin->sdkid(), reqLogin->extradata(),tokenInfo);
		} else {
            verifySuccess = verifyCommon(reqLogin->sdkid(), reqLogin->extradata(), tokenInfo);
        }
    } else {
		Athena::logger->debug("[游戏验证帐号] sdk id %d 帐号验证", reqLogin->sdkid());
        if (g_gateServer->isInnerVerify()){
            verifySuccess = verifyToken(reqLogin->token().c_str(),tokenInfo);	
        }
        else {
            verifySuccess = formatVerifyToken(reqLogin->token().c_str(),tokenInfo); 
        }
    }

	if (verifySuccess){
		//合服 前打开！！！
		// Athena::logger->trace("@@@@用户登录： account=%s",tokenInfo.m_account);
		// GateUserManager::getSingleton().addImmLoginOutAccount(tokenInfo.m_account);
		// GateUserConnMgr::getSingleton().disconnectAccount(tokenInfo.m_account);
		// ::sleep(1);	
		//合服 前打开！！！

		// GateUserManager::getSingleton().addImmLoginOutAccount(tokenInfo.m_account);
		// GateUserConnMgr::getSingleton().disconnectAccount(tokenInfo.m_account);
		Athena::logger->debug("[游戏验证帐号] 帐号验证成功,account=%s,deviceID=%s",tokenInfo.m_account,reqLogin->deviceid().c_str());
		GateUser * pUser = SelectUserManager::getSingleton().getUserByAccount(tokenInfo.m_account);
		if (pUser){
			Athena::logger->error("玩家%s已经在登录了,不能重复发送登录协议",tokenInfo.m_account);
			retErrorCmd.set_errortype(Login::LoginError::HAS_LOGIN_IN);
			GateUserConnMgr::getSingleton().sendPacket2Commponet(pCom,retErrorCmd);
			GateUserConnMgr::getSingleton().writeFinishCloseComponent(pCom);
			return ; 	
		}

		pUser = GateUserManager::getSingleton().getUserByAccount(tokenInfo.m_account);
		if (pUser){
			Athena::logger->error("玩家%s已经游戏中了,前面的玩家被踢掉!",tokenInfo.m_account);
			retErrorCmd.set_errortype(Login::LoginError::LOGIN_IN_OTHER_PLACE);
			TCPComponent * pOldIoc = GateUserConnMgr::getSingleton().getComponentByAccount(tokenInfo.m_account);
			if (pOldIoc != pCom){
				GateUserConnMgr::getSingleton().sendPacket2Commponet(pOldIoc,retErrorCmd);
				GateUserConnMgr::getSingleton().writeFinishCloseComponent(pOldIoc);
				GateUserConnMgr::getSingleton().chgTCPComponent(tokenInfo.m_account,pCom);
				//GateUserConnMgr::getSingleton().writeFinishCloseComponent(pOldIoc);

				/*Global::stReloginGlobal reloginCmd;
				reloginCmd.set_account(tokenInfo.m_account);
				reloginCmd.set_charid(pUser->getID());
				reloginCmd.set_device(reqLogin->deviceid());
				reloginCmd.set_ip(tokenInfo.m_ip);
				if(!GlobalSvrClientMgr::getSingleton().broadcastProto2All(reloginCmd)){
					Athena::logger->trace("向Global发送协议不成功,重新登录失败 account=%s",tokenInfo.m_account);
					GateUserConnMgr::getSingleton().disconnectComponent(pCom);
					return ;
					}*/
				pUser->notifyGlobalLoginOut();
				pUser->notifyGameLoginOut();
				//pUser->setGameState(GateUser::GAME_STATE_SELECT);
				pUser->beginSelect();

				GateUserManager::getSingleton().removeUser(pUser);
				GateUserManager::getSingleton().removeLoginOutAccount(tokenInfo.m_account);
				GateUserManager::getSingleton().cancelLogout(tokenInfo.m_account);
				SelectUserManager::getSingleton().addUserByAccount(pUser);

				Role::RefreshSnapData retCmd;
				retCmd.set_flag(1);
				std::map<UINT32,stGateRoleInfo>::iterator it = pUser->m_gateRoleInfo.begin();
				for (;it != pUser->m_gateRoleInfo.end();++it){
					Role::stSynSnapData * pSyn = retCmd.add_data();
					stGateRoleInfo & roleInfo = it->second;
					if (pSyn){
						pSyn->set_charid(roleInfo.m_charID);
						pSyn->set_name(roleInfo.m_name);
						pSyn->set_level(roleInfo.m_level);
						pSyn->set_job(roleInfo.m_job);
						pSyn->set_sex(roleInfo.m_sex);
					}
				}
				SelectUserManager::getSingleton().sendProto(tokenInfo.m_account,retCmd);
			}
			return;
		}

		pUser = ATHENA_NEW GateUser(tokenInfo.m_account);
		if (pUser) {
			std::string loginIP = tokenInfo.m_ip;//GateUserConnMgr::getSingleton().getComponentIP(pCom);
			if (loginIP == ""){
				loginIP = reqLogin->ip();
			}
			pUser->m_channelID = tokenInfo.m_channelID;
			pUser->m_deviceID = reqLogin->deviceid();
			strncpy(pUser->m_ip,loginIP.c_str(),sizeof(pUser->m_ip));
			Global::stReqSnapData reqSnapCmd;
			reqSnapCmd.set_opid(reqLogin->opid());
			reqSnapCmd.set_deviceid(reqLogin->deviceid());
			reqSnapCmd.set_account(tokenInfo.m_account);
			reqSnapCmd.set_gateid(g_gateServer->m_svrInfo.m_svrID);
			reqSnapCmd.set_ip(pUser->m_ip);
			reqSnapCmd.set_serverid(reqLogin->serverid());

			Athena::logger->trace("[游戏验证帐号]账号%s从ip%s登录了",tokenInfo.m_account,loginIP.c_str());

			pUser->beginSelect();
			if (!GlobalSvrClientMgr::getSingleton().broadcastProto2All(reqSnapCmd)){
				Athena::logger->trace("向Global发送协议不成功,登录失败 account=%s",tokenInfo.m_account);
				GateUserConnMgr::getSingleton().disconnectComponent(pCom);
				return;
			}

			GateUserConnMgr::getSingleton().addAccountComponent(tokenInfo.m_account,pCom);
			SelectUserManager::getSingleton().addUserByAccount(pUser);
			GateUserManager::getSingleton().removeLoginOutAccount(tokenInfo.m_account);
			GateUserManager::getSingleton().cancelLogout(tokenInfo.m_account);
		}
		else {
			Athena::logger->error("[游戏验证帐号]账号%s new失败",tokenInfo.m_account);
			GateUserConnMgr::getSingleton().disconnectComponent(pCom);
			return ;
		}
		return;
	}
	else {
		Athena::logger->error("[游戏验证帐号] 帐号验证失败account=%s",reqLogin->token().c_str());
		GateUserConnMgr::getSingleton().disconnectComponent(pCom);
		return ;
	}
}

void UserLoginModHandle::reqSvrTime(TCPComponent * pCom,Login::stReqServerTime * recvCmd)
{
	if (!recvCmd){
		return ;
	}	

	Login::stRetServerTime retCmd;
	retCmd.set_svrtime(g_gateServer->getSec());
	GateUserConnMgr::getSingleton().sendPacket2Commponet(pCom,retCmd);	
}

void UserLoginModHandle::autoConnectGate(TCPComponent * pCom,Login::stAutoLoginGate * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	if (g_gateServer->isBanLogin()){//禁止登录
		GateUserConnMgr::getSingleton().disconnectComponent(pCom);
		return ;
	}
	
	stLoginTokenInfo tokenInfo;
	bool verifySuccess = false;
    if ((recvCmd->sdkid() >= _11Wan && recvCmd->sdkid() <= Sdk_Max)  && (YuWan != recvCmd->sdkid())) {
		Athena::logger->debug("[游戏验证帐号]自动重连, sdk id %d 帐号验证", recvCmd->sdkid());
        if(recvCmd->sdkid() == Starpy ){	
			verifySuccess = verifyStarpy(recvCmd->sdkid(), recvCmd->extradata(), tokenInfo);
		}else if((recvCmd->sdkid() == Tipcat) || (recvCmd->sdkid() == GoldGame)){
			verifySuccess = verifyTipcat(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == XiaoMi){
			verifySuccess = verifyXiaoMi(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == XueGao){
			verifySuccess = verifyXueGao(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == Sagegame){
			verifySuccess = verifySagegame(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == _1tsdk || recvCmd->sdkid() == _1tsdk_){
			verifySuccess = verify1tsdk(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == _3hgame){
			verifySuccess = verify3hgame(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);	
		}else if(recvCmd->sdkid() == _01game){
			verifySuccess = verify01game(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == Quicksdk){
			verifySuccess = verifyQuicksdk(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == QuicksdkHW){
			verifySuccess = verifyQuicksdkHW(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == BR_sdk){
			verifySuccess = verifyBRsdk(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == Xinyu){
			verifySuccess = verifyXinyu(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == DongNanYa){
			verifySuccess = verifyDongNanYa(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);	
		}else if(recvCmd->sdkid() == HuaWei){
			verifySuccess = verifyHuaWei(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == _51sfsy){
			verifySuccess = verify51sfsy(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == H365 || recvCmd->sdkid() == NftWallet){
			verifySuccess = verifyCommon(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == Laosiji){
			verifySuccess = verifyLaosiji(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		}else if(recvCmd->sdkid() == GongKou){
			verifySuccess = verifyGongKou(recvCmd->sdkid(), recvCmd->extradata(),tokenInfo);
		} else {
            verifySuccess = verifyCommon(recvCmd->sdkid(), recvCmd->extradata(), tokenInfo);
        }
    } else {
		Athena::logger->debug("[游戏验证帐号]自动重连, sdk id %d 帐号验证", recvCmd->sdkid());
        if (g_gateServer->isInnerVerify()){
            verifySuccess = verifyToken(recvCmd->token().c_str(),tokenInfo);	
        }
        else {
            verifySuccess = formatVerifyToken(recvCmd->token().c_str(),tokenInfo); 
        }
    }

	if (verifySuccess){
		Athena::logger->debug("[自动重连验证成功]account=%s,deviceID=%s",tokenInfo.m_account,recvCmd->deviceid().c_str());
		GateUser * pUser = GateUserManager::getSingleton().getUserByAccount(tokenInfo.m_account);
		if (!pUser){//没有在游戏中,要重新登录
			Login::stNotifyRelogin retCmd;
			GateUserConnMgr::getSingleton().sendPacket2Commponet(pCom,retCmd);
			GateUserConnMgr::getSingleton().writeFinishCloseComponent(pCom);
			Athena::logger->error("没有在服务器中,需要重新登录!");
			return ;	
		}
		
		if (pUser->m_bNeedRelogin){
			pUser->m_bNeedRelogin = false;
			Login::stNotifyRelogin retCmd;
			GateUserConnMgr::getSingleton().sendPacket2Commponet(pCom,retCmd);
			GateUserConnMgr::getSingleton().writeFinishCloseComponent(pCom);
			Athena::logger->error("服务器需要(account:%s,name:%s,id:%d)重新登录!",pUser->m_account,pUser->getName(),pUser->getID());
			return ;	
		}

		GateUserConnMgr::getSingleton().chgTCPComponent(tokenInfo.m_account,pCom);
		GateUserManager::getSingleton().removeLoginOutAccount(tokenInfo.m_account);
		GateUserManager::getSingleton().cancelLogout(tokenInfo.m_account);

		Global::stPlayerAutoConnect reloginCmd;
		reloginCmd.set_account(tokenInfo.m_account);
		reloginCmd.set_charid(pUser->getID());
		reloginCmd.set_device(recvCmd->deviceid());
		reloginCmd.set_ip(tokenInfo.m_ip);
		if(!GlobalSvrClientMgr::getSingleton().broadcastProto2All(reloginCmd)){
			Athena::logger->trace("向Global发送协议不成功,重新登录失败 account=%s",tokenInfo.m_account);
			GateUserConnMgr::getSingleton().disconnectComponent(pCom);
			return ;
		}
	}
}

void UserLoginModHandle::queryPingValue(TCPComponent * pCom,Login::stQueryPingValue * recvCmd)
{
	if (!recvCmd){
		return ;
	}
	
	Login::stQueryPingValue retCmd;
	retCmd.set_curtime(recvCmd->curtime());
	GateUserConnMgr::getSingleton().sendPacket2Commponet(pCom,retCmd);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("收到了来自客户端的心跳:%d",recvCmd->curtime());
#endif
}

void UserLoginModHandle::recClientTime(TCPComponent * pCom,Login::stSendClient2Svr * recvCmd)
{
	std::string account = GateUserConnMgr::getSingleton().getAccountByIoc(pCom);
	GateUser * pUser = GateUserManager::getSingleton().getUserByAccount(account);
	if (pUser){
		pUser->m_bReqClientTime = false;
		pUser->m_clientTime = recvCmd->curtime();
		pUser->m_recvClientTime = g_gateServer->getMSec();
#ifdef _HDZ_DEBUG
		Athena::logger->trace("收到了玩家(account:%s,name:%s,id:%d)返回的客户端时间:%d",pUser->m_account,pUser->getName(),pUser->getID(),recvCmd->curtime());
#endif
	}
}
