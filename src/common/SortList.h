#ifndef __H_SORT_LIST_H__
#define __H_SORT_LIST_H__
#include "TypesDef.h"
#include "Log4cxxHelp.h"
#include <set>
#include <map>

template <class T,int MAX_SORT_NUM = 1000>
class CSortList
{
public :
	//构造
	CSortList(){}
	//析构
	~CSortList(){}
	//获得排名前几的信息
	void getTopN(std::vector<T> & vec,UINT16 n);
	//获得第n1到第n2的排名
	void getFromN12N2(std::vector<T> & vec,UINT16 n1,UINT16 n2);
	void getFromN12N2(std::map<UINT32, T> & map,UINT16 n1,UINT16 n2);
	//插入一个排名信息
	void insertSortInfo(T & sortInfo);
	//更新排名信息
	void updateSortInfo(T & sortInfo);
	//删除一个排名信息
	void removeSortInfo(UINT32 key);
	//获得我的排名
	UINT16 getMySortRank(UINT32 key);
	//是否已经在排名表里
	bool isInSortList(UINT32 key);
	//获得排名信息
	T getSortInfo(UINT32 key);
	//获得当前列表有多少记录
	UINT16 getListNum(){ return m_sortCnt.size(); }
	//获得最后一名的key
	UINT32 getLastOneKey();
	//清空所有数据
	void clear()
	{
		m_sortCnt.clear();
		m_findCnt.clear();
	}
protected :
	typedef std::multiset<T>	 SortSet;
	typedef typename SortSet::iterator SortSet_IT;
	typedef typename SortSet::reverse_iterator SortSet_RIT;
	typedef std::map<UINT32,T>	 FindMap;
	typedef typename FindMap::iterator    FindMap_IT;
	SortSet m_sortCnt;		//排名容器
	FindMap m_findCnt;		//查找容器
};

template <class T,int MAX_SORT_NUM >
void CSortList<T,MAX_SORT_NUM>::getTopN(std::vector<T> & vec,UINT16 n)
{
	UINT16 i = 0;
	SortSet_IT it = m_sortCnt.begin();
	for (;it != m_sortCnt.end(); ++it){
		vec.push_back(*it);
		i += 1;
		if (i >= n){
			break;
		}
	}
}

template <class T,int MAX_SORT_NUM >
void CSortList<T,MAX_SORT_NUM>::getFromN12N2(std::vector<T> & vec,UINT16 n1,UINT16 n2)
{
	if (n2 < n1){
		return ;
	}
	
	SortSet_IT it = m_sortCnt.begin();
	for (UINT16 i = 1;(i < n1) && (it != m_sortCnt.end()); ++it,++i);

	if (it == m_sortCnt.end()){
		return ;
	}
	UINT16 num = n2 - n1 + 1;
	for (UINT16 i = 0;(i < num) && (it != m_sortCnt.end()) ; ++i,++it){
		vec.push_back(*it);	
	}	
}

template <class T,int MAX_SORT_NUM > 
void CSortList<T,MAX_SORT_NUM>::getFromN12N2(std::map<UINT32, T> & map,UINT16 n1,UINT16 n2)
{
	if (n2 < n1){
		 return ;
	}

	SortSet_IT it = m_sortCnt.begin();
	for (UINT16 i = 1;(i < n1) && (it != m_sortCnt.end()); ++it,++i); 
	if (it == m_sortCnt.end()){
		 return ;
	}
	UINT16 num = n2 - n1 + 1;  
	for (UINT16 i = 0;(i < num) && (it != m_sortCnt.end()) ; ++i,++it){
		map.insert(std::make_pair((*it).getKey(), *it));
	}
}

template <class T,int MAX_SORT_NUM>
void CSortList<T,MAX_SORT_NUM>::insertSortInfo(T & sortInfo)
{
	if (m_sortCnt.size() >= MAX_SORT_NUM){
		SortSet_RIT it = m_sortCnt.rbegin();
		if (it != m_sortCnt.rend()){
			if ((*it).getCmpValue() >= sortInfo.getCmpValue()){//比最后一名还小
				return ;
			}
		}
	}

	m_sortCnt.insert(sortInfo);
	m_findCnt[sortInfo.getKey()] = sortInfo;
	while (m_sortCnt.size() > MAX_SORT_NUM)	{
		SortSet_RIT it = m_sortCnt.rbegin();
		if (it != m_sortCnt.rend()){
			T tmp = *it;
			m_findCnt.erase(tmp.getKey());
			/*SortSet_IT curIt = m_sortCnt.find(tmp);
			if (curIt != m_sortCnt.end()){
				m_sortCnt.erase(curIt);
			}*/
			 //m_sortCnt.erase(tmp);
			++it;
			SortSet_IT curIt = it.base();
			if (curIt != m_sortCnt.end()) {
				m_sortCnt.erase(curIt);
				//Athena::logger->trace("删除掉key1=%d,key2=%d",tmp.getKey(),curIt->getKey());
			}
			//Athena::logger->trace("开始删除列表中后面的对象!");
		}
	}
	
}

template <class T,int MAX_SORT_NUM>
void CSortList<T,MAX_SORT_NUM>::updateSortInfo(T & sortInfo)
{
	SortSet_IT it = m_sortCnt.begin();
	for (;it != m_sortCnt.end();++it){
		const T & one = *it;
		if (sortInfo.getKey() == one.getKey()){
			m_findCnt.erase(one.getKey());
			m_sortCnt.erase(it);
			break;
		}
	}
	insertSortInfo(sortInfo);
}

template <class T,int MAX_SORT_NUM>
void CSortList<T,MAX_SORT_NUM>::removeSortInfo(UINT32 key)
{
	m_findCnt.erase(key);
	SortSet_IT it = m_sortCnt.begin();
	for (;it != m_sortCnt.end();++it){
		const T & one = *it;
		if (key == one.getKey()){
			m_sortCnt.erase(it);
			return ;
		}
	}
}

template <class T,int MAX_SORT_NUM>
UINT16 CSortList<T,MAX_SORT_NUM>::getMySortRank(UINT32 key)
{
	if (!isInSortList(key)){//不在排名表里
		return (UINT16)-1;
	}

	UINT16 ret = 0;
	SortSet_IT it = m_sortCnt.begin();
	for (;it != m_sortCnt.end();++it){
		const T & one = *it;
		++ret;
		if (key == one.getKey()){
			return ret;
		}
	}
	return MAX_SORT_NUM + 1;		
}

template <class T,int MAX_SORT_NUM>
bool CSortList<T,MAX_SORT_NUM>::isInSortList(UINT32 key)
{
	FindMap_IT it = m_findCnt.find(key);
	if (it != m_findCnt.end()){
		return true;
	}
	return false;
}

template <class T,int MAX_SORT_NUM>
T CSortList<T,MAX_SORT_NUM>::getSortInfo(UINT32 key)
{
	FindMap_IT it = m_findCnt.find(key);
	if (it != m_findCnt.end()){
		return it->second;
	}
	
	return T();
}

template <class T,int MAX_SORT_NUM>
UINT32 CSortList<T,MAX_SORT_NUM>::getLastOneKey()
{
	SortSet_RIT it = m_sortCnt.rbegin();
	if (it != m_sortCnt.rend()){
		return it->getKey();
	}
	return 0;	
}
#endif
