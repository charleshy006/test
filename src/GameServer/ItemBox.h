#ifndef __ITEMBOX_H__
#define __ITEMBOX_H__
#include "TypesDef.h"
#include <map>

class GameUser;
class ItemBoxMgr
{
public :
	//构造函数
	ItemBoxMgr(GameUser * pOwner);
	//析构函数
	~ItemBoxMgr();
	//给自己某个箱子
	void openBox(UINT32 boxID,UINT32 num = 1,bool bNotify = false,UINT32 boxItemID = 0);
	//获得某个箱子需要的容量
	UINT16 needSlotNum(UINT32 boxID);
public :
	GameUser * m_pOwner;
};
#endif
