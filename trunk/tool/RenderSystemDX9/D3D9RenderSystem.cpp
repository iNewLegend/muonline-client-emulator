#include "D3D9RenderSystem.h"
#include "D3D9RenderSystemCommon.h"

//#include "VB.h"
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

CShaderMgr& CD3D9RenderSystem::GetShaderMgr()
{
	return m_ShaderMgr;
}

CRenderWindow* CD3D9RenderSystem::CreateRenderWindow(WNDPROC pWndProc, const std::wstring& strWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen)
{
	CD3D9RenderWindow* pD3D9RenderWindow = new CD3D9RenderWindow;
	pD3D9RenderWindow->Create(pWndProc, strWindowTitle, nWidth, nHeight,bFullScreen);
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
	myMgrTransform(m_ShaderMgr.m_Items, &CD3D9Shader::OnResetDevice);
	// ----
	m_mapChangeTexture.clear();
	m_mapChangeStreamSource.clear();

	//FillMemory(m_Lights, 8*sizeof(D3DLIGHT9), NULL);
	//FillMemory(m_LightEnable, 8*sizeof(bool), NULL);

	m_uChangeFVF = 0;
	m_pChangeIB	= NULL;
	m_pChangeShader= m_pOldShader = NULL;

	m_nShaderID = m_nChangeShaderID = -1;

//////////////////////////////////////////////////////////////////////////
	SetAlphaTestFunc(false);
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

	// 设置默认材质
	SetMaterial(Vec4D(0.2f,0.2f,0.2f,0.2f),Vec4D(0.8f,0.8f,0.8f,0.8f));

	// 设置默认灯光
	DirectionalLight light(Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec3D(-1.0f,-1.0f,-1.0f));

	SetDirectionalLight(0,light);
	LightEnable(0, true);

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

	{
// 		D3DCAPS9 devCaps;
// 		m_pD3D9Device->GetDeviceCaps(&devCaps);
// 		if( !(devCaps.Caps2 & D3DCAPS2_CANCALIBRATEGAMMA) )   
// 		{
// 			MessageBoxW(NULL, L"不支持应用Gamma坡道之前进行校正效果，SetGammaRamp() 不能使用 D3DSGR_CALIBRATE 标志", L"ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
// 		}
	}

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

	commit();
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
	myMgrTransform(m_ShaderMgr.m_Items, &CD3D9Shader::OnLostDevice);
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
	myMgrTransform(m_ShaderMgr.m_Items, &CD3D9Shader::OnDestroyDevice);
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

void CD3D9RenderSystem::SetRenderTarget(CTexture* pRenderTarget)
{
	LPDIRECT3DSURFACE9 pD3D9Surface = NULL;
	if (pRenderTarget)
	{
		pD3D9Surface = ((CD3D9Texture*)pRenderTarget)->GetD3D9Surface();
	}
	D3D9HR( m_pD3D9Device->SetRenderTarget(0, pD3D9Surface) );
	/*HRESULT hr = m_pD3D9Device->SetRenderTarget(0, pD3D9Surface);
	if(FAILED(hr))
	{
		if (D3DERR_INVALIDCALL == hr)
		{
			::MessageBoxA(0, "pRenderTarget = NULL and RenderTargetIndex = 0\npRenderTarget is != NULL and the render target is invalid.", 0, 0);
		}
		else
		{
			::MessageBoxA(0, "NULL", 0, 0);
		}
	}*/
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

void CD3D9RenderSystem::OnFrameMove()
{
	//m_TextureMgr.OnFrameMove();
	//OnResetDevice();
	//FillMemory(m_nTexture, 8*sizeof(IDirect3DTexture9*), NULL);
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
	Matrix mDx=m;mDx.transpose();
	CD3D9Shader* shared = (CD3D9Shader*)m_ShaderMgr.getSharedShader();
	if(shared)
	{
		shared->setMatrix("g_mWorld",m);
		if (m_pOldShader)
		{
			((CD3D9Shader*)m_pOldShader)->getD3DXEffect()->CommitChanges();
		}
	}
	D3D9HR( m_pD3D9Device->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&mDx) );
}

void CD3D9RenderSystem::setViewMatrix(const Matrix& m)
{
	Matrix mDx=m;mDx.transpose();
	D3D9HR( m_pD3D9Device->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&mDx) );
}

