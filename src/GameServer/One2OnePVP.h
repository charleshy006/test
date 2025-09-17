#ifndef __H_ONE2ONEPVP_H__
#define __H_ONE2ONEPVP_H__
#include "Singleton.h"
#include "TypesDef.h"
#include "SortList.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "SortList.pb.h"
#include "HistoryRankMgr.h"
#include "GameUser.h"

#define ONE2ONE_PVP_REWARD_INFO_LEN    40000
bool PkWithRobot(Global::stOne2OnePkWithRobot * recvCmd);
bool PkWithOther(Global::stOne2OnePVPEnterCopyMap * recvCmd);

class Player;

//PubGameServer只负责匹配及战斗，其他逻辑处理在globalServer
class COne2OnePVPMgr
{
	USE_SINGLETON_NEW(COne2OnePVPMgr);
public :
	static const UINT8 s_SEND_SORTITEM_MAX = 10;
	static const UINT32 s_MAX_REWARD_NUM = 10000;
	static const UINT16 s_MAX_REWARD_DAY = 7;
	static const UINT32 s_CHECK_MATCH_INTERVAL = 2;
	static const UINT16 s_MIN_WAIT_SEX = 3;

	//匹配规则相关
	struct stMatchCfg
	{
		UINT16 m_id;				//ID
		UINT32 m_startPoint;		//积分上线
		UINT32 m_endPoint;			//积分下线
		UINT16 m_matchType;			//匹配类型
		UINT32 m_matchTimeState1;	//匹配时间1
		UINT32 m_matchTimeState2;	//匹配时间2
		UINT16 m_matchRangeID;		//匹配ID

		std::map<UINT32,UINT32> m_waitCharIDs;

		stMatchCfg()
		{
			m_id = 0;
			m_startPoint = 0;
			m_endPoint = 0;
			m_matchType = 0;
			m_matchTimeState1 = 0;
			m_matchTimeState2 = 0;
			m_matchRangeID = 0;
		}
	};
	
	enum {
		eMatch_Type1 = 1,
		eMatch_Type2 = 2,
		eMatch_Type3 = 3,
		eMatch_Type4 = 4,
	};

	//匹配的机器人组
	struct stMatchRobotGroup
	{
		UINT16 m_minLv;
		UINT16 m_maxLv;
		UINT16 m_groupID;

		stMatchRobotGroup()
		{
			m_minLv = 0;
			m_maxLv = 0;
			m_groupID = 0;
		}
	};

	//每日奖励
	struct stEverydayReward
	{
		UINT16 m_beginRank;			//开始名次
		UINT16 m_endRank;			//结束名次
		UINT32 m_itemID;			//奖励ID
		UINT32 m_num;				//数量

		stEverydayReward()
		{
			m_beginRank = 0;
			m_endRank = 0;
			m_itemID = 0;
			m_num = 0;
		}
	};

	//每周奖励
	struct stWeekReward
	{
		UINT16 m_beginRank;			//开始名次
		UINT16 m_endRank;			//结束名次
		UINT32 m_itemID1;			//奖励ID
		UINT32 m_num1;				//数量
		UINT32 m_itemID2;
		UINT32 m_num2;
		UINT32 m_itemID3;
		UINT32 m_num3;

		stWeekReward()
		{
			m_beginRank = 0;
			m_endRank = 0;
			m_itemID1 = 0;
			m_num1 = 0;
			m_itemID2 = 0;
			m_num2 = 0;
			m_itemID3 = 0;
			m_num3 = 0;
		}
	};
private :
	COne2OnePVPMgr() 
	{
		m_lastCheckTime = 0;
	}
public :
	//析构函数
	~COne2OnePVPMgr() {}
	//获得排名管理实例
	static COne2OnePVPMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
	
	//定时回调
	void timer(UINT32 cur);

	//增加等待匹配的玩家
	void addMatchUser(GameUser * pUser);
	//玩家取消匹配
	void cancelMatch(GameUser * pUser);
	
	//新的匹配检查
	void checkMatchNew();
	//获得匹配机器人组
	UINT16 getRobotGroupID(UINT16 matchType,UINT16 lv);
	//重置每个阶段玩家
	void checkMatchReset();
private :

	std::map<UINT16,std::vector<stMatchRobotGroup> > m_matchRobotInfos;		//匹配机器人信息

	std::map<UINT16,stMatchCfg> m_waitMatchUsers;								//匹配规则
	std::vector<stEverydayReward> m_everydayRewards;						//每天奖励
	std::vector<stWeekReward>  m_weekRewards;								//每周奖励

	UINT32  m_lastCheckTime;												//上次检查时间

	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >		m_waitState1Users;			//第一阶段的玩家<matchType,vector<pair<charID,matchID>> >
	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >         m_waitState2Users;			//第二阶段的玩家
	std::map<UINT16,std::vector<std::pair<UINT32,UINT16> > >       m_waitState3Users;			//第三阶段的玩家
};
#endif
