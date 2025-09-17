#ifndef __HERO_LEAGUE_MGR_H__
#define __HERO_LEAGUE_MGR_H__

#include "Singleton.h"
#include "TypesDef.h"
#include "CommonMicro.h"
#include "SortList.h"
#include "SortList.pb.h"
#include "Global.pb.h"
#include "Player.h"
#include <ext/hash_map>
#include <map>
#include <set>

#pragma pack(1)
struct stChampionDB
{
	UINT32	m_charID;    	         	//角色ID
	char 	m_name[MAX_NAMESIZE + 1];	//名字
	UINT32	m_areaCode;					//区Id
	UINT16 	m_level;        	      	//等级
	UINT8 	m_job;               		//职业
	UINT8 	m_sex;              	  	//性别
	UINT32 	m_battlePower;        		//战力
	UINT32 	m_headID;             		//头部ID
	UINT32 	m_bodyID;             		//衣服ID
	UINT32 	m_weaponID;           		//武器ID
	UINT32 	m_cloackID;          		//披风ID
	UINT32 	m_wingID;            		//翅膀ID
	UINT32 	m_shoeID;            		//鞋子
	UINT32 	m_fashionHair;       		//发型
	UINT32 	m_fashionFace;       		//表情
	UINT32 	m_fashionBody;       		//衣服
	UINT32 	m_fashionWeapon;     		//武器
	UINT32 	m_fashionShoe;       		//鞋子
	UINT32 	m_fashionDecoration;  		//脸饰品
	UINT32 	m_effectID;          		//特效ID
	UINT32 	m_wingBreakLv;        		//翅膀突破等级
	UINT16	m_photoID;					//头像ID

	stChampionDB()
	{
		m_charID			=	0;
		bzero(m_name,sizeof(m_name));
		m_areaCode			=	0;
		m_level				=	0;            
		m_job				=	0;              
		m_sex				=	0;              
		m_battlePower		=	0;      
		m_headID			=	0;
		m_bodyID			=	0;
		m_weaponID			=	0;
		m_cloackID			=	0;
		m_wingID			=	0;
		m_shoeID			=	0;
		m_fashionHair		=	0;
		m_fashionFace		=	0;      
		m_fashionBody		=	0;
		m_fashionWeapon		=	0;    
		m_fashionShoe		=	0;      
		m_fashionDecoration	=	0;
		m_effectID			=	0;         
		m_wingBreakLv		=	0;      
		m_photoID			=	0;
	}
};
#pragma pack()

struct stChampionInfo
{
	UINT32	m_charID;    	         	//角色ID
	char 	m_name[MAX_NAMESIZE + 1];	//名字
	UINT32  m_areaCode; 	 	        //区ID
	UINT16 	m_level;        	      	//等级
	UINT8 	m_job;               		//职业
	UINT8 	m_sex;              	  	//性别
	UINT32 	m_battlePower;        		//战力
	UINT32 	m_headID;             		//头部ID
	UINT32 	m_bodyID;             		//衣服ID
	UINT32 	m_weaponID;           		//武器ID
	UINT32 	m_cloackID;          		//披风ID
	UINT32 	m_wingID;            		//翅膀ID
	UINT32 	m_shoeID;            		//鞋子
	UINT32 	m_fashionHair;       		//发型
	UINT32 	m_fashionFace;       		//表情
	UINT32 	m_fashionBody;       		//衣服
	UINT32 	m_fashionWeapon;     		//武器
	UINT32 	m_fashionShoe;       		//鞋子
	UINT32 	m_fashionDecoration;  		//脸饰品
	UINT32 	m_effectID;          		//特效ID
	UINT32  m_wingBreakLv;              //翅膀突破等级
	UINT16	m_photoID;					//头像id
	
	stChampionInfo()
	{
		m_charID			=	0;
		bzero(m_name,sizeof(m_name));
		m_areaCode			=	0;
		m_level				=	0;            
		m_job				=	0;              
		m_sex				=	0;              
		m_battlePower		=	0;      
		m_headID			=	0;
		m_bodyID			=	0;
		m_weaponID			=	0;
		m_cloackID			=	0;
		m_wingID			=	0;
		m_shoeID			=	0;
		m_fashionHair		=	0;
		m_fashionFace		=	0;      
		m_fashionBody		=	0;
		m_fashionWeapon		=	0;    
		m_fashionShoe		=	0;      
		m_fashionDecoration	=	0;
		m_effectID			=	0;         
		m_wingBreakLv		=	0;      
		m_photoID			=	0;
	}

