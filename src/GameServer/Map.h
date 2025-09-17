#ifndef __MAP_H__
#define __MAP_H__

#include "Cell.h"
#include <string>
#include <tinyxml/tinyxml.h>
#include "Element.h"
#include "Area.h"
#include "NpcDataDefine.h"
#include "MapTemplate.h"
#include "GameUserManager.h"
#include <set>
//新手村地图ID
#define NEW_PERSON_MAPID 1
#define OPEN_SHOW_MAPID 10000
#define ONE2ONE_PVP_MAPID 150001
#define PKWITH_FRIEND_MAPID 20001
#define REALTIME_PVP_MAPID 20100
#define REALTIME_MOBA_MAPID 20101
#define MANOR_FIGHT_MAPID  20102
#define GUILD_MAP_ID  99999
#define HERO_LEAGUE_MAPID 150002

// #define PLAYER_APPEAR_DEBUG 1

enum MAP_STATE
{
	MAP_STATE_ACTIVE,		//活动状态
	MAP_STATE_UNLOAD,		//等待卸载，玩家不可进入
};

//火强信息
struct stFireWallInfo
{
	CPos m_center;
	UINT8 m_attType;
	UINT32 m_attMoID;
	UINT64 m_beginTime;
	UINT32 m_fireDamage;
	UINT64 m_nextCalcTime;
	UINT8  m_campID;

	stFireWallInfo()
	{
		m_attType = 0;
		m_attMoID = 0;
		m_beginTime = 0;
		m_fireDamage = 0;
		m_nextCalcTime = 0;
		m_campID = 0;
	}
};

class PathFinder;
class CMap : public Element
{
public:
	CMap();
	virtual ~CMap();
	//返回玩家数量
	UINT32 getPlayerCount();
	//获得第一个非机器人
	GameUser * getFirstGameUser();
	//返回最大屏索引
	CAreaIdx  getAreaIdxMax() { return m_areaIdxMax;}
	//是否副本
	virtual bool isCopyMap(){return false;}
	//由另外一个模板生成
	bool initByTemplate(stMapTemplate & templateMap);
	//初始化完毕
	virtual void afterInit() { }
	//更新
	virtual void timerUpdate(UINT64 uTime,UINT16 group);
	//是否需要卸载本地图
	bool needUnLoad() { return m_mapState == MAP_STATE_UNLOAD ;}
	//回收某个物件
	bool clearObj(CObj*);
	//卸载地图
	void unLoadMap();
	//根据areaIdx获得CArea对象
	CArea * operator [](const CAreaIdx& idx);
	//根据cellIdx 获得CCell 对象
	CCell * operator [](const CCellIdx& idx);
	//根据areaIdx获得CArea对象
	CArea * getArea(const CAreaIdx& idx);
	//世界坐标是否有效
	bool isValidPos(const CPos& pos);
	//各自坐标是否有效
	bool isValidIdx(const CAreaIdx& idx);
	//区域坐标是否有效
	bool isValidIdx(const CCellIdx& idx);
	//获得出生点
	bool getOnBirthPos(CPos & pos,UINT16 posID = 1); 
public :
	// 位置必须有效，否则失败
	bool add(CObj *pObj,CPos & pos);
	//同步增加
	virtual void synAdd(CObj *pObj,const CAreaIdx & areaIdx);
	//添加玩家后执行的操作
	void doAddPlayer(CArea * pArea);
	//删除
	bool remove(CObj * pObj,bool bSyn= true);
	//同步删除
	virtual void synRemove(CObj *pObj,const CAreaIdx & areaIdx);
	//删除玩家后进行操作
	void doRemovePlayer(CArea * pArea);
	//移动
	bool move(CObj *, CPos & tagetPos,bool teleport = false);
	//同步移动
	virtual void synMove(CObj *pObj,const CAreaIdx & areaIdxS,const CAreaIdx & areaIdxT);
	//同步瞬移
	virtual void synTeleport(CObj *pObj,const CAreaIdx & areaIdxS,const CAreaIdx & areaIdxT);
	//发送地图的基本信息给客户端
	void sendMapInfo(GameUser * pUser);
	//召唤npc出现在地图上 
	CNpc *  summonNpc(const NpcDefine & define,UINT32 initHP = 0);
	//召唤武将在地图上出现
	CHero * summonHero(UINT32 heroID,UINT32 ownerID,UINT16 level,UINT16 evolutionLv,CPos & pos,UINT32 initHP = 0,bool summonBySkill = false);
	//给地图上所有玩家发送协议 
	template <class T>
	void sendProto2Map(T & cmd,UINT32 exceptID = 0);
	//给周围的人发协议
	template <class T>
	void sendProto2Nine(CObj * pObj,T & cmd,UINT32 exceptID = 0);	
	//给某个玩家发送下列区域的物件信息
	void sendAreaObjInfo2User(GameUser * pUser,CAreaS & areas);
	//通知下列区域物件obj的信息
	void notifyObjAppear2Area(CAreaS & areas,CObj * pObj);
	//通知下列区域物件obj消失
	void notifyObjDisappear2Area(CAreaS & areas,CObj * pObj);
	//通知下列区域物件obj移动
	void notifyObjMove2Area(CAreaS & areas,CObj * pObj);
	//获得两点的距离
	static UINT32 getDistance(const CPos& pos1,const CPos& pos2);
	//获得两点的距离
	static UINT32 getDistance(const CCellIdx& idx1,const CCellIdx& idx2);
	//对地图中所有物件进行回调
	void execEveryObjInMap(MapObjCallBack& callBack,CObj::ObjType type);
	//设置阻挡信息
	bool setBlock(const CCellIdx& idx,UINT8 flag);
	//情况阻挡点信息
	bool clearBlock(const CCellIdx & idx,UINT8 flag);
	//检查阻挡点信息
	bool checkBlock(const CCellIdx& idx,UINT8 flag = BLOCK_TYPE_NPC);
	//返回寻路器
	PathFinder  * getPathFinder();
	//获得需要等级
	UINT16  getNeedLv()
	{
		return m_needLv;
	}
	//增加一个火强
	void addFireWall(CBeing * pAtter,CBeing * pDefer);
	//计算火强
	void processFireWall(UINT64 cur);
	//是否静态地图
	bool isStatic()
	{
		return m_type == MAP_TYPE_STATIC;
	}
	//是否要修改boss血量
	virtual bool isReSetBossHP() {  return false ;}	
	//获得boss血量
	virtual UINT32 getBossHP() { return 0; }
	//活得boss等级
	virtual UINT16 getBossLv() { return 0; }
	//是否限时武将
	virtual bool isShowHero() { return false ; }
	//获取Being，一些特殊地图使用，这些地图中临时创建了GameUser数据
	virtual CBeing* getBeing(UINT8 type,UINT32 moID) {return NULL;}
public :
	//获得周围的九空格
	bool getNineArea(const CAreaIdx& center, CAreaS & areas);
	//获得离开、进入、公共区域
	bool getSynArea(const CAreaIdx& srcCenter,const CAreaIdx& dstCenter,CAreaSResult & result);	
public :
	UINT8 m_type;										//地图类型,动态还是静态
	MAP_STATE m_mapState;								//地图状态
protected :
	TObjSet  m_allObjs[CObj::OBJ_TYPE_MAX];
	INT32   m_mapX, m_mapZ;								//地图的X和Y范围,以世界坐标为单位
	CAreaIdx m_areaIdxMax;								//区域编号的范围，闭区间
	CCellIdx m_cellIdxMax;								//单元格编号的范围
	std::map<UINT16,stSummonWave> m_summonWave;			//召唤波次		
	typedef std::vector<CAreaV> TVecAreaV;
	TVecAreaV 			m_vecAreaV;
	std::set<CArea *>   m_vecEffectArea;
	std::map<UINT16,CPos>	m_bornPoss;
	UINT16 				m_npcSummonWave;                 //npc召唤波次
	UINT32				m_summonTime;					 //召唤时间
	UINT32				m_levelID;						 //关卡ID
	std::vector< std::vector<UINT8> >  m_blockInfos;	 //阻挡信息
	PathFinder			*m_pPathFinder;					 //寻路
	UINT16				m_needLv;						 //需要等级
	std::list<stFireWallInfo> m_fireWallsVec;			//火强
	stMapTemplate      *m_pMapTemplate;					//地图模板
};

