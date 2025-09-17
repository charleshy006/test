#ifndef __MAP_MGR_H__
#define __MAP_MGR_H__
#include <string>
#include <map>
#include <TypesDef.h>
#include "UniqueID.h"
#include "AthenaNew.h"
#include "Element.h"
#include "ElementMgr.h"
#include "NpcMgr.h"
#include "MapTemplate.h"

#define  MAX_NPC_GROUP 10
#define  DYNMAP_TEMPID_BEGIN 1000
#define  DYNMAPID_BEGIN  101

//地图管理器
class GameUser;
class CMap;
class CCopyMap;

class CMapMgr : public ElementMgr
{
private :
		CMapMgr();
public  :
		~CMapMgr();
		USE_SINGLETON_NEW(CMapMgr)
public  :
	static CMapMgr & getSingleton();
	static void destroyMe();
	//根据路径，加载所有地图到模板库
	bool init(const char * szDir);
	//根据地图唯一ID，得到地图（包括动态与静态地图）
	CMap * getMapByID(UINT64 mapID);
	//根据名字来获得地图
	CMap * getMapByName(char * name);
	//定时更新所有地图状态
	void timerUpdate(UINT64 uTime);
	//根据模板创建静态地图
	CMap* createStaticMap(UINT32 id);
	//创建copymap对象
	CCopyMap *createCopyMap(UINT16 type);
	//创建关卡地图
	CCopyMap * createCopyMap(const char * szCustomName,UINT32 id);
	//自己定义分配器
	bool genUniqueID(UINT64 & tempid);
	//释放
	void putbackUniqueID(UINT64  tempid);
	//切换到新的地图
	bool changeMap(CBeing * pBeing,CMap * pNewMap,UINT16 posID = 1);
	//一组人切换地图
	bool teamChangeMap(std::vector<GameUser *> & vec,CMap * pNewMap);
	//机器人切换地图
	bool robotChangeMap(GameUser * pUser,CMap * pNewMap,UINT16 posID = 1);
protected:
	typedef std::map<UINT32,stMapTemplate> TMapTemplate;
private:
	TMapTemplate m_templateMap;			//模板地图库，用以迅速复制新地图。地图类型ID-地图模板指针
	UniqueIDMgr<UINT64> m_IDAllocater; //ID分配器:动态地图ID分配（1001-10000）
};

#endif

