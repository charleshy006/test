#include "ArenaConfig.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

bool ArenaConfig::loadCfg()
{
	TiXmlDocument doc;
	std::string file = Athena::global["confDir"] + "/arena.xml";
	if (!doc.LoadFile(file.c_str())){
		Athena::logger->error("[竞技场配置]打开配置文件%s错误",file.c_str());
		return false;
	} 

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[竞技场配置]找不到节点root");
		return false;
	}

	INT32 iVal = 0;

	TiXmlElement * pHistoryArenaNode = pRoot->FirstChildElement("Diaarena");
	if (pHistoryArenaNode){
		TiXmlElement * pRewardNode = pHistoryArenaNode->FirstChildElement("Reward");
		while (pRewardNode){
			UINT16 rank = 0;
			UINT32 totalNum = 0;

			if (pRewardNode->Attribute("id",&iVal)){
				rank = iVal ;	
			}
			
			if (pRewardNode->Attribute("totalRewardnum",&iVal)){
				totalNum = iVal;
			}

			m_rank2TotalReward[rank] = totalNum;
			pRewardNode = pRewardNode->NextSiblingElement("Reward");
		}
	}

#ifdef _HDZ_DEBUG
	{
		std::map<UINT16,UINT32>::iterator it = m_rank2TotalReward.begin();
		for (;it != m_rank2TotalReward.end();++it){
			Athena::logger->trace("竞技场历史排名奖励:id=%d,totalNum=%d",it->first,it->second);
		}
	}
