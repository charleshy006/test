#include "RedisOpHelp.h"
#include "Log4cxxHelp.h"
#include "GlobalServer.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "AthenaNew.h"
#include "RedisOperation.h"
#include "xRedisClient.h"
#include "RedisDelayThreadPool.h"

extern GlobalServer * g_globalServer;

void CRedisOpHelp::testGet(std::string & key)
{
	Athena::RedisQueryCallback<CRedisOpHelp> * pCallBack = ATHENA_NEW Athena::RedisQueryCallback<CRedisOpHelp>(this,&CRedisOpHelp::testGetCB,NULL);
	CQueryRedisOperation * pDelaySql = ATHENA_NEW CQueryRedisOperation(pCallBack);
	pDelaySql->setKey(key);
	CRedisDelayThreadPool::getSingleton().delay(pDelaySql);
}
	
void CRedisOpHelp::testGetCB(std::string * value)
{
	Athena::logger->trace("从redis中读出的数据是%s",value->c_str());
}

