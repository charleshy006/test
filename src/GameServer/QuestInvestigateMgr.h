#ifndef __H_QUEST_INVESTIGATE_MGR_H__
#define __H_QUEST_INVESTIGATE_MGR_H__
#include "TypesDef.h"
#include <set>
#include <map>

class GameUser;
class CQuestInvestigateMgr
{
public :
	//构造
	CQuestInvestigateMgr(GameUser * pOwner);
	//析构
	~CQuestInvestigateMgr();
	//保存称号
	UINT32 saveAll(byte * buf,UINT32 needSpace);
	//加载称号
	UINT32 loadAll(byte * buf);
	//用户上线
	void userOnline();
	//问卷调查奖励
	bool giveReward(UINT16 questID,UINT16 level);
private :
	GameUser * m_pOwner;
	std::map<UINT16,std::set<UINT16> >  m_questInvestigateInfos;
};
#endif
