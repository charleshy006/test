#ifndef __ROLE_EXTBIN_MGR__
#define __ROLE_EXTBIN_MGR__

#include "ExtBinMgr.h"

class GameUser;
class RoleExtBinMgr : public ExtBinMgr
{
	public :
		UINT32 m_test;
	private :
		GameUser * m_pOwner;
	public :
		enum
		{
			ROLE_EXTBIN_TEST = 1,
			ROLE_EXTBIN_ONLINETIME = 2,				//在线时间
			ROLE_EXTBIN_RECOVERSPTIME = 3,			//体力回复时间
			ROLE_EXTBIN_BUYINVESTPLAN = 4,			//购买投资计划
			ROLE_EXTBIN_CURRENT_TASKID = 5,			//当前正在进行的任务
			ROLE_EXTBIN_HISTORY_ARENA_RANK = 6,		//竞技场历史排名
			ROLE_EXTBIN_SYSTEM_SETTING = 7,			//系统设置
			ROLE_EXTBIN_ONLINE_REWARD_PASSTIME = 8,	//在线奖励经过时间
			ROLE_EXTBIN_COMPESATIONID = 9,			//已经领取的补偿ID
			ROLE_EXTBIN_SEVENDAY_ACTIVITY = 11, 	//7天活动奖励
			ROLE_EXTBIN_CUMULOGIN = 12,				//累积登录
			ROLE_EXTBIN_LUCKSHOP = 13,				//幸运商店
			ROLE_EXTBIN_LIMIT_FASHION = 14,			//限时时装
			ROLE_EXTBIN_CHANGE_NAME_COUNT = 15,		//改名次数
			ROLE_EXTBIN_BUYSUMMONPLAN = 16,			//购买召唤计划
			ROLE_EXTBIN_BUYSUMMON_NUM = 17,			//橙色次数计划
			ROLE_EXTBIN_BUYSUMMONPLAN_FREE = 18,    //召唤计划免费
			ROLE_EXTBIN_BUYSUMMONPLAN_RECHARGE = 19,//召唤计划收费
			ROLE_EXTBIN_BUYLIMITSUMMON_NUM = 20,//限时召唤计划收费
			ROLE_EXTBIN_BUYLIMITSUMMONPLAN = 21,//限时召唤战令是否购买
			ROLE_EXTBIN_BUYLIMITSUMMONPLAN_FREE = 22,    //限时召唤计划免费
			ROLE_EXTBIN_BUYLIMITSUMMONPLAN_RECHARGE = 23,//限时召唤计划收费
			ROLE_EXTBIN_BUYTREASUREBOX_COUNT = 24,//购买宝箱字数统计

		};
		RoleExtBinMgr(GameUser * pOwner);
		virtual ~RoleExtBinMgr()
		{
		
		}
		virtual bool isNeedSave(byte s); 
		virtual UINT32 getSaveSize(byte s); 
		virtual UINT32 doSaveAction(byte* data, byte s, UINT32 dataSize);
		virtual UINT32 doRestorAction(byte* data);
};

#endif
