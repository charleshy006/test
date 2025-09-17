#ifndef __LEVEL_WFLFARE_MGR_H__
#define __LEVEL_WFLFARE_MGR_H__

#include "TypesDef.h"
#include <string>
#include <map>

struct stActivityTime
{
	UINT32 m_openTime;
	UINT32 m_endTime;

	stActivityTime()
	{
		m_openTime = 0;
		m_endTime = 0;
	}
};

class GameUser;
class CLevelWflfareMgr
{
	static const UINT32 s_LEVEL_WFLFARE_SAVE_VERSION = 20181008;
public:
	//构造函数
	CLevelWflfareMgr(GameUser *pUser);
	//析构函数
	~CLevelWflfareMgr();
	//加载时间数据
	void loadTimeData(const char * buf);
	//保存时间数据
	std::string saveTimeData();
	//加载数据
	void loadData(const char * buf);
	//保存数据
	std::string saveData();
	//发送已领取数据
	void sendHasGotReward();
	//增加已领取数据
	void addInfoToMap(UINT16 level, UINT16 type);
	//增加开始时间与结束时间
	void addOpenTime();
	//活动是否开启
	bool isOpen();
	//检查是否已领取
	bool checkHasGotReward(UINT16 level, UINT16 type);
private:
	GameUser *pOwner;
	stActivityTime m_activityTime;
	std::map<UINT16, std::map<UINT16,UINT16> > m_hasGotReward;
};
#endif
