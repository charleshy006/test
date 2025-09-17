#ifdef _USE_BEHAVIAC_AI

#ifndef __H_BEHAVIAC_AI_H__
#define __H_BEHAVIAC_AI_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "behaviac/agent/agent.h"
#include "behaviac/world/world.h"
#include "behaviac/agent/registermacros.h"

class CBeing;

class CBehaviacAIGOBase : public behaviac::Agent
{
public :
	DECLARE_BEHAVIAC_OBJECT(CBehaviacAIGOBase, behaviac::Agent);
	//构造函数
	CBehaviacAIGOBase();
	//析构函数
	virtual ~CBehaviacAIGOBase();
	//每帧回调
	virtual bool doActions(float dt);
	//测试方法
	void    testMethod(UINT32 test) {}
private :
	CBeing * m_pOwner;
	UINT32   m_property;			//测试属性
};

class CBehavicAIWorldState : public behaviac::World
{
public :
	DECLARE_BEHAVIAC_OBJECT(CBehavicAIWorldState,behaviac::World);
	//构造函数
	CBehavicAIWorldState();
	//析构函数
	~CBehavicAIWorldState();
};

class CBehaviacAIMgr
{
	USE_SINGLETON_NEW(CBehaviacAIMgr)
private :
	//析构函数
	~CBehaviacAIMgr();
public :
	static const UINT32 kIdMask_Wolrd =		0x0001;
public :
	//构造函数
	CBehaviacAIMgr();
	//返回实例
	static CBehaviacAIMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();	
	}
	//初始化
	bool init();
	//定时器
	void timer(UINT32 cur);
private :
	CBehavicAIWorldState * m_pWs;
};
#endif
#endif
