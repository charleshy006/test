#ifndef __GAMEUSER_H__
#define __GAMEUSER_H__
#include "CommonMicro.h"
#include "RoleInfo.h"
#include "Being.h"
#include "RoleExtBinMgr.h"
#include "Item.h"
#include "CountMgr.h"
#include "GameServer.h"
#include "Game.pb.h"
#include "PubGame.pb.h"
#include "Global.pb.h"
#include "Hero.h"
#include "Task.h"
#include "ShopMgr.h"
#include "ItemBox.h"
#include "RewardSys.h"
#include "TitleMgr.h"
#include "SpriteSys.h"
#include "DrawMgr.h"
#include "LevelMgr.h"
#include "LeitaiMgr.h"
#include "RMBShop.h"
#include "Activity.h"
#include "SevenDayActivityMgr.h"
#include "RechargeActivity.h"
#include "QuestInvestigateMgr.h"
#include "LuckShopMgr.h"
#include "LimitFashonDrawMgr.h"
#include "OpenFunction.h"
#include "ExpeditionMgr.h"
#include "PubSvrClient.h"
#include "GuildBlackMarketMgr.h"
#include "MonsterMgr.h"
#include "MountMgr.h"
#include "ProFilePhotoMgr.h"
#include "LevelWflfareMgr.h"

extern GameServer * g_gameServer;

enum
{
	ECOUNT_HERO_STUDY_SKILL_NUM = 1, //使用钻石领悟次数
	ECOUNT_FETCH_LILIAN_NUM = 2,	 //领取历练次数
	ECOUNT_GUILD_DONATE = 3,		 //公会捐赠
	ECOUNT_GUILD_PRAY = 4,			 //公会祈福
	ECOUT_TODAY_RECHARGE = 5,		 //每日充值
	ECOUT_EAT_BIG_DINNER = 6,		 //吃大餐次数
	ECOUNT_EQUIP_JUEXING = 7,		//装备觉醒
	ECOUNT_LEITAI_ROUND = 8,		//擂台的轮次
	ECOUNT_DRAW_NUM = 9,			//抽奖次数
	ECOUNT_LEITAI_USETIME = 10,		//擂台使用时间
	ECOUNT_LEITAI_RESETNUM = 11,	//擂台重置次数
	ECOUNT_REFRESH_HEISHI = 12,		//黑市刷新次数
	ECOUNT_DAY_FRESH_LEVEL = 13,	//关卡刷新次数
	ECOUNT_DAY_CONSUME = 14,		//日消耗
	ECOUNT_WEEK_CONSUME = 15,		//周消耗
	ECOUNT_LIMIT_LOGIN_NUM = 16,	//限时登录次数
	ECOUNT_REAL_TIME_NUM = 17,		//实时副本次数
	ECOUNT_ONE2ONE_WIN_NUM = 18,	//1v1胜利次数
	ECOUNT_TEAM_COPYMAP_RESETNUM = 19,//组队副本重置次数
	ECOUNT_REALTIME_RESETNUM = 20,	//实时副本充值次数
	ECOUNT_SIGN_RESET = 21,			//开服重置
	ECOUNT_EXPEDITION_NUM = 22,     //远征副本次数
	ECOUNT_DIAMOND_BUY_GOLD_NUM = 23,//宝石购买金币次数
	ECOUNT_EMERALD_DREAM_RESETNUM = 24,//翡翠梦境重置次数
	ECOUNT_EMERALD_DREAM_CAN_ENTER_NUM  = 25,//翡翠梦境每天可进入次数
	ECOUNT_EMERALD_DREAM_CURRENT_NUM = 26,//翡翠梦境当前进入次数
	ECOUNT_HERO_LEAGUE_LIMIT_NUM = 27,	//英雄联赛初赛挑战次数
	ECOUNT_WORSKIP_NUM = 28,			//朝拜次数
	ECOUNT_BLACK_MARKET_BUY_NUM = 29,	//公会黑市购买次数
	ECOUNT_REFRESH_NUM = 30,			//公会黑市刷新次数
	ECOUNT_DIAMOND_REFRESH_NUM = 31,	//公会黑市钻石刷新次数
	ECOUNT_REWARD_RECEIVE_NUM = 32,		//每日首冲奖励领取次数
	ECOUNT_MONSTER_BUY_COUNT = 33,		//小怪兽购买次数
	ECOUNT_PET_COUNT = 34,				//宠物商店刷新次数
};

class GameUser ;
class CGameUserCountMgr : public CCountMgr
{
public :
	CGameUserCountMgr(GameUser *pOwner);
	~CGameUserCountMgr();
	void  resetValueCB(UINT32 id);
protected :
	virtual UINT32  curTime();
	GameUser * m_pOwner;
};

class GameUser;
class CGameOpenFunction : public COpenFunction
{
public :
	CGameOpenFunction(GameUser * pOwner);
	virtual ~CGameOpenFunction();
	virtual UINT16 getLevel();
private :
	GameUser * m_pOwner;
};

