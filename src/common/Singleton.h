#ifndef __H_SINGLETON_H__
#define __H_SINGLETON_H__
#include "AthenaNew.h"

template <class T>
class TSingletonStatic
{
	public:
		static T & Instance()
		{
			static T t;
			return t;
		}
};

template <class T>
class TSingletonNew
{
	public:
		static T & Instance()
		{
			if (pT == NULL){
				pT = ATHENA_NEW T;	// never delete
			}
			return *pT;
		}
		static void destroy()
		{
			delete pT;
			pT = NULL;
		}
	static T * pT;
};

template <class T>
T * TSingletonNew<T>::pT = NULL;

#define USE_SINGLETON_STATIC(C) typedef TSingletonStatic<C> _TYPE_THE_Singleton;	\
	friend class TSingletonStatic<C>;

#define USE_SINGLETON_NEW(C) typedef TSingletonNew<C> _TYPE_THE_Singleton;	\
	friend class TSingletonNew<C>;

#define THE_SINGLETON _TYPE_THE_Singleton


#endif
