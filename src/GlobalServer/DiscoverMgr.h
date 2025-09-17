#ifndef __DISCOVER_MGR_H__
#define __DISCOVER_MGR_H__

#include "Singleton.h"
#include "TypesDef.h"
#include "TypesDef.h"
#include "Global.pb.h"
#include "Misc.pb.h"
#include "CommonMicro.h"
#include <ext/hash_map>
#include "Player.h"
#include <map>
#include <set>

enum eTaskState
{
	TASK_STATE_BEGIN = 0,		//开始
	TASK_STATE_NOT_RECEIVE,		//未领取
	TASK_STATE_UNFINISHED,		//领取未完成
	TASK_STATE_FINISH,			//完成
	TASK_STATE_GIVEUP,			//放弃
	TASK_STATE_END,				//结束
};

#pragma pack(1)
struct stDiscoverDB
{
	UINT32		m_charID;		//角色ID
	UINT8		m_mapID;		//地图ID
	UINT8		m_place;		//玩家当前在第几格
	UINT8		m_taskID;		//任务ID
	UINT8		m_state;		//任务状态(1-未领取 2-领取未完成 3-完成 4-放弃)
	UINT8		m_battleNum;	//战斗次数
	UINT32		m_integral;		//积分
	UINT32		m_refreshNum;	//刷新次数
	UINT32		m_playDiceNum;	//掷骰子次数
	UINT32		m_canUseNum;	//可以掷骰子次数
	UINT8		m_hasReceivedID;//是否领取过积分奖励识(二进制表示: 1-已领取，0-未领取)

	stDiscoverDB()
	{
		m_charID		=	0;
		m_mapID			=	0;
		m_place			=	0;
		m_taskID		=	0;
		m_state			=	0;
		m_battleNum		=	0;
		m_integral		=	0;
		m_refreshNum	=	0;
		m_playDiceNum	=	0;
		m_canUseNum		=	0;
		m_hasReceivedID =	0;
	}
};
#pragma pack()

struct stDiscoverInfo
{
	UINT32		m_charID;		//角色ID
	UINT8		m_mapID;		//地图ID
	UINT8		m_place;		//玩家当前在第几格
	UINT8       m_taskID;       //任务ID
	UINT8		m_state;		//任务状态（1-未领取 2-领取未完成 3-完成 4-放弃）
	UINT8		m_battleNum;	//战斗次数
	UINT32		m_integral;		//积分
	UINT32		m_refreshNum;	//刷新次数
	UINT32		m_playDiceNum;	//掷骰子次数
	UINT32      m_canUseNum;    //可以掷骰子次数
	UINT8       m_hasReceivedID;//是否领取过积分奖励标识(二进制表示)

	stDiscoverInfo()
	{
		m_charID		=	0;
		m_mapID			=	0;
		m_place			=	0;
		m_taskID        =   0;
		m_state			=	0;
		m_battleNum		=	0;
		m_integral		=	0;
		m_refreshNum	=	0;
		m_playDiceNum	=	0;
		m_canUseNum		=	0;
		m_hasReceivedID =   0;
	}

	void initFromDB(stDiscoverDB & data);
	void fillDBData(stDiscoverDB & data);
};

class CDiscoverMgr
{
	USE_SINGLETON_NEW(CDiscoverMgr);
private:
	CDiscoverMgr();
public:
	~CDiscoverMgr() {}
	//返回管理实例
	static CDiscoverMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
 	//加载
	void load();
	//定时器
	void timer(UINT32 cur);
	//增加一个记录
	void addRecord(const char * pData);
	//增加记录回调                                    
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一条记录                                    
	void updateRecord(UINT32 key,const char * pData);
	//更新一条记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除一条记录                                
	void removeRecord(UINT32 key);
	//删除一条记录回调                               
	void removeRecordCB(bool opResult,UINT64 autoID);
	//删除所有记录
	void removeAllRecord();
	//删除所有记录回调
	void removeAllRecordCB(bool opResult,UINT64 autoID);
	//发送活动信息
	void sendActivityInfo(Player * pUser);
	//返回某人信息
	stDiscoverInfo * getUserInfo(UINT32 charID);
	//掷骰子
	void playDice(Player *pUser);
	//刷新掷骰子次数
	void refreshNum(Player *pUser);
	void finishRefresh(Global::stDeductMoneyResult *recvCmd);
	//检查任务是否完成
	bool isFinish(Player * pUser);
	//领取任务
	void getTask(Player *pUser);
	//放弃任务
	void giveUpTask(Player *pUser);
	//检查格子类型是不是5或squareInfo->m_rewardBounds6
	bool is5Or6Type(UINT8 type);
	//检查2/3/4关卡任务
	void checkTask(UINT32 charID,UINT32 levelID);
	//检查5/6类型
	void checkFightType(UINT32 charID,UINT8 squareType, UINT8 battleType);
	//领取积分奖励
	void getIntegralReward(Player * pUser, UINT8 id);
	//领取奖励（非积分奖励）
	void giveReward(Player *pUser,MiscProto::stReqGiveReward * recvCmd);
	//领取奖励回调
	void getRewardCB(Global::stGiveRewardResult * recvCmd);
	//红点
	void redPointRuls(Player * pUser);
private:	
	bool m_zeroRefresh;
	__gnu_cxx::hash_map<UINT32, stDiscoverInfo> m_allDiscoverInfos;
	typedef __gnu_cxx::hash_map<UINT32, stDiscoverInfo>::iterator DiscoverInfos_IT;
};
#endif

