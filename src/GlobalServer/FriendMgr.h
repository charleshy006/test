#ifndef __H_FRIENDMGR_H__
#define __H_FRIENDMGR_H__
#include "TypesDef.h"
#include "ElementMgr.h"
#include "TblQueryResult.h"
#include "Friend.pb.h"
#include "Singleton.h"
#include <ext/hash_map>
#include <list>
#include <set>
#include <map>
#include <string>

#pragma pack(1)
struct stFriendDBInfo
{
	UINT32 m_charID;						//角色ID
	UINT32 m_otherID;						//对方的角色ID
	char   m_otherName[MAX_NAMESIZE + 1];	//对方的角色名字
	UINT8  m_job;							//职业
	UINT8  m_sex;							//性别
	UINT16 m_photoID;						//头像ID

	stFriendDBInfo()
	{
		m_charID = 0;
		m_otherID = 0;
		bzero(m_otherName,sizeof(m_otherName));
		m_job = 0;
		m_sex = 0;
		m_photoID = 0;
	}
};
#pragma pack()

#pragma pack(1)
struct stLastLoginTime
{
	UINT32		m_charID;					//角色ID
	char		m_lastLoginTime[20];		//最近一次登录时间

	stLastLoginTime()
	{
		m_charID = 0;
		bzero(m_lastLoginTime, sizeof(m_lastLoginTime));
	}
};
#pragma pack()

//角色最后一次上线时间管理
class CUserLastLoginTimeMgr
{
	USE_SINGLETON_NEW(CUserLastLoginTimeMgr);
private:
	CUserLastLoginTimeMgr(){}
public:
	~CUserLastLoginTimeMgr(){}

	//返回管理实例
	static CUserLastLoginTimeMgr & getSingleton()
	{
	    return THE_SINGLETON::Instance();
	}
	//加载数据
	void load();
	//添加时间
	void setLastTime(UINT32 id,std::string time){
		m_lastTimeMap[id] = time;
	}
	//通过ID 获得时间
	std::string getLastTime(UINT32 id);
private:
	//first 角色ID， second：标准时间，非时间戳
	std::map<UINT32, std::string> m_lastTimeMap;
};

//好友信息
class CFriendInfo   : public Element
{
public :
	CFriendInfo()
	{
		m_ownerID = 0;
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_job = 0;
		m_sex = 0;
		bzero(m_lastLoginTime, sizeof(m_lastLoginTime));
	}
	
	//填充DB数据
	void fillDBData(stFriendDBInfo & data);
	//初始化来自db的数据
	void initFromDBData(const stFriendDBInfo & data);
	//填充发送往客户端的数据
	void fillSynData(FriendProto::stSynFriendInfo & data);	
public :
	UINT32 m_ownerID;					//属主名字
	UINT32 m_charID;					//角色ID
	char   m_name[MAX_NAMESIZE + 1];	//角色名字
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT16 m_photoID;					//头像ID
	char m_lastLoginTime[20];			//近一次登录时间
};

class Player;
class CFriendMgr  : public ElementMgr
{
public :
	static const UINT32 s_MAX_FRIEND_NUM = 50;
public :
	//构造函数
	CFriendMgr(Player * pOwner);
	//析构函数
	~CFriendMgr();
	//增加一个好友
	bool addFriend(CFriendInfo * pFriend);
	//删除一个好友
	bool removeFriend(CFriendInfo * pFriend);
	//获得一个好友信息
	CFriendInfo * getFriend(UINT32 charID) { return (CFriendInfo *)ElementMgr::getElementByID(charID);}
	//根据名字获得好友 
	CFriendInfo * getFriend(char * name)   { return (CFriendInfo *)ElementMgr::getElementByName(name); } 
	//新增一个好友
	void addNewFriend(const stFriendDBInfo & dbData);
	//用户上线
	void userOnline();
	//用户下线
	void userOffline();	
	//好友数量
	UINT16 getFriendNum()
	{
		return ElementMgr::size();
	}
	//发送可以协助的玩家列表
	void sendCanHelpGuardFriend(Player * pUser);
	//同步好友
	void synAllFriend();
private :
	Player * m_pOwner;
};

struct stApplyAddFriend
{
	UINT32 m_charID ;					//设置者的角色ID
	char   m_name[MAX_NAMESIZE + 1];	//申请者的名字
	UINT16 m_level;						//等级
	UINT8  m_job;						//职业
	UINT8  m_sex;						//性别
	UINT32 m_applyTime;					//申请时间
	UINT16 m_photoID;					//头像ID

	stApplyAddFriend()
	{
		m_charID = 0;
		bzero(m_name,sizeof(m_name));
		m_level = 0;
		m_job = 0;
		m_sex = 0;
		m_applyTime = 0;
		m_photoID = 0;
	}

	void fillSynInfo(FriendProto::stSynApplyerInfo * pSyn);
};

