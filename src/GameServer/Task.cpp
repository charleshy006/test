#include "Task.h"
#include "GameUser.h"
#include "GameServer.h"
#include "TblDataMgr.h"
#include <sstream>

extern GameServer * g_gameServer;

bool stTaskTemplate::parse(stTaskBase * pTaskBase)
{
	if (!pTaskBase){
		return false;
	}
	m_taskID = pTaskBase->m_id ;
	strncpy(m_name,pTaskBase->m_name,sizeof(m_name));
	m_taskType = pTaskBase->m_taskType;
	m_daiyTaskScore = pTaskBase->m_daiyTaskScore;

	if (pTaskBase->m_awardExp){//有经验奖励
		stTaskReward reward;
		reward.m_type = TR_EXP;
		reward.m_id = 0;
		reward.m_num = pTaskBase->m_awardExp;
		m_reward.push_back(reward);
	}


	if (pTaskBase->m_awardGoldCoin){//金币
		stTaskReward reward;
		reward.m_type = TR_GOLDCOIN;
		reward.m_id = 0;
		reward.m_num = pTaskBase->m_awardGoldCoin;
		m_reward.push_back(reward);
	}

	if (pTaskBase->m_awardDiamond){//元宝
		stTaskReward reward;
		reward.m_id = 0;
		reward.m_type = TR_POINTCARD;
		reward.m_num = pTaskBase->m_awardDiamond;
		m_reward.push_back(reward);
	}

	
	for (UINT8 i = 0;i < pTaskBase->m_awardItem.size(); ++i){//道具
		stTaskReward reward;
		reward.m_id = pTaskBase->m_awardItem[i].m_id;
		reward.m_num = pTaskBase->m_awardItem[i].m_num;
		reward.m_type = TR_ITEM;
		m_reward.push_back(reward);
	}
	
	stTaskFinishCond finishCond;
	if (pTaskBase->m_finishCond == TaskProto::ET_FINISH_COPYMAP){//完成某个关卡
		finishCond.m_goalID = TaskProto::ET_FINISH_COPYMAP;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_FINISH_COPYMAP << "_" <<pTaskBase->m_finishPara1;
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_RAISE_EQUIP_TO_LV){//强化装备到某个级别
		finishCond.m_goalID = TaskProto::ET_RAISE_EQUIP_TO_LV;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = TaskProto::ET_RAISE_EQUIP_TO_LV;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_REFINE_EQUIP_TO_LV){//精炼装备到某个级别
		finishCond.m_goalID = TaskProto::ET_REFINE_EQUIP_TO_LV;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = TaskProto::ET_REFINE_EQUIP_TO_LV;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_HERO_BREAK_INTO_LV){//武将突破到某个级别
		finishCond.m_goalID = TaskProto::ET_HERO_BREAK_INTO_LV;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = TaskProto::ET_HERO_BREAK_INTO_LV;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_HERO_LEVELUP_TO_LV){//武将进化到某个级别
		finishCond.m_goalID = TaskProto::ET_HERO_LEVELUP_TO_LV;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = TaskProto::ET_HERO_LEVELUP_TO_LV;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_RAISE_SKILL_TO_LV){//技能突破到某个级别
		finishCond.m_goalID = TaskProto::ET_RAISE_SKILL_TO_LV;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = TaskProto::ET_RAISE_SKILL_TO_LV;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_EQUIP_FUWEN_NUM){//装备5个符文的次数
		finishCond.m_goalID = TaskProto::ET_EQUIP_FUWEN_NUM;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = TaskProto::ET_EQUIP_FUWEN_NUM;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_RAISE_FUWEN_TO_LV){//符文强化到某个级别
		finishCond.m_goalID = TaskProto::ET_RAISE_FUWEN_TO_LV;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = TaskProto::ET_RAISE_FUWEN_TO_LV;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_COLLECT_FASHION_NUM){//时装数量
		finishCond.m_goalID = TaskProto::ET_COLLECT_FASHION_NUM;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = TaskProto::ET_COLLECT_FASHION_NUM;
		finishCond.m_propParam = pTaskBase->m_finishPara2;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_CAPTRURE_TREASURE_NUM){//夺宝次数
		finishCond.m_goalID = TaskProto::ET_CAPTRURE_TREASURE_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_CAPTRURE_TREASURE_NUM << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);

	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_ARENA_NUM){//竞技场次数
		finishCond.m_goalID = TaskProto::ET_ARENA_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_ARENA_NUM<<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_TEAM_FINISH_COPYMAP_NUM){//组队完成副本次数
		finishCond.m_goalID = TaskProto::ET_TEAM_FINISH_COPYMAP_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_TEAM_FINISH_COPYMAP_NUM<<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_DIANFENG_PK_NUM){//巅峰对决次数
		finishCond.m_goalID = TaskProto::ET_DIANFENG_PK_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_DIANFENG_PK_NUM<<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_REALTIME_COPYMAP_NUM){//实时战场次数
		finishCond.m_goalID = TaskProto::ET_REALTIME_COPYMAP_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_REALTIME_COPYMAP_NUM<<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_LEITAI_COPYMAP_NUM){//擂台次数
		finishCond.m_goalID = TaskProto::ET_LEITAI_COPYMAP_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_LEITAI_COPYMAP_NUM<<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_SHENGCUN_COPYMAP_NUM){//生存关
		finishCond.m_goalID = TaskProto::ET_SHENGCUN_COPYMAP_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_SHENGCUN_COPYMAP_NUM<<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_GOLDCOIN_COPYMAP_NUM){//金币关卡次数
		finishCond.m_goalID = TaskProto::ET_GOLDCOIN_COPYMAP_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_GOLDCOIN_COPYMAP_NUM<<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_ELITE_COPYMAP_NUM){//英关卡次数
		finishCond.m_goalID = TaskProto::ET_ELITE_COPYMAP_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os <<TaskProto::ET_ELITE_COPYMAP_NUM<<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_DRAW_NUM){//抽奖次数
		finishCond.m_goalID = TaskProto::ET_DRAW_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_DRAW_NUM << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_RAISE_HERO_NUM){//强化武神次数
		finishCond.m_goalID = TaskProto::ET_RAISE_HERO_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_RAISE_HERO_NUM << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_RAISE_SKILL_NUM){//强化技能次数
		finishCond.m_goalID = TaskProto::ET_RAISE_SKILL_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_RAISE_SKILL_NUM <<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_RAISE_EQUIP_NUM){//强化物品次数
		finishCond.m_goalID = TaskProto::ET_RAISE_EQUIP_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_RAISE_EQUIP_NUM <<"_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_RAISE_FUWEN_NUM){//强化符文次数
		finishCond.m_goalID = TaskProto::ET_RAISE_FUWEN_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_RAISE_FUWEN_NUM << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_GUARD){//完成护送次数
		finishCond.m_goalID = TaskProto::ET_GUARD;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_GUARD << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_INTERCEPT){
		finishCond.m_goalID = TaskProto::ET_INTERCEPT;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_INTERCEPT << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_GET_WELFARE_CARD){//领取福利卡
		finishCond.m_goalID = TaskProto::ET_GET_WELFARE_CARD;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = TaskProto::ET_GET_WELFARE_CARD;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_GET_MONTH_CARD){//领取月卡
		finishCond.m_goalID = TaskProto::ET_GET_MONTH_CARD;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = TaskProto::ET_GET_MONTH_CARD;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_GET_QUARTER_CARD){//领取季度卡
		finishCond.m_goalID = TaskProto::ET_GET_QUARTER_CARD;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = TaskProto::ET_GET_QUARTER_CARD;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_GET_YEAR_CARD){//领取年卡
		finishCond.m_goalID = TaskProto::ET_GET_YEAR_CARD;
		finishCond.m_tt = TT_PROPPERTY; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;
		finishCond.m_propID = TaskProto::ET_GET_YEAR_CARD;
		finishCond.m_propParam = pTaskBase->m_finishPara1;
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_BUY_SHOP_ITEM_NUM){//商城购买某个物品次数
		finishCond.m_goalID = TaskProto::ET_BUY_SHOP_ITEM_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara2;
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_BUY_SHOP_ITEM_NUM << "_" << pTaskBase->m_finishPara1;
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_EXPEDITION_NUM){//远征完成次数
		finishCond.m_goalID = TaskProto::ET_EXPEDITION_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;//para1 is num
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_EXPEDITION_NUM << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_RESET_RUSH_NUM){//重置主线关卡
		finishCond.m_goalID = TaskProto::ET_RESET_RUSH_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;//para1 is num
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_RESET_RUSH_NUM << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_GIVE_LILIAN_NUM){//赠送好友历练
		finishCond.m_goalID = TaskProto::ET_GIVE_LILIAN_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;//para1 is num
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_GIVE_LILIAN_NUM << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else if (pTaskBase->m_finishCond == TaskProto::ET_GUILD_PRAY_NUM){//公会祈祷
		finishCond.m_goalID = TaskProto::ET_GUILD_PRAY_NUM;
		finishCond.m_tt = TT_COUNTER; 
		finishCond.m_taskCounter = pTaskBase->m_finishPara1;//para1 is num
		finishCond.m_propID = 0;
		finishCond.m_propParam = 0;
		std::stringstream os;
		os << TaskProto::ET_GUILD_PRAY_NUM << "_";
		finishCond.m_eventKey = os.str();
		m_finishCond.push_back(finishCond);
	}
	else {
		Athena::logger->error("解析任务taskID=%d出错,完成条件填写错误",pTaskBase->m_id);
		return true;
	}

	stPropPair propPair;
	if (pTaskBase->m_openLv) {
		propPair.m_propId = PT_USER_LEVEL;
		propPair.m_param = 0;
		propPair.m_minValue = pTaskBase->m_openLv;
		propPair.m_maxValue = (UINT32)-1;
		m_acceptCond.m_propPairList.push_back(propPair);
	}

	if (pTaskBase->m_unlockTaskID) {
		propPair.m_propId = PT_USER_PRE_TASK;
		propPair.m_param = pTaskBase->m_unlockTaskID;
		propPair.m_minValue = 1;
		propPair.m_maxValue = 1;
		m_acceptCond.m_propPairList.push_back(propPair);
	}

	if (pTaskBase->m_finishLevelID){
		propPair.m_propId = PT_USER_FINISH_LV;
		propPair.m_param = pTaskBase->m_finishLevelID;
		propPair.m_minValue = 1;
		propPair.m_maxValue = 1;
		m_acceptCond.m_propPairList.push_back(propPair);
	}
	
	if (pTaskBase->m_preTaskID) {
		m_preTask.push_back(pTaskBase->m_preTaskID);
	}
	return true;
}

