#ifndef __H_ACTIVITY_H__
#define __H_ACTIVITY_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "json/reader.h"
#include "Misc.pb.h"
#include <map>
#include <vector>
#include <set>

class GameUser;

/*enum eActivityID
{
	eActivity_Daily_Consume = 1,		//每日消耗
	eActivity_Weekly_Consume = 2,		//每周消耗
	eActivity_Cumu_Login = 3,			//累积登录
	eActivity_Weekly_Cumu_Login = 4,	//周累积登录
	eActivity_Special_Time_Login = 5,	//限时登录奖励
};*/

template <class T>
class ActivityFactory
{
	USE_SINGLETON_NEW(ActivityFactory<T>);
private :
	ActivityFactory()
	{}
public :
	static ActivityFactory<T> & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	//创建对象
	T * create()
	{
		return ATHENA_NEW T();
	}

	//销毁对象
	void recycle(T * pObj)
	{
		delete pObj;
	}
};

//活动的条件 
/*class ActivityCondition
{
public :
	//构造函数
	ActivityCondition()
	{}
	//析构函数
	virtual ~ActivityCondition()
	{}
	//检查条件
	virtual bool checkValid(GameUser * pUser) = 0;
	//初始化
	virtual bool init(const char * szCond) = 0;
};*/

//活动阶段可选奖励
struct stActivityStepOptionalReward
{
	std::vector<std::pair<UINT32,UINT32> > m_rewardItems;	//奖励道具
	UINT16 m_minLv;											//最小等级
	UINT16 m_maxLv;											//最大等级
	UINT16 m_minVipLv;										//最小vip等级
	UINT16 m_maxVipLv;										//最大vip等级

	stActivityStepOptionalReward()
	{
		m_minLv = 0;
		m_maxLv = (UINT16)-1;
		m_minVipLv = 0;
		m_maxVipLv = (UINT16)-1;
	}
};

class Activity;
//活动的阶段
class ActivityStep
{
public :
	//构造函数
	ActivityStep(Activity * pActivity);
	//析构函数
	virtual ~ActivityStep();
	//奖励
	virtual void doReward(GameUser * pUser,UINT16 selIndex);
	//初始化
	virtual bool init(UINT32 id,std::string desc);
	//增加一个道具奖励
	bool addRewardItem(std::string & itemStr,std::string & resStr,UINT16 minLv = 0,UINT16 maxLv = (UINT16)-1,UINT16 minVipLv = 0,UINT16 maxVipLv = (UINT16)-1);
	//打印数据
	virtual void print();
	//填充活动阶段
	void fillActivityStepInfo(MiscProto::stSynActivityStepInfo & pSyn);
	//判断阶段奖励的条件是否满足
	virtual bool isCondMeet(UINT32 curStepVal) = 0;
	//设置ID
	void setID(UINT32 id)
	{
		m_id = id;
	}
	//设置描述
	void setDesc(std::string desc)
	{
		m_desc = desc;
	}
	//获得描述
	std::string getDesc()
	{
		return m_desc;
	}
	//获得ID
	UINT32 getID()
	{
		return m_id;
	}
protected :
	//std::vector<ActivityCondition *> m_conditions;
	UINT32 m_id;												//id
	std::string m_desc;											//描述
	Activity * m_pActivity;										//对应的活动
	std::vector<stActivityStepOptionalReward> m_rewards;		//奖励
};

//消耗类的活动阶段
class ConsumeActivityStep : public ActivityStep
{
public :
	//构造函数
	ConsumeActivityStep(Activity * pActivity);
	//析构函数
	~ConsumeActivityStep();
	//阶段条件
	virtual bool isCondMeet(UINT32 curStepVal);
};

//周累积消耗活动
class WeeklyConsumeActivityStep : public ActivityStep
{
public :
	//构造函数
	WeeklyConsumeActivityStep(Activity * pActivity);
	//析构函数
	~WeeklyConsumeActivityStep();
	//阶段条件
	virtual bool isCondMeet(UINT32 curStepVal);
};

//登录类的活动阶段
class CumuLoginActivityStep : public ActivityStep
{
public :
	//构造函数
	CumuLoginActivityStep(Activity * pActivity);
	//析构函数
	~CumuLoginActivityStep();
	//阶段条件
	virtual bool isCondMeet(UINT32 curStepVal);
};


