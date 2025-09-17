#ifndef __GLORYFASHION_MGR_H__
#define __GLORYFASHION_MGR_H__

#include "Singleton.h"
#include "TypesDef.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "SortList.h"
#include <ext/hash_map>
#include <map>
#include <set>


#pragma pack(1)
struct stGloryfashionRankDB
{
	UINT32 m_charID;					//角色ID
	UINT32 m_time;                      //购买时间
	char   m_name[MAX_NAMESIZE + 1];    //角色名字
	UINT8  m_job;                       //职业
	UINT8  m_sex;                       //性别

	stGloryfashionRankDB()
	{
		m_charID = 0;
		m_time = 0;
		bzero(m_name,sizeof(m_name));
		m_job = 0;
		m_sex = 0;
	}
};
#pragma pack()

struct stGloryfashionRankInfo
{
	UINT32 m_charID;    				//角色ID  
	UINT32 m_time;						//购买时间
	char   m_name[MAX_NAMESIZE + 1];    //角色名字
	UINT8  m_job;                       //职业
	UINT8  m_sex;                       //性别

	bool operator < (const stGloryfashionRankInfo & one) const
	{
		return one.m_time > m_time;
	}

	UINT32 getKey() const
	{
		return m_charID;
	}

	UINT32 getCmpValue() const
	{
		return m_time;
	}
	
	void fillDBData(stGloryfashionRankDB & data)
	{
		data.m_charID = m_charID;
		data.m_time = m_time;
		strncpy(data.m_name,m_name,sizeof(m_name));
		data.m_job = m_job;
		data.m_sex = m_sex;
	}

};

#pragma pack(1)
struct stGloryfashionActivityDB
{
	UINT32 m_id ;
	UINT32 m_activityID;
	UINT8  m_isStart;
	
	stGloryfashionActivityDB()
	{
		m_id = 0;
		m_activityID = 0;
		m_isStart = 0;
	}
};
#pragma pack(0)

struct stGloryfashionActivityInfo
{
	UINT32 m_id ;
	UINT32 m_activityID;
	UINT8  m_isStart;

	stGloryfashionActivityInfo()
	{
		m_id = 0;
		m_activityID = 0;
		m_isStart = 0;
	}

	void initFromDB(stGloryfashionActivityDB & data);
	void fillDBData(stGloryfashionActivityDB & data);
};

typedef CSortList<stGloryfashionRankInfo> CGloryfashionSortList;

class Player;
class CGloryfashionRankMgr
{
	USE_SINGLETON_NEW(CGloryfashionRankMgr);
public :
	static const UINT8 s_SEND_SORTITEM_MAX = 50; 
private :
	//析构函数
	CGloryfashionRankMgr() {}
public:
	//构造函数
	~CGloryfashionRankMgr() {}
	//返回管理实例
	static CGloryfashionRankMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	void load(); 
	//增加一个记录                                    
	void addRecord(const char * pData);               
	//增加记录回调                                    
	void addRecordCB(bool opResult,UINT64 autoID);    
	//更新一条记录                                    
	void updateRecord(UINT32 key,const char * pData); 
	//更新一条记录回调                                
	void updateRecordCB(bool opResult,UINT64 autoID); 
	//删除一条记录                                   
	void removeRecord(UINT32 key);                   
	//删除一条记录回调                               
	void removeRecordCB(bool opResult,UINT64 autoID);
	//更新荣耀时装购买排名                                          
	void updateGloryfashionTime(Player * pUser,UINT32 time);      
	//删除所有购买的排名                                    
	void removeAllGloryfashionRecord();                             
	//删除所有购买排名的回调                                
	void removeAllGloryfashionRecordCB(bool opResult,UINT64 autoID);
	//发送购买排行                                          
	void sendGloryfashionSortList(Player *pUser,UINT32 beginRank);  
	/*
	//获得我的排名
	UINT16 getMyRank(UINT32 charID)
	{
		return m_drawHeroSortList.getMySortRank(charID);
	}
	*/
 	//保存活动信息                                         
 	void updateActivityInfo();                             
 	//保存活动回调                                         
 	void updateActivityInfoCB(bool opResult,UINT64 autoID);
	//定时器                                               
 	void timer(UINT32 cur);                                
 	//活动开始                                             
 	void activityStart();
	//活动结束
	void activityEnd();                                    
	//获得活动ID                                           
	UINT32 getActivityID()
	{
		return m_activityInfo.m_activityID;
	}
	//同步记录
	void synRecord(Player * pUser);
private:
	CGloryfashionSortList			m_gloryfashionSortList;
	stGloryfashionActivityInfo		m_activityInfo;
};
#endif
