#include "HeroLeagueMgr.h"
#include "TblQueryResult.h"
#include "MysqlDatabase.h"
#include "GlobalServer.h"
#include "SortList.pb.h"
#include "PlayerManager.h"
#include "HeroLeagueCfg.h"
#include "Mail.h"
#include "SysLanguages.h"
#include <math.h> 

extern GlobalServer  * g_globalServer;

CTblField g_championTblDef[] = {
	{ "CHARID",         	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",           	CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "AREACODE",			CTblField::DB_TYPE_UINT32,  sizeof(UINT32)	 },
	{ "LEVEL",          	CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "JOB",         		CTblField::DB_TYPE_UINT8,   sizeof(UINT8)	 },
	{ "SEX",            	CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "BATTLEPOWER",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "HEADID",    			CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "BODYID",    			CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "WEAPONID",  			CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CLOACKID",    		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   }, 
	{ "WINGID",    			CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   }, 
	{ "SHOEID",    			CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "FASHIONHAIR",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },  
	{ "FASHIONFACE",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   }, 
	{ "FASHIONBODY",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   }, 
	{ "FASHIONWEAPON",  	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "FASHIONSHOE",    	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "FASHIONDECORATION",	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "EFFECTID",			CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "WINGBREAKLV",		CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "USERPHOTOID",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL,        		    0,            				0 				 },
};

CTblField g_registerInfoTblDef[] = {
	{ "CHARID",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "PUBCHARID",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "AREACODE",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",               CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "LEVEL",              CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "VIPLV",              CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "JOB",                CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    }, 
	{ "SEX",                CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    }, 
	{ "BATTLEPOWER",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "PUBSERVERINDEX",     CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "SCORE",				CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "USERPHOTOID",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL,                 0,                          0                },
};

CTblField g_rankListTblDef[] = {
	{ "SESSION",            CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "RANK",             	CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "CHARID",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "PUBCHARID",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "AREACODE",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",               CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "LEVEL",              CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "VIPLV",              CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "JOB",                CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",                CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "BATTLEPOWER",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "USERPHOTOID",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL,                 0,                          0                },
};

CTblField g_finalTblDef[] = {
	{ "MATCH",              CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "WINORLOSS",          CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "CHARID",             CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "PUBCHARID",          CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "AREACODE",           CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "NAME",               CTblField::DB_TYPE_STRING,  MAX_NAMESIZE + 1 },
	{ "LEVEL",              CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "VIPLV",              CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ "JOB",                CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "SEX",                CTblField::DB_TYPE_UINT8,   sizeof(UINT8)    },
	{ "BATTLEPOWER",        CTblField::DB_TYPE_UINT32,  sizeof(UINT32)   },
	{ "USERPHOTOID",        CTblField::DB_TYPE_UINT16,  sizeof(UINT16)   },
	{ NULL,                 0,                          0                },
};

inline void stChampionInfo::initFromDB(stChampionDB & data)
{
	m_charID            =   data.m_charID;
	strncpy(m_name,data.m_name,sizeof(m_name));
	m_areaCode			=	data.m_areaCode;
	m_level             =   data.m_level;
	m_job               =   data.m_job;
	m_sex               =   data.m_sex;
	m_battlePower       =   data.m_battlePower;
	m_headID            =   data.m_headID;
	m_bodyID            =   data.m_bodyID;
	m_weaponID          =   data.m_weaponID;
	m_cloackID          =   data.m_cloackID;
	m_wingID            =   data.m_wingID;
	m_shoeID            =   data.m_shoeID;
	m_fashionHair       =   data.m_fashionHair;
	m_fashionFace       =   data.m_fashionFace;
	m_fashionBody       =   data.m_fashionBody;
	m_fashionWeapon     =   data.m_fashionWeapon;
	m_fashionShoe       =   data.m_fashionShoe;
	m_fashionDecoration =   data.m_fashionDecoration;
	m_effectID          =   data.m_effectID;
	m_wingBreakLv       =   data.m_wingBreakLv;
	m_photoID			=	data.m_photoID;
}

inline void stChampionInfo::fillDBData(stChampionDB & data)
{
	data.m_charID				=		m_charID;
	strncpy(data.m_name,m_name,sizeof(m_name));
	data.m_areaCode				=		m_areaCode;
	data.m_level				=		m_level;
	data.m_job					=		m_job;
	data.m_sex					=		m_sex;
	data.m_battlePower			=		m_battlePower;
	data.m_headID				=		m_headID;
	data.m_bodyID				=		m_bodyID;
	data.m_weaponID				=		m_weaponID;
	data.m_cloackID				=		m_cloackID;
	data.m_wingID				=		m_wingID;
	data.m_shoeID				=		m_shoeID;
	data.m_fashionHair			=		m_fashionHair;
	data.m_fashionFace			=		m_fashionFace;
	data.m_fashionBody			=		m_fashionBody;
	data.m_fashionWeapon		=		m_fashionWeapon;
	data.m_fashionShoe			=		m_fashionShoe;
	data.m_fashionDecoration	=		m_fashionDecoration;
	data.m_effectID				=		m_effectID;
	data.m_wingBreakLv			=		m_wingBreakLv;
	data.m_photoID				=		m_photoID;
}

inline void stRankList::initFromDB(stRankListDB & data)
{
	m_session		=	data.m_session;    
	m_rank 			= 	data.m_rank;       
	m_charID 		= 	data.m_charID;     
	m_pubCharID 	= 	data.m_pubCharID;  
	m_areaCode 		= 	data.m_areaCode;
	strncpy(m_name, data.m_name, sizeof(m_name));
	m_level 		= 	data.m_level;      
	m_vipLv 		= 	data.m_vipLv;      
	m_job 			= 	data.m_job;        
	m_sex 			= 	data.m_sex;        
	m_battlePower 	=	data.m_battlePower;
	m_photoID 		=	data.m_photoID;
}

inline void stRankList::fillDBData(stRankListDB & data)
{
	data.m_session			=	m_session;    
	data.m_rank				=	m_rank;       
	data.m_charID			=	m_charID;     
	data.m_pubCharID		=	m_pubCharID;  
	data.m_areaCode			=	m_areaCode;   
	strncpy(data.m_name, m_name, sizeof(m_name)); 
	data.m_level			=	m_level;      
	data.m_vipLv			=	m_vipLv;      
	data.m_job				=	m_job;        
	data.m_sex				=	m_sex;        
	data.m_battlePower		=	m_battlePower;
	data.m_photoID			=	m_photoID;
}

inline void stRankList::initFromPubUserInfo(stPubUserInfo & data)
{
	m_session			=	0;
	m_rank				=	0;        
	m_charID			=	data.m_charID;      
	m_pubCharID			=	data.m_pubCharID;   
	m_areaCode			=	data.m_areaCode;    
	strncpy(m_name, data.m_name, sizeof(m_name)); 
	m_level				=	data.m_level;       
	m_vipLv				=	data.m_vipLv;       
	m_job				=	data.m_job;         
	m_sex				=	data.m_sex;         
	m_battlePower		=	data.m_battlePower; 
	m_photoID			=	data.m_photoID;
}

inline void stRankList::initFromFinalInfo(stFinalInfo & data)
{
	m_session           =   0;
	m_rank              =   0;
	m_charID            =   data.m_charID;
	m_pubCharID         =   data.m_pubCharID;
	m_areaCode          =   data.m_areaCode;
	strncpy(m_name, data.m_name, sizeof(m_name));
	m_level             =   data.m_level;
	m_vipLv             =   data.m_vipLv;
	m_job               =   data.m_job;
	m_sex               =   data.m_sex;
	m_battlePower       =   data.m_battlePower;
	m_photoID			=	data.m_photoID;
}

inline void stFinalInfo::initFromDB(stFinalDB & data)
{
	m_match			=	data.m_match;
	m_winOrLoss		=	data.m_winOrLoss;
	m_charID		=	data.m_charID;
	m_pubCharID		=	data.m_pubCharID;
	m_areaCode		=	data.m_areaCode;
	strncpy(m_name, data.m_name, sizeof(m_name));
	m_level			=	data.m_level;
	m_vipLv			=	data.m_vipLv;
	m_job			=	data.m_job;
	m_sex        	=	data.m_sex;
	m_battlePower	=	data.m_battlePower;
	m_photoID		=	data.m_photoID;
}

inline void stFinalInfo::fillDBData(stFinalDB & data)
{
	data.m_match 		= 		m_match;
	data.m_winOrLoss	= 		m_winOrLoss;
	data.m_charID 		= 		m_charID;
	data.m_pubCharID 	= 		m_pubCharID;
	data.m_areaCode 	= 		m_areaCode;
	strncpy(data.m_name, m_name, sizeof(m_name));
	data.m_level 		= 		m_level;
	data.m_vipLv 		= 		m_vipLv;
	data.m_job 			= 		m_job;
	data.m_sex 			= 		m_sex;
	data.m_battlePower 	= 		m_battlePower;
	data.m_photoID		=		m_photoID;
}

inline void stFinalInfo::copyData(const stFinalInfo & data)
{
	m_match         =   data.m_match;          
	m_winOrLoss     =   data.m_winOrLoss;      
	m_charID        =   data.m_charID;         
	m_pubCharID     =   data.m_pubCharID;      
	m_areaCode      =   data.m_areaCode;       
	strncpy(m_name, data.m_name, sizeof(m_name));
	m_level         =   data.m_level;          
	m_vipLv         =   data.m_vipLv;          
	m_job           =   data.m_job;            
	m_sex           =   data.m_sex;            
	m_battlePower   =   data.m_battlePower;    
	m_photoID		=	data.m_photoID;
}

inline void stFinalInfo::initFromPubUserInfo(stPubUserInfo & user1, stPubUserInfo & user2)
{
	m_match         =   user2.m_pubCharID; 
	m_winOrLoss     =   0;
	m_charID        =   user1.m_charID;         
	m_pubCharID     =   user1.m_pubCharID;      
	m_areaCode      =   user1.m_areaCode;       
	strncpy(m_name, user1.m_name, sizeof(m_name));
	m_level         =   user1.m_level;          
	m_vipLv         =   user1.m_vipLv;          
	m_job           =   user1.m_job;            
	m_sex           =   user1.m_sex;            
	m_battlePower   =   user1.m_battlePower;    
	m_photoID		=	user1.m_photoID;
}


CHeroLeagueMgr:: CHeroLeagueMgr()
{
}

void CHeroLeagueMgr::load()
{
	CTblQueryResult * result = NULL; 
	result = g_databaseEngine.select(g_championTblDef,"HERO_LEAGUE_CHAMPION",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stChampionDB * pData = (stChampionDB * )result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stChampionInfo info;
			info.initFromDB(pData[i]);
			m_championMap.insert(std::make_pair(info.m_charID, info));
		}
	}
	SAFE_DELETE(result); 

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[英雄联赛]上届冠军信息 MapSize:%d", m_championMap.size());
		std::map<UINT32, stChampionInfo>::iterator it = m_championMap.begin();
		for(; it != m_championMap.end(); ++it){
			stChampionInfo one = it->second;
			Athena::logger->trace("[英雄联赛]上届冠军信息：key:%d, charID:%d, name:%s, areaCode:%d, level:%d, job:%d, sex:%d, battlePower:%d, headID:%d, bodyID:%d, weaponID:%d, cloackID:%d, wingID:%d, shoeID:%d, fashionHair:%d, fashionFace:%d, fashionBody:%d, fashionWeapon:%d, fashionShoe:%d, fashionDecoration:%d, effectID:%d, wingBreakLv:%d", it->first, one.m_charID, one.m_name,  one.m_areaCode, one.m_level, one.m_job, one.m_sex, one.m_battlePower, one.m_headID, one.m_bodyID, one.m_weaponID, one.m_cloackID, one.m_wingID, one.m_shoeID, one.m_fashionHair, one.m_fashionFace, one.m_fashionBody, one.m_fashionWeapon, one.m_fashionShoe, one.m_fashionDecoration, one.m_effectID, one.m_wingBreakLv);
		}
	}
#endif

	result = g_databaseEngine.select(g_registerInfoTblDef,"HERO_LEAGUE_INFO",NULL,NULL,0,NULL); 
	if (result && result->getData()){
		stPubUserInfoDB *pData = (stPubUserInfoDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stPubUserInfo info;
			info.initFromDB(pData[i]);
			m_pubUserInfoSortList.insertSortInfo(info);
#ifdef _HDZ_DEBUG
			Athena::logger->trace("charid:%d, pubCharId:%d, areaCode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, PubServerIndex:%d, score:%d",  info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name,info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower, info.m_pubServerIndex, info.m_score);
#endif
		}
	}
	SAFE_DELETE(result);

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[英雄联赛]m_pubUserInfoSortList :size :%d", m_pubUserInfoSortList.getListNum());
	}
#endif

	result = g_databaseEngine.select(g_rankListTblDef,"HERO_RANK_LIST",NULL,NULL,0,NULL);
	if (result && result->getData()){
		stRankListDB *pData = (stRankListDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stRankList info;
			info.initFromDB(pData[i]);
			m_rankSortList.insertSortInfo(info);
#ifdef _HDZ_DEBUG

			Athena::logger->trace("[英雄联赛历届排行榜]session:%d, rank:%d, charID:%d, pubCharID:%d, areaCode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d", info.m_session, info.m_rank, info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower); 
#endif

		}
	}
	SAFE_DELETE(result);

#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[英雄联赛]历届英雄榜：size:%d", m_rankSortList.getListNum());
	}
