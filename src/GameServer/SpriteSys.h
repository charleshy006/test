#ifndef __H_SPRITESYS_H__
#define __H_SPRITESYS_H__
#include "TypesDef.h"
#include "TblDataMgr.h"
#include "Misc.pb.h"
#include <set>
#include <map>
#pragma pack(1)
struct stSpriteDBInfo
{
	UINT32 m_id;
	UINT32 m_level;

	stSpriteDBInfo()
	{
		m_id = 0;
		m_level = 0;
	}
};

struct stSpriteSaveInfo
{
	UINT32 m_version;				//版本号
	UINT32 m_followSprite;			//跟随精灵
	UINT16 m_num;

	stSpriteDBInfo m_data[0];

	stSpriteSaveInfo()
	{
		m_version = 0;
		m_followSprite = 0;
		m_num = 0;
	}
};
#pragma pack()

struct stSpriteInfo
{
	UINT32 m_id;
	UINT16 m_level;
	stSpriteBase * m_pSpriteBase;
    //幻化新加
    UINT16 m_illusionLvl;   				//幻化等级
    UINT16 m_illusionExp;   				//幻化当前经验
    UINT32 m_talentNum;     				//当前天赋点
    std::map<UINT32, UINT16> m_skillMap;    //当前技能信息表

	stSpriteInfo()
	{
		m_id = 0;
		m_level = 0;
		m_pSpriteBase = NULL;
        //幻化新加
        m_illusionLvl = 0;
        m_illusionExp = 0;
        m_talentNum = 0;
	}

	bool init();

	void fillSyn(MiscProto::stSynSpriteInfo * pSyn);
	//加载数据
	UINT32 load(byte * buf);
	//保存数据
	UINT32 save(byte * buf, UINT32 needSpace);
};

class GameUser;
class CSpriteSysMgr
{
public :
	static const UINT32 s_SPRITE_SAVE_VERSION = 20150728;
	static const UINT32 s_SECOND_SPRITE_SAVE_VERSION = 2016818;//新增精灵幻化
public :
	//构造
	CSpriteSysMgr(GameUser * pOwner);
	//析构
	~CSpriteSysMgr();
	//保存称号
	UINT32 saveData(byte * buf,UINT32 needSpace);
	//加载称号
	UINT32 loadData(byte * buf);
	//增加角色属性值
	void addProp();
	//玩家上线
	void userOnline();
	//解锁精灵
	void unlockedSprite(UINT32 spriteID);
	//升级精灵
	void levelupSprite(UINT32 spriteID);
	//跟随主人
	void followMaster(UINT32 spriteID);
    //发送精灵幻化信息
    void sendIllusionSprite(UINT32 spriteID);
    //精灵幻化喂养
    void illusionSpriteEat(MiscProto::stReqIllusionSpriteEat * pCmd);
    //精灵幻化技能升级
    void illusionSpriteSkillUp(UINT32 spriteID, UINT32 skillId);
    //获取跟随精灵id
    UINT32 getFollowSprite() {
        return m_followSpriteID;
    }
    //根据精灵id获取等级
    UINT16 getSpriteLvl(UINT32 id);
private :
	GameUser * m_pOwner;
	UINT32     m_followSpriteID;				//跟随精灵ID
	std::map<UINT32,stSpriteInfo> m_sprites;	//精灵
};
#endif
