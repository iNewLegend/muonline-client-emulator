#pragma once
#include "Vec2D.h"
#include "Matrix.h"
#include "Texture.h"

class CShader
{
public:
	virtual bool create(const char* szFilename)=0;
	virtual void setFloat(const char* szName, float val)=0;
	virtual void setVec2D(const char* szName, const Vec2D& val)=0;
	virtual void setVec3D(const char* szName, const Vec3D& val)=0;
	virtual void setVec4D(const char* szName, const Vec4D& val)=0;
	virtual void setMatrix(const char* szName, const Matrix& mat)=0;
	virtual void setTexture(const char* szName, unsigned long uTexID)=0;
	virtual void setTexture(const char* szName, const CTexture* pTexture)=0;
	virtual bool begin(const std::string& strTec)=0;
	virtual void end()=0;
};