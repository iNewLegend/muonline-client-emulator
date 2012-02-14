#include "D3D9RenderSystem.h"
#include "D3D9RenderSystemCommon.h"

#include "D3D9Shader.h"
#include "D3D9Texture.h"
#include "D3D9RenderWindow.h"

#include "D3D9HardwareVertexBuffer.h"
#include "D3D9HardwareIndexBuffer.h"
#include "D3D9VertexDeclaration.h"

CRenderSystem* WINAPI RENDER_SYSTEM_PLUG_CREATE_SYSTEM()
{
	return &GetD3D9RenderSystem();
}

CD3D9RenderSystem& GetD3D9RenderSystem()
{
	static CD3D9RenderSystem g_RenderSystem;
	return g_RenderSystem;
}

#define D3DRENDERSTATETYPE_NUM 210
#define D3DSAMPLERSTATETYPE_NUM 14
#define D3DTEXTURESTAGESTATETYPE_NUM 33

CD3D9RenderSystem::CD3D9RenderSystem()
	:m_TextureMgr(this)
	,m_pD3D9Device(NULL)
{
}

CD3D9RenderSystem::~CD3D9RenderSystem()
{
}

CTextureMgr& CD3D9RenderSystem::GetTextureMgr()
{
	return m_TextureMgr;
}

CHardwareBufferMgr& CD3D9RenderSystem::GetHardwareBufferMgr()
{
	return m_D3D9HardwareBufferMgr;
}

CRenderWindow* CD3D9RenderSystem::CreateRenderWindow(WNDPROC pWndProc, const std::wstring& strWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen)
{
	CD3D9RenderWindow* pD3D9RenderWindow = new CD3D9RenderWindow;
	pD3D9RenderWindow->Create(pWndProc, strWindowTitle, nWidth, nHeight,bFullScreen);
	// ----
	return pD3D9RenderWindow;
}

//检查显卡能力
HRESULT CheckResourceFormatSupport(IDirect3DDevice9* pd3dDevice, unsigned long fmt, D3DRESOURCETYPE resType, unsigned long dwUsage)
{
	HRESULT hr = S_OK;
	IDirect3D9* tempD3D = NULL;
	pd3dDevice->GetDirect3D(&tempD3D);
	D3DCAPS9 devCaps;
	pd3dDevice->GetDeviceCaps(&devCaps);

	D3DDISPLAYMODE displayMode;
	tempD3D->GetAdapterDisplayMode(devCaps.AdapterOrdinal, &displayMode);

	hr = tempD3D->CheckDeviceFormat(devCaps.AdapterOrdinal, devCaps.DeviceType, displayMode.Format, dwUsage, resType, static_cast<D3DFORMAT>(fmt));

	tempD3D->Release(), tempD3D = NULL;

	return hr;
}


HRESULT CD3D9RenderSystem::OnResetDevice()
{
	m_pD3D9Device = DXUTGetD3DDevice();
	//HRESULT hr = S_OK;
	// 备份清为0

	// ----
	// # Reset All Texture
	// ----
	myTransform(m_TextureMgr.getTextureList(),	&CD3D9Texture::OnResetDevice);
	// ----
	// # Reset All Hardware Buffer
	// ----
	myTransform(m_D3D9HardwareBufferMgr.m_setVertexHardwareBuffer,	&CD3D9HardwareVertexBuffer::recreateIfDefaultPool);
	myTransform(m_D3D9HardwareBufferMgr.m_setIndexHardwareBuffer,	&CD3D9HardwareIndexBuffer::recreateIfDefaultPool);
	// ----
	// # Reset All Shader
	// ----
	myMapTransform(m_mapShaders, &CD3D9Shader::OnResetDevice);

	m_pOldShader = NULL;

//////////////////////////////////////////////////////////////////////////
	SetStencilFunc(false);
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILREF,       0x0) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILMASK,      0xffffffff) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP) );

	// Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
	// shader debugger. Debugging vertex shaders requires either REF or software vertex 
	// processing, and debugging pixel shaders requires REF.  The 
	// D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
	// shader debugger.  It enables source level debugging, prevents instruction 
	// reordering, prevents dead code elimination, and forces the compiler to compile 
	// against the next higher available software target, which ensures that the 
	// unoptimized shaders do not exceed the shader model limitations.  Setting these 
	// flags will cause slower rendering since the shaders will be unoptimized and 
	// forced into software.  See the DirectX documentation for more information about 
	// using the shader debugger.
