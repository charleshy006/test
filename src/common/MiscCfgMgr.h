#ifndef __MISCCFG_CONFIG_H__
#define __MISCCFG_CONFIG_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

enum 
{
	MISC_CFG_ID_LINGWU = 1,			//领悟技能次数
	MISC_CFG_ID_OPEN_SKILL_LV = 2,	//领悟技能格子开放等级
	MISC_CFG_ID_JUEXING_OPEN_LV = 3,//觉醒开放等级
	MISC_CFG_ID_FUWEN_OPEN_LV = 4,	//符文开放等级
	MISC_CFG_ID_RUSH_RESET  = 5,	//扫荡重置次数
	MISC_CFG_ID_VIPLV2_RECHARGE = 6,//充值与vip等级关系
	MISC_CFG_ID_FIRST_RECHARGE = 7,	//是否是首次充值
	MIS_CFG_ID_JUEXING_NEED_DIAMOND = 8,//觉醒需要钻石
	MISC_CFG_ID_BATTLE_POS = 9,		//宠物出战数量
	MISC_CFG_ID_WORLD_BOSS_LEVEL_LIMIT = 16, //世界boss等级限制
	MISC_CFG_ID_WORLD_BOSS_BUY_PRICE = 17,	//世界boss士气鼓舞价格
	MISC_CFG_ID_ONEKEY_ROLELV = 20,			//一键相关配置
	MISC_CFG_ID_TEAM_COPYMAP_RESET_COST = 21,//组队重置价格
	MISC_CFG_ID_REALTIME_RESET_COST = 22,	//实时战场重置价格
	MISC_CFG_ID_ARENA_COOLDOWN = 26,		//天梯冷却时间
	MISC_CFG_ID_ARENA_RESET = 27,			//天梯重置时间
	MISC_CFG_ID_GUARD_BUY_INTERCEPT_NUM = 28, //购买拦截次数需要钻石
	MISC_CFG_ID_GHANGE_NAME_NUM = 33,		//改名次数消耗砖石
	MISC_CFG_ID_BUY_GOLD_NEED_DIAMOND = 34,	//金币每日购买所需钻石
	MISC_CFG_ID_CHAT_LIMIT_TIME = 36,			//世界聊天冷却时间
	MISC_CFG_ID_MAX_ROLE_LVL = 38,	        //角色最大等级
	MISC_CFG_ID_EXP_LEVEL_LIMIT = 39,		//经验书等级限制
	MISC_CFG_ID_RUSH_LEVEL_OPEN = 42,		//扫荡功能是否开启
	MISC_CFG_ID_RUNE_EVOLVE_LVL_LIMIT = 43,	//符文强化等级限制
	MISC_CFG_ID_OPERATION_LVL_LIMIT = 44,   //装备觉醒,随从领悟,装备突破等级
	MISC_CFG_ID_INTENSIFY_LVL_LIMIT = 45,   //强化最大等级
	MISC_CFG_ID_LEVEL_WFLFARE = 51,			//冲级好礼开启等级
	MISC_CFG_ID_GUAJI_LIMIT = 52,			//挂机配置
	MISC_CFG_ID_CHAPTER_EQUIP = 53,			//章节装备限制
	MISC_CFG_ID_RECAST_EQUIP = 54,			//装备重铸所需钻石
};

struct stRecastDiamond
{
	int m_n;
	int m_r;
	int m_sr;
	int m_ssr;
	int m_ur;

	stRecastDiamond()
	{
		m_n = 40;
		m_r = 40;
		m_sr = 40;
		m_ssr = 40;
		m_ur = 40;
	}
};

struct stChapEquipLimit
{
	int m_chapID;	//
	int m_sr;	//
	int m_ssr;	//

	stChapEquipLimit()
	{
		m_chapID = 0;
		m_sr = 0;
		m_ssr = 0;
	}
};


struct stGuajiLimit
{
	UINT32 m_maxtime;	//最大时长
	UINT32 m_exp;		//每分钟经验
	UINT32 m_goldRate;		//金币系数
	std::map<UINT32,UINT32> m_mjCoin;		//mj项目挂机花费钻石

	stGuajiLimit()
	{
		m_maxtime = 0;
		m_exp = 0;
		m_goldRate = 0;
	}
};