#endif

	result = g_databaseEngine.select(g_finalTblDef,"HERO_LEAGUE_SIXTEEN",NULL,NULL,0,NULL);
	if (result && result->getData()){
		std::map<UINT32, stFinalInfo> _map;
		stFinalDB *pData = (stFinalDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stFinalInfo info;
			info.initFromDB(pData[i]);
			_map.insert(std::make_pair(info.m_pubCharID, info));
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[英雄联赛十六强]：match:%d, winOrLoss:%d, charID:%d, pubCharID:%d, areaCode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d", info.m_match, info.m_winOrLoss, info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower);
#endif
		}
		//配对
		std::map<UINT32, stFinalInfo>::iterator it = _map.begin();                  
		for(;it != _map.end(); ++it){                                               
			stFinalInfo &one = it->second;                                          
			stPair user;                                                            
			user.user1.copyData(one);                                               
			std::map<UINT32, stFinalInfo>::iterator subIt = _map.find(one.m_match); 
			if(subIt != _map.end()){                                                
				stFinalInfo &two = subIt->second;                                   
				user.user2.copyData(two);                                           
				m_sixteen.push_back(user);                                            
				_map.erase(two.m_pubCharID);                                        
			}                                                                       
		}                                                                          
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[英雄联赛十六强]：size:%d", m_sixteen.size());
	}
#endif

	result = g_databaseEngine.select(g_finalTblDef,"HERO_LEAGUE_EIGHT",NULL,NULL,0,NULL);
	if (result && result->getData()){
		std::map<UINT32, stFinalInfo> _map;
		stFinalDB *pData = (stFinalDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stFinalInfo info;
			info.initFromDB(pData[i]);
			_map.insert(std::make_pair(info.m_pubCharID, info));
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[英雄联赛八强]：match:%d, winOrLoss:%d, charID:%d, pubCharID:%d, areaCode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d", info.m_match, info.m_winOrLoss, info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower);
#endif
		}
		//配对
		std::map<UINT32, stFinalInfo>::iterator it = _map.begin();
		for(;it != _map.end(); ++it){
			stFinalInfo &one = it->second;
			stPair user;
			user.user1.copyData(one);
			std::map<UINT32, stFinalInfo>::iterator subIt = _map.find(one.m_match);
			if(subIt != _map.end()){
				stFinalInfo &two = subIt->second;
				user.user2.copyData(two);
				m_eight.push_back(user);
				_map.erase(two.m_pubCharID);
			}
		}
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[英雄联赛八强]：size:%d", m_eight.size());
	}
#endif

	result = g_databaseEngine.select(g_finalTblDef,"HERO_LEAGUE_FOUR",NULL,NULL,0,NULL);
	if (result && result->getData()){
		std::map<UINT32, stFinalInfo> _map;
		stFinalDB *pData = (stFinalDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stFinalInfo info;
			info.initFromDB(pData[i]);
			_map.insert(std::make_pair(info.m_pubCharID, info));
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[英雄联赛四强]：match:%d, winOrLoss:%d, charID:%d, pubCharID:%d, areaCode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d", info.m_match, info.m_winOrLoss, info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower);
#endif
		}
		//配对
		std::map<UINT32, stFinalInfo>::iterator it = _map.begin();                 
		for(;it != _map.end(); ++it){                                              
			stFinalInfo &one = it->second;                                         
			stPair user;                                                           
			user.user1.copyData(one);                                              
			std::map<UINT32, stFinalInfo>::iterator subIt = _map.find(one.m_match);
			if(subIt != _map.end()){                                               
				stFinalInfo &two = subIt->second;                                  
				user.user2.copyData(two);                                          
				m_four.push_back(user);                                           
				_map.erase(two.m_pubCharID);                                       
			}                                                                      
		}                                                                          
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[英雄联赛四强]：size:%d", m_four.size());
	}
#endif

	result = g_databaseEngine.select(g_finalTblDef,"HERO_LEAGUE_TWO",NULL,NULL,0,NULL);
	if (result && result->getData()){
		std::map<UINT32, stFinalInfo> _map;
		stFinalDB *pData = (stFinalDB *)result->getData();
		for (UINT16 i = 0;i < result->getRowCount();++i){
			stFinalInfo info;
			info.initFromDB(pData[i]);
			_map.insert(std::make_pair(info.m_pubCharID, info));
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[英雄联赛冠亚军]：match:%d, winOrLoss:%d, charID:%d, pubCharID:%d, areaCode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d", info.m_match, info.m_winOrLoss, info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower);
#endif
		}
		//配对
		std::map<UINT32, stFinalInfo>::iterator it = _map.begin();
		for(;it != _map.end(); ++it){
			stFinalInfo &one = it->second;                                         
			stPair user;
			user.user1.copyData(one);
			std::map<UINT32, stFinalInfo>::iterator subIt = _map.find(one.m_match);
			if(subIt != _map.end()){
				stFinalInfo &two = subIt->second;
				user.user2.copyData(two);
				m_two.push_back(user);
				_map.erase(two.m_pubCharID);
			}
		}                                                                          
	}
	SAFE_DELETE(result);
#ifdef _HDZ_DEBUG
	{
		Athena::logger->trace("[英雄联赛冠亚军]：size:%d", m_two.size());
	}
#endif
}

