#ifndef __LUCKYDRAW_CONFIG_H__
#define __LUCKYDRAW_CONFIG_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>

class CLuckyDrawCfg
{
	USE_SINGLETON_NEW(CLuckyDrawCfg);
public:
    enum Draw_Type {
        Draw_Once = 1,
        Draw_Ten = 2,
    };
    struct PointReward {
        UINT8 id;
        UINT32 num;
        UINT32 boxId;
    };

    struct ActivityItem {
        UINT32 id;
        UINT8 activityId;
        UINT32 start;
        UINT32 end;
        UINT32 next;
    };

    struct stDrawItem
    {
		UINT32 m_id;			//道具ID
		UINT32 m_num;			//数量
		UINT32 m_rate;			//概率

		stDrawItem()
		{
			m_id = 0;
			m_num = 0;
			m_rate = 0;
		}
    };
	struct stDrawInfo
	{
        UINT32 m_drawType;
		UINT32 m_moneyType;
		UINT32 m_needMoney;
		UINT32 m_interval;
        UINT32 m_totalRate;

		std::vector<stDrawItem> m_items;	

		stDrawInfo()
		{
			m_drawType = 0;
			m_moneyType = 0;
			m_needMoney = 0;	
			m_interval = 0;
			m_totalRate = 0;
		}
	};

	struct stCertainInfo
	{
		UINT32 m_baseNum;
        UINT32 m_addNum;
        UINT32 m_totalRate;

		std::vector<stDrawItem> m_items;	

		stCertainInfo()
		{
			m_baseNum = 0;
			m_addNum = 0;
			m_totalRate = 0;
		}
    };
    static CLuckyDrawCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	bool loadCfg();
	bool loadRewardCfg();
    UINT32 getActiveId(UINT32 now);
    bool getActivityItem(UINT32 id, ActivityItem & item);
    void getAllActivityItem(std::vector<ActivityItem> & itemVec);
    UINT8 getDrawId(UINT8 id);
    bool getDrawInfo(UINT32 drawId, UINT8 drawType, stDrawInfo & info);
    bool getCertainInfo(UINT32 drawId, stCertainInfo & certain);
    bool isExpireId(UINT32 id, UINT32 now);
    bool getPointReward(UINT8 id, UINT8 pointId, PointReward & reward);
    UINT8 getActivityId(UINT32 id);
private:
    std::map<UINT32, ActivityItem> m_activityMap;
    std::map<UINT8, std::pair<std::vector<PointReward>, UINT8> > m_pointRewardMap;
    std::map<UINT32, std::pair<stDrawInfo, stDrawInfo> > m_drawMap; //抽奖信息, 左边单抽,右边十连抽
    std::map<UINT32, stCertainInfo> m_certainMap;                   //满足一定次数额外奖励
};
#endif
