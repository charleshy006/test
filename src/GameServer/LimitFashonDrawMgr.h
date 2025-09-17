#ifndef __H_LUCK_FASHIONDRAW_MGR_H__
#define __H_LUCK_FASHIONDRAW_MGR_H__
#include "TypesDef.h"
#include <set>

/**
 * 限时时装抽奖
 **/
class GameUser;
class CLimitFashonDrawMgr
{
public :
	static const UINT32 s_SAVE_VERSION_INFO = 20160321;
public :
	//构造函数
	CLimitFashonDrawMgr(GameUser * pOwner);
	//析构函数
	~CLimitFashonDrawMgr();
	//保存商店数据
	UINT32 save(byte * buf);
	//获得保存数据
	UINT32 getSaveSize();
	//加载商店数据
	UINT32 load(byte * buf);
	//增加次数
	void addDrawNum(UINT16 num)
	{
		m_drawNum += num;
	}
	//打开抽奖界面
	void openDrawWin();
	//进行抽奖
	void draw();
private :
	GameUser * m_pOwner;
	UINT16 m_drawNum;
	std::set<UINT16> m_drawSets;				//已经抽取的集合
};
#endif
