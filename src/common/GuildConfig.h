#ifndef __H_ARMYCONFIG_H__
#define __H_ARMYCONFIG_H__
#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>

class CGuildConfig
{
	USE_SINGLETON_NEW(CGuildConfig);
public :
	struct stCreateGuildInfo
	{
		UINT32 m_needLevel;			//需要等级 
		UINT32 m_needGoldCoin;		//需要金币
		UINT32 m_needDiamond;		//需要钻石

		stCreateGuildInfo()
		{
			m_needLevel = 0;
			m_needGoldCoin = 0;
			m_needDiamond = 0;
		}
	};
	
	//大厅配置
	struct stHallInfo
	{
		UINT16 m_lv;			//等级
		UINT32 m_contribute;	//升级需要贡献
		UINT16 m_maxMem;		//最大成员
		UINT8  m_viceLeaderNum; //副会长数量
		UINT8  m_eliteNum ;		//精英数量

		stHallInfo()
		{
			m_lv = 0;
			m_contribute = 0;
			m_maxMem = 0;
			m_viceLeaderNum = 0;
			m_eliteNum = 0;
		}
	};

	//祈福
	struct stPrayInfo
	{
		UINT16 m_lv;				//等级
		UINT32 m_contribute;		//升级需要贡献度
		UINT16 m_maxPrayNum;		//最大祈福次数
		UINT32 m_item1ID;			//道具1的ID
		UINT32 m_item1Num;			//道具1的数量
		UINT32 m_item2ID;			//道具2的ID
		UINT32 m_item2Num;			//道具2的数量
		UINT32 m_item3ID;			//道具3的ID
		UINT32 m_item3Num;			//道具3的数量


		stPrayInfo()
		{
			m_lv = 0;
			m_contribute = 0;
			m_maxPrayNum = 0;
			m_item1ID = 0;
			m_item1Num = 0;
			m_item2ID = 0;
			m_item2Num = 0;
			m_item3ID = 0;
			m_item3Num = 0;
		}
	};

	//商店成长
	struct stShopInfo
	{
		UINT16 m_lv ;		//等级
		UINT32 m_contribute;//贡献度

		stShopInfo()
		{
			m_lv = 0;
			m_contribute = 0;
		}
	};

	//捐赠配置
	struct stDonateInfo
	{
		UINT8  m_donateType;			//捐赠类型
		UINT8  m_costResType;			//消耗资源类型
		UINT32 m_costResNum;			//消耗资源数量
		UINT32 m_donateNum;				//捐赠数量
		UINT32 m_addContribute;			//增加贡献度

		stDonateInfo()
		{
			m_donateType = 0;
			m_costResType = 0;
			m_costResNum = 0;
			m_donateNum = 0;
			m_addContribute = 0;
		}
	};
	
	enum eRight
	{
		eRight_Manifesto = 1,		//宣言
		eRight_Audit = 2,			//审核
		eRight_Dissolve = 3,		//解散
		eRight_Dislodge = 4,		//驱散
		eRight_Nofity = 5,			//公告
		eRight_OpenMap = 6,			//开启副本
		eRight_Lvup = 7,			//升级
		eOpen_Manor = 8,			//开启领地
	};

	//权限
	struct stRightInfo
	{
		UINT8  m_title;			//头衔
		bool   m_manifesto;		//宣言
		bool   m_audit;			//审核
		bool   m_dissolve;		//解散
		bool   m_dislodge;		//驱逐成员
		bool   m_notify;		//公告
		bool   m_openCopymap;	//开启副本
		bool   m_lvUp;			//升级

		stRightInfo()
		{
			m_title = 0;
			m_manifesto = false;
			m_audit = false;
			m_dissolve = false;
			m_dislodge = false;
			m_notify = false;
			m_openCopymap = false;
			m_lvUp = false;
		}
	};

	//公会副本奖励
	struct stCopyMapReward
	{
		UINT32 m_levelID;			//关卡ID
		UINT32 m_completeBoxID;		//完成boxID
		UINT32 m_killBoxID;			//击杀boxID
		UINT32 m_bossID;			//获得bossID

