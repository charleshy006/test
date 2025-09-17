#include "Map.h"
#include <assert.h>
#include "Log4cxxHelp.h"
#include "AthenaNew.h"
#include "Game.pb.h"
#include "GameServer.h"
#include "Map.pb.h"
#include "Map.h"
#include "MapMgr.h"
#include "PacketProcessor.h"
#include "GameUserManager.h"
#include "HeroMgr.h"
#include "PathFinder.h"
#include "DropItemObj.h"
#include "RobotMgr.h"
#include "CopyMap.h"

using namespace std;

CMap::CMap()
{
	m_mapX  = 0;
	m_mapZ  = 0;
	m_npcSummonWave = 0;
	m_summonTime = 0;
	m_levelID = 0;
	m_pPathFinder = NULL;
	m_needLv = 0;
	m_pMapTemplate = NULL;
}


CMap::~CMap()
{
	SAFE_DELETE(m_pPathFinder);
}


bool CMap::initByTemplate(stMapTemplate & templateMap)
{
	m_pMapTemplate = &templateMap;
	setID(templateMap.m_mapID);
	setName(templateMap.m_name.c_str());
	//初始化区域
	m_mapX = templateMap.m_mapX;
	m_mapZ = templateMap.m_mapZ;
		
	if (MAP_TYPE_STATIC == templateMap.m_type){//如果是静态地图
		setTempID(getID());	//静态地图的临时ID跟ID一样
	}
	else if (MAP_TYPE_DYN == templateMap.m_type){//如果是动态地图
		UINT64 tempid = 0;
		if (CMapMgr::getSingleton().genUniqueID(tempid)){
			setTempID(tempid);
		}
		else {
			Athena::logger->error("[初始化地图]分配临时ID错误");
			return false;
		}
	}
	else {
		Athena::logger->error("[初始化地图]要创建的地图类型不存在");
		return false;
	}

	// 初始化成员变量
	m_areaIdxMax = templateMap.m_areaIdxMax;
	m_cellIdxMax = templateMap.m_cellIdxMax;

	m_vecAreaV.resize(m_areaIdxMax.m_x + 1);
	for (int i=0, iEnd = m_vecAreaV.size(); i < iEnd; i++) {
		m_vecAreaV[i].resize(m_areaIdxMax.m_z + 1);
		for (int j=0, jEnd = m_areaIdxMax.m_z + 1; j < jEnd; j++) {
			CArea * pArea = ATHENA_NEW CArea(this);
			m_vecAreaV[i][j] = pArea;
			pArea->m_x = i;
			pArea->m_z = j;
		}
	}
	
	m_pPathFinder = ATHENA_NEW PathFinder();
	m_pPathFinder->init((m_cellIdxMax.m_x + 1) , (m_cellIdxMax.m_z + 1));

	m_blockInfos.resize(m_cellIdxMax.m_x + 1);
	for (UINT16 i = 0;i <= m_cellIdxMax.m_x; ++i){
		m_blockInfos[i].resize(m_cellIdxMax.m_z + 1);
	}

	m_summonWave = templateMap.m_summonWave;	
	
	m_mapState = MAP_STATE_ACTIVE; 
	m_bornPoss = templateMap.m_bornPoss;
	m_type = templateMap.m_type;
	m_needLv = templateMap.m_needLv;

	/*if (m_summonWave.size() > 0){
		std::map<UINT16,stSummonWave>::iterator it = m_summonWave.begin();	
		stSummonWave & wave = it->second;
		m_summonTime = g_gameServer->getSec() + wave.m_delay;
	}*/

	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(getID());
	if (pCopyMapBase){
		m_levelID = pCopyMapBase->m_id;
	}
	afterInit();
	return true;
}

bool CMap::isValidPos(const CPos& pos)
{
	if (!pos.isValid()){
		return false;
	}
	return (pos.m_x >= 0)&&(pos.m_z >= 0)&&(pos.m_x <= m_mapX) && (pos.m_z <= m_mapZ);
}

bool CMap::isValidIdx(const CAreaIdx& idx)
{
	if (!idx.isValid()){
		return false;
	}
	return (idx.m_x >= 0)&&(idx.m_z >= 0) &&(idx.m_x <= m_areaIdxMax.m_x) && (idx.m_z <= m_areaIdxMax.m_z);
}

bool CMap::isValidIdx(const CCellIdx& idx)
{
	if (!idx.isValid()){
		return false;
	}
	return (idx.m_x >= 0) && (idx.m_z >= 0) && (idx.m_x <= m_cellIdxMax.m_x) && (idx.m_z <= m_cellIdxMax.m_z);
}

CArea * CMap::getArea(const CAreaIdx& idx)
{
	if (!isValidIdx(idx)){
		return NULL;
	}
	// 无需再检查idxg
	return m_vecAreaV[idx.m_x][idx.m_z];
}

CArea * CMap::operator [](const CAreaIdx& idx)
{
	if (!isValidIdx(idx)){
		return NULL;
	}
	return getArea(idx);
}

bool CMap::getOnBirthPos(CPos & pos,UINT16 posID)
{
	std::map<UINT16,CPos>::iterator it = m_bornPoss.find(posID);
	if (it == m_bornPoss.end()){
		return  false;
	}
	pos = it->second;
	return true;
}

