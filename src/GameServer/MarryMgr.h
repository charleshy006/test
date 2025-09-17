#ifndef __H_MARRY_MGR_H__
#define __H_MARRY_MGR_H__

#include "Singleton.h"

#include <map>
#include <set>

class GameUser;

class MarryMgr
{
	USE_SINGLETON_NEW(MarryMgr)
public:
	static MarryMgr & getSingleton()
	{
		return THE_SINGLETON::Instance();
	}
    void synMove(GameUser * pUser);
    void addUserToRoom(UINT32 userId, UINT32 roomId);
    void delUserToRoom(UINT32 userId);
    bool userInRoom(UINT32 userId);
private:
    std::map<UINT32, UINT32> m_userToRoomMap;
    std::map<UINT32, std::set<UINT32> > m_inRoomUserMap;
};
#endif
