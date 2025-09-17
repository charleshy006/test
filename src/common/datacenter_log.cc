#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/time.h>
#include <pthread.h>
#include "datacenter_log.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "TypesDef.h"

using namespace uuzu::logsdk;
using namespace std;

UINT32 getType(const char* logType) {
    std::string type(logType);
    if (type == "est") {
        return 1001;
    } else if (type == "login"){
        return 1002;
    } else if (type == "pay"){
        return 1020;
    } else if (type == "prop"){
        return 1021;
    } else if (type == "curr"){
        return 1031;
    } else if (type == "ser"){
        return 1033;
    }
    return 0;
}

DatacenterLog::~DatacenterLog(){
    if(serverFifoFd_!=-1)
    {
        close(serverFifoFd_);
        if(isDebug_)  cout<<"close fd!"<<endl;
    }
}


int32_t DatacenterLog::sendDataLog(const UserInfo& userInfo,
        const char* str1, const char* str2, const char* str3, const char* str4,
        const char* str5, const char* str6, const char* str7, const char* str8,
        const char* logType, int count) {
    if (Athena::global["isNotUuzu"] == std::string("true")) {
        return sendDataLogKorea(userInfo, str1, str2, str3, str4, str5,
                    str6, str7, str8, logType, count);
    }
    if(strcmp(logType,"est")==0){
        if(userInfo.passport.length()==0){
            errorNo_ = 3;
            cout<<"check error!!"<<endl;
            return -1;
        }
    }else{
        if(userInfo.passport.length()==0||userInfo.uid.length()==0){
            errorNo_ = 3;
            return -1;
        }
    }

    string log = getServerInfoString(logType);
    log += SEPARATOR+userInfo.passport+SEPARATOR+userInfo.uid;
    log += SEPARATOR+convertToString(count);

	log += SEPARATOR;
	log += str1;
	log += SEPARATOR;
	log += str2;
	log += SEPARATOR;
	log += str3;
	log += SEPARATOR;
	log += str4;
	log += SEPARATOR;
    log += str5;
	log += SEPARATOR;
	log += str6;
	log += SEPARATOR;
	log += str7;
	log += SEPARATOR;
	log += str8;
    log += SEPARATOR;

	log += getUserInfoString(userInfo);
    if(isDebug_) cout<<logType <<endl<<log<<endl;
	string str(gid_);
	str += "_";
	str += logType;

    return  sendLog(str.c_str(), log.c_str());

}


int32_t DatacenterLog::sendDataLogKorea(const UserInfo& userInfo,
        const char* str1, const char* str2, const char* str3, const char* str4,
        const char* str5, const char* str6, const char* str7, const char* str8,
        const char* logType, int count) {
    if(strcmp(logType,"est")==0){
        if(userInfo.passport.length()==0){
            errorNo_ = 3;
            cout<<"check error!!"<<endl;
            return -1;
        }
    }else{
        if(userInfo.passport.length()==0||userInfo.uid.length()==0){
            errorNo_ = 3;
            return -1;
        }
    }

    string log = getServerInfoStringKorea(logType);
    log += SEPARATOR_LEFT + userInfo.passport + SEPARATOR_RIGHT;
    log += SEPARATOR_LEFT + userInfo.uid + SEPARATOR_RIGHT;
    log += SEPARATOR_LEFT + convertToString(count) + SEPARATOR_RIGHT;

	log += SEPARATOR_LEFT + str1 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str2 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str3 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str4 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str5 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str6 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str7 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str8 + SEPARATOR_RIGHT;

	log += getUserInfoStringKorea(userInfo);
    if(isDebug_) cout<<logType <<endl<<log<<endl;

    return sendLog(log.c_str());
}


