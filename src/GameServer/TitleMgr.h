#ifndef __H_TITLEMGR_H__
#define __H_TITLEMGR_H__
#include "TypesDef.h"
#include <set>
#include <map>
#include "PubGame.pb.h"

#define Title_Index_Coef  100000

class GameUser;
class CTitleMgr
{
public :
	//构造
	CTitleMgr(GameUser * pOwner);
	//析构
	~CTitleMgr();
	//保存称号
	UINT32 saveAllTitle(byte * buf,UINT32 needSpace);
	//加载称号
	UINT32 loadAllTitle(byte * buf);
	//增加角色属性值
	void addProp();
	//增加某个称号
	void addTitle(UINT32 id);
	//玩家上线
	void userOnline();
	//激活头衔
	void activeTitle(UINT32 id);
	//取消激活
	void cancelActiveTitle();
	//检查是否获得称号
	void checkObtainTitle(UINT16 type,UINT32 id,bool saved = true,bool bNotify = true);
	//删除称号
	void delTitle(UINT32 titleID);
	//根据条件获得称号ID
	UINT32 getTitleIDByGotCond(UINT16 type,UINT32 id);
	//获得激活的称号
	UINT32 getActiveTitleID()
	{
		return m_activeTitleID;
	}
	void pack(PubGame::stTitleToPub* st);
	void parse(const PubGame::stTitleToPub& st);
	//加载配置完后处理
	static bool afterLoadTitleTbl();
	//增加一个获取条件索引
	static void addGotTitleIndex(UINT16 type,UINT32 condID,UINT32 titleID);
private :
	GameUser * m_pOwner;
	std::set<UINT32> m_hasGotTitles;			//已经获得的称号
	std::set<UINT32> m_hasGotTitlesNotSaved;	//已经获得的非保存称号

	UINT32   m_activeTitleID;					//激活的称号ID

	static std::map<UINT32,UINT32>  m_conditions;	//获取条件
};
#endif
