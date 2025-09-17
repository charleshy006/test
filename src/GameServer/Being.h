#ifndef __H_CBEING__
#define __H_CBEING__
#include"TypesDef.h"
#include"Obj.h"
#include "Time.h"
#include "Skill.h"
#include "PKData.h"
#include<string.h>
#include<vector>

class Being;
class CSkillCoolDownMgr
{
public :
	//cd管理
	CSkillCoolDownMgr(CBeing * pOwner);
	//定义器
	void timer(UINT64 cur);
	//使用某个技能成功
	void throwSkillSuccess(UINT32 skillID,UINT32 lastTime,UINT32 maxHitNum);
	//碰撞是否合法
	bool hitValid(UINT32 skillID);
	//增加碰撞次数
	void incHitNum()
	{
		++m_hitNum;
	}
private :
	UINT64 m_lastUseTime;		//上次使用时间
	UINT32 m_lastTime;			//持续时间
	UINT32 m_useSkillID;		//使用的技能
	UINT32 m_hitNum;			//已经碰撞了多少次
	UINT32 m_maxHitNum;			//最大碰撞次数
	CBeing * m_pOwner;
};

class GameUser;
class CBeing : public CObj
{
	public:
		//构造
		CBeing( );
		//析构
		virtual ~CBeing( );
		//获得类型
		virtual CObj::ObjType getType()const
		{
			return OBJ_TYPE_MONSTER;
		}
		//取得唯一ID
		virtual UINT32 getUniqueID() const
		{
			return this->getTempID();
		}
		//设置当前状态
		void setState(UINT16 state)
		{
			if (Map::BEING_STATE_MAX < state){
				return;
			}
			m_state[state / 8] |=  (1 << (state % 8));
		}
		//清空状态
		void clearState(UINT16 state)
		{
			if (Map::BEING_STATE_MAX < state){
				return;
			}
			m_state[state / 8] &= ~(1 << (state % 8));
		}
		//是否设置了状态
		bool isSetState(UINT16 state)
		{
			if (Map::BEING_STATE_MAX < state){
				return false;
			}
			return m_state[state / 8] & (1 << (state % 8));	
		}
		//是否普通状态
		bool isNormal()
		{
			return !isSetState(Map::BEING_STATE_DEATH);
		}
		//是否已经死亡
		bool isDie()
		{
			return isSetState(Map::BEING_STATE_DEATH);
		}
		//定时回调
		virtual bool heartBeat( UINT64 uTime);
		//处理死亡
		virtual void processDeath(CBeing *pAtter) {}
		//扣除血量
		virtual UINT32 decHP(UINT32 hp,CBeing * pAtter);
		//增加血量
		virtual void addHP(UINT32 hp);
		//增加魔法值
		virtual void addMP(UINT32 mp);
		//减少魔法值
		virtual bool decMP(UINT32 mp);
		//增加霸体值
		virtual void addBati(UINT32 bati);
		//减少霸体值
		virtual bool decBati(UINT32 bati);
		//获得霸体最大值
		UINT32  getMaxBati() { return m_maxBati ;}
		//获得当前血量
		virtual UINT32 getHP() = 0;
		//获得当前最大血量
		UINT32  getMaxHP() { return m_maxHP ;}
		//获得技能ID
		CSkill * getSkillByID(UINT32 skillID);
		//获得关系类型
		virtual UINT8  getRelation(CBeing * pOther) { return 0 ;}
		//开始计算伤害
		UINT32 calcDamage(CBeing * pDefer);
		//获得pk竖直
		stPKCalcData  & getPKCalcData()  { return m_pkData ; }
		//计算pk竖直
		virtual void calcPKData() = 0;
		//重置pk竖直
		void resetPKData() { m_pkData.reset(); }
		//移动
		bool move(CPos & pos,bool inInstant = false);
		//移动冷却时间
		bool isMoveColdDown(UINT64 uTime);
		//设置上一次移动时间
		void setLastMoveTime(UINT64 uTime)
		{
			m_lastAttackTime = 0;	
		}
		//设置上一次攻击时间
		void setLastAttackTime(UINT64 uTime)
		{
			m_lastAttackTime = uTime;
		}
		//攻击冷却时间
		bool isAttackColdDown(UINT64 uTime);
		//获得目标
		CBeing* getCurTarget();
		//移动频率,多久移动一次
		virtual UINT32 getMoveSpeed() { return 640; }
		//攻击频率,多久攻击一次
		virtual UINT32 getAttackSpeed() { return 640 ;}
		//设置目标
		void   setCurTarget(UINT32 curID,CObj::ObjType type)
		{
			m_curTargetID = curID;
			m_curTargetType = type;
		}
		//是否敌对关系
		virtual bool isEnemy(CBeing* pObj){return false;}
		//打断了技能播放
		void interruptSkillPlay(std::string nextAction);
		//改变了朝向
		void changeDir(UINT32 dir);
		//回复MP
		virtual UINT32 getRecoverMP() { return 0 ;}
		//回复血
		virtual UINT32 getRecoverHP() { return 0;}
		//获得吸血比例
		virtual UINT32 getAbsorbHPPer() { return 0;}
		//收到普通攻击
		virtual void   hitByNormal() {}
		//霸体开始变为0
		virtual void   batiEmpty() {}
		//召唤自己
		virtual void summonSelf(UINT32 lastTime,UINT32 hpPer,UINT32 damagePer) {}
		//获得属性
		stBufferState & getBuffData() { return m_buffData ;}
		//是否可以清除
		virtual bool canClearMe() {  return false; }
		//设置阵营ID
		void setCampID(UINT8 campID)
		{
			m_campID = campID;	
		}
		//获取阵营
		UINT8 getCampID()
		{
			return m_campID;
		}
		//获得属主
		virtual UINT32 getOwnerID() { return 0 ;}
		//根据类型和ID获得对象
		static CBeing * getBeing(UINT8 type,UINT32 moID, CMap* pMap);
		//清空所有状态,除了是否死亡这个状态保留外
		void   clearAllState();
		//是否是机器人,自己或者主人是机器人都返回true
		bool isRobot();
		//获得技能加强数据
		virtual UINT32 getRaiseSkillValue(UINT32 skillID, UINT32 raiseType);
        void setCoolDown(bool res) {
            m_coolDown = res;
        }
        bool isCoolDown() {
            return m_coolDown;
        }
	public :
		CSkillMgr     m_skillMgr;                               //技能管理
		CStateMgr	  m_stateMgr;								//状态管理
		CSkillCoolDownMgr m_skillCDMgr;							//技能cd管理
	protected:
		UINT32 		  m_hp;										//当前血量
		UINT32 		  m_lastHP;									//前次的血量
		UINT32 		  m_maxHP;                         			//血量
		UINT32		  m_mp;										//当前魔法
		UINT32		  m_maxMP;									//最大魔法
		UINT32		  m_bati;									//当前霸体值
		UINT32		  m_maxBati;								//最大霸体值
		UINT32 		  m_curTargetID;							//当前攻击目标ID
		CObj::ObjType m_curTargetType;							//当前目标类型：决定从NPC管理器还是玩家管理器中取得目标
		UINT64 		  m_lastAttackTime;							//上次攻击时间
		UINT64 		  m_lastMoveTime;  							//上次移动时间
		byte 	      m_state[Map::BEING_STATE_MAX / 8 + 1];	//状态
		stPKCalcData  m_pkData;									//pk战斗数值
		stBufferState m_buffData;								//状态影响的数值
		UINT8 		  m_campID;									//阵营ID
        bool          m_coolDown;
};
#endif
