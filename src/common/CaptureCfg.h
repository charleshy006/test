#ifndef __H_CAPTURE_CFG_H__
#define __H_CAPTURE_CFG_H__
#include <string>
#include <map>
#include "tinyxml/tinyxml.h"
#include "Singleton.h"

class CCaptureCfg
{
	USE_SINGLETON_NEW(CCaptureCfg);
public:
	struct stPeaceModulePrice
	{
		UINT16 m_hour;
		UINT32 m_itemID;
		UINT16 m_costType;
		UINT32 m_costNum;

		stPeaceModulePrice()
		{
			m_hour = 0;
			m_itemID = 0;
			m_costType = 0;
			m_costNum = 0;
		}
	};
private:
	//构造函数
	CCaptureCfg();
public :
	//获得实例
	static CCaptureCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CCaptureCfg();
	//加载配置
	bool loadCfg();
	//获得1小时
	stPeaceModulePrice & get1HourPrice()
	{
		return m_1HourPrice;
	}
	//获得3小时
	stPeaceModulePrice & get3HourPrice()
	{
		return m_3HourPrice;
	}
	//获得12小时
	stPeaceModulePrice & get12HourPrice()
	{
		return m_12HourPrice;
	}
	//是否开始
	bool canCapture(UINT32 secNumOfToday)
	{
		return (secNumOfToday >= m_beginTime) && (secNumOfToday <= m_endTime);	
	}
private :
	stPeaceModulePrice  m_1HourPrice;
	stPeaceModulePrice 	m_3HourPrice;
	stPeaceModulePrice  m_12HourPrice;
	UINT32				m_beginTime;
	UINT32				m_endTime;
};
#endif
