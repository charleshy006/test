#include "Obj.h"
#include "Being.h"
#include "Cell.h"
#include "Map.h"
#include "Hero.h"

CObj::CObj()
{
	// ???所有成员变量的初始化，包括函数指针
	m_pCurMap = NULL;
	m_needClear = false;
}

CObj::~CObj()
{

}

CMap * CObj::getMap()
{
	return m_pCurMap;
}

void CObj::setMap(CMap * pMap)
{
	m_pCurMap = pMap;
}

CPos & CObj::getPos()
{
	return m_curPos;
}

void CObj::setPos(CPos& pos)
{
	m_curPos = pos;
}

CPos & CObj::getLastPos()
{
	return m_lastPos;
}

void CObj::saveLastPos()
{
	m_lastPos = m_curPos;
}

GameUser * CObj::theGameUser()
{
	if(getType() == OBJ_TYPE_PLAYER ){//如果是玩家，则返回玩家指针，否则返回空
		return (GameUser *)this;
	}
	return NULL;
}

CNpc  * CObj::theNpc()
{
	if (getType() == OBJ_TYPE_MONSTER){
		return (CNpc *)this;
	}
	return NULL;
}

CHero  * CObj::theHero()
{
	if (getType() == OBJ_TYPE_HERO){
		return (CHero *)this;
	}
	return NULL;
}

CDropItemObj * CObj::theItem()
{
	if (getType() == OBJ_TYPE_ITEM){
		return (CDropItemObj *)this;
	} 
	return NULL;
}

#ifdef _NEW_SYN_METHOD
bool  CObj::enterViewSight(UINT8 type,UINT32 moID)
{
	if (type >= OBJ_TYPE_MAX){
		return false;
	}
	
	std::set<UINT32>::iterator it = m_viewSight[type].find(moID);
	if (it != m_viewSight[type].end()){
		return false;
	}
	
	if (m_viewSight[type].size() >= MAX_SAW_OBJ){
		return false;
	}
	return m_viewSight[type].insert(moID).second;
}

bool  CObj::leaveViewSight(UINT8 type,UINT32 moID)
{
	if (type >= OBJ_TYPE_MAX){
		return false;
	}
	
	std::set<UINT32>::iterator it = m_viewSight[type].find(moID);
	if (it == m_viewSight[type].end()){
		return false;
	}
	
	m_viewSight[type].erase(it);
	return true;        
}

bool  CObj::isInViewSight(UINT8 type,UINT32 moID)
{
	if (type >= OBJ_TYPE_MAX){
		return false;
	}
	
	if (m_viewSight[type].find(moID) != m_viewSight[type].end()){
		return true;
	}
	else {
		return false;
	}
	return true;
}
#endif