bool stTaskTemplate::canAcceptTask(GameUser * pUser)
{
	if (!pUser){
		return false;
	}
		
	for (UINT8 i = 0; i < m_acceptCond.m_propPairList.size(); ++i){//检查所有条件
		stPropPair &  propPair = m_acceptCond.m_propPairList[i];
		UINT32 propVal = pUser->getPropVal(propPair.m_propId,propPair.m_param);
		if (!((propVal >= propPair.m_minValue) && (propVal <= propPair.m_maxValue))){
			return false;	
		}
	}

	if (CTaskTemplateMgr::getSingleton().isRechargeTask(m_taskID)){//充值相关任务
		if (!pUser->m_taskMgr.canAcceptRechargeTask(m_taskID)){
			return false;
		}	
	}
	
	if (!m_preTask.size()) {
		return true;
	}

	//检查前置任务
	for (UINT16 i = 0;i < m_preTask.size(); ++i){
		if (pUser->m_taskMgr.isTaskReward(m_preTask[i])) {
			return true;
		}
	}	
	return false;
}

bool stTaskTemplate::completeTask(GameUser * pUser,stTaskDBInfo * pTaskInfo,bool bNotify)
{
	if (!pUser || !pTaskInfo){
		return false;
	}
	
	stTaskTemplate * pTemplate = CTaskTemplateMgr::getSingleton().getTemplateByID(pTaskInfo->m_taskID);
	if (!pTemplate){//找不到模板
		return false;
	}
	
	bool finish = true;
	for (UINT8 i = 0;i < pTemplate->m_finishCond.size() ; ++i){
		stTaskFinishCond & finishCond= pTemplate->m_finishCond[i];
		if (pTaskInfo->m_taskCounter[finishCond.m_goalID] < finishCond.m_taskCounter){//只要有一个条件不符合就不能完成
			finish = false;
			break;
		}
	}
	
	if (finish){
		pTaskInfo->m_ts = TaskProto::TS_COMPLETE;
	}

	if (bNotify && finish){
		pUser->taskStatusUpdate(NT_UPDATE,pTaskInfo);
	}
	return finish;
}

