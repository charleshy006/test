#ifndef __H_DRAW_MGR_H__
#define __H_DRAW_MGR_H__
#include "TypesDef.h"
#include "Misc.pb.h"
#include <map>
#include <tinyxml/tinyxml.h>

class GameUser;
class CDrawMgr 
{
public :
	static const UINT32 s_FREE_GOLD_DRAW_MAX_NUM = 5;
	static const UINT32 s_DRAW_CARD_ITEM_ID = 122609;
	static const UINT32 s_ONE_DRAW_CARD_ITEM_ID = 122209;
	enum eFlag
	{
		eFlag_First_Diamond_Draw = 1,				//首次钻石抽奖
		eFlag_First_Free_Diamond_Draw = 2,			//首次免费钻石抽奖
		eFlag_First_10_Diamond_Draw = 4,			//首次钻石10连抽
		eFlag_Second_10_Diamond_Draw = 8,			//第二次钻石10连抽
		eFlag_First_10Gold_Draw = 16,				//第一次金币10连抽
		eFlag_First_Gold_Draw = 32,					//第一次金币抽奖
	};
public :
	struct stDrawItem
	{
		UINT32 m_id;			//道具ID
		UINT32 m_num;			//数量
		UINT32 m_rate;			//概率

		stDrawItem()
		{
			m_id = 0;
			m_num = 0;
			m_rate = 0;
		}
	};

	struct stDrawInfo
	{
		UINT32 m_moneyType;
		UINT32 m_needMoney;
		UINT32 m_interval;

		std::vector<stDrawItem> m_items;	

		stDrawInfo()
		{
			m_moneyType = 0;
			m_needMoney = 0;	
			m_interval = 0;
		}
	};

	struct stLimitPetDrawInfo
	{
		stDrawInfo m_drawHeroCfg;				//武神抽取配置
		stDrawInfo m_10DrawHeroCfg;				//十次武神抽取配置
		stDrawInfo m_drawHero10Mod10DrawCfg; 	//10倍数时候的抽奖
		stDrawInfo m_drawHero4Mod3DrawCfg;		//4倍数时候的抽奖
		
		stLimitPetDrawInfo()
		{
			
		}
	};
public : 
	//构造函数
	CDrawMgr(GameUser * pOwner);
	//析构函数
	~CDrawMgr();
	//保存
	UINT32 save(byte * buf,UINT32 needSpace);
	//加载
	UINT32 load(byte * buf);
	//检查刷新
	void checkRefresh();
	//开打抽奖界面
	void openDrawWindow();
	//背包格子检查
	bool checkBagSpace(MiscProto::eDrawType type);
	//是否可以免费
	bool canFreeDraw(MiscProto::eDrawType type);
	//扣除抽奖花费
	bool reduceCost(MiscProto::eDrawType type);
	//给抽奖道具,
	void giveDrawItem(std::vector<stDrawItem> & fromVec,std::vector<std::pair<UINT32,UINT32> > & retCnt,UINT8 drawNum);
	//抽奖
	void draw(MiscProto::eDrawType type);
	//设置某个标志
	void setFlag(eFlag flag);
	//是否设置了某个标志
	bool isSetFlag(eFlag flag);
	//获得钻石抽奖信息
	stDrawInfo * getDrawInfoByDiamondDrawNum(bool bTen);
	//获得武将抽奖信息
	stDrawInfo * getDrawInfoByHeroDrawNum(bool bTen);
	//获得宝藏抽奖的信息
	stDrawInfo * getDrawInfoByTreasureDrawNum(bool bTen);
	//解析某个子节点
	static void parseSubNode(TiXmlElement * pSubNode,stDrawInfo & cfg);
	//加载所有配置
	static bool loadCfg();
	//获得当前抽奖ID
	static stLimitPetDrawInfo  * getLimitPetDrawInfo(); 
private :
	GameUser * m_pOwner;						
	UINT32     m_lastFreeGoldDrawTime;			//最近一次进行金币抽奖时间
	UINT16     m_freeGoldDrawNum;				//已经进行了免费金币抽奖次数
	UINT32     m_lastFreeDiamondDrawTime;		//最近一次进行钻石抽奖时间
	UINT32 	   m_lastFreeDrawHeroTime;			//最近一次武神免费抽奖时间
	UINT32	   m_lastFreeDrawTreasureTime;		//最近一次宝藏抽奖时间

	UINT32	   m_diamondDrawNum;				//钻石抽奖次数
	UINT32     m_heroDrawNum;					//武神抽奖次数
	UINT32 	   m_treasureDrawNum;				//宝藏抽奖次数

	UINT16 	   m_flag;							//标志的设置
	UINT32	   m_drawHeroScore;					//抽武将积分
	UINT32	   m_clearDrawHeroTime;				//清空抽武将积分时间

	static stDrawInfo m_gold1DrawCfg;			//金币抽奖
	static stDrawInfo m_gold10DrawCfg;			//金币10连抽
	static stDrawInfo m_firstGoldDrawCfg;		//首次金币抽奖
	static stDrawInfo m_first10GoldDrawCfg;		//首次金币10连抽

	static stDrawInfo m_diamond1DrawCfg;		//钻石抽奖
	static stDrawInfo m_diamond10DrawCfg;		//钻石10连抽
	static stDrawInfo m_firstDiamondDrawCfg;	//第一次钻石抽奖
	static stDrawInfo m_firstFreeDiamondDrawCfg;//首次免费钻石抽奖
	static stDrawInfo m_first10DiamondDrawCfg;	//第一次钻石10连抽
	static stDrawInfo m_second10DiamondDrawCfg;	//第二次钻石10连抽
	static stDrawInfo m_diamond10Mod10DrawCfg;	//抽奖次数是10倍数时候
	static stDrawInfo m_diamond150Mod2DrawCfg;	//抽奖次数是150余2时候
	static stDrawInfo m_diamond150Mod32DrawCfg; //抽奖次数是150余32时候
	static stDrawInfo m_diamond150Mod62DrawCfg;	//抽奖次数是150余62时候
	static stDrawInfo m_diamond150Mod92DrawCfg;	//抽奖次数是150余92时候
	static stDrawInfo m_diamond150Mod122DrawCfg;//抽奖次数是150余122时候
	static stDrawInfo m_diamond30Mod12DrawCfg;	//抽奖次数是30余12时候

	//static stDrawInfo m_drawHeroCfg;			//武神抽取配置
	//static stDrawInfo m_10DrawHeroCfg;			//十次武神抽取配置
	//static stDrawInfo m_drawHero10Mod10DrawCfg; //10倍数时候的抽奖
	//static stDrawInfo m_drawHero3Mod3DrawCfg;	//3倍数时候的抽奖
	static std::map<UINT16,stLimitPetDrawInfo> m_limitPetDrawInfoCnt;

	static stDrawInfo m_treasureDrawCfg;		//宝藏抽奖
	static stDrawInfo m_treasure10DrawCfg;		//十次宝藏抽奖
	static stDrawInfo m_treasure10Mod10DrawCfg; //宝藏10的倍数时候抽奖
	static stDrawInfo m_treasure3Mod3DrawCfg;	//宝藏3的倍数时候抽奖
};
#endif
