#ifndef __H_GUILD_H__
#define __H_GUILD_H__
#include "TypesDef.h"
#include "ElementMgr.h"
#include "TblQueryResult.h"
#include "SortList.h"
#include "GuildProto.pb.h"
#include "Player.h"
#include "PlayerManager.h"
#include "Mail.h"
#include "TemplateDB.h"
#include <ext/hash_map>
#include <list>

#pragma pack(1)

struct stGuildMemberResetDB
{
	UINT16 m_challengeNum;					//挑战次数
	UINT16 m_assitNum;						//协助次数
	UINT32 m_damage1;						//伤害1
	UINT32 m_damage2;						//伤害2
	UINT32 m_damage3;						//伤害3
	
	stGuildMemberResetDB()
	{
		m_challengeNum = 0;
		m_assitNum = 0;
		m_damage1 = 0;
		m_damage2 = 0;
		m_damage3 = 0;
	}
};

struct stGuildMemberDB
{
	UINT32 m_charID;						//角色ID
	char   m_name[MAX_NAMESIZE + 1];		//角色名字 
	UINT8  m_job;							//职业
	UINT8  m_sex;							//性别
	UINT8  m_title;							//头衔
	UINT32 m_contribute;					//总贡献度
	UINT32 m_guildID;						//公会ID
	UINT32 m_joinInTime;					//加入时间
	UINT32 m_lastLoginTime;					//最近一次登录时间
	UINT32 m_activityDegree;				//活跃度
	UINT16 m_challengeNum;					//挑战次数
	UINT16 m_assitNum;						//协助次数
	UINT32 m_damage1;						//伤害1
	UINT32 m_damage2;						//伤害2
	UINT32 m_damage3;						//伤害3
	UINT16 m_photoID;						//头像ID

	stGuildMemberDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_job = 0;
		m_sex = 0;
		m_title = 0;
		m_contribute = 0;
		m_guildID = 0;
		m_joinInTime = 0;
		m_lastLoginTime = 0;
		m_activityDegree = 0;
		m_challengeNum = 0;
		m_assitNum = 0;
		m_damage1 = 0;
		m_damage2 = 0;
		m_damage3 = 0;
		m_photoID = 0;
	}
};

#define  GUILD_SIGN_LEN 150
#define  GUILD_NOTICE_LEN 150

struct stGuildDB
{
	UINT32 m_id;						//公会ID
	char   m_name[MAX_NAMESIZE + 1];	//公会名字
	UINT32 m_leaderID;					//公会角色ID
	UINT32 m_contribute;				//贡献值 
	UINT8  m_hallLv;				//公会大营等级
	UINT8  m_prayBuildingLv;				//兵粮大营等级	
	UINT8  m_shopLv;				//特训营等级	
	UINT32 m_fireTime;					//解散时间
	char   m_sign[GUILD_SIGN_LEN];		//签名
	char   m_notice[GUILD_NOTICE_LEN];	//公告
	UINT16 m_badge;						//徽章
	UINT32 m_activityDegree;			//活跃度
	UINT8 m_blackMarketLv;				//黑市等级

	stGuildDB()
	{
		m_id = 0;
		bzero(m_name,sizeof(m_name));
		m_leaderID = 0;
		m_contribute = 0;
		m_hallLv = 0;
		m_prayBuildingLv = 0;
		m_shopLv = 0;
		m_fireTime = 0;
		bzero(m_sign,sizeof(m_sign));
		bzero(m_notice,sizeof(m_notice));
		m_badge = 0;
		m_activityDegree = 0;
		m_blackMarketLv = 0;
	}
};

struct stGuildCopyMapDB
{
	UINT32 m_copyMapID;			//副本ID
	UINT32 m_openTime;			//开启时间
	UINT32 m_bossHP;			//boss血量
	UINT16 m_stepID;			//阶段ID
	UINT32 m_guildID;			//公会ID
	UINT32 m_killCharID;		//击杀角色ID

	stGuildCopyMapDB()
	{
		m_copyMapID = 0;
		m_openTime = 0;
		m_bossHP = 0;
		m_stepID = 0;
		m_guildID = 0;
		m_killCharID = 0;
	}
};

//公会积分排名
struct stGuildCopyMapRankDB
{
	UINT32 m_copyMapID;
	UINT32 m_guildID;
	UINT32 m_score;

	stGuildCopyMapRankDB()
	{
		m_copyMapID = 0;
		m_guildID = 0;
		m_score = 0;
	}
};

#pragma pack()