//回收掉某个物件
bool CMap::clearObj(CObj * pObj)
{
	if (!remove(pObj)){
		return false;
	}
	
	/*Map::NotifyObjDisappear  cmd;
	Map::NotifyObjDisappear_stObjDisappearInfo * pAddOne = cmd.add_data();
	pAddOne->set_motype(pObj->getType());
	pAddOne->set_moid(pObj->getTempID());
	this->sendProto2Nine(pObj,cmd);*/
#ifdef  _HDZ_DEBUG
	Athena::logger->trace("清空了id=%d,type=%d,tempID=%d",pObj->getID(),pObj->getType(),pObj->getTempID());
#endif
	CNpc* pNpc = pObj->theNpc();
	if(pNpc){
		CNpcMgr::getSingleton().remove(pNpc);
	}
	
	CHero * pHero = pObj->theHero();
	if (pHero){
		 CHeroMgr::getSingleton().remove(pHero);
	}
	
	CDropItemObj * pItem = pObj->theItem();
	if (pItem){
		CDropItemObjMgr::getSingleton().removeDropItemObj(pItem);	
	}
	
	GameUser * pUser = pObj->theGameUser();
	if (!pUser){//玩家不能在这里释放
		SAFE_DELETE(pObj);
	}
	else {
		pObj->setMap(NULL);
		if (pUser && pUser->m_bRobot){
			CRobotMgr::getSingleton().returnRobot(m_levelID,pUser->getID());	
		}
	}
	
	return true;
}


void CMap::unLoadMap()
{
	if (MAP_STATE_UNLOAD == m_mapState){
		return ;
	}
	m_mapState = MAP_STATE_UNLOAD;
	std::set<CObj *> delSet;
	for (UINT16 i = 0;i < CObj::OBJ_TYPE_MAX;++i){
		delSet.insert(m_allObjs[i].begin(),m_allObjs[i].end());
	}
	
	for (std::set<CObj*>::iterator it = delSet.begin();it != delSet.end();++it){
		clearObj(*it);
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("[地图关闭]id=%d,tempid=%d",getID(),getTempID());
#endif
}

void CMap::timerUpdate(UINT64 cur,UINT16 group)
{
	//这里执行AI计算结果：等待切换地图的物件
	//不在以上循环中执行
	std::set<CArea *>::iterator areaIt =  m_vecEffectArea.begin();
	std::set<CArea *>::iterator areaEnd =  m_vecEffectArea.end();
	for (;areaIt != areaEnd; ++areaIt){
		UINT32 curGroup = (*areaIt)->getAreaIndex(this->m_areaIdxMax.m_x) % MAX_NPC_GROUP;
		if (curGroup  == group){
			(*areaIt)->update(cur);
		}
	}
	
	std::vector<CObj*> m_objsNeedClear;
	for (UINT16 i = 0;i < CObj::OBJ_TYPE_MAX;++i){
		//遍历所有地图物件
		for(TObjSet::iterator itr = m_allObjs[i].begin(); itr != m_allObjs[i].end();)
		{
			CObj* pObj = *itr;

			itr++;

			if(pObj->needClear())//需要回收释放的物件
			{
				//需要清除
				//需要退出索引
				m_objsNeedClear.push_back(pObj);
			}
			else
			{
				//函数内部不能实现的东西：NPC自行切换地图
				//不可在内部执行物件更改地图的操作，尤其是本物体造成其他物体更改地图
				//允许执行在内部增加物件的操作：比如怪物掉落，怪物召唤宠物等
				GameUser *pUser = pObj->theGameUser();
				if (pUser){
					if ((pUser->m_groupID % MAX_NPC_GROUP) == group){
						pObj->heartBeat(cur);
					}
				}
				else {
					pObj->heartBeat(cur);
				}
			}
		}
	}
	
	processFireWall(cur);

	//得到所有需要回收的地图物件
	for(std::vector<CObj*>::iterator itr = m_objsNeedClear.begin();itr!=m_objsNeedClear.end();itr++)
	{
		CObj* pObj = *itr;
		if(pObj)
		{
			clearObj(pObj);	
		}
	}

}

bool CMap::add(CObj * pObj,CPos & pos)
{
	//检查是否给分配了唯一ID
	if(!pObj->getTempID()){
		Athena::logger->error("[地图添加物件](name:%s,id:%d)失败，因为没有临时ID",pObj->getName(),pObj->getID());
		return false;
	}
	
	if (!isValidPos(pos)){
		Athena::logger->error("[地图添加物件](name:%s,id:%d)x=%d,y=%d,z=%d失败,坐标不合法",pObj->getName(),pObj->getID(),pos.m_x,pos.m_y,pos.m_z);
		return false;
	}
	
	if (pObj->getType() >= CObj::OBJ_TYPE_MAX){
		Athena::logger->error("[地图添加物件](name:%s,id:%d)物件的类型不对",pObj->getName(),pObj->getID());
		return false;
	}

	if (m_allObjs[pObj->getType()].find(pObj) != m_allObjs[pObj->getType()].end()){
		Athena::logger->error("[地图添加物件](name:%s,id:%d)失败,物件已经在地图中",pObj->getName(),pObj->getID());
		return false;
	}

	//各自坐标的判定	
	CCellIdx cellIdx(pos);
	if (!cellIdx.isValid()){
		return false;
	}

	// 将Obj加到Area中
	CAreaIdx areaIdx(pos);
	CArea * pArea = (*this)[areaIdx];
	if (!pArea){
		Athena::logger->error("[地图添加物件]失败,找不到所在的area");
		return false;
	}

	pArea->add(pObj);
	pObj->setMap(this);
	pObj->setPos(pos);

	if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
		doAddPlayer(pArea);
	}
		
	m_allObjs[pObj->getType()].insert(pObj);
	pObj->onEnterMap(this);
	//放好物件后同步
	if (MAP_TYPE_STATIC != m_type){//静态地图,比如主城这些不同步
		this->synAdd(pObj,areaIdx);
	}
	return true;
}

