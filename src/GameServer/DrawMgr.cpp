#include "DrawMgr.h"
#include "GameUser.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "Misc.pb.h"
#include "GlobalSvrClient.h"
#include "GameEventNotify.h"
#include "Chat.h"
#include "LimitTimePetCfg.h"
#include "OperateActivityCfg.h"

extern GameServer * g_gameServer;

CDrawMgr::stDrawInfo CDrawMgr::m_gold1DrawCfg;			//金币抽奖
CDrawMgr::stDrawInfo CDrawMgr::m_gold10DrawCfg;			//金币10连抽
CDrawMgr::stDrawInfo CDrawMgr::m_firstGoldDrawCfg;		//首次金币抽奖

CDrawMgr::stDrawInfo CDrawMgr::m_first10GoldDrawCfg;	//首次金币10连抽

CDrawMgr::stDrawInfo CDrawMgr::m_diamond1DrawCfg;		//钻石抽奖
CDrawMgr::stDrawInfo CDrawMgr::m_diamond10DrawCfg;		//钻石10连抽
CDrawMgr::stDrawInfo CDrawMgr::m_firstDiamondDrawCfg;	//第一次钻石抽奖
CDrawMgr::stDrawInfo CDrawMgr::m_firstFreeDiamondDrawCfg;//首次免费钻石抽奖

CDrawMgr::stDrawInfo CDrawMgr::m_first10DiamondDrawCfg;	//第一次钻石10连抽
CDrawMgr::stDrawInfo CDrawMgr::m_second10DiamondDrawCfg;	//第二次钻石10连抽
CDrawMgr::stDrawInfo CDrawMgr::m_diamond10Mod10DrawCfg;	//抽奖次数是10倍数时候
CDrawMgr::stDrawInfo CDrawMgr::m_diamond150Mod2DrawCfg;	//抽奖次数是150余2时候
CDrawMgr::stDrawInfo CDrawMgr::m_diamond150Mod32DrawCfg; //抽奖次数是150余32时候
CDrawMgr::stDrawInfo CDrawMgr::m_diamond150Mod62DrawCfg;	//抽奖次数是150余62时候
CDrawMgr::stDrawInfo CDrawMgr::m_diamond150Mod92DrawCfg;	//抽奖次数是150余92时候
CDrawMgr::stDrawInfo CDrawMgr::m_diamond150Mod122DrawCfg;//抽奖次数是150余122时候
CDrawMgr::stDrawInfo CDrawMgr::m_diamond30Mod12DrawCfg;	//抽奖次数是30余12时候

//CDrawMgr::stDrawInfo CDrawMgr::m_drawHeroCfg;			//武神抽取配置
//CDrawMgr::stDrawInfo CDrawMgr::m_10DrawHeroCfg;			//十次武神抽取配置
//CDrawMgr::stDrawInfo CDrawMgr::m_drawHero10Mod10DrawCfg; //10倍数时候的抽奖
//CDrawMgr::stDrawInfo CDrawMgr::m_drawHero3Mod3DrawCfg;	//3倍数时候的抽奖

std::map<UINT16,CDrawMgr::stLimitPetDrawInfo> CDrawMgr::m_limitPetDrawInfoCnt;

CDrawMgr::stDrawInfo CDrawMgr::m_treasureDrawCfg;		//宝藏抽奖
CDrawMgr::stDrawInfo CDrawMgr::m_treasure10DrawCfg;		//十次宝藏抽奖
CDrawMgr::stDrawInfo CDrawMgr::m_treasure10Mod10DrawCfg; //宝藏10的倍数时候抽奖
CDrawMgr::stDrawInfo CDrawMgr::m_treasure3Mod3DrawCfg;	//宝藏3的倍数时候抽奖

CDrawMgr::CDrawMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_lastFreeGoldDrawTime = 0;
	m_freeGoldDrawNum = 0;
	m_lastFreeDiamondDrawTime = 0;
	m_lastFreeDrawHeroTime = 0;
	m_diamondDrawNum = 0;
	m_heroDrawNum = 0;
	m_flag = 0;
	m_drawHeroScore = 0;
	m_clearDrawHeroTime = 0;
	m_treasureDrawNum = 0;
	m_lastFreeDrawTreasureTime = 0;
}

CDrawMgr::~CDrawMgr()
{

}

void CDrawMgr::parseSubNode(TiXmlElement * pSubNode,stDrawInfo & cfg)
{
	if (!pSubNode){
		return ;
	}
	INT32 iVal = 0;
	
	if (pSubNode->Attribute("Moneytype",&iVal)){
		cfg.m_moneyType = iVal;
	}

	if (pSubNode->Attribute("needMoney",&iVal)){
		cfg.m_needMoney = iVal;
	}
	
	if (pSubNode->Attribute("interval",&iVal)){
		cfg.m_interval = iVal;
	}

	TiXmlElement * pDrawItemNode = pSubNode->FirstChildElement("drawitem");
	while (pDrawItemNode){
		stDrawItem item;
		if (pDrawItemNode->Attribute("ID",&iVal)){
			item.m_id = iVal;
		}

		if (pDrawItemNode->Attribute("drawnum",&iVal)){
			item.m_num = iVal;
		}

		if (pDrawItemNode->Attribute("drawRP",&iVal)){
			item.m_rate = iVal;
		}
#ifdef _HDZ_DEBUG
		/*for (UINT16 i = 0;i < cfg.m_items.size(); ++i){
			stDrawItem & drawItem = cfg.m_items[i];
			if (drawItem.m_id == item.m_id){
				Athena::logger->error("抽奖表同一个配置了发送相同的ID:%d",item.m_id);
			}
		}*/
#endif
		cfg.m_items.push_back(item);
		pDrawItemNode = pDrawItemNode->NextSiblingElement("drawitem");
	}

}

