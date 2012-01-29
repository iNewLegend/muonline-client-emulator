#pragma once
#include "RenderSystem.h"
//#include "TextureMgr.h"
//#include "HardwareBufferMgr.h"
//#include "D3D9ShaderMgr.h"

#if defined(_DEBUG)
#pragma comment(lib, "rendersystemd.lib")
#else
#pragma comment(lib, "rendersystem.lib")
#endif

struct IDirect3DTexture9;
struct IDirect3DDevice9;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;

template<class _InContainer, class T, class _Ret>
inline
void myTransform(_InContainer& _Container, _Ret (T::*_pFunc)(void))
{
	for (_InContainer::iterator it=_Container.begin(); it!=_Container.end(); it++)
	{
		(((T*)*it)->*_pFunc)();
	}
}

template<class _InContainer, class T, class _Ret>
inline
void myMgrTransform(_InContainer& _Container, _Ret (T::*_pFunc)(void))
{
	for (_InContainer::iterator it=_Container.begin(); it!=_Container.end(); it++)
	{
		(((T*)it->second.pItem)->*_pFunc)();
	}
}

template<class _InContainer, class T, class _Ret>
inline
	void myMapTransform(_InContainer& _Container, _Ret (T::*_pFunc)(void))
{
	for (auto it=_Container.begin(); it!=_Container.end(); it++)
	{
		(((T*)it->second)->*_pFunc)();
	}
}

class CD3D9RenderSystem : public CRenderSystem
{
public:
	CD3D9RenderSystem();
	~CD3D9RenderSystem();
public:
	IDirect3DDevice9* GetD3D9Device(){return m_pD3D9Device;}
public:
	CTextureMgr& GetTextureMgr();
	CHardwareBufferMgr& GetHardwareBufferMgr();
	CRenderWindow* CreateRenderWindow(WNDPROC pWndProc, const std::wstring& strWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen = false);
	// 创建设备
	//virtual HRESULT OnCreateDevice(LPDIRECT3DDEVICE9 pd3dDevice);
	// 重置D3D设备
	virtual HRESULT OnResetDevice();
	void OnLostDevice();
	void OnDestroyDevice();
	// ----
	// # new
	// ----
	CTexture*	newTexture			();
	CShader*	newShader			();
	virtual CHardwareVertexBuffer*	newHardwareVertexBuffer	();
	virtual CHardwareIndexBuffer*	newHardwareIndexBuffer	();
	// ----
	bool BeginFrame();	// 帧渲染开始
	void EndFrame();	// 帧渲染结束
	// ----
	//
	CTexture* GetRenderTarget();
	void SetRenderTarget(int index, CTexture* pRenderTarget);
	//
	CTexture* GetDepthStencil();
	void SetDepthStencil(CTexture* pDepthStencil);



	void setViewport(const CRect<int>& rect);
	void getViewport(CRect<int>& rect);
	//
	void ClearBuffer(bool bZBuffer, bool bTarget, Color32 color);

	// 填充模式
	void SetFillMode(FillMode mode);

	// set matrix
	void setWorldMatrix(const Matrix& m);
	void setViewMatrix(const Matrix& m);
	void setProjectionMatrix(const Matrix& m);
	void setTextureMatrix(unsigned char uTexChannel, TextureTransformFlag flag, const Matrix& m = Matrix::ZERO);
	// get matrix
	void getWorldMatrix(Matrix& m)const;
	void getViewMatrix(Matrix& m)const;
	void getProjectionMatrix(Matrix& m)const;
	void getTextureMatrix(unsigned char uTexChannel, Matrix& m)const;

	// Func
	void SetStencilFunc(bool bStencil, StencilOP op=STENCILOP_INCR,							// 模板检测
		CompareFunction stencilFunction = CMPF_LESS_EQUAL);
	//
	void setShaderFloat(const char* szName, float val);
	void setShaderVec2D(const char* szName, const Vec2D& val);
	void setShaderVec3D(const char* szName, const Vec3D& val);
	void setShaderVec4D(const char* szName, const Vec4D& val);
	void setShaderMatrix(const char* szName, const Matrix& mat);

