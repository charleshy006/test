#ifndef __TBL_CLOSE_BETA_WELFARE_H__
#define __TBL_CLOSE_BETA_WELFARE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

struct CloseBetaWelfareRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}
	
	UINT32  m_field_1;						//ID
	char    m_field_2[100];                 //账号	
	UINT32  m_field_3;						//服务器ID
	UINT32  m_field_4;						//内侧礼包
	UINT32  m_field_5;						//钻石返还
	UINT32  m_field_6;						//vip经验返回
}__attribute__((packed));

struct stCloseBetaWelfareBase
{
	UINT32 m_id;												//ID
	std::string m_account;										//账号
	UINT32 m_serverID;											//服务器ID
	UINT32 m_boxID;												//boxID
	UINT32 m_giveDiamond;										//钻石
	UINT32 m_giveRechargeNum;									//vip经验返回

	void fill(const  CloseBetaWelfareRawData& data)
	{
		m_id  = data.m_field_1;
		m_account = data.m_field_2;
		m_serverID = data.m_field_3;
		m_boxID = data.m_field_4;
		m_giveDiamond = data.m_field_5;
		m_giveRechargeNum = data.m_field_6;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("id:%d,account:%s,serverID:%d,boxID:%d,giveDiamond:%d,giveRechargeNum:%d",m_id,m_account.c_str(),m_serverID,m_boxID,m_giveDiamond,m_giveRechargeNum);
#endif
	}
};

#endif