UINT32 getNowTime()                                                     
{                                                                       
	struct tm _tm;                                                      
	RealTime::getLocalTime(_tm,g_globalServer->getSec());                 
	UINT32 secNum = _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec;  

	/* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
	UINT16 wday = _tm.tm_wday;                                          
	//secNum = secNum + wday * 86400 ;                                  
	return secNum + wday * 86400;
}                                                                       

void CHeroLeagueMgr::timer(UINT32 cur)
{
	UINT32 secNum = getNowTime();
	//初赛结算时间
	if (secNum == CHeroLeagueCfg::getSingleton().getPreConfiguration().m_clearing){//初赛结算时间
		preSettlement();
	}

	//复赛结算时间
	if(secNum == CHeroLeagueCfg::getSingleton().getIntermediary().m_clearing){
		IntermedSettlement();
	}
	//决赛结算时间
	if(secNum == CHeroLeagueCfg::getSingleton().getFinals().m_clearing){
		finalSettlement();	
	}
	//决赛每场次结算时间
	if(CHeroLeagueCfg::getSingleton().finalMiddleSettlement(secNum)){
		finalSessionSettlement();
	}
	//决赛报名时间删除上届冠军信息
	if(secNum == CHeroLeagueCfg::getSingleton().getFinals().m_applyTime){
		removeAllChamopionRecord();
		m_championMap.clear();
	}
}
//历届冠军记录
void CHeroLeagueMgr::addChamopionRecord(stChampionDB & data)
{
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::addChamopionRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_championTblDef, (const char *)&data, "HERO_LEAGUE_CHAMPION", autoID, pDelaySql);
}

void CHeroLeagueMgr::addChamopionRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::updateChamopionRecord(stChampionDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "CHARID=%u",data.m_charID);
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::updateChamopionRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_championTblDef, (const char *)&data, "HERO_LEAGUE_CHAMPION", where, pDelaySql);
}

void CHeroLeagueMgr::updateChamopionRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::removeAllChamopionRecord()
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere,sizeof(szWhere),"CHARID>0");
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::removeAllChamopionRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("HERO_LEAGUE_CHAMPION",szWhere,pDelaySql);
}

void CHeroLeagueMgr::removeAllChamopionRecordCB(bool opResult,UINT64 autoID)
{}

//加一个报名记录
void CHeroLeagueMgr::addRecord(stPubUserInfoDB & data)
{	
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::addRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_registerInfoTblDef, (const char *)&data, "HERO_LEAGUE_INFO", autoID, pDelaySql);
}

void CHeroLeagueMgr::addRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::updateRecord(stPubUserInfoDB & data)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "PUBCHARID=%u", data.m_pubCharID);
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::updateRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_registerInfoTblDef, (const char *)&data, "HERO_LEAGUE_INFO", where, pDelaySql);
}

void CHeroLeagueMgr::updateRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::removeRecord(UINT32 key)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "PUBCHARID=%d",key);
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::removeRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("HERO_LEAGUE_INFO",where,pDelaySql);
}

void CHeroLeagueMgr::removeRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::removeInfoRecord()
{
	char szWhere[65] = { 0 };
	bzero(szWhere, 65);
	snprintf(szWhere,sizeof(szWhere),"PUBCHARID>0");
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::removeInfoRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("HERO_LEAGUE_INFO",szWhere,pDelaySql);
}

void CHeroLeagueMgr::removeInfoRecordCB(bool opResult,UINT64 autoID)
{}

//增加一条排行榜记录 
void CHeroLeagueMgr::addListRecord(stRankListDB & data)
{
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::addListRecordCB,false,0); 
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_rankListTblDef, (const char *)&data, "HERO_RANK_LIST", autoID,pDelaySql);
}

void CHeroLeagueMgr::addListRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::updateListRecord(stRankListDB & data)
{

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "AREACODE=%u AND CHARID=%u",data.m_areaCode, data.m_charID);
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::updateListRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_registerInfoTblDef, (const char *)&data, "HERO_RANK_LIST", where, pDelaySql);
}

void CHeroLeagueMgr::updateListRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::removeListRecord(UINT32 key)
{
	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "SESSION<%d",key);
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::removeListRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord("HERO_RANK_LIST",where,pDelaySql);
}


void CHeroLeagueMgr::removeListRecordCB(bool opResult,UINT64 autoID)
{}

//增加一条决赛记录 
void CHeroLeagueMgr::addFinalRecord(stFinalDB & data, EFINAL_TYPE type)
{
	std::string tableName;
	switch(type){
		case eHERO_LEAGUE_SIXTEEN:	tableName = "HERO_LEAGUE_SIXTEEN";	break;
		case eHERO_LEAGUE_EIGHT:	tableName = "HERO_LEAGUE_EIGHT";	break;
		case eHERO_LEAGUE_FOUR:		tableName = "HERO_LEAGUE_FOUR";		break;
		case eHERO_LEAGUE_TWO: 	    tableName = "HERO_LEAGUE_TWO"; 	    break;
		case eHERO_LEAGUE_NULL:	return;
	}

	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::addFinalRecordCB,false,0); 
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	UINT64 autoID = 0;
	g_databaseEngine.insertRecord(g_finalTblDef, (const char *)&data, tableName.c_str(), autoID,pDelaySql);

}

void CHeroLeagueMgr::addFinalRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::updateFinalRecord(stFinalDB & data, EFINAL_TYPE type)
{
	std::string tableName;                                                      
	switch(type){                                                               
		case eHERO_LEAGUE_SIXTEEN:  tableName = "HERO_LEAGUE_SIXTEEN";    break;
		case eHERO_LEAGUE_EIGHT:    tableName = "HERO_LEAGUE_EIGHT";      break;
		case eHERO_LEAGUE_FOUR:     tableName = "HERO_LEAGUE_FOUR";       break;
		case eHERO_LEAGUE_TWO:	    tableName = "HERO_LEAGUE_TWO";		  break;
		case eHERO_LEAGUE_NULL: return;
	}

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "PUBCHARID=%u", data.m_pubCharID);
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::updateFinalRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_finalTblDef, (const char *)&data, tableName.c_str(), where,pDelaySql);
}


void CHeroLeagueMgr::updateFinalRecordByCharID(stFinalDB & data, EFINAL_TYPE type)
{
	std::string tableName;                                                      
	switch(type){                                                               
		case eHERO_LEAGUE_SIXTEEN:  tableName = "HERO_LEAGUE_SIXTEEN";    break;
		case eHERO_LEAGUE_EIGHT:    tableName = "HERO_LEAGUE_EIGHT";      break;
		case eHERO_LEAGUE_FOUR:     tableName = "HERO_LEAGUE_FOUR";       break;
		case eHERO_LEAGUE_TWO:	    tableName = "HERO_LEAGUE_TWO";		  break;
		case eHERO_LEAGUE_NULL: return;
	}

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "AREACODE=%u AND CHARID=%u", data.m_areaCode, data.m_charID);
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::updateFinalRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.updateRecord(g_finalTblDef, (const char *)&data, tableName.c_str(), where,pDelaySql);
}


void CHeroLeagueMgr::updateFinalRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::removeFinalRecord(EFINAL_TYPE type)
{
	std::string tableName;                                                    
	switch(type){                                                             
		case eHERO_LEAGUE_SIXTEEN:  tableName = "HERO_LEAGUE_SIXTEEN";    break;
		case eHERO_LEAGUE_EIGHT:    tableName = "HERO_LEAGUE_EIGHT";      break;
		case eHERO_LEAGUE_FOUR:     tableName = "HERO_LEAGUE_FOUR";       break;
		case eHERO_LEAGUE_TWO:      tableName = "HERO_LEAGUE_TWO";        break;
		case eHERO_LEAGUE_NULL: return;
	}

	char where[65] = { 0 };
	bzero(where, 65);
	snprintf(where, 64, "ID>0");
	Athena::CUDSqlCallBack<CHeroLeagueMgr> * pDelayCB = ATHENA_NEW Athena::CUDSqlCallBack<CHeroLeagueMgr>(this,&CHeroLeagueMgr::removeFinalRecordCB,false,0);
	CCUDSqlOperation * pDelaySql = ATHENA_NEW CCUDSqlOperation(pDelayCB);
	g_databaseEngine.deleteRecord(tableName.c_str(),where,pDelaySql);
}

void CHeroLeagueMgr::removeFinalRecordCB(bool opResult,UINT64 autoID)
{}

void CHeroLeagueMgr::openWindow(Player * pUser)
{
	if(!pUser)
	{
		return ;
	}

	SortProto::stOpenLeagueWindowResult retCmd;
	if(m_championMap.size()){
		SortProto::stSynChampionInfo *info = retCmd.add_champion();
		std::map<UINT32, stChampionInfo>::iterator it = m_championMap.begin();
		stChampionInfo &champion = it->second; 
		if(info){
			info->set_charid(champion.m_charID);						//角色ID
			info->set_name(champion.m_name);               				//名字
			info->set_areacode(champion.m_areaCode);				//所在服务器ID
			info->set_level(champion.m_level);              			//等级
			info->set_job(champion.m_job);               				//职业
			info->set_sex(champion.m_sex);                				//性别
			info->set_leader(true);				             			//队长
			info->set_battlepower(champion.m_battlePower);        		//战力
			info->set_pos(0);											//位置
			info->set_headid(champion.m_headID);             			//头部ID
			info->set_bodyid(champion.m_bodyID);            			//衣服ID
			info->set_weaponid(champion.m_weaponID);          			//武器ID
			info->set_cloackid(champion.m_cloackID);          			//披风ID
			info->set_wingid(champion.m_wingID);            			//翅膀ID
			info->set_shoeid(champion.m_shoeID);            			//鞋子
			info->set_fashionhair(champion.m_fashionHair);       		//发型
			info->set_fashionface(champion.m_fashionFace);       		//表情
			info->set_fashionbody(champion.m_fashionBody);       		//衣服
			info->set_fashionweapon(champion.m_fashionWeapon);      	//武器
			info->set_fashionshoe(champion.m_fashionShoe);       		//鞋子
			info->set_fashiondecoration(champion.m_fashionDecoration);  //脸饰品
			info->set_effectid(champion.m_effectID);          			//特效ID
			info->set_wingbreaklv(champion.m_wingBreakLv);        		//翅膀突破等级
			info->set_photoid(champion.m_photoID);						//头像ID
		}
	}

	SortProto::stMyIntegralAndNum *cmd = retCmd.add_myinfo();
	if(cmd){

		std::map<UINT32, UINT32>::iterator mapIt = m_charIDs.begin();
		for(;mapIt != m_charIDs.end(); ++mapIt){
			if(m_pubUserInfoSortList.isInSortList(mapIt->first)){
				m_pubUserInfoSortList.removeSortInfo(mapIt->first);  
			}
		} 

		if(!m_pubUserInfoSortList.isInSortList(pUser->m_pubCharID)){
			cmd->set_score(0);
			CHeroLeagueCfg cfg = CHeroLeagueCfg::getSingleton();

			UINT32 secNum = getNowTime();

			if(cfg.isRemOpen(secNum) || (cfg.getFinals().m_applyTime < getNowTime() && getNowTime() < cfg.getFinals().m_clearing)){
				retCmd.set_status(SortProto::stOpenLeagueWindowResult::eNoQualified);
			}else{
				retCmd.set_status(SortProto::stOpenLeagueWindowResult::eQualified); 
			}
		}
		else{
			retCmd.set_status(SortProto::stOpenLeagueWindowResult::eQualified);
			stPubUserInfo info = m_pubUserInfoSortList.getSortInfo(pUser->m_pubCharID);
			cmd->set_score(info.m_score);
		}
	}

	UINT32 nowTime = getNowTime();
	if(((nowTime > CHeroLeagueCfg::getSingleton().getIntermediary().m_clearing)&&(nowTime < 604800))
			|| (nowTime < CHeroLeagueCfg::getSingleton().getFinals().m_clearing)){
		UINT32 num = m_pubUserInfoSortList.getListNum(); 
		if(num <= 16 && num > 8){
			retCmd.set_esession(SortProto::stOpenLeagueWindowResult::eSixteen);   	
		}

		else if(num > 4){
			retCmd.set_esession(SortProto::stOpenLeagueWindowResult::eEight);
		}

		else if(num > 2){
			retCmd.set_esession(SortProto::stOpenLeagueWindowResult::eFour); 
		}

		else/* if(num > 0)*/{
			retCmd.set_esession(SortProto::stOpenLeagueWindowResult::eTwo); 
		}
	}

	if(m_rankSortList.getListNum()){
		stRankList ranklist = m_rankSortList.getSortInfo(m_rankSortList.getLastOneKey());
		retCmd.set_session(ranklist.m_session);
	}else{
		retCmd.set_session(0);
	}
	pUser->sendProtoToMe(retCmd);
}

void CHeroLeagueMgr::updateChampionInfo(Global::stReqSynChampionInfo *recvCmd)
{
	if(!recvCmd){
		return;
	}

	stChampionInfo info;                                           
	info.m_charID = recvCmd->info().charid();                      
	strncpy(info.m_name, recvCmd->info().name().c_str(),sizeof(info.m_name));     
	info.m_areaCode = recvCmd->info().areacode();                  
	info.m_level = recvCmd->info().level();                        
	info.m_job = recvCmd->info().job();                            
	info.m_sex = recvCmd->info().sex();                            
	info.m_battlePower = recvCmd->info().battlepower();            
	info.m_headID = recvCmd->info().headid();                      
	info.m_bodyID = recvCmd->info().bodyid();                      
	info.m_weaponID = recvCmd->info().weaponid();                  
	info.m_cloackID = recvCmd->info().cloackid();                  
	info.m_wingID = recvCmd->info().wingid();                      
	info.m_shoeID = recvCmd->info().shoeid();                      
	info.m_fashionHair = recvCmd->info().fashionhair();            
	info.m_fashionFace = recvCmd->info().fashionface();            
	info.m_fashionBody = recvCmd->info().fashionbody();            
	info.m_fashionWeapon = recvCmd->info().fashionweapon();        
	info.m_fashionShoe = recvCmd->info().fashionshoe();            
	info.m_fashionDecoration = recvCmd->info().fashiondecoration();
	info.m_effectID = recvCmd->info().effectid();                  
	info.m_wingBreakLv = recvCmd->info().wingbreaklv(); 
	info.m_photoID = recvCmd->info().photoid();

	stChampionDB  data;
	info.fillDBData(data);
	//removeAllChamopionRecord();
	addChamopionRecord(data);

	//m_championMap.clear();
	m_championMap.insert(std::make_pair(info.m_charID, info));
}

