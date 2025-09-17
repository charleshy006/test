#ifndef __H_HERO_INFO_H__
#define __H_HERO_INFO_H__
#include "TypesDef.h"
#include "TblDataMgr.h"
#include "SkillInfo.h"
#include "Being.h"
#include "UniqueID.h"
#include "Map.pb.h"
#include "HeroBaseInfo.h"
#include "Role.pb.h"
#include <map>
#include <vector>
#include <set>

class GameUser ;
class HeroInfoMgr;

//领悟技能的位置
enum
{
	HERO_SKILL_POS1 = 1,		//位置1
	HERO_SKILL_POS2 = 2,		//位置2
	HERO_SKILL_POS3 = 3,		//位置3
	HERO_SKILL_POS4 = 4,		//位置4
	HERO_SKILL_POS5 = 5,		//位置5
	HERO_SKILL_POS6 = 6,		//位置6
	MAX_HERO_SKILL_POS = 6, 	
};

enum
{
	eHERO_BATTLE_POS1 = 1,		//出战位置1
	eHERO_BATTLE_POS2 = 2,		//出战位置2
	eHERO_BATTLE_POS3 = 3,		//出战位置3
	eHERO_BATTLE_POS4 = 4,		//出战位置4
	eHERO_MAX_BATTLE_NUM = 4,	//最大出战数量	
};

class GameUser;
class HeroInfo
{
public :
	//构造函数
	HeroInfo(HeroInfoMgr  * pHeroInfoMgr);
	//析构函数
	~HeroInfo();
	//出事化
	bool init(bool clearall=false);
	//填充数据
	void fillSynInfo(GameUser * pOwner, Role::stSynHeroInfo & heroInfo);
	//加载数据
	UINT32 load(byte * buf, UINT32 version);
	//保存数据
	UINT32 save(byte * buf,UINT32 needSpace);
	//升级
	void levelup();
	//进化
	void evolution();
	//替换某个位置的领悟技能
	void replaceSkill(UINT8 pos,UINT32 skillID);
	//获得某个位置的技能
	stHeroSkillInfo getSkillIDByPos(UINT8 pos);	
	//升级技能
	void upgradeSkill(UINT32 skillID, UINT32 maxSkillLv);
	//设置领悟技能
	void setStudySkillID(UINT32 skillID) { m_dbData.m_studySkillID = skillID;}
	//返回领悟技能ID
	UINT32 getStudySkillID() { return m_dbData.m_studySkillID ; }
	//增加一个主动技能
	bool addActiveSkill(UINT32 skillID);
	//主动技能是否已经存在了
	bool isActiveSkillExist(UINT32 skillID);
	//提升主动技能的等级
	void upgradeActiveSkill(UINT32 skillID);
	//获得主动技能的等级
	UINT16 getActiviSkillLv(UINT32 skillID);
	//检查主动技能解锁
	void checkSkillUnlock();
	//是否存在领悟技能
	bool isExistSkill(UINT32 skillID);
	//重置
	void reset(bool clearall=false);
    //获取符文增加的属性
    void getRuneAdd(GameUser * pOwner, UINT32 & runeBattlePower, std::vector<UINT32> & runeBattlePropVec, bool getAllAdd);
    //是否拥有该符文
    bool hasRune(UINT16 slot);
public :
	stHeroDBData m_dbData;					//存档数据
    stHeroRuneDBData m_runeDbData;          //装备符文数据
	stRoleBase  * m_pHeroBase;				//武将基础值
	stHeroLvPropBase * m_pHeroLvPropBase;	//武将等级属性
	stHeroEvolutionBase * m_pEvolutionBase; //进化属性表
	std::map<UINT8,stHeroSkillInfo> m_skillCnt;	//技能,领悟技能
	UINT32 m_battlePower;					//战力
	UINT32 m_maxHP;							//最大血量
	UINT32 m_damage;						//伤害
	std::vector<UINT32 > m_addOwnerBattleProps;     //给玩家增加战斗属性
	std::vector<UINT32 > m_addHeroBattleProps;	//给宠物增加属性
	std::vector<UINT32>  m_battleProps;
	std::vector<stEvolutionAddProp> m_addProps;	//进化受影响的宠物属性
	std::vector<UINT32>  m_impactProps;			//羁绊影响的属性, reset时候不重置
	GameUser * m_pOwner;
};

struct stHeorInfoCBBase
{
	stHeorInfoCBBase()
	{
	
	}

	virtual ~stHeorInfoCBBase()
	{
	
	}

	virtual  bool doIt(HeroInfo * pHeroInfo) = 0;
};

struct stHeroInfoCBGetLv : public stHeorInfoCBBase
{
	UINT16 m_num;
	UINT16 m_lv ;

	stHeroInfoCBGetLv(UINT16 lv) : m_num(0), m_lv(lv)
	{
	
	}

	virtual  bool doIt(HeroInfo * pHeroInfo)
	{
		if (pHeroInfo){
			if (pHeroInfo->m_dbData.m_level >= m_lv){
				m_num += 1;
			}
		}
		return true;
	}
};

struct stHeroInfoCBEvolution : public stHeorInfoCBBase
{
	UINT16 m_num;
	UINT16 m_lv ;

	stHeroInfoCBEvolution(UINT16 lv) : m_num(0), m_lv(lv)
	{
		
	}

	virtual  bool doIt(HeroInfo * pHeroInfo)
	{
		if (pHeroInfo){
			if (pHeroInfo->m_dbData.m_evolutionLv >= m_lv){
				m_num += 1;
			}
		}	
		return true;
	}
};

