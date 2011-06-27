#pragma once
#include "DataPlugsMgr.h"
#include "TSingleton.h"
#include "SkeletonNode.h"
#include "Particle.h"
#include "SkinModel.h"
#include "SkeletonData.h"
#include "LodMesh.h"

typedef iRenderNode*	(__thiscall *P_FUNC_NEW_RENDER_NODE)(void);
typedef void*			(__thiscall *P_FUNC_NEW_RENDER_DATA)(void);

class CRenderNodeMgr:public iRenderNodeMgr, public TSingleton<CRenderNodeMgr>
{
public:
	CRenderNodeMgr();
	virtual ~CRenderNodeMgr(){};
	void registerRenderNode(const char* szClassName, P_FUNC_NEW_RENDER_NODE pfn);
	void registerRenderData(const char* szClassName, P_FUNC_NEW_RENDER_DATA pfn);
	virtual iRenderNode*	loadRenderNode(const char* szFilename, iRenderNode* pRenderNode);
	virtual iRenderNode*	createRenderNode(const char* szClassName);
	virtual void*			createRenderData(const char* szClassName);
	virtual void*			getRenderData(const char* szClassName, const char* szName);
	virtual void*			createRenderData(const char* szClassName, const char* szName);
	CDataPlugsMgr&			getDataPlugsMgr(){return m_DataPlugsMgr;}
private:
	std::map<std::string, P_FUNC_NEW_RENDER_NODE>		m_mapRenderNodeFunc;
	std::map<std::string, P_FUNC_NEW_RENDER_DATA>		m_mapRenderDataFunc;
	std::map<std::string, std::map<std::string, void*>>		m_mapRenderData;
	CDataPlugsMgr	m_DataPlugsMgr;
};
