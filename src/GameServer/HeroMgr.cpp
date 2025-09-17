#include "HeroMgr.h"
#include "string.h"
#include "AthenaNew.h"
#include "GameUserManager.h"
#include "CopyMap.h"

CHeroMgr::CHeroMgr():ElementMgr(true,true,false,true),m_tempIDAllocater(1,(UINT32)-1)
{
	
}

CHeroMgr::~CHeroMgr()
{
	for (TTempIDMap_IT it = m_tempIDMap.begin(); it != m_tempIDMap.end(); ++it){
		CHero * pNpc = (CHero *)it->second;
		SAFE_DELETE(pNpc);
	}		
}

bool CHeroMgr::add(CHero * pHero)
{
	if (!pHero){
		return false;
	}
	return addElement(pHero);
}

bool CHeroMgr::remove(CHero * pHero)
{
	if (!pHero){
		return false;
	}
	removeElement(pHero);
	return true;
}

CHero * CHeroMgr::getHeroByTempID(UINT32 npcID)
{
	return (CHero *)getElementByTempID(npcID);
}

bool CHeroMgr::genTempID(UINT32 & tempID)
{
	tempID = m_tempIDAllocater.genID();	
	return m_tempIDAllocater.isValidID(tempID);
}

void  CHeroMgr::putBackID(UINT32 tempID)
{
	m_tempIDAllocater.putID(tempID);	
}

void  CHeroMgr::clearHeroByOwerID(UINT32 charID)
{
	GameUser * pUser = GameUserManager::getSingleton().getUser(charID);

	std::map<UINT32,UINT32> heroIDMap;//添加出战的宠物ID，以便id重复的被多次遍历（如：斯巴达使用召唤技能会出现多个 811607 ID）
	for (TTempIDMap_IT it = m_tempIDMap.begin(); it != m_tempIDMap.end(); ++it){
		CHero * pHero = (CHero *)it->second;
		std::map<UINT32,UINT32>::iterator mapIt = heroIDMap.find(pHero->getID());
		if ((charID == pHero->getOwnerID()) && (mapIt == heroIDMap.end())){
			pHero->setClearState();
			UINT32 hp = pHero->getHP();
			if (pHero && pHero->getMap()->isCopyMap()){
				heroIDMap[pHero->getID()] = pHero->getID();
				hp = ((CCopyMap *)pHero->getMap())->getOrgHP(hp);
			}
			if (pUser){
				pUser->m_heroMgr.updateBattleHeroHP(pHero->getID(),hp);
			}
		}
	}
	heroIDMap.clear();
}