//	unsigned long dwShaderFlags = D3DXFX_NOT_CLONEABLE;
//#ifdef DEBUG_VS
//	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
//#endif
//#ifdef DEBUG_PS
//	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
//#endif

	//if(Caps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
	//else

	// If 2-sided stencil is supported, use it.
	//if(Caps.StencilCaps & D3DSTENCILCAPS_TWOSIDED)
	//{
	//	//ShowShadowVolume2Sided
	//}
	//else
	//{
	//	//ShowShadowVolume
	//}

	D3D9HR( m_pD3D9Device->LightEnable(0, false) );

	// 设置默认的顶点采样器状态
	SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	SetSamplerFilter(1, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	SetSamplerFilter(2, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);

	//r->SetRenderStateFV(D3DRS_POINTSIZE_MAX, maxSize);
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_POINTSIZE_MIN, static_cast<unsigned long>(0.0f)) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_POINTSCALE_A, static_cast<unsigned long>(0.0f)) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_POINTSCALE_B, static_cast<unsigned long>(0.0f)) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_POINTSCALE_C, static_cast<unsigned long>(1.0f)) );

	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_SPECULARENABLE, false) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_LOCALVIEWER, true) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_NORMALIZENORMALS, true) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_LOCALVIEWER, false) );
	
	unsigned long zFormat = D3DFMT_D16;//D3DFMT_D24S8;
	//m_bitDepth = 24;

	if(FAILED(CheckResourceFormatSupport(m_pD3D9Device, D3DFMT_X8R8G8B8, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_SRGBREAD)))
	{
		MessageBoxW(NULL, L"Device does not support hardware D3DRTYPE_TEXTURE  D3DUSAGE_QUERY_SRGBREAD!", L"ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
	}
	if(FAILED(CheckResourceFormatSupport(m_pD3D9Device, D3DFMT_X8R8G8B8, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_SRGBWRITE)))
	{
		MessageBoxW(NULL, L"Device does not support hardware D3DRTYPE_TEXTURE  D3DUSAGE_QUERY_SRGBWRITE!", L"ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
	}	
	if(FAILED(CheckResourceFormatSupport(m_pD3D9Device, D3DFMT_D24S8, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL)))
	{
		::MessageBoxW(NULL, L"Device does not support hardware D3DRTYPE_TEXTURE  D3DUSAGE_DEPTHSTENCIL!", L"ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
		return E_FAIL;
	}

	//SetRenderState(D3DSAMP_SRGBTEXTURE, 1);
	//SetRenderState(D3DRS_SRGBWRITEENABLE, 1);

	m_pD3D9Device->SetRenderState(D3DRS_LIGHTING, false);

	SetSamplerFilter(0, TEXF_POINT, TEXF_POINT, TEXF_POINT);
	SetSamplerFilter(1, TEXF_POINT, TEXF_POINT, TEXF_POINT);
	SetSamplerFilter(2, TEXF_POINT, TEXF_POINT, TEXF_POINT);

	//
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, false) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FOGENABLE, false) );

	SetFillMode(FILL_SOLID);

	SetSamplerAddressUV(0,ADDRESS_WRAP,ADDRESS_WRAP);
	SetSamplerAddressUV(1,ADDRESS_WRAP,ADDRESS_WRAP);

	D3D9HR( m_pD3D9Device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE) );
	D3D9HR( m_pD3D9Device->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE) );

	//SetVertexShader(NULL);
	//SetPixelShader(NULL);

	return S_OK;
}

