#include "RoleExtBinMgr.h"
#include "Log4cxxHelp.h"
#include "GameUser.h"

bool RoleExtBinMgr::isNeedSave(byte s)
{
	switch (s)
	{
		case ROLE_EXTBIN_TEST :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_ONLINETIME :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_RECOVERSPTIME :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYINVESTPLAN :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_CURRENT_TASKID :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_HISTORY_ARENA_RANK :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_SYSTEM_SETTING :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_ONLINE_REWARD_PASSTIME :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_COMPESATIONID :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_SEVENDAY_ACTIVITY :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_CUMULOGIN :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_LUCKSHOP :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_LIMIT_FASHION :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_CHANGE_NAME_COUNT :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYSUMMONPLAN :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYSUMMON_NUM :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYSUMMONPLAN_FREE :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYSUMMONPLAN_RECHARGE :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYLIMITSUMMON_NUM :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN_FREE :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN_RECHARGE :
			{
				return true;
			}
			break;
		case ROLE_EXTBIN_BUYTREASUREBOX_COUNT :
			{
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

UINT32 RoleExtBinMgr::getSaveSize(byte s)
{
	switch (s) {
		case ROLE_EXTBIN_TEST :
			{
				return sizeof(m_test);
			}
			break;
		case ROLE_EXTBIN_ONLINETIME :
			{
				return sizeof(m_pOwner->m_onlineTime);
			}
			break;
		case ROLE_EXTBIN_RECOVERSPTIME :
			{
				return sizeof(m_pOwner->m_recoverSPLilianTime);
			}
			break;
		case ROLE_EXTBIN_BUYINVESTPLAN :
			{
				return sizeof(m_pOwner->m_buyInvestPlan);
			}
			break;
		case ROLE_EXTBIN_CURRENT_TASKID :
			{
				return sizeof(m_pOwner->m_currentTaskID) + sizeof(m_pOwner->m_currentTaskStage);
			}
			break;
		case ROLE_EXTBIN_HISTORY_ARENA_RANK :
			{
				return sizeof(m_pOwner->m_arenaHistoryRank);
			}
			break;
		case ROLE_EXTBIN_SYSTEM_SETTING :
			{
				return sizeof(m_pOwner->m_systemSetting);
			}
			break;
		case ROLE_EXTBIN_ONLINE_REWARD_PASSTIME :
			{
				return sizeof(UINT32);
			}
			break;
		case ROLE_EXTBIN_COMPESATIONID :
			{
				return sizeof(m_pOwner->m_compensationID);
			}
			break;
		case ROLE_EXTBIN_CHANGE_NAME_COUNT :
			{
				return sizeof(m_pOwner->m_changeNameNum);
			}
			break;
		case ROLE_EXTBIN_SEVENDAY_ACTIVITY :
			{
				return m_pOwner->m_sevenDayActivityMgr.getSaveSize();
			}
			break;
		case ROLE_EXTBIN_CUMULOGIN :
			{
				return sizeof(m_pOwner->m_yestodayFirstLoginTime) + sizeof(m_pOwner->m_cumulativeLogin);
			}
			break;
		case ROLE_EXTBIN_LUCKSHOP :
			{
				return m_pOwner->m_luckShopMgr.getSaveSize();
			}
			break;
		case ROLE_EXTBIN_LIMIT_FASHION :
			{
				return m_pOwner->m_limitFashionDrawMgr.getSaveSize();
			}
			break;
		case ROLE_EXTBIN_BUYSUMMONPLAN :
			{
				return sizeof(m_pOwner->m_buySummonPlan);
			}
			break;
		case ROLE_EXTBIN_BUYSUMMON_NUM :
			{
				return sizeof(m_pOwner->m_buySummonNum);
			}
			break;		
		case ROLE_EXTBIN_BUYSUMMONPLAN_FREE :
			{
				return sizeof(m_pOwner->m_freeSummonSet);
			}
			break;	
		case ROLE_EXTBIN_BUYSUMMONPLAN_RECHARGE :
			{
				return sizeof(m_pOwner->m_rechargeSummonSet);
			}
			break;	
		case ROLE_EXTBIN_BUYLIMITSUMMON_NUM :
			{
				return sizeof(m_pOwner->m_buyLimitSummonNum);
			}
			break;	
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN :
			{
				return sizeof(m_pOwner->m_buyLimitSummonPlan);
			}
			break;		
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN_FREE :
			{
				return sizeof(m_pOwner->m_freeLimitSummonSet);
			}
			break;	
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN_RECHARGE :
			{
				return sizeof(m_pOwner->m_rechargeLimitSummonSet);
			}
			break;		
		case ROLE_EXTBIN_BUYTREASUREBOX_COUNT :
			{
				return sizeof(m_pOwner->m_buyTreasureCount);
			}
			break;		
		default:
			break;
	}
	return 0;
}

UINT32 RoleExtBinMgr::doSaveAction(byte* data, byte s, UINT32 dataSize)
{
	stSaveItem* index = (stSaveItem*)data;
	index->m_scheme = s;
	index->m_count = dataSize;
	if(index->m_count == 0){
		return index->getSize();
	}

	switch ( s ) {
		case ROLE_EXTBIN_TEST :
			{
				bcopy(&m_test,index->m_data,dataSize);
			}
			break;
		case ROLE_EXTBIN_ONLINETIME :
			{
				*(UINT32 *)(index->m_data) = m_pOwner->m_onlineTime;
			}
			break;
		case ROLE_EXTBIN_RECOVERSPTIME :
			{
				*(UINT32 *)(index->m_data) = m_pOwner->m_recoverSPLilianTime;
			}
			break;
		case ROLE_EXTBIN_BUYINVESTPLAN :
			{
			   *(UINT8 *)(index->m_data) = m_pOwner->m_buyInvestPlan;
			}
			break;
		case ROLE_EXTBIN_CURRENT_TASKID :
			{
				UINT32 offset = 0;
				*(UINT32 *)(index->m_data) = m_pOwner->m_currentTaskID;
				offset += sizeof(m_pOwner->m_currentTaskID);
				*(UINT32 *)(index->m_data + offset) = m_pOwner->m_currentTaskStage;
			}
			break;
		case ROLE_EXTBIN_HISTORY_ARENA_RANK :
			{
				*(UINT32 *)(index->m_data) = m_pOwner->m_arenaHistoryRank;
			}
			break;
		case ROLE_EXTBIN_SYSTEM_SETTING :
			{
				*(UINT64 *)(index->m_data) = m_pOwner->m_systemSetting;
			}
			break;
		case ROLE_EXTBIN_ONLINE_REWARD_PASSTIME :
			{
				*(UINT64 *)(index->m_data) = m_pOwner->m_rewardSys.getOnlineRewardPassTime();
			}
			break;
		case ROLE_EXTBIN_COMPESATIONID :
			{
				*(UINT8 *)(index->m_data) = m_pOwner->m_compensationID;
			}
			break;
		case ROLE_EXTBIN_CHANGE_NAME_COUNT :
			{
				*(UINT32 *)(index->m_data) = m_pOwner->m_changeNameNum;
			}
			break;
		case ROLE_EXTBIN_SEVENDAY_ACTIVITY :
			{
				m_pOwner->m_sevenDayActivityMgr.save(index->m_data);
			}
			break;
		case ROLE_EXTBIN_CUMULOGIN :
			{
				UINT32 offset = 0;
				*(UINT32 *)(index->m_data) = m_pOwner->m_yestodayFirstLoginTime;
				offset += sizeof(m_pOwner->m_yestodayFirstLoginTime);
				*(UINT16 *)(index->m_data + offset) = m_pOwner->m_cumulativeLogin;
				offset += sizeof(m_pOwner->m_cumulativeLogin);
			}
			break;
		case ROLE_EXTBIN_LUCKSHOP :
			{
				m_pOwner->m_luckShopMgr.save(index->m_data);
			}
			break;
		case ROLE_EXTBIN_LIMIT_FASHION :
			{
				m_pOwner->m_limitFashionDrawMgr.save(index->m_data);
			}
			break;
		case ROLE_EXTBIN_BUYSUMMONPLAN :
			{
			   *(UINT8 *)(index->m_data) = m_pOwner->m_buySummonPlan;
			}
			break;		
		case ROLE_EXTBIN_BUYSUMMON_NUM :
			{
			   *(UINT8 *)(index->m_data) = m_pOwner->m_buySummonNum;
			}
			break;			
		case ROLE_EXTBIN_BUYSUMMONPLAN_FREE :
			{
				UINT32 ret = 0;
				*(UINT16*)(index->m_data + ret) = m_pOwner->m_freeSummonSet.size();
				ret += sizeof(UINT16);

				std::set<UINT16>::iterator it = m_pOwner->m_freeSummonSet.begin();
				for (;it != m_pOwner->m_freeSummonSet.end();++it){
					*(UINT16 *)(index->m_data + ret) = *it;
					ret += sizeof(UINT16);
				}
			}
			break;	
		case ROLE_EXTBIN_BUYSUMMONPLAN_RECHARGE :
			{
				UINT32 ret = 0;
				*(UINT16*)(index->m_data + ret) = m_pOwner->m_rechargeSummonSet.size();
				ret += sizeof(UINT16);

				std::set<UINT16>::iterator it = m_pOwner->m_rechargeSummonSet.begin();
				for (;it != m_pOwner->m_rechargeSummonSet.end();++it){
					*(UINT16 *)(index->m_data + ret) = *it;
					ret += sizeof(UINT16);
				}
			}
			break;	
		case ROLE_EXTBIN_BUYLIMITSUMMON_NUM :
			{
			   *(UINT8 *)(index->m_data) = m_pOwner->m_buyLimitSummonNum;
			}
			break;		
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN :
			{
			   *(UINT8 *)(index->m_data) = m_pOwner->m_buyLimitSummonPlan;
			}
			break;	
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN_FREE :
			{
				UINT32 ret = 0;
				*(UINT16*)(index->m_data + ret) = m_pOwner->m_freeLimitSummonSet.size();
				ret += sizeof(UINT16);

				std::set<UINT16>::iterator it = m_pOwner->m_freeLimitSummonSet.begin();
				for (;it != m_pOwner->m_freeLimitSummonSet.end();++it){
					*(UINT16 *)(index->m_data + ret) = *it;
					ret += sizeof(UINT16);
				}
			}
			break;	
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN_RECHARGE :
			{
				UINT32 ret = 0;
				*(UINT16*)(index->m_data + ret) = m_pOwner->m_rechargeLimitSummonSet.size();
				ret += sizeof(UINT16);

				std::set<UINT16>::iterator it = m_pOwner->m_rechargeLimitSummonSet.begin();
				for (;it != m_pOwner->m_rechargeLimitSummonSet.end();++it){
					*(UINT16 *)(index->m_data + ret) = *it;
					ret += sizeof(UINT16);
				}
			}
			break;		
		case ROLE_EXTBIN_BUYTREASUREBOX_COUNT :
			{
				UINT32 ret = 0;
				*(UINT32 *)(index->m_data + ret) = (UINT32)m_pOwner->m_buyTreasureCount.size();
				ret += sizeof(UINT32);
				for (std::map<UINT32, UINT32>::iterator it = m_pOwner->m_buyTreasureCount.begin();
						it != m_pOwner->m_buyTreasureCount.end(); it++) {
					*(UINT32 *)(index->m_data + ret) = (UINT32)it->first;
					ret += sizeof(UINT32);
					*(UINT32 *)(index->m_data + ret) = (UINT32)it->second;
					ret += sizeof(UINT32);
				}


				// UINT32 ret = 0;
				// *(UINT32 *)(index->m_data + ret) = (UINT32)m_pOwner->m_buyTreasureCount.size();
				// ret += sizeof(UINT32);
				// for (std::map<UINT32, UINT32>::iterator it = m_pOwner->m_buyTreasureCount.begin();
				// 		it != m_pOwner->m_buyTreasureCount.end(); it++) {
				// 	*(UINT32 *)(index->m_data + ret) = (UINT32)it->first;
				// 	ret += sizeof(UINT32);
				// 	*(UINT32 *)(index->m_data + ret) = (UINT32)it->second;
				// 	ret += sizeof(UINT32);
				// }
			}
			break;						
		default:
			break;
	}
	return index->getSize();
}

UINT32 RoleExtBinMgr::doRestorAction(byte* data)
{
	stSaveItem* index = (stSaveItem*)data;
	if(index->getSize() == sizeof(stSaveItem))
	{
		return index->getSize();//返回大小
	}

	switch ( index->m_scheme ) {
		case ROLE_EXTBIN_TEST :
			{
				bcopy(index->m_data,&m_test,index->m_count);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_ONLINETIME :
			{
				m_pOwner->m_onlineTime = *(UINT32 *)(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_RECOVERSPTIME :
			{
				m_pOwner->m_recoverSPLilianTime = *(UINT32 *)(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_BUYINVESTPLAN :
			{
				m_pOwner->m_buyInvestPlan = *(UINT8 *)(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_CURRENT_TASKID :
			{
				UINT32 offset = 0;
				m_pOwner->m_currentTaskID = *(UINT32 *)(index->m_data);
				offset += sizeof(m_pOwner->m_currentTaskID);
				m_pOwner->m_currentTaskStage = *(UINT32 *)(index->m_data + offset);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_HISTORY_ARENA_RANK :
			{
				m_pOwner->m_arenaHistoryRank = *(UINT32 *)(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_SYSTEM_SETTING :
			{
				m_pOwner->m_systemSetting = *(UINT64 *)(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_ONLINE_REWARD_PASSTIME :
			{
				m_pOwner->m_rewardSys.setOnlineRewardPassTime(*(UINT32 *)(index->m_data));
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_COMPESATIONID :
			{
				m_pOwner->m_compensationID = *(UINT8 *)(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_CHANGE_NAME_COUNT :
			{
				m_pOwner->m_changeNameNum = *(UINT32 *)(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_SEVENDAY_ACTIVITY :
			{
				m_pOwner->m_sevenDayActivityMgr.load(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_CUMULOGIN :
			{
				UINT32 offset = 0;
				m_pOwner->m_yestodayFirstLoginTime = *(UINT32*)(index->m_data);
				offset += sizeof(m_pOwner->m_yestodayFirstLoginTime);
				m_pOwner->m_cumulativeLogin = *(UINT16 *)(index->m_data + offset);
				offset += sizeof(m_pOwner->m_cumulativeLogin);
				return index->getSize();	
			}
			break;
		case ROLE_EXTBIN_LUCKSHOP :
			{
				m_pOwner->m_luckShopMgr.load(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_LIMIT_FASHION :
			{
				m_pOwner->m_limitFashionDrawMgr.load(index->m_data);
				return index->getSize();
			}
			break;
		case ROLE_EXTBIN_BUYSUMMONPLAN :
			{
				m_pOwner->m_buySummonPlan = *(UINT8 *)(index->m_data);
				return index->getSize();
			}
		case ROLE_EXTBIN_BUYSUMMON_NUM :
			{
				m_pOwner->m_buySummonNum = *(UINT8 *)(index->m_data);
				return index->getSize();
			}	
		case ROLE_EXTBIN_BUYSUMMONPLAN_FREE :
			{
				byte * buf = index->m_data;
				UINT32 ret = 0;
				UINT16 size = *(UINT16 *)(buf + ret);
				ret += sizeof(UINT16);

				for (UINT16 i = 0;i < size ; ++i){
					m_pOwner->m_freeSummonSet.insert(*(UINT16 *)(buf + ret));
					ret += sizeof(UINT16);
				}
				return index->getSize();
			}			
			break;	
		case ROLE_EXTBIN_BUYSUMMONPLAN_RECHARGE :
			{
				byte * buf = index->m_data;
				UINT32 ret = 0;
				UINT16 size = *(UINT16 *)(buf + ret);
				ret += sizeof(UINT16);

				for (UINT16 i = 0;i < size ; ++i){
					m_pOwner->m_rechargeSummonSet.insert(*(UINT16 *)(buf + ret));
					ret += sizeof(UINT16);
				}
				return index->getSize();
			}			
			break;	
		case ROLE_EXTBIN_BUYLIMITSUMMON_NUM :
			{
				m_pOwner->m_buyLimitSummonNum = *(UINT8 *)(index->m_data);
				return index->getSize();
			}	
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN :
			{
				m_pOwner->m_buyLimitSummonPlan = *(UINT8 *)(index->m_data);
				return index->getSize();
			}	
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN_FREE :
			{
				byte * buf = index->m_data;
				UINT32 ret = 0;
				UINT16 size = *(UINT16 *)(buf + ret);
				ret += sizeof(UINT16);

				for (UINT16 i = 0;i < size ; ++i){
					m_pOwner->m_freeLimitSummonSet.insert(*(UINT16 *)(buf + ret));
					ret += sizeof(UINT16);
				}
				return index->getSize();
			}			
			break;	
		case ROLE_EXTBIN_BUYLIMITSUMMONPLAN_RECHARGE :
			{
				byte * buf = index->m_data;
				UINT32 ret = 0;
				UINT16 size = *(UINT16 *)(buf + ret);
				ret += sizeof(UINT16);

				for (UINT16 i = 0;i < size ; ++i){
					m_pOwner->m_rechargeLimitSummonSet.insert(*(UINT16 *)(buf + ret));
					ret += sizeof(UINT16);
				}
				return index->getSize();
			}			
			break;		
		case ROLE_EXTBIN_BUYTREASUREBOX_COUNT :
			{
				// byte * buf = index->m_data;
				// UINT32 ret = 0;
				// UINT32 num = *(UINT32 *)(buf + ret);
				// ret += sizeof(UINT32);
				// UINT32 id;
				// UINT32 count;
				// for (UINT32 i = 0; i < num; i++) {
				// 	id = *(UINT32 *)(buf + ret);
				// 	ret += sizeof(UINT32);
				// 	count = *(UINT32 *)(buf + ret);
				// 	ret += sizeof(UINT32);
				// 	m_pOwner->m_buyTreasureCount.insert(std::make_pair(id, count));
				// }

				byte * buf = index->m_data;
				UINT32 ret = 0;
				UINT32 num = *(UINT32 *)(buf + ret);
				ret += sizeof(UINT32);
				UINT32 id;
				UINT32 count;
				for (UINT32 i = 0; i < num; i++) {

					id = *(UINT32 *)(buf + ret);
					ret += sizeof(UINT32);

					count = *(UINT32 *)(buf + ret);
					ret += sizeof(UINT32);
					Athena::logger->trace("【ROLE__BUYTREASUREBOX_COUNT [INSERT] id is  %d , count is  %d", id, count);

					m_pOwner->m_buyTreasureCount.insert(std::make_pair(id, count));

				}

				return index->getSize();
			}			
			break;						
		default:
			break;
	}

	Athena::logger->error("不存在的类型 %d size:%d", index->m_scheme, index->getSize());
	return 0;
}

RoleExtBinMgr::RoleExtBinMgr(GameUser * pOwner) : m_pOwner(pOwner) 
{
	m_test = 10;
	m_scheme.push_back(ROLE_EXTBIN_TEST);
	m_scheme.push_back(ROLE_EXTBIN_ONLINETIME);
	m_scheme.push_back(ROLE_EXTBIN_RECOVERSPTIME);
	m_scheme.push_back(ROLE_EXTBIN_BUYINVESTPLAN);
	m_scheme.push_back(ROLE_EXTBIN_CURRENT_TASKID);
	m_scheme.push_back(ROLE_EXTBIN_HISTORY_ARENA_RANK);
	m_scheme.push_back(ROLE_EXTBIN_SYSTEM_SETTING);
	m_scheme.push_back(ROLE_EXTBIN_ONLINE_REWARD_PASSTIME);
	m_scheme.push_back(ROLE_EXTBIN_COMPESATIONID);
	m_scheme.push_back(ROLE_EXTBIN_SEVENDAY_ACTIVITY);
	m_scheme.push_back(ROLE_EXTBIN_CUMULOGIN);
	m_scheme.push_back(ROLE_EXTBIN_LUCKSHOP);
	m_scheme.push_back(ROLE_EXTBIN_LIMIT_FASHION);
	m_scheme.push_back(ROLE_EXTBIN_CHANGE_NAME_COUNT);
	m_scheme.push_back(ROLE_EXTBIN_BUYSUMMONPLAN);
	m_scheme.push_back(ROLE_EXTBIN_BUYSUMMON_NUM);
	m_scheme.push_back(ROLE_EXTBIN_BUYSUMMONPLAN_FREE);
	m_scheme.push_back(ROLE_EXTBIN_BUYSUMMONPLAN_RECHARGE);
	m_scheme.push_back(ROLE_EXTBIN_BUYLIMITSUMMON_NUM);
	m_scheme.push_back(ROLE_EXTBIN_BUYLIMITSUMMONPLAN);
	m_scheme.push_back(ROLE_EXTBIN_BUYLIMITSUMMONPLAN_FREE);
	m_scheme.push_back(ROLE_EXTBIN_BUYLIMITSUMMONPLAN_RECHARGE);
	m_scheme.push_back(ROLE_EXTBIN_BUYTREASUREBOX_COUNT);

}
