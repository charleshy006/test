#ifndef __H_USERMGR_H__
#define __H_USERMGR_H__
#include "Singleton.h"
#include "TypesDef.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "json/reader.h"

#include <map>
#include <set>
#include <ext/hash_map>
#pragma pack(1)
//玩家升级日志
struct stUserLevelupLogDB
{
	UINT32 m_charID;				//角色ID
	char   m_account[49];			//账号
#ifdef _THAILAND_VER
	char   m_name[153];				//名字
#else
	char   m_name[33];
#endif
	UINT16 m_oldLv;					//升级之前玩家等级
	UINT16 m_newLv;					//升级之后玩家等级
	UINT32 m_oldExp;				//升级之前玩家等级
	UINT32 m_newExp;				//升级之后玩家等级
	char   m_levelupTime[20];		//升级时间

	stUserLevelupLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_oldLv = 0;
		m_newLv = 0;
		m_oldExp = 0;
		m_newExp = 0;
		bzero(m_levelupTime,sizeof(m_levelupTime));
	}
};

//玩家登录日志
struct stUserLoginLogDB
{
	UINT32 m_charID ;				//角色ID
	UINT32 m_opID;					//运营ID
	UINT8  m_adUser;				//是否是广告用户
	char   m_account[49];           //账号
#ifdef _THAILAND_VER
	char   m_name[153];              //名字
#else
	char   m_name[33];
#endif
	char   m_loginIP[40];			//登录IP
	char   m_loginTime[20];			//登录时间
	char   m_createTime[20];		//创建时间
	char   m_deviceID[125];

	stUserLoginLogDB()
	{
		m_charID = 0;
		m_opID = 0;
		m_adUser = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		bzero(m_loginIP,sizeof(m_loginIP));
		bzero(m_loginTime,sizeof(m_loginTime));
		bzero(m_createTime,sizeof(m_createTime));
		bzero(m_deviceID,sizeof(m_deviceID));
	}
};

//玩家充值日志
struct stUserRechargeLogDB
{
	UINT32 m_charID;			//角色ID
	char   m_account[49];       //账号
#ifdef _THAILAND_VER
	char   m_name[153];          //名字
#else
	char   m_name[33];
#endif
	char   m_tid[50];			//单号
	UINT32 m_rechargeNum;		//充值数量
	UINT32 m_nowDiamond;		//现在钻石数量
	char   m_rechargeTime[20];	//充值时间
	UINT8  m_newRecharge;		//新充值
	UINT32 m_opID;				//运营商ID
	UINT32 m_addDiamond;		//本次充值增加的钻石
	UINT16 m_viLv;				//vip等级
	UINT16 m_itemID;			//充值购买的道具ID

	stUserRechargeLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		bzero(m_tid,sizeof(m_tid));
		m_rechargeNum = 0;
		m_nowDiamond = 0;
		bzero(m_rechargeTime,sizeof(m_rechargeTime));
		m_rechargeNum = 0;
		m_opID = 0;
		m_addDiamond = 0;
		m_viLv = 0;
		m_itemID = 0;
	}
};

//玩家vip提升日志
struct stUserVipLevelupLogDB
{
	UINT32 m_charID;			//角色ID
	char   m_account[49];       //账号
#ifdef _THAILAND_VER
	char   m_name[153];          //名字
#else
	char   m_name[33];
#endif
	UINT32 m_oldExp;			//旧的经验
	UINT32 m_newExp;			//现在的经验
	UINT32 m_addDiamond;		//增加的钻石
	char   m_levelupTime[20];	//升级时间
	

	stUserVipLevelupLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_oldExp  = 0;
		m_newExp = 0;
		m_addDiamond = 0;
		bzero(m_levelupTime,sizeof(m_levelupTime));
	}
};

