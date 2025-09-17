#ifndef __TYPESDEF_H__
#define __TYPESDEF_H__
#include <string.h>
#include <ext/hash_map>

typedef unsigned char		byte;
typedef char 				INT8;
typedef unsigned char       UINT8;
typedef short 				INT16;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;
typedef int 				INT32;
typedef unsigned long long	UINT64;
typedef  long long 			INT64;

const unsigned int MAX_DATASIZE  = (64 * 1024 - 1 - sizeof(UINT32)); 
#define MSGBUF_MAX          4096

template <typename keyT>
struct my_key_equal : public std::binary_function<keyT, keyT, bool>
{
	inline bool operator () (keyT s1, keyT s2) const;
};

template<>
inline bool my_key_equal<std::string>::operator () (std::string s1, std::string s2) const
{
	return s1 == s2;
}

template<>
inline bool my_key_equal<UINT32>::operator () (UINT32 s1,UINT32 s2) const
{
	return s1 == s2;
}

template<>
inline bool my_key_equal<unsigned long>::operator () (unsigned long s1, unsigned long s2) const
{
	return s1 == s2;
}

template<>
inline bool my_key_equal<unsigned long long>::operator () (unsigned long long s1, unsigned long long s2) const
{
	return s1 == s2;
}

template <class key> 
struct my_hash : public std::unary_function<const key, size_t> 
{
	size_t operator()(key v) const
	{
		__gnu_cxx::hash<key> H;
		return H(v);
	}
};

template<>
inline size_t my_hash<std::string>::operator()(std::string v) const
{
	 __gnu_cxx::hash<const char *> H; 
	 return H(v.c_str());
}

template<>
inline size_t my_hash<unsigned long long>::operator()(unsigned long long v) const
{
	return v;
}

class KVPair;
namespace Athena
{
	extern __thread UINT32 seedp ;
	extern KVPair global;   //存放全局变量的容器
};

template <class _T1>
inline void allocInStack(_T1* _p)
{
	new (static_cast<void*>(_p)) _T1();
}

#define _getMessage(msg, msglen, pat)	\
	do {	\
		va_list ap;	\
		bzero(msg, msglen);	\
		va_start(ap, pat);	\
		vsnprintf(msg, msglen - 1, pat, ap);	\
		va_end(ap);	\
	} while(0);

#define SAFE_DELETE(p)			if ((p)) { delete (p); (p) = NULL; }
#define SAFE_DELETE_ARRAY(p)	if ((p)) { delete [] (p); (p) = NULL; }

enum eEncDecMethod
{
	ENCDEC_NONE,
	ENCDEC_ECBDES
};

#endif
