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
	bool create(const std::string& strFilename);
	bool createFromMemory(void* pBuf, int nSize,LPD3DXINCLUDE pInclude);
	ID3DXEffect* getD3DXEffect();
public:
	void setFloat(const std::string& strFloat, float val);
	void setVec2D(const std::string& strVec2D, const Vec2D& val);
	void setVec3D(const std::string& strVec3D, const Vec3D& val);
	void setVec4D(const std::string& strVec4D, const Vec4D& val);
	void setMatrix(const std::string& strMatrix, const Matrix& mat);
	void setTexture(const std::string& strTexture, unsigned long uTexID);
	void setTexture(const std::string& strTexture, const CTexture* pTexture);
	bool begin(const std::string& strTec);
	void end();
private:
	ID3DXEffect* m_pEffect;
	static ID3DXEffectPool* ms_pEffectPool;    //¹²Ïí³Ø
};