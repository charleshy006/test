#ifndef __NPC_H__
#define __NPC_H__
#include "Being.h"
#include "Map.pb.h"
#include "TblRoleBase.h"
#include "TblMonster.h"
#include "AIState.h"

#define MAX_CHAR_PATH_NODE_NUMBER 1024
class CAINpc;
class CNpc  : public CBeing
{
	friend class CAINpc;
public:
	CNpc();
	virtual ~CNpc();
	//从地图出现时候，填充指令
	virtual void getAppear(Map::NotifyNpcAppear::stNpcData & data);
	//获得类型
	virtual ObjType getType()const
	{
		return CObj::OBJ_TYPE_MONSTER;
	}
	//地面通行等级	
	virtual UINT8 getDriveLevel()const
	{
		return 1;
	}
	//移动时候造成阻挡
	virtual bool isBlockObj()const
	{
		return false;
	}
	//取得唯一ID
	virtual UINT32 getUniqueID()const
	{
		return getTempID();
	}
public:
	//轮询用
	virtual bool heartBeat( UINT64 uTime);
	//初始化
	bool init(UINT32 id,UINT16 level);
	//处理死亡 
	void processDeath(CBeing *pAtter);
	//初始化血量
	void initHP(UINT32 hp);
	//获得当前血量
	virtual UINT32 getHP()  { return m_hp ;}
	//返回当前最大血量
	UINT32  getMaxHP() { return  m_maxHP ;}
	//创建技能
	virtual void createSkill();
	//设置
	virtual void setupState();
	//返回类型
	virtual UINT8  getRelation(CBeing * pOther) ;
	//计算pk数值
	virtual void calcPKData();
	//设置npc的AI
	void setAI(CAINpc *  pNpcAI);
	//返回NpcAI
	CAINpc * getAI();
	//执行AI动作
	void doAIAction(UINT64 uTime);
	//返回AI状态
	AIState * getState() { return m_pState ; }
	//切换状态
	void changeState(ENUM_AI_STATE eState);
	//触发技能
	void trigerSkill();
	//返回敌人数量
	INT16 getEnemies(std::vector<CBeing*>& enemies,INT16 radius);
	//尸体存在时间
	UINT64 getCorpseTime() const { return 3000; }
	//死亡时候,尸体复活时间,该时间了应该大于尸体存在时间。
	UINT64 getReliveTime()const{ return 5000; }
	//掉落道具
	void dropItem();
	//重生
	bool relive();
	//npc说话
	void speak();
	//返回攻击距离
	UINT32	attackDis() { return 3; }
	//移动到目标点用寻路算法
	bool move( const CPos& tarPos,bool useAStar);
	//朝着某个方向移动,不用寻路算法
	bool move( INT8 dir,INT16 dist=1);
	//瞬移到某点
	bool telePort(const CPos&);
	//是否敌我关系
	virtual bool isEnemy(CBeing* pObj);
	//设置是否是boss
	void setBoss() { m_bIsBoss = true; }
	//是否是boss
	bool isBoss() { return m_bIsBoss; }
	//设置出生动画ID
	void setBornAnimID(UINT32 aminID)
	{
		m_bornAminID = aminID;
	}
	//获得出生动画
	UINT32 getBornAnimID()
	{
		return m_bornAminID;
	}
	//设置出生朝向
	void setBornDir(UINT16 angle)
	{
		m_bornDir = angle;
	}
	//获得出生朝向
	UINT16 getBornDir()
	{
		return m_bornDir;
	}
	//收到普通攻击
	 virtual void   hitByNormal();
	//霸体变空
	virtual void   batiEmpty();
	//设置动作组ID
	void setActionGroupID(UINT16 groupID)
	{
		m_actionGroupID = groupID;
	}
	//获得动作组
	UINT16 getActionGroupID()
	{
		return m_actionGroupID;
	}
	//设置等级
	void setLevel(UINT16 level)
	{
		m_level = level;
	}
	//获得等级
	UINT16 getLevel()
	{
		return m_level;
	}
	//设置清空自己时间
	void setClearMeTime(UINT32 clearMeTime)
	{
		m_clearMeTime = clearMeTime;
	}
	//是否可以清除
	virtual bool canClearMe();
protected:
	UINT16 m_level;							//等级
	UINT32 m_damage;						//伤害
	UINT32 m_antiBangDamge;					//抗暴击伤害
	UINT32 m_antiBangRate;					//抗暴击概率
	UINT32 m_bangDamage;					//暴击伤害
	UINT32 m_bangRate;						//暴击概率
	UINT32 m_damageReduce;					//伤害减免
	UINT32 m_finalDamage;					//最终伤害
	UINT32 m_hitRate;						//命中率
	UINT32 m_dodgeRate;						//闪避率
	
	UINT32 m_fireRate;                      //火强概率
	UINT32 m_iceRate;                       //冰冻概率
	UINT32 m_thunderRate;                   //雷电概率
	UINT32 m_refFireRate;                   //火强概率免疫
	UINT32 m_refIceRate;                    //冰冻概率免疫
	UINT32 m_refThunderRate;                //雷电概率免疫
	UINT32 m_fireDamage;                    //火伤害
	UINT32 m_iceDamage;                     //冰冻伤害
	UINT32 m_thunderDamage;                 //雷电伤害
	UINT32 m_refFireDamage;                 //火伤害免疫
	UINT32 m_refIceDamage;                  //冰伤害免疫
	UINT32 m_refThunderDamage;              //雷电伤害免疫

	stRoleBase * m_pRoleBase;				//配置信息
	stMonsterBase * m_pMonsterBase;			//怪物信息
	AIState* m_pState;						//当前状态编号
	CAINpc * m_pNpcAI;						//npc的AI
	bool	 m_bHasDropItem;				//是否已经掉落道具
	bool	 m_bIsBoss;						//是否是boss
	UINT32	 m_bornAminID;					//出生动画ID
	UINT16   m_bornDir;						//出生朝向
	UINT32   m_lastHitByNormalTime;			//最近一次受普通攻击时间
	UINT32   m_batiEmptyTime;      			//霸体变空状态
	Timer    m_oneSec;           			//1秒
	UINT16   m_actionGroupID;				//动作组ID
	UINT32   m_clearMeTime;                 //清空自己时间
};
#endif