		stCopyMapReward()
		{
			m_levelID = 0;
			m_completeBoxID = 0;
			m_killBoxID = 0;
			m_bossID = 0;
		}
	};
	
	//领地争夺阶段时间
	struct stManorFightStepTime
	{
		UINT16 m_id;				//阶段ID
		UINT32 m_breakBeginTime;	//准备时间
		UINT32 m_breakEndTime;		//准备结束时间
		UINT32 m_fightBeginTime;	//开始战斗时间
		UINT32 m_fightEndTime;		//战斗结束时间

		stManorFightStepTime()
		{
			m_id = 0;
			m_breakBeginTime = 0;
			m_breakEndTime = 0;
			m_fightBeginTime = 0;
			m_fightEndTime = 0;
		}
	};
	//领地争夺时间
	struct stManorFightTime
	{
		UINT16 m_id;					//id,唯一标志
		UINT32 m_applyBeginTime;		//开始申请,都是距离一周的秒算
		UINT32 m_applyEndTime;			//申请结束,同上
		UINT32 m_readyBeginTime;		//准备开始时间,同上
		UINT32 m_readyEndTime;			//准备结束时间,同上
		UINT32 m_applyOverBeginTime; 	//申请结束开始
		UINT32 m_applyOverEndTime;		//申请结束开始
		UINT32 m_giveRewardBeginTime;	//结算时间,同上
		UINT32 m_giveRewardEndTime;		//结算结束时间
		std::map<UINT16,stManorFightStepTime> m_steps;

		stManorFightTime()
		{
			m_id = 0;
			m_applyBeginTime = 0;
			m_applyEndTime = 0;
			m_readyBeginTime = 0;
			m_readyEndTime = 0;
			m_applyOverBeginTime = 0;
			m_applyOverEndTime = 0;
			m_giveRewardBeginTime = 0;
			m_giveRewardEndTime = 0;
		}
	};

	//领地争夺地图信息
	struct stManorFightInfo
	{
		UINT32 m_mapID;
		UINT32 m_winBoxID;
		UINT32 m_loserBoxID;

		stManorFightInfo()
		{
			m_mapID = 0;
			m_winBoxID = 0;
			m_loserBoxID = 0;
		}
	};

	//领地信息
	struct stManorInfo
	{
		UINT32 m_id;
		UINT16 m_star;
		UINT32 m_prayExtraBoxID;			//祈祷额外boxID

		stManorInfo()
		{
			m_id = 0;
			m_star = 0;
			m_prayExtraBoxID = 0;
		}
	};

	//工会黑市
	struct stBlackMarketInfo
	{
		UINT16	m_lv;				//黑市等级
		UINT32	m_construction;		//成长所需建设点数
		UINT8	m_purchasequantity;	//每日个人购买限制次数
		UINT8	m_limitfreshtime;	//每日刷新次数限制
		UINT32	m_shopID;			//引用shop的商店ID
		UINT8	m_free;				//免费刷新次数
		UINT16	m_freshprice;		//刷新价格	

