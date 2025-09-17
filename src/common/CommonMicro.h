#ifndef __COMMON_MICRO__
#define __COMMON_MICRO__
//#define _HDZ_DEBUG

#define MAX_UZLIB_CHAR	(200 * 1024)
#define MAX_IP_LENGTH     15
#define MAX_ACCNAMESIZE     48

#ifdef _THAILAND_VER
    #define MAX_NAMESIZE 152
#else
    #define MAX_NAMESIZE 32
#endif

enum ServerType
{
	UNKNOWNSERVER	= 0,
	GLOBALSERVER    = 1,
	GAMESERVER      = 2,
	GATESERVER		= 3,
	LOGINSERVER 	= 4,
	WEBSERVER		= 5,
	BILLSERVER 		= 6,
	UPDATESERVER    = 7,
	TXGIFTSERVER    = 8,
	PUBGAMESERVER    = 9,
	MASTERSERVER    = 10,
	MAX_SERVERTYPE	= 10,
};

#ifndef	MAX_ACCNAMESIZE
	#define MAX_ACCNAMESIZE	48
#endif

#ifndef MAX_NAMESIZE
	#define MAX_NAMESIZE	32
#endif

#define MAX_PASSWORD_LEN  33
#define MAX_LOGIN_VERIFY_STR_LEN 135

#undef SEND_PROTO_2_CLIENT
#define SEND_PROTO_2_CLIENT(pCom,retCmd)  do {\
			std::string retStr;				\
			retCmd.SerializeToString(&retStr); \
			sendClientProto(pCom,retCmd.ModID,retCmd.FunID,retStr.c_str(),retStr.size());\
		}while(false);

//战斗属性
enum eBattleProp
{
	eBATTLE_PROP_HP = 1,			//生命
	eBATTLE_PROP_DAMAGE = 2,		//伤害
	eBATTLE_PROP_HITRATE = 3,		//命中率
	eBATTLE_PROP_DODGERATE = 4,		//闪避率
	eBATTLE_PROP_BANGRATE = 5,		//暴击率
	eBATTLE_PROP_BANGDAMAGE = 6,	//暴击伤害
	eBATTLE_PROP_ANTIBANGRATE = 7,	//抗暴击
	eBATTLE_PROP_ANTIBANGDAMAGE = 8,//暴击免伤
	eBATTLE_PROP_FINALDAMAGE = 9,	//最终伤害
	eBATTLE_PROP_RECOVERHP = 10,	//普攻回复血
	eBATTLE_PROP_RECOVERMP = 11,	//普攻回复法力
	eBATTLE_PROP_ABSORBHP  = 12,	//普攻吸血
	eBATTLE_PROP_REFLECTDAMAGE = 13,//伤害反弹
	eBATTLE_PROP_FIREDAMAGE = 14,	//火强伤害
	eBATTLE_PROP_ICEDAMAGE = 15,	//冰墙伤害
	eBATTLE_PROP_THUNDERDAMAGE = 16,//雷电伤害
	eBATTLE_PROP_FIRERATE = 17,		//火墙触发概率
	eBATTLE_PROP_ICERATE = 18,		//冰墙伤害概率
	eBATTLE_PROP_THUNDERRATE = 19,	//雷电伤害概率
	eBATTLE_PROP_DAMAGEREDUCE = 20,	//伤害减免
	eBATTLE_PROP_MOVE_SPEED = 21,	//移动速度
	eBATTLE_PROP_ATTACK_SPEED = 22, //攻击速度
	eBATTLE_PROP_BATI_DAMAGE = 23,	//霸体伤害
	eBATTLE_PROP_ANTI_BATI_DAMAGE = 24,//霸体免伤
	eBATTLE_PROP_HP_PERCENT = 25,	  //生命百分比
	eBATTLE_PROP_DAMAGE_PERCENT = 26, //伤害百分比
	eBATTLE_PROP_ANTI_FIRE_DAMAGE = 27,//火强伤害减免
	eBATTLE_PROP_ANTI_ICE_DAMAGE = 28, //冰冻伤害减免
	eBATTLE_PROP_ANTI_THUNDER_DAMAGE = 29,//雷电伤害减免
	eBATTLE_PROP_FUWEN_ACTIVATE = 30,	  //符文激活属性
	eBATTLE_PROP_MP = 31,				  //魔法
	eBATTLE_PROP_BATI = 32,				  //加霸体
	eBATTLE_PROP_RAISE_SKILL = 33,		  //强化技能
	eBATTLE_PROP_REF_FIRE_RATE = 34,	  //火强概率免疫
	eBATTLE_PROP_REF_ICE_RATE = 35,		  //冰冻概率免疫
	eBATTLE_PROP_REF_THUNDER_RATE = 36,	  //雷电概率免疫
	eMAX_BATTLE_PROP
};

