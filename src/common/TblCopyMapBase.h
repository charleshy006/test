#ifndef __TBL_COPYMAPBASE_H1__
#define __TBL_COPYMAPBASE_H1__
#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

struct CopyMapRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}
	
	UINT32 	m_field_1;						//卡ID
	char    m_field_2[33];                  //名字
	UINT32  m_field_3;                      //关卡类型
	UINT32  m_field_4;                      //隶属章节
	UINT32  m_field_5;						//前置关卡
	UINT32  m_field_6;						//进入等级
	UINT32  m_field_7;						//体力消耗
	UINT32  m_field_8;						//历练消耗
	UINT32  m_field_9;						//关卡掉落组
	UINT32  m_field_10;						//关卡抽奖组
	UINT32  m_field_11;						//是否可以扫荡
	UINT32  m_field_12;						//推荐战斗力
	UINT32  m_field_13;						//1星评价时间
	UINT32  m_field_14;						//2星评价时间
	UINT32  m_field_15;						//3星评价时间
	UINT32  m_field_16;						//boss时间
	char	m_field_17[50];					//怪物1
	char    m_field_18[50];                 //怪物2
	char	m_field_19[50];					//怪物3
	char    m_field_20[50];                 //怪物4
	char	m_field_21[50];					//怪物5
	char    m_field_22[50];                 //怪物6
	char	m_field_23[50];					//boss
	UINT32  m_field_24;						//胜利条件
	UINT32  m_field_25;						//章节掉落ID
	char	m_field_26[50];					//关卡掉落次数
	char	m_field_27[50];					//章节掉落次数
	UINT32  m_field_28;						//经验系数
	UINT32  m_field_29;						//获得金币
	UINT32  m_field_30;						//装备掉落ID
	char	m_field_31[50];					//装备掉落次数	
	char	m_field_32[50];					//怪物7
	char	m_field_33[50];					//怪物8
	char	m_field_34[50];					//怪物9
	char	m_field_35[50];					//怪物10
}__attribute__((packed));

enum eLevelType
{
	eLevelType_Normal = 1,		//普通关卡
	eLevelType_Leader = 2,		//首领关卡
	eLevelType_Elite  = 3,		//精英
	eLevelType_GoldCoin = 4,	//金币
	eLevelType_Survival = 5,	//生存
	eLevelType_COC = 6,			//塔防
	eLevelType_Guild_Copymap = 7,//公会组队副本
	eLevelType_World_Boss = 8,	//世界boss
	eLevelType_PK_With_Friend = 9,//切磋
	eLevelType_AreaPK = 11,		//竞技场
	eLevelType_RealTime = 12,	//实时
	eLevelType_Team		= 13,	//组队
	eLevelType_Peakedness = 14, //巅峰
	eLevelType_Leitai = 15,		//擂台
	eLevelType_Moba_RealTime = 16,//moba3v3
	eLevelType_GuildHall = 17,	//公会大厅
	eLevelType_OpenShow = 18,	//openshow
	eLevelType_ManorFight = 20, //领地战
	eLevelType_EmeraldDream = 21, //翡翠梦境
	eLevelType_Gold = 22,    	//金币f副本
	eLevelType_Long = 23,    //龙晶副本
	eLevelType_Snow = 24,    //积雪霜原
	eLevelType_AreaPKServerCheck = 25,	//竞技场 服务器校验
	eLevelType_LeitaiServerCheck = 26,		//擂台 服务器校验
	eLevelType_InterceptServerCheck = 27,		//劫持战 服务器校验
	eLevelType_League = 28,     //跨服联赛
	eLevelType_Dragon_Nest = 29,//巨龙巢穴
	eLevelType_Stone_CopyMap = 34,	//宝石副本
};

struct stMonsterInfo
{
	UINT32 m_npcID;							//npcID
	UINT16 m_npcLv;							//npc等级

	stMonsterInfo()
	{
		m_npcID = 0;
		m_npcLv = 0;
	}
};

struct stDropNumInfo
{
	UINT16 m_num;			//次数
	UINT32 m_rate;			//概率

	stDropNumInfo()
	{
		m_num = 0;
		m_rate = 0;
	}
};

