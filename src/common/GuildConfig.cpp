#include "GuildConfig.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool CGuildConfig::loadCfg()
{
	TiXmlDocument doc;
	std::string guildFile = Athena::global["confDir"] + "/GU.xml";
	if (!doc.LoadFile(guildFile.c_str())){
		Athena::logger->error("[公会配置]打开配置文件%s错误",guildFile.c_str());
		return false;
	}  
	
	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[公会配置]读取配置文件找不到节点<root>");
		return false;
	}

	TiXmlElement * pConstantNode = pRoot->FirstChildElement("constant");
	if (!pConstantNode){
		Athena::logger->error("[公会配置]找不到节点<constant>");
		return false;
	}
	
	INT32 iVal = 0;

	TiXmlElement * pCreateNode = pConstantNode->FirstChildElement("establish");
	if (!pCreateNode){
		Athena::logger->error("[公会配置]找不到节点<establish>");
		return false;
	}
	
	TiXmlElement * pCostNode = pCreateNode->FirstChildElement("cost");
	while (pCostNode){
		UINT8 id = 0;
		if (pCostNode->Attribute("id",&iVal)){
			id = iVal;	
		}

		if (1 == id){
			if (pCostNode->Attribute("costResmun",&iVal)){
				m_createInfo.m_needGoldCoin = iVal;		
			}	
		}
		else if (2 == id){
			if (pCostNode->Attribute("costResmun",&iVal)){
				m_createInfo.m_needDiamond = iVal;		
			}	
		}

		pCostNode = pCostNode->NextSiblingElement("cost");
	}

	if (pCreateNode->Attribute("Openlv",&iVal)){
		m_createInfo.m_needLevel = (UINT32)iVal;		
	}
	
#ifdef _HDZ_DEBUG
	Athena::logger->trace("创建公会需要等级=%d,需要金币=%d,钻石=%d",m_createInfo.m_needLevel,m_createInfo.m_needGoldCoin,m_createInfo.m_needDiamond);
