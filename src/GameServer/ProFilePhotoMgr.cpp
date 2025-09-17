#include "ProFilePhotoMgr.h"
#include "AthenaNew.h"
#include "Log4cxxHelp.h"
#include "KVPair.h"
#include "UtilFunction.h"
#include "GameServer.h"
#include "GameUser.h"
#include "TblDataMgr.h"

CProFilePhotoMgr::CProFilePhotoMgr(GameUser *pUser):pOwner(pUser)
{
	m_usePhotoID = 0;
}

CProFilePhotoMgr::~CProFilePhotoMgr()
{}


UINT32 CProFilePhotoMgr::loadData(const char * buf)
{
	UINT8 ret = 0;
	UINT32 version = *(UINT32*)(buf + ret);
	ret += sizeof(UINT32);
	if(s_PHOTO_SAVE_VERSION == version){
		UINT16 num = *(UINT16*)(buf + ret);
		ret += sizeof(UINT16);
		for(UINT16 i = 0; i < num; ++i){
			UINT16 id = *(UINT16*)(buf + ret);
			ret += sizeof(UINT16);
			m_photoIDSet.insert(id);
		}
	}

#ifdef _HDZ_DEBUG 
	Athena::logger->trace("[头像加载] m_usePhotoID：%d, m_photoIDSet.size:%d", m_usePhotoID, m_photoIDSet.size());
#endif
	return ret;
}

UINT32 CProFilePhotoMgr::saveData(char * buf)
{
	UINT8 ret = 0;
	*(UINT32*)(buf+ret) = s_PHOTO_SAVE_VERSION;
	ret += sizeof(UINT32);
	*(UINT16*)(buf+ret) = m_photoIDSet.size();
	ret += sizeof(UINT16);
	std::set<UINT16>::iterator it = m_photoIDSet.begin();
	for(;it != m_photoIDSet.end(); ++it){
		*(UINT16*)(buf+ret) = *it;
		ret += sizeof(UINT16);
	}

#ifdef _HDZ_DEBUG 
	Athena::logger->trace("[头像保存] m_usePhotoID：%d, m_photoIDSet.size:%d", m_usePhotoID, m_photoIDSet.size());
#endif
	return ret;
}

void CProFilePhotoMgr::sendPhotoList()
{
	Role::stHasPhotoIDResult retCmd;

	if(m_photoIDSet.empty()){//为空，设置为初始头像
		m_photoIDSet.insert(1);
	}

	std::set<UINT16>::iterator it = m_photoIDSet.begin();
	for(;it != m_photoIDSet.end(); ++it){
		retCmd.add_photoids(*it);
#ifdef _HDZ_DEBUG 
		Athena::logger->trace("[发送头像] sendPhotoID:%d", *it);
#endif
	}

	pOwner->sendProto2Me(retCmd);
}
