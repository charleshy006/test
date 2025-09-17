#include "Tile.h"
#include "Obj.h"
#include "Being.h"

CTile::CTile()
{}

CTile::~CTile()
{

}

bool CTile::add(CObj * pObj)
{
	if (!pObj) {
		return false;
	}	
	return m_objs.insert(pObj).second; 
}

bool CTile::remove(CObj * pObj)
{
	if (!pObj) {
		return false;
	}
	m_objs.erase(pObj);
	return true;
}

void CTile::get(std::vector<CBeing *> &  vec)
{
	std::set<CObj *>::iterator it = m_objs.begin();	
	for (;it != m_objs.end() ; ++it){
		vec.push_back((CBeing *)(*it));	
	}
}