	void initFromDB(stChampionDB & data);
	void fillDBData(stChampionDB & data);
};

#pragma pack(1)
struct stPubUserInfoDB
{
	UINT32	m_charID;					//角色ID
	UINT32	m_pubCharID;				//pub上角色ID
	UINT32	m_areaCode;					//区ID
	char    m_name[MAX_NAMESIZE + 1];   //名字
	UINT16	m_level;					//等级
	UINT16	m_vipLv;					//VIP等级
	UINT8	m_job;						//职业
	UINT8	m_sex;						//性别
	UINT32	m_battlePower;				//战力
	UINT32	m_pubServerIndex;			//所在服务器ID
	UINT32	m_score;					//积分
	UINT16	m_photoID;					//头像ID

	stPubUserInfoDB()
	{
		m_charID			=	0;
		m_pubCharID			=	0;
		m_areaCode			=	0;
		bzero(m_name,sizeof(m_name));
		m_level				=	0;
		m_vipLv				=	0;
		m_job				=	0;
		m_sex				=	0;
		m_battlePower		=	0;
		m_pubServerIndex	=	0;
		m_score				=	0;
		m_photoID			=	0;
	}
};
#pragma pack()

struct stPubUserInfo
{
	UINT32	m_charID;					//角色ID
	UINT32	m_pubCharID;				//pub上角色ID
	UINT32	m_areaCode;					//区ID
	char    m_name[MAX_NAMESIZE + 1];   //名字
	UINT16	m_level;					//等级
	UINT16	m_vipLv;					//VIP等级
	UINT8	m_job;						//职业
	UINT8	m_sex;						//性别
	UINT32	m_battlePower;				//战力
	UINT32	m_pubServerIndex;			//所在服务器ID
	UINT32	m_score;					//积分
	UINT32	m_photoID;					//头像ID

	stPubUserInfo()
	{
		m_charID			=	0;
		m_pubCharID			=	0;
		m_areaCode			=	0;
		bzero(m_name,sizeof(m_name));
		m_level				=	0;
		m_vipLv				=	0;
		m_job				=	0;
		m_sex				=	0;
		m_battlePower		=	0;
		m_pubServerIndex	=	0;
		m_score				=	0;
		m_photoID			=	0;
	}

	bool operator < (const stPubUserInfo & one) const
	{
		return m_score > one.m_score;
	}

	UINT32 getKey() const
	{
		//跨服普通ID可能重复,以pub上的ID作为唯一ID
		return m_pubCharID;
	}

	UINT32 getCmpValue() const
	{
	    return m_score;
	}

	void initFromDB(stPubUserInfoDB & data)
	{
		m_charID			=	data.m_charID;
		m_pubCharID			=	data.m_pubCharID;
		m_areaCode			=	data.m_areaCode;
		strncpy(m_name, data.m_name, sizeof(m_name));
		m_level				=	data.m_level;
		m_vipLv				=	data.m_vipLv;
		m_job				=	data.m_job;
		m_sex				=	data.m_sex;
		m_battlePower		=	data.m_battlePower;
		m_pubServerIndex	=	data.m_pubServerIndex;
		m_score				=	data.m_score;
		m_photoID			=	data.m_photoID;
	}

	void fillDBData(stPubUserInfoDB & data)
	{
		data.m_charID			=	m_charID;
		data.m_pubCharID		=	m_pubCharID;
		data.m_areaCode			=	m_areaCode;
		strncpy(data.m_name, m_name, sizeof(m_name));
		data.m_level			=	m_level;
		data.m_vipLv			=	m_vipLv;
		data.m_job				=	m_job;
		data.m_sex				=	m_sex;
		data.m_battlePower		=	m_battlePower;
		data.m_pubServerIndex   =   m_pubServerIndex;
		data.m_score			=	m_score;
		data.m_photoID			=	m_photoID;
	}

	void initFromProto(const Global::stPubRoleInfo &info)
	{
		m_charID        	=	info.charid();
		m_pubCharID      	=	info.pubcharid();
		m_areaCode       	=	info.areacode();
		strncpy(m_name, info.name().c_str(), sizeof(m_name));
		m_level          	=	info.level();
		m_vipLv          	=	info.viplv();
		m_job            	=	info.job();
		m_sex            	=	info.sex();
		m_battlePower    	=	info.battlepower();
		m_pubServerIndex 	=	info.pubserverindex();
		m_score   		 	=	info.score();
		m_photoID			=	info.photoid();
	}


