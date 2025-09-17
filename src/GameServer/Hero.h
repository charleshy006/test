#ifndef __H_HERO_H__
#define __H_HERO_H__

#include "TypesDef.h"
#include "TblDataMgr.h"
#include "SkillInfo.h"
#include "UniqueID.h"
#include "Map.pb.h"
#include "HeroInfo.h"
#include "Npc.h"
#include "Item.h"
#include <map>
#include <vector>
#include <set>

class CHero : public CBeing
{
public :
	//构造
	CHero();
	//析构
	virtual ~CHero();
	//获得广播数据
	void getAppear(Map::NotifyHeroAppear::stHeroData & pData);
	//获得类型
	virtual CObj::ObjType getType()const
	{   
		return CObj::OBJ_TYPE_HERO;
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
public :
	//定时回调
	virtual bool heartBeat( UINT64 uTime);
	//设置主人ID
	void setOwnerID(UINT32 ownerID)  {  m_ownerID = ownerID ; }
	//处理死亡
	void processDeath(CBeing *pAtter);
	//获得当前血量
	virtual UINT32 getHP()  { return m_hp ;}
	//设置当前血量
	void setHP(UINT32 hp) 
	{
		m_hp = hp;
		if (m_maxHP < m_hp){
			m_maxHP = m_hp;
		}
	}
	//返回当前最大血量
	UINT32  getMaxHP() { return  m_maxHP ;}
	//移动
	bool move(CPos & pos);
	//设置状态
	virtual void setupState();
	//武将的最终竖直是
	void printData();
	//初始化
	bool init(UINT32 heroID,UINT32 ownerID,UINT16 level,UINT16 evolutionLv, CMap* pMap);
	//计算攻击数值
	virtual void calcPKData();
	//获得主任ID
	UINT32 getOwnerID()
	{
		return m_ownerID;
	}
	//血量乘系数
	void hpMultiCoef(UINT32 coef)
	{
		m_hp = m_hp * coef;
		m_maxHP = m_maxHP * coef;
	}
	//进入副本
	virtual void onEnterMap(CMap *);
	//召唤自己
	virtual void summonSelf(UINT32 lastTime,UINT32 hpPer,UINT32 damagePer);
	//是否可以删除自己
	virtual bool canClearMe();
	//设置清空自己时间
	void setClearMeTime(UINT32 clearMeTime)
	{
		m_clearMeTime = clearMeTime;
	}
	//设置创建标志
	void setSummonBySkill(bool flag)
	{
		m_summonBySkill = flag;
	}
private :
	UINT32 m_ownerID;						//主人ID
	UINT16 m_level;							//等级
	UINT16 m_evolutionLv;					//进化等级
	stRoleBase * m_pRoleBase;               //配置信息
	stHeroLvPropBase * m_pHeroLvPropBase;	//宠物的等级属性
	stHeroEvolutionBase * m_pEvolutionBase;	//进化数据
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

	UINT32 m_clearMeTime;					//清空自己时间
	bool   m_summonBySkill;					//由技能召唤
	UINT32 m_damagePer;						//召唤出来的伤害百分比
};
#endif
