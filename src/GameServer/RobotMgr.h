#ifndef __ROBOTMGR_H__
#define __ROBOTMGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "Item.h"
#include "HeroInfo.h"
#include <map>
#include <vector>

class GameUser;
struct stRobotInfo
{
	stRobotInfo()
	{
		m_id = 0;
		m_lv = 0;
		m_levelID = 0;
		m_groupID = 0;
		m_pUser = NULL;
	}
	
	UINT32 m_id;					//ID
	UINT16 m_lv;					//等级
	UINT32 m_levelID;				//关卡ID
	UINT8  m_groupID;				//所在的组ID
	std::vector<std::pair<UINT8,UINT8> > m_jobSexs; //职业性别
	GameUser * m_pUser;
};
class GameUser;
class CRobotMgr
{
	USE_SINGLETON_NEW(CRobotMgr);
private :
	CRobotMgr()
	{
	}
public :
	~CRobotMgr()
	{
	
	}

	static CRobotMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	void destroyMe()
	{
		THE_SINGLETON::destroy();
	}
	//初始化玩家符文
	void initFuwen(GameUser *pUser,std::vector<std::pair<UINT32,UINT16> > &fuWenVec,UINT16 offset);
	//加载配置
	bool loadCfg();
	//添加一个机器人
	void addRobot(stRobotInfo * pRobot);
	//获得一个机器人
	stRobotInfo * getRobot(UINT32 robotID);
	//根据关卡获得机器人
	stRobotInfo * getRobotByLevelID(UINT32 levelID,UINT16 num);
	//获得1v1机器人
	stRobotInfo * getOne2OneRobot(UINT32 levelID,UINT16 groupID);
	//使用机器人
	void useRobot(UINT32 charID,stRobotInfo * pRobot);
	//获得机器人
	GameUser * getRobotByID(UINT32 charID);
	//返回机器人
	void returnRobot(UINT32 levelID,UINT32 charID);
	//打印目前机器人使用情况
	void printRobotInfo();
private :
	std::map<UINT32,stRobotInfo *>  m_robotCfgs;
	std::map<UINT32,stRobotInfo *>  m_useRobots;
	std::map<UINT32,std::map<UINT32,std::vector<UINT32> > > m_groupID2RobotIDs;
};

#endif