int32_t DatacenterLog::sendServerLog(const char* str1, const char* str2, const char* str3, const char* str4,
        const char* str5, const char* str6, const char* str7, const char* str8,
        const char* logType, int count){
    if (Athena::global["isNotUuzu"] == std::string("true")) {
        return sendServerLogKorea(str1, str2, str3, str4, str5, str6, str7, str8, logType, count);
    }
    string log = getServerInfoString(logType);
    log += SEPARATOR+SEPARATOR+SEPARATOR+convertToString(count);

    log += SEPARATOR;
    log += str1;
    log += SEPARATOR;
    log += str2;
    log += SEPARATOR;
    log += str3;
    log += SEPARATOR;
    log += str4;
    log += SEPARATOR;
    log += str5;
    log += SEPARATOR;
    log += str6;
    log += SEPARATOR;
    log += str7;
    log += SEPARATOR;
    log += str8;
    log += SEPARATOR;

    //log += getUserInfoString(userInfo);
    if(isDebug_) cout<<logType <<endl<<log<<endl;
    string str(gid_);
    str += "_";
    str += logType;

    return  sendLog(str.c_str(), log.c_str());

}

int32_t DatacenterLog::sendServerLogKorea(const char* str1, const char* str2, const char* str3, const char* str4,
        const char* str5, const char* str6, const char* str7, const char* str8,
        const char* logType, int count){
    string log = getServerInfoStringKorea(logType);
    log += SEPARATOR_LEFT + SEPARATOR_RIGHT;
    log += SEPARATOR_LEFT + SEPARATOR_RIGHT;
    log += SEPARATOR_LEFT + convertToString(count) + SEPARATOR_RIGHT;

	log += SEPARATOR_LEFT + str1 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str2 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str3 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str4 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str5 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str6 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str7 + SEPARATOR_RIGHT;
	log += SEPARATOR_LEFT + str8 + SEPARATOR_RIGHT;

    for (size_t i = 0; i < 12; i++) {
        log += SEPARATOR_LEFT + SEPARATOR_RIGHT;
    }
    //log += getUserInfoString(userInfo);
    if(isDebug_) cout<<logType <<endl<<log<<endl;

    return sendLog(log.c_str());
}