class CGuildMember : public Element
{
	friend class CGuild;
	friend class CGuildMgr;
public :
	static const  UINT32 s_SP_2_ACTIVITY_RATE = 1;
	static const  UINT32 s_LILIAN_2_ACTIVITY_RATE = 5;
public :
	//构造函数
	CGuildMember();
	//析构函数
	~CGuildMember();
	//从数据库数据初始化
	void initFromDB(const stGuildMemberDB & data);
	//填充数据库数据
	void fillDBdata(stGuildMemberDB & data);
	//填充同步数据
	void fillSynData(GuildProto::stSynGuildMemInfo & data);
	//获得贡献度
	UINT32 getContribute() const
	{
		return m_contribute;
	}
	//增加贡献度
	bool synContribute(UINT32 num);
	//发送详细信息
	void sendDetailInfo(Player * pUser);
	//零点重置
	void reset()
	{
		m_todayContribute = 0;
		m_lastAddCbTime  = 0;
		m_activityDegree = 0;
		m_challengeNum = 0;
		m_assitNum = 0;
		m_damage1 = 0;
		m_damage2 = 0;
		m_damage3 = 0;
	}
	//增加贡献度
	void addTodayContribute(UINT32 num)
	{
		m_todayContribute += num;
	}
	//设置伤害
	void setDamage1(UINT32 damage1)
	{
		m_damage1 = damage1;
	}
	//返回伤害
	UINT32 getDamage1()
	{
		return m_damage1;
	}
	//设置伤害
	void setDamage2(UINT32 damage2)
	{
		m_damage2 = damage2;
	}
	//返回伤害
	UINT32 getDamage2()
	{
		return m_damage2;
	}
	//设置伤害
	void setDamage3(UINT32 damage3)
	{
		m_damage3 = damage3;
	}
	//返回伤害
	UINT32 getDamage3()
	{
		return m_damage3;
	}
	//获得挑战次数
	UINT16 getChallengeNum()
	{
		return m_challengeNum;
	}
	//获得协战次数
	UINT16 getAssistNum()
	{
		return m_assitNum;
	}
	//扣除挑战次数
	bool addChallengeNum(UINT16 num);
	//扣除协战次数
	bool addAssistNum(UINT16 num);
	//扣除体力
	void onDecSp(UINT16 sp);
	//扣除的历练
	void onDecLilian(UINT16 lilian);
	//获得头衔
	UINT8 getTitle() const
	{
		return m_title;
	}
	//获得最近一次登录时间
	UINT32 getLastLoginTime()
	{
		return m_lastLoginTime;
	}
	//获得入时间
	UINT32 getJoinInTime()
	{
		return m_joinInTime;	
	}
	//设置头像ID
	void setPhotoID(UINT16 id)
	{
		m_photoID = id;
	}
private :
	UINT8  m_job;							//职业
	UINT8  m_sex;
	UINT8  m_title;							//头衔
	UINT32 m_contribute;					//贡献度
	UINT32 m_guildID;						//所在公会
	UINT32 m_joinInTime;					//加入时间
	UINT32 m_lastLoginTime;					//最近一次登录时间
	UINT32 m_lastAddCbTime;					//上次增加贡献度时间
	UINT32 m_todayContribute;				//今天贡献度
	UINT32 m_activityDegree;				//活跃度
	UINT16 m_challengeNum;					//挑战次数
	UINT16 m_assitNum;						//协助次数
	UINT32 m_damage1;						//伤害1
	UINT32 m_damage2;						//伤害2
	UINT32 m_damage3;						//伤害3
	UINT16 m_decLilian;						//扣除的体力
	UINT16 m_photoID;						//头像ID
};

struct stApplyJoinInInfo
{
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字 
	UINT16 m_level;						//等级
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT32 m_applyTime;					//请求时间
	UINT16 m_photoID;					//头像ID

	stApplyJoinInInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_job = 0;
		m_sex = 0;
		m_applyTime = 0;
		m_photoID = 0;
	}
};



struct stTraceInfo
{
	UINT32 m_id;
	UINT32 m_charID;
	GuildProto::eTraceID m_eID;
	std::string m_charName;					
	UINT32 m_doTime;					

	stTraceInfo()
	{
		m_id = 0;
		m_charID = 0;
		m_eID = GuildProto::eTrace_None;	
		m_doTime = 0;
	}
};

#pragma pack(1)

struct stTraceInfoDB
{
	UINT32 m_id;							//ID
	UINT32 m_traceID;						//跟踪类型
	UINT32 m_charID;						//角色ID
	char   m_charName[MAX_NAMESIZE + 1];	//角色名字
	UINT32 m_doTime;						//改行为的时间
	UINT32 m_guildID;						//公会ID

	stTraceInfoDB()
	{
		m_id = 0;
		m_traceID = 0;
		m_charID = 0;
		bzero(m_charName,sizeof(m_charName));
		m_doTime = 0;
		m_guildID = 0;
	}
};

#pragma pack()

struct stMemberDamageInfo
{
	UINT32 m_charID;			//角色ID
	UINT32 m_damage;			//伤害

