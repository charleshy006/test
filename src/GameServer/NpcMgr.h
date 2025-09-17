#ifndef __NPC_MGR_H__
#define __NPC_MGR_H__

#include "Npc.h"
#include "UniqueID.h"
#include "AthenaNew.h"
#include "ElementMgr.h"
#include "Singleton.h"
#include <map>

//NPC全局管理器
class CNpcMgr  : public ElementMgr
{
	USE_SINGLETON_NEW(CNpcMgr)
public:
	//根据唯一ID获得Npc
	CNpc* getNpcByTempID(UINT32 tempID);
	//增加一个NPC到管理器中
	bool add(CNpc*);
	//删除NPC
	bool remove(CNpc *);
	//获得NPC个数
	UINT32 getNpcSize()
	{
		return size();
	}
	//获得一个临时ID
	bool genTempID(UINT32 & tempID);
	//回收一个临时ID
	void   putBackID(UINT32 tempID);
public:
	static CNpcMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	static void destroyMe()
	{
		THE_SINGLETON::destroy();	
	}
	//npc管理器构造
	CNpcMgr();
	//npc管理器析构 
	~CNpcMgr();
private:
	//ID分配器
	UniqueIDMgr<UINT32> m_tempIDAllocater;//ID分配器
};
#endif