struct EnchantItem {
    UINT8   color;      //附魔color
    UINT8   locked;     //是否锁定
    UINT8   propType;   //附魔属性
    UINT32  propVal;    //附魔值
    EnchantItem () {
        color = 0;
        locked = 0;
        propType = 0;
        propVal = 0;
    }
};

struct LuckyDrawMgr {
    UINT32 m_curId;                      //活动id
    UINT32 m_diamond;                   //累计消费钻石
    UINT32 m_lastFreeDraw;              //上一次免费抽奖时间
    UINT32 m_drawNum;                  //抽奖次數
    std::map<UINT8, bool> m_drawMap;    //积分领取记录
    bool drawReward(UINT8 id);
    LuckyDrawMgr () {
        m_curId = 0;
        m_diamond = 0;
        m_lastFreeDraw = 0;
        m_drawNum = 0;
    }
    void load(const char * buf);
    void save(char * buf);
};

struct LuckyDrawRecord {
    UINT32 id;
    UINT32 num;
    std::string name;
};

struct DailyInviteFriend {
    enum {
        None_Reward,    //没奖励
        Reward_Get,     //已领
        Has_Reward,     //有奖励
    };

    UINT32 time;
    UINT32 num;
    UINT32 onceReward;
    std::set<UINT32> drawSet;

    std::set<UINT32> & getDrawList() {
        return drawSet;
    }
    bool hasDrawDailyReward(UINT8 id) {
        return drawSet.find(id) != drawSet.end();
    }
    void drawDailyReward(UINT8 id) {
        drawSet.insert(id);
    }
    DailyInviteFriend () {
        time = 0;
        num = 0;
        onceReward = None_Reward;
    }
    void addNum(int val = 1) {
        num += val;
    }
    UINT32 getNum() {
        return num;
    }
    bool hasOnceReward() {
        return onceReward == Has_Reward;
    }
    bool isDrawOnceReward() {
        return onceReward == Reward_Get;
    }
    void setOnceReward() {
        if (onceReward == None_Reward) {
            onceReward = Has_Reward;
        }
    }
    void drawOnceReward() {
        onceReward = Reward_Get;
    }
    void checkSameDay();
    std::string toString();
    void fromString(const char * data);
};

struct DragonNestInfo {
    enum {
        Max_Used_Times = 6,
    };

    UINT8  usedTimes;
    UINT32 resetTimes;
    UINT32 resetDate;

    DragonNestInfo () {
        usedTimes = 0;
        resetTimes = 0;
        resetDate = 0;
    }
    void checkReset();
    std::string toString();
    void fromString(const char * data);
    void addUsedTimes(UINT32 val = 1) {
        usedTimes += val;
    }
};

struct StoneCopyMapInfo {                 
	enum {                              
		Max_Used_Times = 6,             
	};                                  

	UINT8  usedTimes;                   
	UINT32 resetTimes;                  
	UINT32 resetDate;                   

	StoneCopyMapInfo() {                 
		usedTimes = 0;                  
		resetTimes = 0;                 
		resetDate = 0;                  
	} 

	void checkReset();                  
	std::string toString();             
	void fromString(const char * data); 
	void addUsedTimes(UINT32 val = 1) { 
		usedTimes += val;               
	}   
};

struct LateWalfareInfo {
    UINT32 id;
    UINT32 expireTime;
    std::set<UINT32> rewardSet;

    LateWalfareInfo () {
        id = 0;
        expireTime = 0;
    }
	std::string toString();             
	void fromString(const char * data); 
    void setId(UINT32 activityId) {
        id = activityId;
    }
    UINT32 getId() {
        return id;
    }
    UINT32 getExpireTime() {
        return expireTime;
    }
    void setExpireTime(UINT32 time) {
        expireTime = time;
    }
    void drawReward(UINT32 id) {
        rewardSet.insert(id);
    }
    bool rewardDrawed(UINT32 id) {
        return rewardSet.find(id) != rewardSet.end();
    }
};

struct ResBackItem {
    std::string dayTime;
    UINT16 value;
    UINT16 vipLvl;
    ResBackItem () {
        value = 0;
        vipLvl = 0;
    }
};

struct ResBackInfo {
    //vip和次数挂钩的特殊处理一下
    std::map<UINT32, std::vector<ResBackItem> > m_resBackMap;

	std::string toString();
	void fromString(const char * data); 
    void addBackRes(UINT32 type, UINT16 num, UINT16 vipLvl);
    void checkBackRes(GameUser * pUser);
};

