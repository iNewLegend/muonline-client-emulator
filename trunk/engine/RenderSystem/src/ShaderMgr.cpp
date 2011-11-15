#include "ShaderMgr.h"
#include "IORead.h"
#include "RenderSystem.h"

CShaderMgr::CShaderMgr()
{
}
CShaderMgr::~CShaderMgr()
{
}

unsigned long CShaderMgr::registerItem(const char* szFilename)
{
	if(!IOReadBase::Exists(szFilename))
	{
		return 0;
	}
	if (find(szFilename))
	{
		return addRef(szFilename);
	}
	CShader* pShader = CRenderSystem::getSingleton().newShader();
	pShader->create(szFilename);
	return add(szFilename, pShader);
}