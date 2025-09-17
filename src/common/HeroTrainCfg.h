#ifndef __HEROTRAIN_CFG_H__
#define __HEROTRAIN_CFG_H__

#include "TypesDef.h"
#include "Singleton.h"
#include <map>
#include <vector>
#include <tinyxml/tinyxml.h>

enum
{
	TRAIN_LEVELUP_AWARD_CG = 1,
	TRAIN_LEVELUP_AWARD_ITEM = 2,
};

struct stTrainLvAwardItem
{
	UINT8	m_type;
	UINT32	m_id;
	UINT32	m_count; 

	stTrainLvAwardItem()
	{
		m_type		=	0;
		m_id		=	0;
		m_count		=	0; 
	}
};

struct stTrainLvAward
{ 
	UINT32	m_heroID;
	UINT32	m_level; 
	std::vector<stTrainLvAwardItem>		m_awards;

	stTrainLvAward()
	{
		m_heroID	=	0;
		m_level		=	0;
	}
};

class CHeroTrainCfg
{  
	USE_SINGLETON_NEW(CHeroTrainCfg);
public:
	//构造函数 
	CHeroTrainCfg() {}
	//析构函数
	~CHeroTrainCfg() {}
	//获得实例                              
	static CHeroTrainCfg & getSingleton() {
		return THE_SINGLETON::Instance();  
	}    
	//获取最大调教等级
    UINT32 getMaxHeroTrainLvl() {
        return m_maxHeroTrainLvl;
    }
	//获取初始调教等级
    UINT32 getInitHeroTrainLvl() {
        return m_initHeroTrainLvl;
    }

	stTrainLvAward* GetAwardCfg(UINT32 heroid, UINT32 level);

	//加载配置
	bool loadCfg();
private:
	std::map<UINT32,std::map<UINT32,stTrainLvAward> > m_trainLvAwards;	//<heroID,<trainlv,stTrainLvAward>> 
	UINT32 m_initHeroTrainLvl;							//随从初始调教等级
    UINT32 m_maxHeroTrainLvl;							//随从最高调教等级
};

#endif
