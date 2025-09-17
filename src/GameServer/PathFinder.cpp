#include "PathFinder.h"
#include <string.h>
#include "AthenaNew.h"
#include "Log4cxxHelp.h"

#ifdef _HDZ_DEBUG
#include <iostream>
using namespace std;
#endif
#include "Obj.h"

PathFinder::AStarNode * PathFinder::s_AStartNodeMem = NULL;	//A *寻路中格子所需要的内存

PathFinder::PathFinder()
{
	m_height = 0;
	m_width = 0;
	m_allNode = NULL;
	m_openCount = 0;
	m_allOpen = NULL;
	m_nearNode = NULL;
	initEightDirections();
}

void PathFinder::allocAstarNodeMem(UINT32 maxSize)
{
	s_AStartNodeMem = ATHENA_NEW AStarNode[sizeof(AStarNode) * maxSize];	
}

void PathFinder::deallocAstarNodeMem()
{
	SAFE_DELETE_ARRAY(s_AStartNodeMem);
}

void PathFinder::initEightDirections()
{
	DZX[0].x = 0;
	DZX[0].y = -1;

	DZX[1].x = 1;
	DZX[1].y = -1;

	DZX[2].x = 1;
	DZX[2].y = 0;


	DZX[3].x = 1;
	DZX[3].y = 1;

	DZX[4].x = 0;
	DZX[4].y = 1;


	DZX[5].x = -1;
	DZX[5].y = 1;

	DZX[6].x = -1;
	DZX[6].y = 0;

	DZX[7].x = -1;
	DZX[7].y = -1;
}


bool PathFinder::init(int width,int height)
{
	m_width = width;
	m_height = height;
	int maxSize = width * height;

	m_allNode = s_AStartNodeMem;
	if(!m_allNode)
	{
		return false;
	}

	//初始化坐标
	bzero(m_allNode,sizeof(AStarNode)*maxSize);
	m_openCount = 0 ;
	m_allOpen = NULL;

	return true;
}

PathFinder::~PathFinder()
{
}


void PathFinder::reset()
{
	//大数组初始化，有待提高
	bzero(m_allNode,m_width*m_height*sizeof(AStarNode));
	m_allOpen = NULL;
	m_openCount = 0;
}


int PathFinder::addToOpenQueue(AStarNode *node)
{
	//p,b为插入位置的后一个结点和前一个结点
	AStarNode *p,*b;

	int i;

	//寻找合适的位置插入，b记录了插入位置,插入到b大后面
	for(b=NULL,p=m_allOpen,i=0; p&&i<=m_openCount; b=p,p=p->next,i++){ 
		if(*node <= *p) {
			break;
		}
	}

	node->next=p;
	node->prev=b;

	if(b) { 
		b->next=node;
	}
	else { //队列中第一个元素 
		m_allOpen=node;
	}

	if(p) { 
		p->prev=node;
	}

	node->state = OPEN;

	m_openCount++;

#ifdef _HDZ_DEBUG
	{
		//cout<<"\t\t开启列表大小"<<m_openCount<<endl;
	}
#endif
	return 0;
}

//弹出代价最小的结点，并放入关闭队列
PathFinder::AStarNode* PathFinder::popOpenQueue()
{
	AStarNode *bestNode=m_allOpen;
	if(!m_allOpen) {
		return NULL;
	}
	
	m_allOpen = m_allOpen->next;
	if(m_allOpen) {//弹出队列顶部
		m_allOpen->prev=NULL;
	}
	bestNode->state = UNKNOWN;//清楚开启标记

	m_openCount--;

	return bestNode;
}

bool PathFinder::getPosByDir(const CCellIdx& start,CCellIdx& pos,byte dir,int dist)
{
	if(dir > 7){
		return false;
	}

	pos.m_x = start.m_x + DZX[dir].x * dist; 
	pos.m_z = start.m_z + DZX[dir].y * dist;

	if(isValidPos(pos)){
		return true;
	}

	return false;
}


bool PathFinder::findPath(const CCellIdx& s,char dir,INT16 dist,MapMoveAbleExec& moveAble,CCellIdx* posNode,int& numNode)
{
	//计算数组下标
	numNode = 0;
	CCellIdx dstPos;
	for(INT16 i=0;i < dist;i++)
	{
		if(getPosByDir(s,dstPos,dir,i+1))
		{
			if(moveAble(dstPos)){
				posNode[numNode++] = dstPos;
			}
			else {//没有到达终点
				return false;	
			}
		}
	}

	return true;

}

