#ifndef __H_GAMEEVENT_NOTIFY_H_
#define __H_GAMEEVENT_NOTIFY_H_
#include "Task.h"
#include "GameUser.h"

template <int EventType,int paraNum>
struct stGameEventNotify
{
public :
	void notify(GameUser *pUser,UINT32 value,UINT32 para1 = 0,UINT32 para2 = 0,UINT32 para3 = 0,UINT32 para4 = 0)
	{
		UINT32 eventParam[4] = {para1,para2,para3,para4}; 
		pUser->m_taskMgr.onGameEvent(EventType,eventParam,paraNum,value);	
	}
};

template <int PropType>
struct stGamePropNotify
{
	public :
		void notify(GameUser * pUser)
		{
			pUser->m_taskMgr.onPropChange(PropType);
		}
};

typedef stGameEventNotify<TaskProto::ET_FINISH_COPYMAP,1>  stFinishCopyMapNotify;
typedef stGamePropNotify<TaskProto::ET_RAISE_EQUIP_TO_LV>  stRaiseEquip2Lv; 
typedef stGamePropNotify<TaskProto::ET_REFINE_EQUIP_TO_LV> stRefineEquip2Lv;
typedef stGamePropNotify<TaskProto::ET_HERO_BREAK_INTO_LV> stHeroBreakIntoLv;
typedef stGamePropNotify<TaskProto::ET_HERO_LEVELUP_TO_LV> stHeroLevelup2Lv;
typedef stGamePropNotify<TaskProto::ET_RAISE_SKILL_TO_LV>  stRaiseSkill2Lv;
typedef stGamePropNotify<TaskProto::ET_EQUIP_FUWEN_NUM>    stEquipFuwenNum;
typedef stGamePropNotify<TaskProto::ET_RAISE_FUWEN_TO_LV>  stRaiseFuwen2Lv;
typedef stGamePropNotify<TaskProto::ET_COLLECT_FASHION_NUM>stCollectFashionNum;

typedef stGameEventNotify<TaskProto::ET_DRAW_NUM,0>		   		 	stDrawNum;
typedef stGameEventNotify<TaskProto::ET_CAPTRURE_TREASURE_NUM,0> 	stCaptureTreasureNum;
typedef stGameEventNotify<TaskProto::ET_ARENA_NUM,0>			 	stArenaNum;
typedef stGameEventNotify<TaskProto::ET_TEAM_FINISH_COPYMAP_NUM,0>  stTeamFinishCopymapNum;
typedef stGameEventNotify<TaskProto::ET_DIANFENG_PK_NUM,0>			stDianfengPkNum;
typedef stGameEventNotify<TaskProto::ET_REALTIME_COPYMAP_NUM,0> 	stRealTimeCopymapNum;
typedef stGameEventNotify<TaskProto::ET_LEITAI_COPYMAP_NUM,0>  		stLeitaiCopymapNum;
typedef stGameEventNotify<TaskProto::ET_SHENGCUN_COPYMAP_NUM,0> 	stShengCunCopymapNum;
typedef stGameEventNotify<TaskProto::ET_GOLDCOIN_COPYMAP_NUM,0> 	stGoldcoinCopymapNum;
typedef stGameEventNotify<TaskProto::ET_ELITE_COPYMAP_NUM,0>    	stEliteCopymapNum;
typedef stGameEventNotify<TaskProto::ET_RAISE_HERO_NUM,0>			stRaiseHeroNum;
typedef stGameEventNotify<TaskProto::ET_RAISE_SKILL_NUM,0>			stRaiseSkillNum;
typedef stGameEventNotify<TaskProto::ET_RAISE_EQUIP_NUM,0>			stRaiseEquipNum;
typedef stGameEventNotify<TaskProto::ET_RAISE_FUWEN_NUM,0>			stRaiseFuwenNum;
typedef stGameEventNotify<TaskProto::ET_BUY_SHOP_ITEM_NUM, 1>       stBuyShopItemNum;
typedef stGameEventNotify<TaskProto::ET_EXPEDITION_NUM, 0>			stExpeditionNum;
typedef stGameEventNotify<TaskProto::ET_RESET_RUSH_NUM, 0>			stResetRushNum;;
typedef stGameEventNotify<TaskProto::ET_GIVE_LILIAN_NUM, 0>			stGiveLilianNum;
typedef stGameEventNotify<TaskProto::ET_GUILD_PRAY_NUM, 0>			stGuildPrayNum;

typedef stGamePropNotify<TaskProto::ET_GET_WELFARE_CARD>	 stGetWelfareCard;
typedef stGamePropNotify<TaskProto::ET_GET_MONTH_CARD>		 stGetMonthCard;
typedef stGamePropNotify<TaskProto::ET_GET_QUARTER_CARD>	 stGetQuarterCard;
typedef stGamePropNotify<TaskProto::ET_GET_YEAR_CARD>		 stGetYearCard;

typedef stGameEventNotify<TaskProto::ET_GUARD,0>			 stGuardNum;
typedef stGameEventNotify<TaskProto::ET_INTERCEPT,0>		 stInterceptNum;

#endif
