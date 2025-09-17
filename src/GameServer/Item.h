#ifndef __H_ITEM_H__
#define __H_ITEM_H__
#include "TblItemBase.h"
#include "TypesDef.h"
#include "ItemInfo.h"
#include "Element.h"
#include "ElementMgr.h"
#include "Role.pb.h"
#include  <vector>
class GameUser;
#pragma pack(1)
struct stItemSaveInfo
{
	UINT32 m_version;
	UINT8  m_mainBagSize;
	UINT8  m_fashionBagSize;
	UINT16 m_num;
	ItemInfo  m_data[0];

	stItemSaveInfo()
	{
		m_mainBagSize = 0;
		m_fashionBagSize = 0;
		m_version = 20150301;
		m_num = 0;
	}
};
#pragma pack()

enum
{
	YUNTIE_ITEM_ID = 12000	,		//陨铁
	WUHUNSHI_ITEM_ID = 821601,		//武魂石
	LINGWUDAN_ITEM_ID = 101204,		//领悟丹
	JUEXING_ITEM_ID = 122207,		//觉醒经验丹
};


class CItem : public Element 
{
public :
	static  UINT16 s_ITEM_GEN_ID ;
public :
	//构造函数
	CItem();
	//构造函数
	CItem(stItemBase * pItemBase);
	//析构函数
	~CItem();
	//初始化道具数据
	bool init();
	//生成instanceID
	static UINT64 genInstanceID();
	//填充同步到客户端的数据
	void fillSynData(Role::stSynItemInfo & data);
	//设置属性状态 
	void setupPropState();	
	//是否和某个道具叠加
	bool canOverlayWithItem(const CItem * pItem);
	//计算战力
	void calcBattlePower();
	//道具是否碎片
	static bool isEquipPiece(UINT32 itemID);
	//道具是否是碎片
	static bool isEquipPieceByType(UINT16 type);
	//是否是武魂石
	static bool isWuHunShi(UINT32 itemID);
	//是否是武魂石
	static bool isWuHunShiByType(UINT16 type);
	//是否是符文
	static bool isFuwen(UINT32 itemID);
	//是否是符文
	static bool isFuwenByType(UINT16 type);
	//是否符文碎片
	static bool isFuwenPiece(UINT32 itemID);
	//是否是符文碎片
	static bool isFuwenPieceByType(UINT16 type);
	//是否是装备
	static bool isEquipByID(UINT32 itemID);
	//是否是装备
	static bool isEquipByType(UINT16 type);
	//是否是碎片
	static bool isPiece(UINT32 itemID);
	//经验符石
	static bool isFuWenExpStone(UINT16 type);
	//符文经验丹
	static bool isRuneExpStone(UINT16 type);
	//是否是时装碎片
	static bool isFashionPiece(UINT32 itemID);
	//是否是否时装碎片
	static bool isFashionPieceByType(UINT16 type);
	//是否是时装
	static bool isFashion(UINT32 itemID);
	//是否是时装
	static bool isFashionByType(UINT16 type);
	//是否是符文
	static bool isRune(UINT32 itemID);
	//是否是符文
	static bool isRuneByType(UINT16 type);
	//是否是符文碎片
	static bool isRunePiece(UINT32 itemID);
	//是否是符文碎片
	static bool isRunePieceByType(UINT16 type);
	//根据职业和性别获得对应的装备ID
	static UINT32 realItemIDByJobAndSex(UINT32 itemID,UINT8 job,UINT8 sex);
	//是否可以批量使用
	static bool   canBatchUse(UINT16 type);
	//是否是绿装
	static bool   isGreen(UINT8 quality);
	//是否是蓝装
	static bool   isBlue(UINT8 quality);
	//是否是紫装
	static bool   isPurplue(UINT8 quality);
	//是否是橙装
	static bool   isOrange(UINT8 quality);
	//是否是n
	static bool   isN(UINT8 quality);
	//是否是r
	static bool   isR(UINT8 quality);
	//是否是sr
	static bool   isSR(UINT8 quality);
	//是否是ssr
	static bool   isSSR(UINT8 quality);
	//是否是ur
	static bool   isUR(UINT8 quality);
	//是否要打日志
	static bool   needSendWebLog(stItemBase * pItemBase);
	//是否是坐骑晶石
	static bool isMountStone(UINT32 itemID);
	//是否是坐骑晶石
	static bool isMountStoneByType(UINT16 type);
	//获得道具的战力
	UINT32 getBattlePower() {  return m_battlePower ;}
	//计算百分比加成
	void calcPercentAdd();
	//是否有觉醒技能
	bool hasEquipSkill();
	//更换技能
	void exechangegSkill(CItem * pItem);
public :
	ItemInfo m_itemInfo;
	std::vector<UINT32> m_battleProps;
	UINT32 m_battlePower;				//战力
	stItemBase * m_pItemBase;
	std::vector<stBattlePropValue> m_fuwenActiveBPVec;		//符文激活的属性
	std::vector<stBattlePropValue> m_raiseSkillBPVec;		//强化技能的属性
};

