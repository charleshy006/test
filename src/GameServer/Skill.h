#ifndef __H_SKILL_H__
#define __H_SKILL_H__
#include "TypesDef.h"
#include "Element.h"
#include "ElementMgr.h"
#include "SkillInfo.h"
#include "TblSkillBase.h"
#include "Singleton.h"
#include "Obj.h"
#include "PKData.h"
#include <tinyxml/tinyxml.h>

#pragma pack(1)
struct stSkillSaveInfo
{
	UINT32 m_version;
	UINT16 m_num;
	SkillInfo  m_data[0];

	stSkillSaveInfo()
	{
		m_version = 20150305;
		m_num = 0;
	}
};
#pragma pack()

class CBeing;
class CSkill;
class stStateInfo;

struct stFunctionBase
{
	UINT16 m_funcType;

	stFunctionBase()
	{
		m_funcType = 0;
	}

	virtual ~stFunctionBase()
	{
	
	}
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff) = 0;
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState) = 0;
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff) = 0;
};


enum
{
	FUNCTION_TYPE_NULL = 0,				//空的函数类型
	FUNCTION_TYPE_ADD_HP = 1,			//增加血
	FUNCTION_TYPE_SILENCE = 2,			//沉默
	FUNCTION_TYPE_ADD_IMMUNCE = 3,		//诅咒免疫
	FUNCTION_TYPE_SUMMON_HERO = 4,		//召唤武将
	FUNCTION_TYPE_RAGE = 5,				//狂暴
	FUNCTION_TYPE_LIMIT_DEFENSE = 6,	//极限防御
	FUNCTION_TYPE_DEC_VELOCITY = 7,		//减速
	FUNCTION_TYPE_DIZZY = 8,			//眩晕
	FUNCTION_TYPE_PROVOCATIVE = 9,		//挑衅
	FUNCTION_TYPE_CHG_DAMAGE_PROP = 10,	//伤害属性变化
	FUNCTION_TYPE_CHG_REF_BANG = 11,	//改变抗暴击
	FUNCTION_TYPE_CHG_HIT_RATE = 12,	//改变命中率
	FUNCTION_TYPE_TIMER_DEC_HP = 13,	//定时扣血
	FUNCTION_TYPE_CHG_BATI = 14,		//定时霸体
	FUNCTION_TYPE_TIMER_ADD_HP = 15,	//定时加血
	FUNCTION_TYPE_CHG_DODGE = 16,		//改变闪避
	FUNCTION_TYPE_ICE = 17,				//冰冻
	FUNCTION_TYPE_CLEAR_BUF = 18,		//删除地方增益buff
	FUNCTION_TYPE_SUMMONNPC = 19,		//召唤npc
	FUNCTION_TYPE_CHGAPPEARANCE = 20,	//变身 
	FUNCTION_TYPE_CHG_BANG = 21,		//改变暴击
	FUNCTION_TYPE_MAX ,
};

struct stFunctionNull : public stFunctionBase
{
	stFunctionNull()
	{
		m_funcType  = FUNCTION_TYPE_NULL;	
	}

	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff){}
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState){}
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff){}
};