//限时登录阶段
class SpecialTimeLoginActivityStep : public ActivityStep
{
public :
	//构造函数
	SpecialTimeLoginActivityStep(Activity * pActivity);
	//析构函数
	~SpecialTimeLoginActivityStep();
	//阶段条件
	virtual bool isCondMeet(UINT32 curStepVal);
};

//兑换活动
class ExchangeActivityStep : public ActivityStep
{
public :
	//构造函数
	ExchangeActivityStep(Activity * pActivity);
	//析构函数
	~ExchangeActivityStep();
	//奖励
	virtual void doReward(GameUser * pUser,UINT16 selIndex);
	//打印数据
	virtual void print();
	//判断阶段奖励的条件是否满足
	virtual bool isCondMeet(UINT32 curStepVal);
	//增加兑换需要物品
	bool addNeedItems(std::string & itemStr,std::string & resStr);
	//增加兑换活动物品
	bool addGotItems(std::string & itemStr,std::string & resStr);
	//最大兑换次数
	UINT16 getMaxExchangeNum()
	{
		return m_maxExchangeNum;
	}
	//设置最大兑换次数
	void setMaxExchangeNum(UINT16 maxExchangeNum)
	{
		m_maxExchangeNum = maxExchangeNum;
	}
	//填充阶段信息
	void fillStepInfo(MiscProto::stSynExchangeActivityiStep * pSyn);
protected :
	std::vector<std::pair<UINT32,UINT32> > m_needItems;				//需要的道具
	std::vector<std::pair<UINT32,UINT32> > m_gotItems;				//获得道具
	UINT16  m_maxExchangeNum;										//最大兑换次数
};

//单日单笔充值
class OneDayOneRechargeStep : public ActivityStep
{
public :
	//构造函数
	OneDayOneRechargeStep(Activity * pActivity);
	//析构函数
	~OneDayOneRechargeStep();
	//判断阶段奖励的条件是否满足
	virtual bool isCondMeet(UINT32 curStepVal);
};


//单日累计充值
class OneDayAccRechargeStep : public ActivityStep
{
public :
	//构造函数
	OneDayAccRechargeStep(Activity * pActivity);
	//析构函数
	~OneDayAccRechargeStep();
	//判断阶段奖励的条件是否满足
	virtual bool isCondMeet(UINT32 curStepVal);
};

//单日循环充值
class OneDayRecycleRechargeStep : public ActivityStep
{
public :
	//构造函数
	OneDayRecycleRechargeStep(Activity * pActivity);
	//析构函数
	~OneDayRecycleRechargeStep();
	//奖励
	virtual void doReward(GameUser * pUser,UINT16 selIndex);
	//打印数据
	virtual void print();
	//判断阶段奖励的条件是否满足
	virtual bool isCondMeet(UINT32 curStepVal);
	//奖励次数
	void setRewardNum(UINT16 rewardNum)
	{
		m_rewardNum = rewardNum;
	}
	//获得最大奖励次数
	UINT16 getRewardNum()
	{
		return m_rewardNum;
	}
protected :
	UINT16 m_rewardNum;			//奖励次数
};

//多日累计充值
class MutilDayAccRechargeStep : public ActivityStep
{
public :
	//构造函数
	MutilDayAccRechargeStep(Activity * pActivity);
	//析构函数
	~MutilDayAccRechargeStep();
	//判断阶段奖励的条件是否满足
	virtual bool isCondMeet(UINT32 curStepVal);
};

//累计充值天数
class AccRechargeDayStep : public ActivityStep
{
public :
	//构造函数
	AccRechargeDayStep(Activity * pActivity);
	//析构函数
	~AccRechargeDayStep();
	//判断阶段奖励的条件是否满足
	virtual bool isCondMeet(UINT32 curStepVal);
};

