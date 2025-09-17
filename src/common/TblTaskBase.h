#ifndef __TBL_TASKBASE_H__
#define __TBL_TASKBASE_H__

#include <TypesDef.h>
#include <string>
#include "Log4cxxHelp.h"
#include "UtilFunction.h"

struct TaskRawData
{
	INT32 getID() const
	{
		return m_field_1;
	}

	UINT32  m_field_1;						//任务ID
	char    m_field_2[33];					//名字
	UINT32  m_field_3;                      //任务类型
	char  	m_field_4[50];					//解锁条件
	UINT32  m_field_5;                 		//后置任务
	UINT32  m_field_6;						//前置任务
	char	m_field_7[50];					//完成条件	
	UINT32  m_field_8;						//金币奖励
	UINT32  m_field_9;                      //经验奖励
	UINT32  m_field_10;                     //钻石奖励
	char	m_field_11[100];				//道具奖励
	UINT32	m_field_12;						//积分奖励
}__attribute__((packed));

struct stTaskRewardRaw
{
	UINT32 m_id;			//道具ID
	UINT32 m_num;			//数量
	UINT32 m_raiseLv;		//强化等级

	stTaskRewardRaw()
	{
		m_id = 0;
		m_num = 0;
		m_raiseLv = 0;
	}
};

struct stTaskBase
{
	UINT32 m_id;							//关卡ID
	char   m_name[MAX_NAMESIZE + 1];		//名字
	UINT8   m_taskType;                     //任务类型
	UINT32  m_openLv;						//解锁等级
	UINT32  m_unlockTaskID;					//解锁任务
	UINT32  m_finishLevelID;				//解锁条件 完成关卡条件
	UINT32  m_nextTaskID;					//后置任务ID
	UINT32  m_preTaskID;					//前置任务ID
	UINT16  m_finishCond;					//完成条件
	UINT32  m_finishPara1;					//完成参数1
	UINT32  m_finishPara2;                   //完成参数2
	UINT32  m_awardGoldCoin;				//金币奖励
	UINT32  m_awardExp;						//经验奖励
	UINT32  m_awardDiamond;					//奖励钻石
	std::vector<stTaskRewardRaw> m_awardItem;	//奖励道具
	UINT32  m_daiyTaskScore;				//日常任务积分


	void fill(const TaskRawData & data)
	{
		m_id = data.m_field_1;
		strncpy(m_name,data.m_field_2,sizeof(m_name));
		m_taskType = data.m_field_3;
		std::vector<std::string> strVec;
		g_stringtok(strVec,data.m_field_4,"-");
		m_unlockTaskID = 0;
		m_openLv = 0;
		m_finishLevelID = 0;
		if (strVec.size() >= 2){
			UINT16 type = (UINT16)atoi(strVec[0].c_str());
			if (type == 1){
				m_unlockTaskID = (UINT32)atoi(strVec[1].c_str());
			}
			else if (type == 2){
				m_openLv = (UINT32)atoi(strVec[1].c_str());
			}
			else if (type == 3){
				m_finishLevelID = (UINT32)atoi(strVec[1].c_str());
			}
		}

		m_nextTaskID = data.m_field_5;
		m_preTaskID = data.m_field_6;

		strVec.clear();
		g_stringtok(strVec,data.m_field_7,"-");
		if (strVec.size() >= 3){
			m_finishCond = (UINT32)atoi(strVec[0].c_str());
			m_finishPara1 = (UINT32)atoi(strVec[1].c_str());
			m_finishPara2 = (UINT32)atoi(strVec[2].c_str());
		}	

		m_awardGoldCoin = data.m_field_8;
		m_awardExp =  data.m_field_9;
		m_awardDiamond = data.m_field_10;
		strVec.clear();
		g_stringtok(strVec,data.m_field_11,";");
		for (UINT8 i = 0;i < strVec.size(); ++i){
			std::vector<std::string> subStrVec;
			g_stringtok(subStrVec,strVec[i].c_str(),"-");
			if (subStrVec.size() >= 2){
				stTaskRewardRaw item;
				item.m_id = (UINT32)atoi(subStrVec[0].c_str());
				item.m_num = (UINT32)atoi(subStrVec[1].c_str());
				//item.m_raiseLv = (UINT16)atoi(subStrVec[2].c_str());
				m_awardItem.push_back(item);	
			}
		}
		m_daiyTaskScore = data.m_field_12;
#ifdef _HDZ_DEBUG
		Athena::logger->trace("加载任务配置表id=%d,name=%s,m_openLv=%d,m_finishCond=%d,m_finishPara1=%d,m_finishPara2=%d,m_unlockTaskID=%d",m_id,m_name,m_openLv,m_finishCond,m_finishPara1,m_finishPara2,m_unlockTaskID);
#endif
	}
};

#endif
