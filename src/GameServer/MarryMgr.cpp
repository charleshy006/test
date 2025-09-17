#include "MarryMgr.h"

#include "KVPair.h"
#include "GameUserManager.h"

void MarryMgr::addUserToRoom(UINT32 userId, UINT32 roomId) {
    std::map<UINT32, std::set<UINT32> >::iterator it = m_inRoomUserMap.find(roomId);
    if (it == m_inRoomUserMap.end()) {
        std::set<UINT32> s;
        s.insert(userId);
        m_inRoomUserMap.insert(std::make_pair(roomId, s));
    } else {
        it->second.insert(userId);
    }
    bool res = m_userToRoomMap.insert(std::make_pair(userId, roomId)).second;
    Athena::logger->trace("addUserToRoom : insert userId %d, roomId %d, res is %d", userId, roomId, res);
}

void MarryMgr::delUserToRoom(UINT32 userId) {
    std::map<UINT32, UINT32>::iterator it = m_userToRoomMap.find(userId);
    if (it == m_userToRoomMap.end()) {
        Athena::logger->trace("delUserToRoom : not find userId %d", userId);
        return ;
    }
    std::map<UINT32, std::set<UINT32> >::iterator tmpIt = m_inRoomUserMap.find(it->second);
    if (tmpIt == m_inRoomUserMap.end()) {
        Athena::logger->trace("delUserToRoom : not find roomId %d", it->second);
        return ;
    }
    tmpIt->second.erase(userId);
    if (tmpIt->second.empty()) {
        m_inRoomUserMap.erase(it->second);
        Athena::logger->trace("delUserToRoom : del empty room");
    }
    m_userToRoomMap.erase(userId);
}

void MarryMgr::synMove(GameUser * pUser) {
    if (!pUser) {
        return ;
    }
    std::map<UINT32, UINT32>::iterator it = m_userToRoomMap.find(pUser->getID());
    if (it == m_userToRoomMap.end()) {
        Athena::logger->trace("m_userToRoomMap not find user id %d in room id %d",
                pUser->getID(), it->second);
        return ;
    }
    std::map<UINT32, std::set<UINT32> >::iterator tmpIt = m_inRoomUserMap.find(it->second);
    if (tmpIt == m_inRoomUserMap.end()) {
        Athena::logger->trace("m_inRoomUserMap room id %d, in room user size %d", it->second, tmpIt->second.size());
        return ;
    }
    std::vector<UINT32> userList;
    for (std::set<UINT32>::iterator it = tmpIt->second.begin();
            it != tmpIt->second.end(); it++ ) {
        userList.push_back(*it);
    }
    Athena::logger->trace("synMove user id %d, room id %d", pUser->getID(), it->second);
	Map::stNotifyMODMove cmd;
	Map::stNotifyMODMove_stModMoveInfo * pMoveInfo = cmd.add_data();
	if (!pMoveInfo){
		return ;
	}
	pMoveInfo->set_type(pUser->getType());
	pMoveInfo->set_x(pUser->getPos().m_x);
	pMoveInfo->set_y(pUser->getPos().m_y);
	pMoveInfo->set_z(pUser->getPos().m_z);
	pMoveInfo->set_dir(pUser->getPos().m_dir);
	pMoveInfo->set_modid(pUser->getUniqueID());
	GameUserManager::getSingleton().sendProto2UserList(userList,cmd);		
}

bool MarryMgr::userInRoom(UINT32 userId) {
    return m_userToRoomMap.find(userId) != m_userToRoomMap.end();
}