#endif

	TiXmlElement * pArenaRewardNode = pRoot->FirstChildElement("ArenaReward");
	if (pArenaRewardNode){
		TiXmlElement * pRewardNode = pArenaRewardNode->FirstChildElement("Reward");
		while (pRewardNode){
			stArenaEverydayReward reward;
			if (pRewardNode->Attribute("id",&iVal)){
				reward.m_id = iVal;
			}

			if (pRewardNode->Attribute("Startrank",&iVal)){
				reward.m_startRank = iVal;	
			}

			if (pRewardNode->Attribute("Endrank",&iVal)){
				reward.m_endRank = iVal;
			}

			if (pRewardNode->Attribute("Fristrewardid",&iVal)){
				reward.m_itemID1 = iVal;
			}

			if (pRewardNode->Attribute("Fristrewardnum",&iVal)){
				reward.m_num1 = iVal;
			}

			if (pRewardNode->Attribute("Secondrewardid",&iVal)){
				reward.m_itemID2 = iVal;
			}
			
			if (pRewardNode->Attribute("Secondrewardnum",&iVal)){
				reward.m_num2 = iVal;
			}

			m_everydayReward[reward.m_id] = reward;
			pRewardNode = pRewardNode->NextSiblingElement("Reward");
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("每日竞技场排名奖励:");
		std::map<UINT16,stArenaEverydayReward>::iterator it = m_everydayReward.begin();
		for (;it != m_everydayReward.end();++it){
			stArenaEverydayReward & reward = it->second;
			Athena::logger->trace("id:%d,startRank:%d,endRank:%d,itemID1:%d,num1:%d,itemID2:%d,num2:%d",reward.m_id,reward.m_startRank,reward.m_endRank,reward.m_itemID1,reward.m_num1,reward.m_itemID2,reward.m_num2);
		}
	}
#endif

	TiXmlElement * pLowerringNode = pRoot->FirstChildElement("Lowerring");
	if (pLowerringNode){
		parseReward(pLowerringNode,m_easyLeitaiReward);	
	}

	TiXmlElement * pSeniorringNode = pRoot->FirstChildElement("Seniorring");
	if (pSeniorringNode){
		parseReward(pSeniorringNode,m_difficultLeitaiReward);	
	}

	TiXmlElement * pLowerringmatchNode = pRoot->FirstChildElement("Lowerringmatch");
	if (pLowerringmatchNode){
		parseMatch(pLowerringmatchNode,m_easyLeitaiMatch);	
	}

	TiXmlElement * pSeniorringmatchNode = pRoot->FirstChildElement("Seniorringmatch");
	if (pSeniorringmatchNode){
		parseMatch(pSeniorringmatchNode,m_difficultLeitaiMatch);	
	}
	
	TiXmlElement * pLowerringmatchpercentNode = pRoot->FirstChildElement("Lowerringmatchpercent");
	if (pLowerringmatchpercentNode){
		TiXmlElement *pTimeNode = pLowerringmatchpercentNode->FirstChildElement("time");
		while (pTimeNode){
			UINT16 round = 0;
			UINT16 percent = 0;
			if (pTimeNode->Attribute("num",&iVal)){
				round = iVal;
			}
			if (pTimeNode->Attribute("percent",&iVal)){
				percent = iVal;
			}
			m_easyLeitaiPercent[round] = percent;
			pTimeNode = pTimeNode->NextSiblingElement("time");
		}
	}
	
	TiXmlElement * pSeniorringmatchpercentNode = pRoot->FirstChildElement("Seniorringmatchpercent");
	if (pSeniorringmatchpercentNode){
		TiXmlElement *pTimeNode = pLowerringmatchpercentNode->FirstChildElement("time");
		while (pTimeNode){
			UINT16 round = 0;
			UINT16 percent = 0;
			if (pTimeNode->Attribute("num",&iVal)){
				round = iVal;
			}
			if (pTimeNode->Attribute("percent",&iVal)){
				percent = iVal;
			}
			m_difficultLeitaiPercent[round] = percent;
			pTimeNode = pTimeNode->NextSiblingElement("time");
		}
	}
	
	TiXmlElement * pSwitchmapNode = pRoot->FirstChildElement("switchmap");
	if (pSwitchmapNode){
		TiXmlElement * pSwitchNode = pSwitchmapNode->FirstChildElement("switch");
		while (pSwitchNode){
			if (pSwitchNode->Attribute("id",&iVal)){
				if (150001 == iVal){//简单模式
					TiXmlElement *pLevelNode = pSwitchNode->FirstChildElement("level");	
					while (pLevelNode){
						UINT32 levelID = 0;
						UINT32 rate = 0;
						if (pLevelNode->Attribute("levelid",&iVal)){
							levelID = iVal;
						}
						
						if (pLevelNode->Attribute("probability",&iVal)){
							rate = iVal;
						}
						m_easyLeitaiSwitchMap.m_id = 150001;
						m_easyLeitaiSwitchMap.m_level2Rate[levelID] = rate;
						pLevelNode = pLevelNode->NextSiblingElement("level");
					}
				}	
				else {
					TiXmlElement *pLevelNode = pSwitchNode->FirstChildElement("level");	
					while (pLevelNode){
						UINT32 levelID = 0;
						UINT32 rate = 0;
						if (pLevelNode->Attribute("levelid",&iVal)){
							levelID = iVal;
						}
						
						if (pLevelNode->Attribute("probability",&iVal)){
							rate = iVal;
						}
						m_difficultLeitaiSwitchMap.m_id = 150002;
						m_difficultLeitaiSwitchMap.m_level2Rate[levelID] = rate;
						pLevelNode = pLevelNode->NextSiblingElement("level");
					}
				}
			}
			pSwitchNode = pSwitchNode->NextSiblingElement("switch");
		}
	}

	TiXmlElement *pRealtimeBattleNode = pRoot->FirstChildElement("Realtimebattle");
	if (pRealtimeBattleNode){
		TiXmlElement * pRewardNode	= pRealtimeBattleNode->FirstChildElement("Reward");
		while (pRewardNode){
			bool bWin = false;
			if (pRewardNode->Attribute("iswin",&iVal)){
				if (1 == iVal){
					bWin = true;
				}
			}	
			
			if (pRewardNode->Attribute("rewardtype",&iVal)){
				if (bWin){
					m_realTimeWinReward.m_itemID = iVal;	
				}
				else {
					m_realTimeLoseReward.m_itemID = iVal;
				}
			}

			if (pRewardNode->Attribute("num",&iVal)){
				if (bWin){
					m_realTimeWinReward.m_itemNum = iVal;
				}
				else {
					m_realTimeLoseReward.m_itemNum = iVal;	
				}
			}
			pRewardNode = pRewardNode->NextSiblingElement("Reward");	
		}
	}
	
	TiXmlElement *pOne2OneRewardNode = pRoot->FirstChildElement("PointReward");
	if (pOne2OneRewardNode){
		TiXmlElement *pRewardNode = pOne2OneRewardNode->FirstChildElement("reward");
		while (pRewardNode){
			bool bWin = false;
			UINT32 rewardNum = 0;
			UINT32 doubleRewardNum = 0;
			if (pRewardNode->Attribute("iswin",&iVal)){
				if (iVal > 0){
					bWin = true;
				}
			}
			
			if (pRewardNode->Attribute("rewardnum",&iVal)){
				rewardNum = iVal;	
			}
			
			if (pRewardNode->Attribute("doublerewardnum",&iVal)){
				doubleRewardNum = iVal;
			}

			if (bWin){
				m_one2OneWinReward.m_rewardNum = rewardNum;
				m_one2OneWinReward.m_doubleRewardNum = doubleRewardNum;
			}
			else {
				m_one2OneLoseReward.m_rewardNum = rewardNum;
				m_one2OneLoseReward.m_doubleRewardNum = doubleRewardNum;
			}
			pRewardNode = pRewardNode->NextSiblingElement("reward");
		}
	}

	TiXmlElement * pRandomBuffNode = pRoot->FirstChildElement("RandomBuff");
	if (pRandomBuffNode){
		TiXmlElement * pBuffNode = pRandomBuffNode->FirstChildElement("Buff");
		while (pBuffNode){
			UINT32 buffID = 0;
			UINT32 rate = 0;

			if (pBuffNode->Attribute("Buffid",&iVal)){
				buffID = iVal;
			}
			
			if (pBuffNode->Attribute("BuffRP",&iVal)){
				rate = iVal;
			}
			m_mobaBuffRates[buffID] = rate;
			m_mobaBuffTotalRate += rate;
			pBuffNode = pBuffNode->NextSiblingElement("Buff");
		}
	}

	TiXmlElement * pCollectNode = pRoot->FirstChildElement("Collect");
	if (pCollectNode){
		TiXmlElement * pTimeNode = pCollectNode->FirstChildElement("time");
		if (pTimeNode){
			if (pTimeNode->Attribute("num",&iVal)){
				m_mobaCollectTime = (iVal / 1000);
			}	
		}
	}

	TiXmlElement * pReviveTimeNode = pRoot->FirstChildElement("ReviveTime");
	if (pReviveTimeNode){
		TiXmlElement * pTimeNode = pReviveTimeNode->FirstChildElement("time");
		if (pTimeNode){
			if (pTimeNode->Attribute("num",&iVal)){
				m_mobaReliveTime = (iVal / 1000 );
			}	
		}
	}
	
	const char *szVal = NULL;

	TiXmlElement * pBattleOpenTimeNode = pRoot->FirstChildElement("BattleOpenTime");
	if (pBattleOpenTimeNode){
		TiXmlElement * pTimeNode = pBattleOpenTimeNode->FirstChildElement("time");
		while (pTimeNode){
			UINT8 type = 0;
			if (pTimeNode->Attribute("type",&iVal)){
				type = iVal;
			}
			
			if ((szVal = pTimeNode->Attribute("opentime"))){
				std::vector<std::string> strVec;
				g_stringtok(strVec,szVal,":");

				for (UINT16 i = 0;i < strVec.size();++i){
					std::vector<std::string> subStrVec;
					
					g_stringtok(subStrVec,strVec[i],"-");
					if (subStrVec.size() >= 2){
						UINT32 beginTime = (UINT32)atoi(subStrVec[0].c_str());
						UINT32 endTime = (UINT32)atoi(subStrVec[1].c_str());

						if (1 == type){
							m_realTimeOpenTimes.push_back(std::make_pair(beginTime,endTime));
						}
						else {
							m_mobaOpenTimes.push_back(std::make_pair(beginTime,endTime));
						}
					}
				}
			}
			pTimeNode = pTimeNode->NextSiblingElement("time");
		}
	}

	TiXmlElement *  pPointOpenTimeNode = pRoot->FirstChildElement("PointOpenTime");
	if (pPointOpenTimeNode){
		TiXmlElement *  pTimeNode = pPointOpenTimeNode->FirstChildElement("time");
		if (pTimeNode){
			if ((szVal = pTimeNode->Attribute("opentime"))){
				std::vector<std::string> strVec;
				g_stringtok(strVec,szVal,":");

				for (UINT16 i = 0;i < strVec.size();++i){
					std::vector<std::string> subStrVec;
					
					g_stringtok(subStrVec,strVec[i],"-");
					if (subStrVec.size() >= 2){
						UINT32 beginTime = (UINT32)atoi(subStrVec[0].c_str());
						UINT32 endTime = (UINT32)atoi(subStrVec[1].c_str());
						one2OneOpenTimes.push_back(std::make_pair(beginTime,endTime));
					}
				}
			}

			if ((szVal = pTimeNode->Attribute("doublepointtime"))){
				std::vector<std::string> strVec;
				g_stringtok(strVec,szVal,":");

				for (UINT16 i = 0;i < strVec.size();++i){
					std::vector<std::string> subStrVec;
					
					g_stringtok(subStrVec,strVec[i],"-");
					if (subStrVec.size() >= 2){
						UINT32 beginTime = (UINT32)atoi(subStrVec[0].c_str());
						UINT32 endTime = (UINT32)atoi(subStrVec[1].c_str());
						one2OneDoubleOpenTimes.push_back(std::make_pair(beginTime,endTime));
					}
				}
			}
		}
	}

	TiXmlElement * pChallengeBuffNode = pRoot->FirstChildElement("ChallengeBuff");
	if (pChallengeBuffNode){
		TiXmlElement * pDiamondEasyNode = pChallengeBuffNode->FirstChildElement("DiamondEasy");
		if (pDiamondEasyNode) {
			TiXmlElement * pBuffNode = pDiamondEasyNode->FirstChildElement("Buff");
			while (pBuffNode){
				stLeitaiBuffInfo buffInfo;
				if (pBuffNode->Attribute("ID",&iVal)){
					buffInfo.m_buffID = iVal;	
				}

				if (pBuffNode->Attribute("Type",&iVal)){
					buffInfo.m_type = iVal;	
				}

				if (pBuffNode->Attribute("BuffRP",&iVal)){
					buffInfo.m_rate = iVal;	
				}
				if (1 == buffInfo.m_type){
					m_leitaiBuffEasyMode.m_diamondRefreshBuffVecType1.push_back(buffInfo);
				}
				else {
					m_leitaiBuffEasyMode.m_diamondRefreshBuffVecType2.push_back(buffInfo);
				}
				pBuffNode = pBuffNode->NextSiblingElement("Buff");
			}
		}

		TiXmlElement * pAutoEasyNode = pChallengeBuffNode->FirstChildElement("AutoEasy");
		if (pAutoEasyNode){
			TiXmlElement * pBuffNode = pAutoEasyNode->FirstChildElement("Buff");
			while (pBuffNode){
				stLeitaiBuffInfo buffInfo;
				if (pBuffNode->Attribute("ID",&iVal)){
					buffInfo.m_buffID = iVal;	
				}

				if (pBuffNode->Attribute("Type",&iVal)){
					buffInfo.m_type = iVal;	
				}

				if (pBuffNode->Attribute("BuffRP",&iVal)){
					buffInfo.m_rate = iVal;	
				}
				if (1 == buffInfo.m_type){
					m_leitaiBuffEasyMode.m_autoRefreshBuffVecType1.push_back(buffInfo);
				}
				else {
					m_leitaiBuffEasyMode.m_autoRefreshBuffVecType2.push_back(buffInfo);
				}
				pBuffNode = pBuffNode->NextSiblingElement("Buff");
			}
		}

		TiXmlElement * pDiamondhardNode = pChallengeBuffNode->FirstChildElement("Diamondhard");
		if (pDiamondhardNode){
			TiXmlElement * pBuffNode = pDiamondhardNode->FirstChildElement("Buff");
			while (pBuffNode){
				stLeitaiBuffInfo buffInfo;
				if (pBuffNode->Attribute("ID",&iVal)){
					buffInfo.m_buffID = iVal;	
				}

				if (pBuffNode->Attribute("Type",&iVal)){
					buffInfo.m_type = iVal;	
				}

				if (pBuffNode->Attribute("BuffRP",&iVal)){
					buffInfo.m_rate = iVal;	
				}
				if (1 == buffInfo.m_type){
					m_leitaiBuffDiffMode.m_diamondRefreshBuffVecType1.push_back(buffInfo);
				}
				else {
					m_leitaiBuffDiffMode.m_diamondRefreshBuffVecType2.push_back(buffInfo);
				}
				pBuffNode = pBuffNode->NextSiblingElement("Buff");
			}
		}

		TiXmlElement * pAutohardNode = pChallengeBuffNode->FirstChildElement("Autohard");
		if (pAutohardNode){
			TiXmlElement * pBuffNode = pAutohardNode->FirstChildElement("Buff");
			while (pBuffNode){
				stLeitaiBuffInfo buffInfo;
				if (pBuffNode->Attribute("ID",&iVal)){
					buffInfo.m_buffID = iVal;	
				}

				if (pBuffNode->Attribute("Type",&iVal)){
					buffInfo.m_type = iVal;	
				}

				if (pBuffNode->Attribute("BuffRP",&iVal)){
					buffInfo.m_rate = iVal;	
				}
				if (1 == buffInfo.m_type){
					m_leitaiBuffDiffMode.m_autoRefreshBuffVecType1.push_back(buffInfo);
				}
				else {
					m_leitaiBuffDiffMode.m_autoRefreshBuffVecType2.push_back(buffInfo);
				}
				pBuffNode = pBuffNode->NextSiblingElement("Buff");
			}
		}
	}
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("简单模式下擂台奖励:boxID:%d",m_easyLeitaiReward.m_dropID);
		std::map<UINT16,stLeitaiRoundReward>::iterator it = m_easyLeitaiReward.m_roundReward.begin();
		for (;it != m_easyLeitaiReward.m_roundReward.end();++it){
			stLeitaiRoundReward & roundReward = it->second;
			Athena::logger->trace("每轮的奖励有:boxID:%d,item1:%d,num1:%d,rate1:%d,item2:%d,num2:%d,rate2:%d,item3:%d,num3:%d,rate3:%d,item4:%d,num4:%d,rate4:%d",roundReward.m_id,roundReward.m_itemID1,roundReward.m_num1,roundReward.m_rate1,roundReward.m_itemID2,roundReward.m_num2,roundReward.m_rate2,roundReward.m_itemID3,roundReward.m_num3,roundReward.m_rate3,roundReward.m_itemID4,roundReward.m_num4,roundReward.m_rate4);
		}

		Athena::logger->trace("复杂模式下擂台奖励:boxID:%d",m_difficultLeitaiReward.m_dropID);
		it = m_difficultLeitaiReward.m_roundReward.begin();
		for (;it != m_difficultLeitaiReward.m_roundReward.end();++it){
			stLeitaiRoundReward & roundReward = it->second;
			Athena::logger->trace("每轮的奖励有:boxID:%d,item1:%d,num1:%d,rate1:%d,item2:%d,num2:%d,rate2:%d,item3:%d,num3:%d,rate3:%d,item4:%d,num4:%d,rate4:%d",roundReward.m_id,roundReward.m_itemID1,roundReward.m_num1,roundReward.m_rate1,roundReward.m_itemID2,roundReward.m_num2,roundReward.m_rate2,roundReward.m_itemID3,roundReward.m_num3,roundReward.m_rate3,roundReward.m_itemID4,roundReward.m_num4,roundReward.m_rate4);
		}
		
		std::map<UINT16,stLeitaiMatch>::iterator subIt = m_easyLeitaiMatch.begin();
		for (;subIt != m_easyLeitaiMatch.end();++subIt){
			stLeitaiMatch & match = subIt->second;
			Athena::logger->trace("简单模式下擂台的匹配规则:id:%d,startRank:%d,endRank:%d,range:%d,randomMin:%d,randomMax:%d",match.m_id,match.m_startRank,match.m_endRank,match.m_range,match.m_randomMin,match.m_randomMax);
		}

		subIt = m_difficultLeitaiMatch.begin();
		for (;subIt != m_difficultLeitaiMatch.end();++subIt){
			stLeitaiMatch & match = subIt->second;
			Athena::logger->trace("复杂模式下擂台的匹配规则:id:%d,startRank:%d,endRank:%d,range:%d,randomMin:%d,randomMax:%d",match.m_id,match.m_startRank,match.m_endRank,match.m_range,match.m_randomMin,match.m_randomMax);
		}
		
		Athena::logger->trace("简单模式下的百分比参数");
		std::map<UINT16,UINT16>::iterator thdIt = m_easyLeitaiPercent.begin();
		for (;thdIt != m_easyLeitaiPercent.end();++thdIt){
			Athena::logger->trace("round:%d,percent:%d",thdIt->first,thdIt->second);
		}
	
		Athena::logger->trace("复杂模式下的百分比参数");
		thdIt = m_difficultLeitaiPercent.begin();
		for (;thdIt != m_difficultLeitaiPercent.end();++thdIt){
			Athena::logger->trace("round:%d,percent:%d",thdIt->first,thdIt->second);
		}
		
		Athena::logger->trace("简单模式下的关卡:");
		std::map<UINT32,UINT32>::iterator fourIt = m_easyLeitaiSwitchMap.m_level2Rate.begin();
		for (;fourIt != m_easyLeitaiSwitchMap.m_level2Rate.end();++fourIt){
			Athena::logger->trace("level:%d,rate:%d",fourIt->first,fourIt->second);
		}
		
		Athena::logger->trace("复杂模式下的关卡:");
		fourIt = m_difficultLeitaiSwitchMap.m_level2Rate.begin();
		for (;fourIt != m_difficultLeitaiSwitchMap.m_level2Rate.end();++fourIt){
			Athena::logger->trace("level:%d,rate:%d",fourIt->first,fourIt->second);
		}

		Athena::logger->trace("实时战场胜利一方奖励:itemID:%d,num:%d.失败一方奖励:itemID:%d,num:%d",m_realTimeWinReward.m_itemID,m_realTimeWinReward.m_itemNum,m_realTimeLoseReward.m_itemID,m_realTimeLoseReward.m_itemNum);

		Athena::logger->trace("一对一pk胜利奖励:reward:%d,doubleReward:%d,失败的一方奖励:reward:%d,doubleReward:%d",m_one2OneWinReward.m_rewardNum,m_one2OneWinReward.m_doubleRewardNum,m_one2OneLoseReward.m_rewardNum,m_one2OneLoseReward.m_doubleRewardNum);

		Athena::logger->trace("moba实时战场的随机buff:");
		std::map<UINT32,UINT32>::iterator fiveIt = m_mobaBuffRates.begin();
		for (;fiveIt != m_mobaBuffRates.end();++fiveIt){
			Athena::logger->trace("buffID:%d,rate:%d",fiveIt->first,fiveIt->second);	
		}

		Athena::logger->trace("moba 采集时间:%d,复活时间:%d",m_mobaCollectTime,m_mobaReliveTime);

		Athena::logger->trace("实时战场开启时间:");
		for (UINT16 i = 0;i < m_realTimeOpenTimes.size();++i){
			Athena::logger->trace("beginTime:%d,endTime:%d",m_realTimeOpenTimes[i].first,m_realTimeOpenTimes[i].second);	
		}

		Athena::logger->trace("moba开启时间:");
		for (UINT16 i = 0;i < m_mobaOpenTimes.size();++i){
			Athena::logger->trace("beginTime:%d,endTime:%d",m_mobaOpenTimes[i].first,m_mobaOpenTimes[i].second);	
		}

		Athena::logger->trace("1v1开启时间");
		for (UINT16 i = 0;i < one2OneOpenTimes.size();++i){
			Athena::logger->trace("beginTime:%d,endTime:%d",one2OneOpenTimes[i].first,one2OneOpenTimes[i].second);	
		}

		for (UINT16 i = 0;i < one2OneDoubleOpenTimes.size();++i){
			Athena::logger->trace("beginTime:%d,endTime:%d",one2OneDoubleOpenTimes[i].first,one2OneDoubleOpenTimes[i].second);	
		}

		Athena::logger->trace("擂台buff相关数据:");
		Athena::logger->trace("简单模式钻石刷新的:");
		for (UINT16 i = 0;i < m_leitaiBuffEasyMode.m_diamondRefreshBuffVecType1.size();++i){
			stLeitaiBuffInfo & buffInfo = m_leitaiBuffEasyMode.m_diamondRefreshBuffVecType1[i];
			Athena::logger->trace("buffID:%d,type:%d,rate:%d",buffInfo.m_buffID,buffInfo.m_type,buffInfo.m_rate);
		}
		
		for (UINT16 i = 0;i < m_leitaiBuffEasyMode.m_diamondRefreshBuffVecType2.size();++i){
			stLeitaiBuffInfo & buffInfo = m_leitaiBuffEasyMode.m_diamondRefreshBuffVecType2[i];
			Athena::logger->trace("buffID:%d,type:%d,rate:%d",buffInfo.m_buffID,buffInfo.m_type,buffInfo.m_rate);
		}
		
		for (UINT16 i = 0;i < m_leitaiBuffEasyMode.m_autoRefreshBuffVecType1.size();++i){
			stLeitaiBuffInfo & buffInfo = m_leitaiBuffEasyMode.m_autoRefreshBuffVecType1[i];
			Athena::logger->trace("buffID:%d,type:%d,rate:%d",buffInfo.m_buffID,buffInfo.m_type,buffInfo.m_rate);
		}
		
		for (UINT16 i = 0;i < m_leitaiBuffEasyMode.m_autoRefreshBuffVecType2.size();++i){
			stLeitaiBuffInfo & buffInfo = m_leitaiBuffEasyMode.m_autoRefreshBuffVecType2[i];
			Athena::logger->trace("buffID:%d,type:%d,rate:%d",buffInfo.m_buffID,buffInfo.m_type,buffInfo.m_rate);
		}

		Athena::logger->trace("困难模式下的相关数据:");
		for (UINT16 i = 0;i < m_leitaiBuffDiffMode.m_diamondRefreshBuffVecType1.size();++i){
			stLeitaiBuffInfo & buffInfo = m_leitaiBuffDiffMode.m_diamondRefreshBuffVecType1[i];
			Athena::logger->trace("buffID:%d,type:%d,rate:%d",buffInfo.m_buffID,buffInfo.m_type,buffInfo.m_rate);
		}
		
		for (UINT16 i = 0;i < m_leitaiBuffDiffMode.m_diamondRefreshBuffVecType2.size();++i){
			stLeitaiBuffInfo & buffInfo = m_leitaiBuffDiffMode.m_diamondRefreshBuffVecType2[i];
			Athena::logger->trace("buffID:%d,type:%d,rate:%d",buffInfo.m_buffID,buffInfo.m_type,buffInfo.m_rate);
		}
		
		for (UINT16 i = 0;i < m_leitaiBuffDiffMode.m_autoRefreshBuffVecType1.size();++i){
			stLeitaiBuffInfo & buffInfo = m_leitaiBuffDiffMode.m_autoRefreshBuffVecType1[i];
			Athena::logger->trace("buffID:%d,type:%d,rate:%d",buffInfo.m_buffID,buffInfo.m_type,buffInfo.m_rate);
		}
		
		for (UINT16 i = 0;i < m_leitaiBuffDiffMode.m_autoRefreshBuffVecType2.size();++i){
			stLeitaiBuffInfo & buffInfo = m_leitaiBuffDiffMode.m_autoRefreshBuffVecType2[i];
			Athena::logger->trace("buffID:%d,type:%d,rate:%d",buffInfo.m_buffID,buffInfo.m_type,buffInfo.m_rate);
		}

	}
