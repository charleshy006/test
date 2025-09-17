#ifndef __INVITE_FRIEND_H__
#define __INVITE_FRIEND_H__

#include "TypesDef.h"
#include "Singleton.h"
#include "Player.h"
#include <list>
#include <map>

#define INVITE_CODE_LEN 50
#define INVITE_REWARD_LEN 100

#pragma pack(1)
struct stInviteFriendInfoDB 
{
	UINT32 id;                                      //自增长id,用于生成邀请码
    char account[MAX_ACCNAMESIZE + 1];              //登陆账号
    char code[INVITE_CODE_LEN];                     //邀请码
    UINT32 inviteCount;                             //邀请总人数
    char inviteReward[INVITE_REWARD_LEN];           //邀请其他人获得的奖励
    UINT32 beInvited;                                //是否被邀请过,使用过邀请码认定被邀请过

	stInviteFriendInfoDB ()
	{
		id = 0;
		bzero(account, sizeof(account));
		bzero(code,sizeof(code));
		inviteCount = 0;
		bzero(inviteReward, sizeof(inviteReward));
        beInvited = 0;
	}
};
#pragma pack()

class Player;
//需要时生成
class InviteFriend
{
public:
	//填充DB数据
	void fillDBData(stInviteFriendInfoDB & data);
	//初始化来自db的数据
	void initFromDBData(const stInviteFriendInfoDB & data);
    //发送玩家好友初始信息
    void sendData(Player * pUser);
    //获取邀请人数
    UINT32 getInviteCount() {
        return m_inviteCount;
    }
    //获取指定id是否已经领取奖励
    bool isDrawReward(UINT8 id);
    //是否被邀请
    bool isInvited() {
        return m_beInvited;
    }
    std::string & getCode() {
        return m_inviteCode;
    }
    std::string & getAccount() {
        return m_account;
    }
    void addInviteCount(int val = 1) {
        m_inviteCount += val;
    }
    void setBeInvited(bool val) {
        m_beInvited = val;
    }
    void drawReward(UINT8 id);
private:
    UINT32 m_id;
    bool m_beInvited;
    UINT32 m_inviteCount;
    std::string m_account;
    std::string m_inviteCode;
    std::map<UINT8, bool> m_inviteRewardMap;
};

class CInviteFriendMgr
{
    USE_SINGLETON_NEW(CInviteFriendMgr);
public :
    CInviteFriendMgr() { }
    ~CInviteFriendMgr();
	static CInviteFriendMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//加载所有数据
	void load();
	//插入新记录
	void insertRecord(const stInviteFriendInfoDB & data, Player * pUser);
	//插入记录回调
	void insertRecordCB(bool opResult,UINT64 autoID,
            stInviteFriendInfoDB data, Player * pUser);
	//更新数据
	void updateRecord(const stInviteFriendInfoDB & data);
	//更新记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
    //获取邀请好友指针
    InviteFriend * getInviteFriend(const std::string & account);
    //增加一个玩家好友信息
    void addInviteFriend(const std::string & account, Player * pUser);
    //生成需要的gameId
    static std::string getGameId();
    //邀请码格式是否合法
    static bool checkCode(const std::string & code);
    //获取邀请码对应的主人
    bool getOwner(const std::string & code, std::string & owner);
private :
    std::map<std::string, InviteFriend* > m_inviteMap;
    std::map<std::string, std::string> m_codeMap;
};
#endif
