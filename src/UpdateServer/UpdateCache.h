#ifndef __UPDATE_CACHE_MGR_H__
#define __UPDATE_CACHE_MGR_H__

#include "Singleton.h"
#include "net.h"

class UpdateCacheMgr
{
    USE_SINGLETON_NEW(UpdateCacheMgr);
public:
	static UpdateCacheMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
    }
	//定时回调
	void timer(UINT32 cur);
    //构造函数
    UpdateCacheMgr() { }
    //析构函数
    virtual ~UpdateCacheMgr(){ }
    //添加记录
    void addRecord(const std::string & partnerId, const std::string & version, bool totalUpdate,
                    std::map<std::string, std::string> & valueMap);
    //获取记录
    void getRecord(const std::string & partnerId, const std::string & version, bool totalUpdate,
                    std::map<std::string, std::string> & valueMap);
    //请求更新
    static int getUpdate(const std::string & partnerId, const std::string & version, bool totalUpdate,
                                std::map<std::string, std::string> & valueMap);
private :
    Mutex m_mutex;
    static std::map<std::string, std::map<std::string, std::string> > m_cacheMap;
};
#endif