	bool operator < (const stMemberDamageInfo & one) const 
	{ 
		return m_damage > one.m_damage;
	}   

	UINT32 getKey() const 
	{   
		return m_charID;
	}   

	UINT32 getCmpValue() const 
	{   
		return m_damage;
	}   
};

struct stGuildCopyMapInfo
{
	UINT32 m_copyMapID;			//副本ID
	UINT32 m_openTime;			//开启时间
	UINT32 m_bossHP;			//boss血量
	UINT16 m_stepID;			//阶段ID
	UINT32 m_guildID;			//公会ID
	UINT32 m_killCharID;		//击杀角色ID

	stGuildCopyMapInfo()
	{
		m_copyMapID = 0;
		m_openTime = 0;
		m_bossHP = 0;
		m_stepID = 0;
		m_guildID = 0;
		m_killCharID = 0;
	}
	//从数据库初始化
	void initFromDB(const stGuildCopyMapDB & data);
	//填充数据库信息
	void fillDBInfo(stGuildCopyMapDB & data);
};

typedef CSortList<stMemberDamageInfo>  TMemberDamageSortList;

class CGuild : public Element,public ElementMgr
{
	friend class CGuildMgr;
public :
	static const UINT16 s_MAX_APPLY_NUM = 20;		//申请者列表长度
	static const UINT16 s_MAX_TRACE_LEN = 100;		//跟踪记录的条数
public :
	//构造函数
	CGuild();
	//析构函数
	~CGuild();
	//从数据库数据初始化
	void initFromDB(const stGuildDB & data);
	//填充数据库数据
	void fillDBdata(stGuildDB & data);
	//加载所有成员
	void loadMember();
	//添加成员
	bool addMember(CGuildMember * pMember);
	//获得某个成员 
	CGuildMember * getGuildMemberByID(UINT32 id) {  return (CGuildMember *)ElementMgr::getElementByID(id) ;}
	//更新某个成员信息
	void updateMemberInfo(Player * pUser);
	//删除成员
	bool removeMember(CGuildMember * pMember);
	//填充列表数据
	void fillGuildListInfo(GuildProto::stGuildListInfo & data);
	//填充公会相信信息
	void fillDetailInfo(GuildProto::stRetGuildDetailInfo * data);
	//发送公会信息给玩家 
	void sendGuildInfo(Player * pUser);
	//添加申请者 
	void addApplier(Player * pUser);
	//是否申请过
	bool isInApplyList(UINT32 charID);
	//从申请列表中删除
	void delFromApplyList(UINT32 charID);
	//给玩家发送申请列表
	void sendApplyList(Player * pUser);
	//答复申请者
	void replyApplier(Player * pUser,UINT32 otherID,bool agree);	
	//返回公会可以容纳最大人数
	UINT16 getMaxMember();
	//给所有人发送信息
	template <class T>
	void sendProto2Mem(const  T & cmd,UINT32 exceptID = 0);
	//离开公会
	void leaveGuild(Player * pUser);
	//晋职
	void assignTitle(Player * pLeader,UINT32 charID,GuildProto::eGuildTile eTitle);
	//取消职位
	void cancelTitle(Player * pLeader,UINT32 charID);
	//转让
	void giveLeader(Player * pLeader,UINT32 charID);
	//是否是团长
	bool isLeader(UINT32 charID) { return charID == m_leaderID ;}
	//踢人
	void killOutMember(Player * pLeader,UINT32 charID);
	//增加贡献度
	void addContribute(UINT32 num,const char * desc);
	//扣除贡献度
	bool removeContribute(UINT32 num,const char * desc);
	//同步贡献度 
	void synContribute();
	//获得贡献度
	UINT32 getContribute() { return m_contribute ; }
	//检查贡献度
	bool checkContribute(UINT32 num ) { return num <= m_contribute; }
	//修改签名
	void chgSign(Player * pUser,const char * sign);
	//修改公告
	void chgNotice(Player * pUsesr,const char * notice);
	//设置离开公会时间
	void setAllMemberLeaveTime();
	//玩家等级
	void userLevelup(UINT32 charID,UINT16 level);
	//定时回调
	void timer(UINT32 cur);
	//解散时间
	void setFireTime(UINT32 fireTime)
	{
		m_fireTime = fireTime;
	}
	//升级大厅
	void levelupHall(Player * pUser);
	//升级祈福殿
	void levelupPrayBuilding(Player * pUser);
	//升级商店
	void levelupShop(Player * pUser);
	//升级黑市
	void levelupBlackMarket(Player * pUser);
	//增加一条跟踪记录
	void addTrace(GuildProto::eTraceID eID,UINT32 charID,std::string charName);
	//发送所有跟踪记录
	void sendTraceList(Player * pUser);
	//公会祈福
	void pray(Player * pUser);	
	//更换徽章
	void changeBadge(Player * pUser,UINT16 badgeID);
	//获得公会等级
	UINT8 getLevel()
	{
		return m_hallLv;
	}
	//是否满了
	bool isFull()
	{
		return size() >= getMaxMember();
	}
	//更新某个成员的伤害
	void updateMemberDamage(UINT32 charID,UINT32 damage);
	//增加副本活跃度
	void addActivityDegree(UINT32 degree,const char * desc);
	//删除副本活跃度
	bool removeActivityDegree(UINT32 degree,const char * desc);
	//返回活跃度
	UINT32 getActivityDegree()
	{
		return m_activityDegree;	
	}
	//获得真实的copyMapID
	static UINT32 getRealCopyMapID(UINT32 copymapID)
	{
		return (copymapID / 100) * 100;	
	}
	//给某个玩家发送
	void sendDamageSortlist(Player * pUser,UINT16 beginRank);
	//发送界面
	void sendCopymapWin(Player * pUser);
	//开启副本
	void startCopyMap(Player * pUser,UINT32 copymapID);
	//进入公会副本
	void enterCopyMap(Player * pUser,UINT32 copymapID);
	//更新公会boss血量
	void updateGuildBossHP(UINT32 copymapID,UINT32 curHP);
	//更新公会boss的阶段
	void killGuildBoss(UINT32 copymapID,UINT32 killerID);
	//获得公会boss的阶段
	UINT16 getGuildBossStepID(UINT32 copymapID);
	//获得boss血量
	UINT32 getGuildBossHP(UINT32 copymapID);
	//收集伤害奖励玩家
	void collectDamageRewardChar(std::map<UINT16,stBatchSendMailTask> & sendTask);
	//收集玩家的数量
	void addMember2BatchSendTask(stBatchSendMailTask & task);
	//发送公会日子
	void sendGuildLog(Player * pUser,const char * szAction,const char * szOpObj);
	//检查某个成员的权限
	bool checkMemberRight(UINT32 memberID,UINT32 right);
	//获得所有成员ID
	void getAllMember(std::vector<UINT32> & charIDVec);
	//会长自动转让
	void transferGuild();
	//获得黑市等级
	UINT8 getBlackMarketLv()
	{
		return m_blackMarketLv;
	}
	//改名同步给所有人
	void synAllMenber();
private :
	UINT32 m_exp;								//公会经验
	UINT32 m_leaderID;							//团长角色ID
	UINT32 m_contribute;						//贡献值 
	char   m_sign[GUILD_SIGN_LEN];				//签名
	char   m_notice[GUILD_NOTICE_LEN];			//公告
	std::list<stApplyJoinInInfo> m_applyList;	//申请列表
	UINT8  m_hallLv;							//大厅等级
	UINT8  m_prayBuildingLv;					//祈福大厅等级	
	UINT8  m_shopLv;							//商店等级	
	UINT32 m_fireTime;							//解散时间
	UINT16 m_payNum;							//已经祈福的次数
	bool   m_refresh;
	std::list<stTraceInfo> m_traces;			//跟踪列表
	UINT16 m_badge;								//徽章
	UINT32 m_activityDegree;                	//活跃度
	TMemberDamageSortList	m_damageSortList;	//伤害排名
	std::map<UINT32,stGuildCopyMapInfo>  m_guildCopyMapInfos;//公会副本信息
	bool m_zeroRefresh;
	UINT8 m_blackMarketLv;						//黑市等级
};