//活动
class Activity
{
public :
	//构造函数
	Activity()
	{
		m_id = 0;
		m_startTime = 0;
		m_endTime = 0;
		//m_curRoundID = 0;
	}
	//析构函数
	virtual ~Activity()
	{}
	//奖励
	void doReward(GameUser * pUser,UINT32 step,UINT16 selIndex);
	//获得当前轮次
	/*UINT32 getCurRound()
	{
		return m_curRoundID;
	}*/
	virtual bool init(Json::Value & configJson);
	//初始化开始和结束时间
	bool initActivityTime(const std::string & beginTime,const std::string & endTime);
	//设置活动名字
	void setTitle(const std::string & title)
	{
		m_title = title;
	}
	//获得活动标题
	std::string getTitle()
	{
		return m_title;
	}
	//设置活动描述
	void setDesc(const std::string & desc)
	{
		m_desc = desc;
	}
	//获得活动的描述
	std::string getDesc()
	{
		return m_desc;
	}
	//设置活动的id
	void setID(UINT32 id)
	{
		m_id = id;
	}
	//获得活动的ID
	UINT32 getID()
	{
		return m_id;
	}
	//活动是否开启
	bool isActive();
	//角色创建时间是否满足要求
	virtual bool roleCreateTimeValid(UINT32 createTime)
	{
		return true;
	}
	//活动类型
	virtual MiscProto::eActivityType getActivityType() = 0 ;
	//获得活动的所有阶段
	std::map<UINT32,ActivityStep *> & getAllStep()
	{
		return m_steps;
	}
	//开始时间
	UINT32 getStartTime()
	{
		return m_startTime;
	}
	//结束时间
	UINT32 getEndTime()
	{
		return m_endTime;
	}
protected :
	std::map<UINT32,ActivityStep *> m_steps;		//任务的步骤
	UINT32 m_id;									//活动ID
	std::string m_title;							//活动标题
	std::string m_desc;								//任务描述
	UINT32 m_startTime;								//开始时间
	UINT32 m_endTime;								//结束时间
		//UINT32 m_curRoundID;							//当前轮次
};

//创角时间相关的活动
class CreateRoleRelActivity : public Activity
{
public :
	//构造函数
	CreateRoleRelActivity()
	{
		m_createRoleBeginTime = 0;
		m_createRoleEndTime = 0;
	}
	//析构函数
	~CreateRoleRelActivity(){}
	//初始化
	virtual bool init(Json::Value & configJson);
	//初始化创角色时间
	bool initCreateRoleTime(const std::string & beginTime,const std::string & endTime);
	//角色创建时间是否满足要求
	virtual bool roleCreateTimeValid(UINT32 createTime);
protected :	
	UINT32 m_createRoleBeginTime;					//创建角色开始时间
	UINT32 m_createRoleEndTime;						//创建角色结束时间
};
//消耗类活动
class ConsumeActivity : public Activity
{
public :
	//构造函数
	ConsumeActivity();
	//析构函数
	~ConsumeActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_Consume;
	}
};

//周消耗类活动
class WeeklyConsumeActivity : public Activity
{
public :
	//构造函数
	WeeklyConsumeActivity();
	//析构函数
	~WeeklyConsumeActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_WeeklyConsume;
	}
};

//累积登录奖励
class CumuLoginActivity : public Activity
{
public :
	//构造函数
	CumuLoginActivity();
	//析构函数
	~CumuLoginActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_CumuLogin;
	}
};

//登录间隔
struct stLoginTimeInterval
{
	UINT32 m_beginTime;				//开始时间
	UINT32 m_endTime;				//结束时间

	stLoginTimeInterval()
	{
		m_beginTime = 0;
		m_endTime = 0;
	}
};

//限时登录奖励
class SpecialTimeLoginActivity : public Activity
{
public :
	//构造函数
	SpecialTimeLoginActivity();
	//析构函数
	~SpecialTimeLoginActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_SpecialTimeLogin;
	}
private :
	std::vector<stLoginTimeInterval> m_limitTimes;			//限制时间
};

//兑换活动
class ExchangeActivity : public Activity
{
public :
	//构造函数
	ExchangeActivity();
	//析构函数
	~ExchangeActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_Exchange;
	}
	//是否每日刷新
	bool isDailyRefresh()
	{
		return m_bDayFresh;
	}
private :
	bool m_bDayFresh;		//是否每天刷新
};

//单日单笔充值活动
class OneDayOneRechargeActivity : public CreateRoleRelActivity
{
public :
	//构造函数
	OneDayOneRechargeActivity();
	//析构函数
	~OneDayOneRechargeActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_OneDayOneRecharge;;
	}
};

//单日累计充值
class OneDayAccRechargeActivity : public CreateRoleRelActivity
{
public :
	//构造函数
	OneDayAccRechargeActivity();
	//析构函数
	~OneDayAccRechargeActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_OneDayAccRecharge;;
	}
};

//单日循环充值 
class OneDayRecyleRechargeActivity :  public CreateRoleRelActivity
{
public :
	//构造函数
	OneDayRecyleRechargeActivity();
	//析构函数
	~OneDayRecyleRechargeActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_OneDayRecyleRecharge;;
	}
};

