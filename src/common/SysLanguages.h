#ifndef __SYS_LANGUAGES_H__
#define __SYS_LANGUAGES_H__

#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>


enum eTitleID
{
	SYS_LANGUAGE_ID_SYSTEM = 1,							//系统
	SYS_LANGUAGE_ID_ONETOONE = 2,						//1V1战场
	SYS_LANGUAGE_ID_HIGHLADDER_SORTLIST = 3,			//天梯排行系统
	SYS_LANGUAGE_ID_WORLD_BOSS_KILL_REWARD = 4,			//世界BOSS击杀奖励
	SYS_LANGUAGE_ID_SNATCH_SYSTEM = 5,					//夺宝系统
	SYS_LANGUAGE_ID_GUILD_INTEGRAL_REWARD = 6,			//工会积分奖励
	SYS_LANGUAGE_ID_GUILD_COPYMAP_SORTLIST = 7,			//工会副本伤害排名奖励
	SYS_LANGUAGE_ID_LEADER = 8,							//会长已转让
	SYS_LANGUAGE_ID_WORLD_BOSS_REWARD = 9,				//世界BOSS奖励
	SYS_LANGUAGE_ID_GUILD_COPYMAP_KILL_REWARD = 10,		//工会副本击杀奖励
	SYS_LANGUAGE_ID_GUILD_COPYMAP_COMPLETE_REWARD = 11,	//公会副本完成奖励 
	SYS_LANGUAGE_ID_TIME_WUJIANG_DRAW = 12,				//限时武神抽奖奖励
	SYS_LANGUAGE_ID_OPEN_SERVER_POWER_SORTLIST = 13,	//开服战力排名奖励
	SYS_LANGUAGE_ID_GUILD_MANOR_WAR = 14,				//公会领地战
	SYS_LANGUAGE_ID_ESCORT_REWARD = 15,					//护送奖励 
	SYS_LANGUAGE_ID_ESCORT_OBTAIN_REWARD = 16,			//护送获得奖励
	SYS_LANGUAGE_ID_EMERALD_DREAM = 17,					//翡翠梦境奖励
	SYS_LANGUAGE_ID_LEAGUE_REWARD = 19,					//英雄联赛奖励
	SYS_LANGUAGE_ID_VIP_LVL_NOT_EQUAL = 20,				//vip等级不足,无法装备翅膀
	SYS_LANGUAGE_ID_NFT_VIP_MAIL_TITLE = 21,			//nft vip每日奖励邮件标题
};

class CSysLanguages
{
	USE_SINGLETON_NEW(CSysLanguages);
private:
	CSysLanguages(){}
public:
	static CSysLanguages & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}

	bool load();

	const char * getLanguage(eTitleID id);
private:
	std::map<UINT8, std::string> m_languageMap;

};
#endif