struct stGuildRankInfo
{
	UINT32 m_guildID;			//公会ID
	UINT32 m_level;				//经验

	bool operator < (const stGuildRankInfo & one) const 
	{ 
		return m_level > one.m_level;
	}   

	UINT32 getKey() const 
	{   
		return m_guildID;
	}   

	UINT32 getCmpValue() const 
	{   
		return m_level;
	}   
};

struct stGuildCopymapScoreInfo
{
	UINT32 m_guildID;			//公会ID
	UINT32 m_score;				//积分

	bool operator < (const stGuildCopymapScoreInfo & one) const 
	{ 
		return m_score > one.m_score;
	}   

	UINT32 getKey() const 
	{   
		return m_guildID;
	}   

	UINT32 getCmpValue() const 
	{   
		return m_score;
	}   
};


typedef CSortList<stGuildRankInfo>  GuildSortList;
typedef CSortList<stGuildCopymapScoreInfo> GuildCopymapScoreList;

class CGuildMgr : public ElementMgr
{
	USE_SINGLETON_NEW(CGuildMgr);
public :
	static const UINT32 s_FIRE_GUILD_LASTTIME = 60;
public :
	//构造函数
	CGuildMgr();
	//析构函数
	~CGuildMgr();
	//获得实例
	static CGuildMgr & getSingleton()
	{
		return THE_SINGLETON::Instance(); 
	}
	//加载所有公会
	void loadAllGuild();
	//公会数量
	UINT32 guildNum();
	//增加一个公会
	bool addGuild(CGuild * pGroup);
	//删除一个公会 
	bool removeGuild(CGuild * pGroup);
	//获得公会
	CGuild * getGuildByID(UINT32 guildID);
	//根据名字获得公会
	CGuild * getGuildByName(char * guildName);
	//根据角色ID获得公会 
	CGuild * getGuildByCharID(UINT32 charID);
	//增加一个角色id到公会ID映射
	bool addCharID2GuildID(UINT32 charID,UINT32 guildID);
	//删除一个角色到公会映射
	bool removeCharID2GuildID(UINT32 charID);
	//玩家上线
	void userOnline(Player * pUser);
	//玩家上线 
	void userOffline(Player * pUser);
	//创建回调
	void createGuildCB(bool opResult,UINT64 autoID,stGuildDB data);
	//创建一个公会成员
	void createGuildMem(const stGuildMemberDB & data);
	//创建一个公会成员回调
	void createGuildMemCB(bool opResult,UINT64 autoID);
	//删除一个成员
	void deleteGuildMem(UINT32 charID);
	//删除一个成员回调
	void deleteGuildMemCB(bool opResult,UINT64 autoID);
	//更新一个成员数据
	void updateGuildMem(const stGuildMemberDB & data);
	//更新成员回调
	void updateGuildMemCB(bool opResult,UINT64 autoID);
	//删除公会
	void deleteGuild(UINT32 guildID);
	//删除公会回调
	void deleteGuildCB(bool opResult,UINT64 autoID);
	//更新公会信息
	void updateGuild(const stGuildDB & data);
	//更新公会信息回调
	void updateGuildCB(bool opResult,UINT64 autoID);
	//解散公会
	void fireGuild(UINT32 guildID);
	//定时器
	void timer(UINT32 cur);
	//发送公会列表
	void sendGuildList(Player * pUser,UINT16 rank);
	//请求公会的详细信息
	void reqGuildDetail(Player * pUser,UINT32 guildID);
	//增加一条跟踪记录
	void addTraceInfoRecord(stTraceInfoDB & dbData);
	//增加一条跟踪记录回调
	void addTraceInfoRecordCB(bool opResult,UINT64 autoID);
	//删除一条跟踪记录
	void delTraceInfo(UINT32 id);
	//删除跟踪记录回调
	void delTraceInfoCB(bool opResult,UINT64 autoID);
	//获得公会排名
	UINT16 getGuildRank(UINT32 guildID);
	//公会改变
	void   guildLevelChg(UINT32 guildID,UINT8 lv);
	//搜索公会
	void   searchGuild(Player * pUser,const char * pGuilName);
	//搜索回调
	void  searchGuildCB(CTblQueryResult* result,UINT32 charID);
	//更新公会副本积分
	void  updateCopymapScore(UINT32 guildID,UINT32 copymapID,UINT32 score);
	//发送积分排名
	void sendCopymapSortList(Player * pUser,UINT32 copymapID,UINT16 beginRank);
	//重置
	void resetMemberDBCB(bool opResult,UINT64 autoID);
	//增加积分排名记录
	void addScoreRankRecord(stGuildCopyMapRankDB & dbData);
	//增加积分排名记录回调
	void addScoreRankRecordCB(bool opResult,UINT64 autoID);
	//更新积分排名记录
	void updateScoreRankRecord(stGuildCopyMapRankDB & dbData);
	//更新积分排名记录
	void updateScoreRankRecordCB(bool opResult,UINT64 autoID);
	//删除积分排行表
	void deleteScoreRank();
	//删除积分排行榜回调
	void deleteScoreRankCB(bool opResult,UINT64 autoID);
	//获得公会最IID
	UINT32 getCurMaxGuildID()
	{
		return m_curMaxGuildID;
	}
	//增加公会最大ID
	void incCurMaxGuildID()
	{
		++m_curMaxGuildID;	
	}
private :
	__gnu_cxx::hash_map<UINT32,UINT32> m_charID2GuildID;
	typedef __gnu_cxx::hash_map<UINT32,UINT32>::iterator  CharID2GuildID_IT;
	GuildSortList  m_guildSortList;
	std::map<UINT32,GuildCopymapScoreList> m_copymapScoreList;
	UINT32 m_curMaxGuildID;
};

