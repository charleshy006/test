#ifndef __H_AI_NPC_H__
#define __H_AI_NPC_H__
#include "NpcDataDefine.h"
#include "AIState.h"
#include "TblMonster.h"
#include "RectArea.h"
//npc警戒范围
#define NPC_ALERT_RANGE 19

class CNpc;
class CBeing;
class CAINpc 
{
public :
	enum NPC_DIE_STATE
	{
		DIE_STATE_NO, 		//非死亡
		DIE_STATE_ROT,		//尸体腐烂-消失
		DIE_STATE_RELIVE,	//准备重生
	};
public:
	//构造函数
	CAINpc(CNpc * pNpc,NpcAIBase * pNpcAIBase);
	//析构函数
	virtual ~CAINpc(){}
	//范围判断接口
	bool isOutOfRange();
	//是否到达
	bool isArrived(const CCellIdx& dest);
	//是否到达
	bool isArrived();
	//随机移动,移动步长，默认移动一格子
	bool randomMove(INT16 step=1);
	//走进了攻击
	bool runToEnemyAndAttack(CBeing* pBeing,UINT64 uTime);
	//返回当前状态下活动范围长度
	virtual CRectArea&  getActiveArea(){	return m_activeArea;}
	//直接设置活动区域为某个矩形区域
	void setActiveArea( const CRectArea& area );
	//死亡触发
	virtual void onDie( );
	//血量触发
	virtual void onHP(UINT32 lastPercent,UINT32 curPercent);
	//状态切换
	void changeState(ENUM_AI_STATE eState) ;
	//同一状态,不同控制器可能采取不同的动作：
	//各种逻辑具体实现,被state所调用
	virtual void actionIdle(UINT64 uTime);
	virtual void actionDead(UINT64 uTime);
	virtual void actionCombat(UINT64 uTime);
	virtual void actionGohome(UINT64 uTime);
	//命条数
	INT32 getLeftLifeTimes()
	{
		return m_lifeTimes;
	}
	//死亡时,尸体清除时间
	UINT64 getBodyClearTime();
	//死亡后，复活时间，该时间必须比清除尸体时间晚一些
	UINT64 getReliveTime();	
	//可以消失了
	bool canClearAfterDie(UINT64 uTime);
	//可以重生了
	bool canReliveAfterDie(UINT64 uTime);
	//设置死亡状态
	void setDieState(NPC_DIE_STATE dState)
	{
		m_dieState = dState;
	}
	//获得死亡状态
	NPC_DIE_STATE getDieState()
	{
		return m_dieState;
	}
	//选择敌人
	CBeing* chooseEnemy();
	//技能冷却时间到:不同怪物冷却时间可能不同
	virtual bool skillColdDown(UINT64 uTime);
	//触发事件
	void trigerEvent();
	//设置死亡事件
	void setDeathTime(UINT64 deathTime) { m_deathTime = deathTime; }
public:
	struct AIEnemyItem
	{
		UINT32 objID;
		CObj::ObjType type;
		INT32 value;//造成仇恨度
		AIEnemyItem()
		{
			objID=0;
			type = CObj::OBJ_TYPE_MAX;
			value = 0;
		}
		bool operator < (const AIEnemyItem& i)
		{
			return value < i.value;
		}

	};
	std::set<AIEnemyItem> m_enemiesList;//仇恨列表-最大记录16个
protected:
	UINT32 m_lastSkillTime;				//上次释放技能的时间
	CRectArea m_activeArea;				//当前活动区域，活动区域不一定为出生区域
	CCellIdx m_destPos;					//位置调整后定要到达的地方
	CNpc * m_pOwner;					//当前AI的属主
	NpcAIBase *m_pNpcAIBase;			//AI基本属性
private:
	UINT32 m_lifeTimes;					//剩余生命条数
	UINT32 m_bornTime;					//出生时间
	UINT64 m_deathTime;					//死亡时间
	NPC_DIE_STATE m_dieState;			//死亡具体状态
};
#endif

