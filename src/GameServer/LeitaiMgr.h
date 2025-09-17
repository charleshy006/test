#ifndef __H_LEITAI_MGR_H__
#define __H_LEITAI_MGR_H__
#include "TypesDef.h"
#include "HeroInfo.h"
#include <map>

class GameUser;
class CLeitaiMgr
{
public :
	static const UINT32 s_MAX_ROUND_NUM = 10;
	static const UINT32 s_RESET_USE_DIAMOND = 10;
	static const UINT32 s_REFRESH_BUFF_NEED_DIAMOND = 20;
	static const UINT32 s_REFRESH_BUFF_INTERVAL = 1200;
public :
	//构造函数
	CLeitaiMgr(GameUser * pOwner);	
	//析构函数
	~CLeitaiMgr();
	//保存
	UINT32 save(byte * buf,UINT32 needSpace);
	//加载
	UINT32 load(byte * buf);
	//刷新
	void checkRefresh();
	//发送擂台数据
	void sendLeitaiInfo();
	//设置玩家当前血量
	void setUserHP(UINT32 hp)
	{
		m_playerCurHP = hp;
	}
	//返回玩家当前血量
	UINT32 getUserHP() {  return m_playerCurHP ; }
	//设置宠物的血量
	void setHeroHP(UINT32 heroID,UINT32 curHP);
	INT32 getHeroHP(UINT32 heroID);
	//设置pk模式
	void setPKMode(UINT8 mode)
	{
		m_pkMode = mode;
	}
	//获得pk模式
	UINT8 getPKMode()
	{
		return m_pkMode;
	}
	//获得pk轮次
	UINT8 getPKRound();
	//重置pk信息
	void resetInfo();
	//获得擂台位置
	UINT8 getHeroSrcPos(UINT32 heroID);
	//交换移动宠物出战位置
	void moveHeroPos(UINT32 heroID,UINT8 dstPos);
	//取消出战
	void cancelGoOut(UINT8 pos);
	//设置挑战者ID
	void setChallengeCharID(UINT32 charID)
	{
		m_challengeCharID = charID;
	}
	//返回挑战者ID
	UINT32 getChallengeCharID()
	{
		return m_challengeCharID;
	}
	//刷新擂台buff
	void refreshBuff();
	//设置buffID
	void setBuffID(UINT16 buffID)
	{
		m_buffID = buffID;	
	}
	UINT32 getBuffID(){return m_buffID;}
	//检查刷新时间
	void checkRefreshBuffTime();
	//发送刷新buff剩余时间
	void sendRefreshBuffTime();
	//
	UINT32 getHeroIDByPos(UINT32 pos);
private :
	GameUser * m_pOwner;
	UINT8	   m_pkMode;				//模式
	UINT32     m_refreshTime;			//刷新时间
	UINT32 	   m_playerCurHP;			//玩家当前血量
	std::map<UINT32,UINT32> m_heroHP;	//宠物血量
	UINT32 	   m_challengeCharID;		//挑战对象ID
	UINT32	   m_battleHeroIDs[HeroInfoMgr::s_MAX_BATTLE_HERO_NUM + 1];
	UINT32	   m_lastRefreshBuffTime;	//最近一次刷buff时间
	UINT16	   m_buffID;				//刷出来的buff
	UINT16     m_heroBuffID;			//宠物的buff
};
#endif
