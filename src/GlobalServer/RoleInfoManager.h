#ifndef __ROLEINFOMANAGER_H__
#define __ROLEINFOMANAGER_H__
#include "RoleInfo.h"
#include "TblQueryResult.h"
#include "SqlOperation.h"
#include <ext/hash_map>
#include "Player.h"
#include "Global.pb.h"

struct stReqCreateInfo
{
	UINT16 m_gateID;
	std::string m_ip;
	std::string m_deviceID;
	std::string m_channelID;
	UINT32 m_serverID;	
	
	stReqCreateInfo()
	{
		m_gateID = 0;	
	}
};

struct stReqLoadSnapInfo
{
	std::string m_account;
	UINT16 m_gateID;	
	std::string m_deviceID;
	UINT32 m_opID;
	
	stReqLoadSnapInfo()
	{
		m_gateID = 0;
		m_opID = 0;
	}
};

struct stReqLoadInfo
{
	std::string m_account;
	UINT16 m_gameSvrID;
	UINT16 m_gateSvrID;
	std::string m_ip;
	std::string m_deviceID;
	std::string m_channelID;
	std::string m_netIP;

	stReqLoadInfo()
	{
		m_gameSvrID  = 0;
		m_gateSvrID  = 0;
	}
};

struct stRoleBaseInfo
{
	UINT32 m_charID;
	std::string m_name;
	UINT8 m_job;
	UINT8 m_sex;

	stRoleBaseInfo()
	{
		m_charID = 0;
		m_job = 0;
		m_sex = 0;
	}
};

#pragma pack(1)
struct stRoleBaseInfoDB
{
	UINT32 m_charID;
	char   m_name[MAX_NAMESIZE + 1];
	UINT8  m_job;
	UINT8  m_sex;
	char   m_account[MAX_ACCNAMESIZE + 1];
	char   m_createTime[20];

	stRoleBaseInfoDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_job = 0;
		m_sex = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_createTime,sizeof(m_createTime));
	}
};
#pragma pack()

#pragma pack(1)
struct stGMModifyUserInfoDB
{
	UINT16 m_newbieGuideID;
    UINT32 m_goldCoin;
    UINT32 m_diamond;
    UINT32 m_giveDiamond;
    UINT32 m_vipPoint;

	stGMModifyUserInfoDB()
	{
		m_newbieGuideID = 0;
		m_goldCoin = 0;
		m_diamond = 0;
		m_giveDiamond = 0;
		m_vipPoint = 0;
	}
};
struct stGMGetUserInfoDB
{
	UINT32 m_charID;
	char   m_account[MAX_ACCNAMESIZE + 1];
	char   m_name[MAX_NAMESIZE + 1];
    UINT32 m_goldCoin;
    UINT32 m_diamond;
    UINT32 m_giveDiamond;
    UINT32 m_vipPoint;

	stGMGetUserInfoDB()
	{
		m_charID = 0;
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_goldCoin = 0;
		m_diamond = 0;
		m_giveDiamond = 0;
		m_vipPoint = 0;
	}
};
#pragma pack()

#pragma pack(1)
struct stChangeNameDB
{
	UINT32 m_charID;
	char   m_name[MAX_NAMESIZE + 1];
	UINT16 m_photoID;

	stChangeNameDB()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_photoID = 0;
	}
};

#pragma pack()