class GameUser : public CBeing
{
public :
	static UINT64  USER_TEMPID_GEN;									//玩家连是ID生成器
	static UINT32  USER_GROUPID_GEN;								//玩家的组ID生成器
	static const UINT32 s_RECOVER_SP_LILIAN_INTERVAL = 480;			//恢复体力间隔
	static std::vector<LuckyDrawRecord> m_luckyDrawRecordVec;        //最新50条数据
	static void loadLuckyDrawRecord(std::vector<LuckyDrawRecord> & recordVec);
	static void addLuckyDrawRecord(LuckyDrawRecord & record);
	static void addLuckyDrawRecord(std::vector<LuckyDrawRecord> & recordVec);
public :
	char                 	m_account[MAX_ACCNAMESIZE + 1];        	//玩家帐号
private:
	UINT16					m_gateID;				//网关链接
	char 				    m_ip[MAX_IP_LENGTH + 1];//登录的IP				
	char					m_netIP[MAX_IP_LENGTH +1];
private:
	std::map<UINT64, std::vector<EnchantItem> > m_enchantMap;      //附魔增加的属性
	std::map<UINT32, std::map<UINT32, UINT32> > m_raiseSkillProp;      //技能强化的属性
public :
	RoleInfo 				m_roleInfo;			//角色数据
	stRoleState				m_roleState;		//角色状态
	Timer 					m_saveTimer;		//保存定时器
	Timer					m_oneSec;			//1秒
	Timer					m_10Sec;			//10秒
	RoleExtBinMgr           m_roleExtBinMgr;	//角色扩展2进制
	UINT32 					m_startTime;		//开始时间
	UINT8					m_gmType;			//gm账号类型
	UINT32					m_groupID;			//玩家的组ID
	CBagMgr 				m_bagMgr;			//包裹管理
	CTaskMgr                m_taskMgr;          //任务管理
	bool					m_zeroRefresh;		//零点刷新
	CGameUserCountMgr       m_countMgr;			//计数器管理器
	HeroInfoMgr				m_heroMgr;			//武将管理器
	INT32					m_iBroadcastIdx;	//所在数组的索引
	CShopMgr				m_shopMgr;			//商店信息
	ItemBoxMgr				m_boxMgr;			//箱子管理
	CRewardSys				m_rewardSys;		//奖励系统
	UINT32 					m_onlineTime;		//累计在线时间,每天刷新
	CTitleMgr				m_titleMgr;			//称号系统
	CSpriteSysMgr			m_spriteSysMgr;		//打星系统
	CDrawMgr				m_drawMgr;			//抽奖管理
	CLevelMgr				m_levelMgr;			//关卡管理
	UINT32					m_guildID;			//公会ID
	UINT16					m_guildLv;			//公会等级
	UINT32					m_joinInTime;		//加入公会时间
	UINT16					m_blackMacketLv;	//公会黑市等级
	UINT8 					m_eCreateGuildRes;	//创建公会
	UINT16 					m_vipLv;			//vip等级
	UINT8					m_buyInvestPlan;	//购买投资
	UINT32					m_buySummonNum;		//购买召唤次数
	UINT8					m_buySummonPlan;	//购买召唤
	std::set<UINT16> 		m_freeSummonSet;	//免费召唤列表
	std::set<UINT16> 		m_rechargeSummonSet;//收费召唤列表
	UINT32					m_buyLimitSummonNum;//购买限时召唤次数
	UINT8					m_buyLimitSummonPlan;	//购买限时战令
	std::set<UINT16> 		m_freeLimitSummonSet;	//免费限时召唤列表
	std::set<UINT16> 		m_rechargeLimitSummonSet;//收费限时召唤列表
	std::map<UINT32,UINT32> m_buyTreasureCount;		//购买宝箱次数限制
	UINT32 					m_recoverSPLilianTime;//上一次恢复体力/历练/调教体力时间
	CLeitaiMgr				m_leitaiMgr;		//擂台管理数据
	CRMBShop				m_rmbShop;			//人民币商店管理
	UINT32 					m_currentTaskID;	//当前正在进行的任务
	UINT32					m_currentTaskStage;	//当前任务场景
	std::string				m_deviceID;			//设备ID
	UINT32					m_loginTime;		//登录时间
	UINT32					m_firstLogin;		//首次登录时间
	UINT32					m_arenaHistoryRank;	//竞技场历史排名
	UINT32					m_lastHitByNormalTime;//最近一次受普通攻击时间
	UINT32					m_batiEmptyTime;	  //霸体变空状态
	UINT64					m_systemSetting;	 //系统设置
	CActivityDataMgr		m_activityDataMgr;	 //活动管理
	UINT32 					m_lastIntoMapID;	//上一次进入关卡ID
	UINT32					m_beginCollectTime;	//开始采集时间
	UINT32					m_deathTime;		//死亡时间
	UINT32					m_compensationID;	//补偿id
	CSevenDayActivityMgr	m_sevenDayActivityMgr;//7天活动奖励
	UINT16					m_extraDamagePer;	//额外的伤害百分比
	UINT16					m_createRoleDay;	//创建角色天数
	bool					m_bRobot;			//是否是机器人
	CRechargeActivityMgr    m_rechargeActivityMgr;//充值活动管理器
	UINT32    				m_yestodayFirstLoginTime; //昨天第一次登录时间
	UINT16    				m_cumulativeLogin;        //累积登录
	CQuestInvestigateMgr	m_questInvestigateMgr;	 //问卷调查
	CLuckShopMgr			m_luckShopMgr;			//幸运商店
	UINT32					m_arenaRank;			//竞技场排名
	CLimitFashonDrawMgr		m_limitFashionDrawMgr;	//限时时装抽奖
	UINT32					m_lastSpeakTime;		//最后一次世界聊天
	std::string 			m_channelID;			//渠道ID
	CGameOpenFunction		m_openFuncMgr;			//开放等级管理
	UINT16 					m_equipAveBreakLv;      //装备平均突破等级
	UINT8     				m_wingBreakLv;          //翅膀突破等级
	UINT8					m_changeNameNum;		//改名次数
    CExpeditionMgr          m_expeditionMgr;        //远征副本管理
    LuckyDrawMgr            m_luckyDrawMgr;         //挖宝
    DailyInviteFriend       m_dailyInvite;          //每日分享
    DragonNestInfo          m_dragonNest;           //巨龙巢穴
    bool                    m_inMarryScene;         //是否在结婚虚拟场景内
    bool                    m_isSetDivorceExpireTime;//是否设置过离婚
	CGuildBlackMarketMgr	m_guildBlackMarketMgr;	//公会黑市
	StoneCopyMapInfo		m_stoneCopyMap;			//宝石副本
	LateWalfareInfo         m_lateWalfareMgr;		//限时福利
    ResBackInfo             m_resBackMgr;           //找回管理
	CMonsterMgr				m_monsterMgr;			//小怪兽
	CMountMgr				m_mountMgr;				//坐骑
	CProFilePhotoMgr		m_photoMgr;				//头像管理
	CLevelWflfareMgr		m_wflfareMgr;			//冲级好礼
public:
	//获得类型
	virtual ObjType getType()const
	{
		return CObj::OBJ_TYPE_PLAYER;
	}
	//地面通行等级	
	virtual UINT8 getDriveLevel()const
	{
		return 1;
	}
	//移动时候造成阻挡
	virtual bool isBlockObj()const
	{
		return false;
	}
	//取得唯一ID
	virtual UINT32 getUniqueID()const
	{
		return getID();
	}
	//构造函数
	GameUser(const char * account,UINT16 gateID,const char * ip);
	//析构函数
	virtual ~GameUser();
	//获得账号
	const char *  getAccount() {  return m_account ;}
	//返回等级
	UINT16 getLevel() { return m_roleInfo.m_level ; }
	//注销角色
	void unreg(Global::SaveType saveType = Global::SAVE_LOGOUT);
	//发送消息给客户端
	template <class T>
	bool sendProto2Me(const T & cmd);
	//给网关发送协议
	template <class T>
	bool sendProto2Gate(const T & cmd);
	//获得网关连接
	UINT16  getGateID(){ return m_gateID; }
	//压缩数据
	INT32 compressSaveData(byte* buf, const UINT32 size=MAX_DATASIZE);
	//解压缩据
	bool uncompressSaveData(const byte * data, const UINT32 size);
	//保存数据
	bool save(const Global::SaveType savetype);
	//定时回调 
	bool heartBeat( UINT64 uTime);
	//第一次登陆
	void firstLogin();
	//初始化角色
	void initTestRoleData();
	//玩家上线
	void userOnline();
	//填充主句数据
	void fillMainData(Role::stRoleMainData * pRoleMainData);
	//发送主角数据
	void sendMainData2Me();
	//初始化角色数据
	bool initData(const RoleInfo & roleInfo,byte * binData,UINT32 cmdLen);
	//获得角色ID
	UINT32 getRoleID();
	//设置角色的状态
	void setupRoleState(bool login = false);
	//计算自身战力
	void calcBattlePower();	
	//增加了经验
	UINT32 addExp(UINT32 exp);
	//等级提升了
	void levelup();
	//获得金币数量
	UINT32 getGoldCoin(){  return m_roleInfo.m_goldCoin ;}
	//获得钻石数量
	UINT32 getDiamond() {  return m_roleInfo.m_giveDiamond + m_roleInfo.m_diamond ; }
	//获得黑市结晶
	UINT32 getHeishijiejing() { return m_roleInfo.m_heishijiejing ; }
	//获得声望
	UINT32 getReputation() { return m_roleInfo.m_reputaion ; }
	//获得声荣誉
	UINT32 getHonour() { return m_roleInfo.m_honour ; }
	//获得勇敢值
	UINT32 getBraveVal() { return m_roleInfo.m_braveVal; }
	//获得巅峰令
	UINT32 getPeakCertificate() { return m_roleInfo.m_peakCertificate ; }
	//获得虎符
	UINT32 getHufu() { return m_roleInfo.m_hufu ; }
	//获得陨铁
	UINT32 getYunTie() { return m_roleInfo.m_yuntie ; }
	//获得觉醒经验
	UINT32 getJuxingExp() { return m_roleInfo.m_juexingExp; }
	//获得积分
	UINT32 getIntegral() {return m_roleInfo.m_integral; }
	//获得公会贡献度
	UINT32 getContribute() { return m_roleInfo.m_contribute; }
	//获得体力
	UINT32 getSP() { return m_roleInfo.m_sp; }
	//获得体力
	UINT32 getTrainSP() { return m_roleInfo.m_trainSp; }
	//获得历练
	UINT32 getLilian() { return m_roleInfo.m_lilian ; }
	//获得魂火
	UINT32 getSoulFire() { return m_roleInfo.m_soulFire; }
	//获得生命精华
	UINT32 getLifeEssence() {return m_roleInfo.m_lifeEssence; }
	//获得公会黑市结晶
	UINT32 getGuildCrystal() {return m_roleInfo.m_guidCrystal; }
	//
	void sendGuaji();
	void checkNftVipGift();
	void CheckGuajiStartClear(bool send=true);
private :
	//检查金币是否足够
	bool  checkGoldCoin(UINT32 num);
	//增加金币
	bool  addGoldCoin(UINT32 num,const char * desc);
	//删除金币
	bool  removeGoldCoin(UINT32 num,const char * desc);
	//检查钻石
	bool checkDiamond(UINT32 num);
	//增加钻石
	bool addDiamond(UINT32 num,const char * desc);
	//删除钻石
	bool removeDiamond(UINT32 num,const char * desc);
	//增加黑市结晶
	bool addHeishijiejing(UINT32 num, const char * desc);
	//删除黑市结晶
	bool removeHeishijiejing(UINT32 num,const char * desc);
	//检查黑市结晶
	bool checkHeishijiejing(UINT32 num);
	//增加声望
	bool addReputation(UINT32 num,const char * desc);
	//扣除声望
	bool removeReputaion(UINT32 num,const char * desc);
	//检查声望
	bool checkReputation(UINT32 num);
	//增加荣誉
	bool addHonour(UINT32 num,const char * desc);
	//扣除荣誉
	bool removeHonour(UINT32 num,const char * desc);
	//检查荣誉值
	bool checkHonour(UINT32 num);
	//增加勇敢值
	bool addBraveVal(UINT32 num,const char * desc);
	//删除勇敢值
	bool removeBraveVal(UINT32 num,const char * desc);
	//检查勇敢值
	bool checkBraveVal(UINT32 num);
	//增加巅峰证
	bool addPeakCertificate(UINT32 num,const char * desc);
	//删除巅峰证
	bool removePeakCertificate(UINT32 num,const char * desc);
	//检查巅峰
	bool checkPeakCertificate(UINT32 num);
	//增加虎符
	bool addHufu(UINT32 num,const char * desc);
	//删除虎符
	bool removeHufu(UINT32 num,const char * desc);
	//检查虎符
	bool checkHufu(UINT32 num);
	//增加陨铁
	bool addYuntie(UINT32 num ,const char * desc);
	//扣除陨铁
	bool removeYuntie(UINT32 num,const char * desc);
	//检查陨铁
	bool checkYuntie(UINT32 num);
	//检查觉醒经验
	bool checkJuexingExp(UINT32 num);
	//增加觉醒经验
	bool addJuxingExp(UINT32 num,const char * desc);
	//减少觉醒经验
	bool removeJuxingExp(UINT32 num,const char * desc);
	//检查积分
	bool checkIntegral(UINT32 num);
	//增加积分
	bool addIntegral(UINT32 num,const char * desc);
	//减少积分
	bool removeIntegral(UINT32 num,const char * desc);
	//增加公会贡献度
	bool addContribute(UINT32 num,const char * desc);
	//减少公会贡献度
	bool removeContribute(UINT32 num,const char * desc);
	//检查贡献度
	bool checkContribute(UINT32 num);
	//增加体力
	bool addSP(UINT32 sp,const char * desc);
	//扣除体力
	bool removeSP(UINT32 sp,const char * desc);
	//检查体力
	bool checkSP(UINT32 sp);
	//增加调教体力
	bool addTrainSP(UINT32 sp,const char * desc);
	//扣除调教体力
	bool removeTrainSP(UINT32 sp,const char * desc);
	//检查调教体力
	bool checkTrainSP(UINT32 sp);
	//增加历练
	bool addLilian(UINT32 lilian,const char * desc);
	//减少历练
	bool removeLilian(UINT32 lilian,const char * desc);
	//检查历练
	bool checkLilian(UINT32 lilian);
	//增加魂火
	bool addSoulFire(UINT32 soulFire,const char * desc);
	//减少魂火
	bool removeSoulFire(UINT32 soulFire,const char * desc);
	//检查魂火
	bool checkSoulFire(UINT32 soulFire);
	//增加生命精华 
	bool addLifeEssence(UINT32 fileEssence,const char * desc);
	//减少生命精华 
	bool removeLifeEssence(UINT32 fileEssence,const char * desc); 
	//检查生命精华
	bool checkLifeEssence(UINT32 fileEssence);
	//增加公会黑市结晶                                     
	bool addGuildCrystal(UINT32 guildCrystal,const char * desc);                  
	//减少公会黑市结晶                                        
	bool removeGuildCrystal(UINT32 guildCrystal,const char * desc);
	//检查公会黑市结晶                                        
	bool checkGuildCrystal(UINT32 guildCrystal);                
public :
	//增加资源
	bool addResource(eResource eRes,UINT32 num,const char * desc);
	//扣除资源
	bool removeResource(eResource eRes,UINT32 num,const char * desc);
	//检查资源是否足够
	bool checkResource(eResource eRes,UINT32 num);
	//获得属性值
	UINT32 getPropVal(UINT16 propID,UINT32 propParam);
	//任务状态更新
	void  taskStatusUpdate(NOTIFY_TYPE type,stTaskDBInfo *pTaskInfo);
	//获得血量
	UINT32 getHP() { return m_hp ;}
	//计算战斗数值
	void calcPKData();
	//进入地图
	virtual bool intoNewMap(CMap * newMap,bool needInit = false, bool send2Client = true);
	//收集玩家数据
	virtual void getAppear(Map::stPlayerData & data);
	//根据计数类型获得次数
	UINT32 getCountByVip(UINT16 countType);
	//获得vip等级
	UINT16 getVipLv(){ return m_vipLv ; }
	//获得职业
	UINT8  getJob() { return m_roleInfo.m_job ;}
	//获得性别
	UINT8  getSex() { return m_roleInfo.m_sex ; }
	//计算vip等级
	void calcVipLv();
	//同步充值
	void synTotalRecharge();
	//同步等级
	void synMyLevel(bool bLevelup = false);
	//同步我的战力
	void synMyBattlePower();
	//同步我的关卡星数
	void synMyCopymapStarNum();
	//同步我的头像ID
	void synMyPhotoID();
	//填充玩家详细信息
	void fillDetailInfo(Role::stUserDetailInfo * pDetail);
	//给玩家财产
	bool giveAssert(UINT32 itemID,UINT32 num,UINT16 raiseLv,const char * desc);
	//扣除财产
	bool removeAssert(UINT32 itemID,UINT32 num,UINT16 raiseLv,const char * desc);
	//检查资源是否足够
	bool checkAssert(UINT32 itemID,UINT32 num);
	//离开地图
	virtual void onLeaveMap(CMap *);
	//加入地图时候调用
	virtual void onEnterMap(CMap *);
	//死亡处理
	virtual void processDeath(CBeing *pAtter);
	//回复到最大血量
	void resetBattleProp()
	{
		m_hp = m_maxHP;
		m_lastHP = m_hp;
		m_mp = m_maxMP;
		m_bati = m_maxBati;
	}
	//强制设置血量
	void forceSetHP(UINT32 hp);
	//同步贡献度
	void synContribyte2Global();
	//获得IP
	const char * getIP()
	{
		return m_ip;
	}
	void setNetIP(const char * netIP)
	{
		if(netIP){
			strncpy(m_netIP,netIP,sizeof(m_netIP));	
		}
	}
	const char * getNetIP()
	{
		return m_netIP;
	}
	//血量乘系数
	void hpMultiCoef(float coef)
	{
		m_maxHP = m_hp = m_lastHP = UINT32(m_roleState.m_battleProps[eBATTLE_PROP_HP] * coef);
	}
	//霸体乘系数
	void batiMultiCoef(UINT32 coef)
	{
		m_bati = m_maxBati = m_roleState.m_battleProps[eBATTLE_PROP_BATI] * coef;
	}
	//领地战血量设置
	void hpMutilCoefInManor(UINT32 coef)
	{
		m_maxHP = m_roleState.m_battleProps[eBATTLE_PROP_HP] * coef;
		m_hp = m_lastHP = m_hp * coef;
		if (m_hp > m_maxHP){
			m_lastHP = m_hp = m_maxHP;
		}
	}
	//获得蓝恢复
	virtual UINT32 getRecoverMP();
	//回复血量
	virtual UINT32 getRecoverHP();
	//吸血比例
	virtual UINT32 getAbsorbHPPer();
	//收到普通攻击
	virtual void   hitByNormal();
	//霸体变空
	virtual void   batiEmpty();
	//充值
	void recharge(UINT32 money,UINT32 addDiamondNum,UINT32 itemID,const std::string & tid,UINT32 giveDiamond = 0);
	//给global同步自己信息
	void synMyInfoToGlobal();
	//同步所有战斗属性
	void sendBattleProp();
	//增加组队副本次数
	void addTeamCopyMapNum(UINT16 num);
	//增加实时副本次数
	void addReamTimePKNum(UINT16 num);
	//增加vip经验
	void addGiveVipExp(UINT32 num);
	//增加vip经验
	void addVipExp(UINT32 num);
	//增加1vs1积分
	void addOne2OnePVPScore(UINT32 score);
	//扣除1vs1积分
	void removeOne2OnePVPScore(UINT32 score);
	//获得积分
	UINT32 getOne2OnePVPScore()
	{
		return m_roleInfo.m_one2OnePVPScore;
	}
	//清空1vs1积分
	void clearOne2OneScore()
	{
		m_roleInfo.m_one2OnePVPScore = 0;
	}
	//检查采集
	void checkCollect();
	//检查复活
	void checkRelive();
	//检测霸体恢复
	void checkBati();
	//复活
	void relive();
	//获得战力
	UINT32 getBattlePower()
	{
		return m_roleInfo.m_battlePower;
	}
	//获得充值数量
	UINT32 getRechargeNum()
	{
		return m_roleInfo.m_rechargeNum;
	}
	//发送登录日志
	void sendLoginLog(bool bEnterSelectLog = false);
	//获得装备特效ID
	UINT16 getEffectID();
	//设置伤害百分比加成 
	void setDamagePer(UINT16 addDamagePer)
	{
		m_extraDamagePer = addDamagePer;
	}
	//计算创建角色天数
	void calcCreateRoleDay();
	//获取创建角色天数
	UINT16 getCreateRoleDay()
	{
		return m_createRoleDay;	
	}
	//创建普通攻击
	void createNormalSkill();