void FinalExtend(bool completeState, stFinalInfo& finalInfo, CHeroLeagueMgr& mgr, EFINAL_TYPE type)
{
	stFinalInfo info;
	if(completeState){
		finalInfo.m_winOrLoss = 1;//0:未参加， 1:胜利， 2：失败
	}else{
		finalInfo.m_winOrLoss = 2; //0:未参加， 1:胜利， 2：失败 
	}           
	info.copyData(finalInfo);
	stFinalDB data;                              
	info.fillDBData(data);                       
	mgr.updateFinalRecord(data,type);
}

void CHeroLeagueMgr::calculation(std::vector<stPair> & vec, Global::stHeroLeagueResult * recvCmd, EFINAL_TYPE type)
{
	if(!recvCmd){
		return;
	}

	std::vector<stPair>::iterator it = vec.begin();
	for(;it != vec.end(); ++it){
		stPair & stpair = *it;
		if(recvCmd->user1().pubcharid() == stpair.user1.m_pubCharID){
			stpair.user1.m_battlePower = recvCmd->user1().battlepower();
			stpair.user1.m_level = recvCmd->user1().level();
			stpair.user1.m_vipLv = recvCmd->user1().viplv();
			FinalExtend(recvCmd->user1().winorloss(),stpair.user1, *this, type);
#ifdef _HDZ_DEBUG          
			stFinalInfo user = stpair.user1;
			Athena::logger->trace("[英雄联赛决赛更新信息]charID:%d, pubcharID:%d, areacode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, winOrLoss:%d", user.m_charID, user.m_pubCharID, user.m_areaCode, user.m_name, user.m_level, user.m_vipLv, user.m_job, user.m_sex, user.m_battlePower, recvCmd->user1().winorloss());
#endif
		}

		if(recvCmd->user1().pubcharid() == stpair.user2.m_pubCharID){
			stpair.user2.m_battlePower = recvCmd->user1().battlepower();
			stpair.user2.m_level = recvCmd->user1().level();            
			stpair.user2.m_vipLv = recvCmd->user1().viplv();            
			FinalExtend(recvCmd->user1().winorloss(), stpair.user2, *this, type);
#ifdef _HDZ_DEBUG          
			stFinalInfo user = stpair.user2;
			Athena::logger->trace("[英雄联赛决赛更新信息]charID:%d, pubcharID:%d, areacode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, winOrLoss:%d", user.m_charID, user.m_pubCharID, user.m_areaCode, user.m_name, user.m_level, user.m_vipLv, user.m_job, user.m_sex, user.m_battlePower, recvCmd->user1().winorloss());
#endif
		}

		if(recvCmd->user2().pubcharid() == stpair.user1.m_pubCharID){                            
			stpair.user1.m_battlePower = recvCmd->user2().battlepower();
			stpair.user1.m_level = recvCmd->user2().level();            
			stpair.user1.m_vipLv = recvCmd->user2().viplv();            

			FinalExtend(recvCmd->user2().winorloss(),stpair.user1, *this, type); 
#ifdef _HDZ_DEBUG          
			stFinalInfo user = stpair.user1;
			Athena::logger->trace("[英雄联赛决赛更新信息]charID:%d, pubcharID:%d, areacode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, winOrLoss:%d", user.m_charID, user.m_pubCharID, user.m_areaCode, user.m_name, user.m_level, user.m_vipLv, user.m_job, user.m_sex, user.m_battlePower, recvCmd->user2().winorloss());
#endif
		}                                                                                         

		if(recvCmd->user2().pubcharid() == stpair.user2.m_pubCharID){
			stpair.user2.m_battlePower = recvCmd->user2().battlepower();
			stpair.user2.m_level = recvCmd->user2().level();            
			stpair.user2.m_vipLv = recvCmd->user2().viplv();            
			FinalExtend(recvCmd->user2().winorloss(), stpair.user2, *this, type);
#ifdef _HDZ_DEBUG          
			stFinalInfo user = stpair.user2;
			Athena::logger->trace("[英雄联赛决赛更新信息]charID:%d, pubcharID:%d, areacode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, winOrLoss:%d", user.m_charID, user.m_pubCharID, user.m_areaCode, user.m_name, user.m_level, user.m_vipLv, user.m_job, user.m_sex, user.m_battlePower, recvCmd->user2().winorloss());
#endif
		}                                                                                         
	}
	
	if(m_pubUserInfoSortList.getListNum() == 2){
		//removeInfoRecord();
		//m_pubUserInfoSortList.clear();
		return;
	}

	if(m_pubUserInfoSortList.isInSortList(recvCmd->user1().pubcharid())){
		if(recvCmd->user1().winorloss() != 1){
			//m_pubUserInfoSortList.removeSortInfo(recvCmd->user1().pubcharid());
			UINT32 pubCharID = recvCmd->user1().pubcharid();
			m_charIDs.insert(std::make_pair(pubCharID,pubCharID));
		}
	}

	if(m_pubUserInfoSortList.isInSortList(recvCmd->user2().pubcharid())){
		if(recvCmd->user2().winorloss() != 1){
			//m_pubUserInfoSortList.removeSortInfo(recvCmd->user2().pubcharid());
			UINT32 pubCharID = recvCmd->user2().pubcharid();
			m_charIDs.insert(std::make_pair(pubCharID,pubCharID));
		}
	}
}