struct stItemCallBack
{
	//构造函数
	stItemCallBack(){}
	//析构函数
	virtual ~stItemCallBack(){}
	//对道具进行回调
	virtual  bool doIt(CItem * pItem) = 0;
};

//强化到某个级别装备个数
struct stRaiseEquipNumCB : public stItemCallBack
{
	UINT16 m_num ;
	UINT16 m_lv ;

	stRaiseEquipNumCB(UINT16 lv) : m_lv(lv)
	{
		m_num = 0;	
	}

	virtual  bool doIt(CItem * pItem)
	{
		if (pItem && (CItem::isEquipByType(pItem->m_pItemBase->m_type))){
			if (pItem->m_itemInfo.m_raiseLv >= m_lv){
				m_num += 1;
			}
		}	
		return true;
	}
};

//符文强化到某个级别的个数
struct stRaiseFuwenNumCB : public stItemCallBack
{
	UINT16 m_num;
	UINT16 m_lv ;

	stRaiseFuwenNumCB(UINT16 lv) :m_num(0),m_lv(lv)
	{
		
	}

	virtual  bool doIt(CItem * pItem)
	{
		if (pItem && (CItem::isFuwenByType(pItem->m_pItemBase->m_type))){
			if (pItem->m_itemInfo.m_raiseLv >= m_lv){
				m_num += 1;
			}
		}	
		return true;
	}
};

//精炼到某个级别装备个数
struct stRefineEquipNumCB : public stItemCallBack
{
	UINT16 m_num;
	UINT16 m_lv;
	
	stRefineEquipNumCB(UINT16 lv) : m_lv(lv)
	{
		m_num = 0;
	}

	virtual  bool doIt(CItem * pItem)
	{
		if (pItem){
			if (pItem->m_itemInfo.m_refineLv >= m_lv){
				m_num += 1;
			}
		}
		return true;
	}
};

//获得所有绿装，蓝装
struct stGetAllBlueAndGreen : public stItemCallBack
{
	std::vector<CItem * > m_equipVecs;

	stGetAllBlueAndGreen()
	{}

	virtual  bool doIt(CItem * pItem)
	{
		if (!pItem || !pItem->m_pItemBase){
			return true;
		}	
		
		if (!CItem::isEquipByType(pItem->m_pItemBase->m_type)){
			return true;
		}

		if (CItem::isGreen(pItem->m_pItemBase->m_quality) || CItem::isBlue(pItem->m_pItemBase->m_quality)){
			m_equipVecs.push_back(pItem);
		}
		return true;
	}
};

struct stTotalFashionNumCB : public stItemCallBack
{
	UINT16 m_num;

	stTotalFashionNumCB()
	{
		m_num = 0;
	}
	
	virtual  bool doIt(CItem * pItem)
	{
		if (!pItem || !pItem->m_pItemBase){
			return true;
		}
		
		if (CItem::isFashionByType(pItem->m_pItemBase->m_type) && (pItem->m_pItemBase->m_expireTime <= 0)){
			m_num += 1;	
		}
		return true;
	}
};

