#include "MiscCfgMgr.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include <string>
#include "UtilFunction.h"

bool CMiscCfgMgr::loadCfg()
{
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/grow.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[杂项配置]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[杂项配置]找不到节点root");
		return false;
	}

	TiXmlElement * pConstantNode = pRoot->FirstChildElement("constant");
	if (!pConstantNode){
		Athena::logger->error("[杂项配置]找不到节点constant");
		return false;
	}

	INT32 iVal = 0;
	const char * szVal = NULL;
	TiXmlElement * pEntryNode = pConstantNode->FirstChildElement("entry");
	while (pEntryNode){
		UINT32 id = 0;
		if (pEntryNode->Attribute("id",&iVal)){
			id = iVal;
		}

		if (MISC_CFG_ID_LINGWU == id){//领悟技能
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamond = 0;
				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamond = iVal;
				}

				m_lingwuNum2Diamonds[num] =  diamond;
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}
#ifdef _HDZ_DEBUG
			std::map<UINT16,UINT32>::iterator it = m_lingwuNum2Diamonds.begin();
			for (;it != m_lingwuNum2Diamonds.end();++it){
				Athena::logger->trace("[领悟次数配置] num=%d,diamonds=%d",it->first,it->second);	
			}
#endif
		}
		else if (MISC_CFG_ID_OPEN_SKILL_LV == id){//领悟技能格子开放等级
			TiXmlElement * pSpacesNode = pEntryNode->FirstChildElement("Spaces");
			while (pSpacesNode){
				UINT8 num = 0;
				UINT16 level = 0;

				if (pSpacesNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pSpacesNode->Attribute("Level",&iVal)){
					level = iVal;
				}
				m_heroSkillOpenLv[num] = level;
				pSpacesNode = pSpacesNode->NextSiblingElement("Spaces");
			}
#ifdef _HDZ_DEBUG
			std::map<UINT8,UINT16>::iterator it = m_heroSkillOpenLv.begin();
			for (;it != m_heroSkillOpenLv.end();++it){
				Athena::logger->trace("[领悟技能格子配置] num=%d,level=%d",it->first,it->second);
			}
#endif
		}
		else if ( MISC_CFG_ID_VIPLV2_RECHARGE == id){//vip与充值关系
			TiXmlElement * pVipNode = pEntryNode->FirstChildElement("Vip");	
			while (pVipNode){
				UINT8 vipLv = 0;
				UINT32 diamonds = 0;
				if (pVipNode->Attribute("Lv",&iVal)){
					vipLv = iVal;
				}

				if (pVipNode->Attribute("Diamonds",&iVal)){
					diamonds = iVal;
				}
				m_vipLv2Recharge[vipLv] = diamonds;
				pVipNode = pVipNode->NextSiblingElement("Vip");	
			}

#ifdef _HDZ_DEBUG
			Athena::logger->trace("打印vip等级与充值数量");
			std::map<UINT8,UINT32>::iterator it = m_vipLv2Recharge.begin();
			for (;it != m_vipLv2Recharge.end();++it){
				Athena::logger->trace("vipLv=%d,diamonds=%d",it->first,it->second);			
			}
#endif
		}
		else if (MISC_CFG_ID_BATTLE_POS == id){//宠物出战格子与等级关系
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 level = 0, pos = 0;
				if (pTimeNode->Attribute("num",&iVal)){
					pos = iVal;
				}

				if (pTimeNode->Attribute("Level",&iVal)){
					level = iVal;
				}

				m_battlePetPos2Lv[pos] = level;
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}
		}
		else if (MISC_CFG_ID_JUEXING_OPEN_LV == id){//觉醒开放等级
			TiXmlElement * pSpacesNode = pEntryNode->FirstChildElement("Spaces");
			while (pSpacesNode){
				UINT16 num = 0;
				UINT16 level = 0;
				if (pSpacesNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pSpacesNode->Attribute("Level",&iVal)){
					level = iVal;
				}
				m_juexingOpenLv[num] = level;
				pSpacesNode = pSpacesNode->NextSiblingElement("Spaces");
			}

#ifdef _HDZ_DEBUG
			Athena::logger->trace("答应觉醒开放等级:");
			std::map<UINT16,UINT16>::iterator it = m_juexingOpenLv.begin();
			for (;it != m_juexingOpenLv.end();++it){
				Athena::logger->trace("位置=%d 开放等级=%d",it->first,it->second);	
			}
#endif
		}
		else if (MISC_CFG_ID_FUWEN_OPEN_LV == id){//符文开放等级
			TiXmlElement * pSpacesNode = pEntryNode->FirstChildElement("Spaces");
			while (pSpacesNode){
				UINT16 num = 0;
				UINT16 level = 0;
				if (pSpacesNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pSpacesNode->Attribute("Level",&iVal)){
					level = iVal;
				}
				m_fuwenOpenLv[num] = level;
				pSpacesNode = pSpacesNode->NextSiblingElement("Spaces");
			}

#ifdef _HDZ_DEBUG
			Athena::logger->trace("打印符文开放等级:");
			std::map<UINT16,UINT16>::iterator it = m_fuwenOpenLv.begin();
			for (;it != m_fuwenOpenLv.end();++it){
				Athena::logger->trace("位置=%d 开放等级=%d",it->first,it->second);	
			}
#endif
		}
		else if (MISC_CFG_ID_RUSH_RESET == id){//扫荡重置
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamondmun = 0;
				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamondmun = iVal;
				}
				m_rushResetNum2Diamond[num] = diamondmun;
				pTimeNode = pTimeNode->NextSiblingElement("time"); 
			}

