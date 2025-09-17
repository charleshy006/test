#ifndef __H_TILE_H_
#define __H_TILE_H_
#include "TypesDef.h"
#include <set>

class CObj;
class CBeing;

class CTile
{
public :
	//构造
	CTile();
	//析构
	~CTile();
	//增加
	bool add(CObj * pObj);
	//扣除
	bool remove(CObj * pObj);
	//获得obj
	void get(std::vector<CBeing *> &  vec);
	//obj数量
	UINT16 objSize() { return  m_objs.size() ; }
private :
	std::set<CObj *> m_objs;
};

#endif