//荣誉值变化日志
struct stHonourChgLogDB
{
	UINT32 m_charID;			//角色ID
	char   m_account[49];       //账号
#ifdef _THAILAND_VER
	char   m_name[153];          //名字
#else
	char   m_name[33];
#endif
	UINT32 m_oldHonour;			//变化之前的荣誉
	UINT32 m_newHonour;			//变化之后的荣誉
	char   m_reason[50];		//原因
	char   m_chgTime[20];		//变化的时间

	stHonourChgLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_oldHonour = 0;
		m_newHonour = 0;
		bzero(m_reason,sizeof(m_reason));
		bzero(m_chgTime,sizeof(m_chgTime));
	}
};

//道具变化日志 
struct stItemChgLogDB
{
	UINT32 m_charID;			//角色ID
	char   m_account[49];       //账号
#ifdef _THAILAND_VER
	char   m_name[153];          //名字
	char   m_itemName[153];		//道具名字
#else
	char   m_name[33];          //名字
	char   m_itemName[33];		//道具名字
#endif
	INT32  m_num;				//变化数量
	char   m_reason[50];		//原因
	UINT16 m_userLv;			//玩家等级
	UINT16 m_vipLv;				//vip等级
	UINT8  m_isAdd;				//是否增加
	UINT16 m_type;				//类型
	UINT16 m_quality;			//品质
	UINT16 m_raiseLv;			//强化等级
	UINT16 m_refineLv;			//精炼等级
	char   m_chgTime[20];		//变化时间

	stItemChgLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		bzero(m_itemName,sizeof(m_itemName));
		m_num = 0;
		bzero(m_reason,sizeof(m_reason));
		m_userLv = 0;
		m_vipLv = 0;
		m_isAdd = 0;
		m_type = 0;
		m_quality = 0;
		m_raiseLv = 0;
		m_refineLv = 0;
		bzero(m_chgTime,sizeof(m_chgTime));
	}
};

//资源变化日志
struct stResChgLogDB
{
	UINT32 m_charID;			//角色ID
	char   m_account[49];       //账号
#ifdef _THAILAND_VE
	char   m_name[153];          //名字
	char   m_resName[153];		//资源名字
#else
	char   m_name[33];          //名字
	char   m_resName[33];      //资源名字
#endif
	UINT32 m_addNum;			//增加数量
	UINT32 m_oldValue;			//变化前的值
	UINT32 m_newValue;			//变化后的值
	char   m_reason[50];		//原因
	char   m_chgTime[20];			//变化时间

	stResChgLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		bzero(m_resName,sizeof(m_resName));
		m_addNum = 0;
		m_oldValue = 0;
		m_newValue = 0;
		bzero(m_reason,sizeof(m_reason));
		bzero(m_chgTime,sizeof(m_chgTime));
	}
};

//日常任务日志
struct stDailyTaskLogDB
{
	UINT32 m_charID;			//角色ID
	char   m_account[49];       //账号
#ifdef _THAILAND_VE
	char   m_name[153];          //名字
 	char   m_taskName[153];     //任务名字
#else
	char   m_name[33];          //名字
	char   m_taskName[33];     //任务名字
#endif
	UINT32 m_taskID;			//任务ID
	char    m_doTime[20];			//时间

	stDailyTaskLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_taskID = 0;
		bzero(m_taskName,sizeof(m_taskName));
		bzero(m_doTime,sizeof(m_doTime));
	}
};

//消耗日志
struct stExchangeListDB
{
	UINT32 m_charID;			//角色ID
	char   m_account[49];       //账号
#ifdef _THAILAND_VE
	char   m_name[153];          //名字
	char   m_moduleName[153];   //模块名字
#else
	char   m_name[33];          //名字
	char   m_moduleName[33];   //模块名字
#endif
	UINT32 m_flag;				//标志
	UINT32 m_num;				//数量
	char    m_doTime[20];           //时间

	stExchangeListDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_flag = 0;
		m_num = 0;
		bzero(m_moduleName,sizeof(m_moduleName));
		bzero(m_doTime,sizeof(m_doTime));
	}
};


