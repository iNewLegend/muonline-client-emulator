#pragma once
#include "Shader.h"
#include "RenderSystemCommon.h"
#include "TextureMgr.h"
#include "MaterialMgr.h"
#include "RenderWindow.h"
#include "HardwareVertexBuffer.h"
#include "HardwareIndexBuffer.h"
#include "HardwareBufferMgr.h"
#include "ShaderMgr.h"
#include "VertexDeclaration.h"
#include "Pos2D.h"
#include "Rect.h"

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
public:
	virtual CTextureMgr&		GetTextureMgr() = 0;
	virtual CMaterialMgr&		getMaterialMgr();
	virtual CHardwareBufferMgr&	GetHardwareBufferMgr() = 0;
	virtual CShaderMgr&			GetShaderMgr() = 0;
	virtual CRenderWindow*		CreateRenderWindow(WNDPROC pWndProc, const std::wstring& strWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen = false) = 0;
	// ----
	// # new
	// ----
	virtual CTexture*	newTexture			() = 0;
	virtual CShader*	newShader			() = 0;
	virtual CHardwareVertexBuffer*	newHardwareVertexBuffer	() = 0;
	virtual CHardwareIndexBuffer*	newHardwareIndexBuffer	() = 0;
	// ----
	virtual void		OnFrameMove			() = 0;
	// ----
	virtual bool		BeginFrame()		= 0;	// 帧渲染开始
	virtual void		EndFrame()			= 0;	// 帧渲染结束
	// ----
	virtual void		ClearBuffer			(bool bZBuffer, bool bTarget, Color32 color) = 0;
	// ----
	virtual CTexture*	GetRenderTarget		() = 0;
	virtual CTexture*	GetDepthStencil		() = 0;
	// ----
	virtual void		SetRenderTarget		(CTexture* pRenderTarget)	= 0;
	virtual void		SetDepthStencil		(CTexture* pDepthStencil)	= 0;
	// ----
	virtual void		setViewport			(const CRect<int>& rect)	= 0;
	virtual void		getViewport			(CRect<int>& rect)			= 0;
	// ----
	// # 填充模式
	// ----
	virtual void		SetFillMode			(FillMode mode) = 0;
	// ----
	// # set matrix
	// ----
	virtual void		setWorldMatrix		(const Matrix& m) = 0;
	virtual void		setViewMatrix		(const Matrix& m) = 0;
	virtual void		setProjectionMatrix	(const Matrix& m) = 0;
	virtual void		setTextureMatrix	(unsigned char uTexChannel, TextureTransformFlag flag, const Matrix& m = Matrix::ZERO) = 0;
	// ----
	// # get matrix
	// ----
	virtual void		getWorldMatrix		(Matrix& m)const = 0;
	virtual void		getViewMatrix		(Matrix& m)const = 0;
	virtual void		getProjectionMatrix	(Matrix& m)const = 0;
	virtual void		getTextureMatrix	(unsigned char uTexChannel, Matrix& m)const = 0;
	// ----
	// # Func
	// ----
	virtual void		SetDepthBufferFunc	(bool bDepthTest = true, bool bDepthWrite = true,						// 深度检测
												CompareFunction depthFunction = CMPF_LESS_EQUAL ) = 0;
	virtual void		SetAlphaTestFunc	(bool bAlphaTest = true, CompareFunction func = CMPF_GREATER_EQUAL,		// ALPHA检测
												unsigned char value = 0x80) = 0;
	virtual void		SetBlendFunc		(bool bBlend = true, SceneBlendOperation op = BLENDOP_ADD,				// 混合参数
												SceneBlendFactor src = SBF_SOURCE_ALPHA, SceneBlendFactor dest = SBF_ONE_MINUS_SOURCE_ALPHA) = 0;
	virtual void		SetStencilFunc		(bool bStencil, StencilOP op=STENCILOP_INCR,							// 模板检测
												CompareFunction stencilFunction = CMPF_LESS_EQUAL) = 0;
	// ----
	// # 设置剔除模式
	// ----
	virtual void		SetCullingMode		(CullingMode mode)	= 0;
	// ----
	// # 设置纹理因素颜色
	// ----
	virtual void		SetPixelShaderConstantF(unsigned int StartRegister,const float* pConstantData,unsigned int Vector4fCount)=0;
	virtual void		SetTextureFactor	(Color32 color)		= 0;
	// ----
	// # TextureOP
	// ----
	virtual void		setResultARGToTemp	(size_t unit, bool bResultARGToTemp=true) = 0;
	virtual void		SetTextureColorOP	(size_t unit, TextureBlendOperation op = TBOP_MODULATE,
												TextureBlendSource src1 = TBS_TEXTURE, TextureBlendSource src2 = TBS_DIFFUSE) = 0;
	virtual void		SetTextureAlphaOP	(size_t unit, TextureBlendOperation op,
												TextureBlendSource src1 = TBS_TEXTURE, TextureBlendSource src2 = TBS_DIFFUSE) = 0;
	// ----
	virtual void		SetSamplerFilter	(size_t unit, TextureFilterType MagFilter, TextureFilterType MinFilter, TextureFilterType MipFilter) = 0;
	virtual void		SetSamplerAddressUV	(size_t unit, AddressUV addressU, AddressUV addressV) = 0;
	// ----
	// # 状态初始化 !!!
	// ----
	virtual void SetupRenderState() = 0;
	// ----
	// # 去色 !!!
	// ----
	virtual void SetTextureStageStateDecolor() = 0;
	// ----
	// # 设置shader
	// ----
	virtual void SetShader(CShader* pShader) = 0;
	virtual void SetShader(unsigned long id) = 0;
	// ----
	// # Light
	// ----
	virtual void SetDirectionalLight(unsigned long uIndex,const DirectionalLight& light) = 0;
	virtual void setPointLight(unsigned long uIndex,const PointLight& light) = 0;
	virtual void LightEnable(unsigned long Index, bool bEnable) = 0;
	virtual void SetLightingEnabled(bool bEnable) = 0;
	virtual void SetTexCoordIndex(unsigned long stage, unsigned long index) = 0;
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
	// ----
	virtual void SetMaterial(const Vec4D& vAmbient, const Vec4D& vDiffuse) = 0;
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

	virtual void setFog(const Fog& fog) = 0;
	virtual void setFogEnable(bool bEnable) = 0;

	virtual void StretchRect(CTexture* pSource,const CRect<int>* pSourceRect,CTexture* pDest,const CRect<int>* pDestRect,TextureFilterType filter) = 0;
	//
	void world2Screen(const Vec3D& vWorldPos, Pos2D& posScreen);
	void GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir,int x, int y);

	// set material
	bool prepareMaterial(const char* szMaterialName, float fOpacity=1.0f);
	bool prepareMaterial(CMaterial& material, float fOpacity=1.0f);
	void finishMaterial();
private:
	CMaterialMgr m_MaterialMgr;
};

void SetRenderSystem(CRenderSystem* pRenderSystem);
CRenderSystem& GetRenderSystem();