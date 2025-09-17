#ifndef __TBL_REFIMINGBASE_H__
#define __TBL_REFIMINGBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

struct RefimingRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}
	
	UINT32  m_field_1;						//次数
	char	m_field_2[33];					//名字
	char  	m_field_3[20];						//属性
	UINT32  m_field_4;						//消耗数量
}__attribute__((packed));


struct stRefimingBase
{
	UINT32 m_id;							//关卡ID
	char   m_name[MAX_NAMESIZE + 1];		//名字
	UINT8  m_propID;						//属性类型
	UINT32 m_propVal;						//属性值
	UINT32 m_cost;							//消耗

	void fill(const RefimingRawData & data)
	{
		m_id = data.m_field_1;
		strncpy(m_name,data.m_field_2,sizeof(m_name));
		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_3,"-");
		if (strVec.size() >= 2){
			m_propID = (UINT8)atoi(strVec[0].c_str());
			m_propVal = (UINT32)atoi(strVec[1].c_str());
		}
		m_cost = data.m_field_4;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("炼星配置id=%d,name=%s,prop=%d,val=%d,costs=%d",m_id,m_name,m_propID,m_propVal,m_cost);
#endif
	}
};

#endif
