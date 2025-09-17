#ifndef __SERVER_LISTENER_H__
#define __SERVER_LISTENER_H__

#include "Singleton.h"

class ServerListener
{
	USE_SINGLETON_NEW(ServerListener);
public:
	static ServerListener & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}
    ServerListener();
    ~ServerListener() { }
    //报告自身信息
    void notifySelf();
	//定时器
	void timer(UINT32 cur);
    //校验
    bool verifySign(std::vector<std::string> &vec,std::string &sign);

private:
    std::string m_ip;
};
#endif