		stBlackMarketInfo()
		{
			m_lv				=	0;              
			m_construction		=	0;    
			m_purchasequantity	=	0;
			m_limitfreshtime	=	0;
			m_shopID			=	0;          
			m_free				=	0;
			m_freshprice		=	0;
		}
	};

public :
	//构造函数 
	static CGuildConfig & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置 
	bool loadCfg();
	//获得创建需要等级
	UINT16  createNeedLv() { return m_createInfo.m_needLevel ;}
	//创建需要金币
	UINT32  createNeedGoldCoin() { return m_createInfo.m_needGoldCoin ;}
	//创建需要钻石
	UINT32 createNeedDiamond() { return  m_createInfo.m_needDiamond; }
	//获得大厅配置
	stHallInfo * getHallInfo(UINT16 lv);
	//获得祈祷配置
	stPrayInfo * getPrayInfo(UINT16 lv);
	//获得商店配置
	stShopInfo * getShopInfo(UINT16 lv);
	//获得捐赠配置
	stDonateInfo * getDonateInfo(UINT16 id);
	//获得权利配置
	stRightInfo * getRightInfo(UINT8 title);
	//获得黑市配置
	stBlackMarketInfo * getBlackMarketInfo(UINT16 lv);	
	//获取副本开放等级
	UINT16		  getCopyMapOpenLv(UINT32 copymapID);
	//获得副本开启消耗
	UINT32        getStartCopymapCost(UINT32 copymapID);
	//获得伤害排名奖励
	UINT32        getRankReward(UINT16 rank);
	//获得bossID
	UINT32        getBossIDByLevelID(UINT32 levelID);
	//获得积分排行奖励
	UINT32 		  getScoreRankBoxID(UINT32 copyMapID,UINT16 rank);
	//获得副本的完成奖励
	UINT32		  getCompleteBoxID(UINT32 copymapID);
	//获得副本的击杀奖励
	UINT32		  getKillBoxID(UINT32 copymapID);
	//获得挑战次数
	UINT16		  getMaxChallengeNum()
	{
			return m_challengeNum;
	}
	//获得协助次数
	UINT16        getMaxAssistNum()
	{
		return m_assistNum;
	}
	//是否有权限
	bool hasRight(UINT8 title,UINT32 right);
	//获得成员活跃度上限
	UINT32 getMemberActivityLimit()
	{
		return m_memberActivityLimit;
	}
	//获得公会活跃度上限
	UINT32 getGuildActivtyLimit()
	{
		return m_guildActivityLimit;
	}
	//获得战斗阶段时间
	std::map<UINT16,stManorFightTime> & getManorFightTime()
	{
		return m_manorFightTimeInfos; 	
	}
	//获得所有领地信息
	std::map<UINT16,stManorInfo> & getManorInfo()
	{
		return m_manorInfos;
	}
	//获得领地战胜利奖励
	UINT32 getManorWinBoxID()
	{
		return m_manorFightInfo.m_winBoxID;
	}
	//获得失败者的奖励
	UINT32 getManorLoserBoxID()
	{
		return m_manorFightInfo.m_loserBoxID;
	}
	//占领者祈祷boxID
	UINT32 getOwnerPrayBoxID(UINT32 manorID);
private :
	stCreateGuildInfo  m_createInfo;									//创建信息
	std::map<UINT16,stHallInfo> m_hallInfos;							//大厅信息
	std::map<UINT16,stPrayInfo> m_prayInfos;							//祈祷信息
	std::map<UINT16,stShopInfo> m_shopInfos;							//商店信息
	std::map<UINT16,stDonateInfo> m_donateInfos;						//捐赠信息
	std::map<UINT8,stRightInfo> m_rightInfos;							//权限信息
	std::map<UINT32,UINT8>		m_copyMapOpenLv;						//公会本开放等级
	std::map<UINT32,UINT32>		m_openCopyMapCost;						//开启副本消耗
	UINT32						m_memberActivityLimit;					//成功活跃度上限
	UINT32						m_guildActivityLimit;					//公会活跃度上限
	UINT16						m_challengeNum;							//挑战次数
	UINT16						m_assistNum;							//协助次数
	std::map<UINT16,UINT32>		m_damageRankReward;						//<rank,boxID>伤害排名奖励
	std::map<UINT32,std::map<UINT16,UINT32> > m_scoreRankReward;		//积分排名 <copymapID,<rank,boxID>>
	std::map<UINT32,stCopyMapReward>  m_copyMapRewards;					//公会副本奖励
	stManorFightInfo   m_manorFightInfo;								//领地争夺战
	std::map<UINT16,stManorInfo>  m_manorInfos;							//领地信息
	std::map<UINT16,stManorFightTime> m_manorFightTimeInfos;			//战斗时间
	std::map<UINT16, stBlackMarketInfo> m_blackMarketInfos;				//工会黑市信息
};
#endif