template <class T>
void CMap::sendProto2Map(T & cmd,UINT32 exceptID)
{
	if (g_gameServer->isPubGameServer())
	{
		TObjSet::iterator  it = m_allObjs[CObj::OBJ_TYPE_PLAYER].begin();
		for (;it != m_allObjs[CObj::OBJ_TYPE_PLAYER].end();++it){
			GameUser * pUser = (GameUser *)*it;
			if (pUser->getID() != exceptID) {
				pUser->sendProto2Me(cmd);
			}
		}
	}else
	{
		TVecUserID allNotifyUser;
		TObjSet::iterator  it = m_allObjs[CObj::OBJ_TYPE_PLAYER].begin();
		for (;it != m_allObjs[CObj::OBJ_TYPE_PLAYER].end();++it){
			GameUser * pUser = (GameUser *)*it;
			if (pUser->getID() != exceptID) {
				allNotifyUser.push_back(pUser->getID());
			}
		}

		GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,cmd);
	}
}
template <class T>
void CMap::sendProto2Nine(CObj * pObj,T & cmd,UINT32 exceptID )
{
	if (!pObj){
		return ;
	}
	
	TVecUserID allNotifyUser;

	CAreaIdx centerArea(pObj->getPos());
	CAreaS result;
	getNineArea(centerArea,result);
	CAreaS::iterator it = result.begin();
	for (;it != result.end(); ++it){
		CArea * pArea = *it;
		TObjSet::iterator  subIt = pArea->m_objs[CObj::OBJ_TYPE_PLAYER].begin();
		for (;subIt != pArea->m_objs[CObj::OBJ_TYPE_PLAYER].end();++subIt){
			GameUser * pUser = (GameUser *)*subIt;
			if (pUser && (pUser->getID() != exceptID)){
				if (g_gameServer->isPubGameServer())
				{
					pUser->sendProto2Me(cmd);
				}else
				{
					allNotifyUser.push_back(pUser->getID());
				}
			}
		}
	}
	if (!g_gameServer->isPubGameServer())
	{
		GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,cmd);
	}
}
#endif