//商城购买日志
struct stMallBuyLogDB
{
	UINT32 m_charID;					//角色ID
	char   m_account[49];       		//账号
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_level;						//玩家等级
	UINT16 m_vipLv;						//vip等级
	UINT32 m_shopID;					//商店ID
	UINT32 m_resID;						//资源ID
	UINT32 m_resNum;					//资源数量
	UINT32 m_itemID;					//道具ID
	UINT32 m_itemNum;					//道具数量
	char   m_doTime[20];				//时间

	stMallBuyLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_vipLv = 0;
		m_shopID = 0;
		m_resID = 0;
		m_resNum = 0;
		m_itemID = 0;
		m_itemNum = 0;
		bzero(m_doTime,sizeof(m_doTime));
	}
};

//武将属性变化日志
struct stHeroPropChgLogDB
{
	UINT32 m_charID;					//角色ID
	char   m_account[49];       		//账号
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT16 m_level;						//玩家等级
	UINT16 m_vipLv;						//vip等级
	UINT32 m_heroID;					//武将ID
	char   m_action[10];				//行为
	UINT16 m_oldLv;						//变化前等级
	UINT16 m_newLv;						//变化后等级
	UINT16 m_evoOldLv;					//变化前等级
	UINT16 m_evoNewLv;					//变化后等级
	char   m_doTime[20];				//时间

	stHeroPropChgLogDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_vipLv = 0;
		m_heroID  = 0;
		bzero(m_action,sizeof(m_action));
		m_oldLv = 0;
		m_newLv = 0;
		m_evoOldLv = 0;
		m_evoNewLv = 0;
		bzero(m_doTime,sizeof(m_doTime));
	}
};

//进入选择界面日志
struct stEnterSelectPageDB
{
	char m_account[49];				//账号
	char m_firstEnterTime[20];		//首次进入时间
	char m_createRoleTime[20];		//创建角色时间
	char m_lastEnterTime[20];		//最后一次进入时间
	UINT32 m_opID;					//运营商ID
	char m_deviceID[125];			//设备ID

	stEnterSelectPageDB()
	{
		bzero(m_account,sizeof(m_account));
		bzero(m_firstEnterTime,sizeof(m_firstEnterTime));
		bzero(m_createRoleTime,sizeof(m_createRoleTime));
		bzero(m_lastEnterTime,sizeof(m_lastEnterTime));
		m_opID = 0;
		bzero(m_deviceID,sizeof(m_deviceID));
	}
};

//进入选角页面日志
struct stEnterSelectPageLogDB
{
	char m_account[49];				//账号
	char m_firstEnterTime[20];		//首次进入时间
	UINT32 m_opID;					//运营商ID
	char m_deviceID[125];			//设备ID
	char m_doTime[20];				//时间

	stEnterSelectPageLogDB()
	{
		bzero(m_account,sizeof(m_account));
		bzero(m_firstEnterTime,sizeof(m_firstEnterTime));
		m_opID = 0;
		bzero(m_deviceID,sizeof(m_deviceID));
		bzero(m_doTime,sizeof(m_doTime));
	}
};

//公会日志
struct stGuildLogDB
{
	char m_account[49];			//账号
	UINT32 m_opID;				//运行商ID
	UINT32 m_charID;			//角色ID
#ifdef _THAILAND_VER
	char   m_name[153];			//名字
#else
	char   m_name[33];
#endif
	UINT16 m_level;				//等级
	UINT16 m_vipLv;				//vip等级
	UINT32 m_guildID;			//公会ID
	char   m_action[20];		//操作类型
	char   m_opObjID[20];		//操作对象
	char   m_doTime[20];		//操作时间

	stGuildLogDB()
	{
		bzero(m_account,sizeof(m_account));
		m_opID = 0;
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_vipLv = 0;
		m_guildID = 0;
		bzero(m_action,sizeof(m_action));
		bzero(m_opObjID,sizeof(m_opObjID));
		bzero(m_doTime,sizeof(m_doTime));
	}
};

