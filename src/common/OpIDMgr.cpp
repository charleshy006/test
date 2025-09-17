#include "OpIDMgr.h"


COpIDMgr::COpIDMgr()
{
	m_gameOpID = 0;
	//QA测试平台
	m_opIDVec[ePLATFORM_QA].push_back(eOP_ID_YOUZU_ANDRIOD);
	m_opIDVec[ePLATFORM_QA].push_back(eOP_ID_YOUZU_IOS);
	
	//混服平台
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_YOUZU_ANDRIOD);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_YOUZU_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_JINGLI_ANDRIOD);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_HUAWEI_ANDRIOD); 
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_VIVO_ANDRIOD);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_OPPO_ANDRIOD);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_XIAOMI_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_BAIDU_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_360_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_UC_ANDROID);
	
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_ANZHI_ANDROIND);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_LENOVO_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_LESHI_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_KUPAI_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_PPVT_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_WANDOUJIA_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_LINYOU_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_4399_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_MEIZU_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_PENGYOUWAN_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_CHONGCHONG_ANDROID);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_DANGLE_ANDROID);
	
	//越狱渠道也放到这里了
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_YOUZU_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_XY_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_AS_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_91_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_TB_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_PP_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_HAIMA_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_KUAIYONG_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_IAPPLE_IOS);
	m_opIDVec[ePLATFORM_HUNFU].push_back(eOP_ID_ITOOL);

	//ios越狱
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_YOUZU_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_XY_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_AS_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_91_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_TB_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_PP_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_HAIMA_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_KUAIYONG_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_IAPPLE_IOS);
	m_opIDVec[ePLATFORM_IOS].push_back(eOP_ID_ITOOL);

	//appstore
	m_opIDVec[ePLATFORM_APPSTORE].push_back(eOP_ID_APPSTORE);

	//应用宝
	m_opIDVec[ePLATFORM_YYB].push_back(eOP_ID_YINGYONGBAO_ANDRIOD);

	//韩国
	m_opIDVec[ePLATFORM_KOREA].push_back(eOP_ID_KOREA_GOOGLE_PLAY);
	m_opIDVec[ePLATFORM_KOREA].push_back(eOP_ID_KOREA_ONE_STORE);
	m_opIDVec[ePLATFORM_KOREA].push_back(eOP_ID_KOREA_CULLAN_STORE);
	m_opIDVec[ePLATFORM_KOREA].push_back(eOP_ID_KOREA_CULTRUE_LAND);
	m_opIDVec[ePLATFORM_KOREA].push_back(eOP_ID_APPSTORE);

	//台湾
	m_opIDVec[ePLATFORM_TW].push_back(eOP_ID_TW_APPSTORE);
	m_opIDVec[ePLATFORM_TW].push_back(eOP_ID_TW_GOOGLE);

	//暴走
	m_opIDVec[ePLATFORM_BZ].push_back(eOP_ID_BZ_ANDROID);
	m_opIDVec[ePLATFORM_BZ].push_back(eOP_ID_BZ_APPSTORE);
	m_opIDVec[ePLATFORM_BZ].push_back(eOP_ID_BZ_APPSTORE_SEC);
	m_opIDVec[ePLATFORM_BZ].push_back(eOP_ID_BZ_APPSTORE_HD);

    //龙牙
    m_opIDVec[ePLATFORM_LY].push_back(eOP_ID_LY_YOUAI);
}

COpIDMgr::~COpIDMgr()
{

}

std::vector<UINT32> & COpIDMgr::getAllOpID(UINT32 platform)
{
	return m_opIDVec[platform];
}

