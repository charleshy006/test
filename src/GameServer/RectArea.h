#ifndef __H_RECTAREA_H__
#define __H_RECTAREA_H__
#include "TypesDef.h"
#include "Cell.h"
#include "Obj.h"

struct CRectArea
{
	CPos m_pos;				//左上角坐标
	int m_width;			//宽 
	int m_height;			//高

	CRectArea()
	{
		m_width = 0;
		m_height = 0;
	}

	CRectArea(const CPos& pos1,const CPos& pos2)//两点定区域
	{
		m_pos = pos1 ;
		m_width = pos2.m_x - pos1.m_x ;
		m_height = pos2.m_y - pos1.m_y;
	}

	CRectArea(const CPos & pos,int w,int h)//两点定区域
	{	
		m_pos = pos;
		m_width = w;
		m_height = h;
	}

	CRectArea(const CRectArea& rc)
	{
		operator=(rc);
	}

	CRectArea& operator=(const CRectArea& rc)
	{
		m_pos = rc.m_pos;
		m_width = rc.m_width;
		m_height = rc.m_height;
		return *this;
	}

	//不检测坐标有效性
	bool isInArea(CPos & pos) const
	{
		return pos.m_x >= m_pos.m_x && pos.m_y >= m_pos.m_y
			&& (pos.m_x <= m_pos.m_x + m_width) && (pos.m_y <= m_pos.m_y + m_height);
	}
	
	//坐标在哪个范围
	bool isInArea(CCellIdx & idx) const 
	{
		return true;
		//return (idx.m_x * CCellIdx.THE_X >= pos.m_x)	&& (idx.m_y * CCellIdx.THE_Y >= m_pos.m_z);
	}
};
#endif