void CD3D9RenderSystem::setProjectionMatrix(const Matrix& m)
{
	Matrix mDx=m;mDx.transpose();
	D3D9HR( m_pD3D9Device->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&mDx) );
}

void CD3D9RenderSystem::setTextureMatrix(unsigned char uTexChannel, TextureTransformFlag flag, const Matrix& m)
{
	unsigned long uD3D9Flag = D3DTTFF_DISABLE;
	switch(flag)
	{
	case TTF_COUNT1:
		uD3D9Flag = D3DTTFF_COUNT1;
		break;
	case TTF_COUNT2:
		uD3D9Flag = D3DTTFF_COUNT2;
		break;
	case TTF_COUNT3:
		uD3D9Flag = D3DTTFF_COUNT3;
		break;
	case TTF_COUNT4:
		uD3D9Flag = D3DTTFF_COUNT4;
		break;
	case TTF_PROJECTED:
		uD3D9Flag = D3DTTFF_PROJECTED;
		break;
	}
	D3D9HR( m_pD3D9Device->SetTextureStageState(uTexChannel, D3DTSS_TEXTURETRANSFORMFLAGS, uD3D9Flag) );
	Matrix mDx=m;mDx.transpose();
	m_pD3D9Device->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + uTexChannel), (D3DXMATRIX*)&mDx);
}

void CD3D9RenderSystem::getWorldMatrix(Matrix& m)const
{
	D3D9HR( m_pD3D9Device->GetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m) );
	m.transpose();
}

void CD3D9RenderSystem::getViewMatrix(Matrix& m)const
{
	D3D9HR( m_pD3D9Device->GetTransform(D3DTS_VIEW, (D3DXMATRIX*)&m) );
	m.transpose();
}

void CD3D9RenderSystem::getProjectionMatrix(Matrix& m)const
{
	D3D9HR( m_pD3D9Device->GetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&m) );
	m.transpose();
}

void CD3D9RenderSystem::getTextureMatrix(unsigned char uTexChannel, Matrix& m)const
{
	D3D9HR( m_pD3D9Device->GetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + uTexChannel), (D3DXMATRIX*)&m) );
	m.transpose();
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

void CD3D9RenderSystem::SetDepthBufferFunc(bool bDepthTest, bool bDepthWrite, CompareFunction depthFunction)
{
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_ZENABLE, bDepthTest) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_ZWRITEENABLE, bDepthWrite) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_DEPTHBIAS, CompareFunctionForD3D9(depthFunction)) );
}

void CD3D9RenderSystem::SetAlphaTestFunc(bool bAlphaTest, CompareFunction func, unsigned char value)
{
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_ALPHATESTENABLE, bAlphaTest) );
	if (bAlphaTest)
	{
		D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_ALPHAFUNC, CompareFunctionForD3D9(func)) );
		D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_ALPHAREF, value) );
	}
}

inline unsigned long BlendOperationForD3D9(SceneBlendOperation op)
{
	switch(op)
	{
	case BLENDOP_ADD:
		return D3DBLENDOP_ADD;
		break;
	case BLENDOP_SUBTRACT:
		return D3DBLENDOP_SUBTRACT;
		break;
	case BLENDOP_REVSUBTRACT:
		return D3DBLENDOP_REVSUBTRACT;
		break;
	case BLENDOP_MIN:
		return D3DBLENDOP_MIN;
		break;
	case BLENDOP_MAX:
		return D3DBLENDOP_MAX;
		break;
	default:
		return D3DBLENDOP_ADD;
	}
}

