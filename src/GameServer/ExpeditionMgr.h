#ifndef __H_EXPEDITION_MGR_H__
#define __H_EXPEDITION_MGR_H__

#include "TypesDef.h"
#include "SortList.pb.h"
#include <map>
#include <set>
#include <vector>

class GameUser;

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
	UINT32 save(byte * buf, UINT32 needSpace);
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
    void getUsedHero(std::vector<stExpeditionHero> ** pHero) {
        *pHero = &m_usedHeroVec;
    }
    //设置已使用随从
    void setUsedHero(std::vector<stExpeditionHero> & heroVec) {
        m_usedHeroVec = heroVec;
    }
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

class CExpeditionMgr
{
public:
	//构造
	CExpeditionMgr(GameUser * pOwner);
	//析构
	~CExpeditionMgr();
	//加载数据
	UINT32 loadAll(byte * buf);
	//保存数据
	UINT32 saveAll(byte * buf, UINT32 needSpace);
    //获取远征副本详情
    void loadItems(SortProto::stRetExpeditionInfo& retCmd);
    //获取远征副本摘要
    void getSummary(SortProto::stRetExpeditionSummary& retCmd);
    //获取远征副本id
    void loadIds(std::vector<UINT16> & idVec);
    //添加已使用的随从
    void addUsedHero(std::set<UINT32> & heroSet);
    //随从是否在使用中
    bool heroInUsed(std::set<UINT32> & heroSet);
    //添加一个副本
    void addExpedition(CExpeditionItem * pItem);
    //获取一个副本
    const CExpeditionItem * getExpedition(UINT16 id);
    //删除一个副本
    void deleteExpedition(UINT16 id);
private:
	GameUser * m_pOwner;
    std::set<UINT32> m_usedHeroSet;                     //远征中的随从列表
    std::map<UINT16, CExpeditionItem*> m_expeditionMap; //远征副本管理列表
};
#endif
