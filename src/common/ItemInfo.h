#ifndef __ITEMINFO_H__
#define __ITEMINFO_H__

#include <cstdlib>
#include "TypesDef.h"
#include "CommonMicro.h"

#pragma pack(1)

enum
{
	ITEM_CLASSTYPE_GREEN_MIN_VAL = 1,		//绿色最小
	ITEM_CLASSTYPE_GREEN_MAX_VAL = 3,		//绿色最大	 
	ITEM_CLASSTYPE_BULE_MIN_VAL  = 4,		//蓝色最小
	ITEM_CLASSTYPE_BULE_MAX_VAL  = 10,		//蓝色最大
	ITEM_CLASSTYPE_PURPLE_MIN_VAL = 11,		//紫色最小
	ITEM_CLASSTYPE_PURPLE_MAX_VAL = 20,		//紫色最大
	ITEM_CLASSTYPE_ORANGE_MIN_VAL = 21,		//橙色最小
	ITEM_CLASSTYPE_ORANGE_MAX_VAL = 30,		//橙色最大

	
	ITEM_CLASSTYPE_N_MIN_VAL = 1,		//n最小
	ITEM_CLASSTYPE_N_MAX_VAL = 3,		//n最大
	ITEM_CLASSTYPE_R_MIN_VAL = 4,		//r最小
	ITEM_CLASSTYPE_R_MAX_VAL = 11,		//r最大
	ITEM_CLASSTYPE_SR_MIN_VAL = 12,		//sr最小
	ITEM_CLASSTYPE_SR_MAX_VAL = 21,		//sr最大
	ITEM_CLASSTYPE_SSR_MIN_VAL = 22,		//ssr最小
	ITEM_CLASSTYPE_SSR_MAX_VAL = 25,		//ssr最大
	ITEM_CLASSTYPE_UR_MIN_VAL = 26,		//Ur最小
	ITEM_CLASSTYPE_UR_MAX_VAL = 30,		//Ur最大
};

//道具位置
struct BagPos
{
	UINT8 m_bagType;   //所在包裹类型
	UINT8 m_pos;	   //所在的包裹格子位置

	BagPos()
	{
		m_bagType = 0;
		m_pos = 0;
	}
};

enum
{
	//时装类
	ITEM_TYPE_FASHION_HEAD = 10,		//时装头部
	ITEM_TYPE_SPECIAL_FASHION_HEAD = 11,//特殊头部时装
	ITEM_TYPE_FASHION_BODY = 20,		//时装上衣
	ITEM_TYPE_FASHION_WEAPON = 30,		//时装武器
	ITEM_TYPE_FASHION_SHOE = 40,		//时装鞋子
	ITEM_TYPE_FASHION_FACE = 50, 		//时装表情
	ITEM_TYPE_FASHION_CLOAK = 60,		//时装披风
	ITEM_TYPE_FASHION_RING = 70,		//时装戒子
	ITEM_TYPE_FASHION_WING = 80,		//时装翅膀
	ITEM_TYPE_FASHION_SUIT = 90,		//时装套装
	ITEM_TYPE_FUWEN 	= 100,			//符文

	//装备类
	ITEM_TYPE_HEAD = 110,				//头部
	ITEM_TYPE_BODY = 120,				//衣服
	ITEM_TYPE_WEAPON = 130,				//武器
	ITEM_TYPE_SHOE = 140,				//鞋子
	ITEM_TYPE_NECKLACE = 150,			//项链
	ITEM_TYPE_CLOAK = 160,				//披风
	ITEM_TYPE_RING = 170,				//戒子
	ITEM_TYPE_WING = 180,				//翅膀
	ITEM_TYPE_SUIT = 190,				//套装
	