void CD3D9RenderSystem::OnLostDevice()
{
	for (int i=0; i<8; ++i)
	{
		D3D9HR( m_pD3D9Device->SetTexture(i, NULL) );
		D3D9HR( m_pD3D9Device->SetStreamSource(i,NULL,0,0) );
	}
	D3D9HR( m_pD3D9Device->SetIndices(NULL) );

	// ----
	// # Lost All Texture
	// ----
	myTransform(m_TextureMgr.getTextureList(),	&CD3D9Texture::OnLostDevice);
	// ----
	// # Lost All Hardware Buffer
	// ----
	myTransform(m_D3D9HardwareBufferMgr.m_setVertexHardwareBuffer,	&CD3D9HardwareVertexBuffer::releaseIfDefaultPool);
	myTransform(m_D3D9HardwareBufferMgr.m_setIndexHardwareBuffer,	&CD3D9HardwareIndexBuffer::releaseIfDefaultPool);
	// ----
	// # Lost All Shader
	// ----
	myMapTransform(m_mapShaders, &CD3D9Shader::OnLostDevice);
}

void CD3D9RenderSystem::OnDestroyDevice()
{
	// ----
	// # Destroy All Texture
	// ----
	myTransform(m_TextureMgr.getTextureList(),	&CD3D9Texture::OnDestroyDevice);
	// ----
	// # Destroy All Hardware Buffer
	// ----
	myTransform(m_D3D9HardwareBufferMgr.m_setVertexHardwareBuffer,	&CD3D9HardwareVertexBuffer::releaseBuffer);
	myTransform(m_D3D9HardwareBufferMgr.m_setIndexHardwareBuffer,	&CD3D9HardwareIndexBuffer::releaseBuffer);
	// ----
	// # Destroy All Shader
	// ----
	myMapTransform(m_mapShaders, &CD3D9Shader::OnDestroyDevice);
}

CTexture* CD3D9RenderSystem::GetRenderTarget()
{
	CD3D9Texture* pRenderTarget = (CD3D9Texture*)GetTextureMgr().createTexture();
	if (pRenderTarget)
	{
		D3D9HR( m_pD3D9Device->GetRenderTarget(0 , &pRenderTarget->m_pD3D9Surface) );
	}
	return pRenderTarget;
}

void CD3D9RenderSystem::SetRenderTarget(int index, CTexture* pRenderTarget)
{
	LPDIRECT3DSURFACE9 pD3D9Surface = NULL;
	if (pRenderTarget)
	{
		pD3D9Surface = ((CD3D9Texture*)pRenderTarget)->GetD3D9Surface();
	}
	D3D9HR( m_pD3D9Device->SetRenderTarget(index, pD3D9Surface) );
}

CTexture* CD3D9RenderSystem::GetDepthStencil()
{
	CD3D9Texture* pDepthStencil = (CD3D9Texture*)GetTextureMgr().createTexture();
	if (pDepthStencil)
	{
		D3D9HR( m_pD3D9Device->GetDepthStencilSurface(&pDepthStencil->m_pD3D9Surface) );
	}
	return pDepthStencil;
}

void CD3D9RenderSystem::SetDepthStencil(CTexture* pDepthStencil)
{
	LPDIRECT3DSURFACE9 pD3D9Surface = NULL;
	if (pDepthStencil)
	{
		pD3D9Surface = ((CD3D9Texture*)pDepthStencil)->GetD3D9Surface();
	}
	D3D9HR( m_pD3D9Device->SetDepthStencilSurface(pD3D9Surface) );
}

CTexture* CD3D9RenderSystem::newTexture()
{
	CTexture* pTexture = new CD3D9Texture;
	return pTexture;
}

CShader* CD3D9RenderSystem::newShader()
{
	CShader* pShader = new CD3D9Shader;
	return pShader;
}

