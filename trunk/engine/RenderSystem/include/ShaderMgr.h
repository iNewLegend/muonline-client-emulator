#pragma once
#include "manager.h"
#include "Shader.h"

class CShaderMgr : public CManager<CShader>
{
public:
	CShaderMgr();
	~CShaderMgr();
	unsigned long registerItem(const char* szFilename);
protected:
};