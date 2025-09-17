#ifndef __LEVELWFLFARE_CFG_H__
#define __LEVELWFLFARE_CFG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

struct stWflfareInfo
{
	UINT16	m_type;			//奖励类型
	UINT32	m_costDiamond;	//需求钻石
	std::map<UINT32, UINT32> m_rewardItem;	//对应item表ID

	stWflfareInfo()
	{
		m_type = 0;
		m_costDiamond = 0;
	}
};

class CLevelWflfareCfg
{
	USE_SINGLETON_NEW(CLevelWflfareCfg);
public:
	//构造函数
	CLevelWflfareCfg();
	//析构函数
	~CLevelWflfareCfg();
	//获得实例
	static CLevelWflfareCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	
	//加载配置
	bool loadCfg();

	//通过等级获取福利配置
	bool getWflfareInfoByLevel(std::map<UINT16, stWflfareInfo> & rewardMap, UINT16 level);

	//获取配置
	const std::map<UINT16, std::map<UINT16, stWflfareInfo> > & getConfig() const
	{
		return m_reward;
	}
	//获取奖励数量
	UINT16 getWflfareSize()
	{
		return m_size;
	}
private:
	std::map<UINT16, std::map<UINT16, stWflfareInfo> > m_reward;//first:等级  second first:奖励类型
	UINT16 m_size;
};

#endif