inline unsigned long BlendFactorForD3D9(SceneBlendFactor factor)
{
	// 没有用到的D3D9的混合因素
	//D3DBLEND_SRCALPHASAT        = 11,
	//D3DBLEND_BOTHSRCALPHA       = 12,
	//D3DBLEND_BOTHINVSRCALPHA    = 13,
	//D3DBLEND_BLENDFACTOR        = 14, /* Only supported if D3DPBLENDCAPS_BLENDFACTOR is on */
	//D3DBLEND_INVBLENDFACTOR     = 15, /* Only supported if D3DPBLENDCAPS_BLENDFACTOR is on */
	switch(factor)
	{
	case SBF_ONE:
		return D3DBLEND_ONE;
		break;
	case SBF_ZERO:
		return D3DBLEND_ZERO;
		break;
	case SBF_DEST_COLOUR:
		return D3DBLEND_DESTCOLOR;
		break;
	case SBF_SOURCE_COLOUR:
		return D3DBLEND_SRCCOLOR;
		break;
	case SBF_ONE_MINUS_DEST_COLOUR:
		return D3DBLEND_INVDESTCOLOR;
		break;
	case SBF_ONE_MINUS_SOURCE_COLOUR:
		return D3DBLEND_INVSRCCOLOR;
		break;
	case SBF_DEST_ALPHA:
		return D3DBLEND_DESTALPHA;
		break;
	case SBF_SOURCE_ALPHA:
		return D3DBLEND_SRCALPHA;
		break;
	case SBF_ONE_MINUS_DEST_ALPHA:
		return D3DBLEND_INVDESTALPHA;
		break;
	case SBF_ONE_MINUS_SOURCE_ALPHA:
		return D3DBLEND_INVSRCALPHA;
		break;
	default:
		return D3DBLEND_ONE;
	}
}

void CD3D9RenderSystem::SetBlendFunc(bool bBlend, SceneBlendOperation op, SceneBlendFactor src, SceneBlendFactor dest)
{
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_ALPHABLENDENABLE, bBlend) );
	if (bBlend)
	{
		D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_BLENDOP, BlendOperationForD3D9(op)) );
		D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_SRCBLEND, BlendFactorForD3D9(src)) );
		D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_DESTBLEND, BlendFactorForD3D9(dest)) );
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

void CD3D9RenderSystem::SetCullingMode(CullingMode mode)
{
	unsigned long uCullingMode = D3DCULL_CCW;
	switch(mode)
	{
	case CULL_NONE:
		uCullingMode = D3DCULL_NONE;
		break;
	case CULL_CLOCK_WISE:
		uCullingMode = D3DCULL_CW;
		break;
	case CULL_ANTI_CLOCK_WISE:
		uCullingMode = D3DCULL_CCW;
		break;
	default:
		uCullingMode = D3DCULL_CCW;
	}
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_CULLMODE, uCullingMode) );
}

void CD3D9RenderSystem::SetTextureFactor(Color32 color)
{
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_TEXTUREFACTOR, color.c) );
}

