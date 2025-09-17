#ifndef __EXPEDITION_H__
#define __EXPEDITION_H__

#include "TypesDef.h"
#include "Singleton.h"
#include "SortList.pb.h"
#include "Global.pb.h"
#include <map>
#include <set>
#include <vector>


#define MAX_EXPEDITION_SIZE 1000
#define MAX_DEVILCALL_SIZE 100

struct stExpeditionInfoDB
{
	UINT32 m_charID ;					//角色ID
	char   m_data[MAX_EXPEDITION_SIZE];	//名字

	stExpeditionInfoDB()
	{
		m_charID = 0;
		bzero(m_data,sizeof(m_data));
	}
};

struct stExpeditionHero
{
    UINT32  m_id;       //随从id
    UINT16  m_lvl;      //随从等级
    UINT32  m_power;    //随从战斗力

    stExpeditionHero()
    {
        m_id = 0;
        m_lvl = 0;
        m_power = 0;
    }
};

class CExpeditionItem
{
public:
    //构造函数
    CExpeditionItem()
    {
        m_expeditionId = 0;
        m_finishTime = 0;
    }
    //析构函数
    ~CExpeditionItem();
	//加载数据
	UINT32 load(byte * buf);
	//保存数据
	UINT32 save(byte * buf);
    //设置远征副本id
    void setId(UINT16 id) {
        m_expeditionId = id;
    }
    //获取远征副本id
    UINT16 getId() {
        return m_expeditionId;
    }
    //设置远征副本完成时间
    void setFinishTime(UINT32 time) {
        m_finishTime = time;
    }
    //获取远征副本完成时间
    UINT32 getFinishTime() {
        return m_finishTime;
    }
    //获取远征副本已使用随从id
    void getUsedHero(std::vector<UINT32> & heroVec);
    //获取远征副本已使用随从详情
    void getUsedHero(std::vector<stExpeditionHero> & heroVec) {
        heroVec = m_usedHeroVec;
    }
    //设置已使用随从
    void setUsedHero(std::vector<stExpeditionHero> & heroVec) {
        m_usedHeroVec = heroVec;
    }
    void setUsedHero(Global::stExpeditionRet * recvCmd);
    //快速完成
    void fastFinish(UINT32 time) {
        m_finishTime = time;
    }
    //获得随从总数量
    UINT32 getUsedHeroNum() {
        return m_usedHeroVec.size();
    }
    //获得随从的总战斗力
    UINT32 getTotalHeroPower();
    //是否有等级超过(包括等于)lvl
    bool greatorLvl(UINT16 lvl);

private:
    UINT16  m_expeditionId;                         //远征副本id
    UINT32  m_finishTime;                           //远征副本完成时间
    std::vector<stExpeditionHero> m_usedHeroVec;    //远征已使用随从
};

class CExpedition
{
public:
	//构造
	CExpedition();
	//析构
	~CExpedition();
	//加载数据
	UINT32 loadAll(byte * buf);
	//保存数据
	UINT32 saveAll(byte * buf);
    //获取远征副本详情
    void loadItems(Global::stExpeditionInfo & retCmd);
    //获取远征副本摘要
    void getSummary(SortProto::stRetExpeditionSummary& retCmd);
    void getSummary(Global::stExpeditionSummary& retCmd);
    //获取远征副本id
    void loadIds(std::vector<UINT16> & idVec);
    //获取已使用随从
    void loadUsedHeros(std::vector<UINT32> & idVec);
    //添加已使用的随从
    void addUsedHero(std::set<UINT32> & heroSet);
    void addUsedHero(Global::stExpeditionRet * recvCmd);
    //随从是否在使用中
    bool heroInUsed(std::set<UINT32> & heroSet);
    //添加一个副本
    void addExpedition(CExpeditionItem * pItem);
    //获取一个副本
    const CExpeditionItem * getExpedition(UINT16 id);
    //删除一个副本
    void deleteExpedition(UINT16 id);
private:
    std::set<UINT32> m_usedHeroSet;                     //远征中的随从列表
    std::map<UINT16, CExpeditionItem*> m_expeditionMap; //远征副本管理列表
};

class ExpeditionMgr
{
    USE_SINGLETON_NEW(ExpeditionMgr);
public :
    ExpeditionMgr() { }
    ~ExpeditionMgr();
	static ExpeditionMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载所有数据
	void load();
	//插入新记录
	void insertRecord(const stExpeditionInfoDB & data);
	//插入记录回调
	void insertRecordCB(bool opResult,UINT64 autoID);
	//更新数据
	void updateRecord(const stExpeditionInfoDB & data);
	//更新记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
    //获取远征
    CExpedition * getExpedition(UINT32 id);
    //新增远程
    void addExpedition(UINT32 id, Global::stExpeditionRet * recvCmd);
    //刪除一個遠征
    void delExpedition(UINT32 id, Global::stExpeditionRewardRet* recvCmd);
    //快速遠征
    void fastExpedition(UINT32 id, Global::stFastExpeditionRet* recvCmd);
    //取消遠征
    void cancelExpedition(UINT32 id, Global::stCancelExpeditionRet* recvCmd);
private :
    std::map<UINT32, CExpedition * > m_expeditionMgrMap;
};

struct stDevilCallInfoDB
{
	UINT32 m_charID ;					//角色ID
	char   m_data[MAX_DEVILCALL_SIZE];	//名字

	stDevilCallInfoDB()
	{
		m_charID = 0;
		bzero(m_data,sizeof(m_data));
	}
};

class CDevilCall
{
public:
    //构造函数
    CDevilCall() {}
    //析构函数
    ~CDevilCall() {}
	//加载数据
	UINT32 load(byte * buf);
	//保存数据
	UINT32 save(byte * buf);
    //获取抽奖次数
    UINT32 getDrawNum(UINT8 id);
    //更新获奖次数
    void updateDrawNum(UINT8 id, UINT32 num);
    //获取奖励状态
    UINT8 getRewardStatus(UINT8 id, UINT8 rewardType);
    //更新奖励状态
    void updateRewardStatus(UINT8 id, UINT8 rewardType, UINT8 rewardStatus);
private:
    std::map<UINT8, UINT32> m_drawNumMap;       //抽奖次数
    std::map<UINT16, UINT8> m_devilRewardMap;    //魔王召唤奖励
};

class DevilCallMgr
{
    USE_SINGLETON_NEW(DevilCallMgr);
public :
    DevilCallMgr() { }
    ~DevilCallMgr();
	static DevilCallMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载所有数据
	void load();
	//插入新记录
	void insertRecord(const stDevilCallInfoDB & data);
	//插入记录回调
	void insertRecordCB(bool opResult,UINT64 autoID);
	//更新数据
	void updateRecord(const stDevilCallInfoDB & data);
	//更新记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
    //獲取大魔王
    CDevilCall* getDevilCall(UINT32 id);
    //新增大魔王
    void addDevilCall(UINT32 id);
    //更新大魔王獎勵
    void updateDevilCallReward(UINT32 id, Global::stDevilCallRewardRet* recvCmd);
    //更新大魔王獎勵
    void updateDevilCallDrawNum(UINT32 id, Global::stDevilCallRet* recvCmd);
private :
    std::map<UINT32, CDevilCall* > m_devilMap;
};
#endif
