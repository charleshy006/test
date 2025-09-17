#ifndef __H_AICONFIGMGR_H__
#define __H_AICONFIGMGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <vector>
#include <map>

class CAIConfigMgr
{
	USE_SINGLETON_NEW(CAIConfigMgr);
private :
	//析构函数
	CAIConfigMgr();
public :

	struct stDistance
	{
		UINT32 m_minDistance;			//最小距离
		UINT32 m_maxDistance;			//最大距离
		UINT8  m_aiStatus;				//AI状态

		stDistance()
		{
			m_minDistance = 0;
			m_maxDistance = 0;
			m_aiStatus = 0;
		}
	};
	
	struct stAction
	{
		UINT16 m_actionID;				//动作ID
		UINT16 m_skillType;				//技能类型
		UINT8  m_skillState;			//技能状态
		UINT32 m_skillCD;				//技能CD
		UINT32 m_rate;					//释放概率

		stAction()
		{
			m_actionID = 0;
			m_skillType = 0;
			m_skillState = 0;
			m_skillCD = 0;
			m_rate = 0;
		}
	};

	struct stStatus
	{
		stDistance m_distance;					//距离条件
		std::vector<stAction> m_actions;		//行为

		stStatus()
		{
		
		}
	};

	struct stAI
	{
		UINT32 m_id;			//ID
		UINT32 m_speed;			//速度
		std::vector<stStatus> m_statuses;	//状态

		stAI()
		{
			m_id = 0;
			m_speed = 0;
		}
	};
public :
	//构造函数
	~CAIConfigMgr();
	//获得实例
	static CAIConfigMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
	//根据ID获得stAI
	stAI * getAIByID(UINT32 id);
private :
	std::map<UINT32,stAI * > m_allAIs;
};
#endif
