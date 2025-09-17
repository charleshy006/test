#include "RobotMgr.h"
#include "TblDataMgr.h"
#include "GameUser.h"

struct  stLoadRobotBase : public TblDatabaseCB<stRobotBase>
{
	stLoadRobotBase()
	{
		
	}

	~stLoadRobotBase()
	{
	
	}

	virtual bool doit(stRobotBase * pBase)
	{
		if (!pBase){
			return true;
		}

		stRobotInfo * pRobot = ATHENA_NEW stRobotInfo();
		if (!pRobot){
			return true;
		}
		pRobot->m_id = pBase->m_id;
		pRobot->m_lv = pBase->m_lv;
		pRobot->m_groupID = pBase->m_groupID;
		pRobot->m_levelID = pBase->m_levelID;

		GameUser * pUser = ATHENA_NEW GameUser("",0,"");

		if (!pUser){
			return true;
		}
		pRobot->m_pUser = pUser;
		pUser->m_bRobot = true;
		pUser->m_roleInfo.m_charID = pBase->m_id;
		pUser->m_roleInfo.m_job = pBase->m_job;
		pUser->m_roleInfo.m_sex = pBase->m_sex;
		pUser->m_roleInfo.m_level = pBase->m_lv;		
		pUser->setName(pBase->m_robotName);
		pUser->setID(pBase->m_id);
		pUser->m_skillMgr.checkSkillUnlock();
		pUser->createNormalSkill();

		for (UINT16 i = 0;i < pBase->m_equips.size(); ++i){
			std::pair<UINT32,UINT16> & p =  pBase->m_equips[i];
			UINT32 itemID = CItem::realItemIDByJobAndSex(p.first,pBase->m_job,pBase->m_sex);
			CItem * pItem = CBagMgr::createItem(itemID,1,p.second);
			if (pItem){
				pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_EQUIP;
				pItem->m_itemInfo.m_pos.m_pos = CEquipBag::getSlotIndexByItemType(pItem->m_pItemBase->m_type);
				if (!pUser->m_bagMgr.m_equipBag.addItem(pItem)){
					SAFE_DELETE(pItem);
				}
			}
		}
	
		for (UINT16 i = 0; i < pBase->m_pets.size(); ++i){
			std::pair<UINT32,UINT16> & p = pBase->m_pets[i];
			HeroInfo * pHeroInfo = pUser->m_heroMgr.createHero(p.first,p.second);
			if (pHeroInfo){
				if (0 == i){
					pHeroInfo->m_dbData.m_evolutionLv = pBase->m_pet1EvelotionLv; 
				}
				else {
					pHeroInfo->m_dbData.m_evolutionLv = pBase->m_pet2EvelotionLv;
				}

				if (!pUser->m_heroMgr.addHeroInfo(pHeroInfo)){
					SAFE_DELETE(pHeroInfo);
					continue;
				}
				UINT8 battlePos = HeroInfoMgr::getBattlePosByGroup(0,i);
				pUser->m_heroMgr.addBattleHero(p.first,battlePos);
			}
		}
		
		CRobotMgr::getSingleton().initFuwen(pUser,pBase->m_headFuWens,HEAD_CORE_FUWEN_INDEX);
		CRobotMgr::getSingleton().initFuwen(pUser,pBase->m_bodyFuWens,BODY_CORE_FUWEN_INDEX);
		CRobotMgr::getSingleton().initFuwen(pUser,pBase->m_weaponFuWens,WEAPON_CORE_FUWEN_INDEX);
		CRobotMgr::getSingleton().initFuwen(pUser,pBase->m_necklaceFuWens,FACE_CORE_FUWEN_INDEX);
		CRobotMgr::getSingleton().initFuwen(pUser,pBase->m_cloakFuWens,CLOAK_CORE_FUWEN_INDEX);
		CRobotMgr::getSingleton().initFuwen(pUser,pBase->m_shoeFuWens,SHOE_CORE_FUWEN_INDEX);
		CRobotMgr::getSingleton().initFuwen(pUser,pBase->m_ringFuWens,RING_CORE_FUWEN_INDEX);

		pUser->setupRoleState(true);
		CRobotMgr::getSingleton().addRobot(pRobot);
		return true;
	}
};

void CRobotMgr::initFuwen(GameUser *pUser,std::vector<std::pair<UINT32,UINT16> > &fuWenVec,UINT16 offset)
{
	if (!pUser){
		return;
	}

	for (UINT16 i = 0;i < fuWenVec.size();++i){
		std::pair<UINT32,UINT16> & p =  fuWenVec[i];
		CItem * pItem = CBagMgr::createItem(p.first,1,p.second);
		if (pItem){
			pItem->m_itemInfo.m_pos.m_bagType = Role::BAG_TYPE_EQUIP;
			pItem->m_itemInfo.m_pos.m_pos = CEquipBag::getFuwenSlotIndex(pItem->m_pItemBase->m_type,offset);
			if (!pUser->m_bagMgr.m_equipBag.addItem(pItem)){
				Athena::logger->error("机器人穿符文失败;itemID:%d",p.first);
				SAFE_DELETE(pItem);
			}
		}
	}
}


