#ifndef __H_COMPENSATION_SYS_H__
#define __H_COMPENSATION_SYS_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include <map>
#include <vector>

enum eSysCompensation
{
	eSynCompensation_Type_Day_Refresh = 1,		//每日刷新的补偿	
};

#pragma pack(1)
struct 	stCompensationInfoDB
{
	UINT32 m_id;						//编号
	UINT8 m_type;						//补偿类型
	char m_title[25];					//补偿标题
	char m_content[150];				//补偿内容
	char m_rewards[150];				//补偿道具
	UINT32 m_createTime;				//创建时间

	stCompensationInfoDB()
	{
		m_id = 0;
		m_type = 0;
		bzero(m_title,sizeof(m_title));
		bzero(m_content,sizeof(m_content));
		bzero(m_rewards,sizeof(m_rewards));
		m_createTime = 0;
	}
};
#pragma pack()

struct stCompensationInfo
{
	UINT32 m_id;										//id
	UINT8 m_type;										//类型
	std::string m_title;								//标题
	std::string m_content;								//内容
	std::vector<std::pair<UINT32,UINT32> > m_rewards;	//补偿道具
	UINT32 m_createTime;

	stCompensationInfo()
	{
		m_id = 0;
		m_type = 0;	
		m_createTime = 0;
	}

	void initFromDB(const stCompensationInfoDB & data);
	void fillDBData(stCompensationInfoDB & data);
};

class CCompensationMgr
{
	USE_SINGLETON_NEW(CCompensationMgr);
public :
	static const UINT32 s_MAX_COMPENSATION_NUM = 10;
private :
	//构造函数
	CCompensationMgr()
	{}
public :
	//获得单例
	static CCompensationMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CCompensationMgr();
	//加载全部
	bool load();
	//新增一个全局记录
	void addRecord(stCompensationInfoDB & data);
	//新增记录回调
	void addRecordCB(bool opResult,UINT64 autoID,stCompensationInfoDB data);
	//更新一个全局记录
	void updateRecord(stCompensationInfoDB & data);
	//更新一个全局记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除一条记录
	void removeRecord(UINT32 id);
	//删除记录回调
	void removeRecordCB(bool opResult,UINT64 autoID);
	//检查用户补偿
	void userOnline(Player * pUser);
	//增加一个补偿
	void addCompensation(const stCompensationInfo & info);
	//发送补偿列表
	void sendList2Me(Player * pUser);
	//领取补偿
	void gotCompensation(Player * pUser,UINT32 id);
	//定时器
	void timer(UINT32 cur);
private :
	std::map<UINT32,stCompensationInfo> m_allCompensations;		//所有补偿
	typedef std::map<UINT32,stCompensationInfo>::iterator Compensation_IT;
};
#endif
