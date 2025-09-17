#include "NpcMgr.h"
#include "Npc.h"
#include "string.h"
#include "AthenaNew.h"
#include "AIState.h"

CNpcMgr::CNpcMgr():ElementMgr(true,true,false,true),m_tempIDAllocater(1,(UINT32)-1)
{
}

CNpcMgr::~CNpcMgr()
{
	for (TTempIDMap_IT it = m_tempIDMap.begin(); it != m_tempIDMap.end(); ++it){
		CNpc * pNpc = (CNpc *)it->second;
		SAFE_DELETE(pNpc);
	}		
}

bool CNpcMgr::add(CNpc* pNpc)//增加一个NPC到管理器中
{
	if (!pNpc){
		return false;
	}
	return addElement(pNpc);
}

bool CNpcMgr::remove(CNpc * pNpc)//删除NPC
{
	if (!pNpc){
		return false;
	}
	removeElement(pNpc);
	return true;
}

CNpc* CNpcMgr::getNpcByTempID(UINT32 npcID)
{
	return (CNpc *)getElementByTempID(npcID);
}

bool CNpcMgr::genTempID(UINT32 & tempID)
{
	tempID = m_tempIDAllocater.genID();	
	return m_tempIDAllocater.isValidID(tempID);
}

void  CNpcMgr::putBackID(UINT32 tempID)
{
	m_tempIDAllocater.putID(tempID);	
}
