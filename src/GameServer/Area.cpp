#include "Area.h"
#include <assert.h>
#include"Being.h"
#include "Log4cxxHelp.h"
#include "AthenaNew.h"
#include "Npc.h"
#include "Map.h"

CArea::CArea(CMap * pOwner) : m_pOwner(pOwner)
{
	for (UINT16 i = 0;i < CObj::OBJ_TYPE_MAX; ++i){
		m_objs[i].clear();
	}
	m_broadcastUserIDs.clear();
	m_broadcastObjs.clear();
	m_x = m_z = -1;
	m_effectCount = 0;
}

CArea::~CArea()
{
}

bool CArea::add(CObj * pObj)
{
	if (!pObj){
		return false;
	}

	if (pObj->getType() >= CObj::OBJ_TYPE_MAX){
		return false;
	}

	bool retval = m_objs[pObj->getType()].insert(pObj).second;
	if (retval)
	{
		GameUser * pUser = pObj->theGameUser();
		if (pUser)
		{
			doAddPlayer(pUser);
		}
	}
	return retval;
}

bool CArea::remove(CObj * pObj)
{
	if (!pObj){
		return false;
	}

	if (pObj->getType() >= CObj::OBJ_TYPE_MAX){
		return false;
	}

	if (m_objs[pObj->getType()].erase(pObj))
	{
		GameUser * pUser = pObj->theGameUser();
		if (pUser)
		{
			doRemovePlayer(pUser);
		}
		return true;
	}
	return false;
}

void CArea::update(UINT64 curTime)
{
	TObjSet copyObjs =  m_objs[CObj::OBJ_TYPE_MONSTER];
	
	TObjSet::iterator it = copyObjs.begin();
	for (;it != copyObjs.end();++it){
		CNpc * pNpc = (CNpc * )(*it);
		pNpc->doAIAction(curTime);
	}
}

void CArea::doAddPlayer(GameUser * pUser)
{
	m_broadcastUserIDs.push_back(pUser->getID());
	m_broadcastObjs.push_back(pUser);
	int idx = m_broadcastUserIDs.size() -1;
	pUser->m_iBroadcastIdx = idx;
}

void CArea::doRemovePlayer(GameUser * pUser)
{
	int idx = pUser->m_iBroadcastIdx;
	int iEnd = m_broadcastUserIDs.size()-1;
	if (idx != iEnd)
	{
		m_broadcastUserIDs[idx] = m_broadcastUserIDs[iEnd];
		m_broadcastObjs[idx] = m_broadcastObjs[iEnd];
		m_broadcastObjs[idx]->m_iBroadcastIdx = idx;
	}

	m_broadcastUserIDs.pop_back();
	m_broadcastObjs.pop_back();
}

bool CArea::execEveryObj(MapObjCallBack& callBack,CObj::ObjType type)
{
	if (type >= CObj::OBJ_TYPE_MAX){
		return false;
	}
	for(std::set<CObj*>::iterator itr = m_objs[type].begin();itr!=m_objs[type].end();itr++)
	{
		//此处如果改变CObj区域所属，可能导致itr实效
		if((*itr)->getType() == type || type== CObj::OBJ_TYPE_MAX)
			if(!callBack(*itr))
				return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
CAreaV::CAreaV()
{
	this->reserve(9);
}

