#include "SpriteSys.h"
#include "GameUser.h"
#include "Misc.pb.h"
#include "TblSkillBase.h"
#include "TblRoleBase.h"

void stSpriteInfo::fillSyn(MiscProto::stSynSpriteInfo * pSyn)
{
	pSyn->set_spriteid(m_id);
	pSyn->set_level(m_level);
}

bool stSpriteInfo::init()
{
	m_pSpriteBase = g_spriteBaseMgr.get(stSpriteBase::getFindID(m_id,m_level));	
	if (!m_pSpriteBase){
		return false;
	}
	return true;
}

UINT32 stSpriteInfo::load(byte * buf) {
	UINT32 ret = 0;
    m_id = *(UINT32*)(buf);
    ret += sizeof(UINT32);
    m_level = *(UINT16*)(buf + ret);
    ret += sizeof(UINT16);
    m_illusionLvl = *(UINT16*)(buf + ret);
    ret += sizeof(UINT16);
    m_illusionExp = *(UINT16*)(buf + ret);
    ret += sizeof(UINT16);
    m_talentNum = *(UINT32*)(buf + ret);
    ret += sizeof(UINT32);
    UINT8 num = *(UINT8*)(buf + ret);
    ret += sizeof(UINT8);
    UINT32 skillId = 0;
    UINT16 skillLvl = 0;
    for (UINT8 i = 0; i < num; i++) {
        skillId = *(UINT32 *)(buf + ret);
        ret += sizeof(UINT32);
        skillLvl = *(UINT16 *)(buf + ret);
        ret += sizeof(UINT16);
        m_skillMap.insert(std::make_pair(skillId, skillLvl));
    }
    return ret;
}

UINT32 stSpriteInfo::save(byte * buf, UINT32 needSpace) {
    UINT32 ret = 0;
    *(UINT32 *)(buf + ret) = m_id;
    ret += sizeof(UINT32);
    *(UINT16 *)(buf + ret) = m_level;
    ret += sizeof(UINT16);
    *(UINT16 *)(buf + ret) = m_illusionLvl;
    ret += sizeof(UINT16);
    *(UINT16 *)(buf + ret) = m_illusionExp;
    ret += sizeof(UINT16);
    *(UINT32 *)(buf + ret) = m_talentNum;
    ret += sizeof(UINT32);
    *(UINT8 *)(buf + ret) = (UINT8)m_skillMap.size();
    ret += sizeof(UINT8);
    for (std::map<UINT32, UINT16>::iterator it = m_skillMap.begin();
            it != m_skillMap.end(); it++) {
        *(UINT32 *)(buf + ret) = it->first;
        ret += sizeof(UINT32);
        *(UINT16 *)(buf + ret) = it->second;
        ret += sizeof(UINT16);
    }
    return ret;
}

CSpriteSysMgr::CSpriteSysMgr(GameUser * pOwner) : m_pOwner(pOwner)
{
	m_followSpriteID = 0;
}

CSpriteSysMgr::~CSpriteSysMgr()
{
	
}
	
UINT32 CSpriteSysMgr::saveData(byte * buf,UINT32 needSpace)
{
    /*
	UINT32 ret = 0;
	stSpriteSaveInfo * pSaveInfo = (stSpriteSaveInfo *) buf;
	ret += sizeof(stSpriteSaveInfo);
	
	pSaveInfo->m_version = s_SPRITE_SAVE_VERSION;
	pSaveInfo->m_followSprite = m_followSpriteID;
	pSaveInfo->m_num = 0;
	
	std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.begin();
	
	for (;it != m_sprites.end();++it){
		stSpriteInfo & sprite = it->second;
		pSaveInfo->m_data[pSaveInfo->m_num].m_id = sprite.m_id;
		pSaveInfo->m_data[pSaveInfo->m_num].m_level = sprite.m_level;
		pSaveInfo->m_num += 1;
		ret += sizeof(stSpriteDBInfo);
	}
    */
	UINT32 ret = 0;
    *(UINT32 *)(buf + ret) = s_SECOND_SPRITE_SAVE_VERSION;
    ret += sizeof(UINT32);
    *(UINT32 *)(buf + ret) = m_followSpriteID;
    ret += sizeof(UINT32);
    *(UINT8 *)(buf + ret) = m_sprites.size();
    ret += sizeof(UINT8);
    UINT32 size = 0;
    for (std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.begin(); it != m_sprites.end(); it++) {
        size = it->second.save(buf + ret, needSpace);
        ret += size;
    }

	return ret;
}

