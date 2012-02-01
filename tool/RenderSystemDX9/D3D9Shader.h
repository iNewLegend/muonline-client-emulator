#pragma once
#include "Shader.h"
#include "dxstdafx.h"

class CD3D9Shader : public CShader
{
public:
	CD3D9Shader();
	~CD3D9Shader();
	void OnResetDevice();
	void OnLostDevice();
	void OnDestroyDevice();
	bool create(const char* szFilename);
	bool createFromMemory(void* pBuf, int nSize,LPD3DXINCLUDE pInclude);
	ID3DXEffect* getD3DXEffect();
public:
	void setFloat(const char* szName, float val);
	void setFloatArray(const char* szName, const float* pVal, int nCount);
	void setMatrix(const char* szName, const Matrix& mat);
	void setTexture(const char* szName, unsigned long uTexID);
	void setTexture(const char* szName, const CTexture* pTexture);
	bool begin(const std::string& strTec);
	void end();
private:
	ID3DXEffect* m_pEffect;
	static ID3DXEffectPool* ms_pEffectPool;    //¹²Ïí³Ø
};