#endif
	return true;
}
	
void ArenaConfig::parseReward(TiXmlElement *pNode,stLeitaiReward & reward)
{
	if (!pNode){
		return ;
	}
	
	INT32 iVal = 0;
	if (pNode->Attribute("dropid",&iVal)){
		reward.m_dropID = iVal;	
	}

	TiXmlElement *pRewardNode = pNode->FirstChildElement("Reward");
	while (pRewardNode){
		stLeitaiRoundReward round;
		if (pRewardNode->Attribute("id",&iVal)){
			round.m_id = iVal;
		}	
		
		if (pRewardNode->Attribute("rewardtype1",&iVal)){
			round.m_itemID1 = iVal;
		}	
	
		if (pRewardNode->Attribute("rewardnum1",&iVal)){
			round.m_num1 = iVal;
		}

		if (pRewardNode->Attribute("rewardprobability1",&iVal)){
			round.m_rate1 = iVal;
		}
		
		if (pRewardNode->Attribute("rewardtype2",&iVal)){
			round.m_itemID2 = iVal;
		}	
	
		if (pRewardNode->Attribute("rewardnum2",&iVal)){
			round.m_num2 = iVal;
		}

		if (pRewardNode->Attribute("rewardprobability2",&iVal)){
			round.m_rate2 = iVal;
		}
		
		if (pRewardNode->Attribute("rewardtype3",&iVal)){
			round.m_itemID3 = iVal;
		}	
	
		if (pRewardNode->Attribute("rewardnum3",&iVal)){
			round.m_num3 = iVal;
		}

		if (pRewardNode->Attribute("rewardprobability3",&iVal)){
			round.m_rate3 = iVal;
		}
	
		if (pRewardNode->Attribute("rewardtype4",&iVal)){
			round.m_itemID4 = iVal;
		}	
	
		if (pRewardNode->Attribute("rewardnum4",&iVal)){
			round.m_num4 = iVal;
		}

		if (pRewardNode->Attribute("rewardprobability4",&iVal)){
			round.m_rate4 = iVal;
		}
		reward.m_roundReward[round.m_id] = round;
		pRewardNode = pRewardNode->NextSiblingElement("Reward");
	}
}