template <class T>
void CGuild::sendProto2Mem(const  T & cmd,UINT32 exceptID)
{
	TIDMap_IT it = m_idMap.begin();
	for (;it != m_idMap.end();++it){
		CGuildMember * pGuildMem = (CGuildMember *)it->second;
		if (pGuildMem){
			Player * pUser = PlayerManager::getSingleton().getUserByID(pGuildMem->getID());
			if (pUser && (pUser->getID() != exceptID)){
				pUser->sendProtoToMe(cmd);
			}
		}
	}
}


class CGuildCopyMapMgr
{
	USE_SINGLETON_NEW(CGuildCopyMapMgr);
private :
	//构造函数
	CGuildCopyMapMgr()
	{}
public :
	//析构函数
	~CGuildCopyMapMgr()
	{
	}
	//返回实例本身
	static CGuildCopyMapMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();		
	}
	//增加公会组队副本记录
	void addRecord(stGuildCopyMapDB & data);
	//增加公会组队副本记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新公会组队副本信息
	void updateRecord(stGuildCopyMapDB & data);
	//更新公会组队副本信息回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//清空公会组队副本信息
	void removeRecord();
	//清空公会组队副本信息回调
	void removeRecordCB(bool opResult,UINT64 autoID);
};


#pragma pack(1)
struct stManorStateDB
{
	UINT32 m_id;
	UINT16 m_state;
	UINT8  m_bGiveReward;
	