class CFriendDBOp
{
	USE_SINGLETON_NEW(CFriendDBOp);
private :
	CFriendDBOp();
public :
	static const UINT32 s_MAX_INTERACTIVE_LIST = 5;
public :
	~CFriendDBOp();
	//返回好友数据库操作类
	static CFriendDBOp  & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载配置
	bool loadCfg();
	//加载所有好友回调
	void loadAllFriendCB(CTblQueryResult* result,UINT32 charID);
	//增加一个好友
	void addFriendRecord(const stFriendDBInfo & data);
	//增加好友回调
	void addFriendRecordCB(bool opResult,UINT64 autoID);
	//更新莫个人的信息
	void updateOneInfo(Player * pUser);
	//更新信息
	void updateRecord(const stFriendDBInfo& data);
	//更新信息回调 
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除好友记录
	void delFriendRecord(UINT32 charID,UINT32 otherID);
	//删除好友记录回调
	void delFriendRecordCB(bool opResult,UINT64 autoID);
	//上线下线通知,type 1 表示上线，2表示下线
	void notifyAllFriend(UINT32 charID,UINT8 type);
	//通知所有好友回调
	void notifyAllFriendCB(CTblQueryResult* result,UINT32 charID,UINT8 type);
	//查找玩家基本信息回调
	void selectFriendInfoCB(CTblQueryResult* result,UINT32 charID);
	//查询玩家最后一次登录时间
	void selectLastLoginTime(UINT32 charID);
	//查询玩家最后一次登录时间回调
	void selectLastLoginTimeCB(CTblQueryResult* result,UINT32 charID);
	//查找玩家回调
	void findRoleInfoCB(CTblQueryResult* result,UINT32 charID);
	//增加申请信息到列表 
	void addApplyInfo2List(UINT32 charID,stApplyAddFriend & one);
	//增加申请列表回调
	void addApplyInfo2ListCB(CTblQueryResult* result,UINT32 charID,stApplyAddFriend one);	
	//删除某个申请者信息
	void delApplyInfo(UINT32 charID,UINT32 otherID);
	//是否在申请列表中
	bool isInApplyList(UINT32 charID,UINT32 otherID);
	//请求添加好友
	void reqAddFriend(UINT32 charID,UINT32 otherID);
	//请求删除好友
	void reqRemoveFriend(Player * pUser,UINT32 charID);
	//查找添加
	void findAndAdd(Player * pUser,char * name);
	//请求申请列表
	void reqApplyList(Player * pUser);
	//获取申请列表
	void getApplyList(Player * pUser,std::vector<UINT32> & applyList);
	//定时器处理
	void timer(UINT32 cur);
	//同步好友回调
	void synAllFriendCB(CTblQueryResult* result,UINT32 charID);
private :
	__gnu_cxx::hash_map<UINT32 ,std::list<stApplyAddFriend> > m_applyList;
	typedef __gnu_cxx::hash_map<UINT32 ,std::list<stApplyAddFriend> >::iterator ApplyList_IT;
	bool m_zeroRefresh;
};

#pragma pack(1)
struct stLilianDB
{
	stLilianDB()
	{
		
	}
};
#pragma pack()


class CLilianMgr
{
	USE_SINGLETON_NEW(CLilianMgr)
public :
	static const UINT32 s_MAX_FETCH_LILIAN_NUM = 30;
private :
	CLilianMgr();
public :
	static CLilianMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();		
	}
	
	//析构函数
	~CLilianMgr();
	//赠送历练给某个好友
	void giveLilian2Friend(Player * pUser,UINT32 charID);
	//请求历练列表
	void getLilianList(Player * pUser);
	//领取历练
	void fetchLilian(Player * pUser,UINT32 charID);
	//领取所有历练
	void fetchAllLilian(Player * pUser);
	//定时回调
	void timer(UINT32 cur);
	//获得玩家的赠送名单
	std::map<UINT32,std::set<UINT32> > & getGiveLilianInfos()
	{
		return m_giveLilianInfos;
	}
	//删除某人来自某人的历练值
	void removeLilian(UINT32 myID,UINT32 otherID);
private :
	std::map<UINT32,std::set<UINT32> > m_recLilianInfos;				//收到的历练信息
	std::map<UINT32,std::set<UINT32> > m_fetchLilianInfos;				//收取的历练信息
	std::map<UINT32,std::set<UINT32> > m_giveLilianInfos;				//已经赠送历练信息
	bool m_zeroRefresh;									
};

#pragma pack(1)
struct stMarryInfoDB
{
    UINT32 id;          //自增长id,用于生成房间号
    UINT32 husband;     //结婚对象
    UINT32 wife;        //结婚对象
    UINT32 husRing;     //男方戒指,不为0表示已经领取
    UINT32 wifeRing;    //女方戒指,不为0表示已经领取
    UINT32 weddingId;   //婚礼规格
    UINT32 expireTime;  //离婚到期时间

