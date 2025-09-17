#ifndef __H_CHAT_H__
#define __H_CHAT_H__
#include "TypesDef.h"
#include "Singleton.h"
#include "string"
#include "UtilFunction.h"
#include "KVPair.h"
#include <map>

class GameUser;
class ChatMgr
{
	USE_SINGLETON_NEW(ChatMgr);
	public :
		typedef std::map<std::string,UINT8> GMAccountCfg;
		typedef GMAccountCfg::iterator      GMAccountCfg_IT;
	private :
		GMAccountCfg m_gmAccountCfg;
	private :
		ChatMgr();
	public  :
		~ChatMgr()
		{
			
		}
	public :
		enum
		{
			GM_TYPE_NONE   = 0, 	//非GM
			GM_TYPE_NORMAL = 1,		//普通GM
			GM_TYPE_CAPTION = 2,	//组长GM
			GM_TYPE_SUPER   = 3,	//超级GM
		};
	private :		
		typedef bool (* GMFunPtr)(GameUser * pUser,KVPair & prop) ;

		//gm指令的信息
		struct stGMCmdInfo
		{
			byte m_gmType;			//使用的GM
			std::string m_name;		//指令的名字
			GMFunPtr m_fun;			//指令对应的函数
			std::string m_des;		//描述

			stGMCmdInfo(byte gmType,std::string name,GMFunPtr fun,std::string des)
			{
				m_gmType = gmType;
				m_name = name;
				m_fun = fun;
				m_des = des;
			}
		};
		
		static std::map<std::string,stGMCmdInfo>  m_gmCmdCtx; 

	 public :
		static ChatMgr & getSingleton()
		{
			return THE_SINGLETON::Instance();
		}
		//初始化GM帐号	
		bool initGMAccountInfo();
		//获得GM类型
		UINT8 getGMAccountType(const char * );
		//解析GM指令
		static void parseGMCmd(KVPair & prop,const char* cmdLine, const char* flag);
		//执行GM指令
		static bool exec(GameUser * user,std::string & cmd);
		//增加道具
		static bool addItem(GameUser *pUser,KVPair & prop);
		//加元宝
		static bool addDiamond(GameUser *pUser,KVPair & prop);
		//加金币
		static bool addGoldCoin(GameUser * pUser,KVPair & prop);
		//增加经验
		static bool addExp(GameUser * pUser,KVPair & prop);
		//增加等级
		static bool levelup(GameUser * pUser,KVPair & prop);
		//升级武将
		static bool levelupHero(GameUser * pUser,KVPair & prop);
		//打印所有在线玩家
		static bool printAllUser(GameUser * pUser,KVPair & prop);
		//添加一个宠物
		static bool addPet(GameUser * pUser,KVPair & prop);
		//添加资源
		static bool addRes(GameUser * pUser,KVPair & prop);
		//增加一个称号
		static bool addTitle(GameUser * pUser,KVPair & prop);
		//测试用指令
		static bool test(GameUser * pUser,KVPair & prop);
		//发送邮件
		static bool addMail(GameUser * pUser,KVPair & prop);
		//模拟充值
		static bool recharge(GameUser *pUser,KVPair & prop);
		//换服测试
		static bool changeGameSvr(GameUser * pUser,KVPair & prop);
		//测试redis服务器读取
		static bool testRedis(GameUser * pUser,KVPair & prop);
		//查实换地图
		static bool changeMap(GameUser * pUser,KVPair & prop);
		//贡献度
		static bool addContribute(GameUser * pUser,KVPair & prop);
		//发送全服公告
		static bool sendNotify(const char * szContent);
		//发送获得道具公告
		static bool sendObtainItemNotify(GameUser * pUser,const char *szKey,UINT32 itemID,UINT32 num,UINT32 boxID = 0);
		//完成所有关卡
		static bool finishAllLv(GameUser * pUser,KVPair & prop);
		//玩家一个任务
		static bool finishTask(GameUser * pUser,KVPair & prop);
		//增加公会活跃度
		static bool addActDegree(GameUser * pUser,KVPair & prop);
};
#endif