//资源
enum eResource
{
	eResource_GoldCoin = 1,				//金币
	eResource_Diamond = 2,				//钻石
	eResource_Heishijiejing = 3,		//黑市结晶
	eResource_Reputation = 4,			//声望值
	eResource_Honour = 5,				//荣誉值
	eResource_Brave = 6,				//勇者证
	eResource_PeakCertificate = 7,		//巅峰证
	eResource_Hufu = 8,			 		//虎符
	eResource_Yuntie = 9,				//陨铁
	eResource_Guild_Contri = 10,		//公会贡献度
	eResource_Sp = 11,					//体力
	eResource_Lilian = 12,				//历练
	eResource_Juexing_Exp = 13,			//觉醒经验
	eResource_Lingwu_Exp = 14,			//领悟经验
	eResource_Integral = 16,			//积分
	eResource_Soul_Fire = 17,			//魂火
	eResource_Life_Essence = 19,		//生命精华
	eResource_Guild_Crystal = 20,		//公会黑市结晶
	eResource_TrainSp = 21,				//调教体力
	eResource_ID_Max = 21,				//资源最大ID
};

//vip增加次数
enum eVipFunCount
{
	eVipFun_Swing = 2,			//翅膀
	eVipFun_Gift = 3,			//礼包
	eVipFun_MainBagNum = 4,		//道具背包格子数量
	eVipFun_MaxSp = 5,			//体力上限
	eVipFun_MaxLilian = 6,		//历练上限
	eVipFun_GoldcoinBag = 7,	//金币袋数量
	eVipFun_Sp_Item  = 8,		//体力丹
	eVipFun_Lilian_Item = 9,	//历练丹
	eVipFun_Yuntie_Bag = 10,	//陨铁袋
	eVipFun_Hero_Understand_Skill = 11,//武神领悟技能
	eVipFun_Equip_Juexing = 12,	//装备钻石觉醒次数
	eVipFun_Heishi_Refresh = 13,//黑市钻石刷新次数
	eVipFun_Buy_GoldBox	= 14,	//钻石买黄金宝箱
	eVipFun_Buy_GoldKey = 15,	//购买黄金要是次数
	eVipFun_Buy_SilverBox = 16,	//购买白银宝箱数量
	eVipFun_Buy_SilverKey = 17,	//购买白银要是次数
	eVipFun_Buy_Chg_Soul_Item = 18,//购买还魂石数量
	eVipFun_Buy_Yueshenzhilei_Item = 19,//购买月神之类道具数量
	eVipFun_5Times_Capture_Treasure = 20,//五连夺宝功能
	eVipFun_One_Key_Raise_Equip = 21,	//一键强化
	eVipFun_Battle_Num = 22,			//战役次数
	eVipFun_5Times_Rush = 23,			//5次扫荡
	eVipFun_Team_Copymap = 24,			//组队副本
	eVipFun_BattleGround = 25,			//战场次数
	eVipFun_Bazhu_Reset = 26,           //霸主重置
	eVipFun_Invest_Plan = 27,			//投资计划
	eVipFun_FashionBagNum = 28,			//时装格子
	eVipFun_TeamCopyResetNum = 29,		//组队副本重置次数
	eVipFun_RealTimeResetNum = 30,		//实时战场重置次数
	eVipFun_PVEResetNum = 31,			//战役里重置次数
	eVipFun_OneKeyGotEquipSkill = 32,	//一键觉醒
	eVipFun_OneKeyStudyPetSkill = 33,	//一键领悟
	eVipFun_GuildDonate = 34,			//公会捐赠	
	eVipFun_ArenaResetNum = 35,			//竞技场重置次数
	eVipFun_Guard_Can_Summon = 36,		//护送是否可以召唤
	eVipFun_Guard_Reset_Num = 37,		//护送重置次数
	eVipFun_Rush_Ticket_Num = 38,		//扫荡券购买次数
	eVipFun_AKay_PaPaPa = 39,			//一键啪啪啪开放
	eVipFun_MaxTrainSp = 40,			//调教体力上限
};

