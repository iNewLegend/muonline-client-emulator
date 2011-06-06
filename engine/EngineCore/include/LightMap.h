#pragma once
#include "Scene.h"
#include "RenderSystem.h"
#include "3DMapSceneObj.h"

class CLightMap
{
public:
	CLightMap();
	~CLightMap();

	// �����豸
	HRESULT OnResetDevice();
	// ��ʧ�豸
	void OnLostDevice();
	// �����豸
	void OnDestroyDevice();
public:
	void		SetScene(CScene* pScene);
	void		CalcLightMap(int nCalcCount = 1);
	void		ClearLightMap();
	void		CalcLightMapByShader(int nCalcCount = 1);
protected:

	void		RenderDepthToTex(const Matrix& mLight);
	void		RenderTerrainLightMapToTex(const Matrix& mLight, const Cube& cube);
	void		RenderObjectLightMapToTex(const Matrix& mLight, const C3DMapSceneObj& object);
	Matrix		CalcLightMatrix(const BBox& bbox, const Vec3D& vLightDir);
	void		CalcChunkLightMap(const Vec3D& vLightDir, const Cube& cube);
	void		CalcObjectLightMap(const Vec3D& vLightDir, const C3DMapSceneObj& object);
	//void		CalcObjectLightMap(const D3DXVECTOR3& vLightDir, HR3D_CSceneObject* pOject);
	//void		CalcLightMapMatrix(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vLightDir);
public:
	unsigned long			m_uShaderID;
	// �豸Ĭ����ȾĿ��
	CTexture*		m_pBackRenderTarget;
	CTexture*		m_pBackDepthStencil;
	// ��Ӱ����
	CTexture*		m_pLightMapRenderTarget;
	CTexture*		m_pZDepthStencil;
	// �������
	CTexture*		m_pDepthRenderTarget;
	//
	int m_nWidth,m_nHeight;

	CScene* m_pScene;
};