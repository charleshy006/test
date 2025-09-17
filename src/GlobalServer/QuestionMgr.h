#ifndef __H_QUEST_MGR_H__
#define __H_QUEST_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include <map>
#pragma pack(1)
struct 	stProblemInfoDB
{
	UINT32 m_id;
	UINT32 m_questID;				//问卷ID
	UINT32 m_problemID;				//题目ID
	UINT32 m_answerA;				//选项A的人数
	UINT32 m_answerB;				//选项B的人数
	UINT32 m_answerC;				//选项C的人数
	UINT32 m_answerD;				//选项D的人数
	UINT32 m_answerE;				//选项E的人数
	UINT32 m_answerF;				//选项A的人数
	UINT32 m_answerG;				//选项B的人数
	UINT32 m_answerH;				//选项C的人数
	UINT32 m_answerI;				//选项D的人数
	UINT32 m_answerJ;				//选项E的人数

	stProblemInfoDB()
	{
		m_id = 0;
		m_questID = 0;
		m_problemID = 0;
		m_answerA = 0;
		m_answerB = 0;
		m_answerC = 0;
		m_answerD = 0;
		m_answerE = 0;
		m_answerF = 0;
		m_answerG = 0;
		m_answerH = 0;
		m_answerI = 0;
		m_answerJ = 0;
	}
};
#pragma pack()

struct stProblemInfo
{
	UINT32 m_id;
	UINT32 m_questID;				//问卷ID
	UINT32 m_problemID;				//题目ID
	UINT32 m_answerA;				//选项A的人数
	UINT32 m_answerB;				//选项B的人数
	UINT32 m_answerC;				//选项C的人数
	UINT32 m_answerD;				//选项D的人数
	UINT32 m_answerE;				//选项E的人数
	UINT32 m_answerF;				//选项A的人数
	UINT32 m_answerG;				//选项B的人数
	UINT32 m_answerH;				//选项C的人数
	UINT32 m_answerI;				//选项D的人数
	UINT32 m_answerJ;				//选项E的人数

	stProblemInfo()
	{
		m_id = 0; 
		m_questID = 0;
		m_problemID = 0;
		m_answerA = 0;
		m_answerB = 0;
		m_answerC = 0;
		m_answerD = 0;
		m_answerE = 0;
		m_answerF = 0;
		m_answerG = 0;
		m_answerH = 0;
		m_answerI = 0;
		m_answerJ = 0;
	}
	
	UINT32 getKey()
	{
		return m_questID << 16 | m_problemID;
	}

	//从数据库初始化
	void initFromDB(const stProblemInfoDB & data);
	//填充数据库内容
	void fillDBData(stProblemInfoDB & data);
};

class QuestionMgr
{
	USE_SINGLETON_NEW(QuestionMgr);
private :
	//构造函数
	QuestionMgr()
	{}
public :
	//获得单例
	static QuestionMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~QuestionMgr();
	//加载全部
	bool load();
	//新增一个全局记录
	void addRecord(const stProblemInfoDB & data);
	//新增记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新一个全局记录
	void updateRecord(const stProblemInfoDB & data);
	//更新一个全局记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//更新问卷统计结果
	void updateProblemInfo(UINT32 questID,bool a,bool b,bool c,bool d,bool e ,bool f,bool g,bool h,bool i,bool j);
private :
	std::map<UINT32,stProblemInfo> m_problemInfos;
};
#endif