class CMiscCfgMgr
{
	USE_SINGLETON_NEW(CMiscCfgMgr);
private :
	CMiscCfgMgr()
	{
		m_worldBossLvLimit = 0;
		m_oneKeyCaptureLv = 0;
		m_oneKeyRaiseLv = 0;
		m_oneKeyRushLv = 0;
		m_oneKeyStudyHeroSkillLv = 0;
		m_oneKeyStudyEquipSkillLv = 0;
        m_rushLvlOpen = false;
	}
public :
    enum Misc_Operation_Lvl_Limit {
        Limit_Equip_Awake = 1,      //装备觉醒
        Limit_Hero_Comprehen = 2,   //随从领悟
        Limit_Equip_Break = 3,      //装备突破
    };

	struct stArenaCoolDownInfo
	{
		UINT32 m_challengeNum;			//挑战次数
		UINT32 m_cd;					//cd时间
		UINT32 m_needDiamond;			//需要钻石

		stArenaCoolDownInfo()
		{
			m_challengeNum = 0;
			m_cd = 0;
			m_needDiamond = 0;
		}
	};
	
	struct stBuyGoldInfo
	{
		UINT32 m_gainsGoldNum;	//每次购买获得金币
		UINT32 m_needDiamond;	//每次购买所需砖石

		stBuyGoldInfo()
		{
			m_gainsGoldNum = 0;
			m_needDiamond = 0;
		}
	};

	struct stExpLevelLimit
	{
		UINT16 m_minLevel;	//最低等级
		UINT16 m_maxLevel;	//最高等级

		stExpLevelLimit()
		{
			m_minLevel = 0;
			m_maxLevel = 0;
		}
	};

public :
	//析构函数
	~CMiscCfgMgr(){}
	//获得实例
	static CMiscCfgMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
	//根据充值得到vip等级
	UINT8 getVipLv(UINT32 totalRecharge);
	//根据次数获得返回钻石
	UINT32 getStudySkillDiamond(UINT32 num);
	//根据出战位置获得等级
	UINT16  getLvByBattlePetPos(UINT16 pos);
	//根据扫荡重置次数获得需要钻石
	UINT32  getDiamondByRushReset(UINT16 resetNum);
	//根据觉醒次数获得需要钻石
	UINT32  getDiamondByJueXing(UINT16 num);
	//根据觉醒格子位置获得需要等级
	UINT16  getNeedLvByPos(UINT16 pos);
	//获得开放符文的装备数量
	UINT8  getUnlockedFuwenEquipNum(UINT16 roleLv);
	//世界boss等级限制
	UINT16 getWorldBossLvLimit()
	{
		return m_worldBossLvLimit;
	}
	//首次充值奖励
	std::vector<std::pair<UINT32,UINT32> > & getFirstRechargeReward()
	{
		return m_firstRechargeRewards;
	}
	//世界boss士气鼓舞最大购买次数
	UINT16  getWorldBossMaxBuyNum()
	{
		return m_worldBossBuyPrices.size();
	}
	//士气鼓舞购买价格
	UINT32 getWorldBossPriceByTime(UINT16 num);
	//获得一键抢夺等级
	UINT16 getOneKeyCaptureLv()
	{
		return m_oneKeyCaptureLv;
	}
	//获得一键强化等级
	UINT16 getOneKeyRaiseLv()
	{
		return m_oneKeyRaiseLv;
	}
	//获得一键扫荡等级
	UINT16 getOneKeyRushLv()
	{
		return m_oneKeyRushLv;
	}
	//获得一键领悟等级
	UINT16 getOneKeyStudyHeroSkillLv()
	{
		return m_oneKeyStudyHeroSkillLv;
	}
	//获得一键觉醒等级
	UINT16 getOneKeyStudyEquipSkillLv()
	{
		return m_oneKeyStudyEquipSkillLv;
	}
	//获得组队重置花费
	UINT32 getTeamCopyResetCostByNum(UINT16 num);
	//获得实时战场重置花费
	UINT32 getRealTimeResetCostByNum(UINT16 num);
	//获得挑战次数
	UINT16 getArenaChallengeNum()
	{
		return m_arenaCDInfo.m_challengeNum;
	}
	//获得挑战的cd
	UINT32 getArenaCD()
	{
		return m_arenaCDInfo.m_cd;
	}
	//获得清除cd需要钻石
	UINT32 getNeedDiamond()
	{
		return m_arenaCDInfo.m_needDiamond;
	}
	//竞技场重置需要金钱
	UINT32 getNeedDiamondByNum(UINT32 num);
	//重置拦截次数需要金钱
	UINT32 getInterceptNeedDiamond(UINT32 num);
	//改名次数消耗砖石
	UINT32 getChangeNameNeedDiamond(UINT8 num);
	//购买金币所需砖石数量
	stBuyGoldInfo * getBuyGoldNeedDiamond(UINT8 num);
	//获得世界聊天冷却时间
	UINT16 getChatLimitTime();
    //获取最大等级
    UINT32 getMaxRoleLvl() {
        return m_maxRoleLvl;
    }
	//获得限制等级
	stExpLevelLimit * getLimitLevel(UINT32 id);
    //扫荡功能是否开启
    bool rushLvlIsOpen() {
        return m_rushLvlOpen;
    }
    //获取人物限制等级
    UINT32 getRuneRoleLvl(UINT32 refineLv);
    //获取各种操作等级限制
    UINT32 getOperaLvlLimit(UINT32 type);
    //获取强化最大等级
    UINT32 getIntensifyLvlLimit() {
        return m_intensifyLvlLimit;
    }
	//获取冲级好礼开启等级
	UINT16 getLevelWflfare(){
		return m_levelWflfare;
	}
	const stGuajiLimit& getGuajiLimit(){
		return m_guaji;
	}
	stChapEquipLimit* getChapEqupLimit(UINT32 chapID);
	