inline unsigned long  TextureBlendOperationForD3D9(TextureBlendOperation op)
{
	//D3DTOP_ADDSIGNED2X          =  9,   // as above but left  1 bit
	//// Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
	//D3DTOP_BLENDTEXTUREALPHAPM  = 15, // texture alpha
	//// Specular mapping
	//D3DTOP_PREMODULATE            = 17,     // modulate with next texture before use
	//D3DTOP_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
	//// COLOROP only
	//D3DTOP_MODULATECOLOR_ADDALPHA = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
	//// COLOROP only
	//D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
	//// COLOROP only
	//D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
	//// COLOROP only

	//// Bump mapping
	//D3DTOP_BUMPENVMAP           = 22, // per pixel env map perturbation
	//D3DTOP_BUMPENVMAPLUMINANCE  = 23, // with luminance channel
	//// Triadic ops
	//D3DTOP_MULTIPLYADD          = 25, // Arg0 + Arg1*Arg2
	switch(op)
	{
	case TBOP_DISABLE:
		return D3DTOP_DISABLE;
		break;
	case TBOP_SOURCE1:
		return D3DTOP_SELECTARG1;
		break;
	case TBOP_SOURCE2:
		return D3DTOP_SELECTARG2;
		break;
	case TBOP_MODULATE:
		return D3DTOP_MODULATE;
		break;
	case TBOP_MODULATE_X2:
		return D3DTOP_MODULATE2X;
		break;
	case TBOP_MODULATE_X4:
		return D3DTOP_MODULATE4X;
		break;
	case TBOP_ADD:
		return D3DTOP_ADD;
		break;
	case TBOP_ADD_SIGNED:
		return D3DTOP_ADDSIGNED;
		break;
	case TBOP_ADD_SMOOTH:
		return D3DTOP_ADDSMOOTH;
		break;
	case TBOP_SUBTRACT:
		return D3DTOP_SUBTRACT;
		break;
	case TBOP_BLEND_DIFFUSE_ALPHA:
		return D3DTOP_BLENDDIFFUSEALPHA;
		break;
	case TBOP_BLEND_TEXTURE_ALPHA:
		return D3DTOP_BLENDTEXTUREALPHA;
		break;
	case TBOP_BLEND_CURRENT_ALPHA:
		return D3DTOP_BLENDCURRENTALPHA;
		break;
	case TBOP_BLEND_MANUAL:
		return D3DTOP_BLENDFACTORALPHA;//?
		break;
	case TBOP_DOTPRODUCT:
		return D3DTOP_DOTPRODUCT3;
		break;
	case TBOP_BLEND_DIFFUSE_COLOUR:
		return D3DTOP_LERP;
		break;
	default:
		return D3DTOP_DISABLE;
	}
}

inline unsigned long TextureBlendSourceForD3D9(TextureBlendSource src)
{
//D3DTA_SELECTMASK        0x0000000f  // mask for arg selector
//D3DTA_TEMP              0x00000005  // select temporary register color (read/write)
//D3DTA_CONSTANT          0x00000006  // select texture stage constant
//D3DTA_COMPLEMENT        0x00000010  // take 1.0 - x (read modifier)
//D3DTA_ALPHAREPLICATE    0x00000020  // replicate alpha to color components (read modifier)
	switch(src)
	{
	case TBS_CURRENT:
		return D3DTA_CURRENT;
		break;
	case TBS_TEXTURE:
		return D3DTA_TEXTURE;
		break;
	case TBS_DIFFUSE:
		return D3DTA_DIFFUSE;
		break;
	case TBS_SPECULAR:
		return D3DTA_SPECULAR;
		break;
	case TBS_MANUAL:
		return D3DTA_TFACTOR;// ?
		break;
	case TBS_TFACTOR:
		return D3DTA_TFACTOR;
		break;
	case TBS_TEMP:
		return D3DTA_TEMP;
		break;
	default:
		return D3DTA_DIFFUSE;
	}
}

void CD3D9RenderSystem::setResultARGToTemp(size_t unit, bool bResultARGToTemp)
{
	D3D9HR( m_pD3D9Device->SetTextureStageState(unit, D3DTSS_RESULTARG, bResultARGToTemp?D3DTA_TEMP:D3DTA_CURRENT));
}

void CD3D9RenderSystem::SetTextureColorOP(size_t unit, TextureBlendOperation op, TextureBlendSource src1, TextureBlendSource src2)
{
	D3D9HR( m_pD3D9Device->SetTextureStageState(unit, D3DTSS_COLOROP, TextureBlendOperationForD3D9(op)) );
	if (TBOP_DISABLE != op)
	{
		D3D9HR( m_pD3D9Device->SetTextureStageState(unit, D3DTSS_COLORARG1, TextureBlendSourceForD3D9(src1)) );
		D3D9HR( m_pD3D9Device->SetTextureStageState(unit, D3DTSS_COLORARG2, TextureBlendSourceForD3D9(src2)) );
	}
}

