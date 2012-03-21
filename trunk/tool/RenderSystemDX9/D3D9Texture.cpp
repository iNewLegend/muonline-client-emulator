#include "D3D9Texture.h"
#include "DXUT.h"
#include "FileSystem.h"
#include "D3D9RenderSystemCommon.h"
#include "TextureMgr.h"

CD3D9Texture::CD3D9Texture():
m_pd3dTexture(NULL),
m_pD3D9Surface(NULL)
{
}

CD3D9Texture::~CD3D9Texture()//:~CTexture()
{
	releaseBuffer();
}

void CD3D9Texture::createTexture(int nWidth, int nHeight, int nLevels, bool bPoolManaged)
{
	DXUTGetD3DDevice()->CreateTexture(
		nWidth,
		nHeight,
		nLevels,
		0,
		D3DFMT_A8R8G8B8,
		bPoolManaged?D3DPOOL_MANAGED:D3DPOOL_DEFAULT,
		&m_pd3dTexture,
		NULL);
}

void CD3D9Texture::createDynamicTexture(int nWidth, int nHeight)
{
	DXUTGetD3DDevice()->CreateTexture(
		nWidth,
		nHeight,
		0,
		D3DUSAGE_DYNAMIC,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pd3dTexture,
		NULL);
	if (m_pd3dTexture)
	{
		m_pd3dTexture->GetSurfaceLevel(0 , &m_pD3D9Surface);
	}
}

void CD3D9Texture::createTextureFromFileInMemory(void* pBuf, int nSize, int nLevels)
{
	D3DXIMAGE_INFO info;
	if (FAILED (D3DXCreateTextureFromFileInMemoryEx (DXUTGetD3DDevice(),
		pBuf,
		nSize,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		nLevels,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		&info,
		NULL,
		&m_pd3dTexture)))
	{
		//erro
		return;
	}
	m_nWidth = info.Width;
	m_nHeight = info.Height;
	//D3DXCreateTextureFromFileInMemory(DXUTGetD3DDevice(), pImageBits, Size, &m_pd3dTexture);

	//D3DXCreateTextureFromFileInMemoryEx(DXUTGetD3DDevice(),pImageBits,Size,Width,Height,
	//									D3DX_DEFAULT,0, D3DFMT_R8G8B8, D3DPOOL_MANAGED, 
	//									D3DX_DEFAULT, D3DX_DEFAULT, 0, 
	//									NULL, NULL, &m_pd3dTexture);
}

void CD3D9Texture::createCubeTexture(int nSize, int nLevels, bool bPoolManaged)
{
	D3DXCreateCubeTexture(DXUTGetD3DDevice(), nSize, nLevels, 0, D3DFMT_A8R8G8B8,
		bPoolManaged?D3DPOOL_MANAGED:D3DPOOL_DEFAULT, (LPDIRECT3DCUBETEXTURE9*)&m_pd3dTexture);
}

void CD3D9Texture::createCubeTextureFromFile(const std::string& strFilename)
{
	createCubeTexture(512,1);
	static const char* szCubes[6]={"_RT","_LF","_UP","_DN","_FR","_BK"};
	std::string strExtension = GetExtension(strFilename);
	std::string strFilename1 = strFilename.substr(0,strFilename.length()-strExtension.length());
	for (int i=0;i<6; i++)
	{
		std::string strTemp = strFilename1+szCubes[i]+strExtension;
		//CTexture* pTexture = R.GetTextureMgr().CreateTextureFromFile(szSkyBoxFilename[i]);

		CD3D9Texture* d3D9Texture=(CD3D9Texture*)m_pTextureMgr->CreateTextureFromFile(strTemp,1);
		if (NULL==d3D9Texture)
		{
			MessageBoxA(0,"CD3D9Texture::createCubeTextureFromFile()", "Error",0);
			continue;
		}
		//LPDIRECT3DTEXTURE9 pd3dTexture;
		//D3DXCreateTextureFromFileExA(
		//	DXUTGetD3DDevice(), strTemp.c_str(),
		//	D3DX_DEFAULT, D3DX_DEFAULT, 
		//	0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
		//	D3DX_DEFAULT, D3DX_DEFAULT, 0,
		//	NULL, NULL, &pd3dTexture);

		D3DLOCKED_RECT TexLRect;
		d3D9Texture->GetD3D9Texture()->LockRect(0, &TexLRect, 0, 0);

		DWORD* imageData = (DWORD*)TexLRect.pBits;

		D3DLOCKED_RECT LRect;
		((LPDIRECT3DCUBETEXTURE9)m_pd3dTexture)->LockRect(_D3DCUBEMAP_FACES(D3DCUBEMAP_FACE_POSITIVE_X+i), 0, &LRect, NULL, 0);
		DWORD* pPixel = (DWORD*)(LRect.pBits);
		for (int T = 0; T < 512*512; T++)
		{
			*pPixel = *imageData;
			pPixel++;
			imageData++;
		}
		//for (int T = 0; T < 512; T++)
		//{
		//	for (int S = 0; S < 512; S++)
		//	{
		//		pPixel[S] = imageData[S];
		//	}
		//	pPixel += LRect.Pitch;
		//	imageData += TexLRect.Pitch;
		//}
		((LPDIRECT3DCUBETEXTURE9)m_pd3dTexture)->UnlockRect(_D3DCUBEMAP_FACES(D3DCUBEMAP_FACE_POSITIVE_X+i),0);
		d3D9Texture->GetD3D9Texture()->UnlockRect(0);
		delete d3D9Texture;
	}
}