CHardwareVertexBuffer* CD3D9RenderSystem::newHardwareVertexBuffer()
{
	CHardwareVertexBuffer* pBuffer = new CD3D9HardwareVertexBuffer;
	return pBuffer;
}

CHardwareIndexBuffer* CD3D9RenderSystem::newHardwareIndexBuffer()
{
	CHardwareIndexBuffer* pBuffer = new CD3D9HardwareIndexBuffer;
	return pBuffer;
}

bool CD3D9RenderSystem::BeginFrame()
{
	if(D3D9HR( m_pD3D9Device->BeginScene() ))
	{
		return true;
	}
	return false;
}

void CD3D9RenderSystem::EndFrame()
{
	D3D9HR( m_pD3D9Device->EndScene() );
}

void CD3D9RenderSystem::setViewport(const CRect<int>& rect)
{
	D3DVIEWPORT9 vp;
	vp.X = rect.left;
	vp.Y = rect.top;
	vp.Width = rect.getWidth();
	vp.Height = rect.getHeight();
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;
	D3D9HR( m_pD3D9Device->SetViewport(&vp) );
}

void CD3D9RenderSystem::getViewport(CRect<int>& rect)
{
	D3DVIEWPORT9 vp;
	D3D9HR( m_pD3D9Device->GetViewport(&vp) );
	rect.set(vp.X,vp.Y,vp.X+vp.Width,vp.Y+vp.Height);
}

void CD3D9RenderSystem::ClearBuffer (bool bZBuffer, bool bTarget, Color32 color)
{
	unsigned long dwFlags = D3DCLEAR_STENCIL;
	SetStencilFunc(false);
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILREF,       0x1) );
	//SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
	//SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	//SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP);
	//SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP);
	if (bZBuffer)
		dwFlags |= D3DCLEAR_ZBUFFER;
	if (bTarget)
		dwFlags |= D3DCLEAR_TARGET;
	D3DRECT rect;
	{
		D3DVIEWPORT9 vp;
		D3D9HR( m_pD3D9Device->GetViewport(&vp) );
		rect.x1 = vp.X;
		rect.y1 = vp.Y;
		rect.x2 = vp.Width + rect.x1;
		rect.y2 = vp.Height + rect.y1;
	}
	D3D9HR( m_pD3D9Device->Clear(1, &rect, dwFlags, color.c, 1.0f, 0 ));
}

void CD3D9RenderSystem::SetFillMode(FillMode mode)
{
	D3DFILLMODE D3DMode;
	switch(mode)
	{
	case D3DFILL_POINT:D3DMode = D3DFILL_POINT;
		break;
	case D3DFILL_WIREFRAME:D3DMode = D3DFILL_WIREFRAME;
		break;
	case FILL_SOLID:D3DMode = D3DFILL_SOLID;
		break;
	}
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FILLMODE, D3DMode) );
}

void CD3D9RenderSystem::setWorldMatrix(const Matrix& m)
{
	m_mWorld = m;
	Matrix wvpm = m_mProjection * m_mView * m_mWorld;
	Matrix wvm = m_mView * m_mWorld;
	setShaderMatrix("wvm",wvm);
	setShaderMatrix("wvpm",wvpm);
	setShaderMatrix("g_mWorld",m);
	if (m_pOldShader)
	{
		((CD3D9Shader*)m_pOldShader)->getD3DXEffect()->CommitChanges();
	}
	Matrix mDx=m;mDx.transpose();
	D3D9HR( m_pD3D9Device->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&mDx) );
}

void CD3D9RenderSystem::setViewMatrix(const Matrix& m)
{
	m_mView = m;
	Matrix vpm = m_mProjection * m_mView;
	setShaderMatrix("vpm", vpm);
	setShaderMatrix("vm", m);
	if (m_pOldShader)
	{
		((CD3D9Shader*)m_pOldShader)->getD3DXEffect()->CommitChanges();
	}
	Matrix mDx=m;mDx.transpose();
	D3D9HR( m_pD3D9Device->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&mDx) );
}

