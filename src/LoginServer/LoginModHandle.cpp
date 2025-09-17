#include "LoginModHandle.h"
#include "SvrInfo.h"
#include "md5.h"
#include "LoginInfo.h"
#include "UserManager.h"
#include "json/writer.h"
#include "UtilFunction.h"
#include "md5.h"
#include "KVPair.h"

extern ClientBinPacketSpliter * g_spliter;

LoginModHandle::LoginModHandle()
{
	registerHandle(&LoginModHandle::verifyAccount);
}

LoginModHandle::~LoginModHandle()
{

}

bool LoginModHandle::handle(TCPComponent * pCom,const char * pData,const UINT32 & cmdLen,UINT8 funID)
{
	return ProtoMsgHandle<LoginModHandle>::handle(pCom,pData,cmdLen,funID); 
}

bool LoginModHandle::md5String(const char * account,const UINT32 & userID,const UINT32 & curTime,char * outStr)
{
	if (!account || !outStr){
		return false;	
	}
	
	//static std::string  key = "72abf593384ba09ae41ac8ad6b435d4a";
	std::map<std::string,std::string> kvSortMap;
	kvSortMap["user_id"] = g_int2string(userID);
	kvSortMap["time"] = g_int2string(curTime);
	kvSortMap["osdk_user_id"] = account;
	
	stSpecialAccountInfo *pInfo = UserManager::getSingleton().getAccountInfo(account);
	if (pInfo){
		kvSortMap["ip"] = pInfo->m_ip;	
	}

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
    if (Athena::global["isNotUuzu"] == std::string("true")) {
        os << Athena::global["server_secret"];
    } else {
        os << SERVER_SECRET;
    }
#ifdef _HDZ_DEBUG
	Athena::logger->trace("用来计算md5的字符串是:%s",os.str().c_str());
#endif
	char tokenBuf[1024] = {0};
	int  bufLen = 0;
	snprintf(tokenBuf,sizeof(tokenBuf) - 1,"%s",os.str().c_str());
	bufLen = strlen(tokenBuf);
	utils::Md5(tokenBuf,bufLen,outStr);
	utils::HexString::bin2asc(outStr,16,outStr,64,bufLen);
	outStr[bufLen] = 0;
	return true;
}

void LoginModHandle::verifyAccount(TCPComponent * pCom,Login::ReqLogin * reqLogin)
{
	if (!pCom||!reqLogin) {
		return ;
	}

	Athena::logger->trace("收到的帐号是:%s,密码是%s",reqLogin->account().c_str(),reqLogin->password().c_str());
	Login::RetLoginSucess  retCmd;
	LoginInfo  loginInfo;
	if (!UserManager::getSingleton().loginVerify(loginInfo,reqLogin->account().c_str(),reqLogin->password().c_str())){
		Athena::logger->trace("(account=%s,password=%s)帐号密码不正确,登录失败",reqLogin->account().c_str(),reqLogin->password().c_str());
		Login::LoginError errorCmd;
		errorCmd.set_errortype(Login::LoginError::PASSWORD_WRONG);
		SEND_PROTO_2_CLIENT(pCom,errorCmd);
		return ;
	}
	char digest[64] = {0};
	RealTime cur;
	md5String(reqLogin->account().c_str(),loginInfo.m_userID,cur.sec(),digest);
	stSpecialAccountInfo *pInfo = UserManager::getSingleton().getAccountInfo(reqLogin->account());
	Json::FastWriter writer;
	Json::Value root;
	root["user_id"] = g_int2string(loginInfo.m_userID);
	root["osdk_user_id"] = reqLogin->account().c_str();
	root["time"] = cur.sec();
	if (pInfo){
		root["ip"] = pInfo->m_ip;
	}
	root["sign"] = digest;
	std::string content = writer.write(root);
	content = g_encode64(content);
	//char loginToken[128] = {0};
	//snprintf(loginToken,sizeof(loginToken),"%d,%s,%d,%s",loginInfo.m_userID,reqLogin->account().c_str(),cur.sec(),digest);
	//retCmd.set_token(loginToken);
	retCmd.set_token(content);
	if (pInfo){
		retCmd.set_ip(pInfo->m_ip);
		retCmd.set_device(pInfo->m_device);
		retCmd.set_opid(pInfo->m_opID);
	}

	SEND_PROTO_2_CLIENT(pCom,retCmd);
	Athena::logger->trace("account=%s 登录成功!",reqLogin->account().c_str());
}
