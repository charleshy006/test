#ifndef DATACENTER_LOG_H_
#define DATACENTER_LOG_H_

#include <sys/types.h>
#include <string>
#include <iostream>
#include <sstream>

#define FIFO_NAME "/tmp/youzu_scribe_agent.fifo"
#define DATA_BUFFER_PATH "/data/data/scribelog/local/"
#define UIU_NUM -1 //userinfo undefined用户信息数字未定义

namespace uuzu{ namespace logsdk{
	struct UserInfo{
		std::string uid;
		std::string passport;
		int level; //等级
		int viplevel; //vip等级
		long moneycoin; //付费货币，如元宝
		long blackcoin; //赠送货币
        long gamecoin;  //免费货币
	    int time; //发生时间戳
        long exp; //经验值
        int is_test;//是否测试用户
        std::string version; //版本
        std::string entrance;//入口
        std::string device; //设备表示
        std::string clientip;//客户端ip
        int firstintime; //首次进入时间
        UserInfo():uid(""),passport(""),level(UIU_NUM),viplevel(UIU_NUM),moneycoin(UIU_NUM),blackcoin(UIU_NUM),gamecoin(UIU_NUM),
        exp(UIU_NUM),is_test(UIU_NUM),clientip("127.0.0.1"),firstintime(UIU_NUM){
        }
	};

    class DatacenterLog{

    private:
        int32_t errorNo_; // 1 打开FIFO错误 2 写入错误 3
        int32_t	sendCount_;
        int serverFifoFd_;

        std::string clientIP_;
        //服务器信息
        std::string pid_;
        std::string sid_;
        std::string gid_;
		std::string opid_;
        std::string serverFIFOName_;
        std::string SEPARATOR;
        std::string SEPARATOR_LEFT;
        std::string SEPARATOR_RIGHT;
        bool  isDebug_;

    public:
        DatacenterLog(): errorNo_(0) ,sendCount_(0) , serverFifoFd_(-1) ,serverFIFOName_(FIFO_NAME),SEPARATOR("|"),
        SEPARATOR_LEFT("["), SEPARATOR_RIGHT("]"), isDebug_(false)
		{
			pthread_mutexattr_t attr;
			::pthread_mutexattr_init(&attr);
			::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_FAST_NP);
			::pthread_mutex_init(&mutex, &attr);
        }

        ~DatacenterLog();

        bool init(const char* pid,  // 开服系统中运营商id，1=uuzu
                const char* sid, //游戏服务器serverid
                const char* gid, //开服系统中游戏id
                bool isDebug = false);

        bool init(const char* pid,  // 开服系统中运营商id，1=uuzu
        	const char* sid, //游戏服务器serverid
        	const char* gid, //开服系统中游戏id
			const char* opid,//平台id
        	bool isDebug = false);

        int getLastErrorNo(){
              return errorNo_;
        }

        void setDebug(bool isDebug){
            isDebug_ = isDebug;
        }
        void setOpid(const char* opid){
            opid_ = opid;
        }
		//设置运营商ID
		void setPID(const char * pid)
		{
			pid_ = pid;
		}
        std::string  getServerInfoString(const char* logType="login");
        std::string  getServerInfoStringKorea(const char* logType="login");
        std::string  getUserInfoString(const UserInfo& userInfo);
        std::string  getUserInfoStringKorea(const UserInfo& userInfo);

        void setClientIP(std::string clientIP){
            clientIP_ = clientIP;
        }
        int32_t sendDataLog(const UserInfo& userInfo ,
                            const char* str1, const char* str2, const char* str3, const char* str4,
        			        const char* str5, const char* str6, const char* str7, const char* str8,
                            const char* logType = "login", int count = 1);

        int32_t sendDataLogKorea(const UserInfo& userInfo ,
                                const char* str1, const char* str2, const char* str3, const char* str4,
                                const char* str5, const char* str6, const char* str7, const char* str8,
                                const char* logType = "login", int count = 1);

        int32_t sendServerLog(const char* str1, const char* str2, const char* str3, const char* str4="",
                const char* str5="", const char* str6="", const char* str7="", const char* str8="",
                const char* logType = "ser", int count=1);

        int32_t sendServerLogKorea(const char* str1, const char* str2, const char* str3, const char* str4="",
                const char* str5="", const char* str6="", const char* str7="", const char* str8="",
                const char* logType = "ser", int count=1);

    private:
        pthread_mutex_t mutex;
        template <class T>
        std::string convertToString(T value) {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        bool writeLocal(const char* log);
        bool writeLocal(const char* category, const char* log);
        unsigned int getUnixTimestamp();
        int32_t sendLog(const char* log);
        int32_t sendLog(const char* category, const char* log);
    };

}};
#endif /* DATACENTER_LOG_H_ */
