#ifndef __H_REWARDSYS_H__
#define __H_REWARDSYS_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <vector>
#include <set>

class GameUser;
class CRewardSys;
class CRewardBase
{
public :
	//奖励构造
	CRewardBase();
	//奖励析构
	virtual ~CRewardBase();
	//给奖励
	virtual bool doReward(GameUser *pUser,UINT32 val) = 0;
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace) = 0;
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf) = 0;
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode) = 0;	
};

enum
{
	REWARD_TYPE_SIGN  = 1,			//签到奖励
	REWARD_TYPE_OPEN_SVR = 2,		//开服奖励
	REWARD_TYPE_LEVEL = 3,			//升级奖励
	REWARD_TYPE_ONLINE = 4,			//在线奖励
	REWARD_TYPE_INVESTPLAN = 5,		//投资计划
	REWARD_TYPE_EAT_DINNER = 6,		//吃大餐
	//REWARD_TYPE_EVERAYDAY_RECHARGE = 7,//每日充值
	//REWARD_TYPE_CONSUME = 8,		//消费奖励
	//REWARD_TYPE_CUMULATIVE_LOGIN = 9,//累积登录
	REWARD_TYPE_SUMMON = 7,		//召唤

};

class CSignReward : public CRewardBase
{
public :
	struct stSignRewardInfo
	{
		UINT32 	m_boxID;		//箱子ID

		stSignRewardInfo()
		{
			m_boxID  = 0;
		}
	};
public :
	//构造
	CSignReward();
	//析构
	~CSignReward();
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
private :
	std::map<UINT16,std::vector<stSignRewardInfo> >  m_signRewardInfos;
	typedef std::map<UINT16,std::vector<stSignRewardInfo> >::iterator RewardItem_IT;
};


class COpenSvrReward : public CRewardBase
{
public :
	struct stOpenSvrRewardInfo
	{
		UINT16 m_day;							//天数
		std::vector<UINT32> m_boxVec;			//箱子

		stOpenSvrRewardInfo()
		{
			m_day = 0;
		}
	};
public :
	//构造
	COpenSvrReward();
	//析构
	~COpenSvrReward();
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
private :
	std::map<UINT16,stOpenSvrRewardInfo>  m_openSvrRewardInfos;
};

class CLevelReward : public CRewardBase
{
public :
	struct stLevelReward
	{
		UINT32 m_level;  					//等级
		std::vector<UINT32> m_boxIDVec;		//箱子

		stLevelReward()
		{
			m_level = 0;
		}
	};
public :
	//构造函数
	CLevelReward();
	//析构函数
	~CLevelReward();
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
private :
	std::map<UINT32,stLevelReward> m_lvRewardCfg;
};

class COnlineReward : public CRewardBase
{
public :
	struct stOnlineReward
	{
		UINT32 m_id;					//ID
		UINT32 m_onlineTime;			//在线时间
		std::vector<UINT32> m_boxIDVec; //箱子ID

		stOnlineReward()
		{
			m_id = 0;
			m_onlineTime = 0;
		}
	};
public :
	//构造函数
	COnlineReward();
	//析构函数
	~COnlineReward();
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
	//获得到当前ID位置总共时间
	UINT32 getTotalOnline(UINT32 id);
private :
	std::map<UINT32,stOnlineReward> m_onlineRewardCfg;
};

class CInvestPlan : public CRewardBase
{
public :
	struct stInvestReward
	{
		UINT32 m_itemID ;
		UINT32 m_num;

		stInvestReward()
		{
			m_itemID = 0;
			m_num = 0;
		}
	};

	struct stInvestCfg
	{
		UINT32 m_beginTime;		//一天中的开始时间
		UINT32 m_endTime;		//一天中的结束时间
		UINT16 m_needVip;		//需要vip
		UINT8  m_resType;		//资源类型
		UINT32 m_resNum;		//资源数量
		
		std::map<UINT16,stInvestReward > m_rewards;

		stInvestCfg()
		{
			m_beginTime = 0;
			m_endTime = 0;
			m_needVip = 0;
			m_resType = 0;
			m_resNum = 0;
		}
	};
public :
	//构造函数
	CInvestPlan();
	//析构函数
	~CInvestPlan();
	//购买投资计划
	void buyInvestPlan(GameUser * pUser);
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
private :
	stInvestCfg m_cfgs;
};

class CSummonPlan : public CRewardBase
{
public :
	struct stSummonReward
	{
		UINT32 m_itemID;
		UINT32 m_num;