	void fillstTeam(Global::stPubRoleInfo & pTeam)
	{                                                   
		pTeam.set_charid(m_charID);
		pTeam.set_pubcharid(m_pubCharID);
		pTeam.set_areacode(m_areaCode);
		pTeam.set_name(m_name);
		pTeam.set_level(m_level);
		pTeam.set_viplv(m_vipLv);
		pTeam.set_job(m_job);
		pTeam.set_sex(m_sex);
		pTeam.set_battlepower(m_battlePower);
		pTeam.set_pubserverindex(m_pubServerIndex);
		pTeam.set_score(m_score);
		pTeam.set_photoid(m_photoID);
	}                                                   
};


#pragma pack(1)
struct stFinalDB
{
	UINT32	m_match;					//与之匹配的角色id
	UINT8 	m_winOrLoss;				//胜负(胜：1，负：2, 没打：0)
	UINT32	m_charID;					//id
	UINT32  m_pubCharID;                //pub上角色ID
	UINT32  m_areaCode;                 //区ID       
	char    m_name[MAX_NAMESIZE + 1];   //名字       
	UINT16  m_level;                    //等级       
	UINT16  m_vipLv;                    //VIP等级    
	UINT8   m_job;                      //职业       
	UINT8   m_sex;                      //性别       
	UINT32  m_battlePower;              //战力       
	UINT16	m_photoID;					//头像ID

	stFinalDB()
	{
		m_match				=		0;
		m_winOrLoss			=		0;             
		m_charID			=		0;                
		m_pubCharID			=		0;
		m_areaCode			=		0;
		bzero(m_name,sizeof(m_name));
		m_level				=		0;
		m_vipLv				=		0;
		m_job				=		0;
		m_sex				=		0;
		m_battlePower		=		0;
		m_photoID			=		0;	
	}
};
#pragma pack()

struct stFinalInfo
{
	UINT32 	m_match;					//与之匹配的角色id
	UINT8  	m_winOrLoss;				//胜负(胜：1，负：2, 没打：0)
	UINT32 	m_charID;					//id
	UINT32  m_pubCharID;                //pub上角色ID
	UINT32  m_areaCode;                 //区ID       
	char    m_name[MAX_NAMESIZE + 1];   //名字       
	UINT16  m_level;                    //等级       
	UINT16  m_vipLv;                    //VIP等级    
	UINT8   m_job;                      //职业       
	UINT8   m_sex;                      //性别       
	UINT32  m_battlePower;              //战力       
	UINT16	m_photoID;					//头像ID

	stFinalInfo()
	{
		m_match				=		0;
		m_winOrLoss			=		0;             
		m_charID			=		0;                
		m_pubCharID			=		0;
		m_areaCode			=		0;
		bzero(m_name,sizeof(m_name));
		m_level				=		0;
		m_vipLv				=		0;
		m_job				=		0;
		m_sex				=		0;
		m_battlePower		=		0;
		m_photoID			=		0;
	}

	void initFromDB(stFinalDB & data);
	void fillDBData(stFinalDB & data);
	void copyData(const stFinalInfo & data);
	void initFromPubUserInfo(stPubUserInfo & user1, stPubUserInfo & user2);
	void fillQualifiedList(SortProto::stRoleBasicInfo & data)
	{
		data.set_areacode(m_areaCode);      
		data.set_name(m_name);              
		data.set_leve(m_level);             
		data.set_viplv(m_vipLv);            
		data.set_job(m_job);                
		data.set_sex(m_sex);                
		data.set_battlepower(m_battlePower);
		data.set_photoid(m_photoID);
		if(1 == m_winOrLoss){
			data.set_winorloss(SortProto::stRoleBasicInfo::eSTATE_WIN);
		}else if(2 == m_winOrLoss){
			data.set_winorloss(SortProto::stRoleBasicInfo::eSTATE_LOSS);
		}else{
			data.set_winorloss(SortProto::stRoleBasicInfo::esTATE_NOT_ATTEND);
		}
	}
};

