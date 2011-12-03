#if !defined(AFX_RMESH_H__6FD23F3A_D138_4F55_B03F_A629D35788CB__INCLUDED_)
#define AFX_RMESH_H__6FD23F3A_D138_4F55_B03F_A629D35788CB__INCLUDED_

#include <vector>
#include <hash_map>

#include "RMeshNode.h"
#include "RAnimationMgr.h"
#include "mempool.h"

class MXmlElement;

_NAMESPACE_REALSPACE2_BEGIN

/////////////////////////////////////////////////////////////////////
// ������ ĳ���Ͱ� ������ ��� �޽� ���ϰ� �� ���ϸ��̼� ����,
// ���͸��� ���ϵ��� ���� �����Ѵ�. ���հ��� ����. 
// ( ������Ʈ ������ �ø��� ������ )

#pragma warning(disable : 4996)

typedef RHashList<RMeshNode*>			RMeshNodeHashList;
typedef RHashList<RMeshNode*>::iterator	RMeshNodeHashList_Iter;

/*
class AniTreeNodeMgr {
public:
	AniTreeNodeMgr() {
		m_AniTreeRoot = NULL;
	}
	~AniTreeNodeMgr() {
	}

	void AddNode(RMeshNode* pnode,RMeshNode* node) {

		if(pnode==NULL){
			if(m_AniTreeRoot==NULL) {
				m_AniTreeRoot = node;
				node->m_Next = NULL;
			}
			else {
				node->m_Next = m_AniTreeRoot;
				m_AniTreeRoot = node;
			}
		}
		else {
			if(pnode->m_ChildRoot==NULL) {
				pnode->m_ChildRoot = node;
				node->m_Next = NULL;
			} else {
				node->m_Next = pnode->m_ChildRoot;
				pnode->m_ChildRoot = node;
			}
		}
	}

	RMeshNode* m_AniTreeRoot;
};
*/

//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////

class RMeshMgr;
class RVisualMesh;
class ROcclusionList;
class RPickInfo;

extern int g_poly_render_cnt;

////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////

enum {

	eRRenderNode_Alpha = 0,
	eRRenderNode_Add,

	eRRenderNode_End,
};

enum ALPHAPASS {
	PASS_NORMAL = 0,
	PASS_DEPTH_ONLY = 1,
	PASS_COLOR = 2
};

//	visual mesh

class RRenderNode //: public CMemPoolSm<RRenderNode>
{
public:
	RRenderNode() {
		Clear();
	}

	RRenderNode(int Rmode,rmatrix& m,RMeshNode* pMNode,int nMtrl,int begin,int size,float vis_alpha) {
		Clear();
		Set(Rmode,m,pMNode,nMtrl,begin,size,vis_alpha);
	}

	// �ӽ� �����Ͷ���..
	void Set(int Rmode,rmatrix& m,RMeshNode* pMNode,RMtrl* pMtrl,int begin,int size,float vis_alpha) {
		m_RenderMode = Rmode;
		m_matWorld = m;
		m_pNode = pMNode;
		m_pMtrl = pMtrl;
		m_begin = begin;
		m_size	= size;
		m_vis_alpha = vis_alpha;
	}

	void Set(int Rmode,rmatrix& m,RMeshNode* pMNode,int nMtrl,int begin,int size,float vis_alpha) {
		m_RenderMode = Rmode;
		m_matWorld = m;
		m_pNode = pMNode;
		m_nMtrl = nMtrl;
		m_begin = begin;
		m_size	= size;
		m_vis_alpha = vis_alpha;
	}

	void Clear() {

		D3DXMatrixIdentity(&m_matWorld);
		m_pNode = NULL;
		m_nMtrl = 0;
		m_pMtrl = NULL;
		m_RenderMode = eRRenderNode_Alpha;

		m_begin = 0;
		m_size	= 0;

		m_vis_alpha = 1.0f;
	}

	void Render();

public:

	float		m_vis_alpha;
	int			m_RenderMode;
	rmatrix		m_matWorld;
	RMeshNode*	m_pNode;

