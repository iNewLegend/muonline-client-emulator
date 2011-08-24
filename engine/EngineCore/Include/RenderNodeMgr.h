#pragma once
#include "TSingleton.h"
#include "SkeletonNode.h"
#include "Particle.h"
#include "SkinModel.h"
#include "SkeletonData.h"
#include "LodMesh.h"

typedef struct{
	CDataPlugBase * pObj;
	HINSTANCE hIns;
}DATA_PLUG_ST, * LPDATA_PLUG_ST;

typedef iRenderNode*	(__thiscall *P_FUNC_NEW_RENDER_NODE)(void);
typedef void*			(__thiscall *P_FUNC_NEW_RENDER_DATA)(void);

class CRenderNodeMgr:public iRenderNodeMgr, public TSingleton<CRenderNodeMgr>
{
public:
	CRenderNodeMgr();
	virtual ~CRenderNodeMgr(){};
	void registerRenderNode(const char* szClassName, P_FUNC_NEW_RENDER_NODE pfn);
	void registerRenderData(const char* szClassName, P_FUNC_NEW_RENDER_DATA pfn);
	virtual bool			loadRenderNode(const char* szFilename, iRenderNode* pRenderNode);
	virtual iRenderNode*	createRenderNode(const char* szClassName);
	virtual void*			createRenderData(const char* szClassName);
	virtual void*			getRenderData(const char* szClassName, const char* szName);
	virtual void*			createRenderData(const char* szClassName, const char* szName);

	CDataPlugBase*			getPlugByExtension(const char* szExt);
	bool					loadPlugs(const char* szFilename);
	std::string				getAllExtensions();
private:
	bool loadPlug(const char* szFilename);
	std::vector<DATA_PLUG_ST> m_setPlugObj;

	std::map<std::string, P_FUNC_NEW_RENDER_NODE>		m_mapRenderNodeFunc;
	std::map<std::string, P_FUNC_NEW_RENDER_DATA>		m_mapRenderDataFunc;
	std::map<std::string, std::map<std::string, void*>>		m_mapRenderData;
};
