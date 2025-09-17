#ifndef __H_TBL_READER_
#define __H_TBL_READER_

#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string>
#include<map>
#include <ext/hash_map>
#include "AthenaNew.h"

using namespace std;
#define TBL_READ_BUF_SIZE 1024*24


//健值提取器：提取自定义结构的键值,请尽量继承该类，并实现接口
template<typename Key,typename TBLSrcStruct>
struct KeyOfTBLSrcStruct
{
	KeyOfTBLSrcStruct(){}
	virtual ~KeyOfTBLSrcStruct(){}
    virtual Key operator()(const TBLSrcStruct& s)=0;
};

template<class TBLSrcStruct>
struct TBLDstStructBase
{
	TBLDstStructBase() {}
	virtual ~TBLDstStructBase(){}
	virtual bool fill(const TBLSrcStruct& ss) = 0;
};

//TBL读取器：读取TBL文件到自定义结构中,以Key为健值，建立索引存储到结果集合
template<class TBLSrcStruct,//源结构
	class TBLDstStruct,		//目标集合
	class Key,				//键值类型
	class KeyOfValue,		//源结键值提取器
	class ResultSet=std::map<Key,TBLDstStruct *> >
class TBLKeyLoader
{
	public:	
	
	static int loadTBL(const char* fileName,ResultSet& RSet)
	{

		FILE* fp=NULL;
		fp = fopen(fileName,"rb");
		if(!fp)
		{
			return -1;
		}

		char buf[TBL_READ_BUF_SIZE];
		int maxNum = TBL_READ_BUF_SIZE / sizeof(TBLSrcStruct);//最大读取数量

		if(!maxNum)//缓冲区过小，无法一次保证一次读取至少一个完整的条目
		{
			return -3;
		}

		//读取TBL头部:数据
		int bytesCount =0;
		int len =fread(&bytesCount,sizeof(int),1,fp);
		if(len<=0)
		{
			return -2;
		}

		//结构验证,数据大小是否为收取结构的整数倍，如果不是则有异常
		/*if(bytesCount%sizeof(TBLSrcStruct))
		{
			return -4; 
		}*/

		unsigned int size = 0 ;

		//读取数据部分
		int num = 0;
		do
		{	
			num = fread(buf,sizeof(TBLSrcStruct),maxNum,fp);
			const TBLSrcStruct* pDs = (TBLSrcStruct*)buf;

			for(int i=0;i<num;i++)
			{
				Key key = KeyOfValue()(pDs[i]);//取得健值
				typename ResultSet::iterator it = RSet.find(key);
				if (it != RSet.end()){
					RSet[key]->fill(pDs[i]);
				}
				else {
					TBLDstStruct * dstStruct = ATHENA_NEW TBLDstStruct();
					dstStruct->fill(pDs[i]);
					RSet[key] = dstStruct;
				}
				size++;
			}

		}while(num>0);

		fclose(fp);

		/*if(size!=bytesCount/sizeof(TBLSrcStruct))
			return -5;*/
		if ((int)size != bytesCount){
			return -5;
		}

		return size;
	}
};

#define TBL_LOADER_MAP(TBLSrcStruct,TBLDstStruct,KeyType,KeyOfValue) \
	TBLKeyLoader<TBLSrcStruct,TBLDstStruct,KeyType,KeyOfValue,std::map<KeyType,TBLDstStruct *> >
#define TBL_LOADER_HASHMAP(TBLSrcStruct,TBLDstStruct,KeyType,KeyOfValue) \
	TBLKeyLoader<TBLSrcStruct,TBLDstStruct,KeyType,KeyOfValue,__gnu_cxx::hash_map<KeyType,TBLDstStruct *> >
#endif

