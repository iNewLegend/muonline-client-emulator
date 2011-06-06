#pragma once
#include "Vec2D.h"
#include "Matrix.h"
#include "Texture.h"

class CShader
{
public:
	virtual bool create(const std::string& strFilename)=0;
	virtual void setFloat(const std::string& strFloat, float val)=0;
	virtual void setVec2D(const std::string& strVec2D, const Vec2D& val)=0;
	virtual void setVec3D(const std::string& strVec3D, const Vec3D& val)=0;
	virtual void setVec4D(const std::string& strVec4D, const Vec4D& val)=0;
	virtual void setMatrix(const std::string& strMatrix, const Matrix& mat)=0;
	virtual void setTexture(const std::string& strTexture, unsigned long uTexID)=0;
	virtual void setTexture(const std::string& strTexture, const CTexture* pTexture)=0;
	virtual bool begin(const std::string& strTec)=0;
	virtual void end()=0;
};