bool CRobotMgr::loadCfg()
{
	stLoadRobotBase cb;
	g_robotBaseMgr.execEveryOne(cb);

#ifdef _HDZ_DEBUG
	std::map<UINT32,std::map<UINT32,std::vector<UINT32> > >::iterator it = m_groupID2RobotIDs.begin();

	for (;it != m_groupID2RobotIDs.end();++it){
		std::map<UINT32,std::vector<UINT32> >::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			Athena::logger->trace("关卡:%d,group:%d,拥有的机器人数量:%d",it->first,subIt->first,subIt->second.size());
		}
	}
#endif
	return true;	
}

void CRobotMgr::addRobot(stRobotInfo * pRobot)
{
	if (!pRobot){
		return ;
	}
	m_robotCfgs[pRobot->m_id] = pRobot;
	if (pRobot->m_levelID > 0){
		m_groupID2RobotIDs[pRobot->m_levelID][pRobot->m_groupID].push_back(pRobot->m_id);
	}
#ifdef _HDZ_DEBUG
	//Athena::logger->trace("增加了一个机器人id=%d,Pointer=%p",robot.m_id,robot.m_pUser);
#endif 
}

stRobotInfo * CRobotMgr::getRobot(UINT32 robotID)
{
	std::map<UINT32,stRobotInfo *>::iterator it =  m_robotCfgs.find(robotID);
	if (it != m_robotCfgs.end()){
		return it->second;
	}
	return NULL;
}

stRobotInfo * CRobotMgr::getRobotByLevelID(UINT32 levelID,UINT16 num)
{
	UINT32 robotID = 0;
	
	std::vector<UINT32> * pVec = NULL;
	if (num <= 1){
		pVec = &m_groupID2RobotIDs[levelID][1];   	
	} 
	else {
		pVec = &m_groupID2RobotIDs[levelID][2];
	}

	if (!pVec || (pVec->size() <= 0)){
		return NULL;
	}
	
	UINT16 index = g_randBetween(0,pVec->size() - 1);
	robotID = (*pVec)[index];

	if (!robotID){
		return NULL;
	}
	return getRobot(robotID);	
}

stRobotInfo * CRobotMgr::getOne2OneRobot(UINT32 levelID,UINT16 groupID)
{
	UINT32 robotID = 0;
	
	std::vector<UINT32> & vec = m_groupID2RobotIDs[levelID][groupID];

	if (vec.size() <= 0){
		return NULL;
	}
	
	UINT16 index = g_randBetween(0,vec.size() - 1);
	robotID = vec[index];

	if (!robotID){
		return NULL;
	}
	return getRobot(robotID);	
}

void CRobotMgr::useRobot(UINT32 charID,stRobotInfo * pRobot)
{
	if (!pRobot || !charID){
		return ;
	}

	m_robotCfgs.erase(charID);
	m_useRobots.insert(std::make_pair(charID,pRobot));

	std::vector<UINT32> & robotVec = m_groupID2RobotIDs[pRobot->m_levelID][pRobot->m_groupID];
	std::vector<UINT32>::iterator it = robotVec.begin();
	for (;it != robotVec.end();++it){
		if (charID == *(it)){
			robotVec.erase(it);
			break;
		}		
	}
}

GameUser * CRobotMgr::getRobotByID(UINT32 charID)
{
	std::map<UINT32,stRobotInfo *>::iterator it =  m_useRobots.find(charID);	

	if (it != m_useRobots.end()){
		return it->second->m_pUser;
	}
	return NULL;
}

void CRobotMgr::returnRobot(UINT32 levelID,UINT32 charID)
{
	std::map<UINT32,stRobotInfo *>::iterator it =  m_useRobots.find(charID);	
	if (it == m_useRobots.end()){
		return ;
	}
	m_groupID2RobotIDs[levelID][it->second->m_groupID].push_back(charID);
	m_robotCfgs.insert(std::make_pair(charID,it->second));
	m_useRobots.erase(it);
}

void CRobotMgr::printRobotInfo()
{
	 std::map<UINT32,std::map<UINT32,std::vector<UINT32> > >::iterator it = m_groupID2RobotIDs.begin();

	 for (;it != m_groupID2RobotIDs.end();++it){
		Athena::logger->trace("关卡:%d的机器人使用情况:",it->first);

		std::map<UINT32,std::vector<UINT32> >::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			Athena::logger->trace("group:%d,人数 :%d",subIt->first,subIt->second.size());	
		}
	 }
}
