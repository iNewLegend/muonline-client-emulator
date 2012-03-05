#pragma once
#include "Texture.h"
#include "dxstdafx.h"

class CD3D9Texture : public CTexture
{
	friend class CD3D9TextureMgr;
	friend class CD3D9RenderSystem;
public:
	CD3D9Texture();
	~CD3D9Texture();
public:
	void FillLevel(void* pBuf, int nSize, int nWidth, int nHeight, int nLevel);
	void Filter(int nSrcLevel, int nFilter);
	void SaveToFile(const std::string& strFilename);

	void OnResetDevice();
	void OnLostDevice();
	void OnDestroyDevice();
	// 获取D3D纹理
	LPDIRECT3DTEXTURE9 GetD3D9Texture()const;
	LPDIRECT3DSURFACE9 GetD3D9Surface()const;
	//
	void releaseBuffer();
private:
	// 创建纹理
	void createTexture(int nWidth, int nHeight, int nLevels=-1, bool bPoolManaged=true);
	void createDynamicTexture(int nWidth, int nHeight);
	void createTextureFromFileInMemory(void* pBuf, int nSize, int nLevels=-1);
	void createCubeTexture(int nSize, int nLevels=-1, bool bPoolManaged=true);
	void createCubeTextureFromFile(const std::string& strFilename);
	//virtual void CreateTextureFromMemory(void* pBuf, int nSize, int nWidth, int nHeight, int nLevels=-1);
	void createRenderTarget(int nWidth, int nHeight);

	LPDIRECT3DTEXTURE9	m_pd3dTexture;
	LPDIRECT3DSURFACE9	m_pD3D9Surface;
};