void CMap::sendAreaObjInfo2User(GameUser * pUser,CAreaS & areas)
{
	//如果是玩家还要把其它物件发给他
	Map::NotifyPlayerAppear playerCmd;

	Map::NotifyNpcAppear npcCmd;

	Map::NotifyHeroAppear heroCmd;
	
	//Map::stNotifyItemAppear itemCmd;

	CAreaS::iterator it = areas.begin();
	for (;it != areas.end();++it){
		CArea * pArea = *it;
		TObjSet::iterator  subIt = pArea->m_objs[CObj::OBJ_TYPE_PLAYER].begin();
		for (;subIt != pArea->m_objs[CObj::OBJ_TYPE_PLAYER].end();++subIt){
			CObj * pSynObj = *subIt;
			if ((CObj *)pUser == pSynObj) {//是自己不用发
				continue;
			}
#ifdef _NEW_SYN_METHOD
			if (!pUser->enterViewSight(pSynObj->getType(),pSynObj->getUniqueID())){
				continue;
			}
#endif
			Map::stPlayerData  * pPlayerData = ATHENA_NEW Map::stPlayerData();
			if (!pPlayerData){
				return ;
			}
			pSynObj->getAppear(*pPlayerData);

			if (pPlayerData->ByteSize() + playerCmd.ByteSize() >= (INT32)(MAX_DATASIZE - 5)){//人数满了
				pUser->sendProto2Me(playerCmd);
				playerCmd.mutable_data()->Clear();
#ifdef PLAYER_APPEAR_DEBUG
				Athena::logger->trace("[Map]给玩家charID=%d发送了NotifyPlayerAppear协议 targetID[%d]",pUser->getID(), pSynObj->getID());
#endif
			}
			playerCmd.mutable_data()->AddAllocated(pPlayerData);
		}

		subIt = pArea->m_objs[CObj::OBJ_TYPE_MONSTER].begin();
		for (;subIt != pArea->m_objs[CObj::OBJ_TYPE_MONSTER].end(); ++subIt){
			CObj * pSynObj = *subIt;
			
#ifdef _NEW_SYN_METHOD
			if (!pUser->enterViewSight(pSynObj->getType(),pSynObj->getUniqueID())){
				continue;
			}
#endif
			Map::NotifyNpcAppear_stNpcData *pNpcData = ATHENA_NEW Map::NotifyNpcAppear_stNpcData();
			if (!pNpcData){
				return ;
			}
			pSynObj->getAppear(*pNpcData);
			if (npcCmd.ByteSize() +  pNpcData->ByteSize()>= (INT32)(MAX_DATASIZE - 5)){//满了,先发送
				pUser->sendProto2Me(npcCmd);
				npcCmd.mutable_data()->Clear();
			}
			npcCmd.mutable_data()->AddAllocated(pNpcData);
		}

		subIt = pArea->m_objs[CObj::OBJ_TYPE_HERO].begin();
		for (;subIt != pArea->m_objs[CObj::OBJ_TYPE_HERO].end(); ++subIt){
			CObj * pSynObj = *subIt;

#ifdef _NEW_SYN_METHOD
			if (!pUser->enterViewSight(pSynObj->getType(),pSynObj->getUniqueID())){
				continue;
			}
#endif

			Map::NotifyHeroAppear_stHeroData * pHeroData = ATHENA_NEW Map::NotifyHeroAppear_stHeroData();
			 pSynObj->getAppear(*pHeroData);
			if (heroCmd.ByteSize() +  pHeroData->ByteSize()>= (INT32)(MAX_DATASIZE - 5)){
				pUser->sendProto2Me(heroCmd);
				heroCmd.mutable_data()->Clear();
			}
			heroCmd.mutable_data()->AddAllocated(pHeroData);	
		}

		/*subIt = pArea->m_objs[CObj::OBJ_TYPE_ITEM].begin();
		for (;subIt != pArea->m_objs[CObj::OBJ_TYPE_ITEM].end(); ++subIt){
			CObj * pSynObj = *subIt;
			Map::stNotifyItemAppear::stItemData * pItemData = ATHENA_NEW Map::stNotifyItemAppear::stItemData();
			pSynObj->getAppear(*pItemData);
			if (itemCmd.ByteSize() + pItemData->ByteSize() >= (INT32)(MAX_DATASIZE - 5)){
				pUser->sendProto2Me(itemCmd);
				itemCmd.mutable_itemdatas()->Clear();
			}
			itemCmd.mutable_itemdatas()->AddAllocated(pItemData);
		}*/
	}

	if (playerCmd.data_size()){
		pUser->sendProto2Me(playerCmd);
#ifdef PLAYER_APPEAR_DEBUG
		for (UINT32 i = 0; i < (UINT32)playerCmd.data_size();++i){
			Athena::logger->trace("[Map]给charID=%d发送了charID=%d",pUser->getID(),playerCmd.data(i).moid());
		}
#endif
	}

	if (npcCmd.data_size()){
		pUser->sendProto2Me(npcCmd);
	}

	if (heroCmd.data_size()){
		pUser->sendProto2Me(heroCmd);
	}
}

void CMap::notifyObjAppear2Area(CAreaS & areas,CObj * pObj)
{
	if (!pObj){
		return ;
	}
	
	CAreaS::iterator it = areas.begin();
	TVecUserID  allNotifyUser;
	allNotifyUser.reserve(1024);

#ifdef _NEW_SYN_METHOD
	TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
	TObjSet::iterator objSetIt = objSet.begin();
	for (;objSetIt != objSet.end();++objSetIt){
		CObj * pSynObj = *objSetIt;
		if (pSynObj->enterViewSight(pObj->getType(),pObj->getUniqueID())){
			allNotifyUser.push_back(pObj->getUniqueID());	
		}
	}
#else
	for (;it != areas.end();++it){
		CArea * pArea = *it;
		UINT32 capacity = allNotifyUser.size() + pArea->m_broadcastUserIDs.size();
		allNotifyUser.reserve(capacity > 1024 ? capacity : 1024);
		allNotifyUser.insert(allNotifyUser.end(),pArea->m_broadcastUserIDs.begin(),pArea->m_broadcastUserIDs.end());
	}
#endif
	if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
		Map::NotifyPlayerAppear playerCmd;
		Map::stPlayerData  * pPlayerData = playerCmd.add_data();
		if (!pPlayerData){
			return ;
		}
		pObj->getAppear(*pPlayerData);
		GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,playerCmd);
