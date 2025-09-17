#ifndef __H_CLOSEBETA_WELFARE_MGR_H__
#define __H_CLOSEBETA_WELFARE_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "TemplateDB.h"
#include <ext/hash_map>
#include <ext/hash_set>
#include <string>

struct CloseBetaRechargeInfo
{
	std::string m_account;
	UINT32 m_serverID;                                          //服务器ID
	UINT32 m_boxID;                                             //boxID
	UINT32 m_giveDiamond;                                       //钻石
	UINT32 m_giveRechargeNum;                                   //vip经验返回

	CloseBetaRechargeInfo()
	{
		m_serverID = 0;
		m_boxID = 0;
		m_giveDiamond = 0;
		m_giveRechargeNum = 0;
	}
};
#pragma pack(1)
struct stCloseBetaRewardDB
{
	char m_account[50];

	stCloseBetaRewardDB()
	{
		bzero(m_account,sizeof(m_account));
	}
};
#pragma pack()

struct stCloseBetaRewardInfo
{
	std::string m_account;

	stCloseBetaRewardInfo()
	{
		
	}
	void initFromDB(const stCloseBetaRewardDB & data);
	void fillDBData(stCloseBetaRewardDB & data);
	void print();
};

class Player;
class CCloseBetaWelfareMgr : public  CTemplateDB<stCloseBetaRewardDB,stCloseBetaRewardInfo>
{
	USE_SINGLETON_NEW(CCloseBetaWelfareMgr);
public :
	static CCloseBetaWelfareMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//构造函数
	CCloseBetaWelfareMgr();
	//析构函数
	~CCloseBetaWelfareMgr();
	//玩家上线
	void userOnline(Player * pUser);
	//添加账号
	virtual void add2Container(stCloseBetaRewardInfo & info);
	//初始化账号信息
	void loadTblInfo();
	//增加充值账号信息
	void addRechargeAccount(CloseBetaRechargeInfo * pRechargeInfo);
	//获得奖励
	void giveReward(Player *pUser);
private :
	typedef __gnu_cxx::hash_map<std::string,CloseBetaRechargeInfo *, my_hash<std::string> > RechargeAccount;
	typedef  RechargeAccount::iterator RechargeAccount_It;
	typedef __gnu_cxx::hash_set<std::string, my_hash<std::string> > RewardAccount;
	typedef  RewardAccount::iterator RewardAccount_It;
	RechargeAccount m_rechargeAccount;
	RewardAccount   m_rewardAccount;
};
#endif
