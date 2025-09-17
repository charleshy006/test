#ifndef __H_OPID_MGR_H__
#define __H_OPID_MGR_H__
#include "TypesDef.h"
#include "Singleton.h"
#include <vector>
#include <string>

class COpIDMgr
{
	USE_SINGLETON_NEW(COpIDMgr);
public :
	enum ePLATFORM
	{
		ePLATFORM_QA = 2103	,		//QA测试平台
		ePLATFORM_HUNFU = 2186,		//混服
		ePLATFORM_IOS   = 2217,		//IOS越狱
		ePLATFORM_APPSTORE = 2229,	//IOS APPSTORE
		ePLATFORM_YYB  = 2235,		//应用宝
		ePLATFORM_KOREA  = 2280,    //韩国 用于后台统计(acu, pcu, dt)
		ePLATFORM_TW = 2290,        //台湾 用于后台统计(acu, pcu, dt)
		ePLATFORM_BZ = 2300,        //暴走 用于后台统计(acu, pcu, dt)
		ePLATFORM_LY = 2310,        //龙牙 用于后台统计(acu, pcu, dt)
	};

	enum eOP_ID
	{
		eOP_ID_YOUZU_ANDRIOD = 2106,			//游族安卓
		eOP_ID_YINGYONGBAO_ANDRIOD = 2151,		//应用宝安卓
		eOP_ID_JINGLI_ANDRIOD = 2139,			//金立安卓
		eOP_ID_HUAWEI_ANDRIOD = 2123,			//华为安卓
		eOP_ID_VIVO_ANDRIOD = 2130,				//VIVO安卓
		eOP_ID_OPPO_ANDRIOD = 2115,				//OPPO安卓
		eOP_ID_XIAOMI_ANDROID = 2108,			//小米安卓
		eOP_ID_BAIDU_ANDROID = 2102,			//百度安卓
		eOP_ID_360_ANDROID = 2105,				//360安卓
		eOP_ID_UC_ANDROID = 2110,				//UC安卓
		eOP_ID_YOUZU_IOS = 2120,				//游族ios
		eOP_ID_XY_IOS = 2131,					//xy
		eOP_ID_AS_IOS = 2143,					//爱思
		eOP_ID_91_IOS = 2101,					//91
		eOP_ID_TB_IOS = 2111,					//同步推
		eOP_ID_PP_IOS = 2103,					//PP助手
		eOP_ID_HAIMA_IOS = 2138,				//海马
		eOP_ID_KUAIYONG_IOS = 2113,				//快用
		eOP_ID_IAPPLE_IOS = 2153,				//I苹果
		eOP_ID_APPSTORE = 2107,					//appstore
		eOP_ID_ITOOL = 2116,					//itool

		eOP_ID_ANZHI_ANDROIND = 2112,			//安智
		eOP_ID_LENOVO_ANDROID = 2141,			//联想
		eOP_ID_LESHI_ANDROID = 2210,			//乐视
		eOP_ID_KUPAI_ANDROID = 2140,			//酷派
		eOP_ID_PPVT_ANDROID = 2177,				//pptv
		eOP_ID_WANDOUJIA_ANDROID = 2122,		//豌豆荚
		eOP_ID_LINYOU_ANDROID = 2164,			//麟游
		eOP_ID_4399_ANDROID = 2183,				//4399
		eOP_ID_MEIZU_ANDROID = 2152,			//魅族
		eOP_ID_PENGYOUWAN_ANDROID = 2241,		//朋友玩
		eOP_ID_CHONGCHONG_ANDROID = 2266,		//虫虫
		eOP_ID_DANGLE_ANDROID = 2117,			//当乐

		eOP_ID_KOREA_GOOGLE_PLAY = 3100,        //GooglePlay
		eOP_ID_KOREA_ONE_STORE = 3200,          //OneStore
		eOP_ID_KOREA_CULLAN_STORE = 3300,       //CullanStore
		eOP_ID_KOREA_CULTRUE_LAND = 3301,       //CultrueLand

		eOP_ID_BZ_ANDROID = 4200,               //bz 安卓
		eOP_ID_BZ_APPSTORE = 5200,              //bz ios
        eOP_ID_BZ_APPSTORE_SEC = 5210,          //bz ios 2
        eOP_ID_BZ_APPSTORE_HD  = 5211,          //bz 萌将

		eOP_ID_TW_APPSTORE = 5100,              //tw ios
		eOP_ID_TW_GOOGLE = 7000,                //tw google

		eOP_ID_LY_YOUAI = 6000,                //游爱网络
	};
private :
	//构造函数
	COpIDMgr();
public :
	//析构函数
	~COpIDMgr();
	//获得实例ID
	static COpIDMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
	//获得所有的渠道ID
	std::vector<UINT32> & getAllOpID(UINT32 platform);
	//获得渠道名字
	const char * getOpIDName(UINT32 opID);
	//改渠道是否是appstore
	bool isAppstore(UINT32 opID){
		return eOP_ID_APPSTORE == opID || (opID > 5000 && opID < 6000);
	}
	//获得游戏平台ID
	UINT32 getGameOpID()
	{
		return m_gameOpID;
	}
	//设置平台ID
	void setGameOpID(UINT32 gameOpID)
	{
		m_gameOpID = gameOpID;
	}
	//是否是应用宝
	bool isYYB()
	{
		return ePLATFORM_YYB == m_gameOpID;
	}
private :
	std::map<UINT32,std::vector<UINT32> > m_opIDVec;
	UINT32 m_gameOpID;
};
#endif