class CBag : public ElementMgr 
{
	friend class CBagMgr;
public :
	//构造函数
	CBag(UINT16 slotNum,UINT8 bagType);
	//增加道具到包裹
	bool addItem(CItem * pItem);
	//从包裹里删除道具
	bool removeItem(CItem * pItem);
	//获得包裹容量
	UINT16 getCapacity() { return m_capacity ;}
	//获得包裹道具数量
	UINT16 getItemSize();
	//析构函数
	virtual ~CBag();
	//根据道具实例ID获得道具
	CItem * getItemByInstaceID(UINT64 instanceID) {  return (CItem *)getElementByTempID(instanceID); }
	//获得空闲的各自索引
	UINT16 getFreeSlotIndex();
	//根据位置获得道具
	CItem * getItemBySlotIndex(UINT16 index);
	//检查格子索引是否合法 
	bool  checkSlotIndex(UINT16 index);
	//返回某个id的所有道具对象,返回值是数量
	UINT32 getItemByID(UINT32 itemID,UINT64 exceptInstanceID,std::vector<CItem * > & vec);
	//根据条件获得道具数量 
	UINT32 getItemByID(stItemCallBack & cb);
	//设置包裹大小
	void setBagSize(UINT16 bagSize) { m_bagSize = bagSize ;}
	//返回包裹大小
	UINT16 getBagSize() { return m_bagSize ;}
	//对所有道具回调
	void exeEveryOne(stItemCallBack & cb);
	//获得所有空格子数量
	UINT16 getFreeSlotNum();
	//获得一个可以叠加的对象
	CItem * getOverlayItem(CItem * pItem);
	//UINT16 CBag::fillAllItem(Proto::stSynItemInfo  * data);
	//重新刷新包裹里的道具
	void refreshBagItem(GameUser * pUser );
    //获取时装数量
    UINT32 getFashionNum();
protected :
	//添加道具到包裹的时候检查
	virtual bool checkAddItem(CItem * pItem);
protected :
	CItem ** 		m_pSlot;			//插槽
	UINT8    		m_bagType;			//道具类型
	UINT16			m_bagSize;			//包裹大小，<= m_capacity
	const UINT16   	m_capacity;			//包裹容量
};

class CMainBag : public CBag
{
	friend class CBagMgr;
public :
	const static UINT16 s_MAIN_BAG_SLOT_NUM = 120;
	const static UINT16 s_PIECE_BAG_SLOT_NUM = 120;
	const static UINT16 s_HERO_PIECE_BAG_SLOT_NUM = 30;
	const static UINT16 s_FASHION_BAG_SLOT_NUM = 200;
    const static UINT16 s_RUNE_PIECE_BAG_SLOT_NUM = 40;
	const static UINT16 s_MOUNT_BAG_SLOT_NUM = 200;
public :
	//构造函数
	CMainBag(UINT8 bagType,UINT16 slotNum);
	//析构函数
	~CMainBag();
	//重新整理
	void reArrange();
};

//装备数值
struct stEquipData
{
	std::vector<UINT32> m_battleProps;
	UINT32 m_battlePower;					//战力
	UINT16 m_equipAveRaiseLv;				//装备平均强化等级
	UINT8  m_equipAveRefineLv;				//装备平均精炼等级
	UINT8  m_wingAveRefineLv;				//翅膀精炼等级
	UINT16 m_fuwenAveRaiseLv;				//符文平均强化等级
	UINT8  m_fuwenAveQuality;				//符文平均品质
	UINT8  m_equipAveBreakLv;				//装备平均突破等级
	UINT8  m_wingAveBreakLv;				//翅膀突破等级
	std::vector<stBattlePropValue> m_raiseSkillBPVec;		//强化技能的属性

	stEquipData()
	{
		reset();
	}

	void reset()
	{
		m_battleProps.clear();
		m_battleProps.resize(eMAX_BATTLE_PROP);
		m_battlePower = 0;
		m_equipAveRaiseLv = 0;
		m_equipAveRefineLv = 0;
		m_wingAveRefineLv = 0;
		m_fuwenAveRaiseLv = 0;
		m_fuwenAveQuality = 0;
		m_equipAveBreakLv = 0;
		m_wingAveBreakLv = 0;
		m_raiseSkillBPVec.clear();
	}
};