#pragma pack(1)                                             
struct stRankListDB                                      
{                
	UINT32 	m_session;					//第几届
	UINT32  m_rank;                     //排名
	UINT32  m_charID;                   //角色ID            
	UINT32  m_pubCharID;                //pub上角色ID       
	UINT32  m_areaCode;                 //区ID              
	char    m_name[MAX_NAMESIZE + 1];   //名字              
	UINT16  m_level;                    //等级              
	UINT16  m_vipLv;                    //VIP等级           
	UINT8   m_job;                      //职业              
	UINT8   m_sex;                      //性别              
	UINT32  m_battlePower;              //战力              
	UINT16	m_photoID;					//头像ID

	stRankListDB()                                       
	{
		m_session			=	0;
		m_rank				=	0;   
		m_charID            =   0;                          
		m_pubCharID         =   0;                          
		m_areaCode          =   0;                          
		bzero(m_name,sizeof(m_name));                       
		m_level             =   0;                          
		m_vipLv             =   0;                          
		m_job               =   0;                          
		m_sex               =   0;                          
		m_battlePower       =   0;                          
		m_photoID			=	0;
	}                                                       
};                                                          
#pragma pack()                                              

struct stRankList
{
	UINT32  m_session;                  //第几届
	UINT32  m_rank;                     //排名
	UINT32  m_charID;                   //角色ID     
	UINT32  m_pubCharID;                //pub上角色ID
	UINT32  m_areaCode;                 //区ID       
	char    m_name[MAX_NAMESIZE + 1];   //名字       
	UINT16  m_level;                    //等级       
	UINT16  m_vipLv;                    //VIP等级    
	UINT8   m_job;                      //职业       
	UINT8   m_sex;                      //性别       
	UINT32  m_battlePower;              //战力       
	UINT16	m_photoID;					//头像ID

	stRankList()
	{                
		m_session           =   0;
		m_rank              =   0;
		m_charID            =   0;
		m_pubCharID         =   0;                   
		m_areaCode          =   0;                   
		bzero(m_name,sizeof(m_name));                
		m_level             =   0;                   
		m_vipLv             =   0;                   
		m_job               =   0;                   
		m_sex               =   0;                   
		m_battlePower       =   0;                   
		m_photoID			=	0;
	}

	bool operator < (const stRankList & one) const
	{                                                
		return one.m_session > m_session;    
	}

	UINT32 getKey() const                           
	{                                               
		//跨服普通ID可能重复,以pub上的ID作为唯一ID
		return m_pubCharID;                         
	}
	
	UINT32 getCmpValue() const                      
	{
		return m_session;                       
	}     

	void initFromDB(stRankListDB & data);
	void fillDBData(stRankListDB & data);
	void initFromPubUserInfo(stPubUserInfo & data);
	void initFromFinalInfo(stFinalInfo & data);
};

struct stPair
{
	stFinalInfo user1;
	stFinalInfo user2;
};

enum EFINAL_TYPE{
	eHERO_LEAGUE_NULL,
	eHERO_LEAGUE_SIXTEEN,
	eHERO_LEAGUE_EIGHT,
	eHERO_LEAGUE_FOUR,
	eHERO_LEAGUE_TWO,
};

typedef CSortList<stPubUserInfo, 10000> CPubUserInfoSortList;

typedef CSortList<stRankList, 1000> CRankSortList;

class CHeroLeagueMgr
{
	USE_SINGLETON_NEW(CHeroLeagueMgr);
private :
	//析构函数
	CHeroLeagueMgr();
public:
	//构造函数
	~CHeroLeagueMgr() {}
	//返回管理实例
	static CHeroLeagueMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	void load();
	//定时器
	void timer(UINT32 cur);
	
	//增加一个冠军记录
	void addChamopionRecord(stChampionDB & data); 
	//增加一个冠军记录回调
	void addChamopionRecordCB(bool opResult,UINT64 autoID);
	//更新冠军记录
	void updateChamopionRecord(stChampionDB & data);
	//更新冠军记录回调
	void updateChamopionRecordCB(bool opResult,UINT64 autoID);
	//删除所有记录
	void removeAllChamopionRecord();
	//删除所有记录回调
	void removeAllChamopionRecordCB(bool opResult,UINT64 autoID);
	