//公会资源日志
struct stGuildResLogDB
{
	UINT32 m_guildID;			//公会ID
	char   m_res[20];			//资源
	UINT32 m_oldVal;			//原来值
	UINT32 m_newVal;			//现在值
	char  m_reason[20];			//原因
	char  m_doTime[20];			//时间

	stGuildResLogDB()
	{
		m_guildID = 0;
		bzero(m_res,sizeof(m_res));
		m_oldVal = 0;
		m_newVal = 0;
		bzero(m_reason,sizeof(m_reason));
	}
};


//模块玩法日志
struct stModuleLogDB
{
	char m_account[49];			//账号
	UINT32 m_opID;				//运行商ID
	UINT32 m_charID;			//角色ID
#ifdef _THAILAND_VER
	char  m_name[153];			//名字
#else
	char  m_name[33];
#endif
	UINT16 m_level;				//等级
	UINT16 m_vipLv;				//vip等级
	char   m_module[20];		//模块
	UINT32 m_copymapID;			//副本ID
	char   m_otherCharID[50];	//其它玩家ID
	UINT8  m_result;			//结果,1胜利,0失败
	UINT32 m_oldRank;			//之前排名
	UINT32 m_newRank;			//新排名
	char   m_doTime[20];		//时间

	stModuleLogDB()
	{
		bzero(m_account,sizeof(m_account));
		m_opID = 0;
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_vipLv = 0;
		bzero(m_module,sizeof(m_module));
		m_copymapID = 0;
		bzero(m_otherCharID,sizeof(m_otherCharID));
		m_result = 0;
		m_oldRank = 0;
		m_newRank = 0;
		bzero(m_doTime,sizeof(m_doTime));
	}
};

//创建角色日志
struct stCreateRoleLogDB
{
	char m_account[49];				//账号
	UINT32 m_opID;					//运营商ID
	char m_deviceID[125];			//设备ID
	char m_doTime[20];				//时间

	stCreateRoleLogDB()
	{
		bzero(m_account,sizeof(m_account));
		m_opID = 0;
		bzero(m_deviceID,sizeof(m_deviceID));
		bzero(m_doTime,sizeof(m_doTime));
	}
};

//技能变化日志
struct stSkillChgLogDB
{
	UINT32 m_skillID;			//技能ID
	UINT8  m_pos;				//位置
	UINT16 m_oldLv;				//原来等级
	UINT16 m_newLv;				//现在等级
	char   m_action[20];		//行为
	char   m_doTime[20];        //时间
	UINT32 m_decGoldcoin;		//扣除金币

	stSkillChgLogDB()
	{
		m_skillID = 0;
		m_pos  = 0;
		m_oldLv = 0;
		m_newLv = 0;
		bzero(m_action,sizeof(m_action));
		bzero(m_doTime,sizeof(m_doTime));
		m_decGoldcoin = 0;
	}
};

//精灵日志
struct stSpriteLogDB
{
	UINT32 m_spriteID;			//精灵ID
	UINT16 m_oldLv;				//原来等级
	UINT16 m_newLv;				//新的等级
	char   m_action[20];		//行为
	char   m_doTime[20];        //时间
	UINT32 m_consume;			//扣除

	stSpriteLogDB()
	{
		m_spriteID = 0;
		m_oldLv = 0;
		m_newLv = 0;
		bzero(m_action,sizeof(m_action));
		bzero(m_doTime,sizeof(m_doTime));
		m_consume = 0;
	}
};

//副本日志
struct stCopymapLogDB
{
	UINT32 m_copymapID;			//地图ID
	UINT8  m_win;				//是否胜利	
	char   m_action[20];		//行为
	char   m_doTime[20];        //时间

	stCopymapLogDB()
	{
		m_copymapID = 0;
		m_win = 0;
		bzero(m_action,sizeof(m_action));
		bzero(m_doTime,sizeof(m_doTime));
	}
};