#endif

	TiXmlElement * pLobbyNode = pConstantNode->FirstChildElement("Lobby");
	if (pLobbyNode){
		TiXmlElement * pEntryNode = pLobbyNode->FirstChildElement("entry");
		while (pEntryNode){
			stHallInfo hall;
			if (pEntryNode->Attribute("lv",&iVal)){
				hall.m_lv = iVal;
			}
			
			if (pEntryNode->Attribute("construction",&iVal)){
				hall.m_contribute = iVal;
			}
			
			if (pEntryNode->Attribute("upperlimit",&iVal)){
				hall.m_maxMem = iVal;
			}
			
			if (pEntryNode->Attribute("adjutant",&iVal)){
				hall.m_viceLeaderNum = iVal;
			}
			
			if (pEntryNode->Attribute("elite",&iVal)){
				hall.m_eliteNum = iVal;
			}
			m_hallInfos[hall.m_lv] = hall;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[公会配置]大厅相关配置:");
		std::map<UINT16,stHallInfo>::iterator it = m_hallInfos.begin();
		for (;it != m_hallInfos.end(); ++it){
			stHallInfo & hall = it->second;
			Athena::logger->trace("等级ID=%d,contribute=%d,upperlimit=%d,adjutant=%d,elite=%d",hall.m_lv,hall.m_contribute,hall.m_maxMem,hall.m_viceLeaderNum,hall.m_eliteNum);
		}
	}
#endif

	TiXmlElement * pPrayNode = pConstantNode->FirstChildElement("pray");
	if (pPrayNode){
		TiXmlElement * pEntryNode = pPrayNode->FirstChildElement("entry");
		while (pEntryNode){
			stPrayInfo pray;
			if (pEntryNode->Attribute("lv",&iVal)){
				pray.m_lv = iVal;		
			}

			if (pEntryNode->Attribute("construction",&iVal)){
				pray.m_contribute = iVal;
			}
			
			if (pEntryNode->Attribute("num",&iVal)){
				pray.m_maxPrayNum = iVal;
			}
			
			if (pEntryNode->Attribute("prayitem1",&iVal)){
				pray.m_item1ID = iVal;
			}

			if (pEntryNode->Attribute("item1mun",&iVal)){
				pray.m_item1Num = iVal;
			}
			
			if (pEntryNode->Attribute("prayitem2",&iVal)){
				pray.m_item2ID = iVal;
			}

			if (pEntryNode->Attribute("item2mun",&iVal)){
				pray.m_item2Num = iVal;
			}
			
			if (pEntryNode->Attribute("prayitem3",&iVal)){
				pray.m_item3ID = iVal;
			}

			if (pEntryNode->Attribute("item3mun",&iVal)){
				pray.m_item3Num = iVal;
			}
			m_prayInfos[pray.m_lv] = pray;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("祈福配置信息:");
		std::map<UINT16,stPrayInfo>::iterator it = m_prayInfos.begin();
		for (;it != m_prayInfos.end();++it){
			stPrayInfo & pray = it->second;
			Athena::logger->trace("lv=%d,contribute=%d,maxNum=%d,item1=%d,num1=%d,item2=%d,num2=%d,item3=%d,num3=%d",pray.m_lv,pray.m_contribute,pray.m_maxPrayNum,pray.m_item1ID,pray.m_item1Num,pray.m_item2ID,pray.m_item2Num,pray.m_item3ID,pray.m_item3Num);
		}
	}
#endif

	TiXmlElement * pGushopNode = pConstantNode->FirstChildElement("gushop");
	if (pGushopNode){
		TiXmlElement * pEntryNode = pGushopNode->FirstChildElement("entry");
		while (pEntryNode){
			stShopInfo shop;
			if (pEntryNode->Attribute("lv",&iVal)){
				shop.m_lv = iVal;
			}

			if (pEntryNode->Attribute("construction",&iVal)){
				shop.m_contribute = iVal;
			}

			m_shopInfos[shop.m_lv] = shop;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("公会商店:");
		std::map<UINT16,stShopInfo>::iterator it = m_shopInfos.begin();
		for (;it != m_shopInfos.end();++it){
			stShopInfo & shop = it->second;
			Athena::logger->trace("shopLV=%d,contribute=%d",shop.m_lv,shop.m_contribute);
		}
	}
#endif

	TiXmlElement * pDonateNode = pConstantNode->FirstChildElement("donate");
	if (pDonateNode){
		TiXmlElement * pEntryNode = pDonateNode->FirstChildElement("entry");
		while (pEntryNode){
			stDonateInfo donate;
			if (pEntryNode->Attribute("id",&iVal)){
				donate.m_donateType  = iVal;
			}

			if (pEntryNode->Attribute("costResType",&iVal)){
				donate.m_costResType = iVal;
			}
			
			if (pEntryNode->Attribute("costResmun",&iVal)){
				donate.m_costResNum = iVal;	
			}

			if (pEntryNode->Attribute("construction",&iVal)){
				donate.m_addContribute = iVal;
			}
			
			if (pEntryNode->Attribute("donatenum",&iVal)){
				donate.m_donateNum = iVal;
			}
			m_donateInfos[donate.m_donateType] = donate;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("公会捐赠:");
		std::map<UINT16,stDonateInfo>::iterator it = m_donateInfos.begin();	
		for (;it != m_donateInfos.end(); ++it){
			stDonateInfo & donate = it->second;
			Athena::logger->trace("id=%d,costResType=%d,costResmun=%d,construction=%d,donatenum=%d",donate.m_donateType,donate.m_costResType,donate.m_costResNum,donate.m_addContribute,donate.m_donateNum);
		}
	}
#endif

	TiXmlElement * pRightNode = pConstantNode->FirstChildElement("jurisdiction");
	if (pRightNode){
		TiXmlElement * pEntryNode = pRightNode->FirstChildElement("entry");
		while (pEntryNode){
			stRightInfo right;
			if (pEntryNode->Attribute("lv",&iVal)){
				right.m_title  = iVal;
			}
		
			if (pEntryNode->Attribute("manifesto",&iVal)){
				if (iVal){
					right.m_manifesto = true;
				}
			}
			
			if (pEntryNode->Attribute("audit",&iVal)){
				if (iVal){
					right.m_audit = true;
				}
			}

			if (pEntryNode->Attribute("dissolve",&iVal)){
				if (iVal){
					right.m_dissolve = true;
				}
			}

			if (pEntryNode->Attribute("dislodge",&iVal)){
				if (iVal){
					right.m_dislodge = true;
				}
			}
		
			if (pEntryNode->Attribute("announcement",&iVal)){
				if (iVal){
					right.m_notify = true;
				}
			}
			
			if (pEntryNode->Attribute("GUchallengeopen",&iVal)){
				if (iVal){
					right.m_openCopymap = true;
				}
			}
			
			if (pEntryNode->Attribute("GULvup",&iVal)){
				if (iVal){
					right.m_lvUp = true;
				}
			}
			m_rightInfos[right.m_title] = right;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("公会权限:");

		std::map<UINT8,stRightInfo>::iterator it = m_rightInfos.begin();
		for (;it != m_rightInfos.end();++it){
			stRightInfo & right = it->second;
			Athena::logger->trace("lv=%d,manifesto=%s,audit=%s,dissolve=%s,dislodge=%s,announcement=%s",right.m_title,right.m_manifesto ? "true" : "false",right.m_audit ? "true" : "false",right.m_dissolve ? "true" : "false",right.m_dislodge ? "true" : "false",right.m_notify ? "true" : "false");	
		}
	}
#endif
	TiXmlElement * pGUgamecopyNode = pConstantNode->FirstChildElement("GUgamecopy");
	if (pGUgamecopyNode){
		TiXmlElement * pEntryNode = pGUgamecopyNode->FirstChildElement("entry");
		while (pEntryNode){
			UINT16  guildLv = 0;
			UINT32  copymapID = 0;

			if (pEntryNode->Attribute("GUcopylv",&iVal)){
				guildLv = iVal;
			}
			
			if (pEntryNode->Attribute("Openlevel",&iVal)){
				copymapID = iVal;	
			}
			m_copyMapOpenLv[copymapID] = guildLv;
			pEntryNode = pEntryNode->NextSiblingElement("entry");	
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("副本开启等级:");
		std::map<UINT32,UINT8>::iterator it  =   m_copyMapOpenLv.begin();	
		for (;it != m_copyMapOpenLv.end();++it){
			Athena::logger->trace("copymapID:%d,lv:%d",it->first,it->second);
		}
	}
#endif

	TiXmlElement * pGUopencostNode = pConstantNode->FirstChildElement("GUopencost");
	if (pGUopencostNode){
		TiXmlElement * pEntryNode = pGUopencostNode->FirstChildElement("entry");
		while (pEntryNode){
			UINT32 copymapID = 0;
			UINT32 cost = 0;
			
			if (pEntryNode->Attribute("Levelid",&iVal)){
				copymapID = iVal;
			}
			
			if (pEntryNode->Attribute("costactivity",&iVal)){
				cost = iVal;	
			}

			m_openCopyMapCost[copymapID] = cost;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("公会开启副本消耗:");
		std::map<UINT32,UINT32>::iterator it =  m_openCopyMapCost.begin();
		for (;it != m_openCopyMapCost.end();++it){
			Athena::logger->trace("copymapID:%d,cost:%d",it->first,it->second);
		}
	}
#endif

	m_memberActivityLimit  = 0;					//成功活跃度上限
	m_guildActivityLimit = 0;					//公会活跃度上限
	m_challengeNum = 0;							//挑战次数
	m_assistNum = 0;							//协助次数
	
	TiXmlElement * pGUactivityNode = pConstantNode->FirstChildElement("GUactivity");
	if (pGUactivityNode){
		TiXmlElement * pEntryNode = pGUactivityNode->FirstChildElement("entry");
		while (pEntryNode){
			UINT16 id = 0;
			UINT32 value = 0;
			if (pEntryNode->Attribute("id",&iVal)){
				id = iVal;	
			}
			
			if (pEntryNode->Attribute("activitylimit",&iVal)){
				value = iVal;	
			}
			
			if (1 == id){
				m_memberActivityLimit = value;
			}
			else {
				m_guildActivityLimit = value;
			}

			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

	TiXmlElement * pGUchallengeNode = pConstantNode->FirstChildElement("GUchallenge");
	if (pGUchallengeNode){
		TiXmlElement * pEntryNode = pGUchallengeNode->FirstChildElement("entry");
		while (pEntryNode){
			UINT16 id = 0;
			UINT16 value = 0;
			if (pEntryNode->Attribute("id",&iVal)){
				id = iVal;
			}
			
			if (pEntryNode->Attribute("timelimit",&iVal)){
				value = iVal;
			}

			if (1 == id){
				m_challengeNum = value;	
			}
			else {
				m_assistNum = value;	
			}
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("m_memberActivityLimit:%d,m_guildActivityLimit:%d,m_challengeNum:%d,m_assistNum:%d",m_memberActivityLimit,m_guildActivityLimit,m_challengeNum,m_assistNum);
	}
#endif

	TiXmlElement * pGUrankrewardNode = pConstantNode->FirstChildElement("GUrankreward");
	if (pGUrankrewardNode){
		TiXmlElement * pEntryNode = pGUrankrewardNode->FirstChildElement("entry");
		while (pEntryNode){
			UINT16 rank = 0;
			UINT32 boxID = 0;
			
			if (pEntryNode->Attribute("id",&iVal)){
				rank = iVal;
			}
			
			if (pEntryNode->Attribute("rewarditem",&iVal)){
				boxID = iVal;
			}
			m_damageRankReward[rank] = boxID;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("伤害排名奖励:");
	{
		std::map<UINT16,UINT32>::iterator it =  m_damageRankReward.begin();	
		for (;it != m_damageRankReward.end();++it){
			Athena::logger->trace("排名:%d,奖励boxID:%d",it->first,it->second);	
		}
	}
#endif

	TiXmlElement * pGUpointrewardNode = pConstantNode->FirstChildElement("GUpointreward");
	if (pGUpointrewardNode){
		TiXmlElement *pLevelNode = pGUpointrewardNode->FirstChildElement("Level");
		while (pLevelNode){
			UINT32 levelID = 0;
			if (pLevelNode->Attribute("id",&iVal)){
				levelID = iVal;	
			}

			std::map<UINT16,UINT32> rewards;

			TiXmlElement * pEntryNode = pLevelNode->FirstChildElement("entry");
			while (pEntryNode){
				UINT16 rank = 0;
				UINT32 boxID = 0;
				if (pEntryNode->Attribute("id",&iVal)){
					rank = iVal;
				}
				
				if (pEntryNode->Attribute("rewarditem",&iVal)){
					boxID = iVal;
				}
				rewards[rank] = boxID;
				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}
			m_scoreRankReward[levelID] = rewards;
			pLevelNode = pLevelNode->NextSiblingElement("Level");	
		}
	}

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("打印积分排行信息:");
		std::map<UINT32,std::map<UINT16,UINT32> >::iterator it = m_scoreRankReward.begin();	
		for (;it != m_scoreRankReward.end();++it){
			Athena::logger->trace("打印关卡(id:%d)的排名奖励:",it->first);
			
			std::map<UINT16,UINT32>::iterator subIt = it->second.begin();
			for (;subIt != it->second.end();++subIt){
				Athena::logger->trace("rank:%d,boxID:%d",subIt->first,subIt->second);	
			}
		}
	}
#endif
	
	TiXmlElement * pGUgamecopyrewardNode = pConstantNode->FirstChildElement("GUgamecopyreward");
	if (pGUgamecopyrewardNode){
		TiXmlElement * pEntryNode = pGUgamecopyrewardNode->FirstChildElement("entry");
		while (pEntryNode){
			stCopyMapReward reward;
			if (pEntryNode->Attribute("levelid",&iVal)){
				reward.m_levelID = iVal;
			}
			if (pEntryNode->Attribute("completerewarditem",&iVal)){
				reward.m_completeBoxID = iVal;
			}
			if (pEntryNode->Attribute("killrewarditem",&iVal)){
				reward.m_killBoxID = iVal;
			}
			if (pEntryNode->Attribute("bossid",&iVal)){
				reward.m_bossID = iVal;
			}
			m_copyMapRewards.insert(std::make_pair(reward.m_levelID,reward));
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("公会副本奖励:");
		std::map<UINT32,stCopyMapReward>::iterator it =  m_copyMapRewards.begin();
		for (;it != m_copyMapRewards.end();++it){
			stCopyMapReward & one = it->second;
			Athena::logger->trace("levelID:%d,completeBoxID:%d,killBoxID:%d,bossID:%d",one.m_levelID,one.m_completeBoxID,one.m_killBoxID,one.m_bossID);
		}
	}
#endif

	TiXmlElement * pGUturfwarMapNode = pConstantNode->FirstChildElement("GUturfwarMap");
	if (pGUturfwarMapNode){
		TiXmlElement * pEntryNode = pGUturfwarMapNode->FirstChildElement("entry");
		if (pEntryNode){
			if (pEntryNode->Attribute("Mapid",&iVal)){
				m_manorFightInfo.m_mapID = iVal;	
			}	

			if (pEntryNode->Attribute("winawarditemID",&iVal)){
				m_manorFightInfo.m_winBoxID = iVal;	
			}

			if (pEntryNode->Attribute("defeatedawardID",&iVal)){
				m_manorFightInfo.m_loserBoxID = iVal;
			}
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("公会领地信息: MapID:%d,winawardbox:%d,defeatedawardbox:%d",m_manorFightInfo.m_mapID,m_manorFightInfo.m_winBoxID,m_manorFightInfo.m_loserBoxID);
#endif

	TiXmlElement * pGUmanorNode = pConstantNode->FirstChildElement("GUmanor");
	if (pGUmanorNode){
		TiXmlElement *  pManorNode = pGUmanorNode->FirstChildElement("manor");
		while (pManorNode){
			stManorInfo manor;
			if (pManorNode->Attribute("id",&iVal)){
				manor.m_id = iVal;
			}
			if (pManorNode->Attribute("manorstar",&iVal)){
				manor.m_star = iVal;
			}
			if (pManorNode->Attribute("outputboxID",&iVal)){
				manor.m_prayExtraBoxID = iVal;
			}
			m_manorInfos[manor.m_id] = manor;
			pManorNode = pManorNode->NextSiblingElement("manor");
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("领地信息:");
		std::map<UINT16,stManorInfo>::iterator it =  m_manorInfos.begin();
		for (;it != m_manorInfos.end();++it){
			stManorInfo & manor = it->second;
			Athena::logger->trace("id:%d,star:%d,boxID:%d",manor.m_id,manor.m_star,manor.m_prayExtraBoxID);
		}
	}
#endif
	
	const char * szVal = NULL;
	TiXmlElement * pGUturfwartimeNode = pConstantNode->FirstChildElement("GUturfwartime");
	if (pGUturfwartimeNode){
		std::vector<std::string> strVec;
		TiXmlElement *  pWerNode = pGUturfwartimeNode->FirstChildElement("Wer");
		while (pWerNode){
			stManorFightTime  manorFightTime;
			
			if (pWerNode->Attribute("id",&iVal)){
				manorFightTime.m_id = iVal;
			}
			strVec.clear();	
			if ((szVal = pWerNode->Attribute("applytime"))){
				g_stringtok(strVec,szVal,"-");
				if (strVec.size() >= 2){
					manorFightTime.m_applyBeginTime = (UINT32)atoi(strVec[0].c_str());
					manorFightTime.m_applyEndTime = (UINT32)atoi(strVec[1].c_str());
				}
			}
			
			strVec.clear();
			if ((szVal = pWerNode->Attribute("applyover"))){
				g_stringtok(strVec,szVal,"-");
				if (strVec.size() >= 2){
					manorFightTime.m_applyOverBeginTime = (UINT32)atoi(strVec[0].c_str());
					manorFightTime.m_applyOverEndTime = (UINT32)atoi(strVec[1].c_str());
				}
			}

			strVec.clear();
			if ((szVal = pWerNode->Attribute("readinesstime"))){
				g_stringtok(strVec,szVal,"-");
				if (strVec.size() >= 2){
					manorFightTime.m_readyBeginTime = (UINT32)atoi(strVec[0].c_str());
					manorFightTime.m_readyEndTime = (UINT32)atoi(strVec[1].c_str());
				}
			}
			
			strVec.clear();
			if ((szVal = pWerNode->Attribute("accounttime"))){
				g_stringtok(strVec,szVal,"-");
				if (strVec.size() >= 2){
					manorFightTime.m_giveRewardBeginTime = (UINT32)atoi(strVec[0].c_str());
					manorFightTime.m_giveRewardEndTime = (UINT32)atoi(strVec[1].c_str());
				}
			}
			
			TiXmlElement * pEntryNode = pWerNode->FirstChildElement("entry");
			while (pEntryNode){
				stManorFightStepTime stepInfo;
				
				if ((pEntryNode->Attribute("id",&iVal))){
					stepInfo.m_id = iVal;	
				}
				
				strVec.clear();
				if ((szVal = pEntryNode->Attribute("breakTime"))){
					g_stringtok(strVec,szVal,"-");
					if (strVec.size() >= 2){
						stepInfo.m_breakBeginTime = (UINT32)atoi(strVec[0].c_str());
						stepInfo.m_breakEndTime = (UINT32)atoi(strVec[1].c_str());
					}
				}

				strVec.clear();
				if ((szVal = pEntryNode->Attribute("WarTime"))){
					g_stringtok(strVec,szVal,"-");
					if (strVec.size() >= 2){
						stepInfo.m_fightBeginTime = (UINT32)atoi(strVec[0].c_str());
						stepInfo.m_fightEndTime = (UINT32)atoi(strVec[1].c_str());
					}
				}

				manorFightTime.m_steps.insert(std::make_pair(stepInfo.m_id,stepInfo));
				pEntryNode = pEntryNode->NextSiblingElement("entry");
			}

			m_manorFightTimeInfos[manorFightTime.m_id] = manorFightTime;
			pWerNode = pWerNode->NextSiblingElement("Wer");	
		}
	}

	{
#ifdef _HDZ_DEBUG
		std::map<UINT16,stManorFightTime>::iterator it = m_manorFightTimeInfos.begin();
		for (;it != m_manorFightTimeInfos.end();++it){
			stManorFightTime & fightTime = it->second;
			Athena::logger->trace("[公会领地战时间] id:%d,applyBegin:%d,applyEnd:%d,applyOverBegin:%d,applyOverEnd:%d,readyBegin:%d,readyEnd:%d",fightTime.m_id,fightTime.m_applyBeginTime,fightTime.m_applyEndTime,fightTime.m_applyOverBeginTime,fightTime.m_applyOverEndTime,fightTime.m_readyBeginTime,fightTime.m_readyEndTime);

			std::map<UINT16,stManorFightStepTime>::iterator subIt = fightTime.m_steps.begin();
			for (;subIt != fightTime.m_steps.end();++subIt){
				stManorFightStepTime & stepInfo = subIt->second;
				Athena::logger->trace("id:%d,breakBegin:%d,breakEnd:%d,fightBegin:%d,fightEnd:%d",stepInfo.m_id,stepInfo.m_breakBeginTime,stepInfo.m_breakEndTime,stepInfo.m_fightBeginTime,stepInfo.m_fightEndTime);
			}
		}
#endif
	}

	TiXmlElement * pBlackmarketNode = pConstantNode->FirstChildElement("blackmarket");
	if (pBlackmarketNode){
		UINT16 freshprice = 0;
		if (pBlackmarketNode->Attribute("freshprice",&iVal)){
			freshprice = iVal;
		}

		TiXmlElement *  pEntryNode = pBlackmarketNode->FirstChildElement("entry");          
		while (pEntryNode){
			stBlackMarketInfo info;
			if (pEntryNode->Attribute("lv",&iVal)){                                         
				info.m_lv = iVal;
			}

			if(pEntryNode->Attribute("construction",&iVal)){
				info.m_construction = iVal;
			}

			if(pEntryNode->Attribute("purchasequantity",&iVal)){ 
				info.m_purchasequantity = iVal;
			}

			if(pEntryNode->Attribute("limitfreshtime",&iVal)){
				info.m_limitfreshtime = iVal;
			}

			if(pEntryNode->Attribute("shopID",&iVal)){ 
				info.m_shopID = iVal;
			}

			if(pEntryNode->Attribute("free",&iVal)){
				info.m_free = iVal;
			}

			info.m_freshprice = freshprice;

			m_blackMarketInfos[info.m_lv] = info;

			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}
	{
#ifdef _HDZ_DEBUG
		std::map<UINT16, stBlackMarketInfo>::iterator it = m_blackMarketInfos.begin();
		for(;it != m_blackMarketInfos.end(); ++it){
			stBlackMarketInfo & info = it->second;
			Athena::logger->trace("[公会黑市]lv:%d, construction:%d, purchasequantity:%d, limitfreshtime:%d,shopID:%d, free:%d, freshprice:%d", info.m_lv, info.m_construction, info.m_purchasequantity,info.m_limitfreshtime, info.m_shopID, info.m_free, info.m_freshprice);
		}
#endif
	}
	return true;
}

CGuildConfig::stHallInfo * CGuildConfig::getHallInfo(UINT16 lv)
{
	std::map<UINT16,stHallInfo>::iterator it = m_hallInfos.find(lv);
	if (it == m_hallInfos.end()){
		return NULL;
	}
	return &it->second;
}

CGuildConfig::stPrayInfo * CGuildConfig::getPrayInfo(UINT16 lv)
{
	std::map<UINT16,stPrayInfo>::iterator it = m_prayInfos.find(lv);
	if (it == m_prayInfos.end()){
		return NULL;
	}

	return &it->second;
}

CGuildConfig::stShopInfo * CGuildConfig::getShopInfo(UINT16 lv)
{
	std::map<UINT16,stShopInfo>::iterator it= m_shopInfos.find(lv);
	if (it == m_shopInfos.end()){
		return NULL;
	}

	return &it->second;
}
	
CGuildConfig::stDonateInfo * CGuildConfig::getDonateInfo(UINT16 id)
{
	std::map<UINT16,stDonateInfo>::iterator it = m_donateInfos.find(id);
	if (it ==  m_donateInfos.end()){
		return NULL;
	}
	return &it->second;
}
	
CGuildConfig::stRightInfo * CGuildConfig::getRightInfo(UINT8 title)
{
	std::map<UINT8,stRightInfo>::iterator it = m_rightInfos.find(title);
	if (it == m_rightInfos.end()){
		return NULL;
	}
	return &it->second;
}

CGuildConfig::stBlackMarketInfo * CGuildConfig::getBlackMarketInfo(UINT16 lv)
{
	std::map<UINT16, stBlackMarketInfo>::iterator it = m_blackMarketInfos.find(lv);
	if(it == m_blackMarketInfos.end()){
		return NULL;
	}
	return &it->second;
}

UINT16	CGuildConfig::getCopyMapOpenLv(UINT32 copymapID)
{
	std::map<UINT32,UINT8>::iterator it = m_copyMapOpenLv.find(copymapID);	
	if (it == m_copyMapOpenLv.end()){
		return (UINT16)-1;
	}
	else {
		return it->second;
	}
}

UINT32  CGuildConfig::getStartCopymapCost(UINT32 copymapID)
{
	std::map<UINT32,UINT32>::iterator it =  m_openCopyMapCost.find(copymapID);
	if (it == m_openCopyMapCost.end()){
		return (UINT32)-1;
	}
	else {
		return it->second;
	}
}

UINT32   CGuildConfig::getRankReward(UINT16 rank)
{
	std::map<UINT16,UINT32>::iterator it =  m_damageRankReward.find(rank);
	if (it != m_damageRankReward.end()){
		return it->second;
	}
	else {
		return 0;
	}
}

UINT32    CGuildConfig::getBossIDByLevelID(UINT32 levelID)
{
	std::map<UINT32,stCopyMapReward>::iterator it =  m_copyMapRewards.find(levelID);
	if (it != m_copyMapRewards.end()){
		return it->second.m_bossID;
	}
	return 0;
}

UINT32 	CGuildConfig::getScoreRankBoxID(UINT32 copyMapID,UINT16 rank)
{
	std::map<UINT32,std::map<UINT16,UINT32> >::iterator it = m_scoreRankReward.find(copyMapID); 
	if (it != m_scoreRankReward.end()){
		std::map<UINT16,UINT32>::iterator subIt = it->second.find(rank);	
		if (subIt != it->second.end()){
			return subIt->second;
		}
	}
	return 0;
}
	
UINT32	CGuildConfig::getCompleteBoxID(UINT32 copymapID)
{
	std::map<UINT32,stCopyMapReward>::iterator it =  m_copyMapRewards.find(copymapID);
	if (it != m_copyMapRewards.end()){
		return it->second.m_completeBoxID;	
	}
	return 0;
}

UINT32  CGuildConfig::getKillBoxID(UINT32 copymapID)
{
	std::map<UINT32,stCopyMapReward>::iterator it =  m_copyMapRewards.find(copymapID);
	if (it != m_copyMapRewards.end()){
		return it->second.m_killBoxID;	
	}
	return 0;
}

bool CGuildConfig::hasRight(UINT8 title,UINT32 right)
{
	std::map<UINT8,stRightInfo>::iterator it =  m_rightInfos.find(title);
	if (it == m_rightInfos.end()){
		return false;
	}
	
	stRightInfo & info = it->second;

	switch (right){
		case eRight_Manifesto :		//宣言
			{
				return info.m_manifesto;
			}
			break;
		case eRight_Audit :			//审核
			{
				return info.m_audit;
			}
			break;
		case eRight_Dissolve :		//解散
			{
				return info.m_dissolve;
			}
			break;
		case eRight_Dislodge  :		//驱散
			{
				return info.m_dislodge;
			}
			break;
		case eRight_Nofity :		//公告
			{
				return info.m_notify;
			}
			break;
		case eRight_OpenMap :  //开启副本
			{
				return info.m_openCopymap;
			}
			break;
		case eRight_Lvup : //升级
			{
				return info.m_lvUp;
			}
			break;
	}
	return false;
}

UINT32 CGuildConfig::getOwnerPrayBoxID(UINT32 manorID)
{
	std::map<UINT16,stManorInfo>::iterator it =  m_manorInfos.find(manorID);
	if (it != m_manorInfos.end()){
		return it->second.m_prayExtraBoxID;
	}
	return 0;
}
