#ifndef __H_QUESTTION_MGR_H__
#define __H_QUESTTION_MGR_H__
#include "TypesDef.h"
#include "tinyxml/tinyxml.h"
#include "Singleton.h"
#include <map>
#include <vector>
#include <set>

class CQuestionCfg
{
	USE_SINGLETON_NEW(CQuestionCfg);
public :
	struct stAnswer
	{
		UINT32 m_id;				//答案ID
		std::string m_answerDesc;	//答案描述

		stAnswer()
		{
			m_id = 0;	
		}
	};

	struct stProblem
	{
		UINT32 m_id;
		std::string m_problemDesc;
		std::vector<stAnswer> m_answers;

		stProblem()
		{
			m_id = 0;
		}
	};

	struct stLevel
	{
		std::map<UINT16,stProblem> m_problems;
		UINT16 m_level;								//等级
		UINT32 m_boxID;

		stLevel()
		{
			m_level = 0;
			m_boxID = 0;
		}
	};

	struct stQuestion
	{
		UINT32 m_id;							//问卷调查ID
		std::map<UINT16,stLevel> m_levels;		//等级

		stQuestion()
		{
			m_id = 0;	
		}
	};
public :
	static CQuestionCfg & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
	//获得所有题目
	std::set<std::pair<UINT32,UINT32> > getAllProblems();
	//是这个问卷是否有这个等级
	UINT32 getRewardBoxID(UINT16 questID,UINT16 level);
	//获得问题数量
	UINT16 getProblemNum(UINT16 questID,UINT16 level);
private :
	std::map<UINT32,stQuestion> m_questions;
};
#endif
