#ifndef __H_OPERATION_MGR_H__
#define __H_OPERATION_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include <map>
#include <string>
#include <vector>

#pragma pack(1)
struct stGameTblNameInfo
{
	char m_name[100];

	stGameTblNameInfo()
	{
		bzero(m_name,sizeof(m_name));
	}
};

struct stLoginIPTypeDB
{
	UINT16 m_type;					//ip类型
	char   m_ip[50];				//ip

	stLoginIPTypeDB()
	{
		m_type = 0;
		bzero(m_ip,sizeof(m_ip));
	}
};
#pragma pack()

#define OPERATION_SERVER_KEY "6EPin6wUV,7mYkbM"

class COperationMgr
{
	USE_SINGLETON_NEW(COperationMgr);
public :
	//析构函数
	~COperationMgr(){}
	//获得返回值
	static COperationMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//清空所有表
	void clearAllTbl();
	//加载所有登录ip类型
	void load();	
	//增加一个ip类型
	void addOneIPType(const char * szIP,eLoginIPType type);
	//判断签名是否正确
	bool verifySign(std::vector<std::string> &vec, std::string &sign);
	//清除所有的ip
	void clearAllIP();
private :
	//构造函数
	COperationMgr()
	{}
};
#endif
