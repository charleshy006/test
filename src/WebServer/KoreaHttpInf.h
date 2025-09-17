#ifndef __H_KOREA_HTTP_INF_H__
#define __H_KOREA_HTTP_INF_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include <map>
#include <string>
#include <vector>

#pragma pack(1)
struct stGetRoleDataDB
{
	UINT32 m_charID;
	char   m_account[MAX_ACCNAMESIZE +1];
	char   m_name[MAX_NAMESIZE + 1];
	UINT16 m_level;
	UINT32 m_goinCoin;
	UINT32 m_diamond;
	UINT32 m_giveDiamond;

	stGetRoleDataDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_goinCoin = 0;
		m_diamond = 0;
		m_giveDiamond = 0;
	}
};
#pragma pack()

#define KOREA_HTTP_INF_SERVER_KEY "FFb=.rWBHp3pIE)xe"

class CKoreaHttpInfMgr
{
	USE_SINGLETON_NEW(CKoreaHttpInfMgr);
public :
	//析构函数
	~CKoreaHttpInfMgr(){}
	//获得返回值
	static CKoreaHttpInfMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载
	void load();	
	//判断签名是否正确
	bool verifySign(std::vector<std::string> &vec, std::string &sign);
	//更新在线数量
	void updateOnline(UINT32 num)
	{
		m_onlineNum = num;
	}
	//获取在线玩家数据
	std::string getOnlineInfo();	
	//搜索角色ID
	std::string searchRoleID(std::string roleName);
	//获得角色信息
	std::string getRoleData(UINT32 charID);
	//封号
	bool lockCharacter(UINT32 charID,bool bBan,UINT16 connID);
	//发送走马灯
	void sendNotice(UINT32 noticeID,std::string content,UINT32 interval,UINT32 num);
	//删除公告
	void delNotice(UINT32 noticeID);
	//发送邮件
	void sendMail(UINT32 charID,std::string title,std::string content,UINT32 itemID1,UINT32 itemNum1,UINT32 itemID2,UINT32 itemNum2,UINT32 itemID3,UINT32 itemNum3,UINT16 connID);	
private :
	//构造函数
	CKoreaHttpInfMgr()
	{
		m_onlineNum = 0;
	}
private :
	UINT32 m_onlineNum;			//在线数量
};
#endif