class RoleInfoManager 
{
	USE_SINGLETON_NEW(RoleInfoManager);
public:
	//static const UINT32 s_MIN_CHARID = 15000;
public:
	static RoleInfoManager & getSingleton()
	{
		 return THE_SINGLETON::Instance();
	}
	
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}

	RoleInfoManager()
	{
		m_maxCharID = 0;
	}
	~RoleInfoManager()
	{
	
	}
	void loadMaxCharID();
	//创建角色 
	void createRole(stCreateRoleInfo & createData ,stReqCreateInfo reqCreateInfo);
	//创建角色返回
	void createRoleCB(bool opResult,UINT64 autoID,stCreateRoleInfo createData,stReqCreateInfo reqCreateInfo);
	//加载选择界面数据
	void loadSelectRoleInfo(std::string account,UINT16 gateID,std::string deviceID,UINT32 opID, UINT32 serverID);
	//加载选择界面数据返回
	void loadSelectRoleInfoCB(CTblQueryResult* result,stReqLoadSnapInfo loadSnapInfo);
	//删除角色
	bool deleteRole(const UINT32 charid);
	//加载角色数据
	void loadRoleInfo(CQuerySqlOperation * pDelaySql,const char * szWhere);
	//加载角色数据回调
	void loadRoleInfoCB(CTblQueryResult* result,stReqLoadInfo reqData,UINT16 gateID);
	//保存角色数据
	void saveRoleInfo(RoleInfoForSave& roleInfo);
	//保存角色数据回调
	void saveRoleInfoCB(bool opResult,UINT64 autoID,RoleInfo roleInfo);
	//是否存在名字
	void existInTbl(const stCreateRoleInfo &createInfo,stReqCreateInfo reqCreateInfo);
	//是否存在名字回调
	void existInTblCB(CTblQueryResult* result,stCreateRoleInfo createData ,stReqCreateInfo reqCreateInfo);
	//是否存在账号
	void existAccountInTbl(const stCreateRoleInfo &createInfo,stReqCreateInfo reqCreateInfo);
	//是否存在账号回调
	void existAccountInTblCB(CTblQueryResult* result,stCreateRoleInfo createInfo,stReqCreateInfo reqCreateInfo);
	//检查职业跟性别是否已经创建了
	void existJobAndSex(const stCreateRoleInfo &createInfo,UINT16 gateID);
	//检查职业跟性别是否已经创建回调
	void existJobAndSexCB(CTblQueryResult* result,stCreateRoleInfo createInfo,UINT16 gateID);
	//查找充值对象相关信息
	void findRechargeUserInfo(UINT32 charID,UINT32 pointCard);
	//查找充值对象相关信息回调
	void findRechargeUserInfoCB(CTblQueryResult* result,UINT32 pointCard);
	//请求某个玩家的数据
	void loadOneUserDetail(UINT32 charID,UINT32 myCharID);
	//请求某个玩家的数据回调
	void loadOneUserDetailCB(CTblQueryResult* result,UINT32 charID);
	//加载离线玩家挑战
	void loadOfflineUserChallege(UINT32 myCharID,UINT32 otherID,UINT8 pkType,UINT32 rank);
	//载离线玩家挑战回调
	void loadOfflineUserChallegeCB(CTblQueryResult* result,UINT32 myCharID,UINT8 pkType,UINT32 rank);
	//设置GM值
	void setUserGMValue(UINT32 charID,UINT16 gmValue);
	//设置GM值返回
	void setUserGMValueCB(bool opResult,UINT64 autoID);
	//更新玩家离开公会时间
	void updateLeaveGuildTime(UINT32 charID,UINT32 leaveTime);
	//离开公会时间回调
	void updateLeaveGuildTimeCB(bool opResult,UINT64 autoID);
	//查询玩家的bitmask
	void selectRoleBitMaskForModify(UINT32 charID,bool ban);
	//查询玩家的bitMask返回
	void selectRoleBitMaskForModifyCB(CTblQueryResult* result,UINT32 charID,bool ban);
	//更新某个玩家的bitmask
	void updateBitMask(UINT32 charID,UINT32 bitMask);
	//更新某个玩家的bitmask回调
	void updateBitMaskCB(bool opResult,UINT64 autoID);
	//GM修改离线玩家数据
	void gmModifyOfflineUserInfo(UINT32 charID,const stGMModifyUserInfoDB & data);
	//GM修改玩家数据返回
	void gmModifyOfflineUserInfoCB(bool opResult,UINT64 autoID);
	//重置one2one积分
	void clearOne2OneScore();
	//重置one2one积分回调
	void clearOne2OneScoreCB(bool opResult,UINT64 autoID);
	//获得角色基本信息
	stRoleBaseInfo * getRoleBaseInfo(UINT32 charID);
	//上报登录信息
	void  notifyLogin2GiftSvr(std::string account,UINT32 charID);
	//获得第一登录的角色ID
	UINT32 getFirstCreateCharID(std::string account);
	//检查名字是否存在
	void existNameInTbl(const std::string& newName, UINT32 charID);
	//检查名字是否存在回调
	void existNameInTblCB(CTblQueryResult * result, std::string newName, UINT32 charID);
    //是否是新用户
    bool isNewAccount(const std::string & account);
	//改名后同步数据
	void updateNameRecord(const stChangeNameDB & data);
	//改名后同步数据回调
	void updateNameRecordCB(bool opResult,UINT64 autoID);
	//同步记录
	void synRecord(Player * pUser);
	//nft请求某个离线玩家的数据
	void loadNftOfflineUser(UINT32 charID, UINT32 connID);
	//nft请求某个离线玩家的数据回调
	void loadNftOfflineUserCB(CTblQueryResult* result, UINT32 charID, UINT32 connID);
	//nft请求某个离线玩家的数据,然后修改
	void loadNftOfflineUserForChange(UINT32 charID, UINT32 connID, UINT32 itemID, INT32 count);
	//nft请求某个离线玩家的数据回调,然后修改
	void loadNftOfflineUserForChangeCB(CTblQueryResult* result,  UINT32 connID, UINT32 itemID, INT32 count);
private :
	UINT32 m_maxCharID;
	typedef __gnu_cxx::hash_multimap<UINT32, stRoleBaseInfo>::iterator AllRoleBaseInfo_IT;
	__gnu_cxx::hash_map<UINT32, stRoleBaseInfo> m_allRoleBaseInfos;
	typedef __gnu_cxx::hash_map<std::string,UINT32,my_hash<std::string> >::iterator  FirstCreateRoleCharID_IT;
	 __gnu_cxx::hash_map<std::string,UINT32,my_hash<std::string> >  m_firstCreateRoleCharIDs;
};
#endif