	int			m_nMtrl;
	RMtrl*		m_pMtrl;

	int			m_begin;
	int			m_size;
};

class RRenderNodeList : public list<RRenderNode*>
{
public:
	RRenderNodeList() {

	}

	void Render() {

		if(empty())
			return;

		list<RRenderNode*>::iterator node = begin();

		RRenderNode* pNode;

		while( node != end() ) {

			pNode = (*node);

			if(pNode)
				(pNode)->Render();

			node++;
		}
	}

	void Clear() {

		if(empty()) return;

		list<RRenderNode*>::iterator node = begin();

		RRenderNode* pNode;

		for(node = begin(); node != end(); ) {
			pNode = (*node);
			delete pNode;
			pNode = NULL;
			node = erase(node);// ++node
		}

		clear();
	}

#ifdef _DEBUG
	RRenderNode* m_data[1024*1000];//debug
#endif

};

// �۵��ϸ� �޸�Ǯ�� ��ü list �� ����Ѵ�..

#define VERTEX_NODE_MAX_CNT		1000
#define LVERTEX_NODE_MAX_CNT	1000

#define DEF_RRenderNode_ADD_SIZE 10

class RRenderNodeMgr
{
public:
	RRenderNodeMgr() {
		m_nTotalCount = 0;
		Clear();
	}

	virtual ~RRenderNodeMgr() {
	}

	int Add(rmatrix& m,int mode,RMeshNode* pMNode,int nMtrl);

	void Clear();

	void Render();

	int m_nTotalCount;

public:

	static bool m_bRenderBuffer;

public:

	RRenderNodeList	m_RenderNodeList[eRRenderNode_End];
};

// effect , weapon , character , mapobject
// soft �� ���� �ڽ��� ���۸� ������ �־�� �Ѵ�.

inline bool render_alpha_sort(RMeshNode* _a,RMeshNode* _b) 
{
	if( _a->m_fDist < _b->m_fDist )
		return true;
	return false;
}

class RMeshRenderMgr
{
	RMeshRenderMgr() {

	}

public:


	~RMeshRenderMgr() {

	}

	enum RMeshRenderMgrType {

		RRMT_Normal = 0,
		RRMT_DIffuse,
		RRMT_Alpha,
		RRMT_Add,
		RRMT_End
	};

	void Add(int RenderType,RMeshNode* pNode) {

	}

	void Sort(rvector vCameraPos) {
//		for(int i=0;i<RRMT_End;i++) {
//			m_node_list[i].sort(func);
//		}
//		���ĸ� ��Ʈ..
//		ī�޶���� �Ÿ�����ϰ�..
		m_node_list[RRMT_Alpha].sort(render_alpha_sort);
	}

	// alpha �Ÿ� sort ����?

	void Render() {
		// normal,diffuse
		// alpha
		// add 
/*
		for(int i=0;i<RRMT_End;i++) {
			m_node_list[i]
			
		}
*/
	}

	void Clear() {
		for(int i=0;i<RRMT_End;i++) {
//			m_node_list[i].clear(func);
		}
	}

	RMeshNodeHashList m_node_list[RRMT_End];
};

class RParticleLinkInfo
{
public:
	RParticleLinkInfo() {

	}