	//获得累计登录次数
	UINT16 getCumuLoginNum()
	{
		return m_cumulativeLogin;
	}
	//设置累积登录次数
	void setCumuLoginNum(UINT16 cumuLogin)
	{
		m_cumulativeLogin = cumuLogin;
	}
	//给内测奖励
	void closeBetaReward(UINT32 boxID,UINT32 diamond,UINT32 rechargeNum);
	//改名
	void changeName(const std::string & newName);
	//同步名字
	void synName(const std::string & name);
	//钻石消费日志
    void sendDiamondDataLog(UINT32 num, UINT32 decGiveDiamond, UINT32 decDiamond, const char * desc);
    bool isValidVipItem(UINT32 itemId);
    void loadEnchant();
    void saveEnchant();
    void synEnchant(Role::stSynItemInfo * pSynInfo);
    UINT8 getEnchantLockNum(UINT64 id);
    void addEnchant(UINT64 id, EnchantItem & item);
    UINT8 getEnchantPropNum(UINT64 id);
    bool getEnchantById(UINT64 id);
    bool lockEnchantById(UINT64 id, size_t index, bool locked);
    void removeEnchantById(UINT64 id);
    UINT8 replaceEnchant(UINT64 id, EnchantItem & item);
    UINT32 getEnchantSize();
    virtual UINT32 getRaiseSkillValue(UINT32 skillID, UINT32 raiseType);
    void OnInterceptEnd(bool win, bool timeout);
    void OnleitaiResult(bool timeout, UINT32 usetime, UINT32 userHPRate);
    void forceDivorce();
    bool isSetDivorceExpireTime() { return m_isSetDivorceExpireTime; }
    void setDivorceExpireTime(bool res) { m_isSetDivorceExpireTime = res; }
    UINT32 getCoupleId() { return m_roleInfo.m_coupleId; }
    void setCoupleId(UINT32 val) { m_roleInfo.m_coupleId = val; }
    std::string getCoupleName() { return m_roleInfo.m_coupleName; }
    void setCoupleName(const std::string & name) {
        memset(m_roleInfo.m_coupleName, 0, sizeof(m_roleInfo.m_coupleName));
        memcpy(m_roleInfo.m_coupleName, name.c_str(), name.length()); 
    }
    void resetMarryRecord();
    void sendLimitPetActivity();
    void checkLateWalfare();
    void drawLateWalfare(UINT32 rewardId);
    void setBackResData();
    void setDayBackRes(UINT32 lastTime);
    void synBackResVip(UINT16 oldVip, UINT16 newVip);
    void drawBackRes(UINT32 type, UINT32 drawNum);
    void drawAllBackRes();

//=====================跨服相关 start==================
	//发送用户数据给PubGame
	void sendData2PubGame();
	//通知pubserver用户离线
	void NotifyPubLogout();
	//转发Proto给PubGame
	template <class T>
	bool sendProto2PubGame(const T & cmd);
	//转发Proto给NormalGame
	template <class T>
	bool sendProto2NormalGame(const T & cmd);
	//转发Proto给NormalGame 再转发给gate 下发到客户端
	template <class T>
	bool sendProtoPub2Me(const T & cmd);
	//通知normal 用户在跨服
	void NotifyNormalOnPub(bool bOnPub);
	//通知客户端用户跨服唯一id
	void NotifyPubUniqueID();
	//
	bool isOnPub()
	{
		return m_bIsOnPubGameServer;
	}
	void setOnPub(bool bOnPub)
	{
		m_bIsOnPubGameServer = bOnPub;
	}
	UINT32 getUniquePubID()
	{
		//建角色时，zoneid<<22 | maxcharid,故charid有效范围2^22
		//charid左移10 再右移10 去除zoneID信息
		return g_gameServer->getPubServerIndex() << 22 | m_roleInfo.m_charID << 10 >> 10 ;
	}
	UINT32 getNormalCharID() const
	{
		return m_normalCharID; 
	}
	void setNormalCharID(UINT32 id)
	{
		m_normalCharID = id;
	}

