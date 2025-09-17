#ifndef __H_AISTATE__H__
#define __H_AISTATE__H__
#include "TypesDef.h"
#include "AthenaNew.h"
#include "Singleton.h"

enum ENUM_AI_STATE
{
	AISTATE_INVALID = -1,
	AISTATE_IDLE = 0,			//空闲状态:搜索敌人
	AISTATE_DEAD,				//死亡状态，清除尸体，重生
	AISTATE_GOHOME,				//出界返回
	AISTATE_COMBAT,				//战斗状态:技能攻击
	AISTATE_MAX,
};

//AI状态机基类,转调CAINpc接口。具体行为需要CAINpc来定义
class CAINpc;
class AIState
{
	friend class CAINpc;
	public:
		//构造函数
		AIState();
		//析构函数
		virtual ~AIState();
		typedef AIState Base;
		//返回状态类型
		virtual INT16 getStateType() const 
		{
			return AISTATE_INVALID;
		}
	public:
		//此状态具体行为
		virtual bool doAction(CAINpc* pAI,UINT64 nTime)=0;
		//闲置行为
		void actionIdle(CAINpc* pAI, UINT64 uTime);
		//死亡行为
		void actionDead(CAINpc* pAI, UINT64 uTime);
		//战斗行为
		void actionCombat(CAINpc* pAI, UINT64 uTime);
		//往会走状态
		void actionGohome(CAINpc* pAI, UINT64 uTime);
};

class AIStateMgr
{
	USE_SINGLETON_NEW(AIStateMgr);
private :
	AIStateMgr()
	{
		m_states = ATHENA_NEW AIState*[AISTATE_MAX];

		for(int i=0;i<AISTATE_MAX;i++)
		{	
			m_states[i] = NULL;
		}
	}
public :
	static AIStateMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	~AIStateMgr()
	{
		clear();
		SAFE_DELETE_ARRAY(m_states);
		m_states = NULL;
	}

	void clear()
	{
		for(INT16 i=0;i<AISTATE_MAX;i++)
		{	
			if(m_states[i])
			{
				SAFE_DELETE(m_states[i]);
				m_states[i] = NULL;
			}
		}
	}

	AIState* getState(ENUM_AI_STATE id)
	{
		return m_states[id];
	}
	
	AIState* operator[](ENUM_AI_STATE id)
	{
		return m_states[id];
	}

	AIStateMgr& operator()(AIState* f)
	{
		registState(f);
		return *this;
	}
private:
	bool registState(AIState* f)
	{
		if(!f) {
			return false;
		}
		m_states[f->getStateType()] = f;
		return true;
	}
	AIState** m_states;
};

//空闲状态类，
class IdleState : public AIState
{
	public:
		IdleState() {}	
		~IdleState() {}
		virtual INT16 getStateType(void) const {
			return AISTATE_IDLE;
		}
		virtual bool doAction(CAINpc* pAI,UINT64 nTime);
};


//死亡状态类:死亡呼救，到时间可能重生等
class DeadState : public AIState
{
	public:
		DeadState() {}	
		~DeadState() {}
		virtual INT16 getStateType(void) const {
			return AISTATE_DEAD;
		}
		virtual bool doAction(CAINpc* pAI,UINT64 nTime);
};

//战斗状态类:使用技能攻击敌人
class CombatState : public AIState
{
	public:
		CombatState() {}	
		~CombatState() {}
		virtual INT16 getStateType(void) const
		{
			return AISTATE_COMBAT;
		}
		virtual bool doAction(CAINpc* pAI,UINT64 nTime);
};

//返回状态类:一般在走出活动范围时转换，一路走回出生点，不还击
class GoHomeState : public AIState
{
	public:
		GoHomeState() {}	
		~GoHomeState() {}
		virtual INT16 getStateType(void) const 
		{
			return AISTATE_GOHOME;
		}
		virtual bool doAction(CAINpc* pAI,UINT64 nTime);
};
#endif
