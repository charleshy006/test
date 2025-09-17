#ifndef __ROLEINFOMANAGER_H__
#define __ROLEINFOMANAGER_H__
#include "TblQueryResult.h"
#include "SqlOperation.h"
#include "json/reader.h"
#include "json/writer.h"
#include "CommonMicro.h"
#include "RoleInfo.h"

#define MAX_WRITE_COUNT 30
#pragma pack(1) 
struct stRoleListInfo 
{
	stRoleListInfo()
	{
		bzero(this, sizeof(stRoleListInfo));
	}
	UINT32	  m_charID;						//角色ID
	char 	  m_account[MAX_ACCNAMESIZE + 1];//账号
	char      m_name[MAX_NAMESIZE + 1];     //名字
	UINT16    m_level;                      //角色等级
	UINT32    m_goldCoin;                   //金币
	UINT32    m_giveDiamonod;				//钻石
	UINT32    m_diamond;					//钻石
	UINT32	  m_rechargeNum;				//充值累计
	UINT32    m_giveRechargeNum;			//赠送的累积充值
	char      m_lastLoginTime[20];				//最近一次登录时间
	char      m_createTime[20];             //创建时间
	UINT16    m_gm;							//gm类型 
};

struct stRoleAccInfo {
    stRoleAccInfo() {
		bzero(this, sizeof(stRoleAccInfo));
    }
	UINT32	  m_charID;						//角色ID
	char      m_name[MAX_NAMESIZE + 1];     //名字
};

struct stRoleDetailInfo 
{
	stRoleDetailInfo()
	{
		bzero(this, sizeof(stRoleDetailInfo));
	}
	UINT32	  m_charID;						//角色ID
	char 	  m_account[MAX_ACCNAMESIZE + 1];//账号
	char      m_name[MAX_NAMESIZE + 1];     //名字
	UINT16    m_level;                      //角色等级
	UINT32    m_rechargeNum;                //充值累计
	UINT32    m_giveRechargeNum;			//赠送的累积充值
	UINT32    m_sp;                         //体力
	UINT32    m_honour;                     //荣誉
	UINT32    m_reputaion;                  //声望
	UINT32    m_lilian;                     //历练
	UINT32    m_braveVal;                   //勇敢值
	UINT32    m_peakCertificate;            //巅峰值
	UINT16    m_newbieGuideID;				//新手引导ID
	char   	  m_lastLoginTime[20];			//最近一次登录时间
	char      m_createTime[20];             //创建时间
	UINT32    m_opID;					    //运营商ID
	UINT32    m_GoldCoin;					//金币
	UINT32    m_Diamond;					//钻石
	UINT32    m_giveDiamond;				//赠送的钻石
};

struct stRoleRechargeInfo
{
	char 	  m_account[MAX_ACCNAMESIZE + 1];//账号
	char      m_name[MAX_NAMESIZE + 1];     //名字
	UINT32    m_rechargeNum;                //充值累计
	
	stRoleRechargeInfo()
	{
		bzero(m_account,sizeof(m_account));
		bzero(m_name,sizeof(m_name));
		m_rechargeNum = 0;
	}
};

struct stAdvRoleInfo
{
	char   m_name[MAX_NAMESIZE + 1];     //名字

	stAdvRoleInfo()
	{
		bzero(m_name,sizeof(m_name));
	}
};

struct stLevelStatics
{
	UINT16 m_level;
	UINT32 m_num;

	stLevelStatics()
	{
		m_level = 0;
		m_num = 0;
	}
};

struct stVipLvStatics
{
	UINT16 m_level;
	UINT16 m_vipLv;

	stVipLvStatics()
	{
		m_level = 0;
		m_vipLv = 0;
	}
};

#pragma pack()
 

struct stOrderInfo 
{
    bool isExtend;      //是否用于热云统计
    std::string data;   //json 数据
    std::string order;  //订单号
    UINT32 itemId;      //item id
    stOrderInfo () {
        isExtend = false;
        itemId = 0;
    }
};

class RoleInfoManager 
{
	USE_SINGLETON_NEW(RoleInfoManager);
public:
	static RoleInfoManager & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}
	
	//构造函数
	RoleInfoManager();
	//析构函数
	~RoleInfoManager()
	{}
	
	//获得某一页的数据
	std::string getOnePageUser(Json::Value & searchCond);
	//获得玩家的详细信息
	std::string getUserDetailInfo(Json::Value & searchCond);
	//获得修改字段
	std::string getEditUserInfo(Json::Value & searchCond);
	//获得充值排名
	std::string rechargeRank(Json::Value & searchCond);
	//获得职业统计
	std::string getJobStatics(Json::Value & searchCond);
	//获得等级统计
	std::string getLevelInfo(Json::Value & searchCond);
	//新的获得等级统计接口
	std::string getLevelInfo2(Json::Value & searchCond);
	//获取广告用户
	std::string getAdvUsers(Json::Value & searchCond);
	//获得某个表的记录数
	UINT32 getRecordCount(const char * szTableName,const char * szWhere);
	//获得所有角色数据
	void  getAllRoleFullInfo();
    //获得指定角色数据
    void getRole(std::string & account, std::string & name, UINT32 & charId);
	//定时器
	void timer(UINT32 cur);
	//保存order id, item id
    bool addOrderId(UINT32 charId, const stOrderInfo & orderInfo);
	//获取订单详细信息
    void getOrderInfo(UINT32 charId, const std::string & orderId, stOrderInfo & orderInfo);
private :
	RoleInfo *m_pAllRoleInfo;
	UINT32    m_roleNum;
	UINT32    m_writeRoleIndex;
	bool	  m_refresh;
	std::string m_createRoleStr;
	std::string m_curRoleInfoTblName;
    std::map<UINT32, std::map<std::string, stOrderInfo> > m_id2OrderItemMap;
    //std::map<UINT32, std::map<std::string, UINT32> > m_id2OrderItemMap;
    UINT32                  m_writeRoleVecIndex;
    std::vector<RoleInfoBase *> m_allRoleVec;
};
#endif
