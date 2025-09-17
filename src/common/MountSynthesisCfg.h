#ifndef __MOUNTSYNTHESIS_CFG_H__
#define __MOUNTSYNTHESIS_CFG_H__

#include "TypesDef.h"
#include "Singleton.h"
#include <map>
#include <vector>
#include <tinyxml/tinyxml.h>

struct stSynthesis
{
	UINT32	m_id;
	UINT32	m_itemID;		//合成的物品ID
	UINT32	m_resultitemID;	//合成后的物品ID
	UINT8	m_synthesisNum;	//需要的物品数量
	UINT32	m_cost;			//需要的金币数量

	stSynthesis()
	{
		m_id			=	0;
		m_itemID		=	0;
		m_resultitemID	=	0;
		m_synthesisNum	=	0;
		m_cost			=	0;
	}
};

struct stEvolvement
{
	UINT8	m_id;
	UINT32	m_item1ID;
	UINT32	m_item2ID;
	UINT32	m_resultitemID;
	UINT32	m_cost;

	stEvolvement()
	{
		m_id			=	0;
		m_item1ID		=	0;
		m_item2ID		=	0;
		m_resultitemID	=	0;
		m_cost			=	0;
	}
};

class CMountSynthesisCfg
{  
	USE_SINGLETON_NEW(CMountSynthesisCfg);
public:
	//构造函数 
	CMountSynthesisCfg() {}
	//析构函数
	~CMountSynthesisCfg() {}
	//获得实例                              
	static CMountSynthesisCfg & getSingleton() {
		return THE_SINGLETON::Instance();  
	}    
	//加载配置
	bool loadCfg();
	//通过id获取合成表
	stSynthesis * getSynthesisByitemID(UINT32 itemID);
	//通过id获取强化表
	stEvolvement * getEvolvementByitemID(UINT32 item1ID, UINT32 item2ID);
private:
	std::map<UINT32, stSynthesis>	m_synthesisMap;
	std::vector<stEvolvement>		m_evolvementVec;
};

#endif