void CD3D9RenderSystem::SetTextureAlphaOP(size_t unit, TextureBlendOperation op, TextureBlendSource src1, TextureBlendSource src2)
{
	D3D9HR( m_pD3D9Device->SetTextureStageState(unit, D3DTSS_ALPHAOP, TextureBlendOperationForD3D9(op)) );
	if (TBOP_DISABLE != op)
	{
		D3D9HR( m_pD3D9Device->SetTextureStageState(unit, D3DTSS_ALPHAARG1, TextureBlendSourceForD3D9(src1)) );
		D3D9HR( m_pD3D9Device->SetTextureStageState(unit, D3DTSS_ALPHAARG2, TextureBlendSourceForD3D9(src2)) );
	}
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

void CD3D9RenderSystem::SetupRenderState()
{
	SetLightingEnabled(false);
	SetDepthBufferFunc(true, true, CMPF_LESS_EQUAL);
	SetAlphaTestFunc(false);
	SetBlendFunc(true, BLENDOP_ADD,SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
	SetCullingMode(CULL_NONE);

	SetTexCoordIndex(0, 0);
	SetTexCoordIndex(1, 0);

	SetTextureColorOP(0,TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
	SetTextureAlphaOP(0,TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
	SetTextureColorOP(1,TBOP_DISABLE);
	SetTextureAlphaOP(1,TBOP_DISABLE);
	SetTextureColorOP(2,TBOP_DISABLE);
	SetTextureAlphaOP(2,TBOP_DISABLE);

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
}

void CD3D9RenderSystem::SetTextureStageStateDecolor()
{
	//SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	const static unsigned long dwLuminanceConv = ((int)(128+0.2125f*128)<<16)+((int)(128+0.7154f*128)<<8)+(int)(128+0.0721f*128);
	Color32 clrLuminanceConv((int)(0.2125f*255), (int)(0.2125f*255), (int)(0.7154f*255), (int)(0.0721f*255));
	SetTextureFactor(dwLuminanceConv);

	SetTextureColorOP(0,TBOP_DOTPRODUCT, TBS_TEXTURE, TBS_TFACTOR);
	SetTextureAlphaOP(0,TBOP_DISABLE);
	SetTextureColorOP(1,TBOP_MODULATE, TBS_CURRENT, TBS_DIFFUSE);
	SetTextureAlphaOP(1,TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
	//SetTextureAlphaOP(1,TBOP_SOURCE1, TBS_DIFFUSE, TBS_DIFFUSE);
	//SetTexture(1, GetTexture(0));
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
	m_mapChangeTexture[Stage] = pD3D9Texture;
}

CTexture* CD3D9RenderSystem::GetTexture(unsigned long Stage)
{
	return NULL;
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
	m_pChangeShader = pShader;
	if (pShader==NULL)
	{
		if (m_pOldShader)
		{
			m_pOldShader->end();
			m_pOldShader=NULL;
		}
	}

/*	if (m_nShaderID != ShaderID)
	{
		//CShader* pShader= GetShaderMgr().GetShader(ShaderID);
		if (pShader)
		{
			pShader->Begin();
		}
		else
		{
		//	pShader= GetShaderMgr().GetShader(m_nShaderID);
			if (pShader)
			{
				pShader->End();
			}
		}
		m_nShaderID = ShaderID;
	}*/
}

void CD3D9RenderSystem::SetShader(unsigned long id)
{
	SetShader(m_ShaderMgr.getItem(id));
}

void CD3D9RenderSystem::SetFVF(unsigned long FVF)
{
	m_uChangeFVF = FVF;
}

void CD3D9RenderSystem::SetVertexDeclaration(CVertexDeclaration* pDecl)
{
	D3D9HR( m_pD3D9Device->SetVertexDeclaration(((CD3D9VertexDeclaration*)pDecl)->GetD3D9Decl()) );
}

void CD3D9RenderSystem::SetStreamSource(unsigned long StreamNumber,CHardwareVertexBuffer* pStreamData,unsigned long OffsetInBytes,unsigned long Stride)
{
	D3D9StreamSource& d3D9StreamSource = m_mapChangeStreamSource[StreamNumber];
	d3D9StreamSource.pStreamData = ((CD3D9HardwareVertexBuffer*)pStreamData)->getD3D9VertexBuffer();
	d3D9StreamSource.uOffsetInBytes = OffsetInBytes;
	d3D9StreamSource.uStride = Stride;
}

void CD3D9RenderSystem::SetIndices(CHardwareIndexBuffer* pIndexData)
{
	m_pChangeIB = ((CD3D9HardwareIndexBuffer*)pIndexData)->getD3DIndexBuffer();
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
	if (commit())// 检测状态是否正确
	{
		D3D9HR( m_pD3D9Device->DrawPrimitive(VertexRenderOperationTypeForD3D9(PrimitiveType),StartVertex,PrimitiveCount) );
	}
}

void CD3D9RenderSystem::DrawIndexedPrimitive(VertexRenderOperationType PrimitiveType,int32 BaseVertexIndex,unsigned long MinVertexIndex,unsigned long NumVertices,unsigned long startIndex,unsigned long primCount)
{
	if (commit())// 检测状态是否正确
	{
		D3D9HR( m_pD3D9Device->DrawIndexedPrimitive(VertexRenderOperationTypeForD3D9(PrimitiveType), BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount) );
	}
}

void CD3D9RenderSystem::DrawPrimitiveUP(VertexRenderOperationType PrimitiveType,unsigned long PrimitiveCount,const void* pVertexStreamZeroData,unsigned long VertexStreamZeroStride)
{
	if (commit())// 检测状态是否正确
	{
		D3D9HR( m_pD3D9Device->DrawPrimitiveUP(VertexRenderOperationTypeForD3D9(PrimitiveType), PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride) );
	}
}

void CD3D9RenderSystem::DrawIndexedPrimitiveUP(VertexRenderOperationType PrimitiveType,unsigned long MinVertexIndex,unsigned long NumVertices,unsigned long PrimitiveCount,const void* pIndexData,const void* pVertexStreamZeroData,unsigned long VertexStreamZeroStride)
{
	if (commit())// 检测状态是否正确
	{
		D3D9HR( m_pD3D9Device->DrawIndexedPrimitiveUP(VertexRenderOperationTypeForD3D9(PrimitiveType), MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, D3DFMT_INDEX16, pVertexStreamZeroData, VertexStreamZeroStride) );
	}
}

void CD3D9RenderSystem::drawIndexedSubset(const IndexedSubset& subset)
{
	if (commit())// 检测状态是否正确
	{
		D3D9HR( m_pD3D9Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, subset.vbase, subset.vstart, subset.vcount, subset.istart, subset.icount/3) );
	}
}

void CD3D9RenderSystem::setFog(const Fog& fog)
{
	#define FtoDW(x) *((DWORD*)(&x))
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FOGSTART,		FtoDW(fog.fStart)) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FOGEND,			FtoDW(fog.fEnd)) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FOGDENSITY,		FtoDW(fog.fDensity)) );
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FOGCOLOR,		fog.color.c) );

	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FOGVERTEXMODE,	D3DFOG_NONE) );	
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FOGTABLEMODE,	D3DFOG_LINEAR) );
}

