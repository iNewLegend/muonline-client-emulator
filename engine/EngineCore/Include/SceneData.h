#pragma once
#include "iScene.h"
#include "RenderSystem.h"
#include <deque>

enum E_TERRAIN_ATT_TYPE
{
	TERRAIN_ATT_TYPE_SAFE	= 1<<0,
	TERRAIN_ATT_TYPE_BALK	= 1<<2,
	TERRAIN_ATT_TYPE_CLEAR	= 1<<3,
};

#define ATTRIBUTE_GRASS 0x8

#define ATTRIBUTE_SAFE		(0x01<<0)
#define ATTRIBUTE_BREAK		(0x01<<2)
#define ATTRIBUTE_UNVISIBLE	(0x01<<3)


// ��ͼ�ļ�����
class CSceneData:public iSceneData
{
public:
	virtual GSET_CONST_VAR	(Fog&,				m_,Fog);
	virtual GSET_CONST_VAR	(DirectionalLight&,	m_,Light);
private:
	Fog					m_Fog;
	DirectionalLight	m_Light;
public:
	CSceneData(); 
	~CSceneData();
	//
	void				clear();
	void				create(size_t width, size_t height);
	bool				resize(size_t width, size_t height);
	//
	int					getWidth()const			{ return m_nWidth; }
	int					getHeight()const		{ return m_nHeight; }
	int					getVertexCount()const	{ return m_Cells.size(); }
	//
	bool				isCellIn(int x, int y)const;
	bool				isPointIn(int x, int y)const;
	//
	TerrainCell*		getCell(int x, int y);
	const TerrainCell*	getCell(int x, int y)const;
	//
	unsigned char		getCellTileID(int x, int y, size_t layer = 0)const;
	void				setCellTileID(int x, int y, unsigned char uTileID, size_t layer = 0);
	//
	unsigned long		getVertexIndex(int nCellX, int nCellY)const;
	int					getCellXByVertexIndex(unsigned long uVertexIndex)const;
	int					getCellYByVertexIndex(unsigned long uVertexIndex)const;
	//
	float				getVertexHeight(int x, int y)const;
	void				setVertexHeight(int x, int y, float fHeight);
	//
	Vec3D				getVertexNormal(int x, int y)const;
	//
	unsigned char		getCellAttribute(int x, int y)const;
	void				setCellAttribute(int x, int y, unsigned char uAtt);
	//
	Color32				getVertexColor(int x, int y)const;
	void				setVertexColor(int x, int y, Color32 color);
	//
	float				getHeight(float fX, float fY)const;
	Vec4D				getColor(float fX, float fY)const;
	// ��ײ��ѡ
	virtual bool		pickCell(int nCellX, int nCellY, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const;
	virtual bool		pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos = NULL)const;
	//
	const std::string&	getFilename()const{return m_strFilename;}
	//
	bool				hasGrass(int nCellX, int nCellY)const;

	std::vector<TerrainCell>&		getCells()	{return m_Cells;}

	unsigned char		getPath(int sx,int sy,int tx,int ty, std::deque<char>& path);
	//////////////////����ʽ����(A*)Ѱ·/////////////////////////////////
#define MAX_NODE 		100 //����ͬʱ���ڶ��ٴ���չ�ڵ�
#define MAX_ALLNODE 	1000 //����ڵ���
	struct Node
	{
		int x,y,f,level,n;
	};
	//����չ�ڵ������
	Node node[MAX_NODE];
	//�ڵ������
	struct AllNode
	{
		unsigned char dir;
		int father;
	}
	m_allnode[MAX_ALLNODE];

	int m_nNodeCount; //Ŀǰ�Ĵ���չ�ڵ���
	int m_nAllNodeCount; //Ŀǰ�Ľڵ���
	void AddNode(int x,int y,int tx,int ty,unsigned char dir,int level,int father);
	Node GetNode();
	void Add2Hash(int x,int y);
	bool CheckHash(int x,int y);

	long m_SearchedFlag;
protected:
	std::string					m_strFilename;
	int							m_nWidth;
	int							m_nHeight;

	std::vector<TerrainCell>	m_Cells;
	unsigned short				m_uMuFlgMap;
	unsigned long				m_uMuFlgAtt;
};