	void SetPixelShaderConstantF(unsigned int StartRegister,const float* pConstantData,unsigned int Vector4fCount);
	//
	void SetSamplerFilter(size_t unit, TextureFilterType MagFilter, TextureFilterType MinFilter, TextureFilterType MipFilter);
	void SetSamplerAddressUV(size_t unit, AddressUV addressU, AddressUV addressV);

	// 设置shader
	void SetShader(CShader* pShader);
	void SetShader(const char* szShader);
	// Light
	void SetDirectionalLight(unsigned long uIndex,const DirectionalLight& light);
	void setPointLight(unsigned long uIndex,const PointLight& light);
	//
	// 设置纹理
	void SetTexture(unsigned long Stage, unsigned long TextureID);
	void SetTexture(unsigned long Stage, const CTexture* pTexture);
	// Get
	CTexture* GetTexture(unsigned long Stage);

	CVertexDeclaration* CreateVertexDeclaration();
	// 创建VB IB
	CHardwareVertexBuffer* CreateVertexBuffer(size_t numVerts, size_t vertexSize,
		CHardwareBuffer::Usage usage=CHardwareBuffer::HBU_STATIC);
	CHardwareIndexBuffer* CreateIndexBuffer(size_t numIndexes, CHardwareIndexBuffer::IndexType itype = CHardwareIndexBuffer::IT_16BIT,
		CHardwareBuffer::Usage usage=CHardwareBuffer::HBU_STATIC);
	// 顶点
	void SetVB(int nVBID);
	// 设置FVF顶点格式
	void SetFVF(unsigned long FVF);
	//
	void SetVertexDeclaration(CVertexDeclaration* pDecl);
	//
	void SetStreamSource(unsigned long StreamNumber, CHardwareVertexBuffer* pStreamData,unsigned long OffsetInBytes,unsigned long Stride);
	void SetIndices(CHardwareIndexBuffer* pIndexData);

	// 绘制
	void DrawPrimitive(VertexRenderOperationType PrimitiveType,unsigned long StartVertex,unsigned long PrimitiveCount);
	void DrawIndexedPrimitive(VertexRenderOperationType PrimitiveType,int32 BaseVertexIndex,unsigned long MinVertexIndex,unsigned long NumVertices,unsigned long startIndex,unsigned long primCount);
	void DrawPrimitiveUP(VertexRenderOperationType PrimitiveType,unsigned long PrimitiveCount,const void* pVertexStreamZeroData,unsigned long VertexStreamZeroStride);
	void DrawIndexedPrimitiveUP(VertexRenderOperationType PrimitiveType,unsigned long MinVertexIndex,unsigned long NumVertices,unsigned long PrimitiveCount,const void* pIndexData,const void* pVertexStreamZeroData,unsigned long VertexStreamZeroStride);
	
	void drawIndexedSubset(const IndexedSubset& subset);

	void setFog(const Fog& fog);
	void setFogEnable(bool bEnable);

	void StretchRect(CTexture* pSource,const CRect<int>* pSourceRect,CTexture* pDest,const CRect<int>* pDestRect,TextureFilterType filter);
	void commond(const char* szCommond);
protected:
	void SetTexture(unsigned long Stage, IDirect3DTexture9* pD3D9Texture);
	//
	//void SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX* pMatrix);
	//void GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);
protected:
	// ----
	// # 非插件数据
	// ----
	CTextureMgr					m_TextureMgr;
	CHardwareBufferMgr			m_D3D9HardwareBufferMgr;
	// ----
	IDirect3DDevice9*			m_pD3D9Device;

	CShader*					m_pOldShader;

	//////////////////////////////////////////////////////////////////////////
};

CD3D9RenderSystem& GetD3D9RenderSystem();