void CD3D9RenderSystem::setFogEnable(bool bEnable)
{
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_FOGENABLE,	bEnable) );
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

void CD3D9RenderSystem::SetMaterial(const Vec4D& vAmbient, const Vec4D& vDiffuse)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient	= *(const D3DXCOLOR*)&vAmbient;//D3DXCOLOR(0.2,0.2,0.2,0.2);
	mtrl.Diffuse	= *(const D3DXCOLOR*)&vDiffuse;//D3DXCOLOR(0.8,0.8,0.8,0.8);
	mtrl.Specular	= D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	mtrl.Emissive	= D3DXCOLOR(0,0,0,0);
	mtrl.Power		= 72;
	D3D9HR( m_pD3D9Device->SetMaterial(&mtrl) );
}

void CD3D9RenderSystem::SetDirectionalLight(unsigned long uIndex,const DirectionalLight& light)
{
	D3DLIGHT9 D3D9Light;
	ZeroMemory( &D3D9Light, sizeof(D3D9Light) );
	D3D9Light.Type		= D3DLIGHT_DIRECTIONAL;
	D3D9Light.Ambient	= *(D3DXCOLOR*)(&light.vAmbient);
	D3D9Light.Diffuse	= *(D3DXCOLOR*)(&light.vDiffuse);
	D3D9Light.Specular	= *(D3DXCOLOR*)(&light.vSpecular);
	D3D9Light.Direction = *(D3DXVECTOR3*)(&light.vDirection);
	D3D9HR( m_pD3D9Device->SetLight(uIndex, &D3D9Light) );
}