	ITEM_TYPE_TILI = 210,				//体力丹
	ITEM_TYPE_LILIAN = 220,				//历练丹
	ITEM_TYPE_LINGXING = 230,			//灵性丹
	ITEM_TYPE_LINGWUJINGYUAN = 240,		//领悟经验丹
	ITEM_TYPE_LINGWU		= 250,		//领悟丹
	ITEM_TYPE_TRAINTILI = 254,			//调教体力丹
	ITEM_TYPE_GUAJI = 255,				//增加挂机时间道具
	ITEM_TYPE_JINGLIAN_IRON	= 260,		//精炼矿石
	ITEM_TYPE_JUEXING_BOOK = 263,		//觉醒书
	ITEM_TYPE_LINGWU_BOOK  = 264,		//领悟书
	ITEM_TYPE_RATE_BOOK  = 265,		    //倍数经验之书
	ITEM_TYPE_CHG_SOUL		= 266,		//换魂石
	ITEM_TYPE_EXP_BOOK		= 268,		//经验书
	ITEM_TYPE_FIRE_WORK 	= 269,		//烟火
	ITEM_TYPE_BOX1			= 270,		//宝箱(需要钥匙)
	ITEM_TYPE_BOX2			= 280,		//宝箱(不需要钥匙)
	ITEM_TYPE_BOX3			= 281,
	ITEM_TYPE_BOX4			= 283,      //累计消耗钻石(周消耗活动)
	ITEM_TYPE_BOX5			= 284,      //累计充值获得称号(累计充值活动)
	ITEM_TYPE_BOX9			= 288,      //成长计划 月卡 福利卡等
	ITEM_TYPE_BOX6			= 271,		//等级直升宝箱
	ITEM_TYPE_GIFTBOX		= 290,		//礼包
	ITEM_TYPE_FORCE_FINISH_TASK = 291,	//跳关卡
	ITEM_TYPE_REMOVE_ALL_ITEM = 292,	//一键卸除灵魂祭坛装备
	ITEM_TYPE_IRON			= 310,		//陨铁
	ITEM_TYPE_TITLE         = 311,      //称号类道具
	ITEM_TYPE_GIVE_VIPEXP        = 312, //赠送VIP经验
	ITEM_TYPE_VIPEXP   = 313,			//VIP经验
	ITEM_TYPE_JINGLIAN		 = 320,		//精炼石
	ITEM_TYPE_TAIJIXINGFU	= 330,		//太极练星符
	ITEM_TYPE_BREAKINTO		= 340,		//突破丹
	ITEM_TYPE_MONUT_FEED	= 341,		//坐骑饲料
	ITEM_TYPE_MONUT_ADVANCED = 342,		//坐骑进阶药
	ITEM_TYPE_UNREAL		=	343,	//坐骑幻化水
	ITEM_TYPE_SUMMON		=	344,	//马的召唤圣物
	ITEM_TYPE_WUHUNSHI		= 350,		//武魂石
    ITEM_TYPE_ENCHANT       = 353,      //附魔石
	ITEM_TYPE_ILLUSION      = 354,		//幻化经验丹
	ITEM_TYPE_RUSH_TICKET   = 355,      //扫荡券
	ITEM_TYPE_TRAIN = 356,				//调教道具
	ITEM_TYPE_YAOSHI		= 360,		//钥匙
	ITEM_TYPE_YUNTIE_PIECE  = 390,		//陨铁碎片
	ITEM_TYPE_JINGLIAN_PIECE = 391,		//精炼水晶碎片
	ITEM_TYPE_FUWEN_PIECE  = 392,		//核心符文碎片
	ITEM_TYPE_FUWEN_EXP_PIECE = 393,	//符文经验石
	ITEM_TYPE_DIAMOND_CARD  = 394,		//钻石卡碎片
	ITEM_TYPE_GOLDCOIN_CARD = 395,		//金币卡碎片
	ITEM_TYPE_PET_SOUL_STONE = 400,		//宠物灵魂石头
	
	ITEM_TYPE_FUWEN_CORE = 410,			//核心符文
	ITEM_TYPE_FUWEN_FIRE  = 420,		//火符文
	ITEM_TYPE_FUWEN_WATER = 430,		//水符文
	ITEM_TYPE_FUWEN_WING = 440,			//风符文
	ITEM_TYPE_FUWEN_SOIL = 450,			//土符文
	
	ITEM_TYPE_FUWEN_EXP_STONE = 460,	//符文经验石

    ITEM_TYPE_FUWEN_STONE_FIRE  = 470,      //高阶火符文
    ITEM_TYPE_FUWEN_STONE_ICE  = 480,       //高阶冰符文
    ITEM_TYPE_FUWEN_STONE_THUNDER = 490,    //高阶雷符文
    ITEM_TYPE_FUWEN_STONE_EXP = 500,        //高阶符文经验丹

