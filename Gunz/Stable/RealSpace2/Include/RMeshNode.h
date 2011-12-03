#ifndef _RMeshNode_h
#define _RMeshNode_h

#include "RMeshNodeData.h"
#include "RShaderMgr.h"

_NAMESPACE_REALSPACE2_BEGIN


//#define USE_TOON_RENDER


///////////////////////////////////////////////////////////////
// ��� ����ü ����

class CIndexBufferMake;

///////////////////////////////////////////////////////////////

enum CalcVertexBufferBboxMode
{
	CalcVertexBufferBboxMode_None = 0,

	CalcVertexBufferBboxMode_VertexAni,
	CalcVertexBufferBboxMode_Physique,
	CalcVertexBufferBboxMode_TM_MapObject,
	CalcVertexBufferBboxMode_TM_Object,

	CalcVertexBufferBboxMode_End,
};


class RBoneBaseMatrix
{
public:
	RBoneBaseMatrix() {
		D3DXMatrixIdentity(&mat);
		id = -1;
	}
	D3DXMATRIX	mat;
	int			id;
};

#define MAX_MATRIX		60
#define MAX_PRIMITIVE	10

class RMeshNodeInfo
{
public:
	RMeshNodeInfo();
	virtual ~ RMeshNodeInfo();

public:

	bool	m_isAddMeshNode;
	bool	m_isCollisionMesh;
	bool	m_isDummyMesh;
	bool	m_isWeaponMesh;
	bool	m_isDummy;
	bool	m_isAlphaMtrl;
	bool	m_isLastModel;

	bool	m_isClothMeshNode;
	bool	m_bClothMeshNodeSkip;
	float	m_vis_alpha;

	int						m_nAlign;

	CutParts				m_CutPartsType;			// ��尡 ��ü,��ü�� ���ϴ��� ���� Ÿ��
	LookAtParts				m_LookAtParts;			// ���α׷����� ��� ���� �� Ư�� ��� Ÿ��
	WeaponDummyType			m_WeaponDummyType;		// ���� �𵨿� �ٴ� ���̵� Ÿ��

	RMeshPartsType			m_PartsType;			// �� ���� + ������ Ÿ��
	RMeshPartsPosInfoType	m_PartsPosInfoType;		// �����е� + ���� ��ġ�� Ÿ��

	bool			m_bNpcWeaponMeshNode;			// NPC �� �տ� �� ���� ������ ����..
	float			m_AlphaSortValue;				// ĳ������ ���İ� ������ ��� ���ð�..

};

class RBatch
{
public:
	RBatch();
	virtual ~RBatch();

	bool CreateVertexBuffer(char* pVert,DWORD fvf,int vertexsize,int vert_num,DWORD flag);
	bool UpdateVertexBuffer(char* pVert);
	bool UpdateVertexBufferSoft(char* pVert);

	bool CreateIndexBuffer(int index,WORD* pIndex,int _size);

public:

	RIndexBuffer*		m_ib[MAX_PRIMITIVE];

	RVertexBuffer*		m_vb;
	RVertexBuffer*		m_vsb;

	bool				m_bIsMakeVertexBuffer;

};

class RMeshNode :public RMeshNodeData , public RMeshNodeMtrl, public RMeshNodeInfo , public RBatch
{
public:

	RMeshNode();
	virtual ~RMeshNode();

	///////////////////////////////////////////
	// animation

	void ConnectToNameID();// �ڱ� �̸����� ����
	bool ConnectMtrl();

	void UpdateNodeBuffer();
	void MakeNodeBuffer(DWORD flag);

	void MakeVertexBuffer(int index,bool lvert,char* pBuf,int _vsize,DWORD flag);
	bool MakeVSVertexBuffer();// vertex update �� ���ҵ� ���Ѵ�..	

	void RenderNodeVS(RMesh* pMesh,D3DXMATRIX* world_mat,ESHADER shader_ = SHADER_SKIN );

	void Render(D3DXMATRIX* pWorldMatrix=NULL);

	RBoneBaseMatrix* GetBaseMatrix(int pid);		// ���������� index �����ϱ� �Ⱦ

	bool isSoftRender();

	void CalcVertexBuffer(D3DXMATRIX* world_mat,bool box=false);
	bool CalcPickVertexBuffer(D3DXMATRIX* world_mat,D3DXVECTOR3* pVec);
	int	 CalcVertexBuffer_VertexAni(int frame);
	void CalcVertexBuffer_Physique(D3DXMATRIX* world_mat,int frame);
	void CalcVertexBuffer_Tm(D3DXMATRIX* world_mat,int frame);
	void CalcVertexBuffer_Bbox(CalcVertexBufferBboxMode nBboxMode,rmatrix& mat);

	void CalcVertexNormal(D3DXMATRIX* world_mat);

	void CheckAlign(rmatrix* worldmat);
	void CheckAlignMapObject(rmatrix& hr_mat); // �ʿ�����Ʈ��..

	float GetNodeVisValue();
	int   GetNodeAniSetFrame();

	bool isAlphaMtrlNode();

	void ToonRenderSettingOnOld(RMtrl* pMtrl);// �ӽ�,,
	void ToonRenderSettingOn(RMtrl* pMtrl);// �ӽ�,,
	void ToonRenderSettingOff();

	void ToonRenderSilhouetteSettingOn();// �ӽ�,,
	void ToonRenderSilhouetteSettingOff();

private:

	bool SetBVertData(RBlendVertex* pBVert,int i,int j ,int pv_index,int* DifferenceMap,int& matrixIndex);

public:

	///////////////////////////////////////////
	// reference pointer

	RMesh*			m_pParentMesh;	// �ڽ��� �θ� �� ����...
	RMesh*			m_pBaseMesh;	// ������ �⺻ �� ����...
	RMeshNode*		m_pParent;
	RMeshNode*		m_Next;
	RMeshNode*		m_ChildRoot;

	float				m_fDist;

	RBoneBaseMatrix*	m_BoneBaseMatrix;
	int					m_nBoneBaseMatrixCnt;

	int m_MatrixMap[MAX_MATRIX];			// Matrix Register�� Bone�� index�� �����ϱ� ���� ��
	int m_MatrixCount;						// ���̴� Matrix�� ����( �ʱⰪ : -1 )

	D3DXMATRIX		m_ModelWorldMatrix;
	
	bool			m_bConnectPhysique;

#ifndef _BLEND_ANIMATION
	RAnimationNode*	m_pAnimationNode;
#else
	RAnimationNode* m_pAnimationNode[2];
#endif

};

_NAMESPACE_REALSPACE2_END

#endif//_RMeshNode_h