void ArenaConfig::parseMatch(TiXmlElement *pNode,std::map<UINT16,stLeitaiMatch> & matchCnt)
{
	if (!pNode){
		return ;
	}
	INT32 iVal = 0;
	TiXmlElement *pMatchNode = pNode->FirstChildElement("match");
	while (pMatchNode){
		stLeitaiMatch match;
		if (pMatchNode->Attribute("id",&iVal)){
			match.m_id = iVal;
		}
		
		if (pMatchNode->Attribute("Startrank",&iVal)){
			match.m_startRank = iVal;
		}
		
		if (pMatchNode->Attribute("Endrank",&iVal)){
			match.m_endRank = iVal;
		}
		
		if (pMatchNode->Attribute("range",&iVal)){
			match.m_range = iVal;
		}
		
		if (pMatchNode->Attribute("randomMin",&iVal)){
			match.m_randomMin = iVal;
		}
		
		if (pMatchNode->Attribute("randomMax",&iVal)){
			match.m_randomMax = iVal;
		}
		matchCnt[match.m_id] = match;
		pMatchNode = pMatchNode->NextSiblingElement("match");
	}
}

UINT32 ArenaConfig::getHistoryRankReward(UINT16 oldRank,UINT16 newRank)
{
	UINT32 val1 = 0;
	UINT32  val2 = 0;
	
	std::map<UINT16,UINT32>::iterator it = m_rank2TotalReward.find(oldRank);
	if (it != m_rank2TotalReward.end()){
		val1 = it->second;
	}
	
	it = m_rank2TotalReward.find(newRank);
	if (it != m_rank2TotalReward.end()){
		val2 = it->second;	
	}
	
	return val2 > val1 ? val2 - val1 : 0;
}

