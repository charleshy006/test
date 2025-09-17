#ifndef __PROFILEPHOTO_MGR_H__
#define __PROFILEPHOTO_MGR_H__

#include "TypesDef.h"
//#include <map>
//#include <vector>
#include <set>

class GameUser;
class CProFilePhotoMgr
{
	static const UINT32 s_PHOTO_SAVE_VERSION = 20170508;
public:
	//构造函数
	CProFilePhotoMgr(GameUser *pUser);
	//析构函数
	~CProFilePhotoMgr();
	//加载数据
	UINT32 loadData(const char * buf);
	//保存数据
	UINT32 saveData(char * buf);
	//发送已有头像列表
	void sendPhotoList();
	//id是否存在
	bool isExists(UINT16 id){
		std::set<UINT16>::iterator it = m_photoIDSet.find(id);
		if(it == m_photoIDSet.end()){
			return false;
		}
		return true;
	}
	//增加ID
	void addIDToSet(UINT16 id){m_photoIDSet.insert(id);}
	//获取当前使用的ID
	UINT16 getUsePhotoID(){return m_usePhotoID ? m_usePhotoID : 1;}
	//设置当前使用的ID
	void setUsePhotoID(UINT16 id){m_usePhotoID = id;}
private:
	GameUser *pOwner;
	std::set<UINT16> m_photoIDSet;		//已解锁的ID
	UINT16 m_usePhotoID;				//当前使用的ID
};

#endif
