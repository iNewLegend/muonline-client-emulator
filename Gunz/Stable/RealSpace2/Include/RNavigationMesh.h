#ifndef _RNAVIGATIONMESH_H
#define _RNAVIGATIONMESH_H

#include "RTypes.h"
#include "RLine.h"
#include "RAStar.h"
#include <vector>
#include <list>
using namespace std;

class RNavigationNode;
class MZFileSystem;

// �׺���̼� �޽� ���̽�
struct RNavFace
{
	unsigned short	v1, v2, v3;
};


class RNavigationPath
{
public:
	RNavigationPath() {}
	virtual ~RNavigationPath() { }
};

class RNavigationMesh
{
public:
	// --- Ÿ�� ���� -------------
	typedef	std::vector<RNavigationNode*> RNodeArray;
private:
	// --- data ------------------
	RNodeArray			m_NodeArray;
	list<rvector>		m_WaypointList;

	RAStar				m_AStar;
	RNavigationNode*	m_pStartNode, *m_pGoalNode;
	int					m_nVertCount;
	int					m_nFaceCount;
	rvector*			m_vertices;
	RNavFace*			m_faces;


	void AddNode(int nID, const rvector& PointA, const rvector& PointB, const rvector& PointC);
	void LinkNodes();
	void MakeNodes();
	void ClearNodes();
public:
	// --- func ------------------
	RNavigationMesh();
	virtual ~RNavigationMesh();
	void Clear();
	bool Open(const char* szFileName, MZFileSystem* pZFileSystem=NULL);
	bool Save(const char* szFileName);
	void ClearAllNodeWeight();

	RNavigationNode* FindClosestNode(const rvector& point) const;
	bool BuildNavigationPath(RNavigationNode* pStartNode, 
							 const rvector& StartPos, 
							 RNavigationNode* pEndNode, 
							 const rvector& EndPos);

	// Path Finding
	rvector SnapPointToNode(RNavigationNode* pNode, const rvector& Point);
	rvector SnapPointToMesh(RNavigationNode** NodeOut, const rvector& Point);
	bool LineOfSightTest(RNavigationNode* pStartNode, const rvector& StartPos, RNavigationNode* pGoalNode, const rvector& EndPos);
	bool BuildNavigationPath(const rvector& vStartPos, const rvector& vGoalPos);



	RNodeArray* GetNodes()		{ return &m_NodeArray; }
	int GetNodeCount()			{ return (int)m_NodeArray.size(); }
	list<rvector>&	GetWaypointList() { return m_WaypointList; }

	// �׽�Ʈ�� ------------------
	void Render();
	void RenderLinks();

	// exporter���� ��� ---------
	inline void InitVertices(int vert_count);
	inline void InitFaces(int face_count);
	inline void SetVertex(int index, rvector& v);
	inline void SetFace(int index, RNavFace& f);
};




// inline /////////////////////////////////////////////////////////////////////
inline void RNavigationMesh::InitVertices(int vert_count)
{
	if (m_vertices != NULL) delete [] m_vertices;
	m_nVertCount = vert_count;
	m_vertices = new rvector[vert_count];
}
inline void RNavigationMesh::InitFaces(int face_count)
{
	if (m_faces != NULL) delete [] m_faces;
	m_nFaceCount = face_count;
	m_faces = new RNavFace[face_count];
}

inline void RNavigationMesh::SetVertex(int index, rvector& v)
{
	m_vertices[index] = v;
}

inline void RNavigationMesh::SetFace(int index, RNavFace& f)
{
	m_faces[index] = f;
}

#endif