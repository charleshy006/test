#ifndef __GAME_STATICS_MGR_H__
#define __GAME_STATICS_MGR_H__
#include "TblQueryResult.h"
#include "SqlOperation.h"
#include "json/reader.h"
#include "json/writer.h"
#include "CommonMicro.h"

#pragma pack(1) 

//在线数据
struct stOnlineInfo
{
	UINT32 m_year;				//年
	UINT32 m_month;				//月
	UINT32 m_day;				//日
	UINT32 m_timestamp;			//时间戳
	UINT32 m_num;               //人数

	stOnlineInfo()
	{
		m_year = 0;
		m_month = 0;
		m_day = 0;
		m_num = 0;
		m_timestamp = 0;
	}
};

//充值统计
struct stRechargeStatics
{
	UINT32 m_users;					//新增用户
	UINT32 m_times;					//充值次数
	UINT32 m_accounts;				//付费人数
	UINT32 m_cash;					//充值现金
	UINT32 m_dau;					//登录账号数
	UINT32 m_pcu;					//最该在线人数
	char   m_date[25];				//时间

	stRechargeStatics()
	{
		m_users = 0;
		m_times = 0;
		m_accounts = 0;
		m_cash = 0;
		m_dau = 0;
		m_pcu = 0;
		bzero(m_date,sizeof(m_date));
	}
};

//留存统计
struct stHaveInfoStatics
{
	char    m_newUsers[250];			//新增用户
	char   m_day_2[250];				//2日留存
	char   m_day_3[250];				//3日留存
	char   m_day_4[250];				//4日留存
	char   m_day_5[250];				//5日留存
	char   m_day_6[250];				//6日留存
	char   m_day_7[250];				//7日留存
	char   m_day_14[250];				//14日留存
	char   m_day_30[250];				//30日留存
	char   m_date[25];					//日期

	stHaveInfoStatics()
	{
		bzero(m_newUsers,sizeof(m_newUsers));
		bzero(m_day_2,sizeof(m_day_2));
		bzero(m_day_3,sizeof(m_day_3));
		bzero(m_day_4,sizeof(m_day_4));
		bzero(m_day_4,sizeof(m_day_4));
		bzero(m_day_5,sizeof(m_day_5));
		bzero(m_day_6,sizeof(m_day_6));
		bzero(m_day_7,sizeof(m_day_7));
		bzero(m_day_14,sizeof(m_day_14));
		bzero(m_day_30,sizeof(m_day_30));
		bzero(m_date,sizeof(m_date));
	}
};

struct stServerInfoStatics
{
	UINT32 m_regDevice;			//注册设备数量
	UINT32 m_loginDevice;		//登录设备数
	UINT32 m_regAccount;		//注册账号数
	UINT32 m_loginAccount;		//登录账号数
	UINT32 m_oldLoginAccount;	//老账号登录数
	UINT32 m_newLoginAccount;	//新账号登录数
	UINT32 m_newRegAccount;		//新注册账号数量
	UINT32 m_roleNum;			//创建角色数量
	UINT32 m_createRoleRate;	//创角率
	UINT32 m_acu;				//平局在线
	UINT32 m_pcu;				//最高在线
	UINT32 m_dt;				//在线时间
	UINT32 m_rechargeNum;		//充值账号数
	UINT32 m_accRechargeNum;	//累计充值账号
	UINT32 m_newRechargeNum;	//新增充值账号
	UINT32 m_rechargeTimes;		//充值次数
	UINT32 m_rechargeAmount;	//充值总额
	UINT32 m_rechargeRate;		//付费率
	UINT32 m_arpu;				//arpu值
	UINT32 m_arppu;				//arppu值
	UINT32 m_hasDiamond;		//付费玩家用户钻石
	UINT32 m_avHasDiamond;		//付费玩家平均钻石
	UINT32 m_rechargeUsers;		//充值保有人数
	UINT32 m_opID;				//运营编号
	char   m_date[25];          //日期
	
