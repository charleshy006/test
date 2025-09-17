#ifndef __H_GUARD_MGR_H__
#define __H_GUARD_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include <map>
#include <list>
#pragma pack(1)
struct 	stGuardInfoDB
{
	UINT32 m_charID;				//角色ID
	UINT8  m_quality;				//品质
	UINT32 m_endTime;				//结束时间
	UINT32 m_helpFriendID;			//协助好友ID
	UINT16 m_beInterceptNum;		//被拦截次数
	UINT16 m_refreshNum;			//刷新次数
	UINT32 m_decResNum1;			//扣除的资源1
	UINT32 m_decResNum2;			//扣除的资源2

	stGuardInfoDB()
	{
		m_charID = 0;
		m_quality = 0;
		m_endTime = 0;
		m_helpFriendID = 0;
		m_beInterceptNum = 0;
		m_refreshNum = 0;
		m_decResNum1 = 0;
		m_decResNum2 = 0;
	}
};

struct stGuardUserInfoDB
{
	UINT32 m_charID;			//角色ID
	UINT16 m_guardNum;			//护送次数
	UINT16 m_interceptNum;		//拦截次数
	UINT16 m_helpNum;			//协助次数
	UINT8  m_bHasRefresh;		//是否已经刷新
	UINT16 m_buff;				//召唤的buff
	UINT16 m_resetNum;			//重置次数
	UINT32 m_lastInterceptTime;	//上次拦截次数
	//UINT32 m_decResNum1;			//扣除的资源1
	//UINT32 m_descResNum2;			//扣除的资源2

	stGuardUserInfoDB()
	{
		m_charID = 0;
		m_guardNum = 0;
		m_interceptNum = 0;
		m_helpNum = 0;
		m_bHasRefresh = 0;
		m_buff = 0;
		m_resetNum = 0;
		m_lastInterceptTime = 0;
		//m_decResNum1 = 0;
		//m_descResNum2 = 0;
	}
};

struct stResetGuardUserInfoDB
{
	UINT16 m_guardNum;			//护送次数
	UINT16 m_interceptNum;		//拦截次数
	UINT16 m_helpNum;			//协助次数
	UINT16 m_resetNum;			//重置次数

	stResetGuardUserInfoDB()
	{
		m_guardNum = 0;
		m_interceptNum = 0;
		m_helpNum = 0;
		m_resetNum = 0;
	}
};
#pragma pack()

struct stGuardInfo
{
	UINT32 m_charID;				//角色ID
	UINT8  m_quality;				//品质
	UINT32 m_endTime;				//开始时间
	UINT32 m_helpFriendID;			//协助好友ID
	UINT16 m_beInterceptNum;		//被拦截次数
	UINT16 m_refreshNum;            //刷新次数
	UINT32 m_decResNum1;			//扣除的资源1
	UINT32 m_decResNum2;			//扣除的资源2
	bool   m_beIntercepting;		//是否正在被劫持

	stGuardInfo()
	{
		m_charID = 0;
		m_quality = 0;
		m_endTime = 0;
		m_helpFriendID = 0;
		m_beInterceptNum = 0;
		m_refreshNum = 0;
		m_decResNum1 = 0;
		m_decResNum2 = 0;
		m_beIntercepting = false;
	}

	void initFromDB(const stGuardInfoDB & data);
	void fillDBData(stGuardInfoDB & data);
};

struct stGuardEnemy
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//名字
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别

	stGuardEnemy()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_job = 0;
		m_sex = 0;
	}
};


struct stGuardUserInfo
{
	UINT32 m_charID;			//角色ID
	UINT16 m_guardNum;			//护送次数
	UINT16 m_interceptNum;		//拦截次数
	UINT16 m_helpNum;			//协助次数
	UINT8  m_bHasRefresh;		//是否已经刷新
	UINT16 m_buff;				//召唤的buff
	UINT16 m_resetNum;			//重置次数
	UINT32 m_lastInterceptTime;	//上次拦截次数
	std::list<stGuardEnemy> m_emenies;//敌人列表
	std::map<UINT32,std::string> m_interceptInfos; //拦截信息
	bool   m_bHelpGuarding;		//协助护送中

