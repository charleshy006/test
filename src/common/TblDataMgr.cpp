#include "TblDataMgr.h" 
#include "Log4cxxHelp.h"
#include "UtilFunction.h"
#include "KVPair.h"

#undef LOADTBLFILE
#define LOADTBLFILE(mgr,filename) do {\
	std::string tmpFileName = Athena::global["tblResDir"] + "/" ;\
	tmpFileName += filename;\
	if (!mgr.load(tmpFileName.c_str())){\
		Athena::logger->error("[加载表格]加载%s出现错误",tmpFileName.c_str());\
		return false;\
	}\
}while(false);

TblDataMgr<stRoleRawData,stRoleBase>  g_roleBaseMgr;								//角色管理器
TblDataMgr<stPlayGrownRawData,stPlayGrownBase> g_playGrownBaseMgr;				//角色成长管理
TblDataMgr<ItemTblRawData,stItemBase> g_itemBaseMgr;                            //道具基本信息 
TblDataMgr<ItemLevelupTblRawData,stItemLevelupBase> g_itemLevelupBaseMgr;       //道具升级表
TblDataMgr<SkillTblRawData,stSkillBase> g_skillBaseMgr;                         //技能基本信息
TblDataMgr<SkillGrownRawData,SkillGrownBase> g_skillGrownBaseMgr;				//技能升级表
TblDataMgr<ItemCombineRawData,ItemCombineBase> g_combineBaseMgr;				//合成管理
TblDataMgr<CopyMapRawData,stCopyMapBase> g_copyMapBaseMgr;                      //关卡配置管理器

//TblDataMgr<stHeroTblRawData,stHeroBase>  g_heroBaseMgr;							//武将信息管理
TblDataMgr<stHeroLvPropTblRawData,stHeroLvPropBase> g_heroLvPropBaseMgr;		//武将等级属性管理器
TblDataMgr<stHeroEvolutionRawData,stHeroEvolutionBase> g_heroEvolutionBaseMgr;	//进化管理
TblDataMgr<stSkillundRawData,stSkillundBase> g_heroSkillundBaseMgr;				//技能领悟

TblDataMgr<TaskRawData,stTaskBase>  g_taskBaseMgr;                              //任务管理
TblDataMgr<stTitleRawData,stTitleBase> g_titleBaseMgr;							//称号管理
TblDataMgr<RefimingRawData,stRefimingBase> g_refimingBaseMgr;					//练星管理
TblDataMgr<stVipRawData,stVipBase>  g_vipBaseMgr;								//vip次数管理
TblDataMgr<RefineRawData,stRefineBase> g_refineBaseMgr;							//精炼管理
TblDataMgr<stCharpterRawData,stCharpterBase> g_charpterBaseMgr;					//章节管理
TblDataMgr<stMonsterRawData,stMonsterBase> g_monsterBaseMgr;					//怪物管理
TblDataMgr<stSoullinkRawData,stSoulinkBase> g_soulinkBaseMgr;					//锁链套装管理器
TblDataMgr<stPriceRawData,stPriceBase>  g_priceBaseMgr;							//价格配置管理
TblDataMgr<RobotRawData,stRobotBase>	g_robotBaseMgr;							//机器人配置
TblDataMgr<NpcAIRawData,NpcAIBase>	  g_npcAIBaseMgr;							//NpcAI基本数据管理
TblDataMgr<SpriteRawData,stSpriteBase> g_spriteBaseMgr;							//精灵管理系统
TblDataMgr<BufferRawData,BufferBase>	  g_bufferBaseMgr;						//buff管理系统
TblDataMgr<stRefineEffectRawData,stRefineEffectBase> g_refineEffectBaseMgr;		//精炼特效管理系统
TblDataMgr<CloseBetaWelfareRawData,stCloseBetaWelfareBase> g_closeBetaWelfareBaseMgr; //封测福利
TblDataMgr<BreakRawData,stBreakBase> g_breakBaseMgr;							//突破管理
TblDataMgr<IllusionismRawData,stIllusionismBase> g_illusionismBaseMgr;          //精灵幻化管理系统
TblDataMgr<stMountgrowRawData, stMountgrowBase> g_stMountGrowBaseMgr;			//坐骑升级管理系统
TblDataMgr<stMountIllusionRawData, stMountIllusionBase> g_stMountIllusionBaseMgr;//坐骑幻化管理系统
TblDataMgr<stHeroTrainRawData,stHeroTrain> g_heroTrainExpMgr;					//随从调教升级经验信息 