void CD3D9RenderSystem::setProjectionMatrix(const Matrix& m)
{
	m_mProjection = m;
	Matrix mDx=m;mDx.transpose();
	D3D9HR( m_pD3D9Device->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&mDx) );
}

const Matrix& CD3D9RenderSystem::getWorldMatrix(Matrix& m)const
{
	return m_mWorld;
}

const Matrix& CD3D9RenderSystem::getViewMatrix(Matrix& m)const
{
	return m_mView;
}

const Matrix& CD3D9RenderSystem::getProjectionMatrix(Matrix& m)const
{
	return m_mProjection;
}

inline unsigned long CompareFunctionForD3D9(CompareFunction cmpFunc)
{
	switch(cmpFunc)
	{
	case CMPF_ALWAYS_FAIL:
		return D3DCMP_NEVER;
		break;
	case CMPF_ALWAYS_PASS:
		return D3DCMP_ALWAYS;
		break;
	case CMPF_LESS:
		return D3DCMP_LESS;
		break;
	case CMPF_LESS_EQUAL:
		return D3DCMP_LESSEQUAL;
		break;
	case CMPF_EQUAL:
		return D3DCMP_EQUAL;
		break;
	case CMPF_NOT_EQUAL:
		return D3DCMP_NOTEQUAL;
		break;
	case CMPF_GREATER_EQUAL:
		return D3DCMP_GREATEREQUAL;
		break;
	case CMPF_GREATER:
		return D3DCMP_GREATER;
		break;
	default:
		return D3DCMP_LESSEQUAL;
	}
}

void CD3D9RenderSystem::SetStencilFunc(bool bStencil, StencilOP op, CompareFunction stencilFunction)
{
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILENABLE, bStencil) );
	if (bStencil)
	{
		D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILFUNC, CompareFunctionForD3D9(stencilFunction)) );
		D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP(op)) );
	}
}

void CD3D9RenderSystem::setShaderFloat(const char* szName, float val)
{
	m_mapShaders.begin()->second->setFloat(szName, val);
}

void CD3D9RenderSystem::setShaderFloatArray(const char* szName, const void* pVal, int nCount)
{
	m_mapShaders.begin()->second->setFloatArray(szName, (const float*)pVal, nCount);
}

void CD3D9RenderSystem::setShaderMatrix(const char* szName, const Matrix& mat)
{
	m_mapShaders.begin()->second->setMatrix(szName, mat);
}

void CD3D9RenderSystem::SetPixelShaderConstantF(unsigned int StartRegister,const float* pConstantData,unsigned int Vector4fCount)
{
	D3D9HR( m_pD3D9Device->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount) );
}

inline D3DTEXTUREFILTERTYPE TextureFilterTypeForD3D9(TextureFilterType filter)
{
	switch(filter)
	{
	case TEXF_NONE:
		return D3DTEXF_NONE;
		break;
	case TEXF_POINT:
		return D3DTEXF_POINT;
		break;
	case TEXF_LINEAR:
		return D3DTEXF_LINEAR;
		break;
	case TEXF_ANISOTROPIC:
		return D3DTEXF_ANISOTROPIC;
		break;
	case TEXF_PYRAMIDALQUAD:
		return D3DTEXF_PYRAMIDALQUAD;
		break;
	case TEXF_GAUSSIANQUAD:
		return D3DTEXF_GAUSSIANQUAD;
		break;
	default:
		return D3DTEXF_NONE;
	}
}

void CD3D9RenderSystem::SetSamplerFilter(size_t unit, TextureFilterType MagFilter, TextureFilterType MinFilter, TextureFilterType MipFilter)
{
	D3D9HR( m_pD3D9Device->SetSamplerState(unit, D3DSAMP_MAGFILTER, TextureFilterTypeForD3D9(MagFilter)) );
	D3D9HR( m_pD3D9Device->SetSamplerState(unit, D3DSAMP_MINFILTER, TextureFilterTypeForD3D9(MinFilter)) );
	D3D9HR( m_pD3D9Device->SetSamplerState(unit, D3DSAMP_MIPFILTER, TextureFilterTypeForD3D9(MipFilter)) );
}

