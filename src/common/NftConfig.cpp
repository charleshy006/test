#include "NftConfig.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool NftConfig::loadCfg()
{
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/nftItem.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[NFT配置]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[NFT配置]找不到节点root");
		return false;
	}

	INT32 iVal = 0;
	const char * szVal = NULL;

	TiXmlElement * pNode = pRoot->FirstChildElement("vip");
	if (pNode){
		
		TiXmlElement * pRewardNode = pNode->FirstChildElement("dailyreward");
		while (pRewardNode){
			UINT32 VipLevel; 
			if (pRewardNode->Attribute("level",&iVal)){
				VipLevel = iVal ;	
			}
			TiXmlElement * pItemNode = pRewardNode->FirstChildElement("entry");
			while (pItemNode){   
				UINT32 itemID; 
				UINT32 itemCnt;
				if (pItemNode->Attribute("itemid",&iVal)){
					itemID = iVal ;	
				}
				if (pItemNode->Attribute("count",&iVal)){
					itemCnt = iVal ;	
				}
		
				m_nftVipItems[VipLevel][itemID] = itemCnt;
				pItemNode = pItemNode->NextSiblingElement("entry");
			}
			pRewardNode = pRewardNode->NextSiblingElement("dailyreward");
		}
	}

	pNode = pRoot->FirstChildElement("nftitem");
	if (pNode){
		TiXmlElement * pItemNode = pNode->FirstChildElement("entry");
		while (pItemNode){  
			stNftItem* st = new(stNftItem);

			if (pItemNode->Attribute("id",&iVal)){
				st->m_id = iVal ;	
			}

			if (pItemNode->Attribute("atk",&iVal)){
				st->m_atk = iVal ;	
			}
			if (pItemNode->Attribute("hp",&iVal)){
				st->m_hp = iVal ;	
			}
			if (pItemNode->Attribute("flag",&iVal)){
				st->m_flag = iVal ;	
			}else{
				st->m_flag = 0 ;	
			}
	 
			if ((szVal = pItemNode->Attribute("url"))){
				strcpy(st->m_url,szVal);	 
			}

			m_nftItems[st->m_id] = st;
			pItemNode = pItemNode->NextSiblingElement("entry");
		}
	}

	
	pNode = pRoot->FirstChildElement("createreward");
	if (pNode){
		TiXmlElement * pAccNode = pNode->FirstChildElement("entry");
		while (pAccNode){  
			// stNftItem* st = new(stNftItem);

			if (!(szVal = pAccNode->Attribute("account"))){
				continue;
			}

			if (!pAccNode->Attribute("diamond",&iVal)){
 				continue;
			}
			 
			m_acc2Diamond[std::string(szVal)] = iVal;
			pAccNode = pAccNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	{
		std::map<UINT32,stNftItem*>::iterator it = m_nftItems.begin();
		for (;it != m_nftItems.end();++it){
			Athena::logger->trace("NFT 道具:id=%d,flag=%d",it->first,it->second->m_flag);
		}
	}
	{
		std::map<UINT32,std::map<UINT32,UINT32> >::iterator it = m_nftVipItems.begin();
		for (;it != m_nftVipItems.end();++it){
			std::map<UINT32,UINT32>::iterator itSub = it->second.begin();
			for (;itSub != it->second.end();++itSub){
				Athena::logger->trace("NFT VIP道具:level:%d,id=%d,count=%d",it->first, itSub->first, itSub->second);
			}
		}
	}
	{
		std::map<std::string,UINT32>::iterator it = m_acc2Diamond.begin();
		for (;it != m_acc2Diamond.end();++it){
			 Athena::logger->trace("NFT 创建账号给钻石:id=%s,diamond=%d",it->first.c_str(),it->second);
		}
	}
#endif 
	return true;
}