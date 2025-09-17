#include "RecastEquipCfg.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "TblDataMgr.h"
#include <tinyxml/tinyxml.h>


struct stCombineParseCB : public TblDatabaseCB<ItemCombineBase>
{
	stCombineParseCB() {}
	~stCombineParseCB() {}

	virtual bool doit(ItemCombineBase * pBase)
	{
		if (!pBase){
			return true;
		}
		RecastEquipCfg::getSingleton().AddRecastCfg(pBase->m_id,pBase->m_pieceNum, pBase->m_combineItemID);
		return true;	
	}
};

RecastEquipCfg::RecastEquipCfg()
{
	
}

RecastEquipCfg::~RecastEquipCfg()
{

}

bool RecastEquipCfg::loadCfg()
{
	stCombineParseCB cb;
	g_combineBaseMgr.execEveryOne(cb); 
	return true;
}

void RecastEquipCfg::AddRecastCfg(UINT32 pieceID, UINT32 pieceCnt, UINT32 itemID)
{
	stItem* st = new(stItem);
	st->m_itemID = pieceID;
	st->m_num = pieceCnt;

	m_Cfg[itemID] = st;
}

RecastEquipCfg::stItem * RecastEquipCfg::getRecastItem(UINT32 equipID) 
{
	std::map<UINT32,stItem*>::iterator it = m_Cfg.find(equipID);
	if (it != m_Cfg.end()){
		return it->second;
	}
	return NULL;
}
