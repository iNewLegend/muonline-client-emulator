#include "RenderNodeMgr.h"
#include "IORead.h"
#include "FileSystem.h"
#include "RenderSystem.h"

iRenderNode* newSkeletonNode(){return new CSkeletonNode;}
iRenderNode* newParticleEmitter(){return new CParticleEmitter;}
iRenderNode* newSkinModel(){return new CSkinModel;};

void* newSkeletonData(){return new CSkeletonData;}
void* newParticleData(){return new ParticleData;}
void* newLodMesh(){return new CLodMesh;}

CRenderNodeMgr::CRenderNodeMgr()
{
	m_DataPlugsMgr.loadPlugs("Plugins\\*.dll");
	registerRenderNode("skeleton",	(P_FUNC_NEW_RENDER_NODE)newSkeletonNode/*(P_FUNC_NEW_RENDER_NODE)&[](){return new CSkeletonNode;}*/);
	registerRenderNode("particle",	(P_FUNC_NEW_RENDER_NODE)newParticleEmitter);
	registerRenderNode("mesh",		(P_FUNC_NEW_RENDER_NODE)newSkinModel);

	registerRenderData("skeleton",	(P_FUNC_NEW_RENDER_DATA)newSkeletonData);
	registerRenderData("particle",	(P_FUNC_NEW_RENDER_DATA)newParticleData);
	registerRenderData("mesh",		(P_FUNC_NEW_RENDER_DATA)newLodMesh);
}

void CRenderNodeMgr::registerRenderNode(const char* szClassName, P_FUNC_NEW_RENDER_NODE pfn)
{
	m_mapRenderNodeFunc[szClassName] = pfn;
}

void CRenderNodeMgr::registerRenderData(const char* szClassName, P_FUNC_NEW_RENDER_DATA pfn)
{
	m_mapRenderDataFunc[szClassName] = pfn;
}

bool CRenderNodeMgr::loadRenderNode(const char* szFilename, iRenderNode* pRenderNode)
{
	// 判断格式--根据文件后缀名
	std::string strExt = GetExtension(szFilename);
	CModelPlugBase* pModelPlug = (CModelPlugBase*)m_DataPlugsMgr.getPlugByExtension(strExt.c_str());
	if (pModelPlug)
	{
		return pModelPlug->importData(this, pRenderNode, szFilename);
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