void CD3D9RenderSystem::setPointLight(unsigned long uIndex,const PointLight& light)
{
	D3DLIGHT9 D3D9Light;
	ZeroMemory( &D3D9Light, sizeof(D3D9Light) );
	D3D9Light.Attenuation1 = 1.0f/light.fRange;
	D3D9Light.Type		= D3DLIGHT_POINT;
	D3D9Light.Ambient	= *(D3DXCOLOR*)(&light.vAmbient);
	D3D9Light.Diffuse	= *(D3DXCOLOR*)(&light.vDiffuse);
	D3D9Light.Specular	= *(D3DXCOLOR*)(&light.vSpecular);
	D3D9Light.Position = *(D3DXVECTOR3*)(&light.vPosition);
	D3D9Light.Range = light.fRange;
	D3D9HR( m_pD3D9Device->SetLight(uIndex, &D3D9Light) );
}

void CD3D9RenderSystem::SetVB(int nVBID)
{
	//CVBPool& VBPool = GetVBPool();
	//VBSub* pSub = (VBSub*)VBPool.GetSub(nVBID);
	//if (pSub)
	//{
	//	CVBChunk* pChunk = (CVBChunk*)VBPool.GetChunk(pSub->dwChunkID);
	//	if (pChunk)
	//	{
	//		SetFVF(pSub->dwFVF);
	//		SetStreamSource(0, (LPDIRECT3DVERTEXBUFFER9)pChunk->m_pBuffer, pSub->dwStart, pSub->dwVertexSize);
	//	}
	//}
}

void SetMesh(int nMeshID)
{
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

//void CD3D9RenderSystem::SetMaterial(D3DMATERIAL9* Mtl)
//{
//	//m_ChangeMaterial = *Mtl;
//	m_pD3D9Device->SetMaterial(Mtl);
//
//	//if (GetShaderMgr().IsEnable())
//	//{
//	//	GetShaderMgr().SetValue(PT_DIFFUSE, &Mtl->Diffuse);
//	//	GetShaderMgr().SetValue(PT_SPECULAR, &Mtl->Specular);
//	//	GetShaderMgr().SetValue(PT_POWER, &Mtl->Power);
//	//}
//}

//void CD3D9RenderSystem::SetLight(unsigned long Index, const D3DLIGHT9* Light)
//{
//	m_pD3D9Device->SetLight(Index, Light);
//
//	//if (GetShaderMgr().IsEnable())
//	//{
//	//	D3DXVECTOR3 vLightDir;
//	//	D3DXVec3Normalize(&vLightDir, (D3DXVECTOR3*)&Light->Direction);
//	//	GetShaderMgr().SetValue(PT_LIGHTDIR, &vLightDir);
//	//	D3DXVECTOR3 vUp(0,1,0);
//	//	D3DXVECTOR3 vEyePt(0,0,0);
//	//	D3DXMATRIX mView,mProj;
//	//	D3DXMatrixLookAtLH(&mView, &vEyePt, &vLightDir, &vUp);
//	//	D3DXMatrixOrthoLH(&mProj, 8, 8, -512, 512);
//	//	D3DXMatrixMultiply(&mView, &mView, &mProj);
//	//	GetShaderMgr().SetValue(PT_LIGHT_VIEW, &mView);
//	//}
//}

void CD3D9RenderSystem::LightEnable(unsigned long Index, bool bEnable)
{
	//LEList.bChangeValue[Index] = Enable;
	//LEList.nIndex.push_back(Index);
	D3D9HR( m_pD3D9Device->LightEnable(Index, bEnable) );
}

void CD3D9RenderSystem::SetLightingEnabled(bool bEnable)
{
	D3D9HR( m_pD3D9Device->SetRenderState(D3DRS_LIGHTING, bEnable) );
}

void CD3D9RenderSystem::SetTexCoordIndex(unsigned long stage, unsigned long index)
{
	DWORD uIndex=0;
	if (index&TCI_CAMERASPACE_NORMAL)
	{
		uIndex|=D3DTSS_TCI_CAMERASPACENORMAL;
	}
	if (index&TCI_CAMERASPACE_POSITION)
	{
		uIndex|=D3DTSS_TCI_CAMERASPACEPOSITION;
	}
	if (index&TCI_CAMERASPACE_REFLECTION_VECTOR)
	{
		uIndex|=D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
	}
	if (index&TCI_SPHEREMAP)
	{
		uIndex|=D3DTSS_TCI_SPHEREMAP;
	}
	D3D9HR( m_pD3D9Device->SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX, uIndex) );
}