enum eUSER_LOG_TYPE
{
	eLog_Type_Levelup = 1,		//升级日志
	eLog_Type_Login = 2,		//玩家登录
	eLog_Type_Recharge = 3,		//充值日志
	eLog_Type_VipLevelup = 4,	//vip升级日志
	eLog_Type_HonourChg = 5,	//荣誉变化
	eLog_Type_ItemChg = 6,		//道具变化日志
	eLog_Type_ResChg = 7,		//资源变化日志
	eLog_Type_DailyTask = 8,	//日常任务
	eLog_Type_MallBuy = 9,		//商店购买
	eLog_Type_HeroPropChg = 10, //武将属性变化
	eLog_Type_Guild = 11,		//guild日志
	eLog_Type_Module = 12,		//模块日志
	eLog_Type_SkillChg = 13,	//技能变化日志
	eLog_Type_Sprite = 14,		//精灵日志
	eLog_Type_Copymap = 15,		//副本日志
	eLog_Type_Equip = 16,		//装备日志
	eLog_Type_Activity = 17,	//活动日志
	eMax_Log_Type  = 17,
};

#define BP_HP_COEF 0.1f
#define BP_MP_COEF 10.0f
#define BP_DAMAGE_COEF 1.0f
#define BP_HIT_COEF 10.0f
#define BP_DODGE_COEF 10.0f
#define BP_BANG_COEF 10.f
#define BP_BANGDAMAGE_COEF 5.0f
#define BP_ANTI_BANG_COEF 10.0f
#define BP_ANTI_BANGDAMAGE_COEF 5.0f
#define BP_FINAL_DAMAGE_COEF 1.0f
#define BP_RECOVERHP_COEF 5.0f
#define BP_ABSORBHP_COEF   100.0f
#define BP_DAMAGE_REFLECT_COEF  100.0f
#define BP_FIRE_DAMAGE_COEF  1.0f
#define BP_ICE_DAMAGE_COEF 1.0f
#define BP_THUNDER_DAMAGE_COEF 1.0f
#define BP_FIRE_RATE_COEF 40.0f
#define BP_ICE_RATE_COEF 40.0f
#define BP_THUNDER_RATE_COEF 40.0f
#define BP_DAMAGEREDUCE_COEF 1.0f

#define BP_MP_BASE  1000
#define BP_HITRATE_BASE 100
#define BP_BANGDAMAGE_BASE 50

#define Capture_Need_Lilian 5
#define LEAVE_GUILD_COOLDOWN 86400

#define SERVER_SECRET "aDfbx4NSRprgEKpPY7RwTC4XI7OzQRHC"

#define ONE_2_ONE_PLAYER_HP_COEF 11
#define ONE_2_ONE_HERO_HP_COEF 12

#define TEAM_PLAYER_HP_COEF 11
#define TEAM_HERO_HP_COEF 12