class HeroInfoMgr
{
public :
	static const UINT32 s_HERO_SAVE_VERSION = 20150922;				//宠物版本数据
	static const UINT32 s_HERO_SAVE_VERSION_SECOND = 20161124;		//宠物版本数据
	static const UINT32 s_COMBINE_NEED_STONE_NUM = 10;				//合成需要石头数量
	static const UINT32 s_MAX_EVOLUTION_LEVEL = 13;					//最大进化等级
	static const UINT32 s_MAX_BATTLE_HERO_NUM = 4;					//出战武将的数量
	static const UINT32 s_CHANGE_HERO_CD = 30;						//切换宠物冷却时间
public :
	//构造函数
	HeroInfoMgr(GameUser * pOwner);
	//析构函数
	~HeroInfoMgr();
	//加载数据
	UINT32 loadAllHero(byte * buf);
	//保存数据
	UINT32 saveAllHero(byte * buf,UINT32 needSpace);
	//同步所有英雄数据给客户端
	void synAllHero();
	//重算英雄属性
	void calcAllHero();
	//通知增加一个武将
	void notifyAddHero(HeroInfo * pHeroInfo);
	//刷新武将数据 
	void refreshHero(HeroInfo * pHeroInfo);
	//刷新阵上武将进化对其他上阵武将的影响
	void refreshEvolutionHero(UINT32 heroID);
	//刷新受影响武将数据
	void refreshImpactHero(UINT32 heroID, bool isFight = false, bool onlyAffectsOthers = false, bool isUp = false, bool send = true);
	//创建武将
	HeroInfo * createHero(UINT32 heroID,UINT16 lv);
	//添加一个武将
	bool addHeroInfo(HeroInfo * pHeroInfo);
	//获得武将
	HeroInfo * getHeroInfoByID(UINT32 heroID);
	//获得属主
	GameUser * getOwner() { return m_pOwner ; }
	//添加出战武将
	bool addBattleHero(UINT32 heroID,UINT8 pos);
	//取消出战武将
	bool delBattleHero(UINT32 heroID,UINT8 pos);
	//是否在出战状态
	bool isBattle(UINT32 heroID);
	//获得出战宠物所在位置
	UINT8 getBattlePos(UINT32 heroID);
	//出战数量
	UINT16 battleHeroNum();
	//增加技能经验
	bool   addSkillExp(UINT32 num,const char * desc);
	//扣除经验
	bool   removeSkillExp(UINT32 num,const char * desc);
	//获得宠物技能经验
	UINT32 getSkillExp() {  return m_skillExp; }
	//填充我的武将信息
	template <class T>
	void fillMyHero(T * pDetailInfo);
	//召唤武神
	void summonHero();
	//对每个宠物回调
	void exeEveryOne(stHeorInfoCBBase & cb);
	//获得出战宠物的战力和
	UINT32 getBattlePower();
	//计算给主人的加成
	void addProp2Owner();
	//交换两个宠物的位置
	bool swapBattlePos(UINT32 srcHeroID,UINT32 destHeroID);
	//切换宠物
	void changeHero(UINT8 group);
	//某个组是否处于出战状态
	bool isGroupGoOut(UINT8 group);
	//是否处于冷却时间
	bool isColdDown();
	//设置冷却时间
	void setColdDown();
	//当前的组ID
	UINT8 getCurGroupID()
	{
		return m_curGroupID;
	}
	//更新某个出战宠物的血量
	void updateBattleHeroHP(UINT32 heroID,UINT32 hp);
	//获得某组的出战位置
	static UINT8 getBattlePosByGroup(UINT8 group,UINT8 index)
	{
		if ((group < 2) && (index < 2)){
			return g_goOutGroup[group][index];
		}
		else {
			return g_goOutGroup[0][0];
		}
	}
	//擂台替换上阵宠物
	void useLeitaiBattleHero();
	//还原擂台上阵宠物
	void restoreLeitaiBattleHero();
	// 获取战斗宝宝id
	UINT32 getBattleHeroID(UINT32 pos);
	// 获取战斗宝宝血量
	UINT32 getBattleHeroHP(UINT32 pos);
	//校验是否调教升级
	bool checkTrainLevelUp(HeroInfo* pHeroInfo);
	//调教升级
	void onTrainLevelUp(HeroInfo* pHeroInfo, UINT32 lvl);
	//获取调教升级所需经验
	UINT32 GetHeroTrainLevelUpExp(UINT32 heroID, UINT32 lv);
private :
	std::map<UINT32,HeroInfo *> m_allHero;
	typedef std::map<UINT32,HeroInfo *>::iterator Hero_IT;
	GameUser * m_pOwner;
	UINT32 m_skillExp;											//技能经验
	UINT32 m_aBattleHero[s_MAX_BATTLE_HERO_NUM];				//出战武将
	static UINT8 g_goOutGroup[2][2];							//出战位置
	UINT32  m_lastChgHeroTime;									//最近一次切换宠物时间
	UINT8   m_curGroupID;										//当前组ID
	UINT32  m_aBattleHeroHP[s_MAX_BATTLE_HERO_NUM];				//出战宠物的血量
	UINT32 m_aBattleHero_bak[s_MAX_BATTLE_HERO_NUM];			//出战武将备份
};

template <class T>
void HeroInfoMgr::fillMyHero(T * pDetailInfo)
{
	if (!pDetailInfo){
		return ;
	}	
	
	for (UINT8 i = 0;i < s_MAX_BATTLE_HERO_NUM; ++i){
		HeroInfo *pHeroInfo =  getHeroInfoByID(m_aBattleHero[i]);
		if (pHeroInfo){
			Role::stSynHeroInfo * pSynInfo = pDetailInfo->add_herodata();
			if (pSynInfo){
				pHeroInfo->fillSynInfo(m_pOwner, *pSynInfo);	
			}
		}
	}
}
#endif
