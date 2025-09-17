#ifndef __H_RECHARGEMGR_H__
#define __H_RECHARGEMGR_H__
#include "Singleton.h"
#include "Player.h"
#include "net.h"
#include <map>

struct stRechargeDBInfo
{
	UINT32 m_charID;
	UINT32 m_money;
	UINT32 m_itemID;
	char   m_tid[50];
	UINT8  m_bObtain;				//是否已经获取
#ifdef _YUNDING_TONGJI
	UINT8  m_isExtend;              //是否统计
	char   m_data[1024];             //扩展
#endif
	stRechargeDBInfo()
	{
		m_charID = 0;
		m_money = 0;
		m_itemID = 0;
		bzero(m_tid,sizeof(m_tid));
		m_bObtain = 0;
#ifdef _YUNDING_TONGJI
		m_isExtend = 0;
		bzero(m_data,sizeof(m_data));
#endif
	}
}__attribute__((packed));

struct stRechargeInfo
{
	UINT32 m_charID;
	UINT32 m_money;
	UINT32 m_itemID;
	char   m_tid[50];
	UINT8  m_bObtain;				//是否已经获取
#ifdef _YUNDING_TONGJI
	UINT8  m_isExtend;				//是否统计
	char   m_data[1024];				//扩展
#endif
	stRechargeInfo()
	{
		m_charID = 0;
		m_money = 0;
		m_itemID = 0;
		bzero(m_tid,sizeof(m_tid));
		m_bObtain = 0;
#ifdef _YUNDING_TONGJI
		m_isExtend = 0;
		bzero(m_data,sizeof(m_data));
#endif
	}

	void initFromDB(const stRechargeDBInfo & dbData);
	void fillDBData(stRechargeDBInfo & dbData);
};

class CRechargeMgr
{
	USE_SINGLETON_NEW(CRechargeMgr);
public :
	//构造函数
	CRechargeMgr();
	//析构函数
	~CRechargeMgr();
	//获得实例
	static CRechargeMgr  & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//用户上线
	void userOnline(Player * pUser);
	//在线玩家充值
	void onlineRecharge(Player * pUser,UINT32 money,UINT32 itemID,const std::string & tid, bool isExtend = false, const std::string & data = "");
	//离线玩家充值
	void offlineRecharge(UINT32 charID,UINT32 money,UINT32 itemID,const std::string & tid, bool isExtend = false, const std::string & data = "");
	//增加一条记录
	void addRecord(stRechargeDBInfo & data);
	//增加记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一条记录
	void updateRecord(stRechargeDBInfo & data);
	//更新一条记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//更新已获取标记
	void updateObtainFlag(UINT32 charID);
	//获取已获取标记回调
	void updateObtainFlagCB(bool opResult,UINT64 autoID);
	//删除一条记录
	void delRecord(UINT32 charID);
	//删除一条记录回调
	void delRecordCB(bool opResult,UINT64 autoID);
	//启动服务器加载
	void load();
	//添加充值流水
	void addTid(std::string tid);
	//充值流水是否已经存在
	bool checkTid(std::string tid);
private :
	std::map<UINT32,std::vector<stRechargeInfo> >  m_userRechargeMap;
	std::set<std::string>	 m_tidCaches;			//充值流水缓存
};
#endif
