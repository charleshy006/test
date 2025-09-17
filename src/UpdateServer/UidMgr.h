#ifndef __UIDMGR_H__
#define __UIDMGR_H__
#include "Singleton.h"
#include "net.h"

#include <set>

class UidMgr
{
	USE_SINGLETON_NEW(UidMgr);
public:
	static UidMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	static void destroyMe()
	{
		THE_SINGLETON::destroy();
    }
    UidMgr();
    virtual ~UidMgr(){ }
    //生成一个唯一id
    std::string generateUid();
    //测试id是否存在
    bool existUid(const std::string & uid);
    //删除id
    bool delUid(const std::string & uid);
private :
    Mutex m_mutex;
    std::string m_dictStr;
    std::set<std::string> m_uidSet;;
};

#endif