bool PathFinder::findPath(const CCellIdx& start,const CCellIdx& end,
		MapMoveAbleExec& moveAble,
		CCellIdx* posNode,int& numNode,
		int maxRoot,bool bLine)
{
	if  (start == end){//起点跟终点一直
		return false;
	}
	//计算数组下标
	int mStartxy = start.m_z*m_width +start.m_x;
	int mEndxy = end.m_z*m_width + end.m_x;
	
	numNode = 0;

	{
		reset();
		
		//将起始节点放入open队列中
		AStarNode *root=&m_allNode[mStartxy];
		root->g= 0;//花费初始化为0
		root->h= judgeCost(start,end);//初始化总代价
		root->f= root->g+root->h;
		root->father=NULL; 

		//将根结点增加到开启队列中
		addToOpenQueue(root);

		int count = maxRoot;

		do
		{
			//从开启队列中，找出一个估价值最小的结点
			AStarNode* bestNode = popOpenQueue();

			if(!bestNode){//没有最优路径了
#ifdef _HDZ_DEBUG
				Athena::logger->error("寻路失败了,找不到最优路径");
#endif
				return false;
			}

			CCellIdx bestPos = getPosByNode(bestNode);
#ifdef _HDZ_DEBUG
			//cout<<"取出结点("<<bestPos.m_x<<","<<bestPos.m_y<<")"<<maxRoot-count<<"次"<<endl;
#endif
			//相等为终点,则完成了寻路
			if(bestNode == &m_allNode[mEndxy])
			{				
				//取得路径
				AStarNode * backNode = bestNode;
				for(;backNode;backNode = backNode->father)
				{	
					CCellIdx cellPos= getPosByNode(backNode); 
					posNode[numNode++] = cellPos;
				}

#ifdef _HDZ_DEBUG
				/*{
					for(int i=0;i<numNode;i++)
					{
						cout<<"<--"<<"("<<posNode[i].m_x<<","<<posNode[i].m_y<<")";
					}
					cout<<endl;
				}*/
#endif

				return true;
			}

			//关闭此结点
			bestNode->state = CLOSED;

			//打开所有相邻节点
			for( byte d = 0;d < EIGHT_DIRECTIONS ; d++)	{
				//出界的结点全部裁掉
				CCellIdx dest;
				dest.m_x = bestPos.m_x+DZX[d].x;
				dest.m_z = bestPos.m_z+DZX[d].y;
				tryTile(dest,end,bestNode,moveAble,0);
			}
		} 
		while( --count>0 );
		if (count <= 0){
			Athena::logger->error("寻路算法寻路完了,还没有找到一条路径!");
		}
	}

	return false;
}

char PathFinder::tryTile(const CCellIdx& dest,const CCellIdx& end,AStarNode *father,MapMoveAbleExec& moveAble,char FromDir)
{
	//访问过的方向，可以不予考虑，进行忽略，提高效率
	//检测是否可以移动
	if(!isValidPos(dest)){
		return 1;
	}

	//CCellIdx pos = getPosByNode(father);
	
	if(!moveAble(dest))
		return 1;            // 如果地图无法通过则退出

	int xy = dest.m_z * m_width + dest.m_x;

	AStarNode* node=&m_allNode[xy];
	int g=father->g+1;        	// 实际值计算
	if(node->state != UNKNOWN && g>=node->g)//该结点访问过，否则视为无限大
		return 1;	//代价更大，则不需要更新

#ifdef _HDZ_DEBUG
	{
		//cout<<"\t更新结点("<<dest.m_x<<","<<dest.m_y<<")"<<endl;
	}
#endif


	if(node->state==OPEN)//在Open表中则更新：清除再插入
	{
		//将此结点从列表中清除
		if(node->next) {
			node->next->prev=node->prev;    
		}
		if(node->prev) {
			node->prev->next=node->next;
		}
		else {
			m_allOpen=node->next;
		}

		m_openCount--;
		node->father=father;//修改指向
		node->g=g;
		node->f=node->g+node->h;
		m_nearNode = node;
		addToOpenQueue(node);
	}
	else if(node->state==CLOSED) { //已经在CLOSE列表中，需要更新，转入OPEN队列
		node->state = UNKNOWN;//从CLOSE列表中退出
		
		node->father=father;
		node->g=g;
		node->h=judgeCost(dest,end);
		node->f=node->g+node->h;
		m_nearNode = node;
		addToOpenQueue(node);            // 将节点加入Open队列
	}    
	else{ //没有标记过，则更新，并加入开启列表
		node->father=father;
		node->g=g;
		node->h=judgeCost(dest,end);
		node->f=node->g+node->h;
		m_nearNode = node;
		addToOpenQueue(node);            // 将节点加入Open队列
	}
	return 0;
}

