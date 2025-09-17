#ifndef __H_DROPMGR_H__
#define __H_DROPMGR_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <vector>

class GameUser;
class CDropMgr
{
	USE_SINGLETON_NEW(CDropMgr);
public :
	struct stDropEntry
	{
		UINT16 m_entryID;			//实体ID
		UINT32 m_itemID;			//道具ID
		UINT32 m_num;				//道具数量
		UINT32 m_rate;				//概率

		stDropEntry()
		{
			m_entryID = 0;
			m_itemID = 0;
			m_num = 0;
			m_rate = 0;
		}
	};

	struct stDropGroup
	{
		UINT32 m_id;								//掉落组ID
		UINT8  m_certainType;						//比较掉落的类型,0:全部,1:随机
		std::vector<stDropEntry> m_certainVec;		//必定掉落
		std::vector<stDropEntry> m_probabilityVec;	//随机掉落

		stDropGroup()
		{
			m_id = 0;	
			m_certainType = 0;
		}
	};
public :
	//构造函数 
	static CDropMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CDropMgr()
	{
	}
	//加载配置 
	bool loadCfg();
	//随机出一个道具
	INT32  randomFrom(std::vector<stDropEntry> & inVec);
	//某个掉落组掉落
	void processDropGroup(UINT32 dropID,std::vector<stDropEntry> &outVec,UINT32 drawNum);
private :
	CDropMgr()
	{
	
	}
private :
	std::map<UINT32,stDropGroup>  m_groups;
};
#endif
