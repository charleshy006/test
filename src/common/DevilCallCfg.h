#ifndef __H_DevilCall_CFG_H__
#define __H_DevilCall_CFG_H__

#include "TypesDef.h"
#include "Singleton.h"

#include <map>
#include <vector>
#include <tinyxml/tinyxml.h>

class CDevilCallCfg
{
	USE_SINGLETON_NEW(CDevilCallCfg);
public:
    struct stDevilCallInfo      //首杀信息
    {
        UINT32 m_copyMapID;
        std::string name1;
        UINT32 m_charID1;
        std::string name2;
        UINT32 m_charID2;
        std::string name3;
        UINT32 m_charID3;
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
		UINT32 m_moneyType;
		UINT32 m_needMoney;
		UINT32 m_interval;
        UINT32 m_totalRate;

		std::vector<stDrawItem> m_items;	

		stDrawInfo()
		{
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

    enum RewardType
    {
        Boss_Reward = 1,
        Common_Reward = 2,
    };

    struct stDevilReward
    {
        UINT32 m_type;
        UINT32 m_boxId;
        stDevilReward()
        {
            m_type = 0;
            m_boxId = 0;
        }
    };
    struct stDevilInfo
    {
        UINT32 m_bossId;
        UINT32 m_bossDrawId;
        UINT32 m_openCondition;
        
        std::map<UINT32, UINT32> m_rewardMap;
        stDevilInfo()
        {
            m_bossId = 0;
            m_bossDrawId = 0;
            m_openCondition = 0;
        }
    };

public:
	CDevilCallCfg() {}
	//析构函数
	~CDevilCallCfg() {}
    //获得实例
    static CDevilCallCfg& getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//保存
	UINT32 save(byte * buf,UINT32 needSpace);
	//加载
	UINT32 load(byte * buf);
	//加载抽奖配置
	bool loadCfg();
	//加载魔王召唤配置
	bool loadDevil();
    //获取全部魔王召唤信息
    void getAllDevilCall(std::map<UINT32, UINT32> & devilsMap);
    //获取魔王通关奖励
    UINT32 getDevilReward(UINT32 bossId, UINT8 rewardType);
    //获取魔王通关条件
    UINT32 getOpenCondition(UINT32 bossId);
    //获取魔王抽奖id
    UINT32 getDrawId(UINT32 bossId);
    //获取魔王消耗条件
    void getDrawInfo(UINT32 drawId, UINT32 & moneyType, UINT32 & needMoney);
    //获取抽奖信息
    void drawItem(UINT32 drawId, std::map<UINT32, UINT32> & drawMap);
    //获取额外抽奖信息
    void drawCertailItem(UINT32 drawId, UINT32 drawNum, std::map<UINT32, UINT32> & drawMap);
private:
    std::map<UINT32, stDevilInfo> m_devilMap;                   //魔王id对应抽奖表
    std::map<UINT32, stDrawInfo> m_drawMap;                     //抽奖信息
    std::map<UINT32, std::vector<stCertainInfo> > m_certainMap; //满足一定次数额外奖励
};
#endif