bool CDrawMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/draw.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[抽奖配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[抽奖配置]抽奖配置加载错误,找不到Root节点!");
		return false;
	}

	TiXmlElement * pDrawNode = pRoot->FirstChildElement("draw");

	if (pDrawNode) {
		TiXmlElement * pGoldDraw1Node = pDrawNode->FirstChildElement("golddrawgroup01");
		if (pGoldDraw1Node){
			parseSubNode(pGoldDraw1Node,m_gold1DrawCfg);
		}

		TiXmlElement * pGoldDraw10Node = pDrawNode->FirstChildElement("golddrawgroup10");
		if (pGoldDraw10Node){
			parseSubNode(pGoldDraw10Node,m_gold10DrawCfg);	
		}
		
		TiXmlElement * pFirstGoldDraw10Node = pDrawNode->FirstChildElement("firstTenGolddraw");
		if (pFirstGoldDraw10Node){
			parseSubNode(pFirstGoldDraw10Node,m_first10GoldDrawCfg);
		}
		
		TiXmlElement * pFirstGoldDrawNode = pDrawNode->FirstChildElement("firstGolddraw");
		if (pFirstGoldDrawNode){
			parseSubNode(pFirstGoldDrawNode,m_firstGoldDrawCfg);
		}

		TiXmlElement * pDiamond1Node = pDrawNode->FirstChildElement("Diamonddraw01");
		if (pDiamond1Node){
			parseSubNode(pDiamond1Node,m_diamond1DrawCfg);	
		}

		TiXmlElement * pDiamond10Node = pDrawNode->FirstChildElement("Diamonddraw10");
		if (pDiamond10Node){
			parseSubNode(pDiamond10Node,m_diamond10DrawCfg);	
		}

		TiXmlElement * pFirstDiamonddrawNode = pDrawNode->FirstChildElement("firstDiamonddraw"); 
		if (pFirstDiamonddrawNode){
			parseSubNode(pFirstDiamonddrawNode,m_firstDiamondDrawCfg);	
		}
		
		TiXmlElement * pFirstFreeDiamonddraw = pDrawNode->FirstChildElement("firstFreeDiamonddraw");
		if (pFirstFreeDiamonddraw){
			parseSubNode(pFirstFreeDiamonddraw,m_firstFreeDiamondDrawCfg);
		}

		TiXmlElement * pFirstTenDiamonddrawNode = pDrawNode->FirstChildElement("firstTenDiamonddraw");
		if (pFirstTenDiamonddrawNode){
			parseSubNode(pFirstTenDiamonddrawNode,m_first10DiamondDrawCfg);	
		}

		TiXmlElement * pSecondTenDiamonddrawNode = pDrawNode->FirstChildElement("secondTenDiamonddraw");
		if (pSecondTenDiamonddrawNode){
			parseSubNode(pSecondTenDiamonddrawNode,m_second10DiamondDrawCfg);
		}

		TiXmlElement * pCertain01Node = pDrawNode->FirstChildElement("certain01");
		if (pCertain01Node){
			parseSubNode(pCertain01Node,m_diamond10Mod10DrawCfg);	
		}

		 TiXmlElement * pCertain02Node = pDrawNode->FirstChildElement("certain02");
		 if (pCertain02Node){
		 	parseSubNode(pCertain02Node,m_diamond150Mod2DrawCfg);
		 }

		TiXmlElement * pCertain03Node = pDrawNode->FirstChildElement("certain03");
		if (pCertain03Node){
			parseSubNode(pCertain03Node,m_diamond150Mod32DrawCfg);
		}

		TiXmlElement * pCertain04Node = pDrawNode->FirstChildElement("certain04");
		if (pCertain04Node){
			parseSubNode(pCertain04Node,m_diamond150Mod62DrawCfg);
		}

		TiXmlElement * pCertain05Node = pDrawNode->FirstChildElement("certain05");
		if (pCertain05Node){
			parseSubNode(pCertain05Node,m_diamond150Mod92DrawCfg);	
		}

		TiXmlElement * pCertain06Node = pDrawNode->FirstChildElement("certain06");
		if (pCertain06Node){
			parseSubNode(pCertain06Node,m_diamond150Mod122DrawCfg);
		}

		TiXmlElement * pCertain07Node = pDrawNode->FirstChildElement("certain07");
		if (pCertain07Node){
			parseSubNode(pCertain07Node,m_diamond30Mod12DrawCfg);
		}
	}
	
	INT32 iVal = 0;
	TiXmlElement *pLimitTimeDrawNode = pRoot->FirstChildElement("limitTimeDraw");
	if (pLimitTimeDrawNode){
		TiXmlElement * pLimitTimeDrawIDNode	= pLimitTimeDrawNode->FirstChildElement("limitTimeDrawID");
		while (pLimitTimeDrawIDNode){
			stLimitPetDrawInfo limitPetDrawInfo;
			UINT16 drawID = 0;
			if (pLimitTimeDrawIDNode->Attribute("ID",&iVal)){
				drawID = iVal;
			}
			TiXmlElement * pDiamonddraw01Node = pLimitTimeDrawIDNode->FirstChildElement("Diamonddraw01");
			if (pDiamonddraw01Node){
				parseSubNode(pDiamonddraw01Node,limitPetDrawInfo.m_drawHeroCfg);	
			}

			TiXmlElement * pDiamonddraw10Node = pLimitTimeDrawIDNode->FirstChildElement("Diamonddraw10");
			if (pDiamonddraw10Node){
				parseSubNode(pDiamonddraw10Node,limitPetDrawInfo.m_10DrawHeroCfg);	
			}

			TiXmlElement * pCertain01Node = pLimitTimeDrawIDNode->FirstChildElement("certain01");
			if (pCertain01Node){
				parseSubNode(pCertain01Node,limitPetDrawInfo.m_drawHero10Mod10DrawCfg);	
			}

			TiXmlElement * pCertain02Node = pLimitTimeDrawIDNode->FirstChildElement("certain02");
			if (pCertain02Node){
				parseSubNode(pCertain02Node,limitPetDrawInfo.m_drawHero4Mod3DrawCfg);	
			}
			m_limitPetDrawInfoCnt[drawID] = limitPetDrawInfo;
			pLimitTimeDrawIDNode = pLimitTimeDrawIDNode->NextSiblingElement("limitTimeDrawID");
		}
	}
	
	TiXmlElement * pLimitTimeTreasureNode = pRoot->FirstChildElement("limitTimeTreasure");
	if (pLimitTimeTreasureNode){
		TiXmlElement *  pLimitTimeTreasureIDNode = pLimitTimeTreasureNode->FirstChildElement("limitTimeTreasureID");
		if (pLimitTimeTreasureIDNode){
			TiXmlElement * pDiamonddraw01Node = pLimitTimeTreasureIDNode->FirstChildElement("Diamonddraw01");
			if (pDiamonddraw01Node){
				parseSubNode(pDiamonddraw01Node,m_treasureDrawCfg);		
			}

			TiXmlElement * pDiamonddraw10Node = pLimitTimeTreasureIDNode->FirstChildElement("Diamonddraw10");
			if (pDiamonddraw10Node){
				parseSubNode(pDiamonddraw10Node,m_treasure10DrawCfg);
			}

			TiXmlElement * pCertain01Node = pLimitTimeTreasureIDNode->FirstChildElement("certain01");
			if (pCertain01Node){
				parseSubNode(pCertain01Node,m_treasure10Mod10DrawCfg);	
			}

			TiXmlElement * pCertain02Node = pLimitTimeTreasureIDNode->FirstChildElement("certain02");
			if (pCertain02Node){
				parseSubNode(pCertain02Node,m_treasure3Mod3DrawCfg);
			}
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载配置抽奖配置信息:");
	Athena::logger->trace("金币抽奖需要金钱:%d",m_gold1DrawCfg.m_needMoney);	
	for (UINT16 i = 0;i < m_gold1DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_gold1DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);			
	}

	Athena::logger->trace("金币10连抽需要金钱:%d",m_gold10DrawCfg.m_needMoney);	
	for (UINT16 i = 0;i < m_gold10DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_gold10DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);			
	}
	
	Athena::logger->trace("首次金币10连抽:");
	for (UINT16 i = 0;i < m_first10GoldDrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_first10GoldDrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);			
	}

	Athena::logger->trace("钻石抽奖需要金钱:%d",m_diamond1DrawCfg.m_needMoney);	
	for (UINT16 i = 0;i < m_diamond1DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond1DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);			
	}

	Athena::logger->trace("钻石十连抽需要金钱:%d",m_diamond10DrawCfg.m_needMoney);	
	for (UINT16 i = 0;i < m_diamond10DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond10DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);			
	}

	Athena::logger->trace("首次钻石抽奖可获得道具:");
	for (UINT16 i = 0;i < m_firstDiamondDrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_firstDiamondDrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}	

	Athena::logger->trace("首次钻石10连抽可获得的道具:");
	for (UINT16 i = 0;i < m_first10DiamondDrawCfg.m_items.size();++i){
		stDrawItem & item = m_first10DiamondDrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}

	Athena::logger->trace("第二次钻石10连抽获得道具:");
	for (UINT16 i = 0;i < m_second10DiamondDrawCfg.m_items.size() ; ++i){
		stDrawItem & item = m_second10DiamondDrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}	

	Athena::logger->trace("抽奖次数是10的倍数时候获得道具:");
	for (UINT16 i = 0;i < m_diamond10Mod10DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond10Mod10DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}
	
	Athena::logger->trace("抽奖次数是150余2时候获得道具:");
	for (UINT16 i = 0;i < m_diamond150Mod2DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond150Mod2DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}

	Athena::logger->trace("抽奖次数是150余32时候获得道具:");
	for (UINT16 i = 0;i < m_diamond150Mod32DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond150Mod32DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}
	
	Athena::logger->trace("抽奖次数是150余62时候获得道具:");
	for (UINT16 i = 0;i < m_diamond150Mod62DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond150Mod62DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}

	Athena::logger->trace("抽奖次数是150余92时候获得道具:");
	for (UINT16 i = 0;i < m_diamond150Mod92DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond150Mod92DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}

	Athena::logger->trace("抽奖次数是150余122时候获得道具:");
	for (UINT16 i = 0;i < m_diamond150Mod122DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond150Mod122DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}
	
	Athena::logger->trace("抽奖次数是30余12时候获得道具:");
	for (UINT16 i = 0;i < m_diamond30Mod12DrawCfg.m_items.size(); ++i){
		stDrawItem & item = m_diamond30Mod12DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}
	std::map<UINT16,stLimitPetDrawInfo>::iterator it = m_limitPetDrawInfoCnt.begin();
	for (;it != m_limitPetDrawInfoCnt.end();++it){
		stLimitPetDrawInfo & petDrawInfo = it->second;
		Athena::logger->trace("武将抽奖获得道具是:");
		for (UINT16 i = 0;i < petDrawInfo.m_drawHeroCfg.m_items.size(); ++i){
			stDrawItem & item = petDrawInfo.m_drawHeroCfg.m_items[i];
			Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
		}

		Athena::logger->trace("interval:%u,needMoney=%d 10连抽武将抽奖获得道具是:",petDrawInfo.m_10DrawHeroCfg.m_interval,petDrawInfo.m_10DrawHeroCfg.m_needMoney);
		for (UINT16 i = 0;i < petDrawInfo.m_10DrawHeroCfg.m_items.size(); ++i){
			stDrawItem & item = petDrawInfo.m_10DrawHeroCfg.m_items[i];
			Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
		}

		Athena::logger->trace("武将抽奖 10倍数时候可获得道具:");
		for (UINT16 i = 0;i < petDrawInfo.m_drawHero10Mod10DrawCfg.m_items.size();++i){
			stDrawItem & item = petDrawInfo.m_drawHero10Mod10DrawCfg.m_items[i];
			Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
		}

		Athena::logger->trace("武将抽奖 4倍数时候可获得道具:");
		for (UINT16 i = 0;i < petDrawInfo.m_drawHero4Mod3DrawCfg.m_items.size();++i){
			stDrawItem & item = petDrawInfo.m_drawHero4Mod3DrawCfg.m_items[i];
			Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
		}
	}

	Athena::logger->trace("宝藏抽奖可获得道具:");
	for (UINT16 i = 0;i < m_treasureDrawCfg.m_items.size();++i){
		stDrawItem & item = m_treasureDrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}

	Athena::logger->trace("宝藏10连抽获得道具:");
	for (UINT16 i = 0;i < m_treasure10DrawCfg.m_items.size();++i){
		stDrawItem & item = m_treasure10DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}

	Athena::logger->trace("宝藏抽奖次数是10倍数时候获得道具:");
	for (UINT16 i = 0;i < m_treasure10Mod10DrawCfg.m_items.size();++i){
		stDrawItem & item = m_treasure10Mod10DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}

	Athena::logger->trace("宝藏抽奖次数3倍数时候获得道具:");
	for (UINT16 i = 0;i < m_treasure3Mod3DrawCfg.m_items.size();++i){
		stDrawItem & item = m_treasure3Mod3DrawCfg.m_items[i];
		Athena::logger->trace("可获得道具id=%d,num=%d,rate=%d",item.m_id,item.m_num,item.m_rate);
	}	
