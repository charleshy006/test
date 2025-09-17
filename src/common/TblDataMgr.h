#ifndef __TBLDATA_MGR_H__
#define __TBLDATA_MGR_H__
#include "TypesDef.h"
#include "TblReader.h"
#include <ext/hash_map>
#include "TblCopyMapBase.h"
#include "TblTaskBase.h"
#include "TblHeroBase.h"
#include "TblRoleBase.h"
#include "TblItemBase.h"
#include "TblSkillBase.h"
#include "TblTaskBase.h"
#include "TblTitleBase.h"
#include "TblRefiming.h"
#include "TblVipBase.h"
#include "TblRefine.h"
#include "TblMonster.h"
#include "TblPrice.h"
#include "TblRobot.h"
#include "TblSprite.h"
#include "TblCloseBetaWelfare.h"
#include "TblBreak.h"
#include "TblMountBase.h"
#include "TblHeroTrain.h"

template <class T>
struct KeyOfTblData:public KeyOfTBLSrcStruct<UINT32,T>
{
    UINT32 operator()(const T& v)
    {   
	        return v.getID();
	}   
};

template <class T>
struct TblDatabaseCB
{
	TblDatabaseCB(){}
	virtual ~TblDatabaseCB(){}
	virtual bool doit(T * ) = 0;
};

template <class TSrc,class TDst>
class TblDataMgr
{
	public :
		bool load(const char * filename)
		{
			int errorCode  = TBL_LOADER_HASHMAP(TSrc,TDst,UINT32,KeyOfTblData<TSrc>)::loadTBL(filename,m_tblDataCtn);
			return errorCode >= 0;	
		}
		TDst * get(UINT32 id)
		{
			typename __gnu_cxx::hash_map<UINT32,TDst *>::iterator it = m_tblDataCtn.find(id);
			if (it != m_tblDataCtn.end()){
				return it->second;
			}
			else{
				return NULL;
			}
		}
		~TblDataMgr()
		{
			clear();
		}
		void clear()
		{
			typename __gnu_cxx::hash_map<UINT32,TDst *>::iterator it = m_tblDataCtn.begin();
			typename __gnu_cxx::hash_map<UINT32,TDst *>::iterator end = m_tblDataCtn.end();

			for (;it != end;++it){
				SAFE_DELETE(it->second);
			}
		}

		void execEveryOne(TblDatabaseCB<TDst> & cb)
		{
			typename __gnu_cxx::hash_map<UINT32,TDst *>::iterator it = m_tblDataCtn.begin();
			typename __gnu_cxx::hash_map<UINT32,TDst *>::iterator end = m_tblDataCtn.end();

			for (;it != end;++it){
				if (!cb.doit(it->second)){
					return ;
				}
			}

		}

		UINT32 size()
		{
			return m_tblDataCtn.size();	
		}
	private :
		__gnu_cxx::hash_map<UINT32,TDst *> m_tblDataCtn;
};

extern TblDataMgr<stRoleRawData,stRoleBase>  g_roleBaseMgr;										//角色管理器
extern TblDataMgr<stPlayGrownRawData,stPlayGrownBase> g_playGrownBaseMgr;						//角色成长管理
extern TblDataMgr<ItemTblRawData,stItemBase> g_itemBaseMgr;                             		//道具基本信息 
extern TblDataMgr<ItemLevelupTblRawData,stItemLevelupBase> g_itemLevelupBaseMgr;        		//道具升级表
extern TblDataMgr<SkillTblRawData,stSkillBase> g_skillBaseMgr;                          		//技能基本信息
extern TblDataMgr<SkillGrownRawData,SkillGrownBase> g_skillGrownBaseMgr;						//技能升级表
extern TblDataMgr<ItemCombineRawData,ItemCombineBase> g_combineBaseMgr;							//合成管理
extern TblDataMgr<CopyMapRawData,stCopyMapBase> g_copyMapBaseMgr;                       		//关卡配置管理器

//extern TblDataMgr<stHeroTblRawData,stHeroBase>  g_heroBaseMgr;								//武将信息管理
extern TblDataMgr<stHeroLvPropTblRawData,stHeroLvPropBase> g_heroLvPropBaseMgr;					//武将等级属性管理器
extern TblDataMgr<stHeroEvolutionRawData,stHeroEvolutionBase> g_heroEvolutionBaseMgr;			//进化管理
extern TblDataMgr<stSkillundRawData,stSkillundBase> g_heroSkillundBaseMgr;						//技能领悟
extern TblDataMgr<TaskRawData,stTaskBase>  g_taskBaseMgr;                               		//任务管理
extern TblDataMgr<stTitleRawData,stTitleBase> g_titleBaseMgr;									//称号管理
extern TblDataMgr<RefimingRawData,stRefimingBase> g_refimingBaseMgr;							//练星管理
extern TblDataMgr<stVipRawData,stVipBase>  g_vipBaseMgr;										//vip次数管理
extern TblDataMgr<RefineRawData,stRefineBase> g_refineBaseMgr;									//精炼管理
extern TblDataMgr<stCharpterRawData,stCharpterBase> g_charpterBaseMgr;							//章节管理
extern TblDataMgr<stMonsterRawData,stMonsterBase> g_monsterBaseMgr;								//怪物管理
extern TblDataMgr<stSoullinkRawData,stSoulinkBase> g_soulinkBaseMgr;							//锁链套装管理器
extern TblDataMgr<stPriceRawData,stPriceBase>  g_priceBaseMgr;									//价格配置管理
extern TblDataMgr<RobotRawData,stRobotBase>	  g_robotBaseMgr;									//机器人配置
extern TblDataMgr<NpcAIRawData,NpcAIBase>	  g_npcAIBaseMgr;									//NpcAI基本数据管理
extern TblDataMgr<SpriteRawData,stSpriteBase> g_spriteBaseMgr;									//精灵管理系统
extern TblDataMgr<BufferRawData,BufferBase>	  g_bufferBaseMgr;									//buff管理系统
extern TblDataMgr<stRefineEffectRawData,stRefineEffectBase> g_refineEffectBaseMgr;				//精炼特效管理系统
extern TblDataMgr<CloseBetaWelfareRawData,stCloseBetaWelfareBase> g_closeBetaWelfareBaseMgr;	//封测福利
extern TblDataMgr<BreakRawData,stBreakBase> g_breakBaseMgr;										//突破管理
extern TblDataMgr<IllusionismRawData,stIllusionismBase> g_illusionismBaseMgr;           		//精灵幻化管理系统
extern TblDataMgr<stMountgrowRawData, stMountgrowBase> g_stMountGrowBaseMgr;           			//坐骑升级管理系统
extern TblDataMgr<stMountIllusionRawData, stMountIllusionBase> g_stMountIllusionBaseMgr;		//坐骑幻化管理系统
extern TblDataMgr<stHeroTrainRawData,stHeroTrain> g_heroTrainExpMgr;					//随从调教升级经验信息 

bool g_loadAllTbl();
void g_unloadAllTbl();
bool g_webLoadAllTbl();
bool g_globalLoadTbl();
#endif
