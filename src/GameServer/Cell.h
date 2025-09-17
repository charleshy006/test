#ifndef __CELL_H__
#define __CELL_H__

#include "Obj.h"

class CCell
{
public:
	const static int POSCEL_Z = 100;
	const static int POSCEL_X = 100;

	const static int THE_Z = POSCEL_Z;
	const static int THE_X = POSCEL_X;

};

//地图格子信息
enum MapBlockType
{
	BLOCK_TYPE_NONE= 0x00,	//无阻挡
	BLOCK_TYPE_NPC = 0x01,	//npc阻挡
	BLOCK_TYPE_LVMAX = 0x08,//最高级别阻挡，任何物体都无法通过。地面阻挡
	//如果有空中飞行，则需要更高级的通行方式
};

#define BLOCK_TYPE_PLAYER BLOCK_TYPE_LVMAX
#endif

