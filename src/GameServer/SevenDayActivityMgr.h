#ifndef __H_SEVENDAY_ACTIVITY_MGR_H__
#define __H_SEVENDAY_ACTIVITY_MGR_H__
#include "TypesDef.h"
#include <map>
#include <set>

class GameUser;
class CSevenDayActivityMgr
{
public :
	static const UINT32 s_SEVENDAY_DATA_VERSION = 20151124;			//版本号
	static const UINT32 s_MAX_REWARD_DAY = 7;						//最大奖励天数
public :
	//构造函数
	CSevenDayActivityMgr(GameUser * pOwner);
	//析构函数
	~CSevenDayActivityMgr();
	//获得保存数据大小
	UINT32 getSaveSize();
	//保存数据
	UINT32 save(byte * buf);
	//加载数据
	UINT32 load(byte * buf);
	//获得目标值
	UINT32 getGoalValue(UINT8 day,UINT8 goalID,UINT32 goalValue,UINT32 extraVal);
	//是否已经领取了奖励
	bool   hasGotReward(UINT8 day,UINT8 goalType,UINT16 id);
	//设置已经领取了奖励
	void   setGotReward(UINT8 day,UINT8 goalType,UINT16 id);
	//发送7天活动信息
	void send7DayActivityInfo(UINT8 day);
	//领取7填活动奖励
	void got7DayActivityReward(UINT8 day,UINT8 goalType,UINT16 id);
	//增加one2one胜利次数
	void addOne2OneWinNum(UINT32 addNum)
	{
		m_one2OneWinNum += addNum;
	}
	//增加黑市刷新次数
	void addHeishiRefreshNum(UINT32 addNum)
	{
		m_heishiRefreshNum += addNum;
	}
	//增加实时pk次数
	void addRealTimePkNum(UINT32 addNum)
	{
		m_realTimePkNum += addNum;	
	}
	//增加抽奖次数
	void addDrawNum(UINT32 addNum)
	{
		m_drawNum += addNum;
	}
	//用户上线
	void userOnline();
	//登录天数
	void addLoginDay(UINT32 day);
private :
	GameUser * m_pOwner;
	std::map<UINT8,std::map<UINT8,std::set<UINT8> > > m_rewardInfos;		//<day,<goalType,set<id> >>
	UINT32 	   m_one2OneWinNum;
	UINT32	   m_heishiRefreshNum;
	UINT32	   m_realTimePkNum;
	UINT32	   m_drawNum;
	std::set<UINT8> m_loginDays;											//登录的天数
};
#endif