#ifdef PLAYER_APPEAR_DEBUG
		Athena::logger->trace("[Map]给下列玩家通知了charID=%d,发送了NotifyPlayerAppear协议",pObj->getID());
		for (UINT32 i = 0;i < allNotifyUser.size();++i){
			if (allNotifyUser[i] > 0)
			{
				Athena::logger->trace("charID=%d",allNotifyUser[i]);
			}
		}
#endif
	}
	else if (CObj::OBJ_TYPE_MONSTER == pObj->getType()){
		Map::NotifyNpcAppear npcCmd;
		Map::NotifyNpcAppear_stNpcData *pNpcData = npcCmd.add_data();
		if (!pNpcData){
			return ;
		}
		pObj->getAppear(*pNpcData);
		GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,npcCmd);
	}
	else if (CObj::OBJ_TYPE_HERO == pObj->getType()){
		Map::NotifyHeroAppear heroCmd;
		Map::NotifyHeroAppear_stHeroData * pHeroData = heroCmd.add_data();
		if (!pHeroData){
			return ;
		}
		pObj->getAppear(*pHeroData);
		GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,heroCmd);
	}
	/*else if (CObj::OBJ_TYPE_ITEM == pObj->getType()){
		Map::stNotifyItemAppear  itemCmd;
		Map::stNotifyItemAppear::stItemData * pItemData = itemCmd.add_itemdatas();
		if (pItemData){
			pObj->getAppear(*pItemData);
			GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,itemCmd);
		}
	}*/
}

void CMap::notifyObjDisappear2Area(CAreaS & areas,CObj * pObj)
{
	if (!pObj){
		return;
	}

	CAreaS::iterator it = areas.begin();
	TVecUserID  allNotifyUser;
	allNotifyUser.reserve(1024);

#ifdef _NEW_SYN_METHOD
	TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
	TObjSet::iterator objSetIt = objSet.begin();
	for (;objSetIt != objSet.end();++objSetIt){
		CObj * pSynObj = *objSetIt;
		if (pSynObj->leaveViewSight(pObj->getType(),pObj->getUniqueID())){
			allNotifyUser.push_back(pObj->getUniqueID());	
		}
	}
#else
	for (;it != areas.end();++it){
		CArea * pArea = *it;
		UINT32 capacity = allNotifyUser.size() + pArea->m_broadcastUserIDs.size();
		allNotifyUser.reserve(capacity > 1024 ? capacity : 1024);
		allNotifyUser.insert(allNotifyUser.end(),pArea->m_broadcastUserIDs.begin(),pArea->m_broadcastUserIDs.end());
	}
#endif	
	Map::NotifyObjDisappear cmd;
	Map::NotifyObjDisappear_stObjDisappearInfo * disappearInfo = cmd.add_data();
	if (!disappearInfo){
		return ;
	}
	
	disappearInfo->set_motype(pObj->getType());
	disappearInfo->set_moid(pObj->getUniqueID());
	GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,cmd);

#ifdef _HDZ_DEBUG
	Athena::logger->trace("给下列玩家发送了玩家消失协议:");
	for (UINT16 i = 0;i < allNotifyUser.size();++i){
		Athena::logger->trace("给charID=%d发送了NotifyObjDisappear",allNotifyUser[i]);
	}
#endif
}

void CMap::notifyObjMove2Area(CAreaS & areas,CObj * pObj)
{
	CAreaS::iterator it = areas.begin();
	TVecUserID  allNotifyUser;
	allNotifyUser.reserve(1024);

#ifdef _NEW_SYN_METHOD
	TObjSet & objSet = m_allObjs[CObj::OBJ_TYPE_PLAYER];
	TObjSet::iterator objSetIt = objSet.begin();
	for (;objSetIt != objSet.end();++objSetIt){
		CObj * pSynObj = *objSetIt;
		if (pSynObj->isInViewSight(pObj->getType(),pObj->getUniqueID())){
			allNotifyUser.push_back(pObj->getUniqueID());	
		}
	}
#else 
	for (;it != areas.end();++it){
		CArea * pArea = *it;
		UINT32 capacity = allNotifyUser.size() + pArea->m_broadcastUserIDs.size();
		allNotifyUser.reserve(capacity > 1024 ? capacity : 1024);
		allNotifyUser.insert(allNotifyUser.end(),pArea->m_broadcastUserIDs.begin(),pArea->m_broadcastUserIDs.end());
	}
#endif	
	Map::stNotifyMODMove cmd;
	Map::stNotifyMODMove_stModMoveInfo * pMoveInfo = cmd.add_data();
	if (!pMoveInfo){
		return ;
	}
	pMoveInfo->set_type(pObj->getType());
	pMoveInfo->set_x(pObj->getPos().m_x);
	pMoveInfo->set_y(pObj->getPos().m_y);
	pMoveInfo->set_z(pObj->getPos().m_z);
	pMoveInfo->set_dir(pObj->getPos().m_dir);
	if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
		pMoveInfo->set_modid(pObj->getID());
	}
	else if (CObj::OBJ_TYPE_MONSTER == pObj->getType()){
		pMoveInfo->set_modid(pObj->getTempID());
#ifdef _HDZ_DEBUG
		Athena::logger->trace("通知了客户端怪物移动了!");
#endif
	}
	else if (CObj::OBJ_TYPE_HERO == pObj->getType()){
		pMoveInfo->set_modid(pObj->getTempID());
#ifdef _HDZ_DEBUG
		Athena::logger->trace("通知移动时候武将的moID=%d",pObj->getTempID());
#endif
	}

	GameUserManager::getSingleton().sendProto2UserList(allNotifyUser,cmd);
}