#ifdef _HDZ_DEBUG
			Athena::logger->trace("扫荡重置需要钻石:");
			std::map<UINT16,UINT32>::iterator it = m_rushResetNum2Diamond.begin();
			for (;it != m_rushResetNum2Diamond.end();++it){
				Athena::logger->trace("扫荡次数 %d 需要钻石 %d",it->first,it->second);
			}
#endif
		}
		else if (MIS_CFG_ID_JUEXING_NEED_DIAMOND == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamondmun = 0;
				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamondmun = iVal;
				}
				m_juexingNeedDiamond[num] = diamondmun;
				pTimeNode = pTimeNode->NextSiblingElement("time"); 
			}

#ifdef _HDZ_DEBUG
			Athena::logger->trace("觉醒需要钻石:");
			std::map<UINT16,UINT32>::iterator it = m_juexingNeedDiamond.begin();
			for (;it != m_juexingNeedDiamond.end();++it){
				Athena::logger->trace("觉醒次数 %d 需要钻石 %d",it->first,it->second);
			}
#endif

		}
		else if (MISC_CFG_ID_WORLD_BOSS_LEVEL_LIMIT == id){//世界boss等级限制
			TiXmlElement * pOpenNode = pEntryNode->FirstChildElement("Openlevel");
			if (pOpenNode){
				if (pOpenNode->Attribute("Level",&iVal)){
					m_worldBossLvLimit = iVal;
				}			
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("世界boss等级限制:%d",m_worldBossLvLimit);
#endif
		}
		else if (MISC_CFG_ID_FIRST_RECHARGE == id){//首次充值道具
			TiXmlElement * pFirstBuyNode = pEntryNode->FirstChildElement("firstbuy");
			while (pFirstBuyNode){
				UINT32 itemID = 0;
				UINT32 num = 0;
				if (pFirstBuyNode->Attribute("itemID",&iVal)){
					itemID = iVal;
				}

				if (pFirstBuyNode->Attribute("itemnum",&iVal)){
					num = iVal;
				}
				m_firstRechargeRewards.push_back(std::make_pair(itemID,num));
				pFirstBuyNode = pFirstBuyNode->NextSiblingElement("firstbuy");
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("首次充值奖励:");
			for (UINT16 i = 0;i < m_firstRechargeRewards.size();++i){
				std::pair<UINT32,UINT32> & reward = m_firstRechargeRewards[i];
				Athena::logger->trace("itemID:%d,num:%d",reward.first,reward.second);
			}
#endif
		}
		else if (MISC_CFG_ID_WORLD_BOSS_BUY_PRICE == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamond = 0;

				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamond = iVal;
				}
				m_worldBossBuyPrices[num] = diamond;
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("世界boss士气鼓舞购买价格:");
			std::map<UINT16,UINT32>::iterator it =  m_worldBossBuyPrices.begin();
			for (;it != m_worldBossBuyPrices.end();++it){
				Athena::logger->trace("num:%d,diamond:%d",it->first,it->second);	
			}
#endif
		}
		else if (MISC_CFG_ID_ONEKEY_ROLELV == id){
			TiXmlElement * pLevelNode = pEntryNode->FirstChildElement("level");
			while (pLevelNode){
				UINT16 funID = 0;
				UINT16 openLv = 0;
				if (pLevelNode->Attribute("functiontype",&iVal)){
					funID = iVal;
				}

				if (pLevelNode->Attribute("openlevel",&iVal)){
					openLv = iVal;
				}

				if (1 == funID){
					m_oneKeyCaptureLv = openLv;
				}
				else if (2 == funID){
					m_oneKeyRaiseLv = openLv;	
				}
				else if (3 == funID){
					m_oneKeyRushLv = openLv;	
				}
				else if (4 == funID){
					m_oneKeyStudyEquipSkillLv = openLv;	
				}
				else if (5 == funID){
					m_oneKeyStudyHeroSkillLv  = openLv;	
				}
#ifdef _HDZ_DEBUG
				Athena::logger->trace("一键强化需要的等级分别为:m_oneKeyCaptureLv=%d,m_oneKeyRaiseLv=%d,m_oneKeyRushLv=%d",m_oneKeyCaptureLv,m_oneKeyRaiseLv,m_oneKeyRushLv);
#endif
				pLevelNode = pLevelNode->NextSiblingElement("level");
			}
		}
		else if (MISC_CFG_ID_TEAM_COPYMAP_RESET_COST == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamond = 0;
				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;	
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamond = iVal;
				}
				m_teamCopyResetCosts[num] = diamond;
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("组队副本重置价格表:");
			std::map<UINT16,UINT32>::iterator it =  m_teamCopyResetCosts.begin();
			for (;it != m_teamCopyResetCosts.end();++it){
				Athena::logger->trace("重置次数:%d,价钱:%d",it->first,it->second);	
			}
#endif
		}
		else if (MISC_CFG_ID_REALTIME_RESET_COST == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamond = 0;
				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;	
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamond = iVal;
				}
				m_realTimeResetCosts[num] = diamond;
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("实时战场重置价格表:");
			std::map<UINT16,UINT32>::iterator it =  m_realTimeResetCosts.begin();
			for (;it != m_realTimeResetCosts.end();++it){
				Athena::logger->trace("重置次数:%d,价钱:%d",it->first,it->second);	
			}
