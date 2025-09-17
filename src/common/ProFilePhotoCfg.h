#ifndef __PROFILEPHOTO_H__
#define __PROFILEPHOTO_H__

#include "TypesDef.h"
#include "Singleton.h"
#include <map>
#include <vector>
#include <tinyxml/tinyxml.h>

struct stProFilePhotoInfo
{
	UINT16	m_id;
	UINT8	m_unlockCondition;	//解锁条件
	UINT32	m_conditionNum;		//解锁条件对应值

	stProFilePhotoInfo()
	{
		m_id				=	0;
		m_unlockCondition	=	0;
		m_conditionNum		=	0;
	}
};

class CProFilePhotoCfg
{
	USE_SINGLETON_NEW(CProFilePhotoCfg);
public:
	//构造函数
	CProFilePhotoCfg() {}

		//析构函数
	~CProFilePhotoCfg() {}
	//获得实例
	static CProFilePhotoCfg & getSingleton() {
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
	//根据id获取配置
	stProFilePhotoInfo * getConfigurationByID(UINT32 roleID, UINT16 confID);
private:
	std::map<UINT32, std::map<UINT16, stProFilePhotoInfo> > m_conf; //profession::id  entry::id
};

#endif
