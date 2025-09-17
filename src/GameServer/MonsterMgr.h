#ifndef __MONSTER_MGR_H__
#define __MONSTER_MGR_H__

#include "TypesDef.h"
#include "SortList.pb.h"
#include <map>
#include <set>
#include <vector>

class GameUser;
class CMonsterMgr
{
public:
	struct stBossInfo
	{
		UINT32 m_bossLv;		//boss等级
		UINT32 m_bossHp;		//boss剩余血量
		UINT32 m_time;			//上一次的回复时间
		UINT32 m_attackTime;	//剩余攻击次数
		
		stBossInfo()
		{
			m_bossLv  = 0;
			m_bossHp  = 0;
			m_time    = 0;
			m_attackTime = 0;
		}
	};

	const static UINT32 s_MONSTER_DB_VERSION = 20170329;
public:
	//构造函数
	CMonsterMgr(GameUser * pOwner);
	//西沟函数
	~CMonsterMgr();
	//加载数据
	UINT32 loadData(const char * buf);
	//保存数据
	UINT32 saveData(char * buf);
	//请求打开窗口
	void openWindow();
	//请求攻击小怪兽
	void attackMonster();
	//请求一键攻击
	void aKeyAttackMonster();
	//请求购买次数
	void buyAttackTime();
	//定时器
	void timer(UINT32 cur);
private:
	stBossInfo m_bossInfo;
	GameUser * m_pOwner;
	bool tag;
};

#endif
