#ifndef __H_HISTORYRANK_MGR_H__
#define __H_HISTORYRANK_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "Global.pb.h"
#include <map>

struct stHistoryRankInfo
{
	UINT16 m_moduleID;
	std::map<UINT32,UINT16> m_charID2Rank;

	stHistoryRankInfo()
	{
		m_moduleID = 0;
	}
};

class GameUser;
class CHistoryRankMgr
{
	USE_SINGLETON_NEW(CHistoryRankMgr);
private :
	CHistoryRankMgr();
public :
	//析构函数
	~CHistoryRankMgr();
	//获得实例
	static CHistoryRankMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//模块ID与称号的获得类型ID的转换
	UINT16 getGotTitleTypeByModuleID(UINT16 moduleID);
	//取消原来的人称号
	void cancelOldUserTitle();
	//实例
	void initHistoryRankInfo(Global::stSynRankModuleInfo * recvCmd);
	//检查某个玩家是否获得称号
	void checkGotTitle(GameUser * pUser);	
private :
	std::map<UINT16,stHistoryRankInfo> m_moduleRanks;
};
#endif
