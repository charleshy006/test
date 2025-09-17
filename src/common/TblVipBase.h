#ifndef __TBL_VIPBASE_H__
#define __TBL_VIPBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "CommonMicro.h"

#define VIP_NUM_COEF 1000

struct stVipRawData
{
	INT32 getID() const
	{
		return m_field_1 * VIP_NUM_COEF + m_field_2;
	}

	UINT32  m_field_1;						//称号ID
	UINT32  m_field_2;						//vip等级
	UINT32  m_field_3;						//次数
}__attribute__((packed));


struct stVipBase
{
	UINT32 m_id;				//ID
	UINT32 m_vipLv;				//vip等级
	UINT32 m_num;				//数量

	void fill(const stVipRawData & data)
	{
		m_id = data.m_field_1;
		m_vipLv = data.m_field_2;
		m_num = data.m_field_3;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("称号表 id = %d,vipLv=%d,num=%d",m_id,m_vipLv,m_num);
#endif
	}
};
#endif