#define MAX_LEVEL_MONSTER_ID 10

#define INIT_MONSTER_INFO(monsterInfo,field) do { \
		strVec.clear();\
		g_stringtok(strVec,data.field,"-");	\
		if (strVec.size() >= 2){ \
			monsterInfo.m_npcID = (UINT32)atoi(strVec[0].c_str());	\
			monsterInfo.m_npcLv = (UINT16)atoi(strVec[1].c_str());	\
		}\
	} while(false);

struct stCopyMapBase
{
	UINT32 m_id;							//关卡ID
	char   m_name[MAX_NAMESIZE + 1];		//名字
	UINT8  m_type;							//关卡类型
	UINT32 m_chapter;						//章节
	UINT32 m_sceneID;						//场景ID
	UINT32 m_preLvID;						//前置关卡ID
	UINT16 m_limitLv;						//前置等级
	UINT32 m_costSP;						//体力消耗
	UINT32 m_costLiLian;					//历练消耗
	UINT32 m_dropID;						//掉落ID
	UINT32 m_drawCardID;					//抽卡ID
	bool   m_canRush;						//是否可以扫荡
	UINT32 m_recomBattlePower;				//推荐战力
	UINT16 m_1starTime;						//1星时间
	UINT16 m_2starTime;						//2星时间
	UINT16 m_3starTime;						//3星时间
	UINT16 m_bossTime;						//boss倒计时
	stMonsterInfo m_monsterInfo[MAX_LEVEL_MONSTER_ID];		//怪物1的信息
	//stMonsterInfo m_monster2Info;			//怪物2的信息
	//stMonsterInfo m_monster3Info;			//怪物3的信息
	//stMonsterInfo m_monster4Info;			//怪物4的信息
	//stMonsterInfo m_monster5Info;			//怪物5的信息
	//stMonsterInfo m_monster6Info;			//怪物6的信息
	stMonsterInfo m_bossInfo;				//boss信息
	UINT16	m_finishCond;					//完成条件
	UINT32	m_charpterDropID;				//章节掉落ID
	
	std::vector<stDropNumInfo> m_levelDropNumRate;	//关卡掉落次数概率
	std::vector<stDropNumInfo> m_charpterDropNumRate;//章节掉落次数概率
	UINT16 m_expCoef;						//经验系数
	UINT32 m_goldCoin;						//奖励金币
	UINT32 m_equipDrawID;					//装备掉落ID
	std::vector<stDropNumInfo> m_equipDropNumRate;	//装备掉落次数