void stTaskDBInfo::fillSynInfo(TaskProto::stSynTaskInfo & data)
{
	data.set_taskid(m_taskID);
	data.set_ts(m_ts);
	data.set_subts(m_subTs);

	stTaskTemplate * pTemplate = CTaskTemplateMgr::getSingleton().getTemplateByID(m_taskID);
	if (pTemplate && pTemplate->m_finishCond.size()){
		stTaskFinishCond & finishCond = pTemplate->m_finishCond[0];
		TaskProto::stTaskCounter *pCounter =  data.add_counter();
		pCounter->set_eventtype(TaskProto::ET_FINISH_COPYMAP);
		pCounter->set_id(0);
		pCounter->set_value( m_taskCounter[finishCond.m_goalID]);
	}
}

void stTaskDBInfo::reset(GameUser * pOwner)
{
	if (!pOwner){
		return ;
	}

	m_ts = TaskProto::TS_INCOMPLETE;
	m_subTs = 1;
	m_acceptTime = 0;
	m_taskCounter.clear();

	stTaskTemplate *pTemplate =  CTaskTemplateMgr::getSingleton().getTemplateByID(m_taskID);
	if (pTemplate){
		for (UINT16 i = 0;i < pTemplate->m_finishCond.size();++i){
			if (TT_PROPPERTY == pTemplate->m_finishCond[i].m_tt){
				m_taskCounter[pTemplate->m_finishCond[i].m_goalID] = pOwner->getPropVal(pTemplate->m_finishCond[i].m_propID,pTemplate->m_finishCond[i].m_propParam); 					
			}
			else if (TT_COUNTER == pTemplate->m_finishCond[i].m_tt){
				m_taskCounter[pTemplate->m_finishCond[i].m_goalID] = 0;	
			}
		}
		pTemplate->completeTask(pOwner,this);
	}
}

CTaskTemplateMgr::CTaskTemplateMgr()
{
	
}

CTaskTemplateMgr::~CTaskTemplateMgr()
{

}

struct stTaskTempateParseCB : public TblDatabaseCB<stTaskBase>
{
public :
	stTaskTempateParseCB() {}
	~stTaskTempateParseCB() {}
	
	virtual bool doit(stTaskBase * pBase)
	{
		stTaskTemplate * pTemplate = ATHENA_NEW stTaskTemplate();
		if (pTemplate->parse(pBase)){
			CTaskTemplateMgr::getSingleton().addTaskTemplate(pTemplate->m_taskID,pTemplate);
			return true;
		}
		return false;
	}
};

