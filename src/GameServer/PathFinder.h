#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include"TypesDef.h"

//AI可以行走格子数目
#define MAX_ROUTES 1024 * 2
#define EIGHT_DIRECTIONS 8

#include<stdlib.h>
#include<vector>
#include"Obj.h"

class CMap;


struct MapMoveAbleExec
{
	MapMoveAbleExec(){}
	virtual ~MapMoveAbleExec(){}
	virtual bool operator()(const CCellIdx& pos)
	{
		return true;
	}
};


class PathFinder
{
private:
	//8方向查找表
	struct _D2XZLOOKUP
	{
		int x;
		int y;
	} DZX[EIGHT_DIRECTIONS];

	//结点状态记录
	struct AStarNode
	{
		char state; 		//当前节点状态
		int h; 				//到目标的代价估值
		int g; 				//起点当当前的代价
		int f;				//f=g+h
		char dir;			//上一个点在自己的哪个方向
		AStarNode* father;	//寻路算法中的前一个节点
		AStarNode* prev;	//链表前一个结点	
		AStarNode* next;	//链表后一个结点 
		AStarNode* hashNext;//存放在HASH表中的位置

		bool operator == (const AStarNode& n)
		{
			return f == n.f;
		}
		bool operator > (const AStarNode& n)
		{
			return f > n.f;
		}
		bool operator >=(const AStarNode& n)
		{
			return operator==(n) || operator>(n);
		}
		bool operator < (const AStarNode& n)
		{
			return !operator>=(n); 
		}
		bool operator <= (const AStarNode& n)
		{
			return !operator>(n); 
		}
	};
	//结点状态定义 
	enum
	{
		UNKNOWN = 0,		//未标记状态
		OPEN = 1,			//处于开列表中
		CLOSED = 2,			//处于关闭列表中
	};
	int m_height;            				// 地图的高度
	int m_width;             				// 地图的宽度
	AStarNode *m_allNode;     				//对应地图中每个节点,用一维数组初始化效率比较高:
	int 	  m_openCount;					//开启列表长度
	AStarNode *m_allOpen;   				//开启列表：保存没有处理的按估计值排序的节点
	AStarNode *m_nearNode;     				//记录最近得到的一个结点，最接近目标的结点
	static AStarNode * s_AStartNodeMem;		//A *寻路中格子所需要的内存
public:
	PathFinder();
	//析构函数
	virtual ~PathFinder();
	//分配寻路中需要的内存
	static void allocAstarNodeMem(UINT32 maxSize);
	//释放寻路中分配的内存
	static void deallocAstarNodeMem();
	//初始化
	bool init(int,int);
	void reset();
	//用A *进行寻路
	bool findPath(const CCellIdx& s,const CCellIdx& d,MapMoveAbleExec& moveAble,CCellIdx* posNode,int& numNode,int maxRoot=1024,bool bLine=false);
	//沿某个方向是直走一定距离
	bool findPath(const CCellIdx& s,char dir,INT16 dist,MapMoveAbleExec& moveAble,CCellIdx* posNode,int& numNode);
private:
	//返回某个方向上，距离start距离为dist的点
	bool getPosByDir(const CCellIdx& start,CCellIdx& dstPos,byte dir,int dist=1);
	//判断是否为格子上的点，之外的点不在寻路算法上
	inline bool isValidPos(CCellIdx pos)
	{
		return pos.m_x>=0 && pos.m_z>=0 && pos.m_x< m_width && pos.m_z < m_height;
	}
	//计算代价
	inline int judgeCost(CCellIdx s,CCellIdx d)
	{
		return (abs(s.m_x - d.m_x)+abs(s.m_z - d.m_z))*10;
	}
	//根据节点得到代价		
	CCellIdx getPosByNode(AStarNode* node)
	{
		//根据地址值计算出相对位置
		int distance = node - m_allNode;
		CCellIdx idx;
		idx.m_x = distance%m_width;
		idx.m_z = distance/m_width;
		return idx;
	}
private:
	void initEightDirections();
	// 将节点排序加入Open队列
	int addToOpenQueue(AStarNode *node);
	// 从Open队列取出最小的
	AStarNode* popOpenQueue();
	//检查每个格子
	char tryTile(const CCellIdx& s,const CCellIdx& d,AStarNode *father,MapMoveAbleExec& moveAble,char FromDir);	
};
#endif