#endif
	return true;
}

UINT32 CDrawMgr::save(byte * buf,UINT32 needSpace)
{
	UINT32 ret = 0;
	*(UINT32*)(buf + ret) = m_lastFreeGoldDrawTime;
	ret += sizeof(UINT32);

	*(UINT16 *)(buf + ret) = m_freeGoldDrawNum;
	ret += sizeof(UINT16);

	*(UINT32 *)(buf + ret) = m_lastFreeDiamondDrawTime;
	ret += sizeof(UINT32);

	*(UINT32 *)(buf + ret) = m_lastFreeDrawHeroTime;
	ret += sizeof(UINT32);

	*(UINT32 *)(buf + ret) = m_diamondDrawNum;
	ret += sizeof(UINT32);

	*(UINT32 *)(buf + ret) = m_heroDrawNum;
	ret += sizeof(UINT32);

	*(UINT16 *)(buf + ret) = m_flag;
	ret += sizeof(UINT16);

	return ret;
}

UINT32 CDrawMgr::load(byte * buf)
{
	UINT32 ret = 0;
	m_lastFreeGoldDrawTime = *(UINT32*)(buf + ret);
	ret += sizeof(UINT32);

	m_freeGoldDrawNum = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);

	m_lastFreeDiamondDrawTime = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	m_lastFreeDrawHeroTime = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	m_diamondDrawNum = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	m_heroDrawNum = *(UINT32 *)(buf + ret);
	ret += sizeof(UINT32);

	m_flag = *(UINT16 *)(buf + ret);
	ret += sizeof(UINT16);

	return ret;
}

