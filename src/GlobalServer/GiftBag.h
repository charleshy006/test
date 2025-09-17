#ifndef __H_GIFT_BAG_H__
#define __H_GIFT_BAG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "Player.h"
#include <map>

#pragma pack(1)
struct stGiftBagCfgDB
{
	UINT32 m_id;
	char   m_config[250];

	stGiftBagCfgDB()
	{
		m_id = 0;
		bzero(m_config,sizeof(m_config));
	}
};

#pragma pack()


struct stGiftBagInfo
{
	UINT32 m_id ;						//礼包ID
	std::map<UINT32,UINT32>	m_items;	//道具

	stGiftBagInfo()
	{
		m_id = 0;
	}

	void initFromDB(const stGiftBagCfgDB & data);	
};

class CGiftBagMgr
{
	USE_SINGLETON_NEW(CGiftBagMgr);
private :
	//构造函数
	CGiftBagMgr()
	{}
public :
	//析构函数
	~CGiftBagMgr()
	{}
	
	static CGiftBagMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载
	void load();
	//保存礼包
	void addGiftBagCfg(UINT32 id,const std::string & content);
	//保存礼包配置回调
	void addGiftBagCfgCB(bool opResult,UINT64 autoID);
	//更新礼包配置回调
	void updateGiftBagCfgCB(bool opResult,UINT64 autoID);
	//删除礼包配置
	void removeGiftBagCfg(UINT32 id);
	//删除礼包配置回调
	void removeGiftBagCfgCB(bool opResult,UINT64 autoID);
	//领取礼包
	void reqGiftBagReward(Player * pUser,std::string code);
	//获得礼包配置
	stGiftBagInfo * getGiftBagInfo(UINT32 id);
private :
	std::map<UINT32,stGiftBagInfo> m_giftBags;			//所有礼包
};
#endif