UINT32 CSpriteSysMgr::loadData(byte * buf)
{
	UINT32 ret = 0;
    UINT32 version = *(UINT32*)(buf);
    if (s_SPRITE_SAVE_VERSION == version) {
        stSpriteSaveInfo * pSaveInfo = (stSpriteSaveInfo *) buf;
        ret += sizeof(stSpriteSaveInfo);

        if (s_SPRITE_SAVE_VERSION == pSaveInfo->m_version){
            m_followSpriteID = pSaveInfo->m_followSprite;
            
            for (UINT16 i = 0;i < pSaveInfo->m_num;++i){
                stSpriteDBInfo & spriteDB = pSaveInfo->m_data[i];
                stSpriteInfo  sprite;
                sprite.m_id = spriteDB.m_id;
                sprite.m_level = spriteDB.m_level;
                if (!sprite.init()){
                    ret += sizeof(stSpriteDBInfo);
                    continue;
                }
                m_sprites[sprite.m_id] = sprite;
                ret += sizeof(stSpriteDBInfo);
            }
        }
    } else if (s_SECOND_SPRITE_SAVE_VERSION == version) {
        ret += sizeof(UINT32);
        m_followSpriteID = *(UINT32*)(buf + ret);
        ret += sizeof(UINT32);
        UINT8 num = *(UINT8*)(buf + ret);
        ret += sizeof(UINT8);
        UINT32 size = 0;
        for (UINT8 i = 0; i < num; i++) {
            stSpriteInfo sprite;
            size = sprite.load(buf + ret);
            ret += size;
            if (sprite.init()) {
                m_sprites.insert(std::make_pair(sprite.m_id, sprite));
            }
        }
    }
#ifdef _HDZ_DEBUG
	Athena::logger->trace("上线加载了%d个精灵!",m_sprites.size());
#endif
	return ret;
}

void CSpriteSysMgr::addProp()
{
	stRoleState & roleState = m_pOwner->m_roleState;
	
	std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.begin();
	for (;it != m_sprites.end();++it){
		stSpriteInfo & sprite = it->second;
		if (sprite.m_pSpriteBase){
			for (UINT8 i =0;i < sprite.m_pSpriteBase->m_addPropsVec.size();++i){
				std::pair<UINT8,UINT32> & prop = sprite.m_pSpriteBase->m_addPropsVec[i];
				if (prop.first < eMAX_BATTLE_PROP){
					roleState.m_battleProps[prop.first] += prop.second; 
				}
			}
		}
        if (m_followSpriteID == it->first) {
            //幻化精灵技能加成
            for (std::map<UINT32, UINT16>::iterator it = sprite.m_skillMap.begin();
                    it != sprite.m_skillMap.end(); it++) {
                SkillGrownBase * pSkillGrown = g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(it->first, it->second));
                if (pSkillGrown && pSkillGrown->m_battleProp.m_propID < eMAX_BATTLE_PROP){
                    roleState.m_battleProps[pSkillGrown->m_battleProp.m_propID] += pSkillGrown->m_battleProp.m_val1;
                }
            }
        }
	}
}

void CSpriteSysMgr::userOnline()
{
	MiscProto::stSynAllSprite cmd;
	std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.begin();
	for (;it != m_sprites.end();++it){
		stSpriteInfo & sprite = it->second;

		MiscProto::stSynSpriteInfo * pSyn = cmd.add_data();
		if (pSyn){
			pSyn->set_spriteid(sprite.m_id);
			pSyn->set_level(sprite.m_level);
		}
	}
	m_pOwner->sendProto2Me(cmd);
}