void CHeroLeagueMgr::updateAllRecord(Global::stHeroLeagueResult *recvCmd)
{
	if(!recvCmd){
		return;
	}

	UINT32 peopleNum = m_pubUserInfoSortList.getListNum();

	if(CHeroLeagueCfg::getSingleton().isFinalFightOpen(getNowTime())){
		if(16 >= peopleNum && peopleNum > 8){
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[决赛16强]");
#endif
			calculation(m_sixteen, recvCmd, eHERO_LEAGUE_SIXTEEN);
		}else if(4 < peopleNum){
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[决赛8强]");
#endif
			calculation(m_eight, recvCmd, eHERO_LEAGUE_EIGHT);
		}else if(2 < peopleNum){
#ifdef _HDZ_DEBUG
			Athena::logger->trace("[决赛4强]");
#endif
			calculation(m_four, recvCmd, eHERO_LEAGUE_FOUR);  
		}else if( 0 < peopleNum){

#ifdef _HDZ_DEBUG
			Athena::logger->trace("[决赛2强]");
#endif
			calculation(m_two, recvCmd, eHERO_LEAGUE_TWO);

			stRankList user;
			stPubUserInfo info1 = m_pubUserInfoSortList.getSortInfo(recvCmd->user1().pubcharid());
			stPubUserInfo info2 = m_pubUserInfoSortList.getSortInfo(recvCmd->user2().pubcharid());

			UINT32 lastSession = m_rankSortList.getSortInfo(m_rankSortList.getLastOneKey()).m_session;

			std::map<UINT32, UINT32> charIDs;

			UINT8 rank = 0;
			if(recvCmd->user1().winorloss() || recvCmd->user2().winorloss()){
				if(recvCmd->user1().winorloss()){ 
					if(info1.m_charID){
						user.initFromPubUserInfo(info1);
						user.m_session = lastSession + 1;
						user.m_rank = ++rank;
						stRankListDB data1;
						user.fillDBData(data1);
						addListRecord(data1);                
						m_rankSortList.insertSortInfo(user);
						charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID));
					}
					if(info2.m_charID){
						user.initFromPubUserInfo(info2);     
						user.m_session = lastSession + 1;    
						user.m_rank = ++rank;
						stRankListDB data2;                   
						user.fillDBData(data2);               
						addListRecord(data2);                 
						m_rankSortList.insertSortInfo(user);
						charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID)); 
					}
				}else{
					if(info2.m_charID){
						user.initFromPubUserInfo(info2);    
						user.m_session = lastSession + 1;   
						user.m_rank = ++rank;
						stRankListDB data1;                  
						user.fillDBData(data1);              
						addListRecord(data1);                
						m_rankSortList.insertSortInfo(user);
						charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID)); 
					}

					if(info1.m_charID){
						user.initFromPubUserInfo(info1);    
						user.m_session = lastSession + 1;   
						user.m_rank = ++rank;
						stRankListDB data2;                  
						user.fillDBData(data2);              
						addListRecord(data2);                
						m_rankSortList.insertSortInfo(user);
						charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID)); 
					}
				}
			}else{   //决赛两人都没打的情况 1样的名次由客户端区分
				if(info1.m_charID){
					user.initFromPubUserInfo(info1);    
					user.m_session = lastSession + 1;   
					user.m_rank = ++rank;
					stRankListDB data1;                  
					user.fillDBData(data1);              
					addListRecord(data1);                
					m_rankSortList.insertSortInfo(user);
					charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID)); 
				}
				if(info2.m_charID){
					user.initFromPubUserInfo(info2);    
					user.m_session = lastSession + 1;   
					user.m_rank = ++rank;
					stRankListDB data2;                  
					user.fillDBData(data2);              
					addListRecord(data2);                
					m_rankSortList.insertSortInfo(user);
					charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID)); 
				}
			}
			//第三名 第四名
			stPubUserInfo info;
			stRankListDB data;

			std::vector<stPair>::iterator it =  m_four.begin();
			for(;it != m_four.end(); ++it){
				stPair& stpair = *it;
				if(stpair.user1.m_pubCharID){
					std::map<UINT32,UINT32>::iterator it = charIDs.find(stpair.user1.m_pubCharID);
					if(it == charIDs.end()){
						user.initFromFinalInfo(stpair.user1);
						user.m_session = lastSession + 1;                                   
						user.m_rank = ++rank;
						user.fillDBData(data);
						addListRecord(data);  
						m_rankSortList.insertSortInfo(user);
						charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID)); 
					}
				}

				if(stpair.user2.m_pubCharID){
					std::map<UINT32,UINT32>::iterator it = charIDs.find(stpair.user2.m_pubCharID);
					if(it == charIDs.end()){
						user.initFromFinalInfo(stpair.user2);
						user.m_session = lastSession + 1;                                   
						user.m_rank = ++rank;
						user.fillDBData(data);
						addListRecord(data);  
						m_rankSortList.insertSortInfo(user);  
						charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID));
					}
				}
			}
			//第五六七八名
			it = m_eight.begin();
			for(;it != m_eight.end(); ++it){
				stPair& stpair = *it;
				if(stpair.user1.m_pubCharID){
					std::map<UINT32,UINT32>::iterator it = charIDs.find(stpair.user1.m_pubCharID);
					if(it == charIDs.end()){
						user.initFromFinalInfo(stpair.user1);                                    
						user.m_session = lastSession + 1;                                  
						user.m_rank = ++rank;
						user.fillDBData(data);                                             
						addListRecord(data);                                               
						m_rankSortList.insertSortInfo(user);
						charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID)); 
					}
				}

				if(stpair.user2.m_pubCharID){
					std::map<UINT32,UINT32>::iterator it = charIDs.find(stpair.user2.m_pubCharID);
					if(it == charIDs.end()){
						user.initFromFinalInfo(stpair.user2);                                     
						user.m_session = lastSession + 1;                                   
						user.m_rank = ++rank;                                                    
						user.fillDBData(data);                                              
						addListRecord(data);                                                
						m_rankSortList.insertSortInfo(user);
						charIDs.insert(std::make_pair(user.m_pubCharID, user.m_pubCharID));
					}
				}
			}

			//删除前三期以前的记录
			++lastSession;
			INT32 subSession = (INT32)lastSession - 3;
			if(subSession > 0){
				std::vector<stRankList> vec;
				m_rankSortList.getFromN12N2(vec, 1,m_rankSortList.getListNum());
				std::vector<stRankList>::iterator it =  vec.begin(); 
				for(;it != vec.end();++it){
					stRankList & stlist = *it;
					if(stlist.m_session < (UINT32)subSession){
						m_rankSortList.removeSortInfo(stlist.m_pubCharID);
					}
				}

				removeListRecord(subSession);
			}

			removeInfoRecord();              
			m_pubUserInfoSortList.clear(); 

		}
	}else{
		if(!m_pubUserInfoSortList.isInSortList(recvCmd->user1().pubcharid()))
		{
			if(recvCmd->user1().charid()){
				stPubUserInfo info;
				info.initFromProto(recvCmd->user1());

				stPubUserInfoDB dbData;                         
				info.fillDBData(dbData);

				m_pubUserInfoSortList.insertSortInfo(info);            
				addRecord(dbData); 
#ifdef _HDZ_DEBUG                              
				Athena::logger->trace("[英雄联赛更新信息add:user1]charID:%d, pubcharID:%d, areacode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubServerIndex:%d, score:%d", info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower, info.m_pubServerIndex, info.m_score);
#endif                                    
			}
		}else{
			stPubUserInfo info = m_pubUserInfoSortList.getSortInfo(recvCmd->user1().pubcharid());
			info.m_battlePower = recvCmd->user1().battlepower();
			info.m_level = recvCmd->user1().level();
			info.m_vipLv = recvCmd->user1().viplv();

			info.m_score += recvCmd->user1().score();

			stPubUserInfoDB dbData;
			info.fillDBData(dbData);
			m_pubUserInfoSortList.updateSortInfo(info); 
			updateRecord(dbData);

#ifdef _HDZ_DEBUG                              
			Athena::logger->trace("[英雄联赛更新信息update:user1]charID:%d, pubcharID:%d, areacode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubServerIndex:%d, score:%d", info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower, info.m_pubServerIndex, info.m_score);
#endif                                         
		}

		if(!m_pubUserInfoSortList.isInSortList(recvCmd->user2().pubcharid()))                    
		{    
			if(recvCmd->user2().charid()){
				stPubUserInfo info;
				info.initFromProto(recvCmd->user2());
				stPubUserInfoDB dbData;
				info.fillDBData(dbData);

				m_pubUserInfoSortList.insertSortInfo(info);
				addRecord(dbData);

#ifdef _HDZ_DEBUG
				Athena::logger->trace("[英雄联赛更新信息add:user2]charID:%d, pubcharID:%d, areacode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubServerIndex:%d, score:%d", info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower, info.m_pubServerIndex, info.m_score);
#endif
			}
		}else{
			stPubUserInfo info = m_pubUserInfoSortList.getSortInfo(recvCmd->user2().pubcharid());
			info.m_battlePower = recvCmd->user2().battlepower();
			info.m_level = recvCmd->user2().level();            
			info.m_vipLv = recvCmd->user2().viplv();            

			info.m_score += recvCmd->user2().score();                                            

			stPubUserInfoDB dbData;
			info.fillDBData(dbData);
			m_pubUserInfoSortList.updateSortInfo(info);
			updateRecord(dbData);
#ifdef _HDZ_DEBUG                              
			Athena::logger->trace("[英雄联赛更新信息update:user2]charID:%d, pubcharID:%d, areacode:%d, name:%s, level:%d, vipLv:%d, job:%d, sex:%d, battlePower:%d, pubServerIndex:%d, score:%d", info.m_charID, info.m_pubCharID, info.m_areaCode, info.m_name, info.m_level, info.m_vipLv, info.m_job, info.m_sex, info.m_battlePower, info.m_pubServerIndex, info.m_score);
#endif
		} 
	}
}

void CHeroLeagueMgr::preSettlement()
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("英雄联赛初赛结算!");
#endif
	UINT32 peopleNum = m_pubUserInfoSortList.getListNum();
	if(0 == peopleNum){
		return ;
	}

	std::vector<stPubUserInfo> vec;
	std::vector<stPubUserInfo> subVec;
	m_pubUserInfoSortList.getFromN12N2(vec, 1,100);

	if(peopleNum > 100){
		std::vector<stPubUserInfo> subVec;
		m_pubUserInfoSortList.getFromN12N2(subVec, 101,peopleNum);
		std::vector<stPubUserInfo>::iterator subIt = subVec.begin();
		stPreliminary cfg = CHeroLeagueCfg::getSingleton().getPreConfiguration();

		for(;subIt != subVec.end(); ++subIt){
			if(g_globalServer->getPubServerIndex() ==  (*subIt).m_pubServerIndex){
				sendMail("MailWnd_27;", cfg.m_participationBOX, (*subIt).m_charID);
			}
		}

		subIt = subVec.begin();
		for(;subIt != subVec.end(); ++subIt){
			removeRecord((*subIt).m_pubCharID);
			m_pubUserInfoSortList.removeSortInfo((*subIt).m_pubCharID); 
		}
	}

	std::vector<stPubUserInfo>::iterator it = vec.begin();
	stPreliminary cfg = CHeroLeagueCfg::getSingleton().getPreConfiguration();
	for(;it != vec.end(); ++it){
		if(g_globalServer->getPubServerIndex() ==  (*it).m_pubServerIndex){
			sendMail("MailWnd_28;", cfg.m_victoryBOX, (*it).m_charID);
		}
	}
}

void extend(stPubUserInfo & user1, stPubUserInfo & user2
		, CHeroLeagueMgr& mgr, std::vector<stPair> & vec, EFINAL_TYPE type)
{
	stPair user;                                       

	user.user1.initFromPubUserInfo(user1, user2);
	user.user2.initFromPubUserInfo(user2, user1);

	stFinalDB data1;                                   
	stFinalDB data2;                                   

	user.user1.fillDBData(data1);
	user.user2.fillDBData(data2);

	if(type != eHERO_LEAGUE_NULL){
		mgr.addFinalRecord(data1,type);        
		mgr.addFinalRecord(data2,type);
	}

	vec.push_back(user);                         
}

void CHeroLeagueMgr::IntermedSettlement()
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("英雄联赛复赛结算!");   
#endif
	UINT32 peopleNum = m_pubUserInfoSortList.getListNum();
	if(0 == peopleNum){
		return ;
	}

	std::vector<stPubUserInfo> vec;
	m_pubUserInfoSortList.getFromN12N2(vec, 1,peopleNum);
	std::vector<stPubUserInfo>::iterator it = vec.begin();
	for(;it != vec.end(); ++it){
		if(g_globalServer->getPubServerIndex() ==  (*it).m_pubServerIndex){
			UINT16 sortRank = m_pubUserInfoSortList.getMySortRank((*it).m_pubCharID);
			UINT32 rewardID = CHeroLeagueCfg::getSingleton().getRewardid(sortRank);

			std::string mailWnd;
			if(sortRank > 16){
				mailWnd = "MailWnd_29;";
			}else{
				mailWnd = "MailWnd_30;";
			}

			sendMail(mailWnd, rewardID, (*it).m_charID);	
		}
	}
	vec.clear();

	//清除往届十六强 八强 四强记录
	{                                           
		m_sixteen.clear();                      
		removeFinalRecord(eHERO_LEAGUE_SIXTEEN);

		m_eight.clear();                        
		removeFinalRecord(eHERO_LEAGUE_EIGHT);  

		m_four.clear();                         
		removeFinalRecord(eHERO_LEAGUE_FOUR);   

		m_two.clear();
		removeFinalRecord(eHERO_LEAGUE_TWO);
	}  

	division();
}

void CHeroLeagueMgr::pairing(std::vector<stPair> & vec, std::vector<stPubUserInfo> & pubVec,EFINAL_TYPE type, UINT8 num)
{
	removeFinalRecord(type);
	vec.clear();
	UINT8 size = num - 1;
	if(0 == num % 2){
		for(UINT8 i = 0; i < num / 2; ++i, --size){
			extend(pubVec[i], pubVec[size], *this, vec, type);
		}
	}else{
		for(UINT8 i = 0; i < num / 2; ++i, --size){
			extend(pubVec[i], pubVec[size], *this, vec, type);
		}
		stPubUserInfo other;
		extend(pubVec[num / 2], other, *this, vec, type);
	}
}