	stManorStateDB()
	{
		m_id = 0;
		m_state = 0;
		m_bGiveReward = 0;
	}
};
#pragma pack()

struct stManorState
{
	UINT32 m_id;
	UINT16 m_state;
	UINT8  m_bGiveReward;

	stManorState()
	{
		m_id = 0;
		m_state = 0;
		m_bGiveReward = 0;
	}

	void initFromDB(const stManorStateDB & data);
	void fillDBData(stManorStateDB & data);
	void print();
};

class CManorStateMgr : public  CTemplateDB<stManorStateDB,stManorState>
{
	USE_SINGLETON_NEW(CManorStateMgr);
public :
	enum {
		eState_None = 0,
		eState_Apply = 1, //申请状态
		eState_ApplyOver = 2,	//申请结束
		eState_Ready = 3, //准备
		eState_Break1 = 4,//调整
		eState_FIGHT1 = 5,//战斗
		eState_Break2 = 6,//调整
		eState_FIGHT2 = 7,//战斗
		eState_Break3 = 8,//调整
		eState_FIGHT3 = 9,//战斗
		eState_Break4 = 10,//调整
		eState_FIGHT4 = 11,//战斗
		eState_Break5 = 12,//调整
		eState_FIGHT5 = 13,//战斗
		eState_GiveReward = 14,//给奖励
		eState_MAX = 15,
	};

	typedef void (CManorStateMgr::*pStateMemFun)();  
public :
	//构造函数
	CManorStateMgr();
	//析构函数
	~CManorStateMgr();
	//获得实例
	static CManorStateMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//给奖励
	void giveRewardCB();
	//定时器
	void timer(UINT32 cur);
	//获得状态
	UINT8 getState()
	{
		return m_manorState.m_state;
	}
	//通知状态改变
	void notifyStateChg(UINT16 oldState,UINT16 newState);
	//计算一周中的第几秒
	UINT32 calcSecOfWeek(UINT32 cur);
	//添加到容器
	void add2Container(stManorState & info);
	//初始化状态
	void initState();
	//重置奖励状态
	void resetRewardState();
	//检查可以结算
	bool canCheckFinish()
	{
		return (m_manorState.m_state >= eState_Break1) && (m_manorState.m_state <= eState_FIGHT5);	
	}
	//获得状态名字
	const char * getStateName(UINT16 state);
	//更新领地战状态
	void updateManatorState();
	//获得另外一个状态
	UINT16 getNextState(UINT16 state);
private :
	stManorState m_manorState;
	bool   m_bInit;
};

#pragma pack(1)
struct stManorInfoDB
{
	UINT16 m_manorID;			//领地ID
	UINT32 m_ownerGuildID;		//属主
	UINT32 m_pkSideA;			//A方
	UINT32 m_pkSideB;			//MemNum
	UINT32 m_loserGuildID;		//失败的公会ID
	UINT8  m_bFinish;			//是否结束

	stManorInfoDB()
	{
		m_manorID = 0;
		m_ownerGuildID = 0;
		m_pkSideA = 0;
		m_pkSideB = 0;
		m_loserGuildID = 0;
		m_bFinish  = 0;
	}
};
#pragma pack()

enum eManorFightTeam
{
	eManorFightTeam1 = 0,		//领地战队伍1
	eManorFightTeam2 = 1,		//领地战队伍2
	eManorFightTeam3 = 2,		//领地战队伍3
	eManorFightTeam4 = 3,		//领地战队伍4
	eManorFightTeam5 = 4,		//领地战队伍5
	eManorFightMaxTeam = 5,
};

enum eManoFightConstDef
{
	eManoFightTeamMaxMem = 3,
};

//战斗玩家信息
struct stManorFightUser
{
	UINT32 m_id;
	std::string m_name;
	UINT8  m_job;
	UINT8  m_sex;
	UINT32 m_curHP;
	UINT32 m_maxHP;
	UINT16 m_level;
	UINT32 m_battlePower;
	UINT16 m_photoID;