	const stRecastDiamond& getRecastCost(){
		return m_recastDiamond;
	}
private :
	std::map<UINT16,UINT32> m_lingwuNum2Diamonds;		//领悟技能次数需要钻石
	std::map<UINT8,UINT16> m_heroSkillOpenLv;			//武将领悟技能开放等级
	std::map<UINT8,UINT32> m_vipLv2Recharge;			//vip等级与充值数量
	std::map<UINT16,UINT16> m_battlePetPos2Lv;			//战斗宠物数量与等级关系
	std::map<UINT16,UINT16> m_juexingOpenLv;			//觉醒开放等级
	std::map<UINT16,UINT16> m_fuwenOpenLv;				//符文开放等级
	std::map<UINT16,UINT32> m_rushResetNum2Diamond;		//扫荡重置次数与钻石数量
	std::map<UINT16,UINT32> m_juexingNeedDiamond;		//觉醒需要钻石数量
	UINT32 m_worldBossLvLimit;							//世界boss等级限制
	std::vector<std::pair<UINT32,UINT32> > m_firstRechargeRewards; //首次充值奖励
	std::map<UINT16,UINT32>  m_worldBossBuyPrices;		//世界boss购买价格
	UINT16 m_oneKeyCaptureLv;							//一键抢夺等级
	UINT16 m_oneKeyRaiseLv;								//一键强化等级
	UINT16 m_oneKeyRushLv;								//一键扫荡等级
	UINT16 m_oneKeyStudyHeroSkillLv;					//一键领悟等级
	UINT16 m_oneKeyStudyEquipSkillLv;					//一键觉醒等级
	std::map<UINT16,UINT32> m_teamCopyResetCosts;		//组队重置花费
	std::map<UINT16,UINT32> m_realTimeResetCosts;		//实时重置花费
	std::map<UINT16,UINT32> m_arenaResetCosts;			//天梯充值花费
	stArenaCoolDownInfo     m_arenaCDInfo;				//竞技场信息
	std::map<UINT16,UINT32> m_guardBuyInterceptNum;		//购买拦截次数需要钻石
	std::map<UINT8, UINT32> m_changeNameResetCosts;		//改名花费
	std::map<UINT8, stBuyGoldInfo> m_buyGoldInfo;		//购买金币所需砖石数量
	UINT16 m_chatLimitTime;								//世界聊天冷却时间
    UINT32 m_maxRoleLvl;								//角色最高等级
	std::map<UINT32, stExpLevelLimit> m_expLevelLimit;	//经验书等级限制
    bool m_rushLvlOpen;                                 //扫荡功能是否开启
    std::map<UINT32, UINT32> m_runeLimitMap;            //符文强化等级限制
    std::map<UINT32, UINT32> m_operaLvlLimitMap;        //装备觉醒,随从领悟,装备突破等级
	UINT16 m_intensifyLvlLimit;							//强化最大等级
	UINT16 m_levelWflfare;								//冲级好礼开启等级
	stGuajiLimit	m_guaji;							//挂机配置
	std::map<UINT32, stChapEquipLimit> m_chapEquipLimit; //
	stRecastDiamond	m_recastDiamond;			//重铸装备消耗钻石
};
#endif