void CMap::synAdd(CObj *pObj,const CAreaIdx & areaIdx)
{
	CAreaS  areas;
	getNineArea(areaIdx,areas);
	//先通知别人加我
	//CAreaS::iterator it = areas.begin();
	/*for (;it != areas.end();++it){
		CArea * pArea = *it;
		pArea->notifyAdd(pObj);		
	}*/
	notifyObjAppear2Area(areas,pObj);
		

	//如果是玩家还要把其它物件发给他
	if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
		sendAreaObjInfo2User((GameUser *)pObj,areas);
	}
}

bool CMap::remove(CObj * pObj,bool bSyn)
{
	if (!pObj){
		return false;
	}	

	if (pObj->getType() >= CObj::OBJ_TYPE_MAX){
		Athena::logger->error("[移除物件]物件的type不正确!");
		return false;
	}

	if (m_allObjs[pObj->getType()].find(pObj) == m_allObjs[pObj->getType()].end()){
		return false;
	}

	// 获取当前的位置
	CPos pos = pObj->getPos();
	CCellIdx cellIdx(pos);

	CAreaIdx areaIdx(pos);
	CArea * pArea = (*this)[areaIdx];
	if (pArea){
		pArea->remove(pObj);
	}
		
	m_allObjs[pObj->getType()].erase(pObj);
	if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
		doRemovePlayer(pArea);
	}
	pObj->onLeaveMap(this);
	//放好物件后开始同步
	if (MAP_TYPE_STATIC != m_type) {//静态副本不同步
		if (bSyn) {
			synRemove(pObj,areaIdx);
		}
	}
	pObj->setMap(NULL);
	return true;
}

void CMap::synRemove(CObj *pObj,const CAreaIdx & areaIdx)
{
	CAreaS  areas;
	getNineArea(areaIdx,areas);
	//先通知别人删除我
	notifyObjDisappear2Area(areas,pObj);
}

void CMap::doAddPlayer(CArea * pArea)
{
	if (!pArea){
		return;
	}
	CAreaS areas;
	CAreaIdx center;
	center.m_x = pArea->m_x;
	center.m_z = pArea->m_z;
	getNineArea(center,areas);
	
	CAreaS::iterator it = areas.begin();
	for (;it != areas.end();++it){
		(*it)->m_effectCount++;		
		if (m_vecEffectArea.find(*it) == m_vecEffectArea.end()){
			m_vecEffectArea.insert(*it);
#ifdef _HDZ_DEBUG
		//Athena::logger->trace("增加有效屏(x=%d,y=%d)",areas[i]->m_x,areas[i]->m_y);
#endif
		}
	}
}

void CMap::doRemovePlayer(CArea * pArea)
{
	if (!pArea){
		return ;
	}

	CAreaS areas;
	CAreaIdx center;
	center.m_x = pArea->m_x;
	center.m_z = pArea->m_z;
	getNineArea(center,areas);
	
	CAreaS::iterator it = areas.begin();
	for (;it != areas.end();++it){
		(*it)->m_effectCount--;
		if ((*it)->m_effectCount <= 0){
			if (m_vecEffectArea.find(*it) != m_vecEffectArea.end()){
				(*it)->m_effectCount = 0;
				m_vecEffectArea.erase(*it);
#ifdef _HDZ_DEBUG
		//Athena::logger->trace("删除有效屏(x=%d,y=%d)",areas[i]->m_x,areas[i]->m_y);
#endif
			}
		}
	}
}

bool CMap::move(CObj * pObj, CPos & tagetPos,bool teleport)
{
	if (!pObj){
		return false;
	}

	if (!isValidPos(tagetPos)){
		Athena::logger->error("移动出现了不正确的坐标范围(name:%s,%d),tarPos(x:%d,y:%d,z:%d), levelId is %d",pObj->getName(),pObj->getUniqueID(),tagetPos.m_x,tagetPos.m_y,tagetPos.m_z, m_levelID);
		return false;
	}

	if (pObj->getType() >= CObj::OBJ_TYPE_MAX){
		return false;
	}

	TObjSet::iterator it = m_allObjs[pObj->getType()].find(pObj);
	if (it == m_allObjs[pObj->getType()].end()){
		return false;
	}


	// 获取当前的位置
	CPos sourcePos = pObj->getPos();
	CCellIdx cellSource = CCellIdx(sourcePos);
	CCellIdx cellTaget = CCellIdx(tagetPos);
	bool bCellChanged = (cellSource != cellTaget);

	CAreaIdx areaIdxS(sourcePos);
	CArea * pAreaS = (*this)[areaIdxS];

	CAreaIdx areaIdxT(tagetPos);
	CArea * pAreaT = (*this)[areaIdxT];
	
	bool bAreaChanged = areaIdxS != areaIdxT;
	
	if (bCellChanged){
		pObj->saveLastPos();
	}

	if (bAreaChanged){
		if (pAreaS && pAreaS->remove(pObj)){
			if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
				doRemovePlayer(pAreaS);
			}
		}

		if (pAreaT && pAreaT->add(pObj)){
			if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
				doAddPlayer(pAreaT);			
			}
		}
	}
	pObj->setPos(tagetPos);