	stGuardUserInfo()
	{
		m_charID = 0;
		m_guardNum = 0;
		m_interceptNum = 0;
		m_helpNum = 0;
		m_bHasRefresh = 0;
		m_buff = 0;
		m_resetNum = 0;
		m_lastInterceptTime = 0;
		m_bHelpGuarding = false;
	}

	void initFromDB(const stGuardUserInfoDB & data);
	void fillDBData(stGuardUserInfoDB & data);
};

class CGuardMgr
{
	USE_SINGLETON_NEW(CGuardMgr);
public :
	static const UINT16 s_MAX_SEND_ENEMY = 5;
	static const UINT16 s_MAX_SAVE_ENEMY = 10;
	static const UINT16 s_MAX_SEND_GUARDINFO = 40;
	static const UINT16 s_MAX_INSPIRED_NUM = 5;
	static const UINT16 s_MAX_HELP_FRIEND_NUM = 10;
	static const UINT16 s_NEED_LEVEL = 23;
private :
	//构造函数
	CGuardMgr()
	{}
public :
	//获得单例
	static CGuardMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CGuardMgr();
	//加载全部
	bool load();
	//新增一个护送记录
	void addGuard(const stGuardInfoDB & data);
	//新增护送回调
	void addGuardCB(bool opResult,UINT64 autoID);
	//更新一个护送记录
	void updateGuard(stGuardInfoDB & data);
	//更新一个护送回调
	void updateGuardCB(bool opResult,UINT64 autoID);
	//删除一个护送
	void removeGuard(UINT32 charID);
	//删除记录回调
	void removeGuardCB(bool opResult,UINT64 autoID);	
	//新增一个玩家护送记录
	void addGuardUserInfo(const stGuardUserInfoDB & data);
	//新增玩家护送记录回调
	void addGuardUserInfoCB(bool opResult,UINT64 autoID);
	//更新一个护送记录
	void updateGuardUserInfo(stGuardUserInfoDB & data);
	//更新一个护送回调
	void updateGuardUserInfoCB(bool opResult,UINT64 autoID);
	//删除一个护送
	void removeGuardUserInfo(UINT32 charID);
	//删除记录回调
	void removeGuardUserInfoCB(bool opResult,UINT64 autoID);
	//打开护送界面
	void openGuardWin(Player * pUser);
	//获得护送玩家信息
	stGuardUserInfo * getGuardUserInfo(UINT32 charID);
	//获得护送信息
	stGuardInfo * getGuardInfo(UINT32 charID);
	//发送某个护送的详细信息
	void sendGuardDetailInfo(Player * pUser,UINT32 charID);
	//定时器
	void timer(UINT32 cur);
	//完成某个护送
	void finishGuard(UINT32 charID);
	//清空护送拦截cd
	void clearInterceptCD(Player * pUser);
	//重置拦截次数
	void resetInterceptNum(Player * pUser);
	//发送护送信息
	void sendGuardWin(Player * pUser);
	//检查是否有相关数据,没有就添加
	void checkExist(UINT32 charID);
	//召唤最高级别的护送
	void summonHighQualityGuard(Player * pUser,UINT8 quality);
	//鼓舞士气
	void inspired(Player *pUser);
	//开始护送
	void beginGuard(Player * pUser);
	//发送战报
	void sendInterceptInfos(Player * pUser);
	//拦截护送
	void interceptGuard(Player * pUser,UINT32 charID);
	//拦截结果
	void interceptResult(Player * pUser,bool win);
	//快速完成
	void quickFinish(Player * pUser);
	//删除正在护送中的任务
	void delGuarding(UINT32 charID)
	{
		m_guardings.erase(charID);
	}
	//玩家下线
	void userOffline(Player * pUser);
	//玩家上线
	void userOnline(Player * pUser);
	//重置护送信息
	void resetGuardUserInfo();
	//重置护送信息回调
	void resetGuardUserInfoCB(bool opResult,UINT64 autoID);
private :
	std::map<UINT32,stGuardInfo> m_guardInfos;				//护送信息 <charID,stGuardInfo>
	std::map<UINT32,stGuardUserInfo> m_guardUserInfos;		//玩家护送信息 <charID,stGuardUserInfo>
	std::map<UINT32,UINT32>  m_guardings;					//正在护送的列表
};
#endif
