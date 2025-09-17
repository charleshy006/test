#ifndef __H_FIRST_KILL_MGR_H__
#define __H_FIRST_KILL_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "CommonMicro.h"
#include "Player.h"
#include "Global.pb.h"
#include "DevilCallCfg.h"
#include <map>
#pragma pack(1)
struct 	stFirstKillInfoDB
{
	UINT32 m_copyMapID;				//副本ID
	UINT32 m_charID1;				//角色1的ID
	UINT8 m_job1;					//角色1的职业
	UINT8 m_sex1;					//角色1的性别
	char  m_name1[MAX_NAMESIZE + 1];//角色1的名字
	UINT16 m_level1;				//角色1的等级
	UINT32 m_battlePower1;			//角色1的战力
	UINT32 m_charID2;               //角色2的ID
	UINT8 m_job2;					//角色2的职业
	UINT8 m_sex2;					//角色2的性别
	char  m_name2[MAX_NAMESIZE + 1];//角色2的名字
	UINT16 m_level2;				//角色2的等级
	UINT32 m_battlePower2;			//角色2的战力
	 UINT32 m_charID3;              //角色3的ID
	UINT8 m_job3;					//角色3的职业
	UINT8 m_sex3;					//角色3的性别
	char  m_name3[MAX_NAMESIZE + 1];//角色3的名字
	UINT16 m_level3;				//角色3的等级
	UINT32 m_battlePower3;			//角色3的战力
	UINT32 m_killTime;				//首杀时间
	UINT16 m_photoID1;				//角色1的头像ID
	UINT16 m_photoID2;				//角色2的头像ID
	UINT16 m_photoID3;				//角色3的头像ID

	stFirstKillInfoDB()
	{
		m_copyMapID = 0;
		m_charID1 = 0;
		m_job1 = 0;
		m_sex1 = 0;
		bzero(m_name1,sizeof(m_name1));
		m_level1 = 0;
		m_battlePower1 = 0;
		m_charID2 = 0;
		m_job2 = 0;
		m_sex2 = 0;
		bzero(m_name2,sizeof(m_name2));
		m_level2 = 0;
		m_battlePower2 = 0;
		m_charID3 = 0;
		m_job3 = 0;
		m_sex3 = 0;
		bzero(m_name3,sizeof(m_name3));
		m_level3 = 0;
		m_battlePower3 = 0;
		m_killTime = 0;
		m_photoID1 = 0;
		m_photoID2 = 0;
		m_photoID3 = 0;
	}
};
#pragma pack()

struct stFirstKillInfo
{
	UINT32 m_copyMapID;				//副本ID
	UINT32 m_charID1;				//角色1的ID
	UINT8 m_job1;					//角色1的职业
	UINT8 m_sex1;					//角色1的性别
	char  m_name1[MAX_NAMESIZE + 1];//角色1的名字
	UINT16 m_level1;				//角色1的等级
	UINT32 m_battlePower1;			//角色1的战力
	UINT32 m_charID2;               //角色2的ID
	UINT8 m_job2;					//角色2的职业
	UINT8 m_sex2;					//角色2的性别
	char  m_name2[MAX_NAMESIZE + 1];//角色2的名字
	UINT16 m_level2;				//角色2的等级
	UINT32 m_battlePower2;			//角色2的战力
	UINT32 m_charID3;               //角色3的ID
	UINT8 m_job3;					//角色3的职业
	UINT8 m_sex3;					//角色3的性别
	char  m_name3[MAX_NAMESIZE + 1];//角色3的名字
	UINT16 m_level3;				//角色3的等级
	UINT32 m_battlePower3;			//角色3的战力
	UINT32 m_killTime;				//首杀时间
	UINT16 m_photoID1;				//角色1的头像ID
	UINT16 m_photoID2;				//角色2的头像ID
	UINT16 m_photoID3;				//角色3的头像ID

	stFirstKillInfo()
	{
		m_copyMapID = 0;
		m_charID1 = 0;
		m_job1 = 0;
		m_sex1 = 0;
		bzero(m_name1,sizeof(m_name1));
		m_level1 = 0;
		m_battlePower1 = 0;
		m_charID2 = 0;
		m_job2 = 0;
		m_sex2 = 0;
		bzero(m_name2,sizeof(m_name2));
		m_level2 = 0;
		m_battlePower2 = 0;
		m_charID3 = 0;
		m_job3 = 0;
		m_sex3 = 0;
		bzero(m_name3,sizeof(m_name3));
		m_level3 = 0;
		m_battlePower3 = 0;
		m_killTime = 0;
		m_photoID1 = 0;
		m_photoID2 = 0;
		m_photoID3 = 0;
	}
	//从数据库初始化
	void initFromDB(const stFirstKillInfoDB & dbData);
	//填充数据库数据
	void fillDBData(stFirstKillInfoDB & dbData);
};

class CFirstKillMgr
{
	USE_SINGLETON_NEW(CFirstKillMgr);
private :
	//构造函数
	CFirstKillMgr()
	{}
public :
	//获得单例
	static CFirstKillMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//析构函数
	~CFirstKillMgr();
	//加载全部
	bool load();
	//发送某个关卡的首杀
	void sendFirstKillInfo(Player * pUser,UINT32 copymapID);
	//组队副本关卡结束
	void teamCopymapFinish(Global::stTeamCopyMapFinish * recvCmd);
	//玩家上线
	void userOnline(Player * pUser);
	//增加一条记录
	void addRecord(stFirstKillInfoDB & data);
	//增加记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
    //是否有通关
    bool gotPass(UINT32 mapId);
    //获取一条记录
    bool getRecord(UINT32 mapId, CDevilCallCfg::stDevilCallInfo & dc);
	//更新一条记录
	void updateRecord(stFirstKillInfoDB & data);
	//更新一条记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//同步所有记录
	void synAllRecords(Player * pUser);
private :
	std::map<UINT32,stFirstKillInfo> m_firstKillInfos;
};
#endif