class CEquipBag :public  CBag
{
	friend class CBagMgr;
public :
	const static UINT16  s_EQUIP_BAG_SLOT_NUM= 65;
	enum
	{
		EQUIP_POS_FASHION_HEAD = 1,				//时装头
		EQUIP_POS_FUWEN_CORE  = 21,
		EQUIP_POS_FUWEN_FIRE = 22,
		EQUIP_POS_FUWEN_WATER = 23,
		EQUIP_POS_FUWEN_WIND = 24,
		EQUIP_POS_FUWEN_SOIL = 25,
	};
protected :
	virtual bool checkAddItem(CItem * pItem);
public :
	//构造函数
	CEquipBag(UINT8 bagType);
	//析构函数
	~CEquipBag();
	//是否是装备
	//static bool isEquip(CItem * pItem);
	//是否影响换装
	static bool isAffectAppearance(BagPos & pos);
	//是否影响角色数值
	static bool isAffectRoleData(BagPos & pos);
	//计算所有装备数值
	void calcEquipData();
	//根据道具类型获得装备所在空索引
	static UINT8 getSlotIndexByItemType(UINT16 type);
	//根据符文slot索引获得对应装备索引
	static UINT8 getEquipIndexByFuwenIndex(UINT8 index);	
	//根据符文获得应该装备的slot索引
	static UINT8 getFuwenSlotIndex(UINT16 type,UINT16 indexOffset);
	//获得某个部位的状态
	CItem * getEquipByIndex(UINT8 index);
	//获得装备数值
	const stEquipData & getEquipData()
	{
		return m_data;
	}
	//获得穿满符文的装备数量
	UINT16 getWearFullFuwenEquip();
	//获得Owner的职业
	virtual UINT8 getOwnerJob() {   return 0 ;}
	//获得Owner的性别
	virtual UINT8 getOwnerSex() { return 0; }
	//获得Owner的等级
	virtual UINT16 getOwnerLv() { return 0 ;}
	//填充我的装备信息
	template <class T>
	void fillMyEquip(T * pDetailInfo);
	//到达某个精炼等级的装备数
	UINT16 getTotalRefineLv(UINT16 lv);
	//到达某个强化等级的装备数
	UINT16 getTotalRaiseLv(UINT16 lv);
	//紫色符文数量
	UINT16 purpleFuwenNum();
private :
	stEquipData m_data;
};

template <class T>
void CEquipBag::fillMyEquip(T * pDetailInfo)
{
	for (UINT16 i = 0;i < m_capacity; ++i){
		if (m_pSlot[i]){
			CItem * pDestItem = m_pSlot[i];
			Role::stSynItemInfo * pSynInfo = pDetailInfo->add_itemdata();
			pDestItem->fillSynData(*pSynInfo);
		}		
	}
}


class CRoleEquipBag : public CEquipBag
{
public :
	//构造
	CRoleEquipBag(GameUser *pOwner);
	//析构
	~CRoleEquipBag();
	//获得Owner的职业
	virtual UINT8 getOwnerJob() ;
	//获得Owner的性别
	virtual UINT8 getOwnerSex() ;
	//获得Owner的等级
	virtual UINT16 getOwnerLv();
    //获取时装翅膀
    CItem * getWingItem();
protected :
	//重写
	virtual bool checkAddItem(CItem * pItem);
private :
	GameUser * m_pOwner;
};

class CRuneBag : public  CBag
{
    friend class CBagMgr;
public :
    const static UINT16 s_RUNE_BAG_SLOT_NUM = 80;
    const static UINT16 s_HERO_RUNE_BAG_SLOT_NUM = 300;
public :
    //构造函数
    CRuneBag(UINT8 bagType, UINT16 slotNum);
    //析构函数
    ~CRuneBag();
	//重新整理
	void reArrange();
    //获取符文数量
    void collectRune(UINT64 instanceId, UINT32 itemId, std::vector<CItem *> & itemVec);
    //复制符文
    CItem * copyItem(CItem * pSrcItem, UINT8 dstBagType);
    //获取Item
    CItem * getItemBySlot(UINT16 slotIndex);
};