#endif
		}
		else if (MISC_CFG_ID_ARENA_COOLDOWN == id){
			TiXmlElement * pArenNode = pEntryNode->FirstChildElement("arena");
			if (pArenNode){
				if (pArenNode->Attribute("time",&iVal)){
					m_arenaCDInfo.m_challengeNum = iVal;	
				}	

				if (pArenNode->Attribute("coolingtime",&iVal)){
					m_arenaCDInfo.m_cd = iVal;
				}

				if (pArenNode->Attribute("diamondmun",&iVal)){
					m_arenaCDInfo.m_needDiamond = iVal;
				}
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("竞技场冷却相关设置:time:%d,coolingtime:%d,diamondmun:%d",m_arenaCDInfo.m_challengeNum,m_arenaCDInfo.m_cd,m_arenaCDInfo.m_needDiamond);
#endif
		}
		else if (MISC_CFG_ID_ARENA_RESET == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");

			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamond = 0;

				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamond = iVal;
				}

				m_arenaResetCosts.insert(std::make_pair(num,diamond));
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("竞技场重置费用:");
			std::map<UINT16,UINT32>::iterator it = m_arenaResetCosts.begin();
			for (;it != m_arenaResetCosts.end();++it){
				Athena::logger->trace("num:%d,cost:%d",it->first,it->second);
			}
#endif
		}
		else if (MISC_CFG_ID_GUARD_BUY_INTERCEPT_NUM == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamond = 0;

				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamond = iVal;
				}
				m_guardBuyInterceptNum[num] = diamond;
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}

#ifdef _HDZ_DEBUG
			Athena::logger->trace("运镖拦截次数购买:");
			std::map<UINT16,UINT32>::iterator it = m_guardBuyInterceptNum.begin();	
			for (;it != m_guardBuyInterceptNum.end();++it){
				Athena::logger->trace("num=%d,diamond:%d",it->first,it->second);
			}