	//碎片类
	ITEM_TYPE_FASHION_HEAD_PIECE = 510,		//时装头部碎片
	ITEM_TYPE_FASHION_BODY_PIECE = 520,		//时装上衣碎片
	ITEM_TYPE_FASHION_WEAPON_PIECE = 530,	//时装武器碎片
	ITEM_TYPE_FASHION_SHOE_PIECE = 540,		//时装鞋子碎片
	ITEM_TYPE_FASHION_FACE_PIECE = 550,		//时装项链碎片
	ITEM_TYPE_FASHION_CLOAK_PIECE = 560,    //时装披风碎片
	ITEM_TYPE_FASHION_RING_PIECE = 570,	    //时装戒子碎片
	ITEM_TYPE_FASHION_WING_PIECE = 580,	  	//时装翅膀碎片
	ITEM_TYPE_FASHION_SUIT_PIECE = 590,		//套装碎片

	ITEM_TYPE_HEAD_PIECE = 610,			  //头部碎片
	ITEM_TYPE_BODY_PIECE = 620,			  //上衣碎片
	ITEM_TYPE_WEAPON_PIECE = 630,		  //武器碎片
	ITEM_TYPE_SHOE_PIECE = 640,			  //鞋子碎片
	ITEM_TYPE_NECKLACE_PIECE = 650,		  //项链碎片
	ITEM_TYPE_CLOAK_PIECE = 660,  		  //披风碎片
	ITEM_TYPE_RING_PIECE = 670,	  		  //戒子碎片
	ITEM_TYPE_WING_PIECE = 680,	  		  //翅膀碎片

	ITEM_TYPE_FUWEN_FIRE_PIECE = 720,	  //火符文碎片
	ITEM_TYPE_FUWEN_WATER_PIECE = 730,	  //水符文碎片
	ITEM_TYPE_FUWEN_WIND_PIECE = 740,	  //风符文碎片
	ITEM_TYPE_FUWEN_SOIL_PIECE  = 750,	  //土符文碎片
	ITEM_TYPE_FUWEN_LIFE_BOOK_PIECE = 760,//觉醒生命之书残页

    ITEM_TYPE_FUWEN_STONE_PIECE_FIRE  = 770,      //高阶火符文原石
    ITEM_TYPE_FUWEN_STONE_PIECE_ICE  = 780,       //高阶冰符文原石
    ITEM_TYPE_FUWEN_STONE_PIECE_THUNDER = 790,    //高阶雷符文原石

	ITEM_TYPE_MOUNT_LIFE_STONE = 910,		//坐骑生命晶石
	ITEM_TYPE_MOUNT_DAMAGE_STONE = 920,		//坐骑伤害晶石
	ITEM_TYPE_MOUNT_HIT_STONE = 930,		//坐骑命中晶石
	ITEM_TYPE_MOUNT_CRIT_STONE = 940,		//坐骑暴击晶石
	ITEM_TYPE_MOUNT_RESIST_STONE = 950,		//坐骑抗暴晶石
	ITEM_TYPE_MOUNT_FIRE_STONE = 960,		//坐骑火伤晶石
	ITEM_TYPE_MOUNT_ICE_STONE = 970,		//坐骑冰伤晶石
	ITEM_TYPE_MOUNT_RAY_STONE = 980,		//坐骑雷伤结晶
};

#define  MAX_EQUIP_SLOT_NUM 6

struct ItemInfo 
{
	ItemInfo()
	{
		m_itemID  = 0;
		m_instanceID = 0;
		m_num = 0;
		m_raiseLv = 0;
		m_refineLv = 0;
		m_exp = 0;
		bzero(m_slot,sizeof(m_slot));
		m_hasGotSkill = 0;
		m_expireTime = 0;
		m_breakLv = 0;
		bzero(m_reserve,sizeof(m_reserve));
	}

	UINT32	  m_itemID;						//道具ID
	UINT64    m_instanceID;					//实例ID
	UINT32    m_num;						//道具数量
	UINT16    m_raiseLv;					//强化等级
	UINT16	  m_refineLv;					//精炼等级
	UINT32 	  m_exp;						//装备经验
	BagPos    m_pos;						//所在位置
	struct {
			  UINT32 m_skillID;				//技能ID
			  UINT8  m_skillLv;				//技能等级
	}m_slot[MAX_EQUIP_SLOT_NUM];			//觉醒孔
	UINT32    m_hasGotSkill;				//已经觉醒到的技能
	UINT32	  m_expireTime;					//失效时间
	UINT8     m_breakLv;					//突破等级
	char      m_reserve[15];				//道具属性保留
};

#pragma pack()
#endif