void CSpriteSysMgr::unlockedSprite(UINT32 spriteID)
{
	MiscProto::stUnlockedSpriteResult retCmd;
	stSpriteBase * pSpriteBase = g_spriteBaseMgr.get(stSpriteBase::getFindID(spriteID,0));	
	retCmd.set_spriteid(spriteID);

	if (!pSpriteBase){
		retCmd.set_result(MiscProto::stUnlockedSpriteResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	for (UINT8 i = 0;i < pSpriteBase->m_unlockedsVec.size();++i){
		std::pair<UINT8,UINT32> & one = pSpriteBase->m_unlockedsVec[i];
		if (SPRITE_UNLOCKED_ROLE_LV == one.first){
			if (m_pOwner->getLevel() < one.second){//等级不足
				retCmd.set_result(MiscProto::stUnlockedSpriteResult::eLevel_Not_Meet);
				m_pOwner->sendProto2Me(retCmd);
				return ;
			}
		}
		else if (SPRITE_UNLOCKED_VIP_LV == one.first){//vip等级不足
			if (m_pOwner->getVipLv() < one.second){
				retCmd.set_result(MiscProto::stUnlockedSpriteResult::eVipLv_Not_Meet);
				m_pOwner->sendProto2Me(retCmd);
				return ;
			}	
		}
	}

	stSpriteInfo sprite;
	sprite.m_id = spriteID;
	sprite.m_level = 0;
	if (!sprite.init()){
		Athena::logger->error("解锁精灵不成功,没有0级别的配置");
		return ;
	}
	m_sprites.insert(std::make_pair(sprite.m_id,sprite));
	
	MiscProto::stAddSprite addCmd;
	sprite.fillSyn(addCmd.mutable_sprite());	
	m_pOwner->sendProto2Me(addCmd);
	
	retCmd.set_result(MiscProto::stUnlockedSpriteResult::eSuccess);
	m_pOwner->sendProto2Me(retCmd);

	m_pOwner->setupRoleState();
	//m_pOwner->sendMainData2Me();
	m_pOwner->sendBattleProp();
}

void CSpriteSysMgr::levelupSprite(UINT32 spriteID)
{
	MiscProto::stLevelupSpriteResult retCmd;
	retCmd.set_spriteid(spriteID);
	std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.find(spriteID);

	if (it == m_sprites.end()){
		retCmd.set_result(MiscProto::stLevelupSpriteResult::eSprite_Not_Exist);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	stSpriteBase * pSpriteBase = g_spriteBaseMgr.get(stSpriteBase::getFindID(spriteID,it->second.m_level));	
	stSpriteBase * pNextSpriteBase =  g_spriteBaseMgr.get(stSpriteBase::getFindID(spriteID,it->second.m_level + 1));

	if (!pSpriteBase){
		retCmd.set_result(MiscProto::stLevelupSpriteResult::eInner_Error);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}	
	
	if (!pNextSpriteBase){
		retCmd.set_result(MiscProto::stLevelupSpriteResult::eLevel_Full);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}
	
	if (m_pOwner->getLevel() < pSpriteBase->m_needLv){
		retCmd.set_result(MiscProto::stLevelupSpriteResult::eRole_Level_Limit);
		m_pOwner->sendProto2Me(retCmd);
		return ;
	}

	for (UINT8 i =0;i < pSpriteBase->m_consumesVec.size();++i){
		std::pair<UINT32,UINT32> & one = pSpriteBase->m_consumesVec[i];
        UINT32 itemID =
            CItem::realItemIDByJobAndSex(one.first, m_pOwner->getJob(), m_pOwner->getSex()); //首先判断如果是装备,会根据职业跟性别替换吊ID
		if (!m_pOwner->checkAssert(itemID, one.second)){
			retCmd.set_result(MiscProto::stLevelupSpriteResult::eRole_Level_Limit);
			m_pOwner->sendProto2Me(retCmd);
			return ;
		}
	}
	UINT32 consume = 0;
	for (UINT8 i =0;i < pSpriteBase->m_consumesVec.size();++i){
		std::pair<UINT32,UINT32> & one = pSpriteBase->m_consumesVec[i];
		consume = one.second;
        UINT32 itemID =
            CItem::realItemIDByJobAndSex(one.first,m_pOwner->getJob(), m_pOwner->getSex()); //首先判断如果是装备,会根据职业跟性别替换吊ID
		if (!m_pOwner->removeAssert(itemID,one.second,0,"升级精灵")){
			retCmd.set_result(MiscProto::stLevelupSpriteResult::eRole_Level_Limit);
			m_pOwner->sendProto2Me(retCmd);
			Athena::logger->error("[精灵升级]升级精灵失败,扣除资源失败!");
			return ;
		}
	}

	UINT16 oldLv = it->second.m_level;
	it->second.m_level += 1;
	retCmd.set_curlv(it->second.m_level);
	retCmd.set_result(MiscProto::stLevelupSpriteResult::eSuccess);
	it->second.init();
	m_pOwner->sendProto2Me(retCmd);
	m_pOwner->setupRoleState();
	//m_pOwner->sendMainData2Me();
	m_pOwner->sendBattleProp();

	Game::stSpriteLog webCmd;
	webCmd.set_spriteid(spriteID);
	webCmd.set_oldlv(oldLv);
	webCmd.set_newlv(it->second.m_level);
	webCmd.set_consume(consume);
	g_gameServer->getConnMgr().broadcastByType(WEBSERVER,webCmd);
}

void CSpriteSysMgr::followMaster(UINT32 spriteID)
{
	MiscProto::stSpriteFollowMasterResult retCmd;
	retCmd.set_spriteid(spriteID);

    if (spriteID && m_sprites.find(spriteID) == m_sprites.end()) {
        retCmd.set_result(MiscProto::stSpriteFollowMasterResult::eFail);
        m_pOwner->sendProto2Me(retCmd);
        Athena::logger->error("[SpriteSys:line:345]follow sprite failed, id is  %d", spriteID);
        return ;
    }
    bool needUpdate = false;
    if (m_followSpriteID != spriteID) {
        needUpdate = true;
    }
	m_followSpriteID = spriteID;
	retCmd.set_result(MiscProto::stSpriteFollowMasterResult::eSucess);
	m_pOwner->sendProto2Me(retCmd);
    if (needUpdate) {
        m_pOwner->setupRoleState();
        m_pOwner->sendBattleProp();
        Athena::logger->error("[SpriteSys:line:357]follow sprite syn battle, id is %d", spriteID);
    }
}

void CSpriteSysMgr::sendIllusionSprite(UINT32 spriteID) {
    MiscProto::stRetIllusionSprite retCmd;
    std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.find(spriteID);
    if (it == m_sprites.end()) {
        retCmd.set_res(MiscProto::stRetIllusionSprite::eNo_Such_Id);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    stSpriteInfo & sprite = it->second;
    retCmd.set_id(spriteID);
    retCmd.set_lvl(sprite.m_illusionLvl);
    retCmd.set_exp(sprite.m_illusionExp);
    retCmd.set_talentnum(sprite.m_talentNum);
    for (std::map<UINT32, UINT16>::iterator it = sprite.m_skillMap.begin();
            it != sprite.m_skillMap.end(); it++) {
        MiscProto::stIllusionSpriteSkillItem * pSkill = retCmd.add_skills();
        if (pSkill) {
            pSkill->set_skillid(it->first);
            pSkill->set_skilllvl(it->second);
        }
    }
    retCmd.set_res(MiscProto::stRetIllusionSprite::eSuccess);
    m_pOwner->sendProto2Me(retCmd);
}

void CSpriteSysMgr::illusionSpriteEat(MiscProto::stReqIllusionSpriteEat * pCmd) {
    MiscProto::stRetIllusionSpriteEat retCmd;
    retCmd.set_id(pCmd->id());
    std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.find(pCmd->id());
    if (it == m_sprites.end()) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteEat::eNo_Such_Id);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    stIllusionismBase * pIllusionismBase =
        g_illusionismBaseMgr.get(stIllusionismBase::getFindID(pCmd->id(), it->second.m_illusionLvl));
    if (!pIllusionismBase) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteEat::eNo_Such_Id);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    if (!pIllusionismBase->m_exp) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteEat::eMax_Lvl_Limit);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    UINT32 toMapLvlExp = pIllusionismBase->m_toMaxLvlExp;
    if (!toMapLvlExp) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteEat::eMax_Lvl_Limit);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    CItem * pItem = NULL;
    std::map<CItem *, UINT32> eatMap;
    bool hasDoubleEffect = false;
    UINT32 effect = 0, tmpEffect = 0;
    UINT16 curExp = it->second.m_illusionExp;
    for (int i = 0; i < pCmd->eatitem_size(); i++) {
        pItem = m_pOwner->m_bagMgr.getItemByInstaceID(Role::BAG_TYPE_MAIN1,
                pCmd->eatitem(i).id());
        if (!pItem || pItem->m_pItemBase->m_type != ITEM_TYPE_ILLUSION) {
            retCmd.set_res(MiscProto::stRetIllusionSpriteEat::eInvalid_item);
            m_pOwner->sendProto2Me(retCmd);
            return ;
        }
        if (pItem->m_itemInfo.m_num < pCmd->eatitem(i).num()) {
            retCmd.set_res(MiscProto::stRetIllusionSpriteEat::eItem_Not_Enough);
            m_pOwner->sendProto2Me(retCmd);
            return ;
        }
        tmpEffect = pItem->m_pItemBase->m_effect * pCmd->eatitem(i).num();
        for (std::vector<UINT32>::iterator it = pIllusionismBase->m_doubleExpItemsVec.begin();
                it != pIllusionismBase->m_doubleExpItemsVec.end(); it++) {
            if (pItem->m_pItemBase->m_itemID == *it) {
                hasDoubleEffect = true;
                tmpEffect += tmpEffect;
                break;
            }
        }
        if (effect + tmpEffect + curExp >= toMapLvlExp) {
            UINT32 unitExp = (hasDoubleEffect ? pItem->m_pItemBase->m_effect + pItem->m_pItemBase->m_effect :
                                        pItem->m_pItemBase->m_effect);
            UINT32 needNum = 0;
            UINT32 needExp = toMapLvlExp - effect - curExp;
            if (needExp > unitExp) {
                needNum = needExp / unitExp;
                if (needExp % unitExp) {
                    needNum++;
                }
            } else {
                needNum = 1;
            }
            effect += needNum * unitExp;
            eatMap.insert(std::make_pair(pItem, needNum));
            break;
        }
        effect += tmpEffect;
        hasDoubleEffect = false;
        eatMap.insert(std::make_pair(pItem, pCmd->eatitem(i).num()));
    }
    if (eatMap.empty()) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteEat::eInvalid_item);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    for (std::map<CItem *, UINT32>::iterator it = eatMap.begin();
            it != eatMap.end(); it++) {
        m_pOwner->m_bagMgr.decItem(it->first, it->second, "幻化精灵喂养扣除");
    }
    UINT16 tmpExp = 0;
    UINT16 lvl = it->second.m_illusionLvl;
    UINT32 exp = it->second.m_illusionExp + effect;
    UINT32 talentNum = it->second.m_talentNum;
    while (exp > 0) {
        tmpExp = pIllusionismBase->m_exp;
        if (exp < tmpExp) {
            break;
        }
        exp -= tmpExp;
        lvl++;
        pIllusionismBase =
            g_illusionismBaseMgr.get(stIllusionismBase::getFindID(pCmd->id(), lvl));
        if (!pIllusionismBase) {
            exp += tmpExp;//还原操作
            lvl--;
            break;
        }
        talentNum += pIllusionismBase->m_talentNum;
    }
    it->second.m_illusionLvl = lvl;
    it->second.m_illusionExp = exp;
    it->second.m_talentNum = talentNum;
    retCmd.set_lvl(lvl);
    retCmd.set_exp(exp);
    retCmd.set_talentnum(talentNum);
    retCmd.set_res(MiscProto::stRetIllusionSpriteEat::eSuccess);
    m_pOwner->sendProto2Me(retCmd);
}