	void Set(char* _name,char* _dummy_name) {
		name = _name;
		dummy_name = _dummy_name;
	}

public:
	string name;
	string dummy_name;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class RMesh {
public:

	RMesh();
	~RMesh();

	void Init();
	void Destroy();


	bool ReadXml(char* fname);
	bool SaveXml(char* fname);

	bool ReadXmlElement(MXmlElement* pNode,char* Path);

	bool ReadElu(char* fname);
	bool SaveElu(char* fname);

	void RenderFrame();

	void Render(D3DXMATRIX* world_mat,bool NoPartsChange=false);

private:

	void RenderFrameSingleParts();
	void RenderFrameMultiParts();

	void CalcLookAtParts(D3DXMATRIX& pAniMat,RMeshNode* pMeshNode,RVisualMesh* pVisualMesh);
	void CalcLookAtParts2(D3DXMATRIX& pAniMat,RMeshNode* pMeshNode,RVisualMesh* pVisualMesh);

	void RenderSub(D3DXMATRIX* world_mat,bool NoPartsChange,bool RenderBuffer);
	bool CalcParts(RMeshNode* pTMeshNode,RMeshNode* pMeshNode,bool NoPartsChange);

	void RenderNode(RMeshNode *pMeshNode,D3DXMATRIX* world_mat);

private:

	bool SetAnimation1Parts(RAnimation* pAniSet);
	bool SetAnimation2Parts(RAnimation* pAniSet,RAnimation* pAniSetUpper);

public:

	RAnimation* GetNodeAniSet(RMeshNode* pNode);

//	static void RenderSBegin();
//	static void RenderSEnd();

	void SetLitVertexModel(bool v);

	// RenderState

	void SetShaderDiffuseMap(RMtrl* pMtrl,DWORD color);
	void SetShaderAlphaMap();
	void SetShaderAdditiveMap();
	void SetShaderNormalMap();
	void SetShaderAlphaTestMap(int value,float fVis);

	void SetShaderDiffuseMap_OFF();
	void SetShaderAlphaMap_OFF();
	void SetShaderAdditiveMap_OFF();
	void SetShaderNormalMap_OFF();
	void SetShaderAlphaTestMap_OFF();

	void SetMtrlUvAni_ON();
	void SetMtrlUvAni_OFF();

	void SetCharacterMtrl_ON(RMtrl* pMtrl,RMeshNode* pMNode,float vis_alpha,DWORD color);
	void SetCharacterMtrl_OFF(RMtrl* pMtrl,float vis_alpha);
	int  GetCharacterMtrlMode(RMtrl* pMtrl,float vis_alpha);

	void ReloadAnimation();	//�ٽ� �ε� �ϴ°��� �ƴϰ�..�ε� �ȵȰ��� �д´�..

	void SkyBoxMtrlOn();
	void SkyBoxMtrlOff();

public:

	float GetMeshVis();
	void  SetMeshVis(float vis);

	float GetMeshNodeVis(RMeshNode* pNode);

	void SetVisualMesh(RVisualMesh* vm);

	RVisualMesh* GetVisualMesh();

	RMeshNode* FindNode(RMeshPartsPosInfoType type);

	void CheckNodeAlphaMtrl();
	void ClearVoidMtrl();

	bool ConnectPhysiqueParent(RMeshNode* pNode);
	bool CalcLocalMatrix(RMeshNode* pNode);

	void DelMeshList();

	int _FindMeshId(int e_name);
	int _FindMeshId(char* name);
	int	 FindMeshId(RMeshNode* pNode);

	int  FindMeshParentId(RMeshNode* pMeshNode);
	int  FindMeshId(RAnimationNode* pANode);
	int  FindMeshIdSub(RAnimationNode* pANode);

	void RenderBox(D3DXMATRIX* world_mat=NULL);
	void CalcBox(D3DXMATRIX* world_mat=NULL);
	void CalcBoxFast(D3DXMATRIX* world_mat=NULL);
	void CalcBoxNode(D3DXMATRIX* world_mat=NULL);

	void  SetFrame(int nFrame,int nFrame2 = -1);

	bool AddNode(char* name,char* pname,rmatrix& base_mat);
	bool DelNode(RMeshNode* data); // ���Ž� �ڽ��� �θ�� ������ ������Ʈ�� ���ؼ� ���~

	bool ConnectAnimation(RAnimation* pAniSet);
	bool ConnectMtrl();

	void CheckNameToType(RMeshNode* pNode);
	void ConnectMatrix();

	void MakeAllNodeVertexBuffer();

	bool CheckOcclusion( RMeshNode *pMeshNode );

	inline void CalcNodeMatrixBBox(RMeshNode* pNode);
	inline void CalcBBox(D3DXVECTOR3* v);
	inline void SubCalcBBox(D3DXVECTOR3* max,D3DXVECTOR3* min,D3DXVECTOR3* v);

private:

	void _RGetRotAniMat(RMeshNode* pMeshNode,int frame,D3DXMATRIX& t_ani_mat);
	void _RGetPosAniMat(RMeshNode* pMeshNode,int frame,D3DXMATRIX& t_ani_mat);
	void _RGetAniMat(RMeshNode* pMeshNode,int frame,D3DXMATRIX& t_ani_mat);

public:

	bool SetAnimation(RAnimation* pAniSet,RAnimation* pAniSetUpper=NULL);	//�ܺ��� ���ϸ��̼ǰ� �����
	bool SetAnimation(char* ani_name,char* ani_name_lower = NULL);			//������ ���ϸ��̼ǰ� �����
	void ClearAnimation();

	void SetMtrlAutoLoad(bool b);
	bool GetMtrlAutoLoad();

	void SetMapObject(bool b);
	bool GetMapObject();

	char* GetFileName();
	void  SetFileName(char* name);
	bool  CmpFileName(char* name);

	char* GetName();
	void  SetName(char* name);
	bool  CmpName(char* name);

	void GetMeshData(RMeshPartsType type,vector<RMeshNode*>& nodetable);
	void GetPartsNode(RMeshPartsType type,vector<RMeshNode*>& nodetable);
	void TrimStr(const char* szSrcStr, char* outStr);

	RMeshNode* GetMeshData(RMeshPartsType type);
	RMeshNode* GetMeshData(char* name);
	RMeshNode* GetPartsNode(char* name);

	// �׻󾲴°͵��ƴϴϱ� ���߿��ӵ��� �������ٸ� ���
//	RMeshNode* GetPartsNode(RMeshPartsType parts,char* name);

	// RVisualMesh class �� �ű�� ( m_world_mat ������... )

	bool Pick(int x,int y,RPickInfo* pInfo,rmatrix* world_mat=NULL);
	bool Pick(rvector* vInVec,RPickInfo* pInfo,rmatrix* world_mat=NULL);
	bool Pick( const rvector& pos, const rvector& dir, RPickInfo* pInfo, rmatrix* world_mat=NULL );

	void ClearMtrl();
	void SetBaseMtrlMesh(RMesh* pMesh);
	void SetScale(rvector& v);
	void ClearScale();

	void SetPickingType(RPickType type);

	RPickType GetPickingType();

	// ���� ������ΰ�� �ǹ̸� ���´�...
	void SetMeshWeaponMotionType(RWeaponMotionType t);
	RWeaponMotionType GetMeshWeaponMotionType();

	// �� ������Ʈ�� �⺻ ��ġ�� ��� ���� ���
	// ������ ������ ���ϸ��̼��� ���Ǵ� ���� bbox �� ���ؼ� �߽� ��ġ�� ����ؾ���..
	rvector GetOrgPosition();

	void SetPhysiqueMeshMesh(bool b);
	bool GetPhysiqueMesh();

	bool isVertexAnimation(RMeshNode* pNode);

	void SetSpRenderMode(ALPHAPASS ePass);

private:

	bool CalcIntersectsTriangle(rvector* vInVec,RPickInfo* pInfo, D3DXMATRIX* world_mat=NULL,bool fastmode=false);

	void GetNodeAniMatrix(RMeshNode* pMeshNode,D3DXMATRIX& m);

	RMeshNode* UpdateNodeAniMatrix(RMeshNode* pNode);

public:

	RParticleLinkInfo	m_ParticleLinkInfo;

	float	m_fVis;

	bool	m_isPhysiqueMesh;
	bool	m_bUnUsededCheck;

	string	m_FileName;
	string	m_ModelName;

	int		m_id;

	RMeshNodeHashList	m_list;

//	RMeshNode*			m_data[MAX_MESH_NODE_TABLE];
	vector<RMeshNode*>	m_data;

	RWeaponMotionType	m_MeshWeaponMotionType;

	RPickType		m_PickingType;

	int				m_data_num;

	int				m_max_frame[2];
	int				m_frame[2];

	D3DXVECTOR3		m_vBBMax;
	D3DXVECTOR3		m_vBBMin;

	D3DXVECTOR3		m_vBBMaxNodeMatrix;
	D3DXVECTOR3		m_vBBMinNodeMatrix;

	bool			m_is_use_ani_set;	//temp

	rvector			m_vAddBipCenter;

	bool			m_isScale;
	rvector			m_vScale;

	RVisualMesh*	m_pVisualMesh;
	RAnimation*		m_pAniSet[2];

	/////////////////////////////////////
	// �ڽ��� �������� ���� ���� ����..

	RMeshMgr*		m_parts_mgr;

	/////////////////////////////////////
	// �⺻ mesh �� ������ ���. 
	// �������� �ص� ���� ����

	RAnimationMgr	m_ani_mgr;

	bool			m_isNPCMesh;
	bool			m_isCharacterMesh;
	bool			m_LitVertexModel;
	bool			m_mtrl_auto_load;
	bool			m_is_map_object;
	bool			m_bEffectSort;


	RMesh*			m_base_mtrl_mesh;

	////////////////////////////////////
	// mtrl list - tex list 
	
	RMtrlMgr		m_mtrl_list_ex;

	bool			mbSkyBox;

	static int	m_nRMeshCnt;

	static bool mHardwareAccellated;
	static unsigned int mNumMatrixConstant;

	bool m_isMultiAniSet;// ���ϸ��̼��� Ÿ�ٺ��� ��� ������ �ִ� ��

	ALPHAPASS	 m_eRenderMode;

	bool m_isMeshLoaded;

	////////////////////////////////////
	// tool

	void		SetToolSelectNode(RMeshNode* pMNode) {	m_pToolSelectNode = pMNode;	}
	RMeshNode*	GetToolSelectNode()					 { return m_pToolSelectNode;  }
	void		SetToolSelectNodeName(char* name)	 { SetToolSelectNode(GetMeshData(name)); }

	static _RMeshPartsType m_OnlyRenderPartsType;	// ���� : ���⿡ ���Ե� ������ �������Ѵ�

private:

	RMeshNode*	m_pToolSelectNode;

public:

public:

	// util

	static bool  m_bTextureRenderOnOff;
	static bool  m_bVertexNormalOnOff;
	static bool  m_bToolMesh;
	static bool  m_bSilhouette;
	static float m_fSilhouetteLength;


	static int  m_parts_mesh_loading_skip;

	static void SetPartsMeshLoadingSkip(int nSkip) { m_parts_mesh_loading_skip = nSkip;	}

	static void SetToolMesh(bool b) { m_bToolMesh = b; }
	static bool GetToolMesh() { return m_bToolMesh; }

	static void SetTextureRenderOnOff(bool b) { m_bTextureRenderOnOff = b; }
	static bool GetTextureRenderOnOff() { return m_bTextureRenderOnOff; }

	static void SetVertexNormalOnOff(bool b) { m_bVertexNormalOnOff = b; }
	static bool GetVertexNormalOnOff() { return m_bVertexNormalOnOff; }

};

class RPickInfo {
public:
	RPickInfo() {

		vOut.x = 0.f;
		vOut.y = 0.f;
		vOut.z = 0.f;

		t = 0;

		parts=eq_parts_etc;
	}

	~RPickInfo() {
		
	}

public:
	rvector		vOut;
	float		t;
	RMeshPartsType parts;
};

////////////////////////////////////////////////////////////
// help func

void ConvertMat(rmatrix& mat1,rmatrix& mat2);

bool RMeshRenderSBegin();
bool RMeshRenderSEnd();

void SetMtrl(RMtrl* pMtrl,float vis_alpha);

void RenderNodeMgr_Add(rmatrix& m,RMeshNode* pMNode,int nMtrl);
void RenderNodeMgr_Render();

_NAMESPACE_REALSPACE2_END


#endif // !defined(AFX_RMESH_H__6FD23F3A_D138_4F55_B03F_A629D35788CB__INCLUDED_)