class CBagMgr
{
	static const UINT32 s_SAVE_BIN_VERSION = 20130901;
	static const UINT32 s_SAVE_BIN_VERSION_SECOND = 20161124;
public :
	//构造函数
	CBagMgr(GameUser * pOwner);
	//析构函数
	~CBagMgr();
	//保存道具
	UINT32 saveAllItem(byte * buf,UINT32 needSpace);
	//加载道具
	UINT32   loadAllItem(byte * buf);
	//同步所有道具 
	void synAllData();
	//创建一个道具
	static CItem * createItem(UINT32 itemID,UINT32 num,UINT8 raiseLv = 0, bool virtualBag = false);
	//给玩家道具
	bool addItem2Bag(UINT32 itemID,UINT32 num,UINT16 raiseLv,const char * desc);
	//往包裹里添加道具
	bool  addItem2Bag(CItem  * & pItem);
	//检查主包裹里的道具数据
	UINT32 getItemNumByID(UINT32 itemID,UINT16 raiseLv);
	//获得主包裹里的道具数量
	UINT32 getItemNumByID(UINT32 itemID);
	//获得武将碎片包裹里碎片数量
	UINT32 getPieceNumByID(UINT32 pieceID);
	//获得坐骑包裹里道具数量
	UINT32 getMountNumByID(UINT32 itemID);
	//从主包裹删除某个id的道具
	bool  removeItemByItemID(UINT32 itemID,UINT16 raiseLv,UINT32 num,const char * desc);
	//从主包裹删除某个id的道具
	bool  removeItemByItemID(UINT32 itemID,UINT32 num,const char * desc); 
	//扣除道具 
	bool  decItem(CItem *pItem,UINT32 num ,const char * desc);
	//根据实例id获得道具
	CItem * getItemByInstaceID(UINT8 bagType,UINT64 instanceID);
	//获得某个包裹的空闲位置
	UINT16 getBagFreeIndex(UINT8 bagType);
	//设置包裹数量
	void setBagSize(UINT8 bagType,UINT16 size);
	//返回包裹数量
	UINT16 getBagSize(UINT8 bagType);
	//获得某个包裹的空闲插槽
	UINT16 getFreeSlotNum(UINT8 bagType);
	//检查包裹是否有空位
	bool checkHasFreeSlot(std::vector<std::pair<UINT32,UINT32> > & itemID2Num);
	//获得主包裹插槽数量
	UINT16 getMainFreeSlotNum();
	//丢弃道具
	void sellItem(CItem * pItem);
	//通知新增
	void notifyAddItem(CItem * pItem);
	//通知客户端删除道具
	void notifyDeleteItem(CItem * pItem);
	//刷新道具数据
	void refreshItem(CItem * pItem);
	//移动道具
	bool moveItem(CItem * pItem,const BagPos & dstPos);
	//获得某个ID的道具
	UINT32 getItemByID(UINT32 itemID,UINT64 exceptInstanceID,UINT8 bagType,std::vector<CItem * > & vec);
	//根据某个某个条件获得道具数量
	void getItemByCond(stItemCallBack & cb,UINT8 bagType);		
	//发送装备改变日志
	void sendEquipChgLog(UINT32 itemID,UINT16 oldRaiseLv,UINT16 newRaiseLv,UINT16 oldRefineLv,UINT16 newRefineLv,const char * szAction,UINT32 decGoldcoin,UINT32 decYuntie,UINT32 decItemID,UINT32 addGoldcoin,UINT32 addYuntie,UINT32 addItemID);
    //符文和随从符文包裹移动装备
    bool moveItemForRune(GameUser * pUser, CItem * pItem, UINT16 & slotIndex);
    //删除符文
    bool removeRune(CItem * pItem);
private :
	GameUser * m_pOwner;
public :
	CMainBag  m_mainBag;					//装备包裹
	CMainBag  m_pieceBag;					//碎片包裹
	CMainBag  m_heroPieceBag;				//武将碎片
	CRoleEquipBag m_equipBag;				//装备包裹
	CMainBag  m_fashionBag;					//时装碎片
    CRuneBag  m_runeBag;                    //符文包裹
    CRuneBag  m_heroRuneBag;                //装备在随从身上的符文包裹
    CMainBag  m_runePieceBag;               //符文碎片包裹
    CMainBag  m_mountBag;       	        //坐骑包裹
	UINT32  m_lastArrangeMainBagTime;		//最近一次整理主包裹时间
};
#endif