bool CD3D9RenderSystem::commitTexture()
{
	for (std::map<unsigned long,IDirect3DTexture9*>::iterator it=m_mapChangeTexture.begin(); it!=m_mapChangeTexture.end(); ++it)
	{
		IDirect3DTexture9* pOldD3D9Texture = NULL;
		D3D9HR( m_pD3D9Device->GetTexture(it->first,(IDirect3DBaseTexture9**) &pOldD3D9Texture) );
		if (pOldD3D9Texture != it->second)
		{
			D3D9HR( m_pD3D9Device->SetTexture(it->first, it->second) );
		}	
		S_REL(pOldD3D9Texture);
	}
	m_mapChangeTexture.clear();
	return true;
}

bool CD3D9RenderSystem::commitStreamSource()
{
	for (std::map<unsigned long,D3D9StreamSource>::iterator it=m_mapChangeStreamSource.begin(); it!=m_mapChangeStreamSource.end(); ++it)
	{
		D3D9StreamSource d3D9StreamSource;
		D3D9HR( m_pD3D9Device->GetStreamSource(it->first,&d3D9StreamSource.pStreamData,&d3D9StreamSource.uOffsetInBytes,&d3D9StreamSource.uStride) );
		if (d3D9StreamSource != it->second)
		{
			D3D9HR( m_pD3D9Device->SetStreamSource(it->first,it->second.pStreamData,it->second.uOffsetInBytes,it->second.uStride) );
		}
		S_REL(d3D9StreamSource.pStreamData);
	}
	m_mapChangeStreamSource.clear();
	return true;
}

bool CD3D9RenderSystem::commitOther()
{
	// Material
	//if (m_Material != m_ChangeMaterial)
	//{

	// FVF
	if (m_uChangeFVF!=0)
	{
		DWORD dwOldFVF = 0;
		D3D9HR( m_pD3D9Device->GetFVF(&dwOldFVF) );
		if (m_uChangeFVF != dwOldFVF)
		{
			D3D9HR( m_pD3D9Device->SetFVF(m_uChangeFVF) );
		}
		m_uChangeFVF = 0;
	}

	if (m_pChangeIB)
	{
		IDirect3DIndexBuffer9* pOldIB = NULL;
		D3D9HR( m_pD3D9Device->GetIndices(&pOldIB) );
		if (pOldIB!=m_pChangeIB)
		{
			D3D9HR( m_pD3D9Device->SetIndices(m_pChangeIB) );
		}
		S_REL(pOldIB);
		m_pChangeIB = NULL;
	}

	if (m_pChangeShader)
	{
		if (m_pOldShader!=m_pChangeShader)
		{
			if (m_pOldShader)
			{
				m_pOldShader->end();
				m_pOldShader=NULL;
			}
			m_pChangeShader->begin("Render");
			m_pOldShader = m_pChangeShader;
			m_pChangeShader = NULL;
		}
	}
	return true;
}

bool CD3D9RenderSystem::commit()
{
	commitTexture();
	if (m_nShaderID>0)
	{
		//GetShaderMgr().m_pParamEffect->CommitChanges();
	}
	commitStreamSource();
	commitOther();
	return true;
}