#ifndef __REDISOPHELP_H__
#define __REDISOPHELP_H__
#include "Singleton.h"
#include <string>

class CRedisOpHelp 
{
	USE_SINGLETON_NEW(CRedisOpHelp);
public:
	static CRedisOpHelp & getSingleton()
	{
		 return THE_SINGLETON::Instance();
	}
	
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}
	
	void testGet(std::string & key);
	
	void testGetCB(std::string * value);

	CRedisOpHelp() {};
	~CRedisOpHelp() {}
};
#endif