void CSpriteSysMgr::illusionSpriteSkillUp(UINT32 spriteID, UINT32 skillId) {
    MiscProto::stRetIllusionSpriteSkillUp retCmd;
    retCmd.set_id(spriteID);
    retCmd.set_skillid(skillId);
    std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.find(spriteID);
    if (it == m_sprites.end()) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteSkillUp::eNo_Such_Id);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    stRoleBase * pRoleBase =  g_roleBaseMgr.get(spriteID);
    if (!pRoleBase) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteSkillUp::eNo_Such_Id);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    if (skillId == pRoleBase->m_skill1) {
    } else if (skillId == pRoleBase->m_skill2) {
    } else if (skillId == pRoleBase->m_skill3) {
    } else if (skillId == pRoleBase->m_skill4) {
    } else if (skillId == pRoleBase->m_skill5) {
    } else if (skillId == pRoleBase->m_skill6) {
    } else if (skillId == pRoleBase->m_skill7) {
    } else if (skillId == pRoleBase->m_skill8) {
    } else {
        retCmd.set_res(MiscProto::stRetIllusionSpriteSkillUp::eNo_Such_Skill_Id);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    //精灵幻化技能开放等级为0,忽略
    UINT16 skillLvl = 0;
    std::map<UINT32, UINT16>::iterator subIt = it->second.m_skillMap.find(skillId);
    if (subIt != it->second.m_skillMap.end()) {
        skillLvl = subIt->second;
    }
    SkillGrownBase * pSkillGrown =  g_skillGrownBaseMgr.get(SkillGrownBase::getFindID(skillId, skillLvl));
    if (!pSkillGrown){
        retCmd.set_res(MiscProto::stRetIllusionSpriteSkillUp::eNo_Such_Skill_Id);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    if (it->second.m_talentNum < pSkillGrown->m_cost) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteSkillUp::eLess_Of_Talent_Num);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    if (skillLvl >= pSkillGrown->m_skillMaxLv) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteSkillUp::eMax_Lvl_Limit);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    std::map<UINT32, UINT16>::iterator tempIt = it->second.m_skillMap.find(pSkillGrown->m_intensifySkillCondition);
    if (tempIt != it->second.m_skillMap.end() && tempIt->second < pSkillGrown->m_intensifySkillVal) {
        retCmd.set_res(MiscProto::stRetIllusionSpriteSkillUp::eLast_Skill_Lvl_Limit);
        m_pOwner->sendProto2Me(retCmd);
        return ;
    }
    if (skillLvl) {
        subIt->second = ++skillLvl;
    } else {
        it->second.m_skillMap.insert(std::make_pair(skillId, ++skillLvl));
    }
    it->second.m_talentNum -= pSkillGrown->m_cost;
    retCmd.set_skilllvl(skillLvl);
    retCmd.set_res(MiscProto::stRetIllusionSpriteSkillUp::eSuccess);
    m_pOwner->sendProto2Me(retCmd);
    if (m_followSpriteID == spriteID) {//跟随精灵才更新战力
        m_pOwner->setupRoleState();
        m_pOwner->sendBattleProp();
    }
}

UINT16 CSpriteSysMgr::getSpriteLvl(UINT32 id) {
    std::map<UINT32,stSpriteInfo>::iterator it = m_sprites.find(id);
    if (it != m_sprites.end()) {
        return it->second.m_level;
    }
    return 0;
}
