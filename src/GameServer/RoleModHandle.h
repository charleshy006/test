#ifndef __H_ROLE_MOD_HANDLE_H__
#define __H_ROLE_MOD_HANDLE_H__
#include "ProtoMsgHandle.h"
#include "Role.pb.h"
#include "net.h"
#include "GameUser.h"
#include "GameProtoMsgHandleBase.h"

using namespace net;
class RoleModHandle : public ProtoMsgHandle<RoleModHandle,GameUser>,public GameProtoMsgHandleBase
{
public :
	//构造
	RoleModHandle();
	//析构
	~RoleModHandle();
	//处理网络协议
	bool handle(GameUser * pUser,const char * pData,const UINT32 & cmdLen,UINT8 funID);
	//处理gm指令
	void processChatCmd(GameUser * pUser,Role::stChatCmd * recvCmd);
	//武将出战
	void heroGoOutForBattle(GameUser * pUser,Role::stReqHeroGoOutForBattle * recvCmd);
	//武将下阵
	void heroChange2WaitState(GameUser *pUser,Role::ReqHeroChange2WaitState * recvCmd);
	//请求合成武将
	void reqCombineHero(GameUser *pUser,Role::stReqCombineHero * recvCmd);
	//武将请求升级
	void reqLevelupHero(GameUser * pUser,Role::stReqLevelupHero * recvCmd);
	//武将请求进化
	void reqHeroEvolution(GameUser *pUser,Role::stReqHeroEvolution * recvCmd);
	//领悟技能
	void reqHeroStudySkill(GameUser *pUser,Role::stReqHeroStudySkill * recvCmd);
	//武将10次领悟技能
	void reqHero10TimesStudySkill(GameUser * pUser,Role::stReqHero10TimeStudySkill * recvCmd);
	//熔炼技能
	void reqRefineSkill(GameUser * pUser,Role::stReqRefineHeroSkill * recvCmd);
	//替换技能
	void reqReplaceSkill(GameUser *pUser,Role::stReqReplaceHeroSkill * recvCmd);
	//升级技能
	void reqUpgradeSkill(GameUser *pUser,Role::stReqUpgradeHeroSkill * recvCmd);
	//请求整理背包
	void reqArrangeBag(GameUser * pUser,Role::stReqArrangeMainBag * recvCmd);
	//请求出售物品
	void reqSellItem(GameUser * pUser,Role::stReqSellItem * recvCmd);
	//请求移动道具
	void reqMoveItem(GameUser * pUser,Role::stMoveItem * recvCmd);
	//请求强化升级
	void reqRaiseItem(GameUser *pUser,Role::stReqRaiseItem * recvCmd);
	//请求5连强化
	void reqFiveTimeRaiseItem(GameUser * pUser,Role::stReqFiveRaiseEquip * recvCmd);
	//请求精炼技能
	void reqRefineItem(GameUser *pUser,Role::stReqRefineEquip * recvCmd);
	//请求熔炼
	void reqSmeltItem(GameUser *pUser,Role::stReqSmeltEquip * recvCmd);
	//请求重铸
	void reqRecastItem(GameUser * pUser,Role::stReqRecastEquip * recvCmd);
	//请求合成
	void reqCombineItem(GameUser *pUser,Role::stReqCombineItem * recvCmd);
	//请求商店列表
	void reqShopList(GameUser * pUser,Role::stReqOpenShop * recvCmd);
	//请求购买道具
	void reqBuyItem(GameUser * pUser,Role::stReqBuyItem * recvCmd);
	//请求刷新商店
	void reqRefreshShoplist(GameUser * pUser,Role::stReqRefreshShoplist * recvCmd);
	//请求完成关卡
	void reqFinishLv(GameUser * pUser,Role::stReqFinishLv * recvCmd);
	//请求关卡奖励
	void reqCharpterReward(GameUser * pUser,Role::stReqCharpterReward * recvCmd);
	//请求扫荡
	void reqRush(GameUser * pUser,Role::stReqRushLevel * recvCmd);
	//请求重置扫荡次数
	void reqResetRushNum(GameUser * pUser,Role::stReqResetRushNum * recvCmd);
	//使用道具
	void reqUseItem(GameUser * pUser,Role::stUseItem * recvCmd);
	//获得装备技能
	void getEquipSkill(GameUser * pUser,Role::stReqGetEquipSkill * recvCmd);
	//装备10次觉醒
	void getEquipSkill10Times(GameUser * pUser,Role::st10TimesGetEquipSkill * recvCmd);
	//熔炼装备技能
	void refineEquipSkill(GameUser * pUser,Role::stReqRefineEquipSkill * recvCmd);
	//移动觉醒的技能
	void moveEquipSkill2Slot(GameUser *pUser,Role::stMoveEquipSkill2Slot * recvCmd);
	//升级装备技能
	void upgradeEquipSkill(GameUser *pUser,Role::stReqUpgradeEquipSkill * recvCmd);
	//玩家技能升级
	void upgradeUserSkill(GameUser *pUser,Role::stReqRaiseUserSkillLv * recvCmd);
	//保存新手引导的步骤
	void saveNewBieGuideID(GameUser * pUser,Role::stSaveNewBieGuideStep * recvCmd);
	//关卡结束开箱子
	void levelOpenBox(GameUser * pUser,Role::stOpenLevelBox * recvCmd);
	//切换角色
	void changeRole(GameUser * pUser,Role::stChangeRole * recvCmd);
	//请求体力历练恢复回见
	void reqSPLilianRecoverTime(GameUser * pUser,Role::stReqRecoverRemainTime * recvCmd);
	//交换出战武将位置
	void swapHeroBattlePos(GameUser * pUser,Role::stSwapHeroBattlePos * recvCmd);
	//系统设置
	void saveSystemSetting(GameUser * pUser,Role::stReqSaveSystemSetting * recvCmd);
	//解锁稀有技能
	void unlockRareSkill(GameUser * pUser,Role::stUnlockRareSkill * recvCmd);
	//升级稀有技能
	void upgradeRareSkill(GameUser * pUser,Role::stUpgradeRareSkill * recvCmd);
	//领取首冲奖励
	void gotFirstRechargeReward(GameUser * pUser,Role::stReqFirstRechargeReward * recvCmd);	
	//新手引导子编号
	void saveSubNewBieGuideID(GameUser * pUser,Role::stSubNewBieGuideID * recvCmd);
	//重置副本
	void resetTeamCopymap(GameUser * pUser,Role::stResetTeamCopyMapNum * recvCmd);
	//重置实时战场
	void resetRealTimeCopymap(GameUser * pUser,Role::stResetRealTimeCopyMapNum * recvCmd);
	//首次10元充值
	void gotFirst10RechargeReward(GameUser * pUser,Role::stReqFirst10RechargeReward * recvCmd);
	//请求换魂
	void reqChgEquipSkill(GameUser * pUser,Role::stReqChgEquipSkill * recvCmd);
	//使用月卡
	void reqUseMonthCard(GameUser * pUser,Role::stReqUseMonthCardItem * recvCmd);
	//使用选择行道具
	void reqUseSelectItemBox(GameUser *pUser,Role::stReqUseSelectItemBox * recvCmd);
	//使用领悟技能书
	void reqUseHeroSkillBook(GameUser *pUser,Role::stReqUseHeroSkillBook * recvCmd);
	//使用觉醒技能数
	void reqUseEquipSkillBook(GameUser *pUser,Role::stReqUseEquipSkillBook * recvCmd);
	//请求装备突破
	void reqBreakItem(GameUser *pUser, Role::stReqBreakEquip * recvCmd);
	//请求英雄重铸
	void reqRecastHero(GameUser *pUser, Role::stReqRecastHero * recvCmd);
	//请求打开宝石购买金币窗口
	void reqOpenBuyGoldWindow(GameUser *pUser, Role::stReqOpenBuyGoldWindow * recvCmd);
	//请求宝石购买金币
	void reqBuyGold(GameUser *pUser, Role::stReqBuyGold * recvCmd);
	//请求附魔
	void reqEnchant(GameUser *pUser, Role::stReqEnchant * recvCmd);
	//请求时装重铸
	void reqRecastEnchant(GameUser *pUser, Role::stReqRecastEnchant * recvCmd);
	//请求锁定附魔时装
	void reqLockEnchant(GameUser *pUser, Role::stReqLockEnchant * recvCmd);
	//随从装备的符文
	void reqHeroRune(GameUser *pUser, Role::stReqHeroRune * recvCmd);
	//给随从添加符文
	void reqAddHeroRune(GameUser *pUser, Role::stReqAddHeroRune * recvCmd);
	//给随从移除符文
	void reqDelHeroRune(GameUser *pUser, Role::stReqDelHeroRune * recvCmd);
	//符文碎片合成符文
	void reqRunePieceCombine(GameUser *pUser, Role::stReqRunePieceCombine * recvCmd);
	//符文强化
	void reqRuneIntensify(GameUser *pUser, Role::stReqRuneIntensify * recvCmd);
	//符文进化
	void reqRuneEvolve(GameUser *pUser, Role::stReqRuneEvolve * recvCmd);
	//宝石合成
	void reqExtractGem(GameUser *pUser, Role::stReqExtractGem *recvCmd);
	//举办婚礼请求
	void reqWedding(GameUser *pUser, Role::stReqWedding *recvCmd);
	//被动方同意婚礼请求
	void reqAgreeWedding(GameUser *pUser, Role::stReqAgreeWedding *recvCmd);
	//离婚请求
	void reqDivorce(GameUser *pUser, Role::stReqDivorce *recvCmd);
	//被动方同意或拒绝离婚
	void reqRejectDivorce(GameUser *pUser, Role::stReqRejectDivorce *recvCmd);
	//请求今日购买次数
	void reqBuyNum(GameUser *pUser, Role::stReqBuyNum *recvCmd);
	//请求黑市列表
	void reqBlackMarketList(GameUser *pUser, Role::stReqColumnList *recvCmd);
	//请求购买黑市商品
	void reqBuyBlackMarketItem(GameUser *pUser, Role::stReqMarketBuyItem *recvCmd);
	//请求打开公会黑市窗口
	void reqOpenBlackMarkerWindow(GameUser *pUser, Role::stReqOpenWindow *recvCmd);
	//请求刷新黑市列表
	void reqRefreshList(GameUser *pUser, Role::stReqRefreshList *recvCmd);
	//请求今日刷新次数
	void reqRefreshNum(GameUser *pUser, Role::stReqRefreshNum *recvCmd);
	//请求技能一键升级
	void reqAKeyRaiseUserSkillLv(GameUser *pUser, Role::stReqAKeyRaiseUserSkillLv * recvCmd);
	//请求晶石合成
	void reqStoneSynthesis(GameUser * pUser, Role::stReqStoneSynthesis * recvCmd);
	//请求晶石进化
	void reqStoneEvolvement(GameUser * pUser, Role::stReqStoneEvolvement * recvCmd);
	//请求坐骑石头属性
	void reqStoneProp(GameUser * pUser, Role::stReqStoneProp *recvCmd);
	//请求更换头像
	void reqChangePhoto(GameUser *pUser, Role::stReqChangePhoto * recvCmd);
	//请求当前已有头像列表
	void reqHasPhotoIDs(GameUser *pUser, Role::stReqHasPhotoID * recvCmd);
	//请求解锁头像
	void reqUnlockPhoto(GameUser *pUser, Role::stReqUnlockPhoto * recvCmd);
	//请求精灵信息
	void reqSpriteInfo(GameUser *pUser, Role::stSpriteInfo * recvCmd);
	//请求调教随从
	void reqTrainHero(GameUser *pUser, Role::stTrainHeroReq * recvCmd);
	//领取挂机奖励
	void reqGuajiAward(GameUser *pUser, Role::stGuaJiAwardReq * recvCmd);
	//开始挂机
	void reqGuajiStart(GameUser *pUser, Role::stGuaJiStartReq * recvCmd);
};
#endif
