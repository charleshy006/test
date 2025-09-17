#ifndef __LOGININFO_H__
#define  __LOGININFO_H__

#include <cstdlib>
#include "TypesDef.h"
#include "CommonMicro.h"
#pragma pack(1)
struct LoginInfo
{
	UINT32 m_userID;							//帐号ID
	char   m_account[MAX_ACCNAMESIZE];			//帐号名字
	char   m_passwd[MAX_PASSWORD_LEN];			//密码
	char   m_tmpAccount[MAX_ACCNAMESIZE];		//临时账号
	UINT8  m_bind;								//是否绑定了
	UINT32 m_createTime;						//创建时间
	UINT32 m_lastLoginTime;						//登录时间

	LoginInfo()
	{
		m_userID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_passwd,sizeof(m_passwd));
		bzero(m_tmpAccount,sizeof(m_tmpAccount));
		m_bind = 0;
		m_createTime = 0;
		m_lastLoginTime = 0;
	}
};
#pragma pack()
#endif