bool CTaskTemplateMgr::addTaskTemplate(UINT32 taskID,stTaskTemplate *pTemplate)
{
	m_allTaskCfg[taskID] = 	pTemplate;
	if (pTemplate->m_taskType == DAILY_TASK){//是否是日常
		m_dailyTaskCnt.insert(taskID);	
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("增加了任务模板taskID=%d",taskID);
#endif
	return true;
}

bool CTaskTemplateMgr::loadTaskCfg()
{
	stTaskTempateParseCB cb;
	g_taskBaseMgr.execEveryOne(cb);
	Template_IT it = m_allTaskCfg.begin();
	//构建任务之前的前后关系
	for (;it != m_allTaskCfg.end();++it){
		Template_IT subIt = m_allTaskCfg.begin();
		for (;subIt != m_allTaskCfg.end();++subIt){
			if (it->second->m_taskID != subIt->second->m_taskID){
				for (UINT8 i = 0;i < subIt->second->m_preTask.size() ;++i){
					if (it->first == subIt->second->m_preTask[i]){
						it->second->m_nextTask.push_back(subIt->second->m_taskID);
					}	
				}				
			}	
		}	
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("加载了%d个任务,%d个日常",m_allTaskCfg.size(),m_dailyTaskCnt.size());
	it = m_allTaskCfg.begin();
	for (;it != m_allTaskCfg.end();++it){
		Athena::logger->trace("任务id=%d,name=%s",it->second->m_taskID,it->second->m_name);
		Athena::logger->trace("前置任务为:");
		for (UINT16 i = 0;i < it->second->m_preTask.size();++i){
			Athena::logger->trace("taskID=%d",it->second->m_preTask[i]);
		}

		Athena::logger->trace("后置任务为:");
		for (UINT16 i = 0;i < it->second->m_nextTask.size();++i){
			Athena::logger->trace("taskID=%d",it->second->m_nextTask[i]);	
		}
	}
#endif

	//加载日志任务相关配置
	TiXmlDocument doc;
	std::string fileName = Athena::global["confDir"] + "/dailyreward.xml";
	if (!doc.LoadFile(fileName.c_str())){
		Athena::logger->error("[日常任务配置]打开配置文件%s错误",fileName.c_str());
		return false;
	}   

	TiXmlElement * pRoot = doc.FirstChildElement("conf");
	if (!pRoot){
		Athena::logger->error("[日常任务配置]加载错误,找不到conf节点!");
		return false;
	} 
	
	INT32 iVal = 0;
	TiXmlElement * pDailyNode = pRoot->FirstChildElement("daily");
	if (pDailyNode){
		TiXmlElement * pEntryNode = pDailyNode->FirstChildElement("entry");
		while (pEntryNode){
			stDailyTaskScoreReward one;
			if (pEntryNode->Attribute("id",&iVal)){
				one.m_id = iVal;
			}
			if (pEntryNode->Attribute("points",&iVal)){
				one.m_socre = iVal;
			}
			if (pEntryNode->Attribute("boxid",&iVal)){
				one.m_boxID = iVal;
			}
			m_dailyTaskScoreRewards[one.m_id] = one;
			pEntryNode = pEntryNode->NextSiblingElement("entry");
		}
	}
#ifdef _HDZ_DEBUG
	Athena::logger->trace("打印日常任何积分:");
	std::map<UINT32,stDailyTaskScoreReward>::iterator it1 = m_dailyTaskScoreRewards.begin();	
	for (;it1 != m_dailyTaskScoreRewards.end();++it1){
		stDailyTaskScoreReward & one = it1->second;
		Athena::logger->trace("id:%d,score:%d,boxid:%d",one.m_id,one.m_socre,one.m_boxID);
	}
#endif
	return true;
}

stTaskTemplate * CTaskTemplateMgr::getTemplateByID(UINT32 taskID)
{
	__gnu_cxx::hash_map<UINT32,stTaskTemplate *>::iterator it =  m_allTaskCfg.find(taskID);
	if (it == m_allTaskCfg.end()){
		return NULL;
	}
	return it->second;
}

bool CTaskTemplateMgr::isDailyTask(UINT32 taskID)
{
	__gnu_cxx::hash_map<UINT32,stTaskTemplate *>::iterator it =  m_allTaskCfg.find(taskID);
	if (it == m_allTaskCfg.end()){
		return false;
	}
	return it->second->m_taskType == DAILY_TASK;
}

stDailyTaskScoreReward *  CTaskTemplateMgr::getScoreRewardInfo(UINT32 id)
{
	std::map<UINT32,stDailyTaskScoreReward>::iterator it =  m_dailyTaskScoreRewards.find(id);	
	if (it != m_dailyTaskScoreRewards.end()){
		return &it->second;
	}
	else {
		return NULL;
	}
}

bool CTaskTemplateMgr::isRechargeTask(UINT32 taskID)
{
	switch (taskID){
		case eMonth_Card_TaskID  :		//月卡任务
		case eQuarter_Card_TaskID  :  	//季卡任务
		case eYear_Card_TaskID  :		//年卡任务
		case eWelfare_Card_TaskID :		//福利卡任务
			{
				return true;
			}
			break;
	}
	return false;
}


CTaskMgr::CTaskMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_refreshDailyTaskTime = 0;
	m_dailyTaskScore = 0;
	m_bNeedRefreshDailyTask = false;
}

CTaskMgr::~CTaskMgr()
{
	
}

UINT32 CTaskMgr::saveAllTask(byte * buf,UINT32 needSpace)
{
	stSaveTaskInfo * pSaveInfo = (stSaveTaskInfo *)buf;
	UINT32 offset = sizeof(stSaveTaskInfo);
	pSaveInfo->m_version = s_TASK_VERSION;
	//日常任务的刷新时间
	*(UINT32 *)(buf + offset) = m_refreshDailyTaskTime;
	offset += sizeof(UINT32);
	//日常任务积分
	*(UINT32 *)(buf + offset)= m_dailyTaskScore;
	offset += sizeof(UINT32);
	//日常任务积分奖励ID
	*(UINT8*)(buf + offset)= m_dailyTaskScoreRewardIDs.size();
	offset += sizeof(UINT8);
	std::set<UINT8>::iterator it1 =  m_dailyTaskScoreRewardIDs.begin();
	for (;it1 != m_dailyTaskScoreRewardIDs.end();++it1){
		*(UINT8*)(buf + offset)= *it1;
		offset += sizeof(UINT8);
	}

	UINT16 finishNum = m_rewardTaskID.size();
	//保存已经完成的任务
	*(UINT16 *)(buf + offset) = finishNum;
	offset += sizeof(UINT16);
	std::set<UINT32>::iterator it = m_rewardTaskID.begin();
	for (;it != m_rewardTaskID.end();++it){
		*(UINT32 *)(buf + offset) = *it;
		offset += sizeof(UINT32);
	}

	//保存正在进行中的任务
	UINT16 acceptNum = m_acceptTask.size();
	*(UINT16 *)(buf + offset) = acceptNum;
	offset += sizeof(UINT16);
	std::map<UINT32,stTaskDBInfo >::iterator subIt = m_acceptTask.begin();	
	for (;subIt != m_acceptTask.end() ;++subIt){
		stTaskDBInfo & taskInfo = subIt->second;
		//保存id
		*(UINT32*)(buf + offset) = taskInfo.m_taskID;
		offset += sizeof(UINT32);
		//保存状态
		*(UINT8 * )(buf + offset) = taskInfo.m_ts;
		offset += sizeof(UINT8);
		//保存任务子状态
		*(UINT8 *)(buf + offset) = taskInfo.m_subTs;
		offset += sizeof(UINT8);
		//保存接任务时间
		*(UINT32 * )(buf + offset) = taskInfo.m_acceptTime;
		offset += sizeof(UINT32);
		//保存计数
		UINT8 taskCounter = taskInfo.m_taskCounter.size();
		*(UINT8*)(buf + offset) = taskCounter;
		offset += sizeof(UINT8);
		std::map<UINT8,UINT32>::iterator thdIt = taskInfo.m_taskCounter.begin();
		for ( ; thdIt != taskInfo.m_taskCounter.end(); ++thdIt){
			*(UINT8 *)(buf + offset) = thdIt->first;
			offset += sizeof(UINT8);
			*(UINT32 *)(buf + offset) = thdIt->second;
			offset += sizeof(UINT32);
			
		}
	}
	pSaveInfo->m_size = offset;	
	return offset;
}

UINT32  CTaskMgr::loadAllTask(byte * buf)
{
	stSaveTaskInfo * pSaveInfo = (stSaveTaskInfo *)buf;
	UINT32 offset = sizeof(stSaveTaskInfo);
	Athena::logger->trace("上线加载任务数据,版本号=%d", s_TASK_VERSION);
	Athena::logger->trace("上线加载任务数据,版本号=%d", pSaveInfo->m_version);

	if (pSaveInfo->m_version == s_TASK_VERSION)	{
		//刷新日常任务时间
		m_refreshDailyTaskTime = *(UINT32 *)(buf + offset);
		offset += sizeof(UINT32);
		//日常任务积分
		m_dailyTaskScore = *(UINT32 *)(buf + offset);
		offset += sizeof(UINT32);
		//领取的积分奖励ID
		UINT8 dailyScoreRewardNum = *(UINT8 *)(buf + offset);
		offset += sizeof(UINT8);
		for (UINT8 i = 0;i < dailyScoreRewardNum;++i){
			m_dailyTaskScoreRewardIDs.insert(*(UINT8 *)(buf + offset));
			offset += sizeof(UINT8);
		}
		//先解析已经完成的任务id
		UINT16 finishNum = *(UINT16 *)(buf + offset);
		offset += sizeof(UINT16);
		for (UINT16 i = 0; i < finishNum ; ++i){
			m_rewardTaskID.insert(*(UINT32*)(buf + offset));
			offset += sizeof(UINT32);
		}

		//解析正在进行中的任务
		UINT16 acceptNum = *(UINT16 *)(buf + offset);
		offset += sizeof(UINT16);
		for (UINT16 i = 0;i < acceptNum; ++i){
			//加载ID
			UINT32 taskID = *(UINT32 *)(buf + offset);
			offset += sizeof(UINT32);
			stTaskDBInfo & taskInfo = m_acceptTask[taskID];
			taskInfo.m_taskID = taskID;
			//加载状态
			taskInfo.m_ts  = *(UINT8 *)(buf + offset);
			offset += sizeof(UINT8);
			//加载任务子状态
			taskInfo.m_subTs = *(UINT8 *)(buf + offset);
			offset += sizeof(UINT8);
			//加载接任务时间
			taskInfo.m_acceptTime =  *(UINT32 *)(buf + offset);
			offset += sizeof(UINT32);
			//任务计数
			UINT8 counterNum = *(UINT8 *)(buf + offset);
			offset += sizeof(UINT8);
			for (UINT8 j = 0;j < counterNum; ++j){
				UINT8 goalID = *(UINT8 *)(buf + offset);
				offset += sizeof(UINT8);
				UINT32 goalValue = *(UINT32 *)(buf + offset);
				offset += sizeof(UINT32);
				taskInfo.m_taskCounter.insert(std::make_pair(goalID,goalValue));
			}
		}
	}
	else {
		offset += pSaveInfo->m_size;
		Athena::logger->warn("account=%s,name=%s,id=%d,上线加载任务数据,版本号不正确!",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID());
	}
#ifdef _HDZ_DEBUG
	Athena::logger->warn("account=%s,name=%s,id=%d,上线加载任务数据,加载了%d个已经完成的任务,%d个正在接的任务",m_pOwner->getAccount(),m_pOwner->getName(),m_pOwner->getID(),m_rewardTaskID.size(),m_acceptTask.size());
#endif
	return offset;
}

UINT16 	CTaskMgr::getAllAcceptTask(std::vector<stTaskDBInfo *> & vecOut)
{
	std::map<UINT32,stTaskDBInfo >::iterator it = m_acceptTask.begin();	
	for (;it != m_acceptTask.end(); ++it){
		vecOut.push_back(&it->second);	
	}
	return m_acceptTask.size();
}

UINT16  CTaskMgr::getAllFinishTask(std::vector<UINT32> & vecOut)
{
	std::set<UINT32>::iterator it = m_rewardTaskID.begin();	
	for (;it != m_rewardTaskID.end() ;++it){
		vecOut.push_back(*it);	
	}
	return m_rewardTaskID.size();
}

UINT8  CTaskMgr::getTaskStatus(UINT32 taskID)
{
	if (m_acceptTask.find(taskID) == m_acceptTask.end()){
		return TaskProto::TS_INVALID;	
	}
	return m_acceptTask[taskID].m_ts;
}

stTaskDBInfo * CTaskMgr::getTaskInfo(UINT32 taskID)
{
	if (m_acceptTask.find(taskID) == m_acceptTask.end()){
		return NULL;	
	}
	return &m_acceptTask[taskID];
}

stTaskDBInfo * CTaskMgr::acceptTask(UINT32 taskID,bool  notify,bool notifyFinish)
{
	stTaskTemplate * pTemplate = CTaskTemplateMgr::getSingleton().getTemplateByID(taskID);
	if (!pTemplate){//找不到模板
		return NULL;
	}

	if (m_rewardTaskID.find(taskID) != m_rewardTaskID.end()){//已经完成的
		return NULL;
	}

	stTaskDBInfo * pTaskInfo = NULL;
	if (pTemplate->canAcceptTask(m_pOwner)){//可以接
		pTaskInfo = &m_acceptTask[taskID];
		pTaskInfo->m_ts = TaskProto::TS_INCOMPLETE;
		pTaskInfo->m_acceptTime = g_gameServer->getSec();
		pTaskInfo->m_taskID = taskID;

		for (UINT16 i = 0;i < pTemplate->m_finishCond.size();++i){
			if (TT_PROPPERTY == pTemplate->m_finishCond[i].m_tt){
				pTaskInfo->m_taskCounter[pTemplate->m_finishCond[i].m_goalID] = m_pOwner->getPropVal(pTemplate->m_finishCond[i].m_propID,pTemplate->m_finishCond[i].m_propParam); 					
			}
			else if (TT_COUNTER == pTemplate->m_finishCond[i].m_tt){
				pTaskInfo->m_taskCounter[pTemplate->m_finishCond[i].m_goalID] = 0;	
			}
		}
		
		bool bFinish = pTemplate->completeTask(m_pOwner,pTaskInfo,notifyFinish);

		//通知增加了任务
		if (notify && (!bFinish)) {
			m_pOwner->taskStatusUpdate(NT_ADD,pTaskInfo);	
		}
#ifdef _HDZ_DEBUG
		/*if (pTemplate->m_taskType == DAILY_TASK){
			Athena::logger->trace("接了一个日常任务taskID=%d!",pTemplate->m_taskID);		
		}*/
#endif
	}
	return pTaskInfo;
}

bool  CTaskMgr::commitTask(UINT32 taskID)
{
	std::map<UINT32,stTaskDBInfo >::iterator it = m_acceptTask.find(taskID);
	if (it == m_acceptTask.end()){
		return false;
	}
	
	stTaskTemplate * pTemplate =  CTaskTemplateMgr::getSingleton().getTemplateByID(taskID);
	if (!pTemplate){//找不到模板
		return false;
	}

	if (TaskProto::TS_COMPLETE != it->second.m_ts){
		return false;
	}
	
	//设置状态 
	it->second.m_ts = TaskProto::TS_REWARD; 
	//先给奖励
	m_pOwner->taskStatusUpdate(NT_REWARD,&it->second);
	m_acceptTask.erase(taskID);

	//if (DAILY_TASK != pTemplate->m_taskType){//日常任务不添加到里面
		m_rewardTaskID.insert(taskID);
	//}

	//处理后续任务
	/*for (UINT16 i = 0;i < pTemplate->m_nextTask.size(); ++i){
		stTaskTemplate * pNextTemplate = CTaskTemplateMgr::getSingleton().getTemplateByID(pTemplate->m_nextTask[i]);
		if (pNextTemplate){
			if (pNextTemplate->canAcceptTask(m_pOwner))	{
				acceptTask(pNextTemplate->m_taskID);	
			}
		}
	}*/	
	return true;	
}

bool  CTaskMgr::cancelTask(UINT32 taskID)
{
	std::map<UINT32,stTaskDBInfo >::iterator it = m_acceptTask.find(taskID);
	if (it == m_acceptTask.end()){
		return false;
	}
	
	if (TaskProto::TS_INCOMPLETE == it->second.m_ts || TaskProto::TS_COMPLETE == it->second.m_ts){
		m_pOwner->taskStatusUpdate(NT_CANCEL,&it->second);		
		m_acceptTask.erase(it);
	}
	return true;	
}

void  CTaskMgr::onGameEvent(UINT32 eventID, UINT32 eventParam[],UINT32 eventParamNum,UINT32 value)
{
	std::stringstream os;
	os << eventID <<"_";
	for (UINT8 i = 0; i < eventParamNum ; ++i){
		os << eventParam[i];
	}

	std::string eventKey = os.str();

	std::map<UINT32,stTaskDBInfo >::iterator it = m_acceptTask.begin();	
	for (;it != m_acceptTask.end(); ++it){
		stTaskDBInfo & taskInfo = it->second;
		if (TaskProto::TS_INCOMPLETE != taskInfo.m_ts){//只有处于已接状态的任务才检查计数
			continue;
		}
		stTaskTemplate * pTemplate =  CTaskTemplateMgr::getSingleton().getTemplateByID(it->first);
		if (!pTemplate){//找不到模板
			continue;
		}
		for (UINT8 i = 0;i <pTemplate->m_finishCond.size();++i){
			if ((eventKey == pTemplate->m_finishCond[i].m_eventKey)
					&&(pTemplate->m_finishCond[i].m_tt == TT_COUNTER)){//增加计数
				taskInfo.m_taskCounter[pTemplate->m_finishCond[i].m_goalID] += value;	
				m_pOwner->taskStatusUpdate(NT_UPDATE,&taskInfo);
				pTemplate->completeTask(m_pOwner,&taskInfo);
			}
		}
	}
}

void  CTaskMgr::onPropChange(UINT16 propID)
{
	std::map<UINT32,stTaskDBInfo >::iterator it = m_acceptTask.begin();	
	for (;it != m_acceptTask.end(); ++it){
		stTaskDBInfo & taskInfo = it->second;
		if (TaskProto::TS_INCOMPLETE != taskInfo.m_ts){//只有处于已接状态才设置属性值
			continue;
		}
		stTaskTemplate * pTemplate =  CTaskTemplateMgr::getSingleton().getTemplateByID(it->first);
		if (!pTemplate){//找不到模板
			continue;
		}
		for (UINT8 i = 0;i <pTemplate->m_finishCond.size();++i){
			if ((propID == pTemplate->m_finishCond[i].m_propID)
					&&(pTemplate->m_finishCond[i].m_tt == TT_PROPPERTY)){//增加计数
				taskInfo.m_taskCounter[pTemplate->m_finishCond[i].m_goalID] = m_pOwner->getPropVal(propID,pTemplate->m_finishCond[i].m_propParam);	
				m_pOwner->taskStatusUpdate(NT_UPDATE,&taskInfo);
				pTemplate->completeTask(m_pOwner,&taskInfo);
			}
		}
	}
}

bool CTaskMgr::isTaskReward(UINT32 taskID)
{
	std::set<UINT32>::iterator it = m_rewardTaskID.find(taskID);
	if (it != m_rewardTaskID.end()){
		return true;
	}
	return false;
}

bool CTaskMgr::isTaskAccept(UINT32 taskID)
{
	if (m_acceptTask.find(taskID) != m_acceptTask.end()) {
		return true;
	}	
	else {
		return false;
	}
}

void CTaskMgr::synAllTask()
{
	TaskProto::stSynFinishTaskID  finishCmd;
	std::set<UINT32>::iterator it = m_rewardTaskID.begin();
	for (;it != m_rewardTaskID.end(); ++it){
		finishCmd.add_finishtaskidlist(*it);
	}
	m_pOwner->sendProto2Me(finishCmd);

	TaskProto::stSynAllAcceptTask   acceptTaskCmd ;
	std::map<UINT32,stTaskDBInfo >::iterator subIt = m_acceptTask.begin();
	for (;subIt != m_acceptTask.end(); ++subIt){
		stTaskDBInfo & taskInfo = subIt->second;
		TaskProto::stSynTaskInfo *pSynTaskInfo =  acceptTaskCmd.add_taskinfolist();
		if (pSynTaskInfo) {
			taskInfo.fillSynInfo(*pSynTaskInfo);
		}
	}
	m_pOwner->sendProto2Me(acceptTaskCmd);

	TaskProto::stDailyTaskScoreInfo scoreInfoCmd;
	scoreInfoCmd.set_dailytaskscore(m_dailyTaskScore);
	std::set<UINT8>::iterator thdIt =  m_dailyTaskScoreRewardIDs.begin();
	for (;thdIt != m_dailyTaskScoreRewardIDs.end();++thdIt){
		scoreInfoCmd.add_rewardids(*thdIt);	
	}
	m_pOwner->sendProto2Me(scoreInfoCmd);
}

void CTaskMgr::userOnline()
{
	if (needRefreshDailyTask()){
		//m_refreshDailyTaskTime = g_gameServer->getSec();
		refreshDailyTask();
	}	
}

bool CTaskMgr::needRefreshDailyTask()
{
	/*if (g_isTheSameDay(m_refreshDailyTaskTime,g_gameServer->getSec())){
		return false;
	}	
	else {
		return true;
	}*/
	UINT32  interval = 0;
	if (0 == m_refreshDailyTaskTime){
		return true;
	}

	interval = g_gameServer->getSec() > m_refreshDailyTaskTime ? g_gameServer->getSec() - m_refreshDailyTaskTime : 0;
	if (interval > 86400){
		return true;
	}
	return false;
}

void CTaskMgr::calcRefreshDailyTaskTime()
{
	struct tm _tm; 
	RealTime::getLocalTime(_tm,g_gameServer->getSec());
	UINT32 curSecOfDay =  _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;
	UINT32 refreshSecOfDay = 5 * 3600;
	if (curSecOfDay > refreshSecOfDay){
		m_refreshDailyTaskTime = g_gameServer->getSec() - (curSecOfDay - refreshSecOfDay);     
	}    
	else {
		m_refreshDailyTaskTime = g_gameServer->getSec() - 86400 + (refreshSecOfDay - curSecOfDay);
	}
}

void CTaskMgr::refreshDailyTask()
{
	calcRefreshDailyTaskTime();
	//m_refreshDailyTaskTime = g_gameServer->getSec();
	std::set<UINT32> &  taskIDs = CTaskTemplateMgr::getSingleton().getDailyTaskIDs();
	std::set<UINT32>::iterator it = taskIDs.begin();

	for (;it != taskIDs.end(); ++it){
		m_rewardTaskID.erase(*it);
		stTaskDBInfo * pTaskInfo = getTaskInfo(*it);
		if (pTaskInfo){
			pTaskInfo->reset(m_pOwner);
			stTaskTemplate *pTemplate =  CTaskTemplateMgr::getSingleton().getTemplateByID(*it);
			if (pTemplate){
				pTemplate->completeTask(m_pOwner,pTaskInfo);
			}
		}
		else {
			acceptTask(*it,false);
		}
	}

	m_dailyTaskScore = 0;
	m_dailyTaskScoreRewardIDs.clear();
}

void CTaskMgr::checkDailyTask()
{
	std::set<UINT32> &  taskIDs = CTaskTemplateMgr::getSingleton().getDailyTaskIDs();
	std::set<UINT32>::iterator it = taskIDs.begin();

	for (;it != taskIDs.end(); ++it){
		stTaskDBInfo * pTaskInfo = getTaskInfo(*it);
		if (pTaskInfo){
		}
		else {
			acceptTask(*it,true);
		}
	}
}

void CTaskMgr::sendAllDailyTask()
{
	std::set<UINT32> &  taskIDs = CTaskTemplateMgr::getSingleton().getDailyTaskIDs();
	std::set<UINT32>::iterator it = taskIDs.begin();
	
	TaskProto::stRefreshAllDailyTask retCmd;
	for (;it != taskIDs.end(); ++it){
		stTaskDBInfo * pTaskInfo = getTaskInfo(*it);
		if (pTaskInfo){
			TaskProto::stSynTaskInfo *pSynTaskInfo =  retCmd.add_tasks();
			if (pSynTaskInfo) {
				pTaskInfo->fillSynInfo(*pSynTaskInfo);
			}
		}
	}
	m_pOwner->sendProto2Me(retCmd);
}

void CTaskMgr::addDailyTaskScore(UINT32 num) 
{
	m_dailyTaskScore += num;   
	TaskProto::stSynDailyTaskScore retCmd;
	retCmd.set_num(m_dailyTaskScore);
	m_pOwner->sendProto2Me(retCmd);
}

bool CTaskMgr::removeDailyTaskScore(UINT32 num) 
{
	if (m_dailyTaskScore < num){
		return false;
	}    
	m_dailyTaskScore -= num; 
	TaskProto::stSynDailyTaskScore retCmd;
	retCmd.set_num(m_dailyTaskScore);
	m_pOwner->sendProto2Me(retCmd);
	return true;
}

void CTaskMgr::gotScoreReward(UINT8 id)
{
	TaskProto::stDailyTaskScoreRewardResult retCmd;
	retCmd.set_id(id);
	
	stDailyTaskScoreReward * pInfo = CTaskTemplateMgr::getSingleton().getScoreRewardInfo(id);
	if (!pInfo){
		retCmd.set_result(TaskProto::stDailyTaskScoreRewardResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	if (m_dailyTaskScore < pInfo->m_socre){
		retCmd.set_result(TaskProto::stDailyTaskScoreRewardResult::eScore_Not_Enough);
		m_pOwner->sendProto2Me(retCmd);
		return ;	
	}

	std::set<UINT8>::iterator it =  m_dailyTaskScoreRewardIDs.find(id);
	if (it != m_dailyTaskScoreRewardIDs.end()){
		retCmd.set_result(TaskProto::stDailyTaskScoreRewardResult::eHas_Got_Reward);
		m_pOwner->sendProto2Me(retCmd);
		return ;	
	}

	if (m_pOwner->m_bagMgr.getMainFreeSlotNum() < m_pOwner->m_boxMgr.needSlotNum(pInfo->m_boxID)){
		retCmd.set_result(TaskProto::stDailyTaskScoreRewardResult::eBag_Full);
		m_pOwner->sendProto2Me(retCmd);
		return ;	
	}

	retCmd.set_result(TaskProto::stDailyTaskScoreRewardResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);
	m_dailyTaskScoreRewardIDs.insert(id);
	m_pOwner->m_boxMgr.openBox(pInfo->m_boxID);
}

bool  CTaskMgr::forceFinish(UINT32 taskID,bool notify)
{
	std::map<UINT32,stTaskDBInfo >::iterator it = m_acceptTask.find(taskID);
	if (it == m_acceptTask.end()){
		return false;
	}
	
	stTaskTemplate * pTemplate =  CTaskTemplateMgr::getSingleton().getTemplateByID(taskID);
	if (!pTemplate){//找不到模板
		return false;
	}
	
	it->second.m_ts = TaskProto::TS_COMPLETE;
	
	m_pOwner->taskStatusUpdate(NT_UPDATE,&it->second);
	return true;
}

bool CTaskMgr::canAcceptRechargeTask(UINT32 taskID)
{
	if (taskID == eMonth_Card_TaskID){//是月卡任务
		if (m_pOwner->m_rmbShop.getItemEndTime(CRMBShop::eItem_Year_Card) > g_gameServer->getSec()){//如果年卡生效,不可以领取
			return false;
		}	
		
		if (m_pOwner->m_rmbShop.getItemEndTime(CRMBShop::eItem_Quarter_Card) > g_gameServer->getSec()){//如果季卡生效,不可以领取
			return false;
		}
		//其它情况都可以接
		return true;
	}
	else if (taskID == eQuarter_Card_TaskID){//季卡任务
		if (m_pOwner->m_rmbShop.getItemEndTime(CRMBShop::eItem_Year_Card) > g_gameServer->getSec()){//如果年卡生效,不可以领取
			return false;
		}
		return true;
	}
	else if (taskID == eYear_Card_TaskID){//年卡任务
		return true;	
	}
	else if (eWelfare_Card_TaskID == taskID){
		return true;
	}
	return true;
}

void CTaskMgr::clearCardTask()
{
	
	cancelTask(eYear_Card_TaskID);
	cancelTask(eQuarter_Card_TaskID);
	cancelTask(eMonth_Card_TaskID);

	m_acceptTask.erase(eYear_Card_TaskID);
	m_acceptTask.erase(eQuarter_Card_TaskID);
	m_acceptTask.erase(eMonth_Card_TaskID);
}