void CHeroLeagueMgr::division()
{
	UINT8 num = m_pubUserInfoSortList.getListNum();
	std::vector<stPubUserInfo> vec; 
	std::vector<stPubUserInfo>::iterator it;
	if(num > 16){
		std::vector<stPubUserInfo> subVec;
		m_pubUserInfoSortList.getFromN12N2(subVec, 17,num);
		std::vector<stPubUserInfo>::iterator subIt = subVec.begin();
		for(;subIt != subVec.end(); ++subIt){
			removeRecord((*subIt).m_pubCharID);
			m_pubUserInfoSortList.removeSortInfo((*subIt).m_pubCharID);
		}
	}
	vec.clear();
	num = m_pubUserInfoSortList.getListNum();
	m_pubUserInfoSortList.getFromN12N2(vec, 1,  num);
	//UINT8 rsize = num -1;
	if(16 >= num && num > 8){
/*		removeFinalRecord(eHERO_LEAGUE_SIXTEEN);
		m_sixteen.clear();
		if(0 == num % 2){
			for(UINT8 i = 0; i < num / 2; ++i, --rsize){
				extend(vec[i], vec[rsize], *this, m_sixteen, eHERO_LEAGUE_SIXTEEN);
			}
		}else{
			for(UINT8 i = 0; i < num / 2; ++i, --rsize){
				extend(vec[i], vec[rsize], *this, m_sixteen, eHERO_LEAGUE_SIXTEEN);
			}
			stPubUserInfo other;
			extend(vec[num / 2], other, *this, m_sixteen, eHERO_LEAGUE_SIXTEEN);
		}
*/
		pairing(m_sixteen, vec, eHERO_LEAGUE_SIXTEEN, num);
	}else if(4 < num){
		/*
		removeFinalRecord(eHERO_LEAGUE_EIGHT);
		m_eight.clear();                      
		if(0 == num % 2){                                                          
			for(UINT8 i = 0; i < num / 2; ++i, --rsize){                           
				extend(vec[i], vec[rsize], *this, m_eight, eHERO_LEAGUE_EIGHT);
			}                                                                      
		}else{                                                                     
			for(UINT8 i = 0; i < num / 2; ++i, --rsize){                           
				extend(vec[i], vec[rsize], *this, m_eight, eHERO_LEAGUE_EIGHT);
			}                                                                      
			stPubUserInfo other;                                                   
			extend(vec[num / 2], other, *this, m_eight, eHERO_LEAGUE_EIGHT);   
		} 
		*/
		pairing(m_eight, vec, eHERO_LEAGUE_EIGHT, num);
	}else if(2 < num){
/*		removeFinalRecord(eHERO_LEAGUE_FOUR);
		m_four.clear();                      
		if(0 == num % 2){                                                      
			for(UINT8 i = 0; i < num / 2; ++i, --rsize){                       
				extend(vec[i], vec[rsize], *this, m_four, eHERO_LEAGUE_FOUR);
			}                                                                  
		}else{                                                                 
			for(UINT8 i = 0; i < num / 2; ++i, --rsize){                       
				extend(vec[i], vec[rsize], *this, m_four, eHERO_LEAGUE_FOUR);
			}                                                                  
			stPubUserInfo other;                                               
			extend(vec[num / 2], other, *this, m_four, eHERO_LEAGUE_FOUR);   
		}
		*/
		pairing(m_four, vec, eHERO_LEAGUE_FOUR, num); 
	}else if(0 < num){
		/*
		removeFinalRecord(eHERO_LEAGUE_TWO); 
		m_two.clear();
		if(0 == num % 2){
			for(UINT8 i = 0; i < num / 2; ++i, --rsize){    
				extend(vec[i], vec[rsize], *this, m_two, eHERO_LEAGUE_TWO);
			}                    
		}else{  
			for(UINT8 i = 0; i < num / 2; ++i, --rsize){    
				extend(vec[i], vec[rsize], *this, m_two, eHERO_LEAGUE_TWO);
			}                    
			stPubUserInfo other;        
			extend(vec[num / 2], other, *this, m_two, eHERO_LEAGUE_TWO);   
		}  
		*/
		pairing(m_two, vec, eHERO_LEAGUE_TWO, num);
	}
}

void CHeroLeagueMgr::endHandle(std::vector<stPair>& vec, EFINAL_TYPE type)
{
	std::vector<stPair>::iterator it = vec.begin();
	for(;it != vec.end(); ++it){//没打的全部失败
		stPair &info = *it;
		if(info.user1.m_winOrLoss == 0){
			if(m_pubUserInfoSortList.isInSortList(info.user1.m_pubCharID)){ 
				FinalExtend(false, info.user1, *this, type);
			}
		}
		if(info.user2.m_winOrLoss == 0){                                        
			if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){
				FinalExtend(false, info.user2, *this, type);
			}
		}
	}

	it = vec.begin();
	for(;it != vec.end();++it){
		stPair &info = *it;
		if(info.user1.m_winOrLoss != 1){//清理没胜利的，只有胜利的才能进入下一轮
			if(m_pubUserInfoSortList.isInSortList(info.user1.m_pubCharID)){
				m_pubUserInfoSortList.removeSortInfo(info.user1.m_pubCharID);
			}
		}
		if(info.user2.m_winOrLoss != 1){
			if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){  
				m_pubUserInfoSortList.removeSortInfo(info.user2.m_pubCharID);
			}
		}
	}
}


void CHeroLeagueMgr::finalSessionSettlement()
{
	endHandle(m_sixteen, eHERO_LEAGUE_SIXTEEN);
	endHandle(m_eight, eHERO_LEAGUE_EIGHT);
	endHandle(m_four, eHERO_LEAGUE_FOUR);

	/*
	//UINT32 eopleNum = m_pubUserInfoSortList.getListNum(); 
	//if(16 >= peopleNum && peopleNum > 8){
	{
		std::vector<stPair>::iterator it = m_sixteen.begin();
		for(;it != m_sixteen.end(); ++it){//没打的全部失败
			stPair &info = *it;
			if(info.user1.m_winOrLoss == 0){
				if(m_pubUserInfoSortList.isInSortList(info.user1.m_pubCharID)){    
					FinalExtend(false, info.user1, *this, eHERO_LEAGUE_SIXTEEN);
				}
			}
			if(info.user2.m_winOrLoss == 0){                                   
				if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){
					FinalExtend(false,info.user2, *this, eHERO_LEAGUE_SIXTEEN);
				}                                                              
			}
		}

		it = m_sixteen.begin();
		for(;it != m_sixteen.end();++it){
			stPair &info = *it;
			if(info.user1.m_winOrLoss != 1){//清理没胜利的，只有胜利的才能进入下一轮
				if(m_pubUserInfoSortList.isInSortList(info.user1.m_pubCharID)){
					m_pubUserInfoSortList.removeSortInfo(info.user1.m_pubCharID);
				}
			}
			if(info.user2.m_winOrLoss != 1){
				if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){   
					m_pubUserInfoSortList.removeSortInfo(info.user2.m_pubCharID); 
				}                                                                 
			}
		}
	}
	//else if(peopleNum > 4){
	{
		std::vector<stPair>::iterator it = m_eight.begin();
		for(;it != m_eight.end(); ++it){
			stPair &info = *it;
			if(info.user1.m_winOrLoss == 0){
				if(m_pubUserInfoSortList.isInSortList(info.user1.m_pubCharID)){    
					FinalExtend(false, info.user1, *this, eHERO_LEAGUE_EIGHT);
				}
			}
			if(info.user2.m_winOrLoss == 0){                                   
				if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){
					FinalExtend(false,info.user2, *this, eHERO_LEAGUE_EIGHT);
				}                                                              
			}
		}

		it = m_eight.begin();
		for(;it != m_eight.end();++it){
			stPair &info = *it;
			if(info.user1.m_winOrLoss != 1){
				if(m_pubUserInfoSortList.isInSortList(info.user1.m_pubCharID)){
					m_pubUserInfoSortList.removeSortInfo(info.user1.m_pubCharID);
				}
			}
			if(info.user2.m_winOrLoss != 1){
				if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){   
					m_pubUserInfoSortList.removeSortInfo(info.user2.m_pubCharID); 
				}                                                                 
			}
		}
	}
//	}else if(peopleNum > 2){
	{
		std::vector<stPair>::iterator it = m_four.begin();
		for(;it != m_four.end(); ++it){
			stPair &info = *it;
			if(info.user1.m_winOrLoss == 0){
				if(m_pubUserInfoSortList.isInSortList(info.user1.m_pubCharID)){    
					FinalExtend(false, info.user1, *this, eHERO_LEAGUE_FOUR);
				}
			}
			if(info.user2.m_winOrLoss == 0){                                   
				if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){
					FinalExtend(false,info.user2, *this, eHERO_LEAGUE_FOUR);
				}                                                              
			}
		}

		it = m_four.begin();
		for(;it != m_four.end();++it){
			stPair &info = *it;
			if(info.user1.m_winOrLoss != 1){
				if(m_pubUserInfoSortList.isInSortList(info.user1.m_pubCharID)){
					m_pubUserInfoSortList.removeSortInfo(info.user1.m_pubCharID);
				}
			}
			if(info.user2.m_winOrLoss != 1){
				if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){   
					m_pubUserInfoSortList.removeSortInfo(info.user2.m_pubCharID); 
				}                                                                 
			}
		}
	}
*/
	division();
}

void CHeroLeagueMgr::finalSettlement()
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("英雄联赛决赛结算");
#endif
	
	stFinalElement * pFinal1 = CHeroLeagueCfg::getSingleton().getFinalsSessionInfo(1);
	stFinalElement * pFinal2 = CHeroLeagueCfg::getSingleton().getFinalsSessionInfo(2);
	stFinalElement * pFinal3 = CHeroLeagueCfg::getSingleton().getFinalsSessionInfo(3);
	stFinalElement * pFinal4 = CHeroLeagueCfg::getSingleton().getFinalsSessionInfo(4);

		std::vector<stPair>::iterator iit = m_two.begin();
		for(;iit != m_two.end(); ++iit){
			stPair &info = *iit;
			if(info.user1.m_winOrLoss == 0){
				if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){
					FinalExtend(false, info.user1, *this, eHERO_LEAGUE_TWO);
				}
			}
			if(info.user2.m_winOrLoss == 0){
				if(m_pubUserInfoSortList.isInSortList(info.user2.m_pubCharID)){  
					FinalExtend(false,info.user2, *this, eHERO_LEAGUE_TWO);      
				}                                                                
			}                                                                    
		}                                                                        

	removeInfoRecord();
	m_pubUserInfoSortList.clear();

	std::vector<stPair>::iterator it = m_sixteen.begin(); 
	for(;it != m_sixteen.end(); ++it){
		stPair& st = *it;
		if(pFinal1 && ((st.user1.m_winOrLoss == 2) || (st.user1.m_winOrLoss == 0))){
		
			if(g_globalServer->getAreaCode() == st.user1.m_areaCode){
				sendMail("MailWnd_31;", pFinal1->m_participationBOX, st.user1.m_charID);
			}
		}

		if(pFinal1 && ((st.user2.m_winOrLoss == 2) || (st.user2.m_winOrLoss == 0))){
			if(g_globalServer->getAreaCode() == st.user2.m_areaCode){
				sendMail("MailWnd_31;", pFinal1->m_participationBOX, st.user2.m_charID);      
			} 
		}
	}

	it = m_eight.begin();
	for(;it != m_eight.end(); ++it){
		stPair& st = *it;                                                                       
		if(pFinal2 && ((st.user1.m_winOrLoss == 2) || (st.user1.m_winOrLoss == 0))){
			if(g_globalServer->getAreaCode() == st.user1.m_areaCode){
				sendMail("MailWnd_32;", pFinal2->m_participationBOX, st.user1.m_charID);
			} 
		}

		if(pFinal2 && ((st.user2.m_winOrLoss == 2) || (st.user2.m_winOrLoss == 0))){
			if(g_globalServer->getAreaCode() == st.user2.m_areaCode){
				sendMail("MailWnd_32;", pFinal2->m_participationBOX, st.user2.m_charID);
			} 
		}
	}

	it = m_four.begin();
	for(;it != m_four.end(); ++it){                                                           
		stPair& st = *it;                                                                      
		if(pFinal3 && ((st.user1.m_winOrLoss == 2) || (st.user1.m_winOrLoss == 0))){
			if(g_globalServer->getAreaCode() == st.user1.m_areaCode){
				sendMail("MailWnd_33;", pFinal3->m_participationBOX, st.user1.m_charID);
			} 
		}

		if(pFinal3 && ((st.user2.m_winOrLoss == 2) || (st.user2.m_winOrLoss == 0))){
			if(g_globalServer->getAreaCode() == st.user2.m_areaCode){
				sendMail("MailWnd_33;", pFinal3->m_participationBOX, st.user2.m_charID);
			} 
		} 
	}                                                                                          

	it = m_two.begin();                                                                       
	for(;it != m_two.end(); ++it){                                                            
		stPair& st = *it;  
		//冠亚军两人都没打的情况
		if(pFinal4 && ((st.user1.m_winOrLoss == 2) || (st.user1.m_winOrLoss == 0))
				&& (((st.user2.m_winOrLoss == 2) || (st.user2.m_winOrLoss == 0)))){
			if(g_globalServer->getAreaCode() == st.user1.m_areaCode){
				sendMail("MailWnd_35;", pFinal4->m_victoryBOX, st.user1.m_charID);
			}

			if(g_globalServer->getAreaCode() == st.user1.m_areaCode){
				sendMail("MailWnd_34;", pFinal4->m_participationBOX, st.user2.m_charID);
			}
			return;
		}

		if(pFinal4 && ((st.user1.m_winOrLoss == 2) || (st.user1.m_winOrLoss == 0))){
			if(g_globalServer->getAreaCode() == st.user1.m_areaCode){
				sendMail("MailWnd_34;", pFinal4->m_participationBOX, st.user1.m_charID);
			} 
		}

		if(pFinal4 && ((st.user2.m_winOrLoss == 2) || (st.user2.m_winOrLoss == 0))){
			if(g_globalServer->getAreaCode() == st.user2.m_areaCode){
				 sendMail("MailWnd_34;", pFinal4->m_participationBOX, st.user2.m_charID);
			} 
		} 

		if(pFinal4 && (st.user1.m_winOrLoss == 1)){
			if(g_globalServer->getAreaCode() == st.user1.m_areaCode){
				sendMail("MailWnd_35;", pFinal4->m_victoryBOX, st.user1.m_charID);
			} 
		}

		if(pFinal4 && (st.user2.m_winOrLoss == 1)){                                            
			if(g_globalServer->getAreaCode() == st.user2.m_areaCode)
			{
				sendMail("MailWnd_35;", pFinal4->m_victoryBOX, st.user2.m_charID);
			}
		} 
	}                                                                                          
}