    stMarryInfoDB ()
    {
        id = 0;
        husband = 0;
        wife = 0;
        husRing = 0;
        wifeRing = 0;
        weddingId = 0;
        expireTime = 0;
    }
};

struct stRoleCoupleInfoDB
{
    UINT32 charId;
    UINT32 coupleId;
    char   coupleName[MAX_NAMESIZE + 1];

    stRoleCoupleInfoDB ()
    {
        charId = 0;
        coupleId = 0;
        memset(coupleName, sizeof(coupleName), 0);
    }
};
#pragma pack()

struct MarryInfo {
    UINT32 id;          //自增长id,用于生成房间号
    UINT32 husband;     //结婚对象
    UINT32 wife;        //结婚对象
    UINT32 husRing;     //男方戒指,不为0表示已经领取
    UINT32 wifeRing;    //女方戒指,不为0表示已经领取
    UINT32 weddingId;   //婚礼规格
    UINT32 expireTime;  //离婚到期时间

    MarryInfo ()
    {
        id = 0;
        husband = 0;
        wife = 0;
        husRing = 0;
        wifeRing = 0;
        weddingId = 0;
        expireTime = 0;
    }
    void load(const stMarryInfoDB & info) {
        id = info.id;
        husband = info.husband;
        wife = info.wife;
        husRing = info.husRing;
        wifeRing = info.husRing;
        weddingId = info.weddingId;
        expireTime = info.expireTime;
    }
};

class CMarryMgr
{
	USE_SINGLETON_NEW(CMarryMgr)
public :
	CMarryMgr() { }
	~CMarryMgr() { }
	static CMarryMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();		
	}
	//加载所有数据
    void load();
	//插入新记录
	void insertRecord(const stMarryInfoDB & data);
	//插入记录回调
	void insertRecordCB(bool opResult,UINT64 autoID, stMarryInfoDB data);
	//删除一条记录
	void delRecord(UINT32 roomId);
	//删除一条记录回调
	void delRecordCB(bool opResult,UINT64 autoID);
	//更新数据
	void updateRecord(const stMarryInfoDB & data);
	//更新记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//更新玩家数据
	void updateCoupleRecord(const stRoleCoupleInfoDB & data);
	//更新玩家记录回调
	void updateCoupleRecordCB(bool opResult,UINT64 autoID);
    //获取房间号
    UINT32 getRoomId(UINT32 coupleId);
	//房间是否存在
	bool roomExists(UINT32 roomId);
    //夫妇结婚
    void coupleMarry(UINT32 husband, UINT32 wife, UINT32 id, const std::string & husName, const std::string & wifeName);
    //夫妇离婚
	bool coupleDivorce(UINT32 friendId);
    //夫妇邀请好友参加婚礼
    bool inviteFriend(UINT32 roomId, UINT32 friendId);
    //接受夫妇邀请
    bool acceptInvite(UINT32 inviteId, UINT32 friendId);
    //玩家进入场景
    void addFriendToRoom(UINT32 friendId, UINT32 roomId);
    //玩家是否在场景中
    bool friendInRoom(UINT32 friendId);
	//获取房间id
	UINT32 getRoomIdByInviteId(UINT32 friendId);
	//玩家是否被邀请
	bool isInvited(UINT32 friendId, UINT32 roomId);
	//获取房间玩家列表
	void getRoomFriends(UINT32 roomId, std::set<UINT32> & s);
	//获取夫妇列表
	void getCouple(UINT32 roomId, UINT32 & husband, UINT32 & wife);
	//获取房间号
	UINT32 getRoomIdByUser(UINT32 friendId);
	//退出
	void quitRoom(UINT32 friendId);
    //获取结婚信息
    bool getMarryInfo(UINT32 roomId, MarryInfo & marryInfo);
    //更新结婚信息
    void updateMarry(UINT32 roomId, UINT32 charId, UINT32 ringId);
    //强制离婚
    void forceDivorce(UINT32 coupleA, UINT32 coupleB, bool coupleOnline);
    //设置离婚时间
    void setDivorceTime(UINT32 coupleId, UINT32 time);
    //定期扫描离婚
    void timer(UINT32 cur);
    //删除邀请列表记录
    void delInviteRecord(UINT32 charId, UINT32 roomId);
	//同步记录
	void synRecord(UINT32 charID, UINT32 coupleId, const std::string & coupleName);
private:
    std::map<UINT32, MarryInfo> m_coupleMap;//房间号对应结婚双方
    std::map<UINT32, UINT32> m_coupleToRoomMap;//结婚对象对应的房间
    std::map<UINT32, std::set<UINT32> > m_roomToInviteMap;    //房间号对应的邀请列表
	std::map<UINT32, UINT32> m_inviteToRoomMap;			//被邀请人对应的房间
    std::map<UINT32, UINT32> m_userToRoomMap;           //已进入房间玩家id对应房间号列表
    std::map<UINT32, std::set<UINT32> > m_inRoomUserMap;//房间对应的玩家列表
};
#endif
