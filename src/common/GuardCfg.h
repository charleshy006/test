#ifndef __H_GUARD_CFG_H__
#define __H_GUARD_CFG_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <map>

class GuardCfg
{
	USE_SINGLETON_NEW(GuardCfg);
public :
public :
	//基本信息
	struct stBasicNum
	{
		UINT16 m_guardNum;				//护送次数
		UINT16 m_interceptNum;			//拦截次数
		UINT16 m_helpNum;				//帮助次数
		UINT32 m_interceptCD;			//拦截cd
		UINT16 m_beInterceptNum;		//被拦截次数

		stBasicNum()
		{
			m_guardNum = 0;
			m_interceptNum = 0;
			m_helpNum = 0;
			m_interceptCD = 0;
			m_beInterceptNum = 0;
		}
	};

	//护送信息
	struct stGuardInfo
	{
		UINT16 m_id;						
		UINT16 m_quality;				//品质
		UINT32 m_guardTime;				//护卫时间
		UINT32 m_byInterceptNum;		//被劫持数量	
		UINT32 m_resID1;				//奖励的资源1ID
		UINT32 m_resNum1;				//奖励的资源1数量
		UINT32 m_resID2;				//奖励的资源2ID
		UINT32 m_resNum2;				//奖励的资源2数量
		UINT32 m_resID3;				//奖励的资源3ID
		UINT32 m_resNum3;				//奖励的资源3数量

		stGuardInfo()
		{
			m_id = 0;
			m_quality = 0;
			m_guardTime = 0;
			m_byInterceptNum = 0;
			m_resID1 = 0;
			m_resNum1 = 0;
			m_resID2 = 0;
			m_resNum2 = 0;
			m_resID3 = 0;
			m_resNum3 = 0;
		}
	};

	//拦截奖励
	struct stInterceptReward
	{
		UINT16 m_mixLv;					//最小等级
		UINT16 m_maxLv;					//最大等级
		UINT16 m_rewardPer1;			//奖励1的百分比
		UINT16 m_rewardPer2;			//奖励2的百分比


		stInterceptReward()
		{
			m_mixLv = 0;
			m_maxLv = 0;
			m_rewardPer1 = 0;
			m_rewardPer2 = 0;
		}
	};

	//协助奖励
	struct stHelpReward
	{
		UINT16 m_rewardPer1;			//协助奖励1
		UINT16 m_rewardPer2;			//协助奖励2

		stHelpReward()
		{
			m_rewardPer1 = 0;
			m_rewardPer2 = 0;
		}
	};
	
	//护送消耗配置
	struct stGuardConsume
	{
		UINT32 m_refreshDiamond;		//刷新宝藏需要钻石
		UINT32 m_callDiamond;			//召唤需要钻石
		UINT32 m_inspireDiamond;		//鼓舞需要钻石
		UINT32 m_quickDiamond;			//快速完成需要钻石
		UINT32 m_clearCDDiamond;		//清空cd需要钻石

		stGuardConsume()
		{
			m_refreshDiamond = 0;
			m_callDiamond = 0;
			m_inspireDiamond = 0;
			m_quickDiamond = 0;
			m_clearCDDiamond = 0;
		}
	};
private :
	//构造函数
	GuardCfg();
public :
	//析构函数
	~GuardCfg();
	//获得实例
	static GuardCfg & getSingleton()
	{   
		return THE_SINGLETON::Instance();
	} 
	//加载所有配置
	bool loadCfg();
	//获得协助次数
	UINT16 getHelpNum()
	{
		return m_basicNum.m_helpNum;
	}
	//获得护送次数
	UINT16 getGuardNum()
	{
		return m_basicNum.m_guardNum;
	}
	//获得拦截cd
	UINT32 getInterceptCD()
	{
		return m_basicNum.m_interceptCD; 
	}
	//获得最大拦截次数
	UINT16 getMaxInterceptNum()
	{
		return m_basicNum.m_interceptNum;
	}
	//被拦截次数
	UINT16 getBeInterceptNum()
	{
		return m_basicNum.m_beInterceptNum;
	}
	//获得清除cd需要钻石
	UINT32 getClearCDNeedDiamond()
	{
		return m_guardConsume.m_clearCDDiamond;
	}
	//获得召唤最高品质护送需要钻石
	UINT32 getCallDiamond()
	{
		return m_guardConsume.m_callDiamond;
	}
	//鼓舞士气需要钻石
	UINT32 getInspiredDiamond()
	{
		return m_guardConsume.m_inspireDiamond;
	}
	//刷宝藏品质
	UINT32 getRefreshQualityDiamond()
	{
		return m_guardConsume.m_refreshDiamond;
	}
	//快速完成
	UINT32 getQuickFinishDiamond()
	{
		return m_guardConsume.m_quickDiamond;
	}
	//刷宝藏品质概率
	UINT32 getRefreshQualityRate(UINT16 quality);
	//护送信息
	stGuardInfo * getGuardInfo(UINT16 quality);
	//获得胜利的协助奖励
	stInterceptReward * getInterceptReward(UINT16 lvDiff);
private :
	stBasicNum m_basicNum;								//基本次数
	std::map<UINT16,stGuardInfo > m_guardInfos;			//护送信息
	std::vector<stInterceptReward> m_interceptRewards;	//拦截奖励
	stHelpReward  m_winHelpReward;						//胜利
	stHelpReward  m_loseHelpReward;						//失败
	std::map<UINT16,UINT32> m_quality2Rates;			//不同品质对应的概率
	stGuardConsume  m_guardConsume;						//护送消费
};
#endif
