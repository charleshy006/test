#include "SevenDayActivityMgr.h"
#include "GameUser.h"
#include "SevenDayActivity.h"
#include "Misc.pb.h"

CSevenDayActivityMgr::CSevenDayActivityMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_one2OneWinNum = 0;
	m_heishiRefreshNum = 0;
	m_realTimePkNum = 0;
	m_drawNum = 0;
}

CSevenDayActivityMgr::~CSevenDayActivityMgr()
{

}

UINT32 CSevenDayActivityMgr::getGoalValue(UINT8 day,UINT8 goalID,UINT32 goalValue,UINT32 extraVal)
{
	switch (goalID){
		case MiscProto::eSD_Goal_Login_Num  :				//登录次数
			{
				return 1;
			}
			break;
		case MiscProto::eSD_Goal_Total_Recharge :			//充值数量
			{
				return m_pOwner->getRechargeNum();
			}
			break;
		case MiscProto::eSD_Goal_GitfBag_Price :			//半价礼包价格
			{
				stSDGoalInfo * pGoalInfo = SevenDayActivityCfg::getSingleton().getGoalInfoByGoalID(day,MiscProto::eSD_GoalType_Gift,goalID);
				if (pGoalInfo){
					return pGoalInfo->m_value;
				}
				return 0;
			}
			break;
		case MiscProto::eSD_Goal_MainTask_Star_Num : 		//主线任务星星数
			 {
			 	return m_pOwner->m_levelMgr.getStarNum();
			 }
			break;
		case MiscProto::eSD_Goal_All_Skill_Raise_Num :		//所有技能强化等级
			{
				return m_pOwner->m_skillMgr.getSkillNumByRaiseLv(extraVal);
			}
			break;
		case MiscProto::eSD_Goal_Arena_History_Rank :		//天梯历史排名
			{
				return m_pOwner->m_arenaHistoryRank;	
			}
			break;
		case MiscProto::eSD_Goal_All_Equip_Refine_Lv :		//所有装备精炼等级
			{
				return m_pOwner->m_bagMgr.m_equipBag.getTotalRefineLv(extraVal);
			}
			break;
		case MiscProto::eSD_Goal_Team_Copymap	:			//组队副本进度
			{
				stSDGoalInfo * pGoalInfo = SevenDayActivityCfg::getSingleton().getGoalInfoByGoalIDAndExtraVal(day,MiscProto::eSD_GoalType_Module1,goalID,extraVal);
				if (pGoalInfo){
					return m_pOwner->m_levelMgr.getRushNum(pGoalInfo->m_extraVal);
				}
				return 0;
			}
			break;
		case MiscProto::eSD_Goal_All_Equip_Raise_Lv :		//所有装备强化等级
			{
				return m_pOwner->m_bagMgr.m_equipBag.getTotalRaiseLv(extraVal);
			}
			break;
		case MiscProto::eSD_Goal_One_One_Win_Num :			//1v1胜利次数
			{
				return m_one2OneWinNum;
			}
			break;
		case MiscProto::eSD_Goal_Heishi_Refresh_Num :		//黑市刷新次数
			{
				return m_heishiRefreshNum;
			}
			break;
		case MiscProto::eSD_Goal_RealTime_PK_Num :			//实时战场次数
			{
				return m_realTimePkNum;
			}
			break;
		case MiscProto::eSD_Goal_Fuwen_Purple_Num :		//符文紫色数量
			{
				return m_pOwner->m_bagMgr.m_equipBag.purpleFuwenNum();
			}
			break;
		case MiscProto::eSD_Goal_Guild_Hall_Lv :			//公会等级
			{
				return m_pOwner->m_guildLv;
			}
			break;
		case MiscProto::eSD_Goal_Role_Lv :					//玩家等级
			{
				return m_pOwner->getLevel();
			}
			break;
		case MiscProto::eSD_Goal_Battle_Power :			//战力
			{
				return m_pOwner->getBattlePower();
			}
			break;
		case MiscProto::eSD_Goal_Draw_num :				//抽奖次数
			{
				return m_drawNum;
			}
			break;
		case MiscProto::eSD_Goal_Hero_EvolutionLv :
			{
				stHeroInfoCBEvolution cb(extraVal);	
				m_pOwner->m_heroMgr.exeEveryOne(cb);
				return cb.m_num;
			}
			break;
		case MiscProto::eSD_Goal_Consume_Diamond :
			{
				return m_pOwner->m_roleInfo.m_sevenDayConsumeDiamond;
			}
			break;
	}
	return 0;
}