#endif
		}
		else if(MISC_CFG_ID_GHANGE_NAME_NUM == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 num = 0;
				UINT32 diamond = 0;

				if (pTimeNode->Attribute("num",&iVal)){
					num = iVal;
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					diamond = iVal;
				}
				m_changeNameResetCosts[num] = diamond;
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}

#ifdef _HDZ_DEBUG
			Athena::logger->trace("改名次数花费:");
			std::map<UINT8,UINT32>::iterator it = m_changeNameResetCosts.begin();
			for (;it != m_changeNameResetCosts.end();++it){
				Athena::logger->trace("num=%d,diamond:%d",it->first,it->second);
			}
#endif
		}
		else if(MISC_CFG_ID_BUY_GOLD_NEED_DIAMOND  == id){
			UINT32 gainsGoldNum = 0;
			if(pEntryNode->Attribute("num",&iVal))
			{
				gainsGoldNum = iVal;
			}
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			while (pTimeNode){
				UINT16 id = 0;
				stBuyGoldInfo buyGoldInfo;
				if (pTimeNode->Attribute("id",&iVal)){
					id = iVal;
				}

				if (pTimeNode->Attribute("diamondmun",&iVal)){
					buyGoldInfo.m_gainsGoldNum  = gainsGoldNum;
					buyGoldInfo.m_needDiamond = iVal;
				}


				m_buyGoldInfo[id] = buyGoldInfo;
				pTimeNode = pTimeNode->NextSiblingElement("time");
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("购买金币次数花费:");
			std::map<UINT8,stBuyGoldInfo>::iterator it =  m_buyGoldInfo.begin();
			for (;it !=  m_buyGoldInfo.end();++it){
				Athena::logger->trace("num=%d, diamond:%d, gainsGoldNum:%d", it->first, it->second.m_needDiamond, it->second.m_gainsGoldNum);
			}
#endif
		}
		else if(MISC_CFG_ID_CHAT_LIMIT_TIME == id)
		{
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			if(pTimeNode)
			{
				if (pTimeNode->Attribute("limittime",&iVal)){
					m_chatLimitTime = iVal;
				}
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("世界聊天冷却时间:%d",m_chatLimitTime);
#endif
		}
		else if(MISC_CFG_ID_MAX_ROLE_LVL == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("time");
			if (pTimeNode){
				int maxLvl = 0;
				if (pTimeNode->Attribute("LVmaxnum", &maxLvl)){
					m_maxRoleLvl = maxLvl;
					Athena::logger->trace("max role lvl is %d", m_maxRoleLvl);
				}
			}
		}
		else if(MISC_CFG_ID_EXP_LEVEL_LIMIT == id){
			TiXmlElement * pTimeNode = pEntryNode->FirstChildElement("item");
			while (pTimeNode){
				UINT32 id = 0;
				stExpLevelLimit one;
				if (pTimeNode->Attribute("id",&iVal)){
					id = iVal;
				}

				if ((szVal = pTimeNode->Attribute("levelRange"))){
					std::vector<std::string> strVec;
					g_stringtok(strVec, szVal, "-");
					if(strVec.size() >= 2){
						one.m_minLevel = (UINT16)atoi(strVec[0].c_str());
						one.m_maxLevel = (UINT16)atoi(strVec[1].c_str());
					}
				}
				m_expLevelLimit[id] = one;
				pTimeNode = pTimeNode->NextSiblingElement("item");
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("经验书使用等级限制:");   
			std::map<UINT32, stExpLevelLimit>::iterator it = m_expLevelLimit.begin();
			for(;it != m_expLevelLimit.end(); ++it){
				Athena::logger->trace("id:%d, minLevel:%d,maxLevel:%d",it->first,it->second.m_minLevel,it->second.m_maxLevel);
			}
#endif
		}
		else if(MISC_CFG_ID_RUSH_LEVEL_OPEN == id){
			TiXmlElement * pDiamond = pEntryNode->FirstChildElement("diamond");
            if (pDiamond) {
                if (pDiamond->Attribute("isopen", &iVal)){
                    m_rushLvlOpen = (bool)iVal;
                    Athena::logger->trace("m_rushLvlOpen is %s", (m_rushLvlOpen ? "open" : "not open"));
                }
            }
        }
		else if(MISC_CFG_ID_RUNE_EVOLVE_LVL_LIMIT == id){
            int roleLv = 0, refineLv = 0;
			TiXmlElement * pLv = pEntryNode->FirstChildElement("LV");
            while (pLv) {
                if (!pLv->Attribute("RefineLV", &refineLv)) {
                    Athena::logger->trace("LV节点找不到RefineLV");
                    return false;
                }
                if (!pLv->Attribute("RoleLv", &roleLv)) {
                    Athena::logger->trace("LV节点找不到RoleLv");
                    return false;
                }
                m_runeLimitMap.insert(std::make_pair(refineLv, roleLv));
                pLv = pLv->NextSiblingElement("LV");
            }
#ifdef _HDZ_DEBUG
            for (std::map<UINT32, UINT32>::iterator it = m_runeLimitMap.begin();
                    it != m_runeLimitMap.end(); it++) {
				Athena::logger->trace("[符文强化等级限制] refineLv is %d, roleLv is %d",
                        it->first, it->second);
            }
#endif
        }
		else if(MISC_CFG_ID_OPERATION_LVL_LIMIT == id){
            int type = 0, lvlLimit = 0;
			TiXmlElement * pLv = pEntryNode->FirstChildElement("level");
            while (pLv) {
                if (!pLv->Attribute("functiontype", &type)) {
                    Athena::logger->trace("level节点找不到functiontype");
                    return false;
                }
                if (!pLv->Attribute("levelNum", &lvlLimit)) {
                    Athena::logger->trace("level节点找不到levelNum");
                    return false;
                }
                m_operaLvlLimitMap.insert(std::make_pair(type, lvlLimit));
                pLv = pLv->NextSiblingElement("level");
            }
#ifdef _HDZ_DEBUG
            for (std::map<UINT32, UINT32>::iterator it = m_operaLvlLimitMap.begin();
                    it != m_operaLvlLimitMap.end(); it++) {
				Athena::logger->trace("[杂项等级限制] type is %d, lvlLimit is %d",
                        it->first, it->second);
            }
#endif
        }
		else if(MISC_CFG_ID_INTENSIFY_LVL_LIMIT == id){
            int lvlLimit = 0;
			TiXmlElement * pLv = pEntryNode->FirstChildElement("level");
            if (pLv && !pLv->Attribute("Maxnum", &lvlLimit)) {
                Athena::logger->trace("level节点找不到Maxnum");
                return false;
            }
            m_intensifyLvlLimit = lvlLimit;
#ifdef _HDZ_DEBUG
            Athena::logger->trace("[强化最大等级] max lvl is %d", m_intensifyLvlLimit);
#endif
        }
		else if(MISC_CFG_ID_LEVEL_WFLFARE == id){
			int level = 0;
			if(pEntryNode->Attribute("lv", &level)){
				m_levelWflfare = level;	
			}
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[冲级好礼开启等级] lv is %d", m_levelWflfare);
#endif
		}
		else if(MISC_CFG_ID_GUAJI_LIMIT == id){
            int maxtime = 0;
			int exp = 0; 
			int gold = 0; 
			TiXmlElement * pElem = pEntryNode->FirstChildElement("elem");
            if (pElem && !pElem->Attribute("maxtime", &maxtime)) {
                Athena::logger->trace("elem节点找不到maxtime");
                return false;
            }
            if (pElem && !pElem->Attribute("exp", &exp)) {
                Athena::logger->trace("elem节点找不到exp");
                return false;
            }
            if (pElem && !pElem->Attribute("gold", &gold)) {
                Athena::logger->trace("elem节点找不到gold");
                return false;
            }
            m_guaji.m_maxtime = maxtime;
            m_guaji.m_exp = exp;
            m_guaji.m_goldRate = gold;

			TiXmlElement * pCoin = pEntryNode->FirstChildElement("mjcoin");
			int id = 0; 
			int coin = 0; 
			while (pCoin) {
                if (!pCoin->Attribute("id", &id)) {
                    Athena::logger->trace("mjcoin节点找不到id");
                    return false;
                }
                if (!pCoin->Attribute("coin", &coin)) {
                    Athena::logger->trace("mjcoin节点找不到coin");
                    return false;
                } 
				m_guaji.m_mjCoin.insert(std::make_pair((UINT32)id, (UINT32)coin));
                pCoin = pCoin->NextSiblingElement("mjcoin");
            }
#ifdef _HDZ_DEBUG
            Athena::logger->trace("[挂机配置] m_maxtime is %d m_exp=%d m_goldRate=%d",m_guaji.m_maxtime,m_guaji.m_exp,m_guaji.m_goldRate);
			for (std::map<UINT32, UINT32>::iterator it = m_guaji.m_mjCoin.begin();
                    it != m_guaji.m_mjCoin.end(); it++) {
				Athena::logger->trace("[挂机配置] mjcoin level %d, coin is %d",
                        it->first, it->second);
            }
#endif
        }
		else if(MISC_CFG_ID_CHAPTER_EQUIP == id){
             
			TiXmlElement * pChap = pEntryNode->FirstChildElement("chap");
 
			stChapEquipLimit st;
			while (pChap) {
                if (!pChap->Attribute("id", &st.m_chapID)) {
                    Athena::logger->trace("chap节点找不到id");
                    return false;
                }
				st.m_sr=0;
                pChap->Attribute("sr", &st.m_sr);
				st.m_ssr=0;
                pChap->Attribute("ssr", &st.m_ssr);
				
				m_chapEquipLimit.insert(std::make_pair(st.m_chapID, st));
                pChap = pChap->NextSiblingElement("chap");
            }
#ifdef _HDZ_DEBUG 
			for (std::map<UINT32, stChapEquipLimit>::iterator it = m_chapEquipLimit.begin();
                    it != m_chapEquipLimit.end(); it++) {
				Athena::logger->trace("[章节装备配置] chap %d, sr %d ssr %d",
                        it->first, it->second.m_sr, it->second.m_ssr);
            }
#endif
        }
		else if(MISC_CFG_ID_RECAST_EQUIP == id){
			TiXmlElement * pItem = pEntryNode->FirstChildElement("item");
			if (pItem)
			{
				int nValue = 0;
				if (pItem->Attribute("n", &nValue)) { 
					m_recastDiamond.m_n = nValue;
				} 
				if (pItem->Attribute("r", &nValue)) { 
					m_recastDiamond.m_r= nValue;
				} 
				if (pItem->Attribute("sr", &nValue)) { 
					m_recastDiamond.m_sr = nValue;
				} 
				if (pItem->Attribute("ssr", &nValue)) { 
					m_recastDiamond.m_ssr= nValue;
				} 
				if (pItem->Attribute("ur", &nValue)) { 
					m_recastDiamond.m_ur = nValue;
				} 
			}
			
		}
		pEntryNode = pEntryNode->NextSiblingElement("entry");
	}
	return true;
}

UINT8 CMiscCfgMgr::getVipLv(UINT32 totalRecharge)
{
	std::map<UINT8,UINT32>::iterator it = m_vipLv2Recharge.begin();	
	for (;it != m_vipLv2Recharge.end();++it){
		if (totalRecharge < it->second){
			return it->first - 1;
		}	
	}
	return m_vipLv2Recharge.size();
}

UINT32 CMiscCfgMgr::getStudySkillDiamond(UINT32 num)
{
	std::map<UINT16,UINT32>::iterator it = m_lingwuNum2Diamonds.find(num);
	if (it != m_lingwuNum2Diamonds.end()){
		return it->second;	
	}

	return (UINT32)-1;
}

UINT16  CMiscCfgMgr::getLvByBattlePetPos(UINT16 pos)
{
	std::map<UINT16,UINT16>::iterator it = m_battlePetPos2Lv.find(pos);
	if (it != m_battlePetPos2Lv.end()){
		return it->second;
	}	
	return 0;
}

UINT32  CMiscCfgMgr::getDiamondByRushReset(UINT16 resetNum)
{
	std::map<UINT16,UINT32>::iterator it = m_rushResetNum2Diamond.find(resetNum);
	if (it != m_rushResetNum2Diamond.end()){
		return it->second;
	}
	else {
		return (UINT32)-1;
	}
}

UINT32  CMiscCfgMgr::getDiamondByJueXing(UINT16 num)
{
	std::map<UINT16,UINT32>::iterator it = m_juexingNeedDiamond.find(num);	
	if (it != m_juexingNeedDiamond.end()){
		return it->second;	
	}
	return (UINT32)-1;
}

UINT16  CMiscCfgMgr::getNeedLvByPos(UINT16 pos)
{
	std::map<UINT16,UINT16>::iterator it = m_juexingOpenLv.find(pos);

	if (it != m_juexingOpenLv.end()){
		return it->second;
	}

	return  (UINT16)-1;
}

UINT8  CMiscCfgMgr::getUnlockedFuwenEquipNum(UINT16 roleLv)
{
	std::map<UINT16,UINT16>::reverse_iterator it = m_fuwenOpenLv.rbegin();	
	for (;it != m_fuwenOpenLv.rend();++it){
		if (roleLv >= it->second){
			return it->first;
		}
	}
	return 0;
}

UINT32 CMiscCfgMgr::getWorldBossPriceByTime(UINT16 num)
{
	std::map<UINT16,UINT32>::iterator it =  m_worldBossBuyPrices.find(num);
	if (it != m_worldBossBuyPrices.end()){
		return it->second;
	}
	return (UINT32)-1;
}

UINT32 CMiscCfgMgr::getTeamCopyResetCostByNum(UINT16 num)
{
	std::map<UINT16,UINT32>::iterator it = m_teamCopyResetCosts.find(num);
	if (it != m_teamCopyResetCosts.end()){
		return it->second;
	}
	return (UINT32)-1;
}

UINT32 CMiscCfgMgr::getRealTimeResetCostByNum(UINT16 num)
{
	std::map<UINT16,UINT32>::iterator it = m_realTimeResetCosts.find(num);
	if (it != m_realTimeResetCosts.end()){
		return it->second;
	}
	return (UINT32)-1;
}

UINT32 CMiscCfgMgr::getNeedDiamondByNum(UINT32 num)
{
	std::map<UINT16,UINT32>::iterator it = m_arenaResetCosts.find(num);
	if (it != m_arenaResetCosts.end()){
		return it->second;
	}
	return (UINT32)-1;
}

UINT32 CMiscCfgMgr::getInterceptNeedDiamond(UINT32 num)
{
	std::map<UINT16,UINT32>::iterator it = m_guardBuyInterceptNum.find(num);

	if (it != m_guardBuyInterceptNum.end()){
		return it->second;
	}
	return (UINT32)-1;
}

UINT32 CMiscCfgMgr::getChangeNameNeedDiamond(UINT8 num)
{
	std::map<UINT8,UINT32>::iterator it = m_changeNameResetCosts.find(num);
	if(it != m_changeNameResetCosts.end()){
		return it->second;
	}
	return (UINT32)-1;
}

CMiscCfgMgr::stBuyGoldInfo *  CMiscCfgMgr::getBuyGoldNeedDiamond(UINT8 num)
{
	std::map<UINT8,stBuyGoldInfo>::iterator it = m_buyGoldInfo.find(num);
	if(it != m_buyGoldInfo.end())
	{
		return &it->second;
	}
	return NULL;
}

UINT16 CMiscCfgMgr::getChatLimitTime()
{
	return m_chatLimitTime;
}

CMiscCfgMgr::stExpLevelLimit * CMiscCfgMgr::getLimitLevel(UINT32 id)
{
	std::map<UINT32, stExpLevelLimit>::iterator it = m_expLevelLimit.find(id);
	if(it != m_expLevelLimit.end()){
		return &it->second;
	}
	return NULL;
}

UINT32 CMiscCfgMgr::getRuneRoleLvl(UINT32 refineLv) {
    std::map<UINT32, UINT32>::iterator it = m_runeLimitMap.find(refineLv);
    if (it != m_runeLimitMap.end()) {
        return it->second;
    }
    return 0;
}

UINT32 CMiscCfgMgr::getOperaLvlLimit(UINT32 type) {
    std::map<UINT32, UINT32>::iterator it = m_operaLvlLimitMap.find(type);
    if (it != m_operaLvlLimitMap.end()) {
        return it->second;
    }
    return 0;
}

stChapEquipLimit* CMiscCfgMgr::getChapEqupLimit(UINT32 chapID)
{
 	std::map<UINT32, stChapEquipLimit>::iterator it = m_chapEquipLimit.find(chapID);
    if (it != m_chapEquipLimit.end()) {
        return &it->second;
    }
    return NULL;
}