void CHeroLeagueMgr::sendMail(const std::string& mailWnd, UINT32 itemID, UINT32 charID)
{
#ifdef _HDZ_DEBUG
	Athena::logger->trace("英雄联赛 发奖励 mailWnd:%s, charID:%d",mailWnd.c_str(), charID);
#endif 
	stBatchSendMailTask task;
	char szMailContent[50] = { 0 };

	task.m_title = CSysLanguages::getSingleton().getLanguage(SYS_LANGUAGE_ID_LEAGUE_REWARD);
	snprintf(szMailContent,sizeof(szMailContent),mailWnd.c_str());
	task.m_content = szMailContent;
	task.m_receiveIDs.insert(charID); 
	task.m_attaches.push_back(std::make_pair(itemID,1));
	CBatchSendMailMgr::getSingleton().addTask(task);
}

void CHeroLeagueMgr::calculation(std::vector<stPair> & vec, Player *pUser, SortProto::stQualifiedListResult & retCmd)
{
	if(!pUser){
		return;
	}
	std::vector<stPair>::iterator it = vec.begin();
#ifdef _HDZ_DEBUG
	Athena::logger->trace("英雄联赛 对阵列表 num:%d", vec.size());
#endif 
	for(;it != vec.end();++it){
		stPair & st = *it;
		SortProto::stTeam * team = retCmd.add_users();
		if(team){
			if(st.user1.m_charID == pUser->getID() || st.user2.m_charID == pUser->getID()){
				retCmd.set_isqualified(true);
			}else{
				retCmd.set_isqualified(false);
			}
			st.user1.fillQualifiedList(*(team->mutable_user1()));
			st.user2.fillQualifiedList(*(team->mutable_user2()));
#ifdef _HDZ_DEBUG
			Athena::logger->trace("英雄联赛 对阵列表:charid:%d, charid:%d",st.user1.m_charID, st.user2.m_charID);
#endif
		}
	}
}

void CHeroLeagueMgr::sendQualifiedList(Player *pUser, SortProto::stReqQualifiedList * recvCmd)
{
	if(!pUser || !recvCmd){
		return;
	}

	SortProto::stQualifiedListResult retCmd; 
	if(recvCmd->type() == SortProto::stReqQualifiedList::eSixteen){
		calculation(m_sixteen, pUser, retCmd);
	}

	if(recvCmd->type() == SortProto::stReqQualifiedList::eEight){
		calculation(m_eight, pUser, retCmd);
	}

	if(recvCmd->type() == SortProto::stReqQualifiedList::eFour){
		calculation(m_four, pUser, retCmd);
	}

	if(recvCmd->type() == SortProto::stReqQualifiedList::eTwo){
		calculation(m_two, pUser, retCmd);
	}

	pUser->sendProtoToMe(retCmd);
}

void CHeroLeagueMgr::sendQualifiedListToPub()
{

	//std::vector<stPubUserInfo> testVec;
	//UINT32 testNum = m_pubUserInfoSortList.getListNum();
	//m_pubUserInfoSortList.getFromN12N2(testVec, 1, testNum);

	std::map<UINT32, UINT32>::iterator mapIt = m_charIDs.begin();
	for(;mapIt != m_charIDs.end(); ++mapIt){
		if(m_pubUserInfoSortList.isInSortList(mapIt->first)){
			m_pubUserInfoSortList.removeSortInfo(mapIt->first);	
		}
	}
	m_charIDs.clear();

	Global::stSynRoleListResult retCmd;
	std::vector<stPubUserInfo> vec;
	UINT32 num = m_pubUserInfoSortList.getListNum();
//	m_pubUserInfoSortList.getFromN12N2(vec, 1, num);
//	std::vector<stPubUserInfo>::iterator it = vec.begin();

	stIntermediary config = CHeroLeagueCfg::getSingleton().getIntermediary();
	UINT32 time = getNowTime();
	if((time <= config.m_openTime) && (time > config.m_openTime - 180)){//复赛同步
		m_pubUserInfoSortList.getFromN12N2(vec, 1, num);
		std::vector<stPubUserInfo>::iterator it = vec.begin();
		for(;it != vec.end(); ++it){
			Global::stPubRoleInfo * stinfo = retCmd.add_charids();
			if(stinfo){
				stPubUserInfo & info = *it;
				info.fillstTeam(*stinfo);	
			}
		}
		retCmd.set_isfinal(false);
	}else {//决赛同步
		retCmd.set_isfinal(true);

		if(num <= 16 && num > 8){
			std::vector<stPair>::iterator it = m_sixteen.begin();
			for(;it != m_sixteen.end(); ++it){
				Global::stTeam * pTeam = retCmd.add_team();
				stPair & team = *it;

				pTeam->mutable_user1()->set_charid(team.user1.m_charID);
				pTeam->mutable_user1()->set_pubcharid(team.user1.m_pubCharID);
				pTeam->mutable_user1()->set_areacode(team.user1.m_areaCode);
				pTeam->mutable_user1()->set_name(team.user1.m_name);
				pTeam->mutable_user1()->set_level(team.user1.m_level);
				pTeam->mutable_user1()->set_viplv(team.user1.m_vipLv);
				pTeam->mutable_user1()->set_job(team.user1.m_job);
				pTeam->mutable_user1()->set_sex(team.user1.m_sex);
				pTeam->mutable_user1()->set_battlepower(team.user1.m_battlePower);
				pTeam->mutable_user1()->set_photoid(team.user1.m_photoID);

				pTeam->mutable_user2()->set_charid(team.user2.m_charID);           
				pTeam->mutable_user2()->set_pubcharid(team.user2.m_pubCharID);     
				pTeam->mutable_user2()->set_areacode(team.user2.m_areaCode);       
				pTeam->mutable_user2()->set_name(team.user2.m_name);               
				pTeam->mutable_user2()->set_level(team.user2.m_level);             
				pTeam->mutable_user2()->set_viplv(team.user2.m_vipLv);             
				pTeam->mutable_user2()->set_job(team.user2.m_job);                 
				pTeam->mutable_user2()->set_sex(team.user2.m_sex);                 
				pTeam->mutable_user2()->set_battlepower(team.user2.m_battlePower); 
				pTeam->mutable_user2()->set_photoid(team.user2.m_photoID); 
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[英雄联赛 synToPub 16]size :%d, 对阵:charid:%d, charid:%d",m_sixteen.size(),team.user1.m_charID, team.user2.m_charID);
#endif
			}
		}
		else if(num > 4){
			std::vector<stPair>::iterator it = m_eight.begin();          
			for(;it != m_eight.end(); ++it){  
				Global::stTeam * pTeam = retCmd.add_team();
				stPair & team = *it;  

				pTeam->mutable_user1()->set_charid(team.user1.m_charID);           
				pTeam->mutable_user1()->set_pubcharid(team.user1.m_pubCharID);     
				pTeam->mutable_user1()->set_areacode(team.user1.m_areaCode);       
				pTeam->mutable_user1()->set_name(team.user1.m_name);               
				pTeam->mutable_user1()->set_level(team.user1.m_level);             
				pTeam->mutable_user1()->set_viplv(team.user1.m_vipLv);             
				pTeam->mutable_user1()->set_job(team.user1.m_job);                 
				pTeam->mutable_user1()->set_sex(team.user1.m_sex);                 
				pTeam->mutable_user1()->set_battlepower(team.user1.m_battlePower); 
				pTeam->mutable_user1()->set_photoid(team.user1.m_photoID);

				pTeam->mutable_user2()->set_charid(team.user2.m_charID);           
				pTeam->mutable_user2()->set_pubcharid(team.user2.m_pubCharID);     
				pTeam->mutable_user2()->set_areacode(team.user2.m_areaCode);       
				pTeam->mutable_user2()->set_name(team.user2.m_name);               
				pTeam->mutable_user2()->set_level(team.user2.m_level);             
				pTeam->mutable_user2()->set_viplv(team.user2.m_vipLv);             
				pTeam->mutable_user2()->set_job(team.user2.m_job);                 
				pTeam->mutable_user2()->set_sex(team.user2.m_sex);                 
				pTeam->mutable_user2()->set_battlepower(team.user2.m_battlePower); 
				pTeam->mutable_user2()->set_photoid(team.user2.m_photoID); 
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[英雄联赛 synToPub 8]size:%d, 对阵:charid:%d, charid:%d",m_eight.size(), team.user1.m_charID, team.user2.m_charID);
#endif 
			}                                                              
		}
		else if(num > 2){
			std::vector<stPair>::iterator it = m_four.begin();
			for(;it != m_four.end(); ++it){
				Global::stTeam * pTeam = retCmd.add_team(); 
				stPair & team = *it;
				pTeam->mutable_user1()->set_charid(team.user1.m_charID);           
				pTeam->mutable_user1()->set_pubcharid(team.user1.m_pubCharID);     
				pTeam->mutable_user1()->set_areacode(team.user1.m_areaCode);       
				pTeam->mutable_user1()->set_name(team.user1.m_name);               
				pTeam->mutable_user1()->set_level(team.user1.m_level);             
				pTeam->mutable_user1()->set_viplv(team.user1.m_vipLv);             
				pTeam->mutable_user1()->set_job(team.user1.m_job);                 
				pTeam->mutable_user1()->set_sex(team.user1.m_sex);                 
				pTeam->mutable_user1()->set_battlepower(team.user1.m_battlePower); 
				pTeam->mutable_user1()->set_photoid(team.user1.m_photoID); 		

				pTeam->mutable_user2()->set_charid(team.user2.m_charID);           
				pTeam->mutable_user2()->set_pubcharid(team.user2.m_pubCharID);     
				pTeam->mutable_user2()->set_areacode(team.user2.m_areaCode);       
				pTeam->mutable_user2()->set_name(team.user2.m_name);               
				pTeam->mutable_user2()->set_level(team.user2.m_level);             
				pTeam->mutable_user2()->set_viplv(team.user2.m_vipLv);             
				pTeam->mutable_user2()->set_job(team.user2.m_job);                 
				pTeam->mutable_user2()->set_sex(team.user2.m_sex);                 
				pTeam->mutable_user2()->set_battlepower(team.user2.m_battlePower); 
				pTeam->mutable_user2()->set_photoid(team.user2.m_photoID); 
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[英雄联赛 synToPub 4]size:%d, 对阵:charid:%d, charid:%d",m_four.size(), team.user1.m_charID, team.user2.m_charID);
#endif
			}                                                              
		}
		else if(num > 0){
			std::vector<stPair>::iterator it = m_two.begin();
			for(;it != m_two.end(); ++it){
				Global::stTeam * pTeam = retCmd.add_team();
				stPair & team = *it;
				pTeam->mutable_user1()->set_charid(team.user1.m_charID);           
				pTeam->mutable_user1()->set_pubcharid(team.user1.m_pubCharID);     
				pTeam->mutable_user1()->set_areacode(team.user1.m_areaCode);       
				pTeam->mutable_user1()->set_name(team.user1.m_name);               
				pTeam->mutable_user1()->set_level(team.user1.m_level);             
				pTeam->mutable_user1()->set_viplv(team.user1.m_vipLv);             
				pTeam->mutable_user1()->set_job(team.user1.m_job);                 
				pTeam->mutable_user1()->set_sex(team.user1.m_sex);                 
				pTeam->mutable_user1()->set_battlepower(team.user1.m_battlePower); 
				pTeam->mutable_user1()->set_photoid(team.user1.m_photoID); 

				pTeam->mutable_user2()->set_charid(team.user2.m_charID);           
				pTeam->mutable_user2()->set_pubcharid(team.user2.m_pubCharID);     
				pTeam->mutable_user2()->set_areacode(team.user2.m_areaCode);       
				pTeam->mutable_user2()->set_name(team.user2.m_name);               
				pTeam->mutable_user2()->set_level(team.user2.m_level);             
				pTeam->mutable_user2()->set_viplv(team.user2.m_vipLv);             
				pTeam->mutable_user2()->set_job(team.user2.m_job);                 
				pTeam->mutable_user2()->set_sex(team.user2.m_sex);                 
				pTeam->mutable_user2()->set_battlepower(team.user2.m_battlePower); 
				pTeam->mutable_user2()->set_photoid(team.user2.m_photoID); 
#ifdef _HDZ_DEBUG
				Athena::logger->trace("[英雄联赛 synToPub 2]size:%d, 对阵:charid:%d, charid:%d",m_two.size(), team.user1.m_charID, team.user2.m_charID);
#endif
			}                                                               
		}

	}
	g_globalServer->getConnMgr().broadcastByType(GAMESERVER,retCmd);	
}

