#ifndef __H_UNIQUE_ID_MGR_
#define __H_UNIQUE_ID_MGR_

#include <set>
//ID必须重载小于运算符,赋值运算符
//
//recycle表示是否进行ID回收
template<typename ID,bool recycle=true>
struct UniqueIDMgr
{
	//设置唯一ID区间，采用前闭后开区间
	UniqueIDMgr(ID min,ID max)
	{
		m_minID 	= min;
		m_maxID 	= max;
		m_curID 	= m_minID;//
	}

	//判断某个ID是否为有效ID
	bool isValidID(ID id )
	{
		return id >= m_minID && id < m_maxID;
	}

	//产生一个ID
	ID genID()
	{
		if(recycle)//如果进行ID回收，则先行使用回收队列中得ID
		{
			if(!m_recycles.empty())
			{
				typename std::set<ID>::iterator itr = m_recycles.begin();
				ID newID = *itr;
				m_recycles.erase(itr);
				return newID;
			}
		}
		if( m_curID < m_maxID )
			return m_curID++;
		return m_maxID;
	}

	//回退一个ID
	void putID(ID id)
	{
		if(recycle && isValidID(id))	
			m_recycles.insert(id);
	}

	std::set<ID> m_recycles;
	ID m_curID;//指向下一个可以分配的ID
	ID m_minID;//最小ID
	ID m_maxID;//最大ID
};

#endif
