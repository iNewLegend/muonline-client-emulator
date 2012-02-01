#pragma once
#include "Vec2D.h"
#include "Matrix.h"
#include "Texture.h"

class CShader
{
public:
	virtual bool create(const char* szFilename)=0;
	virtual void setFloat(const char* szName, float val)=0;
	virtual void setFloatArray(const char* szName, const float* pVal, int nCount)=0;
	virtual void setMatrix(const char* szName, const Matrix& mat)=0;
	virtual void setTexture(const char* szName, unsigned long uTexID)=0;
	virtual void setTexture(const char* szName, const CTexture* pTexture)=0;
	virtual bool begin(const std::string& strTec)=0;
	virtual void end()=0;
};