#define HERO_LEAGUE_PLAYER_HP_COEF 11
#define HERO_LEAGUE_HERO_HP_COEF 12

#define EMERALD_DREAM_PLAYER_HP_COEF  11
#define EMERALD_DREAM_HERO_HP_COEF  12

#define DRAGON_NEST_PLAYER_HP_COEF  11
#define DRAGON_NEST_HERO_HP_COEF  12

#define STONE_COPYMAP_PLAYER_HP_COEF  11
#define STONE_COPYMAP_HERO_HP_COEF  12  

#define POINTCARD_2_DIAMOND_RATE 10

#define GIVE_LILIAN_NUM_PER_TIME 3

#define CHARID_ZONE_INFO_MASK 0xFFF
#define CHARID_ID_INFO_MASK 0xFFFFF

#define ARENA_HP_COEF 6			//天梯血量加成
#define ARENA_HP_HERO_COEF 12			//天梯血量加成
#define INTERCEPT_HP_COEF 6		//护送血量加成
#define INTERCEPT_HP_HERO_COEF 12		//护送血量加成
#define LEITAI_HP_COEF 11		//擂台血量加成 
#define LEITAI_HP_HERO_COEF 12		//擂台血量加成 

enum eLoginIPType
{
	eWhite_Login_IP = 1,            //白名单
	eBlack_Login_IP = 2,            //黑名单
};

//装备所在孔的索引
enum
{
	HEAD_SLOT_INDEX = 0,				//头部
	BODY_SLOT_INDEX = 1,				//衣服
	WEAPON_SLOT_INDEX = 2,				//武器
	SHOE_SLOT_INDEX = 3,				//鞋子
	NECKLACE_SLOT_INDEX = 4,			//项链
	CLOAK_SLOT_INDEX = 5,				//披风
	RING_SLOT_INDEX = 6,				//戒子
	WING_SLOT_INDEX = 7,				//翅膀

	FASHION_HEAD_SLOT_INDEX = 8,		
	FASHION_BODY_SLOT_INDEX = 9,
	FASHION_WEAPON_SLOT_INDEX = 10,
	FASHION_SHOE_SLOT_INDEX = 11,		//裤子
	FASHION_FACE_SLOT_INDEX = 12,
	FASHION_CLOAK_SLOT_INDEX = 13,
	FASHION_RING_SLOT_INDEX = 14,		//脸饰品
	FASHION_WING_SLOT_INDEX = 15,		

	HEAD_CORE_FUWEN_INDEX = 16,			//头部核心符文
	HEAD_FIRE_FUWEN_INDEX = 17,			//头部火符文
	HEAD_WATER_FUWEN_INDEX = 18,		//头部水符文
	HEAD_WIND_FUWEN_INDEX = 19,			//头部风符文
	HEAD_SOIL_FUWEN_INDEX = 20,			//头部土符文

	BODY_CORE_FUWEN_INDEX = 21,			//衣服核心符文
	BODY_FIRE_FUWEN_INDEX = 22,			//衣服火符文
	BODY_WATER_FUWEN_INDEX = 23,		//衣服水符文
	BODY_WIND_FUWEN_INDEX = 24,			//衣服风符文
	BODY_SOIL_FUWEN_INDEX = 25,			//衣服土符文

	WEAPON_CORE_FUWEN_INDEX = 26,		//武器核心符文
	WEAPON_FIRE_FUWEN_INDEX = 27,		//武器火符文
	WEAPON_WATER_FUWEN_INDEX = 28,		//武器水符文
	WEAPON_WIND_FUWEN_INDEX = 29,		//武器风符文
	WEAPON_SOIL_FUWEN_INDEX = 30,		//武器土符文

	SHOE_CORE_FUWEN_INDEX = 31,			//鞋核心符文
	SHOE_FIRE_FUWEN_INDEX = 32,			//鞋火符文
	SHOE_WATER_FUWEN_INDEX = 33,		//鞋水符文
	SHOE_WIND_FUWEN_INDEX = 34,			//鞋风符文
	SHOE_SOIL_FUWEN_INDEX = 35,			//鞋土符文