bool   CSevenDayActivityMgr::hasGotReward(UINT8 day,UINT8 goalType,UINT16 id)
{
	std::map<UINT8,std::map<UINT8,std::set<UINT8> > >::iterator it = m_rewardInfos.find(day);
	if (it == m_rewardInfos.end()){
		return false;
	}
	
	std::map<UINT8,std::set<UINT8> >::iterator subIt = it->second.find(goalType);
	if (subIt == it->second.end()){
		return false;
	}
	
	if (subIt->second.find(id) == subIt->second.end()){
		return false;
	}

	return true;	
}

void CSevenDayActivityMgr::send7DayActivityInfo(UINT8 day)
{
	MiscProto::stRet7DayActivityInfo retCmd;
	retCmd.set_day(day);
	
	stSDActivityInfo *pDayInfo =  SevenDayActivityCfg::getSingleton().getDayInfo(day);
	if (pDayInfo){
		UINT8 eGoalType = MiscProto::eSD_GoalType_Login;
		for (;eGoalType <= MiscProto::eSD_GoalType_Module2; ++eGoalType){
			std::map<UINT8,stSDGoalTypeInfo>::iterator it = pDayInfo->m_goalTypes.find(eGoalType);
			if (it != pDayInfo->m_goalTypes.end()){
				stSDGoalTypeInfo & goalType = it->second;
				std::map<UINT16,stSDGoalInfo>::iterator subIt = goalType.m_goals.begin();	
				for (;subIt != goalType.m_goals.end();++subIt){
					stSDGoalInfo & goalInfo = subIt->second;
					MiscProto::stSynSDGoalInfo * pSyn = NULL;
					if (MiscProto::eSD_GoalType_Login == eGoalType){
						pSyn = retCmd.add_logingoal();	
					}
					else if (MiscProto::eSD_GoalType_Gift == eGoalType){
						pSyn = retCmd.add_gifbaggoal();
					}
					else if (MiscProto::eSD_GoalType_Module1 == eGoalType){
						pSyn = retCmd.add_module1goal();
					}
					else if (MiscProto::eSD_GoalType_Module2 == eGoalType){
						pSyn = retCmd.add_module2goal();
					}

					if (pSyn){
						pSyn->set_id(goalInfo.m_id);
						pSyn->set_cond(goalInfo.m_cond);
						pSyn->set_value(getGoalValue(day,goalInfo.m_cond,goalInfo.m_value,goalInfo.m_extraVal));
						if (eGoalType == MiscProto::eSD_GoalType_Login && goalInfo.m_cond == MiscProto::eSD_Goal_Login_Num){
							if (m_loginDays.find(day) == m_loginDays.end()){
								pSyn->set_value(0);
							}
						}
						pSyn->set_hasgotreward(hasGotReward(day,goalType.m_goalType,goalInfo.m_id));
					}
				}
			}
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}

void   CSevenDayActivityMgr::setGotReward(UINT8 day,UINT8 goalType,UINT16 id)
{
	m_rewardInfos[day][goalType].insert(id);
}

void CSevenDayActivityMgr::got7DayActivityReward(UINT8 day,UINT8 goalType,UINT16 id)
{
	MiscProto::stSevenDayRewardRet retCmd;
	retCmd.set_goaltype(goalType);
	retCmd.set_id(id);
	retCmd.set_day(day);
	
	if (day > m_pOwner->getCreateRoleDay()){
		Athena::logger->error("还没有到时间不能领取七天活动(account:%s,name:%s,id:%d)",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
		return ;
	}

	stSDGoalInfo *pGoalInfo = SevenDayActivityCfg::getSingleton().getGoalInfo(day,goalType,id);
	if (!pGoalInfo){
		retCmd.set_result(MiscProto::stSevenDayRewardRet::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return ;	
	}
	
	if (hasGotReward(day,goalType,id)){
		retCmd.set_result(MiscProto::stSevenDayRewardRet::eHas_Got_Reward);
		m_pOwner->sendProto2Me(retCmd);
		return ;	
	}

	if (pGoalInfo->m_cond == MiscProto::eSD_Goal_GitfBag_Price){//如果是半价礼包,要检查扣钱
		if (!m_pOwner->removeResource(eResource_Diamond,pGoalInfo->m_value,"七天活动")){
			retCmd.set_result(MiscProto::stSevenDayRewardRet::eNot_Enough_Diamond);
			m_pOwner->sendProto2Me(retCmd);
			return ;	
		}
	}
	else if (pGoalInfo->m_cond == MiscProto::eSD_Goal_Arena_History_Rank){
		if (getGoalValue(day,pGoalInfo->m_cond,pGoalInfo->m_value,pGoalInfo->m_extraVal) > pGoalInfo->m_value){
			retCmd.set_result(MiscProto::stSevenDayRewardRet::eCond_Not_Meet);
			m_pOwner->sendProto2Me(retCmd);
			return ;	
		}	
	}
	else if (pGoalInfo->m_cond == MiscProto::eSD_Goal_Login_Num){//如果是登录奖励
		if (m_loginDays.find(day) == m_loginDays.end()){
			retCmd.set_result(MiscProto::stSevenDayRewardRet::eCond_Not_Meet);
			m_pOwner->sendProto2Me(retCmd);
			return ;	
		}	
	}
	else if (getGoalValue(day,pGoalInfo->m_cond,pGoalInfo->m_value,pGoalInfo->m_extraVal) < pGoalInfo->m_value){
		retCmd.set_result(MiscProto::stSevenDayRewardRet::eCond_Not_Meet);
		m_pOwner->sendProto2Me(retCmd);
		return ;	
	}
	
		
	if (m_pOwner->m_bagMgr.getMainFreeSlotNum() < m_pOwner->m_boxMgr.needSlotNum(pGoalInfo->m_boxID)){
		retCmd.set_result(MiscProto::stSevenDayRewardRet::eBag_Full);
		m_pOwner->sendProto2Me(retCmd);
		return ;	
	}

	
	setGotReward(day,goalType,id);
	retCmd.set_result(MiscProto::stSevenDayRewardRet::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
	m_pOwner->m_boxMgr.openBox(pGoalInfo->m_boxID);
}
	
UINT32 CSevenDayActivityMgr::getSaveSize()
{
	UINT32 offset = 0;
	//版本号
	offset += sizeof(UINT32);
	//one2one胜利次数
	offset += sizeof(m_one2OneWinNum);	
	//黑市刷新次数
	offset += sizeof(m_heishiRefreshNum);
	//实时战场次数
	offset += sizeof(m_realTimePkNum);
	//抽奖次数
	offset += sizeof(m_drawNum);
	//登录天数
	offset += sizeof(UINT8) * (1 + m_loginDays.size());
	
	//领取奖励信息
	offset += sizeof(UINT8);
	std::map<UINT8,std::map<UINT8,std::set<UINT8> > >::iterator it = m_rewardInfos.begin();
	for (;it != m_rewardInfos.end();++it){
		offset += sizeof(UINT8);
		offset += sizeof(UINT8);
		
		std::map<UINT8,std::set<UINT8> >::iterator subIt = it->second.begin();
		for (;subIt != it->second.end();++subIt){
			offset += sizeof(UINT8);
			offset += sizeof(UINT8);
			offset += (sizeof(UINT8) * subIt->second.size());
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("CSevenDayActivityMgr::getSaveSize 玩家:%s,id:%d下线保存了:%d数据",m_pOwner->getName(),m_pOwner->getID(),offset);
#endif
	return offset;
}

UINT32 CSevenDayActivityMgr::save(byte * buf)
{
	UINT32 offset = 0;
	
	//版本号
	*(UINT32 *)(buf + offset) = s_SEVENDAY_DATA_VERSION;
	offset += sizeof(UINT32);
	
	//one2one 胜利次数
	*(UINT32 *)(buf + offset) = m_one2OneWinNum;
	offset += sizeof(UINT32);
	
	//黑市刷新次数
	*(UINT32 *)(buf + offset) = m_heishiRefreshNum;
	offset += sizeof(UINT32);
	
	//实时战斗次数
	*(UINT32 *)(buf + offset) = m_realTimePkNum;
	offset += sizeof(UINT32);
	
	//抽奖次数
	*(UINT32 *)(buf + offset) = m_drawNum;
	offset += sizeof(UINT32);
	
	//登录的天数
	UINT8 loginDayNum = m_loginDays.size();
	*(UINT8 *)(buf + offset) = loginDayNum;
	offset += sizeof(UINT8);	
	std::set<UINT8>::iterator loginIt =  m_loginDays.begin();
	for (;loginIt != m_loginDays.end();++loginIt){
		*(UINT8 *)(buf + offset) = *loginIt;
		offset += sizeof(UINT8);
	}

	//保存天数
	*(UINT8 *)(buf + offset) = m_rewardInfos.size();
	offset += sizeof(UINT8);
	std::map<UINT8,std::map<UINT8,std::set<UINT8> > >::iterator it = m_rewardInfos.begin();

	for (;it != m_rewardInfos.end();++it){
		//保存第几天
		*(UINT8 *)(buf + offset) = it->first;
		offset += sizeof(UINT8);

		//保存类型数量
		*(buf + offset) = it->second.size();
		offset += sizeof(UINT8);

		std::map<UINT8,std::set<UINT8> >::iterator subIt = it->second.begin();
			
		for (;subIt != it->second.end();++subIt){
			//类型
			*(UINT8 *)(buf + offset) = subIt->first;
			offset += sizeof(UINT8);
			
			//目标数量
			*(UINT8 *)(buf + offset) = subIt->second.size();
			offset += sizeof(UINT8);

			std::set<UINT8>::iterator thdIt = subIt->second.begin();
			for (;thdIt != subIt->second.end();++thdIt){
				*(UINT8 *)(buf + offset) = *thdIt;
				offset += sizeof(UINT8);
			}
		}
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("CSevenDayActivityMgr::save 玩家:%s,id:%d下线保存了:%d数据",m_pOwner->getName(),m_pOwner->getID(),offset);
#endif
	return offset;
}

UINT32 CSevenDayActivityMgr::load(byte * buf)
{
	UINT32 offset = 0;
	
	UINT32 version = *(UINT32 *)(buf + offset);
	offset += sizeof(UINT32);

	if (s_SEVENDAY_DATA_VERSION == version){
		//one2one胜利次数
		m_one2OneWinNum = *(UINT32 *)(buf + offset);
		offset += sizeof(UINT32);
		//黑市刷新次数
		m_heishiRefreshNum = *(UINT32 *)(buf + offset);
		offset += sizeof(UINT32);
		//实时pk次数
		m_realTimePkNum = *(UINT32 *)(buf + offset);
		offset += sizeof(UINT32);
		//抽奖次数
		m_drawNum = *(UINT32 *)(buf + offset);
		offset += sizeof(UINT32);

		//登录的天数
		UINT8 loginDayNum = *(UINT8*)(buf + offset);
		offset += sizeof(UINT8);
		for (UINT8 i = 0;i < loginDayNum; ++i){
			m_loginDays.insert(*(UINT8*)(buf + offset));
			offset += sizeof(UINT8);
		}

		//天数
		UINT8  days = *(UINT8 *)(buf + offset);
		offset += sizeof(UINT8);

		for (UINT8 i = 0; i < days;++i){
			//第几天
			UINT8 day = *(UINT8 *)(buf + offset);
			offset += sizeof(UINT8);

			std::map<UINT8,std::set<UINT8> > & dayRewardInfo = m_rewardInfos[day]; 
			//目标类型个数
			UINT8 goalTypeNum = *(UINT8 *)(buf + offset);
			offset += sizeof(UINT8);

			for (UINT8 j = 0;j < goalTypeNum;++j){
				//目标类型
				UINT8 goalType = *(UINT8 *)(buf + offset);
				offset += sizeof(UINT8);
				std::set<UINT8> & rewardIDs = dayRewardInfo[goalType];

				//id数量
				UINT8 idNum = *(UINT8 *)(buf + offset);
				offset += sizeof(UINT8);

				for (UINT8 k = 0;k < idNum;++k){
					UINT8 id = *(UINT8 *)(buf + offset);
					offset += sizeof(UINT8);
					rewardIDs.insert(id);
				}
			}
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("CSevenDayActivityMgr::load 玩家:%s,id:%d下线保存了:%d数据",m_pOwner->getName(),m_pOwner->getID(),offset);
#endif
	return offset;
}

void CSevenDayActivityMgr::userOnline()
{
	MiscProto::stSevenDayHasRewardTip retCmd;
	retCmd.set_day(m_pOwner->getCreateRoleDay());
	bool bBreak = false;
	for (UINT8 day = 1;(day <=7) && (day <= m_pOwner->getCreateRoleDay()) ; ++day){
		stSDActivityInfo *pDayInfo =  SevenDayActivityCfg::getSingleton().getDayInfo(day);
		if (pDayInfo){
			UINT8 eGoalType = MiscProto::eSD_GoalType_Login;
			for (;eGoalType <= MiscProto::eSD_GoalType_Module2; ++eGoalType){
				std::map<UINT8,stSDGoalTypeInfo>::iterator it = pDayInfo->m_goalTypes.find(eGoalType);
				if (it != pDayInfo->m_goalTypes.end()){
					stSDGoalTypeInfo & goalType = it->second;
					std::map<UINT16,stSDGoalInfo>::iterator subIt = goalType.m_goals.begin();	
					for (;subIt != goalType.m_goals.end();++subIt){
						stSDGoalInfo & goalInfo = subIt->second;
						if (getGoalValue(day,goalInfo.m_cond,goalInfo.m_value,goalInfo.m_extraVal) >  goalInfo.m_value){
							if (!hasGotReward(day,goalType.m_goalType,goalInfo.m_id)){
								retCmd.set_hasreward(true);
								bBreak = true;
								break;
							}	
						}
					}
				}
				if (bBreak){
					break;
				}
			}
		}
		if (bBreak){
			break;
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}

void CSevenDayActivityMgr::addLoginDay(UINT32 day)
{
	if (day > s_MAX_REWARD_DAY){
		return ;
	}
	m_loginDays.insert(day);
}