void CD3D9Texture::createRenderTarget(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	DXUTGetD3DDevice()->CreateTexture(
		nWidth,
		nHeight,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A16B16G16R16F,
		D3DPOOL_DEFAULT,
		&m_pd3dTexture,
		NULL);
	//D3D9CheckResRef(m_pd3dTexture);
	if (m_pd3dTexture)
	{
		m_pd3dTexture->GetSurfaceLevel(0 , &m_pD3D9Surface);
	}
}

void CD3D9Texture::FillLevel(void* pBuf, int nSize, int nWidth, int nHeight, int nLevel)
{
	D3DSURFACE_DESC textureDesc; 
	m_pd3dTexture->GetLevelDesc(nLevel, &textureDesc);
	// make sure we got the requested format because our code 
	// that fills the texture is hard coded to a 32 bit pixel depth.
	if(textureDesc.Format != D3DFMT_A8R8G8B8)
		return;
	D3DLOCKED_RECT lockedRect;

	m_pd3dTexture->LockRect(nLevel, &lockedRect, 0, 0);
	unsigned long* imageData = (unsigned long*)lockedRect.pBits;
	memcpy(imageData,pBuf,nWidth*nHeight*4);
	m_pd3dTexture->UnlockRect(0);
}

void CD3D9Texture::Filter(int nSrcLevel, int nFilter)
{
	D3DXFilterTexture(
		m_pd3dTexture,
		0, // default palette
		0, // use top level as source level
		D3DX_DEFAULT); // default filter
}

void CD3D9Texture::SaveToFile(const std::string& strFilename)
{
	D3DXSaveTextureToFileA(strFilename.c_str(), D3DXIFF_BMP, m_pd3dTexture, NULL);
}

void CD3D9Texture::OnResetDevice()
{
	if (!m_bPoolManaged)
	{
		reset();
	}
}

void CD3D9Texture::OnLostDevice()
{
	if (!m_bPoolManaged)
	{
		releaseBuffer();
	}
}

void CD3D9Texture::OnDestroyDevice()
{
	releaseBuffer();
}

LPDIRECT3DTEXTURE9 CD3D9Texture::GetD3D9Texture()const
{
	return m_pd3dTexture;
}

LPDIRECT3DSURFACE9 CD3D9Texture::GetD3D9Surface()const
{
	return m_pD3D9Surface;
}

void CD3D9Texture::releaseBuffer()
{
	if (m_pD3D9Surface)
	{
		m_pD3D9Surface->Release();
	}
	D3D9CheckResRef(m_pD3D9Surface);
	D3D9CheckResRef(m_pd3dTexture);
	if (m_pd3dTexture)
	{
		m_pd3dTexture->Release();
	}
	m_pD3D9Surface = NULL;
	m_pd3dTexture = NULL;

	//D3D9S_REL(m_pD3D9Surface);// release surface before release texture
	//D3D9S_REL(m_pd3dTexture);
	m_bLoaded = false;
}