//装备日志
struct stEquipChgLogDB
{
	UINT32 m_itemID;			//道具ID
	UINT16 m_oldRaiseLv;		//原来的强化等级
	UINT16 m_newRaiseLv;		//新的强化等级
	UINT16 m_oldRefineLv;		//原来的精炼等级
	UINT16 m_newRefineLv;		//新的精炼等级
	char   m_action[20];		//行为
	char   m_doTime[20];        //时间
	UINT32 m_decGoldcoin;		//扣除的金币
	UINT32 m_decYuntie;			//扣除的陨铁
	UINT32 m_decItemID;			//扣除的道具
	UINT32 m_addGoldcoin;		//增加的金币
	UINT32 m_addYuntie;			//增加的陨铁
	UINT32 m_addItemID;			//增加的道具

	stEquipChgLogDB()
	{
		m_itemID = 0;
		m_oldRaiseLv = 0;
		m_newRaiseLv = 0;
		m_oldRefineLv = 0;
		m_newRefineLv = 0;
		bzero(m_action,sizeof(m_action));
		bzero(m_doTime,sizeof(m_doTime));
		m_decGoldcoin = 0;
		m_decYuntie = 0;
		m_decItemID = 0;
		m_addGoldcoin = 0;
		m_addYuntie = 0;
		m_addItemID = 0;
	}
};

//活动日志
struct stActivityLogDB
{
	UINT32 m_charID;				//角色ID
	UINT32 m_activityID;			//活动ID
	char   m_name[50];				//活动名字
	UINT32 m_stepID;				//阶段ID
	char   m_desc[50];				//活动阶段
	char   m_doTime[20];        	//时间

	stActivityLogDB()
	{
		m_charID = 0;
		m_activityID = 0;
		bzero(m_name,sizeof(m_name));
		m_stepID  = 0;
		bzero(m_desc,sizeof(m_desc));
		bzero(m_doTime,sizeof(m_doTime));
	}
};

#pragma pack()

struct stLogDesc
{
	UINT16 m_id;				//id
	char   m_desc[50];			//描述	

	stLogDesc()
	{
		m_id = 0;
		bzero(m_desc,sizeof(m_desc));
	}
};

//进入选择界面日志
struct stEnterSelectPage
{
	char m_firstEnterTime[20];		//首次进入时间
	char m_createRoleTime[20];		//创建角色时间

	stEnterSelectPage()
	{
		bzero(m_firstEnterTime,sizeof(m_firstEnterTime));
		bzero(m_createRoleTime,sizeof(m_createRoleTime));
	}
};