	stManorFightUser()
	{
		reset();
	}

	void reset()
	{
		m_id = 0;
		m_job = 0;
		m_sex = 0;
		m_curHP = 0;
		m_maxHP = 0;
		m_level = 0;
		m_battlePower = 0;
		m_name = "";
		m_photoID = 0;
	}
};

struct stManorFightTeam
{
	stManorFightUser m_userInfo[eManoFightTeamMaxMem];
	bool   m_bEnter;
	bool   m_bWin;

	stManorFightTeam()
	{
		reset();	
	}

	void reset();
	bool isAllDie()
	{
		return ((m_userInfo[0].m_id <= 0) || (m_userInfo[0].m_curHP <= 0))
					&& ((m_userInfo[1].m_id <= 0) ||(m_userInfo[1].m_curHP <= 0)) && ((m_userInfo[2].m_id  <= 0) ||(m_userInfo[2].m_curHP <= 0));
	}
	template<class T>
	void sendProto2Mem(const  T & cmd,UINT32 exceptID = 0);
};

template <class T>
void stManorFightTeam::sendProto2Mem(const  T & cmd,UINT32 exceptID)
{
	for (UINT8 i = 0;i < eManoFightTeamMaxMem;++i){
		if (m_userInfo[i].m_id){
			Player * pUser = PlayerManager::getSingleton().getUserByID(m_userInfo[i].m_id);
			if (pUser && (pUser->getID() != exceptID)){
				pUser->sendProtoToMe(cmd);
			}
		}
	}
}

struct stManorInfo;
//阵容
struct stManorFormation
{
	stManorFightTeam m_teamA[eManorFightMaxTeam];
	stManorFightTeam m_teamB[eManorFightMaxTeam];
	bool   m_bFinish;
	bool   m_bSideAWin;
	bool   m_bSideBWin;
	GuildProto::eFormation m_eFormation;

	stManorFormation(GuildProto::eFormation formation);
	void reset();
	UINT8 getTeamMemNum(bool bTeamA,UINT8 teamID);
	void calcBattleFinish(UINT32 manorID);
	bool isBattleFinish()
	{
		return m_bFinish;
	}
	
	template <class T>
	void sendProto2TeamA(const  T & cmd,UINT32 exceptID = 0);
	template <class T>
	void sendProto2TeamB(const  T & cmd,UINT32 exceptID = 0);
};

template <class T>
void stManorFormation::sendProto2TeamA(const  T & cmd,UINT32 exceptID)
{
	for (UINT8 i = 0;i < eManorFightMaxTeam; ++i){
		m_teamA[i].sendProto2Mem(cmd,exceptID);
	}	
}

template <class T>
void stManorFormation::sendProto2TeamB(const  T & cmd,UINT32 exceptID)
{
	for (UINT8 i = 0;i < eManorFightMaxTeam; ++i){
		m_teamB[i].sendProto2Mem(cmd,exceptID);
	}	
}


//领地信息
struct stManorInfo
{
	UINT16 m_manorID;			//领地ID
	UINT32 m_ownerGuildID;		//属主
	UINT32 m_pkSideA;			//A方,公会ID
	UINT32 m_pkSideB;			//B方,公会ID
	//UINT32 m_winGuildID;		//胜利公会
	UINT32 m_loserGuildID;		//失败公会
	UINT32 m_drawGuildID1;		//平局公会1
	UINT32 m_drawGuildID2;		//平局公会2

	stManorFormation m_main;
	stManorFormation m_left;
	stManorFormation m_right;
	bool			 m_bFinish;

	stManorInfo() : m_main(GuildProto::eFormation_Main),m_left(GuildProto::eFormation_Left),m_right(GuildProto::eFormation_Right)
	{
		m_manorID = 0;
		reset();
	}
	
	void reset();
	void initFromDB(const stManorInfoDB & data);
	void fillDBData(stManorInfoDB & data);
	void print();
	void beginBattle(UINT32 round);
	UINT32 getWinGuild()
	{
		return m_ownerGuildID;
	}
	UINT32 getLoserGuild()
	{
		return m_loserGuildID;
	}
};

//玩家在领地的队伍的信息
struct stUserManorFightTeamInfo
{
	UINT32 m_manorID;
	UINT8  m_formation;
	UINT8  m_teamID;
	UINT8  m_pos;

	stUserManorFightTeamInfo()
	{
		m_manorID = 0;
		m_formation = 0;
		m_teamID = 0;
		m_pos = 0;
	}
};

