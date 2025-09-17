#ifndef __H_GAMEVERSION_CFG_H__
#define  __H_GAMEVERSION_CFG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <map>

class CGameVersionCfg
{
	USE_SINGLETON_NEW(CGameVersionCfg);
private :
	//构造函数
	CGameVersionCfg();
public :
	//析构函数
	~CGameVersionCfg();
	//获得实例
	static CGameVersionCfg & getSingleton()
	{   
		return THE_SINGLETON::Instance();
	} 
	//加载所有配置
	bool loadCfg();
	//获得游戏客户端最新版本
	UINT32 getMinGameVersion()
	{
		return m_minGameVersion;
	}
private :
	UINT32 m_minGameVersion;					//最新版本
};
#endif
