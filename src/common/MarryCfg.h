#ifndef _MARRY_CFG_H_
#define _MARRY_CFG_H_


#include "Singleton.h"
#include <tinyxml/tinyxml.h>
#include <map>
#include <list>

class CMarryCfg
{
	USE_SINGLETON_NEW(CMarryCfg);
public:
    enum Divorce_Type {
        Divorce_Common = 1,
        Divorce_Force = 2,
    };

    struct MarryInfo {
        UINT32 id;
        UINT32 levelId;
        UINT32 boxId;
        UINT32 cost;
        UINT32 costType;
        UINT32 candyId;
        UINT32 maxDrawTimes;
    };
    struct FireWorkInfo {
        UINT32 id;
        UINT32 itemId;
        UINT32 cost;
        UINT32 costType;
    };
    struct DivorceInfo {
        UINT32 id;
        UINT32 cost;
        UINT32 costType;
        UINT32 time;
    };
public:
    //获得实例
    static CMarryCfg & getSingleton() {
        return THE_SINGLETON::Instance();
    }
	//加载所有配置
	bool loadCfg();
    //获取结婚信息
    bool getMarryInfo(UINT32 id, MarryInfo & info);
    //获取烟火信息
    bool getFwInfo(UINT32 id, FireWorkInfo & info);
    //获取烟火id
    UINT32 getIndexOfFw(UINT32 itemId);
    //获取离婚信息
    bool getDivorceInfo(bool isForce, DivorceInfo & divorceInfo);


private:
    std::map<UINT32, MarryInfo> m_marryInfoMap;
    std::map<UINT32, FireWorkInfo> m_fireWorkInfoMap;
    std::map<UINT32, DivorceInfo> m_divorceInfoMap;
};
#endif