#ifdef _HDZ_DEBUG
	if (CObj::OBJ_TYPE_MONSTER == pObj->getType()){
		//Athena::logger->trace("怪物id=%d,tempID=%d移动到了x=%d,y=%d,z=%d",pObj->getID(),pObj->getTempID(),pObj->getPos().m_x,pObj->getPos().m_y,pObj->getPos().m_z);
	}
#endif

	//如果是隐身状态,不需要同步网络,只需要改变索引即可
	if(!pObj->isVisible())
	{
		return true;
	}
	
	if (MAP_TYPE_STATIC != m_type) {//静态地图不同步
		if (!teleport){
			synMove(pObj,areaIdxS,areaIdxT);
		}
		else {
			synTeleport(pObj,areaIdxS,areaIdxT);	
		}
	}
	return true;
}

void CMap::synMove(CObj *pObj,const CAreaIdx & areaIdxS,const CAreaIdx & areaIdxT)
{
	CAreaSResult result;
	getSynArea(areaIdxS,areaIdxT,result);
	
	/*CAreaS::iterator it = result.m_leave.begin();
	for (;it != result.m_leave.end();++it){
		CArea * pArea = *it;
		pArea->notifyRemove(pObj);
	}*/
	notifyObjDisappear2Area(result.m_leave,pObj);	

	/*it = result.m_enter.begin();
	for (;it != result.m_enter.end();++it){
		CArea * pArea = * it;
		pArea->notifyAdd(pObj);
	}*/

	notifyObjAppear2Area(result.m_enter,pObj);

	if (CObj::OBJ_TYPE_PLAYER == pObj->getType()){
		sendAreaObjInfo2User((GameUser *)pObj,result.m_enter);
	}

	/*it = result.m_common.begin();
	for (;it != result.m_common.end();++it){
		CArea * pArea = * it;
		pArea->notifyMove(pObj);
	}*/
	notifyObjMove2Area(result.m_common,pObj);
}

struct stGetAllTeleportUser : public MapObjCallBack
{
	TVecUserID m_userIDVec;

	stGetAllTeleportUser()
	{
	
	}

	~stGetAllTeleportUser()
	{
	
	}

	virtual bool operator()(CObj* pObj)
	{
		if (!pObj || !pObj->theGameUser()){
			return true;
		}

		m_userIDVec.push_back(pObj->getID());
		return true;
	}
};

//同步瞬移
void CMap::synTeleport(CObj *pObj,const CAreaIdx & areaIdxS,const CAreaIdx & areaIdxT)
{
	if (!pObj){
		return ;
	}

	stGetAllTeleportUser cb;
	execEveryObjInMap(cb,CObj::OBJ_TYPE_PLAYER);
	Map::stInstantMove cmd;
	cmd.set_type(pObj->getType());
	cmd.set_moid(pObj->getUniqueID());
	cmd.mutable_destpos()->set_x(pObj->getPos().m_x);
	cmd.mutable_destpos()->set_y(pObj->getPos().m_y);
	cmd.mutable_destpos()->set_z(pObj->getPos().m_z);
	GameUserManager::getSingleton().sendProto2UserList(cb.m_userIDVec,cmd);
}

void CMap::sendMapInfo(GameUser *pUser)
{
	if (!pUser){
		return ;
	}
	Map::stNotifyEnterMapInfo mapInfo;
	mapInfo.set_mapid(m_levelID);
	pUser->sendProto2Me(mapInfo);
}

