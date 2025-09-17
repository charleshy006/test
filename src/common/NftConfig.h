#ifndef __NFT_CONFIG_H__
#define __NFT_CONFIG_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>

//nft道具配置
struct stNftItem
{
	UINT32 m_id;				//ID
	UINT32 m_atk;				//ATK
	UINT32 m_hp;				//HP
	UINT32 m_flag;				//flag
	char m_url[200];			// 

	stNftItem()
	{
		m_id = 0;
		m_atk = 0;
		m_hp = 0;
		memset(m_url,0,sizeof(m_url)); 
	}
};     

class NftConfig
{
	USE_SINGLETON_NEW(NftConfig);
 
	  
private :
	NftConfig()
	{  
	}
public :
	//析构函数
	~NftConfig(){}
	//获得实例
	static NftConfig & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
 
	std::map<UINT32,stNftItem*>& getItemMap()
	{
		return m_nftItems;
	}

	std::map<UINT32,UINT32>& getVipMailItemMap(UINT32 vipLv)
	{
		return m_nftVipItems[vipLv];
	}

	stNftItem* getItemCfg(UINT32 itemID)
	{
		return m_nftItems[itemID];
	}
	UINT32 getCreateDiamond(std::string acc)
	{
		if (m_acc2Diamond.find(acc) != m_acc2Diamond.end())
		{
			return m_acc2Diamond[acc];
		}
		return 0;
	}
	
private : 
	std::map<UINT32,stNftItem*> m_nftItems;
	std::map<UINT32,std::map<UINT32,UINT32> > m_nftVipItems;
	std::map<std::string,UINT32>  m_acc2Diamond;
  
};
#endif
