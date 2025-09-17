#ifndef __TBL_PRICE_H__
#define __TBL_PRICE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "CommonMicro.h"

#define NUM_PRICE_COEF 1000

struct stPriceRawData
{
	INT32 getID() const
	{
		return m_field_1 * NUM_PRICE_COEF + m_field_2;
	}

	UINT32  m_field_1;						//价格类型
	UINT32  m_field_2;						//购买次数
	UINT32  m_field_3;						//总价
}__attribute__((packed));


struct stPriceBase
{
	UINT32 m_id;							//ID
	UINT32 m_num;							//vip等级
	UINT32 m_price;							//数量
	
	static UINT32 getFindID(UINT32 id,UINT32 lv)
	{
		return id * NUM_PRICE_COEF + lv;	
	}

	void fill(const stPriceRawData & data)
	{
		m_id = data.m_field_1;
		m_num = data.m_field_2;
		m_price = data.m_field_3;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("价格表 id = %d,num=%d,price=%d",m_id,m_num,m_price);
#endif
	}
};
#endif
