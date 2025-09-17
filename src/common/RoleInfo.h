#ifndef __ROLEINFO_H__
#define __ROLEINFO_H__

#include <cstdlib>
#include "TypesDef.h"
#include "CommonMicro.h"
#include <vector>

enum{
	BITMASK_HAS_LOGON = 1 , 	//是否登录过标志
	BITMASK_FORBID_LOGIN = 2,	//禁止登陆
	BITMASK_GOT_FIRST_RECHARGE_REWARD = 4,//领取了首次奖励
	BITMASK_GOT_10FIRST_RECHARGE_REWARD = 8,//10元首冲奖励
};

#define MAX_ROLE_LEVEL  45

#pragma pack(1)

//网关上面的角色基本数据
struct stGateRoleInfo
{
	UINT32    m_charID;							//角色ID
	char 	  m_account[MAX_ACCNAMESIZE + 1];	//账号
	char      m_name[MAX_NAMESIZE + 1];			//名字
	UINT32    m_serverID;                     	//区ID
	UINT16    m_level;							//等级
	UINT8	  m_job;							//职业
	UINT8     m_sex;							//性别

	stGateRoleInfo()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_serverID = 0;
		m_level = 0;
		m_job = 0;
		m_sex = 0;
	}
};

struct stGateLoadRoleInfo : stGateRoleInfo
{
	UINT16    m_vipLv;                      //vip等级 

	stGateLoadRoleInfo()
	{
		m_vipLv = 0;
	}
};

struct stCreateRoleInfo
{
	UINT32  m_charID;						//角色ID
	char 	m_account[MAX_ACCNAMESIZE + 1];	//账号
	char    m_name[MAX_NAMESIZE + 1];	    //名字
	UINT32  m_serverID;						//区ID
	UINT16  m_level;					    //角色等级
	UINT8	m_job;							//职业
	UINT8   m_sex;							//性别
	char	m_lastLoginTime[20];				//创建时间
	char	m_createTime[20];				//创建时间
	UINT32  m_headID;						//头ID
	UINT32  m_faceID;						//脸ID
	UINT32  m_opID;							//运营商ID

	stCreateRoleInfo()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_serverID = 0;
		m_level = 0;
		m_job = 0;
		m_sex = 0;
		bzero(m_lastLoginTime,sizeof(m_lastLoginTime));
		bzero(m_createTime,sizeof(m_createTime));
		m_headID = 0;
		m_faceID = 0;
		m_opID = 0;
	}
};

enum eGMType
{
	eGMType_Normal = 0,		//普通号
	eGMType_GM = 1,			//GM号
	eGMType_Instructor = 2,	//知道员
	eGMType_Welfare = 3,	//福利号
};

#define ENCHANT_DATA_LEN (1800)
struct RoleInfoBase 
{
	RoleInfoBase()
	{
		bzero(this, sizeof(RoleInfoBase));
	}
	UINT32	  m_charID;						//角色ID
	char 	  m_account[MAX_ACCNAMESIZE + 1];//账号
	char      m_name[MAX_NAMESIZE + 1];     //名字
	UINT16    m_level;                      //角色等级
	UINT8	  m_job;						//职业
	UINT8	  m_sex;						//性别
	UINT32    m_exp;                        //经验
	//UINT32 	  m_hp;							//血量
	//UINT32	  m_mp;							//魔法值
	UINT32    m_goldCoin;                   //金币

	UINT32    m_diamond;                  	//钻石,充值获得的钻石
	UINT32 	  m_giveDiamond;				//非充值获得钻石
	UINT32 	  m_heishijiejing;				//黑市结晶
	UINT32    m_reputaion;					//声望
	UINT32	  m_honour;						//荣誉
	UINT32	  m_braveVal;					//勇敢值
	UINT32	  m_peakCertificate;			//巅峰值
	UINT32	  m_hufu;						//虎符
	UINT32	  m_yuntie;						//陨铁
	UINT32	  m_juexingExp;					//觉醒经验
	UINT32	  m_integral;					//积分

