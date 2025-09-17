#ifndef __H_TASK_H__
#define __H_TASK_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "TblTaskBase.h"
#include "Task.pb.h"
#include <vector>
#include <ext/hash_map>
#include <set>

class GameUser;
//任务状态变化的通知
enum NOTIFY_TYPE
{
	NT_ADD = 0, //任务加到玩家身上
	NT_UPDATE,  //玩家身上任务状态更新
	NT_CANCEL,  //任务从玩家身上删除
	NT_REWARD,  //另外任务奖励	
};

//任务目标类型
enum TASKGOAL_TYPE
{
	TT_PROPPERTY = 0, //基于属性
	TT_COUNTER, 	  //基于计数器
};

//任务奖励类型
enum
{
	TR_GOLDCOIN = 1,		//奖励金币
	TR_POINTCARD = 2,		//奖励元宝
	TR_EXP		= 3,		//奖励经验
	TR_ITEM		= 4,		//奖励奖励道具
};

enum TASK_PROP_TYPE
{
	PT_USER_LEVEL = 100,	//用户等级
	PT_USER_PRE_TASK = 101, //前置任务
	PT_USER_FINISH_LV = 102,//完成的关卡
};

//任务类型 
enum TASK_TYPE
{
	MAIN_TASK = 1,			//主线
	BRANCH_TASK = 2,		//支线,也就是赏金
	DAILY_TASK = 3,			//日常
};

//充值的任务
enum eSPECIAL_TASK
{
	eMonth_Card_TaskID = 915000,	//月卡任务
	eQuarter_Card_TaskID = 916000,  //季卡任务
	eYear_Card_TaskID = 917000,		//年卡任务
	eWelfare_Card_TaskID = 918000,	//福利卡任务
};

//属性对
struct stPropPair
{
	UINT32 m_propId;		//属性ID
	UINT32 m_param; 		//属性参数
	UINT32 m_minValue;		//最小值
	UINT32 m_maxValue;		//最大值
};

//接任务条件
struct stAcceptTaskCond
{
	std::vector<stPropPair> m_propPairList;
	stAcceptTaskCond()
	{}
};

//任务完成条件
struct stTaskFinishCond
{
	UINT8            m_goalID;			//目标ID
	TASKGOAL_TYPE 	 m_tt; 				//任务目标类型 
	UINT16 			 m_propID; 			//属性列表id	针对 TT_PROPPERTY类型
	UINT32           m_propParam; 		//属性参数
	std::string		 m_eventKey;		//事件类型      针对  TT_COUNTER 类型
	UINT32 			 m_taskCounter;		//任务计数
};


//任务奖励
struct stTaskReward
{
	UINT8  m_type;			//奖励类型,包金币、元宝、道具、声望
	UINT32 m_id;			//取决于上面
	UINT32 m_num;			//数量

	stTaskReward()
	{
		m_type = 0;
		m_id = 0;
		m_num = 0;
	}
};


//任务保存到数据里的数据
struct stTaskDBInfo
{
	UINT32 m_taskID;						//任务ID
	UINT8  m_ts;							//任务状态
	UINT8  m_subTs;							//任务子状态
	UINT32 m_acceptTime;					//接任务时间
	std::map<UINT8,UINT32> m_taskCounter;	//任务计数器
	void fillSynInfo(TaskProto::stSynTaskInfo & data);


	stTaskDBInfo()
	{
		m_taskID = 0;
		m_ts = TaskProto::TS_INCOMPLETE;
		m_subTs = 1;
		m_acceptTime = 0;
	}

	void reset(GameUser * pOwner);
};


//任务的模板
struct stTaskTemplate
{
	UINT32							m_taskID;					//任务ID
	UINT8							m_taskType;					//任务类型,主线,之线,
	char 							m_name[MAX_NAMESIZE + 1];	//任务名字
	std::vector<UINT32> 			m_preTask;					//前置任务
	std::vector<UINT32> 			m_nextTask;					//后置任务
	stAcceptTaskCond    			m_acceptCond;				//接受任务的条件
	std::vector<stTaskFinishCond>	m_finishCond;				//完成条件
	std::vector<stTaskReward>		m_reward;					//任务奖励
	UINT8							m_needJob;					//需要职业
	UINT32  						m_daiyTaskScore;                //日常任务积分

	stTaskTemplate()
	{
		m_taskID = 0;
		m_taskType = 0;
		bzero(m_name,sizeof(m_name));
		m_needJob = 0;
		m_daiyTaskScore = 0;
	}
	
	//解析数据
	bool parse(stTaskBase * pTaskBase);
	//某个玩家是否可以接任务
	bool canAcceptTask(GameUser * pUser);
	//某个玩家是否可以完成
	bool completeTask(GameUser * pUser,stTaskDBInfo * pTaskInfo,bool notify = true); 
};

//日常任务积分奖励
struct stDailyTaskScoreReward
{
	UINT32 m_id;				//id
	UINT32 m_socre;				//积分
	UINT32 m_boxID;				//奖励boxID

	stDailyTaskScoreReward()
	{
		m_id = 0;
		m_socre = 0;
		m_boxID = 0;
	}
};