		stSummonReward()
		{
			m_itemID = 0;
			m_num = 0;
		}

	};
	struct stSummonRechargeReward
	{
		UINT32 m_r_itemID;
		UINT32 m_r_num;
		stSummonRechargeReward()
		{
			m_r_itemID = 0;
			m_r_num = 0;
		}

	};


	struct stSummonCfg
	{
		UINT32 m_beginTime;		//一天中的开始时间
		UINT32 m_endTime;		//一天中的结束时间
		UINT16 m_needVip;		//需要vip
		UINT8  m_resType;		//资源类型
		UINT32 m_resNum;		//资源数量
		
		std::map<UINT16,stSummonReward > m_rewards;
		std::map<UINT16,stSummonRechargeReward > m_r_rewards;

		stSummonCfg()
		{
			m_beginTime = 0;
			m_endTime = 0;
			m_needVip = 0;
			m_resType = 0;
			m_resNum = 0;
		}
	};
public :
	//构造函数
	CSummonPlan();
	//析构函数
	~CSummonPlan();
    void sendSummonData(GameUser *pUser,UINT32 summonType);
    bool doSummonReward(GameUser *pUser, UINT32 countid ,UINT32 summontype ,UINT32 rewardtype);

	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
private :
	stSummonCfg m_cfgs;
};

class CEatDinner : public CRewardBase
{
public :
	struct stRewardItem
	{
		UINT32 m_itemID;		//道具ID
		UINT32 m_num;			//道具数量
		UINT32 m_rate;			//概率

		stRewardItem()
		{
			m_itemID = 0;
			m_num = 0;
			m_rate = 0;
		}
	};

	struct EatDinnerCfg
	{
		UINT32 m_sp;											//获得体力
		std::vector<std::pair<UINT32,UINT32> > m_timeLimits;	//时间限制
		std::vector<stRewardItem> 			   m_rewards;		//奖励
			
		EatDinnerCfg()
		{
			m_sp = 0;	
		}
	};
public :
	//构造函数
	CEatDinner();
	//析构函数
	~CEatDinner();
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
    EatDinnerCfg & getCfgs() {
        return m_cfgs;
    }
private :
	EatDinnerCfg m_cfgs;
};

class CEveryDayRechargeReward : public CRewardBase
{
public :
	/*struct stRewardItem
	{
		UINT32 m_itemID;			//道具ID
		UINT32 m_itemNum;			//道具数量

		stRewardItem()
		{
			m_itemID = 0;
			m_itemNum = 0;
		}
	};*/
	struct stRechargeRewardCfg
	{
		UINT32 m_beginTime;							//开始时间
		UINT32 m_endTime;							//结束时间
		std::map<UINT32,UINT32> m_rewards;	//奖励

		stRechargeRewardCfg()
		{
			m_beginTime = 0;
			m_endTime = 0;
		}
	};
public :
	//构造函数
	CEveryDayRechargeReward();
	//析构函数
	~CEveryDayRechargeReward();
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
private :
	stRechargeRewardCfg m_cfgs;
};

/*class CConsumeReward : public CRewardBase
{
public :
	struct stRewardItem
	{
		UINT32 m_itemID;
		UINT32 m_itemNum;

		stRewardItem()
		{
			m_itemID = 0;
			m_itemNum = 0;
		}
	};

	struct stConsumeReward
	{
		UINT32 m_beginTime;
		UINT32 m_endTime;
		std::map<UINT32,UINT32> m_rewards;			//num ---> boxID

		stConsumeReward()
		{
			m_beginTime = 0;
			m_endTime = 0;
		}
	};
public :
	//构造函数
	CConsumeReward();
	//析构函数
	~CConsumeReward();
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
private :
	stConsumeReward m_cfgs;
};*/


/*class CCumulativeLoginReward : public  CRewardBase
{
public :
	struct stReward
	{
		UINT32 m_dayNum;	//天数
		UINT32 m_boxID;		//箱子ID

		stReward()
		{
			m_dayNum = 0;
			m_boxID = 0;
		}
	};

	struct stConfig
	{
		UINT32 m_beginTime;					//开始时间
		UINT32 m_lastTime;					//持续时间
		std::map<UINT32,stReward> m_rewards;//奖励

		stConfig()
		{
			m_beginTime = 0;
			m_lastTime = 0;
		}
	};
public :
	//构造函数
	CCumulativeLoginReward();
	//析构函数
	~CCumulativeLoginReward();
	//奖励
	virtual bool doReward(GameUser *pUser,UINT32 val);
	//保存数据
	virtual UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载数据
	virtual UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	virtual bool loadCfg(TiXmlElement * pNode);
private :
	stConfig m_cfg;
};*/

