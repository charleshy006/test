#ifndef __LIMIT_PET_MGR_H__
#define __LIMIT_PET_MGR_H__

#include "Singleton.h"
#include "TypesDef.h"
#include <map>
#include <set>
#include <vector>

#pragma pack(1)
struct stLimitPetDB
{
	UINT32		m_index;
	UINT32		m_groupId;
	UINT32		m_petId;
    UINT32      m_start;
    UINT32      m_end;

    stLimitPetDB () {
        m_index = 0;
        m_groupId = 0;
        m_petId = 0;
        m_start = 0;
        m_end = 0;
    }
};
#pragma pack()

struct stLimitPet
{
	UINT32		m_index;
	UINT32		m_groupId;
	UINT32		m_petId;
    UINT32      m_start;
    UINT32      m_end;

    stLimitPet () {
        m_index = 0;
        m_groupId = 0;
        m_petId = 0;
        m_start = 0;
        m_end = 0;
    }

    void initFromDB(stLimitPetDB & data) {
        m_index = data.m_index;
        m_groupId = data.m_groupId;
        m_petId = data.m_petId;
        m_start = data.m_start;
        m_end = data.m_end;
    }
};

class CLimitPetMgr
{
	USE_SINGLETON_NEW(CLimitPetMgr);
public:
	CLimitPetMgr() {}
	~CLimitPetMgr() {}
	//返回管理实例
	static CLimitPetMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
 	//加载
	void load();
    //同步数据到Global
    void synInfo();
private:
    std::map<UINT32, stLimitPet> m_petMap;
};
#endif