	FACE_CORE_FUWEN_INDEX = 36,			//脸核心符文
	FACE_FIRE_FUWEN_INDEX = 37,			//脸火符文
	FACE_WATER_FUWEN_INDEX = 38,		//脸水符文
	FACE_WIND_FUWEN_INDEX = 39,			//脸风符文
	FACE_SOIL_FUWEN_INDEX = 40,			//脸土符文

	CLOAK_CORE_FUWEN_INDEX = 41,		//披风核心符文
	CLOAK_FIRE_FUWEN_INDEX = 42,		//披风火符文
	CLOAK_WATER_FUWEN_INDEX = 43,		//披风水符文
	CLOAK_WIND_FUWEN_INDEX = 44,		//披风风符文
	CLOAK_SOIL_FUWEN_INDEX = 45,		//披风土符文

	RING_CORE_FUWEN_INDEX = 46,			//戒子核心符文
	RING_FIRE_FUWEN_INDEX = 47,			//戒子火符文
	RING_WATER_FUWEN_INDEX = 48,		//戒子水符文
	RING_WIND_FUWEN_INDEX = 49,			//戒子风符文
	RING_SOIL_FUWEN_INDEX = 50,			//戒子土符文

	SOUL_BAG_POS1_INDEX = 51,			//灵魂包裹位置
	SOUL_BAG_POS2_INDEX = 52,
	SOUL_BAG_POS3_INDEX = 53,
	SOUL_BAG_POS4_INDEX = 54,
	SOUL_BAG_POS5_INDEX = 55,
	SOUL_BAG_POS6_INDEX = 56,

	MOUNT_BAG_POS1_INDEX = 57,			//坐骑包裹位置
	MOUNT_BAG_POS2_INDEX = 58,
	MOUNT_BAG_POS3_INDEX = 59,
	MOUNT_BAG_POS4_INDEX = 60,
	MOUNT_BAG_POS5_INDEX = 61,
	MOUNT_BAG_POS6_INDEX = 62,
	MOUNT_BAG_POS7_INDEX = 63,
	MOUNT_BAG_POS8_INDEX = 64,
};

enum eGuardQuality
{
	eGuard_Qaulity_1 = 1,				//护送品质1
	eGuard_Qaulity_2 = 2,				//护送品质2
	eGuard_Qaulity_3 = 3,				//护送品质3
	eGuard_Qaulity_4 = 4,				//护送品质4
	eGuard_Qaulity_5 = 5,				//护送品质5
};

enum RaiseSkill 
{  
	eRaiseSkill_Power = 1,
	eRaiseSkill_ExtraPower = 2,
	eRaiseSkill_ReduceCost = 3,
	eRaiseSkill_ReduceCD = 4,
};

#define MAX_ROBOT_CHARID 50000


enum Sdk_Type {
    _11Wan = 1700,
    MoreFun = 1800,
    _Easy = 1900,
    FairyFire = 2000,
    MoeYooo = 2100,
	Starpy = 2300,
	Tipcat = 2400,
	XiaoMi = 2500,
	XueGao = 2600,
	GoldGame = 2700,
	YuWan = 2900,
 	Sagegame = 3000,
	_1tsdk = 3100,
	_3hgame = 3200,
	_1tsdk_ = 3300,
	_01game = 3400,
	Quicksdk = 3500,
	QuicksdkHW = 3510,
	Xinyu = 3600,
	DongNanYa = 3700,
	HuaWei = 3800,
	_51sfsy = 3900,
	H365 = 4000,
	Laosiji = 4100,
	NftWallet = 4200,
	JGG = 4300,
	GongKou = 4400,

	BR_sdk = 4500,
	Sdk_Max = 10000,
};

enum PayChannel {
	PayH365 = 1,
	PayLaosiji = 2,
};


#endif