class CAllRewardMgr
{
	USE_SINGLETON_NEW(CAllRewardMgr);
private :
	//构造
	CAllRewardMgr();
public :
	//析构
	~CAllRewardMgr();
	//获得实例
	static CAllRewardMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//保存
	UINT32 saveData(CRewardSys *pRewardSys,byte * buf,UINT32 needSpace);
	//加载
	UINT32 loadData(CRewardSys *pRewardSys,byte * buf);
	//加载配置
	bool loadCfg();
	//处理奖励
	bool doReward(UINT8 type,GameUser * pUser,UINT32 val);
	//获得当前在线奖励ID,需要等待的时间
	UINT32 getTotalOnline(UINT32 id)
	{
		return ((COnlineReward *)m_subRewards[REWARD_TYPE_ONLINE])->getTotalOnline(id); 
	}
	//购买投资计划
	void buyInvestPlan(GameUser * pUser)
	{
		((CInvestPlan *)m_subRewards[REWARD_TYPE_INVESTPLAN])->buyInvestPlan(pUser);	
	}
    CRewardBase * getRewardByType(UINT8 type);
	//购买召唤计划
	bool doSummonReward(GameUser * pUser, UINT32 countid ,UINT32 summontype ,UINT32 rewardtype)
	{
		return ((CSummonPlan *)m_subRewards[REWARD_TYPE_SUMMON])->doSummonReward(pUser, countid, summontype, rewardtype);	
	}
	//召唤进度拉取
	void sendSummonData(GameUser * pUser, UINT32 summonType)
	{
		((CSummonPlan *)m_subRewards[REWARD_TYPE_SUMMON])->sendSummonData(pUser, summonType); 	
	}
private :
	std::map<UINT8,CRewardBase *> m_subRewards;
};

class CRewardSys
{
	friend class COnlineReward;
	friend class CLevelReward;
	friend class COpenSvrReward;
	friend class CSignReward;
	friend class CInvestPlan;
	friend class CEatDinner;
	//friend class CEveryDayRechargeReward;
	//friend class CConsumeReward;
	friend class CSummonPlan;

public :
	//构造
	CRewardSys(GameUser * pOwner);
	//析构
	~CRewardSys();
	//保存
	UINT32 saveData(byte * buf,UINT32 needSpace);
	//加载
	UINT32 loadData(byte * buf);
	//每秒执行
	void   zeroRefresh();
	//每秒都执行
	void exeEverySec();
	//发送玩家奖励数据
	void userOnline();
	//获得累计登录次数
	//UINT16 getCumuLoginNum()
	//{
	//	return m_cumulativeLogin;
	//}
	//设置累积登录次数
	//void setCumuLoginNum(UINT16 cumuLogin)
	//{
	//	m_cumulativeLogin = cumuLogin;
	//}

	//获得在线奖励经过时间
	UINT32 getOnlineRewardPassTime()
	{
		return m_getOnlineRewardPassTime;
	}
	//设置在线奖励经过时间
	void setOnlineRewardPassTime(UINT32 passTime)
	{
		m_getOnlineRewardPassTime = passTime;
	}
	//检查开服奖励刷新
	void checkSignRefresh(); 
    //清除投资领取等级记录
    void clearInvestRecords() {
        m_getInvestRewardLvs.clear();
    }
private :
	GameUser * m_pOwner;

	UINT16    m_signRewardNum;					//签到奖励次数
	UINT32    m_lastSignRewardTime;				//上一次领取签到奖励时间
	
	UINT16    m_openSvrRewardNum;				//开服奖励领取次数
	UINT32 	  m_lastOpenSvrRewardTime;			//上一次领取开服奖励时间

	std::set<UINT16>  m_hasRewardLv;			//已经领取奖励的等级

	//UINT16    m_getOnlineRewardID;			//领取在线奖励次数
	UINT32	  m_getOnlineRewardTime;			//领取在线奖励时间
	UINT32	  m_getOnlineRewardPassTime;		//领取在线奖励经过时间 
	std::set<UINT8>  m_onlineRewards;			//已经领取的在线奖励

	//std::set<UINT32> m_consumeRewards;			//已经领取的消费奖励
	//std::set<UINT32>  m_getRewardsRecharges;	//已经领取的每日充值奖励
	std::set<UINT16> m_getInvestRewardLvs;		//已经领取投资奖励的等级

	//UINT32	  m_yestodayFirstLoginTime;			//昨天第一次登录时间
	//UINT16    m_cumulativeLogin;				//累积登录
	///std::set<UINT16>    m_cumulativeRewards;	//累积登录奖励
};
#endif