const char * COpIDMgr::getOpIDName(UINT32 opID)
{
	switch (opID){
		case eOP_ID_YOUZU_ANDRIOD :			//游族安卓
			{
				return "游族安卓";
			}
			break;
		case eOP_ID_YOUZU_IOS :				//游族ios
			{
				return "游族ios";
			}
			break;
		case eOP_ID_XY_IOS : 				//XY
			{
				return "XY助手";
			}
			break;
		case eOP_ID_AS_IOS :
			{
				return "爱思";
			}
			break;
		case eOP_ID_91_IOS :
			{
				return "91助手";
			}
			break;
		case eOP_ID_TB_IOS :
			{
				return "同步推";
			}
			break;
		case eOP_ID_PP_IOS :
			{
				return "pp助手";
			}
			break;
		case eOP_ID_HAIMA_IOS :
			{
				return "海马";
			}
			break;
		case eOP_ID_KUAIYONG_IOS :
			{
				return "快用";
			}
			break;
		case eOP_ID_IAPPLE_IOS :
			{
				return "IIApple";
			}
			break;
		case eOP_ID_APPSTORE :
			{
				return "appstore";
			}
			break;
		case eOP_ID_ITOOL :
			{
				return "itool";
			}
			break;
		case eOP_ID_YINGYONGBAO_ANDRIOD :
			{
				return "应用宝";
			}
			break;
		case eOP_ID_JINGLI_ANDRIOD :
			{
				return "金立安卓";
			}
			break;
		case eOP_ID_HUAWEI_ANDRIOD :
			{
				return "华为安卓";
			}
			break;
		case eOP_ID_VIVO_ANDRIOD :
			{
				return "VIVO安卓";
			}
			break;
		case eOP_ID_OPPO_ANDRIOD :
			{
				return "OPPO安卓";
			}
			break;
		case eOP_ID_XIAOMI_ANDROID :
			{
				return "小米安卓";
			}
			break;
		case eOP_ID_BAIDU_ANDROID :
			{
				return "百度安卓";
			}
			break;
		case eOP_ID_360_ANDROID :
			{
				return "360安卓";
			}
			break;
		case eOP_ID_UC_ANDROID :
			{
				return "UC安卓";
			}
			break;
		case eOP_ID_ANZHI_ANDROIND :			//安智
			{
				return "安智";
			}
			break;
		case eOP_ID_LENOVO_ANDROID  :			//联想
			{
				return "联想";
			}
			break;
		case eOP_ID_LESHI_ANDROID :			//乐视
			{
				return "乐视";
			}
			break;
		case eOP_ID_KUPAI_ANDROID  :			//酷派
			{
				return "酷派";
			}
			break;
		case eOP_ID_PPVT_ANDROID :				//pptv
			{
				return "pptv";
			}
			break;
		case eOP_ID_WANDOUJIA_ANDROID :		//豌豆荚
			{
				return "豌豆荚";
			}
			break;
		case eOP_ID_LINYOU_ANDROID :			//麟游
			{
				return "麟游";
			}
			break;
		case eOP_ID_4399_ANDROID :				//4399
			{
				return "4399";
			}
			break;
		case eOP_ID_MEIZU_ANDROID :				//魅族
			{
				return "魅族";
			}
			break;
		case eOP_ID_PENGYOUWAN_ANDROID :		//朋友玩
			{
				return "朋友玩";
			}
			break;
		case eOP_ID_CHONGCHONG_ANDROID :		//虫虫
			{
				return "虫虫";
			}
			break;
		case eOP_ID_DANGLE_ANDROID :			//当乐
			{
				return "当乐";
			}
			break;
		case eOP_ID_KOREA_GOOGLE_PLAY :			//GooglePlay
			{
				return "GooglePlay";
			}
			break;
		case eOP_ID_KOREA_ONE_STORE :			//OneStore
			{
				return "OneStore";
			}
			break;
		case eOP_ID_KOREA_CULLAN_STORE :	    //CullanStore
			{
				return "CullanStore";
			}
			break;
		case eOP_ID_KOREA_CULTRUE_LAND :        //CultrueLand
			{
				return "CultrueLand";
			}
			break;
		case eOP_ID_TW_APPSTORE :        //appstore
			{
				return "appStore";
			}
			break;
		case eOP_ID_TW_GOOGLE :        //google
			{
				return "google";
			}
			break;
		case eOP_ID_BZ_ANDROID :        //Android
			{
				return "android";
			}
			break;
		case eOP_ID_BZ_APPSTORE :        //appstore
			{
				return "appStore";
			}
			break;
		case eOP_ID_BZ_APPSTORE_SEC :        //appstore
			{
				return "appStore 2";
			}
			break;
		case eOP_ID_BZ_APPSTORE_HD :        //appstore
			{
				return "appStore HD";
			}
			break;
		case eOP_ID_LY_YOUAI :          //游爱网络
			{
				return "游爱网络";
			}
			break;

	}
	return "所有渠道";
}