struct stFuncAddHP : stFunctionBase
{
	stFuncAddHP()
	{
		m_funcType = FUNCTION_TYPE_ADD_HP;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncSilence : stFunctionBase
{
	stFuncSilence()
	{
		m_funcType = FUNCTION_TYPE_SILENCE; 		
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncSummonHero : stFunctionBase
{
	stFuncSummonHero()
	{
		m_funcType = FUNCTION_TYPE_SUMMON_HERO;	
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncImmunce : stFunctionBase
{
	stFuncImmunce()
	{
		m_funcType = FUNCTION_TYPE_ADD_IMMUNCE;
	}

	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncRage : stFunctionBase
{
	stFuncRage()
	{
		m_funcType = FUNCTION_TYPE_RAGE	;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);

};

struct stFuncLimitDefense : stFunctionBase
{
	stFuncLimitDefense()
	{
		m_funcType = FUNCTION_TYPE_LIMIT_DEFENSE;
	}

	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncChgVelocity : stFunctionBase
{
	stFuncChgVelocity()
	{
		m_funcType = FUNCTION_TYPE_DEC_VELOCITY;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncDizzy : stFunctionBase
{
	stFuncDizzy()
	{
		m_funcType = FUNCTION_TYPE_DIZZY;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncProvocative : stFunctionBase
{
	stFuncProvocative()
	{
		m_funcType = FUNCTION_TYPE_PROVOCATIVE;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);

};

struct stFuncChgDamageProp : stFunctionBase
{
	stFuncChgDamageProp()
	{
		m_funcType = FUNCTION_TYPE_CHG_DAMAGE_PROP;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncChgRefBangProp : stFunctionBase
{
	stFuncChgRefBangProp()
	{
		m_funcType = FUNCTION_TYPE_CHG_REF_BANG;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncChgHitRateProp : stFunctionBase
{
	stFuncChgHitRateProp()
	{
		m_funcType = FUNCTION_TYPE_CHG_HIT_RATE;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncTimerDecHP : stFunctionBase
{
	stFuncTimerDecHP()
	{
		m_funcType = FUNCTION_TYPE_TIMER_DEC_HP;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncChgBati : stFunctionBase
{
	stFuncChgBati()
	{
		m_funcType = FUNCTION_TYPE_CHG_BATI;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncTimerAddHP : stFunctionBase
{
	stFuncTimerAddHP()
	{
		m_funcType = FUNCTION_TYPE_TIMER_ADD_HP;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);

};

struct stFuncChgDodge : stFunctionBase
{
	stFuncChgDodge()
	{
		m_funcType =  FUNCTION_TYPE_CHG_DODGE;	
	}

	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncIce : stFunctionBase
{
	stFuncIce(){
		m_funcType = FUNCTION_TYPE_ICE;
	}

	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncClearGoodBuf : stFunctionBase
{
	stFuncClearGoodBuf()
	{
		m_funcType = FUNCTION_TYPE_CLEAR_BUF;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncSummonNpc : stFunctionBase
{
	stFuncSummonNpc()
	{
		m_funcType = FUNCTION_TYPE_SUMMONNPC;
	}

	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);

};

struct stFuncChgAppearance : stFunctionBase
{
	stFuncChgAppearance()
	{
		m_funcType = FUNCTION_TYPE_CHGAPPEARANCE;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

struct stFuncChgBangProp : stFunctionBase
{
	stFuncChgBangProp()
	{
		m_funcType = FUNCTION_TYPE_CHG_BANG;
	}
	
	//开始投递状态时候回调
	virtual void doThrowState(CBeing * pAtter,CBeing * pDefer,BufferBase * pBuff);
	//执行定时回调
	virtual void doFunc(CBeing * pDefer,stStateInfo * pState);
	//删除状态时候回调
	virtual void doDelState(CBeing * pDefer,BufferBase * pBuff);
};

class CFunctionMgr
{
	USE_SINGLETON_NEW(CFunctionMgr);
private :
	//构造析构
	CFunctionMgr();
public :
	//析构函数
	~CFunctionMgr(){}
	//技能Function管理器
	static CFunctionMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();	
	}
	//根据ID获得function
	stFunctionBase  * getFunction(UINT16 opID)
	{
		if (opID >= FUNCTION_TYPE_MAX) {
			return NULL;
		}	
		return m_pFunVec[opID];
	}
private :
	stFunctionBase  * m_pFunVec[FUNCTION_TYPE_MAX];	
};

class CSkillRangeCfg
{
	USE_SINGLETON_NEW(CSkillRangeCfg);
public :
	//构造
	CSkillRangeCfg();
	//析构
	~CSkillRangeCfg();
	//获得实例
	static CSkillRangeCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
	//根据范围类型ID获得范围点
	std::vector<CPos> &  getRangePos(UINT16 id);
private :
	std::map<UINT16,std::vector<CPos> > m_ranges;
	std::vector<CPos> m_defaultVec;
};

class CSkill : public Element
{
public :
	//构造函数
	CSkill();
	//析构函数
	~CSkill();
	//初始化技能数据
	bool init();
	//获得所有攻击者
	void getAllDefers(CBeing * pAtter,CPos & attPos,std::vector<CBeing *> & vec);
	//执行技能函数
	void doSkillFunction(CBeing * pAtter,CBeing * pDefer);
	//丢一个buf到自己身上
	static void throwBuff(CBeing * pAtter,CBeing * pDefer,UINT32 buffID);
	//释放技能
	std::pair<UINT32,UINT32> doAction(CBeing * pAtter,CBeing * pDefer,UINT8 step);
	//检查技能消耗
	bool checkCost(CBeing * pAtt);
	//检查技能的射程
	bool checkDis(const CPos & attPos,const CPos & defPos);
	//是否是普攻
	bool isNormal();
	//计算计算伤害
	bool isCalcDam();
	//是否冷却
	bool isCoolDown(UINT32 reduceSec);
	//设置上次使用时间
	void setLastUseTime();
	//获得冷却时间
	UINT32 getLastTime();
	//获得最大碰撞次数
	UINT32 getMaxHitNum();
public :
	SkillInfo m_skillInfo;
	stSkillBase *m_pSkillBase;
	SkillGrownBase * m_pSkillGrownBase;
	UINT64  m_lastUseTime;		//最近一次使用时间
};

struct stSkillCallBackBase
{
	stSkillCallBackBase()
	{
	}

	virtual ~stSkillCallBackBase()
	{
	}

	virtual  bool doIt(CSkill * pSkill) = 0;
};


struct stGetSkillRaiseLvCB : public stSkillCallBackBase
{
	UINT16 m_lv;
	UINT16 m_num;

	stGetSkillRaiseLvCB(UINT16 lv) : m_lv(lv),m_num(0)
	{
		
	}
	
	~stGetSkillRaiseLvCB()
	{
	
	}

	virtual  bool doIt(CSkill * pSkill)
	{
		if (pSkill){
			if (pSkill->m_skillInfo.m_level >= m_lv){
				m_num += 1;
			}
		}
		return true;
	}
};

class CBeing;
class CSkillMgr : public ElementMgr
{
	static const UINT32 s_SKILL_VERSION = 20150304;
public :
	//构造函数
	CSkillMgr(CBeing * pOwner);
	//析构函数
	~CSkillMgr();
	//同步所有技能
	void synAllSkill();
	//保存
	UINT32 save(byte * buf,UINT32 needSpace);
	//加载
	UINT32 load(byte * buf);
	//创建技能
	static CSkill * createSkill(UINT32 skillID,UINT16 lv = 0);
	//根据技能id获得技能
	CSkill * getSkillByID(UINT32 skillID);
	//增加技能
	bool addSkill(CSkill * pSkill);
	//删除一个技能
	bool removeSkill(CSkill * pSkill);
	//触发所有被动技能
	void throwAllPassiveSkill();
	//检查技能解锁
	void checkSkillUnlock();
	//对每个Skill执行回调
	void exeEveryOne(stSkillCallBackBase & cb);
	//所有技能的强化等级之和
	UINT16 getSkillNumByRaiseLv(UINT16 lv);
	//获得增加的战力
	UINT32 getAddBP()
	{
		return m_addBP;
	}
	//计算战力
	void calcBP();
	//临时技能
	bool addTeampSkill(UINT32 skillID,UINT16 lv);
	//清空临时技能
	void clearTeampSkill();
private :
	CBeing * m_pOwner;
	UINT32   m_addBP;
	std::map<UINT32,CSkill *> m_tempSkills;
};

struct stStateInfo
{
	UINT8  m_attType;			//攻击者类型
	UINT32 m_moID;				//攻击者ID
	UINT64 m_addTime;			//添加状态时间
	UINT32 m_lastTime;			//持续时间
	UINT64 m_lastDoFuncTime;	//上次执行定时回调时间
	UINT32 m_interval;			//间隔
	UINT16 m_stateID;			//状态ID
	INT32  m_value;				//对应的值
	BufferBase * m_pBuff;		//所对应的buff

	stStateInfo()
	{
		m_attType = 0;
		m_moID = 0;
		m_addTime = 0;
		m_lastTime = 0;
		m_lastDoFuncTime = 0;
		m_interval = 0;
		m_stateID = 0;
		m_value = 0;
		m_pBuff = 0;
	}
};

//状态管理
class CStateMgr
{
public :
	//构造函数
	CStateMgr(CBeing * pOwner);	
	//析构函数
	~CStateMgr()
	{
	}
	//定时器
	void timer(UINT64 curTime);
	//增加状态
	void addState(CBeing * pAtter,UINT16 stateID,UINT32 lastTime,UINT32 interval = 1000,BufferBase * pBuff = NULL,INT32 extraVal = 0,bool notify = true);
	//删除状态
	void delState(UINT32 buffID,bool notify = true);
	//清空所有debuff
	void clearAllDebuff();
	//清空所有增益buff
	void clearAllGoodBuff();
	//设置buff数值
	void setupBuff();
	//清空所有buf
	void clearAllBuff();
private :
	CBeing * m_pOwner;
	std::map<UINT32,stStateInfo> m_buffs;
	//std::vector<UINT32>  paramVec;
};
#endif
