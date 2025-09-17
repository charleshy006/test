#ifndef __HERO_MGR_H__
#define __HERO_MGR_H__
#include "Hero.h"
#include "UniqueID.h"
#include "AthenaNew.h"
#include "ElementMgr.h"
#include "Singleton.h"
#include <map>

//Hero全局管理器
class CHeroMgr  : public ElementMgr
{
	USE_SINGLETON_NEW(CHeroMgr)
public:
	//根据唯一ID获得Npc
	CHero* getHeroByTempID(UINT32 tempID);
	//增加一个NPC到管理器中
	bool add(CHero * );
	//删除NPC
	bool remove(CHero *);
	//获得NPC个数
	UINT32 getNpcSize()
	{
		return size();
	}
	//获得一个临时ID
	bool genTempID(UINT32 & tempID);
	//回收一个临时ID
	void   putBackID(UINT32 tempID);
	//清空某个玩家的宠物
	void   clearHeroByOwerID(UINT32 charID);
public:
	static CHeroMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	static void destroyMe()
	{
		THE_SINGLETON::destroy();	
	}
	//npc管理器构造
	CHeroMgr();
	//npc管理器析构 
	~CHeroMgr();
private:
	//ID分配器
	UniqueIDMgr<UINT32> m_tempIDAllocater;//ID分配器
};
#endif