	stServerInfoStatics()
	{
		m_regDevice = 0;
		m_loginDevice = 0;
		m_regAccount = 0;
		m_loginAccount = 0;
		m_oldLoginAccount = 0;
		m_newLoginAccount = 0;
		m_newRegAccount = 0;
		m_roleNum = 0;
		m_createRoleRate = 0;
		m_acu = 0;
		m_pcu = 0;
		m_dt = 0;
		m_rechargeNum = 0;
		m_accRechargeNum = 0;
		m_newRechargeNum = 0;
		m_rechargeTimes = 0;
		m_rechargeAmount = 0;
		m_rechargeRate = 0;
		m_arpu = 0;
		m_arppu = 0;
		m_hasDiamond = 0;
		m_avHasDiamond = 0;
		m_rechargeUsers = 0;
		m_opID = 0;
		bzero(m_date,sizeof(m_date));
	}
};

struct stTotalRechargeNumDB
{
	UINT32 m_id;
	UINT32 m_num;

	stTotalRechargeNumDB()
	{
		m_id = 0;
		m_num = 0;
	}
};


struct stActivityQueryInfo
{
	UINT32 m_stepID;				//阶段ID
	UINT32 m_playerTime;			//玩家次数			
	UINT32 m_playerCount;			//玩家次数
	char   m_name[50];              //活动名字
	char   m_desc[50];              //活动阶段

	stActivityQueryInfo()
	{
		m_stepID = 0;
		m_playerTime = 0;
		m_playerCount = 0;
		bzero(m_name,sizeof(m_name));
		bzero(m_desc,sizeof(m_desc));
	}
};
#pragma pack()
 

class CGameStaticsMgr 
{
	USE_SINGLETON_NEW(CGameStaticsMgr);
public :
	static const UINT32 s_REFRESH_HAVA_INTERVAL = 900;
	static const UINT32 s_MAX_WRITE_SERVER_STATIC_CACHE = 4;	
public:
	static CGameStaticsMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	
	static void destroyMe()
	{
		THE_SINGLETON::destroy();
	}
	
	//构造函数
	CGameStaticsMgr()
	{
		m_lastRefreshTime = time(NULL);
		m_totalRechargeNum = 0;
		m_totalRechargeID = 0;
		m_refreshServerStaticNum = 0;
	}
	//析构函数
	~CGameStaticsMgr()
	{}
	//加载
	void load();	
	//返回在线数据
	std::string getOnlineInfo(Json::Value & searchCond);
	//返回商城统计
	std::string getMallBuyInfo(Json::Value & searchCond);
	//查询充值统计数据
	std::string getRechargeInfo(Json::Value & searchCond);
	//返回留存查询
	std::string getHavaInfo(Json::Value & searchCond);
	//返回服务器信息
	std::string getServerInfo(Json::Value & searchCond);
	//获得活动日志
	std::string getActivityLog(Json::Value & searchCond);
	//刷留存
	void refreshHavaInfo();
	//刷新服务器信息统计
	void refreshServerInfoStatics1();
	//定时器
	void timer(UINT32 cur);
	//返回金币消耗查询
	std::string getExchangeList(Json::Value & searchCond);
	//获得记录数
	UINT32 getRecordCount(const char * szTableName,const char * szWhere,const char * szCountField = NULL);
	//获得某个ID的统计记录
	void   getRecordCountByOPID(const char * szCountField,const char * szTableName,const char * szWhere,const char * szGroupBy,std::map<UINT32,UINT32> & kvPair);
	//读取每个渠道的留存
	void initHavaData(std::map<UINT32,UINT32> & channel2Num,std::string inStr);
	//综合查询
	std::string getCommonQuery(Json::Value & searchCond);
	//获得每小时增加的人数
	std::string getIncAccountPerHour(Json::Value & searchCond2,Json::Value & searchCond1);
private :
	UINT32 m_lastRefreshTime;
	UINT32 m_totalRechargeNum;
	UINT32 m_totalRechargeID;
	UINT32 m_refreshServerStaticNum;
};
#endif