//多日累计充值
class MultiDayRechargeActivity : public CreateRoleRelActivity
{
public :
	//构造函数
	MultiDayRechargeActivity();
	//析构函数
	~MultiDayRechargeActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_MultiAccRecharge;;
	}
};

//累计充值天数
class AccRechargeDayActivity : public CreateRoleRelActivity
{
public :
	//构造函数
	AccRechargeDayActivity();
	//析构函数
	~AccRechargeDayActivity();
	//初始化
	virtual bool init(Json::Value & configJson);
	//返回活动类型
	virtual MiscProto::eActivityType getActivityType()
	{
		return MiscProto::eActivityType_AccRechargeDay;
	}
	//充值额度
	UINT32 getRechargeNum()
	{
		return m_rechargeNum;
	}
	//设置充值额度
	void setRechargeNum(UINT32 rechargeNum)
	{
		m_rechargeNum = rechargeNum;
	}
protected :
	UINT32 m_rechargeNum;
};

//活动管理
class ActivityCfgMgr
{
USE_SINGLETON_NEW(ActivityCfgMgr);
private :
	//构造函数
	ActivityCfgMgr();
public :
	//析构函数
	~ActivityCfgMgr();
	//活动管理
	static ActivityCfgMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();		
	}
	//获得活动配置
	Activity * getActivity(UINT32 id);
	//初始化活动
	bool init(std::string config);
	//获得所有可以进行的活动
	std::map<UINT32,Activity * > & getAllActivities()
	{
		return m_activities;
	}
	void delActivity(UINT32 id);
private :
	std::map<UINT32,Activity * > m_activities;
};

//活动数据
struct ActivityData
{
	UINT32 m_id;						//活动的ID
	UINT32 m_stepVal;					//阶段值
	UINT32 m_stepValExt;				//阶段扩展值
	UINT32 m_refreshTime;				//上一次刷新时间
	std::map<UINT32,UINT16> m_steps;	//已经领取的阶段次数

	ActivityData()
	{
		m_id = 0;
		reset();
	}

	void  reset()
	{
		m_stepVal = 0;
		m_stepValExt = 0;
		m_refreshTime  = 0;
		m_steps.clear();
	}
};

class CActivityDataMgr
{
public :
	 static const UINT32 s_SAVE_VERSION_INFO = 20160321;
public :
	//构造函数
	CActivityDataMgr(GameUser *pOwner);
	//析构函数
	~CActivityDataMgr();
	//保存数据
	UINT32 saveAll(byte * buf,UINT32 needSpace);
	//加载数据
	UINT32 loadAll(byte * buf);
	//检查活动是否刷新
	void checkActivityRefresh();
	//给自己发送所有可以进行的活动
	void sendAllAvaibleActivity();
	//领取某个活动的阶段奖励
	void doActivityStepReward(UINT32 id,UINT32 stepID,UINT16 selIndex);
	//根据活动ID获得活动的进度值
	UINT32 getActivityStepVal(UINT16 type,UINT32 id);
	//增加活动值
	void   incActivityStepVal(UINT16 type,UINT32 id);
	//根据活动ID活动的扩展进度值
	UINT32 getActivityExtStepVal(UINT16 type,UINT32 id);
	//根据活动ID活动增加扩展进度值
	void   incActivityExtStepVal(UINT16 type,UINT32 id);
	//获得每阶段的次数
	UINT16 getStepDoTime(UINT32 id,UINT32 stepID);
	//增加每个阶段次数
	void   addStepDoTime(UINT32 id,UINT32 stepID);
	//重置某个活动
	void  checkActivityReset(UINT16 type,UINT32 id, UINT32 startTime);
	//某个活动的某个阶段是否已经完成
	bool  isStepFinish(UINT32 id,UINT32 stepID);
	//增加活动进度ID
	void addStepID(UINT32 id,UINT32 step);
	//重置某个活动
	//void resetActivity(UINT32 id);
	//用户登录
	void onUserLogin();
	//用户消耗了钻石
	void onUseDiamond(UINT32 num);
	//用户充值了
	void onRecharge(UINT32 num);
	//接受新的活动
	void acceptNewActivity();
    //给活动增加数据
    bool addActivityVal(UINT32 num, MiscProto::eActivityType type);
private :
	GameUser * m_pOwner;
	std::map<UINT32,ActivityData> m_activityDatas;	//活动数据
};
#endif