ArenaConfig::stArenaEverydayReward * ArenaConfig::getArenaEverydayReward(UINT16 rank)
{
	std::map<UINT16,stArenaEverydayReward>::iterator it = m_everydayReward.begin();
	
	for (;it != m_everydayReward.end();++it){
		stArenaEverydayReward & one = it->second;
		if ((rank >= one.m_startRank) && (rank <=one.m_endRank)){
			return &one;
		}
	}
	return NULL;
}

ArenaConfig::stRealTimeReward * ArenaConfig::getRealTimeReward(bool win)
{
	if (win){
		return &m_realTimeWinReward;
	}
	else {
		return &m_realTimeLoseReward;
	}
}

ArenaConfig::stOne2OnePVPReward *  ArenaConfig::getOne2OnePVPReward(bool win)
{
	if (win){
		return &m_one2OneWinReward;
	}
	else {
		return &m_one2OneLoseReward;
	}
}

ArenaConfig::stLeitaiMatch * ArenaConfig::getMatchInfo(UINT8 mode,UINT16 rank)
{
	std::map<UINT16,stLeitaiMatch> * pMatchInfo = NULL;
		
	if (mode == 1){
		pMatchInfo = & m_easyLeitaiMatch;	
	}
	else {
		 pMatchInfo = & m_difficultLeitaiMatch; 
	}

	std::map<UINT16,stLeitaiMatch>::iterator it = pMatchInfo->begin();
	for (;it != pMatchInfo->end();++it){
		stLeitaiMatch & one = it->second; 
		if ((rank >= one.m_startRank) && (rank <= one.m_endRank)){
			return &it->second;	
		}
	}
	return NULL;
}

