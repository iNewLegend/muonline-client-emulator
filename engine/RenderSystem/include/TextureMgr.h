#pragma once
#include "manager.h"
#include "Texture.h"
#include <set>

class CRenderSystem;

class CTextureMgr : public CManager<CTexture>
{
public:
	CTextureMgr(CRenderSystem* pRenderSystem);
	~CTextureMgr();
public:
	virtual CTexture*		createTexture();
	virtual CTexture*		CreateTextureFromFile(const std::string& strFilename, int nLevels=-1);
	virtual CTexture*		CreateTexture(int nWidth, int nHeight, int nLevels=0, bool bPoolManaged=true);
	virtual CTexture*		CreateDynamicTexture(int nWidth, int nHeight);
	virtual CTexture*		CreateTextureFromFileInMemory(void* pBuf, int nSize, int nLevels=-1);
	virtual CTexture*		CreateTextureFromMemory(void* pBuf, int nSize, int nWidth, int nHeight, int nLevels=-1);
	virtual CTexture*		CreateCubeTextureFromFile(const std::string& strFilename);

	virtual CTexture*		CreateRenderTarget(int nWidth, int nHeight);
	virtual CTexture*		CreateDepthStencil(int nWidth, int nHeight);

	unsigned long			RegisterTexture(const std::string& strFilename, int nLevels = -1);
	CTexture*				getLoadedTexture(unsigned long uTexID);
	void					releaseBuffer(int nIndex);
	void					releaseBuffer(const std::string& strFilename);
	void					Update();
	void					remove(CTexture* pTexture);
	std::set<CTexture*>&	getTextureList();
protected:
	void					pushLoadingTexture(CTexture* pTexture);
	CTexture*				m_pLoadingTexture;
	std::set<CTexture*>		m_setTextureList;
	CRenderSystem*			m_pRenderSystem;
};