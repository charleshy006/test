#ifndef __MOUNT_MGR_H__
#define __MOUNT_MGR_H__

#include "TypesDef.h"
#include "Misc.pb.h"
#include <map>
#include <set>
#include <vector>

#define EXP_ITEM_ID 121266

struct stMountgrowBase;
class GameUser;

struct stMountGrow
{
	UINT32	m_level;//等级
	UINT32	m_exp;	//当前经验

	stMountGrow()
	{
		m_level	=	0;
		m_exp	=	0;
	}
};

class CMountMgr
{
public:
	static const UINT32 s_MOUNT_SAVE_VERSION = 20170406;
public:
	//构造函数
	CMountMgr(GameUser * pOwner);
	//析构函数
	~CMountMgr();
	//加载数据
	UINT32 loadData(const char * buf);
	//保存数据
	UINT32 saveData(char * buf);
	//玩家上线
	void userOnline();
	//打开窗口
	void openWindow();
	//计算战力
	void calcBattlePower();
	//获得战力
	UINT32 getButtlePower(){return m_battlePower;}
	//计算属性加成
	void addProp();
	//通过等级获得当前星数
	UINT8 getStarByLv(UINT32 lv)
	{
		return lv % 10;
	}
	//通过等级获得当前阶数
	UINT8 getClassByLv(UINT32 lv)
	{
		if(!lv)
			return 1;
		return lv / 10 + 1;
	}
	//判断材料是否充足
	bool checkMaterial(stMountgrowBase * pGrowBase);
	//升级
	void mountLevelUp(MiscProto::stReqMountLevelUp *recvCmd);
	//一键升级
	void akeyLevelUp(MiscProto::stReqMountLevelUp *recvCmd);
	//骑乘
	void followMaster();
	//幻化
	void battleMount(UINT32 mountID); 
	//解锁、进阶
	void mountUnreal(UINT32 mountID);
	//是否骑乘
	bool isRiding(){return m_isRiding;}
	//当前装备的坐骑ID
	UINT32 getBattleMountID(){return m_battleMountID;}
	//计算晶石属性
	void sendStoneProp();
private:
	GameUser * m_pOwner;
	stMountGrow m_mount;					//成长
	std::map<UINT32, UINT32> m_mountMap;	//解锁、进阶
	UINT32 m_battleMountID;					//当前幻化的坐骑ID
	bool m_isRiding;						//是否骑乘
	std::vector<UINT32>  m_battleProps; 	//战斗属性
	UINT32 m_battlePower;					//战力
};

#endif
