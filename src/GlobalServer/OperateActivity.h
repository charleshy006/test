#ifndef __OPERATE_ACTIVITY_H__
#define __OPERATE_ACTIVITY_H__

#include "TypesDef.h"
#include "Singleton.h"
#include "Player.h"
#include <list>
#include <map>

#define MAX_OPERATE_ACTIVITY_NUM 1000

struct stOperateActivityInfoDB 
{
	UINT32 m_charID ;					        //角色ID
	char   m_data[MAX_OPERATE_ACTIVITY_NUM];	//名字

	stOperateActivityInfoDB()
	{
		m_charID = 0;
		bzero(m_data,sizeof(m_data));
	}
};
class OperateActivity
{
public:
    //构造函数
    OperateActivity() {}
    //析构函数
    ~OperateActivity() {}
	//加载数据
	UINT32 load(byte * buf);
	//保存数据
	UINT32 save(byte * buf);
    //更新活动数据
    void updateActivity(UINT8 acvitityId, UINT32 condition);
    //更新活动数据
    void updateItem(UINT8 acvitityId, UINT8 itemId, UINT8 status);
    //加载数据
    bool loadItem(UINT8 acvitityId, UINT8 itemId, UINT32 & condition, UINT8 & status);
    //发送活动信息
    void sendActivity(Player * pUser, std::map<UINT8, std::list<UINT8> > & idMap);
private:
    //id, num, id status
    std::map<UINT8, std::pair<UINT32, std::map<UINT8, UINT8> > > m_activityMap;
};

class OperateActivityMgr
{
    USE_SINGLETON_NEW(OperateActivityMgr);
public :
    OperateActivityMgr() { }
    ~OperateActivityMgr();
	static OperateActivityMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载所有数据
	void load();
	//插入新记录
	void insertRecord(const stOperateActivityInfoDB & data);
	//插入记录回调
	void insertRecordCB(bool opResult,UINT64 autoID);
	//更新数据
	void updateRecord(const stOperateActivityInfoDB & data);
	//更新记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
    //获取活动指针
    OperateActivity* getOperateActivity(UINT32 id);
    //检测活动
    void checkOperateActivity(UINT8 activityId, UINT32 id, UINT32 value);
    //更新活动
    void updateActivity(UINT32 id);
private :
    std::map<UINT32, OperateActivity* > m_operateMap;
};
#endif
