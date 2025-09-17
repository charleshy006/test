#ifndef __H_TXGIFTBAG_MGR_H__
#define __H_TXGIFTBAG_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include <map>
#pragma pack(1)
struct 	stTxGiftBagAwardDB
{
	UINT32 m_charID;
	UINT32 m_taskID;
	char   m_billNo[50];

	stTxGiftBagAwardDB()
	{
		m_charID = 0;
		m_taskID = 0;
		bzero(m_billNo,sizeof(m_billNo));
	}
};
#pragma pack()


class CTxGiftBagMgr
{
	USE_SINGLETON_NEW(CTxGiftBagMgr);
private :
	//构造函数
	CTxGiftBagMgr()
	{}
public :
	//获得单例
	static CTxGiftBagMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CTxGiftBagMgr();
	//加载全部
	bool load();
	//新增一个全局记录
	void addRecord(stTxGiftBagAwardDB & data);
	//新增记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//是否已经领取了礼包
	bool hasGotGiftBag(UINT32 charID,UINT32 taskID);
	//增加领取记录
	void addGotGiftBagInfo(UINT32 charID,UINT32 taskID,const char * billNo);
private :
	std::map<UINT32,std::set<UINT32> > m_awardInfos;  //<charID,std::set<TaskID>>
};
#endif