CNpc * CMap::summonNpc(const NpcDefine & define,UINT32 initHP)
{
	if (m_allObjs[CObj::OBJ_TYPE_MONSTER].size() > 70){
		Athena::logger->error("一个地图的召唤的武将不能超过70");
		return NULL;	
	}

	CNpc * pNpc = ATHENA_NEW CNpc();	
	if (!pNpc){
		Athena::logger->error("初始化分配NPC内存失败!");
		return NULL;
	}

	if (!pNpc->init(define.m_id,define.m_lv)){
		Athena::logger->error("召唤npc(id=%d)时候初始化npc数据失败!",define.m_id);
		SAFE_DELETE(pNpc);
		return NULL;
	}

	if (initHP){
		pNpc->initHP(initHP);
	}
	
	pNpc->setBornAnimID(define.m_aminID);
	pNpc->setBornDir(define.m_dirAngle);
	pNpc->setActionGroupID(define.m_actionGroupID);

	CPos bornPos;
	bornPos.m_x = define.m_x;
	bornPos.m_z = define.m_z;
	bornPos.m_y = define.m_y;
	
	if (define.m_isBoss){
		pNpc->setBoss();
		if (isReSetBossHP()){
			pNpc->initHP(getBossHP());
			pNpc->setLevel(getBossLv());
		}
	}

	if (define.m_lastTime > 0){
		pNpc->setClearMeTime(g_gameServer->getSec() + define.m_lastTime);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("刷出来的boss(%s)血量:HP:%d,maxHP:%d",pNpc->getName(),pNpc->getHP(),pNpc->getMaxHP());
#endif
	if (this->add(pNpc,bornPos)){
		if (!CNpcMgr::getSingleton().add(pNpc)){
			this->remove(pNpc);
			Athena::logger->error("召唤npc(id=%d)时候,往npc全局管理器添加npc时候失败!",define.m_id);
			SAFE_DELETE(pNpc);
			return NULL;
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("召唤npcID=%d成功",define.m_id);
#endif
	}
	else {
		Athena::logger->error("召唤NPC(id=%d)时候,往地图中添加npc不成功",define.m_id);
		SAFE_DELETE(pNpc);
		return NULL;
	}
	return pNpc;
}

CHero * CMap::summonHero(UINT32 heroID,UINT32 ownerID,UINT16 level,UINT16 evolutionLv,CPos & pos,UINT32 initHP,bool summonBySkill)
{
	if (m_allObjs[CObj::OBJ_TYPE_HERO].size() > 50){
		Athena::logger->error("一个地图的召唤的武将不能超过50");
		return NULL;	
	}

	CHero * pHero = ATHENA_NEW CHero();	
	if (!pHero){
		Athena::logger->error("初始化分配Hero内存失败!");
		return NULL;
	}

	if (!pHero->init(heroID,ownerID,level,evolutionLv, this)){
		Athena::logger->error("召唤hero(id=%d)时候初始化Hero数据失败!",heroID);
		SAFE_DELETE(pHero);
		return NULL;
	}
	
	pHero->setSummonBySkill(summonBySkill);

	if (initHP > 0){
		pHero->setHP(initHP);
	}

	if (this->add(pHero,pos)){
		if (!CHeroMgr::getSingleton().add(pHero)){
			this->remove(pHero);
			Athena::logger->error("召唤Hero(id=%d)时候,往HeroObj全局管理器添加HeroObj时候失败!",heroID);
			SAFE_DELETE(pHero);
			return NULL;
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("召唤HeroID=%d成功",heroID);
#endif
	}
	else {
		Athena::logger->error("召唤Hero(ID=%d)时候,往地图中添加heroObj不成功",heroID);
		SAFE_DELETE(pHero);
		return NULL;
	}
	return pHero;
}


bool CMap::getNineArea(const CAreaIdx& center, CAreaS & areas)
{
	if (!isValidIdx(center)){
		return false;
	}
	
	CAreaIdx minIdx,maxIdx;

	minIdx.m_x = center.m_x - 1;
	if (minIdx.m_x < 0) {
		minIdx.m_x = 0;
	}

	minIdx.m_z = center.m_z - 1;
	if (minIdx.m_z < 0){
		minIdx.m_z = 0;
	}

	maxIdx.m_x = center.m_x + 1;
	if (maxIdx.m_x > m_areaIdxMax.m_x){
		maxIdx.m_x = m_areaIdxMax.m_x;
	}

	maxIdx.m_z = center.m_z + 1;
	if (maxIdx.m_z > m_areaIdxMax.m_z){
		maxIdx.m_z = m_areaIdxMax.m_z;
	}
	
	if (!isValidIdx(minIdx)){
		return false;
	}

	if (!isValidIdx(maxIdx)){
		return false;
	}

	CAreaIdx a;
	for (a.m_z = minIdx.m_z; a.m_z <= maxIdx.m_z; a.m_z++){
		for (a.m_x = minIdx.m_x; a.m_x <= maxIdx.m_x; a.m_x++){
			CArea * pArea = getArea(a);
			if (pArea){
				areas.insert(pArea);
			}
		}
	}
	return true;
}

bool CMap::getSynArea(const CAreaIdx& srcCenter,const CAreaIdx& dstCenter,CAreaSResult & result)
{
	if (!isValidIdx(srcCenter))	{
		return false;	
	}

	if (!isValidIdx(dstCenter)){
		return false;
	}

	getNineArea(srcCenter,result.m_leave);
	getNineArea(dstCenter,result.m_enter);

	std::set_intersection(result.m_leave.begin(),
			result.m_leave.end(),
			result.m_enter.begin(),
			result.m_enter.end(),
			inserter(result.m_common,result.m_common.begin()));

	CAreaS::iterator it = result.m_common.begin();
	for (;it != result.m_common.end();++it){
		result.m_enter.erase(*it);
		result.m_leave.erase(*it);
	}
	return true;
}

UINT32 CMap::getDistance(const CPos& pos1,const CPos& pos2)
{
	return max(abs(pos1.m_x - pos2.m_x), abs(pos1.m_z - pos2.m_z));
	//CCellIdx index1(pos1);
	//CCellIdx index2(pos2);
	//return getDistance(index1,index2);
}

UINT32 CMap::getDistance(const CCellIdx& idx1,const CCellIdx& idx2)
{
	return max(abs(idx1.m_x-idx2.m_x), abs(idx1.m_z - idx2.m_z));	
}

void CMap::execEveryObjInMap(MapObjCallBack& callBack,CObj::ObjType type)
{
	if (type >= CObj::OBJ_TYPE_MAX){
		return ;
	}

	TObjSet::iterator it = m_allObjs[type].begin();	
	TObjSet::iterator end = m_allObjs[type].end();	
	for (;it != end;++it){
		if (!callBack(*it)){
			return ;	
		}
	}
}

bool CMap::setBlock(const CCellIdx& idx,UINT8 flag)
{
	m_blockInfos[idx.m_x][idx.m_z] |= flag;
	return true;
}

bool CMap::clearBlock(const CCellIdx & idx,UINT8 flag)
{
	m_blockInfos[idx.m_x][idx.m_z] &= (~flag);
	return true;
}

bool CMap::checkBlock(const CCellIdx& idx,UINT8 flag)
{
	if (!isValidIdx(idx)){
		return false;
	}
	return m_blockInfos[idx.m_x][idx.m_z] & flag;	
}

PathFinder  * CMap::getPathFinder()
{
	return m_pPathFinder;	
}

struct stGetAllFireWallDefer : public MapObjCallBack
{
	CPos m_center;
	UINT32 m_radius;
	UINT8  m_campID;
	std::vector<CBeing *> m_allDefers;

	stGetAllFireWallDefer(CPos &center,UINT32 radius,UINT8 campID) : m_center(center),m_radius(radius),m_campID(campID)
	{
	
	}

	~stGetAllFireWallDefer()
	{
	
	}

	virtual bool operator()(CObj* pObj)
	{
		if (!pObj || !pObj->getMap()){
			return true;
		}
		
		if ((pObj->getType()>= CObj::OBJ_TYPE_PLAYER) && (pObj->getType() <= CObj::OBJ_TYPE_HERO)){
			UINT32 dis = pObj->getMap()->getDistance(m_center,pObj->getPos());
			CBeing * pBeing = (CBeing *)pObj;
#ifdef _HDZ_DEBUG
			//Athena::logger->trace("检查火强伤害的物件:center(x:%d,y:%d,z:%d),campID:%d,obj id:%d,name:%s,(x:%d,y:%d,:z:%d),campID:%d,算出来的dis:%d",m_center.m_x,m_center.m_y,m_center.m_z,m_campID,pObj->getID(),pObj->getName(),pObj->getPos().m_x,pObj->getPos().m_y,pObj->getPos().m_z,pBeing->getCampID(),dis);
#endif
			if ((dis <= m_radius) && (pBeing->getCampID() != m_campID)){
				m_allDefers.push_back(((CBeing *)pObj));
				return true;
			}
		}
		else {
			return true;
		}
		return true;
	}
};

void CMap::addFireWall(CBeing * pAtter,CBeing * pDefer)
{
	if (!pAtter || !pDefer){
		return ;
	}

	stFireWallInfo info;
	info.m_beginTime = g_gameServer->getMSec();
	info.m_attType = pAtter->getType();
	info.m_attMoID = pAtter->getUniqueID();
	info.m_fireDamage = pAtter->getPKCalcData().m_fireDamage;
	info.m_center = pDefer->getPos();
	info.m_nextCalcTime = info.m_beginTime + 350;
	info.m_campID = pAtter->getCampID();
	m_fireWallsVec.push_back(info);
}

void CMap::processFireWall(UINT64 cur)
{
	std::list<stFireWallInfo>::iterator it = m_fireWallsVec.begin();

	for (;it != m_fireWallsVec.end();){
		std::list<stFireWallInfo>::iterator tmpIt = it;
		++it;
		if (tmpIt->m_beginTime + 4500 < cur){//时间到
			m_fireWallsVec.erase(tmpIt);
			continue;
		}
		
		if (tmpIt->m_nextCalcTime > cur){
			continue;
		}
#ifdef _HDZ_DEBUG
		Athena::logger->trace("触发了火强伤害:time:%llu",tmpIt->m_nextCalcTime);
#endif
		tmpIt->m_nextCalcTime += 350;

		
		stGetAllFireWallDefer cb(tmpIt->m_center,50,tmpIt->m_campID);
		execEveryObjInMap(cb,CObj::OBJ_TYPE_PLAYER);
		execEveryObjInMap(cb,CObj::OBJ_TYPE_MONSTER);
		execEveryObjInMap(cb,CObj::OBJ_TYPE_HERO);
		
		Map::stDirectDamge cmd;
		cmd.set_atttype(tmpIt->m_attType);
		cmd.set_attmodid(tmpIt->m_attMoID);
		cmd.set_result(Map::eAttResult_Fire_Damage);
		
		CBeing * pAtter = CBeing::getBeing(tmpIt->m_attType,tmpIt->m_attMoID,  this);

		for (UINT8 i = 0;i < cb.m_allDefers.size();++i){
			CBeing * pDefer = cb.m_allDefers[i];
			cmd.set_modtype(pDefer->getType());
			cmd.set_modid(pDefer->getUniqueID());
			pDefer->calcPKData();
			INT32 dam = (INT32)((tmpIt->m_fireDamage * 2 - pDefer->getPKCalcData().m_refFireDamage) * (g_randBetween(95,105) / 100.0f) );
			dam = std::max(dam,1);
			pDefer->decHP(dam,pAtter);

			/*if (pDefer->getHP() <= 0){//处理死亡
				pDefer->processDeath(pAtter);
			}*/  

			if (this->isCopyMap()) {//在公会副本
				if (pDefer->theNpc()){
					((CCopyMap*)this)->onNpcDecHP(pAtter,dam);
				}
			}

		/*	
			TObjSet::iterator  it = m_allObjs[CObj::OBJ_TYPE_PLAYER].begin();
			for (;it != m_allObjs[CObj::OBJ_TYPE_PLAYER].end();++it){
				CBeing * pUser = (CBeing *)*it;
				if (pUser) {
					if (this->isCopyMap()) {//在公会副本        
						if (pDefer->theNpc()){                  
							((CCopyMap*)this)->onNpcDecHP(pUser,dam);
						}     
					}
				}
			}
		*/	
			cmd.set_damage(dam);
			sendProto2Map(cmd);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("type:%d,modID:%d,收到了火强伤害:%d",pDefer->getType(),pDefer->getUniqueID(),dam);
#endif
		}
	}
}

UINT32 CMap::getPlayerCount()
{
	TObjSet::iterator it = m_allObjs[CObj::OBJ_TYPE_PLAYER].begin();
	TObjSet::iterator end = m_allObjs[CObj::OBJ_TYPE_PLAYER].end();
	UINT16 num = 0;
	for (;it != end;++it){
		CObj * pObj = *it;
		GameUser * pUser = (GameUser *)pObj;
		if (!pUser->m_bRobot){
			++num;
		}
	}
	return num;
}

GameUser * CMap::getFirstGameUser()
{
	TObjSet::iterator it = m_allObjs[CObj::OBJ_TYPE_PLAYER].begin();
	TObjSet::iterator end = m_allObjs[CObj::OBJ_TYPE_PLAYER].end();
	for (;it != end;++it){
		CObj * pObj = *it;
		GameUser * pUser = (GameUser *)pObj;
		if (!pUser->m_bRobot){
			return pUser;
		}
	}
	return NULL;
}