class CManorInfoMgr : public  CTemplateDB<stManorInfoDB,stManorInfo>
{
	USE_SINGLETON_NEW(CManorInfoMgr);
public :
	//构造函数
	CManorInfoMgr();
	//析构函数
	~CManorInfoMgr();
	//获得实例
	static CManorInfoMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//添加到容器
	virtual void add2Container(stManorInfo & info);
	//初始化领地信息
	void initManorInfo();
	//发送领地战界面信息
	void sendManorFightWinInfo(Player * pUser);
	//队伍进入公会战
	void teamEnterBattle(stManorInfo & manorInfo,GuildProto::eFormation formation,UINT8 teamA,UINT8 teamB);
	//开始战斗
	void beginBattle(UINT16 round);
	//战斗结束进行结算
	void battleEnd();
	//进行结算
	void calcResult(stManorInfo & manorInfo);
	//进行结算
	void calcResult1(stManorInfo & manorInfo);
	//检查是否完成
	void checkBattleFinish(stManorInfo & manorInfo);
	//进入准备状态
	void enterReady();
	//进入准备状态
	void enterBreak1();
	//发送领地战斗信息
	void sendManorFightInfo(Player * pUser,UINT32 manorID);
	//发送阵容信息
	void sendFormation(Player * pUser,UINT32 manorID,GuildProto::eFormation eFormat);
	//是否存在领地
	bool isManorExist(UINT32 manorID);
	//设置占领者
	void setOwner(UINT32 manorID,UINT32 guildID);
	//设置pk双方的ID
	void setPKGuildID(UINT32 manorID,UINT32 sideAID,UINT32 sideBID);
	//进入领地战队伍
	void enterManorFightTeam(Player * pUser,UINT32 manorID,GuildProto::eFormation formation,UINT8 teamID,UINT8 pos);
	//玩家离开了领地战界面
	void userLeaveTeam(Player * pUser);
	//给某个领地的玩家奖励
	void giveReward();
	//某个领地队伍完成了战斗
	void finishFight(Global::stFinishManorFight * recvCmd);
	//改变位置
	void chgTeamPos(Player * pUser,UINT8 destTeamID,UINT8 destPos);
	//获得领地信息
	stManorInfo * getManorInfo(UINT32 manorID);	
	//获得领地战获得的boxID
	UINT32 getOwnerPrayBoxID(UINT32 guildID);
private :
	std::map<UINT16,stManorInfo> m_manorInfos;
	std::map<UINT32,stUserManorFightTeamInfo> m_userTeamInfos;
};

#pragma pack(1)
struct stManorApplyDB
{
	UINT32 m_manorID;				//领地ID
	UINT32 m_guildID;				//申请公会
	UINT32 m_contribute;			//贡献度
	UINT32 m_createTime;			//创建时间

	stManorApplyDB()
	{
		m_manorID = 0;
		m_guildID = 0;
		m_contribute = 0;
		m_createTime = 0;
	}
};
#pragma pack()

struct stManorApplyInfo
{
	UINT32 m_manorID;				//领地ID
	UINT32 m_guildID;				//申请公会
	UINT32 m_contribute;			//贡献度
	UINT32 m_createTime;			//创建时间

	stManorApplyInfo()
	{
		m_manorID = 0;
		m_guildID = 0;
		m_contribute = 0;
		m_createTime = 0;
	}

	void initFromDB(const stManorApplyDB & data);
	void fillDBData(stManorApplyDB & data);
	void print();

	bool operator < (const stManorApplyInfo & one) const 
	{   
		if (m_contribute > one.m_contribute){
			return true;
		} 
		else if (m_contribute == one.m_contribute){
			return m_createTime < one.m_createTime;
		}
		return false;
	}   
};



class CManorApplyMgr :public CTemplateDB<stManorApplyDB,stManorApplyInfo>
{
	USE_SINGLETON_NEW(CManorApplyMgr);
public :
	//获得实例
	static CManorApplyMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//构造函数
	CManorApplyMgr();
	//析构函数
	~CManorApplyMgr();
	//添加一天申请目录
	void add2Container(stManorApplyInfo & info);
	//发送某个界面
	void sendManorApplyList(Player * pUser,UINT32 manorID);
	//申请挑战某个公会
	void reqChallengeManor(Player * pUser,UINT32 manorID,UINT32 contribute);
	//刷新申请信息
	void refreshApplyInfo(Player * pUser,UINT32 manorID);
	//申请周期结束
	void endApply();
	//清空所有内容
	void clearApplyInfo()
	{
		m_applyInfos.clear();
		m_guildID2ManorIDs.clear();
	}
	//返回贡献度
	void returnContribute();
	//返回公会申请的领地
	UINT32 getApplyManorID(UINT32 guildID);	
private :
	std::map<UINT32, std::set<stManorApplyInfo> > m_applyInfos;
	std::map<UINT32, UINT32> m_guildID2ManorIDs;					//公会申请的领地
	std::map<UINT32,UINT32> m_retContributeGuilds;
};
#endif
