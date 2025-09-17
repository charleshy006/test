#ifndef __TBL_HEROTRAIN_H__
#define __TBL_HEROTRAIN_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "CommonMicro.h"

#define HEROTRAIN_NUM_COEF 100

struct stHeroTrainRawData
{
	INT32 getID() const
	{
		return m_field_1 * HEROTRAIN_NUM_COEF + m_field_2;
	}

	UINT32  m_field_1;						//heroID
	UINT32  m_field_2;						//调教等级
	UINT32  m_field_3;						//所需exp
}__attribute__((packed));


struct stHeroTrain
{
	UINT32 m_heroId;				//heroID
	UINT32 m_trainLv;				//调教等级
	UINT32 m_exp;					//所需exp

	void fill(const stHeroTrainRawData & data)
	{
		m_heroId = data.m_field_1;
		m_trainLv = data.m_field_2;
		m_exp = data.m_field_3;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("随从调教表 m_heroId = %d,m_trainLv=%d,m_exp=%d",m_heroId,m_trainLv,m_exp);
#endif
	}
};
#endif