inline unsigned long AddressUVForD3D9(AddressUV addressUV)
{
	switch(addressUV)
	{
	case ADDRESS_WRAP:
		return D3DTADDRESS_WRAP;
		break;
	case ADDRESS_MIRROR:
		return D3DTADDRESS_MIRROR;
		break;
	case ADDRESS_CLAMP:
		return D3DTADDRESS_CLAMP;
		break;
	case ADDRESS_BORDER:
		return D3DTADDRESS_BORDER;
		break;
	case ADDRESS_MIRRORONCE:
		return D3DTADDRESS_MIRRORONCE;
		break;
	default:
		return D3DTADDRESS_WRAP;
	}
}

void CD3D9RenderSystem::SetSamplerAddressUV(size_t unit, AddressUV addressU, AddressUV addressV)
{
	D3D9HR( m_pD3D9Device->SetSamplerState(unit, D3DSAMP_ADDRESSU, AddressUVForD3D9(addressU)) );
	D3D9HR( m_pD3D9Device->SetSamplerState(unit, D3DSAMP_ADDRESSV, AddressUVForD3D9(addressV)) );
}

void CD3D9RenderSystem::SetTexture(unsigned long Stage, unsigned long TextureID)
{
	CTexture* pTexture = GetTextureMgr().getLoadedTexture(TextureID);
	SetTexture(Stage, pTexture);
}

void CD3D9RenderSystem::SetTexture(unsigned long Stage, const CTexture* pTexture)
{
	IDirect3DTexture9* pD3D9Texture = NULL;
	if (pTexture)
	{
		pD3D9Texture = static_cast<const CD3D9Texture*>(pTexture)->GetD3D9Texture();
	}
	SetTexture(Stage, pD3D9Texture);
}

void CD3D9RenderSystem::SetTexture(unsigned long Stage, IDirect3DTexture9* pD3D9Texture)
{
 	if (m_TextureBack[Stage]!=pD3D9Texture)
 	{
 		m_TextureBack[Stage]=pD3D9Texture;
 		D3D9HR( m_pD3D9Device->SetTexture(Stage, pD3D9Texture) );
 	}
// 	IDirect3DTexture9* pOldD3D9Texture = NULL;
// 	D3D9HR( m_pD3D9Device->GetTexture(Stage,(IDirect3DBaseTexture9**) &pOldD3D9Texture) );
// 	if (pOldD3D9Texture != pD3D9Texture)
// 	{
// 		D3D9HR( m_pD3D9Device->SetTexture(Stage, pD3D9Texture) );
// 	}	
// 	S_REL(pOldD3D9Texture);
}

CVertexDeclaration* CD3D9RenderSystem::CreateVertexDeclaration()
{
	CD3D9VertexDeclaration* pD3D9VertexDeclaration = new CD3D9VertexDeclaration(m_pD3D9Device);
	return pD3D9VertexDeclaration;
}

//void CD3D9RenderSystem::SetVertexShader(IDirect3DVertexShader9* pShader)
//{
//	m_pD3D9Device->SetVertexShader(pShader);
//}
//
//void CD3D9RenderSystem::SetPixelShader(IDirect3DPixelShader9* pShader)
//{
//	m_pD3D9Device->SetPixelShader(pShader);
//}

void CD3D9RenderSystem::SetShader(CShader* pShader)
{
	if (m_pOldShader!=pShader)
	{
		if (m_pOldShader)
		{
			m_pOldShader->end();
		}
		if (pShader)
		{
			pShader->begin("Render");
		}
		m_pOldShader=pShader;
	}
}

void CD3D9RenderSystem::SetShader(const char* szShader)
{
	SetShader(getShader(szShader));
}

