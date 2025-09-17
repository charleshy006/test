#include "MapMgr.h"
#include "Map.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Log4cxxHelp.h"
#include "MapMgr.h"
#include "TblDataMgr.h"
#include "GameUserManager.h"
#include "LoadStat.h"
#include "Game.pb.h"
#include "GameServer.h"
#include "UtilFunction.h"
#include "KVPair.h"
#include "PacketProcessor.h"
#include "CopyMap.h"
#include "PathFinder.h"
#include "AIState.h"

using namespace std;

//自己定义分配器
bool CMapMgr::genUniqueID(UINT64 & tempid)
{
	tempid = m_IDAllocater.genID();
	if (m_IDAllocater.isValidID(tempid)){
		return true;
	}
	return false;
}

//释放临时ID
void CMapMgr::putbackUniqueID(UINT64  tempid)
{
	m_IDAllocater.putID(tempid);
}

CMapMgr::CMapMgr():ElementMgr(true,true,true,true),m_IDAllocater(DYNMAP_TEMPID_BEGIN,50000)
{
	AIStateMgr::getSingleton()
		(ATHENA_NEW IdleState)
		(ATHENA_NEW DeadState)
		(ATHENA_NEW CombatState)
		(ATHENA_NEW GoHomeState);
}

CMapMgr::~CMapMgr()
{
	//释放所有活动地图
	for (TTempIDMap_IT it = m_tempIDMap.begin(); it != m_tempIDMap.end();++it){
		CMap * pMap = (CMap *)it->second;
		SAFE_DELETE(pMap);
	}
}

CMapMgr& CMapMgr::getSingleton()
{
	return THE_SINGLETON::Instance();
}

void CMapMgr::destroyMe()
{
	THE_SINGLETON::destroy();
}