UINT16 ArenaConfig::getPercentCoef(UINT8 mode,UINT16 round)
{
	std::map<UINT16,UINT16> * pPercentMap = NULL;
	if (mode == 1){
		pPercentMap = & m_easyLeitaiPercent;	
	}
	else {
		pPercentMap = & m_difficultLeitaiPercent; 
	}
	std::map<UINT16,UINT16>::iterator it = pPercentMap->find(round);
	if (it != pPercentMap->end()){
		return it->second;
	}
	return 0;
}

UINT32 ArenaConfig::getMobaBuffID()
{
	INT32 buffID = g_selectByMap(m_mobaBuffRates,m_mobaBuffTotalRate);
	if (-1 != buffID){
		return buffID; 
	}
	else {
		return 0;
	}
}
	
bool ArenaConfig::isOne2OneOpen(UINT32 sec )
{
	for (UINT16 i = 0;i < one2OneOpenTimes.size();++i){
		std::pair<UINT32,UINT32> & one = one2OneOpenTimes[i];

		if ((sec >= one.first) && (sec <= one.second)){
			return true;	
		}
	}
	return false;
}

bool ArenaConfig::isOne2OneDoubleOpen(UINT32 sec)
{
	for (UINT16 i = 0; i < one2OneDoubleOpenTimes.size();++i){
		std::pair<UINT32,UINT32> & one = one2OneDoubleOpenTimes[i];
		
		if ((sec >= one.first) && (sec <= one.second)){
			return true;	
		}
	}
	return false;
}

