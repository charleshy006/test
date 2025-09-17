#ifndef __H_GUILD_HANDLE_H__
#define __H_GUILD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "net.h"
#include "Player.h"
#include "GlobalProtoMsgHandleBase.h"
#include "GuildProto.pb.h"

using namespace net;
class GuildModHandle : public ProtoMsgHandle<GuildModHandle,Player>,public GlobalProtoMsgHandleBase
{
public :
	//构造
	GuildModHandle();
	//析构
	~GuildModHandle();
	//处理网络协议
	bool handle(Player * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//请求公会列表
	void reqGuildList(Player * pUser,GuildProto::stReqGuild * recvCmd);
	//请求公会的详细信息
	void reqGuildDetail(Player * pUser,GuildProto::stReqGuildDetailInfo * recvCmd);
	//请求申请列表
	void reqApplyList(Player * pUser,GuildProto::stReqApplyJoinInList * recvCmd);
	//答复申请者
	void replyApplier(Player * pUser,GuildProto::stReplyApplier * recvCmd);
	//取消申请加入
	void cancelJoinIn(Player * pUser,GuildProto::stCancelJoinInGuild * recvCmd);
	//请求离开公会
	void reqLeaveGuild(Player * pUser,GuildProto::stReqLeaveGuild * recvCmd);
	//给头衔
	void assignTitle(Player * pUser,GuildProto::stAssignGuildTitle * recvCmd);
	//取消头衔
	void cancelTitle(Player * pUser,GuildProto::stCancelTitle * recvCmd);
	//转让领导
	void giveLeader2Other(Player * pUser,GuildProto::stGiveLeader2Other * recvCmd);
	//解散公会
	void fireGuild(Player * pUser,GuildProto::stFireGuild * recvCmd);
	//开除公会成员
	void fireGuildMember(Player * pUser,GuildProto::stKillOutGroupMember * recvCmd);
	//修改公会签名
	void chgSign(Player * pUser,GuildProto::stChgGuildSign * recvCmd);
	//请求公会成员信息
	void reqMemberInfo(Player * pUser,GuildProto::stReqGuildMemInfo * recvCmd);
	//取消解散公会
	void cancelFireGuild(Player * pUser,GuildProto::stCancelFireGuild * recvCmd);
	//请求升级大厅建筑
	void reqLevelupHall(Player * pUser,GuildProto::stReqLevelUpHallLv * recvCmd);
	//请求升级祈福建筑
	void reqLevelupPrayBuilding(Player * pUser,GuildProto::stReqPrayBuildingLv * recvCmd);
	//请求升级商店等级
	void reqLevelupShopLv(Player * pUser,GuildProto::stReqLevelUpShopBuildingLv * recvCmd);
	//请求动态
	void reqGuildTrace(Player * pUser,GuildProto::stReqGuildTrace * recvCmd);
	//修改公告
	void chgNotice(Player * pUser,GuildProto::stChgNotice * recvCmd);
	//祈福
	void guildPray(Player * pUser,GuildProto::stReqPray * recvCmd);
	//更换徽章
	void changeBadge(Player * pUser,GuildProto::stReqChangeBadge * recvCmd);
	//进入公会领地
	void intoGuildCopymap(Player * pUser,GuildProto::stIntoGuildCopymap * recvCmd);		
	//搜索公会
	void searchGuild(Player * pUser,GuildProto::stSearchGuild * recvCmd);
	//开打公会副本界面
	void openGuildCopymap(Player * pUser,GuildProto::stReqOpenGuildCopymapWin * recvCmd);
	//请求伤害列表
	void reqDamageSortlist(Player * pUser,GuildProto::stReqDamageSortlist * recvCmd);
	//请求积分列表
	void reqGuildCopySortlist(Player * pUser,GuildProto::stReqGuildCopymapScoreSortlist * recvCmd);
	//请求进入公会副本
	void reqEnterGuildCopymap(Player * pUser,GuildProto::stReqEnterGuildCopymap * recvCmd);
	//开启公会副本
	void startGuildCopymap(Player * pUser,GuildProto::stStartGuildCopymap * recvCmd);
	//组队邀请
	void teamInvite(Player * pUser,GuildProto::stInviteTeamInGuildChannel * recvCmd);
	//请求打开公会战界面
	void reqGuildManorFightWin(Player * pUser,GuildProto::stReqGuildManorFightWin * recvCmd);
	//打开申请公会战界面
	void reqGuildManorFightApplyWin(Player * pUser,GuildProto::stReqOpenApplyManorFightWin * recvCmd);
	//申请进行公会战
	void applyGuildFight(Player * pUser,GuildProto::stApplyManorFight * recvCmd);
	//申请公会战战斗信息
	void openManorFightWin(Player * pUser,GuildProto::stReqManorFightWin * recvCmd);
	//公会战阵容
	void manorFightFormation(Player * pUser,GuildProto::stReqManorFightFormation * recvCmd);
	//离开公会战界面
	void leaveManorFightWin(Player * pUser,GuildProto::stNotifyLeaveManorFightWin * recvCmd);
	//进入领地战斗队伍
	void enterManorFightTeam(Player * pUser,GuildProto::stEnterManorFightTeam * recvCmd);
	//领地战换位
	void manorFightChgTeamPos(Player * pUser,GuildProto::stReqChgManorFightTeamPos * recvCmd);
	//请求升级公会黑市
	void reqLevelupBlackMarketLv(Player * pUser,GuildProto::stReqLevelUpBlackMarketLv * recvCmd);
};
#endif
