#include "DropItemObj.h"
#include "AthenaNew.h"
#include "Item.h"
#include "Map.h"
#include "Map.pb.h"

UINT32 CDropItemObjMgr::m_genID = 0;

CDropItemObj::CDropItemObj() : m_pItem(NULL)
{
	m_lastTime = 60000;
	m_createTime = g_gameServer->getMSec();
}

CDropItemObj::~CDropItemObj()
{
	SAFE_DELETE(m_pItem);
}

/*void CDropItemObj::getAppear(Map::stNotifyItemAppear::stItemData & itemData)
{
	if (!m_pItem){
		return ;
	}
	
	itemData.set_itemid(getID());
	itemData.set_modid(getUniqueID());
	itemData.set_x(getPos().m_x);
	itemData.set_y(getPos().m_y);
	itemData.set_z(getPos().m_z);
	itemData.set_num(m_pItem->m_itemInfo.m_num);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("ͬ掉落了道具 id=%d,tempID=%d,x=%d,y=%d,z=%d",itemData.itemid(),itemData.modid(),itemData.x(),itemData.y(),itemData.z());
#endif
}*/

bool CDropItemObj::heartBeat(UINT64 uTime)
{
	if (m_createTime + m_lastTime < uTime){//该清除了
		this->setClearState();
	}
	return true;
}

CDropItemObjMgr::CDropItemObjMgr():ElementMgr(true,true,false,true)
{

}

CDropItemObjMgr::~CDropItemObjMgr()
{
	for (TTempIDMap_IT it = m_tempIDMap.begin(); it != m_tempIDMap.end(); ++it){
		CDropItemObj * pItemObj = (CDropItemObj *)it->second;
		SAFE_DELETE(pItemObj);
	}	
}

bool CDropItemObjMgr::addDropItemObj(CDropItemObj * pItemObj)
{
	if (!pItemObj){
		return false;
	}
	return addElement(pItemObj);
}

bool CDropItemObjMgr::removeDropItemObj(CDropItemObj *pItemObj)
{
	if (!pItemObj){
		return false;
	}
	removeElement(pItemObj);
	return true;
}

void CDropItemObjMgr::destroyDropItemObj(CDropItemObj * pItemObj)
{
	if (!pItemObj){
		return ;
	}

	if (!pItemObj->getMap()){
		return;
	}
	
	if (pItemObj->getMap()->remove(pItemObj)){
		SAFE_DELETE(pItemObj);	
	}
}

bool CDropItemObjMgr::createDropItemObj(UINT32 itemID,UINT32 num,CMap * pInsertMap,CPos & pos)
{
	if (!pInsertMap){
		return false;
	}

	CItem * pItem = CBagMgr::createItem(itemID,1);
	if (!pItem){
		return false;
	}

	CDropItemObj * pItemObj = ATHENA_NEW CDropItemObj();
	if (!pItemObj){
		return false;
	}	

	pItemObj->m_pItem = pItem;
	pItemObj->setID(itemID);
	pItemObj->setName(pItem->getName());
	pItemObj->setTempID(++m_genID);
	if (pInsertMap->add(pItemObj,pos)){
		addDropItemObj(pItemObj);
#ifdef _HDZ_DEBUG
		CCellIdx idx(pos);
		Athena::logger->trace("创建了一个掉落道具x=%d,z=%d,itemID=%d",idx.m_x,idx.m_z,itemID);
#endif
		return true;
	}
	else{
		SAFE_DELETE(pItemObj);
		return false;
	}
}

CDropItemObj * CDropItemObjMgr::getWildItemByTempID(UINT32 tempID)
{
	return (CDropItemObj *)getElementByTempID(tempID);	
}