bool ArenaConfig::isRealTimeOpen(UINT32 sec)
{
	for (UINT16 i = 0;i < m_realTimeOpenTimes.size();++i){
		std::pair<UINT32,UINT32> & one = m_realTimeOpenTimes[i];

		if ((sec >= one.first) && (sec <= one.second)){
			return true;	
		}
	}
	return false;
}

bool ArenaConfig::isMobaOpen(UINT32 sec)
{
	for (UINT16 i = 0;i < m_mobaOpenTimes.size();++i){
		std::pair<UINT32,UINT32> & one = m_mobaOpenTimes[i];

		if ((sec >= one.first) && (sec <= one.second)){
			return true;	
		}
	}
	return false;
}

UINT16 ArenaConfig::randomLeitaiBuffID(bool bEasy,bool bAuto,UINT8 type)
{
	std::vector<stLeitaiBuffInfo> * leitaiBuffVec = NULL;

	if (bEasy){
		if (bAuto){
			if (1 == type){
				leitaiBuffVec = & m_leitaiBuffEasyMode.m_autoRefreshBuffVecType1;	
			}
			else {
				leitaiBuffVec = & m_leitaiBuffEasyMode.m_autoRefreshBuffVecType2;
			}
		}
		else{
			if (1 == type){
				leitaiBuffVec = & m_leitaiBuffEasyMode.m_diamondRefreshBuffVecType1;	
			}
			else {
				leitaiBuffVec = & m_leitaiBuffEasyMode.m_diamondRefreshBuffVecType2;
			}
		}
	}
	else {
		if (bAuto){
			if (1 == type){
				leitaiBuffVec = & m_leitaiBuffDiffMode.m_autoRefreshBuffVecType1;	
			}
			else {
				leitaiBuffVec = & m_leitaiBuffDiffMode.m_autoRefreshBuffVecType2;
			}
		}
		else{
			if (1 == type){
				leitaiBuffVec = & m_leitaiBuffDiffMode.m_diamondRefreshBuffVecType1;	
			}
			else {
				leitaiBuffVec = & m_leitaiBuffDiffMode.m_diamondRefreshBuffVecType2;
			}
		}
	}

	UINT32 totalRate = 0;
	 std::map<UINT32,UINT32> leitaiBuffs;
	for (UINT16 i = 0; i < leitaiBuffVec->size();++i){
		stLeitaiBuffInfo & info = (*leitaiBuffVec)[i];
		totalRate += info.m_rate;
		leitaiBuffs.insert(std::make_pair(info.m_buffID,info.m_rate));		
	}
	
	INT32 buffID = g_selectByMap(leitaiBuffs,totalRate);
	if (-1 != buffID){
		return buffID;
	}
	else {
		return 0;
	}
}
