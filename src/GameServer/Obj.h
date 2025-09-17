#ifndef __OBJ_H__
#define __OBJ_H__

#include <list>
#include <set>
#include <vector>
#include <TypesDef.h>
#include <stdio.h>
#include "Element.h"
#include "Map.pb.h"
#include "Pos.h"

template <class T>
class TPos2Idx
{
public:
	TPos2Idx()
	{
		m_x = -1;
		m_z = -1;
	}

	typedef TPos2Idx<T> TThisClass;

	explicit TPos2Idx(const CPos & pos)
	{
		if (pos.isValid())
		{
			m_x = (INT32)pos.m_x / T::THE_X;
			m_z = (INT32)pos.m_z / T::THE_Z;
		}
		else
		{
			m_x = -1;
			m_z = -1;
		}
	}

	bool isValid() const
	{
		return m_x >=0 && m_z >=0 ;
	}

	bool operator == (const TThisClass& idx) const
	{
		return m_x == idx.m_x && m_z == idx.m_z;
	}
	bool operator != (const TThisClass& idx) const
	{
		return !(*this == idx);
	}

	INT32 m_x;
	INT32 m_z;
};

template <class T>
class TIdx2Pos
{
	public:
		TIdx2Pos()
		{
		}

		typedef TIdx2Pos<T> TThisClass;

		explicit TIdx2Pos(const TPos2Idx<T>& idx)
		{
			if (idx.isValid())
			{
				pos.m_x = idx.m_x * T::THE_X + T::THE_X/2;
				pos.m_z = idx.m_z * T::THE_Z + T::THE_Z/2;	
			}
			else
			{
			}
		}


		bool isValid() const
		{
			return pos.isValid();
		}	

		bool operator == (const TThisClass& idx) const
		{
			return pos == idx.pos;
		}
		bool operator != (const TThisClass& idx) const
		{
			return !(*this == idx);
		}

		CPos pos;
};

class CCell;
typedef TPos2Idx<CCell> CCellIdx;
typedef TIdx2Pos<CCell> CCellPos;

class CMap;
class GameUser;
class CNpc;
class CHero;
class CDropItemObj;

class CObj : public Element
{
public:
	CObj();
	virtual ~CObj();
	//物件类型 
	enum ObjType
	{
		OBJ_TYPE_PLAYER,			// 玩家
		OBJ_TYPE_MONSTER,			// 战斗的NPC与怪物
		OBJ_TYPE_HERO,				// 武将
		OBJ_TYPE_ITEM,				// 掉落的道具
		//新类型在此添加
		OBJ_TYPE_MAX,				
	};

	//取得当前所属地图
	CMap * getMap();
	//设置当前地图
	void setMap(CMap * pMap);
	//得到该物件的所属类型
	virtual ObjType getType() const = 0;
	//获得唯一ID
	virtual UINT32 getUniqueID() const = 0;
	//是不是生物:生物特点，具有智能，比如战斗等。
	bool isBeing() const
	{
		return getType()>= OBJ_TYPE_PLAYER && getType() <= OBJ_TYPE_MONSTER;
	}
	//是否游戏玩家
	bool isPlayer()
	{
		return  getType() == CObj::OBJ_TYPE_PLAYER;
	}
	//返回当前坐标
	CPos & getPos();
	//获得上次的坐标
	CPos & getLastPos();
	//设置当前坐标为pos
	void setPos(CPos& pos);
	//每次变更坐标时，设置上一次的坐标
	void saveLastPos();
	//获得方向
	T_DIR getDir() { return  m_curPos.m_dir; } 
	//该物件，地面通行等级
	//阻挡点设置策略：将阻挡点划分为不同等级，只有通行等级大于该等级，才能通过，
	//通过后清除自己相应等级上得阻挡点。
	virtual UINT8 getDriveLevel() const { return 0; }
	//占位物体：移动时候，是否造成路面的阻挡,以影响其他物件的行进 
	virtual bool isBlockObj() const { return false; }
	//判断物体的可见状态。物体可能在不同的状态变得不可见。
	virtual bool isVisible() const { return true; }
	//等待回收资源
	bool needClear(){return m_needClear;}
	//设置清空状态
	void setClearState() {	m_needClear = true; }
	//是否显示
	virtual bool isAppear() { return true ;};
public:
	//进入新的地图
	virtual bool intoNewMap(CMap * newMap,bool needInit = false){ return false; }
	//离开原来的地图
	virtual bool leaveMap() { return false; }
	//物件定时更新状态。由定时器执行。默认不做任何动作。
	virtual bool heartBeat(UINT64 uTime){ return true ;}
	//当物件出现在地图上时回调。用以填充物件信息。此信息将被发送到客户端  
	virtual void getAppear(Map::stPlayerData & data) {}
	//同上 
	virtual void getAppear(Map::NotifyNpcAppear::stNpcData &data) {}
	//同上
	virtual void getAppear(Map::NotifyHeroAppear::stHeroData &data) {}
	//进入地图时候调用
	virtual void onEnterMap(CMap *) {}
	//离开时地图时候调用
	virtual void onLeaveMap(CMap *) {}
#ifdef _NEW_SYN_METHOD
	//有物件进入视野
	bool  enterViewSight(UINT8 type,UINT32 moID);
	//有物件离开视野
	bool  leaveViewSight(UINT8 type,UINT32 moID);
	//视野里是否有物件
	bool  isInViewSight(UINT8 type,UINT32 moID);
#endif
protected:
	CMap * 	 m_pCurMap;		//当前地图
	CPos 	 m_lastPos;		// 上一次的位置
	CPos 	 m_curPos;		// 当前位置
	bool 	 m_needClear;	//是否需要清除
public:
	//是玩家，则返回玩家指针；否则，返回空
	GameUser * theGameUser();
	//是npc,则返回Npc指针
	CNpc     * theNpc();
	//是hero,则返回hero指针
	CHero  * theHero();
	//是item,则返回item指针
	CDropItemObj * theItem();
#ifdef _NEW_SYN_METHOD
	//最大看到的物件
#define MAX_SAW_OBJ  256
	std::set<UINT32> m_viewSight[OBJ_TYPE_MAX];                 //物件的视野
#endif
};
typedef std::list<CObj*> TObjList;
typedef std::set<CObj*> TObjSet;
typedef std::vector<CObj*> TObjVec;
typedef std::vector<GameUser*> TGameUserVec;
typedef std::vector<UINT32>  TVecUserID;

#endif
