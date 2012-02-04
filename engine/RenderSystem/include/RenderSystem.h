#pragma once
#include "Shader.h"
#include "RenderSystemCommon.h"
#include "TextureMgr.h"
#include "RenderWindow.h"
#include "HardwareVertexBuffer.h"
#include "HardwareIndexBuffer.h"
#include "HardwareBufferMgr.h"
#include "VertexDeclaration.h"
#include "Pos2D.h"
#include "Rect.h"
#include "Material.h"

#if defined(_DEBUG)
#pragma comment(lib, "commond.lib")
#pragma comment(lib, "mathd.lib")
#else
#pragma comment(lib, "common.lib")
#pragma comment(lib, "math.lib")
#endif


class CRenderSystem
{
public:
	CRenderSystem();
	virtual ~CRenderSystem();

	static void setSingleton(CRenderSystem* pRenderSystem);
	static CRenderSystem& getSingleton();
public:
	virtual CTextureMgr&		GetTextureMgr() = 0;
	virtual CHardwareBufferMgr&	GetHardwareBufferMgr() = 0;
	virtual CRenderWindow*		CreateRenderWindow(WNDPROC pWndProc, const std::wstring& strWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen = false) = 0;
	// ----
	// # new
	// ----
	virtual CTexture*	newTexture			() = 0;
	virtual CShader*	newShader			() = 0;
	virtual CHardwareVertexBuffer*	newHardwareVertexBuffer	() = 0;
	virtual CHardwareIndexBuffer*	newHardwareIndexBuffer	() = 0;
	// ----
	virtual bool		BeginFrame()		= 0;	// 帧渲染开始
	virtual void		EndFrame()			= 0;	// 帧渲染结束
	// ----
	virtual void		ClearBuffer			(bool bZBuffer, bool bTarget, Color32 color) = 0;
	// ----
	virtual CTexture*	GetRenderTarget		() = 0;
	virtual CTexture*	GetDepthStencil		() = 0;
	// ----
	virtual void		SetRenderTarget		(int index, CTexture* pRenderTarget)	= 0;
	virtual void		SetDepthStencil		(CTexture* pDepthStencil)	= 0;
	// ----
	virtual void		setViewport			(const CRect<int>& rect)	= 0;
	virtual void		getViewport			(CRect<int>& rect)			= 0;
	// ----
	virtual void		SetFillMode			(FillMode mode) = 0;
	// ----
	// # set matrix
	// ----
	virtual void		setWorldMatrix		(const Matrix& m) = 0;
	virtual void		setViewMatrix		(const Matrix& m) = 0;
	virtual void		setProjectionMatrix	(const Matrix& m) = 0;
	// ----
	// # get matrix
	// ----
	virtual void		getWorldMatrix		(Matrix& m)const = 0;
	virtual void		getViewMatrix		(Matrix& m)const = 0;
	virtual void		getProjectionMatrix	(Matrix& m)const = 0;
	virtual void		getTextureMatrix	(unsigned char uTexChannel, Matrix& m)const = 0;
	// ----
	virtual void		SetStencilFunc(bool bStencil, StencilOP op=STENCILOP_INCR,							// 模板检测
								CompareFunction stencilFunction = CMPF_LESS_EQUAL) = 0;// ----
	virtual void		setShaderFloat(const char* szName, float val)=0;
	virtual void		setShaderFloatArray(const char* szName, const void* pVal, int nCount)=0;
	virtual void		setShaderMatrix(const char* szName, const Matrix& mat)=0;

	virtual void		SetPixelShaderConstantF(unsigned int StartRegister,const float* pConstantData,unsigned int Vector4fCount)=0;
	// ----
	virtual void		SetSamplerFilter	(size_t unit, TextureFilterType MagFilter, TextureFilterType MinFilter, TextureFilterType MipFilter) = 0;
	virtual void		SetSamplerAddressUV	(size_t unit, AddressUV addressU, AddressUV addressV) = 0;
	// ----
	// # 设置shader
	// ----
	virtual void SetShader(CShader* pShader) = 0;
	virtual void SetShader(const char* szShader) = 0;
	// ----
	// # 设置纹理
	// ----
	virtual void SetTexture(unsigned long Stage, unsigned long TextureID) = 0;
	virtual void SetTexture(unsigned long Stage, const CTexture* pTexture) = 0;
	// ----
	// # Get
	// ----
	virtual CTexture* GetTexture(unsigned long Stage) = 0;
	// ----
	virtual CVertexDeclaration* CreateVertexDeclaration() = 0;
	// 顶点
	virtual void SetVB(int nVBID) = 0;
	// 设置FVF顶点格式
	virtual void SetFVF(unsigned long FVF) = 0;
	//
	virtual void SetVertexDeclaration(CVertexDeclaration* pDecl) = 0;
	//
	virtual void SetStreamSource(unsigned long StreamNumber, CHardwareVertexBuffer* pStreamData,unsigned long OffsetInBytes,unsigned long Stride) = 0;
	virtual void SetIndices(CHardwareIndexBuffer* pIndexData) = 0;

	// 绘制
	virtual void DrawPrimitive(VertexRenderOperationType PrimitiveType,unsigned long StartVertex,unsigned long PrimitiveCount) = 0;
	virtual void DrawIndexedPrimitive(VertexRenderOperationType PrimitiveType,int32 BaseVertexIndex,unsigned long MinVertexIndex,unsigned long NumVertices,unsigned long startIndex,unsigned long primCount) = 0;
	virtual void DrawPrimitiveUP(VertexRenderOperationType PrimitiveType,unsigned long PrimitiveCount,const void* pVertexStreamZeroData,unsigned long VertexStreamZeroStride) = 0;
	virtual void DrawIndexedPrimitiveUP(VertexRenderOperationType PrimitiveType,unsigned long MinVertexIndex,unsigned long NumVertices,unsigned long PrimitiveCount,const void* pIndexData,const void* pVertexStreamZeroData,unsigned long VertexStreamZeroStride) = 0;

	virtual void drawIndexedSubset(const IndexedSubset& subset) = 0;

	virtual void StretchRect(CTexture* pSource,const CRect<int>* pSourceRect,CTexture* pDest,const CRect<int>* pDestRect,TextureFilterType filter) = 0;
	//
	void world2Screen(const Vec3D& vWorldPos, Pos2D& posScreen);
	void GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir,int x, int y);

	// set material
	bool prepareMaterial(const CMaterial& material);
	// Shader
	CShader* registerShader(const char* szName, const char* szFilename);
	CShader* getShader(const char* szName);
	virtual void commond(const char* szCommond) = 0;
protected:
	std::map<std::string,CShader*>	m_mapShaders;
};