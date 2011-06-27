#include "RenderNodeMgr.h"
#include "IORead.h"
#include "FileSystem.h"
#include "RenderSystem.h"

iRenderNode* _createSkeleton()	{return new CSkeletonNode;}
iRenderNode* _createParticle()	{return new CParticleEmitter;}
iRenderNode* _createMesh()		{return new CSkinModel;}

void* _createSkeletonData()	{return new CSkeletonData;}
void* _createParticleData()	{return new ParticleData;}
void* _createMeshData()		{return new CLodMesh;}

CRenderNodeMgr::CRenderNodeMgr()
{
	m_DataPlugsMgr.loadPlugs("Plugins\\*.dll");

	registerRenderNode("skeleton",	(P_FUNC_NEW_RENDER_NODE)&_createSkeleton);
	registerRenderNode("particle",	(P_FUNC_NEW_RENDER_NODE)&_createParticle);
	registerRenderNode("mesh",		(P_FUNC_NEW_RENDER_NODE)&_createMesh);

	registerRenderData("skeleton",	(P_FUNC_NEW_RENDER_DATA)&_createSkeletonData);
	registerRenderData("particle",	(P_FUNC_NEW_RENDER_DATA)&_createParticleData);
	registerRenderData("mesh",		(P_FUNC_NEW_RENDER_DATA)&_createMeshData);
}

void CRenderNodeMgr::registerRenderNode(const char* szClassName, P_FUNC_NEW_RENDER_NODE pfn)
{
	m_mapRenderNodeFunc[szClassName] = pfn;
}

void CRenderNodeMgr::registerRenderData(const char* szClassName, P_FUNC_NEW_RENDER_DATA pfn)
{
	m_mapRenderDataFunc[szClassName] = pfn;
}

iRenderNode* CRenderNodeMgr::loadRenderNode(const char* szFilename, iRenderNode* pRenderNode)
{
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(szFilename);
	CModelPlugBase* pModelPlug = (CModelPlugBase*)m_DataPlugsMgr.getPlugByExtension(strExt.c_str());
	if (pModelPlug)
	{
		return (iRenderNode*)pModelPlug->importData(this, pRenderNode, szFilename);
	}
	return false;
}

iRenderNode* CRenderNodeMgr::createRenderNode(const char* szClassName)
{
	auto it = m_mapRenderNodeFunc.find(szClassName);
	if (it!=m_mapRenderNodeFunc.end())
	{
		return it->second();
	}
	return NULL;
}

void* CRenderNodeMgr::createRenderData(const char* szClassName)
{
	auto it = m_mapRenderDataFunc.find(szClassName);
	if (it!=m_mapRenderDataFunc.end())
	{
		return it->second();
	}
	return NULL;
}

void* CRenderNodeMgr::getRenderData(const char* szClassName, const char* szName)
{
	if(strcmp(szClassName,"material")==0)
	{
		return &GetRenderSystem().getMaterialMgr().getItem(szName);
	}
	auto it = m_mapRenderData.find(szClassName);
	if (it!=m_mapRenderData.end())
	{
		auto it2 = it->second.find(szName);
		if (it2!=it->second.end())
		{
			return it2->second;
		}
	}
	return NULL;
}

void* CRenderNodeMgr::createRenderData(const char* szClassName, const char* szName)
{
	if(strcmp(szClassName,"material")==0)
	{
		return &GetRenderSystem().getMaterialMgr().getItem(szName);
	}
	if(getRenderData(szClassName,szName))
	{
		return NULL;
	}
	void* pData = createRenderData(szClassName);
	if(pData)
	{
		m_mapRenderData[szClassName][szName] = pData;
	}
	return pData;
}
