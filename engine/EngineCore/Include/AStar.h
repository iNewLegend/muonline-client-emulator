#pragma once
#include "math.h"

class AStar
{
public:
#define MAX_NODE 		100 //允许同时存在多少待扩展节点
#define MAX_ALLNODE 	1000 //允许节点数
	struct Node
	{
		int x,y,f,level,n;
	};

	struct AllNode
	{
		int dir;
		int father;
	};

	void addNode(int x,int y,int dir,int level,int father);
	Node getNode();
	virtual void add2Hash(int x,int y)=0;
	virtual bool checkHash(int x,int y)=0;
public:
	Node	node[MAX_NODE];			//待扩展节点的资料
	AllNode	m_allnode[MAX_ALLNODE];	//节点的资料
	int		m_nNodeCount;			//目前的待扩展节点数
	int		m_nAllNodeCount;		//目前的节点数
	long	m_SearchedFlag;
	int		m_tx;
	int		m_ty;
	int		m_nMinH;
	int		m_nMinHNode;
};