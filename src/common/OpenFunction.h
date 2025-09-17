#ifndef __H_OPENFUNCTION_CFG_H__
#define  __H_OPENFUNCTION_CFG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <map>

enum eOpenFunID
{
	OpenFunID_Lilian = 0,      //历练
	OpenFunID_Arena = 1,      //竞技场
	OpenFunID_Capture = 2,      //夺宝
	OpenFunID_TeamCopy_Normal = 3,  //组队副本
	OpenFunID_TeamCopy_Elite = 4,  //精英副本
	OpenFunID_One2OnePVP = 5,    //1vs1
	OpenFunID_RealTime = 6,      //实时战场
	OpenFunID_Moba = 7,        //实时战场
	OpenFunID_Leitai_Low = 8,    //擂台低级
	OpenFunID_Leitai_High = 9,    //擂台高级
	OpenFunID_Spacecraft_Escort = 11,//飞船护送
	OpenFunID_Discover = 12,    //秘境寻宝
	OpenFunID_HeroLeague = 13,    //英雄联赛
	OpenFunID_Raise_Equip = 21,    //强化状态
	OpenFunID_Refine_Equip = 22,  //精炼
	OpenFunID_JueXing = 23,      //觉醒
	OpenFunID_Equip_Break = 24,    //装备突破
	OpenFunID_SoulLink = 25,    //灵魂祭坛
	OpenFunID_Heishi_Shop = 26,    //黑市商店
	OpenFunID_FashionAttach = 41,  //时装附魔
	OpenFunID_Wing_Up = 42,      //翅膀飞升
	OpenFunID_Raise_Skill = 51,    //技能强化
	OpenFunID_RareSkill_Study = 52,  //稀有技能强化
	OpenFunID_Arrange_Hero = 61,  //随从编组
	OpenFunID_Raise_Hero = 62,    //强化随从
	OpenFunID_Hero_StudySkill = 63,  //随从领悟
	OpenFunID_Draw_Limit_Hero = 64,  //显示武神抽奖
	OpenFunID_Retinue_Expedition = 65,//随从远征
	OpenFunID_Team_Two_2_Hero_Open = 66,//第二组随从开启
	OpenFunID_Guild = 80,      //公会
	OpenFunID_Sprite_Levelup = 101,  //精灵升级
	OpenFunID_Huanhua = 102,      //幻化
	OpenFunID_Tongling = 103,    //通灵
	OpenFunID_Wear_Funwen  = 110,  //装备符文
	OpenFunID_Raise_Fuwen = 111,    //符文强化
	OpenFunID_Gem = 112,            //宝石合成
	OpenFunID_Activate_Title = 120,  //激活称号
	OpenFunID_Gold_Draw = 160,    //金币抽奖
	OpenFunID_10_Gold_Draw = 161,  //金币10连抽
	OpenFunID_Diamond_Draw = 162,  //钻石抽奖
	OpenFunID_10Diamond_Draw = 163,  //钻石10连抽
	OpenFunID_OpenSvrBag = 170,    //开服礼包
	OpenFunID_Small_Monster = 231,  //打小怪兽
};

class COpenFunction
{
public :
	//构造函数
	COpenFunction();
	//析构函数
	virtual ~COpenFunction();
	//加载所有配置
	static bool loadCfg();
	//检查功能是否满足条件
	bool checkFunOpen(eOpenFunID funID);
	//获得功能对应延时天数
	static UINT8 getOpenDay(eOpenFunID funID);
	//获得等级
	virtual UINT16 getLevel() = 0;
private :
	static std::map<UINT16,UINT16> m_funID2Lvs;		//功能号与等级
	static std::map<UINT16,UINT8> m_funID2Day;		//功能号与延时天数
};
#endif