void CD3D9RenderSystem::SetFVF(unsigned long FVF)
{
	DWORD dwOldFVF = 0;
	D3D9HR( m_pD3D9Device->GetFVF(&dwOldFVF) );
	if (FVF != dwOldFVF)
	{
		D3D9HR( m_pD3D9Device->SetFVF(FVF) );
	}
}

void CD3D9RenderSystem::SetVertexDeclaration(CVertexDeclaration* pDecl)
{
	D3D9HR( m_pD3D9Device->SetVertexDeclaration(((CD3D9VertexDeclaration*)pDecl)->GetD3D9Decl()) );
}

void CD3D9RenderSystem::SetStreamSource(unsigned long StreamNumber,CHardwareVertexBuffer* pStreamData,unsigned long OffsetInBytes,unsigned long Stride)
{
	IDirect3DVertexBuffer9* pOldStreamData;
	unsigned int uOldOffsetInBytes;
	unsigned int uOldStride;
	IDirect3DVertexBuffer9* pD3DStreamData = ((CD3D9HardwareVertexBuffer*)pStreamData)->getD3D9VertexBuffer();
	D3D9HR( m_pD3D9Device->GetStreamSource(StreamNumber,&pOldStreamData,&uOldOffsetInBytes,&uOldStride) );
	if (pOldStreamData!=pD3DStreamData || uOldOffsetInBytes!=OffsetInBytes || uOldStride!=Stride)
	{
		D3D9HR( m_pD3D9Device->SetStreamSource(StreamNumber,pD3DStreamData,OffsetInBytes,Stride) );
	}
	S_REL(pOldStreamData);
}

void CD3D9RenderSystem::SetIndices(CHardwareIndexBuffer* pIndexData)
{
	IDirect3DIndexBuffer9* m_pChangeIB = ((CD3D9HardwareIndexBuffer*)pIndexData)->getD3DIndexBuffer();

	IDirect3DIndexBuffer9* pOldIB = NULL;
	D3D9HR( m_pD3D9Device->GetIndices(&pOldIB) );
	if (pOldIB!=m_pChangeIB)
	{
		D3D9HR( m_pD3D9Device->SetIndices(m_pChangeIB) );
	}
	S_REL(pOldIB);
}

inline D3DPRIMITIVETYPE VertexRenderOperationTypeForD3D9(VertexRenderOperationType PrimitiveType)
{
	switch(PrimitiveType)
	{
	case VROT_POINT_LIST:		return D3DPT_POINTLIST;
	case VROT_LINE_LIST:		return D3DPT_LINELIST;
	case VROT_LINE_STRIP:		return D3DPT_LINESTRIP;
	case VROT_TRIANGLE_LIST:	return D3DPT_TRIANGLELIST;
	case VROT_TRIANGLE_STRIP:	return D3DPT_TRIANGLESTRIP;
	case VROT_TRIANGLE_FAN:		return D3DPT_TRIANGLEFAN;
	default:					return D3DPT_TRIANGLELIST;
	}
}

void CD3D9RenderSystem::DrawPrimitive(VertexRenderOperationType PrimitiveType,unsigned long StartVertex,unsigned long PrimitiveCount)
{
	D3D9HR( m_pD3D9Device->DrawPrimitive(VertexRenderOperationTypeForD3D9(PrimitiveType),StartVertex,PrimitiveCount) );
}

void CD3D9RenderSystem::DrawIndexedPrimitive(VertexRenderOperationType PrimitiveType,int32 BaseVertexIndex,unsigned long MinVertexIndex,unsigned long NumVertices,unsigned long startIndex,unsigned long primCount)
{
	D3D9HR( m_pD3D9Device->DrawIndexedPrimitive(VertexRenderOperationTypeForD3D9(PrimitiveType), BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount) );
}

void CD3D9RenderSystem::DrawPrimitiveUP(VertexRenderOperationType PrimitiveType,unsigned long PrimitiveCount,const void* pVertexStreamZeroData,unsigned long VertexStreamZeroStride)
{
	D3D9HR( m_pD3D9Device->DrawPrimitiveUP(VertexRenderOperationTypeForD3D9(PrimitiveType), PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride) );
}

