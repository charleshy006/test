#ifndef __H_POS_H__
#define __H_POS_H__
#include "TypesDef.h"

typedef UINT16  T_DIR;

class CPos
{
public :
	CPos()
	{
		m_x = -1;
		m_y = -1;
		m_z = -1;
		m_dir = 0;
	}

	CPos(INT32 x,INT32 y,INT32 z) : m_x(x),m_y(y),m_z(z)
	{
		m_dir = 0;
	}

	bool isValid() const 
	{
		return (m_x != -1)  && (m_z != -1);	
	}

	bool operator ==(const CPos & pos) const
	{
		return ((pos.m_x == m_x) && (pos.m_y == m_y) && (pos.m_dir == m_dir) && (m_z == pos.m_z));
	}

	bool operator !=(const CPos & pos) const
	{
		return !((pos.m_x == m_x) && (pos.m_y == m_y) && (pos.m_dir == m_dir) && (m_z == pos.m_z));
	}
	
	 bool operator < (const CPos & pos) const
	{
		return (m_x < pos.m_x) && (m_y < pos.m_y) && (m_z < pos.m_z);
	}
public :
	INT32 m_x;
	INT32 m_y;
	INT32 m_z;
	T_DIR m_dir;
};

#endif