bool CMapMgr::init(const char * szDir)
{
	{
		TiXmlDocument doc;
		std::string lvCfgFile = Athena::global["confDir"] + "/OnlineLevel.xml";
		if (!doc.LoadFile(lvCfgFile.c_str())){
			Athena::logger->error("[创建地图]打开配置文件%s错误",lvCfgFile.c_str());
			return false;
		}

		TiXmlElement * pRoot = doc.FirstChildElement("root");
		if (!pRoot){
			Athena::logger->error("[创建地图]找不到OnlineLevel.xml的root节点");
			return false;
		}
		
		TiXmlElement * pOnlineLvNode = pRoot->FirstChildElement("Online");
		if (!pOnlineLvNode){
			Athena::logger->error("[创建地图]找不到OnlineLevel.xml的Online节点");
			return false;
		}
		INT32 iVal = 0;
		double dVal = 0.0;
		const char * szVal = NULL;
		CCellIdx maxCellSize;

		TiXmlElement * pLevelNode = pOnlineLvNode->FirstChildElement("Level");
		while (pLevelNode){
			stMapTemplate mapTmp;
			if (pLevelNode->Attribute("SpaceX",&iVal)){
				mapTmp.m_mapX = iVal * 100;
			}		
			
			if (pLevelNode->Attribute("SpaceZ",&iVal)){
				mapTmp.m_mapZ = iVal * 100;
			}		
			
			if (pLevelNode->Attribute("Dekaronnum",&iVal)){
				mapTmp.m_enterNum = iVal;
			}	
			
			if (pLevelNode->Attribute("Id",&iVal)){
				mapTmp.m_mapID = iVal;
			}
			
			if (pLevelNode->Attribute("needLv",&iVal)){
				mapTmp.m_needLv = iVal;
			}

			TiXmlElement * pBornPosNode = pLevelNode->FirstChildElement("BornPos");
			if (pBornPosNode) {
				TiXmlElement * pStartNode = pBornPosNode->FirstChildElement("Start");
				while (pStartNode) {
					CPos pos;
					UINT16 posID = 0;
					if (pStartNode->Attribute("Id",&iVal)){
						posID = iVal;
					}

					if (pStartNode->Attribute("StartX",&dVal)){
						pos.m_x = (INT32)(dVal * 100);
					}

					if (pStartNode->Attribute("StartY",&dVal)){
						pos.m_y = (INT32)(dVal * 100);
					}

					if (pStartNode->Attribute("StartZ",&dVal)){
						pos.m_z = (INT32)(dVal * 100);
					}
					
					if (pStartNode->Attribute("angle",&iVal)){
						pos.m_dir = iVal;
					}

					mapTmp.m_bornPoss.insert(make_pair(posID,pos));
					pStartNode = pStartNode->NextSiblingElement("Start");
				}
			}
			
			TiXmlElement * pWalkablesNode = pLevelNode->FirstChildElement("walkables");
			if (pWalkablesNode){
				TiXmlElement * pWalkableNode = pWalkablesNode->FirstChildElement("walkable");
				while (pWalkableNode){
					CRectArea rect;
					if ((szVal = pWalkableNode->Attribute("pos"))){
						std::vector<std::string> strVec;
						g_stringtok(strVec,szVal,",");
						if (strVec.size() >= 2){
							rect.m_pos.m_x = (INT32)atoi(strVec[0].c_str());
							rect.m_pos.m_y = 0;
							rect.m_pos.m_z = (INT32)atoi(strVec[1].c_str());
						}
					}

					if (pWalkableNode->Attribute("width",&iVal)){
						rect.m_width = iVal;	
					}

					if (pWalkableNode->Attribute("height",&iVal)){
						rect.m_height = iVal;
					}
					mapTmp.m_walkables.push_back(rect);
					pWalkableNode = pWalkableNode->NextSiblingElement("walkable");
				}
			}
			
			TiXmlElement * pAccumulatePointNode = pLevelNode->FirstChildElement("accumulatepoint");
			if (pAccumulatePointNode){
				TiXmlElement * pStartNode = pAccumulatePointNode->FirstChildElement("start");
				while (pStartNode){
					ZoneDefine zone;
					UINT16 id = 0;
					if (pStartNode->Attribute("Id",&iVal)){
						id = iVal;	
					}
					
					if (pStartNode->Attribute("StartX",&iVal)){
						zone.m_rect.m_pos.m_x = iVal * 100;	
					}
					
					if (pStartNode->Attribute("StartY",&iVal)){
						zone.m_rect.m_pos.m_y = iVal * 100;	
					}
					
					if (pStartNode->Attribute("StartZ",&iVal)){
						zone.m_rect.m_pos.m_z = iVal * 100;	
					}
					
					if (pStartNode->Attribute("radius",&iVal)){
						zone.m_rect.m_height = zone.m_rect.m_width = iVal * 100;	
					}
					zone.m_type = ZONE_TYPE_RES;
					mapTmp.m_resZoneDefines.insert(std::make_pair(id,zone));
					pStartNode = pStartNode->NextSiblingElement("start");
				}
			}
			
			TiXmlElement * pBuffNode = pLevelNode->FirstChildElement("buff");
			if (pBuffNode){
				TiXmlElement * pStartNode = pBuffNode->FirstChildElement("start");
				while (pStartNode){
					ZoneDefine zone;
					UINT16 id = 0;
					if (pStartNode->Attribute("Id",&iVal)){
						id = iVal;	
					}
					
					if (pStartNode->Attribute("StartX",&iVal)){
						zone.m_rect.m_pos.m_x = iVal * 100;	
					}
					
					if (pStartNode->Attribute("StartY",&iVal)){
						zone.m_rect.m_pos.m_y = iVal * 100;	
					}
					
					if (pStartNode->Attribute("StartZ",&iVal)){
						zone.m_rect.m_pos.m_z = iVal * 100;	
					}
					
					if (pStartNode->Attribute("radius",&iVal)){
						zone.m_rect.m_height = zone.m_rect.m_width = iVal * 100;	
					}
					zone.m_type = ZONE_TYPE_BUFF;
					mapTmp.m_buffZoneDefines.insert(std::make_pair(id,zone));
					pStartNode = pStartNode->NextSiblingElement("start");
				}
			}

			if ((szVal = pLevelNode->Attribute("name"))){
				mapTmp.m_name = szVal;
			}

			if (mapTmp.m_mapID < 10000){//小于10000的都是静态地图
				mapTmp.m_type = MAP_TYPE_STATIC;  
			}
			else {
				mapTmp.m_type = MAP_TYPE_DYN;
			}

			CPos maxWH;
			maxWH.m_x = mapTmp.m_mapX;
			maxWH.m_z = mapTmp.m_mapZ;
			maxWH.m_y = 0; 
			maxWH.m_dir = 0; 
			mapTmp.m_cellIdxMax = CCellIdx(maxWH);
			mapTmp.m_areaIdxMax = CAreaIdx(maxWH);
			
			if (maxCellSize.m_x < mapTmp.m_cellIdxMax.m_x){
				maxCellSize.m_x = mapTmp.m_cellIdxMax.m_x;
			}
			
			if (maxCellSize.m_z < mapTmp.m_cellIdxMax.m_z){
				maxCellSize.m_z = mapTmp.m_cellIdxMax.m_z;
			}

			TiXmlElement * pSectionsNode = pLevelNode->FirstChildElement("Sections");	
			if (pSectionsNode)
			{
				TiXmlElement * pSectionNode = pSectionsNode->FirstChildElement("Section");
				while (pSectionNode){
					stSummonWave wave;
					if (pSectionNode->Attribute("Id",&iVal)){
						wave.m_id = iVal;
					}	

					if (pSectionNode->Attribute("TriggerLogic",&iVal)){
						wave.m_triggerLogic = iVal;
					}	

					if (pSectionNode->Attribute("Delay",&iVal)){
						wave.m_delay = iVal;
					}	
					
					if (pSectionNode->Attribute("X",&dVal)){
						wave.m_dirPos.m_x = (INT32)(dVal * 100);
					}
					
					if (pSectionNode->Attribute("Y",&dVal)){
						wave.m_dirPos.m_y = (INT32)(dVal * 100);
					}
					
					if (pSectionNode->Attribute("Z",&dVal)){
						wave.m_dirPos.m_z = (INT32)(dVal * 100);
					}
					
					if (pSectionNode->Attribute("lasttime",&iVal)){
						wave.m_lastTime = iVal;
					}

					TiXmlElement * pMonsterNode = pSectionNode->FirstChildElement("monster");
					while (pMonsterNode){
						NpcDefine monster;		
						if (pMonsterNode->Attribute("Monsterid",&iVal)){
							monster.m_id = iVal;
						}	

						if (pMonsterNode->Attribute("Monsterlv",&iVal)){
							monster.m_lv = iVal;
						}	

						if (pMonsterNode->Attribute("WhetherBoss",&iVal)){
							if (iVal) {
								monster.m_isBoss = 1;
							}
						}	

						if (pMonsterNode->Attribute("CoordinateX",&dVal)){
							monster.m_x = (INT32)(dVal * 100);
						}	

						if (pMonsterNode->Attribute("CoordinateY",&dVal)){
							monster.m_y = (INT32)(dVal * 100);
						}	

						if (pMonsterNode->Attribute("CoordinateZ",&dVal)){
							monster.m_z = (INT32)(dVal * 100);
						}
						
						if (pMonsterNode->Attribute("aminID",&iVal)){
							monster.m_aminID = iVal;
						}
						
						if (pMonsterNode->Attribute("angle",&iVal)){
							monster.m_dirAngle = iVal;
						}
						
						if (pMonsterNode->Attribute("actionGroupID",&iVal)){
							monster.m_actionGroupID = iVal;
						}

						wave.m_npcDefines.push_back(monster);
						pMonsterNode = pMonsterNode->NextSiblingElement("monster");
					}
					mapTmp.m_summonWave[wave.m_id] = wave;
					pSectionNode = pSectionNode->NextSiblingElement("Section");
				}
			}
			m_templateMap[mapTmp.m_mapID] = mapTmp;
			pLevelNode = pLevelNode->NextSiblingElement("Level");
		}
		
		PathFinder::allocAstarNodeMem((maxCellSize.m_x + 1) * (maxCellSize.m_z + 1));
		Athena::logger->trace("寻路器分配的格子x最大为:%d,z最大为:%d",maxCellSize.m_x + 1,maxCellSize.m_z + 1);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("一共加载了%d个地图模板",m_templateMap.size());
	{
		TMapTemplate::iterator it = m_templateMap.begin();
		for (;it != m_templateMap.end();++it){
			stMapTemplate & pTmp = it->second;
			Athena::logger->trace("levelID=%d,x=%d,z=%d,enterNum=%d,cellX=%d,cellZ=%d,areaX=%d,areaZ=%d",it->first,pTmp.m_mapX,pTmp.m_mapZ,pTmp.m_enterNum,pTmp.m_cellIdxMax.m_x,pTmp.m_cellIdxMax.m_z,pTmp.m_areaIdxMax.m_x,pTmp.m_areaIdxMax.m_z);
			Athena::logger->trace("出生点有:");
			
			std::map<UINT16,CPos>::iterator posIt = pTmp.m_bornPoss.begin();
			for (;posIt != pTmp.m_bornPoss.end(); ++posIt){
				CPos & bornPos = posIt->second;
				Athena::logger->trace("出生点有:id=%d,x=%d,y=%d,z=%d",posIt->first,bornPos.m_x,bornPos.m_y,bornPos.m_z);
			}
			
			Athena::logger->trace("可行走区域:");
			for (UINT16 i = 0;i < pTmp.m_walkables.size();++i){
				CRectArea & rect = pTmp.m_walkables[i];
				Athena::logger->trace("可行走区域是:x=%d,y=%d,z=%d,width=%d,height=%d",rect.m_pos.m_x,rect.m_pos.m_y,rect.m_pos.m_z,rect.m_width,rect.m_height);
			}	

			Athena::logger->trace("召唤波次有:");
			std::map<UINT16,stSummonWave>::iterator subIt = pTmp.m_summonWave.begin();
			
			for (;subIt != pTmp.m_summonWave.end();++subIt){
				stSummonWave & wave = subIt->second;
				Athena::logger->trace("waveID=%d,logic=%d,delay=%d",wave.m_id,wave.m_triggerLogic,wave.m_delay);
				Athena::logger->trace("该波次召唤的monster有:");
				
				for (UINT8 i = 0;i < wave.m_npcDefines.size();++i){
					NpcDefine & define = wave.m_npcDefines[i];
					Athena::logger->trace("id=%d,lv=%d,x=%d,y=%d,z=%d,isBoss=%d,angle=%d",define.m_id,define.m_lv,define.m_x,define.m_y,define.m_z,define.m_isBoss,define.m_dirAngle);
				}
			}

			Athena::logger->trace("资源点:");
			std::map<UINT16,ZoneDefine>::iterator thdIt = pTmp.m_resZoneDefines.begin();
			for (;thdIt != pTmp.m_resZoneDefines.end();++thdIt){
				ZoneDefine & zoneDefine = thdIt->second;
				Athena::logger->trace("id:%d,type:%d,x:%d,y:%d,z:%d,width:%d,height:%d",thdIt->first,zoneDefine.m_type,zoneDefine.m_rect.m_pos.m_x,zoneDefine.m_rect.m_pos.m_y,zoneDefine.m_rect.m_pos.m_z,zoneDefine.m_rect.m_width,zoneDefine.m_rect.m_height);
			}

			Athena::logger->trace("buff点:");
			thdIt = pTmp.m_buffZoneDefines.begin();
			for (;thdIt != pTmp.m_buffZoneDefines.end();++thdIt){
				ZoneDefine & zoneDefine = thdIt->second;
				Athena::logger->trace("id:%d,type:%d,x:%d,y:%d,z:%d,width:%d,height:%d",thdIt->first,zoneDefine.m_type,zoneDefine.m_rect.m_pos.m_x,zoneDefine.m_rect.m_pos.m_y,zoneDefine.m_rect.m_pos.m_z,zoneDefine.m_rect.m_width,zoneDefine.m_rect.m_height);
			}
		}
	}
#endif
	
	{
		TiXmlDocument doc;
		std::string mapCfgFile = Athena::global["confDir"] + "/mapcfg.xml";
		if (!doc.LoadFile(mapCfgFile.c_str())){
			Athena::logger->trace("[创建地图]打开配置文件%s错误",mapCfgFile.c_str());
			return false;
		}

		TiXmlElement * pRoot = doc.FirstChildElement("MapCfg");
		if (!pRoot){
			Athena::logger->trace("[创建地图]找不到MapCfg.xml文件");
			return false;
		}

		INT32 workerID = (INT32)(atoi(Athena::global["svrID"].c_str())); 
		TiXmlElement *pElem = pRoot->FirstChildElement("Server");
		while (pElem){
			const char * szVal = pElem->Attribute("id");
			if ( workerID == atoi(szVal)){
				TiXmlElement * pMapElem = pElem->FirstChildElement("map");
				while (pMapElem){
					szVal = pMapElem->Attribute("id");
					CMap * tmpMap = createStaticMap(atoi(szVal));
					if (tmpMap){
						Athena::logger->trace("[创建地图]创建地图%d成功",tmpMap->getID());
					}
					pMapElem = pMapElem->NextSiblingElement("map");
				}
				break;
			}
			pElem = pElem->NextSiblingElement();
		}
	}
	return true;
}

CMap* CMapMgr::createStaticMap(UINT32 id)
{
	//寻找模板
	TMapTemplate::iterator itr = m_templateMap.find(id);
	if(itr==m_templateMap.end()){
		return NULL;
	}
	
	if (itr->second.m_type != MAP_TYPE_STATIC){
		return NULL;
	}

	bool retval = false;
	CMap *pMap = ATHENA_NEW CMap();
	if(!pMap){
		return NULL;
	}

	if(pMap->initByTemplate(itr->second)) {
		retval = addElement(pMap);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("初始化地图%d成功!",pMap->getID());
#endif
	}

	if (!retval) {
		SAFE_DELETE(pMap);
		pMap = NULL;
	}
	return pMap;
}

CCopyMap *CMapMgr::createCopyMap(UINT16 type)
{	
	if (eLevelType_Guild_Copymap == type){//公会组队副本
		return ATHENA_NEW CGuildTeamMap();	
	}
	else if (eLevelType_World_Boss == type){//世界boss
		return ATHENA_NEW CWorldBossMap();	
	}
	else if (eLevelType_RealTime == type){//实时
		return ATHENA_NEW CRealTimeMap();
	}
	else if (eLevelType_Team == type){//组队副本
		return ATHENA_NEW CTeamMap();
	}
	else if (eLevelType_Peakedness == type){//巅峰
		return ATHENA_NEW COne2OneMap();
	}
	else if (eLevelType_PK_With_Friend == type){
		return ATHENA_NEW COne2OneMap();
	}
	else if (eLevelType_Moba_RealTime == type){//moba3v3
		return ATHENA_NEW CMobaRealTime();
	}
	else if (eLevelType_ManorFight == type){//manorFight
		return ATHENA_NEW CManorFightMap();
	}
	else if(eLevelType_EmeraldDream == type){//翡翠梦境
		return ATHENA_NEW CEmeraldDreamMap();
	}
	else if(eLevelType_League == type){//英雄联赛
		return ATHENA_NEW CHeroLeagueMap();
	}
	else if(eLevelType_Dragon_Nest == type){//巨龙巢穴
		return ATHENA_NEW CDragonNestMap();
	}
	else if (eLevelType_AreaPKServerCheck == type) //服务器校验的天梯竞技场
	{
		return ATHENA_NEW CArenaMap();
	}
	else if (eLevelType_InterceptServerCheck == type) //服务器校验的拦截护送
	{
		return ATHENA_NEW CInterceptMap();
	}
	else if (eLevelType_LeitaiServerCheck == type) //服务器校验的擂台赛
	{
		return ATHENA_NEW CLeitaiMap();
	}
	else if(eLevelType_Stone_CopyMap == type)
	{
		return ATHENA_NEW CStoneCopyMap();
	}
	else {
		return ATHENA_NEW CCopyMap();
	}
}

CCopyMap * CMapMgr::createCopyMap(const char * szCustomName,UINT32 id)
{
	if (!szCustomName){
		return NULL;
	}
	
	stCopyMapBase * pCopyMapBase = g_copyMapBaseMgr.get(id);
	if (!pCopyMapBase){
		Athena::logger->error("创建副本失败,找不到copymapid=%d,的配置",id);
		return NULL;
	}
	

	//寻找模板
	TMapTemplate::iterator it = m_templateMap.find(id);
	if(it == m_templateMap.end()){
		Athena::logger->error("创建副本失败,没有找到模板!");
		return NULL;
	}
	
	if (it->second.m_type != MAP_TYPE_DYN){
		Athena::logger->error("创建副本失败,改副本不是动态的!");
		return NULL;
	}

	bool retval = false;
	CCopyMap *pMap = createCopyMap(pCopyMapBase->m_type);
	if(!pMap){
		return NULL;
	}

	if(pMap->initByTemplate(it->second)) {
		char mapName[MAX_NAMESIZE + 1] = {0};
		snprintf(mapName,sizeof(mapName),"%d%s",pCopyMapBase->m_id,szCustomName);	
		pMap->setName(mapName);
		retval = addElement(pMap);
		pMap->setCopyMapBase(pCopyMapBase);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("初始化关卡地图%d成功!",pMap->getID());
#endif
	}

	if (!retval) {
		struct ShowAllMapCB : execElement<Element> { 
			bool exec(Element* entry)
			{
				CCopyMap* u = (CCopyMap*)entry;
				Athena::logger->trace("现有地图 ID=%d, name=%s, tempid=%d",u->getID(),u->getName(), u->getTempID());
				return true;
			}
		};

		ShowAllMapCB cb;
		execEveryElement(cb);
		Athena::logger->error("创建副本失败,添加副本到容器不成功! ID=%d, name=%s, tempid=%d",pMap->getID(),pMap->getName(), pMap->getTempID());
		
		if (pMap->isCopyMap()){
			putbackUniqueID(pMap->getTempID());
		}
		SAFE_DELETE(pMap);
		pMap = NULL;
	}
	return pMap;
}

CMap * CMapMgr::getMapByID(UINT64 mapID)
{
	return (CMap *)getElementByTempID(mapID);
}

CMap * CMapMgr::getMapByName(char * name)
{
	if (!name){
		return NULL;
	}
	return (CMap *)getElementByName(name);
}


//遍历所有地图
void CMapMgr::timerUpdate(UINT64 uTime)
{
	static UINT32 group = 0;
	std::vector<UINT64> mapsClear;//待卸载的地图
	for (TTempIDMap_IT it = m_tempIDMap.begin(); it != m_tempIDMap.end();++it){
		//如果该地图是需要卸载的地图
		//则需要再这里进行卸载或者回收
		CMap * pMap = (CMap *)it->second;
		if(pMap->needUnLoad()) {//需要卸载本张地图
			//卸载地图之前，需要卸载场景所有物件,剔除所有玩家
			mapsClear.push_back(pMap->getTempID());
		}
		else {
			//更新该地图：包括地图状态以及物件状态
			pMap->timerUpdate(uTime,group);
		}
	}
			
	for(std::vector<UINT64>::iterator itr = mapsClear.begin(); itr!=mapsClear.end(); itr++) {
		//这些地图是否需要进行回收。。。
		CMap * pMap = getMapByID(*itr);
		if (pMap){
			Athena::logger->trace("[地图卸载]卸载了地图id=%d,tempid=%d name=%s",pMap->getID(),pMap->getTempID(), pMap->getName());
			removeElement(pMap);
			if (pMap->isCopyMap()){
				((CCopyMap *)pMap)->sendAllUser2City();
				putbackUniqueID(pMap->getTempID());
			}
			SAFE_DELETE(pMap);
		}
		else {
			Athena::logger->error("[地图卸载]在管理找不到该地图卸载错误!");
		}
	}
	group += 1;
	group = group % MAX_NPC_GROUP;
}

bool CMapMgr::teamChangeMap(std::vector<GameUser *> & vec,CMap * pNewMap)
{
	for (UINT8 i = 0;i < vec.size(); ++i){
		GameUser * pUser = vec[i];
		if (!changeMap((CBeing *)pUser,pNewMap)){
			Athena::logger->error("组队进入副本时候account=%s,name=%s,id=%d,进入副本不成功",pUser->getAccount(),pUser->getName(),pUser->getID());
		}	
	}
	return true;
}

bool CMapMgr::changeMap(CBeing * pBeing,CMap * pNewMap,UINT16 posID)
{
	if (!pBeing || !pNewMap){
		return false;
	}

	CMap * pOldMap = pBeing->getMap();
	
	if (!pOldMap){
		return false;
	}
	
	if (pOldMap == pNewMap){
		Athena::logger->error("进入和离开的地图是同一个,切换地图失败!");
		return false;
	}

	CPos oldPos = pBeing->getPos();
	CPos newPos ;
	pNewMap->getOnBirthPos(newPos,posID);

	if (pOldMap->remove(pBeing)){
		if (pBeing->getType() == CObj::OBJ_TYPE_PLAYER){
			pNewMap->sendMapInfo((GameUser*)pBeing);
		}
		if (pNewMap->add(pBeing,newPos)){
			Athena::logger->trace("name=%s,id=%d切换到新的(name=%s,id=%d)地图成功,当前坐标:(x:%d,y:%d,z:%d)",pBeing->getName(),pBeing->getID(),pNewMap->getName(),pNewMap->getID(),pBeing->getPos().m_x,pBeing->getPos().m_y,pBeing->getPos().m_z);
			if ((pBeing->getType() == CObj::OBJ_TYPE_PLAYER) && (pNewMap->isShowHero())){
				((GameUser *)pBeing)->m_heroMgr.summonHero();
			}
		}
		else {
			pOldMap->add(pBeing,oldPos);
			Athena::logger->trace("name=%s,id=%d切换到新的(name=%s,id=%d)地图失败,往新地图添加失败!",pBeing->getName(),pBeing->getID(),pNewMap->getName(),pNewMap->getID());
			return false;
		}
	}
	else {
		Athena::logger->trace("name=%s,id=%d切换到新的(name=%s,id=%d)地图失败,从原来地图删除不成功",pBeing->getName(),pBeing->getID(),pNewMap->getName(),pNewMap->getID());
		return false;
	}
	return true;
}

bool CMapMgr::robotChangeMap(GameUser * pUser,CMap * pNewMap,UINT16 posID)
{
	if (!pUser || !pNewMap){
		return false;
	}

	pUser->setMap(NULL);
	CPos newPos ;
	pNewMap->getOnBirthPos(newPos,posID);
	if (pNewMap->add(pUser,newPos)){
		Athena::logger->trace("机器人 name=%s,id=%d切换到新的(name=%s,id=%d)地图成功,当前坐标:(x:%d,y:%d,z:%d)",pUser->getName(),pUser->getID(),pNewMap->getName(),pNewMap->getID(),pUser->getPos().m_x,pUser->getPos().m_y,pUser->getPos().m_z);
		if (pNewMap->isShowHero()){
			pUser->m_heroMgr.summonHero();
		}
		return true;
	}
	else {
		return false;
	}
}