bool CHeroLeagueMgr::isSameAreaCode(Player *pUser)
{
	if(!pUser){
		return false;
	}

	std::map<UINT32, stChampionInfo>::iterator it = m_championMap.begin();
	for(;it != m_championMap.end(); ++it){
		stChampionInfo &info = it->second;
		if(pUser->m_areaCode == info.m_areaCode){
			return true;
		}
	}
	return false;
}

void CHeroLeagueMgr::sendHeroesList(Player *pUser, SortProto::stReqHeroList *recvCmd)
{
	if(!pUser){
		return;
	}

	SortProto::stHeroListResult retCmd;

	if(m_rankSortList.getListNum()){
		std::vector<stRankList> vec;
		m_rankSortList.getFromN12N2(vec, 1, m_rankSortList.getListNum());

		stRankList rankInfo = m_rankSortList.getSortInfo(m_rankSortList.getLastOneKey());
		UINT32 session = rankInfo.m_session;
		if(recvCmd->type() == SortProto::stReqHeroList_stType_eUpperBounds1){
		}

		if(recvCmd->type() == SortProto::stReqHeroList_stType_eUpperBounds2){
			session -= 1;
		}

		if(recvCmd->type() == SortProto::stReqHeroList_stType_eUpperBounds3){
			session -= 2;
		}

		std::vector<stRankList>::iterator it = vec.begin();
		for(;it != vec.end(); ++it){
			stRankList & user = *it;
			if(user.m_session == session){
				SortProto::stRankInfo *info = retCmd.add_user();
				info->set_session(user.m_session);
				info->set_rank(user.m_rank);
				info->set_charid(user.m_charID);
				info->set_pubcharid(user.m_pubCharID);
				info->set_areacode(user.m_areaCode);
				info->set_name(user.m_name);
				info->set_level(user.m_level);
				info->set_viplv(user.m_vipLv);
				info->set_job(user.m_job);
				info->set_sex(user.m_sex);
				info->set_battlepower(user.m_battlePower);
				info->set_photoid(user.m_photoID);
			}
		}
	}
	pUser->sendProtoToMe(retCmd);
}

void CHeroLeagueMgr::reqRedPoint(Global::stReqRedPoint *recvCmd)
{
	Global::stRedPointResult retCmd;

	Player * pUser = PlayerManager::getSingleton().getUserByID(recvCmd->charid());
	if(!recvCmd || !pUser){
		return;
	}

	if(!m_pubUserInfoSortList.isInSortList(recvCmd->pubcharid())){                                        
		retCmd.set_isqualified(false);
	}else{
		retCmd.set_isqualified(true);
	}

	if(getChampionNum()){
		retCmd.set_canworship(true);	
	}else{
		retCmd.set_canworship(false);
	}

	retCmd.set_charid(recvCmd->charid());

	pUser->sendProto2Game(retCmd);
}

bool isSameCode(stFinalInfo &info, Player * pUser)
{
	if(pUser){
		if((info.m_charID == pUser->getID()) && (info.m_areaCode == pUser->m_areaCode)){
			return true;
		}
		return false;
	}
	return false;
}

void CHeroLeagueMgr::synUpdateRecord(std::vector<stPair> & vec, Player *  pUser,EFINAL_TYPE type)
{
	if(!pUser){
		return;
	}

	std::vector<stPair>::iterator it = vec.begin();
	for(; it != vec.end(); ++it){
		stPair & st = *it;

		if(isSameCode(st.user1, pUser)){
			strncpy(st.user1.m_name, pUser->getName(),sizeof(st.user1.m_name));
			st.user1.m_photoID = pUser->m_photoID;

			stFinalDB data;
			st.user1.fillDBData(data);
			updateFinalRecordByCharID(data,type);
		}

		if(isSameCode(st.user2, pUser)){
			strncpy(st.user2.m_name, pUser->getName(),sizeof(st.user2.m_name));
			st.user2.m_photoID = pUser->m_photoID;

			stFinalDB data;
			st.user2.fillDBData(data);
			updateFinalRecordByCharID(data,type);
		}
	}                                                                           

}

void CHeroLeagueMgr::synRecord(Player * pUser)
{
	if(!pUser){
		return;
	}

	std::vector<stPubUserInfo> vec;
	m_pubUserInfoSortList.getFromN12N2(vec, 1, m_pubUserInfoSortList.getListNum());

	std::vector<stPubUserInfo>::iterator vecIt = vec.begin();
	for(; vecIt != vec.end(); ++vecIt){
		stPubUserInfo & info = *vecIt;
		if((info.m_charID == pUser->getID()) && info.m_areaCode == pUser->m_areaCode){
			strncpy(info.m_name, pUser->getName(), sizeof(info.m_name));
			info.m_photoID = pUser->m_photoID;
			m_pubUserInfoSortList.updateSortInfo(info);

			stPubUserInfoDB dbData;
			info.fillDBData(dbData);
			updateRecord(dbData);
		}
	}

	synUpdateRecord(m_sixteen, pUser, eHERO_LEAGUE_SIXTEEN);
	synUpdateRecord(m_eight, pUser, eHERO_LEAGUE_EIGHT);
	synUpdateRecord(m_four, pUser, eHERO_LEAGUE_FOUR);
	synUpdateRecord(m_two, pUser, eHERO_LEAGUE_TWO);


	std::map<UINT32, stChampionInfo>::iterator mapIt = m_championMap.find(pUser->getID());
	if((mapIt != m_championMap.end())){
		stChampionInfo & info = mapIt->second;
		if(info.m_areaCode == pUser->m_areaCode){
			strncpy(info.m_name, pUser->getName(), sizeof(info.m_name));
			info.m_photoID = pUser->m_photoID;

			stChampionDB  data;
			info.fillDBData(data);
			removeAllChamopionRecord();
			addChamopionRecord(data);
		}
	}

	std::vector<stRankList> rVec;
	m_rankSortList.getFromN12N2(rVec,1,m_rankSortList.getListNum());
	std::vector<stRankList>::iterator rIt = rVec.begin();
	for(; rIt != rVec.end(); ++rIt){
		stRankList & info = *rIt;
		if((info.m_charID == pUser->getID()) && (info.m_areaCode == pUser->m_areaCode)){
			strncpy(info.m_name, pUser->getName(), sizeof(info.m_name));
			info.m_photoID = pUser->m_photoID;

			m_rankSortList.updateSortInfo(info);

			stRankListDB data1;
			info.fillDBData(data1);
			updateListRecord(data1);

		}
	}
}
