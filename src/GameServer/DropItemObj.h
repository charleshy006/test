#ifndef __H_DROPITEMOBJ_H__
#define __H_DROPITEMOBJ_H__
#include "Obj.h"
#include "Item.h"
#include "TypesDef.h"
#include "ElementMgr.h"
#include "Cell.h"
#include "Singleton.h"

class CDropItemObj : public CObj
{
public:
	CDropItemObj();
	~CDropItemObj();

	virtual UINT32 getUniqueID() const
	{
		return getTempID();
	}

	virtual ObjType getType() const
	{
		return CObj::OBJ_TYPE_ITEM;
	}
	//阻挡等级
	virtual UINT8 getDriveLevel()const
	{
		return BLOCK_TYPE_NONE;
	}
	//是否有阻挡
	virtual bool isBlockObj() const { return false; }
	//显示信息
	//virtual void getAppear(Map::stNotifyItemAppear::stItemData & itemData);
	//定时回调
	virtual bool heartBeat(UINT64 uTime);
public :
	CItem * m_pItem;
	UINT32 m_lastTime;		//持续时间
	UINT64 m_createTime;	//创建时间
};

class CDropItemObjMgr : public ElementMgr
{
	USE_SINGLETON_NEW(CDropItemObjMgr);
	private :
		CDropItemObjMgr();
		static UINT32 m_genID;
	public :
		~CDropItemObjMgr();

		static CDropItemObjMgr & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		
		static void destroyMe()
		{
			THE_SINGLETON::destroy();
		}
		//获得掉落的道具
		CDropItemObj * getWildItemByTempID(UINT32 tempID);
		//增加一个掉落道具
		bool addDropItemObj(CDropItemObj * pItemObj);
		//删除一个掉落道具
		bool removeDropItemObj(CDropItemObj *pItemObj);
		//销毁一个掉落道具
		void destroyDropItemObj(CDropItemObj * pItemObj);
		//创建一个掉落道具
		bool createDropItemObj(UINT32 itemID,UINT32 num,CMap * pInsertMap,CPos & pos);
};

#endif
