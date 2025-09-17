#include "QuestionCfg.h"
#include "KVPair.h"
#include "Log4cxxHelp.h"

bool CQuestionCfg::loadCfg()
{
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/question.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[问卷调查配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("root");
	if (!pRoot){
		Athena::logger->error("[问卷调查配置]读取配置文件找不到节点<root>");
		return false;
	}  

	INT32 iVal = 0;
	const char * szVal = NULL;

	TiXmlElement * pQuestionNode = pRoot->FirstChildElement("question");
	while (pQuestionNode){
		stQuestion question;
		if (pQuestionNode->Attribute("id",&iVal)){
			question.m_id = iVal;
		}

		TiXmlElement * pLevelNode = pQuestionNode->FirstChildElement("level");
		while (pLevelNode){
			stLevel levelInfo;
			if (pLevelNode->Attribute("openlevel",&iVal)){
				levelInfo.m_level = iVal;
			}
			
			if (pLevelNode->Attribute("rewardboxid",&iVal)){
				levelInfo.m_boxID = iVal;
			}

			TiXmlElement * pProblemNode	= pLevelNode->FirstChildElement("problem");
			while (pProblemNode){
				stProblem problem;
				if (pProblemNode->Attribute("id",&iVal)){
					problem.m_id = iVal;
				}
				
				if ((szVal = pProblemNode->Attribute("problemdesc"))){
					problem.m_problemDesc = szVal;
				}

				TiXmlElement * pOptionNode = pProblemNode->FirstChildElement("options");
				while (pOptionNode){
					stAnswer answer;
					if (pOptionNode->Attribute("id",&iVal)){
						answer.m_id = iVal;
					}

					if ((szVal = pOptionNode->Attribute("optionsdesc"))){
						answer.m_answerDesc = szVal;
					}
					problem.m_answers.push_back(answer);
					pOptionNode = pOptionNode->NextSiblingElement("options");
				}
				pProblemNode = pProblemNode->NextSiblingElement("problem");
				levelInfo.m_problems[problem.m_id] = problem;
			}
			question.m_levels[levelInfo.m_level] = levelInfo;
			pLevelNode = pLevelNode->NextSiblingElement("level");
		}
		m_questions[question.m_id] = question;
		pQuestionNode = pQuestionNode->NextSiblingElement("question");
	}

#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印问卷调查:");
	std::map<UINT32,stQuestion>::iterator it = m_questions.begin();
	for (;it != m_questions.end();++it){
		stQuestion & quest = it->second;
		Athena::logger->trace("questID:%d,各个等级的问卷题目为:",quest.m_id);

		std::map<UINT16,stLevel>::iterator subIt = quest.m_levels.begin();
		for (;subIt != quest.m_levels.end();++subIt){
			stLevel & lvInfo = subIt->second;
			Athena::logger->trace("等级:%d,奖励为:%d,题目为:",lvInfo.m_level,lvInfo.m_boxID);
			
			std::map<UINT16,stProblem>::iterator thdIt = lvInfo.m_problems.begin();
			for (;thdIt != lvInfo.m_problems.end();++thdIt){
				stProblem & problem = thdIt->second;
				Athena::logger->trace("题目id:%d,内容:%s",problem.m_id,problem.m_problemDesc.c_str());
				for (UINT16 i = 0;i < problem.m_answers.size();++i){
					stAnswer & answer = problem.m_answers[i];
					Athena::logger->trace("答案id:%d,内容:%s",answer.m_id,answer.m_answerDesc.c_str());
				}
			}
		}
	}
#endif
	return true;	
}

std::set<std::pair<UINT32,UINT32> > CQuestionCfg::getAllProblems()
{
	std::set<std::pair<UINT32,UINT32> > result;
	std::map<UINT32,stQuestion>::iterator it = m_questions.begin();
	for (;it != m_questions.end();++it){
		stQuestion & quest = it->second;
		std::map<UINT16,stLevel>::iterator subIt = quest.m_levels.begin();
		for (;subIt != quest.m_levels.end();++subIt){
			std::map<UINT16,stProblem>::iterator thdIt = subIt->second.m_problems.begin();
			
			for (;thdIt != subIt->second.m_problems.end();++thdIt){
				stProblem & problem = thdIt->second;
				result.insert(std::make_pair(problem.m_id,quest.m_id));
			}
		}
	}
	return result;
}

UINT32 CQuestionCfg::getRewardBoxID(UINT16 questID,UINT16 level)
{
	std::map<UINT32,stQuestion>::iterator it = m_questions.find(questID);

	if (it == m_questions.end()){
		return 0;
	}

	std::map<UINT16,stLevel>::iterator subIt = it->second.m_levels.find(level);
	if (subIt == it->second.m_levels.end()){
		return 0;
	}
	return subIt->second.m_boxID;
}

UINT16 CQuestionCfg::getProblemNum(UINT16 questID,UINT16 level)
{
	std::map<UINT32,stQuestion>::iterator it = m_questions.find(questID);
	
	if (it == m_questions.end()){
		return 0;
	}

	std::map<UINT16,stLevel>::iterator subIt = it->second.m_levels.find(level);
	if (subIt == it->second.m_levels.end()){
		return 0;
	}
	return subIt->second.m_problems.size();
}