	UINT32 getNormalServerID()
	{
		return m_normalServerIndex; 
	}
	void setNormalServerID(UINT32 id)
	{
		m_normalServerIndex = id;
	}
	void setAreaCode(UINT32 code){
		m_areaCode = code;
	}
	char * getUniqueName(){
		if (g_gameServer->isPubGameServer())
		{
			sprintf(m_uniqueName,"%s%d",getName(),m_normalServerIndex);
			return m_uniqueName;
		}
		else
		{
			return getName();
		}
	}
	UINT32 getAreaCode()
	{
		return m_areaCode;
	}
private:
	bool m_bIsOnPubGameServer;		//用户是否在跨服，处在normalgame时有效
	UINT32	m_normalServerIndex;	//本来在哪个gameserver，处在pubgame时有效
	UINT32  m_normalCharID;			//用户在normalserver的charid，处在pubgame时有效
	UINT32	m_areaCode;             //区服ID 
	char	m_uniqueName[MAX_NAMESIZE + 10];
	//=====================跨服相关 end==================
};


template <class T>
bool GameUser::sendProto2Me(const T & cmd)
{
	if (g_gameServer->isPubGameServer())
	{
		return sendProtoPub2Me(cmd);
	}

	if (!m_gateID){
		return false;
	}

	Game::stGameForwardGate  forwardMe;
	forwardMe.set_charid(getID());
	forwardMe.set_modid(cmd.ModID);
	forwardMe.set_funid(cmd.FunID);
	std::string retStr;
	cmd.SerializeToString(&retStr);
	forwardMe.set_cmdlen(retStr.size());
	forwardMe.set_data(retStr);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送协议 funid:%d modid:%d", cmd.FunID, cmd.ModID);
#endif
	return g_gameServer->getConnMgr().broadcastByID(m_gateID,forwardMe);
}