class UserLogMgr
{
	USE_SINGLETON_NEW(UserLogMgr);
private :
	UserLogMgr();
public :
	//析构函数
	~UserLogMgr() {}
	//获得排名管理实例
	static UserLogMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//增加升级日志
	void addUserLevelupLog(stUserLevelupLogDB & data);
	//增加登录日志
	void addLoginLog(stUserLoginLogDB & data);
	//增加充值日志
	void addRechargeLog(stUserRechargeLogDB & data);
	//增加vip升级日志
	void addVipLevelupLog(stUserVipLevelupLogDB & data);
	//增加荣誉变化日志
	void addHonourChgLog(stHonourChgLogDB & data);
	//增加道具变化日志
	void addItemChgLog(stItemChgLogDB & data);
	//增加资源变化日志
	void addResChgLog(stResChgLogDB & data);
	//增加日常任务日志
	void addDailyTaskLog(stDailyTaskLogDB & data);
	//增加消耗日志
	void addExchangeLog(stExchangeListDB & data);
	//增加商店购买日志
	void addMallBuyLog(stMallBuyLogDB & data);
	//增加武将属性变化日志
	void addHeroPropChgLog(stHeroPropChgLogDB & data);
	//增加进入选择界面日志
	void addEnterSelectPageLog(stEnterSelectPageLogDB & data);
	//添加公会日志
	void addGuildLog(stGuildLogDB & data);
	//添加公会资源日志
	void addGuildResLog(stGuildResLogDB  & data);
	//添加模块日志
	void addModuleLog(stModuleLogDB & data);
	//增加创建角色日志
	void addCreateRoleLog(stCreateRoleLogDB & data);
	//增加技能变化日志
	void addSkillChgLog(stSkillChgLogDB & data);
	//增加精灵日志
	void addSpriteLog(stSpriteLogDB & data);
	//增加副本日志
	void addCopymapLog(stCopymapLogDB & data);
	//增加装备日志
	void addEquipChgLog(stEquipChgLogDB & data);
	//增加了活动日主
	void addActivityLog(stActivityLogDB & data);
	//获得某个玩家的某个类型的日志
	std::string getUserLogByID(Json::Value & searchCond);
	//构造Union表
	std::string getUnionTblName(std::string tblName);
	//根据时间获得表名字
	std::string getTblNameByDate(std::string tblName,Json::Value & searchCond);
	//获得查询条件
	std::string getSearchCond(Json::Value & searchCond);	
	//获得升级日志
	std::string getUserLevelupLog(Json::Value & searchCond);
	//获得登录日志
	std::string getUserLoginLog(Json::Value & searchCond);
	//获得充值日志
	std::string getRechargeLog(Json::Value & searchCond);
	//获得vip充值日志
	std::string getVipLevelupLog(Json::Value & searchCond);
	//获得荣誉变化ID
	std::string  getHonourChgLog(Json::Value & searchCond);
	//获得道具变化ID
	std::string getItemChgLog(Json::Value & searchCond);
	//获得资源变化ID
	std::string getResChgLog(Json::Value & searchCond);
	//获得日常任务日志
	std::string getDailyTaskLog(Json::Value & searchCond);
	//宠物属性日志 
	std::string getHeroPropChgLog(Json::Value & searchCond);
	//进入日志
	std::string enterSelectLog(Json::Value & searchCond);
	//公会日志
	std::string guildLog(Json::Value & searchCond);
	//公会资源日志
	std::string guildResLog(Json::Value & searchCond);
	//模块日志
	std::string moduleLog(Json::Value & searchCond);
	//商店购买
	std::string mallBuyLog(Json::Value & searchCond);
	//获得技能变化日志
	std::string skillChgLog(Json::Value & searchCond);
	//获得精灵日志
	std::string spriteLog(Json::Value & searchCond);
	//副本日志
	std::string copymapLog(Json::Value & searchCond);
	//装备日志
	std::string equipChgLog(Json::Value & searchCond);
	//测试
	void test();
	//获得日志类型
	std::string getUserLogLists();
	//获得某个条件下的记录数
	UINT32 getRecordCount(const char * szTableName,const char * szWhere);
	//获得今天的表名字
	std::string  getTodayTblName(const char * szTblName);
	//检查表是否存在，不存在就创建
	std::string getRealTblName(const char * szTblName);
	//初始化日志表
	void init();
	//更新创角色时间
	void updateCreateRoleTime(const std::string & account);
	//加载enterSelectPage数据
	void loadEnterSelectPage();
	//删除所有日志表
	void dropAllLogTbl();
	//获得ExchangeList接口的表
	std::string getExchangeListTable(std::string & start,std::string & end);
	//某个表的某天的分表是否存在
	bool isTblExist(std::string tblName,std::string dateTblName);
private :
	stLogDesc  m_maxLogDesc[eMax_Log_Type];
	std::map<std::string,std::set<std::string> > m_logTbls;			//日志表
	std::map<std::string,std::string>  m_createSqls;				//创建sql脚本

	typedef __gnu_cxx::hash_map<std::string,stEnterSelectPage, my_hash<std::string> >   EnterSelectPageCache;
	typedef EnterSelectPageCache::iterator EnterSelectPageCache_IT;

	EnterSelectPageCache m_enterSelectPageCache;
};
#endif