	//增加一个记录
	void addRecord(stPubUserInfoDB & data);
	//增加记录回调
	void addRecordCB(bool opResult,UINT64 autoID);
	//更新记录
	void updateRecord(stPubUserInfoDB & data);
	//更新记录回调
	void updateRecordCB(bool opResult,UINT64 autoID);
	//删除一条记录
	void removeRecord(UINT32 key);
	//删除一条记录回调
	void removeRecordCB(bool opResult,UINT64 autoID);
	//删除所有记录
	void removeInfoRecord(); 
	//删除所有记录回调
	void removeInfoRecordCB(bool opResult,UINT64 autoID);
	
	//增加一条排行榜记录                         
	void addListRecord(stRankListDB & data);
	//增加一条排行榜记录回调
	void addListRecordCB(bool opResult,UINT64 autoID);
	//更新一条排行榜记录
	void updateListRecord(stRankListDB & data);
	//更新一条排行榜记录回调
	void updateListRecordCB(bool opResult,UINT64 autoID);
	//删除一条记录                                   
	void removeListRecord(UINT32 key);                   
	//删除一条记录回调                               
	void removeListRecordCB(bool opResult,UINT64 autoID);
	
	//增加一条决赛记录                                 
	void addFinalRecord(stFinalDB & data, EFINAL_TYPE type = eHERO_LEAGUE_SIXTEEN);             
	//增加一条决赛记录回调                             
	void addFinalRecordCB(bool opResult,UINT64 autoID);
	//更新记录
	void updateFinalRecord(stFinalDB & data, EFINAL_TYPE type = eHERO_LEAGUE_SIXTEEN);       
	void updateFinalRecordByCharID(stFinalDB & data, EFINAL_TYPE type);
	//更新记录回调
	void updateFinalRecordCB(bool opResult,UINT64 autoID);
	//删除决赛记录
	void removeFinalRecord(EFINAL_TYPE type = eHERO_LEAGUE_SIXTEEN);
	//删除决赛记录回调
	void removeFinalRecordCB(bool opResult,UINT64 autoID);
	
	//打开窗口
	void openWindow(Player *pUser);
	//更新冠军名单
	void updateChampionInfo(Global::stReqSynChampionInfo *recvCmd);
	//更新报名信息
	void updateAllRecord(Global::stHeroLeagueResult *recvCmd);
	void calculation(std::vector<stPair> & vec, Global::stHeroLeagueResult * recvCmd, EFINAL_TYPE type);
	//初赛结算
	void preSettlement();
	//复赛结算
	void IntermedSettlement();

	//决赛划分->十六强、八强、四强
	void division();
	//匹配
	void pairing(std::vector<stPair> & vec, std::vector<stPubUserInfo> & pubVec, EFINAL_TYPE type, UINT8 num);

	//决赛场次结算
	void finalSessionSettlement();
	//决赛场次结束处理
	void endHandle(std::vector<stPair>& vec, EFINAL_TYPE type);

	//决赛结算
	void finalSettlement();
	//发奖励
	void sendMail(const std::string& mailWnd, UINT32 itemID, UINT32 charID);
	//发送资格名单
	void sendQualifiedList(Player *pUser, SortProto::stReqQualifiedList * recvCmd);
	void calculation(std::vector<stPair> & vec, Player *pUser, SortProto::stQualifiedListResult & retCmd);
	//历届英雄榜
	void sendHeroesList(Player *pUser, SortProto::stReqHeroList *recvCmd);
	//发送资格名到pub
	void sendQualifiedListToPub();
	//是否有冠军朝拜
	bool  getChampionNum(){
		if(m_championMap.empty()){
			return false;
		}
		return true;
	}
	//冠军与我是否同服
	bool isSameAreaCode(Player *pUser);
	//红点提示
	void reqRedPoint(Global::stReqRedPoint *recvCmd);
	//同步记录
	void synRecord(Player * pUser);
private:
	void synUpdateRecord(std::vector<stPair> & vec, Player * pUser, EFINAL_TYPE type);
private:
	std::map<UINT32, stChampionInfo> m_championMap;//上届冠军
	
	CPubUserInfoSortList m_pubUserInfoSortList;//报名角色信息
	
	CRankSortList m_rankSortList;//往届英雄帮
public:
	std::vector<stPair> m_sixteen;//十六强
	std::vector<stPair> m_eight;	//八强
	std::vector<stPair> m_four;	//四强
	std::vector<stPair> m_two; //冠亚军

	std::map<UINT32, UINT32> m_charIDs;
};
#endif
