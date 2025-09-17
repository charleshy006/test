#ifndef __H_RECHARGE_ACTIVITY_MGR_H__
#define __H_RECHARGE_ACTIVITY_MGR_H__
#include "TypesDef.h"
#include <map>

class GameUser;
class CRechargeActivityMgr
{
public :
	static const UINT32 s_SAVE_VERSION_INFO = 20151212;
public :
	struct stRechargeActivityDB
	{
		UINT32 m_rechargeNum;
		UINT32 m_refreshTime;
		std::set<UINT8> m_rewardSteps;

		stRechargeActivityDB()
		{
			m_rechargeNum = 0;
			m_refreshTime = 0;
		}
	};
public :
	//构造函数
	CRechargeActivityMgr(GameUser * pOwner);
	//析构函数
	~CRechargeActivityMgr();
	//保存商店数据
	UINT32 save(byte * buf,UINT32 needSpace);
	//加载商店数据
	UINT32 load(byte * buf);
	//发送所有活动
	void sendActivityList();
	//请求某个活动奖励
	void doReward(UINT32 id,UINT8 step);
	//玩家充值了
	void onRecharge(UINT32 rechargeNum);
	//玩家上线
	void refreshData();
private :
	std::map<UINT16,stRechargeActivityDB > m_activities;
	GameUser * m_pOwner;
};
#endif