	UINT32	  m_sp;							//体力
	UINT32    m_lilian;						//历练
	UINT16    m_vipLv;						//vip等级	
	UINT32    m_serverID;					//区ID
	UINT16    m_mapID;						//地图ID
	UINT32    m_posX;						//x位置
	UINT32	  m_posY;						//y位置
	UINT32    m_posZ;						//z坐标位置
	UINT32 	  m_leaveGuildTime;				//离开公会时间
	UINT32    m_bitmask;					//比特掩码
	UINT32	  m_rechargeNum;				//充值累计
	UINT32 	  m_giveRechargeNum;			//赠送的充值数量
	UINT64    m_totlaConsume;				//累计消费
	UINT32 	  m_battlePower;				//战力
	UINT16	  m_starNum;					//星星数
	char      m_lastLoginTime[20];			//最近一次登录时间
	char	  m_createTime[20];				//创建时间
	UINT8     m_isAdvUser;					//是否广告用户
	UINT16    m_gm;							//gm类型 0-普通,1-gm,2-指导员,3-福利号
	UINT32 	  m_opID;						//运营商ID
	UINT32    m_contribute;					//公会贡献度
	UINT32    m_headID;						//头ID
	UINT32    m_faceID;						//脸ID
	UINT16	  m_newBieGuideID;				//新手引导ID
	UINT16    m_equipAveRaiseLv;			//装备平均强化等级
	UINT8     m_equipAveRefineLv;			//装备平均精炼等级
	UINT8     m_wingRefineLv;				//翅膀精炼等级
	UINT16    m_fuwenAveRaiseLv;			//符文平均强化等级
	UINT8     m_fuwenAveQuality;			//符文平均品质
	UINT32 	  m_one2OnePVPScore;			//一对一积分
	UINT16    m_subNewBieGuideID;			//子新手引导号
    UINT32    m_sevenDayConsumeDiamond;     //七天活动消耗钻石
};

struct RoleInfo : RoleInfoBase
{
	RoleInfo()
	{
		bzero(this, sizeof(RoleInfo));
	}
    char      m_luckDrawData[60];          //挖宝数据
    char      m_enchantData[ENCHANT_DATA_LEN];  //附魔数据
	char	  m_dailyInviteInfo[60];		//每日分享	--
    UINT32    m_soulFire;                   //魂火
	char	  m_dragonNestInfo[20];		    //巨龙巢穴	--
    UINT32    m_coupleId;                   //夫妇id
	char      m_coupleName[MAX_NAMESIZE + 1];//夫妇名字
	char	  m_guildBlackMarketData[200];	//公会黑市数据
	UINT32	  m_lifeEssence;				//生命精华
	UINT32	  m_guidCrystal;				//公会黑市结晶
	char      m_stoneCopyMapInfo[20];       //宝石副本 --
	char      m_latewalfareInfo[20];        //限时福利 --
	char      m_resBackInfo[1000];          //找回次数 --
	char	  m_monsterInfo[200];			//小怪兽
	char	  m_mountInfo[200];				//坐骑
	char	  m_photoInfo[100];				//头像ID
	UINT16	  m_userPhotoID;				//当前使用的头像ID --
	char	  m_levelWflfareTime[100];		//冲级好礼开放时间
	char	  m_levelWflfare[200];			//冲级好礼
	UINT32	  m_trainSp;					//调教体力
	UINT32	  m_guajiStart;					//挂机开始时间
	UINT32	  m_guajiEnd;					//挂机结束时间
	UINT16	  m_nftVip;						//nftVIP
	UINT32	  m_nftVipGiftTime;					//nftVIP礼物发放时间
	UINT32	  m_guajiTime;					//挂机启动时间
};

struct BinaryStamp
{
	UINT64 m_time;
	UINT32 m_totalSize;
	BinaryStamp()
	{
		m_time = m_totalSize = 0;
	}
};

#define ROLE_BIN_DATA_LEN (1024 * 50)
struct RoleInfoForSave
{
	RoleInfo   m_roleInfo;
	byte       m_binData[ROLE_BIN_DATA_LEN + sizeof(BinaryStamp)];

	RoleInfoForSave()
	{
		bzero(this, sizeof(RoleInfoForSave));
	}

	RoleInfoForSave(RoleInfo& c)
	{
		bcopy(&c, &m_roleInfo, sizeof(RoleInfo));
		bzero(m_binData, sizeof(m_binData));
	}
};
#pragma pack()

struct stRoleState
{
	UINT32 m_nextLvExp;						//下等级需要的经验
	std::vector<UINT32 > m_battleProps;		//战斗属性
	UINT32 m_maxSP;							//当前的最大体力值
	UINT32 m_maxLilian;						//当前最大历练值
	UINT32 m_maxTrainSP;					//当前的最大调教体力值

	stRoleState()
	{
		reset();
	}

	void reset()
	{
		m_nextLvExp = 0;
		m_battleProps.clear();
		m_battleProps.resize(eMAX_BATTLE_PROP);
		m_maxSP = 0;
		m_maxLilian = 0;
		m_maxTrainSP=0;
	}

	void resetBattleProps()
	{ 
		m_battleProps.clear();
		m_battleProps.resize(eMAX_BATTLE_PROP); 
	}
};

#endif