template <class T>
bool GameUser::sendProto2Gate(const T & cmd)
{
	if (!m_gateID){
		return false;
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送协议 funid:%d modid:%d", cmd.FunID, cmd.ModID);
#endif
	return g_gameServer->getConnMgr().broadcastByID(m_gateID,cmd);
}



template <class T>
bool GameUser::sendProto2PubGame(const T &  _cmd)
{
	PubGame::stNormalForwardPub cmd;
	cmd.set_charid(getUniquePubID());
	cmd.set_modid(_cmd.ModID);
	cmd.set_funid(_cmd.FunID);
	std::string retStr;
	_cmd.SerializeToString(&retStr);
	cmd.set_cmdlen(retStr.size());
	cmd.set_data(retStr);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送协议 to PubGame funid:%d modid:%d", _cmd.FunID, _cmd.ModID);
#endif
	return PubSvrClientMgr::getSingleton().broadcast(cmd);
}


template <class T>
bool GameUser::sendProto2NormalGame(const T &  _cmd)
{
	// PubGame::stPubForwardNormal cmd;
	// cmd.set_charid(getNormalCharID());
	// cmd.set_modid(_cmd.ModID);
	// cmd.set_funid(_cmd.FunID);
	// std::string retStr;
	// _cmd.SerializeToString(&retStr);
	// cmd.set_cmdlen(retStr.size());
	// cmd.set_data(retStr);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送协议 to NormalGame funid:%d modid:%d", _cmd.FunID, _cmd.ModID);
#endif
	return g_gameServer->getConnMgr().broadcastByID(m_normalServerIndex,_cmd);
}

template <class T>
bool GameUser::sendProtoPub2Me(const T &  _cmd)
{
	PubGame::stPubForwardGate cmd;
	cmd.set_charid(getNormalCharID());
	cmd.set_modid(_cmd.ModID);
	cmd.set_funid(_cmd.FunID);
	std::string retStr;
	_cmd.SerializeToString(&retStr);
	cmd.set_cmdlen(retStr.size());
	cmd.set_data(retStr);
#ifdef _HDZ_DEBUG
	Athena::logger->trace("发送协议 pub2me funid:%d modid:%d", _cmd.FunID, _cmd.ModID);
#endif
	return g_gameServer->getConnMgr().broadcastByID(m_normalServerIndex,cmd);
}

#endif