void CD3D9RenderSystem::DrawIndexedPrimitiveUP(VertexRenderOperationType PrimitiveType,unsigned long MinVertexIndex,unsigned long NumVertices,unsigned long PrimitiveCount,const void* pIndexData,const void* pVertexStreamZeroData,unsigned long VertexStreamZeroStride)
{
	D3D9HR( m_pD3D9Device->DrawIndexedPrimitiveUP(VertexRenderOperationTypeForD3D9(PrimitiveType), MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, D3DFMT_INDEX16, pVertexStreamZeroData, VertexStreamZeroStride) );
}

void CD3D9RenderSystem::drawIndexedSubset(const IndexedSubset& subset)
{
	D3D9HR( m_pD3D9Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, subset.vbase, subset.vstart, subset.vcount, subset.istart, subset.icount/3) );
}

void CD3D9RenderSystem::StretchRect(CTexture* pSourceTexture,const CRect<int>* pSourceRect,CTexture* pDestTexture,const CRect<int>* pDestRect,TextureFilterType filter)
{
	LPDIRECT3DSURFACE9 pD3D9SourceSurface = NULL;
	if (pSourceTexture)
	{
		pD3D9SourceSurface = ((CD3D9Texture*)pSourceTexture)->GetD3D9Surface();
	}
	LPDIRECT3DSURFACE9 pD3D9DestSurface = NULL;
	if (pDestTexture)
	{
		pD3D9DestSurface = ((CD3D9Texture*)pDestTexture)->GetD3D9Surface();
	}
	D3D9HR( m_pD3D9Device->StretchRect(pD3D9SourceSurface, pSourceRect==NULL?NULL:&pSourceRect->getRECT(), pD3D9DestSurface, pDestRect==NULL?NULL:&pDestRect->getRECT(), TextureFilterTypeForD3D9(filter)) );
}

//LPDIRECT3DVERTEXBUFFER9 CreateVertexBuffer(size_t vertexSize, size_t numVerts, HardwareBuffer::Usage usage, bool useShadowBuffer = false)
//{
//	LPDIRECT3DVERTEXBUFFER9 lpD3DBuffer;
//	HRESULT hr =  m_pD3D9Device->CreateVertexBuffer(
//		static_cast<unsigned long>(mSizeInBytes), 
//		0, 
//		0, // No FVF here, thankyou
//		mD3DPool,
//		&lpD3DBuffer,
//		NULL);
//}

//void CD3D9RenderSystem::SetLight(unsigned long Index, const D3DLIGHT9* Light)
//{
//	m_pD3D9Device->SetLight(Index, Light);
//
//	//if (m_ShaderMgr.IsEnable())
//	//{
//	//	D3DXVECTOR3 vLightDir;
//	//	D3DXVec3Normalize(&vLightDir, (D3DXVECTOR3*)&Light->Direction);
//	//	m_ShaderMgr.SetValue(PT_LIGHTDIR, &vLightDir);
//	//	D3DXVECTOR3 vUp(0,1,0);
//	//	D3DXVECTOR3 vEyePt(0,0,0);
//	//	D3DXMATRIX mView,mProj;
//	//	D3DXMatrixLookAtLH(&mView, &vEyePt, &vLightDir, &vUp);
//	//	D3DXMatrixOrthoLH(&mProj, 8, 8, -512, 512);
//	//	D3DXMatrixMultiply(&mView, &mView, &mProj);
//	//	m_ShaderMgr.SetValue(PT_LIGHT_VIEW, &mView);
//	//}
//}

void CD3D9RenderSystem::commond(const char* szCommond)
{
	if (strcmp(szCommond,"resetshader")==0)
	{
		myMapTransform(m_mapShaders, &CD3D9Shader::OnResetDevice);
	}
}
