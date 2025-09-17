#ifndef __AREA_H__
#define __AREA_H__
#include "Obj.h"
#include <map>
#include <TypesDef.h>
#include "GameUser.h"

//区域物件回调函数，注意，尽量不要在该函数实现中作改变位置的操作，以避免
//造成物件区域发生变化。会导致迭代器失效，引起当机。
struct MapObjCallBack
{
	MapObjCallBack(){}
	virtual ~MapObjCallBack(){}
	virtual bool operator()(CObj*)=0;
};

class CMap;
// 以方格形式分割地图区域
class CArea
{
public:
	CArea(CMap * pOwner);
	~CArea();

	// 广播范围大小（广播区域数量）
	const static int AREA_CZ = 3;
	const static int AREA_CX = 3;
	
	//区域大小
	const static int THE_Z = 1500;
	const static int THE_X = 2200;

	//区域加入新物件：不更新视野
	bool add(CObj * pObj);
	//退出本区域:不更新视野
	bool remove(CObj * pObj);
	//更新
	void update(UINT64 curTime);
	//玩家数量
	UINT32 userCount() {  return m_objs[CObj::OBJ_TYPE_PLAYER].size() ;}
	//获得该区域块的索引
	UINT32 getAreaIndex(UINT32 mapW) { return mapW * m_z + m_x ;}
	//遍历区域内所有物件
	bool execEveryObj(MapObjCallBack& callBack,CObj::ObjType);
protected:
	// 如果加入本区域的是玩家，增加玩家索引，便于快速索引
	void doAddPlayer(GameUser *);
	//玩家退出本区域全局索引时，同时退出玩家索引
	void doRemovePlayer(GameUser *);
private :
	CMap     *m_pOwner;					//所在的地图
public:
	//区域物件索引部分：
	TObjSet m_objs[CObj::OBJ_TYPE_MAX]; // 本区域内的所有objs
	TVecUserID m_broadcastUserIDs;		//本区域内的所有玩家和UserID列表
	TGameUserVec m_broadcastObjs;  		//本区域内所有玩家指针，和m_broadcastUserIDs相对应
	INT32 m_x, m_z;						//本区域所在的x，y坐标
	INT32 m_effectCount;				//有效性计数
};

typedef TPos2Idx<CArea> CAreaIdx;


class CAreaV
	:public std::vector<CArea*>
{
public:
	CAreaV();
};

class CAreaVResult
{
public:
	CAreaV m_leave;		//离开该屏
	CAreaV m_enter;		//进入该屏
	CAreaV m_common;	//不变的屏
};

class CAreaS : public std::set<CArea *>
{
public :
	CAreaS(){}
};

class CAreaSResult
{
public :
	CAreaS m_leave;		//离开的屏
	CAreaS m_enter;		//进入的屏
	CAreaS m_common;	//不变的屏
};
#endif

