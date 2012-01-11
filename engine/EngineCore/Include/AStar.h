#pragma once
#include "math.h"

class AStar
{
public:
#define MAX_NODE 		100 //����ͬʱ���ڶ��ٴ���չ�ڵ�
#define MAX_ALLNODE 	1000 //����ڵ���
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
	Node	node[MAX_NODE];			//����չ�ڵ������
	AllNode	m_allnode[MAX_ALLNODE];	//�ڵ������
	int		m_nNodeCount;			//Ŀǰ�Ĵ���չ�ڵ���
	int		m_nAllNodeCount;		//Ŀǰ�Ľڵ���
	long	m_SearchedFlag;
	int		m_tx;
	int		m_ty;
	int		m_nMinH;
	int		m_nMinHNode;
};