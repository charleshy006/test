#ifndef __H_CAPTURE_TREASURE_H__
#define __H_CAPTURE_TREASURE_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "Misc.pb.h"
#include "Player.h"
#include <vector>
#include <map>
#include <set>

/**
 * 夺宝系统
 */

struct stTreasureCombineCfg
{
	std::vector<UINT32> m_needTreasures;	//需要的资源
	UINT32 m_combineItemID;					//合成的道具ID
	UINT32 m_robotRate;						//机器人概率
	UINT32 m_playerRate;					//玩家概率
	UINT32 m_dropID;						//掉落ID
	UINT32 m_expCoef;						//经验系数
	UINT32 m_moneyCoef;						//金钱系数

	stTreasureCombineCfg()
	{
		m_combineItemID = 0;
		m_robotRate = 0;
		m_playerRate = 0;
		m_dropID = 0;
		m_expCoef = 0;
		m_moneyCoef = 0;
	}
};

#define TREASURE_DATA_LEN  600

#pragma pack(1)
struct stUserTreasureDB
{
	UINT32 m_charID ;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//名字
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT32 m_peaceEndTime;				//和平结束时间
	char   m_data[TREASURE_DATA_LEN];	//资源相关数据
	UINT16 m_photoID;					//头像ID

	stUserTreasureDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_job = 0;
		m_sex = 0;
		m_peaceEndTime = 0;
		bzero(m_data,sizeof(m_data));
		m_photoID = 0;
	}
};
#pragma pack()

class UserTreasureInfo
{
	friend class CCaptureTreasureMgr;
private :
	UINT32 m_charID;						//角色ID
	char   m_name[MAX_NAMESIZE + 1];
	UINT8  m_job;
	UINT8  m_sex;	
	UINT32 m_peaceEndTime;//和平模式结束时间
	std::map<UINT32,UINT32> m_res2Num;
	UINT16 m_photoID;                   //头像ID
public :
	//填充存档数据
	void fillDBData(stUserTreasureDB & dbData);
	//从存档数据初始化
	void initFromDBData(stUserTreasureDB & dbData);
	//是否是合法资源
	bool isValidRes(UINT32 resType);	
	//获得资源数量
	bool checkRes(UINT32 resType,UINT32 num);
	//增加资源
	bool addRes(UINT32 resType,UINT32 num);
	//删除资源
	bool removeRes(UINT32 resType,UINT32 num);
	//返回角色ID
	UINT32 getCharID()
	{
		return m_charID;
	}
	//获得职业
	UINT8 getJob() { return m_job ;}
	//获得性别
	UINT8 getSex() { return m_sex ;}
	//获得头像ID
	UINT16 getPhotoID(){return m_photoID;}
	//设置和平结束时间
	void setPeaceEndTime(UINT32 endTime)
	{
		m_peaceEndTime = endTime;
	}
	//获得和平模式
	UINT32 getPeaceEndTime()
	{
		return m_peaceEndTime;
	}
	//获得名字
	const char * getName()
	{
		return m_name;
	}
	//构造函数
	UserTreasureInfo()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_peaceEndTime = 0;
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};

class CCaptureTreasureMgr
{
	USE_SINGLETON_NEW(CCaptureTreasureMgr)
private :
		CCaptureTreasureMgr();
		static const INT32 s_MAX_CHALLENGE_LIST = 4;
public :
		~CCaptureTreasureMgr();
public :
		static CCaptureTreasureMgr & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}

		//加载数据
		bool loadData();
		//增加一条记录
		void addRecord(const stUserTreasureDB & dbData);
		//增加一条记录返回
		void addRecordCB(bool opResult,UINT64 autoID);
		//更新记录信息
		void updateRecord(const stUserTreasureDB & dbData);
		//更新记录信息回调
		void updateRecordCB(bool opResult,UINT64 autoID);
		//删除记录回调
		void delRecord(UINT32 charID);
		//删除记录回调
		void delRecordCB(bool opResult,UINT64 autoID);
		//增添资源到统计容器
		void addRes2StaticsCnt(UINT32 resType,UINT32 charID,UINT16 level);
		//删除资源统计
		void delRes2StaticsCnt(UINT32 resType,UINT32 charID,UINT16 level);
		//发送玩家的资源
		void sendUserTreasure(Player * pUser);
		//宝藏合成
		void treasureCombine(Player * pUser,UINT32 combineType);
		//挑战玩家
		void captureUserTreasure(Player * pUser,UINT32 charID,UINT32 combineID,UINT32 resType,UINT8 num,bool multiCapture);
		//加载配置
		bool loadConfig();
		//增加某个玩家的资源
		void addUserRes(Player * pUser,UINT32 resID,UINT32 num);
		//发送挑战列表
		void sendChallegeList(Player * pUser,UINT32 resID);
		//设置某个人的和平模式结束时间
		void setPeaceEndTime(Player * pUser,UINT32 endTime);
		//玩家等级变化
		void onUserLvChange(Player * pUser,UINT16 oldLv,UINT16 newLv);
		//同步所有记录
		void synRecord(Player * pUser);
private :
		std::map<UINT32,stTreasureCombineCfg> m_combineCfg;						//合成关系	resID ---> cfg
		std::map<UINT32,UserTreasureInfo> m_charID2ResInfo;						//玩家角色ID资源关系
		typedef std::map<UINT32,std::map<UINT16,std::set<UINT32> > >::iterator Statics_IT;
		typedef std::map<UINT16,std::set<UINT32> >::iterator  Level2Users_IT;
		std::map<UINT32,std::map<UINT16,std::set<UINT32> > > m_resStatics;		//各类资源的统计 <resID,<level,charID> >
};
#endif