//任务模板管理
class CTaskTemplateMgr
{
	USE_SINGLETON_NEW(CTaskTemplateMgr);
private :
	//构造
	CTaskTemplateMgr();
public :
	//析构
	~CTaskTemplateMgr();
	//返回任务模板
	static CTaskTemplateMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载任务配置
	bool loadTaskCfg();
	//根据任务ID获得模板
	stTaskTemplate * getTemplateByID(UINT32 taskID);
	//增加一个任务模板
	bool addTaskTemplate(UINT32 taskID,stTaskTemplate *);
	//是否是日常任务
	bool isDailyTask(UINT32 taskID);
	//返回所有日常任务
	 std::set<UINT32> & getDailyTaskIDs()
	 {
		 return m_dailyTaskCnt;
	 }
	 //获得积分奖励需要的积分
	 stDailyTaskScoreReward *  getScoreRewardInfo(UINT32 id);
	 //是否是充值相关任务
	 bool isRechargeTask(UINT32 taskID);
private :
	__gnu_cxx::hash_map<UINT32,stTaskTemplate *>  m_allTaskCfg;
	typedef __gnu_cxx::hash_map<UINT32,stTaskTemplate *>::iterator Template_IT;
	std::set<UINT32> 		m_dailyTaskCnt;
	std::map<UINT32,stDailyTaskScoreReward>  m_dailyTaskScoreRewards;
};

#pragma pack(1)
struct stSaveTaskInfo
{
	UINT32 m_version;			//版本 
	UINT32 m_size;				//二进制数据大小
	char   m_data[0];	

	stSaveTaskInfo()
	{
		
	}
};
#pragma pack()

class CTaskMgr
{
public :
	static const UINT32 s_TASK_VERSION = 20150112;
public :
	//构造函数
	CTaskMgr(GameUser * pOwner);
	//析构
	~CTaskMgr();
	//保存所有任务
	UINT32 saveAllTask(byte * buf,UINT32 needSpace);
	//加载所有任务
	UINT32  loadAllTask(byte * buf);
	//获取所有已经接得任务,返回已接的数量
	UINT16 	getAllAcceptTask(std::vector<stTaskDBInfo *> & );
	//获得所有已经完成的任务,返回已完成的数量
	UINT16  getAllFinishTask(std::vector<UINT32> & );	
	//获得任务的状态ID
	UINT8   getTaskStatus(UINT32 taskID);
	//获得某个任务的信息
	stTaskDBInfo * getTaskInfo(UINT32 taskID);
	//接任务
	stTaskDBInfo * acceptTask(UINT32 taskID,bool notify = true,bool notifyFinish = true);
	//交任务
	bool		  commitTask(UINT32 taskID);
	//强迫完成
	bool 		forceFinish(UINT32 taskID,bool notify = true);
	//取消任务 
	bool		  cancelTask(UINT32 taskID);
	//任务事件过来了
	void  onGameEvent(UINT32 eventId, UINT32 eventParam[],UINT32 eventParamNum,UINT32 value);
	//属性变化了
	void  onPropChange(UINT16 propID);
	//检查某个任务是否提交了
	bool isTaskReward(UINT32 taskID);
	//某个任务是否已经接了
	bool isTaskAccept(UINT32 taskID);
	//同步所有任务
	void synAllTask();
	//玩家上线
	void userOnline();
	//是否需要刷日常任务
	bool needRefreshDailyTask();
	//计算刷新时间
	void calcRefreshDailyTaskTime();
	//刷新日常任务
	void refreshDailyTask();
	//检查日常任务
	void checkDailyTask();
	//增加日常任务积分       
	void addDailyTaskScore(UINT32 num);
	//扣除日常任何积分
	bool removeDailyTaskScore(UINT32 num);
	//领取积分奖励
	void gotScoreReward(UINT8 id);
	//检查是否可以接充值任务
	bool canAcceptRechargeTask(UINT32 taskID);
	//情况福利开任务
	void clearCardTask();
	//发送日常任务
	void sendAllDailyTask();
	//设置日常任务是否可以刷新
	void setNeedRefreshDailyTask(bool bRefreshDailyTask)
	{
		m_bNeedRefreshDailyTask = bRefreshDailyTask;
	}
	//是否要刷新日志任务
	bool isNeedRefreshDailyTask()
	{
		return m_bNeedRefreshDailyTask;
	}
	//强制添加完成的任务
	void forceAddTaskID(UINT32 taskID)
	{
		m_rewardTaskID.insert(taskID);
	}
private :
	GameUser * m_pOwner;
	std::set<UINT32> m_rewardTaskID;					//已经领取奖励的任务ID
	std::map<UINT32,stTaskDBInfo > m_acceptTask;		//已经接的任务ID
	UINT32   m_refreshDailyTaskTime;					//刷新日常任务时间
	UINT32   m_dailyTaskScore;   						//日常任务积分 
	std::set<UINT8>  m_dailyTaskScoreRewardIDs;			//日常任务奖励ID
	bool	 m_bNeedRefreshDailyTask;
};


#endif