bool DatacenterLog::writeLocal(const char* category, const char* log){

    time_t t = time(0);
    char dateStr[10];
    strftime(dateStr,sizeof(dateStr),"%Y%m%d",localtime(&t));

    string path(DATA_BUFFER_PATH);
    path += dateStr;
    path += "/";

    DIR *dir = NULL;
    dir = opendir(path.c_str());
    if(NULL == dir){
        mkdir(path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }else
    {
        closedir(dir);
    }

    string filename(path);
    filename += category;

    bool ret = true;
    try{
        ofstream in;
        in.open(filename.c_str(),fstream::app);
        pthread_mutex_lock(&mutex);
        //in.write(log);
        in << log << endl;
        pthread_mutex_unlock(&mutex);
        in.close();
    }catch (std::ofstream::failure e) {
        ret = false;
    }
    return ret;

}

bool DatacenterLog::writeLocal(const char* log){

    time_t t = time(0);
    t += 3600;//00-23 record last hour data
    char dateStr[30];
    strftime(dateStr,sizeof(dateStr),"dh-%Y-%m-%d-%H.log",localtime(&t));

    string path(Athena::global["LogDir"]);
    path += "/";

    DIR *dir = NULL;
    dir = opendir(path.c_str());
    if(NULL == dir){
        mkdir(path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }else
    {
        closedir(dir);
    }

    string filename(path);
    filename += string(dateStr);

    bool ret = true;
    try{
        ofstream in;
        pthread_mutex_lock(&mutex);
        in.open(filename.c_str(),fstream::app);
        //in.write(log);
        in << log << endl;
        in.close();
        pthread_mutex_unlock(&mutex);
    }catch (std::ofstream::failure e) {
        ret = false;
    }
    return ret;

}

int32_t DatacenterLog::sendLog(const char* log){
    if (writeLocal(log)) {
        return 0;//不发送到远端，写在本地
    } else {
        errorNo_ = 4; //发送远端失败，写本地失败
        return -1;
    }
}

int32_t DatacenterLog::sendLog(const char* category, const char* log){
    //向管道中写入数据：发送.
    string msg = category;
	msg += ";";
	msg += log;
    int num = write(serverFifoFd_ , msg.c_str(), msg.length() + 1);
    if(isDebug_) cout<<"write "<<num<<" "<<msg<<endl;
    /*
    if(num==-1){
        errorNo_ = 2;
        return -1;
    }
    return num;
    */

    //重新初始化，再尝试一次
    if(num==-1){
        init(pid_.c_str(),sid_.c_str(),gid_.c_str(),opid_.c_str(),isDebug_);
        num = write(serverFifoFd_ , msg.c_str(), msg.length() + 1);
        //if(isDebug_) cout<<"second write "<<num<<" "<<log<<endl;
        if(num==-1){
            bool isWrite = writeLocal(category, log);
            //if(isDebug_) cout<<"writeLocal "<<log<<endl;
            if(isWrite){
                return 0;//不发送到远端，写在本地
            }else{
                errorNo_ = 4; //发送远端失败，写本地失败
                return -1;
            }

        }else{
            return num;
        }
    }else{
        return num;
    }
}


std::string DatacenterLog::getServerInfoStringKorea(const char* logType) {
    std::string str = SEPARATOR_LEFT + convertToString(getType(logType)) + SEPARATOR_RIGHT;
    str += SEPARATOR_LEFT + std::string(logType) + SEPARATOR_RIGHT;
	str += SEPARATOR_LEFT + pid_ + SEPARATOR_RIGHT;
	str += SEPARATOR_LEFT + gid_ + SEPARATOR_RIGHT;
	str += SEPARATOR_LEFT + sid_ + SEPARATOR_RIGHT;
	str += SEPARATOR_LEFT + opid_ + SEPARATOR_RIGHT;
	str += SEPARATOR_LEFT + convertToString(getUnixTimestamp())+ SEPARATOR_RIGHT;
	return str;
}


std::string DatacenterLog::getServerInfoString(const char* logType) {
	std::string str(logType);
	str += SEPARATOR;
	str += pid_;
	str += SEPARATOR;
	str += gid_;
	str += SEPARATOR;
	str += sid_;
	str += SEPARATOR;
    str += opid_;
	str += SEPARATOR;
	str += convertToString(getUnixTimestamp());
	return str;
}

string  DatacenterLog::getUserInfoStringKorea(const UserInfo& userInfo){
   string level = "";
   if(userInfo.level!=UIU_NUM){
       level = convertToString(userInfo.level);
   }
    string viplevel = "";
    if(userInfo.viplevel!=UIU_NUM){
        viplevel = convertToString(userInfo.viplevel);
    }
    string moneycoin = "";
    if(userInfo.moneycoin!=UIU_NUM){
        moneycoin = convertToString(userInfo.moneycoin);
    }
    string blackcoin = "";
    if(userInfo.blackcoin!=UIU_NUM){
        blackcoin = convertToString(userInfo.blackcoin);
    }

    string is_test = "";
    if(userInfo.is_test!=UIU_NUM){
        is_test = convertToString(userInfo.is_test);
    }

    string exp = "";
    if(userInfo.exp!=UIU_NUM){
        exp = convertToString(userInfo.exp);
    }

    string firstintime = "";
    if(userInfo.firstintime!=UIU_NUM){
        firstintime = convertToString(userInfo.firstintime);
    }

    string gamecoin = "";
    if(userInfo.gamecoin!=UIU_NUM){
        gamecoin = convertToString(userInfo.gamecoin);
    }


    return SEPARATOR_LEFT + userInfo.clientip + SEPARATOR_RIGHT + SEPARATOR_LEFT + level + SEPARATOR_RIGHT +
            SEPARATOR_LEFT + viplevel + SEPARATOR_RIGHT + SEPARATOR_LEFT + moneycoin + SEPARATOR_RIGHT +
            SEPARATOR_LEFT + blackcoin + SEPARATOR_RIGHT + SEPARATOR_LEFT + is_test + SEPARATOR_RIGHT +
            SEPARATOR_LEFT + userInfo.version + SEPARATOR_RIGHT + SEPARATOR_LEFT + userInfo.entrance +
            SEPARATOR_RIGHT + SEPARATOR_LEFT + userInfo.device + SEPARATOR_RIGHT + SEPARATOR_LEFT + exp +
            SEPARATOR_RIGHT + SEPARATOR_LEFT + firstintime + SEPARATOR_RIGHT + SEPARATOR_LEFT + gamecoin +
            SEPARATOR_RIGHT;
}


string  DatacenterLog::getUserInfoString(const UserInfo& userInfo){
   string level = "";
   if(userInfo.level!=UIU_NUM){
       level = convertToString(userInfo.level);
   }
    string viplevel = "";
    if(userInfo.viplevel!=UIU_NUM){
        viplevel = convertToString(userInfo.viplevel);
    }
    string moneycoin = "";
    if(userInfo.moneycoin!=UIU_NUM){
        moneycoin = convertToString(userInfo.moneycoin);
    }
    string blackcoin = "";
    if(userInfo.blackcoin!=UIU_NUM){
        blackcoin = convertToString(userInfo.blackcoin);
    }

    string is_test = "";
    if(userInfo.is_test!=UIU_NUM){
        is_test = convertToString(userInfo.is_test);
    }

    string exp = "";
    if(userInfo.exp!=UIU_NUM){
        exp = convertToString(userInfo.exp);
    }

    string firstintime = "";
    if(userInfo.firstintime!=UIU_NUM){
        firstintime = convertToString(userInfo.firstintime);
    }

    string gamecoin = "";
    if(userInfo.gamecoin!=UIU_NUM){
        gamecoin = convertToString(userInfo.gamecoin);
    }


    return userInfo.clientip+SEPARATOR+level+SEPARATOR+viplevel+SEPARATOR+moneycoin+SEPARATOR+blackcoin+SEPARATOR+
           is_test+SEPARATOR+userInfo.version+SEPARATOR+userInfo.entrance+SEPARATOR+userInfo.device+SEPARATOR+
           exp+SEPARATOR+firstintime+SEPARATOR+gamecoin;
}

unsigned int DatacenterLog::getUnixTimestamp() {
    return (int)time(NULL);
}

bool DatacenterLog::init(const char* pid, const char* sid, const char* gid,bool isDebug) {

    pid_ = pid;
    sid_ = sid;
    gid_ = gid;
    opid_ = pid;
    isDebug_ = isDebug;

    if(serverFifoFd_==-1){
        if(isDebug_) cout<<"start open pipe"<<endl;
        int serverFifoFd = open(serverFIFOName_.c_str(),O_WRONLY | O_NONBLOCK);
        if(serverFifoFd == -1)
        {
            errorNo_ = 1;
            if(isDebug_) cout<<"open fd error!"<<endl;
            return false;
        }else{
            serverFifoFd_ =  serverFifoFd;
            if(isDebug_) cout<<"set fd ="<<serverFifoFd<<endl;
        }
    }
    return true;
}

bool DatacenterLog::init(const char* pid, const char* sid, const char* gid, const char* opid,bool isDebug) {

    pid_ = pid;
    sid_ = sid;
    gid_ = gid;
	opid_ = opid;
    isDebug_ = isDebug;

    if(serverFifoFd_==-1){
        if(isDebug_) cout<<"start open pipe"<<endl;
        int serverFifoFd = open(serverFIFOName_.c_str(),O_WRONLY | O_NONBLOCK);
        if(serverFifoFd == -1)
        {
            errorNo_ = 1;
            if(isDebug_) cout<<"open fd error!"<<endl;
            return false;
        }else{
            serverFifoFd_ =  serverFifoFd;
            if(isDebug_) cout<<"set fd ="<<serverFifoFd<<endl;
        }
    }
    return true;
}