	void fill(const CopyMapRawData & data)
	{
		m_id = data.m_field_1;
		strncpy(m_name,data.m_field_2,sizeof(m_name));
		m_type = data.m_field_3;
		m_chapter = data.m_field_4;
		//m_sceneID = data.m_field_5;
		m_preLvID = data.m_field_5;
		m_limitLv = data.m_field_6;
		m_costSP = data.m_field_7;
		m_costLiLian = data.m_field_8;
		m_dropID = data.m_field_9;
		m_drawCardID = data.m_field_10;
		if (data.m_field_11){
			m_canRush = true;
		}
		else {
			m_canRush = false;
		}
		m_recomBattlePower = data.m_field_12;
		m_1starTime = data.m_field_13; 
		m_2starTime = data.m_field_14;
		m_3starTime = data.m_field_15;
		m_bossTime =  data.m_field_16;
		
		std::vector<std::string > strVec;
		INIT_MONSTER_INFO(m_monsterInfo[0],m_field_17);
		INIT_MONSTER_INFO(m_monsterInfo[1],m_field_18);
		INIT_MONSTER_INFO(m_monsterInfo[2],m_field_19);
		INIT_MONSTER_INFO(m_monsterInfo[3],m_field_20);
		INIT_MONSTER_INFO(m_monsterInfo[4],m_field_21);
		INIT_MONSTER_INFO(m_monsterInfo[5],m_field_22);
		INIT_MONSTER_INFO(m_bossInfo,m_field_23);
		
		m_charpterDropID = data.m_field_25;
		m_finishCond = data.m_field_24;

		strVec.clear();
		g_stringtok(strVec,data.m_field_26,";");
		for (UINT16 i = 0;i < strVec.size(); ++i){
			stDropNumInfo dropInfo;
			std::vector<std::string> subStr;
			g_stringtok(subStr,strVec[i],"-");
			if (subStr.size() >= 2){
				dropInfo.m_num = (UINT16)atoi(subStr[0].c_str());
				dropInfo.m_rate = (UINT32)atoi(subStr[1].c_str());
			}
			m_levelDropNumRate.push_back(dropInfo);
		}


		strVec.clear();
		g_stringtok(strVec,data.m_field_27,";");
		for (UINT16 i = 0;i < strVec.size(); ++i){
			stDropNumInfo dropInfo;
			std::vector<std::string> subStr;
			g_stringtok(subStr,strVec[i],"-");
			if (subStr.size() >= 2){
				dropInfo.m_num = (UINT16)atoi(subStr[0].c_str());
				dropInfo.m_rate = (UINT32)atoi(subStr[1].c_str());
			}
			m_charpterDropNumRate.push_back(dropInfo);
		}
		
		m_expCoef = data.m_field_28;
		m_goldCoin = data.m_field_29;
		m_equipDrawID = data.m_field_30;
		
		strVec.clear();
		g_stringtok(strVec,data.m_field_31,";");
		for (UINT16 i = 0;i < strVec.size(); ++i){
			stDropNumInfo dropInfo;
			std::vector<std::string> subStr;
			g_stringtok(subStr,strVec[i],"-");
			if (subStr.size() >= 2){
				dropInfo.m_num = (UINT16)atoi(subStr[0].c_str());
				dropInfo.m_rate = (UINT32)atoi(subStr[1].c_str());
			}
			m_equipDropNumRate.push_back(dropInfo);
		}
		
		INIT_MONSTER_INFO(m_monsterInfo[6],m_field_32);
		INIT_MONSTER_INFO(m_monsterInfo[7],m_field_33);
		INIT_MONSTER_INFO(m_monsterInfo[8],m_field_34);
		INIT_MONSTER_INFO(m_monsterInfo[9],m_field_35);
#ifdef _HDZ_DEBUG
		Athena::logger->trace("加载关卡配置表id=%d,name=%s,type=%d,chapter=%d,preLvID=%d,npc1ID=%d,lv1=%d,npc2ID=%d,lv2=%d,npc3ID=%d,lv3=%d,npc4ID=%d,lv4=%d,npc5ID=%d,lv5=%d,npc6ID=%d,lv6=%d,dropNum:%s,charpterDropNum:%s",m_id,m_name,m_type,m_chapter,m_preLvID,m_monsterInfo[0].m_npcID,m_monsterInfo[0].m_npcLv,m_monsterInfo[1].m_npcID,m_monsterInfo[1].m_npcLv,m_monsterInfo[2].m_npcID,m_monsterInfo[2].m_npcLv,m_monsterInfo[3].m_npcID,m_monsterInfo[3].m_npcLv,m_monsterInfo[4].m_npcID,m_monsterInfo[4].m_npcLv,m_monsterInfo[5].m_npcID,m_monsterInfo[5].m_npcLv,data.m_field_26,data.m_field_27);
#endif
	}
};

struct stCharpterRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}
	
	UINT32 	m_field_1;						//章节
	UINT32  m_field_2;					//9星奖励
	UINT32	m_field_3;					//18星奖励
	UINT32	m_field_4;					//27星奖励
}__attribute__((packed));

struct stCharpterReward
{
	UINT32 m_itemID;		//道具ID
	UINT32 m_num;			//道具数量

	stCharpterReward()
	{
		m_itemID = 0;
		m_num = 0;
	}
};

struct stCharpterBase
{
	UINT32 m_id ;
	UINT32 m_9StarReward;		
	UINT32 m_18StarReward;
	UINT32 m_27StarReward;

	void fill(const stCharpterRawData & data)
	{
		m_id = data.m_field_1;
		m_9StarReward = data.m_field_2;
		m_18StarReward = data.m_field_3;
		m_27StarReward = data.m_field_4;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("章节配置id=%d,box1=%d,box2=%d,box3=%d",m_id,m_9StarReward,m_18StarReward,m_27StarReward);
#endif
	}
};


#endif