void CDrawMgr::checkRefresh()
{
	if (!g_isTheSameDay(m_lastFreeGoldDrawTime,g_gameServer->getSec())){
		m_lastFreeGoldDrawTime = g_gameServer->getSec();
		m_freeGoldDrawNum = 0;
	}

	if (0 == m_lastFreeDiamondDrawTime){
		m_lastFreeDiamondDrawTime = g_gameServer->getSec() - 3600 * 47 - 1800;
	}

	if (0 == m_lastFreeDrawHeroTime){
		m_lastFreeDrawHeroTime = g_gameServer->getSec();
	}

	if (0 == m_lastFreeDrawTreasureTime){
		m_lastFreeDrawTreasureTime = g_gameServer->getSec();
	}
}

void CDrawMgr::openDrawWindow()
{
	checkRefresh();
	
	MiscProto::stDrawWindowInfo retCmd;
	UINT32 remainTime = g_gameServer->getSec() > m_lastFreeDiamondDrawTime + m_diamond1DrawCfg.m_interval?0 :m_lastFreeDiamondDrawTime + m_diamond1DrawCfg.m_interval -  g_gameServer->getSec();

	retCmd.set_diamondfreeremaintime(remainTime);
	remainTime = g_gameServer->getSec() > m_lastFreeGoldDrawTime  + m_gold1DrawCfg.m_interval? 0 :m_lastFreeGoldDrawTime  + m_gold1DrawCfg.m_interval - g_gameServer->getSec();
	retCmd.set_goldfreeremaintime(remainTime);

	UINT32 remainNum = s_FREE_GOLD_DRAW_MAX_NUM > m_freeGoldDrawNum ? s_FREE_GOLD_DRAW_MAX_NUM - m_freeGoldDrawNum : 0;
	retCmd.set_goldfreeremainnum(remainNum);
	
	stLimitPetDrawInfo * pPetDrawInfo = getLimitPetDrawInfo();
	if (pPetDrawInfo){
		remainTime = g_gameServer->getSec() > m_lastFreeDrawHeroTime + pPetDrawInfo->m_drawHeroCfg.m_interval ? 0 : m_lastFreeDrawHeroTime + pPetDrawInfo->m_drawHeroCfg.m_interval - g_gameServer->getSec();
		retCmd.set_drawherofreeremaintime(remainTime);
	}

	retCmd.set_diamonddrawnum(m_diamondDrawNum);
	retCmd.set_herodrawnum(m_heroDrawNum);

	if (isSetFlag(eFlag_First_Gold_Draw)){
		retCmd.set_hasgolddraw(true);
	}
	else {
		retCmd.set_hasgolddraw(false);
	}

	if (isSetFlag(eFlag_First_10Gold_Draw)){
		retCmd.set_hasgold10draw(true);
	}
	else {
		retCmd.set_hasgold10draw(false);
	}

	if (isSetFlag(eFlag_First_Diamond_Draw)){
		retCmd.set_hasdiamonddraw(true);
	}
	else {
		retCmd.set_hasdiamonddraw(false);
	}

	if (isSetFlag(eFlag_First_10_Diamond_Draw)){
		retCmd.set_hasdiamond10draw(true);
	}
	else {
		retCmd.set_hasdiamond10draw(false);
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("剩余%d次免费金币抽奖",remainNum);
#endif
	m_pOwner->sendProto2Me(retCmd);
}

bool CDrawMgr::checkBagSpace(MiscProto::eDrawType type)
{
	UINT32 needSpace = 0;
	switch (type){
		case MiscProto::eDrawType_Gold :
			{
				 needSpace = 2;
			}
			break;
		case MiscProto::eDrawType_10Gold :
			{
				needSpace = 11;
			}
			break;
		case MiscProto::eDrawType_Diamond :
			{
				needSpace = 2;
			}
			break;
		case MiscProto::eDrawType_10Diamond :
			{
				needSpace = 11;
			}
			break;
		case MiscProto::eDrawType_DrawHero :
			{
				needSpace = 2;
			}
			break;
		case MiscProto::eDrawType_10DrawHero :
			{
				needSpace = 11;
			}
			break;
		case MiscProto::eDrawType_TreasureDraw :
			{
				needSpace = 2;
			}
			break;
		case MiscProto::eDrawType_10TreasureDraw :
			{
				needSpace = 11;
			}
			break;
		case MiscProto::eDrawType_DevilCallDraw :
            {
                break;
            }
	}

	if (m_pOwner->m_bagMgr.getFreeSlotNum(Role::BAG_TYPE_MAIN1) >= needSpace){
		return true;	
	}
	return false;
}

bool CDrawMgr::canFreeDraw(MiscProto::eDrawType type)
{
	switch (type){
		case MiscProto::eDrawType_Gold :
			{
				if (!isSetFlag(eFlag_First_Gold_Draw)){
					return true;
				}

				if ((m_lastFreeGoldDrawTime + m_gold1DrawCfg.m_interval < g_gameServer->getSec())
						&& (m_freeGoldDrawNum < s_FREE_GOLD_DRAW_MAX_NUM)){//免费抽奖
					m_lastFreeGoldDrawTime = g_gameServer->getSec();
					m_freeGoldDrawNum += 1;
					return true;	
				}
			}
			break;
		case MiscProto::eDrawType_10Gold :
			{
				if (!isSetFlag(eFlag_First_10Gold_Draw)){
					return true;
				}
				return false;
			}
			break;
		case MiscProto::eDrawType_Diamond :
			{
				if (m_lastFreeDiamondDrawTime + m_diamond1DrawCfg.m_interval < g_gameServer->getSec()){
					m_lastFreeDiamondDrawTime = g_gameServer->getSec();
					return true;
				}
			}
			break;
		case MiscProto::eDrawType_10Diamond :
			{
				return false;
			}
			break;
		case MiscProto::eDrawType_DrawHero :
			{
				stLimitPetDrawInfo * pPetDrawInfo = getLimitPetDrawInfo();
				if (!pPetDrawInfo){
					return false;
				}
				if (m_lastFreeDrawHeroTime + pPetDrawInfo->m_drawHeroCfg.m_interval < g_gameServer->getSec()){
					m_lastFreeDrawHeroTime = g_gameServer->getSec();		
					return true;	
				}
			}
			break;
		case MiscProto::eDrawType_10DrawHero :
			{
				return false;
			}
			break;
		case MiscProto::eDrawType_TreasureDraw :
			{
				if (m_lastFreeDrawTreasureTime + m_treasureDrawCfg.m_interval < g_gameServer->getSec()){
					m_lastFreeDrawTreasureTime = g_gameServer->getSec();
					return true;
				}	
			}
			break;
		case MiscProto::eDrawType_10TreasureDraw :
			{
				return false;
			}
			break;
		case MiscProto::eDrawType_DevilCallDraw :
            {
                break;
            }
	}
	return false;

}

bool CDrawMgr::reduceCost(MiscProto::eDrawType type)
{
	switch (type){
		case MiscProto::eDrawType_Gold :
			{
				/*if ((m_lastFreeGoldDrawTime + m_gold1DrawCfg.m_interval < g_gameServer->getSec())
						&& (m_freeGoldDrawNum < s_FREE_GOLD_DRAW_MAX_NUM)){//免费抽奖
					m_lastFreeGoldDrawTime = g_gameServer->getSec();
					m_freeGoldDrawNum += 1;
					return true;	
				}*/

				if (!m_pOwner->removeAssert(m_gold1DrawCfg.m_moneyType,m_gold1DrawCfg.m_needMoney,0,"金币抽奖")){
					return false;
				}
				return true;
			}
			break;
		case MiscProto::eDrawType_10Gold :
			{
				
				if (!m_pOwner->removeAssert(m_gold10DrawCfg.m_moneyType,m_gold10DrawCfg.m_needMoney,0,"金币10连抽")){
					return false;
				}
				return true;
			}
			break;
		case MiscProto::eDrawType_Diamond :
			{
				/*if (m_lastFreeDiamondDrawTime + m_diamond1DrawCfg.m_interval < g_gameServer->getSec()){
					m_lastFreeDiamondDrawTime = g_gameServer->getSec();
					return true;
				}*/

				if (m_pOwner->removeAssert(s_ONE_DRAW_CARD_ITEM_ID,1,0,"钻石抽奖")){
					return true;
				}

				if (!m_pOwner->removeAssert(m_diamond1DrawCfg.m_moneyType,m_diamond1DrawCfg.m_needMoney,0,"钻石抽奖")){
					return false;
				}	
				return true;
			}
			break;
		case MiscProto::eDrawType_10Diamond :
			{
				if (m_pOwner->removeAssert(s_DRAW_CARD_ITEM_ID,1,0,"钻石10连抽奖")){
					return true;
				}

				if (!m_pOwner->removeAssert(m_diamond10DrawCfg.m_moneyType,m_diamond10DrawCfg.m_needMoney,0,"钻石10连抽奖")){
					return false;
				}	
				return true;
			}
			break;
		case MiscProto::eDrawType_DrawHero :
			{
				/*if (m_lastFreeDrawHeroTime + m_drawHeroCfg.m_interval < g_gameServer->getSec()){
					m_lastFreeDrawHeroTime = g_gameServer->getSec();		
					return true;	
				}*/
				stLimitPetDrawInfo * pPetDrawInfo = getLimitPetDrawInfo();
				if (!pPetDrawInfo){
					return false;
				}

				if (!m_pOwner->removeAssert(pPetDrawInfo->m_drawHeroCfg.m_moneyType,pPetDrawInfo->m_drawHeroCfg.m_needMoney,0,"武将抽取")){
					return false;
				}
				return true;
			}
			break;
		case MiscProto::eDrawType_10DrawHero :
			{
				stLimitPetDrawInfo * pPetDrawInfo = getLimitPetDrawInfo();
				if (!pPetDrawInfo){
					return false;
				}

				if (!m_pOwner->removeAssert(pPetDrawInfo->m_10DrawHeroCfg.m_moneyType,pPetDrawInfo->m_10DrawHeroCfg.m_needMoney,0,"武将10抽奖")){
					return false;
				}	
				return true;
			}
			break;
		case MiscProto::eDrawType_TreasureDraw :
			{
				/*if (m_lastFreeDrawTreasureTime + m_treasureDrawCfg.m_interval < g_gameServer->getSec()){
					m_lastFreeDrawTreasureTime = g_gameServer->getSec();
					return true;
				}*/

				if (!m_pOwner->removeAssert(m_treasureDrawCfg.m_moneyType,m_treasureDrawCfg.m_needMoney,0,"宝藏抽取")){
					return false;
				}
				return true;
			}
			break;
		case MiscProto::eDrawType_10TreasureDraw :
			{
				if (!m_pOwner->removeAssert(m_treasure10DrawCfg.m_moneyType,m_treasure10DrawCfg.m_needMoney,0,"宝藏抽奖")){
					return false;
				}	
				return true;

			}
			break;
		case MiscProto::eDrawType_DevilCallDraw :
            {
                break;
            }
	}
	return false;
}

void CDrawMgr::giveDrawItem(std::vector<stDrawItem> & fromVec,std::vector<std::pair<UINT32,UINT32> > & retCnt,UINT8 drawNum)
{
	std::map<UINT32,UINT32> itemID2Odds;		//道具ID于概率
	//std::map<UINT32,UINT32> itemID2Num;			//道具ID于数量
	UINT32 sumVal = 0;

	for (UINT16 i = 0;i < fromVec.size(); ++i){
		stDrawItem & item = fromVec[i];
		itemID2Odds[i] = item.m_rate;
		//itemID2Num[i] = item.m_num;
		sumVal += item.m_rate;
	}

	for (UINT8 i = 0;i < drawNum ; ++i){
		INT32 index = g_selectByMap(itemID2Odds,sumVal);			
		if ((-1 != index) && (index < (INT32)fromVec.size())){//抽出了一个道具
			stDrawItem & item = fromVec[index];
			retCnt.push_back(std::make_pair(item.m_id,item.m_num));
			sumVal -= item.m_rate;
			itemID2Odds.erase(i);
		}
	}	
}

void CDrawMgr::setFlag(eFlag flag)
{
	m_flag |= flag;
}

bool CDrawMgr::isSetFlag(eFlag flag)
{
	if (m_flag & flag){
		return true;
	}
	else {
		return false;
	}
}

CDrawMgr::stDrawInfo * CDrawMgr::getDrawInfoByDiamondDrawNum(bool bTen)
{
	if ((m_diamondDrawNum > 0)&&((m_diamondDrawNum % 5) == 0)){//10的余数
		return &m_diamond10Mod10DrawCfg;	
	}	
	else if ((m_diamondDrawNum % 150) ==  22){//150 于2
		return &m_diamond150Mod2DrawCfg;
	}
	else if ((m_diamondDrawNum % 150) == 52){
		return &m_diamond150Mod32DrawCfg;
	}	
	else if ((m_diamondDrawNum % 150) == 82){
		return &m_diamond150Mod62DrawCfg;
	}
	else if ((m_diamondDrawNum % 150) == 112){
		return &m_diamond150Mod92DrawCfg; 
	}
	else if ((m_diamondDrawNum % 150) == 142){
		return &m_diamond150Mod122DrawCfg;
	}
	else if ((m_diamondDrawNum % 30) == 32){
		return &m_diamond30Mod12DrawCfg;
	}
	else {
		if (bTen){
			return &m_diamond10DrawCfg;
		}
		else {
			return &m_diamond1DrawCfg;
		}
	}
}

CDrawMgr::stDrawInfo * CDrawMgr::getDrawInfoByHeroDrawNum(bool bTen)
{
	stLimitPetDrawInfo * pPetDrawInfo = getLimitPetDrawInfo();
	if (!pPetDrawInfo){
		return NULL;
	}

	if ((m_heroDrawNum % 10) == 0){
		return &pPetDrawInfo->m_drawHero10Mod10DrawCfg;	
	}	
	else if ((m_heroDrawNum % 4) == 3){
		return &pPetDrawInfo->m_drawHero4Mod3DrawCfg;	
	}
	else {
		if (bTen){
			return &pPetDrawInfo->m_10DrawHeroCfg;	
		}
		else {
			return &pPetDrawInfo->m_drawHeroCfg;
		}
	}
}

CDrawMgr::stDrawInfo * CDrawMgr::getDrawInfoByTreasureDrawNum(bool bTen)
{
	if ((m_heroDrawNum % 10) == 0){
		return &m_treasure10Mod10DrawCfg;	
	}	
	else if ((m_heroDrawNum % 3) == 3){
		return &m_treasure3Mod3DrawCfg;	
	}
	else {
		if (bTen){
			return &m_treasure10DrawCfg;	
		}
		else {
			return &m_treasureDrawCfg;
		}
	}

}

void CDrawMgr::draw(MiscProto::eDrawType type)
{
	//UINT32 limitTimeDrawID = 0;
	if ((MiscProto::eDrawType_10DrawHero == type) || (MiscProto::eDrawType_DrawHero == type)){
		LimitTimePetCfg::stActivityOpenInfo * pActOpenInfo = LimitTimePetCfg::getSingleton().getOpenInfo(g_gameServer->getSec());
		if (!pActOpenInfo){//限时武神没有开启却抽奖
			return ;	
		}

		/*LimitTimePetCfg::stActivity * pActivity = LimitTimePetCfg::getSingleton().getActivity(pActOpenInfo->m_activityID);
		if (!pActivity){
			Athena::logger->error("武神抽奖,找不到相关的抽奖配置");
			return ;	
		}
		limitTimeDrawID = pActivity->m_drawID;*/
	}

	MiscProto::stDrawResult retCmd;
	retCmd.set_type(type);

	if (!checkBagSpace(type)){
		retCmd.set_result(MiscProto::stDrawResult::BAG_FULL);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	
	bool bFree = canFreeDraw(type);

	if ((!bFree)&&(!reduceCost(type))){//费用不足
		if ((type == MiscProto::eDrawType_Gold) || (type == MiscProto::eDrawType_10Gold)){
			retCmd.set_result(MiscProto::stDrawResult::MONEY_NOT_ENOUGH);	
		}
		else {
			retCmd.set_result(MiscProto::stDrawResult::DIAMOND_NOT_ENOUGH);
		}
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	
	std::vector<std::pair<UINT32,UINT32> > ret;
	UINT8 addDrawNum = 0;
	UINT32 heroDrawScore = 0;
	UINT8 diamondDrawNum = 0;

	switch (type){
		case MiscProto::eDrawType_Gold :
			{
				if (!isSetFlag(eFlag_First_Gold_Draw)){
					giveDrawItem(m_firstGoldDrawCfg.m_items,ret,1);	
					setFlag(eFlag_First_Gold_Draw);
				}
				else {
					giveDrawItem(m_gold1DrawCfg.m_items,ret,1);	
				}
				addDrawNum = 1;

				Global::stReqCheckTask cmd;
				cmd.set_charid(m_pOwner->getID());
				cmd.set_squaretype(6);
				cmd.set_battletype(1);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			break;
		case MiscProto::eDrawType_10Gold :
			{
				if (!isSetFlag(eFlag_First_10Gold_Draw)){
					setFlag(eFlag_First_10Gold_Draw);
					giveDrawItem(m_first10GoldDrawCfg.m_items,ret,1);
					giveDrawItem(m_gold10DrawCfg.m_items,ret,9);
				}
				else {
					giveDrawItem(m_gold10DrawCfg.m_items,ret,10);	
				}
				addDrawNum = 10;

				Global::stReqCheckTask cmd;
				cmd.set_charid(m_pOwner->getID());
				cmd.set_squaretype(6);
				cmd.set_battletype(1);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd); 
			}
			break;
		case MiscProto::eDrawType_Diamond :
			{
				if (!isSetFlag(eFlag_First_Free_Diamond_Draw) && bFree){//没有免费抽奖过
					setFlag(eFlag_First_Free_Diamond_Draw);
					giveDrawItem(m_firstFreeDiamondDrawCfg.m_items,ret,1);
				}
				else if (!isSetFlag(eFlag_First_Diamond_Draw) && (!bFree)){//没有钻石抽奖过
					setFlag(eFlag_First_Diamond_Draw);
					giveDrawItem(m_firstDiamondDrawCfg.m_items,ret,1);
				}	
				else {
					m_diamondDrawNum += 1;	
					stDrawInfo * pDrawInfo = getDrawInfoByDiamondDrawNum(false);
					if (pDrawInfo) {
						giveDrawItem(pDrawInfo->m_items,ret,1);	
					}
				}
				addDrawNum = 1;
				diamondDrawNum = 1;
                Global::stOperateActivityCb globalCmd;
                globalCmd.set_charid(m_pOwner->getID());
                globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Draw_Box));
                globalCmd.set_num(1);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);

				Global::stReqCheckTask cmd;
				cmd.set_charid(m_pOwner->getID());
				cmd.set_squaretype(6);
				cmd.set_battletype(1);
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd);
			}
			break;
		case MiscProto::eDrawType_10Diamond :
			{
				bool calcNum = true;
				bool firstDraw = false;
				bool secondDraw = false;
				if (!isSetFlag(eFlag_First_10_Diamond_Draw)){//是否首次是10连抽
					setFlag(eFlag_First_10_Diamond_Draw);	
					calcNum = true;
					firstDraw = true;
				}
				else if (!isSetFlag(eFlag_Second_10_Diamond_Draw)){
					setFlag(eFlag_Second_10_Diamond_Draw);
					calcNum = true;
					secondDraw = true;
				}
				else {
				}
				
				for (UINT8 i = 0;i < 10;++i){
					if (calcNum){//计算次数
						m_diamondDrawNum += 1;
					}
					else {
						//calcNum = true;
					}

					stDrawInfo * pDrawInfo = NULL;
					if (firstDraw) {
						pDrawInfo = &m_first10DiamondDrawCfg;
						firstDraw = false;
					}
					else if (secondDraw){
						pDrawInfo = &m_second10DiamondDrawCfg; 
						secondDraw = false;
					}
					else {
						pDrawInfo = getDrawInfoByDiamondDrawNum(true);
					}

					if (pDrawInfo){
						giveDrawItem(pDrawInfo->m_items,ret,1);
					}
					else {
						Athena::logger->error("抽奖时候找不到抽奖信息!");
					}
				}
				addDrawNum = 10;
				diamondDrawNum = 10;
                Global::stOperateActivityCb globalCmd;
                globalCmd.set_charid(m_pOwner->getID());
                globalCmd.set_activityid(COperateActivityCfg::getSingleton().findIdByType(COperateActivityCfg::Draw_Box));
                globalCmd.set_num(10);
                GlobalSvrClientMgr::getSingleton().broadcastProto2All(globalCmd);

				Global::stReqCheckTask cmd;
				cmd.set_charid(m_pOwner->getID());
				cmd.set_squaretype(6);
				cmd.set_battletype(1); 
				GlobalSvrClientMgr::getSingleton().broadcastProto2All(cmd); 
			}
			break;
		case MiscProto::eDrawType_DrawHero :
			{
				++m_heroDrawNum;
				stDrawInfo * pDrawInfo = getDrawInfoByHeroDrawNum(false);
				if (pDrawInfo){
					if (bFree){
						heroDrawScore = 100;
					}
					else {
						stLimitPetDrawInfo  * pLimitPetDrawInfo = getLimitPetDrawInfo();
						if (pLimitPetDrawInfo){
							heroDrawScore =  pLimitPetDrawInfo->m_drawHeroCfg.m_needMoney;
						}
					}
					giveDrawItem(pDrawInfo->m_items,ret,1);
				}
			}
			break;
		case MiscProto::eDrawType_10DrawHero :
			{
				for (UINT8 i = 0;i < 10;++i){
					++m_heroDrawNum;
					stDrawInfo * pDrawInfo = getDrawInfoByHeroDrawNum(true);
					if (pDrawInfo){
						stLimitPetDrawInfo  * pLimitPetDrawInfo = getLimitPetDrawInfo();
						if (pLimitPetDrawInfo){
							heroDrawScore =  pLimitPetDrawInfo->m_10DrawHeroCfg.m_needMoney;
						}
						giveDrawItem(pDrawInfo->m_items,ret,1);	
					}
				}
			}
			break;
		case MiscProto::eDrawType_TreasureDraw :
			{
				++m_treasureDrawNum;
				stDrawInfo * pDrawInfo = getDrawInfoByTreasureDrawNum(false);
				if (pDrawInfo){
					giveDrawItem(pDrawInfo->m_items,ret,1);
				}
			}
			break;
		case MiscProto::eDrawType_10TreasureDraw :
			{
				for (UINT8 i = 0;i < 10; ++i){
					++m_treasureDrawNum;
					stDrawInfo * pDrawInfo = getDrawInfoByTreasureDrawNum(true);
					if (pDrawInfo){
						giveDrawItem(pDrawInfo->m_items,ret,1);
					}
				}		
			}
			break;
		case MiscProto::eDrawType_DevilCallDraw :
            {
                break;
            }
	}
	
	if (heroDrawScore > 0){
		Global::stSynDrawHeroScore drawHeroCmd;
		drawHeroCmd.set_charid(m_pOwner->getID());
		drawHeroCmd.set_score(heroDrawScore);
		GlobalSvrClientMgr::getSingleton().broadcastProto2All(drawHeroCmd);
	}

	retCmd.set_result(MiscProto::stDrawResult::SUCCESS);
	std::vector<std::pair<UINT32,UINT32> >::iterator it = ret.begin();

	for (;it != ret.end(); ++it){
		m_pOwner->giveAssert(it->first,it->second,0,"宝藏功能");
		ChatMgr::sendObtainItemNotify(m_pOwner,"system_notice_1",it->first,it->second);
		MiscProto::stSynDrawItem * pSynDrawItem = retCmd.add_items();
		if (pSynDrawItem){
			UINT32 tempid = CItem::realItemIDByJobAndSex(it->first,m_pOwner->getJob(),m_pOwner->getSex());

			UINT32 tCurrentTime = g_gameServer->getSec();
			UINT32 tOpenServerTime = (UINT32)atoi(Athena::global["OpenServer_Time"].c_str());
			if (tOpenServerTime > tCurrentTime) {//如果开服时间大于今天 
				Athena::logger->trace("战令活动时间未开启，活动不统计！");
				return;
			} else {
				if (type == MiscProto::eDrawType_Diamond || type == MiscProto::eDrawType_10Diamond) {
					int digit3 = (tempid / 1000) % 10; // 第 3 位
					int digit4 = (tempid / 100) % 10; // 第 4 位
					int fnnum = (digit3 * 10) + digit4;

					if (fnnum >= 22)
					{		
						m_pOwner->m_buySummonNum += 1;
					}
				}
				else if (type == MiscProto::eDrawType_DrawHero || type == MiscProto::eDrawType_10DrawHero) {
					if (tempid == 811606 || tempid == 811607 || tempid == 811608 || tempid == 811609 || 
					tempid == 811614 || tempid == 811615 || tempid == 811617 || tempid == 811618 ||
					tempid == 811621 || tempid == 811623 || tempid == 811624 || tempid == 811625) {
						m_pOwner->m_buyLimitSummonNum += 1;
					}
				}
			}



			pSynDrawItem->set_itemid(tempid);
			pSynDrawItem->set_num(it->second);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("name=%s,id=%d通过抽奖获得了itemID=%d,num=%d",m_pOwner->getName(),m_pOwner->getID(),pSynDrawItem->itemid(),pSynDrawItem->num());
#endif
		}
	}

	retCmd.set_diamonddrawnum(m_diamondDrawNum);
	m_pOwner->sendProto2Me(retCmd);
	stDrawNum drawNumNotify;
	drawNumNotify.notify(m_pOwner,addDrawNum);
	m_pOwner->m_countMgr.addDayRefreshValue(ECOUNT_DRAW_NUM,addDrawNum);
	m_pOwner->m_sevenDayActivityMgr.addDrawNum(diamondDrawNum);
	//m_pOwner->m_taskMgr.onPropChange(TaskProto::ET_DRAW_NUM);
}

CDrawMgr::stLimitPetDrawInfo  * CDrawMgr::getLimitPetDrawInfo()
{
	LimitTimePetCfg::stActivityOpenInfo * pActOpenInfo = LimitTimePetCfg::getSingleton().getOpenInfo(g_gameServer->getSec());
	if (!pActOpenInfo){//限时武神没有开启却抽奖
		return NULL;	
	}

	LimitTimePetCfg::stActivity * pActivity = LimitTimePetCfg::getSingleton().getActivity(pActOpenInfo->m_activityID);
	if (!pActivity){
		Athena::logger->error("武神抽奖,找不到相关的抽奖配置");
		return NULL;	
	}
	
	std::map<UINT16,stLimitPetDrawInfo>::iterator it = m_limitPetDrawInfoCnt.find(pActivity->m_drawID);
	if (it == m_limitPetDrawInfoCnt.end()){
		return NULL;
	}
	return &it->second;
}
