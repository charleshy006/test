#ifndef __SVRINFO_H__
#define __SVRINFO_H__

#include <cstdlib>
#include "TypesDef.h"
#include "CommonMicro.h"
#pragma pack(1)

struct SvrInfo 
{
	UINT16		m_svrID;					//svrID
#ifdef _THAILAND_VER
	char		m_name[32];                 //svr名字
#else
	char		m_name[MAX_NAMESIZE];		//svr名字
#endif
	UINT16		m_svrType;					//svr类型	
	char		m_ip[MAX_IP_LENGTH + 1];	//本地ip 
	UINT16		m_port;						//本地端口

	SvrInfo()
	{
		m_svrID = 0;
		bzero(m_name,sizeof(m_name));
		m_svrType = 0;
		bzero(m_ip,sizeof(m_ip));
		m_port = 0;
	}

	SvrInfo(const SvrInfo& se)
	{
		*this = se;
	}

	SvrInfo& operator = (const SvrInfo& se)
	{
		m_svrID = se.m_svrID;
		strncpy(m_name,se.m_name,sizeof(m_name));
		m_svrType = se.m_svrType;
		strncpy(m_ip,se.m_ip,sizeof(m_ip));
		m_port = se.m_port;
		return *this;
	}

	bool operator == (const SvrInfo &se)
	{
		if ((m_svrID == se.m_svrID)
			 && (m_port == se.m_port)
			 && (strncmp(m_ip,se.m_ip,sizeof(m_ip)) == 0)){
			return true;	
		}
		else {
			return false;
		}
	}
};

#pragma pack()
#endif