bool g_loadAllTbl()
{
	LOADTBLFILE(g_roleBaseMgr,"RoleBase.tbl");
	LOADTBLFILE(g_playGrownBaseMgr,"CharLvInfo.tbl");
	LOADTBLFILE(g_itemBaseMgr,"ItemBase.tbl");
	LOADTBLFILE(g_itemLevelupBaseMgr,"ItemGrowBase.tbl");
	LOADTBLFILE(g_skillBaseMgr,"SkillBase.tbl");
	LOADTBLFILE(g_skillGrownBaseMgr,"SkillGrownBase.tbl");
	LOADTBLFILE(g_combineBaseMgr,"SynthesisBase.tbl");
	LOADTBLFILE(g_copyMapBaseMgr,"CopyMapBase.tbl");
	
	//LOADTBLFILE(g_heroBaseMgr,"PetBase.tbl");
	LOADTBLFILE(g_heroLvPropBaseMgr,"PetGrownBase.tbl");
	LOADTBLFILE(g_heroEvolutionBaseMgr,"PetEvolutionBase.tbl");
	LOADTBLFILE(g_heroSkillundBaseMgr,"PetSkillundBase.tbl");
	LOADTBLFILE(g_taskBaseMgr,"TaskBase.tbl");
	LOADTBLFILE(g_titleBaseMgr,"TitleBase.tbl");
	LOADTBLFILE(g_refimingBaseMgr,"RefiningBase.tbl");
	LOADTBLFILE(g_vipBaseMgr,"VipBase.tbl");
	LOADTBLFILE(g_refineBaseMgr,"RefineBase.tbl");
	LOADTBLFILE(g_charpterBaseMgr,"ChapterBase.tbl");
	LOADTBLFILE(g_monsterBaseMgr,"MonstergrowBase.tbl");
	LOADTBLFILE(g_soulinkBaseMgr,"SoullinkBase.tbl");
	LOADTBLFILE(g_priceBaseMgr,"PriceBase.tbl");
	LOADTBLFILE(g_robotBaseMgr,"RobotBase.tbl");
	LOADTBLFILE(g_npcAIBaseMgr,"AIBase.tbl");
	LOADTBLFILE(g_spriteBaseMgr,"SpriteBase.tbl");
	LOADTBLFILE(g_bufferBaseMgr,"BuffBase.tbl");
	LOADTBLFILE(g_refineEffectBaseMgr,"RefineEffectBase.tbl");
	LOADTBLFILE(g_closeBetaWelfareBaseMgr,"CloseBetaWelfareBase.tbl");
	LOADTBLFILE(g_breakBaseMgr,"ItemBreachBase.tbl");
	LOADTBLFILE(g_illusionismBaseMgr,"IllusionismBase.tbl");
	LOADTBLFILE(g_stMountGrowBaseMgr,"mountgrowBase.tbl");
	LOADTBLFILE(g_stMountIllusionBaseMgr,"mountIllusionBase.tbl");
	LOADTBLFILE(g_heroTrainExpMgr,"PetDiscplineBase.tbl");
	return true;
}

bool g_webLoadAllTbl()
{
	LOADTBLFILE(g_itemBaseMgr,"ItemBase.tbl");
	LOADTBLFILE(g_roleBaseMgr,"RoleBase.tbl");
	LOADTBLFILE(g_copyMapBaseMgr,"CopyMapBase.tbl");
	return true;
}

bool g_globalLoadTbl()
{
	LOADTBLFILE(g_robotBaseMgr,"RobotBase.tbl");
	LOADTBLFILE(g_monsterBaseMgr,"MonstergrowBase.tbl");
	LOADTBLFILE(g_copyMapBaseMgr,"CopyMapBase.tbl");
	LOADTBLFILE(g_vipBaseMgr,"VipBase.tbl");
	LOADTBLFILE(g_closeBetaWelfareBaseMgr,"CloseBetaWelfareBase.tbl");
	return true;
}

void g_unloadAllTbl()
{
	g_roleBaseMgr.clear();
	g_playGrownBaseMgr.clear();
	g_itemBaseMgr.clear();
	g_itemLevelupBaseMgr.clear();
	g_skillBaseMgr.clear();
	g_skillGrownBaseMgr.clear();
	g_combineBaseMgr.clear();
	g_copyMapBaseMgr.clear();

	//g_heroBaseMgr.clear();
	g_heroLvPropBaseMgr.clear();
	g_heroEvolutionBaseMgr.clear();
	g_heroSkillundBaseMgr.clear();
	g_taskBaseMgr.clear();
	g_titleBaseMgr.clear();
	g_refimingBaseMgr.clear();
	g_vipBaseMgr.clear();
	g_refineBaseMgr.clear();
	g_charpterBaseMgr.clear();
	g_soulinkBaseMgr.clear();
	g_priceBaseMgr.clear();
	g_npcAIBaseMgr.clear();
	g_spriteBaseMgr.clear();
	g_bufferBaseMgr.clear();
	g_refineEffectBaseMgr.clear();
	g_breakBaseMgr.clear();
	g_illusionismBaseMgr.clear();
	g_stMountGrowBaseMgr.clear();
	g_stMountIllusionBaseMgr.clear();
	g_heroTrainExpMgr.clear();
}
